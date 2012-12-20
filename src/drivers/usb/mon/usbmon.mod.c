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
	{ 0xaadf600c, "kmem_cache_destroy" },
	{ 0xcff53400, "kref_put" },
	{ 0x67c2fa54, "__copy_to_user" },
	{ 0xb13d73c9, "malloc_sizes" },
	{ 0x36e47222, "remove_wait_queue" },
	{ 0x7eeeac05, "no_llseek" },
	{ 0x323222ba, "mutex_unlock" },
	{ 0xffd5a395, "default_wake_function" },
	{ 0x5f754e5a, "memset" },
	{ 0x6c36a5c1, "__mutex_init" },
	{ 0xdd132261, "printk" },
	{ 0xb2288b87, "kmem_cache_free" },
	{ 0xb97d4c9c, "mutex_lock" },
	{ 0xfed11ed1, "usb_mon_deregister" },
	{ 0x43b0c9c3, "preempt_schedule" },
	{ 0xe9587909, "usb_unregister_notify" },
	{ 0x8523fd53, "kmem_cache_alloc" },
	{ 0x1000e51, "schedule" },
	{ 0x86ea1981, "kmem_cache_create" },
	{ 0x6cb34e5, "init_waitqueue_head" },
	{ 0xb6c70a7d, "__wake_up" },
	{ 0xc818ff24, "kmem_cache_zalloc" },
	{ 0x72270e35, "do_gettimeofday" },
	{ 0xfd8dba4c, "add_wait_queue" },
	{ 0x3ae831b6, "kref_init" },
	{ 0x37a0cba, "kfree" },
	{ 0x9d669763, "memcpy" },
	{ 0x8a1203a9, "kref_get" },
	{ 0x25da070, "snprintf" },
};

static const char __module_depends[]
__attribute_used__
__attribute__((section(".modinfo"))) =
"depends=usbcore";

