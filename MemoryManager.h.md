
# MemoryManager.h

This header file is central to managing and manipulating `WMS` structures in memory, following the principles established in `Memory.h`. The goal is to provide efficient, dynamic memory management for `WMS` entries, ensuring that memory is properly allocated, tracked, and freed when no longer needed.

## Struct `WMS`

The `WMS` structure defines the core data that the memory manager will handle. Each `WMS` entry contains three key fields:
1. **Name**: A pointer to a dynamically allocated string representing the unique identifier for each `WMS` entry.
2. **Size**: A pointer to an unsigned 64-bit integer that dynamically stores information related to the size of the `WMS` entry.
3. **Data**: A pointer to dynamically allocated memory that holds the actual content associated with the `WMS` entry.

## Managing `WMS` Entries

The functions provided in this header (`HaveWMS`, `RemoveWMS`, and `AddWMS`) allow for efficient interaction with `WMS` entries:
- **`HaveWMS(const char* name)`**: Checks if a `WMS` entry with a specific name already exists, ensuring that no duplicate entries are created.
- **`RemoveWMS(const char* name)`**: Safely removes a `WMS` entry by freeing all dynamically allocated memory associated with it, including the `name`, `size`, and `data` fields.
- **`AddWMS(const char* name)`**: Adds a new `WMS` entry, dynamically allocating memory for each of its components and ensuring that the `globalWMS` array is resized properly to accommodate new entries.

## Global Structure and Access by Pointer

While the `WMS` struct is globally accessible via the `globalWMS` pointer in the `.c` file, direct access to this global structure is avoided in favor of functions that manage `WMS` entries by pointer. This provides better encapsulation and control over how memory is handled.

Instead of exposing the `globalWMS` array directly, these functions ensure that any access to or manipulation of `WMS` entries happens in a controlled manner, minimizing the risk of memory mismanagement or corruption.

## Initialization and Resource Management

This header works in conjunction with `MemoryManagerInit.h`, which handles the setup and initialization of the memory management system. The system ensures that memory is properly allocated when needed and freed when no longer required, preventing memory leaks and optimizing resource use.

For a detailed overview of the initialization process, refer to the [MemoryManagerInit.h Documentation](https://github.com/we-make-software/how-to-get-your-attention.com/blob/main/MemoryManagerInit.h.md).

