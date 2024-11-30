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
static inline int wmsFree(const void *objp);
static inline int RegisterLink(struct Link*links,uint16_t ID,unsigned char*Data );
static struct Link*GetLinks(struct Frame*frame,uint16_t version,uint16_t section);
static inline struct Link* GetLink(struct Link* links, uint16_t ID);
static inline int CreateJob(int (*task_fn)(void *), void *data,struct Job* jobs);
static inline int RunJobs(struct Job* Jobs);







static int FrameReader(struct sk_buff*skb,struct net_device*dev,struct packet_type*pt,struct net_device*orig_dev){
    struct Frame*frame;
    if (IsServerClose||skb->len<42||dev->name[0]!=101||dev->name[1]!=116||dev->name[2]!=104||!(frame=CreateFrame(dev->ifindex))){
        kfree_skb(skb); 
        return 1; 
    }
    frame->IEE802Buffer=skb_mac_header(frame->skb=skb);
   /*
    while (frame->Status.Request!=2&& ktime_get()<frame->End)
    {
        switch (frame->Status.Request)
        {
        case 1:{
            frame->Status.Response=frame->Status.Request;
            return ITakeControl(); 
        }
        case 3:{
             frame->Status.Response=frame->Status.Request;
             return DropFrame(frame);
        }
        case 4:{
            frame->Status.Response=frame->Status.Request;
            return DropAndCloseFrame(frame);
        }
        default:
            ndelay(1);
            continue;
        }
    }*/
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
static uint64_t threadsmax = 200, threads = 0;
static inline bool isMemoryAvailable(unsigned long memoryRequiredBytes){
    if(IsServerClose||threads>threadsmax)return false;
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
    frame->Status.Response=frame->Status.Request=0;
    frame->End=frame->Start=ktime_get();
     frame->End = ktime_add(frame->End, ktime_set(10, 0));
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
static struct Link*GetLinks(struct Frame*frame,uint16_t version,uint16_t section){
    struct Standard*this; 
    for(this=frame->Standards;this&&!(this->Version==version&&this->Section==section);this=this->Previous);
    return this->Links;
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
int InitThread(void *data) {
    pr_info("Initialization thread is running!\n");



    pr_info("Initialization thread is exiting.\n");
    return 0;  // Thread exits
}
#include <linux/proc_fs.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
static uint64_t threadsmax = 200, threads = 0;
static inline int CreateThread(void *data, int (*thread_fn)(void *), const char *thread_name) {
    if(threads>threadsmax){
        thread_fn(data);
        return 1;
    }
    return !IS_ERR_OR_NULL(kthread_run(thread_fn, data, thread_name));
}
static inline int RunThread(void*data){
    struct Job*jobs=(struct Job*)data;
    if(jobs&&threads<threadsmax)threads++;
    if(!jobs)goto Exit;
    while(jobs->Previous)jobs=jobs->Previous;
    while(jobs){
        if(!jobs->JobFunction(jobs->Data)){
            ClosesJobs(jobs);
            goto Exit;
        }
        if((jobs=jobs->Next)&&jobs->Previous){
            kfree(jobs->Previous);
            jobs->Previous=NULL;
        }
    }
    Exit:
    threads--;
    return 1;
}
static inline int RunJobs(struct Job* Jobs){
    return CreateThread(Jobs,RunThread,"RunJobs");
}
static inline int ClosesJobs(struct Job*jobs){
    if (!jobs) return 1; 
    if(jobs->Next)ClosesJobs(jobs->Next);
    if(jobs->Previous)ClosesJobs(jobs->Previous);
    kfree(jobs);
    return 1;
}
static inline int CreateJob(int (*task_fn)(void *), void *data,struct Job* jobs){
    struct Job*job=waitForMemory(sizeof(struct Job));
    if(!job){
        ClosesJobs(jobs);
        return 0;
    }
    while(jobs&&jobs->Next)jobs=jobs->Next;
    job->Data=data;
    job->JobFunction=task_fn;
    job->Next=NULL;
    job->Previous=jobs;
    jobs=job;
    return 1;
}
static int __init wms_init(void){
    char buf[16] = {0};
    struct file *f= filp_open("/proc/sys/kernel/threads-max", O_RDONLY, 0);
    if (!IS_ERR(f)) {
        if (kernel_read(f,buf,sizeof(buf)-1,&pos)>0&&!kstrtoull(buf,10, &threadsmax)) 
            threadsmax = threadsmax / 2;
        filp_close(f, NULL);
    }
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