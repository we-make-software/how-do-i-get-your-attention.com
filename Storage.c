#include "StorageInit.h"
#include <linux/fs.h>
#include <linux/file.h>
#include <linux/dcache.h>  
struct file *disk = NULL; 
uint64_t diskSize;
uint64_t CentralProcessingUnitTaskGetStorageSize(void) {
    struct kstat stat;
    struct path path;
    kern_path("/root/wms.disk", LOOKUP_FOLLOW, &path);
    vfs_getattr(&path, &stat, STATX_SIZE, AT_STATX_SYNC_AS_STAT);
    return diskSize=(uint64_t)stat.size;
}
Export(CentralProcessingUnitTaskGetStorageSize);
uint64_t GetStorageSize(void) {
    return diskSize;
}
Export(GetStorageSize);
StorageWriteResult StorageWrite(char *data, uint16_t dataSize, uint64_t position) {
    return (position + dataSize) > diskSize||kernel_write(disk, data, dataSize, &position) < 0 ?StorageWriteError:StorageWriteSuccess; 
}
Export(StorageWrite);
StorageReadResult StorageRead(char *data, uint16_t dataSize, uint64_t position) {
    return (position + dataSize) > diskSize|| kernel_read(disk, data, dataSize, &position)<0? StorageReadError : StorageReadSuccess;  
}
Export(StorageRead);
void StorageInit() {
    disk = filp_open("/root/wms.disk", O_RDWR | O_NONBLOCK, 0);
    CentralProcessingUnitTaskGetStorageSize();
}
Export(StorageInit);
void StorageExit() {
    Warning("StorageExit", "StorageExit called with a potential issue");
    Error("StorageExit", "StorageExit encountered an error");
}
Export(StorageExit);
MODULE_METADATA();