#pragma once
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/types.h>
#include <linux/netdevice.h>
#define MODULE_INFO_SETUP(author, description, version) \
    MODULE_LICENSE("GPL");                              \
    MODULE_AUTHOR(author);                              \
    MODULE_DESCRIPTION(description);                    \
    MODULE_VERSION(version);
extern void* waitForMemory(unsigned long memoryRequiredBytes);
extern bool waitForMemoryIsAvailable(unsigned long memoryRequiredBytes);
extern void Print(const char *title, const unsigned char *data, int from, int to);
struct IEE802_3Buffer {
    unsigned char DestinationMAC[6];
    unsigned char SourceMAC[6];
    uint16_t EtherType;
    unsigned char Payload[];
};
struct Frame{
    struct Frame*Previous;
    struct sk_buff*skb; 
    struct net_device*dev; 
    struct packet_type*pt;
    struct IEE802_3Buffer*IEE802_3Buffer;
    struct Frame*Next;
};

extern void OpenFrame(void);
extern int CloseFrame(struct Frame* frame);
extern int DropFrame(struct Frame* frame);