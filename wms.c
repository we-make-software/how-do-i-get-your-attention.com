#include "Memory.h"


static int __init wmsInit(void) {

    printk(KERN_INFO "Hi from the Kernel Module!\n");
    return 0;
}

static void __exit wmsExit(void) {

    printk(KERN_INFO "Goodbye from the Kernel Module!\n");
}

module_init(wmsInit);
module_exit(wmsExit);

MODULE_METADATA(); 
