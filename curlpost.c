
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

char *jsonout;
char *responseExitCode;
char *message;
char *accountnumber;
char *accounttype;
char *accountname;
char *retballance;



int validation = 0;
int cardles_validation = 0;

char *jsonout;

/* Parse text to JSON, then render back to text, and print! */
void doit(char *text) {
    char *jsonout;
    cJSON *json;

    json = cJSON_Parse(text);
    if (!json) {
        printf("Error before: [%s]\n", cJSON_GetErrorPtr());
    } else {
        jsonout = cJSON_Print(json);
        cJSON_Delete(json);
        printf("%s\n", jsonout);
        free(jsonout);
    }
}

/* Read a file, parse, render back, etc. */
void dofile(char *filename) {
    FILE *f;
    long len;
    char *data;

    f = fopen(filename, "rb");
    fseek(f, 0, SEEK_END);
    len = ftell(f);
    fseek(f, 0, SEEK_SET);
    data = (char*) malloc(len + 1);
    fread(data, 1, len, f);
    fclose(f);
    doit(data);
    free(data);
}

struct string1 {
    char *ptr;
    size_t len;
};

void init_string1(struct string1 *p) {
    p->len = 0;
    p->ptr = malloc(p->len + 1);
    if (p->ptr == NULL) {
        fprintf(stderr, "malloc() failed\n");
        exit(EXIT_FAILURE);
    }
    p->ptr[0] = '\0';
}

size_t writefunc1(void *ptr, size_t size, size_t nmemb, struct string1 *p) {
    size_t new_len = p->len + size*nmemb;
    p->ptr = realloc(p->ptr, new_len + 1);
    if (p->ptr == NULL) {
        fprintf(stderr, "realloc() failed\n");
        exit(EXIT_FAILURE);
    }
    memcpy(p->ptr + p->len, ptr, size * nmemb);
    p->ptr[new_len] = '\0';
    p->len = new_len;

    return size*nmemb;
}


/* Used by some code below as an example datatype. */
struct record {
    const char *precision;
    double lat, lon;
    const char *address, *city, *state, *zip, *country;
};

int withdraw_post(BYTE pin[4], BYTE amount[20], BYTE recipientcardtype[255], BYTE receipentcardno[17], BYTE recipientCardexpiry_date[6],BYTE merchanpin[4],BYTE merchancardtype[255], BYTE merchancardno[17], BYTE merchanCardexpiry_date[6]) {

    BYTE key;
    BYTE sBuf[128];

    {
        cJSON *root, *fmt, *img, *thm, *fld;
        int i; /* declare a few. */

        

        //build json object-string
        root = cJSON_CreateObject();
        cJSON_AddItemToObject(root, "username", cJSON_CreateString("familypos"));
        cJSON_AddItemToObject(root, "password", cJSON_CreateString("password"));
        cJSON_AddItemToObject(root, "merchantCardNo", cJSON_CreateString(merchancardno));
        cJSON_AddItemToObject(root, "merchantCVV", cJSON_CreateString("343"));
        cJSON_AddItemToObject(root, "merchantcard_type", cJSON_CreateString(merchancardtype));
        cJSON_AddItemToObject(root, "merchantCardexpiry_date", cJSON_CreateString(merchanCardexpiry_date));        
        cJSON_AddItemToObject(root, "recipientCVV", cJSON_CreateString("445"));
        cJSON_AddItemToObject(root, "recipientCardNo", cJSON_CreateString(receipentcardno));
        cJSON_AddItemToObject(root, "amount", cJSON_CreateString(amount));
        cJSON_AddItemToObject(root, "accountnumber", cJSON_CreateString("35325235252"));
        cJSON_AddItemToObject(root, "merchantPin", cJSON_CreateString(merchanpin));
        cJSON_AddItemToObject(root, "recipientPin", cJSON_CreateString(pin));
        cJSON_AddItemToObject(root, "recipientcard_type", cJSON_CreateString(recipientcardtype));
        cJSON_AddItemToObject(root, "recipientCardexpiry_date", cJSON_CreateString(recipientCardexpiry_date));
        cJSON_AddItemToObject(root, "auth_code", cJSON_CreateString("3866647702"));



        jsonout = cJSON_Print(root);
        cJSON_Delete(root); /*printf("%s\n",jsonout);	free(jsonout);	/* Print to text, Delete the cJSON, print it, release the string. */


        CURL *curl;
        CURLcode res;
        ClearScreen(4, 26);
        CTOS_LCDTPrintXY(3, 4, "Sending........");

        curl = curl_easy_init();
        if (curl) {
            //char* jsonObj = "{ \"pin\" : \"4444\" , \"amount\" :\"10000\", \"card_number\" : \"72828276766262\" }"; 
            curl_easy_setopt(curl, CURLOPT_URL, "http://196.216.73.150:9990/familypos/request/bankWitdrawal");
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
            /* Check for errors in the request*/
            //CTOS_LCDTPrintXY(1, 1,jsonout);
            //CTOS_KBDGet(&key);
            long http_code = 0;
            curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_code);
            if (http_code == 200 && res != CURLE_ABORTED_BY_CALLBACK) {
                ClearScreen(4, 26);
                CTOS_LCDTPrintXY(4, 6, " Successful");
                CTOS_KBDGet(&key);
                CTOS_Delay(3000);
                validation = 1;
                curl_easy_cleanup(curl);
                ClearScreen(4, 26);
                return 1;
            } else if (http_code == 401) {
                ClearScreen(4, 26);
                CTOS_LCDTPrintXY(4, 6, " Wrong PIN");
                CTOS_KBDGet(&key);
                CTOS_Delay(3000);
                validation = 0;
                curl_easy_cleanup(curl);
                ClearScreen(4, 26);
                return 0;
            }
            CTOS_LCDTPrintXY(3, 4, " Failed");
            curl_easy_cleanup(curl);
            CTOS_KBDGet(&key);
            ClearScreen(4, 26);
            return;
        }

    }


}

int curlpostmain(BYTE pin[4], BYTE amount[5]) {

    BYTE key;
    BYTE sBuf[128];

    {
        cJSON *root, *fmt, *img, *thm, *fld;
        int i; /* declare a few. */



        //build json object-string
        root = cJSON_CreateObject();
        cJSON_AddItemToObject(root, "pin", cJSON_CreateString(pin));
        cJSON_AddItemToObject(root, "amount", cJSON_CreateString(amount));
        cJSON_AddItemToObject(root, "card_number", cJSON_CreateString("555535353535"));


        jsonout = cJSON_Print(root);
        cJSON_Delete(root); /*printf("%s\n",jsonout);	free(jsonout);	/* Print to text, Delete the cJSON, print it, release the string. */


        CURL *curl;
        CURLcode res;
        ClearScreen(4, 26);
        CTOS_LCDTPrintXY(3, 4, "Sending........");

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
            free(jsonout);
            long http_code = 0;
            curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_code);
            if (http_code == 200 && res != CURLE_ABORTED_BY_CALLBACK) {
                ClearScreen(4, 26);
                CTOS_LCDTPrintXY(4, 6, " Successful");
                CTOS_KBDGet(&key);
                CTOS_Delay(3000);
                validation = 1;
                curl_easy_cleanup(curl);
                return 1;
            } else if (http_code == 401) {
                ClearScreen(4, 26);
                CTOS_LCDTPrintXY(4, 6, " Wrong PIN");
                CTOS_KBDGet(&key);
                CTOS_Delay(3000);
                validation = 0;
                curl_easy_cleanup(curl);
                return 0;
            }
            CTOS_LCDTPrintXY(3, 4, " Failed");
            CTOS_KBDGet(&key);
            return;
            curl_easy_cleanup(curl);
        }

    }


}



int deposit_post(BYTE pin[4], BYTE amount[20], BYTE recipientcardtype[255], BYTE receipentcardno[17], BYTE recipientCardexpiry_date[6],BYTE merchanpin[4],BYTE merchancardtype[255], BYTE merchancardno[17], BYTE merchanCardexpiry_date[6]) {

    BYTE key;
    BYTE sBuf[128];

    {
        cJSON *root, *fmt, *img, *thm, *fld;
        int i; /* declare a few. */



        //build json object-string
        root = cJSON_CreateObject();
        cJSON_AddItemToObject(root, "username", cJSON_CreateString("familypos"));
        cJSON_AddItemToObject(root, "password", cJSON_CreateString("password"));
        cJSON_AddItemToObject(root, "merchantCardNo", cJSON_CreateString(merchancardno));
        cJSON_AddItemToObject(root, "merchantCVV", cJSON_CreateString("343"));
        cJSON_AddItemToObject(root, "merchantcard_type", cJSON_CreateString(merchancardtype));
        cJSON_AddItemToObject(root, "merchantCardexpiry_date", cJSON_CreateString(merchanCardexpiry_date));        
        cJSON_AddItemToObject(root, "recipientCVV", cJSON_CreateString("445"));
        cJSON_AddItemToObject(root, "recipientCardNo", cJSON_CreateString(receipentcardno));
        cJSON_AddItemToObject(root, "amount", cJSON_CreateString(amount));
        cJSON_AddItemToObject(root, "accountnumber", cJSON_CreateString("35325235252"));
        cJSON_AddItemToObject(root, "merchantPin", cJSON_CreateString(merchanpin));
        cJSON_AddItemToObject(root, "recipientPin", cJSON_CreateString(pin));
        cJSON_AddItemToObject(root, "recipientcard_type", cJSON_CreateString(recipientcardtype));
        cJSON_AddItemToObject(root, "recipientCardexpiry_date", cJSON_CreateString(recipientCardexpiry_date));
        cJSON_AddItemToObject(root, "auth_code", cJSON_CreateString("3866647702"));



        jsonout = cJSON_Print(root);
        cJSON_Delete(root); /*printf("%s\n",jsonout);	free(jsonout);	/* Print to text, Delete the cJSON, print it, release the string. */


        CURL *curl;
        CURLcode res;
        ClearScreen(4, 26);
        CTOS_LCDTPrintXY(3, 4, "Sending........");

        curl = curl_easy_init();
        if (curl) {
            //char* jsonObj = "{ \"pin\" : \"4444\" , \"amount\" :\"10000\", \"card_number\" : \"72828276766262\" }"; 
            curl_easy_setopt(curl, CURLOPT_URL, "http://196.216.73.150:9990/familypos/request/bankDeposit");
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
            /* Check for errors in the request*/
            //CTOS_LCDTPrintXY(1, 1,jsonout);
            //CTOS_KBDGet(&key);
            long http_code = 0;
            curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_code);
            if (http_code == 200 && res != CURLE_ABORTED_BY_CALLBACK) {
                ClearScreen(4, 26);
                CTOS_LCDTPrintXY(4, 6, " Successful");
                CTOS_KBDGet(&key);
                CTOS_Delay(3000);
                validation = 1;
                curl_easy_cleanup(curl);
                ClearScreen(4, 26);
                return 1;
            } else if (http_code == 401) {
                ClearScreen(4, 26);
                CTOS_LCDTPrintXY(4, 6, " Wrong PIN");
                CTOS_KBDGet(&key);
                CTOS_Delay(3000);
                validation = 0;
                curl_easy_cleanup(curl);
                ClearScreen(4, 26);
                return 0;
            }
            CTOS_LCDTPrintXY(3, 4, " Failed");
            curl_easy_cleanup(curl);
            CTOS_KBDGet(&key);
            ClearScreen(4, 26);
            return;
        }

    }


}

int cardles_curlpostmain(BYTE cdepg_baInputAmt[5], BYTE cdepositaccnt[15],BYTE cdmpin[4],BYTE cdmcardvendor[255],BYTE cdmcardnumber[17],BYTE cdmexpdate[]) {

    BYTE key;
    BYTE sBuf[128];

    {
        cJSON *root, *fmt, *img, *thm, *fld;
        int i; /* declare a few. */



        //build json object-string
        root = cJSON_CreateObject();
        cJSON_AddItemToObject(root, "username", cJSON_CreateString("familypos"));
        cJSON_AddItemToObject(root, "password", cJSON_CreateString("password"));
        cJSON_AddItemToObject(root, "amount", cJSON_CreateString(cdepg_baInputAmt));
        cJSON_AddItemToObject(root, "merchantPin", cJSON_CreateString(cdmpin));
        cJSON_AddItemToObject(root, "receipientAccNo", cJSON_CreateString(cdepositaccnt));
        cJSON_AddItemToObject(root, "merchantCardNo", cJSON_CreateString(cdmcardnumber));
        cJSON_AddItemToObject(root, "merchantCVV", cJSON_CreateString("343"));
        cJSON_AddItemToObject(root, "merchantcard_type", cJSON_CreateString(cdmcardvendor));
        cJSON_AddItemToObject(root, "merchantCardexpiry_date", cJSON_CreateString(cdmexpdate));


        jsonout = cJSON_Print(root);
        cJSON_Delete(root); /*printf("%s\n",jsonout);	free(jsonout);	/* Print to text, Delete the cJSON, print it, release the string. */


        CURL *curl;
        CURLcode res;
        ClearScreen(4, 26);
        CTOS_LCDTPrintXY(3, 4, "Sending........");

        curl = curl_easy_init();
        if (curl) {
            //char* jsonObj = "{ \"pin\" : \"4444\" , \"amount\" :\"10000\", \"card_number\" : \"72828276766262\" }"; 
            curl_easy_setopt(curl, CURLOPT_URL, "http://196.216.73.150:9990/familypos/request/bankDeposit");
            struct curl_slist *headers = NULL;
            headers = curl_slist_append(headers, "Accept: application/json");
            headers = curl_slist_append(headers, "Content-Type: application/json");
            headers = curl_slist_append(headers, "charsets: utf-8");
            /* example.com is redirected, so we tell libcurl to follow redirection */
            curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);

            /* Perform the request, res will get the return code */
            curl_easy_setopt(curl, CURLOPT_POSTFIELDS, jsonout);
            //res = curl_easy_perform(curl);
            //CTOS_LCDTPrintXY(4, 4, " Response is");
            /* Check for errors */
            long http_code = 0;
            curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_code);
            if (http_code == 200 && res != CURLE_ABORTED_BY_CALLBACK) {
                ClearScreen(4, 26);
                CTOS_LCDTPrintXY(4, 6, " Successful");
                CTOS_KBDGet(&key);
                CTOS_Delay(3000);
                cardles_validation = 1;
                curl_easy_cleanup(curl);
                return 1;
            } else if (http_code == 401) {
                ClearScreen(4, 26);
                CTOS_LCDTPrintXY(4, 6, " Wrong PIN");
                CTOS_KBDGet(&key);
                CTOS_Delay(3000);
                cardles_validation = 0;
                curl_easy_cleanup(curl);
                return 0;
            }
            CTOS_LCDTPrintXY(3, 4, " Failed");
            CTOS_KBDGet(&key);
            return;
            curl_easy_cleanup(curl);
        }

    }


}

int ballance_post(BYTE pin[4], BYTE recipientcardtype[255], BYTE receipentcardno[17], BYTE recipientCardexpiry_date[6]) {

    BYTE key;
    BYTE sBuf[128];
    int accntvalidation = 0;
    {
        cJSON *root, *fmt, *img, *thm, *fld;
        int i; /* declare a few. */


        //api_username, api_password, cardNumber, cardPin, CardCVV, CardExpiryDate
        //build json object-string
        root = cJSON_CreateObject();
        cJSON_AddItemToObject(root, "username", cJSON_CreateString("familypos"));
        cJSON_AddItemToObject(root, "password", cJSON_CreateString("password"));
        cJSON_AddItemToObject(root, "cardPin", cJSON_CreateString(pin));
        cJSON_AddItemToObject(root, "CardCVV", cJSON_CreateString("343"));
        cJSON_AddItemToObject(root, "cardNumber", cJSON_CreateString(receipentcardno));
        cJSON_AddItemToObject(root, "CardExpiryDate", cJSON_CreateString(recipientCardexpiry_date));


        jsonout = cJSON_Print(root);
        cJSON_Delete(root); /*printf("%s\n",jsonout);	free(jsonout);	/* Print to text, Delete the cJSON, print it, release the string. */


        CURL *curl;
        CURLcode res;
        ClearScreen(4, 26);
        CTOS_LCDTPrintXY(3, 4, "Sending........");

        curl = curl_easy_init();
        if (curl) {
            struct string1 p;
            init_string1(&p);

            curl_easy_setopt(curl, CURLOPT_URL, "http://196.216.73.150:9990/familypos/request/checkUserBalance");
            struct curl_slist *headers = NULL;
            headers = curl_slist_append(headers, "Accept: application/json");
            headers = curl_slist_append(headers, "Content-Type: application/json");
            headers = curl_slist_append(headers, "charsets: utf-8");
            /* example.com is redirected, so we tell libcurl to follow redirection */
            curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
            curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writefunc1);
            curl_easy_setopt(curl, CURLOPT_WRITEDATA, &p);
            curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);

            /* Perform the request, res will get the return code */
            curl_easy_setopt(curl, CURLOPT_POSTFIELDS, jsonout);
            res = curl_easy_perform(curl);

            //parse json object;


            long http_code = 0;
            curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_code);
            if (http_code == 200 && res != CURLE_ABORTED_BY_CALLBACK) {
                ClearScreen(4, 26);
                char *jsonresponse;
                jsonresponse = malloc(sizeof (char) * strlen(p.ptr));
                strcpy(jsonresponse, p.ptr);
                account_doit(jsonresponse);
                cJSON * root = cJSON_Parse(p.ptr);
                responseExitCode = cJSON_GetObjectItem(root, "status_code")->valuestring;
                message = cJSON_GetObjectItem(root, "message")->valuestring;
                retballance = cJSON_GetObjectItem(root, "balance")->valuestring;
                ClearScreen(4, 26);
                CTOS_LCDTPrintXY(4, 6, " Account Ball is:");
                CTOS_LCDTPrintXY(4, 7, " KSH: ");
                CTOS_LCDTPrintXY(9, 7, retballance);
                CTOS_KBDGet(&key);
                printbal(retballance);
                accntvalidation = 1;
                free(p.ptr);
                curl_easy_cleanup(curl);
                return 1;
            } else if (http_code == 401) {
                ClearScreen(4, 26);
                CTOS_LCDTPrintXY(4, 6, " Wrong PIN");
                CTOS_KBDGet(&key);
                CTOS_Delay(3000);
                accntvalidation = 0;
                curl_easy_cleanup(curl);
                return 0;
            }
            CTOS_LCDTPrintXY(3, 4, " Failed");
            CTOS_KBDGet(&key);
            return;
            curl_easy_cleanup(curl);
        }

    }


}


int agentballance_post(BYTE pin[4], BYTE recipientcardtype[255], BYTE receipentcardno[17], BYTE recipientCardexpiry_date[6]) {

    BYTE key;
    BYTE sBuf[128];
    int accntvalidation = 0;
    {
        cJSON *root, *fmt, *img, *thm, *fld;
        int i; /* declare a few. */


        //api_username, api_password, cardNumber, cardPin, CardCVV, CardExpiryDate
        //build json object-string
        root = cJSON_CreateObject();
        cJSON_AddItemToObject(root, "username", cJSON_CreateString("familypos"));
        cJSON_AddItemToObject(root, "password", cJSON_CreateString("password"));
        cJSON_AddItemToObject(root, "cardPin", cJSON_CreateString(pin));
        cJSON_AddItemToObject(root, "CardCVV", cJSON_CreateString("343"));
        cJSON_AddItemToObject(root, "cardNumber", cJSON_CreateString(receipentcardno));
        cJSON_AddItemToObject(root, "CardExpiryDate", cJSON_CreateString(recipientCardexpiry_date));


        jsonout = cJSON_Print(root);
        cJSON_Delete(root); /*printf("%s\n",jsonout);	free(jsonout);	/* Print to text, Delete the cJSON, print it, release the string. */


        CURL *curl;
        CURLcode res;
        ClearScreen(4, 26);
        CTOS_LCDTPrintXY(3, 4, "Sending........");

        curl = curl_easy_init();
        if (curl) {
            struct string1 p;
            init_string1(&p);

            curl_easy_setopt(curl, CURLOPT_URL, "http://196.216.73.150:9990/familypos/request/checkMerchantBalance");
            struct curl_slist *headers = NULL;
            headers = curl_slist_append(headers, "Accept: application/json");
            headers = curl_slist_append(headers, "Content-Type: application/json");
            headers = curl_slist_append(headers, "charsets: utf-8");
            /* example.com is redirected, so we tell libcurl to follow redirection */
            curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
            curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writefunc1);
            curl_easy_setopt(curl, CURLOPT_WRITEDATA, &p);
            curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);

            /* Perform the request, res will get the return code */
            curl_easy_setopt(curl, CURLOPT_POSTFIELDS, jsonout);
            res = curl_easy_perform(curl);

            //parse json object;


            long http_code = 0;
            curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_code);
            if (http_code == 200 && res != CURLE_ABORTED_BY_CALLBACK) {
                ClearScreen(4, 26);
                char *jsonresponse;
                jsonresponse = malloc(sizeof (char) * strlen(p.ptr));
                strcpy(jsonresponse, p.ptr);
                account_doit(jsonresponse);
                cJSON * root = cJSON_Parse(p.ptr);
                responseExitCode = cJSON_GetObjectItem(root, "status_code")->valuestring;
                message = cJSON_GetObjectItem(root, "message")->valuestring;
                retballance = cJSON_GetObjectItem(root, "balance")->valuestring;
                ClearScreen(4, 26);
                CTOS_LCDTPrintXY(4, 6, " Account Ball is:");
                CTOS_LCDTPrintXY(4, 7, " KSH: ");
                CTOS_LCDTPrintXY(9, 7, retballance);
                CTOS_KBDGet(&key);
                agentprintbal(retballance);
                accntvalidation = 1;
                free(p.ptr);
                curl_easy_cleanup(curl);
                return 1;
            } else if (http_code == 401) {
                ClearScreen(4, 26);
                CTOS_LCDTPrintXY(4, 6, " Wrong PIN");
                CTOS_KBDGet(&key);
                CTOS_Delay(3000);
                accntvalidation = 0;
                curl_easy_cleanup(curl);
                return 0;
            }
            CTOS_LCDTPrintXY(3, 4, " Failed");
            CTOS_KBDGet(&key);
            return;
            curl_easy_cleanup(curl);
        }

    }


}


int statement_post(BYTE pin[4], BYTE recipientcardtype[255], BYTE receipentcardno[17], BYTE recipientCardexpiry_date[6]) {

    BYTE key;
    BYTE sBuf[128];
    int accntvalidation = 0;
    {
        cJSON *root, *fmt, *img, *thm, *fld;
        int i; /* declare a few. */


        //api_username, api_password, cardNumber, cardPin, CardCVV, CardExpiryDate
        //build json object-string
        root = cJSON_CreateObject();
        cJSON_AddItemToObject(root, "username", cJSON_CreateString("familypos"));
        cJSON_AddItemToObject(root, "password", cJSON_CreateString("password"));
        cJSON_AddItemToObject(root, "cardPin", cJSON_CreateString(pin));
        cJSON_AddItemToObject(root, "CardCVV", cJSON_CreateString("343"));
        cJSON_AddItemToObject(root, "cardNumber", cJSON_CreateString(receipentcardno));
        cJSON_AddItemToObject(root, "CardExpiryDate", cJSON_CreateString(recipientCardexpiry_date));


        jsonout = cJSON_Print(root);
        cJSON_Delete(root); /*printf("%s\n",jsonout);	free(jsonout);	/* Print to text, Delete the cJSON, print it, release the string. */


        CURL *curl;
        CURLcode res;
        ClearScreen(4, 26);
        CTOS_LCDTPrintXY(3, 4, "Sending........");

        curl = curl_easy_init();
        if (curl) {
            struct string1 p;
            init_string1(&p);

            curl_easy_setopt(curl, CURLOPT_URL, "http://196.216.73.150:9990/familypos/request/checkUserBalance");
            struct curl_slist *headers = NULL;
            headers = curl_slist_append(headers, "Accept: application/json");
            headers = curl_slist_append(headers, "Content-Type: application/json");
            headers = curl_slist_append(headers, "charsets: utf-8");
            /* example.com is redirected, so we tell libcurl to follow redirection */
            curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
            curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writefunc1);
            curl_easy_setopt(curl, CURLOPT_WRITEDATA, &p);
            curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);

            /* Perform the request, res will get the return code */
            curl_easy_setopt(curl, CURLOPT_POSTFIELDS, jsonout);
            res = curl_easy_perform(curl);

            //parse json object;


            long http_code = 0;
            curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_code);
            if (http_code == 200 && res != CURLE_ABORTED_BY_CALLBACK) {
                ClearScreen(4, 26);
                char *jsonresponse;
                jsonresponse = malloc(sizeof (char) * strlen(p.ptr));
                strcpy(jsonresponse, p.ptr);
                account_doit(jsonresponse);
                cJSON * root = cJSON_Parse(p.ptr);
                responseExitCode = cJSON_GetObjectItem(root, "status_code")->valuestring;
                message = cJSON_GetObjectItem(root, "message")->valuestring;
                retballance = cJSON_GetObjectItem(root, "balance")->valuestring;
                ClearScreen(4, 26);
                CTOS_LCDTPrintXY(2, 6, "Trans: Amount-KSH: Status");
                CTOS_LCDTPrintXY(2, 7, "1                  Success");
                CTOS_LCDTPrintXY(9, 7, retballance);
                CTOS_KBDGet(&key);
                printstatement(retballance);
                accntvalidation = 1;
                free(p.ptr);
                curl_easy_cleanup(curl);
                return 1;
            } else if (http_code == 401) {
                ClearScreen(4, 26);
                CTOS_LCDTPrintXY(4, 6, " Wrong PIN");
                CTOS_KBDGet(&key);
                CTOS_Delay(3000);
                accntvalidation = 0;
                curl_easy_cleanup(curl);
                return 0;
            }
            CTOS_LCDTPrintXY(3, 6, " Failed");
            CTOS_KBDGet(&key);
            return;
            curl_easy_cleanup(curl);
        }

    }


}

int setpin_post(BYTE pin[4], BYTE recipientcardtype[255], BYTE receipentcardno[17], BYTE recipientCardexpiry_date[6],BYTE tmpnewpin[4]) {

    BYTE key;
    BYTE sBuf[128];
    int accntvalidation = 0;
    {
        cJSON *root, *fmt, *img, *thm, *fld;
        int i; /* declare a few. */


        //api_username, api_password, cardNumber, cardPin, CardCVV, CardExpiryDate
        //build json object-string
        root = cJSON_CreateObject();
        cJSON_AddItemToObject(root, "username", cJSON_CreateString("familypos"));
        cJSON_AddItemToObject(root, "password", cJSON_CreateString("password"));
        cJSON_AddItemToObject(root, "cardPin", cJSON_CreateString(pin));
        cJSON_AddItemToObject(root, "CardCVV", cJSON_CreateString("343"));
        cJSON_AddItemToObject(root, "cardNumber", cJSON_CreateString(receipentcardno));
        cJSON_AddItemToObject(root, "CardExpiryDate", cJSON_CreateString(recipientCardexpiry_date));
        cJSON_AddItemToObject(root, "cardNumber", cJSON_CreateString(receipentcardno));
        cJSON_AddItemToObject(root, "newPin", cJSON_CreateString(tmpnewpin));

        jsonout = cJSON_Print(root);
        cJSON_Delete(root); /*printf("%s\n",jsonout);	free(jsonout);	/* Print to text, Delete the cJSON, print it, release the string. */


        CURL *curl;
        CURLcode res;
        ClearScreen(4, 26);
        CTOS_LCDTPrintXY(3, 5, "Sending........");

        curl = curl_easy_init();
        if (curl) {
            struct string1 p;
            init_string1(&p);

            curl_easy_setopt(curl, CURLOPT_URL, "http://196.216.73.150:9990/familypos/request/checkUserBalance");
            struct curl_slist *headers = NULL;
            headers = curl_slist_append(headers, "Accept: application/json");
            headers = curl_slist_append(headers, "Content-Type: application/json");
            headers = curl_slist_append(headers, "charsets: utf-8");
            /* example.com is redirected, so we tell libcurl to follow redirection */
            curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
            curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writefunc1);
            curl_easy_setopt(curl, CURLOPT_WRITEDATA, &p);
            curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);

            /* Perform the request, res will get the return code */
            curl_easy_setopt(curl, CURLOPT_POSTFIELDS, jsonout);
            res = curl_easy_perform(curl);

            //parse json object;


            long http_code = 0;
            curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_code);
            if (http_code == 200 && res != CURLE_ABORTED_BY_CALLBACK) {
                ClearScreen(4, 26);
                char *jsonresponse;
                jsonresponse = malloc(sizeof (char) * strlen(p.ptr));
                strcpy(jsonresponse, p.ptr);
                account_doit(jsonresponse);
                cJSON * root = cJSON_Parse(p.ptr);
                message = cJSON_GetObjectItem(root, "message")->valuestring;
                CTOS_LCDTPrintXY(3, 7, message);
                CTOS_KBDGet(&key);
                accntvalidation = 1;
                free(p.ptr);
                curl_easy_cleanup(curl);
                return 1;
            } else if (http_code == 401) {
                ClearScreen(4, 26);
                CTOS_LCDTPrintXY(4, 6, " Wrong PIN");
                CTOS_KBDGet(&key);
                CTOS_Delay(3000);
                accntvalidation = 0;
                curl_easy_cleanup(curl);
                return 0;
            }
            CTOS_LCDTPrintXY(3, 6, " Failed");
            CTOS_KBDGet(&key);
            return;
            curl_easy_cleanup(curl);
        }

    }


}

int paybill_post(BYTE pin[4],BYTE tmamount[20], BYTE recipientcardtype[255], BYTE receipentcardno[17], BYTE recipientCardexpiry_date[6],BYTE tmpaybill[9],BYTE tmcountycode[9]) {

    BYTE key;
    BYTE sBuf[128];
    int accntvalidation = 0;
    {
        cJSON *root, *fmt, *img, *thm, *fld;
        int i; /* declare a few. */


        //api_username, api_password, cardNumber, cardPin, CardCVV, CardExpiryDate
        //build json object-string
        root = cJSON_CreateObject();
        cJSON_AddItemToObject(root, "username", cJSON_CreateString("familypos"));
        cJSON_AddItemToObject(root, "password", cJSON_CreateString("password"));
        cJSON_AddItemToObject(root, "cardPin", cJSON_CreateString(pin));
        cJSON_AddItemToObject(root, "CardCVV", cJSON_CreateString("343"));
        cJSON_AddItemToObject(root, "cardNumber", cJSON_CreateString(receipentcardno));
        cJSON_AddItemToObject(root, "CardExpiryDate", cJSON_CreateString(recipientCardexpiry_date));
        cJSON_AddItemToObject(root, "countyCode", cJSON_CreateString(tmcountycode));
        cJSON_AddItemToObject(root, "revenueCode", cJSON_CreateString(tmpaybill));
        cJSON_AddItemToObject(root, "amount", cJSON_CreateString(tmamount));

        jsonout = cJSON_Print(root);
        cJSON_Delete(root); /*printf("%s\n",jsonout);	free(jsonout);	/* Print to text, Delete the cJSON, print it, release the string. */


        CURL *curl;
        CURLcode res;
        ClearScreen(4, 26);
        CTOS_LCDTPrintXY(3, 5, "Sending........");

        curl = curl_easy_init();
        if (curl) {
            struct string1 p;
            init_string1(&p);

            curl_easy_setopt(curl, CURLOPT_URL, "http://196.216.73.150:9990/familypos/request/revenueCollectionCard");
            struct curl_slist *headers = NULL;
            headers = curl_slist_append(headers, "Accept: application/json");
            headers = curl_slist_append(headers, "Content-Type: application/json");
            headers = curl_slist_append(headers, "charsets: utf-8");
            /* example.com is redirected, so we tell libcurl to follow redirection */
            curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
            curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writefunc1);
            curl_easy_setopt(curl, CURLOPT_WRITEDATA, &p);
            curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);

            /* Perform the request, res will get the return code */
            curl_easy_setopt(curl, CURLOPT_POSTFIELDS, jsonout);
            res = curl_easy_perform(curl);

            //parse json object;


            long http_code = 0;
            curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_code);
            if (http_code == 200 && res != CURLE_ABORTED_BY_CALLBACK) {
                ClearScreen(4, 26);
                char *jsonresponse;
                jsonresponse = malloc(sizeof (char) * strlen(p.ptr));
                strcpy(jsonresponse, p.ptr);
                account_doit(jsonresponse);
                cJSON * root = cJSON_Parse(p.ptr);
                message = cJSON_GetObjectItem(root, "message")->valuestring;
                CTOS_LCDTPrintXY(3, 7, message);
                CTOS_KBDGet(&key);
                accntvalidation = 1;
                free(p.ptr);
                curl_easy_cleanup(curl);
                return 1;
            } else if (http_code == 401) {
                ClearScreen(4, 26);
                CTOS_LCDTPrintXY(4, 6, " Wrong PIN");
                CTOS_KBDGet(&key);
                CTOS_Delay(3000);
                accntvalidation = 0;
                curl_easy_cleanup(curl);
                return 0;
            }
            CTOS_LCDTPrintXY(3, 6, " Failed");
            CTOS_KBDGet(&key);
            return;
            curl_easy_cleanup(curl);
        }

    }


}


int utility_post(BYTE pin[4],BYTE tmamount[20], BYTE recipientcardtype[255], BYTE receipentcardno[17], BYTE recipientCardexpiry_date[6],BYTE tmputilityNo[9],BYTE tmputilservice[10],BYTE tmutilCode[10]) {

    BYTE key;
    BYTE sBuf[128];
    int accntvalidation = 0;
    {
        cJSON *root, *fmt, *img, *thm, *fld;
        int i; /* declare a few. */


        //cardNumber, cardPin, CardCVV, CardExpiryDate, utilityCode, utilityService, amount, utilityNumber
        //build json object-string
        root = cJSON_CreateObject();
         cJSON_AddItemToObject(root, "username", cJSON_CreateString("familypos"));
        cJSON_AddItemToObject(root, "password", cJSON_CreateString("password"));
        cJSON_AddItemToObject(root, "cardPin", cJSON_CreateString(pin));
        cJSON_AddItemToObject(root, "CardCVV", cJSON_CreateString("343"));
        cJSON_AddItemToObject(root, "cardNumber", cJSON_CreateString(receipentcardno));
        cJSON_AddItemToObject(root, "CardExpiryDate", cJSON_CreateString(recipientCardexpiry_date));
        cJSON_AddItemToObject(root, "utilityCode", cJSON_CreateString(tmutilCode));
        cJSON_AddItemToObject(root, "utilityService", cJSON_CreateString(tmputilservice));
        cJSON_AddItemToObject(root, "utilityNumber", cJSON_CreateString(tmputilityNo));
        cJSON_AddItemToObject(root, "amount", cJSON_CreateString(tmamount));

        jsonout = cJSON_Print(root);
        cJSON_Delete(root); /*printf("%s\n",jsonout);	free(jsonout);	/* Print to text, Delete the cJSON, print it, release the string. */


        CURL *curl;
        CURLcode res;
        ClearScreen(4, 26);
        CTOS_LCDTPrintXY(3, 5, "Sending........");

        curl = curl_easy_init();
        if (curl) {
            struct string1 p;
            init_string1(&p);

            curl_easy_setopt(curl, CURLOPT_URL, "http://196.216.73.150:9990/familypos/request/utilityPaymentCard");
            struct curl_slist *headers = NULL;
            headers = curl_slist_append(headers, "Accept: application/json");
            headers = curl_slist_append(headers, "Content-Type: application/json");
            headers = curl_slist_append(headers, "charsets: utf-8");
            /* example.com is redirected, so we tell libcurl to follow redirection */
            curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
            curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writefunc1);
            curl_easy_setopt(curl, CURLOPT_WRITEDATA, &p);
            curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);

            /* Perform the request, res will get the return code */
            curl_easy_setopt(curl, CURLOPT_POSTFIELDS, jsonout);
            res = curl_easy_perform(curl);

            //parse json object;


            long http_code = 0;
            curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_code);
            if (http_code == 200 && res != CURLE_ABORTED_BY_CALLBACK) {
                ClearScreen(4, 26);
                char *jsonresponse;
                jsonresponse = malloc(sizeof (char) * strlen(p.ptr));
                strcpy(jsonresponse, p.ptr);
                account_doit(jsonresponse);
                cJSON * root = cJSON_Parse(p.ptr);
                message = cJSON_GetObjectItem(root, "message")->valuestring;
                CTOS_LCDTPrintXY(3, 7, message);
                CTOS_KBDGet(&key);
                accntvalidation = 1;
                free(p.ptr);
                curl_easy_cleanup(curl);
                return 1;
            } else if (http_code == 401) {
                ClearScreen(4, 26);
                CTOS_LCDTPrintXY(4, 6, " Wrong PIN");
                CTOS_KBDGet(&key);
                CTOS_Delay(3000);
                accntvalidation = 0;
                curl_easy_cleanup(curl);
                return 0;
            }
            CTOS_LCDTPrintXY(3, 6, " Failed");
            CTOS_KBDGet(&key);
            return;
            curl_easy_cleanup(curl);
        }

    }


}