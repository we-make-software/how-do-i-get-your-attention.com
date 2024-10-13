#pragma once
#include "Memory.h"
typedef struct WMS {
    char *name; 
    uint64_t *size;
    void *data;
} WMS;

extern bool HaveWMS(const char*name);
extern bool RemoveWMS(const char*name);
extern WMS* AddWMS(const char*name);