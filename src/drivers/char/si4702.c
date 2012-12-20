/*
 * linux/drivers/char/si4702.c
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
 * @defgroup Character device driver for si4702 FM radio
 */

/*
 * @file si4702.c
 *
 * @brief si4702 Radio FM driver
 *
 * @ingroup Character
 */

#include <linux/module.h>
#include <linux/init.h>

#include <linux/errno.h>
#include <linux/kernel.h>
#include <linux/major.h>
#include <linux/sched.h>
#include <linux/fs.h>
#include <linux/fcntl.h>
#include <linux/delay.h>
#include <linux/device.h>
#include <linux/spinlock.h>
#include <linux/i2c.h>
#include <linux/platform_device.h>
#include <asm/arch/mxc_i2c.h>

#include <asm/uaccess.h>

#include "si4702.h"

/* major number of this device */
static int major;
static int count = 0;	/* open count */
static spinlock_t count_lock; 	/* spin lock for count */
static struct si4702_device si4702_dev = {0};
static struct class *radio_class = NULL; /* class struct */

static char si4702_reg_buf[SI4702_REG_BYTE];

static void dump_reg(void);
static int si4702_read_reg(u8 reg, u16 *word);
static int si4702_write_reg(u8 reg, u16 word);
static int fm_sys_init(void);
static int fm_sys_exit(void);

extern void gpio_si4702_active(void);
extern void gpio_si4702_inactive(void);
extern void gpio_si4702_reset(void);
extern void gpio_si4702_powerdown(void);

/*!
  * si4702_startup
  * Startup and initial the si4702
  * @param 
  * @return
  * 	0 successful
  */
static int si4702_startup(void)
{
	u16 magic = 0, id;

	si4702_read_reg(SI4702_DEVICEID, &id);
	printk(KERN_INFO "si4702: DEVICEID: 0x%x\n", id);
		
	si4702_write_reg(SI4702_POWERCFG, 0x4001); /* disable mute, stereo, seek down, powerup */
	mdelay(100);
	si4702_read_reg(SI4702_TEST1, &magic);
	if(magic != 0x3C04) // this means the chip power on normally
		printk(KERN_ERR "magic number 0x%x.\n", magic);
	si4702_write_reg(SI4702_CHANNEL, 0); /* close tune, set channel to 0 */
	si4702_write_reg(SI4702_SYSCONFIG1, 0); /* disable interrupt, disable GPIO */
	si4702_write_reg(SI4702_SYSCONFIG2, 0x0f1f); /* seek threshold, band, space select to Europe, volume to max */
	si4702_write_reg(SI4702_SYSCONFIG3, 0x48); 
	
	return 0;
}

/*!
  * si4702_shutdown
  * Shutdown the si4702
  */
static void si4702_shutdown(void)
{
	si4702_write_reg(SI4702_POWERCFG, 0x4041);
	gpio_si4702_powerdown();
}

/*!
  * si4702_channel_select
  *
  * Select a channel
  * @param
  * 	freq	frequence to select in KHz unit
  * @return
  * 	0 successful
  *
  */
static u8 si4702_channel_select(u32 freq)
{
	u16 loop_counter = 0;
	s16 channel;
	u16 si4702_read_data, si4702_write_data;
	u8 error_ind = 0;
	u8 si4702_channel_start_tune[] = {0x40,0x01,0x80,0x00};
	u8 si4702_channel_stop_tune[] = {0x40,0x01,0x00};	

	if (si4702_dev.mute) {
		/* enable mute */
		si4702_channel_start_tune[0] = 0;
		si4702_channel_stop_tune[0] = 0;
	}
	printk(KERN_INFO "Input frequnce is %d\n", freq);
	/* convert freq to channel */
	channel = (freq - BAND) / SPACING;
	if (channel < 0 || channel > 1023) {
		printk(KERN_ERR "Input frequnce is invalid\n");
		return -1;
	}
	/* fill channel bits */
	si4702_channel_start_tune[2] |= channel >> 8;
	si4702_channel_start_tune[3] |= channel & 0xFF;

	/* set tune bit */
 	si4702_write_data = BYTE_TO_WORD(si4702_channel_start_tune[0], si4702_channel_start_tune[1]);
	error_ind = si4702_write_reg(SI4702_POWERCFG, si4702_write_data);

	si4702_write_data = BYTE_TO_WORD(si4702_channel_start_tune[2], si4702_channel_start_tune[3]);
	error_ind = si4702_write_reg(SI4702_CHANNEL, si4702_write_data);

	if (error_ind) {
		printk(KERN_ERR "Failed to set start tune\n");
		return -1;
	}

	/* wait for STC == 1 */
	do {	
		error_ind = si4702_read_reg(SI4702_STATUSRSSI, &si4702_read_data);
		
		if(error_ind) {
			printk(KERN_ERR "Failed to read setted STC\n");
			return -1;	
		}
		if ((si4702_read_data & 0x4000) != 0)
			break;
		/* sleep to wait */
		msleep(200);
		
	} while(++loop_counter < DELAY_WAIT);

	/* check loop_counter */
	if (loop_counter >= DELAY_WAIT) {
		printk(KERN_ERR "Can't wait for STC bit set");
		return -1;
	}
	loop_counter = 0;

	/* clear tune bit */
	error_ind = si4702_write_reg(SI4702_CHANNEL, 0);
	
	if (error_ind) {
		printk(KERN_ERR "Failed to set stop tune\n");
		return -1;
	}	

	/* wait for STC == 0 */
	do {	
		error_ind = si4702_read_reg(SI4702_STATUSRSSI, &si4702_read_data);
		
		if(error_ind) {
			printk(KERN_ERR "Failed to set read STC\n");
			return -1;	
		}
		if ((si4702_read_data & 0x4000) == 0)
			break;
		/* sleep to wait */
		msleep(200);
		
	} while(++loop_counter < DELAY_WAIT);	
		
	/* check loop_counter */
	if (loop_counter >= DELAY_WAIT) {
		printk(KERN_ERR "Can't wait for STC bit set");
		return -1;
	}
#if 1
	/* read RSSI */
	error_ind = si4702_read_reg(SI4702_READCHAN, &si4702_read_data);
	
	if(error_ind) {
		printk(KERN_ERR "Failed to read RSSI\n");
		return -1;
	}
	
	channel = si4702_read_data & 0x03ff;
	printk(KERN_ERR "seek finish: channel(%d)\n", channel);
#endif
	return 0;
}

/*!
  * si4702_channel_seek
  * @param	seek info, direction. 1 seekup
  * @return 
  * 	frequnce been seeked
  * 	-1 	I2C error 
  * 	-2	seek fail
  */
static s32 si4702_channel_seek(s16 dir)
{
	u16 loop_counter = 0;
	u16 si4702_reg_data;
	u8 error_ind = 0;
	s32 seek_error = 0;
	u8 si4702_channel_seek_start[] = {0x41};
	u8 si4702_channel_seek_stop[] = {0x40};
	u32 channel;

	if (si4702_dev.mute) {
		/* check disable mute */
		si4702_channel_seek_start[0] = 0x01;
		si4702_channel_seek_stop[0] = 0x00;
	}
	if (dir) {
		pr_debug("Change to SEEKUP\n");
		si4702_channel_seek_start[0] |= 0x02;
		si4702_channel_seek_stop[0] |= 0x02;
	}
	/* set seek bit */
	error_ind = si4702_read_reg(SI4702_POWERCFG, &si4702_reg_data);
	si4702_reg_data &= 0x0F;
	si4702_reg_data |= si4702_channel_seek_start[0] << 8;
	error_ind = si4702_write_reg(SI4702_POWERCFG, si4702_reg_data);
	
	if (error_ind) {
		printk(KERN_ERR "Failed to set seek start bit\n");
		return -1;
	}

	/* wait STC == 1 */
	do {	
		error_ind = si4702_read_reg(SI4702_STATUSRSSI, &si4702_reg_data);
		if(error_ind) {
			printk(KERN_ERR "Failed to read STC bit\n");
			return -1;	
		}

		if ((si4702_reg_data & 0x4000) != 0) 
			break;
		/* sleep to wait */
		msleep(200);

	} while (++loop_counter < DELAY_WAIT);		

	if (loop_counter >= DELAY_WAIT) {
		printk(KERN_ERR "Can't wait for STC bit set");
		return -1;
	}
	loop_counter = 0;

	/* check whether SF==1 (seek failed bit) */
	if ((si4702_reg_data & 0x2000) != 0) {
		printk(KERN_ERR "Failed to seek any channel\n");
		seek_error = -2;
	}

	/* clear seek bit */
	error_ind = si4702_read_reg(SI4702_POWERCFG, &si4702_reg_data);
	si4702_reg_data &= 0x0F;
	si4702_reg_data |= si4702_channel_seek_stop[0] << 8;
	error_ind = si4702_write_reg(SI4702_POWERCFG, si4702_reg_data);
	
	if (error_ind) {
		printk(KERN_ERR "Failed to stop seek\n");
		return -1;	
	}
	/* wait STC == 0 */
	do {	
		error_ind = si4702_read_reg(SI4702_STATUSRSSI, &si4702_reg_data);
		
		if (error_ind) {
			printk(KERN_ERR "Failed to wait STC bit to clear\n");
			return -1;	
		}
		if ((si4702_reg_data & 0x40) == 0)
			break;
		/* sleep to wait */
		msleep(200);
	} while (++loop_counter < DELAY_WAIT);

	/* check loop_counter */
	if (loop_counter >= DELAY_WAIT) {
		printk(KERN_ERR "Can't wait for STC bit set");
		return -1;
	}

	error_ind = si4702_read_reg(SI4702_READCHAN, &si4702_reg_data);
	
	if (error_ind) {
		printk(KERN_ERR "I2C simulate failed\n");
		return -1;	
	}

	if (seek_error == 0) {
		channel = si4702_reg_data & 0x03ff;
		seek_error = channel * SPACING + BAND;
		printk(KERN_INFO "seek finish: channel(%d), freq(%dKHz)\n", channel, seek_error);
	}
	
	return seek_error;
}

/*!
  * open routing, initial the si4702
  */
static int si4702_open(struct inode *inode, struct file *file)
{

	spin_lock(&count_lock);
	if (count != 0) {
		printk(KERN_ERR "device has been open already\n");
		spin_unlock(&count_lock);
		return -EBUSY;
	}
	count ++;
	spin_unlock(&count_lock);

	/* TODO: detect headphone as RF */

	/* request and active GPIO */
	gpio_si4702_active();
	/* reset the si4702 from it's reset pin */
	gpio_si4702_reset();

	/* startup si4702 */
	if (si4702_startup()) {
		spin_lock(&count_lock);
		count--;
		spin_unlock(&count_lock);
		return -ENODEV;
	}

	/* reset the device info struct */
	si4702_dev.volume = 0x07;
	si4702_dev.channel = 0;
	si4702_dev.mute = 0;

	return 0;
}

/*!
  * release routing, shutdown si4702
  */
static int si4702_release(struct inode *inode, struct file *file)
{
	pr_debug("si4702_release\n");
	/* software shutdown */
	si4702_shutdown();
	/* inactive, free GPIO, cut power */
	gpio_si4702_inactive();

	spin_lock(&count_lock);
	count --;
	spin_unlock(&count_lock);

	return 0;
}

static int si4702_ioctl(struct inode *inode, struct file *file,
		    unsigned int cmd, unsigned long arg)
{
	void __user *argp = (void __user *)arg;
	int mute = 0;
	u16 data;
	int error;

	pr_debug("ioctl, cmd: 0x%x, arg: 0x%lx\n", cmd, arg);

	switch (cmd) {
		case SI4702_SETVOLUME:
		{
			u8 volume;
			int error;
			
			/* get volume from user */
			if (copy_from_user(&volume, argp, sizeof(u8))) {

				printk(KERN_ERR "ioctl, copy volume value from user failed\n");
				return -EFAULT;
			}
			pr_debug("volume %d\n", volume);
			/* refill the register value */
			volume &= 0x0f;
			if (si4702_dev.mute)
				error = si4702_write_reg(SI4702_POWERCFG, 0x0001); 
			else
				error = si4702_write_reg(SI4702_POWERCFG, 0x4001); 

			error = si4702_write_reg(SI4702_CHANNEL, 0); 
			error = si4702_write_reg(SI4702_SYSCONFIG1, 0); 
			error = si4702_write_reg(SI4702_SYSCONFIG2, 0x0f10 | volume); 
			if (error) {
				printk(KERN_ERR "ioctl, set volume failed\n");
				return -EFAULT;
			}
			/* renew the device info */
			si4702_dev.volume = volume;

			break;
		}
		case SI4702_GETVOLUME:
			/* just copy volume value to user */
			if (copy_to_user(argp, &si4702_dev.volume, 
						sizeof(unsigned int))) {
				printk(KERN_ERR "ioctl, copy to user failed\n");
				return -EFAULT;
			}
			break;
		case SI4702_MUTEON:
			mute = 1;
		case SI4702_MUTEOFF:
			if (mute) {
				/* enable mute */
				si4702_read_reg(SI4702_POWERCFG, &data);
				data &= 0x00FF;
				error = si4702_write_reg(SI4702_POWERCFG, data); 
			} else {
				si4702_read_reg(SI4702_POWERCFG, &data);
				data &= 0x00FF;
				data |= 0x4000;
				error = si4702_write_reg(SI4702_POWERCFG, data); 
			}
			if (error) {
				printk(KERN_ERR "ioctl, set mute failed\n");
				return -EFAULT;
			}
			break;
		case SI4702_SELECT:
		{
			unsigned int freq;
			if (copy_from_user(&freq, argp, sizeof(unsigned int))) {

				printk(KERN_ERR "ioctl, copy frequence from user failed\n");
				return -EFAULT;
			}
			/* check frequence */
			if (freq > MAX_BAND || freq < BAND) {
				printk(KERN_ERR "si4702: the frequence select is out of band\n");
				return -EINVAL;
			}
			if (si4702_channel_select(freq)) {
				printk(KERN_ERR "ioctl, failed to select channel\n");
				return -EFAULT;
			}
			break;
		}
		case SI4702_SEEK:
		{
			int dir;
			if (copy_from_user(&dir, argp, sizeof(int))) {

				printk(KERN_ERR "ioctl, copy from user failed\n");
				return -EFAULT;
			}
			/* get seeked channel */
			dir = si4702_channel_seek(dir);
			if (dir == -1) {
				return -EAGAIN;
			} else if (dir == -2) {
				return -EFAULT;
			}
			if (copy_to_user(argp, &dir, sizeof(int))) {
				
				printk(KERN_ERR "ioctl, copy seek frequnce to user failed\n");
				return -EFAULT;
			}
			break;
		}
		default:
			printk(KERN_ERR "SI4702: Invalid ioctl command\n");
			return -EINVAL;

	}
	return 0;
}

static struct file_operations si4702_fops = {
	.owner		= THIS_MODULE,
	.ioctl		= si4702_ioctl,
	.open		= si4702_open,
	.release	= si4702_release,
};


static struct i2c_client *si4702_client = 0;

static int i2c_si4702_attach(struct i2c_adapter *adap);
static int i2c_si4702_detach(struct i2c_client *client);

static struct i2c_driver si4702_i2c_driver = {
	.driver = {
		   .owner = THIS_MODULE,
		   .name = "si4702 driver",
		   },
	.attach_adapter = &i2c_si4702_attach,
	.detach_client = &i2c_si4702_detach,
};

static int si4702_access_reg(char *buf, u16 num, u16 tran_flag)
{
	struct i2c_msg msg[1];
	int ret;
	if(si4702_client == 0)
	{
		printk(KERN_ERR "si4702_client = 0\n");
		return -1;
	}
	msg[0].addr = SI4702_I2C_ADDR;
	msg[0].len = num;
	msg[0].buf = buf;
	msg[0].flags = tran_flag | MXC_I2C_FLAG_POLLING;

	if (tran_flag & MXC_I2C_FLAG_READ) {
		msg[0].flags |= I2C_M_RD;
		memset(si4702_reg_buf, 0, SI4702_REG_BYTE);
	} else {
		msg[0].flags &= ~I2C_M_RD;
	}

	ret = i2c_transfer(si4702_client->adapter, msg, 1);

	if(ret < 0)
		printk(KERN_ERR "register access failed.\n");

	return ret;
}

static int si4702_read_reg(u8 reg, u16 * word)
{
	int ret;
	u8 register1 = reg;
	
	if(register1 >= SI4702_STATUSRSSI && register1 < SI4702_REG_NUM)
		register1 -= SI4702_STATUSRSSI;
	else if(reg < SI4702_STATUSRSSI)
		register1 += SI4702_REG_NUM - SI4702_STATUSRSSI;
	else {
		printk(KERN_ERR "Wrong register number.\n");
		return -1;
	}
	
	ret = si4702_access_reg(&si4702_reg_buf[0], SI4702_REG_BYTE, MXC_I2C_FLAG_READ);

	if(ret < 0)
		return ret;
	
	*word = (si4702_reg_buf[register1 * 2] << 8 )& 0xFF00;
	*word |= si4702_reg_buf[register1 * 2 + 1] & 0x00FF;
	pr_debug("read register %d = 0x%x \n", reg, *word);

	return ret;
}

static int si4702_write_reg(u8 reg, u16 word)
{
	int ret;
	u8 register1 = reg;
	
	if(register1 >= SI4702_POWERCFG && register1 < SI4702_STATUSRSSI)
		register1 -= SI4702_POWERCFG;
	else {
		printk(KERN_ERR "Wrong register number.\n");
		return -1;
	}

	si4702_reg_buf[SI4702_RW_OFFSET * 2 + register1 * 2] = (word & 0xFF00) >> 8;
	si4702_reg_buf[SI4702_RW_OFFSET * 2 + register1 * 2 + 1] = word & 0x00FF;

	pr_debug("###################### write register %d = 0x%x \n", reg, word);
	ret = si4702_access_reg(&si4702_reg_buf[SI4702_RW_OFFSET * 2],
		SI4702_RW_REG_NUM * 2, MXC_I2C_FLAG_POLLING);

	return ret;
}

static void dump_reg(void)
{
	u16 i;
	
	si4702_read_reg(0, &i);

	printk(KERN_INFO "SI4702_DEVICEID	= 0x%x\n", BYTE_TO_WORD(si4702_reg_buf[12], si4702_reg_buf[13]));
	printk(KERN_INFO "SI4702_CHIPID		= 0x%x\n", BYTE_TO_WORD(si4702_reg_buf[14], si4702_reg_buf[15]));
	printk(KERN_INFO "SI4702_POWERCFG 	= 0x%x\n", BYTE_TO_WORD(si4702_reg_buf[16], si4702_reg_buf[17]));
	printk(KERN_INFO "SI4702_CHANNEL 	= 0x%x\n", BYTE_TO_WORD(si4702_reg_buf[18], si4702_reg_buf[19]));
	printk(KERN_INFO "SI4702_SYSCONFIG1 	= 0x%x\n", BYTE_TO_WORD(si4702_reg_buf[20], si4702_reg_buf[21]));
	printk(KERN_INFO "SI4702_SYSCONFIG2 	= 0x%x\n", BYTE_TO_WORD(si4702_reg_buf[22], si4702_reg_buf[23]));
	printk(KERN_INFO "SI4702_SYSCONFIG3 	= 0x%x\n", BYTE_TO_WORD(si4702_reg_buf[24], si4702_reg_buf[25]));
	printk(KERN_INFO "SI4702_TEST1 		= 0x%x\n", BYTE_TO_WORD(si4702_reg_buf[26], si4702_reg_buf[27]));
	printk(KERN_INFO "SI4702_TEST2 		= 0x%x\n", BYTE_TO_WORD(si4702_reg_buf[28], si4702_reg_buf[29]));
	printk(KERN_INFO "SI4702_B00TCONFIG 	= 0x%x\n", BYTE_TO_WORD(si4702_reg_buf[30], si4702_reg_buf[31]));
	printk(KERN_INFO "SI4702_STATUSRSSI 	= 0x%x\n", BYTE_TO_WORD(si4702_reg_buf[0], si4702_reg_buf[1]));
	printk(KERN_INFO "SI4702_READCHAN	= 0x%x\n", BYTE_TO_WORD(si4702_reg_buf[2], si4702_reg_buf[3]));
}

static int i2c_si4702_detect_client(struct i2c_adapter *adapter, int address,
				   int kind)
{
	int ret;	
	struct i2c_client *client;
	u16 id;
	const char *client_name = "si4702 I2C dev";

	pr_debug("si4702: i2c-bus: %s; address: 0x%x\n", adapter->name, address);

	client =
	    kmalloc(sizeof(struct i2c_client) + sizeof(int),
		    GFP_KERNEL);
	if (!client)
	{
		printk(KERN_ERR "client mem allocate failed\n");
		return -ENOMEM;
	}
	memset(client, 0, sizeof(struct i2c_client) + sizeof(int));
	client->addr = address;
	client->adapter = adapter;
	client->driver = &si4702_i2c_driver;
	client->flags = 0;

	strcpy(client->name, client_name);
	pr_debug("si4702: before i2c_attach_client %x, adapter = %x, address = %x\n", 
		(unsigned int)client->dev.driver_data, (unsigned int)adapter, (unsigned int)address);
	i2c_set_clientdata(client, (client + 1));
	
	if (i2c_attach_client(client)) {
		printk(KERN_ERR "si4702: i2c_attach_client() failed.\n");
		kfree(client);
	} else if (si4702_client == 0)
		si4702_client = client;

	/* request and active GPIO */
	gpio_si4702_active();
	/* reset the si4702 from it's reset pin */
	gpio_si4702_reset();

	/*TODO client detection*/
	ret = si4702_read_reg(SI4702_DEVICEID, &id);

	gpio_si4702_inactive();
	if (ret < 0 || id != SI4702_DEVICE_ID) {
		printk(KERN_ERR "si4702: DEVICEID: 0x%x, but it should be 0x%x\n", id, SI4702_DEVICE_ID);
		return 0;
	}
	printk(KERN_ERR "si4702: DEVICEID: 0x%x\n", id);

	return 0;
}


static unsigned short normal_i2c[] = { SI4702_I2C_ADDR, I2C_CLIENT_END };

/* Magic definition of all other variables and things */
I2C_CLIENT_INSMOD;

static int i2c_si4702_attach(struct i2c_adapter *adap)
{
	pr_debug("i2c_si4702_attach\n");
	return i2c_probe(adap, &addr_data, &i2c_si4702_detect_client);
}

static int i2c_si4702_detach(struct i2c_client *client)
{
	int err;

	pr_debug("i2c_si4702_detach\n");
	if ((err = i2c_detach_client(client))) {
		pr_debug("si4702: i2c_detach_client() failed\n");
		return err;
	}

	if (si4702_client == client)
		si4702_client = 0;

	kfree(client);
	return 0;
}

static int __init si4702_init(void)
{
	int err;
	/* initial spin lock */
	spin_lock_init(&count_lock);	

	/* register character device */
	major = register_chrdev(0, SI4702_DEV_NAME, &si4702_fops);
	if (major <= 0) {
		printk(KERN_ERR "SI4702: unable to get major\n");
		return -EIO;
	}

	/* create simple class */
	radio_class = class_create(THIS_MODULE, "radio");
	if (IS_ERR(radio_class)) {
		printk(KERN_ERR "SI4702: failed to create radio class\n");
		goto err1;
	}

	/* add device to this radio class */
	class_device_create(radio_class, NULL, MKDEV(major, 0), NULL,
			"si4702");

	printk(KERN_INFO "SI4702 FM driver\n");

	if ((err = i2c_add_driver(&si4702_i2c_driver))) {
		printk(KERN_ERR "SI4702: driver registration failed\n");
		goto err2;
	}
	
	fm_sys_init();
	return 0;
err2:
	class_destroy(radio_class);
err1:
	unregister_chrdev(major, SI4702_DEV_NAME);
	return -EIO;
}

static void __exit si4702_exit(void)
{
	unregister_chrdev(major, SI4702_DEV_NAME);
	class_device_destroy(radio_class, MKDEV(major, 0));
	class_destroy(radio_class);

	i2c_del_driver(&si4702_i2c_driver);
	fm_sys_exit();
}

enum
{
	FM_SHUTDOWN 	= 0,
	FM_STARTUP	= 1,
	FM_OPEN		= 2,
	FM_RESET	= 3,
	FM_SELECT	= 4,
	FM_REG		= 5,
	FM_READ		= 6,
	FM_WRITE	= 7,
	FM_VOLUP	= 8,
	FM_VOLDOWN	= 9,
	FM_SEEK		= 10,
	FM_SEEK_UP	= 11,
	FM_MUTEON	= 12,
	FM_MUTEDIS	= 13,
	FM_101		= 14,
	FM_103		= 15,
	FM_105		= 16,
	FM_107		= 17,
	FM_CONTROL_MAX
};

static const char * const fm_control[FM_CONTROL_MAX] = {
	[FM_SHUTDOWN]	= "halt",
	[FM_STARTUP]	= "start",
	[FM_OPEN]	= "open",
	[FM_RESET]	= "reset",
	[FM_SELECT]	= "select",
	[FM_REG]	= "reg",
	[FM_READ]	= "read",
	[FM_WRITE]	= "write",
	[FM_VOLUP]	= "vu",
	[FM_VOLDOWN]	= "vd",
	[FM_SEEK]	= "seek",
	[FM_SEEK_UP]	= "seekup",
	[FM_MUTEON]	= "muteon",
	[FM_MUTEDIS]	= "mutedis",
	[FM_101]	= "101",
	[FM_103]	= "103",
	[FM_105]	= "105",
	[FM_107]	= "107"
};

decl_subsys(fm,NULL,NULL);

#define F97_7MHz 	97700
#define F101_7MHz 	101700
#define F103_7MHz 	103700
#define F105_7MHz 	105700
#define F107_7MHz	107700

static int fm_state(unsigned int state)
{
	u16 reg, mute = 0;
	
	switch(state)
	{
	case FM_SHUTDOWN:
		pr_debug("FM_SHUTDOWN\n");
		si4702_shutdown();
		break;
	case FM_STARTUP:
		pr_debug("FM_STARTUP\n");
		si4702_startup();
		break;
	case FM_OPEN:
		pr_debug("FM_OPEN\n");
		si4702_open(0, 0);
		break;		
	case FM_RESET:
		pr_debug("FM_RESET\n");
		gpio_si4702_reset();
		break;
	case FM_SELECT:
		pr_debug("FM_SELECT\n");
		si4702_channel_select(F97_7MHz);
		break;
	case FM_101:
		si4702_channel_select(F101_7MHz);
		break;
	case FM_103:
		si4702_channel_select(F103_7MHz);
		break;
	case FM_105:
		si4702_channel_select(F105_7MHz);
		break;
	case FM_107:
		si4702_channel_select(87900);
		break;
	case FM_REG:
		pr_debug("FM_REG\n");		
		dump_reg();		
		break;
	case FM_READ:
		dump_reg();
		break;
	case FM_WRITE:
		si4702_write_reg(SI4702_POWERCFG, 0x4001);
		break;	
	case FM_VOLUP:
		si4702_dev.volume += 2;
	case FM_VOLDOWN:
		si4702_dev.volume -= 1;
		si4702_dev.volume &= 0xF;
		pr_debug("volume to 0x%x\n", si4702_dev.volume);
		reg = 0x0f10 | si4702_dev.volume;
		si4702_write_reg(SI4702_SYSCONFIG2, reg);
		break;
	case FM_SEEK:
		si4702_channel_seek(0);
		break;
	case FM_SEEK_UP:
		si4702_channel_seek(1);
		break;
	case FM_MUTEON:
		mute = 1;
	case FM_MUTEDIS:
		if (mute) {
			/* enable mute */
			si4702_read_reg(SI4702_POWERCFG, &reg);
			reg &= 0x00FF;
			si4702_write_reg(SI4702_POWERCFG, reg); 
		} else {
			si4702_read_reg(SI4702_POWERCFG, &reg);
			reg &= 0x00FF;
			reg |= 0x4000;
			si4702_write_reg(SI4702_POWERCFG, reg); 
		}
		
		break;
	default:
		pr_debug("default\n");
		break;
	}
	return 0;
}

static ssize_t fm_show(struct subsystem * subsys, char * buf)
{
	return 0;
}

static ssize_t fm_store(struct subsystem * subsys, const char * buf, size_t n)
{
	suspend_state_t state = 0;
	const char * const *s;
	char *p;
	int error;
	int len;

	p = memchr(buf, '\n', n);
	len = p ? p - buf : n;

	for (s = &fm_control[state]; state < FM_CONTROL_MAX; s++, state++) {
		if (*s && !strncmp(buf, *s, len))
			break;
	}
	if (state < FM_CONTROL_MAX && *s)
		error = fm_state(state);
	else
		error = -EINVAL;
	return error ? error : n;
}

si4702_attr(fm);

static struct attribute * g[] = {
	&fm_attr.attr,
	NULL,
};

static struct attribute_group attr_group = {
	.attrs = g,
};

static int fm_suspend(struct platform_device *pdev, pm_message_t state)
{
	return 0;
}

/*!
 * This function is called to correct the system time based on the
 * current MXC RTC time relative to the time delta saved during
 * suspend.
 *
 * @param   pdev  not used
 *
 * @return  The function always returns 0.
 */
static int fm_resume(struct platform_device *pdev)
{
	return 0;
}

/*!
 * Contains pointers to the power management callback functions.
 */
static struct platform_driver fm_driver = {
	.driver = {
		   .name = "si4702",
		   .bus = &platform_bus_type,
		   },
	.suspend = fm_suspend,
	.resume = fm_resume,
};

static int fm_sys_init(void)
{	
	int error; 

	pr_debug("fm sys init\n");
	platform_driver_register(&fm_driver);
		
	error = subsystem_register(&fm_subsys);
	if (!error)
		error = sysfs_create_group(&fm_subsys.kset.kobj,&attr_group);

	memset(si4702_reg_buf, 0, SI4702_REG_BYTE);
	return error;
}

static int fm_sys_exit(void)
{
	pr_debug("fm sys exit\n");
	sysfs_remove_group(&fm_subsys.kset.kobj,&attr_group);
	subsystem_unregister(&fm_subsys);
	platform_driver_unregister(&fm_driver);
		
	return 0;
}

module_init(si4702_init);
module_exit(si4702_exit);

MODULE_AUTHOR("Morninghan Electronics, Inc.");
MODULE_DESCRIPTION("SI4702 FM Radio driver");
MODULE_LICENSE("GPL");
