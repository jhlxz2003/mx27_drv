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
	{ 0x4d3c153f, "sigprocmask" },
	{ 0xd29ce242, "d_path" },
	{ 0x523574d3, "device_remove_file" },
	{ 0xcff53400, "kref_put" },
	{ 0x8e9eceaa, "complete_and_exit" },
	{ 0x350e2f4, "up_read" },
	{ 0x90c18ea8, "dequeue_signal" },
	{ 0x97255bdf, "strlen" },
	{ 0xe8c67d83, "filemap_fdatawait" },
	{ 0xbd8316c7, "bdev_read_only" },
	{ 0xb13d73c9, "malloc_sizes" },
	{ 0x806d5133, "param_array_get" },
	{ 0xae8cf290, "filp_close" },
	{ 0x323222ba, "mutex_unlock" },
	{ 0x53567768, "usb_gadget_unregister_driver" },
	{ 0x1d26aa98, "sprintf" },
	{ 0x89cef6fb, "param_array_set" },
	{ 0x5ebb56fb, "down_read" },
	{ 0xe2d5255a, "strcmp" },
	{ 0x12f237eb, "__kzalloc" },
	{ 0xaa136450, "param_get_charp" },
	{ 0x25fa6f17, "wait_for_completion" },
	{ 0x7b579ec1, "vfs_read" },
	{ 0xfa2a45e, "__memzero" },
	{ 0x9d7e642e, "device_register" },
	{ 0x8d3894f2, "_ctype" },
	{ 0xdd132261, "printk" },
	{ 0x859204af, "sscanf" },
	{ 0xdc76cbcb, "param_set_bool" },
	{ 0xb97d4c9c, "mutex_lock" },
	{ 0x7486289a, "init_uts_ns" },
	{ 0x43b0c9c3, "preempt_schedule" },
	{ 0x231cf494, "up_write" },
	{ 0xb3fe02b1, "down_write" },
	{ 0x55cf2e9e, "fput" },
	{ 0x8320bea8, "__umodsi3" },
	{ 0x1c92a596, "device_create_file" },
	{ 0x9f984513, "strrchr" },
	{ 0x8523fd53, "kmem_cache_alloc" },
	{ 0x8dd19577, "usb_gadget_register_driver" },
	{ 0x72216fa9, "param_get_uint" },
	{ 0x1000e51, "schedule" },
	{ 0x35c2ba9e, "refrigerator" },
	{ 0x483c2a76, "dev_driver_string" },
	{ 0x4e3e4b31, "wake_up_process" },
	{ 0xcc5005fe, "msleep_interruptible" },
	{ 0x6cb34e5, "init_waitqueue_head" },
	{ 0x2cd7da6c, "param_set_charp" },
	{ 0xc818ff24, "kmem_cache_zalloc" },
	{ 0x3ae831b6, "kref_init" },
	{ 0x37a0cba, "kfree" },
	{ 0xa185d238, "kthread_create" },
	{ 0x9d669763, "memcpy" },
	{ 0xd1f7dd57, "send_sig_info" },
	{ 0x8abac70a, "param_set_uint" },
	{ 0x8a1203a9, "kref_get" },
	{ 0x4d6573d5, "invalidate_inode_pages" },
	{ 0xe62d4aaa, "device_unregister" },
	{ 0x5611e4ec, "param_get_bool" },
	{ 0xb742fd7, "simple_strtol" },
	{ 0xa218bf61, "complete" },
	{ 0x25da070, "snprintf" },
	{ 0x99bb8806, "memmove" },
	{ 0xc22616f1, "__init_rwsem" },
	{ 0x8dee9d0c, "vfs_write" },
	{ 0x23640d4a, "filemap_fdatawrite" },
	{ 0x6aade82e, "filp_open" },
};

static const char __module_depends[]
__attribute_used__
__attribute__((section(".modinfo"))) =
"depends=arcotg_udc";

