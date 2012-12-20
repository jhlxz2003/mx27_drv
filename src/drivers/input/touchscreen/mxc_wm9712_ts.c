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

#include <linux/init.h>
#include <linux/fs.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/sched.h>
#include <linux/slab.h>
#include <linux/delay.h>
#include <linux/interrupt.h>
#include <linux/errno.h>
#include <linux/irq.h>

#include <linux/device.h>
#include <linux/spinlock.h>
#include <linux/input.h>
#include <linux/timer.h>

#include <asm/io.h>
#include <asm/arch/clock.h>
#include <asm/arch/irqs.h>

/*
 * WM9712 Register definition
 */
#define WM9712_RESET			0x00	/* Reset */
#define WM9712_POWERDOWN		0x24	/*Powerdown */
#define WM9712_POWERDOWN_CTL		0x26	/* Powerdown control / status */
#define WM9712_EXTENDED_ID		0x28	/* Extended Audio ID */

#define WM9712_GPIO_CFG			0x4C	/* GPIO Configuration */
#define WM9712_GPIO_POLARITY		0x4E	/* GPIO Pin Polarity/Type, 0=low, 1=high active */
#define WM9712_GPIO_STICKY		0x50	/* GPIO Pin Sticky, 0=not, 1=sticky */
#define WM9712_GPIO_WAKEUP		0x52	/* GPIO Pin Wakeup, 0=no int, 1=yes int */
#define WM9712_GPIO_STATUS		0x54	/* GPIO Pin Status, slot 12 */
#define WM9712_GPIO_SHARE		0x56	/* GPIO Pin Sharing */

#define WM9712_DIGITISER_1		0x76	/* Digitiser Register 1 */
#define WM9712_DIGITISER_2		0x78	/* Digitiser Register 2 */
#define WM9712_DIGITISER_READ_BAK	0x7A	/* Digitiser Read Back */

#define WM9712_VENDOR_ID1		0x7C	/* Vendor ID1 */
#define WM9712_VENDOR_ID2		0x7E	/* Vendor ID2 / revision */

/* register 76h */
#define CONTINUOUS_MODE_RATE_SHFIT	8
#define ADC_INPUT_SEL_SHFIT		12
#define COORDINATE_MODE_ENABLE		(1<<11)
#define DATA_TRANFER_ROUTINE		(1<<10)
#define TSC_TIME_SET_SHFIT		4
#define SLOT_READBACK_ENABLE		(1<<3)

/* register 78h */
#define PEN_ADC_ENBLE_MASK_SHFIT	14
#define WAKE_UP_ON_PEN_DOWN_MODE	(1<<13)
#define TOUCHPANEL_TYPE_SEL		(1<<12)
#define MEASURE_PEN_DOWN		(1<<11)

/* ID numbers */
#define WM9712_ID1		0x574D
#define WM9712_ID2		0x4C12

#define DEVICE_NAME		"mxc_wm9712_ts"
#define DEVICE_PHYS		""

#define X_AXIS_MAX		4095
#define X_AXIS_MIN		0
#define Y_AXIS_MAX		4095
#define Y_AXIS_MIN		0

#define PRESS_MAX		3000
#define PRESS_MIN		10

extern unsigned short ssi_ac97_read(unsigned short);
extern void ssi_ac97_write(unsigned short, unsigned short);

struct ts_data {
	unsigned short x;
	unsigned short y;
	unsigned short p;
};

typedef struct {
	struct input_dev *input_d;
	struct ts_data buf;
	int pen_down;
	int irq_gpio;
} adc_t;

#ifdef CONFIG_MACH_MX27_SYP
#define TS_IRQ		IOMUX_TO_IRQ(MX27_PIN_CSPI1_MISO)
#elif defined(CONFIG_MACH_MX27_TUOSI)
#define TS_IRQ		IOMUX_TO_IRQ(MX27_PIN_CSPI2_SS0)
#elif defined(CONFIG_MACH_MX27_MPK271)
#define TS_IRQ		IOMUX_TO_IRQ(MX27_PIN_SSI4_RXDAT)
#else
#error "Touch Panel GPIO interrupt number is not defined."
#endif

#define SAMPLE_TIME0	0		/* no delay. measure immediately */
#define SAMPLE_TIME1	(HZ / 25)	/* 40 ms */

static char time_flag;
static adc_t g_adc;
struct timer_list timer_adc;

static void config_touchscreen_digitiser(void)
{
	unsigned short data = 0;

	ssi_ac97_write(WM9712_DIGITISER_1, 0x40);

	ssi_ac97_write(WM9712_DIGITISER_2, 0xc001);

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

static int wm9712_init(void)
{
	unsigned short id1, id2;

	id1 = ssi_ac97_read(WM9712_VENDOR_ID1);
	id2 = ssi_ac97_read(WM9712_VENDOR_ID2);

	if (id1 == WM9712_ID1 && id2 == WM9712_ID2) {
		printk(KERN_INFO "WM9712 found. ID 0x%04X 0x%04X\n", id1, id2);
		return 0;
	} else {
		printk(KERN_ERR "No WM9712 found.\n");
		return -1;
	}
}

static int wm9712_ts_input_open(struct input_dev *idev)
{
	enable_irq(TS_IRQ);
	config_touchscreen_digitiser();

	return 0;
}

static void wm9712_ts_input_close(struct input_dev *idev)
{
	disable_irq(TS_IRQ);
}

static inline void wm9712_ts_input_evt_add(adc_t * ts)
{
	unsigned short x, y, p;

	x = ts->buf.x;
	y = ts->buf.y;
	p = ts->buf.p;

	if (ts->pen_down && p <= PRESS_MAX && p >= PRESS_MIN) {
		pr_debug("pen down: x = %d, y = %d, p = %d\n", x, y, p);
		input_report_key(ts->input_d, BTN_TOUCH, 1);
		input_report_abs(ts->input_d, ABS_X, x);
		input_report_abs(ts->input_d, ABS_Y, y);
		input_report_abs(ts->input_d, ABS_PRESSURE, p);
		input_sync(ts->input_d);
	}
}

static irqreturn_t pendown_interrupt(int irq, void *dev_id)
{
	disable_irq(TS_IRQ);

	if (ssi_ac97_read(WM9712_DIGITISER_READ_BAK) & 0x8000) {
		g_adc.pen_down = 1;
		timer_adc.expires = jiffies + SAMPLE_TIME0;
		add_timer(&timer_adc);
	} else {
		g_adc.pen_down = 0;
		enable_irq(TS_IRQ);
	}

	return IRQ_HANDLED;
}

/*
 * Read a sample from the adc in polling mode.
 */
static int wm9712_poll_read_adc(unsigned short adcsel, unsigned short *sample)
{
	unsigned short value;
	int timeout = 40;

	value = ssi_ac97_read(WM9712_DIGITISER_1);
	value &= 0x0FFF;
	value |= (0x8000 | adcsel);
	ssi_ac97_write(WM9712_DIGITISER_1, value);

	/* wait for POLL to go low */
	while ((ssi_ac97_read(WM9712_DIGITISER_1) & 0x8000) && timeout) {
		udelay(20);
		timeout--;
	}

	if (timeout > 0) {
		*sample = ssi_ac97_read(WM9712_DIGITISER_READ_BAK);
		pr_debug("sample = 0x%08X   value = %d\n", *sample,
			 *sample & 0x0FFF);
	} else {
		printk(KERN_ERR "ADC read data timeout.\n");
		return -1;
	}

	if ((*sample & 0x8000) == 0) {
		pr_debug("Pen is already up.\n");
		return -1;
	} else if ((*sample & 0x7000) == adcsel) {
		*sample &= 0x0FFF;
		return 0;
	} else {
		printk(KERN_ERR
		       "ADC measurement type changed: 0x%04X -> 0x%04X", adcsel,
		       *sample & 0x7000);
		*sample &= 0x0FFF;
		return -1;
	}
}

 /*
  * Sample the touchscreen in polling mode
  */
static void wm9712_poll_touch(adc_t * ts)
{
	/* read x */
	if (wm9712_poll_read_adc(0x1000, &(ts->buf.x)))
		return;

	/* read y */
	if (wm9712_poll_read_adc(0x2000, &(ts->buf.y)))
		return;

	/* read pressure */
	if (wm9712_poll_read_adc(0x3000, &(ts->buf.p)))
		return;

	wm9712_ts_input_evt_add(ts);
}

static void adc_timer(unsigned long data)
{
	time_flag = 1;

	if (g_adc.pen_down == 1)
		wm9712_poll_touch(&g_adc);

	if (ssi_ac97_read(WM9712_DIGITISER_READ_BAK) & 0x8000) {
		g_adc.pen_down = 1;
		mod_timer(&timer_adc, jiffies + SAMPLE_TIME1);
	} else {
		g_adc.pen_down = 0;
		pr_debug("pen up\n");

		input_report_key(g_adc.input_d, BTN_TOUCH, 0);
		input_report_abs(g_adc.input_d, ABS_X, 0);
		input_report_abs(g_adc.input_d, ABS_Y, 0);
		input_report_abs(g_adc.input_d, ABS_PRESSURE, 0);
		input_sync(g_adc.input_d);

		enable_irq(TS_IRQ);
	}
}

static int __init adc_init(void)
{
	adc_t *adc = &g_adc;
	struct input_dev *input_dev;
	int result;

	g_adc.irq_gpio = TS_IRQ;

	input_dev = input_allocate_device();
	if (!input_dev) {
		printk(KERN_ERR "input_allocate_device() failed\n");
		result = -ENOMEM;
		goto fail;
	}
	adc->input_d = input_dev;
	adc->input_d->name = DEVICE_NAME;
	adc->input_d->phys = DEVICE_PHYS;
	adc->input_d->id.bustype = BUS_HOST;
	adc->input_d->id.vendor = 0;
	adc->input_d->id.product = 0;
	adc->input_d->id.version = 0;
	adc->input_d->private = adc;

	adc->input_d->open = wm9712_ts_input_open;
	adc->input_d->close = wm9712_ts_input_close;

	adc->input_d->evbit[0] = BIT(EV_KEY) | BIT(EV_ABS);
	adc->input_d->keybit[LONG(BTN_TOUCH)] = BIT(BTN_TOUCH);
	adc->input_d->absbit[0] = BIT(ABS_X) | BIT(ABS_Y) | BIT(ABS_PRESSURE);

	input_set_abs_params(adc->input_d, ABS_X, X_AXIS_MIN, X_AXIS_MAX, 4, 8);
	input_set_abs_params(adc->input_d, ABS_Y, Y_AXIS_MIN, Y_AXIS_MAX, 4, 8);
	input_set_abs_params(adc->input_d, ABS_PRESSURE, PRESS_MIN, PRESS_MAX,
			     4, 8);

	timer_adc.function = &adc_timer;
	timer_adc.data = (unsigned long)&g_adc;
	init_timer(&timer_adc);
	time_flag = 0;

	result = wm9712_init();
	if (result < 0)
		goto fail;

	result =
	    request_irq(g_adc.irq_gpio, pendown_interrupt, 0, DEVICE_NAME,
			(void *)&g_adc);
	if (result)
		goto fail;

	result = input_register_device(adc->input_d);
	if (result < 0)
		goto fail1;

	disable_irq(TS_IRQ);

	printk(KERN_INFO "MXC WM9712 Touchscreen driver registered.\n");
	return 0;

      fail1:
	free_irq(g_adc.irq_gpio, (void *)&g_adc);

      fail:
	input_free_device(input_dev);

	return result;
}

void __exit adc_exit(void)
{
	free_irq(g_adc.irq_gpio, (void *)&g_adc);
	if (time_flag > 0) {
		if (del_timer_sync(&timer_adc) > 0)
			printk(KERN_INFO "Timer is active.\n");
	}

	input_unregister_device(g_adc.input_d);
}

module_init(adc_init);
module_exit(adc_exit);

MODULE_AUTHOR("Morninghan Electronics, Inc.");
MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("MXC WM9712 Touchscreen Driver");
