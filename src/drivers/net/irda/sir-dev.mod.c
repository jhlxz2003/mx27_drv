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
	{ 0x12da5bb2, "__kmalloc" },
	{ 0xf9a482f9, "msleep" },
	{ 0x6b043eba, "irda_init_max_qos_capabilies" },
	{ 0xa88fa253, "async_wrap_skb" },
	{ 0xec5cef7d, "netif_carrier_off" },
	{ 0xeb6ed7a5, "queue_work" },
	{ 0x323222ba, "mutex_unlock" },
	{ 0xd6835a71, "irda_device_set_media_busy" },
	{ 0x7d11c268, "jiffies" },
	{ 0xc27140ac, "__create_workqueue" },
	{ 0xdd132261, "printk" },
	{ 0x2922d4f6, "free_netdev" },
	{ 0x328a05f1, "strncpy" },
	{ 0x3b36363c, "register_netdev" },
	{ 0xb97d4c9c, "mutex_lock" },
	{ 0xb02cfec1, "destroy_workqueue" },
	{ 0x43b0c9c3, "preempt_schedule" },
	{ 0x7f516b7f, "irlap_close" },
	{ 0xdfc30eca, "dev_kfree_skb_any" },
	{ 0x3e392c3e, "async_unwrap_char" },
	{ 0x7dceceac, "capable" },
	{ 0xcd9b67ea, "netif_device_detach" },
	{ 0x9e5581c, "__alloc_skb" },
	{ 0x77c37920, "kfree_skb" },
	{ 0x4e3e4b31, "wake_up_process" },
	{ 0x6cb34e5, "init_waitqueue_head" },
	{ 0x35fe47a1, "init_timer" },
	{ 0xf6ebc03b, "net_ratelimit" },
	{ 0xfb7d9c45, "__udivsi3" },
	{ 0x448b8aaa, "irda_qos_bits_to_value" },
	{ 0x37a0cba, "kfree" },
	{ 0x9d669763, "memcpy" },
	{ 0x98adfde2, "request_module" },
	{ 0x1042cbb5, "__up_wakeup" },
	{ 0xb20ee80d, "unregister_netdev" },
	{ 0xd187aaa3, "__netif_schedule" },
	{ 0x7a7ef853, "__down_failed" },
	{ 0x5015eb3c, "irlap_open" },
	{ 0x1bbdb1b9, "alloc_irdadev" },
	{ 0x11b5fb, "queue_delayed_work" },
	{ 0x9e7d6bd0, "__udelay" },
	{ 0x9388003b, "__down_trylock_failed" },
};

static const char __module_depends[]
__attribute_used__
__attribute__((section(".modinfo"))) =
"depends=irda";

