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
	{ 0x5c4a5deb, "stop_tty" },
	{ 0x668c634b, "start_tty" },
	{ 0x8757149d, "skb_queue_tail" },
	{ 0xc4693e56, "skb_under_panic" },
	{ 0x6b2dc060, "dump_stack" },
	{ 0x9d669763, "memcpy" },
	{ 0xc5589b96, "skb_over_panic" },
	{ 0x9e5581c, "__alloc_skb" },
	{ 0xfbc74f64, "__copy_from_user" },
	{ 0x67c2fa54, "__copy_to_user" },
	{ 0x353e3fa5, "__get_user_4" },
	{ 0x11088983, "skb_dequeue" },
	{ 0x1cc121ce, "ppp_input" },
	{ 0xbefaa0b9, "ppp_input_error" },
	{ 0xb13d73c9, "malloc_sizes" },
	{ 0xda161cab, "ppp_register_channel" },
	{ 0x6cb34e5, "init_waitqueue_head" },
	{ 0xa5808bbf, "tasklet_init" },
	{ 0xfa2a45e, "__memzero" },
	{ 0x8523fd53, "kmem_cache_alloc" },
	{ 0x37a0cba, "kfree" },
	{ 0x8ce0db53, "skb_queue_purge" },
	{ 0x75cac46f, "ppp_unregister_channel" },
	{ 0x79ad224b, "tasklet_kill" },
	{ 0x7a7ef853, "__down_failed" },
	{ 0xce9f23a4, "n_tty_ioctl" },
	{ 0xccad87cf, "ppp_output_wakeup" },
	{ 0xd6aff9a8, "ppp_unit_number" },
	{ 0xbc10dd97, "__put_user_4" },
	{ 0x104ce76a, "ppp_channel_index" },
	{ 0x7d11c268, "jiffies" },
	{ 0x799aca4, "local_bh_enable" },
	{ 0x77c37920, "kfree_skb" },
	{ 0x3ff62317, "local_bh_disable" },
	{ 0xf397b9aa, "__tasklet_schedule" },
	{ 0x43b0c9c3, "preempt_schedule" },
	{ 0xec0248ea, "tty_register_ldisc" },
	{ 0xdd132261, "printk" },
	{ 0xa120d33c, "tty_unregister_ldisc" },
	{ 0x75811312, "crc_ccitt_table" },
	{ 0x1042cbb5, "__up_wakeup" },
};

static const char __module_depends[]
__attribute_used__
__attribute__((section(".modinfo"))) =
"depends=ppp_generic,crc-ccitt";

