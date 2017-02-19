/* 
 * File:   main.c
 * Author: Danstan
 *
 * Created on February 17, 2017, 12:28 PM
 */

#include "curl/curl.h"
#include "cJSON.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <ctosapi.h>
#include <emv_cl.h>
#include "debug2.h"
#include "wub_lib.h"
#include "Transaction.h"
#include "Setting.h"
#include "mainmenu.h"
#include "moremenu.h"
#include "settings.h"
#include "ussd.h"
#include "appmain.h"
#include "withdraw.h"
#include "cardlessdeposit.h"
#include "deposit.h"
#include "paywith.h"

void create_account(void) {
    BYTE key;
    //BYTE baBuff[256];
    BYTE fname[23];
    BYTE mname[23];
    BYTE lname[23];
    BYTE mobile[11];
    BYTE idno[9];
    int x = 0;
    while (x == 0) {


        //Define letter mapping to each key
        //normal english keyboard
        STR * keyboardLayoutEnglish[] = {" 0", "qzQZ1", "abcABC2", "defDEF3", "ghiGHI4",
            "jklJKL5", "mnoMNO6", "prsPRS7", "tuvTUV8", "wxyWXY9", ":;/\\|?,.<>", ".!@#$%^&*()"};
        //numeric keyboard (0123456789) and radix point '.'
        STR * keyboardLayoutNumberWithRadixPoint[] = {"0", "1", "2", "3", "4", "5", "6", "7",
            "8", "9", "", "."};
        //numeric keyboard (0123456789) without radix point
        STR * keyboardLayoutNumber[] = {"0", "1", "2", "3", "4", "5", "6", "7", "8", "9", "",
            ""};
        //initial LCD screen and set font to Chinese_Taiwan 16x16
        CTOS_BackLightSet(d_BKLIT_LCD, d_ON); //turn on backlight
        CTOS_LanguageConfig(d_FONT_CHINESE_TAIWAN, d_FONT_16x16, 0, FALSE);
        //CTOS_LanguageLCDFontSize(d_FONT_16x16, 0);
        CTOS_LanguageLCDFontSize(d_FONT_12x24, 0);
        //setfont displayed on the screen.
        CTOS_LCDTSelectFontSize(d_LCD_FONT_12x24);
        CTOS_LCDTClearDisplay();

        CTOS_LCDTPrintXY(4, 4, "First Name:");

        CTOS_UIKeypad(4, 5, keyboardLayoutEnglish, 40, 80, d_TRUE, d_FALSE, 0, 0, fname, 13);

        CTOS_LCDTPrintXY(4, 6, "Middle Name:");

        CTOS_UIKeypad(4, 7, keyboardLayoutEnglish, 40, 80, d_TRUE, d_FALSE, 0, 0, mname, 13);
        //CTOS_LCDTPrintXY(4, 7, baBuff);
        CTOS_LCDTPrintXY(4, 8, "Last Name:");
        CTOS_UIKeypad(4, 9, keyboardLayoutEnglish, 40, 80, d_TRUE, d_FALSE, 0, 0, lname, 13);
        //Mobile Number
        CTOS_LCDTPrintXY(4, 10, "Phone Number:");
        CTOS_UIKeypad(4, 11, keyboardLayoutNumber, 40, 80, d_FALSE, d_FALSE, 0, 0, mobile, 11);
        //National ID
        CTOS_LCDTPrintXY(4, 12, "ID Number:");
        CTOS_UIKeypad(4, 13, keyboardLayoutNumber, 40, 80, d_FALSE, d_FALSE, 0, 0, idno, 9);

        //send the account creation request
        CTOS_LCDTClearDisplay();

        CTOS_LCDTPrintXY(2, 4, "Confirm Details");
        CTOS_LCDTPrintXY(2, 6, "First Name:");
        CTOS_LCDTPrintXY(14, 6, fname);
        CTOS_LCDTPrintXY(2, 8, "Middle Name:");
        CTOS_LCDTPrintXY(14, 8, mname);
        CTOS_LCDTPrintXY(2, 10, "Last Name:");
        CTOS_LCDTPrintXY(14, 10, lname);
        CTOS_LCDTPrintXY(2, 12, "Mobile No.:");
        CTOS_LCDTPrintXY(14, 12, mobile);
        CTOS_LCDTPrintXY(2, 14, "ID No.:");
        CTOS_LCDTPrintXY(14, 14, idno);
        CTOS_LCDTPrintXY(2, 15, "OK TO Confirm");
        CTOS_KBDGet(&key);
        //Details okay
        if (key == d_KBD_ENTER) {
            x = 1;
            break;
        } else {
            x = 0;
        }

    }
    //Details fully verified
    //make the request to create the account with the bank
    account_post(fname, mname, lname, mobile, idno);

    return;
}
// 


/***************************************************************************
 *                                 
 * @ Danstan Otieno Onyango
 * Impalapay Kenya
 *
 ***************************************************************************/
/* <DESC>
 * simple HTTP POST using the easy interface
 * </DESC>
 */

char *jsonout;

/* Parse text to JSON, then render back to text, and print! */
void account_doit(char *text) {
    char *jsonout;
    cJSON *json;

    json = cJSON_Parse(text);
    if (!json) {
        printf("Error before: [%s]\n", cJSON_GetErrorPtr());
    } else {
        jsonout = cJSON_Print(json);
        cJSON_Delete(json);
        printf("%s\n", jsonout);
        free(jsonout);
    }
}

/* Read a file, parse, render back, etc. */
void account_dofile(char *filename) {
    FILE *f;
    long len;
    char *data;

    f = fopen(filename, "rb");
    fseek(f, 0, SEEK_END);
    len = ftell(f);
    fseek(f, 0, SEEK_SET);
    data = (char*) malloc(len + 1);
    fread(data, 1, len, f);
    fclose(f);
    account_doit(data);
    free(data);
}

/* Used by some code below as an example datatype. */
struct record {
    const char *precision;
    double lat, lon;
    const char *address, *city, *state, *zip, *country;
};

struct string {
    char *ptr;
    size_t len;
};

void init_string(struct string *s) {
    s->len = 0;
    s->ptr = malloc(s->len + 1);
    if (s->ptr == NULL) {
        fprintf(stderr, "malloc() failed\n");
        exit(EXIT_FAILURE);
    }
    s->ptr[0] = '\0';
}

size_t writefunc(void *ptr, size_t size, size_t nmemb, struct string *s) {
    size_t new_len = s->len + size*nmemb;
    s->ptr = realloc(s->ptr, new_len + 1);
    if (s->ptr == NULL) {
        fprintf(stderr, "realloc() failed\n");
        exit(EXIT_FAILURE);
    }
    memcpy(s->ptr + s->len, ptr, size * nmemb);
    s->ptr[new_len] = '\0';
    s->len = new_len;

    return size*nmemb;
}

int get_response(void) {
    CURL *curl;
    CURLcode res;
    BYTE key;

    curl = curl_easy_init();
    if (curl) {
        struct string s;
        init_string(&s);

        curl_easy_setopt(curl, CURLOPT_URL, "http://196.216.73.150:9990/familypos/request/accountCreation");
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writefunc);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &s);
        res = curl_easy_perform(curl);

        ///printf("%s\n", s.ptr);
        free(s.ptr);
        CTOS_LCDTClearDisplay();
        CTOS_LCDTPrintXY(4, 6, s.ptr);
        CTOS_KBDGet(&key);

        /* always cleanup */
        curl_easy_cleanup(curl);
    }
    return 0;
}

int account_post(BYTE fname[23], BYTE mname[23], BYTE lname[23], BYTE mobile[11], BYTE idno[9]) {

    BYTE key;
    BYTE sBuf[128];
    int accntvalidation = 0;

    {
        cJSON *root, *fmt, *img, *thm, *fld;
        int i; /* declare a few. */



        //build json object-string
        root = cJSON_CreateObject();
        cJSON_AddItemToObject(root, "firstName", cJSON_CreateString(fname));
        cJSON_AddItemToObject(root, "middleName", cJSON_CreateString(mname));
        cJSON_AddItemToObject(root, "lastName", cJSON_CreateString(lname));
        cJSON_AddItemToObject(root, "mobileNo", cJSON_CreateString(mobile));
        cJSON_AddItemToObject(root, "nationalID", cJSON_CreateString(idno));


        jsonout = cJSON_Print(root);
        cJSON_Delete(root); /*printf("%s\n",jsonout);	free(jsonout);	/* Print to text, Delete the cJSON, print it, release the string. */


        CURL *curl;
        CURLcode res;
        CTOS_LCDTClearDisplay();
        CTOS_LCDTPrintXY(3, 2, "Sending........");

        curl = curl_easy_init();
        if (curl) {
            //char* jsonObj = "{ \"pin\" : \"4444\" , \"amount\" :\"10000\", \"card_number\" : \"72828276766262\" }"; 
            curl_easy_setopt(curl, CURLOPT_URL, "http://196.216.73.150:9990/familypos/request/accountCreation");
            struct curl_slist *headers = NULL;
            headers = curl_slist_append(headers, "Accept: application/json");
            headers = curl_slist_append(headers, "Content-Type: application/json");
            headers = curl_slist_append(headers, "charsets: utf-8");
            /* example.com is redirected, so we tell libcurl to follow redirection */
            curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);

            /* Perform the request, res will get the return code */
            curl_easy_setopt(curl, CURLOPT_POSTFIELDS, jsonout);
            res = curl_easy_perform(curl);
            //CTOS_LCDTPrintXY(4, 4, " Response is");
            /* Check for errors */
            int http_code = 0;
            curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_code);
            if (http_code == 200 && res != CURLE_ABORTED_BY_CALLBACK) {
                CTOS_LCDTClearDisplay();
                CTOS_LCDTPrintXY(4, 6, " Successful");
                CTOS_KBDGet(&key);
                CTOS_Delay(3000);
                accntvalidation = 1;
                curl_easy_cleanup(curl);
                get_response();
                return 1;
            } else if (http_code == 401) {
                CTOS_LCDTClearDisplay();
                CTOS_LCDTPrintXY(4, 6, " Wrong PIN");
                CTOS_KBDGet(&key);
                CTOS_Delay(3000);
                accntvalidation = 0;
                curl_easy_cleanup(curl);
                return 0;
            }
            CTOS_LCDTPrintXY(3, 2, " Failed");
            CTOS_KBDGet(&key);
            return;
            curl_easy_cleanup(curl);
        }

    }


}