# MemoryManager.c


## Global Pointer: `globalWMS`

The `globalWMS` pointer is a lightweight 8-byte structure — it doesn’t need to be any bigger than that. To ensure memory is available, we upgrade the pointer by calling:

```c
waitForMemory(8);
```

This function ensures the system waits until 8 bytes of RAM are ready for use. In a typical startup process, finding 8 bytes of RAM is no challenge — it would be quite unusual if the system couldn’t provide such a small amount!

