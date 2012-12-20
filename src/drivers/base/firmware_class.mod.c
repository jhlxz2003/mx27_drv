#include <linux/module.h>
#include <linux/vermagic.h>
#include <linux/compiler.h>

MODULE_INFO(vermagic, VERMAGIC_STRING);

struct module __this_module
__attribute__((section(".gnu.linkonce.this_module"))) = {
 .name = KBUILD_MODNAME,
 .init = init_module,
#ifdef CONFIG_MODULE_UNLOAD
 .exit = cleanup_module,
#endif
};

static const struct modversion_info ____versions[]
__attribute_used__
__attribute__((section("__versions"))) = {
	{ 0xf6253259, "struct_module" },
	{ 0xe98b0881, "module_refcount" },
	{ 0x9afe9c91, "__mod_timer" },
	{ 0x8b63632e, "class_register" },
	{ 0xd6ee688f, "vmalloc" },
	{ 0x75b38522, "del_timer" },
	{ 0xb13d73c9, "malloc_sizes" },
	{ 0x8cf1e505, "kobject_uevent" },
	{ 0xdce9a993, "class_device_unregister" },
	{ 0x323222ba, "mutex_unlock" },
	{ 0x2fd1d81c, "vfree" },
	{ 0x1d26aa98, "sprintf" },
	{ 0x7d11c268, "jiffies" },
	{ 0x25fa6f17, "wait_for_completion" },
	{ 0x5f754e5a, "memset" },
	{ 0xdd132261, "printk" },
	{ 0xf0b0524c, "class_unregister" },
	{ 0x73e20c1c, "strlcpy" },
	{ 0xd00740c0, "class_device_create_file" },
	{ 0xb97d4c9c, "mutex_lock" },
	{ 0x43b0c9c3, "preempt_schedule" },
	{ 0x2df98d02, "class_create_file" },
	{ 0x7dceceac, "capable" },
	{ 0x8523fd53, "kmem_cache_alloc" },
	{ 0x6b2dc060, "dump_stack" },
	{ 0x9b2a6c7b, "class_device_register" },
	{ 0x4e3e4b31, "wake_up_process" },
	{ 0x6cb34e5, "init_waitqueue_head" },
	{ 0x35fe47a1, "init_timer" },
	{ 0xc818ff24, "kmem_cache_zalloc" },
	{ 0x37a0cba, "kfree" },
	{ 0xa185d238, "kthread_create" },
	{ 0x9d669763, "memcpy" },
	{ 0x153d9373, "sysfs_create_bin_file" },
	{ 0xb742fd7, "simple_strtol" },
	{ 0xa218bf61, "complete" },
	{ 0x45e87ae6, "add_uevent_var" },
};

static const char __module_depends[]
__attribute_used__
__attribute__((section(".modinfo"))) =
"depends=";

