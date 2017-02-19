/* 
 * File:   gsm.h
 * Author: Impala
 *
 * Created on October 29, 2016, 5:52 AM
 */

#ifndef GSM_H
#define	GSM_H

#ifdef	__cplusplus
extern "C" {
#endif

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <ctosapi.h>
#include "wub_lib.h"

#define BUF_SIZE (1024)	
#define GPRS_RX_BUF_SIZE (1500) // must bigger than TCP windows size 1460
#define d_BUFF_SIZE 128			    // Buff Size

    BYTE key, state, bSocket, bNo;
    int i, iY;
    BYTE baIP_G[4];
    STR strAPN[9];
    STR baID[6];
    STR baPW[6];
    //BYTE baIP_R [] = {0xDA, 0xD3, 0x23, 0xDB}; 
    //USHORT usPort = 8019;
    //BYTE baIP_R [] = {218,211,35,220};
    BYTE baBuffer_Rx[GPRS_RX_BUF_SIZE];
    USHORT ret;
    BYTE str[20];
    USHORT usRtn;
    DWORD start, end, distance;
    ULONG ulTime;
    BYTE babuff[d_BUFF_SIZE];

    /*===================================================================
     * FUNCTION NAME:   Check_state
     * DESCRIPTION:   Check the status and the return value
     * RETURN:        TRUE and FALSE
     * NOTES:         none
     *==================================================================*/
    BOOL Check_state(USHORT RET);

    void GSMtest(void);

    void GPRSOpen(void);

    void GPRSConnect(void);

    void GPRSTxRx(void);

    void GPRSDisconnect(void);

    void GPRSPing(void);

    void GPRSClose(void);

    void gsmmain(void);



#ifdef	__cplusplus
}
#endif

#endif	/* GSM_H */

