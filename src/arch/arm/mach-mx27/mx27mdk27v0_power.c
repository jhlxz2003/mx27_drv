/*
 *  Copyright 2007 Freescale Semiconductor, Inc. All Rights Reserved.
 */

/*
 * The code contained herein is licensed under the GNU General Public
 * License. You may obtain a copy of the GNU General Public License
 * Version 2 or later at the following locations:
 *
 * http://www.opensource.org/licenses/gpl-license.html
 * http://www.gnu.org/copyleft/gpl.html
 */

#include <linux/errno.h>
#include <linux/module.h>
#include <linux/device.h>
#include <asm/io.h>
#include <asm/hardware.h>
#include <linux/delay.h>
#include <asm/arch/gpio.h>
#include <asm/arch/pmic_power.h>

#include "gpio_mux.h"

/*!
 * @file mach-mx27/mx27_mdk27v0_power.c
 *
 * @brief This file contains all the GPIO setup functions for the board.
 *
 * @ingroup MSL_MX27
 */

/*define structures for power gate*/
struct power_gate_s;
typedef struct power_gate_s power_gate_t;
typedef int (power_fn_t) (power_gate_t *);

struct power_gate_s {
	int gate;
	int count;
	power_fn_t *power_on;
	power_fn_t *power_off;
	power_gate_t *parent;
};

/*
 *  the default function of power on
 */
static int inline power_default_on(power_gate_t * power)
{
	int ret = 0;
#ifdef CONFIG_MXC_MC13783_POWER
	if ((power->count++) == 0) {
		ret = pmic_power_regulator_on(power->gate);
		/* TODO:: The hardware issue require to wait 100ms */
		if (!ret)
			mdelay(100);
	}
#endif
	return ret;
}

/*
 *  the default function of power off
 */
static int power_default_off(power_gate_t * power)
{
#ifdef CONFIG_MXC_MC13783_POWER
	if ((power->count--) == 1) {
		return pmic_power_regulator_off(power->gate);
	}
#endif
	return 0;
}

/*
 *  enable the output of power gate
 */
static int mxc_power_on(power_gate_t * power)
{
	int ret = 0;
	BUG_ON(!power);
	if (power->parent) {
		ret = mxc_power_on(power->parent);
		if (ret)
			return ret;
	}
	ret = power->power_on(power);
	return ret;
}

/*
 *  disable the output of power gate
 */
static int mxc_power_off(power_gate_t * power)
{
	int ret;
	BUG_ON(!power);
	ret = power->power_off(power);
	if (ret)
		return ret;
	if (power->parent) {
		return mxc_power_off(power->parent);
	}
	return 0;
}

/*!
 * Power gate for external 3.3 v
 */
static power_gate_t mxc_power_ext_3v3 = {
	.gate = REGU_GPO1,
	.count = 0,
	.power_on = power_default_on,
	.power_off = power_default_off,
	.parent = NULL,
};

/*!
 * Power gate for VMMC2
 */
static power_gate_t mxc_power_vmmc2 = {
	.gate = REGU_VMMC2,
	.count = 0,
	.power_on = power_default_on,
	.power_off = power_default_off,
	.parent = NULL,
};

/*!
 * Power gate for ata 3.3v
 */
static power_gate_t mxc_power_ata_3v3 = {
	.gate = REGU_GPO2,
	.count = 0,
	.power_on = power_default_on,
	.power_off = power_default_off,
	.parent = &mxc_power_ext_3v3,
};

/*!
 * Turn power of HDD to on
 *
 */
int mxc_ata_power_on(void)
{
	int ret;
	ret = mxc_power_on(&mxc_power_ata_3v3);
	if (ret)
		return ret;

	mxc_set_gpio_dataout(MX27_PIN_PWMO, 0);

	/*Wait the moto of ATA to reach nice speed */
	msleep(200);
	return 0;
}

/*!
 * Shutdown the power of HDD PHY
 *
 */
int mxc_ata_power_off(void)
{
	mxc_set_gpio_dataout(MX27_PIN_PWMO, 0);

	return mxc_power_off(&mxc_power_ata_3v3);
}

/*!
 * Turn power of FEC PHY to on
 *
 */
int mxc_fec_power_on(void)
{
	int ret;
	ret = mxc_power_on(&mxc_power_ext_3v3);
	if (ret)
		return ret;

	/*Enable FEC_3V3 */
	mxc_set_gpio_dataout(MX27_PIN_USB_OC_B, 1);

	udelay(2);
	/*
	 * Trigger FEC reset signal: Low pulse  
	 */
	mxc_set_gpio_dataout(MX27_PIN_PC_RST, 0);
	/* At least wait 100us */
	udelay(250);
	mxc_set_gpio_dataout(MX27_PIN_PC_RST, 1);
	msleep(350);
	return 0;
}

/*!
 * Shutdown the power of FEC PHY
 *
 */
int mxc_fec_power_off(void)
{
	/*disable FEC_3V3 */
	mxc_set_gpio_dataout(MX27_PIN_USB_OC_B, 0);
	return mxc_power_off(&mxc_power_ext_3v3);
}
/*!
 * Turn power of SD/MMC to on
 *
 */
int mxc_sdmmc_power_on(void)
{
	int ret = 0;
        ret = mxc_power_on(&mxc_power_ext_3v3);
  	if (ret){
  		pr_debug("can't power on the ext_3v3 for sd/mmc card\n");
  		return ret;
 	 }
  	ret = mxc_power_on(&mxc_power_vmmc2);
	if (ret){
  		pr_debug("can't power on the vmmc2 for sd/mmc card\n");
  		return ret;
  	}
  	
  	return ret;	
}

/*!
 * Shutdown the power of SD/MMC
 *
 */
int mxc_sdmmc_power_off(void)
{
	int ret = 0;
  	ret = mxc_power_off(&mxc_power_ext_3v3);
  	if (ret){
  		pr_debug("can't power off the ext_3v3 for sd/mmc card\n");
  		return ret;
	}
	ret = mxc_power_off(&mxc_power_vmmc2);
	if (ret){
  		pr_debug("can't power off the vmmc2 for sd/mmc card\n");
  		return ret;
  	}
			
  	return ret;              
}

EXPORT_SYMBOL(mxc_ata_power_on);
EXPORT_SYMBOL(mxc_ata_power_off);
EXPORT_SYMBOL(mxc_fec_power_on);
EXPORT_SYMBOL(mxc_fec_power_off);
EXPORT_SYMBOL(mxc_sdmmc_power_on);
EXPORT_SYMBOL(mxc_sdmmc_power_off);
