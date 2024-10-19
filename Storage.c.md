# Storage.c

**Storage** is a critical component because it holds the actual data and performance results that power the system. The storage system ensures that the work done by **wms** is preserved, providing reliable access to real data.

The storage system is designed in such a way that we don’t need RAID, as we have confidence that the system will perform flawlessly without the need for redundancy. 

The code in **Storage.c** manages the initialization, reading, writing, and cleanup of the storage. It interacts with a disk image stored at `/root/wms.disk`, performing read/write operations while ensuring the storage size is accurately managed.

Here’s an overview of the current implementation:

- **`CentralProcessingUnitTaskGetStorageSize()`** retrieves the total size of the storage to ensure that all read and write operations remain within the allocated bounds.
- **`StorageWrite()`** performs a write operation to the disk, ensuring the data is written at the correct position and doesn’t exceed the available disk size.
- **`StorageRead()`** reads data from the disk at a given position, ensuring that it does not exceed the storage limits.
- **`StorageInit()`** initializes the storage system by opening the disk image and retrieving its size.
- **`StorageExit()`** ensures that any necessary cleanup is done when the storage system is no longer needed.