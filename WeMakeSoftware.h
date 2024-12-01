#pragma once
#include<linux/module.h>
#include<linux/kernel.h>
#include<linux/types.h>
#include<linux/netdevice.h>
#include<linux/ktime.h>
#include<linux/kthread.h>
#define MODULE_INFO_SETUP(author,description,version)\
MODULE_LICENSE("GPL");\
MODULE_AUTHOR(author);\
MODULE_DESCRIPTION(description);\
MODULE_VERSION(version);
struct Frame{
    struct Frame*Previous,*Next;
    struct sk_buff*skb; 
    int id,IsDeleted;
    ktime_t Start;
    unsigned char*IEE802Buffer;
};
