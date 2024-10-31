#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/types.h>
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
/*Binary test*/
void PrintBytesBinary(const char *title, const unsigned char *data, int from, int to) {
    int index = 0;
    int buf_size = strlen(title) + 8 * (to - from + 1) + (to - from) + 2; // Calculate buffer size
    char *buffer = kmalloc(buf_size, GFP_KERNEL);
    if (!buffer) return;  // Handle memory allocation failure

    // Copy the title
    strcpy(buffer, title);
    index = strlen(title);
    buffer[index++] = ':';
    buffer[index++] = ' ';

    // Create binary representation
    for (int i = from; i <= to; i++) {
        for (int j = 7; j >= 0; j--) {
            buffer[index++] = ((data[i] >> j) & 1) ? '1' : '0';
        }
        buffer[index++] = ' '; // Add space after each byte
    }
    buffer[index - 1] = '\0'; // Null-terminate the string

    // Print the binary string to the kernel log
    printk(KERN_INFO "%s\n", buffer);

    kfree(buffer); // Free the allocated memory
}




/**/

//printk(KERN_INFO "Value: %d\n", value);
//printk(KERN_INFO "Value in hex: %x\n", value);
//printk(KERN_INFO "Message: %s\n", message);
static int ReceiveFrame(int id, int size, unsigned char *data) {
    if (((size >= 34 && (data[12] << 8) | data[13]==2048)||
    (size >= 54 && (data[12] << 8) | data[13]==34525))&&
    !(data[0] & 2) && 
    !(data[6] & 2) &&  
    !(data[0] | data[1] | data[2] | data[3] | data[4] | data[5]) &&
    !(data[6] | data[7] | data[8] | data[9] | data[10] | data[11]))
    switch (data[14] >> 4) { 
        case 4:  
        //https://www.rfc-editor.org/rfc/rfc791 
        break;
        case 6:  
        //https://www.rfc-editor.org/rfc/rfc8200.html)
        break;
    }

    //Data only need to be free if FrameSuccess or FrameError


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