//#include <stdio.h>
//#include <stdlib.h>
//#include "cJSON.h"
//#include "ctosapi.h"
//
//
//char *jsonout;
//
///* Parse text to JSON, then render back to text, and print! */
//void doit(char *text)
//{
//	char *jsonout;cJSON *json;
//	
//	json=cJSON_Parse(text);
//	if (!json) {printf("Error before: [%s]\n",cJSON_GetErrorPtr());}
//	else
//	{
//		jsonout=cJSON_Print(json);
//		cJSON_Delete(json);
//		printf("%s\n",jsonout);
//		free(jsonout);
//	}
//}
//
///* Read a file, parse, render back, etc. */
//void dofile(char *filename)
//{
//	FILE *f;long len;char *data;
//	
//	f=fopen(filename,"rb");fseek(f,0,SEEK_END);len=ftell(f);fseek(f,0,SEEK_SET);
//	data=(char*)malloc(len+1);fread(data,1,len,f);fclose(f);
//	doit(data);
//	free(data);
//}
//
///* Used by some code below as an example datatype. */
//struct record {const char *precision;double lat,lon;const char *address,*city,*state,*zip,*country; };
//
//void create_objects(pin)
//{
//	cJSON *root,*fmt,*img,*thm,*fld;int i;	/* declare a few. */
//        
//	
//	
//	/* Our "Video" datatype: */
//	root=cJSON_CreateObject();	
//	cJSON_AddItemToObject(root, "pin", cJSON_CreateString("4444"));
//	cJSON_AddItemToObject(root, "amount",cJSON_CreateString("1000"));
//        cJSON_AddItemToObject(root, "card_number", cJSON_CreateString("24637846"));
//	
//	
//	jsonout=cJSON_Print(root);	cJSON_Delete(root);	/*printf("%s\n",jsonout);	free(jsonout);	/* Print to text, Delete the cJSON, print it, release the string. */
//
//	
//
//	return;
//
//
//
//}
