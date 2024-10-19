# WeMakeSoftware.c

This file contains the core runtime functionality. It defines the initialization (`WeMakeSoftwareInit`) and cleanup (`Exit`) functions, which handle the module's startup and shutdown processes. These functions control how the module is loaded and unloaded in the kernel.

Additionally, for the `GetFunction` implementation, I renamed `kallsyms_lookup_name` to `GetFunction` because I prefer this name for its simplicity and clarity.

