
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
        ClearScreen(4, 26);
        ShowTitle("   MAIN MENU  ");
        
        CTOS_LCDTPrintXY(2, 6, "2.User Login");
        CTOS_LCDTPrintXY(2, 7, "3.Reports");
        CTOS_LCDTPrintXY(2, 8, "4.Batch Uploads");
        CTOS_LCDTPrintXY(2, 16, " X. BACK");



        CTOS_KBDGet(&key);

        switch (key) {
            case '1':
                dorutilitypay();
                break;

            case '2':
                return;
                break;
            case '3':
                return;
                break;

            case '4':
                return;
                break;

            case d_KBD_CANCEL:
                return;
                break;


        }


    }


}
