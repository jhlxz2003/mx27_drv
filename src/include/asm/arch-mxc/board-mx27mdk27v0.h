/*
 * Copyright 2005-2007 Freescale Semiconductor, Inc. All Rights Reserved.
 */

/*
 * The code contained herein is licensed under the GNU General Public
 * License. You may obtain a copy of the GNU General Public License
 * Version 2 or later at the following locations:
 *
 * http://www.opensource.org/licenses/gpl-license.html
 * http://www.gnu.org/copyleft/gpl.html
 */

#ifndef __ASM_ARCH_MXC_BOARD_MX27_MDK27V0_H__
#define __ASM_ARCH_MXC_BOARD_MX27_MDK27V0_H__

/*!
 * @defgroup BRDCFG_MX27 Board Configuration Options
 * @ingroup MSL_MX27
 */

/*!
 * @file arch-mxc/board-mx27mdk27v0.h
 *
 * @brief This file contains all the board level configuration options.
 *
 * It currently hold the options defined for MX27 MDK27V0 Platform.
 *
 * @ingroup BRDCFG_MX27
 */

/*
 * Include Files
 */
#include <asm/arch/board.h>

/* Start of physical RAM */
#define PHYS_OFFSET             UL(0xA0000000)

/* Size of contiguous memory for DMA and other h/w blocks */
#define CONSISTENT_DMA_SIZE	SZ_16M

/*!
 * @name MXC UART EVB board level configurations
 */
/*! @{ */
/*!
 * Specify the max baudrate for the MXC UARTs for your board, do not specify a max
 * baudrate greater than 1500000. This is used while specifying the UART Power
 * management constraints.
 */
#define MAX_UART_BAUDRATE       1500000
/*!
 * Specifies if the Irda transmit path is inverting
 */
#define MXC_IRDA_TX_INV         0
/*!
 * Specifies if the Irda receive path is inverting
 */
#define MXC_IRDA_RX_INV         0

/* UART 1 configuration */
/*!
 * This define specifies if the UART port is configured to be in DTE or
 * DCE mode. There exists a define like this for each UART port. Valid
 * values that can be used are \b MODE_DTE or \b MODE_DCE.
 */
#define UART1_MODE              MODE_DCE
/*!
 * This define specifies if the UART is to be used for IRDA. There exists a
 * define like this for each UART port. Valid values that can be used are
 * \b IRDA or \b NO_IRDA.
 */
#define UART1_IR                NO_IRDA
/*!
 * This define is used to enable or disable a particular UART port. If
 * disabled, the UART will not be registered in the file system and the user
 * will not be able to access it. There exists a define like this for each UART
 * port. Specify a value of 1 to enable the UART and 0 to disable it.
 */
#define UART1_ENABLED           1
/*! @} */
/* UART 2 configuration */
#define UART2_MODE              MODE_DCE
#define UART2_IR                NO_IRDA
#define UART2_ENABLED           1
/* UART 3 configuration */
#define UART3_MODE              MODE_DCE
#define UART3_IR                NO_IRDA
#define UART3_ENABLED           1
/* UART 4 configuration */
#define UART4_MODE              MODE_DTE
#define UART4_IR                NO_IRDA
#define UART4_ENABLED           0	/* Disable UART 4 as its pins are shared with ATA */
/* UART 5 configuration */
#define UART5_MODE              MODE_DTE
#define UART5_IR                NO_IRDA
#define UART5_ENABLED           1
/* UART 6 configuration */
#define UART6_MODE              MODE_DTE
#define UART6_IR                NO_IRDA
#define UART6_ENABLED           1

#if 0
#define MXC_LL_EXTUART_PADDR	(CS4_BASE_ADDR + 0x20000)
#define MXC_LL_EXTUART_VADDR	(CS4_BASE_ADDR_VIRT + 0x20000)
#define MXC_LL_EXTUART_16BIT_BUS
#endif

#define MXC_LL_UART_PADDR       UART1_BASE_ADDR
#define MXC_LL_UART_VADDR       AIPI_IO_ADDRESS(UART1_BASE_ADDR)

/*!
 * @name Memory Size parameters
 */
/*! @{ */
/*!
 * Size of SDRAM memory
 */
#define SDRAM_MEM_SIZE          SZ_128M

#define MEM_SIZE                (SDRAM_MEM_SIZE - SZ_1M)

/*! @} */

/*! @{ */
/*!
 * @name Keypad Configurations
 */
/*! @{ */
/*!
 * Maximum number of rows (0 to 7)
 */
#define MAXROW                          6
/*!
 * Maximum number of columns (0 to 7)
 */
#define MAXCOL                          6

#define MXC_MAX_EXP_IO_LINES    16

/*! @} */
/*!
 * Base address of PBC controller, CS5
 */
#define PBC_CS5_BASE_ADDRESS        IO_ADDRESS(CS5_BASE_ADDR)
#define PBC_CS5_REG_ADDR(offset)    (PBC_CS5_BASE_ADDRESS + (offset))

#define PBC_CS5_ENET_BASE       PBC_CS5_REG_ADDR( 0x00000 )
#define PBC_CS5_EUARTA_BASE     PBC_CS5_REG_ADDR( 0x08000 )
#define PBC_CS5_EUARTB_BASE     PBC_CS5_REG_ADDR( 0x10000 )

#define PBC_CS5_LED_REG         PBC_CS5_REG_ADDR( 0x20000 )
#define PBC_CS5_SB_REG          PBC_CS5_REG_ADDR( 0x20008 )
#define PBC_CS5_ISR_REG         PBC_CS5_REG_ADDR( 0x20010 )
#define PBC_CS5_ICR_REG         PBC_CS5_REG_ADDR( 0x20020 )
#define PBC_CS5_CTRL1_REG       PBC_CS5_REG_ADDR( 0x20028 )
#define PBC_CS5_CTRL2_REG       PBC_CS5_REG_ADDR( 0x20030 )
#define PBC_CS5_IMR_REG         PBC_CS5_REG_ADDR( 0x20038 )

#define PBC_CS5_SIG1_REG	PBC_CS5_REG_ADDR( 0x20040 )
#define PBC_CS5_SIG2_REG	PBC_CS5_REG_ADDR( 0x20048 )
#define PBC_CS5_VERSION_REG     PBC_CS5_REG_ADDR( 0x20050 )
#define PBC_CS5_SIG3_REG       	PBC_CS5_REG_ADDR( 0x20058 )
#define PBC_CS5_RST_REG         PBC_CS5_REG_ADDR( 0x20060 )
#define PBC_CS5_ID_REG         	PBC_CS5_REG_ADDR( 0x20068 )

/* For interrupts like xuart, enet etc */
#define EXPIO_CS5_PARENT_INT        IOMUX_TO_IRQ(MX27_PIN_SSI3_FS)

/* TODO:: */
/*
 *
 */

#define EXPIO_CS5_INT_ENET_INT  	(MXC_EXP_IO_BASE + 0 )
#define EXPIO_CS5_INT_XUART_INTA    	(MXC_EXP_IO_BASE + 1)
#define EXPIO_CS5_INT_XUART_INTB    	(MXC_EXP_IO_BASE + 2)
#define EXPIO_CS5_INT_BUTTON_INTA      	(MXC_EXP_IO_BASE + 3)
#define EXPIO_CS5_INT_BUTTON_INTB      	(MXC_EXP_IO_BASE + 4)

#define EXPIO_INT_FEC  (IOMUX_TO_IRQ(MX27_PIN_IOIS16))

/*! This is System IRQ used by LAN9217 for interrupt generation taken from platform.h */
#define LAN9217_IRQ              EXPIO_CS5_INT_ENET_INT
/*! This is I/O Base address used to access registers of CS8900A on MXC ADS */
#define LAN9217_BASE_ADDRESS    (PBC_CS5_ENET_BASE)
/*! This is System IRQ used by CS8900A for interrupt generation */
#define CS8900AIRQ              (IOMUX_TO_IRQ(MX27_PIN_SSI3_FS))
/*! This is I/O Base address used to access registers of CS8900A on MDK27V1 */
#define CS8900A_BASE_ADDRESS    (PBC_CS5_BASE_ADDRESS)

#define MXC_CS5_BD_LED1             (1)
#define MXC_CS5_BD_LED2             (1 << 1)
#define MXC_CS5_BD_LED3             (1 << 2)
#define MXC_CS5_BD_LED4             (1 << 3)
#define MXC_CS5_BD_LED5             (1 << 4)
#define MXC_CS5_BD_LED6             (1 << 5)
#define MXC_CS5_BD_LED7             (1 << 6)
#define MXC_CS5_BD_LED8             (1 << 7)
#define MXC_CS5_BD_LED_ON(led)
#define MXC_CS5_BD_LED_OFF(led)

/*! Definitions about CPLD signature*/
#define PBC_SIG_AAAA	0xAAAA
#define PBC_SIG_5555	0x5555
#define PBC_SIG_CAFE	0xCAFE

#define PBC_IS_VALID() 	\
	({	\
		u16 id1, id2, id3;	\
		id1 = __raw_readw(PBC_CS5_SIG1_REG); \
		id2 = __raw_readw(PBC_CS5_SIG2_REG); \
		id3 = __raw_readw(PBC_CS5_SIG3_REG); \
		(id1 == PBC_SIG_AAAA) && (id2 == PBC_SIG_5555) &&\
		(id3 == PBC_SIG_CAFE);\
	})

#define MXC_CKIH_SOURCE()       0

#define MXC_PBC_VERSION_MAJOR(x)	(((x)>>8)&0xFF)
#define MXC_PBC_VERSION_MINOR(x)	((x)&0xFF)

#define MXC_PBC_VER_1_0		1
#define MXC_PBC_VER_2_0		2
#define MXC_PBC_VER_3_0		3
#define MXC_PBC_VER_4_0		4

#ifndef __ASSEMBLY__
#ifdef CONFIG_MXC_POWER_CONTROL
extern int mxc_ata_power_on(void);
extern int mxc_ata_power_off(void);
extern int mxc_fec_power_on(void);
extern int mxc_fec_power_off(void);
extern int mxc_sdmmc_power_on(void);
extern int mxc_sdmmc_power_off(void);
#endif
#endif
#endif				/* __ASM_ARCH_MXC_BOARD_MX27_MDK27V0_H__ */
