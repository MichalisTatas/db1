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
  CALL_BF(BF_OpenFile(fileName, fileDesc));
  
  return HP_OK;
}
HP_ErrorCode HP_CloseFile(int fileDesc) {
  CALL_BF(BF_CloseFile(fileDesc));
  return HP_OK;
}

HP_ErrorCode HP_InsertEntry(int fileDesc, Record record) {
  //insert code here
  return HP_OK;
}

HP_ErrorCode HP_PrintAllEntries(int fileDesc, char *attrName, void* value) {
  //insert code here
  return HP_OK;
}

HP_ErrorCode HP_GetEntry(int fileDesc, int rowId, Record *record) {
  //insert code here
  return HP_OK;
}
