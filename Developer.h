#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/ktime.h>
#include <linux/mutex.h>
#include <linux/kthread.h>
#include <linux/hrtimer.h>
#include <linux/io.h>
#include <linux/slab.h>
#include <linux/sched.h>
#include <linux/string.h>
#define TRUE 1
#define FALSE 0
#define Now ktime_get()
#define TimeByFuture(hours,minutes)ktime_set((hours*3600)+(minutes*60),0)
#define TimeAddNow(Time)Now+Time
#define TimeAddNowSeconds(Seconds)TimeAddNow(((Seconds)*1000000000LL))
#define Export(symbol)EXPORT_SYMBOL(symbol)
#define NewThread kmalloc(sizeof(struct task_struct),GFP_KERNEL)
#define Thread(func, data, name) kthread_run(func, data, name)
#define ThreadRunning set_current_state(TASK_RUNNING)
#define ThreadPause set_current_state(TASK_INTERRUPTIBLE)
#define ThreadStop return 0
typedef unsigned char BOOL;
BOOL Run(void);
#define RUN Run()
#define ThreadIsRunning RUN&&!kthread_should_stop()
#define PowerNap ktime_t
#define Opration struct task_struct*
typedef struct sk_buff Buffer;
typedef long long int Clock;
typedef BOOL Byte,*Bytes;
typedef struct mutex Lock;
#define InitLock(Lock)mutex_init(&Lock)
#define LOCK(Lock)mutex_lock(&Lock)
#define UNLOCK(Lock)mutex_unlock(&Lock)
#define WMSInit static void Init(void)
#define WMSExit static void Exit(void)
#define WMS(Description,Version)WMSInit;WMSExit;static int __init wms_init(void){Init();return 0;}module_init(wms_init);static void __exit wms_exit(void){Exit();}module_exit(wms_exit);MODULE_LICENSE("GPL");MODULE_AUTHOR("we-make-software.com");MODULE_DESCRIPTION(Description);MODULE_VERSION(Version);

