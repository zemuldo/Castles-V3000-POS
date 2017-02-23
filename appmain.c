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
BYTE key;
BYTE temppassword[8];
BYTE password[8] = {'f', 'a', 'm', 'i', 'l', 'y', 'b', '\0'};
BYTE passretrycheck[4];
BYTE loggin[2];

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

void ShowMessageEvent(BYTE bKernel, EMVCL_USER_INTERFACE_REQ_DATA *stUserInterfaceRequestData) {
    DebugAddSTR("Enter Show Message Event");

    DebugAddINT("Current Kernel", bKernel);
    DebugAddHEX("Usr Req Data", (BYTE*) stUserInterfaceRequestData, sizeof (EMVCL_USER_INTERFACE_REQ_DATA));
}

EMVCL_INIT_DATA emvcl_initdat;

void transactionmain(void) {
    BYTE bKey;
    ULONG ulRtn;

    ClearScreen(4, 26);
    CTOS_LCDSelectMode(0x04);
    CTOS_LCDTSelectFontSize(d_FONT_16x30);

    DebugInit();

    emvcl_initdat.stOnEvent.OnCancelTransaction = NULL;
    //emvcl_event.OnCancelTransaction = CancelTransactionEvent;
    //emvcl_initdat.stOnEvent.OnShowMessage = NULL;
    emvcl_initdat.stOnEvent.OnShowMessage = ShowMessageEvent;
    emvcl_initdat.bConfigFilenameLen = strlen("V3CLVpTP_config.xml");
    emvcl_initdat.pConfigFilename = "V3CLVpTP_config.xml";
    ulRtn = EMVCL_Initialize(&emvcl_initdat);
    if (ulRtn != 0) {
        ShowRtn(3, "EMVCL Init Fail ", ulRtn);
        CTOS_KBDGet(&bKey);
        return;
    }
    //CTOS_LanguageLCDFontSize(d_FONT_16x16, 0);
    CTOS_LanguageLCDFontSize(d_FONT_12x24, 0);
    //setfont displayed on the screen.
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
        CTOS_LCDTPrintXY(2, 14, "0.Next Page");
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
                moremenu();
                break;
            case d_KBD_CANCEL:
                loginwithpin();
                break;


        }


    }

}



/** 
 ** The main entry of the terminal application 
 **/


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
        // better:
        // if(fabs(a[ii]-b[ii]) < 1e-10 * (fabs(a[ii]) + fabs(b[ii]))) {
        // with the appropriate tolerance
    }
    return 0;
}

void loginwithpin(void) {
    CTOS_LanguageLCDFontSize(d_FONT_12x24, 0);
    //setfont displayed on the screen.
    CTOS_LCDTSelectFontSize(d_LCD_FONT_12x24);
    ClearScreen(4, 26);
    //Check if session is active
    if (loggin[1] != '1') {
        CTOS_LCDTPrintXY(4, 5, "Enter Password");
        CTOS_UIKeypad(4, 6, keyboardLayoutEnglish, 40, 80, d_TRUE, d_FALSE, 0, '*', temppassword, 13);
        if (strcmp(temppassword, password) != 0) {
            ClearScreen(4, 26);
            CTOS_LCDTPrintXY(3, 5, "Wrong Password");
            CTOS_LCDTPrintXY(3, 6, "   !!!!!");
            CTOS_Delay(1000);

            exit(0);
        } else {
            loggin[1] = '1';
            exit;
        }
    } else {
        exit;
    }
    //National ID

    CTOS_PowerAutoModeEnable();
    CTOS_LCDTClearDisplay();
    ShowTitle("AGENCY BANKING DEMO                ");
    CTOS_LCDTPrintXY(3, 5, "1. Transaction");
    CTOS_LCDTPrintXY(3, 6, "2. Sleep");
    CTOS_LCDTPrintXY(3, 7, "3. Shutdown");
    CTOS_LCDTPrintXY(3, 8, "4. Restart");
    CTOS_LCDTPrintXY(3, 9, "5. Settings");
    CTOS_LCDTPrintXY(1, 15, "              X-Exit");
    CTOS_KBDGet(&key);
    switch (key) {
        case d_KBD_1:
            transactionmain();
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
            exit(0);
    }


}

int main(int argc, char *argv[]) {
    loggin[1] = '0';
    loginwithpin();

    exit(0);
}


