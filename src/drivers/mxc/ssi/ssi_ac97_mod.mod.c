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
	{ 0xcb03d28c, "ssi_tx_word_length" },
	{ 0xac244f0b, "ssi_ac97_read_command" },
	{ 0xe792d65d, "clk_enable" },
	{ 0x2de95c94, "ssi_ac97_get_command_address_register" },
	{ 0xbb26e06a, "ssi_ac97_write_command" },
	{ 0xf6712a05, "ssi_rx_clock_divide_by_two" },
	{ 0x515090fa, "ssi_rx_word_length" },
	{ 0x40022c16, "clk_disable" },
	{ 0x1342fbbc, "gpio_ssi_inactive" },
	{ 0x8bd275a8, "clk_put" },
	{ 0xab58e876, "ssi_ac97_get_tag_register" },
	{ 0xeae3dfd6, "__const_udelay" },
	{ 0x4a125627, "gpio_wm9712_ts_active" },
	{ 0x6a18fb86, "ssi_ac97_mode_enable" },
	{ 0x706041db, "ssi_ac97_get_command_data_register" },
	{ 0x6ccdfecf, "ssi_enable" },
	{ 0xdebb466f, "ssi_ac97_set_tag_register" },
	{ 0xa981d39c, "ssi_receive_enable" },
	{ 0xf101ae5e, "ssi_rx_mask_time_slot" },
	{ 0x66ddf6b5, "ssi_ac97_set_command_data_register" },
	{ 0x9441c95e, "ssi_rx_clock_prescaler" },
	{ 0x698bb058, "ssi_ac97_variable_mode" },
	{ 0x1f0c5c19, "ssi_ac97_frame_rate_divider" },
	{ 0x2bd6e303, "ssi_tx_mask_time_slot" },
	{ 0x43b0c9c3, "preempt_schedule" },
	{ 0x214a10cc, "ssi_ac97_set_command_address_register" },
	{ 0x4f469b8b, "ssi_tx_frame_rate" },
	{ 0x7aa31ece, "ssi_rx_fifo_full_watermark" },
	{ 0x34896012, "clk_get" },
	{ 0x2a0c9b6a, "ssi_ac97_tag_in_fifo" },
	{ 0x7d14c530, "ssi_tx_fifo_empty_watermark" },
	{ 0x17b9a22f, "ssi_tx_clock_prescaler" },
	{ 0x2c20ae2c, "ssi_rx_frame_rate" },
	{ 0xbe63798, "ssi_transmit_enable" },
	{ 0xc9a47012, "ssi_tx_clock_divide_by_two" },
	{ 0xa00d0821, "gpio_wm9712_ts_inactive" },
	{ 0x8c4d1fb0, "gpio_ssi_active" },
};

static const char __module_depends[]
__attribute_used__
__attribute__((section(".modinfo"))) =
"depends=";

