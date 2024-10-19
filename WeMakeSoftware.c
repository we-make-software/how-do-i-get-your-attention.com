#include "WeMakeSoftware.h"
#define WeMakeSoftwareModules "tt.modules" 
static int __init WeMakeSoftwareInit(void)
{
  struct file *WeMakeSoftwareModulesFile;
    
    // Open the file using filp_open
    WeMakeSoftwareModulesFile = filp_open(WeMakeSoftwareModules, 0 , 0);
  if (IS_ERR(WeMakeSoftwareModulesFile)) {
        long err = PTR_ERR(WeMakeSoftwareModulesFile);  // Get the error code
        printk(KERN_ERR "Error: Could not open file: %s, error code: %ld\n", WeMakeSoftwareModules, err);
        return 0;  // Return the error code
    }

/*
    if (IS_ERR(WeMakeSoftwareModulesFile)) {
        Info("Error: Could not open file");
        return PTR_ERR(WeMakeSoftwareModulesFile);  // Return the error
    }
*/
    Info("Module initialized successfully");
    return 0;  // Return 0 to indicate successful loading
}

// Cleanup function (runs when the module is removed)
static void __exit WeMakeSoftwareExit(void)
{
    printk(KERN_INFO "Simple Module: Unloaded successfully\n");
}
module_init(WeMakeSoftwareInit);
module_exit(WeMakeSoftwareExit);
MODULE_METADATA();