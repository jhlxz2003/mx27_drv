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
 * @file mx27_v4l2_capture.c
 *
 * @brief MX27 Video For Linux 2 driver
 *
 * @ingroup MXC_V4L2_CAPTURE
 */

#include <linux/module.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/slab.h>
#include <linux/ctype.h>
#include <linux/pagemap.h>
#include <linux/vmalloc.h>
#include <linux/types.h>
#include <linux/fb.h>
#include <linux/pci.h>
#include <linux/platform_device.h>
#include <linux/version.h>
#include <media/v4l2-dev.h>
#include <asm/io.h>
#include <asm/semaphore.h>

#include "mxc_v4l2_capture.h"
#include "mx27_prp.h"
#include "mx27_csi.h"

//#define BYPASS    1

static int csi_mclk_flag_backup;
static int video_nr = -1;
extern cam_data *g_cam;

static int dq_intr_cnt = 0;
static int dq_timeout_cnt = 0;
static int empty_wq_cnt = 0;

/////////////////////////////////////////////
/* new add on 2012.04.23 for deinterlace */
static struct tasklet_struct deinterlace_tasklet;
static struct list_head undi_q;

/* new add on 2012.04.23,1-need to deinterlace. */
static int   g_dev_minor;
static int   g_image_sz;
static int   g_pixel_sz;
/////////////////////////////////////////////

extern struct camera_sensor cmos_sensor_if;
extern struct camera_sensor ccd_sensor_if;

extern void mxc_camera_select(int i);
extern void mxc_ccd_enable(int i);
extern void mxc_cmos_enable(int i);

extern void gpio_sensor_active(void);
extern void gpio_sensor_inactive(void);
extern void mxc_tvp5150_reset(void);

static struct camera_sensor *g_sensor[2] = {&cmos_sensor_if, &ccd_sensor_if};
static struct video_device  *g_vdev[2];

typedef void (*PWRFUNC)(int);
static PWRFUNC  g_pwr_on[2] = {&mxc_cmos_enable, &mxc_ccd_enable};

int gpio_sensor_activated = 0;

/*!
 * Free frame buffers
 *
 * @param cam      Structure cam_data *
 *
 * @return status  0 success.
 */
static int mxc_free_frame_buf(cam_data * cam)
{
	int i;

	for (i = 0; i < FRAME_NUM; i++) {
		if (cam->frame[i].vaddress != 0) {
			dma_free_coherent(0,
					  cam->frame[i].buffer.length,
					  cam->frame[i].vaddress,
					  cam->frame[i].paddress);
			cam->frame[i].vaddress = 0;
		}
	}

	return 0;
}

/*!
 * Allocate frame buffers
 *
 * @param cam      Structure cam_data *
 *
 * @param count    int number of buffer need to allocated
 *
 * @return status  -0 Successfully allocated a buffer, -ENOBUFS	failed.
 */
 /*
static int mxc_allocate_frame_buf(cam_data * cam, int count)
{
	int i;

	for (i = 0; i < count; i++) {
#ifdef ENABLE_DEINTERLACE_SCALE
		cam->frame[i].vaddress = dma_alloc_coherent(0,
							    PAGE_ALIGN(cam->v2f.fmt.pix.sizeimage*2),
							    &cam->frame[i].paddress,
							    GFP_DMA|GFP_KERNEL);
#else
		cam->frame[i].vaddress = dma_alloc_coherent(0,
							    PAGE_ALIGN(cam->v2f.fmt.pix.sizeimage),
							    &cam->frame[i].paddress,
							    GFP_DMA |GFP_KERNEL);
#endif
		if (cam->frame[i].vaddress == 0) {
			pr_debug("mxc_allocate_frame_buf failed.\n");
			mxc_free_frame_buf(cam);
			return -ENOBUFS;
		}
		cam->frame[i].buffer.index = i;
		cam->frame[i].buffer.flags = V4L2_BUF_FLAG_MAPPED;
		cam->frame[i].buffer.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
#ifdef ENABLE_DEINTERLACE_SCALE
		cam->frame[i].buffer.length = PAGE_ALIGN(cam->v2f.fmt.pix.sizeimage * 2);
#else
		cam->frame[i].buffer.length = PAGE_ALIGN(cam->v2f.fmt.pix.sizeimage);
#endif
		cam->frame[i].buffer.memory = V4L2_MEMORY_MMAP;
		cam->frame[i].buffer.m.offset = cam->frame[i].paddress;
		cam->frame[i].index = i;
	}

	return 0;
}
*/
/* new modified on 2012.04.23 for capturing deinterlace */
static int mxc_allocate_frame_buf(cam_data * cam, int count)
{
	int i;

	for (i = 0; i < count; i++) {
		if (g_dev_minor) {
			cam->frame[i].vaddress = dma_alloc_coherent(0,
							    PAGE_ALIGN(cam->v2f.fmt.pix.sizeimage*2),
							    &cam->frame[i].paddress,
							    GFP_DMA|GFP_KERNEL);
		} else {
			cam->frame[i].vaddress = dma_alloc_coherent(0,
							    PAGE_ALIGN(cam->v2f.fmt.pix.sizeimage),
							    &cam->frame[i].paddress,
							    GFP_DMA |GFP_KERNEL);
		}

		if (cam->frame[i].vaddress == 0) {
			pr_debug("mxc_allocate_frame_buf failed.\n");
			mxc_free_frame_buf(cam);
			return -ENOBUFS;
		}
		cam->frame[i].buffer.index = i;
		cam->frame[i].buffer.flags = V4L2_BUF_FLAG_MAPPED;
		cam->frame[i].buffer.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
		if (g_dev_minor) {
			cam->frame[i].buffer.length = PAGE_ALIGN(cam->v2f.fmt.pix.sizeimage * 2);
		} else {
			cam->frame[i].buffer.length = PAGE_ALIGN(cam->v2f.fmt.pix.sizeimage);
		}
		cam->frame[i].buffer.memory = V4L2_MEMORY_MMAP;
		cam->frame[i].buffer.m.offset = cam->frame[i].paddress;
		cam->frame[i].index = i;
	}

	return 0;
}

/*!
 * Free frame buffers status
 *
 * @param cam    Structure cam_data *
 *
 * @return none
 */
static void mxc_free_frames(cam_data * cam)
{
	int i;

	for (i = 0; i < FRAME_NUM; i++) {
		cam->frame[i].buffer.flags = V4L2_BUF_FLAG_MAPPED;
	}

	cam->enc_counter = 0;
	cam->skip_frame = 0;
	INIT_LIST_HEAD(&cam->ready_q);
	INIT_LIST_HEAD(&cam->working_q);
	INIT_LIST_HEAD(&cam->done_q);

	/* new add on 2012.04.23 for capturing deinterlace */
	INIT_LIST_HEAD(&undi_q);
}

/*!
 * Return the buffer status
 *
 * @param cam 	   Structure cam_data *
 * @param buf      Structure v4l2_buffer *
 *
 * @return status  0 success, EINVAL failed.
 */
static int mxc_v4l2_buffer_status(cam_data * cam, struct v4l2_buffer *buf)
{
	/* check range */
	if (buf->index < 0 || buf->index >= FRAME_NUM) {
		pr_debug("mxc_v4l2_buffer_status buffers not allocated\n");
		return -EINVAL;
	}

	memcpy(buf, &(cam->frame[buf->index].buffer), sizeof(*buf));
	return 0;
}

/* new add on 2012.04.23 for deinterlace */
static void deinterlace(unsigned long private)
{
	struct mxc_v4l_frame *frame;
	cam_data *cam = (cam_data *) private;
	u8  *d;
	u8  *s0;
	u8  *s1;
	int    i, w, h;

	if (list_empty(&undi_q)) {
		printk(KERN_ERR "--- no image need to be deinterlaced ---\n");
		return;
	}

	w = cam->v2f.fmt.pix.width;
	h = cam->v2f.fmt.pix.height;

	frame = list_entry(undi_q.next, struct mxc_v4l_frame, queue);
#ifdef BYPASS
	s0 = (u8 *)frame->vaddress + g_image_sz;
	d =  (u8 *)frame->vaddress;
	memcpy(d, s0, g_image_sz);
#else
	s0 = (u8 *)frame->vaddress + g_image_sz;
	s1 = s0 + (g_pixel_sz>>1);
	d =  (u8 *)frame->vaddress;
	for (i=0; i<h/2; ++i) {
		memcpy(d, s0, w);
		s0 += w;
		d += w;
		memcpy(d, s1, w);
		s1 += w;
		d += w;
	}

	w >>= 1;
	h >>= 1;
	s0 = s1+((w*h)>>1);
	for (i=0; i<h/2; ++i) {
		memcpy(d, s1, w);
		s1 += w;
		d += w;
		memcpy(d, s0, w);
		s0 += w;
		d += w;
	}
	s1 = s0+((w*h)>>1);
	for (i=0; i<h/2; ++i) {
		memcpy(d, s0, w);
		s0 += w;
		d += w;
		memcpy(d, s1, w);
		s1 += w;
		d += w;
	}
#endif

	list_del(undi_q.next);
	list_add_tail(&frame->queue, &cam->done_q);
	cam->enc_counter++;
	wake_up_interruptible(&cam->enc_queue);
}

/*!
 * start the encoder job
 *
 * @param cam      structure cam_data *
 *
 * @return status  0 Success
 */
static int mxc_streamon(cam_data * cam)
{
	struct mxc_v4l_frame *frame;
	int err = 0;

	if (!cam)
		return -EIO;

	if (list_empty(&cam->ready_q)) {
		printk(KERN_ERR "mxc_streamon buffer not been queued yet\n");
		return -EINVAL;
	}

	cam->capture_pid = current->pid;

	if (cam->enc_enable) {
		err = cam->enc_enable(cam);
		if (err != 0) {
			return err;
		}
	}

	cam->ping_pong_csi = 0;
	if (cam->enc_update_eba) {
		frame = list_entry(cam->ready_q.next, struct mxc_v4l_frame, queue);
		list_del(cam->ready_q.next);
		list_add_tail(&frame->queue, &cam->working_q);
		/* new modified on 2012.04.23 for deinterlace */
		err = cam->enc_update_eba(frame->paddress+g_dev_minor*g_image_sz, &cam->ping_pong_csi);

		frame = list_entry(cam->ready_q.next, struct mxc_v4l_frame, queue);
		list_del(cam->ready_q.next);
		list_add_tail(&frame->queue, &cam->working_q);
		/* new modified on 2012.04.23 for deinterlace */
		err |= cam->enc_update_eba(frame->paddress+g_dev_minor*g_image_sz, &cam->ping_pong_csi);

		/* new modified on 2012.04.23 for deinterlace */
		if (g_dev_minor == 1) {
			tasklet_init(&deinterlace_tasklet, deinterlace, (unsigned long)cam);
		}
	} else {
		return -EINVAL;
	}

	return err;
}

/*!
 * Shut down the encoder job
 *
 * @param cam      structure cam_data *
 *
 * @return status  0 Success
 */
static int mxc_streamoff(cam_data * cam)
{
	int err = 0;

	if (!cam)
		return -EIO;

	if (cam->enc_disable) {
		err = cam->enc_disable(cam);
	}

	/* new add on 2012.04.23 for deinterlace */
	if (g_dev_minor == 1) {
		tasklet_kill(&deinterlace_tasklet);
	}

	mxc_free_frames(cam);
	return err;
}

/*!
 * Valid whether the palette is supported
 *
 * @param palette pixel format
 *
 * @return 0 if failed
 */
static inline int valid_mode(u32 palette)
{
	/*
	 * MX27 PrP channel 2 supports YUV444, but YUV444 is not
	 * defined by V4L2 :(
	 */
	return ((palette == V4L2_PIX_FMT_YUYV) ||
		(palette == V4L2_PIX_FMT_YUV420));
}

/*!
 * Valid and adjust the overlay window size, position
 *
 * @param cam      structure cam_data *
 * @param win      struct v4l2_window  *
 *
 * @return 0
 */
static int verify_preview(cam_data * cam, struct v4l2_window *win)
{
	if (cam->output >= num_registered_fb) {
		pr_debug("verify_preview No matched.\n");
		return -1;
	}
	cam->overlay_fb = (struct fb_info *)registered_fb[cam->output];

	/* TODO: suppose 16bpp, 4 bytes alignment */
	win->w.left &= ~0x1;

	if (win->w.width + win->w.left > cam->overlay_fb->var.xres)
		win->w.width = cam->overlay_fb->var.xres - win->w.left;
	if (win->w.height + win->w.top > cam->overlay_fb->var.yres)
		win->w.height = cam->overlay_fb->var.yres - win->w.top;

	/*
	 * TODO: suppose 16bpp. Rounded down to a multiple of 2 pixels for
	 * width according to PrP limitations.
	 */
	if ((cam->rotation == V4L2_MXC_ROTATE_90_RIGHT)
	    || (cam->rotation == V4L2_MXC_ROTATE_90_RIGHT_VFLIP)
	    || (cam->rotation == V4L2_MXC_ROTATE_90_RIGHT_HFLIP)
	    || (cam->rotation == V4L2_MXC_ROTATE_90_LEFT))
		win->w.height &= ~0x1;
	else
		win->w.width &= ~0x1;

	return 0;
}

/*!
 * start the viewfinder job
 *
 * @param cam      structure cam_data *
 *
 * @return status  0 Success
 */
static int start_preview(cam_data * cam)
{
	int err = 0;

	err = prp_vf_select(cam);
	if (err != 0)
		return err;

	cam->overlay_pid = current->pid;
	err = cam->vf_start_sdc(cam);

	return err;
}

/*!
 * shut down the viewfinder job
 *
 * @param cam      structure cam_data *
 *
 * @return status  0 Success
 */
static int stop_preview(cam_data * cam)
{
	int err = 0;

	err = prp_vf_deselect(cam);
	return err;
}

/*!
 * V4L2 - mxc_v4l2_g_fmt function
 *
 * @param cam         structure cam_data *
 *
 * @param f           structure v4l2_format *
 *
 * @return  status    0 success, EINVAL failed
 */
static int mxc_v4l2_g_fmt(cam_data * cam, struct v4l2_format *f)
{
	int retval = 0;

	switch (f->type) {
	case V4L2_BUF_TYPE_VIDEO_CAPTURE:
		f->fmt.pix.width = cam->v2f.fmt.pix.width;
		f->fmt.pix.height = cam->v2f.fmt.pix.height;
		f->fmt.pix.sizeimage = cam->v2f.fmt.pix.sizeimage;
		f->fmt.pix.pixelformat = cam->v2f.fmt.pix.pixelformat;
		f->fmt.pix.bytesperline = cam->v2f.fmt.pix.bytesperline;
		f->fmt.pix.colorspace = V4L2_COLORSPACE_JPEG;
		retval = 0;
		break;
	case V4L2_BUF_TYPE_VIDEO_OVERLAY:
		f->fmt.win = cam->win;
		break;
	default:
		retval = -EINVAL;
	}
	return retval;
}

/*!
 * V4L2 - mxc_v4l2_s_fmt function
 *
 * @param cam         structure cam_data *
 *
 * @param f           structure v4l2_format *
 *
 * @return  status    0 success, EINVAL failed
 */
static int mxc_v4l2_s_fmt(cam_data * cam, struct v4l2_format *f)
{
	int retval = 0;
	int size = 0;
	int bytesperline = 0;

	switch (f->type) {
	case V4L2_BUF_TYPE_VIDEO_CAPTURE:
		if (!valid_mode(f->fmt.pix.pixelformat)) {
			pr_debug("mxc_v4l2_s_fmt: format not supported\n");
			retval = -EINVAL;
		}

		if (cam->rotation != V4L2_MXC_ROTATE_NONE)
			pr_debug("mxc_v4l2_s_fmt: capture rotation ignored\n");

		switch (f->fmt.pix.pixelformat) {
		case V4L2_PIX_FMT_YUYV:
			f->fmt.pix.width &= ~0x1;	/* Multiple of 2 */
			size = f->fmt.pix.width * f->fmt.pix.height * 2;
			bytesperline = f->fmt.pix.width * 2;
			break;
		case V4L2_PIX_FMT_YUV420:
			f->fmt.pix.width &= ~0x7;	/* Multiple of 8 */
			f->fmt.pix.height &= ~0x1;	/* Multiple of 2 */
			size = f->fmt.pix.width * f->fmt.pix.height * 3 / 2;
			bytesperline = f->fmt.pix.width * 3 / 2;
			break;
		default:
			/* Suppose it's YUV444 or 32bpp */
			size = f->fmt.pix.width * f->fmt.pix.height * 4;
			bytesperline = f->fmt.pix.width * 4;
			pr_info("mxc_v4l2_s_fmt: default assume"
				" to be YUV444 interleaved.\n");
			break;
		}

		if (f->fmt.pix.bytesperline < bytesperline) {
			f->fmt.pix.bytesperline = bytesperline;
		} else {
			bytesperline = f->fmt.pix.bytesperline;
		}

		if (f->fmt.pix.sizeimage > size) {
			pr_debug("mxc_v4l2_s_fmt: sizeimage bigger than needed.\n");
			size = f->fmt.pix.sizeimage;
		}
		f->fmt.pix.sizeimage = size;

		cam->v2f.fmt.pix.sizeimage = size;
		cam->v2f.fmt.pix.bytesperline = bytesperline;
		cam->v2f.fmt.pix.width = f->fmt.pix.width;
		cam->v2f.fmt.pix.height = f->fmt.pix.height;
		cam->v2f.fmt.pix.pixelformat = f->fmt.pix.pixelformat;
		/* new add on 2012.04.23 for deinterlace */
		g_image_sz = (PAGE_ALIGN(size*2))>>1;
		g_pixel_sz = f->fmt.pix.width*f->fmt.pix.height;
		retval = 0;
		break;
	case V4L2_BUF_TYPE_VIDEO_OVERLAY:
		retval = verify_preview(cam, &f->fmt.win);
		cam->win = f->fmt.win;
		break;
	default:
		retval = -EINVAL;
	}
	return retval;
}

/*!
 * get control param
 *
 * @param cam         structure cam_data *
 *
 * @param c           structure v4l2_control *
 *
 * @return  status    0 success, EINVAL failed
 */
static int mxc_get_v42l_control(cam_data * cam, struct v4l2_control *c)
{
	int status = 0;

	switch (c->id) {
	case V4L2_CID_HFLIP:
		c->value = cam->rotation;
		break;
	case V4L2_CID_VFLIP:
		c->value = cam->rotation;
		break;
	case V4L2_CID_MXC_ROT:
		c->value = cam->rotation;
		break;
	case V4L2_CID_BRIGHTNESS:
		c->value = cam->bright;
		break;
	case V4L2_CID_HUE:
		c->value = cam->hue;
		break;
	case V4L2_CID_CONTRAST:
		c->value = cam->contrast;
		break;
	case V4L2_CID_SATURATION:
		c->value = cam->saturation;
		break;
	case V4L2_CID_RED_BALANCE:
		c->value = cam->red;
		break;
	case V4L2_CID_BLUE_BALANCE:
		c->value = cam->blue;
		break;
	case V4L2_CID_BLACK_LEVEL:
		c->value = cam->ae_mode;
		break;
	case V4L2_CID_AUTO_WHITE_BALANCE:
		c->value = cam->awb_enable;
		break;
	case V4L2_CID_AUTOGAIN:
		c->value = cam->ae_enable;
		break;
	case V4L2_CID_MXC_FLICKER:
		c->value = cam->flicker_ctrl;
		break;
	default:
		status = -EINVAL;
	}
	return status;
}

/*!
 * V4L2 - set_control function
 * V4L2_CID_MXC_ROT is the extention for rotation/mirroring.
 *
 * @param cam         structure cam_data *
 *
 * @param c           structure v4l2_control *
 *
 * @return  status    0 success, EINVAL failed
 */
static int mxc_set_v42l_control(cam_data * cam, struct v4l2_control *c)
{
	switch (c->id) {
	case V4L2_CID_HFLIP:
		if (cam->cam_sensor->set_flip == NULL) {
		    if (c->value == 1) {
			    if ((cam->rotation != V4L2_MXC_ROTATE_VERT_FLIP) &&
			        (cam->rotation != V4L2_MXC_ROTATE_180))
				    cam->rotation = V4L2_MXC_ROTATE_HORIZ_FLIP;
			    else
				    cam->rotation = V4L2_MXC_ROTATE_180;
		    } else {
			    if (cam->rotation == V4L2_MXC_ROTATE_HORIZ_FLIP)
				    cam->rotation = V4L2_MXC_ROTATE_NONE;
			    else if (cam->rotation == V4L2_MXC_ROTATE_180)
				    cam->rotation = V4L2_MXC_ROTATE_VERT_FLIP;
		    }
	    } else {
	    	int x;
	    	x = (c->value != 0) ? 1 : 0;
	    	cam->rotation = V4L2_MXC_ROTATE_NONE;
	    	cam->cam_sensor->set_flip(x);
	    }
		break;
	case V4L2_CID_VFLIP:
		if (cam->cam_sensor->set_flip == NULL) {
		    if (c->value == 1) {
			    if ((cam->rotation != V4L2_MXC_ROTATE_HORIZ_FLIP) &&
			        (cam->rotation != V4L2_MXC_ROTATE_180))
				    cam->rotation = V4L2_MXC_ROTATE_VERT_FLIP;
			    else
				    cam->rotation = V4L2_MXC_ROTATE_180;
		    } else {
			    if (cam->rotation == V4L2_MXC_ROTATE_VERT_FLIP)
				    cam->rotation = V4L2_MXC_ROTATE_NONE;
			    if (cam->rotation == V4L2_MXC_ROTATE_180)
				    cam->rotation = V4L2_MXC_ROTATE_HORIZ_FLIP;
		    }
		} else {
			int x;
	    	x = (c->value != 0) ? 1 : 0;
	    	cam->rotation = V4L2_MXC_ROTATE_NONE;
	    	cam->cam_sensor->set_flip(x<<1);
		}
		break;
	case V4L2_CID_MXC_ROT:
		switch (c->value) {
		case V4L2_MXC_ROTATE_NONE:
		case V4L2_MXC_ROTATE_VERT_FLIP:
		case V4L2_MXC_ROTATE_HORIZ_FLIP:
		case V4L2_MXC_ROTATE_180:
		case V4L2_MXC_ROTATE_90_RIGHT:
		case V4L2_MXC_ROTATE_90_RIGHT_VFLIP:
		case V4L2_MXC_ROTATE_90_RIGHT_HFLIP:
		case V4L2_MXC_ROTATE_90_LEFT:
			if (cam->rotation == V4L2_MXC_DEINTERLACE)
				break;
			if (cam->cam_sensor->set_flip == NULL) {
				cam->rotation = c->value;
			} else {
			    if (c->value == V4L2_MXC_ROTATE_180) {
				    cam->rotation = V4L2_MXC_ROTATE_NONE;
				    cam->cam_sensor->set_flip(3);
			    } else {
				    cam->cam_sensor->set_flip(0);
			        cam->rotation = c->value;
			    }
			}
			break;
		case V4L2_MXC_DEINTERLACE:
			cam->rotation = V4L2_MXC_DEINTERLACE;
			break;
		default:
			return -EINVAL;
		}
		break;
	/* void (*set_color) (int bright, int hue, int saturation, int con, int red, int blue); */
	case V4L2_CID_BRIGHTNESS:
		cam->bright = c->value;
		printk(KERN_ERR "--- V4L2_CID_BRIGHTNESS: set brightness to %x ---\n", c->value);
		csi_enable_mclk(CSI_MCLK_I2C, true, true);
		cam->cam_sensor->set_color(c->value, -1, -1, -1, -1, -1);
		csi_enable_mclk(CSI_MCLK_I2C, false, false);
		break;
	case V4L2_CID_HUE:
		cam->hue = c->value;
		csi_enable_mclk(CSI_MCLK_I2C, true, true);
		cam->cam_sensor->set_color(-1, c->value, -1, -1, -1, -1);
		csi_enable_mclk(CSI_MCLK_I2C, false, false);
		break;
	case V4L2_CID_SATURATION:
		cam->saturation = c->value;
		printk(KERN_ERR "--- V4L2_CID_SATURATION: set saturation to %x ---\n", c->value);
		csi_enable_mclk(CSI_MCLK_I2C, true, true);
		cam->cam_sensor->set_color(-1, -1, c->value, -1, -1, -1);
		csi_enable_mclk(CSI_MCLK_I2C, false, false);
		break;
	case V4L2_CID_CONTRAST:
		cam->contrast = c->value;
		printk(KERN_ERR "--- V4L2_CID_CONTRAST: set contrast to %x ---\n", c->value);
		csi_enable_mclk(CSI_MCLK_I2C, true, true);
		cam->cam_sensor->set_color(-1, -1, -1, c->value, -1, -1);
		csi_enable_mclk(CSI_MCLK_I2C, false, false);
		break;		
	case V4L2_CID_RED_BALANCE:
		cam->red = c->value;
		printk(KERN_ERR "--- V4L2_CID_RED_BALANCE: set red balance to %x ---\n", c->value);
		csi_enable_mclk(CSI_MCLK_I2C, true, true);
		cam->cam_sensor->set_color(-1, -1, -1, -1, c->value, -1);
		csi_enable_mclk(CSI_MCLK_I2C, false, false);
		break;
	case V4L2_CID_BLUE_BALANCE:
		cam->blue = c->value;
		printk(KERN_ERR "--- V4L2_CID_BLUE_BALANCE: set blue balance to %x ---\n", c->value);
		csi_enable_mclk(CSI_MCLK_I2C, true, true);
		cam->cam_sensor->set_color(-1, -1, -1, -1, -1, c->value);
		csi_enable_mclk(CSI_MCLK_I2C, false, false);
		break;

	case V4L2_CID_BLACK_LEVEL:
		cam->ae_mode = c->value & 0x03;
		csi_enable_mclk(CSI_MCLK_I2C, true, true);
		if (cam->cam_sensor->set_ae_mode)
			cam->cam_sensor->set_ae_mode(cam->ae_mode);
		csi_enable_mclk(CSI_MCLK_I2C, false, false);
		break;
	case V4L2_CID_MXC_FLASH:
		break;
	case V4L2_CID_AUTOGAIN:
		cam->ae_enable = c->value;
		csi_enable_mclk(CSI_MCLK_I2C, true, true);
		if (cam->cam_sensor->set_ae)
			cam->cam_sensor->set_ae(cam->ae_enable);
		csi_enable_mclk(CSI_MCLK_I2C, false, false);
		break;
	case V4L2_CID_AUTO_WHITE_BALANCE:
		cam->awb_enable = c->value;
		csi_enable_mclk(CSI_MCLK_I2C, true, true);
		if (cam->cam_sensor->set_awb)
			cam->cam_sensor->set_awb(cam->awb_enable);
		csi_enable_mclk(CSI_MCLK_I2C, false, false);
		break;
	case V4L2_CID_MXC_FLICKER:
		cam->flicker_ctrl = c->value;
		csi_enable_mclk(CSI_MCLK_I2C, true, true);
		if (cam->cam_sensor->flicker_control)
			cam->cam_sensor->flicker_control(cam->flicker_ctrl);
		csi_enable_mclk(CSI_MCLK_I2C, false, false);
		break;
	case V4L2_CID_MXC_PWRDN:
		{
			int v;
		    printk("--- CAMERA POWER DOWN: %d ---\n", c->value);
		    v = (c->value)?0:1;
		    mxc_cmos_enable(v);
	    }
		break;
	default:
		return -EINVAL;
	}
	return 0;
}

/*!
 * V4L2 - mxc_v4l2_s_param function
 *
 * @param cam         structure cam_data *
 *
 * @param parm        structure v4l2_streamparm *
 *
 * @return  status    0 success, EINVAL failed
 */
static int mxc_v4l2_s_param(cam_data * cam, struct v4l2_streamparm *parm)
{
	sensor_interface *param;
	csi_signal_cfg_t csi_param;

	if (parm->type != V4L2_BUF_TYPE_VIDEO_CAPTURE) {
		pr_debug("mxc_v4l2_s_param invalid type\n");
		return -EINVAL;
	}

	if (parm->parm.capture.timeperframe.denominator >
	    cam->standard.frameperiod.denominator) {
		pr_debug("mxc_v4l2_s_param frame rate %d larger "
			 "than standard supported %d\n",
			 parm->parm.capture.timeperframe.denominator,
			 cam->standard.frameperiod.denominator);
		return -EINVAL;
	}

	cam->streamparm.parm.capture.capability = V4L2_CAP_TIMEPERFRAME;

	csi_enable_mclk(CSI_MCLK_I2C, true, true);
	param = cam->cam_sensor->config(&parm->parm.capture.timeperframe.denominator,
	              parm->parm.capture.capturemode);
	csi_enable_mclk(CSI_MCLK_I2C, false, false);

	cam->streamparm.parm.capture.timeperframe = parm->parm.capture.timeperframe;

	if ((parm->parm.capture.capturemode != 0) &&
	    (parm->parm.capture.capturemode != V4L2_MODE_HIGHQUALITY)) {
		pr_debug("mxc_v4l2_s_param frame un-supported capture mode\n");
		return -EINVAL;
	}

	if (parm->parm.capture.capturemode == cam->streamparm.parm.capture.capturemode) {
		return 0;
	}

	/* resolution changed, so need to re-program the CSI */
	csi_param.sens_clksrc = 0;
	csi_param.clk_mode = param->clk_mode;
	csi_param.pixclk_pol = param->pixclk_pol;
	csi_param.data_width = param->data_width;
	csi_param.data_pol = param->data_pol;
	csi_param.ext_vsync = param->ext_vsync;
	csi_param.Vsync_pol = param->Vsync_pol;
	csi_param.Hsync_pol = param->Hsync_pol;
	csi_init_interface(param->width, param->height, param->pixel_fmt, csi_param);

	if (parm->parm.capture.capturemode != V4L2_MODE_HIGHQUALITY) {
		cam->streamparm.parm.capture.capturemode = 0;
	} else {
		cam->streamparm.parm.capture.capturemode = V4L2_MODE_HIGHQUALITY;
		cam->streamparm.parm.capture.extendedmode = parm->parm.capture.extendedmode;
		cam->streamparm.parm.capture.readbuffers = 1;
	}
	return 0;
}

/*!
 * Dequeue one V4L capture buffer
 *
 * @param cam         structure cam_data *
 * @param buf         structure v4l2_buffer *
 *
 * @return  status    0 success, EINVAL invalid frame number,
 *                    ETIME timeout, ERESTARTSYS interrupted by user
 */
static int mxc_v4l_dqueue(cam_data * cam, struct v4l2_buffer *buf)
{
	int retval = 0;
	struct mxc_v4l_frame *frame;

      cont:
	if (!wait_event_interruptible_timeout(cam->enc_queue, cam->enc_counter != 0, 10 * HZ)) {
		if ((dq_timeout_cnt & 0x1f) == 0)
			printk(KERN_ERR "mxc_v4l_dqueue timeout enc_counter %x\n", cam->enc_counter);
		dq_timeout_cnt++;
		if (cam->overflow == 1) {
			cam->enc_enable(cam);
			cam->overflow = 0;
			if (!list_empty(&cam->ready_q)) {
				frame = list_entry(cam->ready_q.next, struct mxc_v4l_frame, queue);
				list_del(cam->ready_q.next);
				list_add_tail(&frame->queue, &cam->working_q);
				/* new modified on 2012.04.23 for deinterlace */
				cam->enc_update_eba(frame->paddress+g_dev_minor*g_image_sz, &cam->ping_pong_csi);
			}
			goto cont;
		}
		return -ETIME;
	} else if (signal_pending(current)) {
		if (dq_intr_cnt == 0)
			printk(KERN_ERR "mxc_v4l_dqueue() interrupt received %d\n", dq_intr_cnt);
		dq_intr_cnt++;
		if (cam->overflow == 1) {
			cam->enc_enable(cam);
			cam->overflow = 0;
			if (!list_empty(&cam->ready_q)) {
				frame = list_entry(cam->ready_q.next, struct mxc_v4l_frame, queue);
				list_del(cam->ready_q.next);
				list_add_tail(&frame->queue, &cam->working_q);
				/* new modified on 2012.04.23 for deinterlace */
				cam->enc_update_eba(frame->paddress+g_dev_minor*g_image_sz, &cam->ping_pong_csi);
			}
			goto cont;
		}
		return -ERESTARTSYS;
	}

	if (cam->overflow == 1) {
		cam->enc_enable(cam);
		cam->overflow = 0;
		if (!list_empty(&cam->ready_q)) {
			frame = list_entry(cam->ready_q.next, struct mxc_v4l_frame, queue);
			list_del(cam->ready_q.next);
			list_add_tail(&frame->queue, &cam->working_q);
			/* new modified on 2012.04.23 for deinterlace */
			cam->enc_update_eba(frame->paddress+g_dev_minor*g_image_sz, &cam->ping_pong_csi);
		}
		printk(KERN_INFO "mxc_v4l_dqueue - overflow\n");
	}

	cam->enc_counter--;

	frame = list_entry(cam->done_q.next, struct mxc_v4l_frame, queue);
	list_del(cam->done_q.next);
	if (frame->buffer.flags & V4L2_BUF_FLAG_DONE) {
		frame->buffer.flags &= ~V4L2_BUF_FLAG_DONE;
	} else if (frame->buffer.flags & V4L2_BUF_FLAG_QUEUED) {
		printk(KERN_ERR "VIDIOC_DQBUF: Buffer not filled.\n");
		frame->buffer.flags &= ~V4L2_BUF_FLAG_QUEUED;
		retval = -EINVAL;
	} else if ((frame->buffer.flags & 0x7) == V4L2_BUF_FLAG_MAPPED) {
		printk(KERN_ERR "VIDIOC_DQBUF: Buffer not queued.\n");
		retval = -EINVAL;
	}

	buf->bytesused = cam->v2f.fmt.pix.sizeimage;
	buf->index = frame->index;
	buf->flags = frame->buffer.flags;
	return retval;
}

/*!
 * Get the current attached camera device
 *
 * @param inode      struct i2c_client *
 *
 * @param int       int * p_input_index
 *
 * @return           0 success, ENODEV for invalid device instance,
 *                   -1 for other errors.
 */
static int mxc_get_video_input(cam_data * cam)
{
	int retval = 0;
	csi_enable_mclk(CSI_MCLK_I2C, true, true);
	retval = cam->cam_sensor->get_status();
	csi_enable_mclk(CSI_MCLK_I2C, false, false);
	return retval;
}

/*!
 * V4L interface - open function
 *
 * @param inode        structure inode *
 * @param file         structure file *
 *
 * @return  status    0 success, ENODEV invalid device instance,
 *                    ENODEV timeout, ERESTARTSYS interrupted by user
 */
static int mxc_v4l_open(struct inode *inode, struct file *file)
{
	sensor_interface *param;
	csi_signal_cfg_t csi_param;
	struct video_device *dev = video_devdata(file);
	cam_data *cam = dev->priv;
	int err = 0;
	unsigned int minor = iminor(inode);

	dq_intr_cnt = 0;
	dq_timeout_cnt = 0;
	empty_wq_cnt = 0;
	if (!cam||minor<0||minor>1) {
		pr_info("Internal error, cam_data not found or minor error!\n");
		return -ENODEV;
	}

	/* new add on 2012.04.23 for capturing deinterlace */
	g_dev_minor = minor;
	/* new add on 2012.04.17 */
	cam->cam_sensor = g_sensor[minor];
	mxc_camera_select(minor);

	err = mxc_get_video_input(cam);
	if (0 != err)
		return -ENODEV;

	if (down_interruptible(&cam->busy_lock))
		return -EINTR;

	if (signal_pending(current))
		goto oops;

	if (cam->open_count++ == 0) {
		if (minor == 0)
			cam->rotation = V4L2_MXC_ROTATE_NONE;
		else
			cam->rotation = V4L2_MXC_DEINTERLACE;
			
		wait_event_interruptible(cam->power_queue, cam->low_power == false);

		err = prp_enc_select(cam);

		cam->enc_counter = 0;
		cam->skip_frame = 0;
		INIT_LIST_HEAD(&cam->ready_q);
		INIT_LIST_HEAD(&cam->working_q);
		INIT_LIST_HEAD(&cam->done_q);
		/* new add on 2012.04.23 for capturing deinterlace */
		INIT_LIST_HEAD(&undi_q);

		csi_enable_mclk(CSI_MCLK_I2C, true, true);
		param = cam->cam_sensor->reset();
		csi_param.sens_clksrc = 0;
		csi_param.clk_mode = param->clk_mode;
		csi_param.pixclk_pol = param->pixclk_pol;
		csi_param.data_width = param->data_width;
		csi_param.data_pol = param->data_pol;
		csi_param.ext_vsync = param->ext_vsync;
		csi_param.Vsync_pol = param->Vsync_pol;
		csi_param.Hsync_pol = param->Hsync_pol;
		csi_init_interface(param->width, param->height, param->pixel_fmt, csi_param);
		cam->cam_sensor->get_color(&cam->bright, &cam->hue, &cam->saturation,
					   &cam->contrast, &cam->red, &cam->blue);
		cam->cam_sensor->get_ae_mode(&cam->ae_mode);
		cam->cam_sensor->get_control_params(&cam->ae_enable, &cam->awb_enable, &cam->flicker_ctrl);
		csi_enable_mclk(CSI_MCLK_I2C, false, false);
		prp_init(cam);
	}

	file->private_data = dev;
oops:
	up(&cam->busy_lock);
	return err;
}

/*!
 * V4L interface - close function
 *
 * @param inode    struct inode *
 * @param file     struct file *
 *
 * @return         0 success
 */
static int mxc_v4l_close(struct inode *inode, struct file *file)
{
	struct video_device *dev = video_devdata(file);
	int err = 0;
	cam_data *cam = dev->priv;
	unsigned int minor = iminor(inode);

	/* for the case somebody hit the ctrl C */
	if (cam->overlay_pid == current->pid) {
		err = stop_preview(cam);
		cam->overlay_on = false;
	}
	if (cam->capture_pid == current->pid) {
		err |= mxc_streamoff(cam);
		cam->capture_on = false;
		wake_up_interruptible(&cam->enc_queue);
	}

	if (--cam->open_count == 0) {
		wait_event_interruptible(cam->power_queue, cam->low_power == false);
		pr_debug("mxc_v4l_close: release resource\n");

		err |= prp_enc_deselect(cam);

		mxc_free_frame_buf(cam);
		file->private_data = NULL;

		/* capture off */
		wake_up_interruptible(&cam->enc_queue);
		mxc_free_frames(cam);
		cam->enc_counter++;
		prp_exit(cam);
		cam->rotation = V4L2_MXC_ROTATE_NONE;
		(*g_pwr_on[minor])(0);
	}

	return err;
}

#ifdef CONFIG_VIDEO_MXC_CSI_DMA
#include <asm/arch/dma.h>

#define CSI_DMA_STATUS_IDLE	0	/* DMA is not started */
#define CSI_DMA_STATUS_WORKING	1	/* DMA is transfering the data */
#define CSI_DMA_STATUS_DONE	2	/* One frame completes successfully */
#define CSI_DMA_STATUS_ERROR	3	/* Error occurs during the DMA */

/*
 * Sometimes the start of the DMA is not synchronized with the CSI
 * SOF (Start of Frame) interrupt which will lead to incorrect
 * captured image. In this case the driver will re-try capturing
 * another frame. The following macro defines the maximum re-try
 * times.
 */
#define CSI_DMA_RETRY		8

/*
 * Size of the physical contiguous memory area used to hold image data
 * transfered by DMA. It can be less than the size of the image data.
 */
#define CSI_MEM_SIZE		(1024 * 600)

/* Number of bytes for one DMA transfer */
#define CSI_DMA_LENGTH		(1024 * 200)

static int g_dma_channel = 0;
static int g_dma_status = CSI_DMA_STATUS_DONE;
static volatile int g_dma_completed;	/* number of completed DMA transfers */
static volatile int g_dma_copied;	/* number of copied DMA transfers */
static struct tasklet_struct g_dma_tasklet;
static char *g_user_buf;	/* represents the buf passed by read() */
static int g_user_count;	/* represents the count passed by read() */

/*!
 * @brief setup the DMA to transfer data
 *	  There may be more than one DMA to transfer the whole image. Those
 *	  DMAs work like chain. This function is used to setup the DMA in
 *	  case there is enough space to hold the data.
 * @param	data	pointer to the cam structure
 */
static void mxc_csi_dma_chaining(void *data)
{
	cam_data *cam = (cam_data *) data;
	int count, chained = 0;
	int max_dma = CSI_MEM_SIZE / CSI_DMA_LENGTH;
	mxc_dma_requestbuf_t dma_request;

	while (chained * CSI_DMA_LENGTH < g_user_count) {
		/*
		 * Calculate how many bytes the DMA should transfer. It may
		 * be less than CSI_DMA_LENGTH if the DMA is the last one.
		 */
		if ((chained + 1) * CSI_DMA_LENGTH > g_user_count)
			count = g_user_count - chained * CSI_DMA_LENGTH;
		else
			count = CSI_DMA_LENGTH;
		pr_debug("%s() DMA chained count = %d\n", __FUNCTION__, count);

		/* Config DMA */
		memset(&dma_request, 0, sizeof(mxc_dma_requestbuf_t));
		dma_request.dst_addr = cam->still_buf
		    + (chained % max_dma) * CSI_DMA_LENGTH;
		dma_request.src_addr = (dma_addr_t) CSI_CSIRXFIFO_PHYADDR;
		dma_request.num_of_bytes = count;
		mxc_dma_config(g_dma_channel, &dma_request, 1,
			       MXC_DMA_MODE_READ);

		chained++;
	}
}

/*!
 * @brief Copy image data from physical contiguous memory to user space buffer
 *	  Once the data are copied, there will be more spare space in the
 *	  physical contiguous memory to receive data from DMA.
 * @param	data	pointer to the cam structure
 */
static void mxc_csi_dma_task(unsigned long data)
{
	cam_data *cam = (cam_data *) data;
	int count;
	int max_dma = CSI_MEM_SIZE / CSI_DMA_LENGTH;

	while (g_dma_copied < g_dma_completed) {
		/*
		 * Calculate how many bytes the DMA has transfered. It may
		 * be less than CSI_DMA_LENGTH if the DMA is the last one.
		 */
		if ((g_dma_copied + 1) * CSI_DMA_LENGTH > g_user_count)
			count = g_user_count - g_dma_copied * CSI_DMA_LENGTH;
		else
			count = CSI_DMA_LENGTH;
		if (copy_to_user(g_user_buf + g_dma_copied * CSI_DMA_LENGTH,
				 cam->still_buf_vaddr + (g_dma_copied % max_dma)
				 * CSI_DMA_LENGTH, count))
			pr_debug("Warning: some bytes not copied\n");

		g_dma_copied++;
	}

	/* If the whole image has been captured */
	if (g_dma_copied * CSI_DMA_LENGTH >= g_user_count) {
		cam->still_counter++;
		wake_up_interruptible(&cam->still_queue);
	}

	pr_debug("%s() DMA completed = %d copied = %d\n",
		 __FUNCTION__, g_dma_completed, g_dma_copied);
}

/*!
 * @brief DMA interrupt callback function
 * @param	data	pointer to the cam structure
 * @param	error	DMA error flag
 * @param	count	number of bytes transfered by the DMA
 */
static void mxc_csi_dma_callback(void *data, int error, unsigned int count)
{
	cam_data *cam = (cam_data *) data;
	int max_dma = CSI_MEM_SIZE / CSI_DMA_LENGTH;
	unsigned long lock_flags;

	spin_lock_irqsave(&cam->int_lock, lock_flags);

	g_dma_completed++;

	if (error != MXC_DMA_DONE) {
		g_dma_status = CSI_DMA_STATUS_ERROR;
		pr_debug("%s() DMA error\n", __FUNCTION__);
	}

	/* If the whole image has been captured */
	if ((g_dma_status != CSI_DMA_STATUS_ERROR)
	    && (g_dma_completed * CSI_DMA_LENGTH >= g_user_count))
		g_dma_status = CSI_DMA_STATUS_DONE;

	if ((g_dma_status == CSI_DMA_STATUS_WORKING) &&
	    (g_dma_completed >= g_dma_copied + max_dma)) {
		g_dma_status = CSI_DMA_STATUS_ERROR;
		pr_debug("%s() Previous buffer over written\n", __FUNCTION__);
	}

	/* Schedule the tasklet */
	tasklet_schedule(&g_dma_tasklet);

	spin_unlock_irqrestore(&cam->int_lock, lock_flags);

	pr_debug("%s() count = %d bytes\n", __FUNCTION__, count);
}

/*!
 * @brief CSI interrupt callback function
 * @param	data	pointer to the cam structure
 * @param	status	CSI interrupt status
 */
static void mxc_csi_irq_callback(void *data, unsigned long status)
{
	cam_data *cam = (cam_data *) data;
	unsigned long lock_flags;

	spin_lock_irqsave(&cam->int_lock, lock_flags);

	/* Wait for SOF (Start of Frame) interrupt to sync the image */
	if (status & BIT_SOF_INT) {
		if (g_dma_status == CSI_DMA_STATUS_IDLE) {
			/* Start DMA transfer to capture image */
			mxc_dma_enable(g_dma_channel);
			g_dma_status = CSI_DMA_STATUS_WORKING;
			pr_debug("%s() DMA started.\n", __FUNCTION__);
		} else if (g_dma_status == CSI_DMA_STATUS_WORKING) {
			/*
			 * Another SOF occurs during DMA transfer. In this
			 * case the image is not synchronized so need to
			 * report error and probably try again.
			 */
			g_dma_status = CSI_DMA_STATUS_ERROR;
			pr_debug("%s() Image is not synchronized with DMA - "
				 "SOF before DMA completes\n", __FUNCTION__);
		}
	}

	spin_unlock_irqrestore(&cam->int_lock, lock_flags);

	pr_debug("%s() g_dma_status = %d\n", __FUNCTION__, g_dma_status);
}

/*!
 * V4L interface - read function
 *
 * @param file       struct file *
 * @param read buf   char *
 * @param count      size_t
 * @param ppos       structure loff_t *
 *
 * @return           bytes read
 */
static ssize_t
mxc_v4l_read(struct file *file, char *buf, size_t count, loff_t * ppos)
{
	int err = 0;
	struct video_device *dev = video_devdata(file);
	cam_data *cam = dev->priv;
	int retry = CSI_DMA_RETRY;

	g_user_buf = buf;

	if (down_interruptible(&cam->busy_lock))
		return -EINTR;

	/* Video capture and still image capture are exclusive */
	if (cam->capture_on == true) {
		err = -EBUSY;
		goto exit0;
	}

	/* The CSI-DMA can not do CSC */
	if (cam->v2f.fmt.pix.pixelformat != V4L2_PIX_FMT_YUYV) {
		pr_info("mxc_v4l_read support YUYV pixel format only\n");
		err = -EINVAL;
		goto exit0;
	}

	/* The CSI-DMA can not do resize or crop */
	if ((cam->v2f.fmt.pix.width != cam->crop_bounds.width)
	    || (cam->v2f.fmt.pix.height != cam->crop_bounds.height)) {
		pr_info("mxc_v4l_read resize is not supported\n");
		pr_info("supported image size width = %d height = %d\n",
			cam->crop_bounds.width, cam->crop_bounds.height);
		err = -EINVAL;
		goto exit0;
	}
	if ((cam->crop_current.left != cam->crop_bounds.left)
	    || (cam->crop_current.width != cam->crop_bounds.width)
	    || (cam->crop_current.top != cam->crop_bounds.top)
	    || (cam->crop_current.height != cam->crop_bounds.height)) {
		pr_info("mxc_v4l_read cropping is not supported\n");
		err = -EINVAL;
		goto exit0;
	}

	cam->still_buf_vaddr = dma_alloc_coherent(0,
						  PAGE_ALIGN(CSI_MEM_SIZE),
						  &cam->still_buf,
						  GFP_DMA | GFP_KERNEL);

	if (!cam->still_buf_vaddr) {
		pr_info("mxc_v4l_read failed at allocate still_buf\n");
		err = -ENOBUFS;
		goto exit0;
	}

	/* Initialize DMA */
	g_dma_channel = mxc_dma_request(MXC_DMA_CSI_RX, "CSI RX DMA");
	if (g_dma_channel < 0) {
		pr_debug("mxc_v4l_read failed to request DMA channel\n");
		err = -EIO;
		goto exit1;
	}

	err = mxc_dma_callback_set(g_dma_channel, (mxc_dma_callback_t) mxc_csi_dma_callback,
				   (void *)cam);
	if (err != 0) {
		pr_debug("mxc_v4l_read failed to set DMA callback\n");
		err = -EIO;
		goto exit2;
	}

	g_user_buf = buf;
	if (cam->v2f.fmt.pix.sizeimage < count)
		g_user_count = cam->v2f.fmt.pix.sizeimage;
	else
		g_user_count = count & ~0x3;

	tasklet_init(&g_dma_tasklet, mxc_csi_dma_task, (unsigned long)cam);
	g_dma_status = CSI_DMA_STATUS_DONE;
	csi_set_callback(mxc_csi_irq_callback, cam);
	csi_enable_prpif(0);

	/* clear current SOF first */
	csi_clear_status(BIT_SOF_INT);
	csi_enable_mclk(CSI_MCLK_RAW, true, true);

	do {
		g_dma_completed = g_dma_copied = 0;
		mxc_csi_dma_chaining(cam);
		cam->still_counter = 0;
		g_dma_status = CSI_DMA_STATUS_IDLE;

		if (!wait_event_interruptible_timeout(cam->still_queue,
						      cam->still_counter != 0,
						      10 * HZ)) {
			pr_info("mxc_v4l_read timeout counter %x\n",
				cam->still_counter);
			err = -ETIME;
			goto exit3;
		}

		if (g_dma_status == CSI_DMA_STATUS_DONE)
			break;

		if (retry-- == 0)
			break;

		pr_debug("Now retry image capture\n");
	} while (1);

	if (g_dma_status != CSI_DMA_STATUS_DONE)
		err = -EIO;

      exit3:
	csi_enable_prpif(1);
	g_dma_status = CSI_DMA_STATUS_DONE;
	csi_set_callback(0, 0);
	csi_enable_mclk(CSI_MCLK_RAW, false, false);
	tasklet_kill(&g_dma_tasklet);

      exit2:
	mxc_dma_free(g_dma_channel);

      exit1:
	dma_free_coherent(0, PAGE_ALIGN(CSI_MEM_SIZE), cam->still_buf_vaddr, cam->still_buf);
	cam->still_buf = 0;

      exit0:
	up(&cam->busy_lock);
	if (err < 0)
		return err;
	else
		return g_user_count;
}
#else
/*!
 * V4L interface - read function
 *
 * @param file       struct file *
 * @param read buf   char *
 * @param count      size_t
 * @param ppos       structure loff_t *
 *
 * @return           bytes read
 */
static ssize_t
mxc_v4l_read(struct file *file, char *buf, size_t count, loff_t * ppos)
{
	int err = 0;
	u8 *v_address;
	struct video_device *dev = video_devdata(file);
	cam_data *cam = dev->priv;

	if (down_interruptible(&cam->busy_lock))
		return -EINTR;

	/* Video capture and still image capture are exclusive */
	if (cam->capture_on == true) {
		err = -EBUSY;
		goto exit0;
	}

	v_address = dma_alloc_coherent(0, PAGE_ALIGN(cam->v2f.fmt.pix.sizeimage),
				       &cam->still_buf, GFP_DMA | GFP_KERNEL);

	if (!v_address) {
		pr_info("mxc_v4l_read failed at allocate still_buf\n");
		err = -ENOBUFS;
		goto exit0;
	}

	if (prp_still_select(cam)) {
		err = -EIO;
		goto exit1;
	}

	cam->still_counter = 0;
	if (cam->csi_start(cam)) {
		err = -EIO;
		goto exit2;
	}

	if (!wait_event_interruptible_timeout(cam->still_queue, cam->still_counter != 0, 10 * HZ)) {
		pr_info("mxc_v4l_read timeout counter %x\n",
			cam->still_counter);
		err = -ETIME;
		goto exit2;
	}
	err = copy_to_user(buf, v_address, cam->v2f.fmt.pix.sizeimage);

      exit2:
	prp_still_deselect(cam);

      exit1:
	dma_free_coherent(0, cam->v2f.fmt.pix.sizeimage, v_address, cam->still_buf);
	cam->still_buf = 0;

      exit0:
	up(&cam->busy_lock);
	if (err < 0)
		return err;
	else
		return (cam->v2f.fmt.pix.sizeimage - err);
}
#endif				/* CONFIG_VIDEO_MXC_CSI_DMA */

/*!
 * V4L interface - ioctl function
 *
 * @param inode      struct inode *
 *
 * @param file       struct file *
 *
 * @param ioctlnr    unsigned int
 *
 * @param arg        void *
 *
 * @return           0 success, ENODEV for invalid device instance,
 *                   -1 for other errors.
 */
static int
mxc_v4l_do_ioctl(struct inode *inode, struct file *file,
		 unsigned int ioctlnr, void *arg)
{
	struct video_device *dev = video_devdata(file);
	cam_data *cam = dev->priv;
	int retval = 0;
	unsigned long lock_flags;

	if (!cam)
		return -EBADF;

	wait_event_interruptible(cam->power_queue, cam->low_power == false);
	/* make this _really_ smp-safe */
	if (down_interruptible(&cam->busy_lock))
		return -EBUSY;

	switch (ioctlnr) {
		/*!
		 * V4l2 VIDIOC_QUERYCAP ioctl
		 */
	case VIDIOC_QUERYCAP:{
			struct v4l2_capability *cap = arg;
			strcpy(cap->driver, "mxc_v4l2");
			cap->version = KERNEL_VERSION(0, 1, 11);
			cap->capabilities = V4L2_CAP_VIDEO_CAPTURE |
			    V4L2_CAP_VIDEO_OVERLAY | V4L2_CAP_STREAMING
			    | V4L2_CAP_READWRITE;
			cap->card[0] = '\0';
			cap->bus_info[0] = '\0';
			retval = 0;
			break;
		}

		/*!
		 * V4l2 VIDIOC_G_FMT ioctl
		 */
	case VIDIOC_G_FMT:{
			struct v4l2_format *gf = arg;
			retval = mxc_v4l2_g_fmt(cam, gf);
			break;
		}

		/*!
		 * V4l2 VIDIOC_S_FMT ioctl
		 */
	case VIDIOC_S_FMT:{
			struct v4l2_format *sf = arg;
			retval = mxc_v4l2_s_fmt(cam, sf);
			break;
		}

		/*!
		 * V4l2 VIDIOC_REQBUFS ioctl
		 */
	case VIDIOC_REQBUFS:{
			struct v4l2_requestbuffers *req = arg;
			if (req->count > FRAME_NUM) {
				pr_info("VIDIOC_REQBUFS: not enough buffer\n");
				req->count = FRAME_NUM;
			}

			if ((req->type != V4L2_BUF_TYPE_VIDEO_CAPTURE) ||
			    (req->memory != V4L2_MEMORY_MMAP)) {
				pr_debug("VIDIOC_REQBUFS: wrong buffer type\n");
				retval = -EINVAL;
				break;
			}

			mxc_streamoff(cam);
			mxc_free_frame_buf(cam);

			retval = mxc_allocate_frame_buf(cam, req->count);
			break;
		}

		/*!
		 * V4l2 VIDIOC_QUERYBUF ioctl
		 */
	case VIDIOC_QUERYBUF:{
			struct v4l2_buffer *buf = arg;
			int index = buf->index;

			if (buf->type != V4L2_BUF_TYPE_VIDEO_CAPTURE) {
				pr_debug
				    ("VIDIOC_QUERYBUFS: wrong buffer type\n");
				retval = -EINVAL;
				break;
			}

			memset(buf, 0, sizeof(buf));
			buf->index = index;

			down(&cam->param_lock);
			retval = mxc_v4l2_buffer_status(cam, buf);
			up(&cam->param_lock);
			break;
		}

		/*!
		 * V4l2 VIDIOC_QBUF ioctl
		 */
	case VIDIOC_QBUF:{
			struct v4l2_buffer *buf = arg;
			int index = buf->index;
			int overflow = 0;

			pr_debug("VIDIOC_QBUF: %d\n", buf->index);
			if (cam->overflow == 1) {
				cam->enc_enable(cam);
				cam->overflow = 0;
				overflow = 1;
				printk(KERN_INFO "VIDIOC_QBUF - overflow\n");
			}

			spin_lock_irqsave(&cam->int_lock, lock_flags);
			if ((cam->frame[index].buffer.flags & 0x7) == V4L2_BUF_FLAG_MAPPED) {
				cam->frame[index].buffer.flags |= V4L2_BUF_FLAG_QUEUED;
				if ((cam->skip_frame > 0) || (overflow == 1)) {
					list_add_tail(&cam->frame[index].queue, &cam->working_q);
					/* new modified on 2012.04.23 for deinterlace */
					cam->enc_update_eba(cam->frame[index].paddress+g_dev_minor*g_image_sz, &cam->ping_pong_csi);
					cam->skip_frame = 0;
				} else {
					list_add_tail(&cam->frame[index].queue, &cam->ready_q);
				}
			} else if (cam->frame[index].buffer.flags & V4L2_BUF_FLAG_QUEUED) {
				pr_debug("VIDIOC_QBUF: buffer already queued\n");
			} else if (cam->frame[index].buffer.flags & V4L2_BUF_FLAG_DONE) {
				pr_debug("VIDIOC_QBUF: overwrite done buffer.\n");
				cam->frame[index].buffer.flags &= ~V4L2_BUF_FLAG_DONE;
				cam->frame[index].buffer.flags |= V4L2_BUF_FLAG_QUEUED;
			}
			buf->flags = cam->frame[index].buffer.flags;
			spin_unlock_irqrestore(&cam->int_lock, lock_flags);
			break;
		}

		/*!
		 * V4l2 VIDIOC_DQBUF ioctl
		 */
	case VIDIOC_DQBUF:{
			struct v4l2_buffer *buf = arg;

			retval = mxc_v4l_dqueue(cam, buf);

			break;
		}

		/*!
		 * V4l2 VIDIOC_STREAMON ioctl
		 */
	case VIDIOC_STREAMON:{
			cam->capture_on = true;
			retval = mxc_streamon(cam);
			break;
		}

		/*!
		 * V4l2 VIDIOC_STREAMOFF ioctl
		 */
	case VIDIOC_STREAMOFF:{
			retval = mxc_streamoff(cam);
			cam->capture_on = false;
			break;
		}

		/*!
		 * V4l2 VIDIOC_G_CTRL ioctl
		 */
	case VIDIOC_G_CTRL:{
			retval = mxc_get_v42l_control(cam, arg);
			break;
		}

		/*!
		 * V4l2 VIDIOC_S_CTRL ioctl
		 */
	case VIDIOC_S_CTRL:{
			retval = mxc_set_v42l_control(cam, arg);
			break;
		}

		/*!
		 * V4l2 VIDIOC_CROPCAP ioctl
		 */
	case VIDIOC_CROPCAP:{
			struct v4l2_cropcap *cap = arg;

			if (cap->type != V4L2_BUF_TYPE_VIDEO_CAPTURE &&
			    cap->type != V4L2_BUF_TYPE_VIDEO_OVERLAY) {
				retval = -EINVAL;
				break;
			}
			cap->bounds = cam->crop_bounds;
			cap->defrect = cam->crop_defrect;
			break;
		}

		/*!
		 * V4l2 VIDIOC_G_CROP ioctl
		 */
	case VIDIOC_G_CROP:{
			struct v4l2_crop *crop = arg;

			if (crop->type != V4L2_BUF_TYPE_VIDEO_CAPTURE &&
			    crop->type != V4L2_BUF_TYPE_VIDEO_OVERLAY) {
				retval = -EINVAL;
				break;
			}
			crop->c = cam->crop_current;
			break;
		}

		/*!
		 * V4l2 VIDIOC_S_CROP ioctl
		 */
	case VIDIOC_S_CROP:
		{
			struct v4l2_crop *crop = arg;
			struct v4l2_rect *b = &cam->crop_bounds;
			int i;

			if (crop->type != V4L2_BUF_TYPE_VIDEO_CAPTURE &&
			    crop->type != V4L2_BUF_TYPE_VIDEO_OVERLAY) {
				retval = -EINVAL;
				break;
			}
			
			if (iminor(inode) != 0) {
				if (crop->c.height > b->height)
					crop->c.height = b->height;
				if (crop->c.width > b->width)
					crop->c.width = b->width;
				crop->c.left = (b->width - crop->c.width)>>1;
				crop->c.top = (b->height - crop->c.height)>>1;
				break;
			}

			crop->c.top = (crop->c.top < b->top) ? b->top : crop->c.top;
			if (crop->c.top > b->top + b->height)
				crop->c.top = b->top + b->height - 1;
			if (crop->c.height > b->top + b->height - crop->c.top)
				crop->c.height = b->top + b->height - crop->c.top;

			crop->c.left = (crop->c.left < b->left) ? b->left : crop->c.left;
			if (crop->c.left > b->left + b->width)
				crop->c.left = b->left + b->width - 1;
			if (crop->c.width > b->left - crop->c.left + b->width)
				crop->c.width = b->left - crop->c.left + b->width;

			crop->c.width &= ~0x1;

			/*
			 * MX27 PrP limitation:
			 * The right spare space (CSI_FRAME_X_SIZE
			 *  - SOURCE_LINE_STRIDE - PICTURE_X_SIZE)) must be
			 * multiple of 32.
			 * So we tune the crop->c.left value to the closest
			 * desired cropping value and meet the PrP requirement.
			 */
			i = ((b->left + b->width) - (crop->c.left + crop->c.width)) % 32;
			if (i <= 16) {
				if (crop->c.left + crop->c.width + i <= b->left + b->width)
					crop->c.left += i;
				else if (crop->c.left - (32 - i) >= b->left)
					crop->c.left -= 32 - i;
				else {
					retval = -EINVAL;
					break;
				}
			} else {
				if (crop->c.left - (32 - i) >= b->left)
					crop->c.left -= 32 - i;
				else if (crop->c.left + crop->c.width + i <= b->left + b->width)
					crop->c.left += i;
				else {
					retval = -EINVAL;
					break;
				}
			}
			cam->crop_current = crop->c;
			break;
		}
		/*!
		 * V4l2 VIDIOC_OVERLAY ioctl
		 */
	case VIDIOC_OVERLAY:{
			int *on = arg;
			if (*on) {
				cam->overlay_on = true;
				retval = start_preview(cam);
			}
			if (!*on) {
				retval = stop_preview(cam);
				cam->overlay_on = false;
			}
			break;
		}

		/*!
		 * V4l2 VIDIOC_G_FBUF ioctl
		 */
	case VIDIOC_G_FBUF:{
			struct v4l2_framebuffer *fb = arg;
			struct fb_var_screeninfo *var;

			if (cam->output >= num_registered_fb) {
				retval = -EINVAL;
				break;
			}

			var = &registered_fb[cam->output]->var;
			cam->v4l2_fb.fmt.width = var->xres;
			cam->v4l2_fb.fmt.height = var->yres;
			cam->v4l2_fb.fmt.bytesperline =
			    var->xres_virtual * var->bits_per_pixel;
			cam->v4l2_fb.fmt.colorspace = V4L2_COLORSPACE_SRGB;
			*fb = cam->v4l2_fb;
			break;
		}

		/*!
		 * V4l2 VIDIOC_S_FBUF ioctl
		 */
	case VIDIOC_S_FBUF:{
			struct v4l2_framebuffer *fb = arg;
			cam->v4l2_fb.flags = fb->flags;
			cam->v4l2_fb.fmt.pixelformat = fb->fmt.pixelformat;
			break;
		}

	case VIDIOC_G_PARM:{
			struct v4l2_streamparm *parm = arg;
			if (parm->type != V4L2_BUF_TYPE_VIDEO_CAPTURE) {
				pr_debug("VIDIOC_G_PARM invalid type\n");
				retval = -EINVAL;
				break;
			}
			parm->parm.capture = cam->streamparm.parm.capture;
			break;
		}
	case VIDIOC_S_PARM:{
			struct v4l2_streamparm *parm = arg;
			retval = mxc_v4l2_s_param(cam, parm);
			break;
		}

		/* linux v4l2 bug, kernel c0485619 user c0405619 */
	case VIDIOC_ENUMSTD:{
			struct v4l2_standard *e = arg;
			*e = cam->standard;
			pr_debug("VIDIOC_ENUMSTD call\n");
			retval = 0;
			break;
		}

	case VIDIOC_G_STD:{
			v4l2_std_id *e = arg;
			*e = cam->standard.id;
			break;
		}

	case VIDIOC_S_STD:{
		       if (cam->cam_sensor->set_std == NULL) {
				retval = -EINVAL;
			}
			else {
				cam->cam_sensor->set_std(arg);
			}
			break;
		}

	case VIDIOC_ENUMOUTPUT:
		{
			struct v4l2_output *output = arg;

			if (output->index >= num_registered_fb) {
				retval = -EINVAL;
				break;
			}

			strncpy(output->name,
				registered_fb[output->index]->fix.id, 31);
			output->type = V4L2_OUTPUT_TYPE_ANALOG;
			output->audioset = 0;
			output->modulator = 0;
			output->std = V4L2_STD_UNKNOWN;

			break;
		}
	case VIDIOC_G_OUTPUT:
		{
			int *p_output_num = arg;

			*p_output_num = cam->output;
			break;
		}
	case VIDIOC_S_OUTPUT:
		{
			int *p_output_num = arg;

			if (*p_output_num >= num_registered_fb) {
				retval = -EINVAL;
				break;
			}

			cam->output = *p_output_num;
			break;
		}

	case VIDIOC_G_INPUT:
		{
			int *p_input_index = arg;

			retval = mxc_get_video_input(cam);
			if (0 == retval)
				*p_input_index = 1;
			else
				*p_input_index = -ENODEV;

			break;
		}

	case VIDIOC_S_INPUT:
		{
			int *p_input_index = arg;

			if (cam->cam_sensor->set_channel)
				cam->cam_sensor->set_channel(*p_input_index);
			else
				retval = -EINVAL;
			break;
		}
	case VIDIOC_QUERYCTRL:
		{
			if (cam->cam_sensor->query_ctrl == NULL) {
				retval = -EINVAL;
			}
			else {
				cam->cam_sensor->query_ctrl(arg);
			}
			break;
		}
	case VIDIOC_ENUM_FMT:
	case VIDIOC_TRY_FMT:
	case VIDIOC_ENUMINPUT:
	case VIDIOC_G_TUNER:
	case VIDIOC_S_TUNER:
	case VIDIOC_G_FREQUENCY:
	case VIDIOC_S_FREQUENCY:
	default:
		retval = -EINVAL;
		break;
	}

	up(&cam->busy_lock);
	return retval;
}

/*
 * V4L interface - ioctl function
 *
 * @return  None
 */
static int
mxc_v4l_ioctl(struct inode *inode, struct file *file,
	      unsigned int cmd, unsigned long arg)
{
	return video_usercopy(inode, file, cmd, arg, mxc_v4l_do_ioctl);
}

/*!
 * V4L interface - mmap function
 *
 * @param file        structure file *
 *
 * @param vma         structure vm_area_struct *
 *
 * @return status     0 Success, EINTR busy lock error, ENOBUFS remap_page error
 */
static int mxc_mmap(struct file *file, struct vm_area_struct *vma)
{
	struct video_device *dev = video_devdata(file);
	unsigned long size;
	int res = 0;
	cam_data *cam = dev->priv;

	pr_debug("pgoff=0x%lx, start=0x%lx, end=0x%lx\n",
		 vma->vm_pgoff, vma->vm_start, vma->vm_end);

	/* make this _really_ smp-safe */
	if (down_interruptible(&cam->busy_lock))
		return -EINTR;

	size = vma->vm_end - vma->vm_start;
	vma->vm_page_prot = pgprot_noncached(vma->vm_page_prot);
	vma->vm_page_prot = __pgprot(pgprot_val(vma->vm_page_prot) | L_PTE_CACHEABLE);

	if (remap_pfn_range(vma, vma->vm_start, vma->vm_pgoff, size, vma->vm_page_prot)) {
		pr_debug("mxc_mmap: remap_pfn_range failed\n");
		res = -ENOBUFS;
		goto mxc_mmap_exit;
	}

	vma->vm_flags &= ~VM_IO;	/* using shared anonymous pages */

      mxc_mmap_exit:
	up(&cam->busy_lock);
	return res;
}

/*!
 * V4L interface - poll function
 *
 * @param file       structure file *
 *
 * @param wait       structure poll_table *
 *
 * @return  status   POLLIN | POLLRDNORM
 */
static unsigned int mxc_poll(struct file *file, poll_table * wait)
{
	struct video_device *dev = video_devdata(file);
	cam_data *cam = dev->priv;
	wait_queue_head_t *queue = NULL;
	int res = POLLIN | POLLRDNORM;

	if (down_interruptible(&cam->busy_lock))
		return -EINTR;

	queue = &cam->enc_queue;
	poll_wait(file, queue, wait);

	up(&cam->busy_lock);
	return res;
}

static struct
file_operations mxc_v4l_fops = {
	.owner = THIS_MODULE,
	.open = mxc_v4l_open,
	.release = mxc_v4l_close,
	.read = mxc_v4l_read,
	.ioctl = mxc_v4l_ioctl,
	.mmap = mxc_mmap,
	.poll = mxc_poll,
};

static struct video_device mxc_v4l_template[2] = {
	{
	.owner = THIS_MODULE,
	.name = "Mxc CMOS Camera",
	.type = 0,
	.type2 = VID_TYPE_CAPTURE,
	.hardware = 0,
	.fops = &mxc_v4l_fops,
	.release = video_device_release,
    },
    {
	.owner = THIS_MODULE,
	.name = "Mxc CCD Camera",
	.type = 0,
	.type2 = VID_TYPE_CAPTURE,
	.hardware = 0,
	.fops = &mxc_v4l_fops,
	.release = video_device_release,
    },
};

static void camera_platform_release(struct device *device)
{
}

/*! Device Definition for Mt9v111 devices */
static struct platform_device mxc_v4l2_devices[2] = {
	{
	.name = "mxc_cmos_v4l2",
	.dev = {
		.release = camera_platform_release,
		},
	.id = 0,
    },
    {
	.name = "mxc_ccd_v4l2",
	.dev = {
		.release = camera_platform_release,
		},
	.id = 1,
    },
};

/*!
* Camera V4l2 callback function.
*
* @return status
*/
static void camera_callback(u32 mask, void *dev)
{
	struct mxc_v4l_frame *done_frame;
	struct mxc_v4l_frame *ready_frame;

	cam_data *cam = (cam_data *) dev;
	if (cam == NULL)
		return;

	if (mask == 1) {
		cam->overflow = 1;
	}

	if (list_empty(&cam->working_q)) {
		if (empty_wq_cnt == 0) {
			printk(KERN_ERR "camera_callback: working queue empty %d\n", empty_wq_cnt);
		}
		empty_wq_cnt++;
		if (list_empty(&cam->ready_q)) {
			cam->skip_frame++;
		} else {
			ready_frame = list_entry(cam->ready_q.next, struct mxc_v4l_frame, queue);
			list_del(cam->ready_q.next);
			list_add_tail(&ready_frame->queue, &cam->working_q);
			/* new modified on 2012.04.23 for deinterlace */
			cam->enc_update_eba(ready_frame->paddress+g_dev_minor*g_image_sz, &cam->ping_pong_csi);
		}
		return;
	}

	done_frame = list_entry(cam->working_q.next, struct mxc_v4l_frame, queue);
	if (done_frame->buffer.flags & V4L2_BUF_FLAG_QUEUED) {
		done_frame->buffer.flags |= V4L2_BUF_FLAG_DONE;
		done_frame->buffer.flags &= ~V4L2_BUF_FLAG_QUEUED;

		if (list_empty(&cam->ready_q)) {
			cam->skip_frame++;
		} else {
			ready_frame = list_entry(cam->ready_q.next, struct mxc_v4l_frame, queue);
			list_del(cam->ready_q.next);
			list_add_tail(&ready_frame->queue, &cam->working_q);
			/* new modified on 2012.04.23 for deinterlace */
			cam->enc_update_eba(ready_frame->paddress+g_dev_minor*g_image_sz, &cam->ping_pong_csi);
		}

        if (g_dev_minor == 0) {
		      /* Added to the done queue */
			list_del(cam->working_q.next);
			list_add_tail(&done_frame->queue, &cam->done_q);

			/* Wake up the queue */
			cam->enc_counter++;
			wake_up_interruptible(&cam->enc_queue);
        } else { /* new modified on 2012.04.23 for deinterlace */
            list_del(cam->working_q.next);
			list_add_tail(&done_frame->queue, &undi_q);
			tasklet_schedule(&deinterlace_tasklet);
        }
	} else {
		printk(KERN_ERR "camera_callback :buffer not queued\n");
	}
}

/*!
 * initialize cam_data structure
 *
 * @param cam      structure cam_data *
 *
 * @return status  0 Success
 */
static void init_camera_struct(cam_data *cam)
{
	int i;

	/* Default everything to 0 */
	memset(cam, 0, sizeof(cam_data));

	init_MUTEX(&cam->param_lock);
	init_MUTEX(&cam->busy_lock);

	for (i = 0; i < FRAME_NUM; i++) {
		cam->frame[i].width = 0;
		cam->frame[i].height = 0;
		cam->frame[i].paddress = 0;
	}

	init_waitqueue_head(&cam->enc_queue);
	init_waitqueue_head(&cam->still_queue);

	/* setup cropping */
	cam->crop_bounds.left = 0;
	cam->crop_bounds.width = 800;
	cam->crop_bounds.top = 0;
	cam->crop_bounds.height = 600;
	cam->crop_current = cam->crop_defrect = cam->crop_bounds;
	cam->streamparm.parm.capture.capturemode = 0;

	cam->standard.index = 0;
	cam->standard.id = V4L2_STD_UNKNOWN;
	cam->standard.frameperiod.denominator = 30;
	cam->standard.frameperiod.numerator = 1;
	cam->standard.framelines = 600;
	cam->streamparm.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	cam->streamparm.parm.capture.timeperframe = cam->standard.frameperiod;
	cam->streamparm.parm.capture.capability = V4L2_CAP_TIMEPERFRAME;
	cam->overlay_on = false;
	cam->capture_on = false;
	cam->skip_frame = 0;
	cam->v4l2_fb.capability = V4L2_FBUF_CAP_EXTERNOVERLAY;
	cam->v4l2_fb.flags = V4L2_FBUF_FLAG_PRIMARY;

	cam->v2f.fmt.pix.sizeimage = 352 * 288 * 3 / 2;
	cam->v2f.fmt.pix.bytesperline = 288 * 3 / 2;
	cam->v2f.fmt.pix.width = 288;
	cam->v2f.fmt.pix.height = 352;
	cam->v2f.fmt.pix.pixelformat = V4L2_PIX_FMT_YUV420;
	cam->win.w.width = 160;
	cam->win.w.height = 160;
	cam->win.w.left = 0;
	cam->win.w.top = 0;

	cam->enc_callback = camera_callback;

	init_waitqueue_head(&cam->power_queue);
	cam->int_lock = SPIN_LOCK_UNLOCKED;
	spin_lock_init(&cam->int_lock);
}

/*!
 * camera_power function
 *    Turn Sensor power On/Off
 *
 * @param       cameraOn      true to turn camera on, otherwise shut down
 *
 * @return status
 */
static u8 camera_power(bool cameraOn, int i)
{
	if (cameraOn == true) {
		(*g_pwr_on[i])(1);
//		gpio_sensor_active();
		csi_enable_mclk(csi_mclk_flag_backup, true, true);
	} else {
		csi_mclk_flag_backup = csi_read_mclk_flag();
		csi_enable_mclk(csi_mclk_flag_backup, false, false);
		(*g_pwr_on[i])(0);
//		gpio_sensor_inactive();
	}
	return 0;
}

static int _mxc_v4l2_suspend(struct platform_device *pdev, pm_message_t state)
{
	cam_data *cam = platform_get_drvdata(pdev);

	if (cam == NULL) {
		return -1;
	}

	cam->low_power = true;

	if (cam->overlay_on == true)
		stop_preview(cam);
	if ((cam->capture_on == true) && cam->enc_disable) {
		cam->enc_disable(cam);
	}
	return 0;
}

static int _mxc_v4l2_resume(struct platform_device *pdev)
{
	cam_data *cam = platform_get_drvdata(pdev);

	if (cam == NULL) {
		return -1;
	}

	cam->low_power = false;
	wake_up_interruptible(&cam->power_queue);

	if (cam->overlay_on == true)
		start_preview(cam);
	if (cam->capture_on == true)
		mxc_streamon(cam);

	return 0;
}

/*!
 * This function is called to put the sensor in a low power state. Refer to the
 * document driver-model/driver.txt in the kernel source tree for more
 * information.
 *
 * @param   pdev  the device structure used to give information on which I2C
 *                to suspend
 * @param   state the power state the device is entering
 *
 * @return  The function returns 0 on success and -1 on failure.
 */
static int mxc_v4l2_suspend(struct platform_device *pdev, pm_message_t state)
{
	if (_mxc_v4l2_suspend(pdev, state) < 0) return -1;
	camera_power(false, 0);

	return 0;
}

/*!
 * This function is called to bring the sensor back from a low power state.Refer
 * to the document driver-model/driver.txt in the kernel source tree for more
 * information.
 *
 * @param   pdev  the device structure
 *
 * @return  The function returns 0 on success and -1 on failure
 */
static int mxc_v4l2_resume(struct platform_device *pdev)
{
	if (_mxc_v4l2_resume(pdev) < 0) return -1;
	camera_power(true, 0);

	return 0;
}

static int mxc_ccd_v4l2_suspend(struct platform_device *pdev, pm_message_t state)
{
	if (_mxc_v4l2_suspend(pdev, state) < 0) return -1;
	camera_power(false, 1);
	return 0;
}

/*!
 * This function is called to bring the sensor back from a low power state.Refer
 * to the document driver-model/driver.txt in the kernel source tree for more
 * information.
 *
 * @param   pdev  the device structure
 *
 * @return  The function returns 0 on success and -1 on failure
 */
static int mxc_ccd_v4l2_resume(struct platform_device *pdev)
{
	if (_mxc_v4l2_resume(pdev) < 0) return -1;
	camera_power(true, 1);

	return 0;
}

/*!
 * This structure contains pointers to the power management callback functions.
 */
static struct platform_driver mxc_v4l2_driver[2] = {
    {
	.driver = {
		   .name = "mxc_cmos_v4l2",
		   .owner = THIS_MODULE,
		   .bus = &platform_bus_type,
		   },
	.probe = NULL,
	.remove = NULL,
	.suspend = mxc_v4l2_suspend,
	.resume = mxc_v4l2_resume,
	.shutdown = NULL,
    },
    {
	.driver = {
		   .name = "mxc_ccd_v4l2",
		   .owner = THIS_MODULE,
		   .bus = &platform_bus_type,
		   },
	.probe = NULL,
	.remove = NULL,
	.suspend = mxc_ccd_v4l2_suspend,
	.resume = mxc_ccd_v4l2_resume,
	.shutdown = NULL,
    }
};

static int _camera_init(int i)
{
	u8 err = 0;

	g_vdev[i] = video_device_alloc();
	if (g_vdev[i] == NULL) {
		return -1;
	}

	*g_vdev[i] = mxc_v4l_template[i];
	video_set_drvdata(g_vdev[i], g_cam);
	dev_set_drvdata(&mxc_v4l2_devices[i].dev, (void *)g_cam);
	g_vdev[i]->minor = i;

	/* Register the I2C device */
	err = platform_device_register(&mxc_v4l2_devices[i]);
	if (err != 0) {
		pr_debug("camera_init: platform_device_register failed.\n");
		video_device_release(g_vdev[i]);
		return err;
	}

	/* Register the device driver structure. */
	err = platform_driver_register(&mxc_v4l2_driver[i]);
	if (err != 0) {
		platform_device_unregister(&mxc_v4l2_devices[i]);
		pr_debug("camera_init: driver_register failed.\n");
		video_device_release(g_vdev[i]);
		return err;
	}

	/* register v4l device */
	if (video_register_device(g_vdev[i], VFL_TYPE_GRABBER, video_nr) == -1) {
		platform_driver_unregister(&mxc_v4l2_driver[i]);
		platform_device_unregister(&mxc_v4l2_devices[i]);
		video_device_release(g_vdev[i]);
		pr_debug("video_register_device failed\n");
		return -1;
	}
	return 0;
}

static void _camera_exit(int i)
{
	pr_debug("unregistering video%d\n", i);

	video_unregister_device(g_vdev[i]);

	platform_driver_unregister(&mxc_v4l2_driver[i]);
	platform_device_unregister(&mxc_v4l2_devices[i]);
}

/*!
 * Entry point for the V4L2
 *
 * @return  Error code indicating success or failure
 */
static __init int camera_init(void)
{
	cam_data *cam;

	if ((g_cam = cam = kmalloc(sizeof(cam_data), GFP_KERNEL)) == NULL) {
		pr_debug("failed to mxc_v4l_register_camera\n");
		return -1;
	}

    if (gpio_sensor_activated == 0) {
       gpio_sensor_active();
       gpio_sensor_activated = 1;
    }
	init_camera_struct(cam);

    if (_camera_init(0) != 0) {
    	kfree(g_cam);
		g_cam = NULL;
    	return -1;
    }

	if (_camera_init(1) != 0) {
		_camera_exit(0);
		kfree(g_cam);
		g_cam = NULL;
		return -1;
	}

    return 0;
}

/*!
 * Exit and cleanup for the V4L2
 *
 */
static void __exit camera_exit(void)
{
	_camera_exit(0);
	_camera_exit(1);

	if (g_cam->open_count) {
		pr_debug("camera open -- setting ops to NULL\n");
	} else {
		pr_debug("freeing camera\n");
		mxc_free_frame_buf(g_cam);
		kfree(g_cam);
		g_cam = NULL;
	}
	if (gpio_sensor_activated == 1) {
	    gpio_sensor_inactive();
	    gpio_sensor_activated = 0;
	}
}

module_init(camera_init);
module_exit(camera_exit);

EXPORT_SYMBOL(gpio_sensor_activated);

module_param(video_nr, int, -1);

MODULE_AUTHOR("Freescale Semiconductor, Inc.");
MODULE_DESCRIPTION("V4L2 capture driver for Mxc based cameras");
MODULE_LICENSE("GPL");
MODULE_SUPPORTED_DEVICE("video");
