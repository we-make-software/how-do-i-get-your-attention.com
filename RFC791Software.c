#include"RFC791Software.h"
struct RFC791Functions*RFC791;
extern int RFC791In(struct Frame*frame);
int RFC791In(struct Frame*frame){
    return RFC791->iEE802_3->WeMakeSoftware->CloseFrame(frame);
}
EXPORT_SYMBOL(RFC791In);
extern void RFC791Setup(struct IEE802_3Functions*iEE802_3);
void RFC791Setup(struct IEE802_3Functions*iEE802_3){
    if(!RFC791)RFC791=iEE802_3->WeMakeSoftware->waitForMemory(sizeof(struct RFC791Functions));
    RFC791->iEE802_3=iEE802_3;
}
EXPORT_SYMBOL(RFC791Setup);
static void __exit wms_exit(void){
    if(RFC791)kfree(RFC791);
}
module_exit(wms_exit);
MODULE_INFO_SETUP("Pirasath Luxchumykanthan","WeMakeSoftware RFC791","1.0");