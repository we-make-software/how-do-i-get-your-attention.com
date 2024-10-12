#include "MemoryManagerInit.h"
static int __init wmsInit(void) {
    MemoryManagerInit();
    return 0;
}
static void __exit wmsExit(void) {
    MemoryManagerExit();
}
module_init(wmsInit);
module_exit(wmsExit);
MODULE_METADATA(); 