#include"WeMakeSoftware.h"
#include<linux/if_ether.h>
#include<linux/reboot.h>
static bool IsServerClose=false; 
static struct Frame*Frames=NULL;

static inline void*waitForMemory(unsigned long memoryRequiredBytes);
static inline bool waitForMemoryIsAvailable(unsigned long memoryRequiredBytes);
static inline void Print(const char*title,const unsigned char*data,int from,int to);
static inline int ITakeControl(void);
static inline int CloseFrame(struct Frame*frame);
static inline int DropFrame(struct Frame*frame);
static inline int DropAndCloseFrame(struct Frame*frame);
static inline struct Frame*CreateFrame(uint8_t id);
static inline int SetSizeFrame(struct Frame*frame,uint16_t Size);
static inline int SendFrame(struct Frame*frame);
static inline char*GetStandard(struct Frame*frame,uint16_t version,uint16_t section);
static inline int CloseStandard(struct Frame*frame,uint16_t version,uint16_t section);
static inline bool CreateStandard(struct Frame*frame,uint16_t version,uint16_t section,char**pointer,int64_t position);
static inline bool AddStandard(struct Frame*frame,uint16_t version,uint16_t section,char**pointer,char*data,bool IsDynamic);
static inline bool CreateStandardNoPointer(struct Frame*frame, uint16_t version, uint16_t section,int64_t position);
static inline int wmsFree(const void *objp);



static struct Link*GetLinks(struct Frame*frame,uint16_t version,uint16_t section){
    struct Standard*this; 
    for(this=frame->Standards;this&&!(this->Version==version&&this->Section==section);this=this->Previous);
    this->Links;
}
static inline int RegisterLink(struct Link*links,uint16_t ID,unsigned char*Data ){
    struct Link*link=waitForMemory(sizeof(struct Link));
    if(!link)return 0;
    link->ID=ID;
    link->Data=Data;
    link->Next=link->Previous=NULL; 
    if(links){
        links->Next=link;
        link->Previous=links;
    }
    links=link;
    return 1;
}
static inline struct Link* GetLink(struct Link* links, uint16_t ID) {
    struct Link* this = links;
    for (; this && this->ID != ID; this = this->Previous);
    return this;
}



static int FrameReader(struct sk_buff*skb,struct net_device*dev,struct packet_type*pt,struct net_device*orig_dev){
    struct Frame*frame;
    if (IsServerClose||skb->len<42||dev->name[0]!=101||dev->name[1]!=116||dev->name[2]!=104||!(frame=CreateFrame(dev->ifindex))){
        kfree_skb(skb); 
        return 1; 
    }
    frame->IEE802Buffer=skb_mac_header(frame->skb=skb);
    /*
    while (true)
    {
        switch (frame->Status)
        {
        case 1:return ITakeControl(); 
        case 2:return CloseFrame(frame);
        case 3:return DropFrame(frame);
        case 4:return DropAndCloseFrame(frame);;
        default:
            udelay(10);
            break;
        }
    }
    */
    return CloseFrame(frame);
}
#include<linux/slab.h>
#include<linux/string.h>
static void Print(const char *title, const unsigned char *data, int from, int to) {
    int length = to - from + 1;  
    char *buffer = waitForMemory(strlen(title)+8*length + length + 2); 
    strcpy(buffer,title);
    int index=strlen(title);
    buffer[index++]=58;  
    buffer[index++]=32;  
    for(int i=from;i<=to;i++){
        for (int j = 7; j >= 0; j--) 
             buffer[index++] = ((data[i] >> j) & 1) ? 49 : 48; 
        buffer[index++] = 32; 
    }
    buffer[index-1]=0;  
    printk(KERN_INFO "%s\n", buffer);
    kfree(buffer);  
}
#include<linux/mm.h>
#include<linux/swap.h>
#include<linux/sysinfo.h>
static struct sysinfo si;
static inline bool isMemoryAvailable(unsigned long memoryRequiredBytes);
static inline bool isMemoryAvailable(unsigned long memoryRequiredBytes){
    if(IsServerClose)return false;
    si_meminfo(&si);
    return((si.freeram*PAGE_SIZE)-4294967296)>=memoryRequiredBytes;
}
#include<linux/delay.h>
static inline bool waitForMemoryIsAvailable(unsigned long memoryRequiredBytes){
    for (uint8_t attempts=0;attempts<255&&!IsServerClose;attempts++){
        if(isMemoryAvailable(memoryRequiredBytes*2))return true;
        udelay(10);
    }
    return false;
}
static inline void*waitForMemory(unsigned long memoryRequiredBytes){return waitForMemoryIsAvailable(memoryRequiredBytes)?kmalloc(memoryRequiredBytes,GFP_KERNEL):NULL;}
static struct packet_type Gateway={.type=htons(ETH_P_ALL),.func=FrameReader};
static inline char*GetStandard(struct Frame*frame,uint16_t version,uint16_t section) {
    struct Standard*this; 
    for(this=frame->Standards;this&&!(this->Version==version&&this->Section==section);this=this->Previous);
    return this?this->Data:NULL;
}
static inline int CloseStandard(struct Frame*frame,uint16_t version,uint16_t section){
    if(!frame||!frame->Standards)return -1; 
    struct Standard*this;
    for(this=frame->Standards;this&&!(this->Version==version&&this->Section==section);this=this->Previous);
    if(!this)return 0; 
    if(this->Previous)this->Previous->Next=this->Next;
    if(this->Next)this->Next->Previous=this->Previous;
    if(frame->Standards==this)frame->Standards=this->Previous;
    return wmsFree(this->Data)&&wmsFree(this); 
}
static inline int wmsFree(const void *objp){
    if(objp)kfree(objp);
    return 1; 
}
static inline bool CreateStandard(struct Frame*frame,uint16_t version,uint16_t section,char**pointer,int64_t position) {
    struct Standard*this;
    for(this=frame->Standards;this&&!(this->Version==version&&this->Section==section);this=this->Previous);
    if(this||!(this=waitForMemory(sizeof(struct Standard))))return false;
    this->Version=version;
    this->Section=section;
    this->Links=NULL;
    this->Next=NULL;
    *pointer=this->Data=frame->IEE802Buffer+position;
    if((this->Previous=frame->Standards))frame->Standards->Next=this;
    frame->Standards = this;
    return true;
}
static inline bool AddStandard(struct Frame*frame,uint16_t version,uint16_t section,char**pointer,char*data, bool IsDynamic) {
    struct Standard*this;
    for(this=frame->Standards;this&&!(this->Version==version&&this->Section==section);this=this->Previous);
    if(this||!(this=waitForMemory(sizeof(struct Standard))))return false;
    this->IsDynamic=IsDynamic;
    this->Version=version;
    this->Section=section;
    this->Next=NULL;
    *pointer=data;
    if((this->Previous=frame->Standards))frame->Standards->Next=this;
    frame->Standards = this;
    return true;
}
static inline bool CreateStandardNoPointer(struct Frame*frame, uint16_t version, uint16_t section,int64_t position) {
    struct Standard*this;
    for(this=frame->Standards;this&&!(this->Version==version&&this->Section==section);this=this->Previous);
    if(this||!(this=waitForMemory(sizeof(struct Standard))))return false;
    this->Version=version;
    this->Section=section;
    this->Next=NULL;
    this->Data=frame->IEE802Buffer+position;
    if((this->Previous=frame->Standards))frame->Standards->Next=this;
    frame->Standards = this;
    return true;
}
static inline int CloseFrame(struct Frame*frame) {
    if(!frame)return 0;
    if(frame==Frames){
        Frames=frame->Previous;
        if(Frames&&Frames->Next)
            Frames->Next=NULL;
    }else {
        if (frame->Previous) 
            frame->Previous->Next = frame->Next;
        if (frame->Next)
            frame->Next->Previous = frame->Previous;
    }
    if(frame->Standards)
      for(struct Standard*this=frame->Standards;this;this=this->Previous) 
      {
        if(this->Links)
          for(struct Link*link=this->Links;link;link=link->Previous) 
            if(link->Data)kfree(link->Data);
        if(this->IsDynamic)kfree(this->Data);
         kfree(this);
      }

    kfree(frame);
    return 0;
}
static inline int ITakeControl(){
   return NET_RX_DROP;
}
static inline int DropFrame(struct Frame*frame){
    if(frame)kfree(frame->skb);
    return ITakeControl();
}
static inline int DropAndCloseFrame(struct Frame*frame){
    if(frame){
        kfree(frame->skb);
        CloseFrame(frame);
    }
    return NET_RX_DROP;
}
static inline struct Frame*CreateFrame(uint8_t id) {
    struct Frame*frame;
    if(!(frame=waitForMemory(sizeof(struct Frame))))return NULL;
        frame->Next=NULL;
    if(Frames){
        Frames->Next=frame;
        frame->Previous=Frames;
    }else
       frame->Previous=NULL;
    frame->Standards=NULL;
    frame->id=id;
    if((frame->Previous=Frames))Frames->Next=frame;
    return Frames=frame;
}
static inline int SetSizeFrame(struct Frame*frame,uint16_t Size){
    if(!frame||!(Size>=14&&Size<=1514&&!frame->skb)||!waitForMemoryIsAvailable(Size)||!(frame->skb=alloc_skb(Size,GFP_KERNEL)))return NET_RX_DROP;
    skb_put(frame->skb,Size);
    frame->IEE802Buffer=skb_mac_header(frame->skb);
    return 0;
}
static inline int SendFrame(struct Frame*frame){
    struct net_device*dev;
    if(!frame||!(dev=dev_get_by_index(&init_net,frame->id)))return -1;
    frame->skb->dev=dev;
    int result=dev_queue_xmit(frame->skb);
    dev_put(dev);
    return result;
}
static int __init wms_init(void){
    IsServerClose=false;
    dev_add_pack(&Gateway);   
    return 0;
}
module_init(wms_init);
static void __exit wms_exit(void){
    dev_remove_pack(&Gateway);
    IsServerClose=true;
    while(Frames)msleep(100); 
}
module_exit(wms_exit);
MODULE_INFO_SETUP("Pirasath Luxchumykanthan","WeMakeSoftware Kernel Network","1.0");