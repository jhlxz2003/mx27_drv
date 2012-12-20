/*
 * Copyright 2008 Morninghan Electronics, Inc. All Rights Reserved.
 */

/*
 * The code contained herein is licensed under the GNU General Public
 * License. You may obtain a copy of the GNU General Public License
 * Version 2 or later at the following locations:
 *
 * http://www.opensource.org/licenses/gpl-license.html
 * http://www.gnu.org/copyleft/gpl.html
 */

#include <asm/io.h>
#include <asm/delay.h>

#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/clk.h>

#include <asm/arch/clock.h>
#include "ssi.h"
#include "registers.h"

extern void gpio_ssi_active(int);
extern void gpio_ssi_inactive(int);
#if defined(CONFIG_SND_MXC_WM9712) || defined(CONFIG_TOUCHSCREEN_MXC_WM9712) || defined(CONFIG_SND_MXC_WM9712_MODULE) || defined(CONFIG_TOUCHSCREEN_MXC_WM9712_MODULE)
extern void gpio_wm9712_ts_active(void);
extern void gpio_wm9712_ts_inactive(void);
#endif

#define SSI_CODEC		SSI1	/* NOTE: use SSI1 directly somewhere */

#define TX_WATERMARK		0x4	/* set tx_fifo watermark */
#define RX_WATERMARK		0x6	/* set rx_fifo watermark */

#define AC97_TAG_READ		0xc000
#define AC97_TAG_WRITE		0xe000
#define AC97_TAG_DISABLE	0x8000
#define AC97_TAG_PCM		0x9800

static int sisr_reg = IO_ADDRESS(SSI1_BASE_ADDR) + MXC_SSISISR;
static spinlock_t ssi_lock = SPIN_LOCK_UNLOCKED;

static inline void ssi_ac97_wait(void)
{
	/* Wait until the start of the last time slot (Command Data slot) */
	while (!(__raw_readl(sisr_reg) & 0x80))	/* TFS bit */
		udelay(1);

	while (__raw_readl(sisr_reg) & 0x80)	/* TFS bit */
		udelay(1);

	/* 
	 * Now in the beginning of the frame. Add some deley such that until
	 * it reach almost middle of the frame.
	 */
	/* udelay(10); */
}

unsigned short ssi_ac97_read(unsigned short reg)
{
	unsigned int addr = 0;
	unsigned short data16 = 0;
	char weight[2];
	unsigned int rtag[2], raddr[2], data[2];
	int i;
	unsigned long lock_flags;

	/* set Command Address */
	addr = reg << 12;
	weight[0] = weight[1] = 0;

	/*
	 * Assumes only the first 3 time slots (Tag, Command Addr,
	 * Command Data) are active
	 */
	ssi_ac97_wait();

	/*
	 * Set slot 0 (TAG) value to indicate Frame Valid, Command Address
	 * bit and Command Data bit.
	 */
	spin_lock_irqsave(&ssi_lock, lock_flags);
	ssi_ac97_set_tag_register(SSI_CODEC, (AC97_TAG_READ | AC97_TAG_PCM));
	ssi_ac97_set_command_address_register(SSI_CODEC, addr);
	ssi_ac97_read_command(SSI_CODEC, 1);	/* set the read tag bits */
	spin_unlock_irqrestore(&ssi_lock, lock_flags);

	ssi_ac97_wait();

	/* Init the 2nd read */
	spin_lock_irqsave(&ssi_lock, lock_flags);
	ssi_ac97_set_tag_register(SSI_CODEC, (AC97_TAG_READ | AC97_TAG_PCM));
	ssi_ac97_set_command_address_register(SSI_CODEC, addr);
	ssi_ac97_read_command(SSI_CODEC, 1);	/* set the read tag bits */
	spin_unlock_irqrestore(&ssi_lock, lock_flags);

	spin_lock_irqsave(&ssi_lock, lock_flags);
	for (i = 0; i < 2; i++) {
		ssi_ac97_wait();
		/* ssi_ac97_set_tag_register(SSI_CODEC,AC97_TAG_PCM); */

		udelay(4);
		rtag[i] = ssi_ac97_get_tag_register(SSI_CODEC);

		udelay(2);
		raddr[i] = ssi_ac97_get_command_address_register(SSI_CODEC);

		udelay(2);
		data[i] = ssi_ac97_get_command_data_register(SSI_CODEC);
	}
	spin_unlock_irqrestore(&ssi_lock, lock_flags);

	for (i = 0; i < 2; i++) {
		if ((rtag[i] & AC97_TAG_WRITE) == AC97_TAG_WRITE)
			weight[i] += 2;

		if (((raddr[i] >> 12) & 0x7f) == reg)
			weight[i] += 1;
	}

	i = (weight[1] > weight[0]) ? 1 : 0;
	data16 = (unsigned short)(data[i] >> 4);

	return data16;
}

void ssi_ac97_write(unsigned short reg, unsigned short val)
{
	unsigned int addr = 0, data = 0;
	unsigned long lock_flags;

	addr = reg << 12;
	data = val << 4;

	ssi_ac97_wait();

	/*
	 * Set slot 0 (TAG) value to indicate Frame Valid, Command Address
	 * bit and Command Data bit.
	 */
	spin_lock_irqsave(&ssi_lock, lock_flags);
	ssi_ac97_set_tag_register(SSI_CODEC, (AC97_TAG_WRITE | AC97_TAG_PCM));

	ssi_ac97_set_command_address_register(SSI_CODEC, addr);
	ssi_ac97_set_command_data_register(SSI_CODEC, data);

	/* Enable Write in next frame */
	ssi_ac97_write_command(SSI_CODEC, 1);

	spin_unlock_irqrestore(&ssi_lock, lock_flags);
}

static void mxc_ssi_ac97_config(void)
{
	int ssi = SSI_CODEC;

	ssi_enable(ssi, 0);	/* disable ssi */

	/* interrupt config is in somewhere else. */

	/* ssi_rx */
	ssi_rx_word_length(ssi, ssi_16_bits);
	ssi_rx_frame_rate(ssi, 13);

	ssi_rx_clock_divide_by_two(ssi, 0);
	ssi_rx_clock_prescaler(ssi, 0);

	ssi_receive_enable(ssi, 1);
	ssi_rx_mask_time_slot(ssi, 0xFFE0);	/* slot 0 1 2 3 4 */

	ssi_rx_fifo_full_watermark(ssi, ssi_fifo_0, RX_WATERMARK);

	/* ssi_tx */
	ssi_tx_word_length(ssi, ssi_16_bits);
	ssi_tx_frame_rate(ssi, 13);

	ssi_tx_clock_divide_by_two(ssi, 0);
	ssi_tx_clock_prescaler(ssi, 0);

	ssi_transmit_enable(ssi, 1);
	ssi_tx_mask_time_slot(ssi, 0xFFE0);

	ssi_tx_fifo_empty_watermark(ssi, ssi_fifo_0, TX_WATERMARK);

	/* ac97 */
	ssi_ac97_frame_rate_divider(ssi, 0);	/* frame rate is 48K */
	ssi_ac97_variable_mode(ssi, 1);	/* var or not */
	ssi_ac97_write_command(ssi, 0);	/* no write */
	ssi_ac97_read_command(ssi, 0);	/* no read */
	ssi_ac97_tag_in_fifo(ssi, 0);	/* tag in satag register */

	ssi_enable(ssi, 1);

	ssi_ac97_mode_enable(ssi, 1);
}

static void mxc_audmux_config(void)
{
	unsigned long reg = 0;
	unsigned long addr = IO_ADDRESS(AUDMUX_BASE_ADDR);	/* HPCR1 address */

	reg = (0 << 31) |	/* Fs from the internal ssi1 */
	    (1 << 30) |		/* clk to internal ssi1 */
	    (4 << 26) |		/* fs/clk <-->external ssi2 */
	    (0 << 25) |		/* ignored for sync mode */
	    (0 << 24) | (0 << 20) | (4 << 13) |	/* data <--> external ssi2 */
	    (1 << 12) |		/* sync */
	    (0 << 10) | (0 << 8) | (0 << 0);
	__raw_writel(reg, addr);	/* set the host port1(internal ssi1) */

	/* set the peripheral port2 */
	reg = (1 << 31) |	/* Fs to the external ssi2 */
	    (0 << 30) |		/* clk from the external ssi2 */
	    (0 << 26) |		/* fs/clk <-->internal ssi1 */
	    (1 << 25) |		/* ignored for sync mode */
	    (1 << 24) | (0 << 20) | (0 << 13) |	/* data <-->internal ssi1 */
	    (1 << 12) |		/* sync */
	    (0 << 10);
	__raw_writel(reg, addr + 0x14);	/* set the port5(external ssi2) */
}

#if defined(CONFIG_SND_MXC_WM9712) || defined(CONFIG_TOUCHSCREEN_MXC_WM9712) || defined(CONFIG_SND_MXC_WM9712_MODULE) || defined(CONFIG_TOUCHSCREEN_MXC_WM9712_MODULE)

#define WM9712_GPIO_CFG			0x4C	/* GPIO Configuration */
#define WM9712_GPIO_POLARITY		0x4E	/* GPIO Pin Polarity/Type, 0=low, 1=high active */
#define WM9712_GPIO_STICKY		0x50	/* GPIO Pin Sticky, 0=not, 1=sticky */
#define WM9712_GPIO_WAKEUP		0x52	/* GPIO Pin Wakeup, 0=no int, 1=yes int */
#define WM9712_GPIO_STATUS		0x54	/* GPIO Pin Status, slot 12 */
#define WM9712_GPIO_SHARE		0x56	/* GPIO Pin Sharing */

/*
 * Config WM9712 GPIO. The same configurations will be used by WM9712
 * touch panel driver. Thus the WM9712 sound driver can receive constant
 * data in slot 12.
 */
static void config_wm9712_gpio(void)
{
	unsigned short data = 0;

	data = ssi_ac97_read(WM9712_GPIO_CFG);
	data &= ~(3 << 2);	/* gpio2,3 as output */
	ssi_ac97_write(WM9712_GPIO_CFG, data);

	data = ssi_ac97_read(WM9712_GPIO_WAKEUP);
	data |= (3 << 2);	/* gpio3 pin wake-up */
	ssi_ac97_write(WM9712_GPIO_WAKEUP, data);

	data = ssi_ac97_read(WM9712_GPIO_SHARE);
	data &= ~(3 << 2);	/* gpio3 output pendown signal */
	ssi_ac97_write(WM9712_GPIO_SHARE, data);
}
#endif

static struct clk *ssi_clk_0, *ssi_clk_1;
static int __init ssi_ac97_init(void)
{
	spin_lock_init(&ssi_lock);

#if defined(CONFIG_SND_MXC_WM9712) || defined(CONFIG_TOUCHSCREEN_MXC_WM9712) || defined(CONFIG_SND_MXC_WM9712_MODULE) || defined(CONFIG_TOUCHSCREEN_MXC_WM9712_MODULE)

	gpio_wm9712_ts_active();
#endif
	gpio_ssi_active(SSI2);	/* external ssi2-port */

	ssi_clk_0 = clk_get(NULL, "ssi_clk.0");
	clk_enable(ssi_clk_0);
	ssi_clk_1 = clk_get(NULL, "ssi_clk.1");
	clk_enable(ssi_clk_1);

	mxc_audmux_config();
	mxc_ssi_ac97_config();

#if defined(CONFIG_SND_MXC_WM9712) || defined(CONFIG_TOUCHSCREEN_MXC_WM9712) || defined(CONFIG_SND_MXC_WM9712_MODULE) || defined(CONFIG_TOUCHSCREEN_MXC_WM9712_MODULE)
	config_wm9712_gpio();
#endif

	return 0;
}

static void __exit ssi_ac97_exit(void)
{
	gpio_ssi_inactive(SSI2);	/* external ssi2-port */
	gpio_wm9712_ts_inactive();
	clk_disable(ssi_clk_0);
	clk_put(ssi_clk_0);
	clk_disable(ssi_clk_1);
	clk_put(ssi_clk_1);
}

EXPORT_SYMBOL(ssi_ac97_read);
EXPORT_SYMBOL(ssi_ac97_write);

module_init(ssi_ac97_init);
module_exit(ssi_ac97_exit);

MODULE_AUTHOR("Morninghan Electronics, Inc.");
MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("MXC SSI AC97 Driver");
