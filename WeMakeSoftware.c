#include"WeMakeSoftware.h"
#include<linux/if_ether.h>
#include<linux/reboot.h>
struct WeMakeSoftwareFunctions*WeMakeSoftware;
static bool IsServerClose=false; 
static struct Frame*Frames=NULL;
void*waitForMemory(unsigned long memoryRequiredBytes);
bool waitForMemoryIsAvailable(unsigned long memoryRequiredBytes);
void Print(const char*title,const unsigned char*data,int from,int to);
int CloseFrame(struct Frame*frame);
int DropFrame(struct Frame*frame);
int DropAndCloseFrame(struct Frame*frame);
struct Frame*CreateFrame(uint8_t id);
int SetSizeFrame(struct Frame*frame,uint16_t Size);
int SendFrame(struct Frame*frame);
char*GetStandard(struct Frame*frame,uint16_t version,uint16_t section);
int CloseStandard(struct Frame*frame,uint16_t version,uint16_t section);
char*CreateStandard(struct Frame*frame,uint16_t version,uint16_t section);

char*GetStandard(struct Frame*frame,uint16_t version,uint16_t section){
    if(!frame->Standards)return NULL;
    struct Standard*current=frame->Standards;
    while (current){
        if(current->Version==version&&current->Section==section)return current->Data;
        current=current->Previous;
    }
    return NULL;
}
int CloseStandard(struct Frame*frame,uint16_t version,uint16_t section){
    if (!frame||!frame->Standards)return NET_RX_DROP; 
    struct Standard*current=frame->Standards;
    while (current)
    {
        if (current->Version==version&&current->Section==section){
            if(current->Previous)current->Previous->Next=current->Next;
            if(current->Next)current->Next->Previous=current->Previous;
            if(frame->Standards==current)
                if(current->Previous)frame->Standards=current->Previous;
                else frame->Standards=NULL;
            kfree(current);
            return NET_RX_SUCCESS; 
        }
        current = current->Previous;
    }
    return NET_RX_DROP; 
}
char*CreateStandard(struct Frame*frame,uint16_t version,uint16_t section){
    struct Standard*current=frame->Standards;
    while (current)
    {
        if(current->Version==version&&current->Section==section)return NULL; 
        current=current->Previous;
    }
    struct Standard*standard=waitForMemory(sizeof(struct Standard));
    if(!standard)return NULL;
    standard->Version=version;
    standard->Section=section;
    standard->Next=standard->Previous=NULL;
    if(frame->Standards){
        frame->Standards->Next=standard;
        standard->Previous=frame->Standards;
    }
    return(frame->Standards=standard)->Data;
}
int CloseFrame(struct Frame*frame) {
    if(!frame) return NET_RX_SUCCESS;
    if (frame->Previous) 
      frame->Previous->Next = frame->Next;
    if (frame->Next)
        frame->Next->Previous = frame->Previous;
    if (frame==Frames) 
        Frames=frame->Previous?frame->Previous:NULL;
    struct Standard*current=frame->Standards;
    while (current)
    {
        kfree(current);
        current = current->Previous;
    }
    frame->Standards=NULL;
    kfree(frame);
    return NET_RX_SUCCESS;
}
int DropFrame(struct Frame*frame){
    if(frame)kfree(frame->skb);
    return NET_RX_DROP;
}
int DropAndCloseFrame(struct Frame*frame){
    if(frame){
        kfree(frame->skb);
        CloseFrame(frame);
    }
    return NET_RX_DROP;
}
struct Frame*CreateFrame(uint8_t id) {
    struct Frame*frame;
    if(!(frame=waitForMemory(sizeof(struct Frame))))return NULL;
    frame->Next=frame->Previous=NULL;
    frame->id=id;
    if(Frames){
        frame->Previous=Frames;
        Frames->Next=frame;
    } 
    return Frames=frame;
}
int SetSizeFrame(struct Frame*frame,uint16_t Size){
    if(!frame||!(Size>=14&&Size<=1514&&!frame->skb)||!waitForMemoryIsAvailable(Size)||!(frame->skb=alloc_skb(Size,GFP_KERNEL)))return NET_RX_DROP;
    skb_put(frame->skb,Size);
    frame->IEE802_3Buffer=skb_mac_header(frame->skb);
    return NET_RX_SUCCESS;
}
int SendFrame(struct Frame*frame) {
    struct net_device*dev;
    if (!frame||!(dev=dev_get_by_index(&init_net,frame->id)))return NET_RX_DROP;
    frame->skb->dev=dev;
    int result=dev_queue_xmit(frame->skb);
    dev_put(dev);
    return result>=0?NET_RX_SUCCESS:NET_RX_DROP;
}
extern int IEE802_3In(struct Frame*frame);
static int FrameReader(struct sk_buff*skb,struct net_device*dev,struct packet_type*pt,struct net_device*orig_dev){
    struct Frame*frame;
    if (IsServerClose||skb->len<42||dev->name[0]!=101||dev->name[1]!=116||dev->name[2]!=104||!(frame=CreateFrame(dev->ifindex))){
        kfree_skb(skb); 
        return NET_RX_DROP; 
    }
    frame->Standards=NULL;
    frame->IEE802_3Buffer=skb_mac_header((frame->skb=skb));
    return IEE802_3In(frame);
}
#include<linux/slab.h>
#include<linux/string.h>
void Print(const char*title,const unsigned char*data,int from,int to){
    char*buffer=waitForMemory(strlen(title)+8*(to-from+1)+(to-from)+2);
    strcpy(buffer,title);
    int index=strlen(title);
    buffer[index++]=58;
    buffer[index++]=32;
    for(int i=from;i<=to i++){
        for (int j=7;j>= 0;j--)buffer[index++]=((data[i]>>j)&1)?49:48;
        buffer[index++]=32;
    }
    buffer[index-1]=0;
    printk(KERN_INFO "%s\n", buffer);
    kfree(buffer);
}
#include<linux/mm.h>
#include <linux/swap.h>
#include <linux/sysinfo.h>
struct sysinfo si;
bool isMemoryAvailable(unsigned long memoryRequiredBytes);
bool isMemoryAvailable(unsigned long memoryRequiredBytes){
    if(IsServerClose)return false;
    si_meminfo(&si);
    return((si.freeram*PAGE_SIZE)-4294967296)>=memoryRequiredBytes;
}
#include<linux/delay.h>
bool waitForMemoryIsAvailable(unsigned long memoryRequiredBytes){
    for (uint8_t attempts=0;attempts<255&&!IsServerClose;attempts++){
        if(isMemoryAvailable(memoryRequiredBytes*2))return true;
        udelay(10);
    }
    return false;
}
void*waitForMemory(unsigned long memoryRequiredBytes){return waitForMemoryIsAvailable(memoryRequiredBytes)?kmalloc(memoryRequiredBytes,GFP_KERNEL):NULL;}
void RebootServer(void);
static struct packet_type Gateway={.type=htons(ETH_P_ALL),.func=FrameReader};
static bool reboot=true;
module_param(reboot,bool,0644);
void RebootServer(void){
    IsServerClose=true;
    while(Frames)msleep(100); 
    dev_remove_pack(&Gateway);
    if(WeMakeSoftware)kfree(WeMakeSoftware);
    if(reboot)kernel_restart(NULL);
}
extern void IEE802_3Setup(struct WeMakeSoftwareFunctions*weMakeSoftwareFunctions);
static int __init wms_init(void){
    WeMakeSoftware=waitForMemory(sizeof(struct WeMakeSoftwareFunctions));
    WeMakeSoftware->CloseFrame=CloseFrame;
    WeMakeSoftware->DropFrame=DropFrame;
    WeMakeSoftware->DropAndCloseFrame=DropAndCloseFrame;
    WeMakeSoftware->CreateFrame=CreateFrame;
    WeMakeSoftware->SetSizeFrame=SetSizeFrame;
    WeMakeSoftware->SendFrame=SendFrame;
    WeMakeSoftware->RebootServer=RebootServer;
    WeMakeSoftware->waitForMemoryIsAvailable=waitForMemoryIsAvailable;
    WeMakeSoftware->waitForMemory=waitForMemory;
    WeMakeSoftware->Print=Print;
    IEE802_3Setup(WeMakeSoftware);
    IsServerClose=false;
    dev_add_pack(&Gateway);   
    return 0;
}
module_init(wms_init);
static void __exit wms_exit(void){RebootServer();}
module_exit(wms_exit);
MODULE_INFO_SETUP("Pirasath Luxchumykanthan","WeMakeSoftware Kernel Network","1.0");