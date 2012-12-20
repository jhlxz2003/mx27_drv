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
	{ 0x20187c7, "mod_timer" },
	{ 0x2642cebd, "input_event" },
	{ 0x7fcbc1ab, "input_free_device" },
	{ 0x8030f58b, "input_register_device" },
	{ 0x2cf190e3, "request_irq" },
	{ 0x35fe47a1, "init_timer" },
	{ 0x5476dd3f, "input_allocate_device" },
	{ 0xa99804c3, "input_unregister_device" },
	{ 0x75b38522, "del_timer" },
	{ 0xf20dabd8, "free_irq" },
	{ 0xdd132261, "printk" },
	{ 0xeae3dfd6, "__const_udelay" },
	{ 0x832918f, "ssi_ac97_write" },
	{ 0x7d11c268, "jiffies" },
	{ 0xfcec0987, "enable_irq" },
	{ 0x9afe9c91, "__mod_timer" },
	{ 0x10e29d31, "ssi_ac97_read" },
	{ 0x3ce4ca6f, "disable_irq" },
};

static const char __module_depends[]
__attribute_used__
__attribute__((section(".modinfo"))) =
"depends=ssi_ac97_mod";

