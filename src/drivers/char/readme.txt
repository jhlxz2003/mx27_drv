1��������������
   mxc_ain.c/mxcain.h����drivers/charĿ¼�У�����Ӧ�޸�Makefile��


2��gpio��������Ҫ��[Դ�ļ���mxc_gpio.c]
a��/etc/udev/rules.d/10-mx31.rules������һ�У�KERNEL=="gpio",	NAME="misc/gpio"������������misc_register()ע��ʱ�Զ������豸��㡣
b��platform_device��ע�������arch/arm/march-mx27/device.c��ͳһע�ᣬҲ������������ע�ᣬֻ��Ҫ��platform_driver֮ǰע�ἴ�ɡ�
c����linux/miscdevice.h�����Ӵ��豸�ŵĺ궨�塣

3.rtc����
  mxc_rtc.c/mxc_rtc.h

4.watchdog����
  char/watchdog/mxc_wdt.c
  char/watchdog/mxc_wdt.h


