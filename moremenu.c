
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
/** These two files are necessary for calling CTOS API **/
#include <ctosapi.h>

#include "gsm.h"
#include "Transaction.h"

void moremenu(void) {

    BYTE key;
    while (1) {
        ClearScreen(4, 14);
        ShowTitle("   MAIN MENU  ");
        CTOS_LCDTPrintXY(1, 15, "       X. BACK");
        CTOS_LCDTPrintXY(3, 4, "1. E-Bank");
        CTOS_LCDTPrintXY(3, 5, "2. M-Bank");
      


        CTOS_KBDGet(&key);

        switch (key) {
            case '1':
                return;
                break;

            case '2':
                return;
                break;
            
            case d_KBD_CANCEL:
                return;
                break;


        }


    }


}
