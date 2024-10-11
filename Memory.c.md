# Memory.c

This file implements the memory management functionality outlined in `Memory.h`, focusing on checking available memory, waiting for sufficient resources, and dynamically allocating memory. 

The function **`isMemoryAvailable`** calculates the available system memory by calling `si_meminfo` and multiplies the free RAM by `PAGE_SIZE`. It ensures that at least 2 GB of memory is reserved, subtracting that amount if more memory is available. This approach ensures the system doesn’t consume critical memory resources.

**`waitForMemoryIsAvailable`** continuously checks whether enough memory is available, using a delay (`udelay(10)`) to avoid excessive CPU usage. It waits until twice the required amount of memory is available before proceeding, providing a buffer to ensure smooth allocation.

The core function, **`waitForMemory`**, handles the allocation process. It waits until memory is available and attempts to allocate it using `kmalloc`. If the allocation fails, the function loops back, waiting and retrying until the allocation succeeds. This ensures that the system can handle dynamic memory needs without running into failures due to insufficient resources.

Finally, the use of `EXPORT_SYMBOL(waitForMemory)` ensures that this function is accessible to other modules, while `MODULE_METADATA()` provides the necessary metadata to describe the kernel module’s license, author, and description.