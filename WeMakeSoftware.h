#include "Developer.h"
struct Setting;
#ifndef WeMakeSoftwarePrivate
    typedef struct Setting {
        const enum{Connected=1,Disconnected=0}state:1;
    }*Setting;
    typedef struct Exchange{
        uint64_t Size;
        Bytes Data;
        const Clock StartTime;
        Clock EndTime;
        struct Setting*Setting;
        const enum{High=2,Middle=1,Low=0}state:2;
    }*Exchange;
#endif
struct Exchange;
void ThreadPowerNap(Clock);
void ThreadPowerWakeUp(Opration);
void ThreadClose(Opration opration);
struct Exchange* CreateExchange(struct Exchange*);
int CancelExchange(struct Exchange*);
int SkipExchange(struct Exchange*);
BOOL RequestResendExchange(struct Exchange*);
BOOL SendExchange(struct Exchange*);
int UnknownExchange(struct Exchange*);
typedef int(*ExchangePacketReader)(struct Exchange*);
void InitWeMakeSoftware(ExchangePacketReader);
void ExitWeMakeSoftware(void);

