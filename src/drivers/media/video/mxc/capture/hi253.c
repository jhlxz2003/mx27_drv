/*
 * Copyright 2005-2007 Freescale Semiconductor, Inc. All Rights Reserved.
 */

/*
 * The code contained herein is licensed under the GNU General Public
 * License. You may obtain a copy of the GNU General Public License
 * Version 2 or later at the following locations:
 *
 * http://www.opensource.org/licenses/gpl-license.html
 * http://www.gnu.org/copyleft/gpl.html
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
#include <asm/arch/pmic_power.h>
#include "mxc_v4l2_capture.h"

#define HI253_I2C_ADDRESS    (0x40>>1)

#define hi253_select_reg_page(v)  hi253_write_reg(0x03, (unsigned char)(v))

typedef struct hi253_reg {
	unsigned char addr;
	unsigned char data;
} hi253_reg_t;

static sensor_interface *interface_param = NULL;
static int reset_frame_rate = 30;

////////////////////////////////////////////////////
//                SCCB interface                  //
////////////////////////////////////////////////////
#if 0
{
#endif

static int hi253_attach(struct i2c_adapter *adapter);
static int hi253_detach(struct i2c_client *client);

static struct i2c_driver hi253_i2c_driver = {
	.driver = {
		   .owner = THIS_MODULE,
		   .name = "HI253 Client",
		   },
	.attach_adapter = hi253_attach,
	.detach_client = hi253_detach,
};

static struct i2c_client hi253_i2c_client = {
	.name = "hi253 I2C dev",
	.addr = HI253_I2C_ADDRESS,
	.driver = &hi253_i2c_driver,
};

/*!
 * hi253 I2C detect_client function
 *
 * @param adapter            struct i2c_adapter *
 * @param address            int
 * @param kind               int
 * 
 * @return  Error code indicating success or failure
 */
static int hi253_detect_client(struct i2c_adapter *adapter, int address, int kind)
{
	hi253_i2c_client.adapter = adapter;

	if (i2c_attach_client(&hi253_i2c_client)) {
		hi253_i2c_client.adapter = NULL;
		printk(KERN_ERR "hi253_attach: i2c_attach_client failed\n");
		return -1;
	}

	interface_param = (sensor_interface *)kmalloc(sizeof(sensor_interface), GFP_KERNEL);
	if (!interface_param) {
		printk(KERN_ERR "hi253_attach: kmalloc failed \n");
		return -1;
	}
	interface_param->mclk = 27000000;

	printk(KERN_INFO "hi253 Detected\n");

	return 0;
}

static unsigned short normal_i2c[] = { HI253_I2C_ADDRESS, I2C_CLIENT_END };

/* Magic definition of all other variables and things */
I2C_CLIENT_INSMOD;

/*!
 * hi253 I2C attach function
 *
 * @param adapter            struct i2c_adapter *
 * @return  Error code indicating success or failure
 */
static int hi253_attach(struct i2c_adapter *adapter)
{

	uint32_t mclk = 27000000;
	struct clk *clk;
	int err;

	clk = clk_get(NULL, "csi_clk");
	clk_enable(clk);
	set_mclk_rate(&mclk);

	err = i2c_probe(adapter, &addr_data, &hi253_detect_client);
	clk_disable(clk);
	clk_put(clk);
	return err;
}

/*!
 * hi253 I2C detach function
 *
 * @param client            struct i2c_client *
 * @return  Error code indicating success or failure
 */
static int hi253_detach(struct i2c_client *client)
{
	int err;

	if (!hi253_i2c_client.adapter)
		return -1;

	err = i2c_detach_client(&hi253_i2c_client);
	hi253_i2c_client.adapter = NULL;

	if (interface_param)
		kfree(interface_param);
	interface_param = NULL;

	return err;
}

static int hi253_i2c_client_xfer(unsigned int addr, u8 reg, char *buf, int num, int tran_flag)
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

	if (hi253_i2c_client.adapter == NULL) {
		printk(KERN_ERR "%s:adapter error\n", __func__);
		return -1;
	}

	ret = i2c_transfer(hi253_i2c_client.adapter, msg, 2);
	if (ret >= 0)
		return 0;

	printk(KERN_ERR "%s:i2c transfer error:%d\n", __func__, ret);
	return ret;
}

static int hi253_read_reg(u8 reg, u8 *val)
{
	if (hi253_i2c_client_xfer(HI253_I2C_ADDRESS, reg, val, 1, MXC_I2C_FLAG_READ) < 0) {
		printk(KERN_ERR "%s:read reg errorr:reg=%x,val=%x\n", __func__, reg, *val);
		return -1;
	}
	return 0;
}

static int hi253_write_reg(u8 reg, u8 val)
{
	if (hi253_i2c_client_xfer(HI253_I2C_ADDRESS, reg, &val, 1, 0) < 0) {
		printk(KERN_ERR "%s:write reg errorr:reg=%x,val=%x\n", __func__, reg, val);
		return -1;
	}
	return 0;
}

#if 0
}
#endif

////////////////////////////////////////////////////

////////////////////////////////////////////////////
//              HI253 BASIC CONFIG                //
////////////////////////////////////////////////////
#if 0
{
#endif

/* DONE */
static unsigned char
hi253_get_id(void)
{
	unsigned char v = 0;

	hi253_select_reg_page(0);
	if (hi253_read_reg(0x04, &v) == 0)
	{
		printk(KERN_ERR "--- CMOS DevId = %x ---\n", v);
		return v;
	}
	printk(KERN_ERR "--- GET CMOS DevId failed ---\n");
	return 0;
}

///////////////////////////////////////////////////
//               direction config                //
///////////////////////////////////////////////////
/* DONE */
/*
 * t: 0-normal; 1-x_flip; 2-y_flip; 3-xy-flip.
 */
static char *flipdir[] = {
	"normal",
	"x-flip",
	"y-flip",
	"xy-flip"
};

static void hi253_set_flip(int t)
{
	hi253_write_reg(0x03, 0x00);
	hi253_write_reg(0x11, (unsigned char)(0x90+t));
	printk(KERN_ERR "--- hi253.c: set flip mode:%s ---\n", flipdir[t]);
}

///////////////////////////////////////////////////
//            white balance config               //
///////////////////////////////////////////////////
static unsigned char g_wb_data[6][7] = {
	{0x3d, 0x20, 0x32, 0x50, 0x20, 0x50, 0x20},//WB_Auto
    {0x49, 0x20, 0x24, 0x50, 0x45, 0x24, 0x1e},//WB_Cloud
    {0x45, 0x20, 0x27, 0x44, 0x3f, 0x29, 0x23},//WB_Daylight
    {0x33, 0x20, 0x3d, 0x2e, 0x24, 0x43, 0x3d},//WB_Incandescence
    {0x45, 0x20, 0x2f, 0x38, 0x32, 0x39, 0x33},//WB_Fluorscent
    {0x25, 0x20, 0x44, 0x22, 0x1e, 0x50, 0x45} //WB_Tungsten
};

static void hi253_set_awb(int t)
{
	int i;

	hi253_write_reg(0x03, 0x22);
	hi253_write_reg(0x10, 0x69);
	for (i=0; i<7; ++i)
	{
		hi253_write_reg(0x80+i, g_wb_data[t][i]);
	}

	if (t == 0)
	{
		hi253_write_reg(0x10, 0xe9);
	}
}

///////////////////////////////////////////////////
//              brightness config                //
///////////////////////////////////////////////////
static int yoffset_on = 0;

/* DONE
 * 0x00-> 0x7f 随着值的加大，图像越来越明亮；
 * 0x80-> 0xff 随着值的加大，图像越来越暗；
 */
static void hi253_set_yoffset(unsigned char bri)
{
	hi253_select_reg_page(0x10);
	if (yoffset_on == 0)
	{
	    hi253_write_reg(0x12, 0x30);
	    yoffset_on = 1;
	}

    if (bri >= 0x80)
    	bri -= 0x80;
    else
    	bri = 0xff - bri;
    hi253_write_reg(0x40, bri);
}

////////////////////////////////////////////////////
//                 auto explosure                 //
////////////////////////////////////////////////////
static unsigned char ae_regidx[] = {0x70, 0x78, 0x79, 0x76, 0x77};
static unsigned char ae_reg_dat[][5] = {
	{0x5e, 0x23, 0x38, 0x32, 0xc1}, //-3 Level
	{0x66, 0x23, 0x3d, 0x43, 0xd2}, //-2 Level
	{0x6e, 0x23, 0x42, 0x43, 0xe2}, //-1 Level
	{0x76, 0x23, 0x46, 0x43, 0xf2}, //0 Level
	{0x7e, 0x24, 0x4b, 0x53, 0x02}, //1 Level
	{0x86, 0x24, 0x50, 0x53, 0x02}, //2 Level
	{0x8e, 0x24, 0x55, 0x53, 0x02}, //3 Level
};

static void hi253_set_ae_target(int v)
{
	int i;

    if (v < -3||v > 3) return;
	hi253_select_reg_page(0x20);
	for (i=0; i<5; ++i)
	{
		hi253_write_reg(ae_regidx[i], ae_reg_dat[v+3][i]);
	}
}

/*
 * Page10 页 0x41 寄存器（默认值0x00）：0x00->0xff 随着值的加大，图像越来越亮白；
 * DY-offset的开关是Page10的0x12寄存器的B[5]
 * 即(0x03,0x10);
 *   (0x12,0x30);
 * Page10的0x50是AGBRT值，当AG(default:0x10)>AGBRT(default:0x60)值且0x12的B[5]为1时，
 * sensor的DYoffset才会起作用，正常情况下是在暗态起作用。
 */
static void hi253_set_dyoffset(unsigned char v)
{
	hi253_select_reg_page(0x10);
	if (yoffset_on == 0)
	{
	    hi253_write_reg(0x12, 0x30);
	    yoffset_on = 1;
	}
    hi253_write_reg(0x41, v);
}

///////////////////////////////////////////////////
//              saturation config                //
///////////////////////////////////////////////////
/* DONE
 * Page10 页 0x61 寄存器是控制饱和度蓝色分量的，值越大，色彩越鲜艳；
 *           0x62 寄存器是控制饱和度红色分量的，值越大，色彩越鲜艳；
 */
#define hi253_set_red(r)   hi253_write_reg(0x62, (r))
#define hi253_set_blue(b)  hi253_write_reg(0x61, (b))

/* default:0x7c */
static void hi235_set_saturation(unsigned char s)
{
	hi253_select_reg_page(0x10);
	hi253_set_blue(s);
	hi253_set_red(s);
}

///////////////////////////////////////////////////
//              contrast config                  //
///////////////////////////////////////////////////
/* DONE
   Page10 页 0x48 寄存器（默认值是0x84）：
   0x84->0xff 随着值的加大，细节对比强烈；
   0x84->0x00 随着值的减小，细节对比不明显。
*/
static void hi253_set_contrast(unsigned char v)
{
	hi253_select_reg_page(0x10);
	hi253_write_reg(0x48, v);
}

//should be replaced by width and height version.
static hi253_reg_t  init_arrb[] = {
	{0x01,0xf9},
    {0x08,0x0f},
    {0x01,0xf8},

    {0xff,0x01},
    
    {0x03,0x00},
    {0x0e,0x00},

    {0xff,0x01},

    {0x0e,0x00},
    {0x01,0xf1},
    {0x08,0x00},
    {0x01,0xf3},
    {0x01,0xf1},
    
    {0x03,0x20},
    {0x10,0x0c},
    
    {0x03,0x22},
    {0x10,0x69}
};

/* 800*600 */
static hi253_reg_t  init_arr0[] = {
    {0x03,0x00},
    {0x10,0x91},
    {0x11,0x90},
    {0x12,0x04},
    {0x0b,0xaa},
    {0x0c,0xaa},
    {0x0d,0xaa},
    {0x20,0x00},/* window size begin */
    {0x21,0x04},
    {0x22,0x00},
    {0x23,0x07},
    {0x24,0x04},
    {0x25,0xb0},
    {0x26,0x06},
    {0x27,0x40},/* window size end */
};

/* 1600*1200 */
static hi253_reg_t  init_arr1[] = {
    {0x03,0x00},
/*    {0x10,0x80},*/ {0x10,0x00},
    {0x11,0x90},
    {0x12,0x04},
    {0x0b,0xaa},
    {0x0c,0xaa},
    {0x0d,0xaa},
    {0x20,0x00},/* window size begin */
/*    {0x21,0x0a}, */ {0x21,0x04},
    {0x22,0x00},
/*    {0x23,0x0a}, */ {0x23,0x07},
    {0x24,0x04},
    {0x25,0xb0},
    {0x26,0x06},
    {0x27,0x40},/* window size end */
};

static hi253_reg_t  init_arre[] = {
    {0x40,0x01},
    {0x41,0x68},
    {0x42,0x00},
    {0x43,0x14},
    {0x45,0x04},
    {0x46,0x18},
    {0x47,0xd8},
    {0xe1,0x0f},
    {0x80,0x2e},
    {0x81,0x7e},
    {0x82,0x90},
    {0x83,0x00},
    {0x84,0x0c},
    {0x85,0x00},
    {0x90,0x0e},
    {0x91,0x0f},
    {0x92,0x5a},
    {0x93,0x50},
    {0x94,0x75},
    {0x95,0x70},
    {0x96,0xdc},
    {0x97,0xfe},
    {0x98,0x20},
    {0xa0,0x00},
    {0xa2,0x00},
    {0xa4,0x00},
    {0xa6,0x00},
    {0xa8,0x43},
    {0xaa,0x43},
    {0xac,0x43},
    {0xae,0x43},
    {0x99,0x42},
    {0x9a,0x42},
    {0x9b,0x42},
    {0x9c,0x42},

    {0x03,0x02},
    {0x12,0x03},
    {0x13,0x03},
    {0x16,0x00},
    {0x17,0x8c},
    {0x18,0x28},
    {0x19,0x40},
    {0x1a,0x39},
    {0x1c,0x09},
    {0x1d,0x40},
    {0x1e,0x30},
    {0x1f,0x10},
    {0x20,0x77},
    {0x21,0xdd},
    {0x22,0xa7},
    {0x23,0xb0},
    {0x27,0x3c},
    {0x2b,0x80},
    {0x2e,0x11},
    {0x2f,0xa1},
    {0x30,0x05},
    {0x50,0x20},
    {0x52,0x01},
    {0x55,0x1c},
    {0x56,0x00},
    {0x5d,0xa2},
    {0x5e,0x5a},
    {0x60,0x87},
    {0x61,0x99},
    {0x62,0x88},
    {0x63,0x97},
    {0x64,0x88},
    {0x65,0x97},
    {0x67,0x0c},
    {0x68,0x0c},
    {0x69,0x0c},
    {0x72,0x89},
    {0x73,0x97},
    {0x74,0x89},
    {0x75,0x97},
    {0x76,0x89},
    {0x77,0x97},
    {0x7c,0x85},
    {0x81,0x81},
    {0x82,0x23},
    {0x83,0x2b},
    {0x84,0x7d},
    {0x85,0x81},
    {0x86,0x7d},
    {0x87,0x81},
    {0x92,0x53},
    {0x93,0x5e},
    {0x94,0x7d},
    {0x95,0x81},
    {0x96,0x7d},
    {0x97,0x81},
    {0xa1,0x7b},
    {0xa3,0x7b},
    {0xa4,0x7b},
    {0xa6,0x7b},
    {0xa8,0x85},
    {0xa9,0x8c},
    {0xaa,0x85},
    {0xab,0x8c},
    {0xac,0x20},
    {0xad,0x26},
    {0xae,0x20},
    {0xaf,0x26},
    {0xb0,0x99},
    {0xb1,0xa3},
    {0xb2,0xa4},
    {0xb3,0xae},
    {0xb4,0x9b},
    {0xb5,0xa2},
    {0xb6,0xa6},
    {0xb7,0xac},
    {0xb8,0x9b},
    {0xb9,0x9f},
    {0xba,0xa6},
    {0xbb,0xaa},
    {0xbc,0x9b},
    {0xbd,0x9f},
    {0xbe,0xa6},
    {0xbf,0xaa},
    {0xc4,0x36},
    {0xc5,0x4e},
    {0xc6,0x61},
    {0xc7,0x78},
    {0xc8,0x36},
    {0xc9,0x4d},
    {0xca,0x36},
    {0xcb,0x4d},
    {0xcc,0x62},
    {0xcd,0x78},
    {0xce,0x62},
    {0xcf,0x78},
    {0xd0,0x0a},
    {0xd1,0x09},
    {0xd4,0x0f},
    {0xd5,0x0f},
    {0xd6,0x56},
    {0xd7,0x50},
    {0xe0,0xc4},
    {0xe1,0xc4},
    {0xe2,0xc4},
    {0xe3,0xc4},
    {0xe4,0x00},
    {0xe8,0x00},
    {0xea,0x82},

    {0x03,0x10},
    {0x10,0x01},/* UYVY */
    {0x12,0x30},
    {0x20,0x00},
    {0x40,0x80},
    {0x41,0x00},
    {0x30,0x00},
    {0x31,0x00},
    {0x32,0x00},
    {0x33,0x00},
    {0x34,0x30},
    {0x35,0x00},
    {0x36,0x00},
    {0x38,0x00},
    {0x3e,0x58},
    {0x3f,0x00},
    {0x60,0x6f},
    {0x61,0x76},
    {0x62,0x76},
    {0x63,0x30},
    {0x64,0x41},
    {0x66,0x33},
    {0x67,0x00},
    {0x6a,0x90},
    {0x6b,0x80},
    {0x6c,0x80},
    {0x6d,0xa0},
    {0x76,0x01},
    {0x74,0x66},
    {0x79,0x06},

    {0x03,0x11},
    {0x10,0x3f},
    {0x11,0x40},
    {0x12,0xba},
    {0x13,0xcb},
    {0x26,0x20},
    {0x27,0x22},
    {0x28,0x0f},
    {0x29,0x10},
    {0x2b,0x30},
    {0x2c,0x32},
    {0x30,0x70},
    {0x31,0x10},
    {0x32,0x65},
    {0x33,0x09},
    {0x34,0x06},
    {0x35,0x04},
    {0x36,0x70},
    {0x37,0x18},
    {0x38,0x65},
    {0x39,0x09},
    {0x3a,0x06},
    {0x3b,0x04},
    {0x3c,0x80},
    {0x3d,0x18},
    {0x3e,0x80},
    {0x3f,0x0c},
    {0x40,0x09},
    {0x41,0x06},
    {0x42,0x80},
    {0x43,0x18},
    {0x44,0x80},
    {0x45,0x12},
    {0x46,0x10},
    {0x47,0x10},
    {0x48,0x90},
    {0x49,0x40},
    {0x4a,0x80},
    {0x4b,0x13},
    {0x4c,0x10},
    {0x4d,0x11},
    {0x4e,0x80},
    {0x4f,0x30},
    {0x50,0x80},
    {0x51,0x13},
    {0x52,0x10},
    {0x53,0x13},
    {0x54,0x11},
    {0x55,0x17},
    {0x56,0x20},
    {0x57,0x20},
    {0x58,0x20},
    {0x59,0x30},
    {0x5a,0x18},
    {0x5b,0x00},
    {0x5c,0x00},
    {0x60,0x3f},
    {0x62,0x50},
    {0x70,0x06},
    {0x03,0x12},
    {0x20,0x0f},
    {0x21,0x0f},
    {0x25,0x30},
    {0x28,0x00},
    {0x29,0x00},
    {0x2a,0x00},
    {0x30,0x50},
    {0x31,0x18},
    {0x32,0x32},
    {0x33,0x40},
    {0x34,0x50},
    {0x35,0x70},
    {0x36,0xa0},
    {0x40,0xa0},
    {0x41,0x40},
    {0x42,0xa0},
    {0x43,0x90},
    {0x44,0x90},
    {0x45,0x80},
    {0x46,0xb0},
    {0x47,0x55},
    {0x48,0xa0},
    {0x49,0x90},
    {0x4a,0x90},
    {0x4b,0x80},
    {0x4c,0xb0},
    {0x4d,0x40},
    {0x4e,0x90},
    {0x4f,0x90},
    {0x50,0xe6},
    {0x51,0x80},
    {0x52,0xb0},
    {0x53,0x60},
    {0x54,0xc0},
    {0x55,0xc0},
    {0x56,0xc0},
    {0x57,0x80},
    {0x58,0x90},
    {0x59,0x40},
    {0x5a,0xd0},
    {0x5b,0xd0},
    {0x5c,0xe0},
    {0x5d,0x80},
    {0x5e,0x88},
    {0x5f,0x40},
    {0x60,0xe0},
    {0x61,0xe6},
    {0x62,0xe6},
    {0x63,0x80},
    {0x70,0x15},
    {0x71,0x01},
    {0x72,0x18},
    {0x73,0x01},
    {0x74,0x25},
    {0x75,0x15},
    {0x80,0x30},
    {0x81,0x50},
    {0x82,0x80},
    {0x85,0x1a},
    {0x88,0x00},
    {0x89,0x00},
    {0x90,0x5d},
    {0xc5,0x30},
    {0xc6,0x2a},
    {0xd0,0x0c},
    {0xd1,0x80},
    {0xd2,0x67},
    {0xd3,0x00},
    {0xd4,0x00},
    {0xd5,0x02},
    {0xd6,0xff},
    {0xd7,0x18},
    {0x3b,0x06},
    {0x3c,0x06},
    {0xc5,0x30},
    {0xc6,0x2a},

    {0x03,0x13},
    {0x10,0xcb},
    {0x11,0x7b},
    {0x12,0x07},
    {0x14,0x00},
    {0x20,0x15},
    {0x21,0x13},
    {0x22,0x33},
    {0x23,0x04},
    {0x24,0x09},
    {0x25,0x08},
    {0x26,0x18},
    {0x27,0x30},
    {0x29,0x12},
    {0x2a,0x50},
    {0x2b,0x06},
    {0x2c,0x06},
    {0x25,0x08},
    {0x2d,0x0c},
    {0x2e,0x12},
    {0x2f,0x12},
    {0x50,0x10},
    {0x51,0x14},
    {0x52,0x10},
    {0x53,0x0c},
    {0x54,0x0f},
    {0x55,0x0c},
    {0x56,0x10},
    {0x57,0x13},
    {0x58,0x10},
    {0x59,0x0c},
    {0x5a,0x0f},
    {0x5b,0x0c},
    {0x5c,0x0a},
    {0x5d,0x0b},
    {0x5e,0x0a},
    {0x5f,0x08},
    {0x60,0x09},
    {0x61,0x08},
    {0x62,0x08},
    {0x63,0x08},
    {0x64,0x08},
    {0x65,0x06},
    {0x66,0x06},
    {0x67,0x06},
    {0x68,0x07},
    {0x69,0x07},
    {0x6a,0x07},
    {0x6b,0x05},
    {0x6c,0x05},
    {0x6d,0x05},
    {0x6e,0x07},
    {0x6f,0x07},
    {0x70,0x07},
    {0x71,0x05},
    {0x72,0x05},
    {0x73,0x05},
    {0x80,0xfd},
    {0x81,0x1f},
    {0x82,0x05},
    {0x83,0x01},
    {0x90,0x05},
    {0x91,0x05},
    {0x92,0x33},
    {0x93,0x30},
    {0x94,0x03},
    {0x95,0x14},
    {0x97,0x30},
    {0x99,0x30},
    {0xa0,0x04},
    {0xa1,0x05},
    {0xa2,0x04},
    {0xa3,0x05},
    {0xa4,0x07},
    {0xa5,0x08},
    {0xa6,0x07},
    {0xa7,0x08},
    {0xa8,0x07},
    {0xa9,0x08},
    {0xaa,0x07},
    {0xab,0x08},
    {0xb0,0x22},
    {0xb1,0x2a},
    {0xb2,0x28},
    {0xb3,0x22},
    {0xb4,0x2a},
    {0xb5,0x28},
    {0xb6,0x22},
    {0xb7,0x2a},
    {0xb8,0x28},
    {0xb9,0x22},
    {0xba,0x2a},
    {0xbb,0x28},
    {0xbc,0x17},
    {0xbd,0x17},
    {0xbe,0x17},
    {0xbf,0x17},
    {0xc0,0x17},
    {0xc1,0x17},
    {0xc2,0x1e},
    {0xc3,0x12},
    {0xc4,0x10},
    {0xc5,0x1e},
    {0xc6,0x12},
    {0xc7,0x10},
    {0xc8,0x18},
    {0xc9,0x05},
    {0xca,0x05},
    {0xcb,0x18},
    {0xcc,0x05},
    {0xcd,0x05},
    {0xce,0x18},
    {0xcf,0x05},
    {0xd0,0x05},
    {0xd1,0x18},
    {0xd2,0x05},
    {0xd3,0x05},

    {0x03,0x14},
    {0x10,0x11},
    {0x20,0x40},
    {0x21,0x80},
    {0x23,0x80},
    {0x22,0x80},
    {0x23,0x80},
    {0x24,0x80},
    {0x30,0xc8},
    {0x31,0x2b},
    {0x32,0x00},
    {0x33,0x00},
    {0x34,0x90},
    {0x40,0x65},
    {0x50,0x42},
    {0x60,0x3a},
    {0x70,0x42},

    {0x03,0x15},
    {0x10,0x0f},
    {0x14,0x52},
    {0x15,0x42},
    {0x16,0x32},
    {0x17,0x2f},
    {0x30,0x8f},
    {0x31,0x59},
    {0x32,0x0a},
    {0x33,0x15},
    {0x34,0x5b},
    {0x35,0x06},
    {0x36,0x07},
    {0x37,0x40},
    {0x38,0x86},
    {0x40,0x95},
    {0x41,0x1f},
    {0x42,0x8a},
    {0x43,0x86},
    {0x44,0x0a},
    {0x45,0x84},
    {0x46,0x87},
    {0x47,0x9b},
    {0x48,0x23},
    {0x50,0x8c},
    {0x51,0x0c},
    {0x52,0x00},
    {0x53,0x07},
    {0x54,0x17},
    {0x55,0x9d},
    {0x56,0x00},
    {0x57,0x0b},
    {0x58,0x89},
    {0x80,0x03},
    {0x85,0x40},
    {0x87,0x02},
    {0x88,0x00},
    {0x89,0x00},
    {0x8a,0x00},
    {0x03,0x16},
    {0x10,0x31},
    {0x18,0x37},
    {0x19,0x36},
    {0x1a,0x0e},
    {0x1b,0x01},
    {0x1c,0xdc},
    {0x1d,0xfe},
    {0x30,0x00},
    {0x31,0x06},
    {0x32,0x1d},
    {0x33,0x33},
    {0x34,0x53},
    {0x35,0x6c},
    {0x36,0x81},
    {0x37,0x94},
    {0x38,0xa4},
    {0x39,0xb3},
    {0x3a,0xc0},
    {0x3b,0xcb},
    {0x3c,0xd5},
    {0x3d,0xde},
    {0x3e,0xe6},
    {0x3f,0xee},
    {0x40,0xf5},
    {0x41,0xfc},
    {0x42,0xff},
    {0x50,0x00},
    {0x51,0x03},
    {0x52,0x19},
    {0x53,0x34},
    {0x54,0x58},
    {0x55,0x75},
    {0x56,0x8d},
    {0x57,0xa1},
    {0x58,0xb2},
    {0x59,0xbe},
    {0x5a,0xc9},
    {0x5b,0xd2},
    {0x5c,0xdb},
    {0x5d,0xe3},
    {0x5e,0xeb},
    {0x5f,0xf0},
    {0x60,0xf5},
    {0x61,0xf7},
    {0x62,0xf8},
    {0x70,0x00},
    {0x71,0x08},
    {0x72,0x17},
    {0x73,0x2f},
    {0x74,0x53},
    {0x75,0x6c},
    {0x76,0x81},
    {0x77,0x94},
    {0x78,0xa4},
    {0x79,0xb3},
    {0x7a,0xc0},
    {0x7b,0xcb},
    {0x7c,0xd5},
    {0x7d,0xde},
    {0x7e,0xe6},
    {0x7f,0xee},
    {0x80,0xf4},
    {0x81,0xfa},
    {0x82,0xff},
    {0x03,0x17},
    {0xc4,0x3c},
    {0xc5,0x32},

    {0x03,0x20},
    {0x11,0x1c},
    {0x20,0x01},
    {0x21,0x30},
    {0x22,0x10},
    {0x23,0x00},
    {0x24,0x04},
    {0x28,0xff},
    {0x29,0xad},
    {0x2a,0xf0},
    {0x2b,0x34},
    {0x30,0x78},
    {0x2c,0xc3},
    {0x2d,0x5f},
    {0x2e,0x33},
    {0x32,0x03},
    {0x33,0x2e},
    {0x34,0x30},
    {0x35,0xd4},
    {0x36,0xfe},
    {0x37,0x32},
    {0x38,0x04},
    {0x47,0xf0},
    {0x50,0x45},
    {0x51,0x88},
    {0x56,0x10},
    {0x57,0xb7},
    {0x58,0x14},
    {0x59,0x88},
    {0x5a,0x04},
    {0x60,0x55},
    {0x61,0x55},
    {0x62,0x6a},
    {0x63,0xa9},
    {0x64,0x6a},
    {0x65,0xa9},
    {0x66,0x6a},
    {0x67,0xa9},
    {0x68,0x6b},
    {0x69,0xe9},
    {0x6a,0x6a},
    {0x6b,0xa9},
    {0x6c,0x6a},
    {0x6d,0xa9},
    {0x6e,0x55},
    {0x6f,0x55},
    {0x70,0x46},
    {0x71,0xbb},
    {0x76,0x21},
    {0x77,0x02},
    {0x78,0x22},
    {0x79,0x2a},
    {0x78,0x24},
    {0x79,0x23},
    {0x7a,0x23},
    {0x7b,0x22},
    {0x7d,0x23},
    {0x83,0x01},
    {0x84,0x07},
    {0x85,0xac},
    {0x86,0x01},
    {0x87,0xf4},
    {0x88,0x04},
    {0x89,0xce},
    {0x8a,0x78},
    {0x8b,0x57},
    {0x8c,0xe4},
    {0x8d,0x49},
    {0x8e,0x3e},
    {0x98,0xdc},
    {0x99,0x45},
    {0x9a,0x0d},
    {0x9b,0xde},
    {0x9c,0x0f},
    {0x9d,0xa0},
    {0x9e,0x01},
    {0x9f,0xf4},
    {0xa0,0x03},
    {0xa1,0xa9},
    {0xa2,0x80},
    {0xb0,0x1d}, /* AG */
    {0xb1,0x1a},
    {0xb2,0x80},
    {0xb3,0x1a},
    {0xb4,0x1a},
    {0xb5,0x44},
    {0xb6,0x2f},
    {0xb7,0x28},
    {0xb8,0x25},
    {0xb9,0x22},
    {0xba,0x21},
    {0xbb,0x20},
    {0xbc,0x1f},
    {0xbd,0x1f},
    {0xc0,0x30},
    {0xc1,0x20},
    {0xc2,0x20},
    {0xc3,0x20},
    {0xc4,0x08},
    {0xc8,0x80},
    {0xc9,0x40},
    {0x03,0x22},
    {0x10,0xfd},
    {0x11,0x2c},
    {0x19,0x01},
    {0x20,0x30},
    {0x21,0x80},
    {0x23,0x08},
    {0x24,0x01},
    {0x30,0x80},
    {0x31,0x80},
    {0x38,0x11},
    {0x39,0x34},
    {0x40,0xf7},
    {0x41,0x77},
    {0x42,0x55},
    {0x43,0xf0},
    {0x44,0x66},
    {0x45,0x33},
    {0x46,0x00},
    {0x47,0x94},
    {0x50,0xb2},
    {0x51,0x81},
    {0x52,0x98},
    {0x80,0x38},
    {0x81,0x20},
    {0x82,0x3a},
    {0x83,0x5e},
    {0x84,0x20},
    {0x85,0x53},
    {0x86,0x15},
    {0x87,0x54},
    {0x88,0x20},
    {0x89,0x45},
    {0x8a,0x2a},
    {0x8b,0x46},
    {0x8c,0x3f},
    {0x8d,0x34},
    {0x8e,0x2c},
    {0x8f,0x60},
    {0x90,0x5f},
    {0x91,0x5c},
    {0x92,0x4c},
    {0x93,0x41},
    {0x94,0x3b},
    {0x95,0x36},
    {0x96,0x30},
    {0x97,0x27},
    {0x98,0x20},
    {0x99,0x1c},
    {0x9a,0x19},
    {0x9b,0x88},
    {0x9c,0x88},
    {0x9d,0x48},
    {0x9e,0x38},
    {0x9f,0x30},
    {0xa0,0x74},
    {0xa1,0x35},
    {0xa2,0xaf},
    {0xa3,0xf7},
    {0xa4,0x10},
    {0xa5,0x50},
    {0xa6,0xc4},
    {0xad,0x40},
    {0xae,0x4a},
    {0xaf,0x2a},
    {0xb0,0x29},
    {0xb1,0x20},
    {0xb4,0xff},
    {0xb8,0x6b},
    {0xb9,0x00},

    {0x03,0x24},
    {0x10,0x01},
    {0x18,0x06},
    {0x30,0x06},
    {0x31,0x90},
    {0x32,0x25},
    {0x33,0xa2},
    {0x34,0x26},
    {0x35,0x58},
    {0x36,0x60},
    {0x37,0x00},
    {0x38,0x50},
    {0x39,0x00},

    {0x03,0x20},
    {0x10,0x9c},

    {0x03,0x22},
    {0x10,0xe9},

    {0x03,0x00},
    {0x0e,0x00},
    {0x0e,0x62},

    {0xff,0x01},

    {0x03,0x00},
    {0x01,0xe0}
};

/* DONE */
static void hi253_init_reg(hi253_reg_t *reg, int n)
{
	int i;

    for(i=0; i<n; i++)
	{
		if (reg[i].addr == 0xff)
			udelay(1000);
		else
		    hi253_write_reg(reg[i].addr, reg[i].data);
	}
}

/* DONE */
static int hi253_init_1600_1200(void)
{
	hi253_init_reg(init_arrb, sizeof(init_arrb)/sizeof(init_arrb[0]));
	hi253_init_reg(init_arr1, sizeof(init_arr1)/sizeof(init_arr1[0]));
	hi253_init_reg(init_arre, sizeof(init_arre)/sizeof(init_arre[0]));
	return 0;
}

/* DONE */
static int hi253_init_800_600(void)
{
	hi253_init_reg(init_arrb, sizeof(init_arrb)/sizeof(init_arrb[0]));
	hi253_init_reg(init_arr0, sizeof(init_arr0)/sizeof(init_arr0[0]));
	hi253_init_reg(init_arre, sizeof(init_arre)/sizeof(init_arre[0]));
	return 0;
}

#if 0
}
#endif

static int g_bri = 0x80;
static int g_hue;
static int g_sat = 0x80;
static int g_con = 0x80;
static int g_red = 0x80;
static int g_blue = 0x80;

/* DONE */
static void
hi253_set_color(int bri, int hue, int sat, int con, int r, int b)
{
    if (bri >= 0) {
    	hi253_set_yoffset((unsigned char)bri);
    	g_bri = bri;
    	printk(KERN_ERR "--- hi253.c: set brightness to %x ---\n", bri);
    }
    
    if (hue >= 0) {
    	g_hue = hue;
    }

    if (sat >= 0) {
    	hi235_set_saturation((unsigned char)sat);
    	g_red = sat;
    	g_blue = sat;
    	printk(KERN_ERR "--- hi253.c: set saturation to %x ---\n", sat);
    }
    
    if (con >= 0) {
    	hi253_set_contrast((unsigned char)con);
    	g_con = con;
    	printk(KERN_ERR "--- hi253.c: set contrast to %x ---\n", con);
    }
    
    if (r >= 0) {
    	hi253_select_reg_page(0x10);
	    hi253_set_red((unsigned char)r);
	    g_red = r;
	    printk(KERN_ERR "--- hi253.c: set red balance to %x ---\n", r);
    }

    if (b >= 0) {
    	hi253_select_reg_page(0x10);
	    hi253_set_blue((unsigned char)b);
	    g_blue = b;
	    printk(KERN_ERR "--- hi253.c: set blue balance to %x ---\n", b);
    }
}

/* DONE */
static void hi253_get_color(int *bri, int *hue, int *sat, int *con, int *red, int *blue)
{
    *bri = g_bri;
    *hue = g_hue;
    *sat = g_sat;
    *con = g_con;
    *red = g_red;
    *blue = g_blue;
    printk(KERN_ERR "--- hi253.c: get color ---\n");
}

/*!
 * hi253 sensor set AE measurement window mode configuration
 *
 * @param ae_mode      int
 * @return  None
 */
static void hi253_set_ae_mode(int ae_mode)
{
}

/*!
 * hi253 sensor get AE measurement window mode configuration
 *
 * @param ae_mode      int *
 * @return  None
 */
static void hi253_get_ae_mode(int *ae_mode)
{
}

static void hi253_set_ae(int active)
{
}

static void hi253_flicker_control(int control)
{
}

static void hi253_get_control_params(int *ae, int *awb, int *flicker)
{
}

/////////////////////////////////////////////////////
#if 0
{
#endif

/* DONE */
static void hi253_interface(sensor_interface * param, u32 width, u32 height)
{
	param->Vsync_pol = 0x0;
	param->clk_mode = 0x0;	//gated
	param->pixclk_pol = 0x0;
	param->data_width = 0x1;
	param->data_pol = 0x0;
	param->ext_vsync = 0x0;
	param->Vsync_pol = 0x0;
	param->Hsync_pol = 0x0;
	param->width = width - 1;
	param->height = height - 1;
	param->pixel_fmt = IPU_PIX_FMT_UYVY;
	param->mclk = 27000000;
}

/* DONE */
extern cam_data *g_cam; //defined in mxc_csi.c
static sensor_interface *hi253_config(int *frame_rate, int high_quality)
{
	u32 out_width, out_height;

	if (high_quality) {
		out_width = 1600;
		out_height = 1200;
		g_cam->crop_bounds.left = 0;
		g_cam->crop_bounds.width = 1600;
		g_cam->crop_bounds.top = 0;
		g_cam->crop_bounds.height = 1200;
		g_cam->crop_current = g_cam->crop_defrect = g_cam->crop_bounds;
		g_cam->streamparm.parm.capture.capturemode = 1;
	} else {
		out_width = 800;
		out_height = 600;
		g_cam->crop_bounds.left = 0;
		g_cam->crop_bounds.width = 800;
		g_cam->crop_bounds.top = 0;
		g_cam->crop_bounds.height = 600;
		g_cam->crop_current = g_cam->crop_defrect = g_cam->crop_bounds;
		g_cam->streamparm.parm.capture.capturemode = 0;
	}

	hi253_interface(interface_param, out_width, out_height);
	set_mclk_rate(&interface_param->mclk);

	if (high_quality) {
		hi253_init_1600_1200();
	} else {
		hi253_init_800_600();
	}

	return interface_param;
}

/* DONE */
static sensor_interface *hi253_reset(void)
{
	return hi253_config(&reset_frame_rate, V4L2_MODE_HIGHQUALITY);
//	return hi253_config(&reset_frame_rate, 0);
}

/* DONE */
static int hi253_get_status(void)
{
	unsigned char dev_id = 0;

	if (!interface_param)
		return -ENODEV;

    dev_id = hi253_get_id();

	return ((dev_id == 0x92) ? 0 : -1);
}

struct camera_sensor cmos_sensor_if = {
	.set_std = NULL,
	.query_ctrl = NULL,
	.set_flip = hi253_set_flip,
	.set_color = hi253_set_color,
	.get_color = hi253_get_color,
	.set_ae_mode = hi253_set_ae_mode,
	.get_ae_mode = hi253_get_ae_mode,
	.set_ae = hi253_set_ae,
	.set_awb = hi253_set_awb,
	.flicker_control = hi253_flicker_control,
	.get_control_params = hi253_get_control_params,
	.config = hi253_config,
	.reset = hi253_reset,
	.get_status = hi253_get_status,
};

extern void gpio_sensor_active(void);
extern int  gpio_sensor_activated;

/*!
 * hi253 init function
 *
 * @return  Error code indicating success or failure
 */
static __init int hi253_init(void)
{
	u8 err;
#ifdef CONFIG_MACH_MX27_SYP
#elif defined(CONFIG_MACH_MX27_TUOSI)
#elif defined(CONFIG_MACH_MX27_MPK271)
#else
	PMIC_STATUS ret;
	t_regulator_voltage voltage;

#ifdef CONFIG_MACH_MX27_MDK27V1
	/*AVDD--2.8v */
	voltage.vcam = VCAM_2_8V;
	if ((ret = pmic_power_regulator_set_voltage(REGU_VCAM, voltage)) < 0) {
		printk(KERN_ERR "%s:vmmc1 set voltage error:%d\n", __func__, ret);
		return -1;
	} else {
		printk(KERN_INFO "%s:vcam set voltage ok\n", __func__);
	}
	if ((ret = pmic_power_regulator_on(REGU_VCAM)) < 0) {
		printk(KERN_ERR "%s:vcam power on error:%d\n", __func__, ret);
		return -1;
	} else {
		printk(KERN_INFO "%s:vcam power on ok\n", __func__);
	}
#else
	/*AVDD--2.8v */
	voltage.vmmc1 = VMMC1_2_8V;
	if ((ret = pmic_power_regulator_set_voltage(REGU_VMMC1, voltage)) < 0) {
		printk(KERN_ERR "%s:vmmc1 set voltage error:%d\n", __func__,
		       ret);
		return -1;
	} else {
		printk(KERN_INFO "%s:vmmc1 set voltage ok\n", __func__);
	}
	if ((ret = pmic_power_regulator_on(REGU_VMMC1)) < 0) {
		printk(KERN_ERR "%s:vmmc1 power on error:%d\n", __func__, ret);
		return -1;
	} else {
		printk(KERN_INFO "%s:vmmc1 power on ok\n", __func__);
	}
#endif

	/*DVDD--1.3v */
	voltage.vvib = VVIB_1_3V;
	if ((ret = pmic_power_regulator_set_voltage(REGU_VVIB, voltage)) < 0) {
		printk(KERN_ERR "%s:VVIB set voltage error:%d\n", __func__,
		       ret);
		return -1;
	} else {
		printk(KERN_INFO "%s:VVIB set voltage ok\n", __func__);
	}
	if ((ret = pmic_power_regulator_on(REGU_VVIB)) < 0) {
		printk(KERN_ERR "%s:VVIB power regulator on error:%d\n",
		       __func__, ret);
		return -1;
	} else {
		printk(KERN_INFO "%s:VVIB power on ok\n", __func__);
	}

	/*1V8_SW2A for I2C */
	voltage.sw2a = SW2A_1_8V;
	if ((ret = pmic_power_regulator_set_voltage(SW_SW2A, voltage)) < 0) {
		printk(KERN_ERR "%s:SW2A  set voltage error:%d\n", __func__, ret);
		return -1;
	} else {
		printk(KERN_INFO "%s: SW2A power on\n", __func__);
	}

	if ((ret = pmic_power_regulator_set_voltage(SW_SW2B, voltage)) < 0) {
		printk(KERN_ERR "%s:SW2B set voltage error:%d\n", __func__, ret);
		return -1;
	} else {
		printk(KERN_INFO "%s:SW2B set voltage ok\n", __func__);
	}
	if (pmic_power_set_regen_assig(REGU_GPO3, 1) < 0) {
		printk(KERN_ERR "%s:set_regen_assig error\n", __func__);
		return -1;
	} else {
		printk(KERN_INFO "%s:set_regen_assig ok\n", __func__);
	}
	if ((ret = pmic_power_regulator_on(REGU_GPO3)) < 0) {
		printk(KERN_ERR "%s:REGU_GPO3 power on error:%d\n", __func__, ret);
		return -1;
	} else {
		printk(KERN_INFO "%s:REGU_GPO3 power on ok\n", __func__);
	}
#endif				/* CONFIG_MACH_MX27_SYP */

    if (gpio_sensor_activated == 0) {
       gpio_sensor_active();
       gpio_sensor_activated = 1;
    }
	err = i2c_add_driver(&hi253_i2c_driver);

	return err;
}

extern void gpio_sensor_inactive(void);

/*!
 * OV2640 cleanup function
 *
 * @return  Error code indicating success or failure
 */
static void __exit hi253_clean(void)
{
	i2c_del_driver(&hi253_i2c_driver);
    if (gpio_sensor_activated == 1) {
       gpio_sensor_inactive();
       gpio_sensor_activated = 0;
    }
}

module_init(hi253_init);
module_exit(hi253_clean);

/* Exported symbols for modules. */
EXPORT_SYMBOL(cmos_sensor_if);

MODULE_AUTHOR("Freescale Semiconductor, Inc.");
MODULE_DESCRIPTION("OV2640 Camera Driver");
MODULE_LICENSE("GPL");

#if 0
}
#endif
