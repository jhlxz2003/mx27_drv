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
	{ 0x5364fef8, "skb_queue_head" },
	{ 0xfbc74f64, "__copy_from_user" },
	{ 0x350e2f4, "up_read" },
	{ 0x67c2fa54, "__copy_to_user" },
	{ 0xd3dbfbc4, "_find_first_zero_bit_le" },
	{ 0x1a4ad627, "class_device_destroy" },
	{ 0xf9e825e0, "class_device_create" },
	{ 0xb13d73c9, "malloc_sizes" },
	{ 0x36e47222, "remove_wait_queue" },
	{ 0x24c9e2a0, "slhc_init" },
	{ 0x353e3fa5, "__get_user_4" },
	{ 0x996e260e, "slhc_remember" },
	{ 0x735016d, "slhc_uncompress" },
	{ 0xf45e07e7, "alloc_netdev" },
	{ 0x323222ba, "mutex_unlock" },
	{ 0x1d26aa98, "sprintf" },
	{ 0x7d11c268, "jiffies" },
	{ 0x5ebb56fb, "down_read" },
	{ 0x74ac5cf5, "netif_rx" },
	{ 0xf4279a33, "__pskb_pull_tail" },
	{ 0xffd5a395, "default_wake_function" },
	{ 0xfa2a45e, "__memzero" },
	{ 0x8ce0db53, "skb_queue_purge" },
	{ 0x5f754e5a, "memset" },
	{ 0xdd132261, "printk" },
	{ 0x2922d4f6, "free_netdev" },
	{ 0x3b36363c, "register_netdev" },
	{ 0xb97d4c9c, "mutex_lock" },
	{ 0x12f6228, "class_create" },
	{ 0x43b0c9c3, "preempt_schedule" },
	{ 0x231cf494, "up_write" },
	{ 0xb3fe02b1, "down_write" },
	{ 0x4565aaa6, "slhc_free" },
	{ 0x5093fa82, "_clear_bit_le" },
	{ 0xc5589b96, "skb_over_panic" },
	{ 0x8757149d, "skb_queue_tail" },
	{ 0x7dceceac, "capable" },
	{ 0x3ff62317, "local_bh_disable" },
	{ 0x8523fd53, "kmem_cache_alloc" },
	{ 0xbc10dd97, "__put_user_4" },
	{ 0x9e5581c, "__alloc_skb" },
	{ 0x1000e51, "schedule" },
	{ 0x77c37920, "kfree_skb" },
	{ 0x6b2dc060, "dump_stack" },
	{ 0x799aca4, "local_bh_enable" },
	{ 0xc4693e56, "skb_under_panic" },
	{ 0xe9393954, "register_chrdev" },
	{ 0x4e3e4b31, "wake_up_process" },
	{ 0x602ecc9b, "skb_pull_rcsum" },
	{ 0x6cb34e5, "init_waitqueue_head" },
	{ 0xb6c70a7d, "__wake_up" },
	{ 0xf6ebc03b, "net_ratelimit" },
	{ 0xc818ff24, "kmem_cache_zalloc" },
	{ 0xfb7d9c45, "__udivsi3" },
	{ 0xfd8dba4c, "add_wait_queue" },
	{ 0x37a0cba, "kfree" },
	{ 0x98adfde2, "request_module" },
	{ 0xc192d491, "unregister_chrdev" },
	{ 0xab662b1e, "slhc_compress" },
	{ 0x6deb3169, "class_destroy" },
	{ 0x11088983, "skb_dequeue" },
	{ 0xb20ee80d, "unregister_netdev" },
	{ 0xd187aaa3, "__netif_schedule" },
	{ 0x4d9e6045, "skb_copy_bits" },
	{ 0x1a75e2c5, "slhc_toss" },
	{ 0xc22616f1, "__init_rwsem" },
	{ 0x313341a3, "_set_bit_le" },
};

static const char __module_depends[]
__attribute_used__
__attribute__((section(".modinfo"))) =
"depends=slhc";

