#pragma once
#include "MemoryManager.h"
typedef enum StorageWriteResult {
    StorageWriteSuccess,
    StorageWriteError
} StorageWriteResult;
typedef enum StorageReadResult {
    StorageReadSuccess, 
    StorageReadError
} StorageReadResult;
extern uint64_t GetStorageSize(void);
extern StorageWriteResult StorageWrite(char *data,uint16_t dataSize,uint64_t position);
extern StorageReadResult StorageRead(char *data, uint16_t dataSize, uint64_t position);