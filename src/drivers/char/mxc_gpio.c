/*!
 * @file mxc_gpio.c
 *
 * @brief gpio driver
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

static atomic_t gpio_ref;
static unsigned char pin_level[4] = {0};

static DEFINE_MUTEX(gpio_mutex);

static int mxc_gpio_open(struct inode *inode, struct file *file)
{
	int ret;
	mutex_lock(&gpio_mutex);
	if (!atomic_dec_and_test(&gpio_ref)) {
		atomic_inc(&gpio_ref);
		printk(KERN_ALERT "/dev/misc/gpio device busy\n");
		ret = -EBUSY; /* already open */
	} else {
		file->private_data = pin_level;
		ret = 0;
	}
	mutex_unlock(&gpio_mutex);
	return ret;
}

static int mxc_gpio_release(struct inode *inode, struct file *file)
{
	if (file->private_data != pin_level) return -ENODEV;
	file->private_data = NULL;
	atomic_inc(&gpio_ref);
	return 0;
}

static ssize_t
mxc_gpio_read(struct file *file, char __user *data, size_t len, loff_t * ppos)
{
	unsigned char c;
	int retval = 1;

	mutex_lock(&gpio_mutex);
	if (file->private_data != pin_level) {
		retval = -ENODEV;
		goto out;
	}

	if ((file->f_flags & O_ACCMODE) == O_WRONLY) {
		retval = -EPERM;
		goto out;
	}

	if (copy_from_user(&c, data, 1)) {
		retval = -EFAULT;
		goto out;
	}

	if (c > 2) {
		retval = -EINVAL;
		goto out;
	}

	if (copy_to_user(data, &pin_level[c], 1))
		retval = -EFAULT;
out:
	if (retval == 1) {
		printk(KERN_ALERT "--- get gpio-%d: %d ---\n", c, pin_level[c]);
	} else {
		printk(KERN_ALERT "--- get gpio error: %d ---\n", retval);
	}

	mutex_unlock(&gpio_mutex);
	return retval;
}

static ssize_t
mxc_gpio_write(struct file *file, const char __user * data,
	      size_t len, loff_t * ppos)
{
	unsigned char buf[2];
	int retval = 1;

	mutex_lock(&gpio_mutex);
	if (file->private_data != pin_level) {
		retval = -ENODEV;
		goto out;
	}
	if ((file->f_flags & O_ACCMODE) == O_RDONLY) {
		retval = -EPERM;
		goto out;
	}
	
	if (copy_from_user(buf, data, 2)) {
		retval = -EFAULT;
		goto out;
	}
	
	if (buf[0] == 0) {
		mxc_set_gpio_dataout(MX27_PIN_PC_VS2, buf[1]);
		pin_level[0] = buf[1];
	} else if (buf[0] == 1) {
		mxc_set_gpio_dataout(MX27_PIN_PC_VS1, buf[1]);
		pin_level[1] = buf[1];
	} else if (buf[0] == 2) {
		mxc_set_gpio_dataout(MX27_PIN_USBH1_TXDP, buf[1]);
		pin_level[2] = buf[1];
	} /*else if (buf[0] == 3) {
		mxc_set_gpio_dataout(MX27_PIN_CSPI1_SS1, buf[1]);
		pin_level[3] = buf[1];
	} */else {
		retval = -EINVAL;
	}

out:
	if (retval == 1) {
		printk(KERN_ALERT "--- set gpio-%d to %d ---\n", buf[0], buf[1]);
	} else {
		printk(KERN_ALERT "--- set gpio error: %d ---\n", retval);
	}
	mutex_unlock(&gpio_mutex);
	return retval;
}

/*
void
mxc_audio_mute(int v)
{
	mutex_lock(&gpio_mutex);
	mxc_set_gpio_dataout(MX27_PIN_CSPI1_SS1, v);
	mutex_unlock(&gpio_mutex);
}
*/

static int
mxc_gpio_ioctl(struct inode *inode, struct file *file,
	      unsigned int cmd, unsigned long arg)
{
	return -ENODEV;
}

static struct file_operations mxc_gpio_fops = {
	.owner = THIS_MODULE,
	.read = mxc_gpio_read,
	.write = mxc_gpio_write,
	.ioctl = mxc_gpio_ioctl,
	.open = mxc_gpio_open,
	.release = mxc_gpio_release,
};

static struct miscdevice mxc_gpio_miscdev = {
	.minor = GPIO_MINOR,
	.name = "gpio",
	.fops = &mxc_gpio_fops
};

static int __init mxc_gpio_probe(struct platform_device *pdev)
{
	int ret;

	mutex_lock(&gpio_mutex);
	mxc_gpio_miscdev.dev = &pdev->dev;

	gpio_request_mux(MX27_PIN_PC_VS2, GPIO_MUX_GPIO);
	mxc_set_gpio_direction(MX27_PIN_PC_VS2, 0);
	
	gpio_request_mux(MX27_PIN_PC_VS1, GPIO_MUX_GPIO);
	mxc_set_gpio_direction(MX27_PIN_PC_VS1, 0);

	/* RS485_CONTROL */
	gpio_request_mux(MX27_PIN_USBH1_TXDP, GPIO_MUX_GPIO);
	mxc_set_gpio_direction(MX27_PIN_USBH1_TXDP, 0);

/*
	gpio_request_mux(MX27_PIN_CSPI1_SS1, GPIO_MUX_GPIO);
    mxc_set_gpio_direction(MX27_PIN_CSPI1_SS1, 0);
    gpio_set_puen(MX27_PIN_CSPI1_SS1, 0);
*/
	
	atomic_set(&gpio_ref, 1);

	ret = misc_register(&mxc_gpio_miscdev);
	if (ret) {
		printk(KERN_ALERT "--- mxc_gpio_probe failed ---\n");
		mutex_unlock(&gpio_mutex);
		return ret;
	}
	printk(KERN_ALERT "--- mx27_gpio_probe:MX27 GPIO DRIVER:minor:%d ---\n", GPIO_MINOR);
	mutex_unlock(&gpio_mutex);
	return 0;
}

static int __exit mxc_gpio_remove(struct platform_device *pdev)
{
	mutex_lock(&gpio_mutex);
	misc_deregister(&mxc_gpio_miscdev);
	mutex_unlock(&gpio_mutex);
	return 0;
}

static struct platform_driver mxc_gpio_driver = {
	.driver = {
		   .owner = THIS_MODULE,
		   .name = "mxc_gpio",
		   .bus = &platform_bus_type,
		   },
	.probe = mxc_gpio_probe,
	.remove = __exit_p(mxc_gpio_remove),
};

static struct platform_device mxc_gpio_device = {
	.name = "mxc_gpio",
	.id = 0,
};

static int __init mx27_gpio_init(void)
{
	int ret;
	printk(KERN_ALERT "--- mx27_gpio_init ---\n");
	platform_device_register(&mxc_gpio_device);
	ret = platform_driver_register(&mxc_gpio_driver);
	printk(KERN_ALERT "--- platform_driver_register:%d ---\n", ret);
	return ret;
}

static void __exit mx27_gpio_exit(void)
{
	platform_driver_unregister(&mxc_gpio_driver);
}

module_init(mx27_gpio_init);
module_exit(mx27_gpio_exit);

MODULE_AUTHOR("Wishstar, Inc.");
MODULE_LICENSE("GPL");
MODULE_ALIAS_MISCDEV(GPIO_MINOR);
