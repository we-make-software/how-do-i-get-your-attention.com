#include "wms.h"
/*
LiveDocumention* liveDocumention=NULL;

bool isMemoryAvailable(unsigned long memoryRequiredBytes);
bool isMemoryAvailable(unsigned long memoryRequiredBytes) {
    si_meminfo(&si);
    unsigned long available_bytes = si.freeram * PAGE_SIZE;
    if (available_bytes > 2147483648) {
        available_bytes -= 2147483648;
        return available_bytes >= memoryRequiredBytes;
    }
    else return false;
}
void waitForMemoryIsAvailable(unsigned long memoryRequiredBytes);
void waitForMemoryIsAvailable(unsigned long memoryRequiredBytes) {
    while (!isMemoryAvailable(memoryRequiredBytes*2)) udelay(10);
}

void* waitForMemory(unsigned long memoryRequiredBytes){
    if(!liveDocumention||!liveDocumention->fileName)
        return kmalloc(memoryRequiredBytes, GFP_KERNEL);
    waitForMemoryIsAvailable(memoryRequiredBytes);
    void* _kmalloc= kmalloc(memoryRequiredBytes, GFP_KERNEL);
    while (!_kmalloc)
    {
         waitForMemoryIsAvailable(memoryRequiredBytes);
         _kmalloc= kmalloc(memoryRequiredBytes, GFP_KERNEL);
    }
    return _kmalloc;
}


void setLiveDocumentionFilename(const char* fileName, bool iDontCare) {
    if (liveDocumention->fileName)
        kfree(liveDocumention->fileName);
    const char* directory = "/root/we-make-software/";
    ulong fileNameSize = strlen(directory) +  strlen(fileName) + 4;
    liveDocumention->fileName = waitForMalloc(fileNameSize);
    snprintf(liveDocumention->fileName, fileNameSize, "%s%s.md", directory, fileName);
    if (!(liveDocumention->iDontCare=iDontCare)) {
        struct file *file = filp_open(liveDocumention->fileName, O_WRONLY | O_CREAT | O_TRUNC, 0644);
        if (!IS_ERR(file)) 
            filp_close(file, NULL);
    }
}
void setLiveDocumentionWrite(const char* text) {
    if (liveDocumention->iDontCare) return;
    loff_t pos = 0;  
    struct file* file = filp_open(liveDocumention->fileName, O_WRONLY | O_APPEND | O_CREAT, 0644);
    if (kernel_write(file, text, strlen(text), &pos) >= 0)
     kernel_write(file, "\n", 1, &pos); 
    filp_close(file, NULL);
}

void liveDocumentionDisable(void){
    liveDocumention->iDontCare=true;
}
void liveDocumentionEnable(void){
     liveDocumention->iDontCare=false;
}
*/

static int __init wmsInit(void) {

    /*
    liveDocumention = waitForMalloc(sizeof(LiveDocumention));
    liveDocumention->iDontCare=false;
    setLiveDocumentionFilename("Readme",false);
    setLiveDocumentionWrite(
        "*We don't know how long a process takes, but we can find out—that's called output.<br/>\n" 
        "Basically, I don't like making README files or markdown (md) files,<br/>\n"
        "so I think I need to come up with something in wms.h.<br/>\n"
        "We have the following functions: 'waitForMalloc', 'setLiveDocumentionFilename',<br/>\n"
        "'liveDocumentionDisable', 'liveDocumentionEnable', and 'setLiveDocumentionWrite'.<br/>\n"
        "We can continue to improve how we create README (md) files.<br/>\n"
        "But that’s a project in itself.*<br/><br/>\n"
    );
    */
    return 0;
}

static void __exit wmsExit(void) {

    printk(KERN_INFO "Goodbye from the Kernel Module!\n");
}

module_init(wmsInit);
module_exit(wmsExit);

MODULE_METADATA(); 
