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
	{ 0xfbc74f64, "__copy_from_user" },
	{ 0x67c2fa54, "__copy_to_user" },
	{ 0xc8b57c27, "autoremove_wake_function" },
	{ 0x74cc238d, "current_kernel_time" },
	{ 0x76dfd0bb, "kick_iocb" },
	{ 0xb13d73c9, "malloc_sizes" },
	{ 0x7eeeac05, "no_llseek" },
	{ 0x311c3cdd, "generic_delete_inode" },
	{ 0xbcb028b3, "dput" },
	{ 0x323222ba, "mutex_unlock" },
	{ 0xb77e76ec, "aio_complete" },
	{ 0x53567768, "usb_gadget_unregister_driver" },
	{ 0xe2d5255a, "strcmp" },
	{ 0x64ab8c9f, "d_delete" },
	{ 0xfa2a45e, "__memzero" },
	{ 0x2e1fc00, "get_sb_single" },
	{ 0x5f754e5a, "memset" },
	{ 0x58247ac, "kill_litter_super" },
	{ 0x829871bc, "__down_interruptible_failed" },
	{ 0xdd132261, "printk" },
	{ 0xaae3595, "d_rehash" },
	{ 0x5c48665b, "d_alloc_root" },
	{ 0x81f71357, "aio_put_req" },
	{ 0x328a05f1, "strncpy" },
	{ 0x8635104f, "fasync_helper" },
	{ 0xb97d4c9c, "mutex_lock" },
	{ 0x43b0c9c3, "preempt_schedule" },
	{ 0x8320bea8, "__umodsi3" },
	{ 0x8523fd53, "kmem_cache_alloc" },
	{ 0x8dd19577, "usb_gadget_register_driver" },
	{ 0x453690be, "simple_dir_operations" },
	{ 0x72216fa9, "param_get_uint" },
	{ 0x1000e51, "schedule" },
	{ 0xdac2205b, "register_filesystem" },
	{ 0x6cb34e5, "init_waitqueue_head" },
	{ 0xb6c70a7d, "__wake_up" },
	{ 0xc818ff24, "kmem_cache_zalloc" },
	{ 0xfb7d9c45, "__udivsi3" },
	{ 0xb6897257, "iput" },
	{ 0x37a0cba, "kfree" },
	{ 0x9d669763, "memcpy" },
	{ 0x8085c7b1, "prepare_to_wait" },
	{ 0x8abac70a, "param_set_uint" },
	{ 0x9f68ec10, "kill_fasync" },
	{ 0xf1c69456, "simple_statfs" },
	{ 0xc63383c0, "d_alloc_name" },
	{ 0x51493d94, "finish_wait" },
	{ 0x1042cbb5, "__up_wakeup" },
	{ 0xa801ca79, "unregister_filesystem" },
	{ 0xa218bf61, "complete" },
	{ 0xa4f72103, "new_inode" },
	{ 0x99bb8806, "memmove" },
	{ 0x610c7020, "simple_dir_inode_operations" },
	{ 0xab318d88, "d_instantiate" },
	{ 0x9388003b, "__down_trylock_failed" },
};

static const char __module_depends[]
__attribute_used__
__attribute__((section(".modinfo"))) =
"depends=arcotg_udc";

