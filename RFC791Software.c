#include"RFC791Software.h"
struct RFC791Functions*RFC791;
extern int RFC791In(struct Frame*frame);
int RFC791In(struct Frame*frame){
    return RFC791->iEE802_3->WeMakeSoftware->CloseFrame(frame);
}
EXPORT_SYMBOL(RFC791In);
EXPORT_SYMBOL()
extern void RFC791Setup(struct IEE802_3Functions*iEE802_3);
void RFC791Setup(struct IEE802_3Functions*iEE802_3){
    RFC791->iEE802_3=iEE802_3;
}
EXPORT_SYMBOL(RFC791Setup);
static int __init wms_init(void) {
    RFC791=waitForMemory(sizeof(struct RFC791Functions));
    return 0;
}
module_init(wms_init);
static void __exit wms_exit(void){
    if(RFC791)kfree(RFC791);
}
module_exit(wms_exit);
MODULE_INFO_SETUP("Pirasath Luxchumykanthan","WeMakeSoftware RFC791","1.0");