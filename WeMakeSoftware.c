#define WeMakeSoftwarePrivate
#include "WeMakeSoftware.h"
WMS("WeMakeSoftware Project Handler", "1.0")
#include <linux/netdevice.h>
#include <linux/skbuff.h>
#include <linux/etherdevice.h>
#include <linux/netdevice.h>
#include <linux/ethtool.h>
#include <linux/slab.h>
#include <linux/string.h>
void ThreadPowerNap(ktime_t *PowerNap, Clock Time){
    *PowerNap = TimeAddNow(Time);
    set_current_state(TASK_UNINTERRUPTIBLE);
    schedule_hrtimeout(PowerNap, HRTIMER_MODE_ABS);
    ThreadRunning;
}
Export(ThreadPowerNap);
typedef struct NetworkDevice{
    struct net_device*dev;
    struct Setting*Setting;
    struct NetworkDevice*Prev;
} *NetworkDevice;
typedef struct Setting {
        enum{Connected=1,Disconnected=0}state:1;
        BOOL BOOLInterval:1,BOOLUnused:6;
        Byte Identification[6];
        NetworkDevice*networkDevice;
        Clock Interval,MaxTime;
        Lock LockInterval;
        Opration TaskInterval;
}*Setting;
void WeMakeSoftwareSetInterval(Setting setting){
    if(!setting->BOOLInterval)return;
    setting->BOOLInterval=FALSE;
    LOCK(setting->LockInterval);
    setting->Interval=Now;
    UNLOCK(setting->LockInterval);
    setting->BOOLInterval=TRUE;
}
Export(WeMakeSoftwareSetInterval);
void SetWeMakeSoftwareMaxTime(Setting setting, int hours, int minutes){
    Clock MaxTime=TimeByFuture(hours,minutes);
    if(MaxTime<setting->MaxTime)return;
    setting->MaxTime=MaxTime;
}
Export(SetWeMakeSoftwareMaxTime);
BOOL IsWeMakeSoftwareConnected(Setting setting){
    if(!setting->state)return FALSE;
    if(ktime_after(ktime_get(),setting->MaxTime)){
        setting->state=Disconnected;
        return FALSE;
    }
    return TRUE;
}
Export(IsWeMakeSoftwareConnected);
static int AutomatikIntervalCheck(void*data){
    Setting setting=(Setting)data;
    PowerNap powernap;
    while(setting&&setting->state){
        Clock FutureTime=TimeAddNow(setting->MaxTime);
        ThreadPowerNap(&powernap,setting->MaxTime);
        if(setting->Interval>FutureTime)continue;
        break;
    }
    if(!setting)return 0;
    setting->state=Disconnected;
    // Free other resources
    kfree(setting);
    setting=NULL;   
    return 0;
}
NetworkDevice networkDevices;
int WeMakeSoftwareSend(Setting setting,Bytes data,unsigned int length){
    CopyBytes(data+6,setting->Identification,6);
    Buffer *skb;
    skb=alloc_skb(0,GFP_KERNEL);
    skb->data=data;
    skb->tail=skb->len=length;
    skb->dev = (*setting->networkDevice)->dev;
    skb->protocol=(data[12]<<8)|data[13];
    skb_get(skb);
    int Processed=dev_queue_xmit(skb);
    skb->data=NULL;
    skb->len=0;
    kfree_skb(skb);
    return Processed;
}
Export(WeMakeSoftwareSend);
static int PacketReader(struct sk_buff *skb, struct net_device *dev, struct packet_type *pt, struct net_device *orig_dev)
{
    if(skb->data_len<40)return NET_RX_SUCCESS;
    NetworkDevice detect; 
    for(detect=networkDevices;detect;detect=detect->Prev)
        if(detect->dev==dev)break;
    if(!detect)return NET_RX_SUCCESS;
    /*
    if(!detect->Setting){
        printk(KERN_INFO "WeMakeSoftware: Network device detected, ID=%pM\n", skb->data);
        Setting setting=(detect->Setting=kmalloc(sizeof(struct Setting),GFP_KERNEL));
        *setting->networkDevice=detect;
        detect->Setting=setting;
        setting->state=Disconnected;
        CopyBytes(setting->Identification,skb->data,6);
        SetWeMakeSoftwareMaxTime(setting,0,10);
        InitLock(setting->LockInterval);
        WeMakeSoftwareSetInterval(setting);
        setting->state=Connected;
        char AutomatikIntervalCheckThreadName[64];
        snprintf(AutomatikIntervalCheckThreadName, sizeof(AutomatikIntervalCheckThreadName),"WeMakeSoftware - AutomatikIntervalCheck | %pM",setting->Identification);
        setting->TaskInterval = Thread(AutomatikIntervalCheck, setting, AutomatikIntervalCheckThreadName);
        printk(KERN_INFO "WeMakeSoftware: Network device connected | %pM\n", setting->Identification);
    }
    Setting setting=detect->Setting;
    if(!IsWeMakeSoftwareConnected(setting))return NET_RX_SUCCESS;
    WeMakeSoftwareSetInterval(setting);
    */
    return NET_RX_SUCCESS;
}
static struct packet_type PacketTypeSettings={.type=htons(ETH_P_ALL),.func=PacketReader,.ignore_outgoing = 1};
WMSInit{
    printk(KERN_INFO "WeMakeSoftware: Initializing\n");
    struct net_device *dev;
    for_each_netdev(&init_net, dev){
        if(!strncmp(dev->name,"eth",3)&&dev->ethtool_ops&&dev->ethtool_ops->get_link_ksettings){
            struct ethtool_link_ksettings ksettings;
            if (!dev->ethtool_ops->get_link_ksettings(dev,&ksettings)&&ksettings.base.speed&&ksettings.base.speed!=UINT_MAX){
                for(NetworkDevice networkDevice=networkDevices;networkDevice;networkDevice=networkDevice->Prev)
                    if(networkDevice->dev==dev)break;
                printk(KERN_INFO "WeMakeSoftware: Found network device | %pM\n", dev->dev_addr);
                NetworkDevice networkDevice=kmalloc(sizeof(struct NetworkDevice),GFP_KERNEL);
                networkDevice->dev=dev;
                networkDevice->Prev=networkDevices;
                networkDevice->Setting=NULL;
                networkDevices=networkDevice;
                printk(KERN_INFO "WeMakeSoftware: Network device added | %pM\n", dev->dev_addr);
            }
        }
    }
    dev_add_pack(&PacketTypeSettings);     
}
WMSExit{
    PowerNap powernap;
    for(NetworkDevice Prev;networkDevices;networkDevices=Prev){
        Prev=networkDevices->Prev;
        NetworkDevice networkDevice=networkDevices;
        if(!networkDevice->Setting){
            kfree(networkDevice);
            continue;
        }
        Setting setting=networkDevice->Setting;
        setting->state=Disconnected;
        printk(KERN_INFO "WeMakeSoftware: Waiting for AutomatikIntervalCheck to finish | %pM\n", setting->Identification);
        while (setting)ThreadPowerNap(&powernap,setting->Interval+setting->MaxTime);
        printk(KERN_INFO "WeMakeSoftware: AutomatikIntervalCheck completed successfully | %pM\n", setting->Identification);
        kfree(networkDevices);
    }
    printk(KERN_INFO "WeMakeSoftware: Exiting\n");
    dev_remove_pack(&PacketTypeSettings);
}