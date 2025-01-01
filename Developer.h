#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/ktime.h>
#include <linux/mutex.h>
#include <linux/kthread.h>
#include <linux/hrtimer.h>
#define CopyBytes(Destination,Source,Size)memcpy(Destination,Source,Size)
#define InitLock(Lock)mutex_init(&Lock)
#define LOCK(Lock)mutex_lock(&Lock)
#define UNLOCK(Lock)mutex_unlock(&Lock)
#define TRUE 1
#define FALSE 0
#define Now ktime_get()
#define TimeByFuture(hours,minutes)ktime_set((hours*3600)+(minutes*60),0)
#define TimeAddNow(Time)Now+Time
#define Export(symbol)EXPORT_SYMBOL(symbol)
#define Thread(func, data, name) kthread_run(func, data, name)
#define ThreadRunning set_current_state(TASK_RUNNING)
#define PowerNap ktime_t
#define Opration struct task_struct*
typedef struct task_struct*Task;
typedef struct sk_buff Buffer;
typedef long long int Clock;
typedef unsigned char BOOL;
typedef BOOL Byte,*Bytes;
typedef struct mutex Lock;
#define WMSInit static void Init(void)
#define WMSExit static void Exit(void)
#define WMS(Description,Version)WMSInit;WMSExit;static int __init wms_init(void){Init();return 0;}module_init(wms_init);static void __exit wms_exit(void){Exit();}module_exit(wms_exit);MODULE_LICENSE("GPL");MODULE_AUTHOR("we-make-software.com");MODULE_DESCRIPTION(Description);MODULE_VERSION(Version);

