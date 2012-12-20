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
 * mxc_hw_event.c
 * Collect the hardware events, send to user by netlink
 */

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/netlink.h>
#include <linux/sched.h>
#include <linux/list.h>
#include <linux/signal.h>
#include <net/sock.h>

#include "mxc_hw_event.h"

#define EVENT_POOL_SIZE	10

static struct sock *nl_event_sock;	/* netlink socket */
static struct list_head event_head;
static struct list_head free_head;
static struct hw_event_elem events_pool[EVENT_POOL_SIZE];	/* event pool */
static DECLARE_COMPLETION(exit_completion);
static spinlock_t list_lock = SPIN_LOCK_UNLOCKED;
static DECLARE_WAIT_QUEUE_HEAD(event_wq);
static unsigned int seq = 0;	/* send seq */
static int running = 1;
static pid_t pid;

/*!
 * main HW event handler thread
 */
static int hw_event_thread(void *data)
{
	struct sk_buff *skb = NULL;
	struct nlmsghdr *nlh = NULL;
	unsigned int size;
	struct hw_event_elem *event, *n;
	LIST_HEAD(tmp_head);

	/* make this thread daemon, and assign thread name */
	daemonize("mxc_hw_event");

	while (running) {

		try_to_freeze();
		/* wait for event coming */
		if (wait_event_interruptible
		    (event_wq, !list_empty(&event_head))) {
			/* signal cause */
			flush_signals(current);
			continue;
		}

		/* fetch event from list */
		spin_lock_irq(&list_lock);
		tmp_head = event_head;
		tmp_head.prev->next = &tmp_head;
		tmp_head.next->prev = &tmp_head;
		/* clear the event list head */
		INIT_LIST_HEAD(&event_head);
		spin_unlock_irq(&list_lock);

		list_for_each_entry_safe(event, n, &tmp_head, list) {

			size = NLMSG_SPACE(sizeof(struct mxc_hw_event));
			skb = alloc_skb(size, GFP_KERNEL);
			if (!skb) {
				/* if failed alloc skb, we drop this event */
				printk(KERN_WARNING
				       "mxc_hw_event: skb_alloc() failed\n");
				goto alloc_failure;
			}

			/* put the netlink header struct to skb */
			nlh =
			    NLMSG_PUT(skb, 0, seq++, NLMSG_DONE,
				      size - sizeof(*nlh));

			/* fill the netlink data */
			memcpy((struct mxc_hw_event *)NLMSG_DATA(nlh),
			       &event->event, sizeof(struct mxc_hw_event));

			/* free the event node, set to unused */
			spin_lock_irq(&list_lock);
			list_move(&event->list, &free_head);
			spin_unlock_irq(&list_lock);

			/* send to all process that create this socket */
			NETLINK_CB(skb).pid = 0;	/* sender pid */
			NETLINK_CB(skb).dst_group = HW_EVENT_GROUP;
			/* broadcast the event */
			netlink_broadcast(nl_event_sock, skb, 0, HW_EVENT_GROUP,
					  GFP_KERNEL);

			continue;
		      nlmsg_failure:
			printk(KERN_WARNING
			       "mxc_hw_event: No tailroom for NLMSG in skb\n");
		      alloc_failure:
			/* free the event node, set to unused */
			spin_lock_irq(&list_lock);
			list_del(&event->list);
			list_add_tail(&event->list, &free_head);
			spin_unlock_irq(&list_lock);
		}
	}

	complete(&exit_completion);
	return 0;
}

/*!
 *
 * @priority the event priority, REALTIME, EMERENCY, NORMAL
 * @new_event event id to be send
 */
int hw_event_send(int priority, struct mxc_hw_event new_event)
{
	unsigned int size;
	struct sk_buff *skb = NULL;
	struct nlmsghdr *nlh = NULL;
	struct mxc_hw_event *event;
	struct hw_event_elem *event_elem;
	int ret;
	unsigned long flag;
	struct list_head *list_node;

	if (priority == HWE_HIGH_PRIORITY) {
		/**
		 * the most high priority event,
		 * we send it immediatly.
		 */

		size = NLMSG_SPACE(sizeof(struct mxc_hw_event));

		/* alloc skb */
		if (in_interrupt()) {
			skb = alloc_skb(size, GFP_ATOMIC);
		} else {
			skb = alloc_skb(size, GFP_KERNEL);
		}
		if (!skb) {
			/* if failed alloc skb, we drop this event */
			printk(KERN_WARNING
			       "hw_event send: skb_alloc() failed\n");
			goto send_later;
		}

		/* put the netlink header struct to skb */
		nlh = NLMSG_PUT(skb, 0, seq++, NLMSG_DONE, size - sizeof(*nlh));

		/* fill the netlink data */
		event = (struct mxc_hw_event *)NLMSG_DATA(nlh);
		memcpy(event, &new_event, sizeof(struct mxc_hw_event));

		/* send to all process that create this socket */
		NETLINK_CB(skb).pid = 0;	/* sender pid */
		NETLINK_CB(skb).dst_group = HW_EVENT_GROUP;
		/* broadcast the event */
		ret = netlink_broadcast(nl_event_sock, skb, 0, HW_EVENT_GROUP,
					in_interrupt()? GFP_ATOMIC :
					GFP_KERNEL);
		if (ret) {

		      nlmsg_failure:
			/* send failed */
			kfree_skb(skb);
			goto send_later;
		}

		return 0;
	}

      send_later:
	spin_lock_irqsave(&list_lock, flag);
	if (list_empty(&free_head)) {
		spin_unlock_irqrestore(&list_lock, flag);
		/* no more free event node */
		printk(KERN_WARNING "mxc_event send: no more free node\n");
		return -1;
	}

	/* get a free node from free list, and added to event list */
	list_node = free_head.next;
	/* fill event */
	event_elem = list_entry(list_node, struct hw_event_elem, list);
	event_elem->event = new_event;
	list_move(list_node, &event_head);
	spin_unlock_irqrestore(&list_lock, flag);

	wake_up(&event_wq);

	return 1;
}

static int __init mxc_hw_event_init(void)
{
	int i;

	/* initial the list head for event and free */
	INIT_LIST_HEAD(&free_head);
	INIT_LIST_HEAD(&event_head);

	/* initial the free list */
	for (i = 0; i < EVENT_POOL_SIZE; i++) {
		memset(&events_pool[i], 0, sizeof(struct hw_event_elem));
		list_add_tail(&events_pool[i].list, &free_head);
	}

	/* create netlink kernel sock */
	nl_event_sock =
	    netlink_kernel_create(NETLINK_USERSOCK, 0, NULL, THIS_MODULE);
	if (!nl_event_sock) {
		printk("mxc_hw_event: Fail to create netlink socket.\n");
		return 1;
	}
	pid = kernel_thread(hw_event_thread, NULL, CLONE_KERNEL);

	return 0;
}

static void __exit mxc_hw_event_exit(void)
{
	struct task_struct *task;

	running = 0;
	/* force signal to thread */
	task = find_task_by_pid(pid);
	if (!task) {
		panic("FATAL error, can not find task(%d)\n", pid);
	}
	force_sig(SIGSTOP, task);
	/* wait for thread completion */
	wait_for_completion(&exit_completion);
	sock_release(nl_event_sock->sk_socket);
}

module_init(mxc_hw_event_init);
module_exit(mxc_hw_event_exit);

EXPORT_SYMBOL(hw_event_send);
MODULE_LICENSE("GPL");
