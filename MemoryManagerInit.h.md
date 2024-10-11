
# MemoryManagerInit.h

In this header, we define the initialization and cleanup functions for the Memory Manager. These functions are:

```c
#pragma once
#include "Memory.h"
extern void MemoryManagerInit(void);
extern void MemoryManagerExit(void);
```

The main focus is ensuring that only `wmsInit(void)` and `wmsExit(void)` from `wme.c` have access to this process.

However, your primary attention should be on `MemoryManager.h`, as it is already working effectively.

[MemoryManager h Documentation](https://github.com/we-make-software/how-to-get-your-attention.com/blob/main/MemoryManager.h.md)