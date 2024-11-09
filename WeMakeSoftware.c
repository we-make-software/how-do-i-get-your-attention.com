#include "WeMakeSoftware.h"
#include <linux/if_ether.h>

static bool IsServerClose = false; 
static struct Frame*Frames=NULL;
int CloseFrame(struct Frame* frame){
    if(frame->Next)
        frame->Next->Previous = frame->Previous;
    if(frame->Previous)
        frame->Previous->Next = frame->Next;
    if(Frames&& frame->Next&&Frames->Previous==frame->Previous){
        Frames=frame->Previous;
        Frames->Next = NULL;
    }
    if(frame->pt){
        dev_remove_pack(frame->pt);   
        kfree(frame->pt);     
    }
    kfree(frame);
    frame=NULL;
    
    return NET_RX_SUCCESS;
}
EXPORT_SYMBOL(CloseFrame);
int DropFrame(struct Frame* frame){
    kfree_skb(frame->skb);
    CloseFrame(frame);
    return NET_RX_DROP;
}
EXPORT_SYMBOL(DropFrame); 


struct Frame* CreateFrame(){
    struct Frame* frame=waitForMemory(sizeof())
    if(!frame)return NULL;
    if(Frames){
       frame->Previous=Frames;
       Frames->Next=frame;
       Frames=frame;
    }
    else 
       Frames = frame;

}
EXPORT_SYMBOL(CreateFrame); 

static int IEE802_3RoutingReader(struct Frame* frame){
    //struct IEE802_3Routing* iEE802_3Routing = waitForMemory(sizeof(struct IEE802_3Routing));
    return CloseFrame(frame);
}

static int FrameReader(struct sk_buff *skb, struct net_device *dev, struct packet_type *pt, struct net_device *orig_dev) {
    if (dev->name[0] != 101 || dev->name[1] != 116 || dev->name[2] != 104) return NET_RX_DROP;
    if (skb->len < 42)DropIEE802_3(skb);
    struct Frame* frame = waitForMemory(sizeof(struct Frame));
    if (!frame){
        kfree_skb(skb); 
        return NET_RX_DROP; 
    }
    frame->dev = dev;
    frame->skb = skb;
    frame->pt =pt;
    frame->IEE802_3Buffer = (struct IEE802_3Buffer*)skb_mac_header(skb);
    frame->Previous = NULL;
    frame->Next = NULL;
    if (Frames) {
        frame->Previous=Frames;
        Frames->Next=frame;
        Frames=frame;
    }
    else 
        Frames = frame;
    OpenFrame();
    return IEE802_3RoutingReader(frame);
}

void OpenFrame(void) {
    if(IsServerClose)return;
    struct packet_type* NetworkPacket= waitForMemory(sizeof(struct packet_type));
    if(!NetworkPacket)return;
    NetworkPacket->type = htons(ETH_P_ALL);
    NetworkPacket->func = FrameReader;
    dev_add_pack(NetworkPacket);
}
EXPORT_SYMBOL(OpenFrame); 
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
EXPORT_SYMBOL(waitForMemory);
static int __init wms_init(void) {
    OpenFrame();
    IsServerClose = false;
    return 0;
}
static void __exit wms_exit(void) {
    IsServerClose = true;
    while (!Frames)msleep(100); 
}
module_init(wms_init);
module_exit(wms_exit);
MODULE_INFO_SETUP("Pirasath Luxchumykanthan", "WeMakeSoftware Kernel Network", "1.0");