1、报警输入驱动
   mxc_ain.c/mxcain.h放在drivers/char目录中，并相应修改Makefile。


2、gpio驱动程序要点[源文件：mxc_gpio.c]
a、/etc/udev/rules.d/10-mx31.rules中增加一行：KERNEL=="gpio",	NAME="misc/gpio"，这样才能在misc_register()注册时自动创建设备结点。
b、platform_device的注册可以在arch/arm/march-mx27/device.c中统一注册，也可以在驱动中注册，只需要在platform_driver之前注册即可。
c、在linux/miscdevice.h中增加次设备号的宏定义。

3.rtc驱动
  mxc_rtc.c/mxc_rtc.h

4.watchdog驱动
  char/watchdog/mxc_wdt.c
  char/watchdog/mxc_wdt.h


