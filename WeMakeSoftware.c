#include <linux/module.h>
#include <linux/init.h>
#include <linux/netdevice.h>
#include <linux/skbuff.h>
static void DestructSocketBufferr(struct sk_buff*skb) {
    struct skb_shared_info *shinfo = skb_shinfo(skb);
    for (int i = 0; i < shinfo->nr_frags; i++) {
        unsigned char *data = page_address(shinfo->frags[i].page) + shinfo->frags[i].page_offset;
        kfree(data);
    }
}
static void AddToSocketBuffer(struct sk_buff*skb,unsigned char*data,unsigned int len) {
    struct skb_shared_info *shinfo = skb_shinfo(skb);
    shinfo->frags[shinfo->nr_frags].page = virt_to_page(data);
    shinfo->frags[shinfo->nr_frags].page_offset = offset_in_page(data);
    shinfo->frags[shinfo->nr_frags].size = len;
    shinfo->nr_frags++;
    skb->len += len;
}
static struct sk_buff* NewSocketBuffer(struct net_device *dev) {
    struct sk_buff *skb = alloc_skb(0, GFP_KERNEL);
    if (!skb)return NULL; 
    skb->destructor = DestructSocketBufferr;
    skb->dev = dev;
    return skb;
}
static void FreeSocketBuffer(struct sk_buff*skb) {
    kfree_skb(skb);
}


static int PacketReader(struct sk_buff*skb,struct net_device*dev,struct packet_type*pt,struct net_device*orig_dev){



    return NET_RX_SUCCESS;
}
static struct packet_type Gateway = {.type = htons(ETH_P_ALL),.func = PacketReader,.ignore_outgoing = 1};
static int __init wms_init(void){
    dev_add_pack(&Gateway);
    return 0;
}
module_init(wms_init);
static void __exit wms_exit(void){
    dev_remove_pack(&Gateway);
}
module_exit(wms_exit);
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Pirasath Luxchumykanthan");
MODULE_DESCRIPTION("WeMakeSoftware Kernel Network");
MODULE_VERSION("1.0");