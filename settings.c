/** 
 **	A Template for developing new terminal application
 **/

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
/** These two files are necessary for calling CTOS API **/
#include <ctosapi.h>
#include "gsm.h"
#include "systemops.h"
#include "Transaction.h"
#include "appmain.h"
//
void settings(void) {
    BYTE key;

    
    {

        BYTE key;
        CTOS_LCDTClearDisplay();
        ShowTitle("  DEVICE SETTINGS              ");
        while (1) {
            
            CTOS_LCDTPrintXY(3, 5, "1. GPRS SIM"); //when clicked it a withdraw
            CTOS_LCDTPrintXY(3, 7, "2. SYSTEM "); //when clicked it a deposit
            CTOS_LCDTPrintXY(1, 13, "             X. Exit");

            CTOS_KBDGet(&key); //wait until a key is pressed

            switch (key) {
                case '1':
                    CTOS_LCDTClearDisplay();


                    gsmmain();
                    break;
               
                case '2':
                    CTOS_LCDTClearDisplay();
                    systemopsmain();
                    break;

                case d_KBD_CANCEL:
                    loginwithpin();
                    break;


            }


        }



    }


}




