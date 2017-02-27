/**
 ** A Template for developing new terminal application
 **/
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
/** These two files are necessary for calling CTOS API **/
#include <ctosapi.h>
#define PARENT_MSG_LINE1 1
#define PARENT_MSG_LINE2 2
#define PARENT_MSG_LINE3 3
#define PARENT_MSG_LINE4 4
#define PARENT_MSG_LINE5 5
#define PARENT_MSG_LINE6 6
#define PARENT_MSG_LINE7 7
#define PARENT_MSG_LINE8 8
//The function that will execute when sleep or standby

BOOL sleep_test(void *arg, BYTE mach_state) {
    BYTE *buf;
    BYTE Display[20];
    ClearScreen(4, 26);
    buf = (BYTE *) arg;
    sprintf(Display, "Sendstring = %s", buf);
    CTOS_LCDTPrintXY(1, PARENT_MSG_LINE1, Display);
    CTOS_Delay(2000);
    if (mach_state == d_PWR_SLEEP_MODE) {
        CTOS_LCDTPrintXY(1, PARENT_MSG_LINE2, "Doing sleep .... ");
        CTOS_LCDTPrintXY(1, PARENT_MSG_LINE3, "Sleep........ ");
        CTOS_Delay(2000);
        CTOS_LCDTPrintXY(1, PARENT_MSG_LINE3, "Sleep Done .... ");
        CTOS_Delay(1000);
        ClearScreen(4, 26);
    } else if (mach_state == d_PWR_STANDBY_MODE) {
        CTOS_LCDTPrintXY(1, PARENT_MSG_LINE2, "Doing standby .... ");
        CTOS_LCDTPrintXY(1, PARENT_MSG_LINE3, "Standby........ ");
        CTOS_Delay(2000);
        CTOS_LCDTPrintXY(1, PARENT_MSG_LINE3, "Standby Done .... ");
        CTOS_Delay(1000);
        ClearScreen(4, 26);
    }
    return d_OK;
}
//The function that will execute when resume from sleep or standby

BOOL resume_test(void *arg, BYTE last_state, USHORT *wakesrc) {
    const char *buf;
    BYTE buff[32];
    buf = arg;
    printf("%s: last_state:%d, wakesrc:0x%x", buf, last_state, *wakesrc);
    ClearScreen(4, 26);
   
    if (last_state == d_PWR_STANDBY_MODE) {
        sprintf(buff, "Wakeup ..... 0x%x ", *wakesrc);
        CTOS_LCDTPrintXY(1, PARENT_MSG_LINE4, buff);
        CTOS_Delay(2000);
        CTOS_LCDTPrintXY(1, PARENT_MSG_LINE5, "Wakeup Done .... ");
        CTOS_LCDTPrintXY(1, PARENT_MSG_LINE7, "Press any key to test");
        CTOS_LCDTPrintXY(1, PARENT_MSG_LINE8, "again or 'X' to leave");
        CTOS_Delay(1000);
    } else if (last_state == d_PWR_SLEEP_MODE) {
        sprintf(buff, "Resume ..... 0x%02X ", *wakesrc);
        CTOS_LCDTPrintXY(1, PARENT_MSG_LINE4, buff);
        CTOS_Delay(2000);
        CTOS_LCDTPrintXY(1, PARENT_MSG_LINE5, "Resume Done .... ");
        CTOS_LCDTPrintXY(1, PARENT_MSG_LINE7, "Press any to test...");
        CTOS_LCDTPrintXY(1, PARENT_MSG_LINE8, "again, 'X' to leave");
        CTOS_Delay(1000);
    }
    return d_OK;
}
//If you do not need to excute extra movment before or after the terminal sleep.
//Please mark or delete line 101 to 106 and 109.

void sleepmode(void) {
    BYTE *Sendstring = "TEST";
    ULONG ulSec = 12;
    BYTE key;
    CTOS_LCDSelectMode(d_LCD_GRAPHIC_320x240_MODE);
    CTOS_PowerAutoModeEnable(); //Open Auto Power Managment
    //---- [Start]initial sleep function.
    CTOS_stPowerModeCallback sleep_ops;
    memset(&sleep_ops, 0, sizeof (CTOS_stPowerModeCallback));
    sleep_ops.OnEnterPowerMode = sleep_test; //Execte when Enter PowerMode(Sleep / Standby)
    sleep_ops.pEnterPowerModeData = (void *) Sendstring; //Pass data into function"resume_test"sleep_ops.OnExitPowerMode = resume_test; //Execte when ExitPowerMode(Sleep / Standby)
    CTOS_PowerAutoModeRegisterCallback(&sleep_ops); //RegisterCallBack fuction of Auto Power Managment
    //---- [End]initial sleep function.
    //---- Set the timeout of Standby Mode or Sleep Mode
    CTOS_PowerAutoModeTimeToSleep(0); // prevent the system get into Sleep Mode
    CTOS_PowerAutoModeTimeToStandby(ulSec); //If you did not enter any key/InsetSmartCard / Swipe MSR, the AP will get into Stanby Mode
    //CTOS_PowerAutoModeTimeToStandby(0); // prevent the system get into Standby Mode
    //CTOS_PowerAutoModeTimeToSleep(ulSec); //If you did not enter any key/Inset SmartCard / Swipe MSR, the AP will get into Sleep Mode
    do {
        ClearScreen(4, 26);
        CTOS_LCDTPrintXY(1, PARENT_MSG_LINE1, "Wiat 12s to Standby.");
        CTOS_KBDGet(&key);
    } while (key != d_KBD_CANCEL);
    CTOS_PowerAutoModeUNRegisterCallback(); //Unegist CallBack fuction of Auto PowerManagment
    CTOS_PowerAutoModeDisable(); //Close Auto Power ManagmentCastles Technology Co., Ltd. Confidential • All Right Reserved. Pg. 780
    exit(0);
}
