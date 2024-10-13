#include <linux/module.h>
#define INCLUDE_VERMAGIC
#include <linux/build-salt.h>
#include <linux/elfnote-lto.h>
#include <linux/export-internal.h>
#include <linux/vermagic.h>
#include <linux/compiler.h>

#ifdef CONFIG_UNWINDER_ORC
#include <asm/orc_header.h>
ORC_HEADER;
#endif

BUILD_SALT;
BUILD_LTO_INFO;

MODULE_INFO(vermagic, VERMAGIC_STRING);
MODULE_INFO(name, KBUILD_MODNAME);

__visible struct module __this_module
__section(".gnu.linkonce.this_module") = {
	.name = KBUILD_MODNAME,
	.arch = MODULE_ARCH_INIT,
};

#ifdef CONFIG_RETPOLINE
MODULE_INFO(retpoline, "Y");
#endif

KSYMTAB_FUNC(HaveWMS, "", "");
KSYMTAB_FUNC(RemoveWMS, "", "");
KSYMTAB_FUNC(AddWMS, "", "");
KSYMTAB_FUNC(MemoryManagerInit, "", "");
KSYMTAB_FUNC(MemoryManagerExit, "", "");

SYMBOL_CRC(HaveWMS, 0xf25770b3, "");
SYMBOL_CRC(RemoveWMS, 0x95ebd949, "");
SYMBOL_CRC(AddWMS, 0x25e8d58a, "");
SYMBOL_CRC(MemoryManagerInit, 0x4dfa74a5, "");
SYMBOL_CRC(MemoryManagerExit, 0x73ed9c8f, "");

static const char ____versions[]
__used __section("__versions") =
	"\x14\x00\x00\x00\xbb\x6d\xfb\xbd"
	"__fentry__\0\0"
	"\x10\x00\x00\x00\x5a\x25\xd5\xe2"
	"strcmp\0\0"
	"\x1c\x00\x00\x00\xca\x39\x82\x5b"
	"__x86_return_thunk\0\0"
	"\x10\x00\x00\x00\xba\x0c\x7a\x03"
	"kfree\0\0\0"
	"\x14\x00\x00\x00\x4b\x8d\xfa\x4d"
	"mutex_lock\0\0"
	"\x18\x00\x00\x00\x38\xf0\x13\x32"
	"mutex_unlock\0\0\0\0"
	"\x18\x00\x00\x00\x1b\x80\x81\x59"
	"waitForMemory\0\0\0"
	"\x10\x00\x00\x00\x38\xdf\xac\x69"
	"memcpy\0\0"
	"\x10\x00\x00\x00\x9c\x53\x4d\x75"
	"strlen\0\0"
	"\x10\x00\x00\x00\xda\xfa\x66\x91"
	"strncpy\0"
	"\x18\x00\x00\x00\x3a\x0a\xd8\xfc"
	"module_layout\0\0\0"
	"\x00\x00\x00\x00\x00\x00\x00\x00";

MODULE_INFO(depends, "Memory");


MODULE_INFO(srcversion, "BB6C10E898C4B086867D335");
