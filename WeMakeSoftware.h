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
enum Access{
    Read = 0,
    Write = 1,
    Execute = 2
};
struct Frame{
    struct Frame*Previous,*Next;
    struct sk_buff*skb; 
    int id; 
    char*IEE802Buffer;
    struct Standard*Standards;
};

enum Classification {
    AdministrativelyAssigned = 0,
    ExtendedLocal = 4,
    Reserved = 8,
    StandardAssigned = 12
};
struct IEEE802 {
    unsigned char DMAC[6],SMAC[6],ET[2];
};
struct IEEE802MACAddress {
    char LocallyAdministered:1,Multicast:1,VendorSpecific:2,Reserved:4;
};
struct RFC791{
    unsigned char V:4,IHL:4,TOS,L[2],ID[2],FO[2],TTL,P,HC[2],SA[4],DA[4]; 
};
struct RFC8200{
    unsigned char V_TC_FL[4],PL[2],NH,HL,SA[16],DA[16];
};
