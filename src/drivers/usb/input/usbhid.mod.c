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
	{ 0x2d6507b5, "_find_next_zero_bit_le" },
	{ 0x12da5bb2, "__kmalloc" },
	{ 0x32ce5633, "usb_buffer_alloc" },
	{ 0x9924c496, "__usb_get_extra_descriptor" },
	{ 0xfbc74f64, "__copy_from_user" },
	{ 0x67c2fa54, "__copy_to_user" },
	{ 0x75b38522, "del_timer" },
	{ 0x97255bdf, "strlen" },
	{ 0xc8b57c27, "autoremove_wake_function" },
	{ 0xb13d73c9, "malloc_sizes" },
	{ 0x36e47222, "remove_wait_queue" },
	{ 0x91cf88aa, "input_ff_event" },
	{ 0x7ad5eec9, "schedule_work" },
	{ 0xb16f3bb5, "usb_buffer_free" },
	{ 0x864e64f6, "usb_kill_urb" },
	{ 0x14ac9f6, "usb_deregister_dev" },
	{ 0x353e3fa5, "__get_user_4" },
	{ 0x7d11c268, "jiffies" },
	{ 0x211331fa, "__divsi3" },
	{ 0x12f237eb, "__kzalloc" },
	{ 0x2642cebd, "input_event" },
	{ 0xffd5a395, "default_wake_function" },
	{ 0xdd0a2ba2, "strlcat" },
	{ 0xfa2a45e, "__memzero" },
	{ 0x5f754e5a, "memset" },
	{ 0x78c4a377, "usb_string" },
	{ 0x9036f99, "usb_lock_device_for_reset" },
	{ 0x216a2fcb, "usb_deregister" },
	{ 0xdd132261, "printk" },
	{ 0x8635104f, "fasync_helper" },
	{ 0x59445190, "usb_register_dev" },
	{ 0xf007f499, "usb_control_msg" },
	{ 0x73e20c1c, "strlcpy" },
	{ 0x20187c7, "mod_timer" },
	{ 0x43b0c9c3, "preempt_schedule" },
	{ 0x5093fa82, "_clear_bit_le" },
	{ 0xff67b37f, "__lshrdi3" },
	{ 0xadcf2309, "usb_submit_urb" },
	{ 0x8523fd53, "kmem_cache_alloc" },
	{ 0xbc10dd97, "__put_user_4" },
	{ 0x72216fa9, "param_get_uint" },
	{ 0x8030f58b, "input_register_device" },
	{ 0xd62c833f, "schedule_timeout" },
	{ 0x1000e51, "schedule" },
	{ 0x6b2dc060, "dump_stack" },
	{ 0x7fcbc1ab, "input_free_device" },
	{ 0x6cb34e5, "init_waitqueue_head" },
	{ 0x35fe47a1, "init_timer" },
	{ 0xb6c70a7d, "__wake_up" },
	{ 0xc818ff24, "kmem_cache_zalloc" },
	{ 0xfb7d9c45, "__udivsi3" },
	{ 0xfd8dba4c, "add_wait_queue" },
	{ 0x37a0cba, "kfree" },
	{ 0x9d669763, "memcpy" },
	{ 0xa99804c3, "input_unregister_device" },
	{ 0xda5ea696, "_test_and_set_bit_le" },
	{ 0x801678, "flush_scheduled_work" },
	{ 0x8085c7b1, "prepare_to_wait" },
	{ 0x1bb1ab66, "usb_reset_composite_device" },
	{ 0x8abac70a, "param_set_uint" },
	{ 0xd9e75618, "usb_register_driver" },
	{ 0x9f68ec10, "kill_fasync" },
	{ 0x51493d94, "finish_wait" },
	{ 0x1042cbb5, "__up_wakeup" },
	{ 0x25da070, "snprintf" },
	{ 0x3c101f51, "usb_free_urb" },
	{ 0xa32df1f7, "usb_alloc_urb" },
	{ 0x40f07981, "__ashldi3" },
	{ 0x313341a3, "_set_bit_le" },
	{ 0x5476dd3f, "input_allocate_device" },
};

static const char __module_depends[]
__attribute_used__
__attribute__((section(".modinfo"))) =
"depends=usbcore";

MODULE_ALIAS("usb:v*p*d*dc*dsc*dp*ic03isc*ip*");
