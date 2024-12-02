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
static inline void ShowTimeByFrame(struct Frame*frame);




static int FrameReader(struct sk_buff*skb,struct net_device*dev,struct packet_type*pt,struct net_device*orig_dev){
    struct Frame*frame;
    if (IsServerClose||skb->len<42||dev->name[0]!=101||dev->name[1]!=116||dev->name[2]!=104||!(frame=CreateFrame(dev->ifindex))){
        kfree_skb(skb); 
        return 1; 
    }
    frame->IEE802Buffer=skb_mac_header(frame->skb=skb);
    //IEEE802->Destination 
    if(!(frame->IEE802Buffer[0]&3)&&
    //IEEE802->Source 
    !(frame->IEE802Buffer[6]&3))
    //IEEE802->Ethertype
    switch (frame->IEE802Buffer[12]<<8|frame->IEE802Buffer[13]){
        case 2048:
            //RFC791->Flags->Reserved  
            if(!(frame->IEE802Buffer[20]&128)&&
            //RFC791->Version
            (frame->IEE802Buffer[14]&240)==64&&
            //RFC791->Internet Header Length
            (frame->IEE802Buffer[14]&15)>4&&
            ((frame->IEE802Buffer[14]&15)*4)<=
            //RFC791->Total Length
            (frame->IEE802Buffer[16]<<8|frame->IEE802Buffer[17]))
            //RFC791->RFC3168->Type of Service->Explicit Congestion Notification
            switch(frame->IEE802Buffer[15]&3)
            {
                //RFC791->RFC3168->Type of Service->Explicit Congestion Notification : Not-ECT
                case 0:
                    //RFC791->RFC3168->Type of Service->Differentiated Services Code Point
                    if(!(frame->IEE802Buffer[15]&252)){
                        //RFC791->Flags->Don't Fragment
                        if((frame->IEE802Buffer[20]&64)){
                            //RFC791->Flags->More Fragments
                            if(frame->IEE802Buffer[20]&32)return DropAndCloseFrame(frame);
                            pr_info("RFC791->Flags->Don't Fragment");
                            return CloseFrame(frame);
                        }
                        //RFC791->Flags->More Fragments
                        if(frame->IEE802Buffer[20]&32){
                            pr_info("RFC791->Flags->More Fragments");
                            return CloseFrame(frame);
                        }
                        //RFC791->Flags->Last Fragment
                        pr_info("RFC791->Flags->Last Fragment");
                        return CloseFrame(frame); 
                    }else return DropAndCloseFrame(frame);
                 //RFC791->RFC3168->Type of Service->Explicit Congestion Notification : ECT(1)
                case 1:{
                    pr_info("RFC791->RFC3168->Type of Service->Explicit Congestion Notification : ECT(1)");
                    return CloseFrame(frame);
                }
                //RFC791->RFC3168->Type of Service->Explicit Congestion Notification : ECT(0) 
                case 2:{
                    pr_info("RFC791->RFC3168->Type of Service->Explicit Congestion Notification : ECT(0)");
                    return CloseFrame(frame);
                };  
                //RFC791->RFC3168->Type of Service->Explicit Congestion Notification : CE
                case 3:{
                    pr_info("RFC791->RFC3168->Type of Service->Explicit Congestion Notification : CE");
                    return CloseFrame(frame);
                } 
            }else return DropAndCloseFrame(frame);
        case 34525:
            //RF8200->Version
            if((frame->IEE802Buffer[14]&240)==96)
            //RF8200->RFC3168->Traffic Class->Explicit Congestion Notification
            switch (frame->IEE802Buffer[15]&48)
            {
                case 0:{
                    //RF8200->RFC9435->Traffic Class->Differentiated Services Code Point
                    if((frame->IEE802Buffer[14]&15)||(frame->IEE802Buffer[15]&192))return DropAndCloseFrame(frame);
                    //RF8200->RFC3168->Traffic Class->Explicit Congestion Notification : Not-ECT
                    pr_info("RF8200->RFC3168->Traffic Class->Explicit Congestion Notification : Not-ECT"); 
                    return CloseFrame(frame);
                }
                case 16:{
                    //RFC8200->RFC3168->Traffic Class->Explicit Congestion Notification : ECT(1)
                    pr_info("RFC8200->RFC3168->Traffic Class->Explicit Congestion Notification : ECT(1)");
                    return CloseFrame(frame);
                }
                case 32:{
                    //RFC8200->RFC3168->Traffic Class->Explicit Congestion Notification : ECT(0)
                    pr_info("RFC8200->RFC3168->Traffic Class->Explicit Congestion Notification : ECT(0)");
                    return CloseFrame(frame);
                };    
                case 48:{
                    //RFC8200->RFC3168->Traffic Class->Explicit Congestion Notification : CE
                    pr_info("RFC8200->RFC3168->Traffic Class->Explicit Congestion Notification : CE");
                    return CloseFrame(frame);
                } 
            }else return DropAndCloseFrame(frame);
    }
    return CloseFrame(frame);
}

static inline void ShowTimeByFrame(struct Frame*frame){
    pr_info("Time (ns): %lld\n", ktime_to_ns(ktime_sub(ktime_get(), frame->Start)));
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
        ndelay(1);
    }
    return false;
}
static inline void*waitForMemory(unsigned long memoryRequiredBytes){return waitForMemoryIsAvailable(memoryRequiredBytes)?kmalloc(memoryRequiredBytes,GFP_KERNEL):NULL;}
static struct packet_type Gateway={.type=htons(ETH_P_ALL),.func=FrameReader,.ignore_outgoing=1};


static inline int CloseFrame(struct Frame*frame) {
    if(!frame)return 0;
    if(frame==Frames){
        Frames=frame->Previous;
        if(Frames&&Frames->Next)Frames->Next=NULL;
    }else{
        if (frame->Previous) 
        frame->Previous->Next = frame->Next;
        if (frame->Next)
            frame->Next->Previous = frame->Previous;
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
    frame->id=id;
    frame->IsDeleted=0;
    frame->Start=ktime_get();
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
struct net_device *dev = dev_get_by_index(&init_net, frame->id);
if (!dev)return -ENODEV;
struct sk_buff *skb;
// Allocate the skb (socket buffer) for the packet
skb = alloc_skb(ETH_FRAME_LEN, GFP_KERNEL);  // Example: Allocating space for an Ethernet frame
if (!skb) {
    pr_err("Failed to allocate skb\n");
    dev_put(dev);  // Don't forget to release the device reference
    return -ENOMEM;  // Return error if allocation fails
}

// Set the device for the skb
skb->dev = dev;

// Add data to the skb (for example, copying 5 bytes of "Hello" message)
memcpy(skb_put(skb, 5), "Hello", 5);

// Queue the skb for transmission
int result = dev_queue_xmit(skb);
if (result < 0) {
    pr_err("Failed to queue the packet for transmission\n");
    kfree_skb(skb);  // Free the skb if queuing failed
    dev_put(dev);
    return result;  // Return the error code
}
pr_info("Packet successfully queued for transmission\n");
dev_put(dev);
kfree_skb(skb);  // Free the skb after transmission
return 0;  // Success

    /*
    int result = dev_queue_xmit(frame->skb);

if (result < 0) {
    switch (result) {
        case -ENOMEM:
            pr_err("Error (-12): Not enough memory to queue the packet\n");
            break;
        case -ENOBUFS:
            pr_err("Error (-105): No buffer space available in the transmit queue\n");
            break;
        case -EAGAIN:
            pr_err("Error (-11): Temporary failure, try again later\n");
            break;
        case -EPERM:
            pr_err("Error (-1): Permission denied to send the packet\n");
            break;
        case -ENETDOWN:
            pr_err("Error (-100): Network device is down\n");
            break;
        case -EIO:
            pr_err("Error (-5): I/O error during packet transmission\n");
            break;
        case -EINVAL:
            pr_err("Error (-22): Invalid argument passed to dev_queue_xmit\n");
            break;
        default:
            pr_err("Unknown error occurred: %d\n", result);
    }
    return result;  // Return the error code to the caller
} else {
    pr_info("Packet successfully queued for transmission\n");
}
    */
    return result;
}

static int __init wms_init(void){
    IsServerClose = false;
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