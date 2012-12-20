/*
 * Copyright 2004-2007 Freescale Semiconductor, Inc. All Rights Reserved.
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
 * hw_events.c
 * include the headset/cvbs interrupt detect
 */
//#define DEBUG
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/sched.h>
#include <linux/signal.h>
#include <linux/platform_device.h>
#include <linux/interrupt.h>
#include <linux/delay.h>
#include <linux/device.h>

#include "hw_events.h"
#include "mxc_hw_event.h"

/*!
 * the global structure of jack
 */
static mxc_jack_t g_mxc_jack;

/*!
 *  Check & update the status of jack
 */
/*
static int update_jack_state(void)
{
	int status;
	struct mxc_hw_event event = { HWE_PHONEJACK_PLUG, 0 };

	status = g_mxc_jack.get_state();

	if (status != g_mxc_jack.state) {
		if (status) {
			status = g_mxc_jack.check_type();
			if (status < 0) {
				return 0;
			}
			g_mxc_jack.state = 1;
			if (status) {
				event.args = PJT_CVBS;
			} else {
				event.args = PJT_HEADSET;
			}
		} else {
			event.args = PJT_NONE;
			g_mxc_jack.state = 0;
		}
		hw_event_send(1, event);
		return 1;
	}
	return 0;
}
*/

static int update_jack_state(void)
{
	int status;
	struct mxc_hw_event event = { HWE_PHONEJACK_PLUG, 0 };

	status = g_mxc_jack.get_state();
    printk(KERN_ALERT "--- LEVEL :%d ---\n", status);
	if (status != g_mxc_jack.state) {
		if (status) {
		    event.args = PJT_NONE;
			g_mxc_jack.state = 1;
			printk(KERN_ALERT "--- UNPLUGGED ---\n");
		} else {
			event.args = PJT_HEADSET;
			g_mxc_jack.state = 0;
			printk(KERN_ALERT "--- PLUGGED ---\n");
		}
		hw_event_send(1, event);
		return 1;
	}
	return 0;
}

/*!
 * the functions of work queue
 */
static void mxc_jack_fun(void *data)
{
	if (update_jack_state()) {
		g_mxc_jack.setup_irq(g_mxc_jack.state);
	}
	enable_irq(g_mxc_jack.irq);
}

/*!
 * jack detect GPIO irq handler
 */

static irqreturn_t mxc_jack_handler(int irq, void *desc)
{
	disable_irq(g_mxc_jack.irq);
//	queue_delayed_work(g_mxc_jack.work_queue, &(g_mxc_jack.work), HZ / 4);
	queue_delayed_work(g_mxc_jack.work_queue, &(g_mxc_jack.work), HZ / 10);
	return IRQ_HANDLED;
}

extern void gpio_jack_active(void);
extern void gpio_jack_inactive(void);
/*!
 *	Active jack monitor
 */
static int mxc_jack_probe(struct platform_device *pdev)
{
	jack_conf_t *jack_data = pdev->dev.platform_data;

	INIT_WORK(&g_mxc_jack.work, mxc_jack_fun, NULL);
	g_mxc_jack.check_type = jack_data->check_type;
	g_mxc_jack.get_state = jack_data->get_state;
	g_mxc_jack.setup_irq = jack_data->setup_irq;
	if (!(g_mxc_jack.check_type && g_mxc_jack.get_state
	     && g_mxc_jack.setup_irq)) {
		pr_info("Jack get invalid device configuration \n");
		return -EINVAL;
	}
	g_mxc_jack.state = 1;
	g_mxc_jack.work_queue = create_singlethread_workqueue("mxc_jack_handler");
	if (g_mxc_jack.work_queue == NULL) {
		printk(KERN_INFO " Create workqueue fail for detecting jack\n");
		return -EFAULT;
	}

    gpio_jack_active();
	update_jack_state();
	g_mxc_jack.setup_irq(g_mxc_jack.state);

	g_mxc_jack.irq = platform_get_irq(pdev, 0);

	if (request_irq(g_mxc_jack.irq, mxc_jack_handler, IRQF_TRIGGER_FALLING, "mxc_jack_handler", NULL)) {
		if (g_mxc_jack.work_queue) {
			flush_workqueue(g_mxc_jack.work_queue);
			destroy_workqueue(g_mxc_jack.work_queue);
			g_mxc_jack.work_queue = NULL;
		}

		printk(KERN_INFO "Request irq %x fails for detecting jack\n", g_mxc_jack.irq);
        gpio_jack_inactive();
		return -EFAULT;
	}
	disable_irq(g_mxc_jack.irq);
	enable_irq(g_mxc_jack.irq);
	return 0;
}

/*!
 * Inactive jack monitor
 */
static int mxc_jack_remove(struct platform_device *pdev)
{
	gpio_jack_inactive();
	if (g_mxc_jack.irq) {
		free_irq(g_mxc_jack.irq, NULL);
		g_mxc_jack.irq = 0;
	}
	cancel_delayed_work(&g_mxc_jack.work);
	flush_scheduled_work();
	if (g_mxc_jack.work_queue) {
		flush_workqueue(g_mxc_jack.work_queue);
		destroy_workqueue(g_mxc_jack.work_queue);
		g_mxc_jack.work_queue = NULL;
	}
	return 0;
}

/*!
 * Jack monitor driver
 */
static struct platform_driver mxc_jack_driver = {
	.driver = {
		   .name = "mxc_jack",
		   .bus = &platform_bus_type,
		   },
	.suspend = NULL,
	.resume = NULL,
	.probe = mxc_jack_probe,
	.remove = mxc_jack_remove
};

/*!
 * Jack module initilized function
 */
static int __init hw_events_init(void)
{
	pr_info("Jack Monitor loaded\n");
	return platform_driver_register(&mxc_jack_driver);
};

/*!
 * Jack module destory function
 */
static void __exit hw_events_exit(void)
{
	platform_driver_unregister(&mxc_jack_driver);
}

module_init(hw_events_init);
module_exit(hw_events_exit);
MODULE_LICENSE("GPL");
