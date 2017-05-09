/* 
 * File:   main.c
 * Author: fredrick
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

BYTE baBuf[128];
BYTE fname[23];
BYTE mname[23];
BYTE lname[23];
BYTE mobile[11];
BYTE idno[9];
BYTE username[23];
char *jsonout;
char *responseExitCode;
char *message;
char *status;
char *balancekp;
char *accountnumber;
char *accounttype;
char *accountname;
//char key1 = 'A71Z3hIX2v71PgWNGnlP83IaaQ0XIc37staCXCDVHpE';
//char *keyfred[] = {"A","7","1","Z","3","h","I","X","2","v","7","1","P","g","W","N","G","n","l","P","8","3","I","a","a","Q","0","X","I","c","3","7","s","t","a","C","X","C","D","V","H","p","E"};
//char *keyfred = "A71Z3hIX2v71PgWNGnlP83IaaQ0XIc37staCXCDVHpE";
char keyfred[43] = {"A71Z3hIX2v71PgWNGnlP83IaaQ0XIc37staCXCDVHpE"};
BYTE request_json[255];
CTOS_RTC SetRTC;
BYTE keydan;
char *jsonresponse;

void check_balance(void) {
    BYTE keydan;
    //BYTE baBuff[256];

    int x = 0;
    while (x == 0) {
        ClearScreen(4, 26);
        ShowTitle(" CHECK VENDOR BALANCE     ");


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
        ClearScreen(4, 16);
        CTOS_LCDTPrintXY(4, 4, "UserName:");

        CTOS_UIKeypad(4, 5, keyboardLayoutEnglish, 40, 80, d_FALSE, d_FALSE, 0, 0, username, 13);

//        CTOS_LCDTPrintXY(4, 6, "Middle Name:");
//
//        CTOS_UIKeypad(4, 7, keyboardLayoutEnglish, 40, 80, d_FALSE, d_FALSE, 0, 0, mname, 13);
//        //CTOS_LCDTPrintXY(4, 7, baBuff);
//        CTOS_LCDTPrintXY(4, 8, "Last Name:");
//        CTOS_UIKeypad(4, 9, keyboardLayoutEnglish, 40, 80, d_FALSE, d_FALSE, 0, 0, lname, 13);
//        //Mobile Number
//        CTOS_LCDTPrintXY(4, 10, "Phone Number:");
//        CTOS_UIKeypad(4, 11, keyboardLayoutNumber, 40, 80, d_FALSE, d_FALSE, 0, 0, mobile, 11);
//        //National ID
//        CTOS_LCDTPrintXY(4, 12, "ID Number:");
//        CTOS_UIKeypad(4, 13, keyboardLayoutNumber, 40, 80, d_FALSE, d_FALSE, 0, 0, idno, 9);

        //send the account creation request
        ClearScreen(4, 26);

        CTOS_LCDTPrintXY(2, 4, "Confirm Details");
        CTOS_LCDTPrintXY(2, 6, "UserName:");
        CTOS_LCDTPrintXY(14, 6, username);
        //CTOS_LCDTPrintXY(2, 8, "Token:");
        CTOS_LCDTPrintXY(1, 9, keyfred);
//        CTOS_LCDTPrintXY(2, 10, "Last Name:");
//        CTOS_LCDTPrintXY(14, 10, lname);
//        CTOS_LCDTPrintXY(2, 12, "Mobile No.:");
//        CTOS_LCDTPrintXY(14, 12, mobile);
//        CTOS_LCDTPrintXY(2, 14, "ID No.:");
//        CTOS_LCDTPrintXY(14, 14, idno);
        CTOS_LCDTPrintXY(2, 15, "OK TO Confirm");
        CTOS_KBDGet(&keydan);
        //Details okay
        if (keydan == d_KBD_ENTER) {
            x = 1;
            break;
        } else {
            x = 0;
            return;
        }

    }
    //Details fully verified
    //make the request to create the account with the bank
     checkbal_post(username, keyfred);
     //checkbal_post(username, keyfred);
    return;
}

//void parse_object(char *root) {
//    cJSON* responseExitCode = NULL;
//    cJSON* message = NULL;
//    cJSON* accountnumber = NULL;
//    cJSON* accounttype = NULL;
//    cJSON* accountname = NULL;
//
//    int i;
//
//
//    for (i = 0; i < cJSON_GetArraySize(root); i++) {
//        responseExitCode = cJSON_GetObjectItem(root, "responseExitCode");
//        message = cJSON_GetObjectItem(root, "message");
//        accountnumber = cJSON_GetObjectItem(root, "accountnumber");
//        accounttype = cJSON_GetObjectItem(root, "accounttype");
//        accountname = cJSON_GetObjectItem(root, "accountname");
//
//        sprintf(baBuf, "ID No.: %s", responseExitCode);
//        CTOS_PrinterPutString(baBuf);
//
//    }
//}

/* Parse text to JSON, then render back to text, and print! */
void balance_doit(char *text) {
    char *jsonout;
    cJSON *json;

    json = cJSON_Parse(text);
    if (!json) {
        ClearScreen(4, 26);
        CTOS_LCDTPrintXY(4, 6, " Response:");
        CTOS_KBDGet(&keydan);
        CTOS_LCDTPrintXY(4, 7, cJSON_GetErrorPtr());
    } else {
        ClearScreen(4, 26);
        jsonout = cJSON_Print(json);
        cJSON_Delete(json);
        CTOS_LCDTPrintXY(4, 6, " Response:");
        CTOS_LCDTPrintXY(4, 7, jsonout);
        CTOS_KBDGet(&keydan);
        free(jsonout);
    }
}

/* Read a file, parse, render back, etc. */
void balance_dofile(char *filename) {
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
    balance_doit(data);
    free(data);
}

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

int checkbalance_post(BYTE name1[23], BYTE name2[23]) {

    BYTE keydan;
    BYTE sBuf[128];
    int accntvalidation = 0;
    {
        cJSON *root, *fmt, *img, *thm, *fld;
        int i; /* declare a few. */



        //build json object-string
        root = cJSON_CreateObject();
        cJSON_AddItemToObject(root, "username", cJSON_CreateString(name1));
        cJSON_AddItemToObject(root, "key", cJSON_CreateString(name2));
//        cJSON_AddItemToObject(root, "lastName", cJSON_CreateString(name3));
//        cJSON_AddItemToObject(root, "mobileNo", cJSON_CreateString(mobileno));
//        cJSON_AddItemToObject(root, "nationalID", cJSON_CreateString(idnumb));


        jsonout = cJSON_Print(root);
        cJSON_Delete(root); /*printf("%s\n",jsonout);	free(jsonout);	/* Print to text, Delete the cJSON, print it, release the string. */


        CURL *curl;
        CURLcode res;
        ClearScreen(4, 26);
        CTOS_LCDTPrintXY(3, 5, "Sending........");

        curl = curl_easy_init();
        if (curl) {
            struct string s;
            init_string(&s);

            curl_easy_setopt(curl, CURLOPT_URL, "http://test.expresspayments.co.ke:8585/v3/balance");
            struct curl_slist *headers = NULL;
            headers = curl_slist_append(headers, "Accept: application/json");
            headers = curl_slist_append(headers, "Content-Type: application/json");
            headers = curl_slist_append(headers, "charsets: utf-8");
            /* example.com is redirected, so we tell libcurl to follow redirection */
            curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
            curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writefunc);
            curl_easy_setopt(curl, CURLOPT_WRITEDATA, &s);
            curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);

            /* Perform the request, res will get the return code */
            curl_easy_setopt(curl, CURLOPT_POSTFIELDS, jsonout);
            res = curl_easy_perform(curl);

            //parse json object;


            long http_code = 0;
            curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_code);
            if (http_code == 200 && res != CURLE_ABORTED_BY_CALLBACK) {

                jsonresponse = malloc(sizeof (char) * strlen(s.ptr));
                strcpy(jsonresponse, s.ptr);
                balance_doit(jsonresponse);
                cJSON * root = cJSON_Parse(s.ptr);
                responseExitCode = cJSON_GetObjectItem(root, "responseExitCode")->valuestring;
                message = cJSON_GetObjectItem(root, "message")->valuestring;
                status = cJSON_GetObjectItem(root, "status")->valuestring;
                balancekp = cJSON_GetObjectItem(root, "balance")->valuestring;
                //accountname = cJSON_GetObjectItem(root, "accountname")->valuestring;
                //CTOS_LCDTPrintXY(4, 6, responseExitCode);
                //CTOS_LCDTPrintXY(4, 7, message);
                //CTOS_LCDTPrintXY(4, 8, accountnumber);
                //CTOS_LCDTPrintXY(4, 9, accountname);
                //CTOS_KBDGet(&key);
                
                ClearScreen(4, 26);
                CTOS_LCDTPrintXY(4, 6, message);
                CTOS_KBDGet(&keydan);
                accntvalidation = 1;
                printbalance();
                CTOS_KBDGet(&keydan);
                print_agentbalrcpt();
                free(s.ptr);
                curl_easy_cleanup(curl);
                return  accntvalidation;
            } else if (http_code == 401) {
                ClearScreen(4, 26);
                CTOS_LCDTPrintXY(4, 6, " Wrong PIN");
                CTOS_KBDGet(&keydan);
                CTOS_Delay(3000);
                accntvalidation = 0;
                curl_easy_cleanup(curl);
                return  accntvalidation;
            }
            CTOS_LCDTPrintXY(3, 6, " Failed ");
            CTOS_KBDGet(&keydan);
            accntvalidation = 0;
            return  accntvalidation;
            curl_easy_cleanup(curl);
        }

    }


}





//Print receipt

void printbalance(void) {
    //------------------------------------------------------
    CTOS_PrinterPutString("============CUSTOMER===============");
    sprintf(baBuf, "Terminal ID:  8220101400255");
    CTOS_PrinterPutString(baBuf);

    sprintf(baBuf, "Agent: Danstan Onyango");
    CTOS_PrinterPutString(baBuf);

    sprintf(baBuf, "Location: Nairobi");
    CTOS_PrinterPutString(baBuf);

    sprintf(baBuf, "Brunch: Muindi Mbingu");
    CTOS_PrinterPutString(baBuf);

    CTOS_RTCGet(&SetRTC);

    sprintf(baBuf, "Date: %04d:%02d:%02d:%02d:%02d ", SetRTC.bYear + 2000, SetRTC.bMonth, SetRTC.bDay, SetRTC.bHour, SetRTC.bMinute);
    CTOS_PrinterPutString(baBuf);

    sprintf(baBuf, "Name: %s ", username);
    CTOS_PrinterPutString(baBuf);

    sprintf(baBuf, "AccName: %s", accountname);
    CTOS_PrinterPutString(baBuf);

//    sprintf(baBuf, "AccNo: %s", accountnumber);
//    CTOS_PrinterPutString(baBuf);
//
//    sprintf(baBuf, "AccType: %s", accounttype);
//    CTOS_PrinterPutString(baBuf);
//
//
//    sprintf(baBuf, "Phone: %s", mobile);
//    CTOS_PrinterPutString(baBuf);

//    sprintf(baBuf, "ID No.: %s", idno);
//    CTOS_PrinterPutString(baBuf);

    CTOS_PrinterPutString("================================");

    PrintBlank();
    ClearScreen(4, 26);
    CTOS_LCDTPrintXY(3, 6, "Take Customer Receipt");
    CTOS_KBDGet(&keydan);




}

void print_agentbalrcpt(void) {
    CTOS_PrinterPutString("=============AGENT==============");
    sprintf(baBuf, "Terminal ID:  8220101400255");
    CTOS_PrinterPutString(baBuf);

    sprintf(baBuf, "Agent: Fredrick Oluoch");
    CTOS_PrinterPutString(baBuf);

    sprintf(baBuf, "Location: Nairobi");
    CTOS_PrinterPutString(baBuf);

    sprintf(baBuf, "Branch: Koinange");
    CTOS_PrinterPutString(baBuf);

    CTOS_RTCGet(&SetRTC);

    sprintf(baBuf, "Date: %04d:%02d:%02d:%02d:%02d ", SetRTC.bYear + 2000, SetRTC.bMonth, SetRTC.bDay, SetRTC.bHour, SetRTC.bMinute);
    CTOS_PrinterPutString(baBuf);

    sprintf(baBuf, "Name: %s  ", username);
    CTOS_PrinterPutString(baBuf);

//    sprintf(baBuf, "AccName: %s", accountname);
//    CTOS_PrinterPutString(baBuf);
//
//    sprintf(baBuf, "AccNo: %s", accountnumber);
//    CTOS_PrinterPutString(baBuf);

//    sprintf(baBuf, "AccType: %s", accounttype);
//    CTOS_PrinterPutString(baBuf);

    CTOS_PrinterPutString("================================");

    PrintBlank();
    ClearScreen(4, 26);
    //Log Transaction
    CTOS_LCDTPrintXY(3, 6, "Take Agent Receipt");
    CTOS_KBDGet(&keydan);
    sprintf(baBuf, "|| %s || ", jsonresponse);
    autoFileWrite(jsonresponse);
}

