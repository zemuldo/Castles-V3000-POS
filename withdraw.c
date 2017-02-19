/** 
**	A Template for developing new terminal application
**/

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
/** These two files are necessary for calling CTOS API **/
#include <ctosapi.h>
#include "Transaction.h"
#include "emv_cl.h"
#include "mainmenu.h"
#include "Transaction.h"
#include "appmain.h"
#include "cardlessdeposit.h"


/** 
** The main entry of the terminal application 
**/
void withdraw(void)
{
  
  Do_Transaction();
  return;
    
}

