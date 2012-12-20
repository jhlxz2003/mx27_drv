/*
 * linux/drivers/char/si4702.h
 *
 * Copyright 2004-2006 Freescale Semiconductor, Inc. All Rights Reserved.
 */
/*
 *
 * Copyright 2007 Morninghan Electronics, Inc. All Rights Reserved.
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
 * @defgroup Character device driver for SI4702 FM radio
 */

/*
 * @file si4702.h
 *
 * @brief SI4702 Radio FM driver
 *
 * @ingroup Character
 */
#ifndef __SI4702_H__
#define __SI4702_H__

#ifdef __KERNEL__

#define SI4702_DEV_NAME		"si4702"

/* I2C operations */
#define SI4702_I2C_ADDR		0x10	/* 7bits I2C address */

#define BAND			87500	/* 87.5 MHz */
#define MAX_BAND		108000
#define SPACING			100	/* 100 KHz */

#define DELAY_WAIT		8	/* loop_counter max value */

/* Register definition */
#define SI4702_DEVICEID		0x00
#define SI4702_CHIPID		0x01
#define SI4702_POWERCFG		0x02
#define SI4702_CHANNEL		0x03
#define SI4702_SYSCONFIG1	0x04
#define SI4702_SYSCONFIG2	0x05
#define SI4702_SYSCONFIG3	0x06
#define SI4702_TEST1		0x07
#define SI4702_TEST2		0x08
#define SI4702_B00TCONFIG	0x09
#define SI4702_STATUSRSSI	0x0A
#define SI4702_READCHAN		0x0B

#define SI4702_REG_NUM		0x10
#define SI4702_REG_BYTE		(SI4702_REG_NUM * 2)
#define SI4702_DEVICE_ID	0x1242

#define SI4702_RW_REG_NUM	(SI4702_STATUSRSSI - SI4702_POWERCFG)
#define SI4702_RW_OFFSET	(SI4702_REG_NUM - SI4702_STATUSRSSI + SI4702_POWERCFG)

#define BYTE_TO_WORD(hi, lo)	(((hi) << 8) & 0xFF00) | ((lo) & 0x00FF)

struct si4702_device {
	unsigned int volume;
	unsigned int channel;
	unsigned int mute:1;
};

#endif				/* __KERNEL__ */

/* define IOCTL command */
#define SI4702_GETVOLUME	_IOR('S', 0x10, unsigned int)
#define SI4702_SETVOLUME	_IOW('S', 0x11, unsigned int)
#define SI4702_MUTEON		_IO('S', 0x12)
#define SI4702_MUTEOFF		_IO('S', 0x13)
#define SI4702_SELECT		_IOW('S', 0x14, unsigned int)
#define SI4702_SEEK		_IOWR('S', 0x15, unsigned int)

#define si4702_attr(_name) \
static struct subsys_attribute _name##_attr = {	\
	.attr	= {				\
		.name = __stringify(_name),	\
		.mode = 0644,			\
	},					\
	.show	= _name##_show,			\
	.store	= _name##_store,		\
}

#endif				/* __SI4702_H__ */
