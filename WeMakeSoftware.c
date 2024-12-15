#include <linux/module.h>
#include <linux/init.h>
#include <linux/netdevice.h>
#include <linux/skbuff.h>
#include <linux/workqueue.h>
#include <linux/slab.h>
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
// use for :
// - AddDefaultMacAddressesReturnPointer
// - AddDefaultMacAddresses
unsigned char*ServerMacAddress,*RouterMacAddress;
// Frees the socket buffer, providing a clearer and more intuitive name
static void FreeSocketBuffer(struct sk_buff*skb) {
    kfree_skb(skb);
}
// Destruct the socket buffer
static void DestructSocketBuffer(struct sk_buff*skb) {
    struct skb_shared_info *shinfo = skb_shinfo(skb);
    for (int i = 0; i < shinfo->nr_frags; i++) 
        if (shinfo->frags[i].bv_page)
        kfree(page_address(shinfo->frags[i].bv_page) + shinfo->frags[i].bv_offset);

}
// Sets the destructor for the socket buffer to NULL, effectively disabling it.
static void DeactivateDestructSocketBuffer(struct sk_buff*skb){
    if (skb)skb->destructor=NULL;
}
// Create a new socket buffer that align whit memory allocation
static struct sk_buff* NewSocketBuffer(struct net_device *dev) {
    struct sk_buff *skb = alloc_skb(0, GFP_KERNEL);
    if (!skb)return NULL; 
    skb->destructor=DestructSocketBuffer;
    skb->dev=dev;
    return skb;
}
//Add data to the socket buffer
static void AddToSocketBuffer(struct sk_buff*skb,unsigned char*data,unsigned int len) {
    struct skb_shared_info*shinfo=skb_shinfo(skb);
    shinfo->frags[shinfo->nr_frags].bv_page = virt_to_page(data);
    shinfo->frags[shinfo->nr_frags].bv_offset = offset_in_page(data);
    shinfo->frags[shinfo->nr_frags].bv_len = len;
    shinfo->nr_frags++;
    skb->len += len;
}
static void AddMacAddresses(struct sk_buff*skb,unsigned char*destination,unsigned char*source) {
    AddToSocketBuffer(skb,destination,6);
    AddToSocketBuffer(skb,source,6);
}
//Step2: Add the EtherType to the socket buffer
static void AddEtherType(struct sk_buff*skb,unsigned char*type) {
    AddToSocketBuffer(skb,type,2);
}
static void AddEtherTypeByIEEE802(struct sk_buff*skb,unsigned char*ieee802){
    AddEtherType(skb,ieee802+12);
}
static void AddEtherTypeByUint16(struct sk_buff *skb, uint16_t type) {
    AddEtherType(skb,(unsigned char[]){(type>>8)&255,type&255});
}

//We already know its for DNS in RFC 768
static void RFC768RFC1035Reader(struct sk_buff*skb,unsigned char*networkLayer,unsigned char*transportLayer,unsigned char*payload){
    pr_info("RFC9293RFC1035Reader: Processing DNS packet\n");
}
//We already know its for DNS in RFC 9293
static void RFC9293RFC1035Reader(struct sk_buff*skb,unsigned char*networkLayer,unsigned char*transportLayer,unsigned char*payload){
    pr_info("RFC9293RFC1035Reader: Processing DNS packet\n");
}
static struct workqueue_struct*BackgroundApplicationLayerWorkQueue;
struct BackgroundApplicationLayer{
    struct work_struct work;
    struct sk_buff*skb;
    unsigned char*networkLayer,*transportLayer,*payload;
    void (*callback)(struct sk_buff*,unsigned char*,unsigned char*,unsigned char*);
};
static void RunBackgroundApplicationLayerTask(struct work_struct*work) {
    struct BackgroundApplicationLayer*task=container_of(work,struct BackgroundApplicationLayer,work);
    task->callback(task->skb,task->networkLayer,task->transportLayer, task->payload);
    kfree(task);
}
static void StartBackgroundApplicationLayerTask(
    void (*callback)(struct sk_buff*,unsigned char*,unsigned char*,unsigned char*),
    struct sk_buff*skb,
    unsigned char*networkLayer,
    unsigned char*transportLayer,
    unsigned char*payload
    ){
    struct BackgroundApplicationLayer*task=kmalloc(sizeof(struct BackgroundApplicationLayer),GFP_KERNEL);
    if (!task)return;
    INIT_WORK(&task->work,RunBackgroundApplicationLayerTask);
    task->skb=skb;
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
    unsigned char*networkLayer,
    unsigned char* rfc768Next
    ){
    switch ((rfc768Next[2] << 8) | rfc768Next[3]){
        case 53:
        {         
            StartBackgroundApplicationLayerTask(RFC768RFC1035Reader,skb,networkLayer,rfc768Next,rfc768Next+8);
            return NET_RX_DROP;
        }
        default:return NET_RX_SUCCESS;
    }
}
// Incoming : RFC 9293
// Useful links:
// - https://www.iana.org/assignments/service-names-port-numbers/service-names-port-numbers.xhtml
// - https://www.rfc-editor.org/rfc/rfc9293.html
static int RFC9293Reader(
    struct sk_buff*skb,
    unsigned char*networkLayer,
    unsigned char* rfc9293Next){
      switch ((rfc9293Next[2] << 8) | rfc9293Next[3]){
        case 53:
        {
            StartBackgroundApplicationLayerTask(RFC9293RFC1035Reader,skb,networkLayer,rfc9293Next,rfc9293Next+((rfc9293Next[12]>>4)*4));
            return NET_RX_DROP;
        }
        default:return NET_RX_SUCCESS;
    }
}
// Incoming : RFC 8200
// Useful links:
// - https://www.iana.org/assignments/protocol-numbers/protocol-numbers.xhtml
// - https://www.rfc-editor.org/rfc/rfc8200.html
// We only focus on public network addresses.
// There many protocol numbers, but we only care about 6 (rfc9293) and 17 (RFC768).
static int RFC8200Reader(
    struct sk_buff*skb,
    unsigned char*rfc8200Next
    ){
    return(rfc8200Next[8]==252||rfc8200Next[8]==253||(rfc8200Next[8]==254&&(rfc8200Next[9]>=128&&rfc8200Next[9]<=191))||rfc8200Next[8]==255)?NET_RX_SUCCESS:rfc8200Next[6]==6?RFC9293Reader(skb,rfc8200Next,rfc8200Next+40):rfc8200Next[6]==17?RFC768Reader(skb,rfc8200Next,rfc8200Next+40):NET_RX_SUCCESS;
}
// Incoming : RFC 791
// Useful links:
// - https://www.iana.org/assignments/protocol-numbers/protocol-numbers.xhtml
// - https://www.rfc-editor.org/rfc/rfc791
// We only focus on public network addresses.
// There many protocol numbers, but we only care about 6 (rfc9293) and 17 (RFC768).
static int RFC791Reader(
    struct sk_buff*skb,
    unsigned char*rfc791Next
    ){
    return(rfc791Next[12]==172&&rfc791Next[13]>=16&&rfc791Next[13]<=31)||(rfc791Next[16]==172&&rfc791Next[17]>=16&&rfc791Next[17]<=31)||(rfc791Next[12]==192&&rfc791Next[13]==168)||(rfc791Next[16]==192&&rfc791Next[17]==168)||(rfc791Next[12]==127)||(rfc791Next[16]==127)||(rfc791Next[12]==169&&rfc791Next[13]==254)||(rfc791Next[16]==169&&rfc791Next[17]==254)||(rfc791Next[12]==0)||(rfc791Next[16]==0)?NET_RX_SUCCESS:rfc791Next[9]==6?RFC9293Reader(skb,rfc791Next,rfc791Next+((rfc791Next[0]&15)*4)):rfc791Next[9]==17?RFC768Reader(skb,rfc791Next,rfc791Next+((rfc791Next[0]&15)*4)):NET_RX_SUCCESS;
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
// We can make use of it by creating a global variable, which we can set `ServerMacAddress` and `RouterMacAddress`.
// If all 6 bytes are zero, then it's the local MAC address; if not, then it's the server / router MAC address.
static int RFC826Reader(
    unsigned char*ieee802DestinationMediaAccessControlAddress,
    unsigned char*ieee802SourceMediaAccessControlAddress
    ){
    if (
        ieee802DestinationMediaAccessControlAddress[5]||
        ieee802SourceMediaAccessControlAddress[5]||
        ieee802DestinationMediaAccessControlAddress[4]||
        ieee802SourceMediaAccessControlAddress[4]||
        ieee802DestinationMediaAccessControlAddress[3]||
        ieee802SourceMediaAccessControlAddress[3]||
        ieee802DestinationMediaAccessControlAddress[2]||
        ieee802SourceMediaAccessControlAddress[2]||
        ieee802DestinationMediaAccessControlAddress[1]||
        ieee802SourceMediaAccessControlAddress[1]||
        ieee802DestinationMediaAccessControlAddress[0]||
        ieee802SourceMediaAccessControlAddress[0]
        )
        for (int i=0;i<6;i++){
            ServerMacAddress[i]=ieee802DestinationMediaAccessControlAddress[i];
            RouterMacAddress[i]=ieee802SourceMediaAccessControlAddress[i];
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
    return ieee802[12]==8?ieee802[13]?RFC826Reader(ieee802,ieee802+6):RFC791Reader(skb,ieee802+14):RFC8200Reader(skb,ieee802+14);
}
static struct packet_type Gateway = {.type = htons(ETH_P_ALL),.func = IEEE802_3Reader,.ignore_outgoing = 1};
static void AddThePacketToTheNetwork(struct sk_buff*skb){
    if (skb->dev->hard_header_len > 0) {
        skb_push(skb, skb->dev->hard_header_len);
        skb_reset_mac_header(skb);
    }
    dev_queue_xmit(skb);
}
static int __init wms_init(void){
    BackgroundApplicationLayerWorkQueue = alloc_workqueue("BackgroundApplicationLayerWorkQueue", WQ_UNBOUND, 0);
    ServerMacAddress = kmalloc(6*sizeof(unsigned char),GFP_KERNEL);
    RouterMacAddress = kmalloc(6*sizeof(unsigned char),GFP_KERNEL);
    for (int i = 0; i < 6; i++) 
      ServerMacAddress[i]=RouterMacAddress[i]=0;
    dev_add_pack(&Gateway);
    return 0;
}
module_init(wms_init);
static void __exit wms_exit(void){
    dev_remove_pack(&Gateway);
    flush_workqueue(BackgroundApplicationLayerWorkQueue);
    destroy_workqueue(BackgroundApplicationLayerWorkQueue);
    kfree(ServerMacAddress);
    kfree(RouterMacAddress);
}
module_exit(wms_exit);
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Pirasath Luxchumykanthan");
MODULE_DESCRIPTION("WeMakeSoftware Kernel Network");
MODULE_VERSION("1.0");