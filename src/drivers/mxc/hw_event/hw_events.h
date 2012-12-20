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
 * hw_events.h
 * include the headset/cvbs interrupt detect
 */

#ifndef HW_EVENT_H
#define HW_EVENT_H

typedef enum {

	HWE_PHONEJACK_PLUG = 0,
	HWE_BAT_CHARGER_PLUG,
	HWE_BAT_CHARGER_OVERVOLTAGE,
	HWE_BAT_BATTERY_LOW,
	HWE_BAT_POWER_FAILED,
	HWE_BAT_CHARGER_FULL,
	HWE_POWER_KEY,
} HW_EVENT_T;

typedef enum {

	PJT_NONE = 0,
	PJT_CVBS,
	PJT_HEADSET,
} PHONEJACK_TYPE;

typedef enum {

	PWRK_UNPRESS = 0,
	PWRK_PRESS,
} POWERKEY_TYPE;

typedef enum {

	UNPLUG = 0,
	PLUGGED,
} PLUG_TYPE;

#ifdef __KERNEL__
typedef struct {
	int (*check_type) (void);
	int (*get_state) (void);
	void (*setup_irq) (int state);
} jack_conf_t;

typedef struct {
	unsigned int irq;
	unsigned int state;
	int (*check_type) (void);
	int (*get_state) (void);
	void (*setup_irq) (int state);
	struct workqueue_struct *work_queue;
	struct work_struct work;
} mxc_jack_t;
#endif
#endif				/* HW_EVENT_H */
