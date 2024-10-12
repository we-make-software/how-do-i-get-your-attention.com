# MemoryManager.c

## Global Pointer: `globalWMS`

The `globalWMS` pointer references a dynamically sized structure (`WMS`). Since the size of `struct WMS` can vary, we allocate memory for it using:

```c
waitForMemory(sizeof(WMS));
```

This ensures that the system waits until the required memory for the full `WMS` structure is available. During startup, memory allocation is usually not a problem, but this approach guarantees that enough memory is allocated based on the actual size of the structure.

