/* 
 * File:   main.c
 * Author: Danstan
 *
 * Created on February 17, 2017, 12:28 PM
 */

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
#include "curl/curl.h"
#include "cJSON.h"
BYTE key;
BYTE temppassword[13];
BYTE tempusername[13];
BYTE password[8] = {'f', 'a', 'm', 'i', 'l', 'y', 'b', '\0'};
BYTE passretrycheck[4];
BYTE loggin[2];
int token;
STR strAPN[9];
STR baID[6];
STR baPW[6];
BYTE str[20];
USHORT usRtn;
DWORD start, end, distance;
USHORT i;
//BYTE baIP_R [] = {0xDA, 0xD3, 0x23, 0xDB}; 
//BYTE baIP_R [] = {197, 232, 39, 171};
//USHORT usPort = 8019;
//BYTE baIP_R [] = {218,211,35,220};
//USHORT usPort = 80;
//DWORD pdwState = 0;
USHORT ret;
//BYTE baIP_S[] = "\x00\x00\x00\x00";
BYTE baIP_G[4];
BYTE key, state, bSocket, bNo;
int id;
char *jsonout;
char *responseExitCode;
char *message;

BYTE StandbybMode = d_PWR_STANDBY_MODE;
BYTE SleepbMode = d_PWR_SLEEP_MODE;

BOOL CancelTransactionEvent(void) {
    BYTE k;

    CTOS_KBDHit(&k);

    if (k == '1') {
        return TRUE;
    }

    return FALSE;
}

struct string2 {
    char *ptr;
    size_t len;
};

void init_string2(struct string2 *s2) {
    s2->len = 0;
    s2->ptr = malloc(s2->len + 1);
    if (s2->ptr == NULL) {
        fprintf(stderr, "malloc() failed\n");
        exit(EXIT_FAILURE);
    }
    s2->ptr[0] = '\0';
}

size_t writefunc2(void *ptr, size_t size, size_t nmemb, struct string2 *s2) {
    size_t new_len = s2->len + size*nmemb;
    s2->ptr = realloc(s2->ptr, new_len + 1);
    if (s2->ptr == NULL) {
        fprintf(stderr, "realloc() failed\n");
        exit(EXIT_FAILURE);
    }
    memcpy(s2->ptr + s2->len, ptr, size * nmemb);
    s2->ptr[new_len] = '\0';
    s2->len = new_len;

    return size*nmemb;
}

void tryloginadmin(void) {

    BYTE key;
    BYTE sBuf[128];
    int accntvalidation = 0;
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

    if (loggin[1] == '1' && token == 1) {
        loggin[1] = '1';
        token = 1;
        admin_menu();
    } else {
        ClearScreen(4, 26);
        ShowTitle("LOGIN                     ");
        id = 1;
        CTOS_LCDTPrintXY(4, 5, "Enter Admin Username");
        CTOS_UIKeypad(4, 6, keyboardLayoutEnglish, 40, 80, d_TRUE, d_FALSE, 0, 0, tempusername, 13);
        CTOS_LCDTPrintXY(4, 7, "Enter Admin Password");
        CTOS_UIKeypad(4, 8, keyboardLayoutEnglish, 40, 80, d_TRUE, d_FALSE, 0, '*', temppassword, 13);
        ClearScreen(4, 26);
        CTOS_LCDTPrintXY(2, 4, "Loging in....");
        CTOS_Delay(900);
        {
            cJSON *root, *fmt, *img, *thm, *fld;
            int i; /* declare a few. */



            //build json object-string
            root = cJSON_CreateObject();
            cJSON_AddItemToObject(root, "username", cJSON_CreateString(tempusername));
            cJSON_AddItemToObject(root, "password", cJSON_CreateString(temppassword));

            jsonout = cJSON_Print(root);
            cJSON_Delete(root); /*printf("%s\n",jsonout);	free(jsonout);	/* Print to text, Delete the cJSON, print it, release the string. */


            CURL *curl;
            CURLcode res;
            curl = curl_easy_init();
            if (curl) {
                struct string2 s2;
                init_string(&s2);

                curl_easy_setopt(curl, CURLOPT_URL, "http://196.216.73.150:9990/familypos/request/userLogin");
                struct curl_slist *headers = NULL;
                headers = curl_slist_append(headers, "Accept: application/json");
                headers = curl_slist_append(headers, "Content-Type: application/json");
                headers = curl_slist_append(headers, "charsets: utf-8");
                /* example.com is redirected, so we tell libcurl to follow redirection */
                curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
                curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writefunc2);
                curl_easy_setopt(curl, CURLOPT_WRITEDATA, &s2);
                curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);

                /* Perform the request, res will get the return code */
                curl_easy_setopt(curl, CURLOPT_POSTFIELDS, jsonout);
                res = curl_easy_perform(curl);

                //parse json object;
                long http_code = 0;
                curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_code);
                if (http_code == 200 && res != CURLE_ABORTED_BY_CALLBACK) {
                    token = 1;
                    loggin[1] = '1';
                    ClearScreen(4, 26);
                    CTOS_LCDTPrintXY(2, 4, "Login Successful");
                    CTOS_Delay(900);
                    admin_menu();
                } else if (http_code == 401) {
                    token = 0;
                    loggin[1] = '0';
                    curl_easy_cleanup(curl);
                    ClearScreen(4, 26);
                    CTOS_LCDTPrintXY(2, 4, "Invalid Details");
                    CTOS_Delay(900);
                    select_id();
                } else {
                    token = 0;
                    loggin[1] = '0';
                    ClearScreen(4, 26);
                    CTOS_LCDTPrintXY(2, 4, "Login Failed");
                    CTOS_Delay(900);
                    curl_easy_cleanup(curl);
                    select_id();
                }
            }

        }

    }
}

void trylogin_cashier(void) {

    BYTE key;
    BYTE sBuf[128];
    int accntvalidation = 0;
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

    if (loggin[1] == '1' && token == 1) {
        loggin[1] = '1';
        token = 1;
        agent_menu();
    } else {
        ClearScreen(4, 26);
        ShowTitle("LOGIN                     ");
        id = 1;
        CTOS_LCDTPrintXY(4, 5, "Enter Cashier Username");
        CTOS_UIKeypad(4, 6, keyboardLayoutEnglish, 40, 80, d_TRUE, d_FALSE, 0, 0, tempusername, 13);
        CTOS_LCDTPrintXY(4, 7, "Enter Cashier Password");
        CTOS_UIKeypad(4, 8, keyboardLayoutEnglish, 40, 80, d_TRUE, d_FALSE, 0, '*', temppassword, 13);
        ClearScreen(4, 26);
        CTOS_LCDTPrintXY(2, 4, "Loging in....");
        CTOS_Delay(900);
        {
            cJSON *root, *fmt, *img, *thm, *fld;
            int i;

            root = cJSON_CreateObject();
            cJSON_AddItemToObject(root, "username", cJSON_CreateString(tempusername));
            cJSON_AddItemToObject(root, "password", cJSON_CreateString(temppassword));


            jsonout = cJSON_Print(root);
            cJSON_Delete(root);


            CURL *curl;
            CURLcode res;
            curl = curl_easy_init();
            if (curl) {
                struct string2 s2;
                init_string(&s2);

                curl_easy_setopt(curl, CURLOPT_URL, "http://196.216.73.150:9990/familypos/request/userLogin");
                struct curl_slist *headers = NULL;
                headers = curl_slist_append(headers, "Accept: application/json");
                headers = curl_slist_append(headers, "Content-Type: application/json");
                headers = curl_slist_append(headers, "charsets: utf-8");
                /* example.com is redirected, so we tell libcurl to follow redirection */
                curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
                curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writefunc2);
                curl_easy_setopt(curl, CURLOPT_WRITEDATA, &s2);
                curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);

                /* Perform the request, res will get the return code */
                curl_easy_setopt(curl, CURLOPT_POSTFIELDS, jsonout);
                res = curl_easy_perform(curl);

                //parse json object;
                long http_code = 0;
                curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_code);
                if (http_code == 200 && res != CURLE_ABORTED_BY_CALLBACK) {
                    ClearScreen(4, 26);
                    token = 1;
                    loggin[1] = '1';
                    curl_easy_cleanup(curl);
                    ClearScreen(4, 26);
                    CTOS_LCDTPrintXY(2, 4, "Login Successful");
                    CTOS_Delay(900);
                    agent_menu();
                } else if (http_code == 401) {
                    token = 0;
                    loggin[1] = '0';
                    curl_easy_cleanup(curl);
                    ClearScreen(4, 26);
                    CTOS_LCDTPrintXY(2, 4, "Invalid Details");
                    CTOS_Delay(900);
                    select_id();
                } else {
                    token = 0;
                    loggin[1] = '0';
                    ClearScreen(4, 26);
                    CTOS_LCDTPrintXY(2, 4, "Login Failed");
                    CTOS_Delay(900);
                    curl_easy_cleanup(curl);
                    select_id();
                }
            }

        }

    }
}

void ShowMessageEvent(BYTE bKernel, EMVCL_USER_INTERFACE_REQ_DATA *stUserInterfaceRequestData) {
    DebugAddSTR("Enter Show Message Event");

    DebugAddINT("Current Kernel", bKernel);
    DebugAddHEX("Usr Req Data", (BYTE*) stUserInterfaceRequestData, sizeof (EMVCL_USER_INTERFACE_REQ_DATA));
}

EMVCL_INIT_DATA emvcl_initdat;

void agency_menu(void) {
    BYTE bKey;
    ULONG ulRtn;

    ClearScreen(4, 26);
    CTOS_LCDSelectMode(0x04);
    CTOS_LCDTSelectFontSize(d_FONT_16x30);

    DebugInit();

    emvcl_initdat.stOnEvent.OnShowMessage = ShowMessageEvent;
    emvcl_initdat.bConfigFilenameLen = strlen("V3CLVpTP_config.xml");
    emvcl_initdat.pConfigFilename = "V3CLVpTP_config.xml";
    ulRtn = EMVCL_Initialize(&emvcl_initdat);
    if (ulRtn != 0) {
        ShowRtn(3, "EMVCL Init Fail ", ulRtn);
        CTOS_KBDGet(&bKey);
        return;
    }
    CTOS_LanguageLCDFontSize(d_FONT_12x24, 0);
    CTOS_LCDTSelectFontSize(d_LCD_FONT_12x24);
    EMVCL_ShowVirtualLED(NULL);
    EMVCL_SetLED(0x0F, 0x08);
    while (1) {
        ClearScreen(4, 26);
        ShowTitle("   AGENT MENU           ");
        CTOS_LCDTPrintXY(2, 5, "1.A/C Opening");
        CTOS_LCDTPrintXY(2, 6, "2.Cash Withdrawal");
        CTOS_LCDTPrintXY(2, 7, "3.Card Deposit");
        CTOS_LCDTPrintXY(2, 8, "4.Cardless Deposit");
        CTOS_LCDTPrintXY(2, 9, "5.Ballance Inquiry");
        CTOS_LCDTPrintXY(2, 10, "6.Agent Ballance");
        CTOS_LCDTPrintXY(2, 11, "7.Mini Statement");
        CTOS_LCDTPrintXY(2, 12, "8.Set PIN");
        CTOS_LCDTPrintXY(2, 13, "9.Revenue Collection");
        CTOS_LCDTPrintXY(2, 14, "0.Card Utility Pay");
        CTOS_LCDTPrintXY(1, 16, "              X-Back");

        CTOS_KBDGet(&key);

        switch (key) {
            case '1':
                create_account();
                break;
            case '2':
                Do_Transaction();
                break;

            case '3':
                deposit();
                break;

            case '4':
                do_transact();
                break;
            case '5':;
                getballance();
                break;

            case '6':
                agentgetballance();
                break;

            case '7':
                getstatement();
                break;
            case '8':
                changepin();
                break;
            case '9':
                paytransact();
                break;
            case '0':
                dorutilitypay();
                break;
            case d_KBD_CANCEL:
                return;
                break;


        }


    }

}
BYTE key;
STR * keyboardLayoutNumberWithRadixPoint[] = {"0", "1", "2", "3", "4", "5", "6", "7",
    "8", "9", "", "."};
STR * keyboardLayoutEnglish[] = {" 0", "qzQZ1", "abcABC2", "defDEF3", "ghiGHI4",
    "jklJKL5", "mnoMNO6", "prsPRS7", "tuvTUV8", "wxyWXY9", ":;/\\|?,.<>", ".!@#$%^&*()"};
STR * keyboardLayoutNumber[] = {"0", "1", "2", "3", "4", "5", "6", "7", "8", "9", "",
    ""};

int comparepasswords(BYTE inpass[], BYTE tempass[], int n) {
    int i;
    for (i = 1; i <= n; i++) {

        if (inpass[i] == tempass[i]) return 1;
    }
    return 0;
}

void select_id(void) {
    token = 0;
    loggin[1] = '0';
    BYTE key;
    while (1) {
        CTOS_LanguageLCDFontSize(d_FONT_12x24, 0);
        CTOS_LCDTSelectFontSize(d_LCD_FONT_12x24);
        ClearScreen(4, 26);
        ShowTitle("LOGIN                            ");
        CTOS_LCDTPrintXY(4, 5, "Select User ID");
        CTOS_LCDTPrintXY(4, 6, "1. Admin User");
        CTOS_LCDTPrintXY(4, 7, "2. Cashier User");
        CTOS_LCDTPrintXY(4, 8, "3. Settings");
        CTOS_KBDGet(&key);
        //login admin
        if (key == d_KBD_1) {
            tryloginadmin();
            break;
        } else if (key == d_KBD_2) {
            trylogin_cashier();
            break;
        } else if (key == d_KBD_3) {
            presettings();
            break;
        } else if (key == d_KBD_CANCEL) {
            exit(0);
            break;
        } else {
            ClearScreen(4, 26);
            CTOS_LCDTPrintXY(3, 4, "Invalid Selection");
            CTOS_Delay(900);
            select_id();
            break;
        }

    }
}

void agent_menu() {
    while (1) {
        token = 1;
        loggin[1] = '1';
        CTOS_PowerAutoModeEnable();
        CTOS_LCDTClearDisplay();
        ShowTitle("AGENT  MENU                ");
        CTOS_LCDTPrintXY(3, 5, "1. Agency Menu");
        CTOS_LCDTPrintXY(3, 6, "2. Sleep");
        CTOS_LCDTPrintXY(3, 7, "3. Shutdown");
        CTOS_LCDTPrintXY(3, 8, "4. Restart");
        CTOS_LCDTPrintXY(3, 9, "5. Settings");
        CTOS_LCDTPrintXY(1, 15, "              X-Logout");
        CTOS_KBDGet(&key);
        switch (key) {
            case d_KBD_1:
                agency_menu();
                break;
            case d_KBD_2:
                sleepmode();
                break;
            case d_KBD_3:
                CTOS_PowerOff();
                break;
            case d_KBD_4:
                CTOS_SystemReset();
                break;
            case d_KBD_5:
                settings();
                break;

            case d_KBD_CANCEL:
                token = 0;
                loggin[1] = '0';
                select_id();
        }


    }
}

void admin_menu() {
    while (1) {
        token = 1;
        loggin[1] = '1';
        CTOS_PowerAutoModeEnable();
        CTOS_LCDTClearDisplay();
        ShowTitle("ADMINISTRATOR MENU                ");
        CTOS_LCDTPrintXY(3, 5, "1. Agency Menu");
        CTOS_LCDTPrintXY(3, 6, "2. Create Users");
        CTOS_LCDTPrintXY(3, 7, "3. Manage User");
        CTOS_LCDTPrintXY(3, 8, "4. Batch Upload");
        CTOS_LCDTPrintXY(3, 9, "5. Reconcl Repots");
        CTOS_LCDTPrintXY(3, 10, "6. Repair Coms");
        CTOS_LCDTPrintXY(3, 11, "7. Conn Status");
        CTOS_LCDTPrintXY(3, 12, "8. Settings");
        CTOS_LCDTPrintXY(1, 15, "              X-Logout");
        CTOS_KBDGet(&key);
        switch (key) {
            case d_KBD_1:
                agency_menu();
                break;
            case d_KBD_2:
                break;
            case d_KBD_3:
                break;
            case d_KBD_4:
                filemanager();
                break;
            case d_KBD_5:
                filemanager();
                break;
            case d_KBD_6:
                break;
            case d_KBD_7:
                break;
            case d_KBD_8:
                settings();
                break;

            case d_KBD_CANCEL:
                token = 0;
                loggin[1] = '0';
                select_id();
        }

    }
}

int main(int argc, char *argv[]) {

    gsminit();
    CTOS_Delay(900);
    opengsm();
    CTOS_LCDTClearDisplay();
    token = 0;
    loggin[1] = '0';
    select_id();

    exit(0);
}

void gsminit() {
    USHORT rc = 0;
    BYTE key;
    BYTE imsi[128];
    BYTE name[128];
    BYTE strength = 99;
    BYTE nlen;
    BYTE sBuf[128];
    BYTE State;
    BYTE bID;
    CTOS_LCDTClearDisplay();
    CTOS_LCDTPrintXY(1, 2, "  Initializing.....");
    rc = CTOS_GSMOpen(9600, 1);
    if (rc != d_OK) {
        sprintf(sBuf, "%X ", rc);
        return;
    }

    if (key == d_KBD_2) {
        usRtn = CTOS_GSMSelectSIM(d_GPRS_SIM2);
        if (usRtn != d_OK) {
            sprintf(str, "SelSIM: %X ", ret);
            CTOS_KBDHit(&key);
        }
    } else {
        usRtn = CTOS_GSMSelectSIM(d_GPRS_SIM1);
        if (usRtn != d_OK) {
            sprintf(str, "SelSIM: %X ", ret);
            CTOS_KBDHit(&key);
        }
    }
    usRtn = CTOS_GSMSelectSIM(d_GPRS_SIM1);
    if (usRtn != d_OK) {
        sprintf(str, "SelSIM: %X ", ret);
    }

    CTOS_GSMGetBAND(&bID);
    switch (bID) {
        case d_GSM_900_1800:
            break;
        case d_GSM_900_1900:
            break;
        case d_GSM_850_1800:
            break;
        case d_GSM_850_1900:
            break;
        default:
            break;
    }
    start = CTOS_TickGet();
    do {
        memset(str, 0x00, sizeof (str));
        rc = CTOS_SIMCheckReady();
        sprintf(str, "%X", rc);

        CTOS_Delay(500);
        end = CTOS_TickGet();
        distance = end - start;
    } while ((rc != d_GSM_SIM_READY) && (distance < 1000));
    if (rc == d_GSM_SIM_READY) {
        CTOS_Delay(2000);
    } else {
    }
    memset(sBuf, 0x00, sizeof (sBuf));
    start = CTOS_TickGet();
    do {
        rc = CTOS_GPRSGetRegState(&State);
        if (State == d_GSM_GPRS_STATE_NOT_REG)
            sprintf(sBuf, "%s     ", "NOT_REG");
        else if (State == d_GSM_GPRS_STATE_REG)
            sprintf(sBuf, "%s     ", "REG");
        else if (State == d_GSM_GPRS_STATE_TRYING)
            sprintf(sBuf, "%s     ", "TRYING");
        else if (State == d_GSM_GPRS_STATE_DENY)
            sprintf(sBuf, "%s     ", "DENY");
        else if (State == d_GSM_GPRS_STATE_UNKNOW)
            sprintf(sBuf, "%s     ", "UNKNOW");
        else if (State == d_GSM_GPRS_STATE_ROAM)
            sprintf(sBuf, "%s     ", "ROAM");
        CTOS_KBDHit(&key);
        if (key == d_KBD_CANCEL)
            break;

        CTOS_Delay(500);
        end = CTOS_TickGet();
        distance = end - start;
    } while (((State == d_GSM_GPRS_STATE_TRYING) || (State == d_GSM_GPRS_STATE_UNKNOW)) || (!(!(State != d_GSM_GPRS_STATE_REG) || !(State != d_GSM_GPRS_STATE_ROAM)) && (distance < 3000))); //  wait for 30 seconds

    do {
        rc = CTOS_GSMSignalQuality(&strength);
        CTOS_Delay(500);
        memset(sBuf, 0x00, sizeof (sBuf));
        sprintf(sBuf, "GSMSgnQty:%d ", strength);

        CTOS_KBDHit(&key);
        if (key == d_KBD_CANCEL)
            break;
    } while (strength < 10 || strength > 30);
    memset(imsi, 0, sizeof (imsi));
    rc = CTOS_SIMGetIMSI(imsi);
    if (rc == d_OK) {
        sprintf(sBuf, "IMSI: %s ", imsi);
    } else {
        sprintf(sBuf, "SIMGetIMSI: %X ", rc);
    }

    memset(name, 0, sizeof (name));
    rc = CTOS_GSMQueryOperatorName(name, &nlen);
    if (rc == d_OK) {
        sprintf(sBuf, "Oprtr: %s", name);
    } else {
        sprintf(sBuf, "GSMQON: %X ", rc);
    }

    CTOS_KBDHit(&key);
    if (key == d_KBD_CANCEL) {
    }


}

void opengsm() {


    CTOS_TCP_GPRSInit();
    BYTE baIP_S[] = "\x00\x00\x00\x00";
    strcpy(strAPN, "internet");
    strcpy(baID, "yahoo");
    strcpy(baPW, "yahoo");
    ret = CTOS_TCP_GPRSOpen(baIP_S, strAPN, baID, baPW);
    state = Check_auto_state(ret);
    if (state != TRUE) {
        return;
    }
    ret = CTOS_TCP_GPRSGetIP(baIP_G);
    sprintf(str, "IP Ret = %X   ", ret);
    for (i = 0; i < 4; i++) {
        sprintf(str, "%02X", baIP_G[i]);
        return;
    }
}