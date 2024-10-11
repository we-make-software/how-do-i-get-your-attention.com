# MemoryManagerInit.h

In this header, we define the initialization and cleanup functions for the Memory Manager. These functions are:

```c
#pragma once
#include "Memory.h"
extern void MemoryManagerInit(void);
extern void MemoryManagerExit(void);
```

The primary focus of this file is to ensure that only `wmsInit(void)` and `wmsExit(void)` from `wme.c` have access to initialize and terminate the memory management process efficiently.

