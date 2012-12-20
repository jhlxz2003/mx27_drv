#
# Makefile for the linux kernel.
#

# Object file lists.

obj-y		:= mm.o time.o dma.o gpio_mux.o clock.o devices.o serial.o system.o usb.o cpu.o

obj-$(CONFIG_MACH_MX27ADS)	+= mx27ads.o mx27ads_gpio.o 
obj-$(CONFIG_MACH_MX27_MDK27V0)	+= mx27mdk27v0.o mx27mdk27v0_gpio.o

ifdef CONFIG_MXC_POWER_CONTROL
obj-$(CONFIG_MACH_MX27_MDK27V0) += mx27mdk27v0_power.o
endif

# power management
obj-$(CONFIG_PM) 		+= pm.o mxc_pm.o
obj-$(CONFIG_DPM)		+= dpm.o
