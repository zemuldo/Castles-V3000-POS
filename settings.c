/** 
 **	A Template for developing new terminal application
 **/

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <ctosapi.h>
#include "gsm.h"
#include "systemops.h"
#include "Transaction.h"
#include "appmain.h"


void settings(void) {
    while (1) {
    BYTE key;
        ClearScreen(4, 26);
        ShowTitle("  DEVICE SETTINGS              ");
            CTOS_LCDTPrintXY(3, 5, "1. GPRS SIM"); 
            CTOS_LCDTPrintXY(3, 7, "2. SYSTEM ");
            CTOS_LCDTPrintXY(1, 13, "             X. Exit");
            CTOS_KBDGet(&key);
            switch (key) {
                case '1':
                    ClearScreen(4, 26);
                    gsmmain();
                    break;
               
                case '2':
                    ClearScreen(4, 26);
                    systemopsmain();
                    break;

                case d_KBD_CANCEL:
                    return;
                    break;


            }


        }


}
void presettings(void) {
    while (1) {
    BYTE key;
        ClearScreen(4, 26);
        ShowTitle("  DEVICE SETTINGS              ");
            CTOS_LCDTPrintXY(3, 5, "1. GPRS SIM"); 
            CTOS_LCDTPrintXY(3, 7, "2. SYSTEM ");
            CTOS_LCDTPrintXY(1, 13, "             X. Exit");
            CTOS_KBDGet(&key);
            switch (key) {
                case '1':
                    ClearScreen(4, 26);
                    gsmmain();
                    break;
               
                case '2':
                    ClearScreen(4, 26);
                    systemopsmain();
                    break;

                case d_KBD_CANCEL:
                    select_id();
                    break;


            }


        }


}



