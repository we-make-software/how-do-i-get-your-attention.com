#pragma once
#include<linux/module.h>
#include<linux/kernel.h>
#include<linux/types.h>
#include<linux/netdevice.h>
#include<linux/ktime.h>
#include<linux/kthread.h>
#define MODULE_INFO_SETUP(author,description,version)\
MODULE_LICENSE("GPL");\
MODULE_AUTHOR(author);\
MODULE_DESCRIPTION(description);\
MODULE_VERSION(version);
struct Link {
    struct Link *Previous, *Next;
    uint16_t ID; 
    unsigned char*Data; 
};
struct Standard{
    struct Standard*Previous,*Next;
    uint16_t Version,Section;
    bool IsDynamic;
    struct Link*Links;
    unsigned char*Data;
};
struct Status
{
    unsigned char Request:4,Response:4;
};
struct Frame{
    struct Frame*Previous,*Next;
    struct sk_buff*skb; 
    int id;
    struct Status Status; 
    ktime_t Start, End;
    unsigned char*IEE802Buffer;
    struct Standard*Standards;
};
enum ThreadProcess{
    ThreadProcessWait,
    ThreadProcessRunning,
    ThreadProcessClose
};

struct ThreadJob {
    struct ThreadTask*Previous,*Next;
    struct task_struct*Thread;
};

struct Job {
    struct Job *Previous, *Next; 
    void *Data;
    int (*JobFunction)(void*);
};
struct ThreadToDo {
    struct ThreadToDo *Previous, *Next;
    struct Job *Jobs;
    
};