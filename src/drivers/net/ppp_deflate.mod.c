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
	{ 0x3b8be29d, "zlib_inflateInit2" },
	{ 0x12da5bb2, "__kmalloc" },
	{ 0xce5ac24f, "zlib_inflate_workspacesize" },
	{ 0xb13d73c9, "malloc_sizes" },
	{ 0x70a6e8d9, "zlib_deflateInit2" },
	{ 0xd6ee688f, "vmalloc" },
	{ 0xf0caf44b, "zlib_deflate_workspacesize" },
	{ 0xfa2a45e, "__memzero" },
	{ 0x8523fd53, "kmem_cache_alloc" },
	{ 0x39b3bf37, "ppp_register_compressor" },
	{ 0x2fd1d81c, "vfree" },
	{ 0x53dbeea6, "zlib_deflateEnd" },
	{ 0x2b619e9d, "zlib_deflateReset" },
	{ 0x46bc0adc, "zlib_deflate" },
	{ 0x37a0cba, "kfree" },
	{ 0xba7921dc, "zlib_inflateEnd" },
	{ 0xf56a3962, "zlib_inflateReset" },
	{ 0x6b60eef6, "zlib_inflate" },
	{ 0xdd132261, "printk" },
	{ 0x6d9e4435, "zlib_inflateIncomp" },
	{ 0xa9830a8a, "ppp_unregister_compressor" },
};

static const char __module_depends[]
__attribute_used__
__attribute__((section(".modinfo"))) =
"depends=ppp_generic";

