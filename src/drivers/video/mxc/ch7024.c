/*
 * Copyright 2005-2006 Freescale Semiconductor, Inc. All Rights Reserved.
 */

/*
 * The code contained herein is licensed under the GNU General Public
 * License. You may obtain a copy of the GNU General Public License
 * Version 2 or later at the following locations:
 *
 * http://www.opensource.org/licenses/gpl-license.html
 * http://www.gnu.org/copyleft/gpl.html
 */

 /*!
  * @file ch7024.c
  * @brief Driver for CH7024 TV encoder
  *
  * @ingroup Framebuffer
  */
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/delay.h>
#include <linux/spinlock.h>
#include <linux/i2c.h>
#include <asm/arch/mxc_i2c.h>
#include <asm/arch/clock.h>
#include <asm/uaccess.h>
#include <asm/atomic.h>
#include <../arch/arm/mach-mx3/crm_regs.h>
#include <asm/arch/pmic_power.h>
#include <asm/arch/pmic_adc.h>
#include <linux/video_encoder.h>
#include <linux/fb.h>

#include "ch7024.h"
#include "mx2fb.h"

#define PROC_PHONEJACK_CVBS	"cvbs"
#define PROC_PHONEJACK_HEADSET	"headset"
#define PROC_PHONEJACK_NONE	"offline"

static int ch7024_found = 0;
/* proc entries define */
static struct i2c_driver ch7024_driver;
static atomic_t enabled = ATOMIC_INIT(0);	/* enable power on or not */

/*
 * FIXME: ch7024_client will represent the first ch7024 device found by
 * the I2C subsystem, which means ch7024_ioctl() always works on the
 * first ch7024 device.
 */
static struct i2c_client *ch7024_client = 0;

extern struct mx2fb_tvout_if mx2fb_tvout_func;

extern void gpio_jack_active(void);
extern void gpio_lcdc_active(void);
extern void gpio_tvout_active(void);

static int i2c_ch7024_client_xfer(int bus_id, unsigned int addr, char *reg,
				  int reg_len, char *buf, int num,
				  int tran_flag)
{
	struct i2c_msg msg[2];
	int ret;
	if (ch7024_client == 0)
		return -1;
	msg[0].addr = addr;
	msg[0].len = reg_len;
	msg[0].buf = reg;
	msg[0].flags = tran_flag;
	msg[0].flags &= ~I2C_M_RD;

	msg[1].addr = addr;
	msg[1].len = num;
	msg[1].buf = buf;

	msg[1].flags = tran_flag;
	if (tran_flag & MXC_I2C_FLAG_READ) {
		msg[1].flags |= I2C_M_RD;
	} else {
		msg[1].flags &= ~I2C_M_RD;
	}

	ret = i2c_transfer(ch7024_client->adapter, msg, 2);
	return ret;
}

static int mxc_i2c_ch7024_polling_read(int bus_id, unsigned int addr, char *reg,
				       int reg_len, char *buf, int num)
{
	return i2c_ch7024_client_xfer(bus_id, addr, reg, reg_len, buf, num,
				      MXC_I2C_FLAG_READ | MXC_I2C_FLAG_POLLING);
}

static int mxc_i2c_ch7024_polling_write(int bus_id, unsigned int addr,
					char *reg, int reg_len, char *buf,
					int num)
{
	return i2c_ch7024_client_xfer(bus_id, addr, reg, reg_len, buf, num,
				      MXC_I2C_FLAG_POLLING);

}

static int ch7024_read_reg(u32 reg, u32 * word, u32 len)
{
	int i;
	u8 *wp = (u8 *) word;

	*word = 0;

	for (i = 0; i < len; i++) {
		int ret = mxc_i2c_ch7024_polling_read(I2C1_BUS, CH7024_I2C_ADDR,
						      (char *)&reg, 1, wp, 1);
		if (ret < 0)
			return ret;
		wp++;
		reg++;
	}
	return 0;
}

static int ch7024_write_reg(u32 reg, u32 word, u32 len)
{
	return mxc_i2c_ch7024_polling_write(I2C1_BUS, CH7024_I2C_ADDR,
					    (char *)&reg, 1, (u8 *) & word,
					    len);
}

/**
 * PAL B/D/G/H/K/I clock and timting structures
 */
static struct ch7024_clock ch7024_clk_pal = {
	.A = 0x417a17a,
	.P = 0x36b00,
	.N = 0x41eb00,
	.T = 0x3f,
	.PLLN1 = 0x00,
	.PLLN2 = 0x1b,
	.PLLN3 = 0x12,
};

static struct ch7024_input_timing ch7024_timing_pal = {
	.HTI = 1000,
	.VTI = 532,
	.HAI = 640,
	.VAI = 480,
	.HW = 2,
	.HO = 179,
	.VW = 2,
	.VO = 25,
	.VOS = CH7024_VOS_PAL_BDGHKI,
};

/**
 * NTSC_M clock and timting structures
 * TODO: change values to work well.
 */
static struct ch7024_clock ch7024_clk_ntsc = {
	.A = 0x417a17a,
	.P = 0x2ac90,
	.N = 0x36fc90,
	.T = 0x3f,
	.PLLN1 = 0xb,
	.PLLN2 = 0x1b,
	.PLLN3 = 0x12,
};

static struct ch7024_input_timing ch7024_timing_ntsc = {
	.HTI = 801,
	.VTI = 554,
	.HAI = 640,
	.VAI = 480,
	.HW = 2,
	.HO = 80,
	.VW = 2,
	.VO = 36,
	.VOS = CH7024_VOS_NTSC_M,
};

/**
 * ch7024_setup
 * initial the CH7024 chipset by setting register
 * @param:
 * 	vos: output video format
 * @return:
 * 	0 successful
 * 	otherwise failed
 */
static int ch7024_setup(int vos)
{
	struct ch7024_input_timing *ch_timing;
	struct ch7024_clock *ch_clk;
#ifdef DEBUG_CH7024
	int i, val;
#endif

	if (!ch7024_found) {
		printk(KERN_ERR "CH7024: no such device to setup!\n");
		return -ENODEV;
	}

	/* select output video format */
	if (vos == VIDEO_ENCODER_PAL) {
		ch_timing = &ch7024_timing_pal;
		ch_clk = &ch7024_clk_pal;
		pr_debug("CH7024: change to PAL video\n");
	} else if (vos == VIDEO_ENCODER_NTSC) {
		ch_timing = &ch7024_timing_ntsc;
		ch_clk = &ch7024_clk_ntsc;
		pr_debug("CH7024: change to NTSC video\n");
	} else {

		pr_debug("CH7024: no such video format.\n");
		return -EINVAL;
	}

	ch7024_write_reg(CH7024_RESET, 0x00, 1);
	ch7024_write_reg(CH7024_RESET, 0x03, 1);
	ch7024_write_reg(CH7024_POWER, 0x00, 1);
	ch7024_write_reg(CH7024_DATA_IO, 0x02, 1);
	ch7024_write_reg(0x62, 0x34, 1);

	ch7024_write_reg(CH7024_POWER, 0x0C, 1);	/* power on, disable DAC */
	ch7024_write_reg(CH7024_XTAL, CH7024_XTAL_26MHZ, 1);	/* 26MHz cystal */
	ch7024_write_reg(CH7024_SYNC, 0x0D, 1);	/* SLAVE mode, and TTL */
	ch7024_write_reg(CH7024_IDF1, 0x00, 1);
	ch7024_write_reg(CH7024_TVFILTER1, 0x00, 1);	/* set XCH=0 */

	/* set input clock and divider */
	/* set PLL */
	ch7024_write_reg(CH7024_PLL1, ch_clk->PLLN1, 1);
	ch7024_write_reg(CH7024_PLL2, ch_clk->PLLN2, 1);
	ch7024_write_reg(CH7024_PLL3, ch_clk->PLLN3, 1);
	/* set A register */
	ch7024_write_reg(CH7024_PCLK_A1, (ch_clk->A >> 24) & 0xFF, 1);
	ch7024_write_reg(CH7024_PCLK_A2, (ch_clk->A >> 16) & 0xFF, 1);
	ch7024_write_reg(CH7024_PCLK_A3, (ch_clk->A >> 8) & 0xFF, 1);
	ch7024_write_reg(CH7024_PCLK_A4, ch_clk->A & 0xFF, 1);
	/* set P register */
	ch7024_write_reg(CH7024_CLK_P1, (ch_clk->P >> 16) & 0xFF, 1);
	ch7024_write_reg(CH7024_CLK_P2, (ch_clk->P >> 8) & 0xFF, 1);
	ch7024_write_reg(CH7024_CLK_P3, ch_clk->P & 0xFF, 1);
	/* set N register */
	ch7024_write_reg(CH7024_CLK_N1, (ch_clk->N >> 16) & 0xFF, 1);
	ch7024_write_reg(CH7024_CLK_N2, (ch_clk->N >> 8) & 0xFF, 1);
	ch7024_write_reg(CH7024_CLK_N3, ch_clk->N & 0xFF, 1);
	/* set T register */
	ch7024_write_reg(CH7024_CLK_T, ch_clk->T & 0xFF, 1);

	/* set sub-carrier frequency generation method */
	ch7024_write_reg(CH7024_ACIV, 0x00, 1);	/* ACIV = 0, automatical SCF */
	/* TV out pattern and DAC switch */
	ch7024_write_reg(CH7024_OUT_FMT, (0x10 | ch_timing->VOS) & 0xFF, 1);

	/* input settings */
	/* input format, RGB565 */
	ch7024_write_reg(CH7024_IDF2, 0x02, 1);
	/* HAI/HTI VAI */
	ch7024_write_reg(CH7024_IN_TIMING1, ((ch_timing->HTI >> 5) & 0x38) |
			 ((ch_timing->HAI >> 8) & 0x07), 1);
	ch7024_write_reg(CH7024_IN_TIMING2, ch_timing->HAI & 0xFF, 1);
	ch7024_write_reg(CH7024_IN_TIMING8, ch_timing->VAI & 0xFF, 1);
	/* HTI VTI */
	ch7024_write_reg(CH7024_IN_TIMING3, ch_timing->HTI & 0xFF, 1);
	ch7024_write_reg(CH7024_IN_TIMING9, ch_timing->VTI & 0xFF, 1);
	/* HW/HO(h) VW */
	ch7024_write_reg(CH7024_IN_TIMING4, ((ch_timing->HW >> 5) & 0x18) |
			 ((ch_timing->HO >> 8) & 0x7), 1);
	ch7024_write_reg(CH7024_IN_TIMING6, ch_timing->HW & 0xFF, 1);
	ch7024_write_reg(CH7024_IN_TIMING11, ch_timing->VW & 0x3F, 1);
	/* HO(l) VO/VAI/VTI */
	ch7024_write_reg(CH7024_IN_TIMING5, ch_timing->HO & 0xFF, 1);
	ch7024_write_reg(CH7024_IN_TIMING7, ((ch_timing->VO >> 4) & 0x30) |
			 ((ch_timing->VTI >> 6) & 0x0C) |
			 ((ch_timing->VAI >> 8) & 0x03), 1);
	ch7024_write_reg(CH7024_IN_TIMING10, ch_timing->VO & 0xFF, 1);

	/* adjust the brightness */
	ch7024_write_reg(CH7024_TVBRI, 0x90, 1);

	ch7024_write_reg(CH7024_OUT_TIMING1, 0x4, 1);
	ch7024_write_reg(CH7024_OUT_TIMING2, 0xe0, 1);

	if (vos == VIDEO_ENCODER_PAL) {
		ch7024_write_reg(CH7024_V_POS1, 0x03, 1);
		ch7024_write_reg(CH7024_V_POS2, 0x7d, 1);
	} else {
		ch7024_write_reg(CH7024_V_POS1, 0x02, 1);
		ch7024_write_reg(CH7024_V_POS2, 0x7b, 1);
	}

#ifdef DEBUG_CH7024
	for (i = 0; i < CH7024_SC_FREQ4; i++) {

		ch7024_read_reg(i, &val, 1);
		printk(KERN_INFO "CH7024, reg[0x%x] = %x\n", i, val);
	}
#endif
	return 0;
}

static void ch7024_poweron(void)
{
	/* open the VGEN of pmic to supply VDDI */
	/*t_regulator_voltage voltage;
	   voltage.vgen = VGEN_1_8V;
	   pmic_power_regulator_set_voltage(REGU_VGEN, voltage);
	   pmic_power_regulator_on(REGU_VGEN); */
	return;
}

static void ch7024_poweroff(void)
{
	/* close the VGEN of pmic to supply VDDI */
	/*pmic_power_regulator_off(REGU_VGEN); */
	return;
}

/**
 * ch7024_enable
 * Enable the ch7024 Power to begin TV encoder
 */
static void ch7024_enable(void)
{
	if (atomic_inc_return(&enabled) == 1) {
		ch7024_write_reg(CH7024_POWER, 0x00, 1);
		printk(KERN_INFO "CH7024 power on.\n");
	}
}

/**
 * ch7024_disable
 * Disable the ch7024 Power to stop TV encoder
 */
static void ch7024_disable(void)
{
	if (atomic_dec_and_test(&enabled)) {
		ch7024_write_reg(CH7024_POWER, 0x0D, 1);
		pr_debug("CH7024 power off.\n");
	}
}

/**
 * ch7024_get_capability
 * Get the TVOUT module number and support mode
 */
static void ch7024_get_capability(void *cap)
{
	struct video_encoder_capability *capt;
	capt = (struct video_encoder_capability *)cap;
	capt->flags = VIDEO_ENCODER_PAL | VIDEO_ENCODER_NTSC;
	capt->inputs = 1;
	capt->outputs = 1;
}

 /**
  * ch7024_set_input
  * Set the TVOUT input
 */
static void ch7024_set_input(void *input)
{
	/*int val;
	   val = *(int *)input;
	   if (val != 0)
	   return -EINVAL; */
}

  /**
   * ch7024_set_output
   * Set the TVOUT output
 */
static void ch7024_set_output(void *output)
{
	/*int val;
	   val = *(int *)output;
	   if (val != 0)
	   return -EINVAL; */
}

  /**
   * ch7024_enable_output
   * Set the TVOUT output
 */
static void ch7024_enable_output(void *output)
{
	ch7024_poweron();
	ch7024_enable();
}

static int i2c_ch7024_detect_client(struct i2c_adapter *adapter, int address,
				    int kind)
{
	int ret;
	struct i2c_client *client;
	u32 id;
	const char *client_name = "ch7024 I2C dev";

	pr_debug("ch7024: i2c-bus: %s; address: 0x%x\n", adapter->name,
		 address);

	client = kmalloc(sizeof(struct i2c_client) + sizeof(int), GFP_KERNEL);
	if (!client)
		return -ENOMEM;
	memset(client, 0, sizeof(struct i2c_client) + sizeof(int));
	client->addr = address;
	client->adapter = adapter;
	client->driver = &ch7024_driver;
	client->flags = 0;

	strcpy(client->name, client_name);
	i2c_set_clientdata(client, (client + 1));

	if (i2c_attach_client(client)) {
		printk(KERN_ERR "ch7024: i2c_attach_client() failed.\n");
		kfree(client);
	} else if (ch7024_client == 0)
		ch7024_client = client;

	/*TODO client detection */
	ret = ch7024_read_reg(CH7024_DEVID, &id, 1);
	if (ret < 0 || id != CH7024_DEVICE_ID) {
		printk(KERN_ERR
		       "ch7024: TV encoder not present: %d, read ret %d\n", id,
		       ret);
		return 0;
	}
	printk(KERN_INFO "ch7024: TV encoder present: %x, read ret %x\n", id,
	       ret);
	ch7024_found = 1;

	return 0;
}

static unsigned short normal_i2c[] = { CH7024_I2C_ADDR, I2C_CLIENT_END };

/* Magic definition of all other variables and things */
I2C_CLIENT_INSMOD;

static int i2c_ch7024_attach(struct i2c_adapter *adap)
{
	return i2c_probe(adap, &addr_data, &i2c_ch7024_detect_client);
}

static int i2c_ch7024_detach(struct i2c_client *client)
{
	int err;

	if ((err = i2c_detach_client(client))) {
		pr_debug("ch7024: i2c_detach_client() failed\n");
		return err;
	}

	if (ch7024_client == client)
		ch7024_client = 0;

	kfree(client);
	return 0;
}

static struct i2c_driver ch7024_driver = {
	.driver = {
		   .owner = THIS_MODULE,
		   .name = "ch7024 driver",
		   },
	.attach_adapter = &i2c_ch7024_attach,
	.detach_client = &i2c_ch7024_detach,
};

static int __init ch7024_init(void)
{
	int err;
#if defined(CONFIG_MACH_MX27_SYP) || defined(CONFIG_MACH_MX27_TUOSI) || defined(CONFIG_MACH_MX27_MPK271)
#else
	PMIC_STATUS ret = 0;
	t_regulator_voltage voltage;
#endif
	unsigned long mode;
	char *option = NULL;

	pr_debug
	    ("Freescale CH7024 TV encoder driver, (c) 2005 Freescale Semiconductor, Inc.\n");

	if (fb_get_options("mxcfb", &option))
		mode = VIDEO_ENCODER_PAL;
	else if (!option || !*option)
		mode = VIDEO_ENCODER_PAL;
	else {
		if (!strncmp(option, "TVOUT-PAL", 9))
			mode = VIDEO_ENCODER_PAL;
		else if (!strncmp(option, "TVOUT-NTSC", 10))
			mode = VIDEO_ENCODER_NTSC;
		else
			mode = VIDEO_ENCODER_PAL;
	}

#if defined(CONFIG_MACH_MX27_SYP) || defined(CONFIG_MACH_MX27_TUOSI) || defined(CONFIG_MACH_MX27_MPK271)
#else
	voltage.vgen = VGEN_1_8V;
	if ((pmic_power_regulator_set_voltage(REGU_VGEN, voltage)) < 0) {
		printk(KERN_ERR "%s:vdd_lcdio voltage error:%d\n", __func__,
		       ret);
		return -1;
	} else {
		printk(KERN_INFO "%s:vdd_lcdio set voltage ok\n", __func__);
	}
	if ((pmic_power_regulator_on(REGU_VGEN)) < 0) {
		printk(KERN_ERR "%s:vdd_lcdio power on error:%d\n", __func__,
		       ret);
		return -1;
	} else {
		printk(KERN_INFO "%s:vdd_lcdio power on ok\n", __func__);
	}
	/*1V8_SW2A for I2C */
	voltage.sw2a = SW2A_1_8V;
	if ((ret = pmic_power_regulator_set_voltage(SW_SW2A, voltage)) < 0) {
		printk(KERN_ERR "%s:SW2A  set voltage error:%d\n", __func__,
		       ret);
		return -1;
	} else {
		printk(KERN_INFO "%s: SW2A power on\n", __func__);
	}

	if ((ret = pmic_power_regulator_set_voltage(SW_SW2B, voltage)) < 0) {
		printk(KERN_ERR "%s:SW2B set voltage error:%d\n", __func__,
		       ret);
		return -1;
	} else {
		printk(KERN_INFO "%s:SW2B set voltage ok\n", __func__);
	}
	if (pmic_power_set_regen_assig(REGU_GPO3, 1) < 0) {
		printk(KERN_ERR "%s:set_regen_assig error\n", __func__);
		return -1;
	} else {
		printk(KERN_INFO "%s:set_regen_assig ok\n", __func__);
	}
	if ((ret = pmic_power_regulator_on(REGU_GPO3)) < 0) {
		printk(KERN_ERR "%s:REGU_GPO3 power on error:%d\n", __func__,
		       ret);
		return -1;
	} else {
		printk(KERN_INFO "%s:REGU_GPO3 power on ok\n", __func__);
	}

	if (pmic_power_set_regen_assig(REGU_GPO1, 1) < 0) {
		printk(KERN_ERR "%s:set_regen_assig error\n", __func__);
		return -1;
	} else {
		printk(KERN_INFO "%s:set_regen_assig ok\n", __func__);
	}
	if ((ret = pmic_power_regulator_on(REGU_GPO1)) < 0) {
		printk(KERN_ERR "%s:REGU_GPO3 power on error:%d\n", __func__,
		       ret);
		return -1;
	} else {
		printk(KERN_INFO "%s:REGU_GPO1 power on ok\n", __func__);
	}
#endif

	gpio_lcdc_active();

	if ((err = i2c_add_driver(&ch7024_driver))) {
		pr_info("ch7024: driver registration failed\n");
		/* return err; */
	}

	gpio_jack_active();
	ch7024_setup(mode);
	ch7024_enable();

	mx2fb_tvout_func.setup = ch7024_setup;
	mx2fb_tvout_func.enable = ch7024_enable;
	mx2fb_tvout_func.disable = ch7024_disable;
	mx2fb_tvout_func.poweron = ch7024_poweron;
	mx2fb_tvout_func.poweroff = ch7024_poweroff;
	mx2fb_tvout_func.get_capability = ch7024_get_capability;
	mx2fb_tvout_func.set_input = ch7024_set_input;
	mx2fb_tvout_func.set_output = ch7024_set_output;
	mx2fb_tvout_func.enable_output = ch7024_enable_output;

	printk(KERN_INFO "ch7024_init\n");

	return 0;
}

static void __exit ch7024_exit(void)
{
	ch7024_disable();
	i2c_del_driver(&ch7024_driver);
	return;
}

module_init(ch7024_init);
module_exit(ch7024_exit);

MODULE_AUTHOR("Freescale Semiconductor, Inc.");
MODULE_DESCRIPTION("CH7024 TV encoder driver");
MODULE_LICENSE("GPL");
