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
struct IEE802_3Buffer {
    unsigned char DestinationMAC[6];
    unsigned char SourceMAC[6];
    uint16_t EtherType;
    unsigned char Payload[];
};
struct Frame{
    struct Frame*Previous;
    struct sk_buff*skb; 
    int id; 
    struct IEE802_3Buffer*IEE802_3Buffer;
    struct Frame*Next;
};
typedef int(*CloseFrameFunction)(struct Frame*frame);
typedef int(*DropFrameFunction)(struct Frame*frame);
typedef int(*DropAndCloseFrameFunction)(struct Frame*frame);
typedef struct Frame*(*CreateFrameFunction)(uint8_t id);
typedef int(*SetSizeFrameFunction)(struct Frame*frame,uint16_t Size);
typedef int(*SendFrameFunction)(struct Frame*frame);
typedef void(*RebootServerFunction)(void);
typedef bool(*WaitForMemoryIsAvailableFunction)(unsigned long memoryRequiredBytes);
typedef void*(*WaitForMemoryFunction)(unsigned long memoryRequiredBytes);
typedef void(*PrintFunction)(const char*title,const unsigned char*data,int from,int to);
struct WeMakeSoftwareFunctions{
    CloseFrameFunction CloseFrame;
    DropFrameFunction DropFrame;
    DropAndCloseFrameFunction DropAndCloseFrame;
    CreateFrameFunction CreateFrame;
    SetSizeFrameFunction SetSizeFrame;
    SendFrameFunction SendFrame;
    RebootServerFunction RebootServer;
    WaitForMemoryIsAvailableFunction waitForMemoryIsAvailable;
    WaitForMemoryFunction waitForMemory;
    PrintFunction Print; 
};
