#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
/** These two files are necessary for calling CTOS API **/
#include <ctosapi.h>
#include "moremenu.h"
#include "Transaction.h"
#include "emv_cl.h"
#include "withdraw.h"
#include "deposit.h"
#include "balance.h"
#include "paywith.h"
#include "appmain.h"


BYTE key;

void mainmenu(void) {
    
        while (1) {

            ClearScreen(4, 14);
            ShowTitle("   MAIN MENU           ");
            CTOS_LCDTPrintXY(1, 15, "              X-Exit");
            
            CTOS_LCDTPrintXY(3, 4, "1. WITHDRAWAL");
            CTOS_LCDTPrintXY(3, 5, "2. DEPOSIT");
            CTOS_LCDTPrintXY(3, 6, "3. BALLANCE");
            CTOS_LCDTPrintXY(3, 7, "4. SEND MONEY");
            CTOS_LCDTPrintXY(3, 8, "5. BY GOODS");
            CTOS_LCDTPrintXY(3, 9, "6. PAY FEE");
            CTOS_LCDTPrintXY(3, 10, "7. PAY LOAN");
            CTOS_LCDTPrintXY(3, 11, "8. LOAN STATUS");
            CTOS_LCDTPrintXY(3, 12, "9. STATEMENT");
            CTOS_LCDTPrintXY(3, 13, "0. MORE");
            

            CTOS_KBDGet(&key);

            switch (key) {
                case '1':
                    withdraw();
                    break;

                case '2':
                    deposit();
                    break;

                case '3':
                    balance();
                    break;
                case '4':
                    break;

                case '5':
                    
                    break;

                case '6':
                   
                    break;

                case '7':

                    break;
                case '8':

                    break;
                case '9':

                    break;
                case '0':
                    moremenu();
                    break;

                case d_KBD_CANCEL:
                    return;
                    break;


            }


        }


    }
