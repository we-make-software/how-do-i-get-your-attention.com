#include "WeMakeSoftware.h"
#include <linux/if_ether.h>
#include "WeMakeSoftwareExchange.h"
int CloseFrame(struct Frame* frame);
int DropFrame (struct Frame* frame);
int DropAndCloseFrame(struct Frame* frame);
struct Frame* CreateFrame(uint8_t id);
int SetSizeFrame(struct Frame* frame,uint16_t Size);
int SendFrame(struct Frame* frame);
static bool IsServerClose = false; 
static struct Frame*Frames=NULL;
int CloseFrame(struct Frame* frame) {
    if(!frame) return NET_RX_SUCCESS;
    if (frame->Previous) 
      frame->Previous->Next = frame->Next;
    if (frame->Next)
     frame->Next->Previous = frame->Previous;
    if (frame == Frames) 
        Frames = frame->Previous ? frame->Previous : NULL;
    kfree(frame);
    return NET_RX_SUCCESS;
}

int DropFrame(struct Frame* frame){
    if(frame)
        kfree(frame->skb);
    return NET_RX_DROP;
}

int DropAndCloseFrame(struct Frame* frame){
    if(frame){
        kfree(frame->skb);
        CloseFrame(frame);
    }
    return NET_RX_DROP;
}

struct Frame* CreateFrame(uint8_t id) {
    struct Frame* frame = waitForMemory(sizeof(struct Frame));
    if (!frame) return NULL;
    frame->Previous = NULL;
    frame->Next = NULL;
    frame->id=id;
    if (Frames) {
        frame->Previous = Frames;
        Frames->Next = frame;
    } 
    return Frames = frame;
}
int SetSizeFrame(struct Frame* frame,uint16_t Size){
    if(!frame) return NET_RX_DROP;
    if(Size>=14&&Size<=1514&&!frame->skb){
        if(!waitForMemoryIsAvailable(Size)) return NET_RX_DROP;
           frame->skb = alloc_skb(Size, GFP_KERNEL);
        if (!frame->skb) return NET_RX_DROP; 
        skb_put(frame->skb, Size);
        frame->IEE802_3Buffer = (struct IEE802_3Buffer*)skb_mac_header(frame->skb);
        return NET_RX_SUCCESS;
    }
    return NET_RX_DROP;
}
int SendFrame(struct Frame* frame){
    if(!frame) return NET_RX_DROP;
    struct net_device *dev = dev_get_by_index(&init_net, frame->id);
    if (!dev) return NET_RX_DROP;
    frame->skb->dev = dev;
    int result = dev_queue_xmit(frame->skb);
    dev_put(dev);
    return result >= 0 ? NET_RX_SUCCESS : NET_RX_DROP;
}
static int FrameReader(struct sk_buff *skb, struct net_device *dev, struct packet_type *pt, struct net_device *orig_dev) {
    if (skb->len < 42||(dev->name[0] != 101 || dev->name[1] != 116 || dev->name[2] != 104)){
        kfree_skb(skb); 
        return NET_RX_DROP; 
    }
    struct Frame* frame = CreateFrame(dev->ifindex);
    if (!frame){
        kfree_skb(skb); 
        return NET_RX_DROP; 
    }
    frame->skb = skb;
    frame->IEE802_3Buffer = (struct IEE802_3Buffer*)skb_mac_header(skb);
    return IEE802_3In(frame);
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
        for (int j = 7; j >= 0; j--) 
            buffer[index++] = ((data[i] >> j) & 1) ? '1' : '0';
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
    if(IsServerClose) return false;
    si_meminfo(&si);
    unsigned long available_bytes = si.freeram * PAGE_SIZE;
    if (available_bytes > 2147483648) {
        available_bytes -= 2147483648;
        return available_bytes >= memoryRequiredBytes;
    } else return false;
}
#include <linux/delay.h>
bool waitForMemoryIsAvailable(unsigned long memoryRequiredBytes) {
    uint8_t attempts = 0;
    while (!isMemoryAvailable(memoryRequiredBytes * 2) && attempts < 255&&!IsServerClose) {
        udelay(10);
        attempts++;
        if(attempts>=255)
            return false;
    }
    return !IsServerClose;
}
EXPORT_SYMBOL(waitForMemoryIsAvailable);
void* waitForMemory(unsigned long memoryRequiredBytes) {
    if(!waitForMemoryIsAvailable(memoryRequiredBytes)) return NULL;
    return kmalloc(memoryRequiredBytes, GFP_KERNEL);
}
static struct packet_type Gateway = {.type = htons(ETH_P_ALL),  .func = FrameReader};
EXPORT_SYMBOL(waitForMemory);
static int __init wms_init(void) {
    IEE802_3Setup(CloseFrame, DropFrame, DropAndCloseFrame, CreateFrame, SetSizeFrame, SendFrame);
    IsServerClose = false;
    dev_add_pack(&Gateway);   
    return 0;
}
static void __exit wms_exit(void) {
    IsServerClose = true;
    while (!Frames)msleep(100); 
    dev_remove_pack(&Gateway);
}
module_init(wms_init);
module_exit(wms_exit);
MODULE_INFO_SETUP("Pirasath Luxchumykanthan", "WeMakeSoftware Kernel Network", "1.0");