#pragma once
#include<linux/module.h>
#include<linux/kernel.h>
#include<linux/types.h>
#include<linux/netdevice.h>
#include<linux/time.h>
#define MODULE_INFO_SETUP(author,description,version)\
MODULE_LICENSE("GPL");\
MODULE_AUTHOR(author);\
MODULE_DESCRIPTION(description);\
MODULE_VERSION(version);
struct Link {
    struct Link *Previous, *Next;
    uint16_t ID; 
    unsigned char*Data; 
};

struct Standard{
    struct Standard*Previous,*Next;
    uint16_t Version,Section;
    bool IsDynamic;
    struct Link*Links;
    unsigned char*Data;
};

struct Frame{
    struct Frame*Previous,*Next;
    struct sk_buff*skb; 
    int id;
    int Status; 
    unsigned char*IEE802Buffer;
    struct Standard*Standards;
};


struct IEEE802 {unsigned char DMAC[6],SMAC[6],ET[2];};
/*
    VendorSpecific:
    AdministrativelyAssigned = 0,
    Reserved = 8
*/
struct IEEE802MACAddress {unsigned char LocallyAdministered:1,Multicast:1,VendorSpecific:2,Reserved:4,Address[5];};


