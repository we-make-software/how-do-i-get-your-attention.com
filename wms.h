#pragma once
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/slab.h>
#include <linux/wait.h>
#include <linux/sched.h>
#include <linux/kthread.h>
#include <linux/delay.h>
#include <linux/uuid.h> 
#include <linux/swap.h>
#include <linux/mm.h>
#include <linux/types.h> 
#include <linux/mutex.h>
#include <linux/ktime.h> 
#include <linux/uaccess.h>  
#include <linux/err.h>
#include <linux/fs.h>      
#include <linux/path.h>     
#include <linux/namei.h>   
#include <linux/mount.h> 
#include "Memory.h"
#define Export(symbol) EXPORT_SYMBOL(symbol)

#ifndef MODULE_INFO_MACROS
#define MODULE_INFO_MACROS
#define MODULE_METADATA() \
    MODULE_LICENSE("GPL"); \
    MODULE_DESCRIPTION("A Simple Kernel Module"); \
    MODULE_AUTHOR("Pirasath Luxchumykanthan")
#endif
