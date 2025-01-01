#include "Developer.h"

struct Setting;
#ifndef WeMakeSoftwarePrivate
    typedef struct Setting {
        const enum{Connected=1,Disconnected=0}state:1;
    }*Setting;
#endif
void WeMakeSoftwareSetInterval(struct Setting*setting);
int WeMakeSoftwareSend(struct Setting*setting,Bytes data,unsigned int length);
void SetWeMakeSoftwareMaxTime(struct Setting*setting, int hours, int minutes);
BOOL IsWeMakeSoftwareConnected(struct Setting*setting);
void ThreadPowerNap(ktime_t *PowerNap, Clock Time);