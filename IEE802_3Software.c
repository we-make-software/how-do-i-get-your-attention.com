#include"IEE802_3Software.h"
struct IEE802_3Functions*IEE802_3;
int SwapIEE802_3MacAddress(struct Frame*request,struct Frame*response);
int SwapIEE802_3MacAddress(struct Frame*request,struct Frame*response){
    if (!request||!response)return NET_RX_DROP;
    for (int i=0;i<6;i++){
        response->IEE802_3Buffer->DestinationMAC[i]=request->IEE802_3Buffer->SourceMAC[i];
        response->IEE802_3Buffer->SourceMAC[i]=request->IEE802_3Buffer->DestinationMAC[i];
    }
    return NET_RX_SUCCESS;
}
extern int RFC791In(struct Frame*frame);
extern int RFC826In(struct Frame*frame);
extern int RFC8200In(struct Frame*frame);
extern int IEE802_3In(struct Frame*frame);
int IEE802_3In(struct Frame*frame) {
    switch((frame->IEE802_3Buffer->EtherType&65280)>>8|(frame->IEE802_3Buffer->EtherType&255)<<8){
        case 2048:return RFC791In(frame);
        case 2054:return RFC826In(frame); 
        case 34525:return RFC8200In(frame);
        default: {
            printk("IEE802_3In: Missing EtherType %u in IEE802_3Software.c\n", (frame->IEE802_3Buffer->EtherType & 65280) >> 8 | (frame->IEE802_3Buffer->EtherType & 255) << 8);
            return IEE802_3->WeMakeSoftware->CloseFrame(frame);
        }
    }
}
EXPORT_SYMBOL(IEE802_3In);
extern void RFC791Setup(struct IEE802_3Functions*iEE802_3);
extern void RFC826Setup(struct IEE802_3Functions*iEE802_3);
extern void RFC8200Setup(struct IEE802_3Functions*iEE802_3);
extern void IEE802_3Setup(struct WeMakeSoftwareFunctions*weMakeSoftwareFunctions);
void IEE802_3Setup(struct WeMakeSoftwareFunctions*weMakeSoftwareFunctions) {
    IEE802_3->WeMakeSoftware=weMakeSoftwareFunctions;
    RFC791Setup(IEE802_3);
    RFC826Setup(IEE802_3);
    RFC8200Setup(IEE802_3);
}
EXPORT_SYMBOL(IEE802_3Setup);
static int __init wms_init(void) {
    IEE802_3=waitForMemory(sizeof(struct IEE802_3Functions));
    return 0;
}
module_init(wms_init);
static void __exit wms_exit(void){
    if(IEE802_3)kfree(IEE802_3);
}
module_exit(wms_exit);
MODULE_INFO_SETUP("Pirasath Luxchumykanthan","WeMakeSoftware IEE802.3","1.0");