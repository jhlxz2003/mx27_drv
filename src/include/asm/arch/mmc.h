/*
 * Copyright 2006-2007 Freescale Semiconductor, Inc. All Rights Reserved.
 */

/*
 * The code contained herein is licensed under the GNU General Public
 * License. You may obtain a copy of the GNU General Public License
 * Version 2 or later at the following locations:
 *
 * http://www.opensource.org/licenses/gpl-license.html
 * http://www.gnu.org/copyleft/gpl.html
 */
#ifndef __ASM_ARCH_MXC_MMC_H__
#define __ASM_ARCH_MXC_MMC_H__

#include <linux/mmc/protocol.h>

struct mxc_mmc_platform_data {
	unsigned int ocr_mask;	/* available voltages */
	unsigned int min_clk;
	unsigned int max_clk;
	unsigned int card_inserted_state;
//      u32 (*translate_vdd)(struct device *, unsigned int);
	unsigned int (*status) (struct device *);
};

#ifdef CONFIG_MACH_MX27_MDK27V0
/*!
 * this structure is a way for the low level driver to define their own
 * \b mmc_host structure. this structure includes the core \b mmc_host
 * structure that is provided by linux mmc/sd bus protocol driver as an
 * element and has other elements that are specifically required by this
 * low-level driver.
 */
struct mxcmci_host {
	/*!
	 * the mmc structure holds all the information about the device
	 * structure, current sdhc io bus settings, the current ocr setting,
	 * devices attached to this host, and so on.
	 */
	struct mmc_host *mmc;

	/*!
	 * this variable is used for locking the host data structure from
	 * multiple access.
	 */
	spinlock_t lock;

	/*!
	 * resource structure, which will maintain base addresses and irqs.
	 */
	struct resource *res;

	/*!
	 * base address of sdhc, used in readl and writel.
	 */
	void *base;

	/*!
	 * sdhc irq number.
	 */
	int irq;

	/*!
	 * card detect irq number.
	 */
	int detect_irq;

	/*!
	 * clock struct ipg_perclk.
	 */
	struct clk *clk;
	/*!
	 * mmc mode.
	 */
	int mode;

	/*!
	 * dma channel number.
	 */
	int dma;

	/*!
	 * pointer to hold mmc/sd request.
	 */
	struct mmc_request *req;

	/*!
	 * pointer to hold mmc/sd command.
	 */
	struct mmc_command *cmd;

	/*!
	 * pointer to hold mmc/sd data.
	 */
	struct mmc_data *data;

	/*!
	 * holds the number of bytes to transfer using dma.
	 */
	unsigned int dma_size;

	/*!
	 * value to store in command and data control register
	 * - currently unused
	 */
	unsigned int cmdat;

	/*!
	 * power mode - currently unused
	 */
	unsigned int power_mode;

	/*!
	 * dma address for scatter-gather transfers
	 */
	dma_addr_t sg_dma;

	/*!
	 * length of the scatter-gather list
	 */
	unsigned int dma_len;

	/*!
	 * holds the direction of data transfer.
	 */
	unsigned int dma_dir;

	/*!
	 * id for mmc block.
	 */
	unsigned int id;

	/*!
	 * note whether this driver has been suspended.
	 */
	unsigned int mxc_mmc_suspend_flag;

	/*!
	 * note whether the sdio irq wake function is enabled.
	 */
	unsigned int sdio_set_wake_enable;

	/*!
	 * platform specific data
	 */
	struct mxc_mmc_platform_data *plat_data;
};
#endif

#endif
