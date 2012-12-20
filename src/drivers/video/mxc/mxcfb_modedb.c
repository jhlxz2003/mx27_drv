/*
 * Copyright 2006-2007 Freescale Semiconductor, Inc. All Rights Reserved.
 */

/*
 * The code contained herein is licensed under the GNU General Public
 * License. You may obtain a copy of the GNU General Public License
 * Version 2 or later at the following locations:
 *
 * http://www.opensource.org/licenses/gpl-license.html
 * http://www.gnu.org/copyleft/gpl.html
 */

#include <linux/kernel.h>
#include <asm/arch/mxcfb.h>

const struct fb_videomode mxcfb_modedb[] = {
	[0] = {
	       /* 240x320 @ 60 Hz */
	       "Sharp-QVGA", 60, 240, 320, 185925, 9, 16, 7, 9, 1, 1,
	       FB_SYNC_HOR_HIGH_ACT | FB_SYNC_SHARP_MODE |
	       FB_SYNC_CLK_INVERT | FB_SYNC_DATA_INVERT | FB_SYNC_CLK_IDLE_EN,
	       FB_VMODE_NONINTERLACED,
	       0,
	       },
	[1] = {
	       /* 640x480 @ 60 Hz */
	       "NEC-VGA", 60, 640, 480, 38255, 144, 0, 34, 40, 1, 1,
	       FB_SYNC_VERT_HIGH_ACT | FB_SYNC_OE_ACT_HIGH,
	       FB_VMODE_NONINTERLACED,
	       0,
	       },
	[2] = {
	       /* NTSC TV output */
	       "TV-NTSC", 60, 640, 480, 37538,
	       38, 858 - 640 - 38 - 3,
	       36, 518 - 480 - 36 - 1,
	       3, 1,
	       0,
	       FB_VMODE_NONINTERLACED,
	       0,
	       },
	[3] = {
	       /* PAL TV output */
	       "TV-PAL", 50, 640, 480, 37538,
	       38, 960 - 640 - 38 - 32,
	       32, 555 - 480 - 32 - 3,
	       32, 3,
	       0,
	       FB_VMODE_NONINTERLACED,
	       0,
	       },
	[4] = {
	       /* TV output VGA mode, 640x480 @ 65 Hz */
	       "TV-VGA", 60, 640, 480, 40574, 35, 45, 9, 1, 46, 5,
	       0, FB_VMODE_NONINTERLACED, 0,
	       },
	[5] = {
	       /* 480x640 @ 60 Hz */
	       "Epson-VGA", 60, 480, 640, 41701, 60, 41, 10, 5, 20, 10,
	       FB_SYNC_OE_ACT_HIGH,
	       FB_VMODE_NONINTERLACED,
	       0,
	       },
	[6] = {
	       /* 320x240 @ 60 Hz */
	       "SAMSUNG-QVGA", 60, 320, 240, 185925, 14, 147, 8, 16, 19, 3,
	       0,
	       FB_VMODE_NONINTERLACED,
	       0,
	       },
	[7] = {
	       /* 640x480 @ 60 Hz , PAL mode */
	       "TVOUT-PAL", 60, 640, 480, 37594, 179, 179, 25, 25, 2, 2,
	       FB_SYNC_HOR_HIGH_ACT | FB_SYNC_VERT_HIGH_ACT |
	       FB_SYNC_OE_ACT_HIGH,
	       FB_VMODE_NONINTERLACED,
	       0,
	       },
	[8] = {
	       /* 480x640 @ 60 Hz , NTSC mode */
	       "TVOUT-NTSC", 60, 640, 480, 37594, 80, 79, 20, 52, 2, 2,
	       FB_SYNC_HOR_HIGH_ACT | FB_SYNC_VERT_HIGH_ACT |
	       FB_SYNC_OE_ACT_HIGH,
	       FB_VMODE_NONINTERLACED,
	       0,
	       },
#if 0
	[9] = {
	       /* 800x480 @ 60 Hz */
	       "SAMSUNG-WVGA", 60, 800, 480, 60599, 3, 8, 7, 5, 3, 2,
	       FB_SYNC_CLK_INVERT | FB_SYNC_OE_ACT_HIGH,
	       FB_VMODE_NONINTERLACED,
	       0,
	       },
#endif
	[9] = {
	       /* 800x480 @ 60 Hz */
	       "SAMSUNG-WVGA", 60, 800, 480, 40817, 13, 8, 7, 5, 3, 2,
	       FB_SYNC_CLK_INVERT | FB_SYNC_OE_ACT_HIGH,
	       FB_VMODE_NONINTERLACED,
	       0,
	       },

	[10] = {
	       /* 800x480 @ 60 Hz */
	       "HANNSTAR-HSD080", 60, 800, 480, 60599, 13, 29, 40, 40, 48, 3,
	       FB_SYNC_CLK_INVERT | FB_SYNC_OE_ACT_HIGH,
	       FB_VMODE_NONINTERLACED,
	       0,
	       },
};
/*
struct fb_videomode {
	const char *name;
	u32 refresh;
	u32 xres;
	u32 yres;
	u32 pixclock;       //40817,41028
	u32 left_margin;    //13
	u32 right_margin;   //8
	u32 upper_margin;   //7
	u32 lower_margin;   //5
	u32 hsync_len;      //3
	u32 vsync_len;      //2
	u32 sync;
	u32 vmode;
	u32 flag;
};
*/
int mxcfb_modedb_sz = ARRAY_SIZE(mxcfb_modedb);
EXPORT_SYMBOL(mxcfb_modedb);
EXPORT_SYMBOL(mxcfb_modedb_sz);
