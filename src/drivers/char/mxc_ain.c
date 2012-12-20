/*!
 * @file mxc_in.c
 *
 * @brief alarm-in driver
 */
#include <linux/module.h>
#include <linux/types.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/mm.h>
#include <linux/smp_lock.h>
#include <linux/init.h>
#include <linux/cdev.h>
#include <linux/poll.h>
#include <linux/device.h>
#include <linux/irq.h>
#include <linux/err.h>
#include <linux/interrupt.h>
#include <linux/moduleparam.h>
#include <linux/workqueue.h>
#include <linux/mxcain.h>  /* add on 2011.10.29 by LXZ */

#include <asm/io.h>
#include <asm/uaccess.h>
#include <asm/atomic.h>
#include <asm/hardware.h>
#include <asm/irq.h>
#include <asm/bitops.h>

#include <asm/hardware.h>
#include <asm/arch/gpio.h>
#include "gpio_mux.h"

/*
#define AI_IOC_MAGIC  0xfa

#define AI_IOCGLVL   _IOR(AI_IOC_MAGIC, 0, unsigned int)
#define AI_IOCSTMEO  _IOW(AI_IOC_MAGIC, 1, unsigned int)
#define AI_IOC_MAXNR  2
*/

#define INT_OAIN0   IOMUX_TO_IRQ(MX27_PIN_PC_BVD2)
#define INT_OAIN1   IOMUX_TO_IRQ(MX27_PIN_PC_BVD1)
#define INT_OAIN2   IOMUX_TO_IRQ(MX27_PIN_PC_PWRON)

#define DEVICE_NAME        "mxc_ain"

#define MXCAIN_MAJOR       0

#define EV_BUF_SZ          32
#define MAX_AIN_MINORS     3
#define UP_TIMEOUT        (HZ/25)

typedef struct _ain_device {
	unsigned int idx;
	iomux_pin_name_t  pin;
	int irq;
	atomic_t  ref;
	int count;
	unsigned int tmeo;
	struct fasync_struct *fasync;
	int wptr;
	int rptr;
	struct ievent buffer[EV_BUF_SZ];

	struct work_struct  work;
	wait_queue_head_t wait;
	struct cdev cdev;
} ain_dev_t;

static ain_dev_t  ain_dev[MAX_AIN_MINORS] = {
	{
		.idx = 0,
		.pin = MX27_PIN_PC_BVD2,
		.irq = INT_OAIN0,
		.ref = ATOMIC_INIT(1),
		.count = 0,
		.tmeo = UP_TIMEOUT,
		.fasync = NULL,
		.wptr = 0,
		.rptr = 0,
	},
	{
		.idx = 1,
		.pin = MX27_PIN_PC_BVD1,
		.irq = INT_OAIN1,
		.ref = ATOMIC_INIT(1),
		.count = 0,
		.tmeo = UP_TIMEOUT,
		.fasync = NULL,
		.wptr = 0,
		.rptr = 0,
	},
	{
		.idx = 2,
		.pin = MX27_PIN_PC_PWRON,
		.irq = INT_OAIN2,
		.ref = ATOMIC_INIT(1),
		.count = 0,
		.tmeo = UP_TIMEOUT,
		.fasync = NULL,
		.wptr = 0,
		.rptr = 0,
	},
};

static dev_t  ain_dev_number;
static struct class *ain_class;

/* DONE */
static void
ain_work(void *arg)
{
	ain_dev_t *dev = arg;
	int dn;

	dn = 1 - mxc_get_gpio_ssr(dev->pin);
	if (dn) {
		if (dev->count == 1) {
			printk(KERN_ALERT "--- mxcain%d pressed ---\n", dev->idx);

			do_gettimeofday(&dev->buffer[dev->wptr].time);
			dev->buffer[dev->wptr].value = EVT_DN;

			dev->wptr = (dev->wptr + 1) & (EV_BUF_SZ - 1);
			if (dev->fasync)
				kill_fasync(&dev->fasync, SIGIO, POLL_IN);
			wake_up_interruptible(&dev->wait);
		}
		dev->count++;
		schedule_delayed_work(&dev->work, dev->tmeo);
	} else {
		if (dev->count) {
			printk(KERN_ALERT "--- mxcain%d released ---\n", dev->idx);

			do_gettimeofday(&dev->buffer[dev->wptr].time);
			dev->buffer[dev->wptr].value = EVT_UP;

			dev->wptr = (dev->wptr + 1) & (EV_BUF_SZ - 1);
			if (dev->fasync)
				kill_fasync(&dev->fasync, SIGIO, POLL_IN);
			wake_up_interruptible(&dev->wait);
		}
		enable_irq(dev->irq);
	}
}

/* DONE */
static irqreturn_t
ain_irq_handler(int irq, void *data)
{
	ain_dev_t *dev = data;

	disable_irq_nosync(irq);
	printk(KERN_ALERT "--- ain irq:%d ---\n", irq);
	dev->count = 0;
	schedule_work(&dev->work);
	return IRQ_HANDLED;
}

/* DONE */
static int ain_fasync(int fd, struct file *file, int on)
{
	int retval;
	ain_dev_t *dev = file->private_data;

	retval = fasync_helper(fd, file, on, &dev->fasync);

	return retval < 0 ? retval : 0;
}

/* DONE */
static int ain_open(struct inode *inode, struct file *file)
{
	int ret = 0;
	ain_dev_t *dev;
	unsigned int minor = iminor(inode);
	
	dev = &ain_dev[minor];
	if (!atomic_dec_and_test(&dev->ref)) {
		atomic_inc(&dev->ref);
		printk(KERN_ALERT "/dev/mxcain%d device busy\n", minor);
		return -EBUSY;
	}

	ret = request_irq(dev->irq, ain_irq_handler, IRQF_TRIGGER_FALLING, DEVICE_NAME, dev);
	if (ret)
	{ 
		printk(KERN_ERR "%s: cannot grab irq %d\n", __FUNCTION__, dev->irq);
		atomic_inc(&dev->ref);
		return ret;
	}
	file->private_data = dev;
	enable_irq(dev->irq);
	printk(KERN_ERR "---- %s: /dev/mxcain%d device opened, irq = %d ----\n", __FUNCTION__, minor, dev->irq);
	return ret;
}

/* DONE */
static int ain_release(struct inode *inode, struct file *file)
{
	int ret = 0;
	ain_dev_t *dev;
	unsigned int minor = iminor(inode);

	dev = &ain_dev[minor];
	if (file->private_data != dev) return -ENODEV;
	ain_fasync(-1, file, 0);
	file->private_data = NULL;
	disable_irq(dev->irq);
	free_irq(dev->irq, dev);
	atomic_inc(&dev->ref);

	return ret;
}

/* DONE */
static ssize_t
ain_read(struct file *file, char __user *data, size_t count, loff_t * ppos)
{
	int retval;
	ain_dev_t *dev = file->private_data;

	if (dev == NULL) return -ENODEV;
	if ((file->f_flags & O_ACCMODE) != O_RDONLY) return -EPERM;
	if ((dev->wptr == dev->rptr) && (file->f_flags & O_NONBLOCK)) return -EAGAIN;

	retval = wait_event_interruptible(dev->wait, dev->wptr != dev->rptr);
	if (retval) return retval;

	while ((dev->wptr != dev->rptr) && (retval+sizeof(struct ievent) <= count)) {
		if (copy_to_user(data+retval, &dev->buffer[dev->rptr], sizeof(struct ievent)))
			return -EFAULT;
		dev->rptr = (dev->rptr + 1)&(EV_BUF_SZ - 1);
		retval += sizeof(struct ievent);
	}

	return retval;
}

/* DONE */
static unsigned int ain_poll(struct file *file, poll_table *wait)
{
	ain_dev_t *dev = file->private_data;

	if (dev == NULL) return (POLLHUP | POLLERR);
	poll_wait(file, &dev->wait, wait);
	return ((dev->wptr == dev->rptr) ? 0 : (POLLIN | POLLRDNORM));
}

/* DONE */
static int
ain_ioctl(struct inode *inode, struct file *file, unsigned int cmd, unsigned long arg)
{
	ain_dev_t *dev = file->private_data;
	unsigned int minor = iminor(inode);
	int retval = 0;
	int v;

	if (dev != &ain_dev[minor]) return -ENODEV;
	if (_IOC_TYPE(cmd) != AI_IOC_MAGIC) return -ENOTTY;
	if (_IOC_NR(cmd) > AI_IOC_MAXNR) return -ENOTTY;

	if (_IOC_DIR(cmd) & _IOC_READ)
		retval = !access_ok(VERIFY_WRITE, (void __user *)arg, _IOC_SIZE(cmd));
	else if (_IOC_DIR(cmd) & _IOC_WRITE)
		retval =  !access_ok(VERIFY_READ, (void __user *)arg, _IOC_SIZE(cmd));
	if (retval) return -EFAULT;

	switch (cmd) {
	case AI_IOCGLVL:
		v = mxc_get_gpio_ssr(dev->pin);
//		if (copy_to_user((void __user *)arg, &v, sizeof(v)))
		if (__put_user(v, (int __user *)arg))
			retval = -EFAULT;
		break;
	case AI_IOCSTMEO:
//		if (copy_from_user (&v, (void __user *)arg, sizeof(v)))
		if (__get_user(v, (int __user *)arg))
			retval = -EFAULT;
		dev->tmeo = v;
		break;
	default:
		retval = -EINVAL;
		break;
	}

	return retval;
}

//////////////////////////////////////////
static struct file_operations ain_fops = {
	.owner = THIS_MODULE,
	.read = ain_read,
	.ioctl = ain_ioctl,
	.poll =	ain_poll,
	.open = ain_open,
	.release = ain_release,
	.fasync =	ain_fasync,
};

/* DONE */
static void
ain_uninit(int n)
{
	int i;
	ain_dev_t *dev;

  for (i=n-1; i>=0; --i) {
  	dev = &ain_dev[i];
    class_device_destroy(ain_class, ain_dev_number+i);
    cdev_del(&dev->cdev);
    mxc_free_gpio(dev->pin);
  }

  class_destroy(ain_class);
  unregister_chrdev_region(ain_dev_number, MAX_AIN_MINORS);
}

/* DONE */
static int __init mxc_ain_init(void)
{
	int i;
	int result;
	int ain_major = MXCAIN_MAJOR;
	ain_dev_t *dev;

  /* Request dynamic allocation of a device major number */
	if (ain_major) {
		ain_dev_number = MKDEV(ain_major, 0);
		result = register_chrdev_region(ain_dev_number, MAX_AIN_MINORS, DEVICE_NAME);
	} else {
		result = alloc_chrdev_region(&ain_dev_number, 0, MAX_AIN_MINORS, DEVICE_NAME);
		ain_major = MAJOR(ain_dev_number);
	}

	if (result < 0) {
		printk(KERN_ALERT "mxc_ain: can't get major %d\n", ain_major);
		return result;
	}

  ain_class = class_create(THIS_MODULE, DEVICE_NAME);
  if (IS_ERR(ain_class)) {
		printk(KERN_ERR "Error creating ain class.\n");
		unregister_chrdev_region(ain_dev_number, MAX_AIN_MINORS);
		return PTR_ERR(ain_class);
	}

	printk(KERN_ALERT "mxc_ain: get major %d\n", ain_major);

  for (i=0; i<MAX_AIN_MINORS; i++) {
    dev = &ain_dev[i];
    cdev_init(&dev->cdev, &ain_fops);
    dev->cdev.owner = THIS_MODULE;

    if (cdev_add(&dev->cdev, ain_dev_number+i, 1)) {
      printk(KERN_ALERT "--- cdev_add() error ---\n");
      ain_uninit(i);
      return -1;
    }

		gpio_request_mux(dev->pin, GPIO_MUX_GPIO);
		mxc_set_gpio_direction(dev->pin, 1);
		set_irq_type(dev->irq, IRQT_FALLING);
		INIT_WORK(&dev->work, ain_work, dev);
		init_waitqueue_head(&dev->wait);
		memset(dev->buffer, 0, EV_BUF_SZ*sizeof(struct ievent));

		class_device_create(ain_class, NULL, (ain_dev_number + i), NULL, "mxcain%d", i);
	}

	printk("MXCAIN Driver Initialized.\n");
	return 0;
}

/* DONE */
static void __exit mxc_ain_exit(void)
{
	ain_uninit(MAX_AIN_MINORS);
}

module_init(mxc_ain_init);
module_exit(mxc_ain_exit);

MODULE_AUTHOR("Wishstar, Inc.");
MODULE_LICENSE("GPL");
