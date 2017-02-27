#include <string.h>
#include <stdio.h>
#include <ctosapi.h>
#include <emv_cl.h>
#include "wub_lib.h"
#include "debug2.h"
#include "Transaction.h"
//#include "curlpost.h"
#include "withdraw.h"
#include "cJSON.h"

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

USHORT usTxResult;
char baInput[20];
int iLen;
//BYTE pin[20];
UCHAR pbBuf[1024];
STR sBuf[1024];
BYTE baTmp[5000];
BYTE baBuf[128];
BYTE baTemp[256];
BYTE baTemp2[256];
BYTE baTemp3[256];
BYTE baTemp4[256];
BYTE baTmp1[256];

//
BYTE cvv[3];
BYTE expdate[6];
BYTE cardtype[255];
BYTE cardnumber[17];
BYTE cardvendor[255];
BYTE mpin[5];

BYTE mcvv[3];
BYTE mexpdate[6];
BYTE mcardtype[255];
BYTE mcardnumber[17];
BYTE mcardvendor[255];
BYTE mpin[5];

//
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
BYTE amount[5];
char *cardpin;


BYTE key;

BYTE isSoundTurnON;
BYTE pin[4];
USHORT i;

int enter_amount(USHORT usX, USHORT usY, BYTE isMask, BYTE *pbaStr, USHORT *usStrLen) {
    //Declare Local Variable //
    int i = 0;

    memset(pbaStr, 0x00, *usStrLen);
    CTOS_LCDTPutchXY(usX, usY, '_');


    while (1) {

        //Scan the keyboard to detect whether a key is pressed //
        CTOS_KBDHit(&key);

        //Cancel InputString() //
        if (key == d_KBD_CANCEL) {
            CTOS_LCDTPutchXY(usX + i, usY, ' ');
            memset(pbaStr, 0x00, *usStrLen);
            usStrLen = 0;
            return 0;
        }

        //Done input and output a string //
        if (key == d_KBD_ENTER) {
            CTOS_LCDTPutchXY(usX + i, usY, ' ');
            *usStrLen = strlen(pbaStr);
            if (*usStrLen > 0) return 1;
            else return 0;
        } else if (key == d_KBD_CLEAR) { //Backspace //
            CTOS_LCDTPutchXY(usX + i, usY, ' ');
            i--;
            pbaStr[i] = 0x00;
            CTOS_LCDTPutchXY(usX + i, usY, '_');
        } else if (key == d_KBD_00) { //Clear all input data //
            memset(pbaStr, 0x00, *usStrLen);
            i = 0;
            CTOS_LCDTGotoXY(usX + i, usY);
            CTOS_LCDTClear2EOL();
            CTOS_LCDTPutchXY(usX, usY, '_');
        }
        if (i + 1 <= *usStrLen) {
            if ((key >= 0x30) && (key <= 0x39)) {
                pbaStr[i] = key;

                if (isMask) CTOS_LCDTPutchXY(usX + i, usY, '*');
                else CTOS_LCDTPutchXY(usX + i, usY, pbaStr[i]);
                i++;
                CTOS_LCDTPutchXY(usX + i, usY, '_');
            }
        }
    }
    return i;

}

ULONG ASCII_2Int(BYTE *baSBuf, int iL) {
    unsigned long k;
    unsigned long iN;
    int i;

    iN = 0;
    k = 1;
    for (i = iL - 1; i >= 0; i--) {
        iN = (unsigned long) ((baSBuf[i] - '0') * k) + iN;
        k *= 10;
    }
    return iN;
}

void ShowInput_AmountEx(BYTE bX, BYTE bY, BYTE *pAmtStr, BYTE bAmtStrLen) {
    BYTE temp[16];
    ULONG ulTxAmount;
    char sInputAmount[20];
    int ilen;

    ilen = bAmtStrLen;
    CTOS_LCDTPrintXY(bX, bY, "KSH:");
    ulTxAmount = ASCII_2Int(pAmtStr, ilen);
    sprintf(temp, "KSH: %ld", ulTxAmount);
    memcpy(sInputAmount, &temp[1], strlen(temp) - 1);
    CTOS_LCDTPrintXY(bX, bY, "                ");
    CTOS_LCDTPrintXY(bX, bY, temp);
}

ULONG Input_Amount(BYTE bX, BYTE bY, BYTE* strAmt, USHORT AmtBufSize, USHORT* usAmtLen) {
    BYTE bKey;
    USHORT tempLen;

    memset(strAmt, '0', AmtBufSize);
    tempLen = 0;
    *usAmtLen = 0;

    while (1) {
        CTOS_KBDGet(&bKey);

        switch (bKey) {
            case d_KBD_ENTER: //Enter

                if (tempLen == 0) {
                    strAmt[tempLen++] = '0';
                }
                strAmt[tempLen] = 0;
                *usAmtLen = tempLen;
                return d_OK;

                break;
            case d_KBD_CANCEL: //Cancel
                tempLen = 1;
                strAmt[0] = '0';
                *usAmtLen = tempLen;
                return d_NO;

            case d_KBD_CLEAR: //Clear
                if (tempLen == 0) {
                    CTOS_Delay(100);
                    CTOS_Beep();
                    CTOS_Delay(100);
                    CTOS_Beep();
                } else {
                    tempLen--;
                    strAmt[tempLen] = 0;
                    ShowInput_AmountEx(bX, bY, strAmt, tempLen);
                }
                break;
            default:
                if ((bKey >= '0' && bKey <= '9') && tempLen < 5) {
                    strAmt[tempLen++] = bKey;
                    ShowInput_AmountEx(bX, bY, strAmt, tempLen);
                } else {
                    CTOS_Delay(100);
                    CTOS_Beep();
                    CTOS_Delay(100);
                    CTOS_Beep();
                }
        }

    }


}

BYTE enter_pin(BYTE x, BYTE y, BYTE min, BYTE max, char mask, char *buf, BYTE *len) {
    BYTE ilen;
    BYTE c[2];

    CTOS_LCDTGotoXY(x, y);

    ilen = 0;
    c[1] = 0; //NULL

    while (TRUE) {
        CTOS_KBDGet(c);

        if (c[0] >= '0' && c[0] <= '9') //0-9
        {
            if (x <= 6 && ilen < max) {
                buf[ilen++] = c[0];
                if (mask == 0)
                    CTOS_LCDTPutchXY(x, y, c[0]);
                else
                    CTOS_LCDTPutchXY(x, y, mask);

                x++;
            }
        } else if (c[0] == d_KBD_CLEAR) //clear
        {
            if (ilen > 0) {
                ilen--;
                x--;
                CTOS_LCDTPutchXY(x, y, ' ');
            }
        } else if (c[0] == d_KBD_CANCEL) //cancel
        {
            return FALSE;
        } else if (c[0] == d_KBD_ENTER && ilen >= min) //enter
        {
            *len = ilen;
            buf[ilen] = 0x00; //put end of string
            return TRUE;
        }
    }
}

//------------------------------------------------------------------------------

void ClearScreen(BYTE bStartY, BYTE bEndY) {
    BYTE i;

    for (i = bStartY; i <= bEndY; i++) {
        CTOS_LCDTPrintXY(1, i, "                            ");
    }
}
//------------------------------------------------------------------------------

void UnpackData(BYTE *baSBuf, UINT uiLen, BYTE *baTBuf) {
    UINT i;
    char cMe[3];

    strcpy(baTBuf, "");
    for (i = 0; i < uiLen; i++) {
        sprintf(cMe, "%02X", baSBuf[i]);
        strcat(baTBuf, cMe);
    }
}
//------------------------------------------------------------------------------

void ShowReverseLine(BYTE x, BYTE y, char *str) {
    CTOS_LCDTSetReverse(d_TRUE);
    CTOS_LCDTPrintXY(x, y, (STR *) str);
    CTOS_LCDTSetReverse(d_FALSE);
}
//------------------------------------------------------------------------------

void ShowTitle(char *caStr) {
    CTOS_LCDTSetReverse(d_TRUE);
    CTOS_LCDTPrintXY(1, 3, caStr);
    CTOS_LCDTSetReverse(d_FALSE);
}
//------------------------------------------------------------------------------

void ShowRtn(BYTE y, BYTE *baMessage, ULONG ulRtn) {
    STR sBuf[80];
    BYTE bKey;

    CTOS_LCDTPrintXY(1, y, baMessage);
    y = CTOS_LCDTWhereY();
    sprintf(sBuf, "Rtn:0x%08lX  ", ulRtn);
    CTOS_LCDTPrintXY(1, y + 1, sBuf);
}
//------------------------------------------------------------------------------

ULONG ASCII2Int(BYTE *baSBuf, int iL) {
    unsigned long k;
    unsigned long iN;
    int i;

    iN = 0;
    k = 1;
    for (i = iL - 1; i >= 0; i--) {
        iN = (unsigned long) ((baSBuf[i] - '0') * k) + iN;
        k *= 10;
    }
    return iN;
}
//------------------------------------------------------------------------------

void ShowInputAmountEx(BYTE bX, BYTE bY, BYTE *pAmtStr, BYTE bAmtStrLen) {
    BYTE temp[16];
    ULONG ulTxAmount;
    char sInputAmount[20];
    int ilen;

    ilen = bAmtStrLen;

    ulTxAmount = ASCII2Int(pAmtStr, ilen);
    sprintf(temp, " %ld.%02ld KSH", ulTxAmount / 100, ulTxAmount % 100);
    memcpy(sInputAmount, &temp[1], strlen(temp) - 1);
    CTOS_LCDTPrintXY(bX, bY, "                ");
    CTOS_LCDTPrintXY(bX, bY, temp);
}
//------------------------------------------------------------------------------

ULONG InputAmount(BYTE bX, BYTE bY, BYTE* strAmt, USHORT AmtBufSize, USHORT* usAmtLen) {
    BYTE bKey;
    USHORT tempLen;

    memset(strAmt, '0', AmtBufSize);
    tempLen = 0;
    *usAmtLen = 0;

    while (1) {
        CTOS_KBDGet(&bKey);

        switch (bKey) {
            case d_KBD_ENTER: //Enter

                if (tempLen == 0) {
                    strAmt[tempLen++] = '0';
                }
                strAmt[tempLen] = 0;
                *usAmtLen = tempLen;
                return d_OK;

                break;
            case d_KBD_CANCEL: //Cancel
                tempLen = 1;
                strAmt[0] = '0';
                *usAmtLen = tempLen;
                return d_NO;

            case d_KBD_CLEAR: //Clear
                if (tempLen == 0) {
                    CTOS_Delay(100);
                    CTOS_Beep();
                    CTOS_Delay(100);
                    CTOS_Beep();
                } else {
                    tempLen--;
                    strAmt[tempLen] = 0;
                    ShowInputAmountEx(bX, bY, strAmt, tempLen);
                }
                break;
            default:
                if ((bKey >= '0' && bKey <= '9') && tempLen < 7) {
                    strAmt[tempLen++] = bKey;
                    ShowInputAmountEx(bX, bY, strAmt, tempLen);
                } else {
                    CTOS_Delay(100);
                    CTOS_Beep();
                    CTOS_Delay(100);
                    CTOS_Beep();
                }
        }

    }


}


//------------------------------------------------------------------------------

ULONG InputValue(void) {
    ULONG ulRtn;

    ulRtn = 0;

    memset(g_baInputCBAmt, 0, sizeof (g_baInputCBAmt));
    memset(g_baInputAmt, 0, sizeof (g_baInputAmt));
    g_usCBAmtLen = 0;
    g_usAmtLen = 0;

    CTOS_LCDTPrintXY(3, 4, "Input Amount\n");
    CTOS_LCDTPrintXY(3, 5, "KSH:");

    ulRtn = Input_Amount(3, 5, g_baInputAmt, sizeof (g_baInputAmt), &g_usAmtLen);
    if (ulRtn != d_OK) {
        return ulRtn;
    }

    if (g_bTxntype == 0x09) //Cash Back
    {
        ClearScreen(4, 26);
        CTOS_LCDTPrintXY(3, 4, "Input CB Amt\n");
        CTOS_LCDTPrintXY(3, 5, " ");
        ulRtn = Input_Amount(3, 5, g_baInputCBAmt, sizeof (g_baInputCBAmt), &g_usCBAmtLen);
        return ulRtn;
    }


    return ulRtn;


}
//------------------------------------------------------------------------------

void PrintBlank(void) {
    CTOS_PrinterPutString("\n");
    //CTOS_PrinterPutString("\n");
}
//------------------------------------------------------------------------------

void Print_Receipt(EMVCL_RC_DATA_EX *data, BYTE isNeedSignature, ULONG ulValue) {
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


    //Store ID
    sprintf(baBuf, "Store ID    :      8220101400255");
    CTOS_PrinterPutString(baBuf);

    //Terminal ID
    sprintf(baBuf, "Terminal ID :           01401493");
    CTOS_PrinterPutString(baBuf);
    CTOS_PrinterPutString("================================");

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

    //Trans Type
    CTOS_PrinterPutString("Trans Type : Withdrawal");
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
    sprintf(baBuf, "     AMOUNT: KSH %ld", ulValue);
    CTOS_PrinterPutString(baBuf);
    if (isNeedSignature) {
        sprintf(baBuf, "Signature  ");
        CTOS_PrinterPutString(baBuf);
        PrintBlank();
    }

    CTOS_PrinterPutString("================================");

    PrintBlank();

}

void getcarddata(EMVCL_RC_DATA_EX *data/*,BYTE *tmpcardtype,BYTE *tmpcarvendor,BYTE tmpcardnumber[17], BYTE tmpcvv*/) {
    CTOS_FONT_ATTRIB stFONT_ATTRIB;

    char *tmp;
    //Card Type

    if (data->bSID == d_VW_SID_PAYPASS_MAG_STRIPE || data->bSID == d_VW_SID_PAYPASS_MCHIP) {

        tmp = "Master PayPass";
        memset(cardtype, 0x00, sizeof (cardtype));
        memcpy(cardtype, tmp, strlen(tmp));
    } else if (data->bSID == d_VW_SID_AE_EMV || data->bSID == d_VW_SID_AE_MAG_STRIPE) {

        tmp = "American Express";
        memset(cardtype, 0x00, sizeof (cardtype));
        memcpy(cardtype, tmp, strlen(tmp));
    } else if (data->bSID == d_VW_SID_JCB_WAVE_2 || data->bSID == d_VW_SID_JCB_WAVE_QVSDC) {

        tmp = "J/Speedy";
        memset(cardtype, 0x00, sizeof (cardtype));
        memcpy(cardtype, tmp, strlen(tmp));
    } else if (data->bSID == d_VW_SID_VISA_OLD_US || data->bSID == d_VW_SID_VISA_WAVE_2 || data->bSID == d_VW_SID_VISA_WAVE_QVSDC || data->bSID == d_VW_SID_VISA_WAVE_MSD) {

        tmp = " Visa payWave";
        memset(cardtype, 0x00, sizeof (cardtype));
        memcpy(cardtype, tmp, strlen(tmp));
    } else if (data->bSID == d_VW_DISCOVER) {
        tmp = " Discover Zip";
        memset(cardtype, 0x00, sizeof (cardtype));
        memcpy(cardtype, tmp, strlen(tmp));
    } else {
        tmp = " Un-Know";
        memset(cardtype, 0x00, sizeof (cardtype));
        memcpy(cardtype, tmp, strlen(tmp));
    }
    //CTOS_LCDTPrintXY(2, 5, cardtype);
    //CTOS_KBDGet(&key);


    //Card TypeVendor
    if (data->bSID == d_VW_SID_PAYPASS_MAG_STRIPE || data->bSID == d_VW_SID_PAYPASS_MCHIP) {
        tmp = " Discover Zip";
        memset(cardvendor, 0x00, sizeof (cardvendor));
        memcpy(cardvendor, tmp, strlen(tmp));
    } else if (data->bSID == d_VW_SID_AE_EMV || data->bSID == d_VW_SID_AE_MAG_STRIPE) {
        tmp = "AEMX";
        memset(cardvendor, 0x00, sizeof (cardvendor));
        memcpy(cardvendor, tmp, strlen(tmp));
    } else if (data->bSID == d_VW_SID_JCB_WAVE_2 || data->bSID == d_VW_SID_JCB_WAVE_QVSDC) {
        tmp = "JCB";
        memset(cardvendor, 0x00, sizeof (cardvendor));
        memcpy(cardvendor, tmp, strlen(tmp));
    } else if (data->bSID == d_VW_SID_VISA_OLD_US || data->bSID == d_VW_SID_VISA_WAVE_2 || data->bSID == d_VW_SID_VISA_WAVE_QVSDC || data->bSID == d_VW_SID_VISA_WAVE_MSD) {
        tmp = "VISA";
        memset(cardvendor, 0x00, sizeof (cardvendor));
        memcpy(cardvendor, tmp, strlen(tmp));
    } else if (data->bSID == d_VW_DISCOVER) {
        tmp = "DISCOVER";
        memset(cardvendor, 0x00, sizeof (cardvendor));
        memcpy(cardvendor, tmp, strlen(tmp));
    } else {
        tmp = " UN-KMOW";
        memset(cardvendor, 0x00, sizeof (cardvendor));
        memcpy(cardvendor, tmp, strlen(tmp));
    }
    //CTOS_LCDTPrintXY(2, 6, cardvendor);
    //CTOS_KBDGet(&key);

    //Card Number
    if (data->bSID == d_VW_SID_VISA_OLD_US || data->bSID == d_VW_SID_VISA_WAVE_2 || data->bSID == d_VW_SID_VISA_WAVE_QVSDC || data->bSID == d_VW_SID_VISA_WAVE_MSD) {

        memset(cardnumber, 0x00, sizeof (cardnumber));
        memcpy(cardnumber, &data->baTrack2Data[1], 16);


    } else {
        UnpackData(data->baTrack2Data, 20, baTmp);
        memcpy(baTemp, cardnumber, 16);
        //memcpy(baTemp2, &baTmp[12], 4);
        baTemp2[16] = 0;

    }

    //CTOS_LCDTPrintXY(2, 6, cardnumber);
    //CTOS_KBDGet(&key);

    //Exp Date
    if (data->bSID == d_VW_SID_VISA_OLD_US || data->bSID == d_VW_SID_VISA_WAVE_2 || data->bSID == d_VW_SID_VISA_WAVE_QVSDC || data->bSID == d_VW_SID_VISA_WAVE_MSD) {
        memcpy(baTemp, &data->baTrack2Data[20], 4);
        baTemp[2] = 0;
        memcpy(baTmp1, &data->baTrack2Data[18], 4);
        baTmp1[2] = 0;


    } else {
        memcpy(baTemp, baTmp + 19, 4);
        baTemp[2] = 0;
        memcpy(baTmp1, baTmp + 17, 4);
        baTmp1[2] = 0;

    }
    char *separator = "/";
    char *result = malloc(strlen(baTemp) + strlen(separator) + 1); //+1 for the zero-terminator
    tmp = malloc(strlen(result) + strlen(baTmp1) + 1);
    memset(expdate, 0x00, sizeof (expdate));
    memcpy(expdate, tmp, strlen(tmp));
    //in real code you would check for errors in malloc here

    strcpy(result, baTemp);
    strcat(result, separator);
    strcpy(expdate, result);
    strcat(expdate, baTmp1);
    //CTOS_LCDTPrintXY(2, 9, baTemp);
    //CTOS_KBDGet(&key);
    //CTOS_LCDTPrintXY(2, 11, baTmp1);
    //CTOS_KBDGet(&key);
    //CTOS_LCDTPrintXY(2, 12, result);
    //CTOS_KBDGet(&key);
    //CTOS_LCDTPrintXY(2, 13, expdate);
    //CTOS_KBDGet(&key);



}

void getmarchantcarddata(EMVCL_RC_DATA_EX *data/*,BYTE *tmpcardtype,BYTE *tmpcarvendor,BYTE tmpcardnumber[17], BYTE tmpcvv*/) {
    CTOS_FONT_ATTRIB stFONT_ATTRIB;

    char *tmp;
    //Card Type

    if (data->bSID == d_VW_SID_PAYPASS_MAG_STRIPE || data->bSID == d_VW_SID_PAYPASS_MCHIP) {

        tmp = "Master PayPass";
        memset(mcardtype, 0x00, sizeof (mcardtype));
        memcpy(mcardtype, tmp, strlen(tmp));
    } else if (data->bSID == d_VW_SID_AE_EMV || data->bSID == d_VW_SID_AE_MAG_STRIPE) {

        tmp = "American Express";
        memset(mcardtype, 0x00, sizeof (mcardtype));
        memcpy(mcardtype, tmp, strlen(tmp));
    } else if (data->bSID == d_VW_SID_JCB_WAVE_2 || data->bSID == d_VW_SID_JCB_WAVE_QVSDC) {

        tmp = "J/Speedy";
        memset(mcardtype, 0x00, sizeof (mcardtype));
        memcpy(mcardtype, tmp, strlen(tmp));
    } else if (data->bSID == d_VW_SID_VISA_OLD_US || data->bSID == d_VW_SID_VISA_WAVE_2 || data->bSID == d_VW_SID_VISA_WAVE_QVSDC || data->bSID == d_VW_SID_VISA_WAVE_MSD) {

        tmp = " Visa payWave";
        memset(mcardtype, 0x00, sizeof (mcardtype));
        memcpy(mcardtype, tmp, strlen(tmp));
    } else if (data->bSID == d_VW_DISCOVER) {
        tmp = " Discover Zip";
        memset(mcardtype, 0x00, sizeof (mcardtype));
        memcpy(mcardtype, tmp, strlen(tmp));
    } else {
        tmp = " Un-Know";
        memset(mcardtype, 0x00, sizeof (mcardtype));
        memcpy(mcardtype, tmp, strlen(tmp));
    }
    //CTOS_LCDTPrintXY(2, 5, cardtype);
    //CTOS_KBDGet(&key);


    //Card TypeVendor
    if (data->bSID == d_VW_SID_PAYPASS_MAG_STRIPE || data->bSID == d_VW_SID_PAYPASS_MCHIP) {
        tmp = " Discover Zip";
        memset(mcardvendor, 0x00, sizeof (mcardvendor));
        memcpy(mcardvendor, tmp, strlen(tmp));
    } else if (data->bSID == d_VW_SID_AE_EMV || data->bSID == d_VW_SID_AE_MAG_STRIPE) {
        tmp = "AEMX";
        memset(mcardvendor, 0x00, sizeof (mcardvendor));
        memcpy(mcardvendor, tmp, strlen(tmp));
    } else if (data->bSID == d_VW_SID_JCB_WAVE_2 || data->bSID == d_VW_SID_JCB_WAVE_QVSDC) {
        tmp = "JCB";
        memset(mcardvendor, 0x00, sizeof (mcardvendor));
        memcpy(mcardvendor, tmp, strlen(tmp));
    } else if (data->bSID == d_VW_SID_VISA_OLD_US || data->bSID == d_VW_SID_VISA_WAVE_2 || data->bSID == d_VW_SID_VISA_WAVE_QVSDC || data->bSID == d_VW_SID_VISA_WAVE_MSD) {
        tmp = "VISA";
        memset(mcardvendor, 0x00, sizeof (mcardvendor));
        memcpy(mcardvendor, tmp, strlen(tmp));
    } else if (data->bSID == d_VW_DISCOVER) {
        tmp = "DISCOVER";
        memset(mcardvendor, 0x00, sizeof (mcardvendor));
        memcpy(mcardvendor, tmp, strlen(tmp));
    } else {
        tmp = " UN-KMOW";
        memset(mcardvendor, 0x00, sizeof (mcardvendor));
        memcpy(mcardvendor, tmp, strlen(tmp));
    }
    //CTOS_LCDTPrintXY(2, 6, cardvendor);
    //CTOS_KBDGet(&key);

    //Card Number
    if (data->bSID == d_VW_SID_VISA_OLD_US || data->bSID == d_VW_SID_VISA_WAVE_2 || data->bSID == d_VW_SID_VISA_WAVE_QVSDC || data->bSID == d_VW_SID_VISA_WAVE_MSD) {

        memset(mcardnumber, 0x00, sizeof (mcardnumber));
        memcpy(mcardnumber, &data->baTrack2Data[1], 16);


    } else {
        UnpackData(data->baTrack2Data, 20, baTmp);
        memcpy(baTemp, mcardnumber, 16);
        //memcpy(baTemp2, &baTmp[12], 4);
        baTemp2[16] = 0;

    }

    //CTOS_LCDTPrintXY(2, 6, cardnumber);
    //CTOS_KBDGet(&key);

    //Exp Date
    if (data->bSID == d_VW_SID_VISA_OLD_US || data->bSID == d_VW_SID_VISA_WAVE_2 || data->bSID == d_VW_SID_VISA_WAVE_QVSDC || data->bSID == d_VW_SID_VISA_WAVE_MSD) {
        memcpy(baTemp, &data->baTrack2Data[20], 4);
        baTemp[2] = 0;
        memcpy(baTmp1, &data->baTrack2Data[18], 4);
        baTmp1[2] = 0;


    } else {
        memcpy(baTemp, baTmp + 19, 4);
        baTemp[2] = 0;
        memcpy(baTmp1, baTmp + 17, 4);
        baTmp1[2] = 0;

    }
    char *separator = "/";
    char *result = malloc(strlen(baTemp) + strlen(separator) + 1); //+1 for the zero-terminator
    tmp = malloc(strlen(result) + strlen(baTmp1) + 1);
    memset(mexpdate, 0x00, sizeof (mexpdate));
    memcpy(mexpdate, tmp, strlen(tmp));
    //in real code you would check for errors in malloc here

    strcpy(result, baTemp);
    strcat(result, separator);
    strcpy(mexpdate, result);
    strcat(mexpdate, baTmp1);
    //CTOS_LCDTPrintXY(2, 9, baTemp);
    //CTOS_KBDGet(&key);
    //CTOS_LCDTPrintXY(2, 11, baTmp1);
    //CTOS_KBDGet(&key);
    //CTOS_LCDTPrintXY(2, 12, result);
    //CTOS_KBDGet(&key);
    //CTOS_LCDTPrintXY(2, 13, expdate);
    //CTOS_KBDGet(&key);



}
//------------------------------------------------------------------------------

void TLVDataClear(void) {
    TLVDataLen = 0;
}
//------------------------------------------------------------------------------

void TLVDataRemove(ULONG tag) {
    WORD i;
    WORD j;
    USHORT len;
    ULONG tag_value;
    USHORT len_value;
    USHORT data_offset;
    BYTE temp[10];

    i = j = 0;
    while (i < TLVDataLen) {
        data_offset = TLV_Get_Value(&TLVData[i], &tag_value, &len_value);
        len = data_offset + len_value;
        if (tag == tag_value) {
            i += len;
            continue;
        } else {

            memcpy(&TLVData[j], &TLVData[i], len);
            j += len;
            i += len;

        }
    }

    TLVDataLen = j;
}
//------------------------------------------------------------------------------

short TLVDataAdd(ULONG tag, USHORT len, BYTE *buf) {
    USHORT i;
    BYTE str[20];
    ULONG tag_value;
    USHORT len_value;
    USHORT data_offset;
    BYTE temp[10];

    //sprintf(str, "Tag %04X ", tag);
    //DebugAddHEX(str, buf, len);

    i = 0;

    while (i < TLVDataLen) {
        data_offset = TLV_Get_Value(&TLVData[i], &tag_value, &len_value);
        if (tag == tag_value) {
            TLVDataRemove(tag);
            break;
        }

        i += (data_offset + len_value);
    }

    //Add new
    i = 0;

    if ((tag >> 16) > 0) {
        temp[i++] = (BYTE) (tag >> 16);
    }
    if ((tag >> 8) > 0) {
        temp[i++] = (BYTE) (tag >> 8);
    }
    temp[i++] = (BYTE) tag;

    if (len > 0xFF) {
        temp[i++] = 0x82;
        temp[i++] = len / 256;
        temp[i++] = len % 256;
    } else if (len > 0x7F) {
        temp[i++] = 0x81;
        temp[i++] = (BYTE) len;
    } else {
        temp[i++] = (BYTE) len;
    }

    if ((TLVDataLen + i + len) > d_TLV_DATA_BUF_SIZE) {
        return 1;
    }

    memcpy(&TLVData[TLVDataLen], temp, i);
    TLVDataLen += i;

    memcpy(&TLVData[TLVDataLen], buf, len);
    TLVDataLen += len;

    return 0;
}
//------------------------------------------------------------------------------

short TLVDataGet(ULONG tag, BYTE *buf) {
    USHORT i;
    ULONG tag_value;
    USHORT len_value;
    USHORT data_offset;

    i = 0;

    while (i < TLVDataLen) {
        data_offset = TLV_Get_Value(&TLVData[i], &tag_value, &len_value);
        if (tag == tag_value) {
            memcpy(buf, &TLVData[i + data_offset], len_value);
            return len_value;
        }

        i += (data_offset + len_value);
    }

    return 0;
}
//------------------------------------------------------------------------------

void TLVDataParse(BYTE *buf, USHORT len) {
    ULONG tag_value;
    USHORT len_value;
    USHORT i;
    USHORT offset;

    i = 0;
    while (i < len) {
        offset = TLV_Get_Value(&buf[i], &tag_value, &len_value);
        TLVDataAdd(tag_value, len_value, &buf[i + offset]);
        i += offset + len_value;
    }
}
//------------------------------------------------------------------------------

USHORT Online_Process(BYTE *baData, USHORT usDataLen) {
    BYTE baBuff[2048];
    BYTE key;
    USHORT usBuffLen;
    USHORT i;
    USHORT ret;

    //Send data to host
    usBuffLen = 0;
    baBuff[usBuffLen++] = 0x10;
    baBuff[usBuffLen++] = 0x02;
    for (i = 0; i < usDataLen; i++) {
        baBuff[usBuffLen++] = baData[i];
        if (baData[i] == 0x10) {
            baBuff[usBuffLen++] = baData[i];
        }
    }
    baBuff[usBuffLen++] = 0x00; //Ignore LRC check
    baBuff[usBuffLen++] = 0x10;
    baBuff[usBuffLen++] = 0x03;

    DebugAddHEX("Data to Host", baBuff, usBuffLen);

    if (g_IsHostBusy) {
        //IsHostBusy = FALSE;
        return d_EMVCL_OUTCOME_DECLINED;
    }

    //Receive Response
    do {
#ifdef _SIMULATE_ONLINE_RESULT
        CTOS_KBDHit(&key);

        if (key == '1') {
            ret = d_EMVCL_OUTCOME_APPROVAL;
            break;
        } else if (key == '2') {
            ret = d_EMVCL_OUTCOME_DECLINED;
            break;
        }
#else

        i = sizeof (baBuff);
        if (CTOS_RS232RxData(d_COM1, baBuff, &i) == d_OK && i > 0) {
            if (baBuff[0] == '1') {
                ret = d_EMV_CHIP_ONL_APPROVAL;
            } else {
                ret = d_EMV_CHIP_ONL_DECLINED;
            }
            break;
        }

        CTOS_KBDHit(&key);

#endif 

    } while (key != d_KBD_CANCEL);

    if (key == d_KBD_CANCEL) {
        ret = d_EMVCL_OUTCOME_DECLINED;
    }

    return ret;
}
//------------------------------------------------------------------------------
//	x: X position
//	y: Y position
//	min: Minimum length of the input data
//	max: Maximum length of the input data
//	mask: If maskchar = 0, don't need mask char, if > 0, it will show to screen.

char Get_PIN_Input(BYTE x, BYTE y, BYTE min, BYTE max, char mask, char *buf, BYTE *len) {
    BYTE ilen;
    BYTE c[2];

    CTOS_LCDTGotoXY(x, y);

    ilen = 0;
    c[1] = 0; //NULL

    while (TRUE) {
        CTOS_KBDGet(c);

        if (c[0] >= '0' && c[0] <= '9') //0-9
        {
            if (x <= 4 && ilen < max) {
                buf[ilen++] = c[0];
                if (mask == 0)
                    CTOS_LCDTPutchXY(x, y, c[0]);
                else
                    CTOS_LCDTPutchXY(x, y, mask);

                x++;
            }
        } else if (c[0] == d_KBD_CLEAR) //clear
        {
            if (ilen > 0) {
                ilen--;
                x--;
                CTOS_LCDTPutchXY(x, y, ' ');
            }
        } else if (c[0] == d_KBD_CANCEL) //cancel
        {
            return FALSE;
        } else if (c[0] == d_KBD_ENTER && ilen >= min) //enter
        {
            *len = ilen;
            buf[ilen] = 0x00; //put end of string
            return TRUE;
        }
    }
}
//------------------------------------------------------------------------------

BYTE SignatureProcessing(void) {
    BYTE bKey;

    ClearScreen(4, 26);
    CTOS_LCDTPrintXY(1, 4, "Print receipt ?       ");
    CTOS_LCDTPrintXY(1, 5, "Enter = Yes           ");
    CTOS_LCDTPrintXY(1, 6, "Cancel = No           ");

    while (1) {
        CTOS_KBDGet(&bKey);

        if (bKey == d_KBD_ENTER) {
            break;
        } else if (bKey == d_KBD_CANCEL) {
            return 1;
        }
    }

    CTOS_LCDTPrintXY(1, 4, "Signature is correct ?");
    CTOS_LCDTPrintXY(1, 5, "Enter = Yes           ");
    CTOS_LCDTPrintXY(1, 6, "Cancel = No           ");

    while (1) {
        CTOS_KBDGet(&bKey);

        if (bKey == d_KBD_ENTER) {
            return 0;
        } else if (bKey == d_KBD_CANCEL) {
            return 2;
        }
    }
}
//------------------------------------------------------------------------------

void StartTrans(BYTE bType) {
    BYTE bKey;
    BYTE NeedSignature;
    BYTE temp[20];
    BYTE baAmount[32];
    BYTE upload_tx_buf[1024];
    USHORT upload_tx_len;
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
    DebugAddINT("               ", 0);
    DebugAddINT("               ", 0);

    // amount - ascii to int
    g_ulAmt = ASCII2Int(g_baInputAmt, g_usAmtLen);
    g_ulCBAmt = ASCII2Int(g_baInputCBAmt, g_usCBAmtLen);
    g_ulAmt += g_ulCBAmt;

    //Confirm Transaction

    ClearScreen(4, 26);
    CTOS_LCDTPrintXY(2, 4, " Please Confirm");
    CTOS_LCDTPrintXY(2, 6, "   Withdraw");
    sprintf(temp, "   KSH %ld%02ld", g_ulAmt / 100, g_ulAmt % 100);
    memset(baAmount, 0x00, sizeof (baAmount));
    memcpy(baAmount, temp, strlen(temp));
    CTOS_LCDTPrintXY(2, 8, baAmount);
    CTOS_LCDTPrintXY(1, 10, "   OK TO CONFIRM");
    CTOS_LCDTPrintXY(1, 12, "   X TO CANCEL");
    CTOS_KBDGet(&key);
    //Amount okay Continue to Read Card
    if (key == d_KBD_ENTER) {
       ClearScreen(4, 26);
        CTOS_LCDTPrintXY(2, 4, d_MSG_PRESENT_CARD);
       //EMVCL_ShowContactlessSymbol(NULL);
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
    //=================================================================================================================//
    //Get client Card Details
    getcarddata(&stRCDataEx);
    usTxResult = stRCDataAnalyze.usTransResult;
    ClearScreen(4, 26);
    //Present marchant card
    //ClearScreen(4, 26);
    CTOS_LCDTPrintXY(2, 4, d_MSG_PRESENT_mCARD);
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
                CTOS_LCDTPrintXY(1, 4, d_MSG_PRESENT_mCARD);
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
        int i3 = 0;
        CTOS_LCDTPrintXY(1, 4, "Enter ONL PIN :");

        if (enter_pin(3, 6, 4, 16, '*', pin, &i3) == FALSE) {
            ClearScreen(4, 26);
            CTOS_LCDTPrintXY(1, 4, "PIN By Pass       ");
            CTOS_Delay(1000);
            return;
        }

    } else if (stRCDataAnalyze.bCVMAnalysis == d_EMVCL_CVM_REQUIRED_ONLPIN) {
        CVMStr = "     CVM->PIN      ";
        ClearScreen(4, 26);
        //CTOS_LCDTPrintXY(1, 4, "PIN CVM MODE ");
        //CTOS_Delay(1000);
        int i3 = 0;
        CTOS_LCDTPrintXY(1, 4, "Enter ONL PIN :");

        if (enter_pin(3, 6, 4, 16, '*', pin, &i3) == FALSE) {
            ClearScreen(4, 26);
            CTOS_LCDTPrintXY(1, 4, "PIN By Pass       ");
            CTOS_Delay(1000);
            return;
        }

        //set TVR byte 3 if need

        TLVDataGet(0x95, temp);
        temp[2] |= 0x04;
        TLVDataAdd(0x95, 5, temp);
    } else if (stRCDataAnalyze.bCVMAnalysis == d_EMVCL_CVM_REQUIRED_NOCVM) {
        CVMStr = "   CVM->No CVM Req  ";
        ClearScreen(4, 26);
        //CTOS_LCDTPrintXY(1, 4, "NO CVM REQUIRED ");
        //CTOS_Delay(1000);
        int i3 = 0;
        CTOS_LCDTPrintXY(1, 4, "Enter ONL PIN :");

        if (enter_pin(3, 6, 4, 16, '*', pin, &i3) == FALSE) {
            ClearScreen(4, 26);
            CTOS_LCDTPrintXY(1, 4, "PIN By Pass       ");
            CTOS_Delay(1000);
            return;
        }
    } else {
        int i4 = 0;
        ClearScreen(4, 26);
        CTOS_LCDTPrintXY(3, 5, "Enter DEFAULT  PIN:");

        if (enter_pin(3, 6, 4, 16, '*', pin, &i4) == FALSE) {
            ClearScreen(4, 26);
            CTOS_LCDTPrintXY(1, 4, "PIN By Pass       ");
            CTOS_Delay(1000);
            return;
        }
    }

    // TODO: Add your program here //



    //get card details
    getmarchantcarddata(&stRCDataEx);
    //SEND REQUEST

    if (usTxResult == d_EMVCL_OUTCOME_ONL) {


        withdraw_post(pin, g_baInputAmt, cardvendor, cardnumber, expdate, mpin, mcardvendor, mcardnumber, mexpdate);

        //        
        //        ClearScreen(4, 26);
        //        //CTOS_LCDTPrintXY(1, 5, d_MSG_ONLINE);
        //        //CTOS_LCDTPrintXY(1, 6, "                    ");
        //
        //        //Rrepare Upload Data to host
        //        upload_tx_len = 0;
        //
        //        upload_tx_buf[upload_tx_len++] = 14;
        //        memcpy(&upload_tx_buf[upload_tx_len], stRCDataEx.baDateTime, 14);
        //        upload_tx_len += 14;
        //
        //        upload_tx_buf[upload_tx_len++] = stRCDataEx.bTrack1Len;
        //        memcpy(&upload_tx_buf[upload_tx_len], stRCDataEx.baTrack1Data, stRCDataEx.bTrack1Len);
        //        upload_tx_len += stRCDataEx.bTrack1Len;
        //
        //        upload_tx_buf[upload_tx_len++] = stRCDataEx.bTrack2Len;
        //        memcpy(&upload_tx_buf[upload_tx_len], stRCDataEx.baTrack2Data, stRCDataEx.bTrack2Len);
        //        upload_tx_len += stRCDataEx.bTrack2Len;
        //
        //        upload_tx_buf[upload_tx_len++] = stRCDataEx.usChipDataLen / 256;
        //        upload_tx_buf[upload_tx_len++] = stRCDataEx.usChipDataLen % 256;
        //        memcpy(&upload_tx_buf[upload_tx_len], stRCDataEx.baChipData, stRCDataEx.usChipDataLen);
        //        upload_tx_len += stRCDataEx.usChipDataLen;
        //
        //        upload_tx_buf[upload_tx_len++] = stRCDataEx.usAdditionalDataLen / 256;
        //        upload_tx_buf[upload_tx_len++] = stRCDataEx.usAdditionalDataLen % 256;
        //        memcpy(&upload_tx_buf[upload_tx_len], stRCDataEx.baAdditionalData, stRCDataEx.usAdditionalDataLen);
        //        upload_tx_len += stRCDataEx.usAdditionalDataLen;
        //
        //        //send upload data and get the online authen result
        //        usTxResult = Online_Process(upload_tx_buf, upload_tx_len);


    }

    Print_Receipt(&stRCDataEx, NeedSignature, g_ulAmt);
    if (NeedSignature) {
        bStatus = SignatureProcessing();
        if (bStatus != 0) {
            //usTxResult = d_EMVCL_OUTCOME_DECLINED;
        }
    }
    ClearScreen(4, 26);
    CTOS_LCDTPrintXY(3, 6, "Completed");
    return;
    //ClearScreen(4, 26);
    CTOS_LCDTPrintXY(1, 5, msg);


    if (usTxResult == d_EMVCL_OUTCOME_APPROVAL) {
        if (g_bTxntype == 0x20) {
            ClearScreen(4, 26);
            CTOS_LCDTPrintXY(1, 6, d_MSG_REFUND);
        } else {
            ClearScreen(4, 26);
            CTOS_LCDTPrintXY(1, 6, d_MSG_APPROVED);
        }
        CTOS_LCDTPrintXY(1, 7, baAmount);

        CTOS_LCDTPrintXY(1, 8, CVMStr);
        if (stRCDataAnalyze.bVisaAOSAPresent == TRUE) {
            ulValue = wub_bcd_2_long(stRCDataAnalyze.baVisaAOSA, 6);
            DebugAddINT("baVisaAOSA", ulValue);
            sprintf(temp, "   KSH %ld.%02ld    ", ulValue / 100, ulValue % 100);
            CTOS_LCDTPrintXY(1, 9, temp);
        }
        CTOS_Delay(3000);
    } else if (usTxResult == d_EMVCL_OUTCOME_DECLINED) {
        if (g_IsHostBusy) {
            g_IsHostBusy = FALSE;
            ClearScreen(4, 26);
            CTOS_LCDTPrintXY(1, 6, d_MSG_HOST_BUSY);
            CTOS_LCDTPrintXY(1, 7, d_MSG_DECLINE);
        } else {
            ClearScreen(4, 26);
            CTOS_LCDTPrintXY(1, 6, d_MSG_DECLINE);
        }

        CTOS_LCDTPrintXY(1, 8, CVMStr);
        if (stRCDataAnalyze.bVisaAOSAPresent == TRUE) {
            ulValue = wub_bcd_2_long(stRCDataAnalyze.baVisaAOSA, 6);
            DebugAddINT("baVisaAOSA", ulValue);
            sprintf(temp, "   KSH %ld.%02ld    ", ulValue / 100, ulValue % 100);
            CTOS_LCDTPrintXY(1, 9, temp);
        }
        CTOS_Delay(3000);
    } else {
        CTOS_LCDTPrintXY(1, 6, "Unknow Result");
        CTOS_KBDGet(&bKey);
        return;
    }

    return;
}
//------------------------------------------------------------------------------

void SetHostBusy(void) {
    g_IsHostBusy = TRUE;
    CTOS_LCDTPrintXY(1, 8, "Set Host Busy OK!!");
    CTOS_Delay(1000);
}
//------------------------------------------------------------------------------

void SelectTxnType(void) {
    BYTE bKey;

    ClearScreen(4, 26);
    ShowTitle("  Select Txn Type     ");
    CTOS_LCDTPrintXY(1, 4, "1.	Purchase");
    CTOS_LCDTPrintXY(1, 5, "2.	Cash");
    CTOS_LCDTPrintXY(1, 6, "3.	Cash Back");
    CTOS_LCDTPrintXY(1, 7, "4.	Refund");

    do {
        CTOS_KBDGet(&bKey);

        switch (bKey) {
            case d_KBD_1: //Purchase
                g_bTxntype = 0x00;
                break;
            case d_KBD_2: //Cash
                g_bTxntype = 0x01;
                break;
            case d_KBD_3: //Cash Back
                g_bTxntype = 0x09;
                break;
            case d_KBD_4: //Refund
                g_bTxntype = 0x20;
                break;
            case d_KBD_CANCEL: //Cancel
                return;
            default:
                continue;
        }

    } while (0);

    CTOS_LCDTPrintXY(1, 8, " Set OK!!");
    CTOS_Delay(1000);
}
//------------------------------------------------------------------------------

void Transaction(void) {
    ULONG ulRtn;
    BYTE *pstr;
    BYTE key;

    //Perform Transaction via Transaction APIs	
    iBatchNo = 0;

    ClearScreen(4, 26);
    EMVCL_StartIdleLEDBehavior(NULL);
    ShowTitle("  Cash Withdraw          ");
    ulRtn = InputValue();

    if (ulRtn == d_NO) {
        return;
    } else {
    }
    //If the mutual authen was ok, start a transaction.


    StartTrans(d_INIT_TRANS);

    CTOS_Delay(1000);


    EMVCL_StopIdleLEDBehavior(NULL);
    EMVCL_SetLED(0x0F, 0x08);
}

void Do_Transaction(void) {
    //BYTE bKey;

    g_IsHostBusy = FALSE;

    while (1) {
        ClearScreen(4, 26);
        ShowTitle("    Cash Withdraw     ");
        CTOS_LCDTPrintXY(3, 5, "   Welcome ");
        CTOS_LCDTPrintXY(3, 6, "OK to Withdraw ");
        CTOS_KBDGet(&key);
        switch (key) {
            case d_KBD_ENTER:
                Transaction();
                break;

            case d_KBD_CANCEL:
                return;
                break;
        }
    }

}

