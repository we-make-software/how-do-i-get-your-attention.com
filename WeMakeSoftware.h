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
    uint16_t Version;
    uint16_t Section;
    char*Data;
};
struct Frame{
    struct Frame*Previous,*Next;
    struct sk_buff*skb; 
    int id; 
    char*IEE802_3Buffer;
    struct Standard*Standards;
};
struct IEEE802_3 {
    unsigned char DestinationMAC[6],SourceMAC[6],EtherType[2];
};
