#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <ctosapi.h>
#include "wub_lib.h"
#include "Transaction.h"


#define BUF_SIZE (1024)	
#define GPRS_RX_BUF_SIZE (1500) // must bigger than TCP windows size 1460
#define d_BUFF_SIZE 128			    // Buff Size

BYTE key, state, bSocket, bNo;
int i, iY;
BYTE baIP_S[] = "\x00\x00\x00\x00";
BYTE baIP_G[4];
STR strAPN[9];
STR baID[6];
STR baPW[6];
//BYTE baIP_R [] = {0xDA, 0xD3, 0x23, 0xDB}; 
BYTE baIP_R [] = {197, 232, 39, 171};
//USHORT usPort = 8019;
//BYTE baIP_R [] = {218,211,35,220};
USHORT usPort = 80;
DWORD pdwState = 0;
BYTE baBuffer_Tx[BUF_SIZE] = {"\x00,\x5D,\x60,\x01,\x18,\x00,\x00,\x05,\x00,\x20,\x20,\x01,\x00,\x00,\xC0,\x00,\x12,\x92,\x00,\x00,\x00,\x00,\x09,\x01,\x18,\x30,\x30,\x30,\x30,\x36,\x30,\x30,\x31,\x30,\x30,\x38,\x39,\x37,\x33,\x31,\x31,\x34,\x36,\x36,\x31,\x30,\x30,\x31,\x00,\x36,\x30,\x30,\x30,\x30,\x30,\x30,\x30,\x30,\x30,\x30,\x30,\x30,\x30,\x30,\x30,\x30,\x30,\x30,\x30,\x30,\x30,\x30,\x30,\x30,\x30,\x30,\x30,\x30,\x30,\x30,\x30,\x30,\x30,\x30,\x30,\x30,\x00,\x07,\x30,\x30,\x30,\x30,\x30,\x31,\x59"};
BYTE baBuffer_Rx[GPRS_RX_BUF_SIZE];
USHORT usLen_Rx = 0, TotalLen = 0, ret;
BYTE str[20];
USHORT usRtn;
DWORD start, end, distance;
ULONG ulTime;
BYTE babuff[d_BUFF_SIZE];

/*===================================================================
 * FUNCTION NAME:   Check_state
 * DESCRIPTION:   Check the status and the return value
 * RETURN:        TRUE and FALSE
 * NOTES:         none
 *==================================================================*/
BOOL Check_state(USHORT RET) {
    BYTE key = 0;
    BYTE str[20];
    DWORD State;
    while (RET == d_TCP_IO_PROCESSING) {
        RET = CTOS_TCP_GPRSStatus(&State);
        sprintf(str, "Status = %X    ", State);
        CTOS_LCDTPrintXY(1, 6, str);
        CTOS_KBDHit(&key);
        if (key == d_KBD_CANCEL)
            break;
    }
    RET = CTOS_TCP_GPRSStatus(&State);
    sprintf(str, "Status = %X    ", State);
    CTOS_LCDTPrintXY(1, 6, str);
    if (RET != d_OK) {
        sprintf(str, "Ret = %X   ", RET);
        CTOS_LCDTPrintXY(1, 7, str);
        return FALSE;
    }
    CTOS_Beep();
    return TRUE;
}

void GSMtest(void) {
    USHORT rc = 0;
    BYTE key;
    BYTE imsi[128];
    BYTE name[128];
    BYTE strength = 99;
    BYTE nlen;
    BYTE sBuf[128];
    BYTE State;
    BYTE bID;
    ClearScreen(4, 14);
    CTOS_LCDTPrintXY(1, 4, "Select SIM");

    CTOS_LCDTPrintXY(1, 5, "1. SIM1         ");
    CTOS_LCDTPrintXY(1, 6, "2. SIM2         ");
    CTOS_KBDGet(&key);

    CTOS_LCDTPrintXY(1, 5, "GSM Open:       ");
    CTOS_LCDTPrintXY(1, 6, "                ");
    // Open GSM channel and initial to start to use the GSM functions
    rc = CTOS_GSMOpen(9600, 1);
    if (rc != d_OK) {
        sprintf(sBuf, "%X ", rc);
        CTOS_LCDTPrintXY(10, 5, sBuf);
        CTOS_KBDGet(&key);
        return;
    }

    if (key == d_KBD_2) {
        CTOS_LCDTPrintXY(10, 5, "SIM2");
        usRtn = CTOS_GSMSelectSIM(d_GPRS_SIM2);
        if (usRtn != d_OK) {
            sprintf(str, "SelSIM: %X ", ret);
            CTOS_LCDTPrintXY(1, 9, str);
            CTOS_KBDGet(&key);
        }
    } else {
        CTOS_LCDTPrintXY(10, 5, "SIM1");
        usRtn = CTOS_GSMSelectSIM(d_GPRS_SIM1);
        if (usRtn != d_OK) {
            sprintf(str, "SelSIM: %X ", ret);
            CTOS_LCDTPrintXY(1, 9, str);
            CTOS_KBDGet(&key);
        }
    }
    CTOS_LCDTPrintXY(14, 5, "_ok     ");
    CTOS_Delay(1500);

    CTOS_GSMGetBAND(&bID);
    switch (bID) {
        case d_GSM_900_1800:
            CTOS_LCDTPrintXY(1, 5, "GSM_900_1800    ");
            break;
        case d_GSM_900_1900:
            CTOS_LCDTPrintXY(1, 5, "GSM_900_1900    ");
            break;
        case d_GSM_850_1800:
            CTOS_LCDTPrintXY(1, 5, "GSM_850_1800    ");
            break;
        case d_GSM_850_1900:
            CTOS_LCDTPrintXY(1, 5, "GSM_850_1900    ");
            break;
        default:
            CTOS_LCDTPrintXY(1, 5, "GSMGetBAND error");
            break;
    }

    // Check the GSM ready to use the SMS or Phone Book with the SIM card inserted //
    start = CTOS_TickGet();
    do {
        memset(str, 0x00, sizeof (str));

        CTOS_LCDTPrintXY(1, 6, "ChkRdy:    ");
        rc = CTOS_SIMCheckReady();
        sprintf(str, "%X", rc);
        CTOS_LCDTPrintXY(8, 6, str);

        CTOS_Delay(500);
        end = CTOS_TickGet();
        distance = end - start;
    } while ((rc != d_GSM_SIM_READY) && (distance < 1000)); //  wait for 10 seconds
    if (rc == d_GSM_SIM_READY) {
        CTOS_LCDTPrintXY(8, 6, "ok   ");
        CTOS_Delay(2000);
    } else {
        CTOS_LCDTPrintXY(8, 6, "fail ");
        return;
    }

    // Get the GPRS network registration status //
    memset(sBuf, 0x00, sizeof (sBuf));
    CTOS_LCDTPrintXY(1, 6, "RegState:       ");
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
        CTOS_LCDTPrintXY(10, 6, sBuf);

        CTOS_KBDHit(&key);
        if (key == d_KBD_CANCEL)
            break;

        CTOS_Delay(500);
        end = CTOS_TickGet();
        distance = end - start;
    } while (((State == d_GSM_GPRS_STATE_TRYING) || (State == d_GSM_GPRS_STATE_UNKNOW)) || (!(!(State != d_GSM_GPRS_STATE_REG) || !(State != d_GSM_GPRS_STATE_ROAM)) && (distance < 3000))); //  wait for 30 seconds

    // Get signal quality of the GSM network //
    do {
        rc = CTOS_GSMSignalQuality(&strength);
        CTOS_Delay(500);
        memset(sBuf, 0x00, sizeof (sBuf));
        sprintf(sBuf, "GSMSgnQty:%d ", strength);
        CTOS_LCDTPrintXY(1, 7, sBuf);

        CTOS_KBDHit(&key);
        if (key == d_KBD_CANCEL)
            break;
    } while (strength < 10 || strength > 30);

    // Get the SIM card IMSI //
    memset(imsi, 0, sizeof (imsi));
    rc = CTOS_SIMGetIMSI(imsi); // Fails here with code  0x4607
    if (rc == d_OK) {
        sprintf(sBuf, "IMSI: %s ", imsi);
        CTOS_LCDTPrintXY(1, 8, sBuf);
    } else {
        sprintf(sBuf, "SIMGetIMSI: %X ", rc);
        CTOS_LCDTPrintXY(1, 8, sBuf);
    }

    // Currently selected operator //
    memset(name, 0, sizeof (name));
    rc = CTOS_GSMQueryOperatorName(name, &nlen);
    if (rc == d_OK) {
        sprintf(sBuf, "Oprtr: %s", name);
        CTOS_LCDTPrintXY(1, 9, sBuf);
    } else {
        sprintf(sBuf, "GSMQON: %X ", rc);
        CTOS_LCDTPrintXY(1, 9, sBuf);
    }

    CTOS_Beep();
    CTOS_KBDGet(&key);
    if (key == d_KBD_CANCEL) {
        return;
    }


}

void GPRSOpen(void) {
    // Initialize the GPRS //
    CTOS_TCP_GPRSInit();

    // GPRSOpen //
    ClearScreen(4, 14);
    CTOS_LCDTPrintXY(1, 4, "OPENING...         ");
    strcpy(strAPN, "internet");
    strcpy(baID, "yahoo");
    strcpy(baPW, "yahoo");
    // use the default SIM1 slot for GPRSOpen if not select SIM slot after GSMOpen
    ret = CTOS_TCP_GPRSOpen(baIP_S, strAPN, baID, baPW);
    state = Check_state(ret);
    if (state != TRUE) {
        CTOS_LCDTPrintXY(1, 10, "Open fail    ");
        CTOS_KBDGet(&key);
        return;
    }
    CTOS_LCDTPrintXY(1, 8, "OPEN  [OK]       ");

    // Get the local IP //
    ret = CTOS_TCP_GPRSGetIP(baIP_G);
    sprintf(str, "IP Ret = %X   ", ret);
    CTOS_LCDTPrintXY(1, 7, str);
    CTOS_LCDTPrintXY(1, 8, "IP = ");
    for (i = 0; i < 4; i++) {
        sprintf(str, "%02X", baIP_G[i]);
        CTOS_LCDTPrintXY(6 + i * 2, 5, str);
    }
    CTOS_KBDGet(&key);
    return;
}

void GPRSConnect(void) {
    // GPRSConnect //
    ClearScreen(4, 14);
    CTOS_LCDTPrintXY(1, 5, "Connecting...      ");
    sprintf(str, "%d.%d.%d.%d\n%d", baIP_R[0], baIP_R[1], baIP_R[2], baIP_R[3], usPort);
    CTOS_LCDTPrintXY(1, 8, str);
    ret = CTOS_TCP_GPRSConnectEx(&bSocket, baIP_R, usPort);
    state = Check_state(ret);
    if (state != TRUE) {
        CTOS_LCDTPrintXY(1, 10, "Connect fail     ");
        CTOS_KBDGet(&key);
        return;
    }
    CTOS_LCDTPrintXY(1, 5, "Connect  [OK]       ");
    CTOS_KBDGet(&key);
    return;
}

void GPRSTxRx(void) {
    ClearScreen(4, 14);
    CTOS_LCDTPrintXY(1, 5, "Sending...    ");
    //ret = CTOS_TCP_GPRSTx(bSocket, "\x01\x02\x03\x04\x05\x06\x07\x08\x09\x0A", 0x0A);
    ret = CTOS_TCP_GPRSTx(bSocket, "\x31\x32\x33\x34\x35\x36\x37\x38\x39\x3A\x31\x32\x33\x34\x35\x36\x37\x38\x39\x3A\x31\x32\x33\x34\x35\x36\x37\x38\x39\x3A\x31\x32\x33\x34\x35\x36\x37\x38\x39\x3A\x31\x32\x33\x34\x35\x36\x37\x38\x39\x3A\x31\x32\x33\x34\x35\x36\x37\x38\x39\x3A\x31\x32\x33\x34\x35\x36\x37\x38\x39\x3A\x31\x32\x33\x34\x35\x36\x37\x38\x39\x3A\x31\x32\x33\x34\x35\x36\x37\x38\x39\x3A\x31\x32\x33\x34\x35\x36\x37\x38\x39\x3A", 0x64);
    state = Check_state(ret);
    if (state != TRUE) {
        CTOS_LCDTPrintXY(1, 9, "Tx fail     ");
        CTOS_KBDGet(&key);
        return;
    }

    TotalLen = 0;
    CTOS_LCDTPrintXY(1, 5, "Receiving...    ");
    do {
        usLen_Rx = GPRS_RX_BUF_SIZE;
        ret = CTOS_TCP_GPRSRx(bSocket, baBuffer_Rx, &usLen_Rx);
        state = Check_state(ret);
        if (state != TRUE) {
            usLen_Rx = 0;
            CTOS_LCDTPrintXY(1, 9, "Rx fail     ");
            CTOS_Beep();
            CTOS_KBDGet(&key);
            return;
        }

        TotalLen += usLen_Rx;

        memset(str, 0x00, sizeof (str));
        wub_hex_2_str(&baBuffer_Rx[0], str, usLen_Rx);
        CTOS_PrinterPutString(str);

        iY = 4;
        for (i = 0; i < usLen_Rx; i++) {
            memset(str, 0x00, sizeof (str));
            sprintf(str, "%02X", baBuffer_Rx[i]);
            CTOS_LCDTPrintXY((i % 8)*2 + 1, iY, str);
            if (i % 8 == 7)
                iY++;
        }
        memset(str, 0x00, sizeof (str));
        sprintf(str, "RxLen = %d         ", usLen_Rx);
        CTOS_LCDTPrintXY(1, 6, str);

        CTOS_KBDHit(&key);
        if (key == d_KBD_CANCEL)
            break;
    } while (TotalLen < 100);
}

void GPRSDisconnect(void) {
    ClearScreen(4, 14);
    CTOS_LCDTPrintXY(1, 5, "Disconnect...    ");
    ret = CTOS_TCP_GPRSDisconnect(bSocket);
    state = Check_state(ret);
    if (state != TRUE) {
        CTOS_LCDTPrintXY(1, 11, "Disconnect fail     ");
        CTOS_KBDGet(&key);
        return;
    }
}

void GPRSClose(void) {
    ClearScreen(4, 14);
    CTOS_LCDTPrintXY(1, 5, "CLOSING...    ");
    ret = CTOS_TCP_GPRSClose();
    state = Check_state(ret);
    if (state != TRUE) {
        CTOS_LCDTPrintXY(1, 11, "Close fail     ");
        CTOS_KBDGet(&key);
        return;
    }
    CTOS_KBDGet(&key);
    if (key == d_KBD_CANCEL) {
        return;
    }
}

void gsmmain(void) {
    USHORT usItem;

    memset(str, 0, sizeof (str));
    memset(baID, 0, sizeof (baID));
    memset(baPW, 0, sizeof (baPW));

    CTOS_LCDTSelectFontSize(0x101E);

    while (1) {
        CTOS_LCDTClearDisplay();
        ShowTitle(" GPRS SETTINGS        ");
        CTOS_LCDTPrintXY(3, 5, "1. Select SIM");
        CTOS_LCDTPrintXY(3, 6, "2. Internet ON");
        CTOS_LCDTPrintXY(3, 7, "3. Test Internet");
        CTOS_LCDTPrintXY(3, 8, "4. Test Conn");
        CTOS_LCDTPrintXY(3, 9, "5. Disconnect");
        CTOS_LCDTPrintXY(3, 10, "6. Internet OFF");

        CTOS_KBDGet(&key);
        CTOS_LCDTClearDisplay();

        start = CTOS_TickGet();
        //switch(usItem)
        switch (key) {
            case '1':
                GSMtest();
                break;
            case '2':
                GPRSOpen();
                break;
            case '3':
                GPRSConnect();
                break;
            case '4':
                GPRSTxRx();
                break;
            case '5':
                GPRSDisconnect();
                break;
            case '6':
                GPRSClose();
                break;
            case d_KBD_CANCEL:
                return;
                break;

        }

    }

}
