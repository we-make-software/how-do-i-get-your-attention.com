#include <linux/module.h>
#include <linux/init.h>
#include <linux/netdevice.h>
#include <linux/skbuff.h>
#include <linux/workqueue.h>
#include <linux/slab.h>

static void RFC1035Reader(
    struct sk_buff*in,
    unsigned char*dataLinkLayer,
    unsigned char*networkLayer,
    unsigned char*transportLayer,
    unsigned char*applicationLayer
    ){
  

    kfree_skb(in);
}
static struct workqueue_struct*BackgroundApplicationLayerWorkQueue;
struct BackgroundApplicationLayer{
    struct work_struct work;
    struct sk_buff*skb;
    unsigned char*dataLinkLayer,*networkLayer,*transportLayer,*applicationLayer;
    void(*callback)(struct sk_buff*,unsigned char*,unsigned char*,unsigned char*,unsigned char*);
};

static void RunBackgroundApplicationLayerTask(struct work_struct*work) {
    struct BackgroundApplicationLayer*task=container_of(work,struct BackgroundApplicationLayer,work);
    task->callback(task->skb,task->dataLinkLayer,task->networkLayer,task->transportLayer, task->applicationLayer);
    kfree(task);
}
static void StartBackgroundApplicationLayerTask(
    void (*callback)(struct sk_buff*,unsigned char*,unsigned char*,unsigned char*,unsigned char*),
    struct sk_buff*skb,
    unsigned char*dataLinkLayer,
    unsigned char*networkLayer,
    unsigned char*transportLayer,
    unsigned char*applicationLayer
    ){
    struct BackgroundApplicationLayer*task=kmalloc(sizeof(struct BackgroundApplicationLayer),GFP_KERNEL);
    if (!task)return;
    INIT_WORK(&task->work,RunBackgroundApplicationLayerTask);
    task->skb=skb;
    task->dataLinkLayer=dataLinkLayer;
    task->networkLayer=networkLayer;
    task->transportLayer=transportLayer;
    task->applicationLayer=applicationLayer;
    task->callback=callback;
    queue_work(BackgroundApplicationLayerWorkQueue,&task->work);
}
static int RFC768Reader(
    struct sk_buff*skb,
    unsigned char*dataLinkLayer,
    unsigned char*networkLayer,
    unsigned char*rfc768Next
    ){
    switch ((rfc768Next[2]<<8)|rfc768Next[3]){
        case 53:
        {         
            StartBackgroundApplicationLayerTask(RFC1035Reader,skb,dataLinkLayer,networkLayer,rfc768Next,rfc768Next+8);
            return NET_RX_DROP;
        }
    }
    return NET_RX_SUCCESS;
}
static int RFC9293Reader(
    struct sk_buff*skb,
    unsigned char*dataLinkLayer,
    unsigned char*networkLayer,
    unsigned char*rfc9293Next){
    switch ((rfc9293Next[2]<<8)|rfc9293Next[3]){
        case 53:
        {
            StartBackgroundApplicationLayerTask(RFC1035Reader,skb,dataLinkLayer,networkLayer,rfc9293Next,rfc9293Next+((rfc9293Next[12]>>4)*4));
            return NET_RX_DROP;
        }
    }
    return NET_RX_SUCCESS;
}
static int RFC8200Reader(
    struct sk_buff*skb,
    unsigned char*ieee802,
    unsigned char*rfc8200Next
    ){
    return(rfc8200Next[8]==252||rfc8200Next[8]==253||(rfc8200Next[8]==254&&(rfc8200Next[9]>=128&&rfc8200Next[9]<=191))||rfc8200Next[8]==255)?NET_RX_SUCCESS:rfc8200Next[6]==6?RFC9293Reader(skb,ieee802,rfc8200Next,rfc8200Next+40):rfc8200Next[6]==17?RFC768Reader(skb,ieee802,rfc8200Next,rfc8200Next+40):NET_RX_SUCCESS;
}
static int RFC791Reader(
    struct sk_buff*skb,
    unsigned char*dataLinkLayer,
    unsigned char*rfc791Next
    ){
    return(rfc791Next[12]==172&&rfc791Next[13]>=16&&rfc791Next[13]<=31)||(rfc791Next[16]==172&&rfc791Next[17]>=16&&rfc791Next[17]<=31)||(rfc791Next[12]==192&&rfc791Next[13]==168)||(rfc791Next[16]==192&&rfc791Next[17]==168)||(rfc791Next[12]==127)||(rfc791Next[16]==127)||(rfc791Next[12]==169&&rfc791Next[13]==254)||(rfc791Next[16]==169&&rfc791Next[17]==254)||(rfc791Next[12]==0)||(rfc791Next[16]==0)?NET_RX_SUCCESS:rfc791Next[9]==6?RFC9293Reader(skb,dataLinkLayer,rfc791Next,rfc791Next+((rfc791Next[0]&15)*4)):rfc791Next[9]==17?RFC768Reader(skb,dataLinkLayer,rfc791Next,rfc791Next+((rfc791Next[0]&15)*4)):NET_RX_SUCCESS;
}
static int RFC826Reader(
    struct sk_buff*skb,
    unsigned char*ieee802SourceMediaAccessControlAddress
    ){

    return NET_RX_SUCCESS;
}
static int DataLinkLayerReader(
    struct sk_buff*skb,
    struct net_device*dev,
    struct packet_type*pt,
    struct net_device*orig_dev
    ){
    if(skb->len<40)return NET_RX_SUCCESS;
    unsigned char*ieee802=skb_mac_header(skb);
    return ieee802[12]==8?ieee802[13]?RFC826Reader(skb,ieee802+6):RFC791Reader(skb,ieee802,ieee802+14):RFC8200Reader(skb,ieee802,ieee802+14);
}
static struct packet_type Gateway={.type=htons(ETH_P_ALL),.func=DataLinkLayerReader,.ignore_outgoing = 1};
static int __init wms_init(void){
    BackgroundApplicationLayerWorkQueue=alloc_workqueue("BackgroundApplicationLayerWorkQueue",WQ_UNBOUND,0);
    dev_add_pack(&Gateway);
    return 0;
}
module_init(wms_init);
static void __exit wms_exit(void){
    dev_remove_pack(&Gateway);
    flush_workqueue(BackgroundApplicationLayerWorkQueue);
    destroy_workqueue(BackgroundApplicationLayerWorkQueue);

}
module_exit(wms_exit);
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Pirasath Luxchumykanthan");
MODULE_DESCRIPTION("WeMakeSoftware Kernel Network");
MODULE_VERSION("1.0");