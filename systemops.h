/* 
 * File:   systemops.h
 * Author: Impala
 *
 * Created on November 2, 2016, 2:47 PM
 */
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
/** These two files are necessary for calling CTOS API **/
#include <ctosapi.h>

//#define d_BUFF_SIZE 32	      //Buffer Size

//Declare Global Variable //
BYTE key;

#ifndef SYSTEMOPS_H
#define	SYSTEMOPS_H

#ifdef	__cplusplus
extern "C" {
#endif

    void sysshow_screen(int tag);


    void sysSetLCDContrast(void);

    void SystemWait(void);

    void sysSetLCDContrast(void);

    void systemopsmain(void);



#ifdef	__cplusplus
}
#endif

#endif	/* SYSTEMOPS_H */

