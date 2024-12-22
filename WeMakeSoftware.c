#include <linux/module.h>
#include <linux/init.h>
#include <linux/netdevice.h>
#include <linux/skbuff.h>
#include <linux/workqueue.h>
#include <linux/slab.h>
//  memset(header, 0, len); // This is a good practice to clear the memory.



// Incoming : RFC 1035
// Useful links:
// - https://www.rfc-editor.org/rfc/rfc1035
// We already know its for DNS in RFC 768
// We stop the process if the length of the packet is less than 20 bytes. (We need to change this rules when we go deeper)
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
// Incoming : RFC 768
// Useful links:
// - https://www.iana.org/assignments/service-names-port-numbers/service-names-port-numbers.xhtml
// - https://www.rfc-editor.org/rfc/rfc768.html
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
// Incoming : RFC 9293
// Useful links:
// - https://www.iana.org/assignments/service-names-port-numbers/service-names-port-numbers.xhtml
// - https://www.rfc-editor.org/rfc/rfc9293.html
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
// Incoming : RFC 8200
// Useful links:
// - https://www.iana.org/assignments/protocol-numbers/protocol-numbers.xhtml
// - https://www.rfc-editor.org/rfc/rfc8200.html
// We only focus on public network addresses.
// There many protocol numbers, but we only care about 6 (rfc9293) and 17 (RFC768).
static int RFC8200Reader(
    struct sk_buff*skb,
    unsigned char*ieee802,
    unsigned char*rfc8200Next
    ){
    return(rfc8200Next[8]==252||rfc8200Next[8]==253||(rfc8200Next[8]==254&&(rfc8200Next[9]>=128&&rfc8200Next[9]<=191))||rfc8200Next[8]==255)?NET_RX_SUCCESS:rfc8200Next[6]==6?RFC9293Reader(skb,ieee802,rfc8200Next,rfc8200Next+40):rfc8200Next[6]==17?RFC768Reader(skb,ieee802,rfc8200Next,rfc8200Next+40):NET_RX_SUCCESS;
}
// Incoming : RFC 791
// Useful links:
// - https://www.iana.org/assignments/protocol-numbers/protocol-numbers.xhtml
// - https://www.rfc-editor.org/rfc/rfc791
// We only focus on public network addresses.
// There many protocol numbers, but we only care about 6 (rfc9293) and 17 (RFC768).
static int RFC791Reader(
    struct sk_buff*skb,
    unsigned char*dataLinkLayer,
    unsigned char*rfc791Next
    ){
    return(rfc791Next[12]==172&&rfc791Next[13]>=16&&rfc791Next[13]<=31)||(rfc791Next[16]==172&&rfc791Next[17]>=16&&rfc791Next[17]<=31)||(rfc791Next[12]==192&&rfc791Next[13]==168)||(rfc791Next[16]==192&&rfc791Next[17]==168)||(rfc791Next[12]==127)||(rfc791Next[16]==127)||(rfc791Next[12]==169&&rfc791Next[13]==254)||(rfc791Next[16]==169&&rfc791Next[17]==254)||(rfc791Next[12]==0)||(rfc791Next[16]==0)?NET_RX_SUCCESS:rfc791Next[9]==6?RFC9293Reader(skb,dataLinkLayer,rfc791Next,rfc791Next+((rfc791Next[0]&15)*4)):rfc791Next[9]==17?RFC768Reader(skb,dataLinkLayer,rfc791Next,rfc791Next+((rfc791Next[0]&15)*4)):NET_RX_SUCCESS;
}
// Incoming : RFC 826 
// Sender : IEEE802_3Reader
// Useful links:
// - https://www.rfc-editor.org/rfc/rfc826
// - https://www.iana.org/assignments/arp-parameters/arp-parameters.xhtml
//
// Note : Do not return NET_RX_DROP in the RFC826 handling step, as it will prevent 
// other software from obtaining the MAC address. Only listen to ARP requests.
//
// This function ensures compliance with the rule that we do not deny the packet; 
// we simply return NET_RX_SUCCESS by default. 
//
// What can we use this for? Not much, except if the router restarts or undergoes new configuration changes.
// We can make use of it by creating a global variable, which we can set `RouterMacAddress`.
// If all 6 bytes are zero, then it's the local MAC address; if not, then it's the router MAC address.
static int RFC826Reader(
    struct sk_buff*skb,
    unsigned char*ieee802SourceMediaAccessControlAddress
    ){

    return NET_RX_SUCCESS;
}
// Incoming : IEEE 802.3
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