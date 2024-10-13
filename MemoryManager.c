#include "MemoryManager.h"
#include "MemoryManagerInit.h"
WMS** globalWMS;
uint64_t globalsize=0;
bool HaveWMS(const char*name){
   if(globalWMS)
      for (uint64_t i = 0; i < globalsize; i++) 
         if (globalWMS[i]&&strcmp(globalWMS[i]->name, name) == 0) 
            return true;
   return false;
}
EXPORT_SYMBOL(HaveWMS); 
static DEFINE_MUTEX(WMSMutex);
bool RemoveWMS(const char* name) {
    mutex_lock(&WMSMutex);
    if (globalWMS)
        for (uint64_t i = 0; i < globalsize; i++) 
            if (globalWMS[i] && strcmp(globalWMS[i]->name, name) == 0) {
               if (globalWMS[i]->name)
                   kfree(globalWMS[i]->name); 
               if(globalWMS[i]->size)
                   kfree(globalWMS[i]->size); 
               if (globalWMS[i]->data)
                  kfree(globalWMS[i]->data);
               kfree(globalWMS[i]);
               globalWMS[i] = NULL;
               uint64_t resizeGlobalSize = 0;
               for (i = 0; i < globalsize; i++)
                   if (globalWMS[i])
                       resizeGlobalSize++;
                if (resizeGlobalSize == 0) {
                    kfree(globalWMS);  
                    globalWMS = NULL;
                } else if (resizeGlobalSize != globalsize) {
                    WMS** temp = waitForMemory(resizeGlobalSize * sizeof(WMS*));
                    uint64_t c = 0;
                    for (i = 0; i < globalsize; i++)
                        if (globalWMS[i])
                            temp[c++] = globalWMS[i];
                    globalWMS = temp;
                }
                globalsize = resizeGlobalSize;
                mutex_unlock(&WMSMutex);
                return true;
            }
    mutex_unlock(&WMSMutex);
    return false;
}
EXPORT_SYMBOL(RemoveWMS); 
WMS* AddWMS(const char*name){
   if(globalWMS)
     for (uint64_t i = 0; i < globalsize; i++) 
        if (globalWMS[i]&&strcmp(globalWMS[i]->name, name) == 0) 
           return globalWMS[i];
   mutex_lock(&WMSMutex);        
   globalsize++;        
   WMS** temp = waitForMemory(globalsize * sizeof(WMS*));
   if (globalsize > 1)
     memcpy(temp, globalWMS, (globalsize - 1) * sizeof(WMS*));
   WMS* newWMS = waitForMemory(sizeof(WMS));
   newWMS->name = waitForMemory(strlen(name) + 1); 
   strncpy(newWMS->name, name, strlen(name) + 1);
   temp[globalsize - 1] = newWMS;
   globalWMS = temp;
   mutex_unlock(&WMSMutex);
   return newWMS;
}
EXPORT_SYMBOL(AddWMS); 
void MemoryManagerInit(){
    MemoryManagerExit();
    globalWMS = NULL;
}
EXPORT_SYMBOL(MemoryManagerInit); 
void MemoryManagerExit() {
    if (globalWMS)
        for (uint64_t i = 0; i < globalsize; i++) 
            if (globalWMS[i]) {
                if (globalWMS[i]->name)
                    kfree(globalWMS[i]->name);
                if (globalWMS[i]->size)
                    kfree(globalWMS[i]->size);
                if (globalWMS[i]->data)
                    kfree(globalWMS[i]->data);
                kfree(globalWMS[i]);
            }
    if (globalWMS)
        kfree(globalWMS);
    globalWMS = NULL;
    globalsize = 0;
}
EXPORT_SYMBOL(MemoryManagerExit); 
MODULE_METADATA(); 