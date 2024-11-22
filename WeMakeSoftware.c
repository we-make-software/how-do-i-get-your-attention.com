#include"WeMakeSoftware.h"
#include<linux/if_ether.h>
#include<linux/reboot.h>
static bool IsServerClose=false; 
static struct Frame*Frames=NULL;
static inline void*waitForMemory(unsigned long memoryRequiredBytes);
static inline bool waitForMemoryIsAvailable(unsigned long memoryRequiredBytes);
static inline void Print(const char*title,const unsigned char*data,int from,int to);
static inline int CloseFrame(struct Frame*frame);
static inline int DropFrame(struct Frame*frame);
static inline int DropAndCloseFrame(struct Frame*frame);
static inline struct Frame*CreateFrame(uint8_t id);
static inline int SetSizeFrame(struct Frame*frame,uint16_t Size);
static inline int SendFrame(struct Frame*frame);
static inline char*GetStandard(struct Frame*frame,uint16_t version,uint16_t section);
static inline int CloseStandard(struct Frame*frame,uint16_t version,uint16_t section);
static inline bool CreateStandard(struct Frame*frame,uint16_t version,uint16_t section,char**pointer,int64_t position);

static inline int RFC791A(struct Frame* frame,struct RFC791* rfc791, enum Classification Classification){
  return CloseFrame(frame);
}
static inline int RFC8200A(struct Frame* frame, struct RFC8200* rfc8200, enum Classification Classification){
  return CloseFrame(frame);
}


static inline int RFC791R(struct Frame* frame, enum Classification Classification){
    char*Pointer;
    CreateStandard(frame,791,0,&Pointer,14);
    struct RFC791* rfc791 = (struct RFC791*)Pointer;
    if(rfc791->V_IHL&64&&(rfc791->V_IHL&15)>=5){
        {
            uint8_t IHL=(rfc791->V_IHL&15)-5;
            uint16_t ReadPoint=14+sizeof(struct RFC791);
            for(uint8_t i=0;i<IHL;i++){
                char*OptionPointer;
                CreateStandard(frame,791,i+1,&OptionPointer,ReadPoint+(i*4));
            }
        }
        return RFC791A(frame,rfc791,Classification);
    }
    return DropAndCloseFrame(frame);
}
static inline int RFC8200R(struct Frame* frame, enum Classification Classification){
    char*Pointer;
    CreateStandard(frame,802,0,&Pointer,14);
    struct RFC8200* rfc8200 = (struct RFC8200*)Pointer;
    return rfc8200->V_TC_FL[0]&96?RFC8200A(frame,rfc8200,Classification):DropAndCloseFrame(frame);
}
static inline int IEEE802EtherType(struct Frame* frame, struct IEEE802* ieee802, enum Classification Classification) {
    switch ((ieee802->ET[0] << 8) | ieee802->ET[1]) {
        case 2048:return RFC791R(frame,Classification);
        case 34525:return RFC8200R(frame,Classification);
        default:{
            Print("IEEE802EtherType unknown ieee802->ET",ieee802->ET,0,1);
            return CloseFrame(frame);
        }
    }
}
static inline int IEEE802A(struct Frame*frame,struct IEEE802*ieee802){
    if(ieee802->SMAC[0]&1||ieee802->DMAC[0]&1||ieee802->DMAC[0]&2||ieee802->SMAC[0]&2)return CloseFrame(frame);
    {
        enum Classification DMACClassification = ieee802->DMAC[0] & 12;
        if (DMACClassification==ExtendedLocal||DMACClassification==StandardAssigned)return DropAndCloseFrame(frame);
        {
            enum Classification SMACClassification = ieee802->SMAC[0] & 12;
            if (SMACClassification == ExtendedLocal||SMACClassification== StandardAssigned||(DMACClassification==AdministrativelyAssigned&&SMACClassification!=Reserved)||(DMACClassification==Reserved&&SMACClassification!=AdministrativelyAssigned))return DropAndCloseFrame(frame);
        }
        if(DMACClassification==Reserved) goto IEEE802Reserved;
        goto IEEE802AdministrativelyAssigned;
    }
    IEEE802Reserved:
    return IEEE802EtherType(frame,ieee802,Reserved);
    IEEE802AdministrativelyAssigned:
    return IEEE802EtherType(frame,ieee802,AdministrativelyAssigned);
}
static inline int IEEE802R(struct Frame*frame){
    char*Pointer;
    return CreateStandard(frame,802,0,&Pointer,0)?IEEE802A(frame,(struct IEEE802*)Pointer):CloseFrame(frame);
}
static int FrameReader(struct sk_buff*skb,struct net_device*dev,struct packet_type*pt,struct net_device*orig_dev){
    struct Frame*frame;
    if (IsServerClose||skb->len<42||dev->name[0]!=101||dev->name[1]!=116||dev->name[2]!=104||!(frame=CreateFrame(dev->ifindex))){
        kfree_skb(skb); 
        return 1; 
    }
    frame->Standards=NULL;
    frame->IEE802Buffer=skb_mac_header(frame->skb=skb);
    return IEEE802R(frame);
}
#include<linux/slab.h>
#include<linux/string.h>
static void Print(const char*title,const unsigned char*data,int from,int to){
    char*buffer=waitForMemory(strlen(title)+8*(to-from+1)+(to-from)+2);
    strcpy(buffer,title);
    int index=strlen(title);
    buffer[index++]=58;
    buffer[index++]=32;
    for(int i=from;i<=to;i++){
        for (int j=7;j>= 0;j--)buffer[index++]=((data[i]>>j)&1)?49:48;
        buffer[index++]=32;
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
    if(!frame||!frame->Standards)return NET_RX_DROP; 
    struct Standard*this;
    for(this=frame->Standards;this&&!(this->Version==version&&this->Section==section);this=this->Previous);
    if(!this)return NET_RX_DROP; 
    if(this->Previous)this->Previous->Next=this->Next;
    if(this->Next)this->Next->Previous=this->Previous;
    if(frame->Standards==this)frame->Standards=this->Previous;
    kfree(this);
    return NET_RX_SUCCESS; 
}
static inline bool CreateStandard(struct Frame*frame,uint16_t version,uint16_t section,char**pointer,int64_t position) {
    struct Standard*this;
    for(this=frame->Standards;this&&!(this->Version==version&&this->Section==section);this=this->Previous);
    if(this||!(this=waitForMemory(sizeof(struct Standard))))return false;
    this->Version=version;
    this->Section=section;
    this->Next=NULL;
    *pointer=this->Data=frame->IEE802Buffer+position;
    if((this->Previous=frame->Standards))frame->Standards->Next=this;
    frame->Standards = this;
    return true;
}
static inline int CloseFrame(struct Frame*frame) {
    if(!frame)return 0;
    if(frame->Previous)frame->Previous->Next=frame->Next;
    if(frame->Next)frame->Next->Previous=frame->Previous;
    if(frame==Frames)Frames=frame->Previous?frame->Previous:NULL;
    if(frame->Standards)
      for(struct Standard*this=frame->Standards;this;this=this->Previous) 
          kfree(this);
    kfree(frame);
    return 0;
}
static inline int DropFrame(struct Frame*frame){
    if(frame)kfree(frame->skb);
    return NET_RX_DROP;
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
    frame->Next=frame->Previous=NULL;
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
    if(!frame||!(dev=dev_get_by_index(&init_net,frame->id)))return NET_RX_DROP;
    frame->skb->dev=dev;
    int result=dev_queue_xmit(frame->skb);
    dev_put(dev);
    return result>=0;
}
static int __init wms_init(void){
    IsServerClose=false;
    dev_add_pack(&Gateway);   
    return 0;
}
module_init(wms_init);
static void __exit wms_exit(void){
    IsServerClose=true;
    while(Frames)msleep(100); 
    dev_remove_pack(&Gateway);
}
module_exit(wms_exit);
MODULE_INFO_SETUP("Pirasath Luxchumykanthan","WeMakeSoftware Kernel Network","1.0");