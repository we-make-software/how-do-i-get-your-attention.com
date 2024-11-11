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
MODULE_INFO_SETUP("Pirasath Luxchumykanthan","WeMakeSoftware RFC826","1.0");