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

#define d_VW_SID_VISA_OLD_US			0x13
#define d_VW_SID_VISA_WAVE_2			0x16
#define d_VW_SID_VISA_WAVE_QVSDC		0x17
#define d_VW_SID_VISA_WAVE_MSD			0x18
#define d_VW_SID_PAYPASS_MAG_STRIPE		0x20
#define d_VW_SID_PAYPASS_MCHIP			0x21
#define d_VW_SID_JCB_WAVE_2				0x61
#define d_VW_SID_JCB_WAVE_QVSDC			0x62
#define d_VW_SID_AE_EMV					0x50
#define d_VW_SID_AE_MAG_STRIPE			0x52
#define d_VW_DISCOVER					0x41

BYTE bType;

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
    
    //Prompt amount to be entered
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
    BYTE depositaccnt[256];
    BYTE accountNo[256];

    // amount - ascii to int
    g_ulAmt = ASCII2Int(g_baInputAmt, g_usAmtLen);
    g_ulCBAmt = ASCII2Int(g_baInputCBAmt, g_usCBAmtLen);
    g_ulAmt += g_ulCBAmt;

    //Confirm Amount
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
    //Get the account Number
   ClearScreen(4, 14);
        CTOS_LCDTPrintXY(2, 5, "Enter Account NO:");
       CTOS_UIKeypad(2, 6, keyboardLayoutNumber, 40, 80, d_FALSE, d_FALSE, 0, 0, depositaccnt,
                    17);
       //confirm Account Number
    ClearScreen(4, 14);
    CTOS_LCDTPrintXY(3, 5, "Please Confirm");
    CTOS_LCDTPrintXY(3, 6, "Account Number:");
    CTOS_LCDTPrintXY(3, 7, depositaccnt);
     //Pressing okay-Accepting accnt Pressing X returns
    CTOS_LCDTPrintXY(3, 10, " OK TO CONFIRM");
    CTOS_LCDTPrintXY(1, 11, " X TO CANCEL");
    CTOS_KBDGet(&key);
    
    if (key == d_KBD_ENTER) {
        //Amount & Accnt okay Continue to Read Card
        ClearScreen(4, 14);
    }//Amount not OK, go back to re-enter
    else {
        return;
    }

    //Getting Agent card details.
    EMVCL_ShowContactlessSymbol(NULL);
    CTOS_LCDTPrintXY(2, 4, d_MSG_PRESENT_CARD);
    memset(&stACTData, 0, sizeof (EMVCL_ACT_DATA));
    memset(&stRCDataEx, 0, sizeof (EMVCL_RC_DATA_EX));
    memset(&stRCDataAnalyze, 0, sizeof (EMVCL_RC_DATA_ANALYZE));

    //Prepare Input Data
    stACTData.bStart = d_EMVCL_ACT_DATA_START_A;
    stACTData.bTagNum = 0;
    stACTData.usTransactionDataLen = 0;

    //Put 0x9F02 Amount, Authorized (Numeric)
    TransaRelatedData[stACTData.usTransactionDataLen++] = 0x9F;
    TransaRelatedData[stACTData.usTransactionDataLen++] = 0x02;
    TransaRelatedData[stACTData.usTransactionDataLen++] = 0x06;
    wub_long_2_bcd(g_ulAmt, temp, &lenn);
    memset(&TransaRelatedData[stACTData.usTransactionDataLen], 0, 6);
    memcpy(&TransaRelatedData[stACTData.usTransactionDataLen + 6 - lenn], temp, lenn);
    stACTData.usTransactionDataLen += 6;
    stACTData.bTagNum++;

    //Put 0x9F03
    TransaRelatedData[stACTData.usTransactionDataLen++] = 0x9F;
    TransaRelatedData[stACTData.usTransactionDataLen++] = 0x03;
    TransaRelatedData[stACTData.usTransactionDataLen++] = 0x06;
    wub_long_2_bcd(g_ulCBAmt, temp, &lenn);
    memset(&TransaRelatedData[stACTData.usTransactionDataLen], 0, 6);
    memcpy(&TransaRelatedData[stACTData.usTransactionDataLen + 6 - lenn], temp, lenn);

    stACTData.usTransactionDataLen += 6;
    stACTData.bTagNum++;

    //Put 0x9C
    TransaRelatedData[stACTData.usTransactionDataLen++] = 0x9C;
    TransaRelatedData[stACTData.usTransactionDataLen++] = 0x01;
    TransaRelatedData[stACTData.usTransactionDataLen++] = g_bTxntype;
    stACTData.bTagNum++;

    DebugAddHEX("In Data", TransaRelatedData, stACTData.usTransactionDataLen);

    stACTData.pbaTransactionData = TransaRelatedData;

    if (bType == d_INIT_TRANS) {
        isContactlessInterfaceSupport = TRUE;
        isContactInterfaceSupport = TRUE;
        isMastripeInterfaceSupport = TRUE;

        EMVCL_InitTransactionEx(stACTData.bTagNum, stACTData.pbaTransactionData, stACTData.usTransactionDataLen);

        // Clear MSR
        CTOS_MSRRead(baTk1Buf, &usTk1Len, baTk2Buf, &usTk2Len, baTk3Buf, &usTk3Len);

        do {
            do {
                // Contactless transaction
                if (isContactlessInterfaceSupport == TRUE) {
                    ulAPRtn = EMVCL_PerformTransactionEx(&stRCDataEx);
                    if (ulAPRtn != d_EMVCL_PENDING) {
                        break;
                    }
                }

                //Simulate a Contact transaction
                if (isContactInterfaceSupport == TRUE) {
                    //Check the ICC status
                    CTOS_SCStatus(d_SC_USER, &bStatus);

                    if ((bStatus & d_MK_SC_PRESENT)) {
                        // Check the CHIP Card is inserted 
                        ClearScreen(4, 14);
                        CTOS_LCDTPrintXY(1, 5, d_MSG_APPROVED);
                        CTOS_LCDTPrintXY(2, 6, baAmount);
                        CTOS_Delay(3000);
                        break;
                    }
                }

                //Simulate a Magnetic Stripe transaction
                if (isMastripeInterfaceSupport == TRUE) {
                    usTk1Len = usTk2Len = usTk3Len = 128;
                    // Read MSR
                    rtn = CTOS_MSRRead(baTk1Buf, &usTk1Len, baTk2Buf, &usTk2Len, baTk3Buf, &usTk3Len);
                    if (rtn == d_OK && (!(usTk1Len == 0 && usTk2Len == 0 && usTk3Len == 0))) {
                        // MSR is read
                        ClearScreen(4, 14);
                        CTOS_LCDTPrintXY(1, 5, d_MSG_APPROVED);
                        CTOS_LCDTPrintXY(2, 6, baAmount);
                        CTOS_Delay(3000);
                        break;
                    }
                }

                CTOS_KBDHit(&bKey);
                if (bKey == d_KBD_CANCEL) {
                    if (isContactlessInterfaceSupport == FALSE) {
                        return;
                    }
                    EMVCL_CancelTransaction();
                }

            } while (1);


            DebugAddINT("Result rtn", ulAPRtn);
            if (ulAPRtn == d_EMVCL_TX_CANCEL) {
                return;
            } else if (ulAPRtn == d_EMVCL_RC_DEK_SIGNAL) {
                if (stRCDataEx.baChipData[0] == d_DEK_VISA_MOBILE_DEVICE_SEE_PHONE) {
                    CTOS_LCDTPrintXY(1, 6, d_MSG_SEE_PHONE);
                } else if (stRCDataEx.baChipData[0] == d_DEK_VISA_EUR_CL_TIG_FATAL_COMMUNICATION) {
                    CTOS_LCDTPrintXY(1, 6, d_MSG_READ_CARD_ERR);
                }
            } else if (ulAPRtn == d_EMVCL_RC_DATA) {
                break;
            } else if (ulAPRtn == d_EMVCL_RC_FALLBACK) {
                // Disable contactless interface
                isContactlessInterfaceSupport = FALSE;
                isContactInterfaceSupport = TRUE;
                isMastripeInterfaceSupport = TRUE;

                ClearScreen(4, 14);
                CTOS_LCDTPrintXY(1, 4, d_MSG_TRY_OTHER_INTERFACE);
                CTOS_LCDTPrintXY(1, 5, d_MSG_INSERT_OR_SWIPE_CARD);
                CTOS_LCDTPrintXY(1, 6, baAmount);
                CTOS_Delay(3000);
            } else if (ulAPRtn == d_EMVCL_RC_NO_CARD) {
                msg = d_MSG_NO_CARD;
                ShowRtn(4, msg, ulAPRtn);
                CTOS_KBDGet(&bKey);
                return;
            } else if (ulAPRtn == d_EMVCL_RC_MORE_CARDS) {
                msg = d_MSG_MORE_CARD;
                ShowRtn(4, msg, ulAPRtn);
                CTOS_KBDGet(&bKey);
                return;
            } else if (ulAPRtn == d_EMVCL_TRY_AGAIN) {
                msg = d_MSG_TRY_AGAIN;
                ShowRtn(4, msg, ulAPRtn);
                CTOS_Delay(3000);
            } else {
                isContactlessInterfaceSupport = TRUE;
                isContactInterfaceSupport = TRUE;
                isMastripeInterfaceSupport = TRUE;

                ClearScreen(4, 14);
                CTOS_LCDTPrintXY(1, 4, d_MSG_UNSUPPORT_CARD);
                CTOS_LCDTPrintXY(1, 5, d_MSG_INSERT_OR_SWIPE);
                CTOS_LCDTPrintXY(1, 6, d_MSG_USE_OTHER_CARD);
                CTOS_Delay(3000);
                ClearScreen(4, 14);
                CTOS_LCDTPrintXY(1, 4, d_MSG_PRESENT_CARD);
                CTOS_LCDTPrintXY(1, 5, baAmount);
                EMVCL_ShowContactlessSymbol(NULL);
            }
        } while (1);
    } else {
        ulAPRtn = EMVCL_StartTransactionEx(&stACTData, &stRCDataEx);
        if (ulAPRtn == d_EMVCL_TX_CANCEL) {
            return;
        }
    }

    DebugAddINT("SID", stRCDataEx.bSID);
    DebugAddHEX("SCData DateTime", stRCDataEx.baDateTime, sizeof (stRCDataEx.baDateTime));
    DebugAddHEX("SCData Track1", stRCDataEx.baTrack1Data, stRCDataEx.bTrack1Len);
    DebugAddHEX("SCData Track2", stRCDataEx.baTrack2Data, stRCDataEx.bTrack2Len);
    DebugAddHEX("SCData Chip", stRCDataEx.baChipData, stRCDataEx.usChipDataLen);
    DebugAddHEX("SCData Additional", stRCDataEx.baAdditionalData, stRCDataEx.usAdditionalDataLen);

    ClearScreen(4, 14);
    CTOS_LCDTPrintXY(1, 5, d_MSG_READ_CARD_OK);
    CTOS_LCDTPrintXY(1, 6, d_MSG_REMOVE_CARD);
    CTOS_Delay(1300);

    //Parse transaction response data	
    //Parse Scheme ID
    switch (stRCDataEx.bSID) {
        case d_VW_SID_PAYPASS_MAG_STRIPE:
            msg = "  PayPass M-Stripe ";
            break;
        case d_VW_SID_PAYPASS_MCHIP:
            msg = "    PayPass MChip  ";
            break;
        case d_VW_SID_VISA_OLD_US:
            msg = "  VISA Old US MSD   ";
            break;
        case d_VW_SID_VISA_WAVE_MSD:
            msg = "    VisaWave MSD    ";
            break;
        case d_VW_SID_VISA_WAVE_2:
            msg = "     VisaWave 2    ";
            break;
        case d_VW_SID_VISA_WAVE_QVSDC:
            msg = "   VisaWave qVSDC   ";
            break;
        case d_VW_SID_AE_EMV:
            msg = "   ExpressPay EMV  ";
            break;
        case d_VW_SID_AE_MAG_STRIPE:
            msg = "      AE M-Stripe    ";
            break;
        case d_VW_SID_JCB_WAVE_2:
            msg = "   J/Speedy Wave 2    ";
            break;
        case d_VW_SID_JCB_WAVE_QVSDC:
            msg = "   J/Speedy qVSDC  ";
            break;
        case d_VW_DISCOVER:
            msg = "        Zip        ";
            break;
        default:
            msg = "   CardType No Def. ";
            break;
    }

    //Parse received chip and additional data 
    TLVDataClear();
    TLVDataParse(stRCDataEx.baChipData, stRCDataEx.usChipDataLen);
    TLVDataParse(stRCDataEx.baAdditionalData, stRCDataEx.usAdditionalDataLen);

    EMVCL_AnalyzeTransactionEx(&stRCDataEx, &stRCDataAnalyze);

    DebugAddINT("RC Analyze, Trans Result", stRCDataAnalyze.usTransResult);
    DebugAddINT("RC Analyze, CVM Analysis", stRCDataAnalyze.bCVMAnalysis);
    DebugAddHEX("RC Analyze, CVM Result", stRCDataAnalyze.baCVMResults, sizeof (stRCDataAnalyze.baCVMResults));
    DebugAddINT("RC Analyze, Visa AOSA Present", stRCDataAnalyze.bVisaAOSAPresent);
    DebugAddHEX("RC Analyze, Visa AOSA", stRCDataAnalyze.baVisaAOSA, sizeof (stRCDataAnalyze.baVisaAOSA));
    DebugAddINT("RC Analyze, ODA Fail", stRCDataAnalyze.bODAFail);
    ClearScreen(4, 14);
    CTOS_LCDTPrintXY(1, 4, "Checking Card CVM :");
    CTOS_Delay(2000);
    //After parsing transaction data, check if CVM is need and get the transaction result	
    //CVM Require - If Card need CVM, performing CVM at this moment
    NeedSignature = FALSE;
    CVMStr = "                    ";

    if (stRCDataAnalyze.bCVMAnalysis == d_EMVCL_CVM_REQUIRED_SIGNATURE) {
        CVMStr = "   CVM->Signature  ";
        NeedSignature = TRUE;
        CTOS_LCDTPrintXY(1, 4, "SIGNATURE CVM MODE ");
        CTOS_Delay(1000);

    } else if (stRCDataAnalyze.bCVMAnalysis == d_EMVCL_CVM_REQUIRED_ONLPIN) {
        CVMStr = "     CVM->PIN      ";
        ClearScreen(4, 14);
        CTOS_LCDTPrintXY(1, 4, "PIN CVM MODE ");
        CTOS_Delay(1000);
        int i2 = 0;
        CTOS_LCDTPrintXY(1, 4, "Enter ONL PIN :");

        if (enter_pin(3, 6, 4, 16, '*', pin, &i2) == FALSE) {
            ClearScreen(4, 14);
            CTOS_LCDTPrintXY(1, 4, "PIN By Pass       ");
            CTOS_Delay(1000);
            return;
        }

        //set TVR byte 3 if need
ex:
        TLVDataGet(0x95, temp);
        temp[2] |= 0x04;
        TLVDataAdd(0x95, 5, temp);
    } else if (stRCDataAnalyze.bCVMAnalysis == d_EMVCL_CVM_REQUIRED_NOCVM) {
        CVMStr = "   CVM->No CVM Req  ";
        ClearScreen(4, 14);
        CTOS_LCDTPrintXY(1, 4, "NO CVM REQUIRED ");
        CTOS_Delay(1000);
    }


    // TODO: Add your program here //

    int i2 = 0;
    ClearScreen(4, 14);
    CTOS_LCDTPrintXY(3, 5, "Enter DEFAULT  PIN:");

    if (enter_pin(3, 6, 4, 16, '*', pin, &i2) == FALSE) {
        ClearScreen(4, 14);
        CTOS_LCDTPrintXY(1, 4, "PIN By Pass       ");
        CTOS_Delay(1000);
        return;
    }


    usTxResult = stRCDataAnalyze.usTransResult;
    ClearScreen(4, 14);
    CTOS_LCDTPrintXY(1, 4, "   Authenticating...                 ");
    CTOS_LCDTPrintXY(2, 6, " Processing...");
    cardles_curlpostmain(baAmount, baBuff);


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

