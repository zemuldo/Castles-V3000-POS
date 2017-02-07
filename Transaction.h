#ifndef _TRANSACTION_H
#define	_TRANSACTION_H

#ifdef	__cplusplus
extern "C" {
#endif

    //Message
#define d_MSG_PRESENT_CARD						" Present Smart Card     "
#define d_MSG_READ_CARD_OK						"READ CARD OK   "	
#define d_MSG_REMOVE_CARD						"PLEASE REMOVE CARD    "
#define d_MSG_APPROVED							"     APPROVED     "
#define d_MSG_DECLINE							"   NOT AUTHORISED "
#define d_MSG_ONLINE							"  Authenticating...  "
#define d_MSG_REFUND							"  REFUND PROCESSED "
#define d_MSG_HOST_BUSY							"   !!Host busy!!   "

#define d_MSG_SEE_PHONE							"  !SEE PHONE!      "
#define d_MSG_READ_CARD_ERR						"  !READ CARD ERR!  "
#define d_MSG_TRY_OTHER_INTERFACE				"TRY OTHER INTERFACE"
#define d_MSG_INSERT_OR_SWIPE_CARD				"INSERT or SWIPE CARD"
#define d_MSG_NO_CARD							"NO CARD DETECTED           "
#define d_MSG_MORE_CARD							"MORE CARD         "
#define d_MSG_TRY_AGAIN							"TRY AGAIN         "
#define d_MSG_UNSUPPORT_CARD					"!UN-SUPPORT CARD! "
#define d_MSG_INSERT_OR_SWIPE					"INSERT or SWIPE or"
#define d_MSG_USE_OTHER_CARD					"USE OTHER CARD    "

    char *amount;
    //char *cardnumber;
    char *cardpin;


    void PrintBlank(void);
    void StartTrans(BYTE bType);
    void ClearScreen(BYTE bStartY, BYTE bEndY);
    void ShowTitle(char *caStr);
    void ShowRtn(BYTE y, BYTE *baMessage, ULONG ulRtn);
    void Do_Transaction(void);
    ULONG InputValue(void);
    void Transaction(void);
    ULONG ASCII2Int(BYTE *baSBuf, int iL);
    void TLVDataClear(void);
    void TLVDataParse(BYTE *buf, USHORT len);
    char Get_PIN_Input(BYTE x, BYTE y, BYTE min, BYTE max, char mask, char *buf, BYTE *len);
    BYTE SignatureProcessing(void);
    USHORT Online_Process(BYTE *baData, USHORT usDataLen);
    //void Print_Receipt(EMVCL_RC_DATA_EX *data, BYTE isNeedSignature, ULONG ulValue);
#ifdef	__cplusplus
}
#endif

#endif	/* _TRANSACTION_H */

