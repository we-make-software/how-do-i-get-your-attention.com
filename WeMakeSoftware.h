#pragma once
#include<linux/module.h>
#include<linux/kernel.h>
#include<linux/types.h>
#include<linux/netdevice.h>
#define MODULE_INFO_SETUP(author,description,version)\
MODULE_LICENSE("GPL");\
MODULE_AUTHOR(author);\
MODULE_DESCRIPTION(description);\
MODULE_VERSION(version);
struct Standard{
    struct Standard*Previous,*Next;
    uint16_t Version,Section;
    char*Data;
};
struct Frame{
    struct Frame*Previous,*Next;
    struct sk_buff*skb; 
    int id; 
    char*IEE802Buffer;
    struct Standard*Standards;
};
enum Classification {
    ADMINISTRATIVELY_ASSIGNED = 0,
    EXTENDED_LOCAL = 4,
    RESERVED = 8,
    STANDARD_ASSIGNED = 12
};
struct IEEE802 {
    unsigned char DMAC[6],SMAC[6],ET[2];
    struct Frame*Frame;
    enum Classification DMACClassification,SMACClassification;
};
