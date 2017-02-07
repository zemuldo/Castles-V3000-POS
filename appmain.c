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

BOOL CancelTransactionEvent(void)
{
	BYTE k;
	
	CTOS_KBDHit(&k);
	
	if(k == '1')
	{
		return TRUE;
	}
	
	return FALSE;
}

void ShowMessageEvent(BYTE bKernel, EMVCL_USER_INTERFACE_REQ_DATA *stUserInterfaceRequestData)
{
	DebugAddSTR("Enter Show Message Event");
	
	DebugAddINT("Current Kernel", bKernel);
	DebugAddHEX("Usr Req Data", (BYTE*)stUserInterfaceRequestData, sizeof(EMVCL_USER_INTERFACE_REQ_DATA));
}

EMVCL_INIT_DATA emvcl_initdat;

void transactionmain(void) {
   BYTE bKey;
	ULONG ulRtn;
	
	CTOS_LCDTClearDisplay();
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
	if(ulRtn != 0) 
	{
		ShowRtn(3, "EMVCL Init Fail ", ulRtn);
		CTOS_KBDGet(&bKey);
		return;
	}
	
	EMVCL_ShowVirtualLED(NULL);
	EMVCL_SetLED(0x0F, 0x08);
    
    while (1) {

            ClearScreen(4, 14);
            ShowTitle("   AGENT MENU           ");
            CTOS_LCDTPrintXY(2, 5, "1. CASH WITHDRAW");
            CTOS_LCDTPrintXY(2, 6, "2. CARD DEPOSIT");
            CTOS_LCDTPrintXY(2, 7, "3. CARDLESS DPOSIT");
            CTOS_LCDTPrintXY(2, 8, "4. PAY BILL");
            CTOS_LCDTPrintXY(2, 9, "5. PAY FEE");
            CTOS_LCDTPrintXY(1, 10, "              X-Exit");

            CTOS_KBDGet(&key);

            switch (key) {
                case '1':
                    withdraw();
                    break;

                case '2':
                    deposit();
                    break;

                case '3':
                    cardlessdeposit();
                    break;
                case '4':
                    paybill();
                    break;

                case '5':
                    payfee();
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
BYTE baBuff[256];

void loginwithpin(void) {
    ClearScreen(4, 14);
    ShowTitle("AGENCY BANKING DEMO    ");
    CTOS_LCDTPrintXY(3, 5, "1. Transaction");
    CTOS_LCDTPrintXY(3, 6, "2. Power Save Mode");
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
            return;
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
    loginwithpin();

    exit(0);
}


