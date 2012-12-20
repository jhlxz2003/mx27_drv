ifeq ($(CONFIG_VIDEO_MXC_IPU_CAMERA),y)
	obj-$(CONFIG_VIDEO_MXC_CAMERA) += mxc_v4l2_capture.o
	obj-$(CONFIG_MXC_IPU_PRP_VF_ADC) += ipu_prp_vf_adc.o
ifneq ($(CONFIG_FB_MXC_OVERLAY),y)
	obj-$(CONFIG_MXC_IPU_PRP_VF_SDC) += ipu_prp_vf_sdc.o
endif
	obj-$(CONFIG_MXC_IPU_PRP_VF_SDC) += ipu_prp_vf_sdc_bg.o
	obj-$(CONFIG_MXC_IPU_PRP_ENC) += ipu_prp_enc.o ipu_still.o
endif

mx27_capture-objs := mx27_prphw.o mx27_prpsw.o mx27_v4l2_capture.o
obj-$(CONFIG_VIDEO_MXC_EMMA_CAMERA) += mx27_csi.o mx27_capture.o 

mc521da_camera-objs := mc521da.o sensor_clock.o
obj-$(CONFIG_MXC_CAMERA_MC521DA) += mc521da_camera.o

mt9v111_camera-objs := mt9v111.o sensor_clock.o
obj-$(CONFIG_MXC_CAMERA_MICRON111) += mt9v111_camera.o

ov2640_camera-objs := ov2640.o sensor_clock.o
obj-$(CONFIG_MXC_CAMERA_OV2640) += ov2640_camera.o

hi253_camera-objs := hi253.o tvp5150.o sensor_clock.o
obj-$(CONFIG_MXC_CAMERA_HI253) += hi253_camera.o

tvp5150_camera-objs := tvp5150.o sensor_clock.o
obj-$(CONFIG_MXC_CAMERA_TVP5150) += tvp5150_camera.o

adv7180_camera-objs := adv7180.o sensor_clock.o
obj-$(CONFIG_MXC_CAMERA_ADV7180) += adv7180_camera.o

tw2835_camera-objs := tw2835.o sensor_clock.o
obj-$(CONFIG_MXC_CAMERA_TW2835) += tw2835_camera.o
