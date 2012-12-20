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
	{ 0x6483655c, "param_get_short" },
	{ 0x40046949, "param_set_short" },
	{ 0x7006b18f, "platform_bus_type" },
	{ 0x806d5133, "param_array_get" },
	{ 0x89cef6fb, "param_array_set" },
	{ 0x67c2fa54, "__copy_to_user" },
	{ 0xfbc74f64, "__copy_from_user" },
	{ 0x87103356, "i2c_detach_client" },
	{ 0x89d4aacc, "i2c_probe" },
	{ 0xb13d73c9, "malloc_sizes" },
	{ 0x37a0cba, "kfree" },
	{ 0xb957efe9, "i2c_attach_client" },
	{ 0xe914e41e, "strcpy" },
	{ 0x8523fd53, "kmem_cache_alloc" },
	{ 0x9a8c9328, "gpio_si4702_inactive" },
	{ 0x5e2534fe, "sysfs_create_group" },
	{ 0x53f0ecdb, "subsystem_register" },
	{ 0x9d050ef0, "platform_driver_register" },
	{ 0xf1d265d7, "i2c_register_driver" },
	{ 0xf9e825e0, "class_device_create" },
	{ 0x12f6228, "class_create" },
	{ 0xe9393954, "register_chrdev" },
	{ 0x84b183ae, "strncmp" },
	{ 0xdd27fa87, "memchr" },
	{ 0xfb7d9c45, "__udivsi3" },
	{ 0xf9a482f9, "msleep" },
	{ 0x56ddaf87, "gpio_si4702_reset" },
	{ 0xee0ccb4e, "gpio_si4702_active" },
	{ 0x43b0c9c3, "preempt_schedule" },
	{ 0xeae3dfd6, "__const_udelay" },
	{ 0xf2c75c2a, "gpio_si4702_powerdown" },
	{ 0x8bfafebf, "platform_driver_unregister" },
	{ 0xb468220b, "subsystem_unregister" },
	{ 0x56761af3, "sysfs_remove_group" },
	{ 0x460c35fc, "i2c_del_driver" },
	{ 0x6deb3169, "class_destroy" },
	{ 0x1a4ad627, "class_device_destroy" },
	{ 0xc192d491, "unregister_chrdev" },
	{ 0xdd132261, "printk" },
	{ 0x60d7e921, "i2c_transfer" },
	{ 0xfa2a45e, "__memzero" },
};

static const char __module_depends[]
__attribute_used__
__attribute__((section(".modinfo"))) =
"depends=";

