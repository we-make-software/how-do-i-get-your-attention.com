# MemoryManager.c

## Overview of Memory Management

The `MemoryManager` module handles the dynamic management of `MemoryManager` structures, ensuring memory is allocated, accessed, and freed efficiently. This is crucial in environments where memory demands can fluctuate, and it’s essential to manage the size and availability of memory carefully.

### Global Pointer: `globalMemoryManager`

The `globalMemoryManager` pointer is a dynamically managed array that holds multiple `MemoryManager` structures. These structures are created, stored, and freed based on specific operations such as adding or removing a `MemoryManager`. The size of this array can grow or shrink depending on how many valid entries are stored, ensuring that memory is used efficiently without unnecessary allocation.

---

## Adding a New `MemoryManager`

When a new `MemoryManager` entry needs to be added, the system first checks if an entry with the same `name` already exists. If it does, that entry is returned immediately, preventing duplicate entries.

If no existing entry is found, the array’s size is increased, and a new entry is created. Memory is dynamically allocated for this new entry, including its name. The system ensures that the correct amount of memory is allocated for the new `MemoryManager` entry and its associated fields. This helps avoid memory issues, especially when handling varying string lengths for the `name`.

---

## Removing an Existing `MemoryManager`

When a `MemoryManager` entry is removed, the system carefully ensures that all dynamically allocated memory associated with that entry is freed. This includes:
- The `name` field (dynamically allocated string)
- The `size` field (pointer to a 64-bit integer)
- The `data` field (dynamically allocated content)

Once the memory is freed, the system checks how many valid entries remain in the `globalMemoryManager` array. If no valid entries are left, the entire array is freed, releasing all memory back to the system. If some entries remain, the array is resized to fit the remaining valid entries, ensuring that no excess memory is wasted.

---

## Synchronization with Mutex

To ensure thread safety, a mutex (`MemoryManagerMutex`) is used when adding or removing entries. This guarantees that no other process can modify the `globalMemoryManager` array while it is being resized or updated. The mutex ensures that operations on `globalMemoryManager` happen sequentially, preventing race conditions or memory corruption.

---

## Initialization and Cleanup

- **MemoryManagerInit**: Responsible for setting up the environment for managing `MemoryManager` structures. It ensures that the system starts with a clean state by clearing out any previous entries.
  
- **MemoryManagerExit**: Handles proper cleanup by freeing all dynamically allocated memory when the system shuts down or when memory management is no longer needed. This function ensures that there are no memory leaks by releasing memory for each `MemoryManager` entry and the global array itself.

---

## Importance of Dynamic Memory Management

By dynamically managing memory through careful allocation and deallocation, this module ensures efficient use of system resources. It prevents memory leaks by freeing allocated memory when no longer needed and optimizes the system’s memory usage by resizing the array of `MemoryManager` entries as necessary.

