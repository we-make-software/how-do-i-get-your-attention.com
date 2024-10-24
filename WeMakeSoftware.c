#include "WeMakeSoftware.h"
#include <linux/netfilter.h>
#include <linux/netdevice.h>
#include <linux/skbuff.h>
#include <linux/if_ether.h> 
void IEEE802_3_Out(struct net_device *dev, unsigned char *data, size_t len);
void IEEE802_3_In(unsigned char *data, size_t len, struct net_device *dev);
void IEEE802_3_In(unsigned char *data, size_t len, struct net_device *dev) {
    printk(KERN_INFO "Incoming Packet on device %s: length=%zu\n", dev->name, len);
    for (size_t i = 0; i < len && i < 16; i++) {
        printk(KERN_INFO "Data[%zu]: %02x\n", i, data[i]);
    }
}
unsigned int (*hook)(void *priv, struct sk_buff *skb, const struct nf_hook_state *state);
static unsigned int Hook(void *priv, struct sk_buff *skb, const struct nf_hook_state *state) {
    struct net_device *dev = state->in; 
    if (strncmp(dev->name, "eth", 3) != 0 || skb->len < sizeof(struct ethhdr) || !skb->data)
    return NF_ACCEPT;
    IEEE802_3_In(skb->data, skb->len, state->in); 
    return NF_ACCEPT; 
}
void IEEE802_3_Out(struct net_device *dev, unsigned char *data, size_t len) {
    struct sk_buff *skb;
    skb = alloc_skb(len, GFP_KERNEL);
    if (!skb)
        return; 
    skb_put(skb, len);
    memcpy(skb->data, data, len);
    skb->dev = dev; 
    dev_queue_xmit(skb); 
}
static struct nf_hook_ops nfho;
static int __init WeMakeSoftwareInit(void) {
    nfho.hooknum = NF_INET_PRE_ROUTING;
    nfho.pf = PF_INET; 
    nfho.hook = Hook;
    nf_register_net_hook(&init_net, &nfho); 
    return 0;   
}

// Function that runs when the module is removed
static void __exit WeMakeSoftwareExit(void) {
    nf_unregister_net_hook(&init_net, &nfho);
}

// Define module entry and exit points
module_init(WeMakeSoftwareInit);
module_exit(WeMakeSoftwareExit);


MODULE_LICENSE("GPL"); 
MODULE_AUTHOR("how-do-i-get-your-attention.com developer"); 
MODULE_DESCRIPTION("Kernel module for WeMakeSoftware services");  
MODULE_VERSION("1.0");