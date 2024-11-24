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
struct IEEE802 {unsigned char DMAC:48,SMAC:48,ET:16;};
/*
    VendorSpecific:
    AdministrativelyAssigned = 0,
    ExtendedLocal = 4,
    Reserved = 8,
    StandardAssigned = 12
*/
struct IEEE802MACAddress {unsigned char LocallyAdministered:1,Multicast:1,VendorSpecific:2,Reserved:4;};
struct RFC791{unsigned char V:4,IHL:4,TOS,L:16,ID:16,FO:16,TTL,P,HC:16,SA:32,DA:32;};
struct RFC8200{unsigned char V:4,TC:8,FL:20,PL:16,NH,HL,SA:128,DA:128;};
