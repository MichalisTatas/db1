#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "bf.h"
#include "heap_file.h"


#define CALL_BF(call)       \
{                           \
  BF_ErrorCode code = call; \
  if (code != BF_OK) {         \
    BF_PrintError(code);    \
    return HP_ERROR;        \
  }                         \
}

HP_ErrorCode HP_Init() {
  return HP_OK;
}

HP_ErrorCode HP_CreateFile(const char *filename) {
  BF_Block *firstBlock;
  char* data;
  int fd;

  BF_Block_Init(&firstBlock);
  CALL_BF(BF_CreateFile(filename));
  CALL_BF(BF_OpenFile(filename, &fd));
  CALL_BF(BF_AllocateBlock(fd, firstBlock));
  data = BF_Block_GetData(firstBlock);
  memcpy(data, "Heapfile", 8);
  CALL_BF(BF_UnpinBlock(firstBlock));
  BF_Block_Destroy(&firstBlock);

  return HP_OK;
}

HP_ErrorCode HP_OpenFile(const char *fileName, int *fileDesc){
  BF_Block* myBlock;
  char* data;
  int fd;

  BF_Block_Init(&myBlock);
  CALL_BF(BF_OpenFile(fileName, fileDesc));
  CALL_BF(BF_AllocateBlock(fd, myBlock));
  data = BF_Block_GetData(myBlock);
  if(!strcmp("Heapfile", data))
    return 0;
  CALL_BF(BF_UnpinBlock(myBlock));
  BF_Block_Destroy(&myBlock);
  return HP_OK;
}
  
HP_ErrorCode HP_CloseFile(int fileDesc) {
  CALL_BF(BF_CloseFile(fileDesc));
  return HP_OK;
}

HP_ErrorCode HP_InsertEntry(int fileDesc, Record record) {
  BF_Block* myBlock;
  char* data;
  
  BF_Block_Init(&myBlock);
  CALL_BF(BF_AllocateBlock(fileDesc, myBlock));
  CALL_BF(BF_GetBlock(fileDesc, record.id, myBlock));
  data = BF_Block_GetData(myBlock);
  memcpy(data ,record.name, sizeof(record.name));
  memcpy(data + 64, record.surname, sizeof(record.surname)); 
  memcpy(data + 128, record.city, sizeof(record.city));
  memcpy(data + 192, &record.id, sizeof(record.id));
  BF_Block_SetDirty(myBlock); 
  CALL_BF(BF_UnpinBlock(myBlock));
  BF_Block_Destroy(&myBlock);
  return HP_OK;
}

HP_ErrorCode HP_PrintAllEntries(int fileDesc, char *attrName, void* value) {
  BF_Block* myBlock;
  char* data;
  int blocks_number;
  BF_Block_Init(&myBlock);
  
  CALL_BF(BF_GetBlockCounter(fileDesc, &blocks_number));

  for(int i=1; i<blocks_number; i++) {
    CALL_BF(BF_GetBlock(fileDesc, i, myBlock));
    data = BF_Block_GetData(myBlock);

    if(strcmp(data, attrName) && !strcmp(data, value)) {
      printf("%s %s %s %d \n", data, data  + 64, data + 128, *(int*)(data + 192));
    }

    if(strcmp(data + 64, attrName) && !strcmp(data+64, value)) {
      printf("%s %s %s %d \n", data, data  + 64, data + 128, *(int*)(data + 192));
    }

    if(strcmp(data + 128, attrName) && !strcmp(data+128, value)) {
      printf("%s %s %s %d \n", data, data  + 64, data + 128, *(int*)(data + 192));
    }

    CALL_BF(BF_UnpinBlock(myBlock));
  }
  BF_Block_Destroy(&myBlock);

  return HP_OK;
}

HP_ErrorCode HP_GetEntry(int fileDesc, int rowId, Record *record) {
  BF_Block* myBlock;
  char* data;

  BF_Block_Init(&myBlock);
  CALL_BF(BF_GetBlock(fileDesc, rowId, myBlock));
  data = BF_Block_GetData(myBlock);
  memcpy(record->name, data, strlen(data));
  memcpy(record->surname, data + 64, strlen(data));
  memcpy(record->city, data + 128, strlen(data));
  record->id = *(int*)(data + 192);
  CALL_BF(BF_UnpinBlock(myBlock));
  BF_Block_Destroy(&myBlock);
  return HP_OK;
}
