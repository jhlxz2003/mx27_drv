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
	{ 0x72216fa9, "param_get_uint" },
	{ 0x8abac70a, "param_set_uint" },
	{ 0x89b301d4, "param_get_int" },
	{ 0x98bd6f46, "param_set_int" },
	{ 0xd0ee38b8, "schedule_timeout_uninterruptible" },
	{ 0xdbe55549, "usb_hcd_resume_root_hub" },
	{ 0xb13d73c9, "malloc_sizes" },
	{ 0xb53393be, "usb_get_urb" },
	{ 0xc818ff24, "kmem_cache_zalloc" },
	{ 0x75b38522, "del_timer" },
	{ 0x9d050ef0, "platform_driver_register" },
	{ 0xb615f708, "usb_add_hcd" },
	{ 0xec918425, "__ioremap" },
	{ 0x852abecf, "__request_region" },
	{ 0xb5063a2a, "usb_create_hcd" },
	{ 0x1167bcb2, "platform_get_resource" },
	{ 0x19a304ba, "usb_disabled" },
	{ 0x12f237eb, "__kzalloc" },
	{ 0x35c6811d, "dma_alloc_coherent" },
	{ 0xbe61e4b0, "dma_pool_create" },
	{ 0x35fe47a1, "init_timer" },
	{ 0x7d066ef, "dma_free_coherent" },
	{ 0xe91a3f29, "dma_pool_destroy" },
	{ 0x9d669763, "memcpy" },
	{ 0x7e64181d, "usb_calc_bus_time" },
	{ 0x6b2dc060, "dump_stack" },
	{ 0x3ae831b6, "kref_init" },
	{ 0xb174f0e4, "dma_pool_alloc" },
	{ 0xf9a482f9, "msleep" },
	{ 0x5f754e5a, "memset" },
	{ 0xfa2a45e, "__memzero" },
	{ 0x3c101f51, "usb_free_urb" },
	{ 0xc444d519, "usb_hub_tt_clear_buffer" },
	{ 0x20187c7, "mod_timer" },
	{ 0xdd132261, "printk" },
	{ 0x483c2a76, "dev_driver_string" },
	{ 0xfb7d9c45, "__udivsi3" },
	{ 0x37a0cba, "kfree" },
	{ 0x8a4f743c, "usb_hcd_giveback_urb" },
	{ 0x43b0c9c3, "preempt_schedule" },
	{ 0x9e7d6bd0, "__udelay" },
	{ 0xcff53400, "kref_put" },
	{ 0x8a1203a9, "kref_get" },
	{ 0x211331fa, "__divsi3" },
	{ 0xcbb8d2b3, "dma_pool_free" },
	{ 0x594e1317, "__modsi3" },
	{ 0x7d11c268, "jiffies" },
	{ 0x8320bea8, "__umodsi3" },
	{ 0xeae3dfd6, "__const_udelay" },
	{ 0x1f9cfe83, "iomem_resource" },
	{ 0xb309ef9f, "put_device" },
	{ 0x14a1c7a0, "usb_put_hcd" },
	{ 0xef79ac56, "__release_region" },
	{ 0x45a55ec8, "__iounmap" },
	{ 0x2888a36e, "usb_remove_hcd" },
	{ 0x8bfafebf, "platform_driver_unregister" },
};

static const char __module_depends[]
__attribute_used__
__attribute__((section(".modinfo"))) =
"depends=usbcore";

