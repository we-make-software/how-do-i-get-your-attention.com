#include "IEE802_3Software.h"
#include "WeMakeSoftwareExchange.h"
CloseFrameFunc WeMakeSoftwareCloseFrame = NULL;
DropFrameFunc WeMakeSoftwareDropFrame = NULL;
DropAndCloseFrameFunc WeMakeSoftwareDropAndCloseFrame = NULL;
CreateFrameFunc WeMakeSoftwareCreateFrame = NULL;
SetSizeFrameFunc WeMakeSoftwareSetSizeFrame = NULL;
SendFrameFunc WeMakeSoftwareSendFrame = NULL;
void IEE802_3Setup(CloseFrameFunc closeFrame,DropFrameFunc dropFrame,DropAndCloseFrameFunc dropAndCloseFrame,CreateFrameFunc createFrame,SetSizeFrameFunc setSizeFrame,SendFrameFunc sendFrame) {
    WeMakeSoftwareCloseFrame = closeFrame;
    WeMakeSoftwareDropFrame = dropFrame;
    WeMakeSoftwareDropAndCloseFrame = dropAndCloseFrame;
    WeMakeSoftwareCreateFrame = createFrame;
    WeMakeSoftwareSetSizeFrame = setSizeFrame;
    WeMakeSoftwareSendFrame = sendFrame;
}
EXPORT_SYMBOL(IEE802_3Setup);
int SwapIEE802_3MacAddress(struct Frame* request,struct Frame* response);
int IEE802_3Size(void){
    return 14;
}
int SwapIEE802_3MacAddress(struct Frame* request,struct Frame* response){
   if (!request || !response) return NET_RX_DROP;
   for (int i = 0; i < 6; i++) {
    response->IEE802_3Buffer->DestinationMAC[i] = request->IEE802_3Buffer->SourceMAC[i];
    response->IEE802_3Buffer->SourceMAC[i] = request->IEE802_3Buffer->DestinationMAC[i];
   }
   return NET_RX_SUCCESS;
}

int IEE802_3In(struct Frame* frame) {

    
    // Print the MAC addresses from the frame
    printk(KERN_INFO "IEE802_3In: Source MAC: %pM\n", frame->IEE802_3Buffer->SourceMAC);
    printk(KERN_INFO "IEE802_3In: Destination MAC: %pM\n", frame->IEE802_3Buffer->DestinationMAC);

    // Print the MAC address of the network card (assuming `dev` is available)
    struct net_device *dev = dev_get_by_index(&init_net, frame->id); // Replace with the correct way to get `dev`
    if (dev) {
        printk(KERN_INFO "IEE802_3In: Network card MAC: %pM\n", dev->dev_addr);
        dev_put(dev); // Release the reference to `dev`
    } else {
        printk(KERN_INFO "IEE802_3In: Network device not found\n");
    }

    return WeMakeSoftwareCloseFrame(frame);
}

EXPORT_SYMBOL(IEE802_3In);
MODULE_INFO_SETUP("Pirasath Luxchumykanthan", "WeMakeSoftware IEE802.3", "1.0");