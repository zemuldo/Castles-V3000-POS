#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
/** These two files are necessary for calling CTOS API **/
#include <ctosapi.h>
#include "settings.h"

#define d_BUFF_SIZE 32	      //Buffer Size

//Declare Global Variable //
BYTE key;

void sysshow_screen(int tag) {
    CTOS_LCDTClearDisplay();
    CTOS_LCDTSelectFontSize(0x101E);

    switch (tag) {
        case 0:
            CTOS_LCDTPrint("   SYSTEM MODE   ");
            CTOS_LCDTPrintXY(1, 2, "1.System Reset");
            CTOS_LCDTPrintXY(1, 3, "2.Download Mode");
            CTOS_LCDTPrintXY(1, 4, "3.System Wait");
            CTOS_LCDTPrintXY(1, 5, "4.PowerOFF");
            CTOS_LCDTPrintXY(1, 6, "5.LCDSetContrast");
            CTOS_LCDTPrintXY(1, 7, "6.Exit Application");
            break;
        case 1:
            CTOS_LCDTPrint("\fr   SYSTEM WAIT   \fn");
            CTOS_LCDTPrintXY(1, 3, "Plz Press Key");
            break;
        case 2:
            CTOS_LCDTPrint("\fr LCD CONTRAST \fn");
            CTOS_LCDTPrintXY(1, 3, "AAAAAAAAAAAAA");
            CTOS_LCDTPrintXY(1, 4, "bbbbbbbbbbb");
            CTOS_LCDTPrintXY(1, 5, "EEEEEEEEEEEE");
            CTOS_LCDTPrintXY(1, 8, "Down->'+'Up->'-'");
            break;
    }
}

void SystemWait(void) {
    //Declare Local Variable //
    DWORD dwWait = 0, dwWakeup = 0;
    USHORT ret;
    BYTE babuff[d_BUFF_SIZE];
    BYTE sT1Buf[128], sT2Buf[128], sT3Buf[128];
    USHORT bT1Len = 128, bT2Len = 128, bT3Len = 128;

    int i1 = 0, i2 = 0;
    BYTE baTemp[d_BUFF_SIZE];

    while (1) {
        CTOS_LCDTClearDisplay();
        sprintf(baTemp, "clear:%d", i1++);
        CTOS_LCDTPrintXY(1, 5, baTemp);
        //show_screen(1);
        CTOS_LCDTPrintXY(1, 1, "KBD_MSR_SC_Test");
        CTOS_LCDTPrintXY(1, 8, "Press X to exit");

        //dwWait = d_EVENT_KBD;
        dwWait = d_EVENT_KBD | d_EVENT_MSR | d_EVENT_SC;
        //dwWait = d_EVENT_MSR;
        //System waits the device the be set acts.   //System waits for target devices to be triggered and will not return until any target device is triggered //

        ret = CTOS_SystemWait(50000, dwWait, &dwWakeup);
        if ((dwWakeup & d_EVENT_KBD) == d_EVENT_KBD) {
            //CTOS_LCDTPrintXY(1, 8, "Wake up");
            //Detect if any key is pressed and get which key, but not get actual key.
            //If you get actual key, use to "CTOS_KBDGet" function//
            //CTOS_KBDInKeyCheck(&key);
            CTOS_KBDGet(&key);
            sprintf(babuff, "key = %c", key);
            CTOS_LCDTPrintXY(1, 7, babuff);

            sprintf(baTemp, "key:%d", i2++);
            CTOS_LCDTPrintXY(1, 6, baTemp);

            CTOS_Delay(2000);

            if (key == d_KBD_CANCEL)
                return;
        } else if ((dwWakeup & d_EVENT_MSR) == d_EVENT_MSR) {
            CTOS_LCDTPrintXY(1, 7, "MSR");
            CTOS_MSRRead(sT1Buf, &bT1Len, sT2Buf, &bT2Len, sT3Buf, &bT3Len);
            CTOS_LCDTPrintXY(1, 8, "Press any key  ");
            CTOS_KBDGet(&key);
        } else if ((dwWakeup & d_EVENT_SC) == d_EVENT_SC) {
            CTOS_LCDTPrintXY(1, 7, "Smart Card");
            CTOS_LCDTPrintXY(1, 8, "Press any key  ");
            CTOS_KBDGet(&key);
        }

        if (ret != d_OK) {
            sprintf(babuff, "ret = 0x%04X", ret);
            CTOS_LCDTPrintXY(1, 8, babuff);
        }
    }
}

void sysSetLCDContrast(void) {
    //Declare Local Variable //
    BYTE bValue = 0x98;
    BYTE baTemp[20];

    sysshow_screen(2);
    CTOS_LCDTPrintXY(1, 6, "Default:0x98  ");

    while (1) {
        CTOS_KBDHit(&key);

        if (key == d_KBD_DOWN) {
            bValue++;
        }

        if (key == d_KBD_UP) {
            bValue--;
        }

        if (key == d_KBD_CANCEL) {
            sysshow_screen(0);
            return;
        } else {
            //To setup the contrast of the LCD //
            CTOS_LCDSetContrast(bValue);
            sprintf(baTemp, "Now:0x%02x  ", bValue);
            CTOS_LCDTPrintXY(1, 7, baTemp);
        }
    }
}

void systemopsmain(void) {
    sysshow_screen(0);

    while (1) {
        CTOS_KBDGet(&key);

        switch (key) {
            case d_KBD_1:
                //Calling this function will reset all the peripherals, all of the opend files will be closed, 
                //and all the connection session with external device will be close, too. //
                CTOS_SystemReset();
                break;
            case d_KBD_2:
                //To enter the PM(Program Manager) "Download AP" mode //
                CTOS_EnterDownloadMode();
                sysshow_screen(0);
                break;
            case d_KBD_3:
                SystemWait();
                sysshow_screen(0);
                break;
            case d_KBD_4:
                //To power off the system //
                CTOS_PowerOff();
                break;
            case d_KBD_5:
                sysSetLCDContrast();
                break;
            case d_KBD_6:
                exit(0);
                break;
            case d_KBD_CANCEL:
                settings();
        }
    }

    return;
}
