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
struct IEEE802 {unsigned char DMAC[6],SMAC[6],ET[2];};
/*
    VendorSpecific:
    AdministrativelyAssigned = 0,
    Reserved = 8
*/
struct IEEE802MACAddress {unsigned char LocallyAdministered:1,Multicast:1,VendorSpecific:2,Reserved:4;};
struct RFC791{unsigned char IHL:4,V:4,TOS,L[8],ID[8],FO[8],TTL,P,HC[8],SA[4],DA[4];};
/*
    Delay: Indicates if low delay is requested.
    - 1: Low delay preferred (e.g., VoIP, gaming).
    - 0: Default, delay not prioritized.
*/
struct RFC791TypeOfService{unsigned char Precedence:3,Delay:1,Throughput:1,Reliability:1,Reserved:2;};
struct RFC8200 {unsigned char TC:4,V:4,TCN:4, FL:4, FLN[2], PL[2], NH, HL, SA[16], DA[16];};
struct RFC2474 {unsigned char DSCP:6,CU:2;};

