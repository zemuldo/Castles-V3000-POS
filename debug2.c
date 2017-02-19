#include <string.h>
#include <stdio.h>
#include <ctosapi.h>
#include "debug2.h"
#include "wub_lib.h"


#if _DEBUG_LOG_


#define d_READY_TIMEOUT		100

BYTE DebugLog[4096 + 2];
LONG DebugLen;
CTOS_RTC stRTC;
static BYTE DebugPort = d_DEFAULT_DEBUG_COM;
static BYTE isDebugEnable = FALSE;

#define d_HELLOE_MESSAGE	"Hello (AP)"
#define d_SUB_TITLE			"AP:"

//------------------------------------------------------------------------------
void DebugInit(void)
{
	DebugEnable(d_DEFAULT_DEBUG_COM);
}
//------------------------------------------------------------------------------
void DebugEnable(BYTE bPort)
{
	USHORT rtn;
	
	DebugLen = 0;
	DebugPort = bPort;
	
	if(DebugPort == 0xFF)
		rtn = CTOS_USBOpen();
	else
		rtn = CTOS_RS232Open(DebugPort, 115200, 'N', 8, 1);
	
	if(rtn == 0)
	{
		isDebugEnable = TRUE;
		DebugAddSTR((BYTE*)d_HELLOE_MESSAGE);
	}
	else
	{
		isDebugEnable = FALSE;
	}
}
//------------------------------------------------------------------------------
void DebugDisable(void)
{
	if(DebugPort == 0xFF)
	{
		CTOS_USBClose();
	}
	else
	{
		CTOS_RS232Close(DebugPort);
	}
	
	isDebugEnable = FALSE;
}
//------------------------------------------------------------------------------
void DebugExport232(void)
{
	ULONG tick;
	USHORT ret;
	
	if(isDebugEnable == FALSE)
	{
		return;
	}
	
	CTOS_Delay(70);
	tick = CTOS_TickGet();
	do
	{
		if(DebugPort == 0xFF)
			ret = CTOS_USBTxReady();
		else
			ret = CTOS_RS232TxReady(DebugPort);
		
		if(ret == d_OK)
		{
			break;
		}
		//CTOS_Delay(50);
	}while ((CTOS_TickGet() - tick) < d_READY_TIMEOUT);
	
	if(ret == d_OK)
	{
		DebugLog[DebugLen++] = 0x0D;
		DebugLog[DebugLen++] = 0x0A;
		if (DebugPort == 0xFF)
			CTOS_USBTxData(DebugLog, DebugLen);
		else
			CTOS_RS232TxData(DebugPort, DebugLog, DebugLen);
		tick = CTOS_TickGet();
		do
		{
			if(DebugPort == 0xFF)
				ret = CTOS_USBTxReady();
			else
				ret = CTOS_RS232TxReady(DebugPort);
			
			if(ret == d_OK)
			{
				break;
			}
			//CTOS_Delay(50);
		}while ((CTOS_TickGet() - tick) < d_READY_TIMEOUT);
	}
	
	DebugLen = 0;
}
//------------------------------------------------------------------------------
void DebugAddHEX(BYTE *title, BYTE *hex, USHORT len)
{
	if(isDebugEnable == FALSE)
	{
		return;
	}
	
	if(len > (sizeof (DebugLog) / 2) - 8)
	{
		len = (sizeof (DebugLog) / 2) - 8;
	}
	
	CTOS_RTCGet(&stRTC);
	
	wub_memset(DebugLog, 0x00, sizeof(DebugLog));
	sprintf(DebugLog, "<%02d:%02d:%02d> ", stRTC.bHour, stRTC.bMinute, stRTC.bSecond);
	DebugLen = wub_strlen(DebugLog);
	DebugLog[DebugLen++] = '[';

	DebugLog[DebugLen] = 0x00;
	wub_strcat(&DebugLog[DebugLen], d_SUB_TITLE);
	DebugLen += wub_strlen(d_SUB_TITLE);
	
	DebugLog[DebugLen] = 0x00;
	wub_strcat(&DebugLog[DebugLen], title);
	DebugLen += wub_strlen(title);
	DebugLog[DebugLen++] = ']';
	DebugLog[DebugLen++] = ' ';
	DebugLen += wub_hex_2_str(hex, &DebugLog[DebugLen], len);
	
	DebugExport232();
}
//------------------------------------------------------------------------------
void DebugAddINT(BYTE *title, LONG value)
{
	BYTE temp[50];
	
	if(isDebugEnable == FALSE)
	{
		return;
	}
	
	CTOS_RTCGet(&stRTC);
	
	wub_memset(DebugLog, 0x00, sizeof(DebugLog));
	sprintf(DebugLog, "<%02d:%02d:%02d> ", stRTC.bHour, stRTC.bMinute, stRTC.bSecond);
	wub_strcat(DebugLog, "[");
	wub_strcat(DebugLog, d_SUB_TITLE);
	wub_strcat(DebugLog, title);
	wub_strcat(DebugLog, "] ");
	wub_memset(temp, 0x00, sizeof (temp));
	sprintf(temp, "%ld (0x%lX)", value, value);
	wub_strcat(DebugLog, temp);
	
	DebugLen = wub_strlen(DebugLog);
	DebugExport232();
}
//------------------------------------------------------------------------------
void DebugAddSTR(BYTE *title)
{
	if(isDebugEnable == FALSE)
	{
		return;
	}
	
	CTOS_RTCGet(&stRTC);
	
	wub_memset(DebugLog, 0x00, sizeof(DebugLog));
	sprintf(DebugLog, "<%02d:%02d:%02d> ", stRTC.bHour, stRTC.bMinute, stRTC.bSecond);
	wub_strcat(DebugLog, d_SUB_TITLE);
	wub_strcat(DebugLog, title);
	DebugLen = wub_strlen(DebugLog);
	DebugExport232();

}
#endif






