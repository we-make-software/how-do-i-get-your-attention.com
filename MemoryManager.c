#include "MemoryManager.h"
#include "MemoryManagerInit.h"
WMS* globalWMS;
WMS* GetWMS(void){
   return globalWMS;
}


EXPORT_SYMBOL(GetWMS);
void MemoryManagerInit(){
    MemoryManagerExit();
    globalWMS = waitForMemory(sizeof(WMS)); 
printk(KERN_INFO "OK\n");

}
EXPORT_SYMBOL(MemoryManagerInit); 
void MemoryManagerExit(){
  if (globalWMS) {
        kfree(globalWMS);  // Freeing memory allocated by kmalloc or waitForMemory
        printk(KERN_INFO "MemoryManagerExit: Memory freed.\n");
  }
}
EXPORT_SYMBOL(MemoryManagerExit); 
MODULE_METADATA(); 