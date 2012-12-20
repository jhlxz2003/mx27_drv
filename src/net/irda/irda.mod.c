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
	{ 0x9cacaad6, "proc_dointvec_minmax" },
	{ 0x5364fef8, "skb_queue_head" },
	{ 0xb3073a72, "release_sock" },
	{ 0x12da5bb2, "__kmalloc" },
	{ 0x9afe9c91, "__mod_timer" },
	{ 0x600a772f, "sock_init_data" },
	{ 0xfbc74f64, "__copy_from_user" },
	{ 0x4d02acb4, "register_sysctl_table" },
	{ 0x67c2fa54, "__copy_to_user" },
	{ 0x75b38522, "del_timer" },
	{ 0x97255bdf, "strlen" },
	{ 0x85f54af5, "seq_open" },
	{ 0xf94b56dd, "seq_release_private" },
	{ 0xc8b57c27, "autoremove_wake_function" },
	{ 0x875bcc89, "send_sig" },
	{ 0x594e1317, "__modsi3" },
	{ 0x79aa04a2, "get_random_bytes" },
	{ 0xb13d73c9, "malloc_sizes" },
	{ 0xb21af63a, "seq_puts" },
	{ 0x36e47222, "remove_wait_queue" },
	{ 0x3797cc72, "proc_dointvec" },
	{ 0x32762fe5, "skb_clone" },
	{ 0xc907742b, "skb_copy" },
	{ 0xdfef8de4, "seq_printf" },
	{ 0xc3f6f428, "remove_proc_entry" },
	{ 0x5c2d0d57, "sock_queue_rcv_skb" },
	{ 0x353e3fa5, "__get_user_4" },
	{ 0xd9a96e7, "skb_recv_datagram" },
	{ 0x75811312, "crc_ccitt_table" },
	{ 0xf45e07e7, "alloc_netdev" },
	{ 0x3c37f2f2, "seq_read" },
	{ 0x7d11c268, "jiffies" },
	{ 0xe2d5255a, "strcmp" },
	{ 0x211331fa, "__divsi3" },
	{ 0x74ac5cf5, "netif_rx" },
	{ 0x3b70c71b, "sock_no_sendpage" },
	{ 0xf4279a33, "__pskb_pull_tail" },
	{ 0xda7c971a, "sock_get_timestamp" },
	{ 0x4d45e3b0, "sock_no_mmap" },
	{ 0xffd5a395, "default_wake_function" },
	{ 0xfa2a45e, "__memzero" },
	{ 0x8ce0db53, "skb_queue_purge" },
	{ 0x5f754e5a, "memset" },
	{ 0xcd496ba4, "sock_no_socketpair" },
	{ 0x51da2e73, "proc_mkdir" },
	{ 0x1cfb6db9, "proc_net" },
	{ 0xb753c706, "sk_alloc" },
	{ 0xdd132261, "printk" },
	{ 0x3137836c, "sysctl_string" },
	{ 0x328a05f1, "strncpy" },
	{ 0x21fc38f, "seq_putc" },
	{ 0xbfd7f99d, "sysctl_intvec" },
	{ 0x73e20c1c, "strlcpy" },
	{ 0x8ba1a275, "sk_free" },
	{ 0xc527106f, "dev_remove_pack" },
	{ 0x20187c7, "mod_timer" },
	{ 0x43b0c9c3, "preempt_schedule" },
	{ 0x4efabb52, "proc_dostring" },
	{ 0x8320bea8, "__umodsi3" },
	{ 0x551c634, "lock_sock" },
	{ 0xc5589b96, "skb_over_panic" },
	{ 0x8757149d, "skb_queue_tail" },
	{ 0x7dceceac, "capable" },
	{ 0x3ff62317, "local_bh_disable" },
	{ 0xaa484e4e, "proto_register" },
	{ 0x8523fd53, "kmem_cache_alloc" },
	{ 0x9ceb163c, "memcpy_toiovec" },
	{ 0xbc10dd97, "__put_user_4" },
	{ 0x9e5581c, "__alloc_skb" },
	{ 0x5e3f7692, "unregister_sysctl_table" },
	{ 0x8b44f9c7, "datagram_poll" },
	{ 0x1d029bbd, "sock_register" },
	{ 0x1000e51, "schedule" },
	{ 0x77c37920, "kfree_skb" },
	{ 0xec75d206, "proto_unregister" },
	{ 0x6b2dc060, "dump_stack" },
	{ 0x799aca4, "local_bh_enable" },
	{ 0xc4693e56, "skb_under_panic" },
	{ 0x85f3efed, "sock_alloc_send_skb" },
	{ 0xff638178, "create_proc_entry" },
	{ 0x4e3e4b31, "wake_up_process" },
	{ 0x3e8c107f, "skb_copy_datagram_iovec" },
	{ 0x6cb34e5, "init_waitqueue_head" },
	{ 0x35fe47a1, "init_timer" },
	{ 0xb6c70a7d, "__wake_up" },
	{ 0xf6ebc03b, "net_ratelimit" },
	{ 0xc818ff24, "kmem_cache_zalloc" },
	{ 0xfb7d9c45, "__udivsi3" },
	{ 0xfd8dba4c, "add_wait_queue" },
	{ 0xf11dca7e, "seq_lseek" },
	{ 0x37a0cba, "kfree" },
	{ 0x9d669763, "memcpy" },
	{ 0x98adfde2, "request_module" },
	{ 0x8085c7b1, "prepare_to_wait" },
	{ 0x62737e1d, "sock_unregister" },
	{ 0x9fb3dd30, "memcpy_fromiovec" },
	{ 0x51493d94, "finish_wait" },
	{ 0x11088983, "skb_dequeue" },
	{ 0x759001f5, "dev_add_pack" },
	{ 0x1acf6978, "seq_release" },
	{ 0x89981812, "dev_queue_xmit" },
	{ 0xe914e41e, "strcpy" },
	{ 0x6dd9a340, "skb_free_datagram" },
};

static const char __module_depends[]
__attribute_used__
__attribute__((section(".modinfo"))) =
"depends=crc-ccitt";

