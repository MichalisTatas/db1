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
  int blocks_number;
  char* data;

  BF_Block_Init (&myBlock);
  CALL_BF(BF_GetBlockCounter(fileDesc, &blocks_number));
  CALL_BF(BF_GetBlock(fileDesc, blocks_number - 1, myBlock));
  data = BF_Block_GetData(myBlock);

  if(blocks_number == 1) {
    CALL_BF(BF_UnpinBlock(myBlock));
    CALL_BF(BF_AllocateBlock(fileDesc, myBlock));
    CALL_BF(BF_GetBlock(fileDesc, blocks_number, myBlock));
    data = BF_Block_GetData(myBlock);
    memset(data, 0, 1);
    memcpy(data + 1, &record.id, 4);
    memcpy(data + 5, record.name, 15);
    memcpy(data + 20, record.surname, 20);
    memcpy(data + 40, record.city, 20);
    BF_Block_SetDirty(myBlock);
  }
  else if(*data < 8) {
    memset(data, *data + 1, 1);
    memcpy(data + (*data)*56 + 1,  &record.id, 4);
    memcpy(data + (*data)*56 + 5, record.name, 15);
    memcpy(data + (*data)*56 + 20, record.surname, 20);
    memcpy(data + (*data)*56 + 40, record.city, 20);
  }
  else if(*data == 8) {
    memset(data, *data + 1, 1);
    CALL_BF(BF_UnpinBlock(myBlock));
    CALL_BF(BF_AllocateBlock(fileDesc, myBlock));
    CALL_BF(BF_GetBlock(fileDesc, blocks_number, myBlock));
    data = BF_Block_GetData(myBlock);
    memset(data, 0, 1);
    memcpy(data + 1, &record.id, 4);
    memcpy(data + 5, record.name, 15);
    memcpy(data + 20, record.surname, 20);
    memcpy(data + 40, record.city, 20);
    BF_Block_SetDirty(myBlock);
  }
  
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

  for(int i=0; i<blocks_number; i++) {
    CALL_BF(BF_GetBlock(fileDesc, i, myBlock));
    data = BF_Block_GetData(myBlock);
    for(int j=0; j<*data; j++) {
      printf("%s %s %s %d \n",data + j*56 + 5, data + j*56+ 20, data + j*56 + 40, *(int*)(data + j*56 + 1));
      // if((strcmp("name", attrName) == 0) && (strcmp(data + j*56 + 5, value) == 0)) {
      //   printf("%s %s %s %d \n",data + j*56 + 5, data + j*56+ 20, data + j*56 + 40, *(int*)(data + j*56 + 1));
      // }

      // if((strcmp("surname", attrName) == 0) && (strcmp(data + j*56 + 20, value) == 0)) {
      //   printf("%s %s %s %d \n",data + j*56 + 5, data + j*56+ 20, data + j*56 + 40, *(int*)(data + j*56 + 1));
      // }

      // if((strcmp("city", attrName) == 0) && (strcmp(data + j*56 + 40, value) == 0)) {
      //   printf("%s %s %s %d \n",data + j*56 + 5, data + j*56+ 20, data + j*56 + 40, *(int*)(data + j*56 + 1));
      // }

    }
    printf("\n--------------------------------------------\n");
    CALL_BF(BF_UnpinBlock(myBlock));
  }
  BF_Block_Destroy(&myBlock);

  return HP_OK;
}

HP_ErrorCode HP_GetEntry(int fileDesc, int rowId, Record *record) {
  BF_Block* myBlock;
  int blocks_number;
  char* data;

  BF_Block_Init(&myBlock);
  CALL_BF(BF_GetBlockCounter(fileDesc, &blocks_number));
  CALL_BF(BF_GetBlock(fileDesc, rowId/9 + 1, myBlock));
  data = BF_Block_GetData(myBlock);
  memcpy(record->name, data + (rowId%9)*56 + 5, 15);
  memcpy(record->surname, data + (rowId%9)*56 + 20, 20);
  memcpy(record->city, data + (rowId%9)*56 + 40, 20);
  record->id = *(int*)(data + (rowId%9)*56 + 1);
  CALL_BF(BF_UnpinBlock(myBlock));
  BF_Block_Destroy(&myBlock);
  return HP_OK;
}
