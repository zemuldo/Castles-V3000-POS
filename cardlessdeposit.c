/** 
 **	A Template for developing new terminal application
 **/

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
/** These two files are necessary for calling CTOS API **/
#include <ctosapi.h>
#include "Transaction.h"
#include "emv_cl.h"
#include "mainmenu.h"
#include "Transaction.h"
#include "appmain.h"
//#include "curlpost.h"
#include "debug2.h"

ULONG ulRtn;
USHORT usTxResult;
char baInput[20];
int iLen;
BYTE pin[20];
UCHAR pbBuf[1024];
STR sBuf[1024];
BYTE baTmp[5000];
BYTE baBuf[128];
BYTE baTemp[256];
BYTE baTemp2[256];
BYTE baTmp1[256];
BYTE bacBuff[128];
EMVCL_ACT_DATA stACTData;
EMVCL_RC_DATA_EX stRCDataEx;
EMVCL_RC_DATA_ANALYZE stRCDataAnalyze;

int iBatchNo;

#define d_TLV_DATA_BUF_SIZE		1024
USHORT TLVDataLen;
BYTE TLVData[d_TLV_DATA_BUF_SIZE];

BYTE g_baInputCBAmt[20];

USHORT g_usCBAmtLen;
BYTE g_baInputAmt[20];
USHORT g_usAmtLen;
BYTE g_bTxntype;
ULONG g_ulAmt;
ULONG g_ulCBAmt;
BYTE g_IsHostBusy;


BYTE g_IsHostBusy;
int iBatchNo;
BYTE g_bTxntype;
#define d_INIT_TRANS	0x01

/** 
 ** The main entry of the terminal application 
 **/

void printreceipt(void)

{
    EMVCL_RC_DATA_EX *data; ULONG ulValue;
    CTOS_PrinterPutString("================================");
    PrintBlank();
    //Store ID
    sprintf(baBuf, "Store ID    :      8220101400255");
    CTOS_PrinterPutString(baBuf);

    //Terminal ID
    sprintf(baBuf, "Terminal ID :           01401493");
    CTOS_PrinterPutString(baBuf);
    CTOS_PrinterPutString("================================");
    
    //Date
    memcpy(baTemp, data->baDateTime, 4);
    baTemp[4] = 0;
    memcpy(baTmp, data->baDateTime + 4, 2);
    baTmp[2] = 0;
    memcpy(baTmp1, data->baDateTime + 6, 2);
    baTmp1[2] = 0;
    sprintf(baBuf, "Date       : %s/%s/%s", baTemp, baTmp, baTmp1);
    DebugAddHEX("date:", baBuf, strlen(baBuf));
    CTOS_PrinterPutString(baBuf);
    CTOS_PrinterPutString(" ");
    
    
    //Time
    memcpy(baTemp, data->baDateTime + 8, 2);
    baTemp[2] = 0;
    memcpy(baTmp, data->baDateTime + 10, 2);
    baTmp[2] = 0;
    memcpy(baTmp1, data->baDateTime + 12, 2);
    baTmp1[2] = 0;
    sprintf(baBuf, "Time       : %s:%s:%s", baTemp, baTmp, baTmp1);
    DebugAddHEX("time:", baBuf, strlen(baBuf));
    CTOS_PrinterPutString(baBuf);
    CTOS_PrinterPutString(" ");
    
    sprintf(bacBuff, "strlen(): %d", strlen(bacBuff));
    CTOS_PrinterPutString(bacBuff);
    CTOS_PrinterPutString(" ");
    
    // AMOUNT
    sprintf(baBuf, "     AMOUNT: KSH %ld.%02ld", ulValue / 100, ulValue % 100);
    CTOS_PrinterPutString(baBuf);

    CTOS_PrinterPutString("================================");

    
}

void do_transact(void) {
    BYTE bKey;
     BYTE *pstr;
    BYTE NeedSignature;
    BYTE temp[20];
    BYTE baAmount[32];
    BYTE upload_tx_buf[1024];
    USHORT upload_tx_len;
    BYTE pin[20];
    BYTE pin_len;
    ULONG ulAPRtn;
    BYTE *msg;
    BYTE *CVMStr;
    BYTE TransaRelatedData[100];
    BYTE lenn;
    ULONG ulValue;
    BYTE bStatus;
    USHORT usTk1Len, usTk2Len, usTk3Len; //Track 1,2,3 length //
    BYTE baTk1Buf[128], baTk2Buf[128], baTk3Buf[128]; //Track 1,2,3 data buffer //
    USHORT rtn;
    BOOL isContactlessInterfaceSupport;
    BOOL isContactInterfaceSupport;
    BOOL isMastripeInterfaceSupport;
    BYTE key;
    
    //ShowTitle(pstr);
    ClearScreen(4, 14);
    ulRtn = InputValue();

    if (ulRtn == d_NO) {

    }
    ClearScreen(4, 14);
    DebugAddINT("               ", 0);
    DebugAddINT("               ", 0);
    

    STR * keyboardLayoutNumberWithRadixPoint[] = {"0", "1", "2", "3", "4", "5", "6", "7",
        "8", "9", "", "."};
    STR * keyboardLayoutEnglish[] = {" 0", "qzQZ1", "abcABC2", "defDEF3", "ghiGHI4",
        "jklJKL5", "mnoMNO6", "prsPRS7", "tuvTUV8", "wxyWXY9", ":;/\\|?,.<>", ".!@#$%^&*()"};
    STR * keyboardLayoutNumber[] = {"0", "1", "2", "3", "4", "5", "6", "7", "8", "9", "",
        ""};
    BYTE baBuff[256];
    BYTE accountNo[256];

    // amount - ascii to int
    g_ulAmt = ASCII2Int(g_baInputAmt, g_usAmtLen);
    g_ulCBAmt = ASCII2Int(g_baInputCBAmt, g_usCBAmtLen);
    g_ulAmt += g_ulCBAmt;

    //Confirm Transaction

    ClearScreen(4, 14);
    CTOS_LCDTPrintXY(2, 4, "Please Confirm");
    CTOS_LCDTPrintXY(2, 6, "   Deposit");
    sprintf(temp, "   KSH %ld.%02ld", g_ulAmt / 100, g_ulAmt % 100);
    memset(baAmount, 0x00, sizeof (baAmount));
    memcpy(baAmount, temp, strlen(temp));
    CTOS_LCDTPrintXY(2, 8, baAmount);

    CTOS_LCDTPrintXY(1, 10, " OK TO CONFIRM");
    CTOS_LCDTPrintXY(1, 12, " X TO CANCEL");
    
    CTOS_KBDGet(&key);
    if (key == d_KBD_ENTER) {
        ClearScreen(4, 14);
    }
    else{return;}
    
   ClearScreen(4, 14);
        CTOS_LCDTPrintXY(2, 5, "Enter Account NO:");
       CTOS_UIKeypad(2, 6, keyboardLayoutNumber, 40, 80, d_FALSE, d_FALSE, 0, 0, baBuff,
                    17);

            sprintf(baBuff, "strlen(): %d", strlen(baBuff));
        
    
    //Amount okay Continue to Read Card
            CTOS_KBDGet(&key);
    if (key == d_KBD_ENTER) {
        ClearScreen(4, 14);
    }//Amount not OK, go back to re-enter
    else {
        return;
    }
CTOS_LCDTPrintXY(2, 6, " Processing...");
    cardles_curlpostmain(baAmount,baBuff);
   

}

void transact(void) {
    ULONG ulRtn;
    BYTE *pstr;
    BYTE key;

    //Perform Transaction via Transaction APIs	
    iBatchNo = 0;
    while (1) {
        ClearScreen(4, 14);
        EMVCL_StartIdleLEDBehavior(NULL);
        ShowTitle("  Deposit       ");
        CTOS_LCDTPrintXY(1, 4, "Welcome        ");

        switch (g_bTxntype) {
            case 0x00:
                pstr = "    Transaction     ";
                break;
            case 0x01:
                pstr = "    Transaction    ";
                break;
            case 0x09:
                pstr = "    Transaction  ";
                break;
            case 0x20:
                pstr = "    Transaction    ";
                break;
        }
        ClearScreen(4, 14);
        ShowTitle(pstr);
        ulRtn = InputValue();

        if (ulRtn == d_NO) {
            break;
        }
        do_transact();

        CTOS_Delay(1000);
    }

    EMVCL_StopIdleLEDBehavior(NULL);
    EMVCL_SetLED(0x0F, 0x08);
}

void cardlessdeposit(void) {
    do_transact();

    return;
}

