#include"RFC826Software.h"
struct RFC826Functions*RFC826;
extern int RFC826In(struct Frame*frame);
int RFC826In(struct Frame*frame){
    return RFC826->iEE802_3->WeMakeSoftware->CloseFrame(frame);
}
EXPORT_SYMBOL(RFC826In);
extern void RFC826Setup(struct IEE802_3Functions*iEE802_3);
void RFC826Setup(struct IEE802_3Functions*iEE802_3){
    RFC826->iEE802_3=iEE802_3;
}
EXPORT_SYMBOL(RFC826Setup);
static int __init wms_init(void) {
    RFC826=waitForMemory(sizeof(struct RFC826Functions));
    return 0;
}
module_init(wms_init);
static void __exit wms_exit(void){
    if(RFC826)kfree(RFC826);
}
module_exit(wms_exit);
MODULE_INFO_SETUP("Pirasath Luxchumykanthan","WeMakeSoftware RFC826","1.0");