#include"RFC8200Software.h"
struct RFC8200Functions*RFC8200;
extern int RFC8200In(struct Frame*frame);
int RFC8200In(struct Frame*frame){
    return RFC8200->iEE802_3->WeMakeSoftware->CloseFrame(frame);
}
EXPORT_SYMBOL(RFC8200In);
extern void RFC8200Setup(struct IEE802_3Functions*iEE802_3);
void RFC8200Setup(struct IEE802_3Functions*iEE802_3){
     if(!RFC8200)iEE802_3->WeMakeSoftware->waitForMemory(sizeof(struct RFC8200Functions));
    RFC8200->iEE802_3=iEE802_3;
}
EXPORT_SYMBOL(RFC8200Setup);
static void __exit wms_exit(void){
    if(RFC8200)kfree(RFC8200);
}
module_exit(wms_exit);
MODULE_INFO_SETUP("Pirasath Luxchumykanthan","WeMakeSoftware RFC8200","1.0");