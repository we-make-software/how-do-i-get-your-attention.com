#include "WeMakeSoftware.h"
int SendIEEE802_3(struct IEEE802_3*iEEE802_3,int close);
int CloseIEEE802_3(struct IEEE802_3*iEEE802_3,int removeFromNetworkStack);
int SetDefaultIEEE802_3(int id, uint16_t size, struct IEEE802_3 **iEEE802_3);
int IsIEEE802_3Broadcast(struct IEEE802_3* iEEE802_3);
int WeMakeSoftwareSuccess(struct sk_buff*skb);
struct RFC791* ReferenceRFC791(struct IEEE802_3* iEEE802_3);
struct RFC791* ReferenceRFC791(struct IEEE802_3* iEEE802_3) {
    if (iEEE802_3->Reference.rFC791) return iEEE802_3->Reference.rFC791;
    iEEE802_3->Reference.rFC791 = waitForMemory(sizeof(struct RFC791));
    iEEE802_3->Reference.rFC791->VersionAndHeaderLength = iEEE802_3->Payload;
    iEEE802_3->Reference.rFC791->TypeOfService = iEEE802_3->Payload + 1;
    iEEE802_3->Reference.rFC791->TotalLength = iEEE802_3->Payload + 2;
    iEEE802_3->Reference.rFC791->Identification = iEEE802_3->Payload + 4;
    iEEE802_3->Reference.rFC791->FlagsAndFragmentOffset = iEEE802_3->Payload + 6;
    iEEE802_3->Reference.rFC791->TimeToLive = iEEE802_3->Payload + 8;
    iEEE802_3->Reference.rFC791->Protocol = iEEE802_3->Payload + 9;
    iEEE802_3->Reference.rFC791->HeaderChecksum = iEEE802_3->Payload + 10;
    iEEE802_3->Reference.rFC791->SourceAddress = iEEE802_3->Payload + 12;
    iEEE802_3->Reference.rFC791->DestinationAddress = iEEE802_3->Payload + 16;
    iEEE802_3->Reference.rFC791->OptionLength = ((*(iEEE802_3->Reference.rFC791->VersionAndHeaderLength) & 15) - 5) * 4;
    if(iEEE802_3->Reference.rFC791->OptionLength)
        iEEE802_3->Reference.rFC791->Option=iEEE802_3->Payload +20;
    iEEE802_3->Reference.rFC791->Payload=iEEE802_3->Payload + 20 + iEEE802_3->Reference.rFC791->OptionLength;
    return iEEE802_3->Reference.rFC791;
}
struct RFC826* ReferenceRFC826(struct IEEE802_3* iEEE802_3){
    if (iEEE802_3->Reference.rFC826) return iEEE802_3->Reference.rFC826;
    iEEE802_3->Reference.rFC826 = waitForMemory(sizeof(struct RFC826));

    return iEEE802_3->Reference.rFC826;

}
int CloseIEEE802_3(struct IEEE802_3*iEEE802_3,int removeFromNetworkStack){
    if(!iEEE802_3) return 0;
    if(iEEE802_3->Reference.rFC791)kfree(iEEE802_3->Reference.rFC791);
    if(iEEE802_3->Reference.rFC826)kfree(iEEE802_3->Reference.rFC826);
    if(removeFromNetworkStack && iEEE802_3->SKData)WeMakeSoftwareSuccess(iEEE802_3->SKData);
    kfree(iEEE802_3);
    return 1;
}
int RFC826Handler(struct IEEE802_3*iEEE802_3){
    struct RFC826* rFC826 =ReferenceRFC826(iEEE802_3);

    Print("RFC826",iEEE802_3->Payload,0,iEEE802_3->Size-14); 
    CloseIEEE802_3(iEEE802_3,0);
    return WeMakeSoftwareIgnore;
} 
int RFC8200Handler(struct IEEE802_3*iEEE802_3){
    Print("RFC8200",iEEE802_3->Payload,0,iEEE802_3->Size-14);  
    CloseIEEE802_3(iEEE802_3,0);
    return WeMakeSoftwareIgnore;
} 
int RFC791Handler(struct IEEE802_3*iEEE802_3){
    struct RFC791* rFC791=ReferenceRFC791(iEEE802_3);
    {
        int HeaderSize = 20 + rFC791->OptionLength;
        uint16_t originalChecksum = *((uint16_t*)rFC791->HeaderChecksum);
        *((uint16_t*)rFC791->HeaderChecksum) = 0;

        uint32_t sum = 0;
        uint16_t *HeaderWords = (uint16_t *)rFC791->VersionAndHeaderLength;

        for (int i = 0; i < HeaderSize / 2; i++) 
        if ((sum += HeaderWords[i]) > 65535) 
            sum -= 65535;
        *((uint16_t*)rFC791->HeaderChecksum) = originalChecksum;
        if ((uint16_t)~sum != originalChecksum){
         CloseIEEE802_3(iEEE802_3,0);
         return WeMakeSoftwareIgnore;
        }
      
    }


/*
     // Print each field in RFC791
    Print("RFC791 TypeOfService", rFC791->TypeOfService, 0, 1);
    Print("RFC791 TotalLength", rFC791->TotalLength, 0, 2);
    Print("RFC791 Identification", rFC791->Identification, 0, 2);
    Print("RFC791 FlagsAndFragmentOffset", rFC791->FlagsAndFragmentOffset, 0, 2);
    Print("RFC791 TimeToLive", rFC791->TimeToLive, 0, 1);
    Print("RFC791 Protocol", rFC791->Protocol, 0, 1);

    Print("RFC791 SourceAddress", rFC791->SourceAddress, 0, 4);
    Print("RFC791 DestinationAddress", rFC791->DestinationAddress, 0, 4);
    
    // Print Options if they exist
    if (rFC791->OptionLength > 0) {
        Print("RFC791 Options", rFC791->Option, 0, rFC791->OptionLength);
    }

    // Print Payload
    Print("RFC791 Payload", rFC791->Payload, 0, iEEE802_3->Size - (20 + rFC791->OptionLength)); // Adjust for header + options length
*/
    CloseIEEE802_3(iEEE802_3,0);
    return WeMakeSoftwareIgnore;
    
} 




/*Use for later*/


/*
int SetIEEE802_3Multicast(struct IEEE802_3* iEEE802_3) {
    if (!iEEE802_3) return -1;  
    iEEE802_3->DestinationMACAddress[0] |= 0b00000001;

    return 0;
}

int SetIEEE802_3Unicast(struct IEEE802_3* iEEE802_3) {
    if (!iEEE802_3) return -1;  

    // Clear the I/G bit (least significant bit of the first byte) to 0 for unicast
    iEEE802_3->DestinationMACAddress[0] &= ~0b00000001;

    return 0;  // Return success
}
int IEEE802_3UnicastInternetProtocl(){}
*/




int IEEE802_3Broadcast(struct IEEE802_3*iEEE802_3);
int IEEE802_3Broadcast(struct IEEE802_3*iEEE802_3){
    switch (iEEE802_3->EtherType[0]<<8|iEEE802_3->EtherType[1])
    {
    case 0b0000100000000000:
        return RFC791Handler(iEEE802_3);
    case 0b0000100000000110:
        return RFC826Handler(iEEE802_3);
    default:
            Print("IEEE802_3Broadcast EtherType",iEEE802_3->EtherType,0,1);
        break;
    }
    kfree(iEEE802_3);
    return WeMakeSoftwareIgnore;
}
int IEEE802_3Unicast(struct IEEE802_3*iEEE802_3);
int IEEE802_3Unicast(struct IEEE802_3*iEEE802_3){
    switch (iEEE802_3->EtherType[0]<<8|iEEE802_3->EtherType[1])
    {
    case 0b0000100000000000:
        return RFC791Handler(iEEE802_3);
    case 0b1000011011011101:
        return RFC8200Handler(iEEE802_3);
    default:
        Print("IEEE802_3Unicast EtherType",iEEE802_3->EtherType,0,1);
        break;
    }
    kfree(iEEE802_3);
    return WeMakeSoftwareIgnore;
}


//Well Finnish

int Send(int id,int size,unsigned char*data);
int SendIEEE802_3(struct IEEE802_3*iEEE802_3,int close){
   if(!Send(iEEE802_3->ID,iEEE802_3->Size,iEEE802_3->Data))return -1;
   if(close)CloseIEEE802_3(iEEE802_3,1);
   return 0;
}
int SetDefaultIEEE802_3(int id, uint16_t size, struct IEEE802_3 **iEEE802_3) {
    if (*iEEE802_3) return 0;
    struct net_device *dev = dev_get_by_index(&init_net, id);
    if (size > 9018 || !dev) return -1;
    (*iEEE802_3)->SKData = NULL;
    (*iEEE802_3)->ID = id;
    (*iEEE802_3)->Size = size;
    (*iEEE802_3)->Data = waitForMemory(size);
    (*iEEE802_3)->DestinationMACAddress = (*iEEE802_3)->Data;
    (*iEEE802_3)->SourceMacAddress = (*iEEE802_3)->Data + 6;
    (*iEEE802_3)->EtherType = (*iEEE802_3)->Data + 12;
    (*iEEE802_3)->Payload = (*iEEE802_3)->Data + 14;
    (*iEEE802_3)->DestinationMACAddress[0] = 0b11111111;
    (*iEEE802_3)->DestinationMACAddress[1] = 0b11111111;
    (*iEEE802_3)->DestinationMACAddress[2] = 0b11111111;
    (*iEEE802_3)->DestinationMACAddress[3] = 0b11111111;
    (*iEEE802_3)->DestinationMACAddress[4] = 0b11111111;
    (*iEEE802_3)->DestinationMACAddress[5] = 0b11111111;
    (*iEEE802_3)->SourceMacAddress[0] = dev->dev_addr[0];
    (*iEEE802_3)->SourceMacAddress[1] = dev->dev_addr[1];
    (*iEEE802_3)->SourceMacAddress[2] = dev->dev_addr[2];
    (*iEEE802_3)->SourceMacAddress[3] = dev->dev_addr[3];
    (*iEEE802_3)->SourceMacAddress[4] = dev->dev_addr[4];
    (*iEEE802_3)->SourceMacAddress[5] = dev->dev_addr[5];
    dev_put(dev);
    return 0;
}
int IsIEEE802_3Broadcast(struct IEEE802_3* iEEE802_3){
    return (iEEE802_3->DestinationMACAddress[0] == 0b11111111 &&
            iEEE802_3->DestinationMACAddress[1] == 0b11111111 &&
            iEEE802_3->DestinationMACAddress[2] == 0b11111111 &&
            iEEE802_3->DestinationMACAddress[3] == 0b11111111 &&
            iEEE802_3->DestinationMACAddress[4] == 0b11111111 &&
            iEEE802_3->DestinationMACAddress[5] == 0b11111111);
}
int GetIEEE802_3(int id, struct sk_buff *skb, struct IEEE802_3 **iEEE802_3);
int GetIEEE802_3(int id, struct sk_buff *skb, struct IEEE802_3 **iEEE802_3) {
    if (*iEEE802_3)
     return IsIEEE802_3Broadcast(*iEEE802_3) ? 0 : 1;
    *iEEE802_3 = waitForMemory(sizeof(struct IEEE802_3));
    (*iEEE802_3)->ID = id;
    (*iEEE802_3)->SKData = skb;
    (*iEEE802_3)->Size = skb->len;
    (*iEEE802_3)->Data = skb_mac_header(skb);
    (*iEEE802_3)->DestinationMACAddress = (*iEEE802_3)->Data;
    (*iEEE802_3)->SourceMacAddress = (*iEEE802_3)->Data + 6;
    int isIEEE802_3Broadcast = IsIEEE802_3Broadcast(*iEEE802_3);
    if (isIEEE802_3Broadcast || !((*iEEE802_3)->SourceMacAddress[0] & 0b00000010)) {
        (*iEEE802_3)->EtherType = (*iEEE802_3)->Data + 12;
        (*iEEE802_3)->Payload = (*iEEE802_3)->Data + 14;
        return isIEEE802_3Broadcast ? 0 : 1;
    }
    kfree(*iEEE802_3);
    *iEEE802_3 = NULL; 
    return -1;
}
int WeMakeSoftwareSuccess(struct sk_buff *skb){
    kfree_skb(skb); 
    return NET_RX_DROP;
}
EXPORT_SYMBOL(WeMakeSoftwareSuccess);
#include <linux/if_ether.h>
static int IEEE802_3Handler(struct sk_buff *skb, struct net_device *dev, struct packet_type *pt, struct net_device *orig_dev) {
    if (skb->len < 34) return WeMakeSoftwareIgnore;
    struct IEEE802_3* iEEE802_3 = NULL;
    switch (GetIEEE802_3(dev->ifindex, skb, &iEEE802_3)) {
        case 0:
            return IEEE802_3Broadcast(iEEE802_3);
        case 1:
            return IEEE802_3Unicast(iEEE802_3);
        default:
            return WeMakeSoftwareIgnore;
    }
}
#include <linux/slab.h>
#include <linux/string.h>
void Print(const char *title, const unsigned char *data, int from, int to) {
    int index = 0;
    int buf_size = strlen(title) + 8 * (to - from + 1) + (to - from) + 2;
    char *buffer = waitForMemory(buf_size);
    strcpy(buffer, title);
    index = strlen(title);
    buffer[index++] = ':';
    buffer[index++] = ' ';
    for (int i = from; i <= to; i++) {
        for (int j = 7; j >= 0; j--) {
            buffer[index++] = ((data[i] >> j) & 1) ? '1' : '0';
        }
        buffer[index++] = ' ';
    }
    buffer[index - 1] = '\0';
    printk(KERN_INFO "%s\n", buffer);
    kfree(buffer);
}
EXPORT_SYMBOL(Print);
#include <linux/mm.h>
#include <linux/swap.h>
#include <linux/sysinfo.h>
struct sysinfo si;
bool isMemoryAvailable(unsigned long memoryRequiredBytes);
bool isMemoryAvailable(unsigned long memoryRequiredBytes) {
    si_meminfo(&si);
    unsigned long available_bytes = si.freeram * PAGE_SIZE;
    if (available_bytes > 2147483648) {
        available_bytes -= 2147483648;
        return available_bytes >= memoryRequiredBytes;
    } else return false;
}
#include <linux/delay.h>
void waitForMemoryIsAvailable(unsigned long memoryRequiredBytes);
void waitForMemoryIsAvailable(unsigned long memoryRequiredBytes) {
    while (!isMemoryAvailable(memoryRequiredBytes * 2)) udelay(10);
}
void* waitForMemory(unsigned long memoryRequiredBytes){
    waitForMemoryIsAvailable(memoryRequiredBytes);
    void* _kzalloc = kzalloc(memoryRequiredBytes, GFP_KERNEL);
    while (!_kzalloc) {
         waitForMemoryIsAvailable(memoryRequiredBytes);
         _kzalloc = kzalloc(memoryRequiredBytes, GFP_KERNEL);
    }
    return _kzalloc;
}
EXPORT_SYMBOL(waitForMemory);
int Send(int id, int size, unsigned char *data) {
    waitForMemoryIsAvailable(size);
    struct sk_buff* skb = alloc_skb(size, GFP_ATOMIC);
    while (!skb) {
        waitForMemoryIsAvailable(size); 
        skb = alloc_skb(size, GFP_ATOMIC); 
    }
    skb->dev = dev_get_by_index(&init_net, id);
    memcpy(skb_put(skb, size), data, size);
    for (int attempts = 0; attempts < 100; attempts++) {
        if (dev_queue_xmit(skb) >= 0) {
            dev_put(skb->dev);
            return 0; 
        }
        udelay(1); 
    }
    kfree_skb(skb); 
    dev_put(skb->dev);
    return 1;
}
static struct packet_type networkPacketType= {
        .type = htons(ETH_P_ALL),
        .func = IEEE802_3Handler
    };
static int __init wms_init(void) {

    dev_add_pack(&networkPacketType);
    return 0;
}
static void __exit wms_exit(void) {
    dev_remove_pack(&networkPacketType);
}
module_init(wms_init);
module_exit(wms_exit);
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Pirasath Luxchumykanthan");
MODULE_DESCRIPTION("WeMakeSoftware Kernel Network");
MODULE_VERSION("1.0");