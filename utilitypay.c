
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
//#include "curlpost.h"
#include "debug2.h"
#include "wub_lib.h"
//
#define _SIMULATE_ONLINE_RESULT

#define d_START_TRANS	0x00
#define d_INIT_TRANS	0x01

// Scheme ID
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

// Transction Result
#define d_EMV_PAYPASS_MSD_ONLINE				(USHORT)0x0001
#define d_EMV_CHIP_OFF_APPROVAL					(USHORT)0x0002
#define d_EMV_CHIP_OFF_DECLINED					(USHORT)0x0003
#define d_EMV_CHIP_ONL							(USHORT)0x0004
#define d_EMV_VISA_MSD_ONLINE					(USHORT)0x0005
#define d_EMV_CHIP_ONL_APPROVAL					(USHORT)0x0006
#define d_EMV_CHIP_ONL_DECLINED					(USHORT)0x0007
#define d_EMV_CHIP_ONL_ABORT					(USHORT)0x0008

#define d_TLV_DATA_BUF_SIZE		1024
USHORT TLVDataLen;
BYTE TLVData[d_TLV_DATA_BUF_SIZE];
CTOS_RTC SetRTC;
BYTE g_baInputCBAmt[20];

USHORT g_usCBAmtLen;
BYTE g_baInputAmt[20];
USHORT g_usAmtLen;
ULONG g_ulAmt;
ULONG g_ulCBAmt;


BYTE g_IsHostBusy;
int iBatchNo;
BYTE g_bTxntype;
BYTE ulitityNo[9];
BYTE countycode[9];
BYTE utilCode[4];

#define d_INIT_TRANS	0x01

BYTE bType;

ULONG ulRtn;
USHORT usTxResult;
char baInput[20];
int iLen;

UCHAR pbBuf[1024];
STR sBuf[1024];
BYTE baTmp[5000];
BYTE baBuf[128];
BYTE baTemp[256];
BYTE baTemp2[256];
BYTE baTemp3[256];
BYTE baTemp4[256];
BYTE baTmp1[256];


EMVCL_ACT_DATA stACTData;
EMVCL_RC_DATA_EX stRCDataEx;
EMVCL_RC_DATA_ANALYZE stRCDataAnalyze;

BYTE cvv[3];
BYTE expdate[6];
BYTE cardtype[255];
BYTE cardnumber[17];
BYTE cardvendor[255];
BYTE pin[4];
BYTE utilityNo[9];
long utility_code;
BYTE depositaccnt[15];

BYTE utilservice[10];
BYTE utilbillNo[10];

void utilityreceipt(EMVCL_RC_DATA_EX *data, BYTE isNeedSignature,ULONG ulValue) {
    //------------------------------------------------------
    //PrintBlank();
    CTOS_FONT_ATTRIB stFONT_ATTRIB;

    if (data->bSID == d_VW_SID_PAYPASS_MAG_STRIPE || data->bSID == d_VW_SID_PAYPASS_MCHIP) {
        sprintf(baBuf, "           Master PayPass");
    } else if (data->bSID == d_VW_SID_AE_EMV || data->bSID == d_VW_SID_AE_MAG_STRIPE) {
        sprintf(baBuf, "           American Express");
    } else if (data->bSID == d_VW_SID_JCB_WAVE_2 || data->bSID == d_VW_SID_JCB_WAVE_QVSDC) {
        sprintf(baBuf, "           J/Speedy");
    } else if (data->bSID == d_VW_SID_VISA_OLD_US || data->bSID == d_VW_SID_VISA_WAVE_2 || data->bSID == d_VW_SID_VISA_WAVE_QVSDC || data->bSID == d_VW_SID_VISA_WAVE_MSD) {
        sprintf(baBuf, "           Visa payWave");
    } else if (data->bSID == d_VW_DISCOVER) {
        sprintf(baBuf, "           Discover Zip");
    } else {
        sprintf(baBuf, "           Un-Know");
    }
    CTOS_PrinterPutString(baBuf);

    //Terminal ID
    sprintf(baBuf, "Terminal ID :  01401493");
    CTOS_PrinterPutString(baBuf);
    //Store ID
    sprintf(baBuf, "TransactionI:  8220101400255");
    CTOS_PrinterPutString(baBuf);
    CTOS_PrinterPutString("================================");
    
    //Type
    sprintf(baBuf, "Type :  Utility Payment");
    CTOS_PrinterPutString(baBuf);
    CTOS_PrinterPutString(" ");
    
    if (utility_code==200){
        sprintf(baBuf, "        Water Payment");
        CTOS_PrinterPutString(" ");
    }
    else if (utility_code==300){
        sprintf(baBuf, "        KPLC Payment");
        CTOS_PrinterPutString(" ");
    }
    else if (utility_code==400){
        sprintf(baBuf, "        DSTV Payment");
        CTOS_PrinterPutString(" ");
    }
    CTOS_PrinterPutString(baBuf);
    //Card Type
    if (data->bSID == d_VW_SID_PAYPASS_MAG_STRIPE || data->bSID == d_VW_SID_PAYPASS_MCHIP) {
        sprintf(baBuf, "Card Type  : MASTER");
    } else if (data->bSID == d_VW_SID_AE_EMV || data->bSID == d_VW_SID_AE_MAG_STRIPE) {
        sprintf(baBuf, "Card Type  : AEMX");
    } else if (data->bSID == d_VW_SID_JCB_WAVE_2 || data->bSID == d_VW_SID_JCB_WAVE_QVSDC) {
        sprintf(baBuf, "Card Type  : JCB");
    } else if (data->bSID == d_VW_SID_VISA_OLD_US || data->bSID == d_VW_SID_VISA_WAVE_2 || data->bSID == d_VW_SID_VISA_WAVE_QVSDC || data->bSID == d_VW_SID_VISA_WAVE_MSD) {
        sprintf(baBuf, "Card Type  : VISA");
    } else if (data->bSID == d_VW_DISCOVER) {
        sprintf(baBuf, "Card Type  : DISCOVER");
    } else {
        sprintf(baBuf, "Card Type  : UN-KMOW");
    }
    CTOS_PrinterPutString(baBuf);
    CTOS_PrinterPutString(" ");

    //Card Number
    if (data->bSID == d_VW_SID_VISA_OLD_US || data->bSID == d_VW_SID_VISA_WAVE_2 || data->bSID == d_VW_SID_VISA_WAVE_QVSDC || data->bSID == d_VW_SID_VISA_WAVE_MSD) {
        memset(baTemp, 0x00, sizeof (baTemp));
        memset(baTemp2, 0x00, sizeof (baTemp2));
        memset(baTemp3, 0x00, sizeof (baTemp3));
        memset(baTemp4, 0x00, sizeof (baTemp4));
        memcpy(baTemp, &data->baTrack2Data[1], 4);
        memcpy(baTemp2, &data->baTrack2Data[13], 4);
        memcpy(baTemp3, &data->baTrack2Data[5], 4);
        memcpy(baTemp4, &data->baTrack2Data[9], 4);


    } else {
        UnpackData(data->baTrack2Data, 20, baTmp);
        memcpy(baTemp, baTmp, 4);
        memcpy(baTemp2, &baTmp[12], 4);
        baTemp2[16] = 0;

    }


    stFONT_ATTRIB.FontSize = d_FONT_8x16;
    stFONT_ATTRIB.X_Zoom = 2;
    stFONT_ATTRIB.Y_Zoom = 2;
    sprintf(baBuf, "Card Number: %s XXXX XXXX %s", baTemp, baTemp2);
    CTOS_PrinterPutString(baBuf);
    CTOS_PrinterPutString(" ");


    //Exp Date
    if (data->bSID == d_VW_SID_VISA_OLD_US || data->bSID == d_VW_SID_VISA_WAVE_2 || data->bSID == d_VW_SID_VISA_WAVE_QVSDC || data->bSID == d_VW_SID_VISA_WAVE_MSD) {
        memcpy(baTemp, &data->baTrack2Data[20], 2);
        baTemp[2] = 0;
        memcpy(baTmp1, &data->baTrack2Data[18], 2);
        baTmp1[2] = 0;


    } else {
        memcpy(baTemp, baTmp + 19, 2);
        baTemp[2] = 0;
        memcpy(baTmp1, baTmp + 17, 2);
        baTmp1[2] = 0;

    }
    sprintf(baBuf, "Exp Date   : %s/%s", baTemp, baTmp1);
    CTOS_PrinterPutString(baBuf);
    CTOS_PrinterPutString(" ");

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

    //Auth Code
    memcpy(baTemp, data->baTrack2Data + 19, 2);
    baTemp[2] = 0;
    sprintf(baBuf, "Auth Code  : TT2738");
    CTOS_PrinterPutString(baBuf);
    CTOS_PrinterPutString(" ");

    //Batch No
    iBatchNo++;
    sprintf(baBuf, "Batch No   : %06d", iBatchNo);
    CTOS_PrinterPutString(baBuf);

    CTOS_PrinterPutString(" ");

    // AMOUNT
    sprintf(baBuf, "     Amount: KSH: %ld", ulValue);
    CTOS_PrinterPutString(baBuf);
    if (isNeedSignature) {
        sprintf(baBuf, "Signature  ");
        CTOS_PrinterPutString(baBuf);
        PrintBlank();
    }

    CTOS_PrinterPutString("================================");

    PrintBlank();

}

void utility_cardless_receipt(EMVCL_RC_DATA_EX *data, BYTE isNeedSignature,ULONG ulValue) {
    //------------------------------------------------------
    //PrintBlank();
    CTOS_FONT_ATTRIB stFONT_ATTRIB;

    //Terminal ID
    sprintf(baBuf, "Terminal ID :  01401493");
    CTOS_PrinterPutString(baBuf);
    //Store ID
    sprintf(baBuf, "TransactionI:  8220101400255");
    CTOS_PrinterPutString(baBuf);
    CTOS_PrinterPutString("================================");
    
    //Type
    sprintf(baBuf, "Type :  Utility Payment");
    CTOS_PrinterPutString(baBuf);
    CTOS_PrinterPutString(" ");
    
    if (utility_code==200){
        sprintf(baBuf, "        Water Payment");
        CTOS_PrinterPutString(" ");
    }
    else if (utility_code==300){
        sprintf(baBuf, "        KPLC Payment");
        CTOS_PrinterPutString(" ");
    }
    else if (utility_code==400){
        sprintf(baBuf, "        DSTV Payment");
        CTOS_PrinterPutString(" ");
    }
    CTOS_PrinterPutString(baBuf);
    CTOS_PrinterPutString(" ");
    
    sprintf(baBuf, "Date: %04d:%02d:%02d:%02d:%02d ", SetRTC.bYear + 2000, SetRTC.bMonth, SetRTC.bDay, SetRTC.bHour, SetRTC.bMinute);
    CTOS_PrinterPutString(baBuf);
    CTOS_RTC SetRTC;
    //Auth Code
    
    baTemp[2] = 0;
    sprintf(baBuf, "Auth Code  : TT2738");
    CTOS_PrinterPutString(baBuf);
    CTOS_PrinterPutString(" ");

    //Batch No
    iBatchNo++;
    sprintf(baBuf, "Batch No   : %06d", iBatchNo);
    CTOS_PrinterPutString(baBuf);

    CTOS_PrinterPutString(" ");

    // AMOUNT
    sprintf(baBuf, "     Amount: KSH: %ld", ulValue);
    CTOS_PrinterPutString(baBuf);
    if (isNeedSignature) {
        sprintf(baBuf, "Signature  ");
        CTOS_PrinterPutString(baBuf);
        PrintBlank();
    }

    CTOS_PrinterPutString("================================");

    PrintBlank();

}

void utility_transact(BYTE bType) {
    BYTE bKey;
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
    char *tmp;
    
    
    DebugAddINT("               ", 0);
    DebugAddINT("               ", 0);
    
    
    // amount - ascii to int
    g_ulAmt = ASCII2Int(g_baInputAmt, g_usAmtLen);
    g_ulCBAmt = ASCII2Int(g_baInputCBAmt, g_usCBAmtLen);
    g_ulAmt += g_ulCBAmt;
    
    //Confirm Transaction
    ShowTitle("  Utility Payment          ");
    ClearScreen(4, 26);
    CTOS_LCDTPrintXY(2, 4, "Please Confirm");
    CTOS_LCDTPrintXY(2, 6, "   Amount");
    sprintf(temp, "   KSH %ld", g_ulAmt);
    memset(baAmount, 0x00, sizeof (baAmount));
    memcpy(baAmount, temp, strlen(temp));
    CTOS_LCDTPrintXY(2, 8, baAmount);

    CTOS_LCDTPrintXY(1, 10, " OK TO CONFIRM");
    CTOS_LCDTPrintXY(1, 12, " X TO CANCEL");
    
    CTOS_KBDGet(&key);
    if (key == d_KBD_ENTER) {
        ClearScreen(4, 26);
    }
    else{return;}
    

    STR * keyboardLayoutNumberWithRadixPoint[] = {"0", "1", "2", "3", "4", "5", "6", "7",
        "8", "9", "", "."};
    STR * keyboardLayoutEnglish[] = {" 0", "qzQZ1", "abcABC2", "defDEF3", "ghiGHI4",
        "jklJKL5", "mnoMNO6", "prsPRS7", "tuvTUV8", "wxyWXY9", ":;/\\|?,.<>", ".!@#$%^&*()"};
    STR * keyboardLayoutNumber[] = {"0", "1", "2", "3", "4", "5", "6", "7", "8", "9", "",
        ""};
    BYTE baBuff[256];
   ClearScreen(4, 26);
        CTOS_LCDTPrintXY(2, 5, "Enter Utility No:");
       CTOS_UIKeypad(2, 6, keyboardLayoutNumber, 40, 80, d_FALSE, d_FALSE, 0, 0, ulitityNo,
                    8);
       ClearScreen(4, 26);
    CTOS_LCDTPrintXY(2, 5, "Confirm Payment:"); 
     CTOS_LCDTPrintXY(2, 6, baAmount); 
     CTOS_LCDTPrintXY(9, 6, "To BillNo");
     CTOS_LCDTPrintXY(2, 7, ulitityNo);
    //Amount okay Continue to Read Card
     CTOS_KBDGet(&key);
    if (key == d_KBD_ENTER) {
        ClearScreen(4, 26);
        CTOS_LCDTPrintXY(2, 4, d_MSG_PRESENT_CARD);
    }//Amount not OK, go back to re-enter
    else {
        return;
    }

    EMVCL_ShowContactlessSymbol(NULL);

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
                        ClearScreen(4, 26);
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
                        ClearScreen(4, 26);
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

                ClearScreen(4, 26);
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

                ClearScreen(4, 26);
                CTOS_LCDTPrintXY(1, 4, d_MSG_UNSUPPORT_CARD);
                CTOS_LCDTPrintXY(1, 5, d_MSG_INSERT_OR_SWIPE);
                CTOS_LCDTPrintXY(1, 6, d_MSG_USE_OTHER_CARD);
                CTOS_Delay(3000);
                ClearScreen(4, 26);
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

    ClearScreen(4, 26);
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
    ClearScreen(4, 26);
    CTOS_LCDTPrintXY(1, 4, "Checking Card CVM :");
    CTOS_Delay(2000);
    //After parsing transaction data, check if CVM is need and get the transaction result	
    //CVM Require - If Card need CVM, performing CVM at this moment
    NeedSignature = FALSE;
    CVMStr = "                    ";

    if (stRCDataAnalyze.bCVMAnalysis == d_EMVCL_CVM_REQUIRED_SIGNATURE) {
        CVMStr = "   CVM->Signature  ";
        NeedSignature = TRUE;
        //CTOS_LCDTPrintXY(1, 4, "SIGNATURE CVM MODE ");
        //CTOS_Delay(1000);

    } else if (stRCDataAnalyze.bCVMAnalysis == d_EMVCL_CVM_REQUIRED_ONLPIN) {
        CVMStr = "     CVM->PIN      ";
        ClearScreen(4, 26);
        //CTOS_LCDTPrintXY(1, 4, "PIN CVM MODE ");
        //CTOS_Delay(1000);
        int i2 = 0;
        CTOS_LCDTPrintXY(1, 4, "Enter ONL PIN :");

        if (enter_pin(3, 6, 4, 16, '*', pin, &i2) == FALSE) {
            ClearScreen(4, 26);
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
        ClearScreen(4, 26);
        //CTOS_LCDTPrintXY(1, 4, "NO CVM REQUIRED ");
        //CTOS_Delay(1000);
    }


    // TODO: Add your program here //

    int i2 = 0;
    ClearScreen(4, 26);
    CTOS_LCDTPrintXY(3, 5, "Enter  PIN:");

    if (enter_pin(3, 6, 4, 16, '*', pin, &i2) == FALSE) {
        ClearScreen(4, 26);
        CTOS_LCDTPrintXY(1, 4, "PIN By Pass       ");
        CTOS_Delay(1000);
        return;
    }
    if(utility_code==200){ 
    tmp = "Water";
    memset(utilservice, 0x00, sizeof (utilservice));
    memcpy(utilservice, tmp, strlen(tmp));
    getcarddata(&stRCDataEx);
    tmp = "200";
    memset(utilbillNo, 0x00, sizeof (utilbillNo));
    memcpy(utilbillNo, tmp, strlen(tmp));}
    else if(utility_code==300){ 
    tmp = "KPLC";
    memset(utilservice, 0x00, sizeof (utilservice));
    memcpy(utilservice, tmp, strlen(tmp));
    getcarddata(&stRCDataEx);
    tmp = "300";
    memset(utilbillNo, 0x00, sizeof (utilbillNo));
    memcpy(utilbillNo, tmp, strlen(tmp));}
    else{ 
    tmp = "DSTV";
    memset(utilservice, 0x00, sizeof (utilservice));
    memcpy(utilservice, tmp, strlen(tmp));
    getcarddata(&stRCDataEx);
    tmp = "400";
    memset(utilbillNo, 0x00, sizeof (utilbillNo));
    memcpy(utilbillNo, tmp, strlen(tmp));}
    getcarddata(&stRCDataEx);
    //send request
    
    cardles_utility_post(pin, g_baInputAmt, cardvendor, cardnumber, expdate, ulitityNo, utilservice,utilbillNo);
    usTxResult = stRCDataAnalyze.usTransResult;
    utilityreceipt(&stRCDataEx, NeedSignature, g_ulAmt);
    if (NeedSignature) {
        bStatus = SignatureProcessing();
        if (bStatus != 0) {
            //usTxResult = d_EMVCL_OUTCOME_DECLINED;
        }
    }
}

void utility_cardless(BYTE bType) {
    BYTE bKey;
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
    char *tmp;
    
    
    DebugAddINT("               ", 0);
    DebugAddINT("               ", 0);
    
    
    // amount - ascii to int
    g_ulAmt = ASCII2Int(g_baInputAmt, g_usAmtLen);
    g_ulCBAmt = ASCII2Int(g_baInputCBAmt, g_usCBAmtLen);
    g_ulAmt += g_ulCBAmt;
    
    //Confirm Transaction
    

    STR * keyboardLayoutNumberWithRadixPoint[] = {"0", "1", "2", "3", "4", "5", "6", "7",
        "8", "9", "", "."};
    STR * keyboardLayoutEnglish[] = {" 0", "qzQZ1", "abcABC2", "defDEF3", "ghiGHI4",
        "jklJKL5", "mnoMNO6", "prsPRS7", "tuvTUV8", "wxyWXY9", ":;/\\|?,.<>", ".!@#$%^&*()"};
    STR * keyboardLayoutNumber[] = {"0", "1", "2", "3", "4", "5", "6", "7", "8", "9", "",
        ""};
    BYTE baBuff[256];
   ClearScreen(4, 26);
        CTOS_LCDTPrintXY(2, 5, "Enter Utility No:");
       CTOS_UIKeypad(2, 6, keyboardLayoutNumber, 40, 80, d_FALSE, d_FALSE, 0, 0, ulitityNo,
                    8);
       ClearScreen(4, 26);
    CTOS_LCDTPrintXY(2, 5, "Confirm Payment:"); 
     CTOS_LCDTPrintXY(2, 6, baAmount); 
     CTOS_LCDTPrintXY(9, 6, "To BillNo");
     CTOS_LCDTPrintXY(2, 7, ulitityNo);
     
     //Get the account Number
   ClearScreen(4, 26);
        CTOS_LCDTPrintXY(2, 5, "Enter Account NO:");
       CTOS_UIKeypad(2, 6, keyboardLayoutNumber, 40, 80, d_FALSE, d_FALSE, 0, 0, depositaccnt,15);
       //confirm Account Number
    ClearScreen(4, 26);
    CTOS_LCDTPrintXY(3, 5, "Please Confirm");
    CTOS_LCDTPrintXY(3, 6, "Pay  KSH:  ");
    CTOS_LCDTPrintXY(14, 6, baAmount);
    CTOS_LCDTPrintXY(3, 7, "To Account Number:");
    CTOS_LCDTPrintXY(3, 8, depositaccnt);
     //Pressing okay-Accepting accnt Pressing X returns
    CTOS_LCDTPrintXY(3, 10, " OK TO CONFIRM");
    CTOS_LCDTPrintXY(3, 11, " X TO CANCEL");
     CTOS_KBDGet(&key);
    if (key == d_KBD_ENTER) {
        ClearScreen(4, 26);
    }//Amount not OK, go back to re-enter
    else {
        return;
    }
     if(utility_code==200){ 
    tmp = "Water";
    memset(utilservice, 0x00, sizeof (utilservice));
    memcpy(utilservice, tmp, strlen(tmp));
    getcarddata(&stRCDataEx);
    tmp = "200";
    memset(utilbillNo, 0x00, sizeof (utilbillNo));
    memcpy(utilbillNo, tmp, strlen(tmp));}
    else if(utility_code==300){ 
    tmp = "KPLC";
    memset(utilservice, 0x00, sizeof (utilservice));
    memcpy(utilservice, tmp, strlen(tmp));
    getcarddata(&stRCDataEx);
    tmp = "300";
    memset(utilbillNo, 0x00, sizeof (utilbillNo));
    memcpy(utilbillNo, tmp, strlen(tmp));}
    else{ 
    tmp = "DSTV";
    memset(utilservice, 0x00, sizeof (utilservice));
    memcpy(utilservice, tmp, strlen(tmp));
    getcarddata(&stRCDataEx);
    tmp = "400";
    memset(utilbillNo, 0x00, sizeof (utilbillNo));
    memcpy(utilbillNo, tmp, strlen(tmp));}
    getcarddata(&stRCDataEx);
    //send request

    //send request
    cardles_utility_post(g_baInputAmt, depositaccnt, ulitityNo, utilservice, utilbillNo);
    usTxResult = stRCDataAnalyze.usTransResult;
    utilityreceipt(&stRCDataEx, NeedSignature, g_ulAmt);
    if (NeedSignature) {
        bStatus = SignatureProcessing();
        if (bStatus != 0) {
            //usTxResult = d_EMVCL_OUTCOME_DECLINED;
        }
    }
}

void dorutilitypay() {
    ClearScreen(4, 26);
    EMVCL_StartIdleLEDBehavior(NULL);
    BYTE key;

    ClearScreen(4, 26);
    ShowTitle("  Utility Payment         ");
    CTOS_LCDTPrintXY(2, 5, "1.Water");
    CTOS_LCDTPrintXY(2, 6, "2.KPLC");
    CTOS_LCDTPrintXY(2, 7, "3.DSTV");

    CTOS_KBDGet(&key);

    switch (key) {
        case '1':
            utility_code = 200;
            break;
        case '2':
            utility_code = 300;
            break;

        case '3':
            utility_code = 400;
            break;
        case d_KBD_CANCEL:
            return;
            break;


    }


    ClearScreen(4, 26);
    ulRtn = InputValue();

    if (ulRtn == d_NO) {
        return;
    } else {
    }
    ClearScreen(4, 26);
    ShowTitle("  Utility Payment Method         ");
    CTOS_LCDTPrintXY(2, 5, "1.Card Payment");
    CTOS_LCDTPrintXY(2, 6, "2.Cardless Payment");

    CTOS_KBDGet(&key);

    switch (key) {
        case '1':
            utility_transact(d_INIT_TRANS);
            break;
        case '2':
            utility_cardless(d_INIT_TRANS);
            break;
        case d_KBD_CANCEL:
            return;
            break;
    
    }

}



