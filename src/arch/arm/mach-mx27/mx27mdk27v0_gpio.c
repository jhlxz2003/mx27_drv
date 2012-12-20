/*
 *  Copyright 2004-2007 Freescale Semiconductor, Inc. All Rights Reserved.
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
#include <linux/platform_device.h>
#include <linux/device.h>
#include <asm/io.h>
#include <asm/hardware.h>
#include <asm/arch/gpio.h>
#include <linux/delay.h>
#include <asm/arch/pmic_power.h>

#include "gpio_mux.h"
#include <linux/irq.h>
#include <linux/mmc/host.h>
#include <../drivers/mmc/mxc_mmc.h>
#include <asm/arch/mmc.h>
#include "crm_regs.h"
#include <asm/arch/pmic_adc.h>
static int g_uart_activated[MXC_UART_NR] = { 0, 0, 0, 0, 0, 0 };

/*!
 * @file mach-mx27/mx27_mdk27v0_gpio.c
 *
 * @brief This file contains all the GPIO setup functions for the board.
 *
 * @ingroup GPIO_MX27
 */

/*!
 * Setup GPIO for a UART port to be active
 *
 * @param  port         a UART port
 * @param  ir_mode      indicates if the port is used for SIR
 */
void gpio_uart_active(int port, int ir_mode)
{
	if (port < 0 || port >= MXC_UART_NR) {
		pr_info("Wrong port number: %d\n", port);
		BUG();
	}

	if (g_uart_activated[port]) {
		pr_info("UART %d has been activated multiple times\n",
			port + 1);
		return;
	}
	g_uart_activated[port] = 1;

	switch (port) {
	case 0:
		gpio_request_mux(MX27_PIN_UART1_TXD, GPIO_MUX_PRIMARY);
		gpio_request_mux(MX27_PIN_UART1_RXD, GPIO_MUX_PRIMARY);
		gpio_request_mux(MX27_PIN_UART1_CTS, GPIO_MUX_PRIMARY);
		gpio_request_mux(MX27_PIN_UART1_RTS, GPIO_MUX_PRIMARY);
		break;
	case 1:
		gpio_request_mux(MX27_PIN_UART2_TXD, GPIO_MUX_PRIMARY);
		gpio_request_mux(MX27_PIN_UART2_RXD, GPIO_MUX_PRIMARY);
		gpio_request_mux(MX27_PIN_UART2_CTS, GPIO_MUX_PRIMARY);
		gpio_request_mux(MX27_PIN_UART2_RTS, GPIO_MUX_PRIMARY);
		break;
	case 2:
		gpio_request_mux(MX27_PIN_UART3_TXD, GPIO_MUX_PRIMARY);
		gpio_request_mux(MX27_PIN_UART3_RXD, GPIO_MUX_PRIMARY);
		gpio_request_mux(MX27_PIN_UART3_CTS, GPIO_MUX_PRIMARY);
		gpio_request_mux(MX27_PIN_UART3_RTS, GPIO_MUX_PRIMARY);

		break;
	case 3:
		gpio_request_mux(MX27_PIN_USBH1_TXDM, GPIO_MUX_ALT);
		gpio_request_mux(MX27_PIN_USBH1_RXDP, GPIO_MUX_ALT);
		gpio_request_mux(MX27_PIN_USBH1_TXDP, GPIO_MUX_ALT);
		gpio_request_mux(MX27_PIN_USBH1_FS, GPIO_MUX_ALT);
		break;
	case 4:
		gpio_request_mux(MX27_PIN_CSI_D6, GPIO_MUX_ALT);
		gpio_request_mux(MX27_PIN_CSI_D7, GPIO_MUX_ALT);
		gpio_request_mux(MX27_PIN_CSI_VSYNC, GPIO_MUX_ALT);
		gpio_request_mux(MX27_PIN_CSI_HSYNC, GPIO_MUX_ALT);
		break;
	case 5:
		gpio_request_mux(MX27_PIN_CSI_D0, GPIO_MUX_ALT);
		gpio_request_mux(MX27_PIN_CSI_D1, GPIO_MUX_ALT);
		gpio_request_mux(MX27_PIN_CSI_D2, GPIO_MUX_ALT);
		gpio_request_mux(MX27_PIN_CSI_D3, GPIO_MUX_ALT);
		break;
	default:
		break;
	}
}

/*!
 * Setup GPIO for a UART port to be inactive
 *
 * @param  port         a UART port
 * @param  ir_mode      indicates if the port is used for SIR
 */
void gpio_uart_inactive(int port, int ir_mode)
{
	if (port < 0 || port >= MXC_UART_NR) {
		pr_info("Wrong port number: %d\n", port);
		BUG();
	}

	if (g_uart_activated[port] == 0) {
		pr_info("UART %d has not been activated \n", port + 1);
		return;
	}
	g_uart_activated[port] = 0;

	switch (port) {
	case 0:
		gpio_free_mux(MX27_PIN_UART1_TXD);
		gpio_free_mux(MX27_PIN_UART1_RXD);
		gpio_free_mux(MX27_PIN_UART1_CTS);
		gpio_free_mux(MX27_PIN_UART1_RTS);
		break;
	case 1:
		gpio_free_mux(MX27_PIN_UART2_TXD);
		gpio_free_mux(MX27_PIN_UART2_RXD);
		gpio_free_mux(MX27_PIN_UART2_CTS);
		gpio_free_mux(MX27_PIN_UART2_RTS);
		break;
	case 2:
		gpio_free_mux(MX27_PIN_UART3_TXD);
		gpio_free_mux(MX27_PIN_UART3_RXD);
		gpio_free_mux(MX27_PIN_UART3_CTS);
		gpio_free_mux(MX27_PIN_UART3_RTS);
		break;
	case 3:
		gpio_free_mux(MX27_PIN_USBH1_TXDM);
		gpio_free_mux(MX27_PIN_USBH1_RXDP);
		gpio_free_mux(MX27_PIN_USBH1_TXDP);
		gpio_free_mux(MX27_PIN_USBH1_FS);
		break;
	case 4:
		gpio_free_mux(MX27_PIN_CSI_D6);
		gpio_free_mux(MX27_PIN_CSI_D7);
		gpio_free_mux(MX27_PIN_CSI_VSYNC);
		gpio_free_mux(MX27_PIN_CSI_HSYNC);
		break;
	case 5:
		gpio_free_mux(MX27_PIN_CSI_D0);
		gpio_free_mux(MX27_PIN_CSI_D1);
		gpio_free_mux(MX27_PIN_CSI_D2);
		gpio_free_mux(MX27_PIN_CSI_D3);
		break;
	default:
		break;
	}
}

/*!
 * Configure the IOMUX GPR register to receive shared SDMA UART events
 *
 * @param  port         a UART port
 */
void config_uartdma_event(int port)
{
	return;
}

/*!
 * Setup GPIO for USB, Total 34 signals
 * PIN Configuration for USBOTG:   High/Full speed OTG
 *	PE2,PE1,PE0,PE24,PE25 -- PRIMARY
 	PC7 - PC13  -- PRIMARY
 	PB23,PB24 -- PRIMARY

  * PIN Configuration for USBH2:    : High/Full/Low speed host
  *	PA0 - PA4 -- PRIMARY
       PD19, PD20,PD21,PD22,PD23,PD24,PD26 --Alternate (SECONDARY)

  * PIN Configuration for USBH1:  Full/low speed host
  *  PB25 - PB31  -- PRIMARY
      PB22  -- PRIMARY
 */
void gpio_usbh1_active(void)
{
//	gpio_request_mux(MX27_PIN_USBH1_SUSP, GPIO_MUX_PRIMARY);
//	gpio_request_mux(MX27_PIN_USBH1_RCV, GPIO_MUX_PRIMARY);
	gpio_request_mux(MX27_PIN_USBH1_FS, GPIO_MUX_PRIMARY);
	gpio_request_mux(MX27_PIN_USBH1_OE_B, GPIO_MUX_PRIMARY);
	gpio_request_mux(MX27_PIN_USBH1_TXDM, GPIO_MUX_PRIMARY);
//	gpio_request_mux(MX27_PIN_USBH1_TXDP, GPIO_MUX_PRIMARY);
	gpio_request_mux(MX27_PIN_USBH1_RXDM, GPIO_MUX_PRIMARY);
	gpio_request_mux(MX27_PIN_USBH1_RXDP, GPIO_MUX_PRIMARY);
}
void gpio_usbh1_inactive(void)
{
//	gpio_free_mux(MX27_PIN_USBH1_SUSP);
//	gpio_free_mux(MX27_PIN_USBH1_RCV);
	gpio_free_mux(MX27_PIN_USBH1_FS);
	gpio_free_mux(MX27_PIN_USBH1_OE_B);
	gpio_free_mux(MX27_PIN_USBH1_TXDM);
	gpio_free_mux(MX27_PIN_USBH1_TXDP);
	gpio_free_mux(MX27_PIN_USBH1_RXDM);
	gpio_free_mux(MX27_PIN_USBH1_RXDP);
}

/*
 * conflicts with CSPI1 (MC13783) and CSPI2 (Connector)
 */
void gpio_usbh2_active(void)
{
	gpio_set_puen(MX27_PIN_USBH2_CLK, 0);
	gpio_set_puen(MX27_PIN_USBH2_DIR, 0);
	gpio_set_puen(MX27_PIN_USBH2_DATA7, 0);
	gpio_set_puen(MX27_PIN_USBH2_NXT, 0);
	gpio_set_puen(MX27_PIN_USBH2_STP, 0);
	gpio_set_puen(MX27_PIN_CSPI2_SS2, 0);
	gpio_set_puen(MX27_PIN_CSPI2_SS1, 0);
	gpio_set_puen(MX27_PIN_CSPI2_SS0, 0);
	gpio_set_puen(MX27_PIN_CSPI2_SCLK, 0);
	gpio_set_puen(MX27_PIN_CSPI2_MISO, 0);
	gpio_set_puen(MX27_PIN_CSPI2_MOSI, 0);
	gpio_set_puen(MX27_PIN_CSPI1_SS2, 0);

	gpio_request_mux(MX27_PIN_USBH2_CLK, GPIO_MUX_PRIMARY);
	gpio_request_mux(MX27_PIN_USBH2_DIR, GPIO_MUX_PRIMARY);
	gpio_request_mux(MX27_PIN_USBH2_DATA7, GPIO_MUX_PRIMARY);
	gpio_request_mux(MX27_PIN_USBH2_NXT, GPIO_MUX_PRIMARY);
	gpio_request_mux(MX27_PIN_USBH2_STP, GPIO_MUX_PRIMARY);

	gpio_request_mux(MX27_PIN_CSPI2_SS2, GPIO_MUX_ALT);
	gpio_request_mux(MX27_PIN_CSPI2_SS1, GPIO_MUX_ALT);
	gpio_request_mux(MX27_PIN_CSPI2_SS0, GPIO_MUX_ALT);
	gpio_request_mux(MX27_PIN_CSPI2_SCLK, GPIO_MUX_ALT);
	gpio_request_mux(MX27_PIN_CSPI2_MISO, GPIO_MUX_ALT);
	gpio_request_mux(MX27_PIN_CSPI2_MOSI, GPIO_MUX_ALT);
	gpio_request_mux(MX27_PIN_CSPI1_SS2, GPIO_MUX_ALT);

	/* reset */
#if 0
	gpio_request_mux(MX27_PIN_SD2_CLK, GPIO_MUX_GPIO);
	gpio_config_mux(MX27_PIN_SD2_CLK, GPIO_MUX_GPIO); 
	mxc_set_gpio_direction(MX27_PIN_SD2_CLK, 0);
	mxc_set_gpio_dataout(MX27_PIN_SD2_CLK, 0);
	mdelay(10);
	mxc_set_gpio_dataout(MX27_PIN_SD2_CLK, 1);
#endif
}
void gpio_usbh2_inactive(void)
{
	gpio_free_mux(MX27_PIN_USBH2_CLK);
	gpio_free_mux(MX27_PIN_USBH2_DIR);
	gpio_free_mux(MX27_PIN_USBH2_DATA7);
	gpio_free_mux(MX27_PIN_USBH2_NXT);
	gpio_free_mux(MX27_PIN_USBH2_STP);

	//gpio_free_mux(MX27_PIN_CSPI2_SS2);
	//gpio_free_mux(MX27_PIN_CSPI2_SS1);
	gpio_free_mux(MX27_PIN_CSPI2_SS0);
	gpio_free_mux(MX27_PIN_CSPI2_SCLK);
	gpio_free_mux(MX27_PIN_CSPI2_MISO);
	gpio_free_mux(MX27_PIN_CSPI2_MOSI);
	gpio_free_mux(MX27_PIN_CSPI1_SS2);

	gpio_set_puen(MX27_PIN_USBH2_CLK, 1);
	gpio_set_puen(MX27_PIN_USBH2_DIR, 1);
	gpio_set_puen(MX27_PIN_USBH2_DATA7, 1);
	gpio_set_puen(MX27_PIN_USBH2_NXT, 1);
	gpio_set_puen(MX27_PIN_USBH2_STP, 1);
	//gpio_set_puen(MX27_PIN_CSPI2_SS2, 1);
	//gpio_set_puen(MX27_PIN_CSPI2_SS1, 1);
	gpio_set_puen(MX27_PIN_CSPI2_SS0, 1);
	gpio_set_puen(MX27_PIN_CSPI2_SCLK, 1);
	gpio_set_puen(MX27_PIN_CSPI2_MISO, 1);
	gpio_set_puen(MX27_PIN_CSPI2_MOSI, 1);
	gpio_set_puen(MX27_PIN_CSPI1_SS2, 1);
}

void gpio_usbotg_hs_active(void)
{
	t_regulator_voltage volt;

	gpio_request_mux(MX27_PIN_USBOTG_DATA5, GPIO_MUX_PRIMARY);
	gpio_request_mux(MX27_PIN_USBOTG_DATA6, GPIO_MUX_PRIMARY);
	gpio_request_mux(MX27_PIN_USBOTG_DATA0, GPIO_MUX_PRIMARY);
	gpio_request_mux(MX27_PIN_USBOTG_DATA2, GPIO_MUX_PRIMARY);
	gpio_request_mux(MX27_PIN_USBOTG_DATA1, GPIO_MUX_PRIMARY);
	gpio_request_mux(MX27_PIN_USBOTG_DATA3, GPIO_MUX_PRIMARY);
	gpio_request_mux(MX27_PIN_USBOTG_DATA4, GPIO_MUX_PRIMARY);

	gpio_request_mux(MX27_PIN_USBOTG_DIR, GPIO_MUX_PRIMARY);
	gpio_request_mux(MX27_PIN_USBOTG_STP, GPIO_MUX_PRIMARY);
	gpio_request_mux(MX27_PIN_USBOTG_NXT, GPIO_MUX_PRIMARY);
	gpio_request_mux(MX27_PIN_USBOTG_CLK, GPIO_MUX_PRIMARY);
	gpio_request_mux(MX27_PIN_USBOTG_DATA7, GPIO_MUX_PRIMARY);

	gpio_request_mux(MX27_PIN_USB_OC_B, GPIO_MUX_PRIMARY);
	gpio_request_mux(MX27_PIN_USB_PWR, GPIO_MUX_PRIMARY);

	volt.sw2a = SW1A_1_8V;
#ifdef CONFIG_MXC_MC13783_POWER
	pmic_power_regulator_set_voltage(SW_SW2A, volt);
#endif

	gpio_request_mux(MX27_PIN_SSI3_CLK, GPIO_MUX_GPIO);
	gpio_config_mux(MX27_PIN_SSI3_CLK, GPIO_MUX_GPIO);    /* USBOTG_RST_B */
        mxc_set_gpio_direction(MX27_PIN_SSI3_CLK, 0);
        mxc_set_gpio_dataout(MX27_PIN_SSI3_CLK, 0);
	mdelay(1);
        mxc_set_gpio_dataout(MX27_PIN_SSI3_CLK, 1);
};

void gpio_usbotg_hs_inactive(void)
{
	gpio_free_mux(MX27_PIN_USBOTG_DATA5);
	gpio_free_mux(MX27_PIN_USBOTG_DATA6);
	gpio_free_mux(MX27_PIN_USBOTG_DATA0);
	gpio_free_mux(MX27_PIN_USBOTG_DATA2);
	gpio_free_mux(MX27_PIN_USBOTG_DATA1);
	gpio_free_mux(MX27_PIN_USBOTG_DATA3);
	gpio_free_mux(MX27_PIN_USBOTG_DATA4);

	gpio_free_mux(MX27_PIN_USBOTG_DIR);
	gpio_free_mux(MX27_PIN_USBOTG_STP);
	gpio_free_mux(MX27_PIN_USBOTG_NXT);
	gpio_free_mux(MX27_PIN_USBOTG_CLK);
	gpio_free_mux(MX27_PIN_USBOTG_DATA7);

	gpio_free_mux(MX27_PIN_USB_OC_B);
	gpio_free_mux(MX27_PIN_USB_PWR);

	gpio_free_mux(MX27_PIN_SSI3_CLK);
}

void gpio_usbotg_fs_active(void)
{
	return gpio_usbotg_hs_active();
}

void gpio_usbotg_fs_inactive(void)
{
	return gpio_usbotg_hs_inactive();
}

/*!
 * end Setup GPIO for USB
 *
 */

/************************************************************************/
/* for i2c gpio                                                         */
/* I2C1:  PD17,PD18 -- Primary 					*/
/* I2C2:  PC5,PC6    -- Primary					*/
/************************************************************************/
/*!
* Setup GPIO for an I2C device to be active
*
* @param  i2c_num         an I2C device
*/
void gpio_i2c_active(int i2c_num)
{
	switch (i2c_num) {
	case 0:
		gpio_request_mux(MX27_PIN_I2C_CLK, GPIO_MUX_PRIMARY);
		gpio_request_mux(MX27_PIN_I2C_DATA, GPIO_MUX_PRIMARY);
		break;
	case 1:
		gpio_request_mux(MX27_PIN_I2C2_SCL, GPIO_MUX_PRIMARY);
		gpio_request_mux(MX27_PIN_I2C2_SDA, GPIO_MUX_PRIMARY);
		break;
	default:
		printk(KERN_ERR "gpio_i2c_active no compatible I2C adapter\n");
		break;
	}
}

/*!
 *  * Setup GPIO for an I2C device to be inactive
 *   *
 *    * @param  i2c_num         an I2C device
 */
void gpio_i2c_inactive(int i2c_num)
{
	switch (i2c_num) {
	case 0:
		gpio_free_mux(MX27_PIN_I2C_CLK);
		gpio_free_mux(MX27_PIN_I2C_DATA);
		break;
	case 1:
		gpio_free_mux(MX27_PIN_I2C2_SCL);
		gpio_free_mux(MX27_PIN_I2C2_SDA);
		break;
	default:
		break;
	}
}

/*!
 * Setup GPIO for a CSPI device to be active
 *
 * @param  cspi_mod         an CSPI device
 */
void gpio_spi_active(int cspi_mod)
{
	switch (cspi_mod) {
	case 0:
		/* SPI1 */
		gpio_request_mux(MX27_PIN_CSPI1_MOSI, GPIO_MUX_PRIMARY);
		gpio_request_mux(MX27_PIN_CSPI1_MISO, GPIO_MUX_PRIMARY);
		gpio_request_mux(MX27_PIN_CSPI1_SCLK, GPIO_MUX_PRIMARY);
		gpio_request_mux(MX27_PIN_CSPI1_RDY, GPIO_MUX_PRIMARY);
		gpio_request_mux(MX27_PIN_CSPI1_SS0, GPIO_MUX_PRIMARY);
//		gpio_request_mux(MX27_PIN_CSPI1_SS1, GPIO_MUX_PRIMARY);
//		gpio_request_mux(MX27_PIN_CSPI1_SS2, GPIO_MUX_PRIMARY);
		break;
	case 1:
		/*SPI2  */
		gpio_request_mux(MX27_PIN_CSPI2_MOSI, GPIO_MUX_PRIMARY);
		gpio_request_mux(MX27_PIN_CSPI2_MISO, GPIO_MUX_PRIMARY);
		gpio_request_mux(MX27_PIN_CSPI2_SCLK, GPIO_MUX_PRIMARY);
		gpio_request_mux(MX27_PIN_CSPI2_SS0, GPIO_MUX_PRIMARY);
		//gpio_request_mux(MX27_PIN_CSPI2_SS1, GPIO_MUX_PRIMARY);
		//gpio_request_mux(MX27_PIN_CSPI2_SS2, GPIO_MUX_PRIMARY);
		break;
	case 2:
		/*SPI3  */
		gpio_request_mux(MX27_PIN_SD1_D0, GPIO_MUX_ALT);
		gpio_request_mux(MX27_PIN_SD1_CMD, GPIO_MUX_ALT);
		gpio_request_mux(MX27_PIN_SD1_CLK, GPIO_MUX_ALT);
		gpio_request_mux(MX27_PIN_SD1_D3, GPIO_MUX_ALT);
		break;

	default:
		break;
	}
}

/*!
 * Setup GPIO for a CSPI device to be inactive
 *
 * @param  cspi_mod         a CSPI device
 */
void gpio_spi_inactive(int cspi_mod)
{
	switch (cspi_mod) {
	case 0:
		/* SPI1 */
		gpio_free_mux(MX27_PIN_CSPI1_MOSI);
		gpio_free_mux(MX27_PIN_CSPI1_MISO);
		gpio_free_mux(MX27_PIN_CSPI1_SCLK);
		gpio_free_mux(MX27_PIN_CSPI1_RDY);
		gpio_free_mux(MX27_PIN_CSPI1_SS0);
//		gpio_free_mux(MX27_PIN_CSPI1_SS1);
		gpio_free_mux(MX27_PIN_CSPI1_SS2);
		break;
	case 1:
		/*SPI2  */
		gpio_free_mux(MX27_PIN_CSPI2_MOSI);
		gpio_free_mux(MX27_PIN_CSPI2_MISO);
		gpio_free_mux(MX27_PIN_CSPI2_SCLK);
		gpio_free_mux(MX27_PIN_CSPI2_SS0);
		//gpio_free_mux(MX27_PIN_CSPI2_SS1);
		//gpio_free_mux(MX27_PIN_CSPI2_SS2);
		break;
	case 2:
		/*SPI3  */
		gpio_free_mux(MX27_PIN_SD1_D0);
		gpio_free_mux(MX27_PIN_SD1_CMD);
		gpio_free_mux(MX27_PIN_SD1_CLK);
		gpio_free_mux(MX27_PIN_SD1_D3);
		break;

	default:
		break;
	}
}

/*!
 * Setup GPIO for a nand flash device to be active
 *
 */
void gpio_nand_active(void)
{
	unsigned long reg;
	reg = __raw_readl(IO_ADDRESS(SYSCTRL_BASE_ADDR) + SYS_FMCR);
	reg &= ~(1 << 4);
	__raw_writel(reg, IO_ADDRESS(SYSCTRL_BASE_ADDR) + SYS_FMCR);

	gpio_request_mux(MX27_PIN_NFRB, GPIO_MUX_PRIMARY);
	gpio_request_mux(MX27_PIN_NFCE_B, GPIO_MUX_PRIMARY);
	gpio_request_mux(MX27_PIN_NFWP_B, GPIO_MUX_PRIMARY);
	gpio_request_mux(MX27_PIN_NFCLE, GPIO_MUX_PRIMARY);
	gpio_request_mux(MX27_PIN_NFALE, GPIO_MUX_PRIMARY);
	gpio_request_mux(MX27_PIN_NFRE_B, GPIO_MUX_PRIMARY);
	gpio_request_mux(MX27_PIN_NFWE_B, GPIO_MUX_PRIMARY);
}

/*!
 * Setup GPIO for a nand flash device to be inactive
 *
 */
void gpio_nand_inactive(void)
{
	gpio_free_mux(MX27_PIN_NFRB);
	gpio_free_mux(MX27_PIN_NFCE_B);
	gpio_free_mux(MX27_PIN_NFWP_B);
	gpio_free_mux(MX27_PIN_NFCLE);
	gpio_free_mux(MX27_PIN_NFALE);
	gpio_free_mux(MX27_PIN_NFRE_B);
	gpio_free_mux(MX27_PIN_NFWE_B);
}

/*!
 * Setup GPIO for CSI device to be active
 *
 */
void gpio_sensor_active(void)
{
	gpio_request_mux(MX27_PIN_CSI_D0, GPIO_MUX_PRIMARY);
	gpio_request_mux(MX27_PIN_CSI_D1, GPIO_MUX_PRIMARY);
	gpio_request_mux(MX27_PIN_CSI_D2, GPIO_MUX_PRIMARY);
	gpio_request_mux(MX27_PIN_CSI_D3, GPIO_MUX_PRIMARY);
	gpio_request_mux(MX27_PIN_CSI_D4, GPIO_MUX_PRIMARY);
	gpio_request_mux(MX27_PIN_CSI_MCLK, GPIO_MUX_PRIMARY);
	gpio_request_mux(MX27_PIN_CSI_PIXCLK, GPIO_MUX_PRIMARY);
	gpio_request_mux(MX27_PIN_CSI_D5, GPIO_MUX_PRIMARY);
	gpio_request_mux(MX27_PIN_CSI_D6, GPIO_MUX_PRIMARY);
	gpio_request_mux(MX27_PIN_CSI_D7, GPIO_MUX_PRIMARY);
	gpio_request_mux(MX27_PIN_CSI_VSYNC, GPIO_MUX_PRIMARY);
	gpio_request_mux(MX27_PIN_CSI_HSYNC, GPIO_MUX_PRIMARY);
/*
	mdelay(10);
	gpio_request_mux(MX27_PIN_CSPI2_SS2, GPIO_MUX_GPIO);
	mxc_set_gpio_direction(MX27_PIN_CSPI2_SS2, 0);
	mxc_set_gpio_dataout(MX27_PIN_CSPI2_SS2, 0);
	mdelay(10);
#ifdef CONFIG_MACH_MX27_MDK27V1
	gpio_request_mux(MX27_PIN_I2C2_SCL, GPIO_MUX_GPIO);
	mxc_set_gpio_direction(MX27_PIN_I2C2_SCL, 0);
	mxc_set_gpio_dataout(MX27_PIN_I2C2_SCL, 0);
	mdelay(10);
	mxc_set_gpio_dataout(MX27_PIN_I2C2_SCL, 1);
	mdelay(10);
#else
	gpio_request_mux(MX27_PIN_CSPI2_SS1, GPIO_MUX_GPIO);
	mxc_set_gpio_direction(MX27_PIN_CSPI2_SS1, 0);
	mxc_set_gpio_dataout(MX27_PIN_CSPI2_SS1, 0);
	mdelay(100);
	mxc_set_gpio_dataout(MX27_PIN_CSPI2_SS1, 1);
#endif
*/
    /* tvp5150 reset pin */
    gpio_request_mux(MX27_PIN_USBH1_SUSP, GPIO_MUX_GPIO);
    mxc_set_gpio_direction(MX27_PIN_USBH1_SUSP, 0);
    mxc_set_gpio_dataout(MX27_PIN_USBH1_SUSP, 1);

    gpio_request_mux(MX27_PIN_SSI3_FS, GPIO_MUX_GPIO);
	mxc_set_gpio_direction(MX27_PIN_SSI3_FS, 0);
	mxc_set_gpio_dataout(MX27_PIN_SSI3_FS, 1);

    gpio_request_mux(MX27_PIN_USBH1_RXDM, GPIO_MUX_GPIO);
	mxc_set_gpio_direction(MX27_PIN_USBH1_RXDM, 0);
	
	/* cmos: must be zero, or else i2c probe will fail. */
	mxc_set_gpio_dataout(MX27_PIN_USBH1_RXDM, 0);
	
	/* reset tvp5150 */
	mxc_set_gpio_dataout(MX27_PIN_USBH1_SUSP, 0);
	udelay(100);
	mxc_set_gpio_dataout(MX27_PIN_USBH1_SUSP, 1);
}

void gpio_sensor_inactive(void)
{
	gpio_free_mux(MX27_PIN_CSI_D0);
	gpio_free_mux(MX27_PIN_CSI_D1);
	gpio_free_mux(MX27_PIN_CSI_D2);
	gpio_free_mux(MX27_PIN_CSI_D3);
	gpio_free_mux(MX27_PIN_CSI_D4);
	gpio_free_mux(MX27_PIN_CSI_MCLK);
	gpio_free_mux(MX27_PIN_CSI_PIXCLK);
	gpio_free_mux(MX27_PIN_CSI_D5);
	gpio_free_mux(MX27_PIN_CSI_D6);
	gpio_free_mux(MX27_PIN_CSI_D7);
	gpio_free_mux(MX27_PIN_CSI_VSYNC);
	gpio_free_mux(MX27_PIN_CSI_HSYNC);
/*
#ifdef CONFIG_MACH_MX27_MDK27V1
	gpio_free_mux(MX27_PIN_I2C2_SCL);
#else
	gpio_free_mux(MX27_PIN_CSPI2_SS1);
#endif
	gpio_free_mux(MX27_PIN_CSPI2_SS2);
*/
    gpio_free_mux(MX27_PIN_SSI3_FS);
    mxc_set_gpio_dataout(MX27_PIN_USBH1_RXDM, 1);
    gpio_free_mux(MX27_PIN_USBH1_RXDM);
    gpio_free_mux(MX27_PIN_USBH1_SUSP);
}

/*
void mxc_tvp5150_reset(void)
{
	mxc_set_gpio_dataout(MX27_PIN_USBH1_SUSP, 0);
	udelay(100);
	mxc_set_gpio_dataout(MX27_PIN_USBH1_SUSP, 1);
}
*/

/*!
 * Setup GPIO for CSI and TVP5150 device to be active
 *
 */
 /*
void gpio_tvp5150_active(void)
{
	gpio_request_mux(MX27_PIN_CSI_D0, GPIO_MUX_PRIMARY);
	gpio_request_mux(MX27_PIN_CSI_D1, GPIO_MUX_PRIMARY);
	gpio_request_mux(MX27_PIN_CSI_D2, GPIO_MUX_PRIMARY);
	gpio_request_mux(MX27_PIN_CSI_D3, GPIO_MUX_PRIMARY);
	gpio_request_mux(MX27_PIN_CSI_D4, GPIO_MUX_PRIMARY);
	gpio_request_mux(MX27_PIN_CSI_MCLK, GPIO_MUX_PRIMARY);
	gpio_request_mux(MX27_PIN_CSI_PIXCLK, GPIO_MUX_PRIMARY);
	gpio_request_mux(MX27_PIN_CSI_D5, GPIO_MUX_PRIMARY);
	gpio_request_mux(MX27_PIN_CSI_D6, GPIO_MUX_PRIMARY);
	gpio_request_mux(MX27_PIN_CSI_D7, GPIO_MUX_PRIMARY);
	gpio_request_mux(MX27_PIN_CSI_VSYNC, GPIO_MUX_PRIMARY);
	gpio_request_mux(MX27_PIN_CSI_HSYNC, GPIO_MUX_PRIMARY);
	
	// Power down control
	mdelay(10);
	gpio_request_mux(MX27_PIN_USBH1_RXDM, GPIO_MUX_GPIO);
	mxc_set_gpio_direction(MX27_PIN_USBH1_RXDM, 0);
	mxc_set_gpio_dataout(MX27_PIN_USBH1_RXDM, 1);

	// Reset
	mdelay(10);
	gpio_request_mux(MX27_PIN_CSPI1_RDY, GPIO_MUX_GPIO);
	mxc_set_gpio_direction(MX27_PIN_CSPI1_RDY, 0);
	mxc_set_gpio_dataout(MX27_PIN_CSPI1_RDY, 0);
	mdelay(10);
	mxc_set_gpio_dataout(MX27_PIN_CSPI1_RDY, 1);
	mdelay(10);

#if defined(CONFIG_MACH_MX27_SYP) || defined(CONFIG_MACH_MX27_TUOSI) || defined(CONFIG_MACH_MX27_MPK271)
#else
	gpio_request_mux(MX27_PIN_SSI2_TXDAT, GPIO_MUX_GPIO);
	mxc_set_gpio_direction(MX27_PIN_SSI2_TXDAT, 0);
	mxc_set_gpio_dataout(MX27_PIN_SSI2_TXDAT, 1);
#endif

    gpio_request_mux(MX27_PIN_SSI3_FS, GPIO_MUX_GPIO);
	mxc_set_gpio_direction(MX27_PIN_SSI3_FS, 0);
}

void gpio_tvp5150_inactive(void)
{
	gpio_free_mux(MX27_PIN_CSI_D0);
	gpio_free_mux(MX27_PIN_CSI_D1);
	gpio_free_mux(MX27_PIN_CSI_D2);
	gpio_free_mux(MX27_PIN_CSI_D3);
	gpio_free_mux(MX27_PIN_CSI_D4);
	gpio_free_mux(MX27_PIN_CSI_MCLK);
	gpio_free_mux(MX27_PIN_CSI_PIXCLK);
	gpio_free_mux(MX27_PIN_CSI_D5);
	gpio_free_mux(MX27_PIN_CSI_D6);
	gpio_free_mux(MX27_PIN_CSI_D7);
	gpio_free_mux(MX27_PIN_CSI_VSYNC);
	gpio_free_mux(MX27_PIN_CSI_HSYNC);

	// Power down
	mdelay(10);
	gpio_request_mux(MX27_PIN_USBH1_RXDM, GPIO_MUX_GPIO);
	mxc_set_gpio_direction(MX27_PIN_USBH1_RXDM, 0);
	mxc_set_gpio_dataout(MX27_PIN_USBH1_RXDM, 0);

	// Reset
	mdelay(10);
	gpio_request_mux(MX27_PIN_CSPI1_RDY, GPIO_MUX_GPIO);
	mxc_set_gpio_direction(MX27_PIN_CSPI1_RDY, 0);
	mxc_set_gpio_dataout(MX27_PIN_CSPI1_RDY, 0);
	mdelay(10);
	mxc_set_gpio_dataout(MX27_PIN_CSPI1_RDY, 1);
	mdelay(10);

	// new add on 2012.04.10
	gpio_free_mux(MX27_PIN_SSI3_FS);
}
*/

void mxc_ccd_enable(int i)
{
	mxc_set_gpio_dataout(MX27_PIN_SSI3_FS, ((i&0x01)^1));
}

void mxc_cmos_enable(int i)
{
	mxc_set_gpio_dataout(MX27_PIN_USBH1_RXDM, 1-(i&0x01));
}

void mxc_camera_select(int i)
{
	if (i == 0) {
		mxc_set_gpio_dataout(MX27_PIN_SSI3_FS, 1);
		mxc_set_gpio_dataout(MX27_PIN_USBH1_RXDM, 0);
	} else {
		mxc_set_gpio_dataout(MX27_PIN_USBH1_RXDM, 1);
		mxc_set_gpio_dataout(MX27_PIN_SSI3_FS, 0);
	}
}

/*!
 * Setup GPIO for CSI and TVP5150 device to be active
 *
 */
void gpio_adv7180_active(void)
{
	gpio_request_mux(MX27_PIN_CSI_D0, GPIO_MUX_PRIMARY);
	gpio_request_mux(MX27_PIN_CSI_D1, GPIO_MUX_PRIMARY);
	gpio_request_mux(MX27_PIN_CSI_D2, GPIO_MUX_PRIMARY);
	gpio_request_mux(MX27_PIN_CSI_D3, GPIO_MUX_PRIMARY);
	gpio_request_mux(MX27_PIN_CSI_D4, GPIO_MUX_PRIMARY);
	gpio_request_mux(MX27_PIN_CSI_MCLK, GPIO_MUX_PRIMARY);
	gpio_request_mux(MX27_PIN_CSI_PIXCLK, GPIO_MUX_PRIMARY);
	gpio_request_mux(MX27_PIN_CSI_D5, GPIO_MUX_PRIMARY);
	gpio_request_mux(MX27_PIN_CSI_D6, GPIO_MUX_PRIMARY);
	gpio_request_mux(MX27_PIN_CSI_D7, GPIO_MUX_PRIMARY);
	gpio_request_mux(MX27_PIN_CSI_VSYNC, GPIO_MUX_PRIMARY);
	gpio_request_mux(MX27_PIN_CSI_HSYNC, GPIO_MUX_PRIMARY);
	
	/* Power down control */
	mdelay(10);
	gpio_request_mux(MX27_PIN_USBH1_RXDM, GPIO_MUX_GPIO);
	mxc_set_gpio_direction(MX27_PIN_USBH1_RXDM, 0);
	mxc_set_gpio_dataout(MX27_PIN_USBH1_RXDM, 1);

	/* Reset */
	mdelay(10);
	gpio_request_mux(MX27_PIN_CSPI1_RDY, GPIO_MUX_GPIO);
	mxc_set_gpio_direction(MX27_PIN_CSPI1_RDY, 0);
	mxc_set_gpio_dataout(MX27_PIN_CSPI1_RDY, 0);
	mdelay(10);
	mxc_set_gpio_dataout(MX27_PIN_CSPI1_RDY, 1);
	mdelay(10);
}

void gpio_adv7180_inactive(void)
{
	gpio_free_mux(MX27_PIN_CSI_D0);
	gpio_free_mux(MX27_PIN_CSI_D1);
	gpio_free_mux(MX27_PIN_CSI_D2);
	gpio_free_mux(MX27_PIN_CSI_D3);
	gpio_free_mux(MX27_PIN_CSI_D4);
	gpio_free_mux(MX27_PIN_CSI_MCLK);
	gpio_free_mux(MX27_PIN_CSI_PIXCLK);
	gpio_free_mux(MX27_PIN_CSI_D5);
	gpio_free_mux(MX27_PIN_CSI_D6);
	gpio_free_mux(MX27_PIN_CSI_D7);
	gpio_free_mux(MX27_PIN_CSI_VSYNC);
	gpio_free_mux(MX27_PIN_CSI_HSYNC);

	/* Power down control */
	mdelay(10);
	gpio_request_mux(MX27_PIN_USBH1_RXDM, GPIO_MUX_GPIO);
	mxc_set_gpio_direction(MX27_PIN_USBH1_RXDM, 0);
	mxc_set_gpio_dataout(MX27_PIN_USBH1_RXDM, 0);

	/* Reset */
	mdelay(10);
	gpio_request_mux(MX27_PIN_CSPI1_RDY, GPIO_MUX_GPIO);
	mxc_set_gpio_direction(MX27_PIN_CSPI1_RDY, 0);
	mxc_set_gpio_dataout(MX27_PIN_CSPI1_RDY, 0);
	mdelay(10);
	mxc_set_gpio_dataout(MX27_PIN_CSPI1_RDY, 1);
	mdelay(10);
}

/*!
 * Setup GPIO for CSI and TW2835 device to be active
 *
 */
void gpio_tw2835_active(void)
{
	gpio_request_mux(MX27_PIN_CSI_D0, GPIO_MUX_PRIMARY);
	gpio_request_mux(MX27_PIN_CSI_D1, GPIO_MUX_PRIMARY);
	gpio_request_mux(MX27_PIN_CSI_D2, GPIO_MUX_PRIMARY);
	gpio_request_mux(MX27_PIN_CSI_D3, GPIO_MUX_PRIMARY);
	gpio_request_mux(MX27_PIN_CSI_D4, GPIO_MUX_PRIMARY);
	gpio_request_mux(MX27_PIN_CSI_MCLK, GPIO_MUX_PRIMARY);
	gpio_request_mux(MX27_PIN_CSI_PIXCLK, GPIO_MUX_PRIMARY);
	gpio_request_mux(MX27_PIN_CSI_D5, GPIO_MUX_PRIMARY);
	gpio_request_mux(MX27_PIN_CSI_D6, GPIO_MUX_PRIMARY);
	gpio_request_mux(MX27_PIN_CSI_D7, GPIO_MUX_PRIMARY);
	gpio_request_mux(MX27_PIN_CSI_VSYNC, GPIO_MUX_PRIMARY);
	gpio_request_mux(MX27_PIN_CSI_HSYNC, GPIO_MUX_PRIMARY);

#ifdef CONFIG_MACH_MX27_MPK271
	/* Power down control */
	mdelay(10);
	gpio_request_mux(MX27_PIN_USBH1_RXDM, GPIO_MUX_GPIO);
	mxc_set_gpio_direction(MX27_PIN_USBH1_RXDM, 0);
	mxc_set_gpio_dataout(MX27_PIN_USBH1_RXDM, 1);

	/* Reset */
	mdelay(10);
	gpio_request_mux(MX27_PIN_CSPI1_RDY, GPIO_MUX_GPIO);
	mxc_set_gpio_direction(MX27_PIN_CSPI1_RDY, 0);
	mxc_set_gpio_dataout(MX27_PIN_CSPI1_RDY, 0);
	mdelay(10);
	mxc_set_gpio_dataout(MX27_PIN_CSPI1_RDY, 1);
	mdelay(10);
#endif
}

void gpio_tw2835_inactive(void)
{
	gpio_free_mux(MX27_PIN_CSI_D0);
	gpio_free_mux(MX27_PIN_CSI_D1);
	gpio_free_mux(MX27_PIN_CSI_D2);
	gpio_free_mux(MX27_PIN_CSI_D3);
	gpio_free_mux(MX27_PIN_CSI_D4);
	gpio_free_mux(MX27_PIN_CSI_MCLK);
	gpio_free_mux(MX27_PIN_CSI_PIXCLK);
	gpio_free_mux(MX27_PIN_CSI_D5);
	gpio_free_mux(MX27_PIN_CSI_D6);
	gpio_free_mux(MX27_PIN_CSI_D7);
	gpio_free_mux(MX27_PIN_CSI_VSYNC);
	gpio_free_mux(MX27_PIN_CSI_HSYNC);

#ifdef CONFIG_MACH_MX27_MPK271
	/* Power down control */
	mdelay(10);
	gpio_request_mux(MX27_PIN_USBH1_RXDM, GPIO_MUX_GPIO);
	mxc_set_gpio_direction(MX27_PIN_USBH1_RXDM, 0);
	mxc_set_gpio_dataout(MX27_PIN_USBH1_RXDM, 0);

	/* Reset */
	mdelay(10);
	gpio_request_mux(MX27_PIN_CSPI1_RDY, GPIO_MUX_GPIO);
	mxc_set_gpio_direction(MX27_PIN_CSPI1_RDY, 0);
	mxc_set_gpio_dataout(MX27_PIN_CSPI1_RDY, 0);
	mdelay(10);
	mxc_set_gpio_dataout(MX27_PIN_CSPI1_RDY, 1);
	mdelay(10);
#endif
}

#if 0
/*!
 * Setup GPIO for WM9712
 *
 */
void gpio_wm9712_ts_active(void)
{
	/* GPIO_AC97_PENDOWN, input gpio interrupt, low -> high, rising edge */
#ifdef CONFIG_MACH_MX27_SYP
	gpio_request_mux(MX27_PIN_CSPI1_MISO, GPIO_MUX_GPIO);
	mxc_set_gpio_direction(MX27_PIN_CSPI1_MISO, 1);
	set_irq_type(IOMUX_TO_IRQ(MX27_PIN_CSPI1_MISO), IRQT_RISING);
#elif defined(CONFIG_MACH_MX27_TUOSI)
	gpio_request_mux(MX27_PIN_CSPI2_SS0, GPIO_MUX_GPIO);
	mxc_set_gpio_direction(MX27_PIN_CSPI2_SS0, 1);
	set_irq_type(IOMUX_TO_IRQ(MX27_PIN_CSPI2_SS0), IRQT_RISING);
#elif defined(CONFIG_MACH_MX27_MPK271)
	gpio_request_mux(MX27_PIN_SSI4_RXDAT, GPIO_MUX_GPIO);
	mxc_set_gpio_direction(MX27_PIN_SSI4_RXDAT, 1);
	set_irq_type(IOMUX_TO_IRQ(MX27_PIN_SSI4_RXDAT), IRQT_RISING);
#endif

	/* for reset */
	gpio_request_mux(MX27_PIN_SSI2_RXDAT, GPIO_MUX_GPIO);
	mxc_set_gpio_direction(MX27_PIN_SSI2_RXDAT, 0);
	mxc_set_gpio_dataout(MX27_PIN_SSI2_RXDAT, 0);

	gpio_request_mux(MX27_PIN_SSI2_TXDAT, GPIO_MUX_GPIO);
	mxc_set_gpio_direction(MX27_PIN_SSI2_TXDAT, 0);
	mxc_set_gpio_dataout(MX27_PIN_SSI2_TXDAT, 0);

	gpio_request_mux(MX27_PIN_SSI2_CLK, GPIO_MUX_GPIO);
	mxc_set_gpio_direction(MX27_PIN_SSI2_CLK, 0);
	mxc_set_gpio_dataout(MX27_PIN_SSI2_CLK, 0);

	gpio_request_mux(MX27_PIN_SSI2_FS, GPIO_MUX_GPIO);
	mxc_set_gpio_direction(MX27_PIN_SSI2_FS, 0);
	mxc_set_gpio_dataout(MX27_PIN_SSI2_FS, 0);

	/* reset now */
#ifdef CONFIG_MACH_MX27_SYP
	gpio_request_mux(MX27_PIN_CSPI1_MOSI, GPIO_MUX_GPIO);
	mxc_set_gpio_direction(MX27_PIN_CSPI1_MOSI, 0);
	mxc_set_gpio_dataout(MX27_PIN_CSPI1_MOSI, 0);	/* reset active */
	udelay(999);
	mxc_set_gpio_dataout(MX27_PIN_CSPI1_MOSI, 1);	/* reset inactive */
#elif defined(CONFIG_MACH_MX27_TUOSI)
	gpio_request_mux(MX27_PIN_CSPI2_SS1, GPIO_MUX_GPIO);
	mxc_set_gpio_direction(MX27_PIN_CSPI2_SS1, 0);
	mxc_set_gpio_dataout(MX27_PIN_CSPI2_SS1, 0);	/* reset active */
	udelay(999);
	mxc_set_gpio_dataout(MX27_PIN_CSPI2_SS1, 1);	/* reset inactive */
#elif defined(CONFIG_MACH_MX27_MPK271)
	gpio_request_mux(MX27_PIN_SSI4_TXDAT, GPIO_MUX_GPIO);
	mxc_set_gpio_direction(MX27_PIN_SSI4_TXDAT, 0);
	mxc_set_gpio_dataout(MX27_PIN_SSI4_TXDAT, 0);	/* reset active */
	udelay(999);
	mxc_set_gpio_dataout(MX27_PIN_SSI4_TXDAT, 1);	/* reset inactive */
#endif

	gpio_free_mux(MX27_PIN_SSI2_FS); 
	gpio_free_mux(MX27_PIN_SSI2_RXDAT);
	gpio_free_mux(MX27_PIN_SSI2_TXDAT);
	gpio_free_mux(MX27_PIN_SSI2_CLK);
}

void gpio_wm9712_ts_inactive(void)
{
#ifdef CONFIG_MACH_MX27_SYP
	gpio_free_mux(MX27_PIN_CSPI1_MISO);
	/* gpio_free_mux(MX27_PIN_SSI4_FS); This GPIO is not used for now */
	gpio_free_mux(MX27_PIN_CSPI1_MOSI);
#elif defined(CONFIG_MACH_MX27_TUOSI)
	gpio_free_mux(MX27_PIN_CSPI2_SS0);
	gpio_free_mux(MX27_PIN_CSPI2_SS1);
#elif defined(CONFIG_MACH_MX27_MPK271)
	gpio_free_mux(MX27_PIN_SSI4_RXDAT);
	gpio_free_mux(MX27_PIN_SSI4_TXDAT);
#endif
//	gpio_free_mux(MX27_PIN_CSPI1_SCLK);
}
#endif

void gpio_si4702_active(void)
{
	/* FM CLK enable (PA1) */
	gpio_request_mux(MX27_PIN_USBH2_DIR, GPIO_MUX_GPIO);
	mxc_set_gpio_direction(MX27_PIN_USBH2_DIR, 0);
	mxc_set_gpio_dataout(MX27_PIN_USBH2_DIR, 1);

#if defined(CONFIG_MACH_MX27_SYP) || defined(CONFIG_MACH_MX27_TUOSI) || defined(CONFIG_MACH_MX27_MPK271)
#else
	/* Set PC26 to low to select si4702 as audio line in */
	gpio_request_mux(MX27_PIN_SSI2_TXDAT, GPIO_MUX_GPIO);
	mxc_set_gpio_direction(MX27_PIN_SSI2_TXDAT, 0);
	mxc_set_gpio_dataout(MX27_PIN_SSI2_TXDAT, 0);
#endif
}

void gpio_si4702_inactive(void)
{
	/* FM CLK disable (PA1) */
	gpio_request_mux(MX27_PIN_USBH2_DIR, GPIO_MUX_GPIO);
	mxc_set_gpio_direction(MX27_PIN_USBH2_DIR, 0);
	mxc_set_gpio_dataout(MX27_PIN_USBH2_DIR, 0);
}

void gpio_si4702_reset(void)
{
	/* Reset (PD20) */
	gpio_request_mux(MX27_PIN_CSPI2_SS1, GPIO_MUX_GPIO);
	mxc_set_gpio_direction(MX27_PIN_CSPI2_SS1, 0);
	mxc_set_gpio_dataout(MX27_PIN_CSPI2_SS1, 0);
	mdelay(100);
	mxc_set_gpio_dataout(MX27_PIN_CSPI2_SS1, 1);
	mdelay(100);
}

void gpio_si4702_powerdown(void)
{
	/* FM CLK disable (PA1) */
	gpio_request_mux(MX27_PIN_USBH2_DIR, GPIO_MUX_GPIO);
	mxc_set_gpio_direction(MX27_PIN_USBH2_DIR, 0);
	mxc_set_gpio_dataout(MX27_PIN_USBH2_DIR, 0);
}

/*!
 * Setup GPIO for LCDC device to be active
 *
 */
void gpio_lcdc_active(void)
{
	gpio_request_mux(MX27_PIN_LSCLK, GPIO_MUX_PRIMARY);
	gpio_request_mux(MX27_PIN_LD0, GPIO_MUX_PRIMARY);
	gpio_request_mux(MX27_PIN_LD1, GPIO_MUX_PRIMARY);
	gpio_request_mux(MX27_PIN_LD2, GPIO_MUX_PRIMARY);
	gpio_request_mux(MX27_PIN_LD3, GPIO_MUX_PRIMARY);
	gpio_request_mux(MX27_PIN_LD4, GPIO_MUX_PRIMARY);
	gpio_request_mux(MX27_PIN_LD5, GPIO_MUX_PRIMARY);
	gpio_request_mux(MX27_PIN_LD6, GPIO_MUX_PRIMARY);
	gpio_request_mux(MX27_PIN_LD7, GPIO_MUX_PRIMARY);
	gpio_request_mux(MX27_PIN_LD8, GPIO_MUX_PRIMARY);
	gpio_request_mux(MX27_PIN_LD9, GPIO_MUX_PRIMARY);
	gpio_request_mux(MX27_PIN_LD10, GPIO_MUX_PRIMARY);
	gpio_request_mux(MX27_PIN_LD11, GPIO_MUX_PRIMARY);
	gpio_request_mux(MX27_PIN_LD12, GPIO_MUX_PRIMARY);
	gpio_request_mux(MX27_PIN_LD13, GPIO_MUX_PRIMARY);
	gpio_request_mux(MX27_PIN_LD14, GPIO_MUX_PRIMARY);
	gpio_request_mux(MX27_PIN_LD15, GPIO_MUX_PRIMARY);
	gpio_request_mux(MX27_PIN_LD16, GPIO_MUX_PRIMARY);
	gpio_request_mux(MX27_PIN_LD17, GPIO_MUX_PRIMARY);
	gpio_request_mux(MX27_PIN_HSYNC, GPIO_MUX_PRIMARY);
	gpio_request_mux(MX27_PIN_VSYNC, GPIO_MUX_PRIMARY);
	gpio_request_mux(MX27_PIN_CONTRAST, GPIO_MUX_PRIMARY); //pwm output
	gpio_request_mux(MX27_PIN_OE_ACD, GPIO_MUX_PRIMARY);
#ifdef CONFIG_MACH_MX27_TUOSI
	if (gpio_request_mux(MX27_PIN_USBH2_DIR, GPIO_MUX_GPIO)) {
		printk("bug: request GPIO PA1 failed.\n");
		return;
	}
	mxc_set_gpio_direction(MX27_PIN_USBH2_DIR, 0);
	gpio_config_mux(MX27_PIN_USBH2_DIR, GPIO_MUX_GPIO);
	mxc_set_gpio_dataout(MX27_PIN_USBH2_DIR, 0);
	mdelay(10);
	mxc_set_gpio_dataout(MX27_PIN_USBH2_DIR, 1);
#else
#ifdef CONFIG_MACH_MX27_MPK271
#elif defined(CONFIG_MACH_MX27_SYP)
#else
	if (gpio_request_mux(MX27_PIN_USBH2_NXT, GPIO_MUX_GPIO)) {
		printk("bug: request GPIO PA3 failed.\n");
		return;
	}
	mxc_set_gpio_direction(MX27_PIN_USBH2_NXT, 0);
	gpio_config_mux(MX27_PIN_USBH2_NXT, GPIO_MUX_GPIO);
	mxc_set_gpio_dataout(MX27_PIN_USBH2_NXT, 0);
	mdelay(10);
	mxc_set_gpio_dataout(MX27_PIN_USBH2_NXT, 1);
#endif	/* CONFIG_MACH_MX27_MPK271 */
#endif
#if defined(CONFIG_MACH_MX27_SYP) || defined(CONFIG_MACH_MX27_TUOSI) || defined(CONFIG_MACH_MX27_MPK271)
	/* Set PA24 as output high to turn on LCD */
	if (gpio_request_mux(MX27_PIN_REV, GPIO_MUX_GPIO)) {
		printk("warning: request GPIO PA24 failed.\n");
	}
	mxc_set_gpio_direction(MX27_PIN_REV, 0);
	gpio_config_mux(MX27_PIN_REV, GPIO_MUX_GPIO);
	mxc_set_gpio_dataout(MX27_PIN_REV, 0); //should be low level. on 2012.12.07.
#elif defined(CONFIG_MACH_MX27_MDK27V1)
	/* Set PA24 as input to turn on LCD */
	if (gpio_request_mux(MX27_PIN_REV, GPIO_MUX_GPIO)) {
		printk("warning: request GPIO PA24 failed.\n");
	}
	mxc_set_gpio_direction(MX27_PIN_REV, 1);
	gpio_config_mux(MX27_PIN_REV, GPIO_MUX_GPIO);
#endif
}

/*!
 * Setup GPIO for LCDC device to be inactive
 *
 */
void gpio_lcdc_inactive(void)
{
#if 0
#ifdef CONFIG_MACH_MX27_MDK27V1
	/* Set PA24 to output 0 to turn off LCD */
	if (gpio_request_mux(MX27_PIN_REV, GPIO_MUX_GPIO)) {
		printk("warning: request GPIO PA24 failed.\n");
	}
	mxc_set_gpio_direction(MX27_PIN_REV, 0);
	gpio_config_mux(MX27_PIN_REV, GPIO_MUX_GPIO);
	mxc_set_gpio_dataout(MX27_PIN_REV, 1);
#endif
#endif
	/* new add on 2012.12.07 */
	mxc_set_gpio_dataout(MX27_PIN_REV, 1);
	gpio_free_mux(MX27_PIN_REV);
	////////////////////////////////

	gpio_free_mux(MX27_PIN_LSCLK);
	gpio_free_mux(MX27_PIN_LD0);
	gpio_free_mux(MX27_PIN_LD1);
	gpio_free_mux(MX27_PIN_LD2);
	gpio_free_mux(MX27_PIN_LD3);
	gpio_free_mux(MX27_PIN_LD4);
	gpio_free_mux(MX27_PIN_LD5);
	gpio_free_mux(MX27_PIN_LD6);
	gpio_free_mux(MX27_PIN_LD7);
	gpio_free_mux(MX27_PIN_LD8);
	gpio_free_mux(MX27_PIN_LD9);
	gpio_free_mux(MX27_PIN_LD10);
	gpio_free_mux(MX27_PIN_LD11);
	gpio_free_mux(MX27_PIN_LD12);
	gpio_free_mux(MX27_PIN_LD13);
	gpio_free_mux(MX27_PIN_LD14);
	gpio_free_mux(MX27_PIN_LD15);
	gpio_free_mux(MX27_PIN_LD16);
	gpio_free_mux(MX27_PIN_LD17);
	gpio_free_mux(MX27_PIN_HSYNC);
	gpio_free_mux(MX27_PIN_VSYNC);
	gpio_free_mux(MX27_PIN_CONTRAST);
	gpio_free_mux(MX27_PIN_OE_ACD);
//	gpio_free_mux(MX27_PIN_USBH2_NXT);
}

/*!
 * Setup GPIO PA25 low to start hard reset FS453 TV encoder
 *
 */
void gpio_fs453_reset_low(void)
{
	gpio_free_mux(MX27_PIN_CLS);
	if (gpio_request_mux(MX27_PIN_CLS, GPIO_MUX_GPIO)) {
		printk(KERN_ERR "bug: request GPIO PA25 failed.\n");
		return;
	}

	/* PA25 (CLS) as output */
	mxc_set_gpio_direction(MX27_PIN_CLS, 0);
	gpio_config_mux(MX27_PIN_CLS, GPIO_MUX_GPIO);
	mxc_set_gpio_dataout(MX27_PIN_CLS, 0);
}

/*!
 * Setup GPIO PA25 high to end hard reset FS453 TV encoder
 *
 */
void gpio_fs453_reset_high(void)
{
	gpio_free_mux(MX27_PIN_CLS);
	if (gpio_request_mux(MX27_PIN_CLS, GPIO_MUX_GPIO)) {
		printk(KERN_ERR "bug: request GPIO PA25 failed.\n");
		return;
	}

	/* PA25 (CLS) as output */
	mxc_set_gpio_direction(MX27_PIN_CLS, 0);
	gpio_config_mux(MX27_PIN_CLS, GPIO_MUX_GPIO);
	mxc_set_gpio_dataout(MX27_PIN_CLS, 1);
}

/*!
 * This function configures the IOMux block for PMIC standard operations.
 *
 */
void gpio_pmic_active(void)
{
	gpio_config_mux(MX27_PIN_TOUT, GPIO_MUX_GPIO);
	mxc_set_gpio_direction(MX27_PIN_TOUT, 1);
	gpio_config_mux(MX27_PIN_TIN, GPIO_MUX_GPIO);
	mxc_set_gpio_direction(MX27_PIN_TIN, 1);
	gpio_config_mux(MX27_PIN_USBH1_TXDP, GPIO_MUX_GPIO);
	mxc_set_gpio_direction(MX27_PIN_USBH1_TXDP, 1);
#if 0
	gpio_config_mux(MX27_PIN_SSI3_TXDAT, GPIO_MUX_GPIO);	/* PC30,USER_OFF */
	mxc_set_gpio_direction(MX27_PIN_SSI3_TXDAT, 0);
	mxc_set_gpio_dataout(MX27_PIN_SSI3_TXDAT, 1);
#endif
	gpio_config_mux(MX27_PIN_USBH1_OE_B, GPIO_MUX_GPIO);	/* PB27, REGEN */
	mxc_set_gpio_direction(MX27_PIN_USBH1_OE_B, 0);
	mxc_set_gpio_dataout(MX27_PIN_USBH1_OE_B, 1);
	gpio_config_mux(MX27_PIN_USBH1_RXDP, GPIO_MUX_GPIO);	/* PB31, WDI */
	mxc_set_gpio_direction(MX27_PIN_USBH1_RXDP, 0);
	mxc_set_gpio_dataout(MX27_PIN_USBH1_RXDP, 1);
}

/*!
 * GPIO settings not required for keypad
 *
 */
void gpio_keypad_active(void)
{
}

/*!
 * GPIO settings not required for keypad
 *
 */
void gpio_keypad_inactive(void)
{
}

#if 0
/*!
 * Setup GPIO for ATA device to be active
 *
 */
void gpio_ata_active(void)
{
	gpio_request_mux(MX27_PIN_ATA_DATA0, GPIO_MUX_PRIMARY);
	gpio_request_mux(MX27_PIN_ATA_DATA1, GPIO_MUX_PRIMARY);
	gpio_request_mux(MX27_PIN_ATA_DATA2, GPIO_MUX_PRIMARY);
	gpio_request_mux(MX27_PIN_ATA_DATA3, GPIO_MUX_PRIMARY);
	gpio_request_mux(MX27_PIN_ATA_DATA4, GPIO_MUX_PRIMARY);
	gpio_request_mux(MX27_PIN_ATA_DATA5, GPIO_MUX_PRIMARY);
	gpio_request_mux(MX27_PIN_ATA_DATA6, GPIO_MUX_PRIMARY);
	gpio_request_mux(MX27_PIN_ATA_DATA7, GPIO_MUX_PRIMARY);
	gpio_request_mux(MX27_PIN_ATA_DATA8, GPIO_MUX_PRIMARY);
	gpio_request_mux(MX27_PIN_ATA_DATA9, GPIO_MUX_PRIMARY);
	gpio_request_mux(MX27_PIN_ATA_DATA10, GPIO_MUX_PRIMARY);
	gpio_request_mux(MX27_PIN_ATA_DATA11, GPIO_MUX_PRIMARY);
	gpio_request_mux(MX27_PIN_ATA_DATA12, GPIO_MUX_PRIMARY);
	gpio_request_mux(MX27_PIN_ATA_DATA13, GPIO_MUX_PRIMARY);
	gpio_request_mux(MX27_PIN_ATA_DATA14, GPIO_MUX_PRIMARY);
	gpio_request_mux(MX27_PIN_ATA_DATA15, GPIO_MUX_PRIMARY);

	gpio_request_mux(MX27_PIN_PC_CD1_B, GPIO_MUX_ALT);
	gpio_request_mux(MX27_PIN_PC_CD2_B, GPIO_MUX_ALT);
	gpio_request_mux(MX27_PIN_PC_WAIT_B, GPIO_MUX_ALT);
	gpio_request_mux(MX27_PIN_PC_READY, GPIO_MUX_ALT);
#if 0
	gpio_request_mux(MX27_PIN_PC_PWRON, GPIO_MUX_ALT);
	gpio_request_mux(MX27_PIN_PC_VS1, GPIO_MUX_ALT);
	gpio_request_mux(MX27_PIN_PC_VS2, GPIO_MUX_ALT);
	gpio_request_mux(MX27_PIN_PC_BVD1, GPIO_MUX_ALT);
	gpio_request_mux(MX27_PIN_PC_BVD2, GPIO_MUX_ALT);
#endif
	gpio_request_mux(MX27_PIN_PC_RST, GPIO_MUX_ALT);
	gpio_request_mux(MX27_PIN_IOIS16, GPIO_MUX_ALT);
	gpio_request_mux(MX27_PIN_PC_RW_B, GPIO_MUX_ALT);
	gpio_request_mux(MX27_PIN_PC_POE, GPIO_MUX_ALT);

	/*FEC_ENALBE&ATA_ENABLE */
	gpio_request_mux(MX27_PIN_USB_OC_B, GPIO_MUX_GPIO);
	gpio_request_mux(MX27_PIN_PWMO, GPIO_MUX_GPIO);
	mxc_set_gpio_direction(MX27_PIN_USB_OC_B, 0);
	mxc_set_gpio_direction(MX27_PIN_PWMO, 0);
	/*Set default is disabled */
	mxc_set_gpio_dataout(MX27_PIN_USB_OC_B, 0);
	mxc_set_gpio_dataout(MX27_PIN_PWMO, 1);

}

/*!
 * Setup GPIO for ATA device to be inactive
 *
 */
void gpio_ata_inactive(void)
{

	gpio_free_mux(MX27_PIN_USB_OC_B);
	gpio_free_mux(MX27_PIN_PWMO);

	gpio_free_mux(MX27_PIN_ATA_DATA0);
	gpio_free_mux(MX27_PIN_ATA_DATA1);
	gpio_free_mux(MX27_PIN_ATA_DATA2);
	gpio_free_mux(MX27_PIN_ATA_DATA3);
	gpio_free_mux(MX27_PIN_ATA_DATA4);
	gpio_free_mux(MX27_PIN_ATA_DATA5);
	gpio_free_mux(MX27_PIN_ATA_DATA6);
	gpio_free_mux(MX27_PIN_ATA_DATA7);
	gpio_free_mux(MX27_PIN_ATA_DATA8);
	gpio_free_mux(MX27_PIN_ATA_DATA9);
	gpio_free_mux(MX27_PIN_ATA_DATA10);
	gpio_free_mux(MX27_PIN_ATA_DATA11);
	gpio_free_mux(MX27_PIN_ATA_DATA12);
	gpio_free_mux(MX27_PIN_ATA_DATA13);
	gpio_free_mux(MX27_PIN_ATA_DATA14);
	gpio_free_mux(MX27_PIN_ATA_DATA15);

	gpio_free_mux(MX27_PIN_PC_CD1_B);
	gpio_free_mux(MX27_PIN_PC_CD2_B);
	gpio_free_mux(MX27_PIN_PC_WAIT_B);
	gpio_free_mux(MX27_PIN_PC_READY);
#if 0
	gpio_free_mux(MX27_PIN_PC_PWRON);
	gpio_free_mux(MX27_PIN_PC_VS1);
	gpio_free_mux(MX27_PIN_PC_VS2);
	gpio_free_mux(MX27_PIN_PC_BVD1);
	gpio_free_mux(MX27_PIN_PC_BVD2);
#endif
	gpio_free_mux(MX27_PIN_PC_RST);
	gpio_free_mux(MX27_PIN_IOIS16);
	gpio_free_mux(MX27_PIN_PC_RW_B);
	gpio_free_mux(MX27_PIN_PC_POE);
}
#endif

/*!
 * Setup GPIO for FEC device to be active
 *
 */
void gpio_fec_active(void)
{
	gpio_request_mux(MX27_PIN_ATA_DATA15, GPIO_MUX_OUTPUT1);
	mxc_set_gpio_direction(MX27_PIN_ATA_DATA15, 0);
	gpio_request_mux(MX27_PIN_ATA_DATA14, GPIO_MUX_OUTPUT1);
	mxc_set_gpio_direction(MX27_PIN_ATA_DATA14, 0);
	gpio_request_mux(MX27_PIN_ATA_DATA13, GPIO_MUX_INPUT1);
	mxc_set_gpio_direction(MX27_PIN_ATA_DATA13, 1);
	gpio_request_mux(MX27_PIN_ATA_DATA12, GPIO_MUX_INPUT1);
	mxc_set_gpio_direction(MX27_PIN_ATA_DATA12, 1);
	gpio_request_mux(MX27_PIN_ATA_DATA11, GPIO_MUX_INPUT1);
	mxc_set_gpio_direction(MX27_PIN_ATA_DATA11, 1);
	gpio_request_mux(MX27_PIN_ATA_DATA10, GPIO_MUX_INPUT1);
	mxc_set_gpio_direction(MX27_PIN_ATA_DATA10, 1);
	gpio_request_mux(MX27_PIN_ATA_DATA9, GPIO_MUX_INPUT1);
	mxc_set_gpio_direction(MX27_PIN_ATA_DATA9, 1);
	gpio_request_mux(MX27_PIN_ATA_DATA8, GPIO_MUX_INPUT1);
	mxc_set_gpio_direction(MX27_PIN_ATA_DATA8, 1);
	gpio_request_mux(MX27_PIN_ATA_DATA7, GPIO_MUX_OUTPUT1);
	mxc_set_gpio_direction(MX27_PIN_ATA_DATA7, 0);

	gpio_request_mux(MX27_PIN_ATA_DATA6, GPIO_MUX_ALT);
	gpio_request_mux(MX27_PIN_ATA_DATA5, GPIO_MUX_INPUT1);
	mxc_set_gpio_direction(MX27_PIN_ATA_DATA5, 1);
	gpio_request_mux(MX27_PIN_ATA_DATA4, GPIO_MUX_INPUT1);
	mxc_set_gpio_direction(MX27_PIN_ATA_DATA4, 1);
	gpio_request_mux(MX27_PIN_ATA_DATA3, GPIO_MUX_INPUT1);
	mxc_set_gpio_direction(MX27_PIN_ATA_DATA3, 1);
	gpio_request_mux(MX27_PIN_ATA_DATA2, GPIO_MUX_INPUT1);
	mxc_set_gpio_direction(MX27_PIN_ATA_DATA2, 1);
	gpio_request_mux(MX27_PIN_ATA_DATA1, GPIO_MUX_OUTPUT1);
	mxc_set_gpio_direction(MX27_PIN_ATA_DATA1, 0);
	gpio_request_mux(MX27_PIN_ATA_DATA0, GPIO_MUX_OUTPUT1);
	mxc_set_gpio_direction(MX27_PIN_ATA_DATA0, 0);
	gpio_request_mux(MX27_PIN_SD3_CLK, GPIO_MUX_OUTPUT1);
	mxc_set_gpio_direction(MX27_PIN_SD3_CLK, 0);
	gpio_request_mux(MX27_PIN_SD3_CMD, GPIO_MUX_OUTPUT1);
	mxc_set_gpio_direction(MX27_PIN_SD3_CMD, 0);

	mdelay(10);
#ifdef CONFIG_MACH_MX27_MDK27V1
	/* PB24 (FEC_enbale) high */
	if (gpio_request_mux(MX27_PIN_USB_OC_B, GPIO_MUX_GPIO)) {
		printk(KERN_ERR "bug: request GPIO PB24 failed.\n");
		return;
	}

	/* PB24 (MX27_PIN_USB_OC_B) as output */
	mxc_set_gpio_direction(MX27_PIN_USB_OC_B, 0);
	gpio_config_mux(MX27_PIN_USB_OC_B, GPIO_MUX_GPIO);
	mxc_set_gpio_dataout(MX27_PIN_USB_OC_B, 1);
	mdelay(10);


	/* PF10 (FEC_RST) */
	if (gpio_request_mux(MX27_PIN_PC_RST, GPIO_MUX_GPIO)) {
		printk(KERN_ERR "bug: request GPIO PF10 failed.\n");
		return;
	}

	/* PF10 (MX27_PIN_PC_RST) as output */
	mxc_set_gpio_direction(MX27_PIN_PC_RST, 0);
	gpio_config_mux(MX27_PIN_PC_RST, GPIO_MUX_GPIO);
	mxc_set_gpio_dataout(MX27_PIN_PC_RST, 0);
	mdelay(10);
	mxc_set_gpio_dataout(MX27_PIN_PC_RST, 1);
	mdelay(10);
#else
	/*FEC_LINK INTRQ */
	gpio_request_mux(MX27_PIN_IOIS16, GPIO_MUX_GPIO);
	mxc_set_gpio_direction(MX27_PIN_IOIS16, 1);

	/*FEC_ENALBE&ATA_ENABLE */
	gpio_request_mux(MX27_PIN_USB_OC_B, GPIO_MUX_GPIO);
	gpio_request_mux(MX27_PIN_PWMO, GPIO_MUX_GPIO);
	mxc_set_gpio_direction(MX27_PIN_USB_OC_B, 0);
	mxc_set_gpio_direction(MX27_PIN_PWMO, 0);
	/*Set default is disabled */
	mxc_set_gpio_dataout(MX27_PIN_USB_OC_B, 0);
	mxc_set_gpio_dataout(MX27_PIN_PWMO, 1);

	/*FEC_RESET */
	gpio_request_mux(MX27_PIN_PC_RST, GPIO_MUX_GPIO);
	mxc_set_gpio_direction(MX27_PIN_PC_RST, 0);
	mxc_set_gpio_dataout(MX27_PIN_PC_RST, 0);
#endif
}

/*!
 * Setup GPIO for FEC device to be inactive
 *
 */
void gpio_fec_inactive(void)
{
	/*FEC_LINK INTRQ */
	mxc_set_gpio_direction(MX27_PIN_IOIS16, 0);
	gpio_free_mux(MX27_PIN_IOIS16);
	gpio_free_mux(MX27_PIN_USB_OC_B);
	gpio_free_mux(MX27_PIN_PWMO);
	/*FEC_RESET */
	gpio_free_mux(MX27_PIN_PC_RST);

	gpio_free_mux(MX27_PIN_ATA_DATA0);
	gpio_free_mux(MX27_PIN_ATA_DATA1);
	gpio_free_mux(MX27_PIN_ATA_DATA2);
	gpio_free_mux(MX27_PIN_ATA_DATA3);
	gpio_free_mux(MX27_PIN_ATA_DATA4);
	gpio_free_mux(MX27_PIN_ATA_DATA5);
	gpio_free_mux(MX27_PIN_ATA_DATA6);
	gpio_free_mux(MX27_PIN_ATA_DATA7);
	gpio_free_mux(MX27_PIN_ATA_DATA8);
	gpio_free_mux(MX27_PIN_ATA_DATA9);
	gpio_free_mux(MX27_PIN_ATA_DATA10);
	gpio_free_mux(MX27_PIN_ATA_DATA11);
	gpio_free_mux(MX27_PIN_ATA_DATA12);
	gpio_free_mux(MX27_PIN_ATA_DATA13);
	gpio_free_mux(MX27_PIN_ATA_DATA14);
	gpio_free_mux(MX27_PIN_ATA_DATA15);

	gpio_free_mux(MX27_PIN_SD3_CMD);
	gpio_free_mux(MX27_PIN_SD3_CLK);
}

/*!
 * Setup GPIO to init CS8900
 *
 */
void gpio_cs8900_init(void)
{
	/* Reset */
	gpio_request_mux(MX27_PIN_I2C2_SDA, GPIO_MUX_GPIO);
	mxc_set_gpio_direction(MX27_PIN_I2C2_SDA, 0);
	mxc_set_gpio_dataout(MX27_PIN_I2C2_SDA, 1);
	mdelay(10);
	mxc_set_gpio_dataout(MX27_PIN_I2C2_SDA, 0);
	mdelay(10);

	/* Config GPIO interrupt */
	gpio_request_mux(MX27_PIN_SSI3_FS, GPIO_MUX_GPIO);
	mxc_set_gpio_direction(MX27_PIN_SSI3_FS, 1); /* input */
	gpio_set_puen(MX27_PIN_SSI3_FS, 0);
	set_irq_type(IOMUX_TO_IRQ(MX27_PIN_SSI3_FS), IRQT_HIGH);
}

#if 0
/*!
 * Setup GPIO for SLCDC device to be active
 *
 */
void gpio_slcdc_active(int type)
{
	switch (type) {
	case 0:
    #if 0
		//gpio_request_mux(MX27_PIN_SSI3_CLK, GPIO_MUX_ALT);	/* CLK */
		gpio_request_mux(MX27_PIN_SSI3_TXDAT, GPIO_MUX_ALT);	/* CS  */
		gpio_request_mux(MX27_PIN_SSI3_RXDAT, GPIO_MUX_ALT);	/* RS  */
		gpio_request_mux(MX27_PIN_SSI3_FS, GPIO_MUX_ALT);	/* D0  */
	#endif
		break;

	case 1:
		gpio_request_mux(MX27_PIN_SD2_D1, GPIO_MUX_GPIO);	/* CLK */
		gpio_request_mux(MX27_PIN_SD2_D2, GPIO_MUX_GPIO);	/* D0  */
		gpio_request_mux(MX27_PIN_SD2_D3, GPIO_MUX_GPIO);	/* RS  */
		gpio_request_mux(MX27_PIN_SD2_CMD, GPIO_MUX_GPIO);	/* CS  */
		break;

	case 2:
		gpio_request_mux(MX27_PIN_LD0, GPIO_MUX_GPIO);
		gpio_request_mux(MX27_PIN_LD1, GPIO_MUX_GPIO);
		gpio_request_mux(MX27_PIN_LD2, GPIO_MUX_GPIO);
		gpio_request_mux(MX27_PIN_LD3, GPIO_MUX_GPIO);
		gpio_request_mux(MX27_PIN_LD4, GPIO_MUX_GPIO);
		gpio_request_mux(MX27_PIN_LD5, GPIO_MUX_GPIO);
		gpio_request_mux(MX27_PIN_LD6, GPIO_MUX_GPIO);
		gpio_request_mux(MX27_PIN_LD7, GPIO_MUX_GPIO);
		gpio_request_mux(MX27_PIN_LD8, GPIO_MUX_GPIO);
		gpio_request_mux(MX27_PIN_LD9, GPIO_MUX_GPIO);
		gpio_request_mux(MX27_PIN_LD10, GPIO_MUX_GPIO);
		gpio_request_mux(MX27_PIN_LD11, GPIO_MUX_GPIO);
		gpio_request_mux(MX27_PIN_LD12, GPIO_MUX_GPIO);
		gpio_request_mux(MX27_PIN_LD13, GPIO_MUX_GPIO);
		gpio_request_mux(MX27_PIN_LD14, GPIO_MUX_GPIO);
		gpio_request_mux(MX27_PIN_LD15, GPIO_MUX_GPIO);
		break;

	default:
		break;
	}

	return;
}

/*!
 * Setup GPIO for SLCDC device to be inactive
 *
 */
void gpio_slcdc_inactive(int type)
{
	switch (type) {
	case 0:
    #if 0
		//gpio_free_mux(MX27_PIN_SSI3_CLK);	/* CLK */
		gpio_free_mux(MX27_PIN_SSI3_TXDAT);	/* CS  */
		gpio_free_mux(MX27_PIN_SSI3_RXDAT);	/* RS  */
		gpio_free_mux(MX27_PIN_SSI3_FS);	/* D0  */
	#endif
		break;

	case 1:
		gpio_free_mux(MX27_PIN_SD2_D1);	/* CLK */
		gpio_free_mux(MX27_PIN_SD2_D2);	/* D0  */
		gpio_free_mux(MX27_PIN_SD2_D3);	/* RS  */
		gpio_free_mux(MX27_PIN_SD2_CMD);	/* CS  */
		break;

	case 2:
		gpio_free_mux(MX27_PIN_LD0);
		gpio_free_mux(MX27_PIN_LD1);
		gpio_free_mux(MX27_PIN_LD2);
		gpio_free_mux(MX27_PIN_LD3);
		gpio_free_mux(MX27_PIN_LD4);
		gpio_free_mux(MX27_PIN_LD5);
		gpio_free_mux(MX27_PIN_LD6);
		gpio_free_mux(MX27_PIN_LD7);
		gpio_free_mux(MX27_PIN_LD8);
		gpio_free_mux(MX27_PIN_LD9);
		gpio_free_mux(MX27_PIN_LD10);
		gpio_free_mux(MX27_PIN_LD11);
		gpio_free_mux(MX27_PIN_LD12);
		gpio_free_mux(MX27_PIN_LD13);
		gpio_free_mux(MX27_PIN_LD14);
		gpio_free_mux(MX27_PIN_LD15);
		break;

	default:
		break;
	}

	return;
}
#endif

void gpio_ssi_active(int ssi_num)
{
	switch (ssi_num) {
	case 0:
		gpio_request_mux(MX27_PIN_SSI1_FS, GPIO_MUX_PRIMARY);
		gpio_request_mux(MX27_PIN_SSI1_RXDAT, GPIO_MUX_PRIMARY);
		gpio_request_mux(MX27_PIN_SSI1_TXDAT, GPIO_MUX_PRIMARY);
		gpio_request_mux(MX27_PIN_SSI1_CLK, GPIO_MUX_PRIMARY);
		gpio_set_puen(MX27_PIN_SSI1_FS, 0);
		gpio_set_puen(MX27_PIN_SSI1_RXDAT, 0);
		gpio_set_puen(MX27_PIN_SSI1_TXDAT, 0);
		gpio_set_puen(MX27_PIN_SSI1_CLK, 0);
		break;
	case 1:
		gpio_request_mux(MX27_PIN_SSI2_FS, GPIO_MUX_PRIMARY);
		gpio_request_mux(MX27_PIN_SSI2_RXDAT, GPIO_MUX_PRIMARY);
		gpio_request_mux(MX27_PIN_SSI2_TXDAT, GPIO_MUX_PRIMARY);
		gpio_request_mux(MX27_PIN_SSI2_CLK, GPIO_MUX_PRIMARY);
		gpio_set_puen(MX27_PIN_SSI2_FS, 0);
		gpio_set_puen(MX27_PIN_SSI2_RXDAT, 0);
		gpio_set_puen(MX27_PIN_SSI2_TXDAT, 0);
		gpio_set_puen(MX27_PIN_SSI2_CLK, 0);
		break;
	case 3:
                gpio_request_mux(MX27_PIN_SSI4_FS, GPIO_MUX_PRIMARY);
                gpio_request_mux(MX27_PIN_SSI4_RXDAT, GPIO_MUX_PRIMARY);
                gpio_request_mux(MX27_PIN_SSI4_TXDAT, GPIO_MUX_PRIMARY);
                gpio_request_mux(MX27_PIN_SSI4_CLK, GPIO_MUX_PRIMARY);
                gpio_set_puen(MX27_PIN_SSI4_FS, 0);
                gpio_set_puen(MX27_PIN_SSI4_RXDAT, 0);
                gpio_set_puen(MX27_PIN_SSI4_TXDAT, 0);
                gpio_set_puen(MX27_PIN_SSI4_CLK, 0);
                break;
	default:
		break;
	}
	return;
}

/*!
 *  * Setup GPIO for a SSI port to be inactive
 *   *
 *    * @param  ssi_num         an SSI port num
 */

void gpio_ssi_inactive(int ssi_num)
{
	switch (ssi_num) {
	case 0:
		gpio_free_mux(MX27_PIN_SSI1_FS);
		gpio_free_mux(MX27_PIN_SSI1_RXDAT);
		gpio_free_mux(MX27_PIN_SSI1_TXDAT);
		gpio_free_mux(MX27_PIN_SSI1_CLK);
		break;
	case 1:
		gpio_free_mux(MX27_PIN_SSI2_FS);
		gpio_free_mux(MX27_PIN_SSI2_RXDAT);
		gpio_free_mux(MX27_PIN_SSI2_TXDAT);
		gpio_free_mux(MX27_PIN_SSI2_CLK);
		break;
	case 3:
		gpio_free_mux(MX27_PIN_SSI4_FS);
                gpio_free_mux(MX27_PIN_SSI4_RXDAT);
                gpio_free_mux(MX27_PIN_SSI4_TXDAT);
                gpio_free_mux(MX27_PIN_SSI4_CLK);
		break;
	default:
		break;
	}
	return;
}

void gpio_jack_active(void)
{	
    gpio_request_mux(MX27_PIN_SSI3_RXDAT, GPIO_MUX_GPIO);
    mxc_set_gpio_direction(MX27_PIN_SSI3_RXDAT, 1);
    gpio_set_puen(MX27_PIN_SSI3_RXDAT, 0);
}

void gpio_jack_inactive(void)
{
	gpio_free_mux(MX27_PIN_SSI3_RXDAT);
}

/*!
 * Setup GPIO for SDHC to be active
 *
 * @param module SDHC module number
 */
void gpio_sdhc_active(int module)
{
	u16 data;
	switch (module) {
	case 0:
		gpio_request_mux(MX27_PIN_SD1_CLK, GPIO_MUX_PRIMARY);
		gpio_request_mux(MX27_PIN_SD1_CMD, GPIO_MUX_PRIMARY);
		gpio_request_mux(MX27_PIN_SD1_D0, GPIO_MUX_PRIMARY);
		gpio_request_mux(MX27_PIN_SD1_D1, GPIO_MUX_PRIMARY);
		gpio_request_mux(MX27_PIN_SD1_D2, GPIO_MUX_PRIMARY);
		gpio_request_mux(MX27_PIN_SD1_D3, GPIO_MUX_PRIMARY);
		/* 22k pull up for sd1 dat3 pins */
		data = __raw_readw(IO_ADDRESS(SYSCTRL_BASE_ADDR + 0x54));
		data |= 0x0c;
		__raw_writew(data, IO_ADDRESS(SYSCTRL_BASE_ADDR + 0x54));
		/*mxc_clks_enable(SDHC1_CLK);
		   mxc_clks_enable(PERCLK2); */
		   
#ifdef CONFIG_MACH_MX27_MDK27V0
		/* Buffer Enable Pin, Active HI */
//		gpio_request_mux(MX27_PIN_USBH1_RCV, GPIO_MUX_GPIO);
//		mxc_set_gpio_direction(MX27_PIN_USBH1_RCV, 0);
		//mdelay(10);
//		mxc_set_gpio_dataout(MX27_PIN_USBH1_RCV, 1);
#ifdef CONFIG_MACH_MX27_MDK27V1
		/* PA27 Write Protect*/
		gpio_request_mux(MX27_PIN_SPL_SPR, GPIO_MUX_GPIO);
		mxc_set_gpio_direction(MX27_PIN_SPL_SPR, 1);
		gpio_set_puen(MX27_PIN_SPL_SPR, 0);
#endif
#ifdef CONFIG_MACH_MX27_TUOSI
		/* PD19 Write Protect for SD2 */
		gpio_request_mux(MX27_PIN_CSPI2_SS2, GPIO_MUX_GPIO);
		mxc_set_gpio_direction(MX27_PIN_CSPI2_SS2, 1);
		gpio_set_puen(MX27_PIN_CSPI2_SS2, 0);
#endif
#endif
		break;
	case 1:
		gpio_request_mux(MX27_PIN_SD2_CLK, GPIO_MUX_PRIMARY);
		gpio_request_mux(MX27_PIN_SD2_CMD, GPIO_MUX_PRIMARY);
		gpio_request_mux(MX27_PIN_SD2_D0, GPIO_MUX_PRIMARY);
		gpio_request_mux(MX27_PIN_SD2_D1, GPIO_MUX_PRIMARY);
		gpio_request_mux(MX27_PIN_SD2_D2, GPIO_MUX_PRIMARY);
		gpio_request_mux(MX27_PIN_SD2_D3, GPIO_MUX_PRIMARY);
		/* 22k pull up for sd2 pins */
		data = __raw_readw(IO_ADDRESS(SYSCTRL_BASE_ADDR + 0x54));
		data &= ~0xfff0;
		data |= 0xfff0;
		__raw_writew(data, IO_ADDRESS(SYSCTRL_BASE_ADDR + 0x54));
		/*mxc_clks_enable(SDHC2_CLK);
		   mxc_clks_enable(PERCLK2); */
		break;
	case 2:
		gpio_request_mux(MX27_PIN_SD3_CLK, GPIO_MUX_PRIMARY);
		gpio_request_mux(MX27_PIN_SD3_CMD, GPIO_MUX_PRIMARY);
		gpio_request_mux(MX27_PIN_ATA_DATA0, GPIO_MUX_ALT);
		gpio_request_mux(MX27_PIN_ATA_DATA1, GPIO_MUX_ALT);
		gpio_request_mux(MX27_PIN_ATA_DATA2, GPIO_MUX_ALT);
		gpio_request_mux(MX27_PIN_ATA_DATA3, GPIO_MUX_ALT);
		/*mxc_clks_enable(SDHC3_CLK);
		   mxc_clks_enable(PERCLK2); */
		break;
	default:
		break;
	}
}

/*!
 * Setup GPIO for SDHC1 to be inactive
 *
 * @param module SDHC module number
 */
void gpio_sdhc_inactive(int module)
{
	switch (module) {
	case 0:
		gpio_free_mux(MX27_PIN_SD1_CLK);
		gpio_free_mux(MX27_PIN_SD1_CMD);
		gpio_free_mux(MX27_PIN_SD1_D0);
		gpio_free_mux(MX27_PIN_SD1_D1);
		gpio_free_mux(MX27_PIN_SD1_D2);
		gpio_free_mux(MX27_PIN_SD1_D3);
		/*mxc_clks_disable(SDHC1_CLK); */
#ifdef CONFIG_MACH_MX27_MDK27V0
		/* Buffer Enable Pin of SD, Active HI */
//		mxc_set_gpio_dataout(MX27_PIN_USBH1_RCV, 0);
//		gpio_free_mux(MX27_PIN_USBH1_RCV);
#endif
		break;
	case 1:
		gpio_free_mux(MX27_PIN_SD2_CLK);
		gpio_free_mux(MX27_PIN_SD2_CMD);
		gpio_free_mux(MX27_PIN_SD2_D0);
		gpio_free_mux(MX27_PIN_SD2_D1);
		gpio_free_mux(MX27_PIN_SD2_D2);
		gpio_free_mux(MX27_PIN_SD2_D3);
		/*mxc_clks_disable(SDHC2_CLK); */
		break;
	case 2:
		gpio_free_mux(MX27_PIN_SD3_CLK);
		gpio_free_mux(MX27_PIN_SD3_CMD);
		gpio_free_mux(MX27_PIN_ATA_DATA0);
		gpio_free_mux(MX27_PIN_ATA_DATA1);
		gpio_free_mux(MX27_PIN_ATA_DATA2);
		gpio_free_mux(MX27_PIN_ATA_DATA3);
		/*mxc_clks_disable(SDHC3_CLK); */
		break;
	default:
		break;
	}
}

/*
 * Probe for the card. If present the GPIO data would be set.
 */
int sdhc_get_card_det_status(struct device *dev)
{
	if (to_platform_device(dev)->id == 0) {
#ifdef CONFIG_MACH_MX27_MPK271
		return mxc_get_gpio_ssr(MX27_PIN_USBH1_OE_B);
#else
		return mxc_get_gpio_ssr(MX27_PIN_USBH1_FS);
#endif
	} else {
#ifdef CONFIG_MACH_MX27_TUOSI
		return mxc_get_gpio_ssr(MX27_PIN_SSI4_TXDAT);
#else
		return mxc_get_gpio_ssr(MX27_PIN_USBH1_FS);
#endif
	}

}

EXPORT_SYMBOL(sdhc_get_card_det_status);

/*
 * Return the card detect pin.
 */
int sdhc_init_card_det(int id)
{
	if (id == 0) {
#ifdef CONFIG_MACH_MX27_MPK271
		gpio_config_mux(MX27_PIN_USBH1_OE_B, GPIO_MUX_GPIO);
		mxc_set_gpio_direction(MX27_PIN_USBH1_OE_B, 1);
		gpio_set_puen(MX27_PIN_USBH1_OE_B, 0);
		return IOMUX_TO_IRQ(MX27_PIN_USBH1_OE_B);
#else
		gpio_request_mux(MX27_PIN_USBH1_FS, GPIO_MUX_GPIO);
		mxc_set_gpio_direction(MX27_PIN_USBH1_FS, 1);
		gpio_set_puen(MX27_PIN_USBH1_FS, 0);
		return IOMUX_TO_IRQ(MX27_PIN_USBH1_FS);
#endif
	} else {
#ifdef CONFIG_MACH_MX27_TUOSI
		gpio_config_mux(MX27_PIN_SSI4_TXDAT, GPIO_MUX_GPIO );
		mxc_set_gpio_direction(MX27_PIN_SSI4_TXDAT, 1);
		gpio_set_puen(MX27_PIN_SSI4_TXDAT, 0);
		return IOMUX_TO_IRQ(MX27_PIN_SSI4_TXDAT);
#else
		gpio_config_mux(MX27_PIN_USBH1_FS,GPIO_MUX_GPIO );
		mxc_set_gpio_direction(MX27_PIN_USBH1_FS, 1);
		gpio_set_puen(MX27_PIN_USBH1_FS, 0);
		return IOMUX_TO_IRQ(MX27_PIN_USBH1_FS);
#endif
	}
}

EXPORT_SYMBOL(sdhc_init_card_det);

#ifdef CONFIG_MACH_MX27_MDK27V0
void sdhc_irqchip_handle(unsigned int irq)
{
	struct irqdesc *d;
	d = irq_desc + irq;
	if (unlikely(!(d->handle_irq))) {
		printk(KERN_ERR "\nSDIO int unhandled\n");
		BUG();	/* oops */
	}
	d->handle_irq(irq, d);
}
EXPORT_SYMBOL(sdhc_irqchip_handle);

static void mxc_sdio_mask_irq(unsigned int irq)
{
	u32 reg;
	struct mmc_host *host = get_irq_chipdata(irq);
	struct mxcmci_host *mxc_host = mmc_priv(host);

	if (host->sdio_irq != irq)
		return;

	reg = __raw_readl(mxc_host->base + MMC_INT_CNTR);
	reg &= ~INT_CNTR_SDIO_IRQ_EN;
	__raw_writel(reg, mxc_host->base + MMC_INT_CNTR);
}

static void mxc_sdio_ack_irq(unsigned int irq)
{
	struct mmc_host *host = get_irq_chipdata(irq);
	struct mxcmci_host *mxc_host = mmc_priv(host);

	if (host->sdio_irq != irq)
		return;

	mxc_sdio_mask_irq(irq);

	__raw_writel(STATUS_SDIO_INT_ACTIVE, mxc_host->base + MMC_STATUS);
}

static void mxc_sdio_unmask_irq(unsigned int irq)
{
	u32 reg;
	struct mmc_host *host = get_irq_chipdata(irq);
	struct mxcmci_host *mxc_host = mmc_priv(host);

	if (host->sdio_irq != irq)
		return;

	__raw_writel(STATUS_SDIO_INT_ACTIVE, mxc_host->base + MMC_STATUS);

	reg = __raw_readl(mxc_host->base + MMC_INT_CNTR);
	reg |= INT_CNTR_SDIO_IRQ_EN;
	__raw_writel(reg, mxc_host->base + MMC_INT_CNTR);
}

static int mxc_sdio_set_wake(unsigned int irq, unsigned int enable)
{
	struct mmc_host *host = get_irq_chipdata(irq);
	struct mxcmci_host *mxc_host = mmc_priv(host);

	if (host->sdio_irq != irq) {
		return -EINVAL;
	}

	if (enable != 0) {
		pr_debug("enable sdio wake up function\n");
		mxc_host->sdio_set_wake_enable = 1;
	} else {
		pr_debug("disable sdio wake up function\n");
		mxc_host->sdio_set_wake_enable = 0;
	}

	return 0;
}

static struct irq_chip mxc_sdio_irq_chip = {
	.ack = mxc_sdio_ack_irq,
	.mask = mxc_sdio_mask_irq,
	.unmask = mxc_sdio_unmask_irq,
	.set_wake = mxc_sdio_set_wake,
};

/*
 * setup the SDHC host irq chipset
 */
void sdhc_setup_irq(unsigned int irq, struct mmc_host *mmc)
{
	set_irq_chip(irq, &mxc_sdio_irq_chip);
	set_irq_chipdata(irq, mmc);
	set_irq_handler(irq, handle_level_irq);
	set_irq_flags(irq, IRQF_VALID);
}
EXPORT_SYMBOL(sdhc_setup_irq);

/*!
 * Get SD1_WP ADIN7 of ATLAS pin value to detect write protection
 */
int sdhc_write_protect(int id)
{
#ifdef CONFIG_MACH_MX27_TUOSI
	if (id == 0)
		return mxc_get_gpio_ssr(MX27_PIN_SPL_SPR) ? 1 : 0;
	else	/* SD2 */
		return mxc_get_gpio_ssr(MX27_PIN_CSPI2_SS2) ? 1 : 0;
#elif defined(CONFIG_MACH_MX27_MDK27V1)
	/* PA27 Write Protect*/
	return mxc_get_gpio_ssr(MX27_PIN_SPL_SPR) ? 1 : 0;
#else
	unsigned short rc;

	pmic_adc_convert(GEN_PURPOSE_AD7, &rc);
	if (rc > 0)
	    return 1;
	else
	    return 0;
#endif
}
EXPORT_SYMBOL(sdhc_write_protect);

#endif

/*
 * Power on/off Sharp QVGA panel.
 */
void board_power_lcd(int on)
{
}

void gpio_owire_active(void)
{
	gpio_request_mux(MX27_PIN_RTCK, GPIO_MUX_ALT);
}

void gpio_owire_inactive(void)
{
	gpio_request_mux(MX27_PIN_RTCK, GPIO_MUX_PRIMARY);
}

void gpio_irda_active(void)
{
	gpio_uart_active(2, 0);
	/* Band width select */
}

void gpio_irda_inactive(void)
{
	gpio_uart_inactive(2, 0);
}

void gpio_apmwifi_power_on(void)
{
        /* Power on */
	gpio_request_mux(MX27_PIN_CLS, GPIO_MUX_GPIO);
	mxc_set_gpio_direction(MX27_PIN_CLS, 0);
	mxc_set_gpio_dataout(MX27_PIN_CLS, 1);
	mdelay(10);

	/* Reset */
	gpio_request_mux(MX27_PIN_USBH1_TXDM, GPIO_MUX_GPIO);
	mxc_set_gpio_direction(MX27_PIN_USBH1_TXDM, 0);
	mxc_set_gpio_dataout(MX27_PIN_USBH1_TXDM, 0);
	mdelay(10);
	mxc_set_gpio_dataout(MX27_PIN_USBH1_TXDM, 1);
	mdelay(10);
}

void gpio_apmwifi_power_off(void)
{
	mxc_set_gpio_dataout(MX27_PIN_CLS, 0);
}

EXPORT_SYMBOL(gpio_uart_active);
EXPORT_SYMBOL(gpio_uart_inactive);
EXPORT_SYMBOL(config_uartdma_event);
EXPORT_SYMBOL(gpio_usbh1_active);
EXPORT_SYMBOL(gpio_usbh1_inactive);
EXPORT_SYMBOL(gpio_usbh2_active);
EXPORT_SYMBOL(gpio_usbh2_inactive);
EXPORT_SYMBOL(gpio_usbotg_hs_active);
EXPORT_SYMBOL(gpio_usbotg_hs_inactive);
EXPORT_SYMBOL(gpio_usbotg_fs_active);
EXPORT_SYMBOL(gpio_usbotg_fs_inactive);
EXPORT_SYMBOL(gpio_i2c_active);
EXPORT_SYMBOL(gpio_i2c_inactive);
EXPORT_SYMBOL(gpio_spi_active);
EXPORT_SYMBOL(gpio_spi_inactive);
EXPORT_SYMBOL(gpio_nand_active);
EXPORT_SYMBOL(gpio_nand_inactive);
EXPORT_SYMBOL(gpio_sensor_active);
EXPORT_SYMBOL(gpio_sensor_inactive);
//EXPORT_SYMBOL(mxc_tvp5150_reset);
//EXPORT_SYMBOL(gpio_tvp5150_active);
//EXPORT_SYMBOL(gpio_tvp5150_inactive);

EXPORT_SYMBOL(mxc_ccd_enable);
EXPORT_SYMBOL(mxc_cmos_enable);
EXPORT_SYMBOL(mxc_camera_select);

EXPORT_SYMBOL(gpio_tw2835_active);
EXPORT_SYMBOL(gpio_tw2835_inactive);
EXPORT_SYMBOL(gpio_adv7180_active);
EXPORT_SYMBOL(gpio_adv7180_inactive);
//EXPORT_SYMBOL(gpio_wm9712_ts_active);
//EXPORT_SYMBOL(gpio_wm9712_ts_inactive);
EXPORT_SYMBOL(gpio_si4702_active);
EXPORT_SYMBOL(gpio_si4702_inactive);
EXPORT_SYMBOL(gpio_si4702_reset);
EXPORT_SYMBOL(gpio_si4702_powerdown);
EXPORT_SYMBOL(gpio_lcdc_active);
EXPORT_SYMBOL(gpio_lcdc_inactive);
EXPORT_SYMBOL(gpio_fs453_reset_low);
EXPORT_SYMBOL(gpio_fs453_reset_high);
EXPORT_SYMBOL(gpio_pmic_active);
EXPORT_SYMBOL(gpio_keypad_active);
EXPORT_SYMBOL(gpio_keypad_inactive);
//EXPORT_SYMBOL(gpio_ata_active);
//EXPORT_SYMBOL(gpio_ata_inactive);
EXPORT_SYMBOL(gpio_fec_active);
EXPORT_SYMBOL(gpio_fec_inactive);
//EXPORT_SYMBOL(gpio_slcdc_active);
//EXPORT_SYMBOL(gpio_slcdc_inactive);
EXPORT_SYMBOL(gpio_ssi_active);
EXPORT_SYMBOL(gpio_ssi_inactive);
EXPORT_SYMBOL(gpio_sdhc_active);
EXPORT_SYMBOL(gpio_sdhc_inactive);
EXPORT_SYMBOL(board_power_lcd);
EXPORT_SYMBOL(gpio_owire_active);
EXPORT_SYMBOL(gpio_owire_inactive);
EXPORT_SYMBOL(gpio_irda_active);
EXPORT_SYMBOL(gpio_irda_inactive);
EXPORT_SYMBOL(gpio_jack_active);
EXPORT_SYMBOL(gpio_jack_inactive);
EXPORT_SYMBOL(gpio_cs8900_init);
EXPORT_SYMBOL(gpio_apmwifi_power_on);
EXPORT_SYMBOL(gpio_apmwifi_power_off);
