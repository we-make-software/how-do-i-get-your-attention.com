#include "WeMakeSoftware.h"
#include <linux/netfilter.h>
#include <linux/netdevice.h>
#include <linux/skbuff.h>
#include <linux/if_ether.h> 
void IEEE802_3Out(struct net_device *dev, unsigned char *data, size_t len);
void IEEE802_3In(struct Frame *frame);
void IEEE802_3In(struct Frame *frame) {
 
}



unsigned int (*hook)(void *priv, struct sk_buff *skb, const struct nf_hook_state *state);
struct Frame* CreateFrameByHook(unsigned char *data, size_t len, struct net_device *dev) {
    struct Frame *frame = waitForMemory(sizeof(struct Frame));
    frame->data = waitForMemory(len);
    memcpy(frame->data, data, len);
    frame->len = len;
    frame->dev = dev;
    return frame;
}
static unsigned int Hook(void *priv, struct sk_buff *skb, const struct nf_hook_state *state) {
    struct net_device *dev = state->in; 
    if (strncmp(dev->name, "eth", 3) != 0 || skb->len < sizeof(struct ethhdr) || !skb->data)
    return NF_ACCEPT;
    IEEE802_3In(CreateFrameByHook(skb->data, skb->len, state->in)); 
    return NF_ACCEPT; 
}

void waitForMemoryIsAvailable(unsigned long memoryRequiredBytes);
struct sk_buff* waitForMemoryForSKB(size_t len) {
    struct sk_buff *skb;
    waitForMemoryIsAvailable(len);
    skb = alloc_skb(len, GFP_KERNEL);
    while (!skb) {
        waitForMemoryIsAvailable(len);
        skb = alloc_skb(len, GFP_KERNEL);
    }

    return skb;
}
void IEEE802_3Out(struct net_device *dev, unsigned char *data, size_t len) {
    struct sk_buff *skb = waitForMemoryForSKB(len);
    skb_put(skb, len);
    memcpy(skb->data, data, len);
    skb->dev = dev;
    int CountDownReTry = 100;
    while(!dev_queue_xmit(skb))
    if (!CountDownReTry--) {
        kfree_skb(skb);
        return -1;
        }
    return 0;
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
struct sysinfo si;
bool isMemoryAvailable(unsigned long memoryRequiredBytes);
bool isMemoryAvailable(unsigned long memoryRequiredBytes) {
    si_meminfo(&si);
    unsigned long available_bytes = si.freeram * PAGE_SIZE;
    if (available_bytes > 2147483648) {
        available_bytes -= 2147483648;
        return available_bytes >= memoryRequiredBytes;
    }else return false;
}

void waitForMemoryIsAvailable(unsigned long memoryRequiredBytes) {
    while (!isMemoryAvailable(memoryRequiredBytes*2)) udelay(10);
}
void* waitForMemory(unsigned long memoryRequiredBytes){
    waitForMemoryIsAvailable(memoryRequiredBytes);
    void* _kmalloc= kmalloc(memoryRequiredBytes, GFP_KERNEL);
    while (!_kmalloc)
    {
         waitForMemoryIsAvailable(memoryRequiredBytes);
         _kmalloc= kmalloc(memoryRequiredBytes, GFP_KERNEL);
    }
    return _kmalloc;
}
EXPORT_SYMBOL(waitForMemory);
MODULE_LICENSE("GPL"); 
MODULE_AUTHOR("how-do-i-get-your-attention.com developer"); 
MODULE_DESCRIPTION("Kernel module for WeMakeSoftware services");  
MODULE_VERSION("1.0");