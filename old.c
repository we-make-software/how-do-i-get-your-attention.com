#include <linux/module.h>
#include <linux/init.h>
#include <linux/netdevice.h>
#include <linux/skbuff.h>
#include <linux/workqueue.h>
#include <linux/slab.h>
#include <linux/ethtool.h>
// Designed for developers to print the values of the packet data.
static void PrintBinary(const char *title, unsigned char *data, int size) {
    pr_info("%s: ", title); 
    for (int i = 0; i < size; i++) {
        for (int bit = 7; bit >= 0; bit--) 
             pr_cont("%d", (data[i] >> bit) & 1);
        pr_cont(" "); 
    }
    pr_cont("\n");
}
typedef sk_buff SkBuff;
typedef struct workqueue_struct WorkQueue;
struct 
typedef struct ThreadWorker {
    WorkQueue*workqueue;
    const char*name;
    ThreadWorker*prev; 
} ThreadWorker;
typedef struct Device{
    struct net_device*dev;
    unsigned char*RouterMac;
    uint32_t PacketLimitation;
    Device*next,*prev;
    SkBuff*FirstOut,*MiddelOut,*LastOut;
    struct mutex SetDeviceAddRouterMacMutex,GetDataLinkLayerMutex; 
    wait_queue_head_t WaitQueue; 
};
struct BackgroundExpandDevice
{
    struct work_struct work;
    struct Device*Device;
};
struct BackgroundDevice{
    struct work_struct work;
    struct net_device*dev;
    unsigned char RouterMac[6];
};
static struct WorkQueueNode*WorkQueues=NULL;
struct Device*Devices=NULL;
static void Send(Buffer*Out){
    dev_queue_xmit(Out);
}

static struct workqueue_struct*GetWorkQueue(const char*name){
    struct WorkQueueNode*node;
    for(node=WorkQueues;node&&strcmp(node->name,name);node=node->prev);
    if(node)return node->wq;
    node=kmalloc(sizeof(struct WorkQueueNode),GFP_KERNEL);
    node->name=kstrdup(name,GFP_KERNEL);
    node->wq=alloc_workqueue(name,WQ_UNBOUND|WQ_HIGHPRI|WQ_CPU_INTENSIVE,(num_online_cpus()-2)?num_online_cpus()-2:1);
    node->prev=WorkQueues;
    WorkQueues=node;
    return node->wq;
}

static void StartThread(const char*name,void(*func)(struct work_struct*),struct work_struct*work) {
    INIT_WORK(work,func);
    queue_work(GetWorkQueue(name),work);
}
/*End:Thread*/

static void ExpandDevicePacket(struct Device*Device){
    if(Device->LastOut)Device->MiddelOut=Device->LastOut;
    for (uint32_t i=0;i<Device->PacketLimitation;i++) {
        struct sk_buff*NewOut=alloc_skb(1514, GFP_KERNEL);
        NewOut->dev=Device->dev;
        memcpy(NewOut->data,Device->RouterMac,6);
        memcpy(NewOut->data+6,Device->dev->dev_addr,6);
        NewOut->len=12;
        if(Device->LastOut)Device->LastOut->next=NewOut; 
        else Device->MiddelOut->next=NewOut;
        Device->LastOut=NewOut;  
        cond_resched();   
    }
}
static void ThreadExpandDevicePacket(struct work_struct*work){
    struct BackgroundExpandDevice*BackgroundExpandDevice=container_of(work,struct BackgroundExpandDevice,work);
    ExpandDevicePacket(BackgroundExpandDevice->Device);
    kfree(BackgroundExpandDevice);
}
static SkBuff*GetDataLinkLayer(struct Device*Device) {
    mutex_lock(&Device->GetDataLinkLayerMutex);
    wait_event(Device->WaitQueue,Device->FirstOut!=NULL);
    SkBuff*Out=Device->FirstOut;
    if(Out==Device->MiddelOut){
        struct BackgroundExpandDevice*BackgroundExpandDevice=kmalloc(sizeof(struct BackgroundExpandDevice),GFP_KERNEL);
        BackgroundExpandDevice->Device=Device;
        StartThread("ExpandDevicePacket",ThreadExpandDevicePacket,&BackgroundExpandDevice->work);
    }
    Device->FirstOut=Device->FirstOut->next;
    Out->next=NULL;
    mutex_unlock(&Device->GetDataLinkLayerMutex);
    return Out;
}

static struct Device*GetDevice(struct net_device*dev){
    struct Device*Device=Devices;
    for(;Device&&Device->dev!=dev;Device=Device->prev);
    return Device;
}



static void ThreadSetDeviceAddRouterMacByDataLinkLayer(struct work_struct*work){
    struct BackgroundDevice*BackgroundDevice=container_of(work,struct BackgroundDevice,work);
        /*
    struct Device*Device=GetDevice(BackgroundDevice->dev);
    if(!Device){
        printk(KERN_INFO "Device not emty\n");
    }



    struct Device*Device=GetDevice(BackgroundDevice->dev);
    if(!Device||mutex_trylock(&Device->SetDeviceAddRouterMacMutex)){
        kfree(BackgroundDevice);
        return;
    }
    if(memcmp(Device->RouterMac,BackgroundDevice->RouterMac,6)){
        if(Device->FirstOut){
            Device->MiddelOut=Device->FirstOut;
            Device->LastOut=Device->FirstOut=NULL;
            struct sk_buff *next;
            for(Device->LastOut=Device->MiddelOut,*next;Device->LastOut;Device->LastOut=next){
                next=Device->LastOut->next;
                kfree_skb(Device->LastOut);
            }
        }
        memcpy(Device->RouterMac,BackgroundDevice->RouterMac,6);
        struct sk_buff*FirstOut=alloc_skb(1514, GFP_KERNEL);
        Device->MiddelOut=FirstOut;
        FirstOut->dev=BackgroundDevice->dev;
        memcpy(FirstOut->data,BackgroundDevice->RouterMac,6);
        memcpy(FirstOut->data+6,BackgroundDevice->dev->dev_addr,6);
        FirstOut->len=12;
        for(uint32_t i=0;i<Device->PacketLimitation;i++){
            struct sk_buff*NewOut=alloc_skb(1514, GFP_KERNEL);
            NewOut->dev=BackgroundDevice->dev;
            memcpy(NewOut->data,BackgroundDevice->RouterMac,6);
            memcpy(NewOut->data+6,BackgroundDevice->dev->dev_addr,6);
            NewOut->len=12;
            Device->MiddelOut=Device->MiddelOut->next=NewOut;
            cond_resched();
        }
        ExpandDevicePacket(Device);
        Device->FirstOut=FirstOut;
        wake_up(&Device->WaitQueue);
    }
    mutex_unlock(&Device->SetDeviceAddRouterMacMutex);
    */
    kfree(BackgroundDevice);
}
static int DataLinkLayerReader(
    SkBuff*in,
    struct net_device*dev,
    struct packet_type*pt,
    struct net_device*orig_dev
    ){
    if(in->len<40)return NET_RX_SUCCESS;
    unsigned char*Packet=skb_mac_header(in);
    if(Packet[0]&3)return NET_RX_SUCCESS;
 
    struct BackgroundDevice*BackgroundDevice=kmalloc(sizeof(struct BackgroundDevice),GFP_KERNEL);
    if(!BackgroundDevice)return NET_RX_SUCCESS;
    BackgroundDevice->dev=dev;
    memcpy(BackgroundDevice->RouterMac,Packet+6,6);
    StartThread("SetDeviceAddRouterMac",ThreadSetDeviceAddRouterMacByDataLinkLayer,&BackgroundDevice->work);
    
    return NET_RX_SUCCESS;
}
static struct packet_type Gateway={.type=htons(ETH_P_ALL),.func=DataLinkLayerReader,.ignore_outgoing = 1};
static int __init wms_init(void){
    struct net_device *dev;
    for_each_netdev(&init_net, dev)
        if (!strncmp(dev->name,"eth",3)&&dev->ethtool_ops&&dev->ethtool_ops->get_link_ksettings){
            struct ethtool_link_ksettings ksettings;
            if (!dev->ethtool_ops->get_link_ksettings(dev,&ksettings)&&ksettings.base.speed){
                struct Device*NewDevice=kmalloc(sizeof(struct Device),GFP_KERNEL);
                NewDevice->dev=dev;
                NewDevice->RouterMac=kmalloc(6,GFP_KERNEL);
                NewDevice->PacketLimitation=ksettings.base.speed*144;
                mutex_init(&NewDevice->SetDeviceAddRouterMacMutex);
                mutex_init(&NewDevice->GetDataLinkLayerMutex);
                init_waitqueue_head(&NewDevice->WaitQueue);
                NewDevice->FirstOut=NewDevice->MiddelOut=NewDevice->LastOut=NULL;
                if (NewDevice->prev=Devices)
                    Devices->prev=NewDevice;
                NewDevice->next=NULL;
                Devices=NewDevice;
            }
        }
    dev_add_pack(&Gateway);
    return 0;
}
module_init(wms_init);
static void __exit wms_exit(void){
    dev_remove_pack(&Gateway);
    /*
    for(struct WorkQueueNode*node=WorkQueues,*prev;node;node=prev){
        prev=node->prev;
        flush_workqueue(node->wq);
        destroy_workqueue(node->wq);
        kfree(node);
    }
    for (struct Device*temp=Devices;temp;temp=Devices) {
        Devices=Devices->prev;
        for (struct sk_buff*skb=temp->FirstOut,*next;skb;skb=next) {
            next=skb->next;
            kfree_skb(skb);
        }
        kfree(temp->RouterMac);
        kfree(temp);
    }
    */
}
module_exit(wms_exit);
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Pirasath Luxchumykanthan");
MODULE_DESCRIPTION("WeMakeSoftware Kernel Network");
MODULE_VERSION("1.0");

//
typedef struct NetworkDeviceWorker{
    AddThreadWorkerToStruct
    NetworkDevice NetworkDevice;
}NetworkDeviceWorker;

ThreadFunction(NetworkDeviceWorker,"NetworkDeviceWorker"){

}