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
	{ 0xb13d73c9, "malloc_sizes" },
	{ 0xc8ffeefb, "consistent_sync" },
	{ 0xe91a3f29, "dma_pool_destroy" },
	{ 0xe2d5255a, "strcmp" },
	{ 0x25fa6f17, "wait_for_completion" },
	{ 0xfa2a45e, "__memzero" },
	{ 0xdd132261, "printk" },
	{ 0x7d066ef, "dma_free_coherent" },
	{ 0x1e6d26a8, "strstr" },
	{ 0xcbb8d2b3, "dma_pool_free" },
	{ 0x9d050ef0, "platform_driver_register" },
	{ 0xcb9bfc4e, "device_add" },
	{ 0x43b0c9c3, "preempt_schedule" },
	{ 0x35c6811d, "dma_alloc_coherent" },
	{ 0x8523fd53, "kmem_cache_alloc" },
	{ 0xb309ef9f, "put_device" },
	{ 0x2cf190e3, "request_irq" },
	{ 0x6b2dc060, "dump_stack" },
	{ 0xb174f0e4, "dma_pool_alloc" },
	{ 0x37a0cba, "kfree" },
	{ 0x9d669763, "memcpy" },
	{ 0xd3eb1d25, "device_initialize" },
	{ 0xe62d4aaa, "device_unregister" },
	{ 0xa218bf61, "complete" },
	{ 0x8bfafebf, "platform_driver_unregister" },
	{ 0xbe61e4b0, "dma_pool_create" },
	{ 0xf20dabd8, "free_irq" },
	{ 0xe914e41e, "strcpy" },
};

static const char __module_depends[]
__attribute_used__
__attribute__((section(".modinfo"))) =
"depends=";

