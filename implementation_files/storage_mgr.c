#include "dberror.h"
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include "storage_mgr.h"
#include <math.h>



void initStorageManager(void) 
{
	printf(" Storage Manager is Intialized \n");
}

RC createPageFile (char *fileName)
{
  FILE *newFile;
  char *str;
  newFile=fopen(fileName,"w+");
  str= calloc(PAGE_SIZE,sizeof(char));
  if(newFile == NULL)
    return RC_FILE_NOT_FOUND;
memset(str,'\0',PAGE_SIZE);
	fwrite(str ,1 , PAGE_SIZE,newFile);
  fclose(newFile);  
  free(str);
  return RC_OK;
}


RC openPageFile (char *fileName, SM_FileHandle *fHandle)
{
  FILE *newFile;
  newFile = fopen(fileName, "r+");  
  if(newFile == NULL)
    return RC_FILE_NOT_FOUND;
  fHandle->totalNumPages = 1;
  fHandle->curPagePos = 0;
  fHandle->fileName = fileName;
  fHandle->mgmtInfo = newFile;
  
  fseek(newFile, 0, SEEK_SET);
  fclose(newFile);
  return RC_OK;
}

//Close Page
RC closePageFile (SM_FileHandle *fHandle)
{
  if(fopen(fHandle->fileName,"r")==NULL)
		return RC_FILE_NOT_FOUND;
	if(fHandle==NULL)
		return RC_FILE_HANDLE_NOT_INIT;
	if(fclose(fHandle->mgmtInfo)==0)
		return RC_OK;
	else
		return RC_FILE_CANNOT_BE_CLOSED;
}

//Destroy Page
RC destroyPageFile (char *fileName)
{
if (remove(fileName) != 0) {
		perror("Error in deleting a file");
		return RC_FILE_DELETE_FAILED;
	}
	return RC_OK;
  
}



RC readBlock (int pageNum, SM_FileHandle *fHandle, SM_PageHandle memPage)
{int i;
  FILE *newFile;
  newFile = fopen(fHandle->fileName, "r"); 
  if(newFile == NULL)
	return RC_FILE_NOT_FOUND;
  int pos=pageNum*PAGE_SIZE;
  fseek(newFile,pos,SEEK_SET);
  if(fread(memPage, 1, PAGE_SIZE, newFile) > PAGE_SIZE)
	return RC_ERROR;
  memPage[PAGE_SIZE] = 0; 
  for (i = 0; i < strlen (memPage) ; i++) 
	printf ("%c ", memPage[i]);    
  fHandle->curPagePos = pageNum; 
    fclose(newFile); 
     return RC_OK;
}

int getBlockPos (SM_FileHandle *fHandle)
{
  return fHandle->curPagePos;
}


RC readFirstBlock (SM_FileHandle *fHandle, SM_PageHandle memPage)

{
readBlock(0,fHandle,memPage);
}

//Previous Block
RC readPreviousBlock (SM_FileHandle *fHandle, SM_PageHandle memPage)
{
int i;
  FILE *newFile;
  newFile = fopen(fHandle->fileName, "r+");    
  if(newFile == NULL)
    return RC_FILE_NOT_FOUND; 
  int pageNum = fHandle->curPagePos/PAGE_SIZE;     
  int pos=PAGE_SIZE*(pageNum - 2);
  fseek(newFile,pos,SEEK_SET);
    for(i=0;i<PAGE_SIZE;i++)
      {
	memPage[i] = fgetc(newFile);

}
  fHandle->curPagePos = fHandle->curPagePos;
  fclose(newFile); 
  return RC_OK;       	

}

//Current Block
RC readCurrentBlock (SM_FileHandle *fHandle, SM_PageHandle memPage)
{ 
 int i;
  FILE *newFile;
  newFile = fopen(fHandle->fileName, "r+");    
  if(newFile == NULL)
    return RC_FILE_NOT_FOUND; 
  int pageNum=fHandle->curPagePos/PAGE_SIZE;  
  int pos=PAGE_SIZE*(pageNum - 1);
  fseek(newFile,pos,SEEK_SET);
    for( i=0;i<PAGE_SIZE;i++)
      memPage[i] = fgetc(newFile);
    fHandle->curPagePos = pageNum;
    fclose(newFile); 
    return RC_OK;
}

//Next Block
RC readNextBlock (SM_FileHandle *fHandle, SM_PageHandle memPage)
{
  int i;
  FILE *newFile;
  newFile = fopen(fHandle->fileName, "r+");    
  if(newFile == NULL)
    return RC_FILE_NOT_FOUND; 
   int pageNum = ceil((float)fHandle->curPagePos/(float)PAGE_SIZE); 
	int pos=PAGE_SIZE*(pageNum);
   fseek(newFile,pos,SEEK_SET);
    for(i=0;i<PAGE_SIZE;i++)
      memPage[i] = fgetc(newFile);
	  
   fHandle->curPagePos = ftell(newFile);
   fclose(newFile); 
   return RC_OK;       
}

RC readLastBlock (SM_FileHandle *fHandle, SM_PageHandle memPage)
{int i;
  FILE *newFile;
  newFile = fopen(fHandle->fileName, "r+");  
  if(newFile == NULL)
    return RC_FILE_NOT_FOUND; 
  fseek(newFile,((fHandle->totalNumPages - 1) * PAGE_SIZE),SEEK_SET);
    for(i=0;i<PAGE_SIZE;i++)
    memPage[i] = fgetc(newFile);
  fHandle->curPagePos = ftell(newFile);
  fclose(newFile);     
  return RC_OK;
}

RC writeBlock (int pageNum, SM_FileHandle *fHandle, SM_PageHandle memPage)
{int i;
  FILE *newFile;
  newFile = fopen(fHandle->fileName, "r+"); 
  int pos= (pageNum)*PAGE_SIZE;
  if(pageNum==0)
  { 
   fseek(newFile,pos,SEEK_SET);  
   for(i=0;i<PAGE_SIZE;i++)
		fputc(memPage[i],newFile);
   fHandle->curPagePos = ftell(newFile);
   fclose(newFile);
  }
  else
  { 
	fHandle->curPagePos = pos;
	fclose(newFile);
	writeCurrentBlock(fHandle,memPage);  
  } 
  return RC_OK;
}


RC writeCurrentBlock (SM_FileHandle *fHandle, SM_PageHandle memPage)
{
  FILE *newFile;
  newFile = fopen(fHandle->fileName, "r+");
  if(newFile == NULL) 
    return RC_FILE_NOT_FOUND;
  int pos = fHandle->curPagePos;
  fseek(newFile,pos,SEEK_SET);
 // printf("storagemanager %s", memPage);
  fwrite(memPage, sizeof(char), PAGE_SIZE, newFile);
  fHandle->curPagePos = fHandle->curPagePos+1;
  fclose(newFile); 
  return RC_OK;
}

//Append Empty Block
RC appendEmptyBlock (SM_FileHandle *fHandle)
{
RC isFHandleInit = checkFileInit(fHandle);
	if (isFHandleInit != RC_OK) {
		return isFHandleInit;
	}
	//printf("We are adding an empty block to file", fHandle->fileName);
	FILE *filePointer;
	filePointer = fHandle->mgmtInfo;
	char *start;
	filePointer = fopen(fHandle->fileName, "a");
	fHandle->totalNumPages = fHandle->totalNumPages+1;
	fseek(filePointer,0,SEEK_END);

	start = calloc(PAGE_SIZE, 1);
	fwrite("\0", PAGE_SIZE, 1, filePointer);
fHandle->curPagePos=fHandle->curPagePos +1; 
	closePageFile(fHandle);
	free(start);
	return RC_OK;
}


//Ensure Capacity
RC ensureCapacity(int numberOfPages, SM_FileHandle *fHandle) {
	RC isPageNumberValid = pageNumValid(numberOfPages);
	int diffPages, i;
	if (isPageNumberValid != RC_OK) {
		return isPageNumberValid;
	}

	RC isFHandleInit = checkFileInit(fHandle);
	if (isFHandleInit != RC_OK) {
		return isFHandleInit;
	}

	printf("We are ensuring the capacity of the pages %d", numberOfPages);

	if (fHandle->totalNumPages < numberOfPages) {
		diffPages = numberOfPages - fHandle->totalNumPages;
		for (i = 0; i < diffPages; i++) {
			fprintf(fHandle->mgmtInfo, "%c", '\0');
		}
	}
	return RC_OK;
}

RC checkFileInit(SM_FileHandle *fHandle) {
	if (fHandle == NULL || fHandle->fileName == NULL
			|| fHandle->mgmtInfo == NULL || fHandle->totalNumPages < 0
			|| fHandle->curPagePos < 0) {
		// checks whether SM_FileHandle or the file Handle is initiated properly
		return RC_FILE_HANDLE_NOT_INITIATED;
	} else {
		return RC_OK;
	}
}

RC pageNumValid(int pNum) {
	// check to find if page number is greater than 0
	// it is valid only if it is greater than 0
	if (pNum >= 0) {
		return RC_OK;
	} else {
		return RC_PAGENUM_INVALID;
	}

}


static RC getBlock(int position, SM_FileHandle *fHandle, SM_PageHandle memPage) {

// Check if requested page is existing
	if (position > fHandle->totalNumPages && position < 0) {
		return RC_READ_NON_EXISTING_PAGE;
	}

	FILE *fileName; // Pointer to File
	fileName = fHandle->mgmtInfo;
	fflush(fileName);

	fseek(fileName, PAGE_SIZE * position, SEEK_SET); // Seek to the requested block,by moving File pointer
	fread(memPage, PAGE_SIZE, 1, fileName); // Read requested block to memPage

	return RC_OK;
}
