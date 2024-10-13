#include "StorageInit.h"

void StorageInit() {
    Info("StorageInit", "StorageInit called");
}

Export(StorageInit);

void StorageExit() {
    Warning("StorageExit", "StorageExit called with a potential issue");
    Error("StorageExit", "StorageExit encountered an error");
}
Export(StorageExit);
MODULE_METADATA();