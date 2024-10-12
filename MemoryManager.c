#include "MemoryManager.h"
#include "MemoryManagerInit.h"
char* globalWMS;
char* GetWMS(void){
   return globalWMS;
}
EXPORT_SYMBOL(GetWMS);
void MemoryManagerInit(){
    globalWMS = waitForMemory(4096); 
    printk(KERN_INFO "MemoryManagerInit: ddf memory manager.\n");
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