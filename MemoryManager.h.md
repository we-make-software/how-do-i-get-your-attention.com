# MemoryManager.h

This header file is central to managing and manipulating `MemoryManager` structures in memory, following the principles established in `Memory.h`. The goal is to provide efficient, dynamic memory management for `MemoryManager` entries, ensuring that memory is properly allocated, tracked, and freed when no longer needed.

## Struct `MemoryManager`

The `MemoryManager` structure defines the core data that the memory manager will handle. Each `MemoryManager` entry contains three key fields:
1. **Name**: A pointer to a dynamically allocated string representing the unique identifier for each `MemoryManager` entry.
2. **Size**: A pointer to an unsigned 64-bit integer that dynamically stores information related to the size of the `MemoryManager` entry.
3. **Data**: A pointer to dynamically allocated memory that holds the actual content associated with the `MemoryManager` entry.

## Managing `MemoryManager` Entries

The functions provided in this header (`HaveMemoryManager`, `RemoveMemoryManager`, and `AddMemoryManager`) allow for efficient interaction with `MemoryManager` entries:
- **`HaveMemoryManager(const char* name)`**: Checks if a `MemoryManager` entry with a specific name already exists, ensuring that no duplicate entries are created.
- **`RemoveMemoryManager(const char* name)`**: Safely removes a `MemoryManager` entry by freeing all dynamically allocated memory associated with it, including the `name`, `size`, and `data` fields.
- **`AddMemoryManager(const char* name)`**: Adds a new `MemoryManager` entry, dynamically allocating memory for each of its components and ensuring that the `globalMemoryManager` array is resized properly to accommodate new entries.

## Global Structure and Access by Pointer

While the `MemoryManager` struct is globally accessible via the `globalMemoryManager` pointer in the `.c` file, direct access to this global structure is avoided in favor of functions that manage `MemoryManager` entries by pointer. This provides better encapsulation and control over how memory is handled.

Instead of exposing the `globalMemoryManager` array directly, these functions ensure that any access to or manipulation of `MemoryManager` entries happens in a controlled manner, minimizing the risk of memory mismanagement or corruption.

## Initialization and Resource Management

This header works in conjunction with `MemoryManagerInit.h`, which handles the setup and initialization of the memory management system. The system ensures that memory is properly allocated when needed and freed when no longer required, preventing memory leaks and optimizing resource use.

For a detailed overview of the initialization process, refer to the [MemoryManagerInit.h Documentation](https://github.com/we-make-software/how-to-get-your-attention.com/blob/main/MemoryManagerInit.h.md).
