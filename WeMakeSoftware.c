#define WeMakeSoftwarePrivate
#include "WeMakeSoftware.h"
WMS("WeMakeSoftware", "1.0")
#include <linux/netdevice.h>
#include <linux/skbuff.h>
#include <linux/etherdevice.h>
#include <linux/netdevice.h>
#include <linux/ethtool.h>
typedef struct NetworkDevice{
    struct net_device*dev;
    struct Setting*Settings;
    struct NetworkDevice*Prev;
    Clock PacketsLimits;
    Lock LockSetting;
} *NetworkDevice;
typedef struct Setting {
    enum{Connected=1,Disconnected=0}state:1;
    BOOL Ununused:7;
    Byte Identification[6];
    Clock TimePeriodAdd,Interval;
    Opration Monitor;
    uint64_t Receive,Send;
    Lock LockReceive,LockSend;
    struct Setting*Next,*Prev;
    NetworkDevice networkDevice;
}*Setting;
typedef struct Exchange{
    uint64_t size;
    Bytes data;
    Clock StartTime,EndTime;
    Setting setting;
    enum{High=2,Middle=1,Low=0}state;
    BOOL AllowedToSend:6;
    Buffer*buffer;
}*Exchange;
static BOOL Shutdown=FALSE;
BOOL Run(void){
   return !Shutdown;
}
Export(Run);
void ThreadPowerNap(Clock Time){
    PowerNap powerNap=TimeAddNow(Time);
    ThreadPause;
    schedule_hrtimeout(&powerNap, HRTIMER_MODE_ABS);
    ThreadRunning;
}
Export(ThreadPowerNap);
void ThreadPowerWakeUp(Opration opration){
    if (opration&&opration->__state==TASK_INTERRUPTIBLE)
        wake_up_process(opration);
}
Export(ThreadPowerWakeUp);
void ThreadClose(Opration opration){
    ThreadPowerWakeUp(opration);
    if(opration&&opration->__state!=TASK_DEAD&&(opration->__state==TASK_INTERRUPTIBLE||opration->__state==TASK_RUNNING)){
        kthread_stop(opration);
        while(opration&&opration->__state!=TASK_DEAD)schedule();
    }
}
Export(ThreadClose);

BOOL SendExchange(Exchange exchange){
    if(!exchange||!exchange->buffer||!exchange->AllowedToSend)return FALSE;
    exchange->EndTime=Now;
    Buffer*buffer=alloc_skb(0, GFP_KERNEL);
    if(!buffer)return FALSE;
    for(int i=0;i<6;i++)exchange->data[i+6]=exchange->setting->Identification[i];
    buffer->dev=exchange->setting->networkDevice->dev;
    buffer->data=exchange->data;
    buffer->len=buffer->data_len=exchange->size;
    buffer->protocol=exchange->data[12]<<8|exchange->data[13];
    buffer->pkt_type=PACKET_OUTGOING;
    buffer->ip_summed=CHECKSUM_NONE;
    buffer->priority=0;
    int ret = buffer->dev->netdev_ops->ndo_start_xmit(buffer, buffer->dev);
    buffer->data=NULL;
    kfree_skb(buffer);
    exchange->AllowedToSend=FALSE;
    LOCK(exchange->setting->LockSend);
    exchange->setting->Send--;
    UNLOCK(exchange->setting->LockSend);
    return ret!=NETDEV_TX_OK?FALSE:TRUE;
}
Export(SendExchange);
BOOL RequestResendExchange(Exchange exchange){
    if(!exchange||!exchange->buffer)return FALSE;
    if(exchange->AllowedToSend)return TRUE;
    LOCK(exchange->setting->LockSend);
    exchange->setting->Send++;
    UNLOCK(exchange->setting->LockSend);  
    return exchange->AllowedToSend=TRUE;
}
Export(RequestResendExchange);
Exchange CreateExchange(Exchange exchange){
    if(!RUN||!exchange||!exchange->setting->state)return NULL;
  
    Exchange newExchange=kmalloc(sizeof(struct Exchange),GFP_KERNEL);
    if(!newExchange)return NULL;
    newExchange->data=kmalloc(1518,GFP_KERNEL);
    if(!newExchange->data){
        kfree(newExchange);
        return NULL;
    }
    LOCK(exchange->setting->LockSend);
    exchange->setting->Send++;
    UNLOCK(exchange->setting->LockSend);  
    NetworkDevice networkDevice=(newExchange->setting=exchange->setting)->networkDevice;
    exchange->state=networkDevice->PacketsLimits<exchange->setting->Send?High:(networkDevice->PacketsLimits/2)>exchange->setting->Send?Middle:Low;
    newExchange->setting=exchange->setting;
    newExchange->StartTime=Now;
    newExchange->size=newExchange->EndTime=0;
    newExchange->buffer=NULL;
    newExchange->AllowedToSend=true;
    return newExchange;
}
Export(CreateExchange);
int CancelExchange(Exchange exchange){
    if(!exchange)return NET_RX_DROP;
    if(exchange->buffer){
        kfree_skb(exchange->buffer);
        LOCK(exchange->setting->LockReceive);
        exchange->setting->Receive--;
        UNLOCK(exchange->setting->LockReceive);
        kfree(exchange);
        return NET_RX_DROP;
    }
    LOCK(exchange->setting->LockSend);
    exchange->setting->Send--;
    UNLOCK(exchange->setting->LockSend);
    kfree(exchange->data);
    kfree(exchange);
    return NET_RX_DROP;
}
Export(CancelExchange); 
int UnknownExchange(Exchange exchange){
    if(!exchange)return NET_RX_SUCCESS;
    LOCK(exchange->setting->LockReceive);
    exchange->setting->Receive--;
    UNLOCK(exchange->setting->LockReceive);
    kfree(exchange);
    return NET_RX_SUCCESS;
}
Export(UnknownExchange);
static ExchangePacketReader Incoming=NULL;
void InitWeMakeSoftware(ExchangePacketReader exchangePacketReader){
    if(Incoming)return;
    Incoming=exchangePacketReader;
}
Export(InitWeMakeSoftware);
void ExitWeMakeSoftware(void){
    if(!Incoming)return;
    Incoming=NULL;
}
Export(ExitWeMakeSoftware);
int SkipExchange(Exchange exchange){
    if(!exchange||!exchange->buffer)return NET_RX_SUCCESS;
    LOCK(exchange->setting->LockReceive);
    exchange->setting->Receive--;
    UNLOCK(exchange->setting->LockReceive);
    kfree(exchange);
    return NET_RX_SUCCESS;
}
Export(SkipExchange);
static Setting CompareIdentificationSetting(Setting settings,Bytes Identification){
    if(!settings)return NULL;   
    for(;settings&&settings->Identification[5]==Identification[5]&&settings->Identification[4]==Identification[4]&&settings->Identification[3]==Identification[3]&&settings->Identification[2]==Identification[2]&&settings->Identification[1]==Identification[1]&&settings->Identification[0]==Identification[0];settings=settings->Prev);
    return settings;
}
static int MonitorSetting(void*data){
    ThreadRunning;
    Setting setting=data;
    while(ThreadIsRunning&&setting&&setting->state)
         ThreadPowerNap(TimeByFuture(0,setting->TimePeriodAdd));
    setting->state=Disconnected;
    while (!setting->Send)
          ThreadPowerNap(TimeAddNowSeconds(10));
    kfree(setting);
    ThreadStop;
}
NetworkDevice networkDevices;
static int PacketReader(struct sk_buff *skb, struct net_device *dev, struct packet_type *pt, struct net_device *orig_dev)
{
    if(!Incoming||skb->data_len<40)return NET_RX_SUCCESS;
    NetworkDevice detect; 
    for(detect=networkDevices;detect;detect=detect->Prev)
        if(detect->dev==dev)break;
    if(!detect)return NET_RX_SUCCESS;
    const Bytes data=skb->data;
    const Bytes Identification=data+6;
    Setting setting=detect->Settings;
    for(;setting&&CompareIdentificationSetting(setting,Identification);setting=setting->Prev);
    if(RUN&&!setting){
       LOCK(detect->LockSetting);
       if(!setting&&!(setting=CompareIdentificationSetting(setting,Identification))){
            setting=kmalloc(sizeof(struct Setting),GFP_KERNEL);
            for(int i=0;i<6;i++)setting->Identification[i]=Identification[i];
            setting->state=Connected;
            setting->Next=NULL;
            setting->Ununused=FALSE;
            setting->Monitor=Thread(MonitorSetting,setting,"WeMakeSoftware: MonitorSetting");
            setting->Interval=Now;
            setting->TimePeriodAdd=TimeAddNow(10);
            setting->Prev=detect->Settings;
            setting->networkDevice=detect;
            InitLock(setting->LockReceive);
            InitLock(setting->LockSend);
            setting->Send=setting->Receive=0;
            if(detect->Settings)
                detect->Settings->Next=setting;
            detect->Settings=setting;
       }
       UNLOCK(detect->LockSetting);
       if(Shutdown){
        setting->state=Disconnected;   
        ThreadClose(setting->Monitor);
        return NET_RX_SUCCESS;
       }
    }
    if(!setting)return NET_RX_SUCCESS;
    Exchange exchange=kmalloc(sizeof(struct Exchange),GFP_KERNEL);
    if(!exchange)return NET_RX_SUCCESS;
    exchange->data=data;
    exchange->setting=setting;
    exchange->StartTime=Now;
    exchange->EndTime=TimeByFuture(0,10);
    exchange->buffer=skb;
    LOCK(setting->LockReceive);
    setting->Receive++;
    UNLOCK(setting->LockReceive);
    exchange->AllowedToSend=false;
    exchange->state=Low;
    exchange->size=skb->data_len;
    return Incoming(exchange);
}
static struct packet_type PacketTypeSettings={.type=htons(ETH_P_ALL),.func=PacketReader,.ignore_outgoing = 1};
WMSInit{
    struct net_device *dev;
    for_each_netdev(&init_net, dev){
        if(!strncmp(dev->name,"eth",3)&&dev->ethtool_ops&&dev->ethtool_ops->get_link_ksettings){
            struct ethtool_link_ksettings ksettings;
            if (!dev->ethtool_ops->get_link_ksettings(dev,&ksettings)&&ksettings.base.speed&&ksettings.base.speed!=UINT_MAX){
                for(NetworkDevice networkDevice=networkDevices;networkDevice;networkDevice=networkDevice->Prev)
                    if(networkDevice->dev==dev)break;
                NetworkDevice networkDevice=kmalloc(sizeof(struct NetworkDevice),GFP_KERNEL);
                networkDevice->dev=dev;
                networkDevice->PacketsLimits=(ksettings.base.speed*87500)/1518;
                networkDevice->Prev=networkDevices;
                networkDevice->Settings=NULL;
                InitLock(networkDevice->LockSetting);
                networkDevices=networkDevice;
                printk(KERN_INFO "WeMakeSoftware: Network device added | %pM\n", dev->dev_addr);
            }
        }
    }
    dev_add_pack(&PacketTypeSettings);     
}
WMSExit{
    Shutdown=TRUE;
    for(;networkDevices;networkDevices=networkDevices->Prev){
    NetworkDevice networkDevice=networkDevices;

        Setting setting=networkDevice->Settings;
        if(setting){
            LOCK(networkDevice->LockSetting);
            if(setting){
                Setting setting=networkDevice->Settings;
                Setting TheFreeSetting=setting;
                for(;TheFreeSetting;TheFreeSetting=TheFreeSetting->Prev)
                    TheFreeSetting->state=Disconnected;
                TheFreeSetting=setting;
                for(;TheFreeSetting;TheFreeSetting=TheFreeSetting->Prev)
                    ThreadClose(TheFreeSetting->Monitor);     
            }
            UNLOCK(networkDevice->LockSetting);
        }
        kfree(networkDevice);
    }
    if(Incoming)Incoming=NULL;
    dev_remove_pack(&PacketTypeSettings);
}