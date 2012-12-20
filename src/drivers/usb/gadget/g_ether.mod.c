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
	{ 0x8157fe10, "alloc_etherdev" },
	{ 0x97255bdf, "strlen" },
	{ 0x594e1317, "__modsi3" },
	{ 0x79aa04a2, "get_random_bytes" },
	{ 0xf75836c1, "netif_carrier_on" },
	{ 0x7ad5eec9, "schedule_work" },
	{ 0xec5cef7d, "netif_carrier_off" },
	{ 0x53567768, "usb_gadget_unregister_driver" },
	{ 0x93b9ce26, "skb_realloc_headroom" },
	{ 0x7d11c268, "jiffies" },
	{ 0xe2d5255a, "strcmp" },
	{ 0x211331fa, "__divsi3" },
	{ 0x74ac5cf5, "netif_rx" },
	{ 0xaa136450, "param_get_charp" },
	{ 0xfa2a45e, "__memzero" },
	{ 0x8d3894f2, "_ctype" },
	{ 0xdd132261, "printk" },
	{ 0x71c90087, "memcmp" },
	{ 0x2922d4f6, "free_netdev" },
	{ 0x3b36363c, "register_netdev" },
	{ 0x73e20c1c, "strlcpy" },
	{ 0x7486289a, "init_uts_ns" },
	{ 0x43b0c9c3, "preempt_schedule" },
	{ 0xdfc30eca, "dev_kfree_skb_any" },
	{ 0x8320bea8, "__umodsi3" },
	{ 0xc5589b96, "skb_over_panic" },
	{ 0x9f984513, "strrchr" },
	{ 0x8dd19577, "usb_gadget_register_driver" },
	{ 0x9e5581c, "__alloc_skb" },
	{ 0x5418d52a, "param_get_ushort" },
	{ 0x72216fa9, "param_get_uint" },
	{ 0x6b2dc060, "dump_stack" },
	{ 0xc4693e56, "skb_under_panic" },
	{ 0x4f1aa533, "eth_type_trans" },
	{ 0x483c2a76, "dev_driver_string" },
	{ 0x35fe47a1, "init_timer" },
	{ 0x2cd7da6c, "param_set_charp" },
	{ 0x37a0cba, "kfree" },
	{ 0x9d669763, "memcpy" },
	{ 0xda5ea696, "_test_and_set_bit_le" },
	{ 0x801678, "flush_scheduled_work" },
	{ 0x8abac70a, "param_set_uint" },
	{ 0xb20ee80d, "unregister_netdev" },
	{ 0xb742fd7, "simple_strtol" },
	{ 0x25da070, "snprintf" },
	{ 0xd187aaa3, "__netif_schedule" },
	{ 0xe914e41e, "strcpy" },
	{ 0xc4c5509d, "param_set_ushort" },
};

static const char __module_depends[]
__attribute_used__
__attribute__((section(".modinfo"))) =
"depends=arcotg_udc";

