#ifndef DEBUG_H
#define	DEBUG_H

#ifdef	__cplusplus
extern "C" {
#endif


	//Debug Setting------------------------------------------------------------------------------------
	#define _DEBUG_LOG_					1
	#define d_DEFAULT_DEBUG_COM			d_DEBUG_PORT_USB


	#define d_DEBUG_PORT_USB		0xFF
	#define d_DEBUG_PORT_COM1		d_COM1
	#define d_DEBUG_PORT_COM2		d_COM2
	#define d_DEBUG_PORT_COM3		d_COM3

	void DebugInit(void);
	void DebugEnable(BYTE bPort);
	void DebugDisable(void);
	void DebugAddHEX(BYTE *title, BYTE *hex, USHORT len);
	void DebugAddINT(BYTE *title, LONG value);
	void DebugAddSTR(BYTE *title);

#ifdef	__cplusplus
}
#endif

#endif	/* DEBUG_H */

