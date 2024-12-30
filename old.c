//Mix of standard linux kernel headers and custom headers
#include <linux/module.h>
#include <linux/init.h>
#include <linux/netdevice.h>
#include <linux/skbuff.h>
#include <linux/slab.h>
#include <linux/ethtool.h>
#include <linux/mutex.h>
#include <linux/kthread.h>
#include <linux/uuid.h>
#include <linux/hrtimer.h>
#include <linux/ktime.h>
#include <linux/delay.h>
#include <linux/sched.h>
#include <linux/jiffies.h>
#include <linux/pid.h>
#include <linux/reboot.h>
// Designed for developers to print the values of the packet data.
typedef struct sk_buff Buffer;
typedef unsigned char Byte;
typedef struct net_device NetworkHardware;

typedef struct mutex Mutex;
typedef struct task_struct Thread;
static inline void PrintBinary(const Byte*title,Byte*bytes,int size) {
    pr_info("WMS: %s: ", title); 
    for (int i = 0; i < size; i++) {
        for (int bit = 7; bit >= 0; bit--) 
             pr_cont("%d", (bytes[i] >> bit) & 1);
        pr_cont(" "); 
    }
    pr_cont("\n");
}
#define ThreadFunction(Name,Struct)typedef struct Thread##Name##Wrapper{Thread*task;Struct*object;}Thread##Name##Wrapper;static void Thread##Name##Reader(Thread*task,Struct*object);static int Thread##Name##Bind(void*arg){Thread##Name##Wrapper*wrapper=(Thread##Name##Wrapper*)arg;set_current_state(TASK_INTERRUPTIBLE);Thread##Name##Reader(wrapper->task, wrapper->object);kfree(wrapper); return 0;}static void Thread##Name(Struct*object){Thread##Name##Wrapper*_wrapper=kmalloc(sizeof(Thread##Name##Wrapper),GFP_KERNEL);if(!_wrapper){Thread##Name##Reader(NULL,object);return;}_wrapper->object=object;uuid_t uuid;char uuid_str[UUID_STRING_LEN];uuid_gen(&uuid);snprintf(uuid_str, UUID_STRING_LEN,"%02x%02x%02x%02x_%02x%02x_%02x%02x_%02x%02x_%02x%02x%02x%02x%02x%02x",uuid.b[0],uuid.b[1],uuid.b[2],uuid.b[3],uuid.b[4],uuid.b[5],uuid.b[6],uuid.b[7],uuid.b[8],uuid.b[9],uuid.b[10],uuid.b[11],uuid.b[12],uuid.b[13],uuid.b[14],uuid.b[15]);_wrapper->task=kthread_run(Thread##Name##Bind,_wrapper, uuid_str);if(IS_ERR(_wrapper->task)){Thread##Name##Reader(NULL,object);kfree(_wrapper);return;}}static void Thread##Name##Reader(Thread*task,Struct*object) 

// Read to learn!

struct InternetProtocol;
typedef struct InternetProtocol{
    Byte*Address;
    struct InternetProtocol*Prev;
} InternetProtocolVersion4,InternetProtocolVersion6;

struct Gateway;
struct NetworkDevice;
typedef struct Hardware{
    struct NetworkDevice*NetworkDevice;
    Byte*MediaAccessControl;
    ktime_t Expire;
    struct Gateway*Prev;
    Thread*Task;
} Gateway;
// This is use to hold network data and the network device
typedef struct NetworkDevice{
    NetworkHardware*Hardware;
    uint32_t PacketLimitation;
    Hardware*Hardware;
    Mutex*MutexHardware;
    struct NetworkDevice*Prev;
}NetworkDevice;

NetworkDevice*NetworkDevices=NULL;


ThreadFunction(HardwareExpire,Hardware){
    object->Task=task;
    while (!kthread_should_stop()&&object&&ktime_get()<=object->Expire)
           schedule();
    if(kthread_should_stop()||!object)return;
    mutex_lock(object->NetworkDevice->MutexHardware);
    object->Task=NULL;
    if(object==object->NetworkDevice->Hardware)
        object->NetworkDevice->Hardware=object->Prev;
    else{
        Hardware*hardwarePrev;
        for(hardwarePrev=object->NetworkDevice->Hardware;hardwarePrev->Prev!=object;hardwarePrev=hardwarePrev->Prev);
        if(hardwarePrev)hardwarePrev->Prev=object->Prev;
    }
    mutex_unlock(object->NetworkDevice->MutexHardware);
    kfree(object);
}
static inline void RefreshHardware(Hardware*hardware){
    if(!hardware)return;
    mutex_lock(hardware->NetworkDevice->MutexHardware);
    hardware->Expire = ktime_add_ns(ktime_get(), 600000000000);
    mutex_unlock(hardware->NetworkDevice->MutexHardware);
}

// This is use to send data but after the data is send the data is free
static inline int SendAndFree(Buffer* Out) {
    return dev_queue_xmit(Out);
}

// This is use to hold the data of the network data even the data is send wee will be responsible to free the data if wee not use Send
static inline int Send(Buffer*Out){
    skb_get(Out);
    return dev_queue_xmit(Out);
}
static inline void SetBufferSize(Buffer*buffer,int Size){
    buffer->tail=(unsigned int)(uintptr_t)(buffer->data+(buffer->len=buffer->data_len=Size)); 
}
static inline Buffer*CreateBuffer(NetworkDevice*networkDevice){
    Buffer*Out=alloc_skb(1518,GFP_KERNEL);
    if(!Out)return NULL;
    Out->dev=networkDevice->Connection;
    memcpy(Out->data+6,networkDevice->Connection->dev_addr,6);
    Out->truesize=1518;
    Out->priority=0;
    SetBufferSize(Out,12);
    return Out;
}
// This is use to set the standard RFC826 data
static inline void CreateDefaultRFC826Buffer(Buffer*Out){
    SetBufferSize(Out,42);
    Out->data[20]=Out->data[17]=Out->data[14]=0;
    Out->data[20]=Out->data[17]=Out->data[14]=0;
    Out->data[21]=Out->data[15]=1;
    Out->data[19]=4;
    Out->data[16]=Out->data[12]=8;
    Out->data[18]=Out->data[13]=6;
   // memcpy(Out->data+22,Out->data+6,6);
   // memset(Out->data+28,0,14);
}

static inline void CreateDefaultNetworkDeviceRFC826Buffer(NetworkDevice*networkDevice){
    
   // memcpy(Out->data+22,networkDevice->Connection->dev_addr,6);
  //  memset(Out->data+28,networkDevice->Local->Address,4);
// memset(Out->data+32,0,10);
} 

static int RFC826Reader(NetworkDevice*networkDevice,Buffer*In,Byte*InBytes){
    /*
    Hardware*hardware=networkDevice->Hardware;

    for(;hardware&&memcmp(hardware->Mac,InBytes+22,6);hardware=hardware->Prev);
    
    if(!hardware){
        mutex_lock(networkDevice->MutexHardware);
        hardware=kmalloc(sizeof(Hardware),GFP_KERNEL);
        hardware->NetworkDevice=networkDevice;
       
       
        hardware->Prev=networkDevice->Hardware;
        hardware->Expire=ktime_add_ns(ktime_get(),600000000000);
        networkDevice->Hardware=hardware;
        if(memcmp(InBytes+32,networkDevice->Connection->dev_addr,6)==0){
            //make better logic
        }
        mutex_unlock(networkDevice->MutexHardware);
       // ThreadHardwareExpire(hardware);
           PrintBinary("WMS: Source mac",InBytes+6,6);
        PrintBinary("WMS: ar$sha",InBytes+22,6);
        PrintBinary("WMS: ar$spa",InBytes+28,4);
        PrintBinary("WMS: ar$tha",InBytes+32,6);
        PrintBinary("WMS: ar$tpa",InBytes+38,4);
    }
    */
    return NET_RX_SUCCESS;
}
static int RFC971Reader(NetworkDevice*networkDevice,Buffer*In,Byte*InBytes){
  //  PrintBinary("WMS: Destination",InBytes,6);
   // PrintBinary("WMS: Source",InBytes+6,6);
   // PrintBinary("WMS: Ethertype",InBytes+12,2);
    return NET_RX_SUCCESS;
} 
static int RFC8200(NetworkDevice*networkDevice,Buffer*In,Byte*InBytes){
    return NET_RX_SUCCESS;
}
// This is the data link layer reader   
static int DataLinkLayerReader(NetworkDevice*networkDevice,Buffer*In,Byte*InBytes){


    switch (InBytes[12]<<8|InBytes[13])
    {
        case 2048:return RFC971Reader(networkDevice,In,InBytes);
        case 2054:if(InBytes[14]==0&&InBytes[15]==1)return RFC826Reader(networkDevice,In,InBytes);break;
        case 34525:return RFC8200(networkDevice,In,InBytes+14);
    }
    return NET_RX_SUCCESS;
}

// This is the router to the network devices
static int Router(Buffer*In,NetworkConnection*connection,struct packet_type*pt,struct net_device*orig_dev){
    if(In->len<40) return NET_RX_SUCCESS;
    NetworkDevice *networkDevice=NetworkDevices;
    for(;networkDevice&&networkDevice->Connection!=connection;networkDevice=networkDevice->Prev);
    if(!networkDevice)return NET_RX_SUCCESS;
    Byte*InBytes=skb_mac_header(In);
    return DataLinkLayerReader(networkDevice,In,InBytes);
}
// Yes this is the place where the magic happens
static struct packet_type NetworkCardReader={.type=htons(ETH_P_ALL),.func=Router,.ignore_outgoing = 1};
static int __init wms_init(void){

    NetworkHardware*networkHardware;
    for_each_netdev(&init_net, networkHardware)
        if (!strncmp(networkHardware->name,"eth",3)&&networkHardware->ethtool_ops&&networkHardware->ethtool_ops->get_link_ksettings){
            struct ethtool_link_ksettings ksettings;
            if (!networkHardware->ethtool_ops->get_link_ksettings(networkHardware,&ksettings)&&ksettings.base.speed&&ksettings.base.speed!=UINT_MAX){
                NetworkDevice*networkDevice=kmalloc(sizeof(NetworkDevice),GFP_KERNEL);
                networkDevice->Connection=networkHardware;
                networkDevice->PacketLimitation=ksettings.base.speed*144;
                networkDevice->Prev=NetworkDevices;
                networkDevice->MutexHardware=kmalloc(sizeof(Mutex),GFP_KERNEL);

                mutex_init(networkDevice->MutexHardware);
                NetworkDevices=networkDevice;
                Buffer*Out=CreateBuffer(networkDevice);
                memset(Out->data,255,6);
                memset(Out->data+28,0,14);
                SendAndFree(Out);
            }
        }
    dev_add_pack(&NetworkCardReader);

    return 0;
}
module_init(wms_init);
static void __exit wms_exit(void){
    dev_remove_pack(&NetworkCardReader);
    for(NetworkDevice*networkDevicePrev;NetworkDevices;NetworkDevices=networkDevicePrev){
       
        mutex_lock(NetworkDevices->MutexHardware);
        for(;NetworkDevices->Hardware;NetworkDevices->Hardware=NetworkDevices->Hardware->Prev)
          kfree(NetworkDevices->Hardware);

        mutex_unlock(NetworkDevices->MutexHardware);
  
        networkDevicePrev=NetworkDevices->Prev;   
        kfree(NetworkDevices->MutexHardware);
        kfree(NetworkDevices);
    }
}
module_exit(wms_exit);
// Yes that's me/us like to join visit
// https://www.facebook.com/profile.php?id=61571087457082
MODULE_LICENSE("GPL");
MODULE_AUTHOR("we-make-software.com");
MODULE_DESCRIPTION("WeMakeSoftware Kernel Network");
MODULE_VERSION("1.0");

// Ongoing task:
// Make sure wee have setup buffer so it can be faster for the client when the get data.
