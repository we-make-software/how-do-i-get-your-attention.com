#pragma once
#include "Memory.h"
typedef struct MemoryManager {
    char *name; 
    uint64_t *size;
    void *data;
} MemoryManager;

extern bool HaveMemoryManager(const char*name);
extern bool RemoveMemoryManager(const char*name);
extern MemoryManager* AddMemoryManager(const char*name);