#include <linux/types.h>
#include <linux/errno.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/init.h>
#include <linux/input.h>
#include <linux/interrupt.h>
#include <linux/i2c.h>
#include <linux/delay.h>
#include <linux/uaccess.h>
#include <linux/irq.h>
#include <linux/workqueue.h>
#include <asm/arch/gpio.h>
#include <asm/atomic.h>
#include "gpio_mux.h"

static unsigned short normal_i2c[] = {0x48, I2C_CLIENT_END}; 

I2C_CLIENT_INSMOD_1(tsc2003);

#define DRIVER_NAME "mxc_tsc2003" 

#define TS_IRQ  IOMUX_TO_IRQ(MX27_PIN_SSI3_TXDAT)

#define ADC_MAX              ((1 << 12) - 1)

enum tsc2003_pd { 
	PD_POWERDOWN = 0, /* penirq */ 
	PD_IREFOFF_ADCON = 1, /* no penirq */ 
	PD_IREFON_ADCOFF = 2, /* penirq */ 
	PD_IREFON_ADCON = 3, /* no penirq */ 
	PD_PENIRQ_ARM = PD_IREFON_ADCOFF, 
	PD_PENIRQ_DISARM = PD_IREFON_ADCON, 
}; 

enum tsc2003_m { 
	M_12BIT = 0, 
	M_8BIT = 1 
}; 

enum tsc2003_cmd { 
	MEAS_TEMP0 = 0,
	MEAS_VBAT1 = 1,
	MEAS_IN1 = 2,
	MEAS_TEMP1 = 4,
	MEAS_VBAT2 = 5,
	MEAS_IN2 = 6,
	ACTIVATE_NX_DRIVERS = 8,
	ACTIVATE_NY_DRIVERS = 9,
	ACTIVATE_YNX_DRIVERS = 10,
	MEAS_XPOS = 12,
	MEAS_YPOS = 13,
	MEAS_Z1POS = 14,
	MEAS_Z2POS = 15
}; 

#define TSC2003_CMD(cn,pdn,m) (unsigned char)(((cn) << 4) | ((pdn) << 2) | ((m) << 1))

#define READ_X     TSC2003_CMD(MEAS_XPOS, PD_IREFOFF_ADCON, M_12BIT)
#define READ_Y     TSC2003_CMD(MEAS_YPOS, PD_IREFOFF_ADCON, M_12BIT)
#define READ_Z1    TSC2003_CMD(MEAS_Z1POS, PD_IREFOFF_ADCON, M_12BIT)
#define READ_Z2    TSC2003_CMD(MEAS_Z2POS, PD_IREFOFF_ADCON, M_12BIT)

#define PWRDOWN    TSC2003_CMD(MEAS_VBAT1, PD_POWERDOWN, M_12BIT)


#define TS_PEN_UP_TIMEOUT    (HZ/25)

struct ts_event {
    u16 x;
    u16 y;
    u16 z1, z2;
};  

struct tsc2003_data {
	struct i2c_client client;
	struct input_dev *idev;
	struct work_struct	work;
	atomic_t  avail;
};

static int tsc2003_i2c_detect (struct i2c_adapter *adapter, int address, int kind);
static struct tsc2003_data  g_adc;

/* DONE */
static int
tsc2003_xfer(struct tsc2003_data *tsc, unsigned char cmd)  
{
#if 0
	char d[2];
	int  ret;
	int  val;
	ret = i2c_master_send(&tsc->client, &cmd, 1);
	if (ret <= 0) goto err;
	udelay(20);
	ret = i2c_master_recv(&tsc->client, d, 2);
	if (ret <= 0) goto err;
	val = d[0];
	val <<= 4;
	val |= (d[1]>>4);
	return val;
err: 
	if (!ret) ret = -ENODEV;
	return ret;
#endif
	struct i2c_client *client;
	s32 data;
	u16 val;
	client = &tsc->client;
	data = i2c_smbus_read_word_data(client, cmd);
	if (data < 0)
	{
		dev_err(&client->dev, "i2c io error: %d\n", data);
		return data;
	}

	/* The protocol and raw data format from i2c interface:
	 * S Addr Wr [A] Comm [A] S Addr Rd [A] [DataLow] A [DataHigh] NA P
	 * Where DataLow has [D11-D4], DataHigh has [D3-D0 << 4 | Dummy 4bit].
	 */
	val = swab16(data) >> 4;
	return val;
}

/* DONE */
static void
tsc2003_read_values(struct tsc2003_data *tsc, struct ts_event *tc)  
{  
	tc->y = tsc2003_xfer(tsc, READ_Y);
	tc->x = tsc2003_xfer(tsc, READ_X);
	tc->z1 = tsc2003_xfer(tsc, READ_Z1);
	tc->z2 = tsc2003_xfer(tsc, READ_Z2);
//	printk(KERN_ALERT "--- x=%d, y=%d, z1=%d, z2=%d ---\n", tc->x, tc->y, tc->z1, tc->z2);
	tsc2003_xfer(tsc, PWRDOWN);
}

/* DONE */
static void
tsc2003_work(void *arg)
{
	struct tsc2003_data *data = arg;
	struct input_dev *input;
	struct ts_event tc;
	int pendown;

	input = data->idev;
	pendown = 1 - mxc_get_gpio_ssr(MX27_PIN_SSI3_TXDAT);
	if (pendown)
	{
//		printk(KERN_ALERT "--- tsc2003 input pressed report ---\n");
		tsc2003_read_values(data, &tc);
		input_report_key(input, BTN_TOUCH, 1);
		input_report_abs(input, ABS_X, tc.x);
		input_report_abs(input, ABS_Y, tc.y);
		input_report_abs(input, ABS_PRESSURE, 4095);
		input_sync(input);
		schedule_delayed_work(&data->work,  TS_PEN_UP_TIMEOUT);
	}
	else
	{
//		printk(KERN_ALERT "--- tsc2003 input released report ---\n");
		input_report_key(input, BTN_TOUCH, 0);
		input_report_abs(input, ABS_PRESSURE, 0);
		input_sync(input);
		enable_irq(TS_IRQ);
	}
}

/* DONE */
static irqreturn_t
tsc2003_irq(int irq, void *_data)
{
	struct tsc2003_data *data = _data;

	disable_irq_nosync(irq);
//	printk(KERN_ALERT "--- tsc2003 irq ---\n");
	schedule_work(&data->work);
	return IRQ_HANDLED;
}

/* DONE */
static int
tsc2003_open(struct input_dev *dev)
{
	struct tsc2003_data *data = dev->private;

	if (!atomic_dec_and_test(&data->avail))
	{
		atomic_inc(&data->avail);
		printk(KERN_ALERT "device busy\n");
		return -EBUSY; /* already open */
	}

	printk(KERN_ALERT "tsc2003_open\n");
	enable_irq(TS_IRQ);
	return(0);
}

/* DONE */
static void
tsc2003_close(struct input_dev *dev)
{
	struct tsc2003_data *data = dev->private;
	printk(KERN_ALERT "tsc2003_close\n");
	disable_irq(TS_IRQ);
	atomic_inc(&data->avail);
}

///////////////////////////////////////////////////////
/* DONE */
static int
tsc2003_i2c_attach_adapter(struct i2c_adapter *adapter) 
{
	printk(KERN_ALERT "tsc2003_i2c_attach_adapter\n");
	return i2c_probe(adapter, &addr_data, tsc2003_i2c_detect);
}

/* DONE */
static int
tsc2003_i2c_detach_client(struct i2c_client *client) 
{ 
	int err;   
	struct tsc2003_data *d = i2c_get_clientdata(client);

	free_irq(TS_IRQ, d);
	mxc_free_gpio(MX27_PIN_SSI3_TXDAT);
	input_unregister_device(d->idev);
	if ((err = i2c_detach_client(client))) {
		dev_err(&client->dev, "Client deregistration failed, " 
		"client not detached.\n");
		return err;
	}

	return 0;
}

static struct i2c_driver tsc2003_driver = {
	.driver = {
		.owner = THIS_MODULE,
		.name  = DRIVER_NAME,
	},
	.attach_adapter = tsc2003_i2c_attach_adapter,
	.detach_client  = tsc2003_i2c_detach_client
};

static int
tsc2003_driver_register (struct tsc2003_data *data)
{
	struct input_dev *idev;
	int ret = 0;
	
	gpio_request_mux(MX27_PIN_SSI3_TXDAT, GPIO_MUX_GPIO);
	mxc_set_gpio_direction(MX27_PIN_SSI3_TXDAT, 1);
	set_irq_type(IOMUX_TO_IRQ(MX27_PIN_SSI3_TXDAT), IRQT_FALLING);

	ret = request_irq(TS_IRQ, tsc2003_irq, IRQF_TRIGGER_FALLING, DRIVER_NAME, data);
	if (ret)  
	{ 
		printk(KERN_ERR "%s: cannot grab irq %d\n", __FUNCTION__, TS_IRQ);
		mxc_free_gpio(MX27_PIN_SSI3_TXDAT);
		return ret;
	}

	printk(KERN_ALERT "%s: irq %d\n", __FUNCTION__, TS_IRQ);
	disable_irq(TS_IRQ);
	
	INIT_WORK(&data->work, tsc2003_work, data);

	idev = input_allocate_device();
	data->idev = idev;

	idev->private = data;
	idev->name = DRIVER_NAME;
	idev->id.bustype = BUS_I2C;

	idev->open = tsc2003_open;
	idev->close = tsc2003_close;

	idev->evbit[0] =  BIT(EV_KEY) | BIT(EV_ABS);
	idev->keybit[LONG(BTN_TOUCH)] = BIT(BTN_TOUCH);
	idev->absbit[0] = BIT(ABS_X) | BIT(ABS_Y) | BIT(ABS_PRESSURE);

	input_set_abs_params(idev, ABS_X, 0, ADC_MAX, 0, 0);
	input_set_abs_params(idev, ABS_Y, 0, ADC_MAX, 0, 0);
	input_set_abs_params(idev, ABS_PRESSURE, 0, ADC_MAX, 0, 0);

	input_register_device(idev); 
	printk(KERN_ERR "tsc2003 - registering input device\n"); 
	
	return ret; 
} 

static int
tsc2003_i2c_detect (struct i2c_adapter *adapter, int address, int kind) 
{ 
	struct i2c_client *new_client; 
	struct tsc2003_data *data = &g_adc; 
	int err = 0;

	if (!i2c_check_functionality(adapter, I2C_FUNC_SMBUS_BYTE_DATA|I2C_FUNC_I2C|I2C_FUNC_SMBUS_WORD_DATA)) 
	{
		printk(KERN_ERR "tsc2003: i2c bus doesn't support I2C_FUNC_SMBUS_READ_WORD_DATA\n");
		return -ENOSYS;
	}

	memset(data, 0, sizeof(struct tsc2003_data));
	new_client = &data->client; 
	i2c_set_clientdata(new_client, data);
	new_client->addr = address;
	new_client->adapter = adapter;
	new_client->driver = &tsc2003_driver;
	new_client->flags = 0;

	printk(KERN_ALERT "%s: probing address 0x%x\n", __FUNCTION__, address);
	strlcpy(new_client->name, DRIVER_NAME, I2C_NAME_SIZE);
	err = i2c_attach_client(new_client);
	if (err) return err;

	atomic_set(&data->avail, 1);
	err = tsc2003_driver_register(data);
	if (err) goto failed;
	printk(KERN_ALERT "%s: device address 0x%x attached.\n", __FUNCTION__, address);
	tsc2003_xfer(data, PWRDOWN);

	return 0;
failed:
	i2c_detach_client(new_client);
	return err;
} 

/* DONE */
static int __init tsc2003_init(void)
{
	return i2c_add_driver(&tsc2003_driver);
}

/* DONE */
static void __exit tsc2003_exit(void)
{
	i2c_del_driver(&tsc2003_driver);
}

MODULE_AUTHOR("Lxz <lxz@wishstar.com.cn>");
MODULE_DESCRIPTION("TSC2003 Touch Screen Controller driver");
MODULE_LICENSE("GPL");

module_init(tsc2003_init);
module_exit(tsc2003_exit);


