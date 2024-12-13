#include <linux/module.h>
#include <linux/init.h>
#include <linux/netdevice.h>
#include <linux/skbuff.h>

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
static void SwapMacAddressesByIEEE802(struct sk_buff*skb,unsigned char*ieee802){
    AddMacAddresses(skb,ieee802+6,ieee802);
}
static void AddDefaultMacAddressesReturnPointer(
    struct sk_buff*skb, 
    unsigned char**CopyServerMacAddress, 
    unsigned char**CopyRouterMacAddress
) {
    *CopyServerMacAddress=kmalloc(6*sizeof(unsigned char),GFP_KERNEL);
    *CopyRouterMacAddress=kmalloc(6*sizeof(unsigned char),GFP_KERNEL);
    for (int i=0;i<6;i++){
        (*CopyServerMacAddress)[i]=ServerMacAddress[i];
        (*CopyRouterMacAddress)[i]=RouterMacAddress[i];
    }
    AddMacAddresses(skb,*CopyRouterMacAddress,*CopyServerMacAddress);
}
static void AddDefaultMacAddresses(struct sk_buff*skb){
    unsigned char*CopyServerMacAddress,*CopyRouterMacAddress;
    AddDefaultMacAddressesReturnPointer(skb,&CopyServerMacAddress,&CopyRouterMacAddress);
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
// Incoming : RFC 8200
static int RFC8200Reader(
    struct sk_buff*skb,
    unsigned char*ieee802DestinationMediaAccessControlAddress,
    unsigned char*ieee802SourceMediaAccessControlAddress,
    unsigned char*ieee802EtherType,
    unsigned char*rfc8200Next
    ){
        
    return NET_RX_SUCCESS;
}
// Incoming : RFC 791
static int RFC791Reader(
    struct sk_buff*skb,
    unsigned char*ieee802DestinationMediaAccessControlAddress,
    unsigned char*ieee802SourceMediaAccessControlAddress,
    unsigned char*ieee802EtherType,
    unsigned char*rfc791Next
    ){

    return NET_RX_SUCCESS;
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
// For the Hardware Type, we only accept `1`, which represents Ethernet (10Mb). 
// Although the name might be confusing, it is the standard type designation. 
// Note that even for modern gigabit Ethernet, the Hardware Type remains `1`, as it 
// is a legacy naming convention.
// 
// The Protocol Address Space is the EtherType, which is always 2048 for IPv4.
//
// The Hardware Address Length is the length of the MAC address, which is 6 bytes.
//
// The Protocol Address Length is the length of the IPv4 address, which is 4 bytes.
//
// When we talk about the Opcode, we are referring to the ARP operation. 
// But in the case of a server, it would be unusual for the server to send an ARP request to the router for its own address. 
// The router already knows the mapping of IP to MAC address.
//
// The Sender Hardware Address is the MAC address of the sender.
//
// The Sender Protocol Address is the IP address of the sender.
//
// The Target Hardware Address is the MAC address of the target.
//
// The Target Protocol Address is the IP address of the target.
//
//    unsigned char*rfc826HardwareType=rfc826Next;
//    unsigned char*rfc826ProtocolAddressSpace=rfc826Next+2;
//    unsigned char*rfc826HardwareAddressLength=rfc826Next+4;
//    unsigned char*rfc826ProtocolAddressLength=rfc826Next+5;   
//    unsigned char*rfc826Opcode=rfc826Next+6;
//    unsigned char*rfc826SenderHardwareAddress=rfc826Next+8;
//    unsigned char*rfc826SenderProtocolAddress=rfc826Next+14;
//    unsigned char*rfc826TargetHardwareAddress=rfc826Next+18;
//    unsigned char*rfc826TargetProtocolAddress=rfc826Next+24;
//
// What can we use this for? Not much, except if the router restarts or undergoes new configuration changes.
// We can make use of it by creating a global variable, which we can set `ServerMacAddress` and `RouterMacAddress`.
// If all 6 bytes are zero, then it's the local MAC address; if not, then it's the server / router MAC address.
static int RFC826Reader(
    struct sk_buff*skb,
    unsigned char*ieee802DestinationMediaAccessControlAddress,
    unsigned char*ieee802SourceMediaAccessControlAddress,
    unsigned char*ieee802EtherType,
    unsigned char*rfc826Next
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
// By default, it needs to be more than 32 bytes because RFC 791 (IPv4) is at least 20 bytes,
// and IEEE 802.3 is 14 bytes, totaling 34 bytes. The global EtherTypes are 2048 (IPv4), 
// 2054 (ARP), and 34525 (IPv6). While there can be more, these 3 are the global standards.
static int IEEE802_3Reader(
    struct sk_buff*skb,
    struct net_device*dev,
    struct packet_type*pt,
    struct net_device*orig_dev
    ){
    if(skb->len<32)return NET_RX_SUCCESS;
    unsigned char*ieee802=skb_mac_header(skb);
    return (ieee802[12]==8?(ieee802[13]?RFC791Reader:RFC826Reader):RFC8200Reader)(skb,ieee802,ieee802+6,ieee802+12,ieee802+14);
}
static struct packet_type Gateway = {.type = htons(ETH_P_ALL),.func = IEEE802_3Reader,.ignore_outgoing = 1};
static int __init wms_init(void){
    ServerMacAddress = kmalloc(6 * sizeof(unsigned char), GFP_KERNEL);
    RouterMacAddress = kmalloc(6 * sizeof(unsigned char), GFP_KERNEL);
    for (int i = 0; i < 6; i++) 
      ServerMacAddress[i]=RouterMacAddress[i]=0;
    dev_add_pack(&Gateway);
    return 0;
}
module_init(wms_init);
static void __exit wms_exit(void){
    kfree(ServerMacAddress);
    kfree(RouterMacAddress); 
    dev_remove_pack(&Gateway);
}
module_exit(wms_exit);
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Pirasath Luxchumykanthan");
MODULE_DESCRIPTION("WeMakeSoftware Kernel Network");
MODULE_VERSION("1.0");