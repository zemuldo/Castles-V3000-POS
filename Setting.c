#include <string.h>
#include <stdio.h>
#include <ctosapi.h>
#include <emv_cl.h>
#include "wub_lib.h"
#include "Transaction.h"
#include "debug2.h"


BYTE g_baTempbuffer[2048];

//------------------------------------------------------------------------------
EMVCL_AID_SET_TAG_DATA stTagData;
EMVCL_AID_GET_TAG_DATA stGetTagData;

//Data Format : TLV1 + TLV2 + TLV3 + ....
#if 0
\x9F\x35\x01\x00
\x9F\x33\x03\x00\x08\xC8
\x9F\x40\x05\x6F\x00\x00\x20\x01
\x9F\x09\x02\x01\x05
\x9F\x1A\x02\x07\x24
\x5F\x2A\x02\x08\x78
\x9F\x1B\x04\x00\x00\x07\xD4
\x9F\x7A\x01\x01
\xDF\x21\x01\x01
\xDF\x22\x01\x01
\xDF\x25\x01\xFF
\xDF\x00\x01\xFF
\xDF\x01\x06\x00\x00\x00\x00\x09\x01
\xDF\x02\x06\x00\x00\x00\x00\x15\x01
\x9F\x66\x04\x36\x00\x40\x00
\xDF\x06\x01\x01
\xDF\x8F\x4B\x01\x00
\xDF\x24\x01\x01\xDF\x23\x81\x9B\x05\x31\x02\x68\x26\x20\x1F\xDF\x25\x01\xFF\xDF\x8F\x4B\x01\x00\xDF\x00\x01\xFF\xDF\x01\x06\x00\x00\x00\x00\x30\x01\xDF\x02\x06\x00\x00\x00\x00\x30\x01\x08\x31\x02\x68\x26\x12\x00\x00\x03\x1F\xDF\x25\x01\xFF\xDF\x8F\x4B\x01\x00\xDF\x00\x01\xFF\xDF\x01\x06\x00\x00\x00\x00\x15\x01\xDF\x02\x06\x00\x00\x00\x00\x10\x01\x05\x31\x02\x68\x26\x12\x1F\xDF\x25\x01\xFF\xDF\x8F\x4B\x01\x00\xDF\x00\x01\xFF\xDF\x01\x06\x00\x00\x00\x00\x15\x01\xDF\x02\x06\x00\x00\x00\x00\x25\x01\x05\x31\x02\x68\x26\x00\x1F\xDF\x25\x01\xFF\xDF\x8F\x4B\x01\x00\xDF\x00\x01\xFF\xDF\x01\x06\x00\x00\x00\x00\x20\x01\xDF\x02\x06\x00\x00\x00\x00\x15\x01
\xDF\x05\x01\x01
#endif

#define d_SET_TAG "\x9F\x35\x01\x00\x9F\x33\x03\x00\x08\xC8\x9F\x40\x05\x6F\x00\x00\x20\x01\x9F\x09\x02\x01\x05\x9F\x1A\x02\x07\x24\x5F\x2A\x02\x08\x78\x9F\x1B\x04\x00\x00\x07\xD4\x9F\x7A\x01\x01\xDF\x21\x01\x01\xDF\x22\x01\x01\xDF\x25\x01\xFF\xDF\x00\x01\xFF\xDF\x01\x06\x00\x00\x00\x00\x09\x01\xDF\x02\x06\x00\x00\x00\x00\x15\x01\x9F\x66\x04\x36\x00\x40\x00\xDF\x06\x01\x01\xDF\x8F\x4B\x01\x00\xDF\x24\x01\x01\xDF\x23\x81\x9B\x05\x31\x02\x68\x26\x20\x1F\xDF\x25\x01\xFF\xDF\x8F\x4B\x01\x00\xDF\x00\x01\xFF\xDF\x01\x06\x00\x00\x00\x00\x30\x01\xDF\x02\x06\x00\x00\x00\x00\x30\x01\x08\x31\x02\x68\x26\x12\x00\x00\x03\x1F\xDF\x25\x01\xFF\xDF\x8F\x4B\x01\x00\xDF\x00\x01\xFF\xDF\x01\x06\x00\x00\x00\x00\x15\x01\xDF\x02\x06\x00\x00\x00\x00\x10\x01\x05\x31\x02\x68\x26\x12\x1F\xDF\x25\x01\xFF\xDF\x8F\x4B\x01\x00\xDF\x00\x01\xFF\xDF\x01\x06\x00\x00\x00\x00\x15\x01\xDF\x02\x06\x00\x00\x00\x00\x25\x01\x05\x31\x02\x68\x26\x00\x1F\xDF\x25\x01\xFF\xDF\x8F\x4B\x01\x00\xDF\x00\x01\xFF\xDF\x01\x06\x00\x00\x00\x00\x20\x01\xDF\x02\x06\x00\x00\x00\x00\x15\x01\xDF\x05\x01\x01"

void Tag(void)
{
	BYTE bKey, i;
	ULONG rtn;
	
	while(1)
	{
		ClearScreen(4, 14);
		ShowTitle("       Tag     ");
		
		CTOS_LCDTPrintXY(1, 4, "1.Set TagSetting");
		CTOS_LCDTPrintXY(1, 5, "2.Get TagSetting");
		CTOS_LCDTPrintXY(1, 6, "3.Del TagSetting");
		CTOS_LCDTPrintXY(1, 7, "4.Del All Tag   ");
		
next:		
		CTOS_KBDGet(&bKey);
		
		if(bKey == '1')
		{
			//Set PayPass AID with TagsSetting to EMVCL kernel
			
			//tag setting of (AID A0000000031010 + Kernel ID 0x03 +  Txn Type = 0x00(sale))
			stTagData.bAIDLen = 0x07;
			memcpy(stTagData.baAID, "\xA0\x00\x00\x00\x03\x10\x10", 7);
			stTagData.bKernelID = 0x03;
			stTagData.bTransactionType = 0x00;
			stTagData.usTAGDataLen = sizeof(d_SET_TAG)-1;
			stTagData.pbaTAGData = (BYTE*)d_SET_TAG;
	
			rtn = EMVCL_AIDSetTagData(0x00, &stTagData);
			DebugAddINT("Set TagSetting(purchase) Rtn", rtn);
			
			if(rtn != 0)
			{
				ClearScreen(4, 14);
				ShowRtn(4, "Set Tag Fail    ", rtn);
				CTOS_KBDGet(&bKey);
			}
			
			//tag setting of (AID A0000000031010 + Kernel ID 0x03 +  Txn Type = 0x01(cash))
			stTagData.bTransactionType = 0x01;
			rtn = EMVCL_AIDSetTagData(0x00, &stTagData);
			DebugAddINT("Set TagSetting(Cash) Rtn", rtn);
			if(rtn != 0)
			{
				ClearScreen(4, 14);
				ShowRtn(4, "Set Tag Fail    ", rtn);
				CTOS_KBDGet(&bKey);
			}
			
			//tag setting of (AID A0000000031010 + Kernel ID 0x03 +  Txn Type = 0x09(cashback))
			stTagData.bTransactionType = 0x09;
			rtn = EMVCL_AIDSetTagData(0x00, &stTagData);
			DebugAddINT("Set TagSetting(Cashback) Rtn", rtn);
			if(rtn != 0)
			{
				ClearScreen(4, 14);
				ShowRtn(4, "Set Tag Fail    ", rtn);
				CTOS_KBDGet(&bKey);
			}
			
			//tag setting of (AID A0000000031010 + Kernel ID 0x03 +  Txn Type = 0x20(refund))
			stTagData.bTransactionType = 0x20;
			rtn = EMVCL_AIDSetTagData(0x00, &stTagData);
			DebugAddINT("Set TagSetting(Refund) Rtn", rtn);
			if(rtn != 0)
			{
				ClearScreen(4, 14);
				ShowRtn(4, "Set Tag Fail    ", rtn);
				CTOS_KBDGet(&bKey);
			}
			
			ClearScreen(4, 14);
			ShowRtn(4, "Set Tag OK      ", rtn);
			CTOS_KBDGet(&bKey);
		}
		else if(bKey == '2')
		{
			//Get tag setting of (AID A0000000031010 + Kernel ID 0x03 +  Txn Type = 0x00(sale))
			stGetTagData.bAIDLen = 0x07;
			memcpy(stGetTagData.baAID, "\xA0\x00\x00\x00\x03\x10\x10", 7);
			stGetTagData.bKernelID = 0x03;
			stGetTagData.bTransactionType = 0x00;
			stGetTagData.pbaTAGData = g_baTempbuffer;
					
			rtn = EMVCL_AIDGetTagData(&stGetTagData);
			
			ClearScreen(4, 14);
			if(rtn != 0)
			{
				ShowRtn(4, "Get Tag Fail    ", rtn);
			}
			else
			{
				ShowRtn(4, "Get Tag OK      ", rtn);
				DebugAddHEX("Get Tag", stGetTagData.pbaTAGData, stGetTagData.usTAGDataLen);
			}
			
			CTOS_KBDGet(&bKey);
		}
		else if(bKey == '3')
		{
			//Delete tag setting of (AID A0000000031010 + Kernel ID 0x03 +  Txn Type = 0x00(sale))
			stTagData.bAIDLen = 0x07;
			memcpy(stTagData.baAID, "\xA0\x00\x00\x00\x03\x10\x10", 7);
			stTagData.bKernelID = 0x03;
			stTagData.bTransactionType = 0x00;
	
			rtn = EMVCL_AIDSetTagData(0x01, &stTagData);
			DebugAddINT("Del TagSetting(purchase) Rtn", rtn);
			
			ClearScreen(4, 14);
			if(rtn != 0)
			{
				ShowRtn(4, "Del Tag Fail    ", rtn);
			}
			else
			{
				ShowRtn(4, "Del Tag OK    ", rtn);
			}
			
			CTOS_KBDGet(&bKey);
		}
		else if(bKey == '4')
		{
			//Delete All tagsetting.
			rtn = EMVCL_AIDSetTagData(0x02, &stTagData);
			DebugAddINT("Del All TagSetting Rtn", rtn);
			
			ClearScreen(4, 14);
			if(rtn != 0)
			{
				ShowRtn(4, "Del All Tag Fail    ", rtn);
			}
			else
			{
				ShowRtn(4, "Del All Tag OK      ", rtn);
			}
			
			CTOS_KBDGet(&bKey);
		}
		else if(bKey == d_KBD_CANCEL)
		{
			break;
		}
		else
		{
			goto next;
		}
		
	}	
}

//------------------------------------------------------------------------------
#define d_CAPK_RID	"\xA0\x00\x00\x00\x03"

#define d_CAPK_INDEX_1	0x99
#define d_CAPK_M_1	"\xAB\x79\xFC\xC9\x52\x08\x96\x96\x7E\x77\x6E\x64\x44\x4E\x5D\xCD\xD6\xE1\x36\x11\x87\x4F\x39\x85\x72\x25\x20\x42\x52\x95\xEE\xA4\xBD\x0C\x27\x81\xDE\x7F\x31\xCD\x3D\x04\x1F\x56\x5F\x74\x73\x06\xEE\xD6\x29\x54\xB1\x7E\xDA\xBA\x3A\x6C\x5B\x85\xA1\xDE\x1B\xEB\x9A\x34\x14\x1A\xF3\x8F\xCF\x82\x79\xC9\xDE\xA0\xD5\xA6\x71\x0D\x08\xDB\x41\x24\xF0\x41\x94\x55\x87\xE2\x03\x59\xBA\xB4\x7B\x75\x75\xAD\x94\x26\x2D\x4B\x25\xF2\x64\xAF\x33\xDE\xDC\xF2\x8E\x09\x61\x5E\x93\x7D\xE3\x2E\xDC\x03\xC5\x44\x45\xFE\x7E\x38\x27\x77"
#define d_CAPK_E_1	"\x03"

#define d_CAPK_INDEX_2	0x95
#define d_CAPK_M_2	"\xBE\x9E\x1F\xA5\xE9\xA8\x03\x85\x29\x99\xC4\xAB\x43\x2D\xB2\x86\x00\xDC\xD9\xDA\xB7\x6D\xFA\xAA\x47\x35\x5A\x0F\xE3\x7B\x15\x08\xAC\x6B\xF3\x88\x60\xD3\xC6\xC2\xE5\xB1\x2A\x3C\xAA\xF2\xA7\x00\x5A\x72\x41\xEB\xAA\x77\x71\x11\x2C\x74\xCF\x9A\x06\x34\x65\x2F\xBC\xA0\xE5\x98\x0C\x54\xA6\x47\x61\xEA\x10\x1A\x11\x4E\x0F\x0B\x55\x72\xAD\xD5\x7D\x01\x0B\x7C\x9C\x88\x7E\x10\x4C\xA4\xEE\x12\x72\xDA\x66\xD9\x97\xB9\xA9\x0B\x5A\x6D\x62\x4A\xB6\xC5\x7E\x73\xC8\xF9\x19\x00\x0E\xB5\xF6\x84\x89\x8E\xF8\xC3\xDB\xEF\xB3\x30\xC6\x26\x60\xBE\xD8\x8E\xA7\x8E\x90\x9A\xFF\x05\xF6\xDA\x62\x7B"
#define d_CAPK_E_2	"\x03"

#define d_CAPK_INDEX_3	0x92
#define d_CAPK_M_3	"\x99\x6A\xF5\x6F\x56\x91\x87\xD0\x92\x93\xC1\x48\x10\x45\x0E\xD8\xEE\x33\x57\x39\x7B\x18\xA2\x45\x8E\xFA\xA9\x2D\xA3\xB6\xDF\x65\x14\xEC\x06\x01\x95\x31\x8F\xD4\x3B\xE9\xB8\xF0\xCC\x66\x9E\x3F\x84\x40\x57\xCB\xDD\xF8\xBD\xA1\x91\xBB\x64\x47\x3B\xC8\xDC\x9A\x73\x0D\xB8\xF6\xB4\xED\xE3\x92\x41\x86\xFF\xD9\xB8\xC7\x73\x57\x89\xC2\x3A\x36\xBA\x0B\x8A\xF6\x53\x72\xEB\x57\xEA\x5D\x89\xE7\xD1\x4E\x9C\x7B\x6B\x55\x74\x60\xF1\x08\x85\xDA\x16\xAC\x92\x3F\x15\xAF\x37\x58\xF0\xF0\x3E\xBD\x3C\x5C\x2C\x94\x9C\xBA\x30\x6D\xB4\x4E\x6A\x2C\x07\x6C\x5F\x67\xE2\x81\xD7\xEF\x56\x78\x5D\xC4\xD7\x59\x45\xE4\x91\xF0\x19\x18\x80\x0A\x9E\x2D\xC6\x6F\x60\x08\x05\x66\xCE\x0D\xAF\x8D\x17\xEA\xD4\x6A\xD8\xE3\x0A\x24\x7C\x9F"
#define d_CAPK_E_3	"\x03"

#define d_CAPK_INDEX_4	0x94
#define d_CAPK_M_4	"\xAC\xD2\xB1\x23\x02\xEE\x64\x4F\x3F\x83\x5A\xBD\x1F\xC7\xA6\xF6\x2C\xCE\x48\xFF\xEC\x62\x2A\xA8\xEF\x06\x2B\xEF\x6F\xB8\xBA\x8B\xC6\x8B\xBF\x6A\xB5\x87\x0E\xED\x57\x9B\xC3\x97\x3E\x12\x13\x03\xD3\x48\x41\xA7\x96\xD6\xDC\xBC\x41\xDB\xF9\xE5\x2C\x46\x09\x79\x5C\x0C\xCF\x7E\xE8\x6F\xA1\xD5\xCB\x04\x10\x71\xED\x2C\x51\xD2\x20\x2F\x63\xF1\x15\x6C\x58\xA9\x2D\x38\xBC\x60\xBD\xF4\x24\xE1\x77\x6E\x2B\xC9\x64\x80\x78\xA0\x3B\x36\xFB\x55\x43\x75\xFC\x53\xD5\x7C\x73\xF5\x16\x0E\xA5\x9F\x3A\xFC\x53\x98\xEC\x7B\x67\x75\x8D\x65\xC9\xBF\xF7\x82\x8B\x6B\x82\xD4\xBE\x12\x4A\x41\x6A\xB7\x30\x19\x14\x31\x1E\xA4\x62\xC1\x9F\x77\x1F\x31\xB3\xB5\x73\x36\x00\x0D\xFF\x73\x2D\x3B\x83\xDE\x07\x05\x2D\x73\x03\x54\xD2\x97\xBE\xC7\x28\x71\xDC\xCF\x0E\x19\x3F\x17\x1A\xBA\x27\xEE\x46\x4C\x6A\x97\x69\x09\x43\xD5\x9B\xDA\xBB\x2A\x27\xEB\x71\xCE\xEB\xDA\xFA\x11\x76\x04\x64\x78\xFD\x62\xFE\xC4\x52\xD5\xCA\x39\x32\x96\x53\x0A\xA3\xF4\x19\x27\xAD\xFE\x43\x4A\x2D\xF2\xAE\x30\x54\xF8\x84\x06\x57\xA2\x6E\x0F\xC6\x17"
#define d_CAPK_E_4	"\x03"

ULONG SetCAPK(void)
{
	EMVCL_CA_PUBLIC_KEY stCAPubKey;
	ULONG rtn;
	BYTE inbuf[1024];
	SHA_CTX sha;
	
//Set CAPK via EMVCL_SetCAPK function
	
	//bAction = 0x00 -> Add, bAction = 0x01 -> Del
	stCAPubKey.bAction = 0x00;
	
	//CAPK1---------------------------------------------------------------------
	//CAPK Index
	stCAPubKey.bIndex = d_CAPK_INDEX_1;
	//Modulus
	stCAPubKey.uiModulusLen = sizeof(d_CAPK_M_1) -1;
	memcpy(stCAPubKey.baModulus, d_CAPK_M_1, stCAPubKey.uiModulusLen);
	//Exponent
	stCAPubKey.uiExponentLen = sizeof(d_CAPK_E_1) -1;
	memcpy(stCAPubKey.baExponent, d_CAPK_E_1, stCAPubKey.uiExponentLen);
	//HAsh
	CTOS_SHA1Init (&sha);
	CTOS_SHA1Update (&sha, d_CAPK_RID, sizeof(d_CAPK_RID)-1);
	CTOS_SHA1Update (&sha, &stCAPubKey.bIndex, 1);
	CTOS_SHA1Update (&sha, stCAPubKey.baModulus, stCAPubKey.uiModulusLen);
	CTOS_SHA1Update (&sha, stCAPubKey.baExponent, stCAPubKey.uiExponentLen);
	CTOS_SHA1Final (stCAPubKey.baHash, &sha);

	rtn = EMVCL_SetCAPK(d_CAPK_RID, &stCAPubKey);
	DebugAddINT("Set CAPK 1 rtn", rtn);
	if(rtn != 0)
	{
		return rtn;
	}
	
	//CAPK2---------------------------------------------------------------------
	//CAPK Index
	stCAPubKey.bIndex = d_CAPK_INDEX_2;
	//Modulus
	stCAPubKey.uiModulusLen = sizeof(d_CAPK_M_2) -1;
	memcpy(stCAPubKey.baModulus, d_CAPK_M_2, stCAPubKey.uiModulusLen);
	//Exponent
	stCAPubKey.uiExponentLen = sizeof(d_CAPK_E_2) -1;
	memcpy(stCAPubKey.baExponent, d_CAPK_E_2, stCAPubKey.uiExponentLen);
	//HAsh
	CTOS_SHA1Init (&sha);
	CTOS_SHA1Update (&sha, d_CAPK_RID, sizeof(d_CAPK_RID)-1);
	CTOS_SHA1Update (&sha, &stCAPubKey.bIndex, 1);
	CTOS_SHA1Update (&sha, stCAPubKey.baModulus, stCAPubKey.uiModulusLen);
	CTOS_SHA1Update (&sha, stCAPubKey.baExponent, stCAPubKey.uiExponentLen);
	CTOS_SHA1Final (stCAPubKey.baHash, &sha);

	rtn = EMVCL_SetCAPK(d_CAPK_RID, &stCAPubKey);
	DebugAddINT("Set CAPK 2 rtn", rtn);
	if(rtn != 0)
	{
		return rtn;
	}
	
	//CAPK3---------------------------------------------------------------------
	//CAPK Index
	stCAPubKey.bIndex = d_CAPK_INDEX_3;
	//Modulus
	stCAPubKey.uiModulusLen = sizeof(d_CAPK_M_3) -1;
	memcpy(stCAPubKey.baModulus, d_CAPK_M_3, stCAPubKey.uiModulusLen);
	//Exponent
	stCAPubKey.uiExponentLen = sizeof(d_CAPK_E_3) -1;
	memcpy(stCAPubKey.baExponent, d_CAPK_E_3, stCAPubKey.uiExponentLen);
	//HAsh
	CTOS_SHA1Init (&sha);
	CTOS_SHA1Update (&sha, d_CAPK_RID, sizeof(d_CAPK_RID)-1);
	CTOS_SHA1Update (&sha, &stCAPubKey.bIndex, 1);
	CTOS_SHA1Update (&sha, stCAPubKey.baModulus, stCAPubKey.uiModulusLen);
	CTOS_SHA1Update (&sha, stCAPubKey.baExponent, stCAPubKey.uiExponentLen);
	CTOS_SHA1Final (stCAPubKey.baHash, &sha);

	rtn = EMVCL_SetCAPK(d_CAPK_RID, &stCAPubKey);
	DebugAddINT("Set CAPK 3 rtn", rtn);
	if(rtn != 0)
	{
		return rtn;
	}
	
	//CAPK4---------------------------------------------------------------------
	//CAPK Index
	stCAPubKey.bIndex = d_CAPK_INDEX_4;
	//Modulus
	stCAPubKey.uiModulusLen = sizeof(d_CAPK_M_4) -1;
	memcpy(stCAPubKey.baModulus, d_CAPK_M_4, stCAPubKey.uiModulusLen);
	//Exponent
	stCAPubKey.uiExponentLen = sizeof(d_CAPK_E_4) -1;
	memcpy(stCAPubKey.baExponent, d_CAPK_E_4, stCAPubKey.uiExponentLen);
	//HAsh
	CTOS_SHA1Init (&sha);
	CTOS_SHA1Update (&sha, d_CAPK_RID, sizeof(d_CAPK_RID)-1);
	CTOS_SHA1Update (&sha, &stCAPubKey.bIndex, 1);
	CTOS_SHA1Update (&sha, stCAPubKey.baModulus, stCAPubKey.uiModulusLen);
	CTOS_SHA1Update (&sha, stCAPubKey.baExponent, stCAPubKey.uiExponentLen);
	CTOS_SHA1Final (stCAPubKey.baHash, &sha);

	rtn = EMVCL_SetCAPK(d_CAPK_RID, &stCAPubKey);
	DebugAddINT("Set CAPK 4 rtn", rtn);
	if(rtn != 0)
	{
		return rtn;
	}
	
	return 0;
}

ULONG DelCAPK(void)
{
	EMVCL_CA_PUBLIC_KEY stCAPubKey;
	ULONG rtn;
 
	//bAction = 0x00 -> Add, bAction = 0x01 -> Del
	stCAPubKey.bAction = 0x01;
	//CAPK Index
	stCAPubKey.bIndex = d_CAPK_INDEX_1;

	rtn = EMVCL_SetCAPK(d_CAPK_RID, &stCAPubKey);
	
	return rtn;
}

ULONG ReaseAllCAPK(void)
{
	EMVCL_CA_PUBLIC_KEY stCAPubKey;
	ULONG rtn;
 
	//bAction = 0x00 -> Add, bAction = 0x01 -> Del
	stCAPubKey.bAction = 0x02;
	
	rtn = EMVCL_SetCAPK(NULL, &stCAPubKey);
	
	return rtn;
}

void CAPK(void)
{
	BYTE bKey, i;
	ULONG rtn;
	BYTE baRBuf[1024];
	UINT uiRLen;
	EMVCL_CA_PUBLIC_KEY stCAPubKey;
	
	while(1)
	{
		ClearScreen(4, 14);
		ShowTitle("       CAPK       ");
		
		CTOS_LCDTPrintXY(1, 4, "1.Set CAPK   ");
		CTOS_LCDTPrintXY(1, 5, "2.Get CAPK   ");
		CTOS_LCDTPrintXY(1, 6, "3.Del CAPK   ");
		CTOS_LCDTPrintXY(1, 7, "4.Erase CAPK ");
		CTOS_LCDTPrintXY(1, 8, "5.List CAPK ");
		
next:		
		CTOS_KBDGet(&bKey);
		
		if(bKey == '1')
		{
			rtn = SetCAPK();
			DebugAddINT("Set CAPK Rtn", rtn);
			
			ClearScreen(4, 14);
			if(rtn != 0)
			{
				ShowRtn(4, "Set CAPK Fail   ", rtn);
			}
			else
			{
				ShowRtn(4, "Set CAPK OK   ", rtn);
			}
			
			CTOS_KBDGet(&bKey);
			
		}
		else if(bKey == '2')
		{
			rtn = EMVCL_GetCAPK(d_CAPK_RID, d_CAPK_INDEX_1, &stCAPubKey);
			DebugAddINT("EMVCL_GetCAPK Rtn", rtn);
			
			ClearScreen(4, 14);
			if(rtn != 0)
			{
				ShowRtn(4, "Get CAPK Fail   ", rtn);
			}
			else
			{	
				ShowRtn(4, "Get CAPK OK     ", rtn);
				DebugAddHEX("stCAPubKey.baExponent", stCAPubKey.baExponent, stCAPubKey.uiExponentLen);		
				DebugAddHEX("stCAPubKey.baModulus", stCAPubKey.baModulus, stCAPubKey.uiModulusLen);		
			}
			
			CTOS_KBDGet(&bKey);
		}
		else if(bKey == '3')
		{
			rtn = DelCAPK();
			DebugAddINT("DelCAPK Rtn", rtn);
			
			ClearScreen(4, 14);
			if(rtn != 0)
			{
				ShowRtn(4, "Del CAPK Fail   ", rtn);
			}
			else
			{
				ShowRtn(4, "Del CAPK OK     ", rtn);
			}
			
			CTOS_KBDGet(&bKey);
		}
		else if(bKey == '4')
		{
			rtn = ReaseAllCAPK();
			DebugAddINT("ReaseAllCAPK Rtn", rtn);
			
			ClearScreen(4, 14);
			if(rtn != 0)
			{
				ShowRtn(4, "Erase CAPK Fail ", rtn);	
			}
			else
			{
				ShowRtn(4, "Erase CAPK OK   ", rtn);	
			}
			
			CTOS_KBDGet(&bKey);
		}
		else if(bKey == '5')
		{
			EMVCL_ListAllCAPKID(baRBuf, &uiRLen);
			DebugAddHEX("List CAPK", baRBuf, uiRLen);
		}
		else if(bKey == d_KBD_CANCEL)
		{
			break;
		}
		else
		{
			goto next;
		}
		
	}	
}

//------------------------------------------------------------------------------
void UIType(void)
{
	BYTE bKey;
	ULONG rtn;
	BYTE bType;
	
	while(1)
	{
		ClearScreen(4, 14);
		ShowTitle("       UI Type       ");
		
		CTOS_LCDTPrintXY(1, 4, "1.Set Normal UI ");
		CTOS_LCDTPrintXY(1, 5, "2.Set EUR UI    ");
		CTOS_LCDTPrintXY(1, 6, "3.Get UI Type   ");
		
next:		
		CTOS_KBDGet(&bKey);
		
		if(bKey == '1')
		{
			rtn = EMVCL_SetUIType(d_EMVCL_UITYPE_NORMAL);
			DebugAddINT("Set Normal UI Rtn", rtn);
			if(rtn != 0)
			{
				ShowRtn(4, "Set Normal UI   \nFail            ", rtn);
				CTOS_KBDGet(&bKey);
			}
		}
		else if(bKey == '2')
		{
			rtn = EMVCL_SetUIType(d_EMVCL_UITYPE_EUR);
			DebugAddINT("Set EUR UI Rtn", rtn);
			if(rtn != 0)
			{
				ShowRtn(4, "Set EUR UI Fail ", rtn);
				CTOS_KBDGet(&bKey);
			}
		}
		else if(bKey == '3')
		{
			EMVCL_GetUIType(&bType);
			DebugAddINT("Current UI Type", bType);	
		}
		else if(bKey == d_KBD_CANCEL)
		{
			break;
		}
		else
		{
			goto next;
		}
		
	}
	
}

//------------------------------------------------------------------------------
void Do_Setting(void)
{
	BYTE bKey;
    
	while(1)
	{
		ClearScreen(4, 14);
		ShowTitle("      Setting     ");
		
		CTOS_LCDTPrintXY(1, 4, "1. Tag          ");
		CTOS_LCDTPrintXY(1, 5, "2. CAPK         ");
		CTOS_LCDTPrintXY(1, 6, "3. UI Type      ");
next:		
		CTOS_KBDGet(&bKey);
		
		if(bKey == '1')
		{
			Tag();
		}
		else if(bKey == '2')
		{
			CAPK();
		}
		else if(bKey == '3')
		{
			UIType();
		}
		else if(bKey == d_KBD_CANCEL)
		{
			break;
		}
		else
		{
			goto next;
		}
	}

}

