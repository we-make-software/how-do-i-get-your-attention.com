#include "MemoryManagerInit.h"
#include "StorageInit.h"
static int __init wmsInit(void) {
    MemoryManagerInit();
    StorageInit();
    return 0;
}
static void __exit wmsExit(void) {
    StorageExit();
    MemoryManagerExit();
}
module_init(wmsInit);
module_exit(wmsExit);
MODULE_METADATA(); 