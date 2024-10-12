# MemoryManager.c

## Overview of Memory Management

The `MemoryManager` module handles the dynamic management of `WMS` structures, ensuring memory is allocated, accessed, and freed efficiently. This is critical in environments where memory needs can vary, and it’s important to control the size and availability of memory.

### Global Pointer: `globalWMS`

The `globalWMS` pointer is a dynamically managed array that holds multiple `WMS` structures. These structures are created, stored, and freed based on specific operations such as adding or removing a `WMS`. The size of this array can grow or shrink depending on how many valid entries are stored.

### Adding a New `WMS`

When a new `WMS` entry needs to be added, the system first checks if an entry with the same name already exists. If it does, that entry is returned immediately, preventing duplicate entries.

If no existing entry is found, the array's size is increased, and a new entry is created. The necessary memory for this new entry is dynamically allocated. The system ensures that the correct amount of memory is allocated for the new `WMS` entry and its associated fields, such as the name. This avoids memory issues, especially when handling varying string lengths for the `name`.

### Removing an Existing `WMS`

When an entry is removed, the system carefully ensures that all dynamically allocated memory associated with that `WMS` is freed. This includes not just the structure itself but also any additional fields like `name`, `size`, and `data`, which may have been dynamically allocated during the entry's lifetime.

After removing an entry, the system checks how many valid entries remain. If no valid entries are left, the entire `globalWMS` array is freed, releasing the memory back to the system. If some entries remain, the array is resized to fit the remaining valid entries, ensuring no excess memory is wasted.

### Synchronization with Mutex

To ensure thread safety, a mutex (`WMSMutex`) is used when adding or removing entries. This guarantees that no other process can modify the `globalWMS` array while it is being resized or updated. This avoids race conditions and ensures that the operations on `globalWMS` happen sequentially and without conflict.

### Initialization and Cleanup

The `MemoryManagerInit` function is responsible for setting up the environment for managing `WMS` structures. It clears out any previous entries and ensures that the system starts with a clean state.

On the other hand, the `MemoryManagerExit` function handles proper cleanup, ensuring that all dynamically allocated memory is freed when the system shuts down or when memory management is no longer needed.

### Importance of Dynamic Memory Management

By dynamically managing memory through careful allocation and deallocation, this module ensures efficient use of system resources. It prevents memory leaks by freeing allocated memory when no longer needed, and it optimizes the use of memory by resizing the array of `WMS` entries as necessary.

