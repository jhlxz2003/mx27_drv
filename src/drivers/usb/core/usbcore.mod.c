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
	{ 0x7123d6f0, "bus_register" },
	{ 0x2d6507b5, "_find_next_zero_bit_le" },
	{ 0x523574d3, "device_remove_file" },
	{ 0x5469552b, "cdev_del" },
	{ 0x12da5bb2, "__kmalloc" },
	{ 0xc84db0a1, "cdev_init" },
	{ 0xf9a482f9, "msleep" },
	{ 0xcff53400, "kref_put" },
	{ 0xfa65f658, "put_pid" },
	{ 0xac92b01e, "driver_register" },
	{ 0xfbc74f64, "__copy_from_user" },
	{ 0x350e2f4, "up_read" },
	{ 0x45654c68, "mem_map" },
	{ 0x67c2fa54, "__copy_to_user" },
	{ 0x75b38522, "del_timer" },
	{ 0x97255bdf, "strlen" },
	{ 0xbd0e9324, "device_release_driver" },
	{ 0xcccca482, "_test_and_clear_bit_le" },
	{ 0xd8e484f0, "register_chrdev_region" },
	{ 0xc8b57c27, "autoremove_wake_function" },
	{ 0x594e1317, "__modsi3" },
	{ 0x1a4ad627, "class_device_destroy" },
	{ 0x74cc238d, "current_kernel_time" },
	{ 0xf9e825e0, "class_device_create" },
	{ 0xb13d73c9, "malloc_sizes" },
	{ 0x36e47222, "remove_wait_queue" },
	{ 0x7ad5eec9, "schedule_work" },
	{ 0x353e3fa5, "__get_user_4" },
	{ 0x52fb6c82, "driver_for_each_device" },
	{ 0xdce9a993, "class_device_unregister" },
	{ 0xc8ffeefb, "consistent_sync" },
	{ 0x323222ba, "mutex_unlock" },
	{ 0x7485e15e, "unregister_chrdev_region" },
	{ 0xb4a4b9fb, "blocking_notifier_chain_unregister" },
	{ 0xe91a3f29, "dma_pool_destroy" },
	{ 0x1d26aa98, "sprintf" },
	{ 0x56761af3, "sysfs_remove_group" },
	{ 0x7d11c268, "jiffies" },
	{ 0x5ebb56fb, "down_read" },
	{ 0x211331fa, "__divsi3" },
	{ 0x2c0fc782, "bus_rescan_devices" },
	{ 0x12f237eb, "__kzalloc" },
	{ 0xc27140ac, "__create_workqueue" },
	{ 0xffd5a395, "default_wake_function" },
	{ 0x25fa6f17, "wait_for_completion" },
	{ 0xfa2a45e, "__memzero" },
	{ 0x5f754e5a, "memset" },
	{ 0xd0ba8cc, "device_bind_driver" },
	{ 0x534c8c84, "device_del" },
	{ 0x9d7e642e, "device_register" },
	{ 0x8d3894f2, "_ctype" },
	{ 0x6c36a5c1, "__mutex_init" },
	{ 0xdd132261, "printk" },
	{ 0x859204af, "sscanf" },
	{ 0x41135a20, "kthread_stop" },
	{ 0x5e2534fe, "sysfs_create_group" },
	{ 0x71c90087, "memcmp" },
	{ 0x3656bf5a, "lock_kernel" },
	{ 0xde99adde, "driver_unregister" },
	{ 0xdc76cbcb, "param_set_bool" },
	{ 0x328a05f1, "strncpy" },
	{ 0x7d066ef, "dma_free_coherent" },
	{ 0x73e20c1c, "strlcpy" },
	{ 0xb97d4c9c, "mutex_lock" },
	{ 0xb02cfec1, "destroy_workqueue" },
	{ 0x12f6228, "class_create" },
	{ 0x8ea33a7f, "put_driver" },
	{ 0x20187c7, "mod_timer" },
	{ 0x7486289a, "init_uts_ns" },
	{ 0xcbb8d2b3, "dma_pool_free" },
	{ 0x74e0c240, "sysfs_remove_link" },
	{ 0xcb9bfc4e, "device_add" },
	{ 0x43b0c9c3, "preempt_schedule" },
	{ 0x231cf494, "up_write" },
	{ 0xb3fe02b1, "down_write" },
	{ 0x7eb1013, "bus_unregister" },
	{ 0xe2dac8eb, "kill_pid_info_as_uid" },
	{ 0x5093fa82, "_clear_bit_le" },
	{ 0x35c6811d, "dma_alloc_coherent" },
	{ 0x8320bea8, "__umodsi3" },
	{ 0x1c92a596, "device_create_file" },
	{ 0x496d4504, "cdev_add" },
	{ 0xb1f975aa, "unlock_kernel" },
	{ 0x99518da3, "sysfs_create_link" },
	{ 0x9f984513, "strrchr" },
	{ 0x8523fd53, "kmem_cache_alloc" },
	{ 0xbc10dd97, "__put_user_4" },
	{ 0xb309ef9f, "put_device" },
	{ 0x93fca811, "__get_free_pages" },
	{ 0x1a3d5abf, "schedule_delayed_work" },
	{ 0xd741cfcf, "blocking_notifier_call_chain" },
	{ 0x2cf190e3, "request_irq" },
	{ 0x1000e51, "schedule" },
	{ 0x35c2ba9e, "refrigerator" },
	{ 0x6b2dc060, "dump_stack" },
	{ 0xe9393954, "register_chrdev" },
	{ 0x483c2a76, "dev_driver_string" },
	{ 0x3a92af5d, "sysfs_create_file" },
	{ 0xb174f0e4, "dma_pool_alloc" },
	{ 0x4e3e4b31, "wake_up_process" },
	{ 0x98196f5b, "blocking_notifier_chain_register" },
	{ 0x5a4fe93c, "get_device" },
	{ 0x6cb34e5, "init_waitqueue_head" },
	{ 0x35fe47a1, "init_timer" },
	{ 0x4302d0eb, "free_pages" },
	{ 0xb6c70a7d, "__wake_up" },
	{ 0xd2965f6f, "kthread_should_stop" },
	{ 0xc818ff24, "kmem_cache_zalloc" },
	{ 0xfb7d9c45, "__udivsi3" },
	{ 0x3146a8eb, "get_driver" },
	{ 0xfd8dba4c, "add_wait_queue" },
	{ 0x3ae831b6, "kref_init" },
	{ 0x37a0cba, "kfree" },
	{ 0xa185d238, "kthread_create" },
	{ 0x9d669763, "memcpy" },
	{ 0x801678, "flush_scheduled_work" },
	{ 0x8085c7b1, "prepare_to_wait" },
	{ 0x8a1203a9, "kref_get" },
	{ 0xd3eb1d25, "device_initialize" },
	{ 0xc192d491, "unregister_chrdev" },
	{ 0xe9c04db1, "sysfs_remove_file" },
	{ 0x6deb3169, "class_destroy" },
	{ 0x51493d94, "finish_wait" },
	{ 0xe62d4aaa, "device_unregister" },
	{ 0x555ec45c, "driver_attach" },
	{ 0x1042cbb5, "__up_wakeup" },
	{ 0x5611e4ec, "param_get_bool" },
	{ 0xa218bf61, "complete" },
	{ 0x25da070, "snprintf" },
	{ 0x7a7ef853, "__down_failed" },
	{ 0x1fae7a0b, "wait_for_completion_timeout" },
	{ 0xbe61e4b0, "dma_pool_create" },
	{ 0x9388003b, "__down_trylock_failed" },
	{ 0x760a0f4f, "yield" },
	{ 0xf20dabd8, "free_irq" },
	{ 0x45e87ae6, "add_uevent_var" },
	{ 0x313341a3, "_set_bit_le" },
};

static const char __module_depends[]
__attribute_used__
__attribute__((section(".modinfo"))) =
"depends=";

MODULE_ALIAS("usb:v*p*d*dc09dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v*p*d*dc*dsc*dp*ic09isc*ip*");