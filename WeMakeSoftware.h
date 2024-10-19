#pragma once
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/slab.h>
#include <linux/fs.h>
#define Export(symbol) EXPORT_SYMBOL(symbol)
#define Info(fmt, ...) printk(KERN_INFO "[INFO] %s: " fmt "\n", __func__, ##__VA_ARGS__)
#define Warning(fmt, ...) printk(KERN_WARNING "[WARNING] %s: " fmt "\n", __func__, ##__VA_ARGS__)
#define Error(fmt, ...) printk(KERN_ERR "[ERROR] %s: " fmt "\n", __func__, ##__VA_ARGS__)

#define GetFunction(name) ((int)kallsyms_lookup_name(name))
#define Malloc(size, flags)kmalloc(size, flags)

#ifndef MODULE_INFO_MACROS
#define MODULE_INFO_MACROS
#define MODULE_METADATA() \
    MODULE_LICENSE("GPL"); \
    MODULE_DESCRIPTION("A Simple Kernel Module"); \
    MODULE_AUTHOR("Pirasath Luxchumykanthan")
#endif