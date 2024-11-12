#include"IEE802_3Software.h"
struct IEE802_3Functions*IEE802_3;

extern int RFC791In(struct Frame*frame);
extern int RFC826In(struct Frame*frame);
extern int RFC8200In(struct Frame*frame);
extern int IEE802_3In(struct Frame*frame);
int IEE802_3In(struct Frame*frame){
    switch((frame->IEE802_3Buffer[12] << 8) | frame->IEE802_3Buffer[13]){
        case 2048:return RFC791In(frame);
        case 2054:return RFC826In(frame); 
        case 34525:return RFC8200In(frame);
        default: {
             printk("IEE802_3In: Missing EtherType %u in IEE802_3Software.c\n", (frame->IEE802_3Buffer[12] << 8) | frame->IEE802_3Buffer[13]);
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
    if(!IEE802_3)IEE802_3=weMakeSoftwareFunctions->waitForMemory(sizeof(struct IEE802_3Functions));
    IEE802_3->WeMakeSoftware=weMakeSoftwareFunctions;
    RFC791Setup(IEE802_3);
    RFC826Setup(IEE802_3);
    RFC8200Setup(IEE802_3);
}
EXPORT_SYMBOL(IEE802_3Setup);
static void __exit wms_exit(void){
    if(IEE802_3)kfree(IEE802_3);
}
module_exit(wms_exit);
MODULE_INFO_SETUP("Pirasath Luxchumykanthan","WeMakeSoftware IEE802.3","1.0");