#include "Memory.init.h" 
void memoryInit(void) {
    printk(KERN_ALERT "Memory initialized successfully.\n");
}
EXPORT_SYMBOL(memoryInit); 
MODULE_METADATA(); 
/*
bool isMemoryAvailable(unsigned long memoryRequiredBytes);
void waitForMemoryIsAvailable(unsigned long memoryRequiredBytes);
struct sysinfo si;

bool isMemoryAvailable(unsigned long memoryRequiredBytes) {
    si_meminfo(&si);
    unsigned long available_bytes = si.freeram * PAGE_SIZE;
    if (available_bytes > 2147483648) {
        available_bytes -= 2147483648;
        return available_bytes >= memoryRequiredBytes;
    }
    else return false;
}

void waitForMemoryIsAvailable(unsigned long memoryRequiredBytes) {
    while (!isMemoryAvailable(memoryRequiredBytes * 2)) udelay(10);
}

void* waitForMemory(unsigned long memoryRequiredBytes) {
    waitForMemoryIsAvailable(memoryRequiredBytes);
    void* _kmalloc = kmalloc(memoryRequiredBytes, GFP_KERNEL);
    while (!_kmalloc) {
        waitForMemoryIsAvailable(memoryRequiredBytes);
        _kmalloc = kmalloc(memoryRequiredBytes, GFP_KERNEL);
    }
    return _kmalloc;
}

void memoryInit(void) {
    // Initialization logic
}

EXPORT_SYMBOL(memoryInit);  // Export the symbol here
*/