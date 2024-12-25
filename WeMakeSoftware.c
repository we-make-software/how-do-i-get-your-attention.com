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
#include <linux/delay.h>
// Designed for developers to print the values of the packet data.
typedef struct sk_buff Buffer;
typedef struct task_struct Thread;
typedef unsigned char Byte;
typedef struct mutex Mutex;
typedef struct net_device NetworkConnection;
static void PrintBinary(const Byte*title,Byte*bytes,int size) {
    pr_info("%s: ", title); 
    for (int i = 0; i < size; i++) {
        for (int bit = 7; bit >= 0; bit--) 
             pr_cont("%d", (bytes[i] >> bit) & 1);
        pr_cont(" "); 
    }
    pr_cont("\n");
}
#define ThreadFunction(Name,Struct)typedef struct Thread##Name##Wrapper{struct task_struct *task;Struct *object;}Thread##Name##Wrapper; static void Thread##Name##Reader(struct task_struct*,Struct*); static int Thread##Name##Bind(void*arg){Thread##Name##Wrapper*wrapper=(Thread##Name##Wrapper*)arg;Thread##Name##Reader(wrapper->task,wrapper->object);kfree(wrapper);return 0;} static void Thread##Name(Struct*object){Thread##Name##Wrapper*wrapper=kmalloc(sizeof(Thread##Name##Wrapper),GFP_KERNEL);if(!wrapper){Thread##Name##Reader(NULL,object);return;}wrapper->object=object;uuid_t uuid;char uuid_str[UUID_STRING_LEN];uuid_gen(&uuid);snprintf(uuid_str,UUID_STRING_LEN,"%02x%02x%02x%02x-%02x%02x-%02x%02x-%02x%02x-%02x%02x%02x%02x%02x%02x",uuid.b[0],uuid.b[1],uuid.b[2],uuid.b[3],uuid.b[4],uuid.b[5],uuid.b[6],uuid.b[7],uuid.b[8],uuid.b[9],uuid.b[10],uuid.b[11],uuid.b[12],uuid.b[13],uuid.b[14],uuid.b[15]);char thread_name[64];snprintf(thread_name,sizeof(thread_name),"ThreadName_%s",uuid_str);wrapper->task=kthread_run(Thread##Name##Bind,wrapper,thread_name);if(IS_ERR(wrapper->task)){Thread##Name##Reader(NULL,object);kfree(wrapper);return;}}static void Thread##Name##Reader(struct task_struct*thread,Test*object)
// Read to learn!
// This is use to hold network data and the network device
typedef struct NetworkDevice{
    NetworkConnection*Connection;
    Byte*RouterMac;
    uint32_t PacketLimitation;
    Buffer*FirstOut,*MiddelOut,*LastOut;
    Mutex Get;
    struct NetworkDevice*prev;
}NetworkDevice;
NetworkDevice*NetworkDevices=NULL;
// This is use to send data but after the data is send the data is not free
static int SendAndFree(Buffer*Out){
    return dev_queue_xmit(Out);
}
// This is use to hold the data of the network data even the data is send wee will be responsible to free the data if not wee use Send
static int Send(Buffer*Out){
    skb_get(Out);
    return dev_queue_xmit(Out);
}

ThreadFunction(InitializeNetworkDeviceHandlerBuffer,NetworkDevice){
    uint32_t MaximumCreation=object->PacketLimitation/4;

}

static int DataLinkLayerReader(NetworkDevice*networkDevice,Buffer*In,Byte*Payload){
    return NET_RX_SUCCESS;
}

static int Router(Buffer*In,NetworkConnection*connection,struct packet_type*pt,struct net_device*orig_dev){
    if(In->len<40)return NET_RX_SUCCESS;
    NetworkDevice *networkDevice=NetworkDevices;
    for(;networkDevice&&networkDevice->Connection!=connection;networkDevice=networkDevice->prev);
    if(!networkDevice)return NET_RX_SUCCESS;
    Byte*Payload=skb_mac_header(In);
    return DataLinkLayerReader(networkDevice,In,Payload);
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
                networkDevice->RouterMac=kzalloc(6,GFP_KERNEL);
                networkDevice->PacketLimitation=ksettings.base.speed*144;
                networkDevice->prev=NetworkDevices;
                networkDevice->FirstOut=networkDevice->MiddelOut=networkDevice->LastOut=NULL;
                mutex_init(&networkDevice->Get);
             
            }
        }
    dev_add_pack(&NetworkCardReader);
    return 0;
}
module_init(wms_init);
static void __exit wms_exit(void){
    dev_remove_pack(&NetworkCardReader);
    for(NetworkDevice*networkDevicePrev;NetworkDevices;NetworkDevices=networkDevicePrev){
        networkDevicePrev=NetworkDevices->prev;
        for(Buffer*bufferNext;NetworkDevices->FirstOut;NetworkDevices->FirstOut=bufferNext){
            bufferNext=NetworkDevices->FirstOut->next;
            kfree_skb(NetworkDevices->FirstOut);
        }
        kfree(NetworkDevices->RouterMac);
        kfree(NetworkDevices);
    }
}
module_exit(wms_exit);
// Yes that's me/us like to join visit
// https://www.facebook.com/profile.php?id=61571087457082
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Pirasath Luxchumykanthan");
MODULE_DESCRIPTION("WeMakeSoftware Kernel Network");
MODULE_VERSION("1.0");

// Ongoing task:
// Make sure wee have setup buffer so it can be faster for the client when the get data.