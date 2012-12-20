#ifndef _MXCAIN_H
#define _MXCAIN_H

/*
 * Copyright (c) 1999-2002 Vojtech Pavlik
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License version 2 as published by
 * the Free Software Foundation.
 */

#ifdef __KERNEL__
#include <linux/time.h>
#include <linux/list.h>
#else
#include <sys/time.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <asm/types.h>
#endif

struct ievent {
	struct timeval time;
	__s32 value;
};

#define AI_IOC_MAGIC  0xfa

#define AI_IOCGLVL   _IOR(AI_IOC_MAGIC, 0, unsigned int)
#define AI_IOCSTMEO  _IOW(AI_IOC_MAGIC, 1, unsigned int)
#define AI_IOC_MAXNR  2

#define EVT_DN        0
#define EVT_UP        1

#endif
