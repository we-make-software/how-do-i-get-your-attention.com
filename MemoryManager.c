#include "MemoryManager.h"
#include "MemoryManagerInit.h"
WMS** globalMemoryManager;
uint64_t globalsize=0;
bool HaveMemoryManager(const char*name){
   if(globalMemoryManager)
      for (uint64_t i = 0; i < globalsize; i++) 
         if (globalMemoryManager[i]&&strcmp(globalMemoryManager[i]->name, name) == 0) 
            return true;
   return false;
}
Export(HaveMemoryManager); 
static DEFINE_MUTEX(WMSMutex);
bool RemoveMemoryManager(const char* name) {
    mutex_lock(&WMSMutex);
    if (globalMemoryManager)
        for (uint64_t i = 0; i < globalsize; i++) 
            if (globalMemoryManager[i] && strcmp(globalMemoryManager[i]->name, name) == 0) {
               if (globalMemoryManager[i]->name)
                   kfree(globalMemoryManager[i]->name); 
               if(globalMemoryManager[i]->size)
                   kfree(globalMemoryManager[i]->size); 
               if (globalMemoryManager[i]->data)
                  kfree(globalMemoryManager[i]->data);
               kfree(globalMemoryManager[i]);
               globalMemoryManager[i] = NULL;
               uint64_t resizeGlobalSize = 0;
               for (i = 0; i < globalsize; i++)
                   if (globalMemoryManager[i])
                       resizeGlobalSize++;
                if (resizeGlobalSize == 0) {
                    kfree(globalMemoryManager);  
                    globalMemoryManager = NULL;
                } else if (resizeGlobalSize != globalsize) {
                    WMS** temp = waitForMemory(resizeGlobalSize * sizeof(WMS*));
                    uint64_t c = 0;
                    for (i = 0; i < globalsize; i++)
                        if (globalMemoryManager[i])
                            temp[c++] = globalMemoryManager[i];
                    globalMemoryManager = temp;
                }
                globalsize = resizeGlobalSize;
                mutex_unlock(&WMSMutex);
                return true;
            }
    mutex_unlock(&WMSMutex);
    return false;
}
Export(RemoveMemoryManager); 
WMS* AddMemoryManager(const char*name){
   if(globalMemoryManager)
     for (uint64_t i = 0; i < globalsize; i++) 
        if (globalMemoryManager[i]&&strcmp(globalMemoryManager[i]->name, name) == 0) 
           return globalMemoryManager[i];
   mutex_lock(&WMSMutex);        
   globalsize++;        
   WMS** temp = waitForMemory(globalsize * sizeof(WMS*));
   if (globalsize > 1)
     memcpy(temp, globalMemoryManager, (globalsize - 1) * sizeof(WMS*));
   WMS* newWMS = waitForMemory(sizeof(WMS));
   newWMS->name = waitForMemory(strlen(name) + 1); 
   strncpy(newWMS->name, name, strlen(name) + 1);
   temp[globalsize - 1] = newWMS;
   globalMemoryManager = temp;
   mutex_unlock(&WMSMutex);
   return newWMS;
}
EXPORT_SYMBOL(AddMemoryManager); 
void MemoryManagerInit(){
    MemoryManagerExit();
    globalMemoryManager = NULL;
}
Export(MemoryManagerInit); 
void MemoryManagerExit() {
    if (globalMemoryManager)
        for (uint64_t i = 0; i < globalsize; i++) 
            if (globalMemoryManager[i]) {
                if (globalMemoryManager[i]->name)
                    kfree(globalMemoryManager[i]->name);
                if (globalMemoryManager[i]->size)
                    kfree(globalMemoryManager[i]->size);
                if (globalMemoryManager[i]->data)
                    kfree(globalMemoryManager[i]->data);
                kfree(globalMemoryManager[i]);
            }
    if (globalMemoryManager)
        kfree(globalMemoryManager);
    globalMemoryManager = NULL;
    globalsize = 0;
}
Export(MemoryManagerExit); 
MODULE_METADATA(); 