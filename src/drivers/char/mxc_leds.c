/*!
 * @file mxc_leds.c
 *
 * @brief leds driver
 */

#include <linux/module.h>
#include <linux/types.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/mm.h>
#include <linux/miscdevice.h>
#include <linux/reboot.h>
#include <linux/smp_lock.h>
#include <linux/init.h>
#include <linux/err.h>
#include <linux/platform_device.h>
#include <linux/interrupt.h>
#include <linux/moduleparam.h>
#include <linux/mutex.h>

#include <asm/io.h>
#include <asm/uaccess.h>
#include <asm/atomic.h>
#include <asm/hardware.h>
#include <asm/irq.h>
#include <asm/bitops.h>

#include <asm/hardware.h>
#include <asm/arch/gpio.h>
#include "gpio_mux.h"

static atomic_t leds_ref;

static iomux_pin_name_t  leds_pin[] = {MX27_PIN_PC_RW_B, MX27_PIN_PC_POE, MX27_PIN_PWMO};
static unsigned char pin_st = 7;

static DEFINE_MUTEX(leds_mutex);

/* DONE */
static int mxc_leds_open(struct inode *inode, struct file *file)
{
	int ret;
	mutex_lock(&leds_mutex);
	if (!atomic_dec_and_test(&leds_ref)) {
		atomic_inc(&leds_ref);
		printk(KERN_ALERT "/dev/misc/leds device busy\n");
		ret = -EBUSY; /* already open */
	} else {
		file->private_data = &pin_st;
		ret = 0;
	}
	mutex_unlock(&leds_mutex);
	return ret;
}

/* DONE */
static int mxc_leds_release(struct inode *inode, struct file *file)
{
	if (file->private_data != &pin_st) return -ENODEV;
	file->private_data = NULL;
	atomic_inc(&leds_ref);
	return 0;
}

/* DONE */
static ssize_t
mxc_leds_read(struct file *file, char __user *data, size_t len, loff_t * ppos)
{
	int retval = 1;

	mutex_lock(&leds_mutex);
	if (file->private_data != &pin_st) {
		mutex_unlock(&leds_mutex);
		return -ENODEV;
	}

	if ((file->f_flags & O_ACCMODE) == O_WRONLY) {
		mutex_unlock(&leds_mutex);
		return -EPERM;
	}

	if (copy_to_user(data, &pin_st, 1)) {
		mutex_unlock(&leds_mutex);
		return -EFAULT;
	}

	mutex_unlock(&leds_mutex);
	return retval;
}

static ssize_t
mxc_leds_write(struct file *file, const char __user * data, size_t len, loff_t * ppos)
{
	unsigned char val;
	int i;

	mutex_lock(&leds_mutex);
	if (file->private_data != &pin_st) {
		mutex_unlock(&leds_mutex);
		return -ENODEV;
	}
	if ((file->f_flags & O_ACCMODE) == O_RDONLY) {
		mutex_unlock(&leds_mutex);
		return -EPERM;
	}
	
	if (copy_from_user(&val, data, 1)) {
		mutex_unlock(&leds_mutex);
		return -EFAULT;
	}
	
	if (val > 7) {
		mutex_unlock(&leds_mutex);
		return -EINVAL;
	}

	for (i = 0; i < 3; i++) {
		if ((val>>i)&0x01) {
			mxc_set_gpio_dataout(leds_pin[i], 1);
		} else {
			mxc_set_gpio_dataout(leds_pin[i], 0);
		}
	}
	pin_st = val;
	mutex_unlock(&leds_mutex);

	return 1;
}

static int
mxc_leds_ioctl(struct inode *inode, struct file *file,
	      unsigned int cmd, unsigned long arg)
{
	return -ENODEV;
}

static struct file_operations mxc_leds_fops = {
	.owner = THIS_MODULE,
	.read = mxc_leds_read,
	.write = mxc_leds_write,
	.ioctl = mxc_leds_ioctl,
	.open = mxc_leds_open,
	.release = mxc_leds_release,
};

static struct miscdevice mxc_leds_miscdev = {
	.minor = LEDS_MINOR,
	.name = "leds",
	.fops = &mxc_leds_fops
};

static int __init mxc_leds_probe(struct platform_device *pdev)
{
	int ret;

	mutex_lock(&leds_mutex);
	mxc_leds_miscdev.dev = &pdev->dev;

	gpio_request_mux(MX27_PIN_PWMO, GPIO_MUX_GPIO);
	mxc_set_gpio_direction(MX27_PIN_PWMO, 0);
	
	gpio_request_mux(MX27_PIN_PC_POE, GPIO_MUX_GPIO);
	mxc_set_gpio_direction(MX27_PIN_PC_POE, 0);

	gpio_request_mux(MX27_PIN_PC_RW_B, GPIO_MUX_GPIO);
	mxc_set_gpio_direction(MX27_PIN_PC_RW_B, 0);
	
	atomic_set(&leds_ref, 1);

	ret = misc_register(&mxc_leds_miscdev);
	if (ret) {
		printk(KERN_ALERT "--- mxc_leds_probe failed ---\n");
		mutex_unlock(&leds_mutex);
		return ret;
	}
	printk(KERN_ALERT "--- mx27_gpio_probe:MX27 GPIO DRIVER:minor:%d ---\n", LEDS_MINOR);
	mutex_unlock(&leds_mutex);
	return 0;
}

static int __exit mxc_leds_remove(struct platform_device *pdev)
{
	mutex_lock(&leds_mutex);
	gpio_free_mux(MX27_PIN_PWMO);
	gpio_free_mux(MX27_PIN_PC_POE);
	gpio_free_mux(MX27_PIN_PC_RW_B);
	misc_deregister(&mxc_leds_miscdev);
	mutex_unlock(&leds_mutex);
	return 0;
}

static struct platform_driver mxc_leds_driver = {
	.driver = {
		   .owner = THIS_MODULE,
		   .name = "mxc_leds",
		   .bus = &platform_bus_type,
		   },
	.probe = mxc_leds_probe,
	.remove = __exit_p(mxc_leds_remove),
};

static struct platform_device mxc_leds_device = {
	.name = "mxc_leds",
	.id = 0,
};

static int __init mx27_leds_init(void)
{
	int ret;
	printk(KERN_ALERT "--- mx27_leds_init ---\n");
	platform_device_register(&mxc_leds_device);
	ret = platform_driver_register(&mxc_leds_driver);
	printk(KERN_ALERT "--- platform_driver_register:%d ---\n", ret);
	return ret;
}

static void __exit mx27_leds_exit(void)
{
	platform_driver_unregister(&mxc_leds_driver);
}

module_init(mx27_leds_init);
module_exit(mx27_leds_exit);

MODULE_AUTHOR("Wishstar, Inc.");
MODULE_LICENSE("GPL");
MODULE_ALIAS_MISCDEV(LEDS_MINOR);
