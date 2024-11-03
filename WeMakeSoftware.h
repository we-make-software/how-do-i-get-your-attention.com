#pragma once
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/types.h>
#include <linux/netdevice.h>
#include <linux/skbuff.h>
extern int WeMakeSoftwareSuccess(struct sk_buff *skb);//CloseIEEE802_3
#define WeMakeSoftwareIgnore NET_RX_SUCCESS
#define WeMakeSoftwareError -EIO
#define WeMakeSoftwareContinue 2
extern void* waitForMemory(unsigned long memoryRequiredBytes);
extern void Print(const char *title, const unsigned char *data, int from, int to);



struct RFC8200{


}; 
struct RFC791{
    uint8_t OptionLength;
    char *VersionAndHeaderLength, *TypeOfService, *TotalLength, *Identification, *FlagsAndFragmentOffset, *TimeToLive, *Protocol, *HeaderChecksum, *SourceAddress, *DestinationAddress,*Option,*Payload;
};
struct RFC826 { 
    char* HardwareType, *ProtocolType, *HardwareSize, *ProtocolSize, *OperationCode, *SenderMACAddress, *SenderIPAddress, *TargetMACAddress, *TargetIPAddress; 
};
struct IEEE802_3{
    int ID;
    uint16_t Size;
    struct sk_buff*SKData;
    char*Data,*DestinationMACAddress,*SourceMacAddress,*EtherType,*Payload;
    union {
        struct RFC826*rFC826;
        struct RFC791* rFC791;
        struct RFC8200* rFC8200;
    } Reference;
};

