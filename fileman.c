/**
 **    A Template for developing new terminal application
 **/

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
/** These two files are necessary for calling CTOS API **/
#include <ctosapi.h>

/**
 ** The main entry of the terminal application
 **/

#define d_BUFF_SIZE 128 //Buffer Size
#define d_LINE_DOT  12      //A line is 12 dots in the paper
BYTE key;
USHORT ret;
USHORT usTotal;
BYTE strintowrite[255];
BYTE babuff[d_BUFF_SIZE];
BYTE baFileBuff[d_BUFF_SIZE];
USHORT usFileCount = 0;
ULONG ulReadLen;
ULONG ulFreeSize;
BYTE baTemp[d_BUFF_SIZE];
int i;




/* Define Error Code*/
#define d_FILE_EXIST 0x01
#define d_FILE_NO_FILES 0x02
#define d_FILE_DIR_OK 0x03
#define d_FILE_CANCEL 0x04
#define d_FILE_LENGTH_SHORT 0x05

BYTE stringtowrite[255];

void filemanager(void) {

    BYTE key;
    STR * keyboardLayoutEnglish[] = {" 0", "qzQZ1", "abcABC2", "defDEF3", "ghiGHI4",
        "jklJKL5", "mnoMNO6", "prsPRS7", "tuvTUV8", "wxyWXY9", ":;/\\|?,.<>", ".!@#$%^&*()"};
    //numeric keyboard (0123456789) and radix point '.'
    STR * keyboardLayoutNumberWithRadixPoint[] = {"0", "1", "2", "3", "4", "5", "6", "7",
        "8", "9", "", "."};
    //numeric keyboard (0123456789) without radix point
    STR * keyboardLayoutNumber[] = {"0", "1", "2", "3", "4", "5", "6", "7", "8", "9", "",
        ""};
    // TODO: Add your program here //
    ShowTitle("    Manage Logs        ");
    ClearScreen(4, 26);
    while (1) {
        ShowTitle("    Manage Logs        ");
        ClearScreen(4, 26);
        CTOS_LCDTPrintXY(1, 5, "Manage Logs");
        CTOS_LCDTPrintXY(1, 6, "1. Creat New");
        CTOS_LCDTPrintXY(1, 7, "2. Delete");
        CTOS_LCDTPrintXY(1, 8, "3. Directory");
        CTOS_LCDTPrintXY(1, 9, "4. Read Logs");
        CTOS_LCDTPrintXY(1, 10, "5. Test Write");
        CTOS_LCDTPrintXY(1, 11, "6. Test Log");
        CTOS_KBDGet(&key);
        switch (key) {
            case d_KBD_1:
                //create
                ClearScreen(4, 26);
                sprintf(stringtowrite, "FL%02d.txt", usFileCount);
                CTOS_LCDTPrintXY(5, 7, babuff);
                DirectoryFile(&usTotal, baFileBuff);
                usFileCount = (usTotal != 7 ? usTotal + 1 : 1);
                CTOS_LCDTPrintXY(1, 7, "New:");
                sprintf(babuff, "FL%02d.txt", usFileCount);
                CTOS_LCDTPrintXY(5, 7, babuff);
                ret = NewFile(babuff, 0);
                if (ret == d_OK) {
                    ClearScreen(4, 26);
                    CTOS_LCDTPrintXY(1, 8, "Create File Success");
                } else
                    show_errormsg(ret);
                /*sprintf(baTemp,"ret:%X           ", ret);
                CTOS_LCDTPrintXY(1 ,8 ,baTemp);*/
                CTOS_KBDGet(&key);
                break;

            case d_KBD_2:
                ClearScreen(4, 26);
                ret = DeleteFile();
                if (ret == d_OK) {
                    ClearScreen(4, 26);
                    CTOS_LCDTPrintXY(1, 8, "Delete File Success");
                } else if (ret != d_USER_CANCEL)
                    show_errormsg(ret);

                if (ret != d_USER_CANCEL)
                    CTOS_KBDGet(&key);
                break;
            case d_KBD_3:
                ClearScreen(4, 26);
                ret = FileGetAttrib();
                if (ret != d_OK)
                    show_errormsg(ret);
                if (ret != d_FILE_CANCEL)
                    CTOS_KBDGet(&key);
                break;

            case d_KBD_4:
                //readfile
                ClearScreen(4, 26);
                ulReadLen = sizeof (babuff); //Set the maximum reading size
                memset(babuff, 0, sizeof (babuff));
                ret = FileRead(babuff, &ulReadLen);
                if (ret == d_OK) {
                    sprintf(baTemp, "Read OK (%lu)", ulReadLen);
                    CTOS_LCDTPrintXY(1, 8, baTemp);
                    if (ulReadLen > 0) {
                        CTOS_KBDGet(&key);
                        ClearScreen(4, 26);
                        CTOS_LCDTPrint("The Content: \n");
                        for (i = 0; i < 112; i += 16) {
                            if (i > ulReadLen)
                                break;
                            strncpy(baTemp, &babuff[i], 16);
                            baTemp[16] = '\n';
                            if (i >= (112 - 16))
                                baTemp[16] = '\0';
                            CTOS_LCDTPrint(baTemp);
                        }
                    }
                } else if (ret != d_USER_CANCEL) {
                    show_errormsg(ret);
                    CTOS_KBDGet(&key);
                }

                if (ret != d_USER_CANCEL)
                    CTOS_KBDGet(&key);
                break;
            case d_KBD_5:
                //test write
                ClearScreen(4, 26);
                CTOS_LCDTPrintXY(4, 4, "Enter Any Text:");

                CTOS_UIKeypad(4, 5, keyboardLayoutEnglish, 40, 80, d_FALSE, d_FALSE, 0, 0, strintowrite, 255);
                ClearScreen(4, 26);
                ret = FileWrite(strintowrite);
                if (ret == d_OK) {
                    ClearScreen(4, 26);
                    CTOS_LCDTPrintXY(1, 8, "Write Success   ");
                } else if (ret != d_USER_CANCEL)
                    show_errormsg(ret);

                if (ret != d_USER_CANCEL)
                    CTOS_KBDGet(&key);
                break;
            case d_KBD_6:
                //testlog
                ClearScreen(4, 26);
                CTOS_LCDTPrintXY(4, 4, "Enter Any Text:");

                CTOS_UIKeypad(4, 5, keyboardLayoutEnglish, 40, 80, d_FALSE, d_FALSE, 0, 0, strintowrite, 255);
                ClearScreen(4, 26);
                ret = autoFileWrite(strintowrite);
                if (ret == d_OK) {
                    ClearScreen(4, 26);
                    CTOS_LCDTPrintXY(1, 8, "Write Success   ");
                } else if (ret != d_USER_CANCEL)
                    show_errormsg(ret);

                if (ret != d_USER_CANCEL)
                    CTOS_KBDGet(&key);
                break;
            case d_KBD_CANCEL:
                return;
                break;


        }


    }
}


