#pragma once
#include <linux/init.h> 
#include <linux/module.h>
#include <linux/kernel.h>
extern void* waitForMemory(unsigned long memoryRequiredBytes);
struct Frame {
    unsigned char *data;
    size_t len;
    struct net_device *dev;
};
