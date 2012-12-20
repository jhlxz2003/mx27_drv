/*
 *  Copyright (C) 2000 Deep Blue Solutions Ltd
 *  Copyright (C) 2002 Shane Nay (shane@minirl.com)
 *  Copyright 2006-2007 Freescale Semiconductor, Inc. All Rights Reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include <linux/types.h>
#include <linux/sched.h>
#include <linux/interrupt.h>
#include <linux/init.h>
#include <linux/ioport.h>
#include <linux/device.h>
#include <linux/input.h>
#include <linux/nodemask.h>
#include <linux/clk.h>
#include <linux/spi/spi.h>
#include <linux/serial_8250.h>
#if defined(CONFIG_MTD) || defined(CONFIG_MTD_MODULE)
#include <linux/mtd/mtd.h>
#include <linux/mtd/map.h>
#include <linux/mtd/partitions.h>
#include <linux/dm9000.h>

#include <asm/mach/flash.h>
#endif

#include <asm/hardware.h>
#include <asm/irq.h>
#include <asm/setup.h>
#include <asm/mach-types.h>
#include <asm/mach/arch.h>
#include <asm/mach/irq.h>
#include <asm/arch/memory.h>
#include <asm/arch/hardware.h>
#include <asm/arch/gpio.h>
#include <asm/mach/keypad.h>
#include "gpio_mux.h"

/*!
 * @file mach-mx27/mx27_mdk27v0.c
 * @brief This file contains the board specific initialization routines.
 *
 * @ingroup MSL_MX27
 */

extern void mxc_map_io(void);
extern void mxc_init_irq(void);
extern void mxc_clocks_init(void);
extern struct sys_timer mxc_timer;

static u16 mxc_pbc_ver;

static void mxc_nop_release(struct device *dev)
{
	/* Nothing */
}

#if defined(CONFIG_KEYBOARD_MXC) || defined(CONFIG_KEYBOARD_MXC_MODULE)

/*!
 * This array is used for mapping mx27 ADS keypad scancodes to input keyboard
 * keycodes.
 */
static u16 mxckpd_keycodes[(MAXROW * MAXCOL)] = {
	KEY_UP, KEY_DOWN, KEY_RESERVED, KEY_RESERVED, KEY_RESERVED,
	KEY_RESERVED,
	KEY_RIGHT, KEY_LEFT, KEY_ENTER, KEY_RESERVED, KEY_RESERVED,
	KEY_RESERVED,
	KEY_F6, KEY_F8, KEY_F9, KEY_F10, KEY_RESERVED, KEY_RESERVED,
	KEY_RESERVED, KEY_RESERVED, KEY_RESERVED, KEY_RESERVED, KEY_RESERVED,
	KEY_RESERVED,
	KEY_RESERVED, KEY_RESERVED, KEY_RESERVED, KEY_RESERVED, KEY_RESERVED,
	KEY_RESERVED,
	KEY_RESERVED, KEY_RESERVED, KEY_RESERVED, KEY_RESERVED, KEY_RESERVED,
	KEY_RESERVED
};

static struct keypad_data mod_6_by_6_keypad = {
	.rowmax = MAXROW,
	.colmax = MAXCOL,
	.irq = INT_KPP,
	.learning = 0,
	.delay = 2,
	.matrix = mxckpd_keycodes,
};

static struct resource mxc_kpp_resources[] = {
	[0] = {
	       .start = INT_KPP,
	       .end = INT_KPP,
	       .flags = IORESOURCE_IRQ,
	       }
};

/* mxc keypad driver */
static struct platform_device mxc_keypad_device = {
	.name = "mxc_keypad",
	.id = 0,
	.num_resources = ARRAY_SIZE(mxc_kpp_resources),
	.resource = mxc_kpp_resources,
	.dev = {
		.release = mxc_nop_release,
		.platform_data = &mod_6_by_6_keypad,
		},
};

static void mxc_init_keypad(void)
{
	(void)platform_device_register(&mxc_keypad_device);
}
#else
static inline void mxc_init_keypad(void)
{
}
#endif

/* MTD NAND flash */

#if defined(CONFIG_MTD_NAND_MXC) || defined(CONFIG_MTD_NAND_MXC_MODULE)

static struct mtd_partition mxc_nand_partitions[5] = {
	{
	 .name = "IPL-SPL",
	 .offset = 0,
	 .size = 128 * 1024},
	{
	 .name = "nand.kernel",
	 .offset = MTDPART_OFS_APPEND,
	 .size = 4 * 1024 * 1024},
	{
	 .name = "nand.rootfs",
	 .offset = MTDPART_OFS_APPEND,
	 .size = 160 * 1024 * 1024},
	{
	 .name = "Flb",
	 .offset = MTDPART_OFS_APPEND,
	 .size = 8 * 1024 * 1024},
	{
	 .name = "Flc",
	 .offset = MTDPART_OFS_APPEND,
	 .size = MTDPART_SIZ_FULL},
};

static struct flash_platform_data mxc_nand_data = {
	.parts = mxc_nand_partitions,
	.nr_parts = ARRAY_SIZE(mxc_nand_partitions),
	.width = 1,
};

static struct platform_device mxc_nand_mtd_device = {
	.name = "mxc_nand_flash",
	.id = 0,
	.dev = {
		.release = mxc_nop_release,
		.platform_data = &mxc_nand_data,
		},
};

static void mxc_init_nand_mtd(void)
{
	(void)platform_device_register(&mxc_nand_mtd_device);
}
#else
static inline void mxc_init_nand_mtd(void)
{
}
#endif

#if  defined(CONFIG_SMSC911X) || defined(CONFIG_SMSC911X_MODULE)
static struct resource smsc911x_resources[] = {
	{
	 .start = CS5_BASE_ADDR,
	 .end = CS5_BASE_ADDR + 0x100,
	 .flags = IORESOURCE_MEM,
	 },
	{
	 .start = EXPIO_CS5_INT_ENET_INT,
	 .end = EXPIO_CS5_INT_ENET_INT,
	 .flags = IORESOURCE_IRQ,
	 }
};

static struct platform_device mxc_smsc911x_device = {
	.name = "smsc911x",
	.id = 0,
	.dev = {
		.release = mxc_nop_release,
		},
	.num_resources = ARRAY_SIZE(smsc911x_resources),
	.resource = smsc911x_resources,
};

static void mxc_init_enet(void)
{
	u16 major, minor;
	major = MXC_PBC_VERSION_MAJOR(mxc_pbc_ver);
	minor = MXC_PBC_VERSION_MINOR(mxc_pbc_ver);
	if ((major == MXC_PBC_VER_1_0) && (minor < MXC_PBC_VER_2_0)) {
		smsc911x_resources[1].start = EXPIO_CS5_PARENT_INT;
		smsc911x_resources[1].end = EXPIO_CS5_PARENT_INT;
	}

	platform_device_register(&mxc_smsc911x_device);
}
#else
static inline void mxc_init_enet(void)
{
}
#endif
#if defined(CONFIG_FB_MXC_SYNC_PANEL) || defined(CONFIG_FB_MXC_SYNC_PANEL_MODULE)
#if defined(CONFIG_MACH_MX27_SYP) || defined(CONFIG_MACH_MX27_MPK271)
static const char fb_default_mode[] = "SAMSUNG-WVGA";
#elif defined(CONFIG_MACH_MX27_TUOSI)
static const char fb_default_mode[] = "TVOUT-PAL";
#elif defined(CONFIG_MACH_MX27_MDK27V1)
static const char fb_default_mode[] = "SAMSUNG-QVGA";
#else
static const char fb_default_mode[] = "Epson-VGA";
#endif

/* mxc lcd driver */
static struct platform_device mxc_fb_device = {
	.name = "mxc_sdc_fb",
	.id = 0,
	.dev = {
		.release = mxc_nop_release,
		.platform_data = &fb_default_mode,
		.coherent_dma_mask = 0xFFFFFFFF,
		},
};

static void mxc_init_fb(void)
{
	(void)platform_device_register(&mxc_fb_device);
}
#else
static inline void mxc_init_fb(void)
{
}
#endif

#if defined(CONFIG_DM9000) || defined(CONFIG_DM9000_MODULE)
static void mxc_inblk(void __iomem *reg, void *data, int len)
{
	int i;
	for (i = 0; i < ((len + 1) >> 1); i++)
		((u16 *)data)[i] = __raw_readw(reg);
}

static void mxc_outblk(void __iomem *reg, void *data, int len)
{
	int i;
	for (i = 0; i < ((len + 1) >> 1); i++)
		__raw_writew(((u16 *)data)[i], reg);
}

static struct dm9000_plat_data mxc_dm9000_data = {
	.flags = DM9000_PLATF_16BITONLY,
	.inblk = mxc_inblk,
	.outblk = mxc_outblk,
};

static struct resource dm9000_resources[] = {
	[0] = {
		.start	= CS5_BASE_ADDR,
		.end	= CS5_BASE_ADDR + 0x1,
		.flags	= IORESOURCE_MEM,
		},
	[1] = {
		.start	= CS5_BASE_ADDR + 0x2,
		.end	= CS5_BASE_ADDR + 0x3,
		.flags	= IORESOURCE_MEM,
		},
	[2] = {
		.start	= IOMUX_TO_IRQ(MX27_PIN_SSI3_FS),
		.end	= IOMUX_TO_IRQ(MX27_PIN_SSI3_FS),
		.flags	= IORESOURCE_IRQ,
		}
};

static struct platform_device mxc_dm9000_device = {
	.name		= "dm9000",
	.id		= 0x91,
	.num_resources	= ARRAY_SIZE(dm9000_resources),
	.resource	= dm9000_resources,
	.dev = {
		.platform_data=&mxc_dm9000_data,
	}
};

static void mxc_init_dm9000(void)
{
	int ret;

	ret = gpio_request_mux(MX27_PIN_SSI3_FS, GPIO_MUX_GPIO);  
	if (ret) {
		printk(KERN_ERR "Request MUX SSI3_FS failed.\n");
		return;
	}
	mxc_set_gpio_direction(MX27_PIN_SSI3_FS, 1);
	gpio_set_puen(MX27_PIN_SSI3_FS, 0);
	set_irq_type(IOMUX_TO_IRQ(MX27_PIN_SSI3_FS), IRQT_HIGH);

	(void)platform_device_register(&mxc_dm9000_device);
}
#else
static inline void mxc_init_dm9000(void)
{
}
#endif

static struct spi_board_info mxc_spi_board_info[] __initdata = {
	{
	 .modalias = "pmic_spi",
	 .irq = IOMUX_TO_IRQ(MX27_PIN_TOUT),
	 .max_speed_hz = 4000000,
	 .bus_num = 2,
	 .chip_select = 0,
	 },
	{
	 .modalias = "lcd_spi",
	 .max_speed_hz = 1000000,
	 .bus_num = 1,
	 .chip_select = 0,
	 },
};

#ifndef CONFIG_MACH_MX27_MDK27V1
/*!
 * Interrupt handler for the expio (CPLD) to deal with interrupts from
 * external UART and SMSC Ethernet , etc.
 */
static void mxc_expio_irq_handler(u32 irq, struct irqdesc *desc)
{
	u32 expio_irq;
	u32 index, mask;
	desc->chip->mask(irq);	/* irq = gpio irq number */

	index = __raw_readw(PBC_CS5_ISR_REG);
	mask = __raw_readw(PBC_CS5_IMR_REG);

	if (unlikely(!(index & (~mask)))) {
		printk(KERN_ERR "\nEXPIO: Spurious interrupt:0x%0x\n\n", index);
		pr_info("CPLD IMR(0x38)=0x%x, PENDING(0x28)=0x%x\n", mask,
			index);
		goto out;
	}
	index = index & (~mask);

	expio_irq = MXC_EXP_IO_BASE;
	for (; index != 0; index >>= 1, expio_irq++) {
		struct irqdesc *d;
		if ((index & 1) == 0)
			continue;
		d = irq_desc + expio_irq;
		if (unlikely(!(d->handle_irq))) {
			printk(KERN_ERR "\nEXPIO irq: %d unhandeled\n",
			       expio_irq);
			BUG();	/* oops */
		}
		d->handle_irq(expio_irq, d);
	}

      out:
	desc->chip->ack(irq);
	desc->chip->unmask(irq);
}

/*
 * Disable an expio pin's interrupt by setting the bit in the imr.
 * @param irq		an expio virtual irq number
 */
static void expio_mask_irq(u32 irq)
{
	u16 reg, expio = MXC_IRQ_TO_EXPIO(irq);

	reg = __raw_readw(PBC_CS5_IMR_REG);
	/* mask the interrupt */
	__raw_writew(reg | (1 << expio), PBC_CS5_IMR_REG);
}

/*
 * Acknowledge an expanded io pin's interrupt by clearing the bit in the isr.
 * @param irq		an expanded io virtual irq number
 */
static void expio_ack_irq(u32 irq)
{
	u32 expio = MXC_IRQ_TO_EXPIO(irq);
	/* clear the interrupt status */
	__raw_writew(1 << expio, PBC_CS5_ICR_REG);
	__raw_writew(0, PBC_CS5_ICR_REG);
	/* mask the interrupt */
	expio_mask_irq(irq);
}

/*
 * Enable a expio pin's interrupt by clearing the bit in the imr.
 * @param irq		an expio virtual irq number
 */
static void expio_unmask_irq(u32 irq)
{
	u16 reg, expio = MXC_IRQ_TO_EXPIO(irq);

	reg = __raw_readw(PBC_CS5_IMR_REG);
	/* unmask the interrupt */
	__raw_writew(reg & (~(1 << expio)), PBC_CS5_IMR_REG);
}

static struct irqchip expio_irq_chip = {
	.ack = expio_ack_irq,
	.mask = expio_mask_irq,
	.unmask = expio_unmask_irq,
};
#endif				/* CONFIG_MACH_MX27_MDK27V1 */

static int __init mxc_expio_init(void)
{
	int i, major, minor;

#ifdef PBC_IS_VALID
	if (!PBC_IS_VALID()) {
		printk(KERN_ERR
		       "Invalid CPLD magic: Please check your CPLD version!\n");
	}
#endif
	mxc_pbc_ver = __raw_readw(PBC_CS5_VERSION_REG);
	major = MXC_PBC_VERSION_MAJOR(mxc_pbc_ver);
	minor = MXC_PBC_VERSION_MINOR(mxc_pbc_ver);
	pr_info("CPLD Verions is 0x%x [%d.%d] \n", mxc_pbc_ver, major, minor);

#ifndef CONFIG_MACH_MX27_MDK27V1
	/*
	 * Configure INT line as GPIO input
	 */
	gpio_config_mux(EXPIO_CS5_PARENT_INT, GPIO_MUX_GPIO);
	mxc_set_gpio_direction(EXPIO_CS5_PARENT_INT, 1);

	switch (major) {
	case MXC_PBC_VER_1_0:
		/*!
		 * For CPLD VERSION lower than 1.1
		 * Direcltly SET the irq as ENET INT
		 */
		if (minor < MXC_PBC_VER_2_0) {
			set_irq_type(EXPIO_CS5_PARENT_INT, IRQT_FALLING);
			break;
		}
	default:
		/* disable the interrupt and clear the status */
		__raw_writew(0x001F, PBC_CS5_IMR_REG);
		__raw_writew(0x001F, PBC_CS5_ICR_REG);
		__raw_writew(0x0000, PBC_CS5_ICR_REG);

		for (i = MXC_EXP_IO_BASE;
		     i < (MXC_EXP_IO_BASE + MXC_MAX_EXP_IO_LINES); i++) {
			set_irq_chip(i, &expio_irq_chip);
			set_irq_handler(i, do_level_IRQ);
			set_irq_flags(i, IRQF_VALID);
		}

		set_irq_type(EXPIO_CS5_PARENT_INT, IRQT_FALLING);
		set_irq_chained_handler(EXPIO_CS5_PARENT_INT,
					mxc_expio_irq_handler);
		break;
	}
#endif
	return 0;
}

#if defined(CONFIG_SERIAL_8250) || defined(CONFIG_SERIAL_8250_MODULE)

/*!
 * The serial port definition structure. The fields contain:
 * {UART, CLK, PORT, IRQ, FLAGS}
 */
static struct plat_serial8250_port serial_platform_data[] = {
	{
	 .membase = (void __iomem *)(PBC_CS5_EUARTA_BASE),
	 .mapbase = (unsigned long)(CS5_BASE_ADDR + 0x08000),
	 .irq = EXPIO_CS5_INT_XUART_INTA,
	 .uartclk = 14745600,
	 .regshift = 1,
	 .iotype = UPIO_MEM,
	 .flags = UPF_BOOT_AUTOCONF | UPF_SKIP_TEST | UPF_AUTO_IRQ,
	 /*.pm = serial_platform_pm, */
	 },
	{
	 .membase = (void __iomem *)(PBC_CS5_EUARTB_BASE),
	 .mapbase = (unsigned long)(CS5_BASE_ADDR + 0x10000),
	 .irq = EXPIO_CS5_INT_XUART_INTB,
	 .uartclk = 14745600,
	 .regshift = 1,
	 .iotype = UPIO_MEM,
	 .flags = UPF_BOOT_AUTOCONF | UPF_SKIP_TEST | UPF_AUTO_IRQ,
	 /*.pm = serial_platform_pm, */
	 },
	{},
};

/*!
 * REVISIT: document me
 */
static struct platform_device serial_device = {
	.name = "serial8250",
	.id = 0,
	.dev = {
		.platform_data = serial_platform_data,
		},
};

/*!
 * REVISIT: document me
 */
static int __init mxc_init_extuart(void)
{
	int ret = 0, value;
	/*reset ext uart in cpld */
	ret = __raw_readw(PBC_CS5_RST_REG);
	__raw_writew(ret | 0x01, PBC_CS5_RST_REG);
	/*delay some time for reset finish */
	for (value = 0; value < 2000; value++) ;
	__raw_writew(ret & (~0x01), PBC_CS5_RST_REG);

	ret = platform_device_register(&serial_device);
	if (ret < 0) {
		pr_info("Error: register external uart failure\n");
	}
	return ret;
}
#else
static inline int mxc_init_extuart(void)
{
	return 0;
}
#endif

#if defined(CONFIG_MXC_PMIC_MC13783) && defined(CONFIG_SND_MXC_PMIC)
extern void gpio_ssi_active(int ssi_num);

static void __init mxc_init_pmic_audio(void)
{
	struct clk *ssi_clk;
	struct clk *ckih_clk;
	struct clk *cko_clk;

	/* Enable 26 mhz clock on CKO1 for PMIC audio */
	ckih_clk = clk_get(NULL, "ckih");
	cko_clk = clk_get(NULL, "clko_clk");
	if (IS_ERR(ckih_clk) || IS_ERR(cko_clk)) {
		printk(KERN_ERR "Unable to set CLKO output to CKIH\n");
	} else {
		clk_set_parent(cko_clk, ckih_clk);
		clk_set_rate(cko_clk, clk_get_rate(ckih_clk));
		clk_enable(cko_clk);
	}
	clk_put(ckih_clk);
	clk_put(cko_clk);

	ssi_clk = clk_get(NULL, "ssi_clk.0");
	clk_enable(ssi_clk);
	clk_put(ssi_clk);
	ssi_clk = clk_get(NULL, "ssi_clk.1");
	clk_enable(ssi_clk);
	clk_put(ssi_clk);

	gpio_ssi_active(0);
	gpio_ssi_active(1);
	gpio_ssi_active(3);
}
#else
static void __inline mxc_init_pmic_audio(void)
{
}
#endif

#if defined(CONFIG_MXC_HWEVENT) || defined(CONFIG_MXC_HWEVENT_MODULE)

static int jack_get_state(void)
{
	return mxc_get_gpio_ssr(MX27_PIN_SSI3_RXDAT);
}

static int jack_check_type(void)
{
	/*TODO:: Wait ADC module ready */
#if 0
	unsigned short int data[8];
	int i, ret = 0;
	/*Check add ADC : */
	if (pmic_adc_convert_8x(GEN_PURPOSE_AD6, data)) {
		printk("ADC for channel ADC_1 failure\n");
		return -1;
	}
	for (i = 0; i < 8; i++) {
		ret += data[i];
		printk("channel:%d\n", data[i]);
	}
	return ret;
#else
	return 0;

#endif
}

static void jack_setup_irq(int state)
{
	if (state) {
		set_irq_type(IOMUX_TO_IRQ(MX27_PIN_SSI3_RXDAT), IRQT_FALLING);
	} else {
		set_irq_type(IOMUX_TO_IRQ(MX27_PIN_SSI3_RXDAT), IRQT_RISING);
	}
}

struct {
	int (*check_type) (void);
	int (*get_state) (void);
	void (*setup_irq) (int state);
} mxc_jack_conf = {
.check_type = jack_check_type,.get_state = jack_get_state,.setup_irq =
	    jack_setup_irq,};

static struct resource mxc_jack_resources[] = {
	[0] = {
	       .start = IOMUX_TO_IRQ(MX27_PIN_SSI3_RXDAT),
	       .end = IOMUX_TO_IRQ(MX27_PIN_SSI3_RXDAT),
	       .flags = IORESOURCE_IRQ,
	       }
};

/* mxc jack device*/
static struct platform_device mxc_jack_device = {
	.name = "mxc_jack",
	.id = 0,
	.num_resources = ARRAY_SIZE(mxc_jack_resources),
	.resource = mxc_jack_resources,
	.dev = {
		.release = mxc_nop_release,
		.platform_data = &mxc_jack_conf,
		},
};

static void mxc_init_jack(void)
{
	(void)platform_device_register(&mxc_jack_device);
}
#else
static void mxc_init_jack(void)
{
}
#endif

static void mxc_board_init(void)
{
	pr_info("AIPI VA base: 0x%x\n", IO_ADDRESS(AIPI_BASE_ADDR));

	mxc_clocks_init();
	mxc_gpio_init();
	mxc_expio_init();
	mxc_init_keypad();
	mxc_init_nand_mtd();
	mxc_init_extuart();
	mxc_init_pmic_audio();
	mxc_init_enet();
	mxc_init_jack();
	mxc_init_dm9000();

	spi_register_board_info(mxc_spi_board_info,
				ARRAY_SIZE(mxc_spi_board_info));
	mxc_init_fb();
}

static void __init fixup_mxc_board(struct machine_desc *desc, struct tag *tags,
				   char **cmdline, struct meminfo *mi)
{
#ifdef CONFIG_KGDB_8250
	int i;
	for (i = 0;
	     i <
	     (sizeof(serial_platform_data) / sizeof(serial_platform_data[0]));
	     i += 1)
		kgdb8250_add_platform_port(i, &serial_platform_data[i]);
#endif

#ifdef CONFIG_DISCONTIGMEM
	do {
		int nid;
		mi->nr_banks = MXC_NUMNODES;
		for (nid = 0; nid < mi->nr_banks; nid++) {
			SET_NODE(mi, nid);
		}
	} while (0);
#endif

#ifdef CONFIG_MACH_MX27_MDK27V1
	for (; tags->hdr.size; tags = tag_next(tags)) {
		if (tags->hdr.tag == ATAG_MEM) {
			tags->u.mem.size = MEM_SIZE;
			tags->u.mem.start = PHYS_OFFSET;
		}
	}
#endif
}

/*
 * The following uses standard kernel macros define in arch.h in order to
 * initialize __mach_desc_MX27ADS data structure.
 */
/* *INDENT-OFF* */
MACHINE_START(MX27_MDK27V0, "Morninghan i.MX27 MDK27V0")
        /* maintainer: Freescale Semiconductor, Inc. */
#ifdef CONFIG_SERIAL_8250_CONSOLE
        .phys_io        = CS5_BASE_ADDR,
        .io_pg_offst    = ((CS5_BASE_ADDR_VIRT) >> 18) & 0xfffc,
#else
        .phys_io        = AIPI_BASE_ADDR,
        .io_pg_offst    = ((AIPI_BASE_ADDR_VIRT) >> 18) & 0xfffc,
#endif
        .boot_params    = PHYS_OFFSET + 0x100,
        .fixup          = fixup_mxc_board,
        .map_io         = mxc_map_io,
        .init_irq       = mxc_init_irq,
        .init_machine   = mxc_board_init,
        .timer          = &mxc_timer,
MACHINE_END

