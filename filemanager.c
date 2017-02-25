/*========================================*
 * I N C L U D E S *
 *========================================*/
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
/** These two files are necessary for calling CTOS API **/
#include <ctosapi.h>

/*========================================*
 * D E F I N E S *
 *========================================*/
#define d_BUFF_SIZE 128 //Buffer Size
#define d_LINE_DOT  12      //A line is 12 dots in the paper

/* Define Error Code*/
#define d_FILE_EXIST 0x01
#define d_FILE_NO_FILES 0x02
#define d_FILE_DIR_OK 0x03
#define d_FILE_CANCEL 0x04
#define d_FILE_LENGTH_SHORT 0x05

/*========================================*
 *GLOBAL VARIABLES *
 *========================================*/
BYTE key;
USHORT ret;
USHORT usTotal;
BYTE babuff[d_BUFF_SIZE];
BYTE baFileBuff[d_BUFF_SIZE];

/* Define Key Mode Option */
typedef enum {
    eMain,
    eNewFile,
    eDelFile,
    eDirFile,
    eWrite,
    eRead,
    eRename,
    eMainPage2,
    eSetAttrib
} emKeyMode;
emKeyMode emKM;

/*==================================================================
 * FUNCTION NAME: show_screen
 * DESCRIPTION:Show on the LCD screen
 * RETURN:none.
 * NOTES:none.
 * ================================================================ */
void show_screen(int tag) {
    CTOS_LCDTClearDisplay();
    switch (tag) {
        case 0:
            emKM = eMain;
            CTOS_LCDTPrintXY(1, 1, "\fr FILE SYSTEM \fn");
            CTOS_LCDTPrintXY(1, 2, "1.New File");
            CTOS_LCDTPrintXY(1, 3, "2.Del File");
            CTOS_LCDTPrintXY(1, 4, "3.Directory");
            CTOS_LCDTPrintXY(1, 5, "4.Read");
            CTOS_LCDTPrintXY(1, 6, "5.Write");
            CTOS_LCDTPrintXY(1, 7, "6.Rename File");
            CTOS_LCDTPrintXY(1, 8, "'DOWN'->NextPage");
            break;
        case 1:
            emKM = eNewFile;
            CTOS_LCDTPrintXY(1, 1, "\fr STORAGE TYPE \fn");
            CTOS_LCDTPrintXY(1, 2, "1.Flash");
            break;
        case 2:
            CTOS_LCDTPrintXY(1, 1, "\fr DELETE FILE \fn");
            break;
        case 3:
            CTOS_LCDTPrintXY(1, 1, "\fr DIRETCORY \fn");
            break;
        case 4:
            CTOS_LCDTPrintXY(1, 1, "\fr WRITE FILE \fn");
            break;
        case 5:
            CTOS_LCDTPrintXY(1, 1, "\fr READ FILE \fn");
            break;
        case 6:
            CTOS_LCDTPrintXY(1, 1, "\fr RENAME FILE \fn");
            break;
        case 7:
            emKM = eMainPage2;
            CTOS_LCDTPrintXY(1, 1, "\fr FILE SYSTEM \fn");
            CTOS_LCDTPrintXY(1, 2, "1.Cut Content");
            CTOS_LCDTPrintXY(1, 3, "2.Set Attrib");
            CTOS_LCDTPrintXY(1, 4, "3.Get Attrib");
            CTOS_LCDTPrintXY(1, 5, "4.Dir Attrib");
            CTOS_LCDTPrintXY(1, 6, "5.Format");
            CTOS_LCDTPrintXY(1, 7, "6.Disk Free Size");
            CTOS_LCDTPrintXY(1, 8, "'UP'->PrevPage");
            break;
        case 8:
            CTOS_LCDTPrintXY(1, 1, "\fr SET ATTRIB \fn");
            CTOS_LCDTPrintXY(1, 2, "1.Private");
            CTOS_LCDTPrintXY(1, 3, "2.Public");
            break;
        case 9:
            CTOS_LCDTPrintXY(1, 1, "\fr SET ATTRIB \fn");
            break;
        case 10:
            CTOS_LCDTPrintXY(1, 1, "\fr GET ATTRIB \fn");
            break;
        case 11:
            CTOS_LCDTPrintXY(1, 1, "\fr FORMAT \fn");
            CTOS_LCDTPrintXY(1, 3, "Note!!");
            CTOS_LCDTPrintXY(1, 4, "Files will Erase!!");
            CTOS_LCDTPrintXY(1, 8, "Format->'OK' ?");
            break;
        case 12:
            CTOS_LCDTPrintXY(1, 1, "\fr Cut Content \fn");
            break;
        case 13:
            CTOS_LCDTPrintXY(1, 1, "\fr Disk Free Size \fn");
            break;
    }
}

/* ==================================================================
 * FUNCTION NAME: show_errormsg
 * DESCRIPTION: Show error message on the LCD screen
 * RETURN: none.
 * NOTES: none.
 * ================================================================ */
void show_errormsg(USHORT ret) {
    switch (ret) {
        case d_OK:
        case d_USER_CANCEL:
        case d_FILE_DIR_OK:
            break;
        case d_FILE_EXIST:
            CTOS_LCDTPrintXY(1, 8, "File Exist ");
            break;
        case d_FILE_LENGTH_SHORT:
            CTOS_LCDTPrintXY(1, 8, "Length<20bytes");
            break;
        case d_FILE_NO_FILES:
            CTOS_LCDTPrintXY(1, 8, "No Any Files ");
            break;
        case d_FS_INVALID_PARAMETER:
            CTOS_LCDTPrintXY(1, 8, "Invalid Parameter");
            break;
        case d_FS_FHT_FULL:
            CTOS_LCDTPrintXY(1, 8, "FHT Full ");
            break;
        case d_FS_FILE_ALREADY_OPENED:
            CTOS_LCDTPrintXY(1, 8, "Already Opened ");
            break;
        case d_FS_FILE_NOT_OPENED:
            CTOS_LCDTPrintXY(1, 8, "Not Opened ");
            break;
        case d_FS_FILE_NOT_FOUND:
            CTOS_LCDTPrintXY(1, 8, "File Not Found ");
            break;
        case d_FS_FILE_INVALID_HANDLE:
            CTOS_LCDTPrintXY(1, 8, "Invalid handle ");
            break;
        case d_FS_DATA_FULL:
            CTOS_LCDTPrintXY(1, 8, "Data Full ");
            break;
        case d_FS_NOT_INITIALED:
            CTOS_LCDTPrintXY(1, 8, "Not Initialed ");
            break;
        case d_FS_CHECKSUM_ERROR:
            CTOS_LCDTPrintXY(1, 8, "Checksum Error ");
            break;
        case d_FS_FILE_ALREADY_EXISTED:
            CTOS_LCDTPrintXY(1, 8, "Already Existed ");
            break;
        case d_FS_NOT_OWNER:
            CTOS_LCDTPrintXY(1, 8, "Not Owner ");
            break;
        default:
            CTOS_LCDTPrintXY(1, 8, "Other Error !! ");
    }
}

/*==================================================================      
 * FUNCTION NAME: NewFile                                                 
 * DESCRIPTION:   Create a new file in the Flash.  (*NOT* support SRAM type)
 * RETURN:        d_FILE_EXIST --> File already is existed   
 *                ret --> Return other messages                             
 * NOTES:         none.                                              
 * ================================================================ */
USHORT NewFile(BYTE *pbaFileName, BYTE bStorageType) {
    
    ULONG ulFileSize = 0, ulHandle;

    /* Get the file size with specific file name.
     * If Get file size > 0, the file is already existed      */
    ret = CTOS_FileGetSize(pbaFileName, &ulFileSize);

    if (ulFileSize > 0)
        return d_FILE_EXIST;

    if ((ret != d_OK) && (ret != d_FS_FILE_NOT_FOUND))
        return ret;

    /* Open a file and return a number called a file handle.
     * If the specified file name does not exist , it will be created first. */
    ret = CTOS_FileOpen(pbaFileName, bStorageType, &ulHandle);

    if (ret == d_OK)
        CTOS_FileClose(ulHandle);

    return ret;
}

/* ==========================================================================
 * FUNCTION NAME: DirectoryFile
 * DESCRIPTION: List all created files by the same application
 * RETURN: d_FILE_DIR_OK --> Search files success
 * d_FILE_NO_FILES --> No any files found
 * NOTES: none.
 * ========================================================================== */
USHORT DirectoryFile(USHORT *pusTotal, BYTE *pbaFileBuff) {
    int i;
    ULONG ulFileSize[256]; //The maximum value is 256
    BYTE baTemp[d_BUFF_SIZE];

    memset(baTemp, 0x00, sizeof (baTemp));

    // List all created files by the same application.
    // pbaFileBuff --> the file names of the files. Each file name occupies 15 bytes
    // ulFileSize  --> the file sizes of the files. it is array of ULONG
    // pusTotal    --> the number of the files created y the same AP
    CTOS_FileDir(pbaFileBuff, ulFileSize, pusTotal);

    if (*pusTotal == 0)
        return d_FILE_NO_FILES;

    /* Show the files on the LCD screen */
    for (i = 1; i <= *pusTotal; i++) {
        sprintf(baTemp, "%d.%s  %d", i, &pbaFileBuff[15 * (i - 1)], (int) ulFileSize[i - 1]);
        CTOS_LCDTPrintXY(1, i + 5, baTemp);
    }
    return d_FILE_DIR_OK;
}

/* ==================================================================                                     
 * FUNCTION NAME: DeleteFile                                                                             
 * DESCRIPTION:   Delete a file with specific file name                                                   
 * RETURN:        d_USER_CANCEL  --> Cancel to delete the file                                      
 *                d_FILE_NO_FILES --> No any files by the same application
 *                d_FS_FILE_NOT_FOUND --> User select wrong file item
 *                ret --> Return other messages                                                           
 * NOTES:         none.                                                                        
 * ================================================================ */
USHORT DeleteFile(void) {
    /* Search & list the files by the same appliction */
    if (DirectoryFile(&usTotal, baFileBuff) != d_FILE_DIR_OK)
        return d_FILE_NO_FILES;

    /* To select one file */
    CTOS_KBDGet(&key);
    if (key == d_KBD_CANCEL)
        return d_USER_CANCEL;

    if ((key - 0x30) > usTotal)
        return d_FS_FILE_NOT_FOUND;

    /* Delete selected file */
    return CTOS_FileDelete(&baFileBuff[15 * (key - 0x31)]);
}

/* ==================================================================                                    
 * FUNCTION NAME: FileRead                                                                              
 * DESCRIPTION: Read data from the opened file
 * RETURN:      d_USER_CANCEL --> Cancel to Read the file
 *              d_FILE_NO_FILES --> No any files by the same application
 *              d_FS_FILE_NOT_FOUND --> User select wrong file item
 *              ret --> Return other messages                                                                
 * NOTES:       none.                                                                             
 * ================================================================ */
USHORT FileRead(BYTE *pcaReadString, ULONG *pulLength) {
    ULONG ulHandle, ulbuffLen;

    memset(pcaReadString, 0x00, *pulLength);

    /* Search & list the files by the same appliction */
    if (DirectoryFile(&usTotal, baFileBuff) != d_FILE_DIR_OK)
        return d_FILE_NO_FILES;

    /* To select one file */
    CTOS_KBDGet(&key);
    if (key == d_KBD_CANCEL)
        return d_USER_CANCEL;
    if ((key - 0x30) > usTotal)
        return d_FS_FILE_NOT_FOUND;

    /* Open the selected file */
    if (strncmp(&baFileBuff[15 * (key - 0x31)], "FL", 2) == 0) {
        /* Reopen a file with specific file name. the file should be created first if not exist */
        ret = CTOS_FileReopen(&baFileBuff[15 * (key - 0x31)], d_STORAGE_FLASH, &ulHandle);
        if (ret != d_OK)
            return ret;
    } else
        return d_FS_FILE_NOT_FOUND;

    /* Read data from this opened file */
    ret = CTOS_FileRead(ulHandle, pcaReadString, pulLength);
    if (ret != d_OK)
        return ret;

    /* Close the opened file */
    CTOS_FileClose(ulHandle);
    CTOS_LCDTClearDisplay();
    return ret;
}

/* ==================================================================                                    
 * FUNCTION NAME: FileWrite
 * DESCRIPTION:   Write data into the opened file
 * RETURN:        d_USER_CANCEL --> Cancel to wirte the file
 *                d_FILE_NO_FILES   --> No any files by the same application
 *                d_FS_FILE_NOT_FOUND --> User select wrong file item
 *                ret --> Return other messages                                                                
 * NOTES:         none.                                                                        
 * ================================================================ */
USHORT FileWrite(BYTE pcaString) {
    ULONG ulHandle, ulPosition;

    /* Search & list the files by the same appliction*/
    if (DirectoryFile(&usTotal, baFileBuff) != d_FILE_DIR_OK)
        return d_FILE_NO_FILES;

    /* To select one file */
    CTOS_KBDGet(&key);
    if (key == d_KBD_CANCEL)
        return d_USER_CANCEL;
    if ((key - 0x30) > usTotal)
        return d_FS_FILE_NOT_FOUND;

    /* Open the selected file */
    if (strncmp(&baFileBuff[15 * (key - 0x31)], "FL", 2) == 0) {
        /*Reopen a file with specific file name. the file should be created first if not exist */
        ret = CTOS_FileReopen(&baFileBuff[15 * (key - 0x31)], d_STORAGE_FLASH, &ulHandle);
        if (ret != d_OK)
            return ret;
    } else {
        return d_FS_FILE_NOT_FOUND;
    }

    /* Move the file pointer to a specific position.
     * Move backward from the end of the file. */
    ret = CTOS_FileSeek(ulHandle, 0, d_SEEK_FROM_EOF);
    if (ret != d_OK)
        return ret;

    CTOS_FileGetPosition(ulHandle, &ulPosition);
    sprintf(babuff, "BPos=%d", (int) ulPosition);
    CTOS_LCDTPrintXY(1, 7, babuff);

    /* Write data into this opened file */
    ret = CTOS_FileWrite(ulHandle, pcaString, strlen(pcaString));
    if (ret != d_OK)
        return ret;

    CTOS_FileGetPosition(ulHandle, &ulPosition);
    sprintf(babuff, "FPos=%d", (int) ulPosition);
    CTOS_LCDTPrintXY(9, 7, babuff);

    /* Close the opened file */
    CTOS_FileClose(ulHandle);
    return ret;
}
USHORT autoFileWrite(BYTE pcaString) {
    ULONG ulHandle, ulPosition;

    /* Search & list the files by the same appliction*/
    if (DirectoryFile(&usTotal, baFileBuff) != d_FILE_DIR_OK)
        return d_FILE_NO_FILES;

    /* To select one file */
    //CTOS_KBDGet(&key);
    //if (key == d_KBD_CANCEL)
        //return d_USER_CANCEL;
    //if ((key - 0x30) > usTotal)
        //return d_FS_FILE_NOT_FOUND;

    /* Open the selected file */
    char *tmp;
    tmp="FL01.txt";
    BYTE fname[d_BUFF_SIZE];
    //if (strncmp(&baFileBuff[15 * (1 - 0x31)], "FL", 2) == 0) {
        /*Reopen a file with specific file name. the file should be created first if not exist */
        //ret = CTOS_FileReopen(&baFileBuff[15 * (key - 0x31)], d_STORAGE_FLASH, &ulHandle);
        //if (ret != d_OK)
            //return ret;
    //} else {
       // return d_FS_FILE_NOT_FOUND;
    //}
    
    //strncmp(&baFileBuff[15 * (1 - 0x31)], "FL01.txt")
    ret = CTOS_FileReopen(tmp, d_STORAGE_FLASH, &ulHandle);
    /* Move the file pointer to a specific position.
     * Move backward from the end of the file. */
    ret = CTOS_FileSeek(ulHandle, 0, d_SEEK_FROM_EOF);
    if (ret != d_OK)
        return ret;

    CTOS_FileGetPosition(ulHandle, &ulPosition);
    sprintf(babuff, "BPos=%d", (int) ulPosition);
    CTOS_LCDTPrintXY(1, 7, babuff);
    
    /* Write data into this opened file */
    ret = CTOS_FileWrite(ulHandle, pcaString, strlen(pcaString));
    if (ret != d_OK)
        return ret;

    CTOS_FileGetPosition(ulHandle, &ulPosition);
    sprintf(babuff, "FPos=%d", (int) ulPosition);
    CTOS_LCDTPrintXY(9, 7, babuff);

    /* Close the opened file */
    CTOS_FileClose(ulHandle);
    return ret;
}
/* ==================================================================                                  
 * FUNCTION NAME: FileRename
 * DESCRIPTION:   Change the name of the file
 * RETURN:        d_USER_CANCEL  --> Cancel to rename the file
 *                d_FILE_NO_FILES   --> No any files by the same application
 *                d_FS_FILE_NOT_FOUND --> User select wrong file item
 *                ret --> Return other messages                                                              
 * NOTES:         none.                                                                             
 * ================================================================ */
USHORT FileRename(void) {
    BYTE tmpKey;
    BYTE baNewName[d_BUFF_SIZE];

    memset(baNewName, 0x00, sizeof (baNewName));
    memset(baFileBuff, 0x00, sizeof (baFileBuff));

    /* Search & list the files by the same appliction */
    if (DirectoryFile(&usTotal, baFileBuff) != d_FILE_DIR_OK)
        return d_FILE_NO_FILES;

    /* To select one file */
    CTOS_KBDGet(&key);
    if (key == d_KBD_CANCEL)
        return d_USER_CANCEL;
    if ((key - 0x30) > usTotal)
        return d_FS_FILE_NOT_FOUND;

    if (strncmp(&baFileBuff[15 * (key - 0x31)], "FL", 2) == 0) {
        sprintf(baNewName, "%.*s_1.txt", 4, &baFileBuff[15 * (key - 0x31)]);
        CTOS_LCDTPrintXY(1, 8, baNewName);
        CTOS_KBDGet(&tmpKey);
    } else
        return d_FS_FILE_NOT_FOUND;

    /* Change the name of selected file */
    ret = CTOS_FileRename(&baFileBuff[15 * (key - 0x31)], baNewName);
    return ret;
}

/* ==================================================================                                  
 * FUNCTION NAME: FileCut                                                                               
 * DESCRIPTION:   Cut off the file content such that the file length is the specific value                
 * RETURN:        d_FILE_LENGTH_SHORT --> The file data length is short                               
 *                d_USER_CANCEL --> Cancel to cut off the file                                      
 *                d_FILE_NO_FILES   --> No any files by the same application                                
 *                d_FS_FILE_NOT_FOUND --> User select wrong file item
 *                ret --> Return other messages                                                          
 * NOTES:         none.                                                                          
 * ================================================================ */
USHORT FileCut(void) {
    USHORT usret;
    ULONG ulFileSize;

    /* Search & list the files by the same appliction */
    if (DirectoryFile(&usTotal, baFileBuff) != d_FILE_DIR_OK)
        return d_FILE_NO_FILES;

    /* To select one file */
    CTOS_KBDGet(&key);
    if (key == d_KBD_CANCEL)
        return d_USER_CANCEL;
    if ((key - 0x30) > usTotal)
        return d_FS_FILE_NOT_FOUND;

    /* Get the file size with specific file name */
    CTOS_FileGetSize(&baFileBuff[15 * (key - 0x31)], &ulFileSize);

    /* Check whether this file size is greater than 20 bytes */
    if (ulFileSize <= 20)
        return d_FILE_LENGTH_SHORT;

    /* Cut off the file size to be specific value.
     * The length 20 bytes is the final size of the file.  */
    usret = CTOS_FileCut(&baFileBuff[15 * (key - 0x31)], 20);

    if (DirectoryFile(&usTotal, baFileBuff) != d_FILE_DIR_OK) //Re-get file lists
        return d_FILE_NO_FILES;
    return usret;
}

/* ==========================================================================
 * FUNCTION NAME: FileDirAttrib
 * DESCRIPTION:   List the attributes of all files
 * RETURN:        d_FILE_NO_FILES    --> No any files by the same application
 *          ret                --> Return Other messages
 * NOTES:         none.
 * ========================================================================== */
USHORT FileDirAttrib(void) {
    //Declare Local Variable //
    CTOS_FILE_ATTRIB stAttrib[256]; //Structure array to retrieve the attribute of file //
    USHORT usFileNo;
    int i;

    //List the attributes of all files //
    ret = CTOS_FileDirAttrib(stAttrib, &usFileNo);
    if (usFileNo == 0) return d_FILE_NO_FILES;

    if (ret != d_OK) return ret;

    //Print the filename, the filesize, the attribute, the UID of all files //
    CTOS_PrinterPutString("      Attribute Directory     \n");
    for (i = 0; i < usFileNo; i++) {
        sprintf(babuff, "No.%d %s", i + 1, stAttrib[i].Filename);
        CTOS_PrinterPutString(babuff);
        sprintf(babuff, "Size %d", (int) stAttrib[i].Filesize);
        CTOS_PrinterPutString(babuff);
        switch (stAttrib[i].FileAttrib) {
            case d_FA_PRIVATE:
                CTOS_PrinterPutString("Attrib   d_FA_PRIVATE");
                break;
            case d_FA_PUBLIC:
                CTOS_PrinterPutString("Attrib   d_FA_PUBLIC");
                break;
        }
        sprintf(babuff, "UID %d\n", (int) stAttrib[i].UID);
        CTOS_PrinterPutString(babuff);
    }
    CTOS_PrinterFline(d_LINE_DOT * 10);
}

/* ==========================================================================
 * FUNCTION NAME: FileSetAttrib
 * DESCRIPTION:   Set the attribute of the file
 * RETURN:        d_FILE_CANCEL        --> Cancel to cut off the file
 *          d_FILE_NO_FILES    --> No any files by the same application
 *          ret                --> Return Other messages
 * NOTES:         none.
 * ========================================================================== */
USHORT FileSetAttrib(void) {
    //Declare Local Variable //                                                                           
    BYTE AttribKey;

    //Search & list the files by the same appliction //                                                  
    if (DirectoryFile(&usTotal, baFileBuff) != d_FILE_DIR_OK) return d_FILE_NO_FILES;

    //Select and set the file //                                                                          
    CTOS_KBDGet(&key);
    if ((key - 0x30) > usTotal) return d_USER_CANCEL;

    show_screen(8);
    //Select and set the attribute //                                                                     
    CTOS_KBDGet(&AttribKey);

    if (AttribKey == d_KBD_1) {
        //Set d_FA_PRIVATE attribute in the file. d_FA_PRIVATE, access only by owner //                       
        ret = CTOS_FileSetAttrib(&baFileBuff[15 * (key - 0x31)], d_FA_PRIVATE);
    } else if (AttribKey == d_KBD_2) {
        //Set d_FA_PUBLIC attribute in the file. d_FA_PUBLIC, both read and write access are free for all //
        ret = CTOS_FileSetAttrib(&baFileBuff[15 * (key - 0x31)], d_FA_PUBLIC);
    } else return d_USER_CANCEL;

    return ret;
}

/* ==========================================================================
 * FUNCTION NAME: FileGetAttrib
 * DESCRIPTION:   Get the attribute of the file
 * RETURN:        d_FILE_CANCEL        --> Cancel to cut off the file
 *          d_FILE_NO_FILES    --> No any files by the same application
 *          ret                --> Return Other messages
 * NOTES:         none.
 * ========================================================================== */
USHORT FileGetAttrib(void) {
    //Declare Local Variable //
    BYTE AttribKey;
    CTOS_FILE_ATTRIB stAttrib; //Structure to retrieve the attribute of file //
    ShowTitle("    Directory         ");

    //Search & list the files by the same appliction //
    if (DirectoryFile(&usTotal, baFileBuff) != d_FILE_DIR_OK) return d_FILE_NO_FILES;

    //Select to get the attribute of the file //
    CTOS_KBDGet(&key);
    if ((key - 0x30) > usTotal) return d_FILE_CANCEL;

    //Get the attribute of the file //
    ret = CTOS_FileGetAttrib(&baFileBuff[15 * (key - 0x31)], &stAttrib);
    if (ret == d_OK) {
        ShowTitle("    Directory         ");
        CTOS_LCDTPrintXY(1, 4, stAttrib.Filename);
        sprintf(babuff, "Size:   %d", (int) stAttrib.Filesize);
        CTOS_LCDTPrintXY(1, 3, babuff);
        if (stAttrib.FileAttrib == d_FA_PRIVATE) sprintf(babuff, "Attrib: Privte");
        else if (stAttrib.FileAttrib == d_FA_PUBLIC) sprintf(babuff, "Attrib: Public");
        CTOS_LCDTPrintXY(1, 4, babuff);
        sprintf(babuff, "UID:    %d", (int) stAttrib.UID);
        CTOS_LCDTPrintXY(1, 5, babuff);
    }
    return ret;
}

/* ==========================================================================
 * FUNCTION NAME: main
 * DESCRIPTION:   function main is responsible for all the tasks listed in the
 *                introduction to this demo.
 * RETURN:        none.
 * NOTES:         none.
 * ========================================================================== */
void testfileman(void) {
    int i;
    USHORT usFileCount = 0;
    ULONG ulReadLen;
    ULONG ulFreeSize;
    BYTE baTemp[d_BUFF_SIZE];

    CTOS_LCDTSelectFontSize(0x101E);

    memset(babuff, 0x00, sizeof (babuff));
    memset(baTemp, 0x00, sizeof (baTemp));

    show_screen(0);
    while (1) {
        CTOS_KBDGet(&key);
        switch (emKM) {
                /*** The Main Page ***/
            case eMain:
                switch (key) {
                    case d_KBD_1: //Run 'NewFile' Function
                        show_screen(1);
                        CTOS_KBDGet(&key);
                        switch (key) {
                            case d_KBD_1: //Flash
                                DirectoryFile(&usTotal, baFileBuff);
                                usFileCount = (usTotal != 7 ? usTotal + 1 : 1);
                                CTOS_LCDTPrintXY(1, 7, "New:");
                                sprintf(babuff, "FL%02d.txt", usFileCount);
                                CTOS_LCDTPrintXY(5, 7, babuff);
                                ret = NewFile(babuff, 0);
                                if (ret == d_OK)
                                    CTOS_LCDTPrintXY(1, 8, "New File Success");
                                else
                                    show_errormsg(ret);
                                /*sprintf(baTemp,"ret:%X           ", ret);
                                CTOS_LCDTPrintXY(1 ,8 ,baTemp);*/
                                CTOS_KBDGet(&key);
                                show_screen(0);
                                break;
                            case d_KBD_CANCEL: //Go back main layer
                                show_screen(0);
                                break;
                        }
                        break;
                    case d_KBD_2: //Run 'DeleteFile' Function
                        show_screen(2);
                        ret = DeleteFile();
                        if (ret == d_OK)
                            CTOS_LCDTPrintXY(1, 8, "Del File Success");
                        else if (ret != d_USER_CANCEL)
                            show_errormsg(ret);

                        if (ret != d_USER_CANCEL)
                            CTOS_KBDGet(&key);
                        show_screen(0);
                        break;
                    case d_KBD_3: //Run 'DirectoryFile' Function
                        memset(baFileBuff, 0x00, sizeof (baFileBuff));
                        show_screen(3);
                        usTotal = 0;
                        ret = DirectoryFile(&usTotal, baFileBuff);
                        if (ret != d_OK)
                            show_errormsg(ret);
                        CTOS_KBDGet(&key);
                        show_screen(0);
                        break;
                    case d_KBD_4: //Run 'FileRead' Function
                        show_screen(5);
                        ulReadLen = sizeof (babuff); //Set the maximum reading size
                        memset(babuff, 0, sizeof (babuff));
                        ret = FileRead(babuff, &ulReadLen);
                        if (ret == d_OK) {
                            sprintf(baTemp, "Read OK (%lu)", ulReadLen);
                            CTOS_LCDTPrintXY(1, 8, baTemp);
                            if (ulReadLen > 0) {
                                CTOS_KBDGet(&key);
                                ClearScreen(4, 26);
                                CTOS_LCDTPrint("The Content: \n");
                                for (i = 0; i < 112; i += 16) {
                                    if (i > ulReadLen)
                                        break;
                                    strncpy(baTemp, &babuff[i], 16);
                                    baTemp[16] = '\n';
                                    if (i >= (112 - 16))
                                        baTemp[16] = '\0';
                                    CTOS_LCDTPrint(baTemp);
                                }
                            }
                        } else if (ret != d_USER_CANCEL) {
                            show_errormsg(ret);
                            CTOS_KBDGet(&key);
                        }

                        if (ret != d_USER_CANCEL)
                            CTOS_KBDGet(&key);

                        show_screen(0);
                        break;
                    case d_KBD_5: //Run 'FileWrite' Function
                        show_screen(4);
                        ret = FileWrite("12345ABCDEFG6789");
                        if (ret == d_OK)
                            CTOS_LCDTPrintXY(1, 8, "Write Success   ");
                        else if (ret != d_USER_CANCEL)
                            show_errormsg(ret);

                        if (ret != d_USER_CANCEL)
                            CTOS_KBDGet(&key);
                        show_screen(0);
                        break;
                    case d_KBD_6: //Run 'FileRename' Function
                        show_screen(6);
                        ret = FileRename();

                        if (ret == d_OK)
                            CTOS_LCDTPrintXY(1, 8, "Rename Success   ");
                        else if (ret != d_USER_CANCEL)
                            show_errormsg(ret);

                        if (ret != d_USER_CANCEL)
                            CTOS_KBDGet(&key);
                        show_screen(0);
                        break;
                    case d_KBD_CANCEL: //Exit Application                        
                        exit(0);
                    case d_KBD_DOWN: //Go to next page
                        show_screen(7);
                        break;
                }
                break;
                /*** The Second Page ***/
            case eMainPage2:
                switch (key) {
                    case d_KBD_1: //Run 'FileCut' Function
                        show_screen(12);
                        ret = FileCut();
                        if (ret == d_OK)
                            CTOS_LCDTPrintXY(1, 8, "Cut Success   ");
                        else if (ret != d_USER_CANCEL)
                            show_errormsg(ret);
                        if (ret != d_USER_CANCEL)
                            CTOS_KBDGet(&key);
                        show_screen(7);
                        break;
                    case d_KBD_2: //Run  'FileSetAttrib' Function
                        ret = FileSetAttrib();
                        if (ret == d_OK)
                            CTOS_LCDTPrintXY(1, 8, "Set Attrib OK  ");
                        else
                            show_errormsg(ret);
                        if (ret != d_FILE_CANCEL)
                            CTOS_KBDGet(&key);
                        show_screen(7);
                        break;
                    case d_KBD_3: //Run 'FileGetAttrib' Function
                        ret = FileGetAttrib();
                        if (ret != d_OK)
                            show_errormsg(ret);
                        if (ret != d_FILE_CANCEL)
                            CTOS_KBDGet(&key);
                        show_screen(7);
                        break;
                    case d_KBD_4: //Run 'FileDirAttrib' Function
                        FileDirAttrib();
                        show_screen(7);
                        break;
                    case d_KBD_5: //Run 'FileFormat' Function
                        show_screen(11);
                        CTOS_KBDGet(&key);
                        if (key == d_KBD_ENTER) {
                            ret = CTOS_FileFormat(0); //Delete whole files created by the AP
                            if (ret == d_OK)
                                CTOS_LCDTPrintXY(1, 8, "Format Success  ");
                            else if (ret != d_USER_CANCEL)
                                show_errormsg(ret);
                            CTOS_Delay(2000);
                        }
                        show_screen(7);
                        break;
                    case d_KBD_6: //Run 'Get Disk Free Size' Function
                        show_screen(13);
                        ret = CTOS_FileGetFreeSpace(0, &ulFreeSize);
                        if (ret == d_OK) {
                            memset(baTemp, 0, sizeof (baTemp));
                            sprintf(baTemp, "%ld Bytes", ulFreeSize);
                            CTOS_LCDTPrintXY(1, 5, baTemp);
                            CTOS_LCDTPrintXY(1, 8, "Get Success   ");
                        } else if (ret != d_USER_CANCEL) {
                            show_errormsg(ret);
                            CTOS_KBDGet(&key);
                        }

                        if (ret != d_USER_CANCEL)
                            CTOS_KBDGet(&key);

                        show_screen(7);
                        break;
                    case d_KBD_UP: //Go back Main Page
                        show_screen(0);
                        break;
                    case d_KBD_CANCEL: //Exit Application                        
                        exit(0);
                        break;
                }
                break;
        }
    }
    exit(0);
}



