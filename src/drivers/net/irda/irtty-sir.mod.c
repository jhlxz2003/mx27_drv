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
	{ 0x89b301d4, "param_get_int" },
	{ 0x98bd6f46, "param_set_int" },
	{ 0xb13d73c9, "malloc_sizes" },
	{ 0x8523fd53, "kmem_cache_alloc" },
	{ 0xb3341e91, "sirdev_get_instance" },
	{ 0x323222ba, "mutex_unlock" },
	{ 0xb97d4c9c, "mutex_lock" },
	{ 0xf9a482f9, "msleep" },
	{ 0x37a0cba, "kfree" },
	{ 0xb1f975aa, "unlock_kernel" },
	{ 0x3656bf5a, "lock_kernel" },
	{ 0xdc4fcaba, "sirdev_put_instance" },
	{ 0x67c2fa54, "__copy_to_user" },
	{ 0x328a05f1, "strncpy" },
	{ 0xfa2a45e, "__memzero" },
	{ 0x941c9874, "sirdev_set_dongle" },
	{ 0xce9f23a4, "n_tty_ioctl" },
	{ 0x6ce58c6e, "sirdev_receive" },
	{ 0x39c9cd17, "sirdev_write_complete" },
	{ 0xec0248ea, "tty_register_ldisc" },
	{ 0xdd132261, "printk" },
	{ 0xf6ebc03b, "net_ratelimit" },
	{ 0xa120d33c, "tty_unregister_ldisc" },
};

static const char __module_depends[]
__attribute_used__
__attribute__((section(".modinfo"))) =
"depends=sir-dev";

