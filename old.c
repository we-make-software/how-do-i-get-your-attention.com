#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/kthread.h>
#include <linux/uuid.h>
#include <linux/slab.h>
#include <linux/ethtool.h>
#include <linux/netdevice.h> 
#include <linux/kthread.h>
#include <linux/uuid.h>
#include <linux/hrtimer.h>
#include <linux/ktime.h>
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



static int Reader(Buffer*In,NetworkConnection*connection,struct packet_type*pt,struct net_device*orig_dev){

    return NET_RX_SUCCESS;
}

static struct packet_type PacketTypeSettings={.type=htons(ETH_P_ALL),.func=Reader,.ignore_outgoing = 1};
static int __init wms_init(void){
        NetworkHardware*networkHardware;
        for_each_netdev(&init_net, networkHardware)
        if(!strncmp(networkHardware->name,"eth",3)&&networkHardware->ethtool_ops&&networkHardware->ethtool_ops->get_link_ksettings){
            struct ethtool_link_ksettings ksettings;
            if (!networkHardware->ethtool_ops->get_link_ksettings(networkHardware,&ksettings)&&ksettings.base.speed&&ksettings.base.speed!=UINT_MAX){
              
            }
        }
    dev_add_pack(&PacketTypeSettings);
    return 0;
}
module_init(wms_init);
static void __exit wms_exit(void){
    dev_remove_pack(&PacketTypeSettings);
}

module_exit(wms_exit);
MODULE_LICENSE("GPL");
MODULE_AUTHOR("we-make-software.com");
MODULE_DESCRIPTION("WeMakeSoftware Kernel Network");
MODULE_VERSION("1.0");