#include "Memory.init.h"

static int __init wmsInit(void) {
    memoryInit();
    printk(KERN_ALERT "Hallo! Starting wms module.\n");
    return 0;
}

static void __exit wmsExit(void) {
    printk(KERN_ALERT "Goodbye! Exiting wms module.\n");
}

module_init(wmsInit);
module_exit(wmsExit);

MODULE_METADATA(); 