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
// Designed for developers to print the values of the packet data.
typedef struct sk_buff Buffer;
typedef unsigned char Byte;
typedef struct mutex Mutex;
typedef struct net_device NetworkConnection;
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
typedef struct _PrintDataNoRepeat{ 
    Byte*Data;
    int Size;
    struct _PrintDataNoRepeat*Prev;
}_PrintDataNoRepeat;
_PrintDataNoRepeat*_PrintDataNoRepeats=NULL;
static void PrintDataNoRepeat(const Byte*title,Byte*Data,int Size){
    _PrintDataNoRepeat*_printDataNoRepeats=_PrintDataNoRepeats;
    for(;_printDataNoRepeats&&_printDataNoRepeats->Size!=Size&&memcmp(_printDataNoRepeats->Data,Data,Size)!=0;_printDataNoRepeats=_printDataNoRepeats->Prev);
    if(!_printDataNoRepeats){
        _printDataNoRepeats=kmalloc(sizeof(_PrintDataNoRepeat),GFP_KERNEL);
        _printDataNoRepeats->Data=kmalloc(Size,GFP_KERNEL);
        memcpy(_printDataNoRepeats->Data,Data,Size);
        _printDataNoRepeats->Size=Size;
        _printDataNoRepeats->Prev=_PrintDataNoRepeats;
        _PrintDataNoRepeats=_printDataNoRepeats;
        PrintBinary(title,Data,Size);
    }   
}
static 
#define ThreadFunction(Name, Struct) typedef struct Thread##Name##Wrapper { \
        Thread*task; \
        Struct*object; \
    } Thread##Name##Wrapper; \
    \
    /* Function prototype for the thread reader */ \
    static void Thread##Name##Reader(Thread *task, Struct *object); \
    \
    /* Thread binding function that invokes the thread reader */ \
    static int Thread##Name##Bind(void *arg) { \
        Thread##Name##Wrapper *wrapper = (Thread##Name##Wrapper *)arg; \
        set_current_state(TASK_INTERRUPTIBLE);\
        /* Call the thread reader with the task and object */ \
        Thread##Name##Reader(wrapper->task, wrapper->object); \
        kfree(wrapper); /* Free the wrapper after use */ \
        return 0; \
    } \
    \
    /* Function to create and start the thread */ \
    static void Thread##Name(Struct *object) { \
        Thread##Name##Wrapper *_wrapper = kmalloc(sizeof(Thread##Name##Wrapper), GFP_KERNEL); \
        if (!_wrapper) { \
            /* If memory allocation fails, directly call the thread reader */ \
            Thread##Name##Reader(NULL, object); \
            return; \
        } \
        _wrapper->object = object; \
        /* Generate UUID and create a thread name */ \
        uuid_t uuid; \
        char uuid_str[UUID_STRING_LEN]; \
        uuid_gen(&uuid); \
        snprintf(uuid_str, UUID_STRING_LEN, "%02x%02x%02x%02x_%02x%02x_%02x%02x_%02x%02x_%02x%02x%02x%02x%02x%02x", \
            uuid.b[0], uuid.b[1], uuid.b[2], uuid.b[3], uuid.b[4], uuid.b[5], uuid.b[6], uuid.b[7], \
            uuid.b[8], uuid.b[9], uuid.b[10], uuid.b[11], uuid.b[12], uuid.b[13], uuid.b[14], uuid.b[15]); \
        /* Start the kernel thread */ \
        _wrapper->task = kthread_run(Thread##Name##Bind, _wrapper, uuid_str); \
        if (IS_ERR(_wrapper->task)) { \
            /* If thread creation fails, call the thread reader and free the wrapper */ \
            Thread##Name##Reader(NULL, object); \
            kfree(_wrapper); \
            return; \
        } \
    } \
    \
    /* Thread reader function definition (to be implemented elsewhere) */ \
    static void Thread##Name##Reader(Thread *task, Struct *object) //add { /* Implement thread-specific logic here */ } at thhis marco  ThreadFunction in the globel scope

// Read to learn!
struct Hardware;
struct NetworkDevice;
typedef struct Hardware{
    struct NetworkDevice*NetworkDevice;
    Byte*Mac,*Address;
    ktime_t Expire;
    struct Hardware*Prev;
    Thread*Task;
} Hardware;
// This is use to hold network data and the network device
typedef struct NetworkDevice{
    NetworkConnection*Connection;
    uint32_t PacketLimitation;
    Hardware*Hardware;
    Mutex MutexHardware;
    struct NetworkDevice*Prev;
}NetworkDevice;
NetworkDevice*NetworkDevices=NULL;


ThreadFunction(HardwareExpire,Hardware){
    object->Task=task;
    while (!kthread_should_stop()&&object&&ktime_get()<=object->Expire)
           schedule();
    if(kthread_should_stop()||!object)return;
    mutex_lock(&object->NetworkDevice->MutexHardware);
    object->Task=NULL;
    if(object==object->NetworkDevice->Hardware)
        object->NetworkDevice->Hardware=object->Prev;
    else{
        Hardware*hardwarePrev;
        for(hardwarePrev=object->NetworkDevice->Hardware;hardwarePrev->Prev!=object;hardwarePrev=hardwarePrev->Prev);
        if(hardwarePrev)hardwarePrev->Prev=object->Prev;
    }
    mutex_unlock(&object->NetworkDevice->MutexHardware);
    kfree(object->Mac);
    kfree(object->Address);
    kfree(object);
}
static inline void RefreshHardware(Hardware*hardware){
    if(!hardware)return;
    mutex_lock(&hardware->NetworkDevice->MutexHardware);
    if(hardware)
          hardware->Expire = ktime_add_ns(ktime_get(), 600000000000);
    mutex_unlock(&hardware->NetworkDevice->MutexHardware);
}
static inline void SetBufferSize(Buffer*buffer,int Size){
    buffer->tail=buffer->data+(buffer->len=buffer->data_len=Size);
}
static inline  Buffer *CreateBuffer(NetworkDevice*networkDevice){
    Buffer*Out=alloc_skb(1514,GFP_KERNEL);
    if(!Out)return NULL;
    Out->dev=networkDevice->Connection;
    Out->truesize=1514;
    Out->priority=0;
    return Out;
}
static int RFC826Reader(NetworkDevice*networkDevice,Buffer*In,Byte*InBytes){
    Hardware*hardware=networkDevice->Hardware;
    for(;hardware&&memcmp(hardware->Mac,InBytes+22,6);hardware=hardware->Prev);
    if(!hardware){
        mutex_lock(&networkDevice->MutexHardware);
        hardware=kmalloc(sizeof(Hardware),GFP_KERNEL);
        hardware->NetworkDevice=networkDevice;
        hardware->Mac=kmalloc(6,GFP_KERNEL);
        memcpy(hardware->Mac,InBytes+32,6);
        hardware->Address=kmalloc(4,GFP_KERNEL);
        memcpy(hardware->Address,InBytes+28,4);
        hardware->Prev=networkDevice->Hardware;
        hardware->Expire=ktime_add_ns(ktime_get(),600000000000);
        networkDevice->Hardware=hardware;
        mutex_unlock(&networkDevice->MutexHardware);
        ThreadHardwareExpire(hardware);
    }
    /*
    PrintBinary("Destination",InBytes,6);
    PrintBinary("Source",InBytes+6,6);// this is the router where wee get the data from
    PrintBinary("Ethertype",InBytes+12,2);
    PrintBinary("hrd",InBytes+14,2);// Always 1 (1=ETHERNET)
    PrintBinary("pro",InBytes+16,2);//Always 2048 (rfc791)
    PrintBinary("hln",InBytes+18,1);//Always 6 because of mac size
    PrintBinary("pln",InBytes+19,1);// Always 4 ipv4 size
    PrintBinary("op",InBytes+20,2); // 1=Request 2=Reply


    PrintBinary("sha",InBytes+22,6); // this is the hardware address of the sender 
    PrintBinary("spa",InBytes+28,4); //router ip address (because the op is 1)
    PrintBinary("tha",InBytes+32,6); // this is the hardware address of the reply
    PrintBinary("tpa",InBytes+38,4);  // this is the ip address of the reply(US)
    */
    return NET_RX_SUCCESS;
}
static int RFC971Reader(NetworkDevice*networkDevice,Buffer*In,Byte*InBytes){
    return NET_RX_SUCCESS;
} 
static int RFC8200(NetworkDevice*networkDevice,Buffer*In,Byte*InBytes){
    return NET_RX_SUCCESS;
}
// This is the data link layer reader   
static int DataLinkLayerReader(NetworkDevice*networkDevice,Buffer*In,Byte*InBytes){


    switch (InBytes[12]<<8|InBytes[13])
    {
        case 2048:return RFC971Reader(networkDevice,In,InBytes+14);
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
//Yes this is the place where the magic happens
static struct packet_type NetworkCardReader={.type=htons(ETH_P_ALL),.func=Router,.ignore_outgoing = 1};
static int __init wms_init(void){
    NetworkConnection*networkConnection;
    for_each_netdev(&init_net, networkConnection)
        if (!strncmp(networkConnection->name,"eth",3)&&networkConnection->ethtool_ops&&networkConnection->ethtool_ops->get_link_ksettings){
            struct ethtool_link_ksettings ksettings;
            if (!networkConnection->ethtool_ops->get_link_ksettings(networkConnection,&ksettings)&&ksettings.base.speed&&ksettings.base.speed!=UINT_MAX){
                NetworkDevice*networkDevice=kmalloc(sizeof(NetworkDevice),GFP_KERNEL);
                networkDevice->Connection=networkConnection;
                networkDevice->PacketLimitation=ksettings.base.speed*144;
                networkDevice->Prev=NetworkDevices;
                mutex_init(&networkDevice->MutexHardware);
                NetworkDevices=networkDevice;
            }
        }
    dev_add_pack(&NetworkCardReader);
    return 0;
}
module_init(wms_init);
static void __exit wms_exit(void){
    dev_remove_pack(&NetworkCardReader);
    for(NetworkDevice*networkDevicePrev;NetworkDevices;NetworkDevices=networkDevicePrev){
        mutex_lock(&NetworkDevices->MutexHardware);
        for(;NetworkDevices->Hardware;NetworkDevices->Hardware=NetworkDevices->Hardware->Prev){
            kthread_stop(NetworkDevices->Hardware->Task); 
            kfree(NetworkDevices->Hardware->Mac);
            kfree(NetworkDevices->Hardware->Address);
            kfree(NetworkDevices->Hardware);
        }
        mutex_unlock(&NetworkDevices->MutexHardware);
        networkDevicePrev=NetworkDevices->Prev;   
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

// This is use to send data but after the data is send the data is free
static inline int SendAndFree(Buffer*Out){
    return dev_queue_xmit(Out);
}
// This is use to hold the data of the network data even the data is send wee will be responsible to free the data if wee not use Send
static inline int Send(Buffer*Out){
    skb_get(Out);
    return dev_queue_xmit(Out);
}

