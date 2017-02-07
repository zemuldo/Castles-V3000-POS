
/***************************************************************************
 *                                 
 * @ Danstan Otieno Onyango
 * Impalapay Kenya
 *
 ***************************************************************************/
/* <DESC>
 * simple HTTP POST using the easy interface
 * </DESC>
 */
#include <stdio.h>
#include "curl/curl.h"
#include "ctosapi.h"
#include "cJSON.h"
#include <stdlib.h>





int validation=0;

char *jsonout;

/* Parse text to JSON, then render back to text, and print! */
void doit(char *text)
{
	char *jsonout;cJSON *json;
	
	json=cJSON_Parse(text);
	if (!json) {printf("Error before: [%s]\n",cJSON_GetErrorPtr());}
	else
	{
		jsonout=cJSON_Print(json);
		cJSON_Delete(json);
		printf("%s\n",jsonout);
		free(jsonout);
	}
}

/* Read a file, parse, render back, etc. */
void dofile(char *filename)
{
	FILE *f;long len;char *data;
	
	f=fopen(filename,"rb");fseek(f,0,SEEK_END);len=ftell(f);fseek(f,0,SEEK_SET);
	data=(char*)malloc(len+1);fread(data,1,len,f);fclose(f);
	doit(data);
	free(data);
}

/* Used by some code below as an example datatype. */
struct record {const char *precision;double lat,lon;const char *address,*city,*state,*zip,*country; };


int curlpostmain(BYTE pin[4],BYTE amount[5]) {
    
    BYTE key;
    BYTE sBuf[128];
    
    {
        cJSON *root,*fmt,*img,*thm,*fld;int i;	/* declare a few. */
        
	
	
	//build json object-string
	root=cJSON_CreateObject();	
	cJSON_AddItemToObject(root, "pin", cJSON_CreateString(pin));
	cJSON_AddItemToObject(root, "amount",cJSON_CreateString(amount));
        cJSON_AddItemToObject(root, "card_number", cJSON_CreateString("555535353535"));
	
	
	jsonout=cJSON_Print(root);	cJSON_Delete(root);	/*printf("%s\n",jsonout);	free(jsonout);	/* Print to text, Delete the cJSON, print it, release the string. */

	
        CURL *curl;
        CURLcode res;
        CTOS_LCDTClearDisplay();
        CTOS_LCDTPrintXY(3, 2, "Sending........");

        curl = curl_easy_init();
        if (curl) {
            //char* jsonObj = "{ \"pin\" : \"4444\" , \"amount\" :\"10000\", \"card_number\" : \"72828276766262\" }"; 
            curl_easy_setopt(curl, CURLOPT_URL, "http://196.216.73.150:9990/posdemo/request/card");
            struct curl_slist *headers = NULL;
            headers = curl_slist_append(headers, "Accept: application/json");
            headers = curl_slist_append(headers, "Content-Type: application/json");
            headers = curl_slist_append(headers, "charsets: utf-8");
            /* example.com is redirected, so we tell libcurl to follow redirection */
            curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);

            /* Perform the request, res will get the return code */
             curl_easy_setopt(curl, CURLOPT_POSTFIELDS, jsonout);
            res = curl_easy_perform(curl);
            //CTOS_LCDTPrintXY(4, 4, " Response is");
            /* Check for errors */
            int http_code = 0;
            curl_easy_getinfo (curl, CURLINFO_RESPONSE_CODE, &http_code);
            if (http_code == 200 && res != CURLE_ABORTED_BY_CALLBACK)
                 {
                CTOS_LCDTClearDisplay();
                CTOS_LCDTPrintXY(4, 6, " Successful");
                CTOS_KBDGet(&key);
                CTOS_Delay(3000);
                validation=1;
                curl_easy_cleanup(curl);
                return 1;
            }
            else if(http_code == 401)
            {
                CTOS_LCDTClearDisplay();
                CTOS_LCDTPrintXY(4, 6, " Wrong PIN");
                CTOS_KBDGet(&key);
                CTOS_Delay(3000);
                 validation=0;
                 curl_easy_cleanup(curl);
                return 0;
            }
            CTOS_LCDTPrintXY(3, 2, " Failed");
                CTOS_KBDGet(&key);
                return;
            curl_easy_cleanup(curl);
        }
        
    }


}