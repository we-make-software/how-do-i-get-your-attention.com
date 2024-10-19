#pragma once
#include "Memory.h"
typedef struct WMS {
    char *name; 
    uint64_t *size;
    void *data;
} WMS;

extern bool HaveMemoryManager(const char*name);
extern bool RemoveMemoryManager(const char*name);
extern WMS* AddMemoryManager(const char*name);