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
	.init = init_module,
#ifdef CONFIG_MODULE_UNLOAD
	.exit = cleanup_module,
#endif
	.arch = MODULE_ARCH_INIT,
};

#ifdef CONFIG_RETPOLINE
MODULE_INFO(retpoline, "Y");
#endif



static const char ____versions[]
__used __section("__versions") =
	"\x1c\x00\x00\x00\xca\x39\x82\x5b"
	"__x86_return_thunk\0\0"
	"\x10\x00\x00\x00\x7e\x3a\x2c\x12"
	"_printk\0"
	"\x14\x00\x00\x00\x51\x0e\x00\x01"
	"schedule\0\0\0\0"
	"\x14\x00\x00\x00\x5a\x69\x8c\xdf"
	"__ndelay\0\0\0\0"
	"\x14\x00\x00\x00\xcf\xe2\xb6\x02"
	"pcpu_hot\0\0\0\0"
	"\x18\x00\x00\x00\x10\x2f\x1a\xa4"
	"dev_remove_pack\0"
	"\x18\x00\x00\x00\xe5\x6e\x63\x5f"
	"find_get_pid\0\0\0\0"
	"\x14\x00\x00\x00\x5c\xc0\xc7\xbd"
	"kill_pid\0\0\0\0"
	"\x10\x00\x00\x00\xbb\xfd\xc2\x7b"
	"put_pid\0"
	"\x1c\x00\x00\x00\x60\xa6\x43\xbc"
	"kfree_skb_reason\0\0\0\0"
	"\x14\x00\x00\x00\xe6\xd0\xd7\x26"
	"init_net\0\0\0\0"
	"\x24\x00\x00\x00\x7c\xb2\x83\x63"
	"__x86_indirect_thunk_rdx\0\0\0\0"
	"\x18\x00\x00\x00\xcd\xb4\x4a\x8b"
	"dev_add_pack\0\0\0\0"
	"\x1c\x00\x00\x00\x63\xa5\x03\x4c"
	"random_kmalloc_seed\0"
	"\x18\x00\x00\x00\x10\x03\x98\x24"
	"kmalloc_caches\0\0"
	"\x18\x00\x00\x00\xeb\x9d\x19\x1d"
	"kmalloc_trace\0\0\0"
	"\x18\x00\x00\x00\x9f\x0c\xfb\xce"
	"__mutex_init\0\0\0\0"
	"\x14\x00\x00\x00\xaa\xdf\x51\xd9"
	"__alloc_skb\0"
	"\x1c\x00\x00\x00\xb3\x51\x73\xc0"
	"__SCT__cond_resched\0"
	"\x1c\x00\x00\x00\xcb\xf6\xfd\xf0"
	"__stack_chk_fail\0\0\0\0"
	"\x14\x00\x00\x00\xde\x83\xe6\x69"
	"uuid_gen\0\0\0\0"
	"\x14\x00\x00\x00\x6e\x4a\x6e\x65"
	"snprintf\0\0\0\0"
	"\x20\x00\x00\x00\x59\x5e\xe2\xab"
	"kthread_create_on_node\0\0"
	"\x18\x00\x00\x00\x55\x4c\xb8\xa6"
	"wake_up_process\0"
	"\x14\x00\x00\x00\xbb\x6d\xfb\xbd"
	"__fentry__\0\0"
	"\x14\x00\x00\x00\x65\x93\x3f\xb4"
	"ktime_get\0\0\0"
	"\x14\x00\x00\x00\x4b\x8d\xfa\x4d"
	"mutex_lock\0\0"
	"\x10\x00\x00\x00\xba\x0c\x7a\x03"
	"kfree\0\0\0"
	"\x18\x00\x00\x00\x38\xf0\x13\x32"
	"mutex_unlock\0\0\0\0"
	"\x18\x00\x00\x00\xd7\xd3\x75\x6d"
	"module_layout\0\0\0"
	"\x00\x00\x00\x00\x00\x00\x00\x00";

MODULE_INFO(depends, "");


MODULE_INFO(srcversion, "EA472BF992479F8A00FCF60");
