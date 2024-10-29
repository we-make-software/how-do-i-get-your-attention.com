#include <linux/module.h>
#include <linux/kernel.h>
#define Info(fmt, ...) printk(KERN_INFO "[INFO] %s: " fmt "\n", __func__, ##__VA_ARGS__)
#define Warning(fmt, ...) printk(KERN_WARNING "[WARNING] %s: " fmt "\n", __func__, ##__VA_ARGS__)
#define Error(fmt, ...) printk(KERN_ERR "[ERROR] %s: " fmt "\n", __func__, ##__VA_ARGS__)
#define FrameSuccess NET_RX_DROP   
#define FrameIgnore NET_RX_SUCCESS
#define FrameError -EIO
#include <linux/netdevice.h>
#include <linux/skbuff.h>
#include <linux/sysinfo.h> 
#include <linux/delay.h>
#include <linux/gfp.h>  
void* waitForMemory(unsigned long memoryRequiredBytes);
int SendFrame(int id, int size, unsigned char *data);


unsigned char ReceiveFrameZeroes[14] = {0}
static int ReceiveFrame(int id,int size,unsigned char *data) {
    if (size > 14&&memcmp(data, ReceiveFrameZeroes, 14) != 0) {


    }

    return FrameIgnore;
}


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
void waitForMemoryIsAvailable(unsigned long memoryRequiredBytes);
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
int SendFrame(int id, int size, unsigned char *data) {
    waitForMemoryIsAvailable(size);
    struct sk_buff* skb = alloc_skb(size, GFP_ATOMIC);
    while (!skb) {
        waitForMemoryIsAvailable(size); 
        skb = alloc_skb(size, GFP_ATOMIC); 
    }
    skb->dev = dev_get_by_index(&init_net, id);
    memcpy( skb_put(skb, size), data, size);
    for (int attempts = 0; attempts < 100; attempts++) {
        if (dev_queue_xmit(skb) >= 0) {
            dev_put(skb->dev);
            return FrameSuccess; 
        }
        udelay(1); 
    }
    kfree_skb(skb); 
    dev_put(skb->dev);
    return FrameError;
}
static int ethhdr_handlerIn(struct sk_buff *skb, struct net_device *dev,struct packet_type *pt, struct net_device *orig_dev) {
    return ReceiveFrame(dev->ifindex, skb->len, skb_mac_header(skb));
}
static struct packet_type wms_proto;
static int __init wms_init(void) {
    wms_proto.type = htons(ETH_P_ALL);
    wms_proto.func = ethhdr_handlerIn;
    dev_add_pack(&wms_proto);
    return 0;
}
static void __exit wms_exit(void) {
    dev_remove_pack(&wms_proto);
}
module_init(wms_init);
module_exit(wms_exit);
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Pirasath Luxchumykanthan");
MODULE_DESCRIPTION("WeMakeSoftware Kernel Network");
MODULE_VERSION("1.0");