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
 * mxc_hw_event.h
 * Collect the hardware events, send to user by netlink
 */

#ifndef _MXC_HW_EVENT_H
#define _MXC_HW_EVENT_H

#define HW_EVENT_GROUP	2
#define HWE_DEF_PRIORITY	1
#define HWE_HIGH_PRIORITY	0

#ifdef __KERNEL__
#include <linux/list.h>
#endif
#include "hw_events.h"

struct mxc_hw_event {
	unsigned int event;
	int args;
};

#ifdef __KERNEL__
struct hw_event_elem {
	struct mxc_hw_event event;
	struct list_head list;
};

extern int hw_event_send(int priority, struct mxc_hw_event new_event);
#endif

#endif				/* _MXC_HW_EVENT_H */
