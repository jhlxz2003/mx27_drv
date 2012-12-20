/*
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
 * @file tvp5150.c
 *
 * @brief TVP5150 TVin driver functions
 *
 * @ingroup Camera
 */

#include <linux/module.h>
#include <linux/init.h>
#include <linux/slab.h>
#include <linux/ctype.h>
#include <linux/types.h>
#include <linux/delay.h>
#include <linux/device.h>
#include <linux/i2c.h>
#include <linux/clk.h>
#include <asm/arch/mxc_i2c.h>
#include "mxc_v4l2_capture.h"

#include "tvp5150_reg.h"

#define TVP5150_I2C_ADDRESS	    0x5D
#define TVP5150_CHIP_VERSION	0x04

struct i2c_reg_value {
	u8 addr;
	u8 value;
};

static int tvp5150_i2c_client_xfer(unsigned int addr, u8 reg, char *buf, int num, int tran_flag);

#define tvp5150_write_buffer(reg, buf, size)  \
        tvp5150_i2c_client_xfer(TVP5150_I2C_ADDRESS, reg, buf, size, 0)

static sensor_interface *interface_param = NULL;
extern cam_data *g_cam;

static int  tvp5150_read_reg(u8 reg, u8 *val);
static int  tvp5150_write_reg(u8 reg, u8 val);
static void tvp5150_set_channel(int channel);

static int g_bri = 0x80;
static int g_hue = 0x80;
static int g_sat = 0x80;
static int g_con = 0x80;

static u8  msb_id;
static u8  lsb_id;
static u8  msb_rom;
static u8  lsb_rom;

static v4l2_std_id g_std = V4L2_STD_ALL;

#include "tvp5150a_048caa.c"

static struct v4l2_queryctrl tvp5150_qctrl[] = {
	{
		.id = V4L2_CID_BRIGHTNESS,
		.type = V4L2_CTRL_TYPE_INTEGER,
		.name = "Brightness",
		.minimum = 0,
		.maximum = 255,
		.step = 1,
		.default_value = 128,
		.flags = 0,
	}, {
		.id = V4L2_CID_CONTRAST,
		.type = V4L2_CTRL_TYPE_INTEGER,
		.name = "Contrast",
		.minimum = 0,
		.maximum = 255,
		.step = 0x1,
		.default_value = 128,
		.flags = 0,
	}, {
		 .id = V4L2_CID_SATURATION,
		 .type = V4L2_CTRL_TYPE_INTEGER,
		 .name = "Saturation",
		 .minimum = 0,
		 .maximum = 255,
		 .step = 0x1,
		 .default_value = 128,
		 .flags = 0,
	}, {
		.id = V4L2_CID_HUE,
		.type = V4L2_CTRL_TYPE_INTEGER,
		.name = "Hue",
		.minimum = -128,
		.maximum = 127,
		.step = 0x1,
		.default_value = 0,
		.flags = 0,
	}
};

static int
tvp5150_query_ctrl(void *arg)
{
	struct v4l2_queryctrl *qc = arg;
    int i;

	printk(KERN_INFO "VIDIOC_QUERYCTRL called\n");

	for (i = 0; i < ARRAY_SIZE(tvp5150_qctrl); i++)
	    if (qc->id && qc->id == tvp5150_qctrl[i].id) {
			memcpy(qc, &(tvp5150_qctrl[i]), sizeof(*qc));
		return 0;
	}
	return -EINVAL;
}

static int
tvp5150_set_std(void *arg)
{
	v4l2_std_id std;
	u8 fmt = 0;

    std = *(v4l2_std_id *)arg;
	if (g_std == std)
		return 0;

	if (std == V4L2_STD_ALL) {
		fmt = 0;	/* Autodetect mode */
	} else if (std & V4L2_STD_NTSC_443) {
		fmt = 0xa;
	} else if (std & V4L2_STD_PAL_M) {
		fmt = 0x6;
	} else if (std & (V4L2_STD_PAL_N| V4L2_STD_PAL_Nc)) {
		fmt = 0x8;
	} else {
		/* Then, test against generic ones */
		if (std & V4L2_STD_NTSC) {
			fmt = 0x2;
		} else if (std & V4L2_STD_PAL) {
			fmt = 0x4;
		} else if (std & V4L2_STD_SECAM) {
			fmt = 0xc;
		}
	}

	printk(KERN_INFO "Set video std register to %d.\n", fmt);
	if (tvp5150_write_reg(TVP5150_VIDEO_STD, fmt) == 0) {
		g_std = std;
		return 0;
	}
	return -1;
}

static void
tvp5150_download_patch(void)
{
	u8 *ptr = (u8*)g_pData;

	tvp5150_write_reg(0x7f, 0x00); // Restart TVP5150AM1 uP
	tvp5150_write_reg(0x03, 0x69); // Enable Outputs

    // unlock password for patch code download & register write
	tvp5150_write_reg(0x21, 0x51);
	tvp5150_write_reg(0x22, 0x50);
	tvp5150_write_reg(0x23, 0xff);
	tvp5150_write_reg(0x24, 0x04);

	//This will write to slave address TVP5150AM1_ADDR, sub-address 0x7E
    //(auto-incrementing data register) 'nNumBytes' bytes
    //from buffer 'pData' in one continuous I2Ctransfer.
    tvp5150_write_buffer(0x7e, ptr, 6051);

    tvp5150_write_reg(0x7f, 0x00); // Restart TVP5150AM1 uP
    tvp5150_write_reg(0x21, 0x00);
	tvp5150_write_reg(0x22, 0x00);
	tvp5150_write_reg(0x23, 0xff);
	tvp5150_write_reg(0x24, 0x04);
	
	udelay(1000);
	tvp5150_write_reg(0x21, 0x51);
	tvp5150_write_reg(0x22, 0x50);
	tvp5150_write_reg(0x23, 0xff);
	tvp5150_write_reg(0x24, 0x04);
	
	tvp5150_write_reg(0x10, 0x05);
	
	tvp5150_write_reg(0x21, 0x00);
	tvp5150_write_reg(0x22, 0x00);
	tvp5150_write_reg(0x23, 0xff);
	tvp5150_write_reg(0x24, 0x04);
}

/* Default values as sugested at TVP5150AM1 datasheet */
static const struct i2c_reg_value tvp5150_init_default[] = {
	{ /* 0x00 */
		TVP5150_VD_IN_SRC_SEL_1,0x00
	},
	{ /* 0x01 */
		TVP5150_ANAL_CHL_CTL,0x15
	},
	{ /* 0x02 */
		TVP5150_OP_MODE_CTL,0x00
	},
	{ /* 0x03 */
		TVP5150_MISC_CTL,0x01
	},
	{ /* 0x06 */
		TVP5150_COLOR_KIL_THSH_CTL,0x10
	},
	{ /* 0x07 */
		TVP5150_LUMA_PROC_CTL_1,0x60
	},
	{ /* 0x08 */
		TVP5150_LUMA_PROC_CTL_2,0x00
	},
	{ /* 0x09 */
		TVP5150_BRIGHT_CTL,0x80
	},
	{ /* 0x0a */
		TVP5150_SATURATION_CTL,0x80
	},
	{ /* 0x0b */
		TVP5150_HUE_CTL,0x00
	},
	{ /* 0x0c */
		TVP5150_CONTRAST_CTL,0x80
	},
	{ /* 0x0d */
		TVP5150_DATA_RATE_SEL,0x47
	},
	{ /* 0x0e */
		TVP5150_LUMA_PROC_CTL_3,0x00
	},
	{ /* 0x0f */
		TVP5150_CONF_SHARED_PIN,0x08
	},
	{ /* 0x11 */
		TVP5150_ACT_VD_CROP_ST_MSB,0x00
	},
	{ /* 0x12 */
		TVP5150_ACT_VD_CROP_ST_LSB,0x00
	},
	{ /* 0x13 */
		TVP5150_ACT_VD_CROP_STP_MSB,0x00
	},
	{ /* 0x14 */
		TVP5150_ACT_VD_CROP_STP_LSB,0x00
	},
	{ /* 0x15 */
		TVP5150_GENLOCK,0x01
	},
	{ /* 0x16 */
		TVP5150_HORIZ_SYNC_START,0x80
	},
	{ /* 0x18 */
		TVP5150_VERT_BLANKING_START,0x00
	},
	{ /* 0x19 */
		TVP5150_VERT_BLANKING_STOP,0x00
	},
	{ /* 0x1a */
		TVP5150_CHROMA_PROC_CTL_1,0x0c
	},
	{ /* 0x1b */
		TVP5150_CHROMA_PROC_CTL_2,0x14
	},
	{ /* 0x1c */
		TVP5150_INT_RESET_REG_B,0x00
	},
	{ /* 0x1d */
		TVP5150_INT_ENABLE_REG_B,0x00
	},
	{ /* 0x1e */
		TVP5150_INTT_CONFIG_REG_B,0x00
	},
	{ /* 0x28 */
		TVP5150_VIDEO_STD,0x00
	},
	{ /* 0x2e */
		TVP5150_MACROVISION_ON_CTR,0x0f
	},
	{ /* 0x2f */
		TVP5150_MACROVISION_OFF_CTR,0x01
	},
	{ /* 0xbb */
		TVP5150_TELETEXT_FIL_ENA,0x00
	},
	{ /* 0xc0 */
		TVP5150_INT_STATUS_REG_A,0x00
	},
	{ /* 0xc1 */
		TVP5150_INT_ENABLE_REG_A,0x00
	},
	{ /* 0xc2 */
		TVP5150_INT_CONF,0x04
	},
	{ /* 0xc8 */
		TVP5150_FIFO_INT_THRESHOLD,0x80
	},
	{ /* 0xc9 */
		TVP5150_FIFO_RESET,0x00
	},
	{ /* 0xca */
		TVP5150_LINE_NUMBER_INT,0x00
	},
	{ /* 0xcb */
		TVP5150_PIX_ALIGN_REG_LOW,0x4e
	},
	{ /* 0xcc */
		TVP5150_PIX_ALIGN_REG_HIGH,0x00
	},
	{ /* 0xcd */
		TVP5150_FIFO_OUT_CTRL,0x01
	},
	{ /* 0xcf */
		TVP5150_FULL_FIELD_ENA,0x00
	},
	{ /* 0xd0 */
		TVP5150_LINE_MODE_INI,0x00
	},
	{ /* 0xfc */
		TVP5150_FULL_FIELD_MODE_REG,0x7f
	},
	{ /* end of data */
		0xff,0xff
	}
};

/* Default values as sugested at TVP5150AM1 datasheet */
static const struct i2c_reg_value tvp5150_init_enable[] = {
	{
		TVP5150_CONF_SHARED_PIN, 2
	},{	/* Automatic offset and AGC enabled */
		TVP5150_ANAL_CHL_CTL, 0x15
	},{	/* Activate YCrCb output 0x9 or 0xd ? */
		TVP5150_MISC_CTL, 0x6f
	},{	/* Activates video std autodetection for all standards */
		TVP5150_AUTOSW_MSK, 0x0
	},{	/* Default format: 0x47. For 4:2:2: 0x40 */
		TVP5150_DATA_RATE_SEL, 0x47
	},{
		TVP5150_CHROMA_PROC_CTL_1, 0x0c
	},{
		TVP5150_CHROMA_PROC_CTL_2, 0x54
	},{	/* Non documented, but initialized on WinTV USB2 */
		0x27, 0x20
	},{
		0xff,0xff
	}
};

static void
tvp5150_write_inittab(const struct i2c_reg_value *regs)
{
	while (regs->addr != 0xff) {
		tvp5150_write_reg(regs->addr, regs->value);
		regs++;
	}
}

#if 0
struct tvp5150_vbi_type {
	unsigned int vbi_type;
	unsigned int ini_line;
	unsigned int end_line;
	unsigned int by_field :1;
};

struct i2c_vbi_ram_value {
	u16 reg;
	struct tvp5150_vbi_type type;
	unsigned char values[16];
};

/* This struct have the values for each supported VBI Standard
 * by
 tvp5150_vbi_types should follow the same order as vbi_ram_default
 * value 0 means rom position 0x10, value 1 means rom position 0x30
 * and so on. There are 16 possible locations from 0 to 15.
 */

static struct i2c_vbi_ram_value vbi_ram_default[] =
{
	/* FIXME: Current api doesn't handle all VBI types, those not
	   yet supported are placed under #if 0 */
#if 0
	{0x010, /* Teletext, SECAM, WST System A */
		{V4L2_SLICED_TELETEXT_SECAM,6,23,1},
		{ 0xaa, 0xaa, 0xff, 0xff, 0xe7, 0x2e, 0x20, 0x26,
		  0xe6, 0xb4, 0x0e, 0x00, 0x00, 0x00, 0x10, 0x00 }
	},
#endif

	{0x030, /* Teletext, PAL, WST System B */
		{V4L2_SLICED_TELETEXT_B,6,22,1},
		{ 0xaa, 0xaa, 0xff, 0xff, 0x27, 0x2e, 0x20, 0x2b,
		  0xa6, 0x72, 0x10, 0x00, 0x00, 0x00, 0x10, 0x00 }
	},

#if 0
	{0x050, /* Teletext, PAL, WST System C */
		{V4L2_SLICED_TELETEXT_PAL_C,6,22,1},
		{ 0xaa, 0xaa, 0xff, 0xff, 0xe7, 0x2e, 0x20, 0x22,
		  0xa6, 0x98, 0x0d, 0x00, 0x00, 0x00, 0x10, 0x00 }
	},
	{0x070, /* Teletext, NTSC, WST System B */
		{V4L2_SLICED_TELETEXT_NTSC_B,10,21,1},
		{ 0xaa, 0xaa, 0xff, 0xff, 0x27, 0x2e, 0x20, 0x23,
		  0x69, 0x93, 0x0d, 0x00, 0x00, 0x00, 0x10, 0x00 }
	},
	{0x090, /* Tetetext, NTSC NABTS System C */
		{V4L2_SLICED_TELETEXT_NTSC_C,10,21,1},
		{ 0xaa, 0xaa, 0xff, 0xff, 0xe7, 0x2e, 0x20, 0x22,
		  0x69, 0x93, 0x0d, 0x00, 0x00, 0x00, 0x15, 0x00 }
	},
	{0x0b0, /* Teletext, NTSC-J, NABTS System D */
		{V4L2_SLICED_TELETEXT_NTSC_D,10,21,1},
		{ 0xaa, 0xaa, 0xff, 0xff, 0xa7, 0x2e, 0x20, 0x23,
		  0x69, 0x93, 0x0d, 0x00, 0x00, 0x00, 0x10, 0x00 }
	},
	{0x0d0, /* Closed Caption, PAL/SECAM */
		{V4L2_SLICED_CAPTION_625,22,22,1},
		{ 0xaa, 0x2a, 0xff, 0x3f, 0x04, 0x51, 0x6e, 0x02,
		  0xa6, 0x7b, 0x09, 0x00, 0x00, 0x00, 0x27, 0x00 }
	},
#endif

	{0x0f0, /* Closed Caption, NTSC */
		{V4L2_SLICED_CAPTION_525,21,21,1},
		{ 0xaa, 0x2a, 0xff, 0x3f, 0x04, 0x51, 0x6e, 0x02,
		  0x69, 0x8c, 0x09, 0x00, 0x00, 0x00, 0x27, 0x00 }
	},
	{0x110, /* Wide Screen Signal, PAL/SECAM */
		{V4L2_SLICED_WSS_625,23,23,1},
		{ 0x5b, 0x55, 0xc5, 0xff, 0x00, 0x71, 0x6e, 0x42,
		  0xa6, 0xcd, 0x0f, 0x00, 0x00, 0x00, 0x3a, 0x00 }
	},

#if 0
	{0x130, /* Wide Screen Signal, NTSC C */
		{V4L2_SLICED_WSS_525,20,20,1},
		{ 0x38, 0x00, 0x3f, 0x00, 0x00, 0x71, 0x6e, 0x43,
		  0x69, 0x7c, 0x08, 0x00, 0x00, 0x00, 0x39, 0x00 }
	},
	{0x150, /* Vertical Interval Timecode (VITC), PAL/SECAM */
		{V4l2_SLICED_VITC_625,6,22,0},
		{ 0x00, 0x00, 0x00, 0x00, 0x00, 0x8f, 0x6d, 0x49,
		  0xa6, 0x85, 0x08, 0x00, 0x00, 0x00, 0x4c, 0x00 }
	},
	{0x170, /* Vertical Interval Timecode (VITC), NTSC */
		{V4l2_SLICED_VITC_525,10,20,0},
		{ 0x00, 0x00, 0x00, 0x00, 0x00, 0x8f, 0x6d, 0x49,
		  0x69, 0x94, 0x08, 0x00, 0x00, 0x00, 0x4c, 0x00 }
	},
#endif

	{0x190, /* Video Program System (VPS), PAL */
		{V4L2_SLICED_VPS,16,16,0},
		{ 0xaa, 0xaa, 0xff, 0xff, 0xba, 0xce, 0x2b, 0x0d,
		  0xa6, 0xda, 0x0b, 0x00, 0x00, 0x00, 0x60, 0x00 }
	},
	/* 0x1d0 User programmable */

	/* End of struct */
	{ (u16)-1 }
};

static int
tvp5150_vdp_init(struct i2c_vbi_ram_value *regs)
{
	unsigned int i;

	/* Disable Full Field */
	tvp5150_write_reg(TVP5150_FULL_FIELD_ENA, 0);

	/* Before programming, Line mode should be at 0xff */
	for (i = TVP5150_LINE_MODE_INI; i<=TVP5150_LINE_MODE_END; i++)
		tvp5150_write_reg(i, 0xff);

	/* Load Ram Table */
	while (regs->reg != (u16)-1 ) {
		tvp5150_write_reg(TVP5150_CONF_RAM_ADDR_HIGH, regs->reg>>8);
		tvp5150_write_reg(TVP5150_CONF_RAM_ADDR_LOW, regs->reg);

		for (i=0;i<16;i++)
			tvp5150_write_reg(TVP5150_VDP_CONF_RAM_DATA, regs->values[i]);
		regs++;
	}
	return 0;
}
#endif

static void
get_color(void)
{
	u8 v;
	if (tvp5150_read_reg(TVP5150_BRIGHT_CTL, &v) == 0) {
		g_bri = v;
	}

    if (tvp5150_read_reg(TVP5150_SATURATION_CTL, &v) == 0) {
		g_sat = v;
	}
	
	if (tvp5150_read_reg(TVP5150_HUE_CTL, &v) == 0) {
		g_hue = (int)v+0x80;
	}

	if (tvp5150_read_reg(TVP5150_CONTRAST_CTL, &v) == 0) {
		g_con = v;
	}
}

/*!
 * tvp5150 sensor interface Initialization
 * @param param            sensor_interface *
 * @param width            u32
 * @param height           u32
 * @return  None
 */
static void tvp5150_interface(sensor_interface * param, u32 width, u32 height)
{
	param->Vsync_pol = 0x0;
	param->clk_mode = 0x0;	//gated
	param->pixclk_pol = 0x0;
	param->data_width = 0x1;
	param->data_pol = 0x0;
	param->ext_vsync = 0x1;
	param->Vsync_pol = 0x0;
	param->Hsync_pol = 0x0;
	param->width = width - 1;
	param->height = height - 1;
	param->pixel_fmt = IPU_PIX_FMT_UYVY;
	param->mclk = 27000000;
	/* setup cropping */
	g_cam->crop_bounds.left = 0;
	g_cam->crop_bounds.width = width;
	g_cam->crop_bounds.top = 0;
	g_cam->crop_bounds.height = height;

	g_cam->crop_defrect = g_cam->crop_bounds;
	if ((g_cam->crop_current.width > g_cam->crop_bounds.width)
	       || (g_cam->crop_current.height > g_cam->crop_bounds.height))
		g_cam->crop_current = g_cam->crop_bounds;

	g_cam->streamparm.parm.capture.capturemode = 0;

	g_cam->standard.index = 0;
	g_cam->standard.id = (height == 576) ? V4L2_STD_PAL : V4L2_STD_NTSC;
	g_cam->standard.frameperiod.denominator = (height == 576) ? 50 : 60;
	g_cam->standard.frameperiod.numerator = 1;
	g_cam->standard.framelines = height;
}

/*!
 * tvp5150 sensor configuration
 *
 * @param frame_rate       int *
 * @param high_quality     int
 * @return  sensor_interface *
 */
sensor_interface *tvp5150_config(int *frame_rate, int high_quality)
{
	u32 out_width, out_height;
	u8 status, status2;

	out_width = 720;
	out_height = 576;

	set_mclk_rate(&interface_param->mclk);

    tvp5150_write_reg(TVP5150_REV_SELECT, 0);
    tvp5150_write_inittab(tvp5150_init_default);
#if 0
    tvp5150_vdp_init(vbi_ram_default);
#endif
    tvp5150_set_channel(0);
    tvp5150_write_inittab(tvp5150_init_enable);
	tvp5150_write_reg(TVP5150_VIDEO_STD, 0x04);

	/* read out status */
	tvp5150_read_reg(TVP5150_STATUS_REG_5, &status);
	tvp5150_read_reg(TVP5150_STATUS_REG_1, &status2);
	if (!((status2 & 0x0E) && (status & 0x01)))
		printk(KERN_ERR"No video source detected.\n");
	else {
		switch ((status >> 1) & 0x07) {
		case 0x0:
		case 0x4:
			/* 60Hz => NTSC */
			printk(KERN_INFO"NTSC video source.\n");
			out_width = 720;
			out_height = 487;
			break;
		case 0x1:
		case 0x2:
		case 0x3:
			/* 50Hz => PAL */
			printk(KERN_INFO"PAL video source.\n");
			break;
		case 0x5:
			printk(KERN_INFO"SECAM video source.\n");
			break;
		default:
			printk(KERN_ERR"Unknown video source.\n");
			break;
		}
	}
    get_color();
	tvp5150_interface(interface_param, out_width, out_height);

	return interface_param;
}

static void
tvp5150_set_color(int bri, int hue, int sat, int con, int r, int b)
{
	if (bri != g_bri && bri >= tvp5150_qctrl[0].minimum && bri <= tvp5150_qctrl[0].maximum) {
    	tvp5150_write_reg(TVP5150_BRIGHT_CTL, (u8)bri);
    	g_bri = bri;
    	printk(KERN_ERR "--- tvp5150.c: set brightness to %x ---\n", bri);
    }

    if (hue >= 0&&hue != g_hue) {
    	hue -= 128;
    	if (hue >= tvp5150_qctrl[3].minimum && hue <= tvp5150_qctrl[3].maximum) {
    	    tvp5150_write_reg(TVP5150_HUE_CTL, (u8)hue);
    	    g_hue = hue+128;
    	    printk(KERN_ERR "--- tvp5150.c: set hue to %x ---\n", hue);
    	}
    }

    if (sat != g_sat && sat >= tvp5150_qctrl[2].minimum && sat <= tvp5150_qctrl[2].maximum) {
    	tvp5150_write_reg(TVP5150_SATURATION_CTL, (u8)sat);
    	g_sat = sat;
    	printk(KERN_ERR "--- tvp5150.c: set saturation to %x ---\n", sat);
    }

    if (con != g_con && con >= tvp5150_qctrl[1].minimum && con <= tvp5150_qctrl[1].maximum) {
    	tvp5150_write_reg(TVP5150_CONTRAST_CTL, (u8)con);
    	g_con = con;
    	printk(KERN_ERR "--- tvp5150.c: set contrast to %x ---\n", con);
    }
}

static void
tvp5150_get_color(int *bri, int *hue, int *sat, int *con, int *red, int *blue)
{
	*bri = g_bri;
    *hue = g_hue;
    *sat = g_sat;
    *con = g_con;
}

/*!
 * tvp5150 sensor set AE measurement window mode configuration
 *
 * @param ae_mode      int
 * @return  None
 */
static void tvp5150_set_ae_mode(int ae_mode)
{
}

/*!
 * tvp5150 sensor get AE measurement window mode configuration
 *
 * @param ae_mode      int *
 * @return  None
 */
static void tvp5150_get_ae_mode(int *ae_mode)
{
}

/*!
 * tvp5150 sensor enable/disable AE 
 *
 * @param active      int
 * @return  None
 */
static void tvp5150_set_ae(int active)
{
}

/*!
 * tvp5150 sensor enable/disable auto white balance
 *
 * @param active      int
 * @return  None
 */
static void tvp5150_set_awb(int active)
{
}

/*!
 * tvp5150 sensor set the flicker control 
 *
 * @param control      int
 * @return  None
 */
static void tvp5150_flicker_control(int control)
{
}

/*!
 * tvp5150 Get mode&flicker control parameters 
 *
 * @return  None
 */
static void tvp5150_get_control_params(int *ae, int *awb, int *flicker)
{
}

/*!
 * tvp5150 Reset function
 *
 * @return  None
 */
static sensor_interface *tvp5150_reset(void)
{
	return tvp5150_config(0, 0);
}

static int
tvp5150_get_ver(void)
{
	if (tvp5150_read_reg(TVP5150_ROM_MAJOR_VER, &msb_rom) != 0) return -ENODEV;
	if (tvp5150_read_reg(TVP5150_ROM_MINOR_VER, &lsb_rom) != 0) return -ENODEV;
	if (tvp5150_read_reg(TVP5150_MSB_DEV_ID, &msb_id) != 0) return -ENODEV;
	if (tvp5150_read_reg(TVP5150_LSB_DEV_ID, &lsb_id) != 0) return -ENODEV;
	return 0;
}

/*!
 * tvp5150 get_status function, detect whether the device is present
 *
 * @return  Zero on success, others on failure
 */
static int tvp5150_get_status(void)
{
	if (!interface_param)
		return -ENODEV;

	if (tvp5150_get_ver() != 0)
		return -ENODEV;

	if (msb_rom != TVP5150_CHIP_VERSION)
		return -ENODEV;
	else if (lsb_rom != 0x8c){
		tvp5150_download_patch();
	}
	return 0;
}

/*!
 * tvp5150 select channel
 *
 * @param channel: 0 - Channel 1, 1 - Channel 2 - SVIDEO. 
 * @return  None
 */
static void tvp5150_set_channel(int channel)
{
	unsigned char opmode = 0;
	unsigned char input = 0;

	if (!interface_param)
		return;

	switch (channel) {
	case 1:
		input |= 2;
		/* fall through */
	case 0:
		opmode = 0x30;		/* TV Mode */
		break;
	case 2:
		input = 1;
		opmode = 0;		/* Auto Mode */
		break;
	case 3:
		input = 8;
		opmode = 0;		/* Auto Mode */
		break;
	default:
		return;
	}

    tvp5150_write_reg(TVP5150_OP_MODE_CTL, opmode);
	tvp5150_write_reg(TVP5150_VD_IN_SRC_SEL_1, input);
}

struct camera_sensor ccd_sensor_if = {
	.set_std = tvp5150_set_std,
	.query_ctrl = tvp5150_query_ctrl,
	.set_flip = NULL,
	.set_color = tvp5150_set_color,
	.get_color = tvp5150_get_color,
	.set_ae_mode = tvp5150_set_ae_mode,
	.get_ae_mode = tvp5150_get_ae_mode,
	.set_ae = tvp5150_set_ae,
	.set_awb = tvp5150_set_awb,
	.flicker_control = tvp5150_flicker_control,
	.get_control_params = tvp5150_get_control_params,
	.config = tvp5150_config,
	.reset = tvp5150_reset,
	.get_status = tvp5150_get_status,
	.set_channel = tvp5150_set_channel,
};

//////////////////////////////////////////////////////////
//              i2c  operation interface                //
//////////////////////////////////////////////////////////
#if 0
{
#endif

static int tvp5150_attach(struct i2c_adapter *adapter);
static int tvp5150_detach(struct i2c_client *client);

static struct i2c_driver tvp5150_i2c_driver = {
	.driver = {
		   .owner = THIS_MODULE,
		   .name = "TVP5150 Client",
		   },
	.attach_adapter = tvp5150_attach,
	.detach_client = tvp5150_detach,
};

static struct i2c_client tvp5150_i2c_client = {
	.name = "TVP5150 I2C dev",
	.addr = TVP5150_I2C_ADDRESS,
	.driver = &tvp5150_i2c_driver,
};

/*
 * Function definitions
 */
static int tvp5150_i2c_client_xfer(unsigned int addr, u8 reg,
				   char *buf, int num, int tran_flag)
{
	struct i2c_msg msg[2];
	int ret;

	msg[0].addr = addr;
	msg[0].len = 1;
	msg[0].buf = &reg;
	msg[0].flags = tran_flag;
	msg[0].flags &= ~I2C_M_RD;

	msg[1].addr = addr;
	msg[1].len = num;
	msg[1].buf = buf;
	msg[1].flags = tran_flag;

	if (tran_flag & MXC_I2C_FLAG_READ) {
		msg[1].flags |= I2C_M_RD;
	} else {
		msg[1].flags &= ~I2C_M_RD;
	}

	ret = i2c_transfer(tvp5150_i2c_client.adapter, msg, 2);
	if (ret >= 0)
		return 0;

	return ret;
}

static int tvp5150_read_reg(u8 reg, u8 *val)
{
	return tvp5150_i2c_client_xfer(TVP5150_I2C_ADDRESS, reg, val, 1, MXC_I2C_FLAG_READ);
}

static int tvp5150_write_reg(u8 reg, u8 val)
{
	return tvp5150_i2c_client_xfer(TVP5150_I2C_ADDRESS, reg, &val, 1, 0);
}


/*!
 * tvp5150 I2C detect_client function
 *
 * @param adapter            struct i2c_adapter *
 * @param address            int
 * @param kind               int
 * 
 * @return  Error code indicating success or failure
 */
static int tvp5150_detect_client(struct i2c_adapter *adapter, int address, int kind)
{
	tvp5150_i2c_client.adapter = adapter;
	if (i2c_attach_client(&tvp5150_i2c_client)) {
		tvp5150_i2c_client.adapter = NULL;
		printk(KERN_ERR "tvp5150_detect_client: i2c_attach_client failed\n");
		return -1;
	}

	interface_param = (sensor_interface *)kmalloc(sizeof(sensor_interface), GFP_KERNEL);
	if (!interface_param) {
		printk(KERN_ERR "tvp5150_detect_client: kmalloc failed \n");
		return -1;
	}
	interface_param->mclk = 27000000;

	printk(KERN_INFO "TVP5150 Detected\n");

	return 0;
}

static unsigned short normal_i2c[] = { TVP5150_I2C_ADDRESS, I2C_CLIENT_END };

/* Magic definition of all other variables and things */
I2C_CLIENT_INSMOD;

/*!
 * tvp5150 I2C attach function
 *
 * @param adapter            struct i2c_adapter *
 * @return  Error code indicating success or failure
 */
static int tvp5150_attach(struct i2c_adapter *adap)
{
	uint32_t mclk = 27000000;
	struct clk *clk;
	int err;

	clk = clk_get(NULL, "csi_clk");
	clk_enable(clk);
	set_mclk_rate(&mclk);

	err = i2c_probe(adap, &addr_data, &tvp5150_detect_client);

	clk_disable(clk);
	clk_put(clk);

	return err;
}

static void
try_dowload_patch(void)
{
	printk(KERN_ALERT "--- verify firmware version ---\n");
	tvp5150_get_ver();
	if (lsb_rom != 0x8c) {
		uint32_t mclk = 27000000;
	    struct clk *clk;

        printk(KERN_ALERT "--- try downloading patch code ---\n");
        clk = clk_get(NULL, "csi_clk");
	    clk_enable(clk);
	    set_mclk_rate(&mclk);

		tvp5150_download_patch();
		tvp5150_get_ver();
		printk(KERN_ALERT "--- TVP5150 Chip version = %d.%d ---\n", msb_rom, lsb_rom);
	    printk(KERN_ALERT "--- TVP5150 Chip ID = %d.%d ---\n", msb_id, lsb_id);
		if (lsb_rom != 0x8c) {
			printk(KERN_ALERT "--- downloading patch code failed ---\n");
		} else {
			printk(KERN_ALERT "--- patch code downloaded successfully ---\n");
		}

	    clk_disable(clk);
	    clk_put(clk);
	}
}

/*!
 * tvp5150 I2C detach function
 *
 * @param client            struct i2c_client *
 * @return  Error code indicating success or failure
 */
static int tvp5150_detach(struct i2c_client *client)
{
	int err;

	if (!tvp5150_i2c_client.adapter)
		return -1;

	err = i2c_detach_client(&tvp5150_i2c_client);
	tvp5150_i2c_client.adapter = NULL;

	if (interface_param)
		kfree(interface_param);
	interface_param = NULL;

	return err;
}

extern void gpio_sensor_active(void);
extern int  gpio_sensor_activated;

/*!
 * tvp5150 init function
 *
 * @return  Error code indicating success or failure
 */
static __init int tvp5150_init(void)
{
	u8 err;

    if (gpio_sensor_activated == 0) {
       gpio_sensor_active();
       gpio_sensor_activated = 1;
    }

	err = i2c_add_driver(&tvp5150_i2c_driver);
	if (tvp5150_i2c_client.adapter != NULL) {
	    try_dowload_patch();
	}

	return err;
}

extern void gpio_sensor_inactive(void);
/*!
 * tvp5150 cleanup function
 *
 * @return  Error code indicating success or failure
 */
static void __exit tvp5150_clean(void)
{
	i2c_del_driver(&tvp5150_i2c_driver);
    if (gpio_sensor_activated == 1) {
       gpio_sensor_inactive();
       gpio_sensor_activated = 0;
    }
}

module_init(tvp5150_init);
module_exit(tvp5150_clean);

/* Exported symbols for modules. */
EXPORT_SYMBOL(ccd_sensor_if);

MODULE_AUTHOR("Morninghan electronics, Inc.");
MODULE_DESCRIPTION("TVP5150 TVin Driver");
MODULE_LICENSE("GPL");

#if 0
}
#endif
