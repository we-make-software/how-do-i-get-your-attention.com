#include "WeMakeSoftware.h"
#include "Router.h"
WMS("WeMakeSoftware Router", "1.0")

int Incoming(Exchange exchange){

    return 0;
}

WMSInit{
    printk(KERN_INFO "Router: Init\n");
   // InitWeMakeSoftware(Incoming);
}
WMSExit{
    // ExitWeMakeSoftware();
}