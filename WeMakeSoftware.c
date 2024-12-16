#include <linux/module.h>
#include <linux/init.h>
#include <linux/netdevice.h>
#include <linux/skbuff.h>
#include <linux/workqueue.h>
#include <linux/slab.h>


static void Send(struct sk_buff*DataLinkLayerBuffer){

    dev_queue_xmit(skb);
}


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
// Setting from : 
// - wms_init
// - wms_exit
// - RFC826Reader
unsigned char*RouterMacAddress;


static void IncreaseFragments(struct skb_shared_info*fragments,const char value) {
    fragments->nr_frags += value;
}
static skb_frag_t*GetFragmentIndex(struct skb_shared_info*fragments,const uint16_t index){
    return &fragments->frags[index];
}
static void AddOrSetFragmentData(struct skb_shared_info*fragments,const uint16_t index,unsigned char*data,const uint16_t length){
    skb_frag_t*frag=&fragments->frags[index];
    frag->bv_page=virt_to_page(data);
    frag->bv_offset=offset_in_page(data);
    frag->bv_len=length;
}
static struct sk_buff*DataLinkLayerBuffer=NULL;
// Default network device
struct net_device*NetworkDevice;
static void InitDataLinkLayerBuffer(void){
    if(DataLinkLayerBuffer)kfree_skb(DataLinkLayerBuffer);  
    DataLinkLayerBuffer=alloc_skb(0, GFP_KERNEL);
    DataLinkLayerBuffer->dev=NetworkDevice;
    struct skb_shared_info*fragments=skb_shinfo(DataLinkLayerBuffer);
    IncreaseFragments(fragments,2);.
    AddOrSetFragmentData(fragments,0,RouterMacAddress,6);
    AddOrSetFragmentData(fragments,1,NetworkDevice->dev_addr,6);
}
static struct sk_buff*CreateDataLinkLayerBuffer(void){
    return DataLinkLayerBuffer?skb_clone(DataLinkLayerBuffer,GFP_KERNEL):NULL;
}





// Incoming : RFC 768->RFC 1035
// Useful links:
// - https://www.rfc-editor.org/rfc/rfc1035
// We already know its for DNS in RFC 768
// We stop the process if the length of the packet is less than 20 bytes. (We need to change this rules when we go deeper)
static void RFC768RFC1035Reader(struct sk_buff*skb,unsigned char*dataLinkLayer,unsigned char*networkLayer,unsigned char*transportLayer,unsigned char*payload){


}
// Incoming : RFC 9293->RFC 1035
// Useful links:
// - https://www.rfc-editor.org/rfc/rfc1035
// We already know it's for DNS in RFC 9293
// We follow the same rules from RFC768RFC1035Reader (We need to change this rules when we go deeper)
static void RFC9293RFC1035Reader(struct sk_buff*skb,unsigned char*dataLinkLayer,unsigned char*networkLayer,unsigned char*transportLayer,unsigned char*payload){
  
}
static struct workqueue_struct*BackgroundApplicationLayerWorkQueue;
struct BackgroundApplicationLayer{
    struct work_struct work;
    struct sk_buff*skb;
    unsigned char*dataLinkLayer,*networkLayer,*transportLayer,*payload;
    void (*callback)(struct sk_buff*,unsigned char*,unsigned char*,unsigned char*,unsigned char*);
};
static void RunBackgroundApplicationLayerTask(struct work_struct*work) {
    struct BackgroundApplicationLayer*task=container_of(work,struct BackgroundApplicationLayer,work);
    task->callback(task->skb,task->dataLinkLayer,task->networkLayer,task->transportLayer, task->payload);
    kfree(task);
}
static void StartBackgroundApplicationLayerTask(
    void (*callback)(struct sk_buff*,unsigned char*,unsigned char*,unsigned char*,unsigned char*),
    struct sk_buff*skb,
    unsigned char*dataLinkLayer,
    unsigned char*networkLayer,
    unsigned char*transportLayer,
    unsigned char*payload
    ){
    struct BackgroundApplicationLayer*task=kmalloc(sizeof(struct BackgroundApplicationLayer),GFP_KERNEL);
    if (!task)return;
    INIT_WORK(&task->work,RunBackgroundApplicationLayerTask);
    task->skb=skb;
    task->dataLinkLayer=dataLinkLayer;
    task->networkLayer=networkLayer;
    task->transportLayer=transportLayer;
    task->payload=payload;
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
            StartBackgroundApplicationLayerTask(RFC768RFC1035Reader,skb,dataLinkLayer,networkLayer,rfc768Next,rfc768Next+8);
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
            StartBackgroundApplicationLayerTask(RFC9293RFC1035Reader,skb,dataLinkLayer,networkLayer,rfc9293Next,rfc9293Next+((rfc9293Next[12]>>4)*4));
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
    if (
        NetworkDevice!=skb->dev&&
        (ieee802SourceMediaAccessControlAddress[5]||
        ieee802SourceMediaAccessControlAddress[4]||
        ieee802SourceMediaAccessControlAddress[3]||
        ieee802SourceMediaAccessControlAddress[2]||
        ieee802SourceMediaAccessControlAddress[1]||
        ieee802SourceMediaAccessControlAddress[0])
        ){
            NetworkDevice=skb->dev;
            for(int i=0;i<6;i++)RouterMacAddress[i]=ieee802SourceMediaAccessControlAddress[i];
            InitDataLinkLayerBuffer();
        }
    return NET_RX_SUCCESS;
}
// Incoming : IEEE 802.3
//
// Note: skb->len is the length of the entire packet, including the IEEE 802.3 header.
//
// By default, it needs to be more than 40 bytes because RFC 791 (IPv4) is at least 20 bytes, and  RFC 768 (UDP) is 8 bytes,
// and IEEE 802.3 is 14 bytes, totaling 34 bytes. The global EtherTypes are 2048 (IPv4), 
// 2054 (ARP), and 34525 (IPv6). While there can be more, these 3 are the global standards.
static int IEEE802_3Reader(
    struct sk_buff*skb,
    struct net_device*dev,
    struct packet_type*pt,
    struct net_device*orig_dev
    ){
    if(skb->len<40)return NET_RX_SUCCESS;
    unsigned char*ieee802=skb_mac_header(skb);
    return ieee802[12]==8?ieee802[13]?RFC826Reader(skb,ieee802+6):RFC791Reader(skb,ieee802,ieee802+14):RFC8200Reader(skb,ieee802,ieee802+14);
}
static struct packet_type Gateway = {.type = htons(ETH_P_ALL),.func = IEEE802_3Reader,.ignore_outgoing = 1};

static int __init wms_init(void){
    BackgroundApplicationLayerWorkQueue=alloc_workqueue("BackgroundApplicationLayerWorkQueue",WQ_UNBOUND,0);
    RouterMacAddress=kmalloc(6*sizeof(unsigned char),GFP_KERNEL);
    dev_add_pack(&Gateway);
    return 0;
}
module_init(wms_init);
static void __exit wms_exit(void){
    dev_remove_pack(&Gateway);
    flush_workqueue(BackgroundApplicationLayerWorkQueue);
    destroy_workqueue(BackgroundApplicationLayerWorkQueue);
    kfree(RouterMacAddress);
    kfree_skb(DataLinkLayerBuffer);
}
module_exit(wms_exit);
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Pirasath Luxchumykanthan");
MODULE_DESCRIPTION("WeMakeSoftware Kernel Network");
MODULE_VERSION("1.0");