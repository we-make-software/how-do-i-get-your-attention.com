#include "Memory.init.h"

static int __init wmsInit(void) {
    memoryInit();

    return 0;
}

static void __exit wmsExit(void) {
   memoryExit()
}

module_init(wmsInit);
module_exit(wmsExit);

MODULE_METADATA(); 