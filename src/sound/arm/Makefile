#
# Makefile for ALSA
#

obj-$(CONFIG_SND_SA11XX_UDA1341) += snd-sa11xx-uda1341.o
snd-sa11xx-uda1341-objs		:= sa11xx-uda1341.o

obj-$(CONFIG_SND_ARMAACI)	+= snd-aaci.o
snd-aaci-objs			:= aaci.o devdma.o

obj-$(CONFIG_SND_PXA2XX_PCM)	+= snd-pxa2xx-pcm.o
snd-pxa2xx-pcm-objs		:= pxa2xx-pcm.o

obj-$(CONFIG_SND_PXA2XX_AC97)	+= snd-pxa2xx-ac97.o
snd-pxa2xx-ac97-objs		:= pxa2xx-ac97.o

CFLAGS_mxc-cx20707.o		= -I$(TOPDIR)/drivers/mxc -I$(TOPDIR)/arch/arm/mach-mx27
obj-$(CONFIG_SND_MXC_CX20707)	+= snd-mxc-cx20707.o
snd-mxc-cx20707-objs		:= mxc-cx20707.o

#
# Define the header file locations for PMIC drivers.
#
CFLAGS_mxc-alsa-pmic.o  = -I$(TOPDIR)/drivers/mxc
obj-$(CONFIG_SND_MXC_PMIC)	+= mxc-alsa-pmic.o mxc-alsa-mixer.o
