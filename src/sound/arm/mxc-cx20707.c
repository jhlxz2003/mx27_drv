#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/dma-mapping.h>
#include <linux/init.h>
#include <linux/err.h>
#include <linux/errno.h>
#include <linux/ioctl.h>
#include <linux/delay.h>
#include <linux/slab.h>
#include <linux/interrupt.h>
#include <linux/irq.h>
#include <linux/workqueue.h>
#include <linux/i2c.h>
#include <linux/platform_device.h>
#include <linux/soundcard.h>
#include <linux/clk.h>

#include <asm/hardware.h>
#include <asm/atomic.h>
#include <asm/dma.h>
#include <asm/arch/dma.h>
#include <asm/arch/mx27_pins.h>
#include <asm/arch/mx2_dma.h>
#include <asm/arch/gpio.h>

#include <sound/driver.h>
#include <sound/core.h>
#include <sound/pcm.h>
#include <sound/initval.h>
#include <sound/control.h>

#include <ssi/ssi.h>
#include <ssi/registers.h>
#include <ssi/ssi_types.h>
#include <gpio_mux.h>

//#define CX20707_DEBUG         1
#define CX20707_REG_NAMES     1

#define INTERN_SSI         SSI1

#define MAX_INT(a,b)   (((a)>(b))?(a):(b))

#define CX20707_DRIVER  "mxc_cx20707"
#define CX20707_ID      "cx20707_chip"

#define CX20707_TRISTATE_EEPROM	  0


#ifdef CX20707_DEBUG
#define INFO(fmt,...)	printk(KERN_ALERT fmt, ##__VA_ARGS__)
#else
#define INFO(fmt,...)
#endif

#define CX20707_RATES	( SNDRV_PCM_RATE_8000  \
			| SNDRV_PCM_RATE_11025 \
			| SNDRV_PCM_RATE_16000 \
			| SNDRV_PCM_RATE_22050 \
			| SNDRV_PCM_RATE_32000 \
			| SNDRV_PCM_RATE_44100 \
			| SNDRV_PCM_RATE_48000 \
	 		| SNDRV_PCM_RATE_88200 \
			| SNDRV_PCM_RATE_96000 )

//#define CX20707_FORMATS (SNDRV_PCM_FMTBIT_S16_LE | SNDRV_PCM_FMTBIT_S16_BE)
#define CX20707_FORMATS (SNDRV_PCM_FMTBIT_S16_LE)

#define NOINLINE __attribute__((__noinline__))

/* mixer control */
struct soc_mixer_control {
	int min, max, platform_max;
	unsigned int reg, rreg, shift, rshift, invert;
};

typedef struct audio_stream {
	char *id;
	int stream_id;	
	int ssi;      /* SSI ID on the ARM side */
	int dma_channel;
	int active;		/* we are using this stream for transfer now */
	int period;		/* current transfer period */
	int periods;
	int tx_spin;
	mxc_dma_device_t dma_dev;
	dma_addr_t  dma_dev_addr;
	spinlock_t dma_lock;
	void (*dma_func)(struct audio_stream *);
	atomic_t ref;
	struct snd_pcm_substream *substream;
	/* new add on 2012.12.04 */
	int channels;
} audio_stream_t;

typedef struct mxc_cx20707 {
	struct snd_card *card;
	struct snd_pcm *pcm;
	struct i2c_client iic;
	struct audio_stream s[2];	/* playback & capture */
} mx27_codec_t;

static mx27_codec_t  *cx_chip;
static struct platform_device *cx20707_device;

extern void  gpio_ssi_active(int);
extern void  gpio_ssi_inactive(int);

static DEFINE_MUTEX(codec_mutex);

static int  cx20707_init(mx27_codec_t *codec);

/* set by application layer */
/* 0: exclusive, 1: not exclusive */
//static int       g_jack_policy = 0;
//static int       g_iampl_act = 0;

/* set by jack sense thread */
//static atomic_t  g_jack_state = 0;

//////////////////////////////////////////////
//             i2c client attach            //
//////////////////////////////////////////////
#if 0
{
#endif

static unsigned short normal_i2c[] = { 0x14, I2C_CLIENT_END };
//I2C_CLIENT_INSMOD;
I2C_CLIENT_INSMOD_1(cx20707);

static int  cx20707_i2c_attach(struct i2c_adapter *adap);

#define DRIVER_NAME   "Channel I2C Codec"

static struct i2c_driver cx20707_i2c_driver = {
  .driver = {
    .name = DRIVER_NAME,
    .owner = THIS_MODULE,
    },
  .attach_adapter = cx20707_i2c_attach,
  .detach_client  = i2c_detach_client,
};

static int
cx20707_codec_probe(struct i2c_adapter *adap, int address, int kind)
{
	struct i2c_client *ncli;
	int err;

	if (!cx_chip) return -ENODEV;
	if (!i2c_check_functionality(adap,
		I2C_FUNC_SMBUS_BYTE_DATA|I2C_FUNC_I2C|I2C_FUNC_SMBUS_WORD_DATA)) {
		INFO("mxc-cx20707: i2c bus doesn't support I2C_FUNC_SMBUS_READ_WORD_DATA\n");
		return -ENOSYS;
	}

	ncli = &cx_chip->iic;
	memset(ncli, 0, sizeof(struct i2c_client));
	i2c_set_clientdata(ncli, cx_chip);
	ncli->addr = address;
	ncli->adapter = adap;
	ncli->driver = &cx20707_i2c_driver;
	ncli->flags = 0;
	strlcpy(ncli->name, DRIVER_NAME, I2C_NAME_SIZE);
	INFO(" ---- %s: probing address 0x%x ---- \n", __FUNCTION__, address);
	err = i2c_attach_client(ncli);
	if (err == 0) {
		INFO(" ---- %s: device address 0x%x attached. ---- \n", __FUNCTION__, address);
	}

	return err;
}

static int
cx20707_i2c_attach(struct i2c_adapter *adap)
{
	INFO(" ---- cx20707_i2c_driver: attach_adapter ---- \n");
    return i2c_probe(adap, &addr_data, cx20707_codec_probe);
}

#if 0
}
#endif

/* new add */

#define cx20707_hardware_reset()  do {  \
	mxc_set_gpio_dataout(MX27_PIN_USBH1_RCV, 0); \
	mdelay(200);  \
	mxc_set_gpio_dataout(MX27_PIN_USBH1_RCV, 1); \
} while (0)

/* called when audio capture exit */
static void
cx20707_reset(mx27_codec_t *codec)
{
	cx20707_hardware_reset();
	cx20707_init(codec);
}

//////////////////////////////////////////////
//        codec register read/write         //
//////////////////////////////////////////////
#if 0
{
#endif

enum {
  b_00000000,b_00000001,b_00000010,b_00000011, b_00000100,b_00000101,b_00000110,b_00000111,
  b_00001000,b_00001001,b_00001010,b_00001011, b_00001100,b_00001101,b_00001110,b_00001111,
  b_00010000,b_00010001,b_00010010,b_00010011, b_00010100,b_00010101,b_00010110,b_00010111,
  b_00011000,b_00011001,b_00011010,b_00011011, b_00011100,b_00011101,b_00011110,b_00011111,
  b_00100000,b_00100001,b_00100010,b_00100011, b_00100100,b_00100101,b_00100110,b_00100111,
  b_00101000,b_00101001,b_00101010,b_00101011, b_00101100,b_00101101,b_00101110,b_00101111,
  b_00110000,b_00110001,b_00110010,b_00110011, b_00110100,b_00110101,b_00110110,b_00110111,
  b_00111000,b_00111001,b_00111010,b_00111011, b_00111100,b_00111101,b_00111110,b_00111111,
  b_01000000,b_01000001,b_01000010,b_01000011, b_01000100,b_01000101,b_01000110,b_01000111,
  b_01001000,b_01001001,b_01001010,b_01001011, b_01001100,b_01001101,b_01001110,b_01001111,
  b_01010000,b_01010001,b_01010010,b_01010011, b_01010100,b_01010101,b_01010110,b_01010111,
  b_01011000,b_01011001,b_01011010,b_01011011, b_01011100,b_01011101,b_01011110,b_01011111,
  b_01100000,b_01100001,b_01100010,b_01100011, b_01100100,b_01100101,b_01100110,b_01100111,
  b_01101000,b_01101001,b_01101010,b_01101011, b_01101100,b_01101101,b_01101110,b_01101111,
  b_01110000,b_01110001,b_01110010,b_01110011, b_01110100,b_01110101,b_01110110,b_01110111,
  b_01111000,b_01111001,b_01111010,b_01111011, b_01111100,b_01111101,b_01111110,b_01111111,
  b_10000000,b_10000001,b_10000010,b_10000011, b_10000100,b_10000101,b_10000110,b_10000111,
  b_10001000,b_10001001,b_10001010,b_10001011, b_10001100,b_10001101,b_10001110,b_10001111,
  b_10010000,b_10010001,b_10010010,b_10010011, b_10010100,b_10010101,b_10010110,b_10010111,
  b_10011000,b_10011001,b_10011010,b_10011011, b_10011100,b_10011101,b_10011110,b_10011111,
  b_10100000,b_10100001,b_10100010,b_10100011, b_10100100,b_10100101,b_10100110,b_10100111,
  b_10101000,b_10101001,b_10101010,b_10101011, b_10101100,b_10101101,b_10101110,b_10101111,
  b_10110000,b_10110001,b_10110010,b_10110011, b_10110100,b_10110101,b_10110110,b_10110111,
  b_10111000,b_10111001,b_10111010,b_10111011, b_10111100,b_10111101,b_10111110,b_10111111,
  b_11000000,b_11000001,b_11000010,b_11000011, b_11000100,b_11000101,b_11000110,b_11000111,
  b_11001000,b_11001001,b_11001010,b_11001011, b_11001100,b_11001101,b_11001110,b_11001111,
  b_11010000,b_11010001,b_11010010,b_11010011, b_11010100,b_11010101,b_11010110,b_11010111,
  b_11011000,b_11011001,b_11011010,b_11011011, b_11011100,b_11011101,b_11011110,b_11011111,
  b_11100000,b_11100001,b_11100010,b_11100011, b_11100100,b_11100101,b_11100110,b_11100111,
  b_11101000,b_11101001,b_11101010,b_11101011, b_11101100,b_11101101,b_11101110,b_11101111,
  b_11110000,b_11110001,b_11110010,b_11110011, b_11110100,b_11110101,b_11110110,b_11110111,
  b_11111000,b_11111001,b_11111010,b_11111011, b_11111100,b_11111101,b_11111110,b_11111111,
};

#define BIT_WI         ((unsigned char)(1<<1))
#define BIT_WC         ((unsigned char)(1<<2))

#define REG_TYPE_RO	    0
#define REG_TYPE_RW	    1
#define REG_TYPE_WI	   ((unsigned char)(BIT_WI|0x01))
#define REG_TYPE_WC	   ((unsigned char)(BIT_WC|0x01))
#define REG_TYPE_CI	   ((unsigned char)(BIT_WC|BIT_WI|0x01))

#define REG_TYPE_MASK	((unsigned char)0x0F)

enum {
#define __REG(id,addr,data,bias,type,w) id,
#include "cx20707_regs.h"
#undef __REG
};

typedef u8 cx20707_reg_t;

static cx20707_reg_t cx20707_data[] = {
#define __REG(id,addr,data,bias,type,w) data,
#include "cx20707_regs.h"
#undef __REG
};

struct cx20707_reg
{
#if CX20707_REG_NAMES
    char *name;
#endif
    u16   addr;
    u8    bias;
    u8    type;
};

static const struct cx20707_reg cx20707_regs[] = {
#if CX20707_REG_NAMES
#define __REG(a,b,c,d,e,f) { #a,b,d,REG_TYPE_##e },
#else
#define __REG(a,b,c,d,e,f) { b,d,REG_TYPE_##e },
#endif
#include "cx20707_regs.h"
#undef __REG
};

static int NOINLINE
_cx20707_real_read(mx27_codec_t *chip, unsigned int reg)
{
    struct i2c_client  *client;
    struct i2c_adapter *adap;
    struct i2c_msg	    msg[2];
    const struct cx20707_reg *ri;
    int	   dat;
    u8	   data[4];

    client = &chip->iic;
    ri = &cx20707_regs[reg];

    data[0] = (u8)(ri->addr>>8);
    data[1] = (u8)(ri->addr>>0);

    msg[0].addr  = client->addr;
    msg[0].flags = client->flags & I2C_M_TEN;
    msg[0].len   = 2;
    msg[0].buf   = &data[0];

    msg[1].addr  = client->addr;
    msg[1].flags = (client->flags & I2C_M_TEN) | I2C_M_RD;
	msg[1].len   = 1;
    msg[1].buf   = &data[2];

	adap = client->adapter;
    if (i2c_transfer(adap, msg, 2) != 1)
        return -EIO;
	dat = ri->bias + data[2];

    return dat;
}

static int NOINLINE
cx20707_real_read(mx27_codec_t *chip, unsigned int reg)
{
	int err;

	err = _cx20707_real_read(chip, reg);
	if (err < 0)
	{
		INFO("--- read cx20707 error ---\n");
		return err;
	}

    cx20707_data[reg] = (cx20707_reg_t)err;
    return err;
}

static int NOINLINE
cx20707_real_write(mx27_codec_t *chip, unsigned int reg, unsigned char value)
{
    struct i2c_client  *client;
    struct i2c_adapter *adap;
    struct i2c_msg      msg[2];
    u8     data[4];
    int    err;
    const struct cx20707_reg *ri;

	client = &chip->iic;
	adap   = client->adapter;
    ri     = &cx20707_regs[reg];

    if ((ri->type&REG_TYPE_MASK) == 0) return 0;
    msg[0].addr  = client->addr;
	msg[0].flags = client->flags & I2C_M_TEN;
	msg[0].buf   = &data[0];
	data[0]      = (u8)(ri->addr >> 8);
	data[1]      = (u8)(ri->addr >> 0);
	INFO("--- Register name: %s, Address = 0x%04x, value = %x ---\n", ri->name, ri->addr, value);
	data[2] = (u8)(value - ri->bias);
	msg[0].len = 3;

	err = i2c_transfer(adap, msg, 1);
    if (err < 0)
    {
    	printk(KERN_ALERT "--- write cx20707 error ---\n");
    	return err;
    }
	cx20707_data[reg] = (cx20707_reg_t)value;
	#if 0
	{
  		unsigned int val;
	    val = _cx20707_real_read(chip, reg);
        INFO("--- write value: %u, read value: %u ---\n", value, val);
	}
	#endif
	return 1;
}

static int
cx20707_dsp_newc(mx27_codec_t *codec)
{
    unsigned int r;
    int v;
    INFO("--- IN NEWC ---\n");
	if (cx20707_real_write(codec, DSPInit1, cx20707_data[DSPInit1]|NEWC) == 1) {
		for (r = 1000; r > 0; --r) {
			msleep(1);
			v = cx20707_real_read(codec, DSPInit1);
			if (v < 0) return -EIO;
			if (!(v&NEWC)) return 1;
		}
	}
	return -EIO;
}

static void
cx20707_update_register_forced(mx27_codec_t *codec ,unsigned int reg, unsigned char value, unsigned char mask)
{
	unsigned char new, old;

	old = cx20707_data[reg];
	new = ((old & ~mask) | value);
#ifdef CX20707_DEBUG
	{
		const struct cx20707_reg *ri;
		ri = &cx20707_regs[reg];
		printk(KERN_ALERT "--- Register name: %s, Address = 0x%04x, old_val = %x, new_val = %x ---\n", ri->name, ri->addr, old, new);
	}
#endif
	cx20707_real_write(codec, reg, new);
}

static int
cx20707_update_register(mx27_codec_t *codec ,unsigned int reg, unsigned char value, unsigned char mask)
{
	int err = 0;
	unsigned char new, old;
	const struct cx20707_reg *ri;
   
//	mutex_lock(&codec_mutex);
	ri = &cx20707_regs[reg];
	old = cx20707_data[reg];
	new = ((old & ~mask) | value);
	INFO("--- Register name: %s, Address = 0x%04x, old_val = %x, new_val = %x ---\n",
	      ri->name, ri->addr, old, new);
//	if (new != old) {
		INFO("--- Register name: %s, Address = 0x%04x, value changed ---\n", ri->name, ri->addr);
		err = cx20707_real_write(codec, reg, new);
		if (err == 1) {
			if (cx20707_regs[reg].type&BIT_WC) {
				INFO("--- Register name: %s, Address = 0x%04x, NEWC ---\n", ri->name, ri->addr);
				err = cx20707_dsp_newc(codec);
			}
		}
//	}
//	mutex_unlock(&codec_mutex);
	INFO("--- cx20707_update_register return: %d ---\n", err);
	return err;
}

static int NOINLINE
cx20707_write(mx27_codec_t *codec, unsigned int reg, unsigned int value)
{
    int err = 0;
  
//    mutex_lock(&codec_mutex);
    if ((err = cx20707_real_write(codec, reg, value)) == 1) {
		if (cx20707_regs[reg].type&BIT_WC) {
			err = cx20707_dsp_newc(codec);
		}
	}
//	mutex_unlock(&codec_mutex);
	return err;
}

static int
cx20707_init(mx27_codec_t *codec)
{
	int  n, vl, vh, abcode;

	for (n = 5000; n > 0; --n) {
        abcode = cx20707_real_read(codec, ABORT_CODE);
        if (abcode == 0x01) {
            break;
        }
#ifdef CX20707_DEBUG
        else if (abcode == 0x02) {
    	    INFO("--- Short Circuit detected (Class-D) ---\n");
        } else if (abcode == 0x03) {
    	    INFO("--- Over Temperature (Class-D) ---\n");
        } else if (abcode == 0x04) {
    	    INFO("--- DSP core Not responding ---\n");
        } else if (abcode == 0x05) {
    	    INFO("--- FF TBD ---\n");
        } else if (abcode == 0x00) {
    	    INFO("--- Device not ready ---\n");
        } else {
    	    INFO("--- abcode = %d ---\n", abcode);
        }
#endif
        msleep(1);
	}

	if (n == 0) {
		INFO("--- Timeout waiting for Channel to come out of reset! ----\n");
		return -EIO;
	}

    for (n = 0; n < ARRAY_SIZE(cx20707_regs); ++n) {
        if (cx20707_regs[n].type&BIT_WI) {
  		    cx20707_real_write(codec, n, cx20707_data[n]);
  		    if (cx20707_regs[n].type&BIT_WC) {
  			    cx20707_dsp_newc(codec);
  			}
  	    }
  	    else
  	    {
  		    cx20707_real_read(codec, n);
	  	}
	}

    vl = cx20707_data[FV_LO];
    vh = cx20707_data[FV_HI];

    vl = (vl>>4)*10+(vl&0x0f);
    INFO("--- Channel firmware version %u.%u, chip CX207%02u, ---\n", vh, vl, cx20707_data[CHIP_VERSION]);

    switch (cx20707_data[RELEASE_TYPE]) {
	case 12: INFO("--- Custom Release ---\n"); break;
	case 14: INFO("--- Engineering Release ---\n"); break;
	case 15: INFO("--- Field Release ---\n"); break;
	default: INFO("--- Release %u? ---\n",cx20707_data[RELEASE_TYPE]); break;
	}

    vl = cx20707_data[VV_LO];
    vh = cx20707_data[VV_HI];
    INFO("--- %s(): Channel patch version %u.%u ---\n", __func__, vh, vl);

	return 0;
}

#if 0
}
#endif

//////////////////////////////////////////////
//            codec widget setting          //
//////////////////////////////////////////////
#if 0
{
#endif

#include "cx20707_macros.h"

#define FILLER_SZ            2
#define CLK_PER_FRAME       (32 + FILLER_SZ * 16)

#define CLK_PER_FRAME_PHASE ((CLK_PER_FRAME>>3)-1)
#define SYNC_WIDTH          ((CLK_PER_FRAME>>1)-1)

static int inroute = 0;
static int inmute = 0;
static int outmute = 0;

static int rate_idx = -1;

#define INROUTE_MIC           0
#define INROUTE_LINEIN_DIFF   1
#define INROUTE_LINEIN_SE     2

#ifdef CX20707_DEBUG
static void
dbg_clk(int idx)
{
    char *freq[] = {
    	"512kHz",
    	"705kHz",
    	"1024Hz",
    	"1411kHz",
    	"1536kHz",
    	"2048kHz",
    	"2822kHz",
    	"3072kHz",
    	"5644kHz",
    	"6144kHz"
    };

    printk(KERN_ALERT "--- clk: %s ---\n", freq[idx]);
}
#endif

static unsigned char clk_div[] = {
	PORT1_DIV_SEL_512kHz,
	PORT1_DIV_SEL_705kHz,
	PORT1_DIV_SEL_1_024MHz,
	PORT1_DIV_SEL_1_411MHz,
	PORT1_DIV_SEL_1_536MHz,
	PORT1_DIV_SEL_2_048MHz,
	PORT1_DIV_SEL_2_822MHz,
	PORT1_DIV_SEL_3_072MHz,
	PORT1_DIV_SEL_5_644MHz,
	PORT1_DIV_SEL_6_144MHz
};

/* DONE */
static int
clk_divider(unsigned int rate)
{
	int idx;

	switch (rate) {
	case 8000:
	    idx = 0;
	    break;
    case 11025:
		idx = 1;
	    break;
	case 16000:
		idx = 2;
		break;
	case 22050:
		idx = 3;
		break;
	case 24000:
		idx = 4;
		break;
	case 32000:
		idx = 5;
		break;
	case 44100:
		idx = 6;
		break;
	case 48000:
		idx = 7;
		break;
	case 88200:
		idx = 8;
		break;
	case 96000:
		idx = 9;
		break;
	default:
		idx = -1;
		break;
	}
	return idx;
}

/* DONE */
static void
cx20707_i2s_config(mx27_codec_t *codec, int clk_idx)
{
//	mutex_lock(&codec_mutex);
	if (clk_idx != rate_idx) {
		printk(KERN_ALERT "--- rate changed from %d to %d ---\n", rate_idx, clk_idx);
		cx20707_real_write(codec, CLOCK_DIVIDER, clk_div[clk_idx]);

		/* 0x0F51: Left Justified Data delayed 1 bit. */
		cx20707_real_write(codec, PORT1_CTL1, 0x80);

		cx20707_real_write(codec, PORT1_TX_CLOCKS_PER_FRAME_PHASE, CLK_PER_FRAME_PHASE);
		cx20707_real_write(codec, PORT1_RX_CLOCKS_PER_FRAME_PHASE, CLK_PER_FRAME_PHASE);

		cx20707_real_write(codec, PORT1_RX_SYNC_WIDTH, SYNC_WIDTH);
		cx20707_real_write(codec, PORT1_TX_SYNC_WIDTH, SYNC_WIDTH);

        /* sample size: 0101 */
		cx20707_real_write(codec, PORT1_CTL2, 0x05);

		cx20707_real_write(codec, PORT1_RX_STR1_SLOT_CTL, 0);
		cx20707_real_write(codec, PORT1_RX_STR2_SLOT_CTL, FILLER_SZ + 2);
		cx20707_real_write(codec, PORT1_TX_STR1_SLOT_CTL, 0);
		cx20707_real_write(codec, PORT1_TX_STR2_SLOT_CTL, FILLER_SZ + 2);

		cx20707_update_register_forced(codec, S5OutpRate, (2<<4), (3<<4));
		cx20707_dsp_newc(codec);

		cx20707_update_register_forced(codec, S5OutpRate, (clk_idx&0x0f), 0x0f);
		cx20707_dsp_newc(codec);

		rate_idx = clk_idx;
	}
//	mutex_unlock(&codec_mutex);
}

/* DONE */
static void
cx20707_stream3_config(mx27_codec_t* codec, int clk_idx, int chls)
{
	if (chls == 0) {
		cx20707_stream3_disable(codec);
		return;
	}

	if (chls == 1) {
	    cx20707_stream3_mono_left(codec);
	} else {
	    cx20707_stream3_stero(codec);
	}
	
	/* 修改了stream2的声道配置后,出现如下情况: */
	/* 相同采样率的歌曲，只能播放一次，除非不同的声道. */
	/* 相同声道但不同采样率的歌曲可以播放 */
	/* new add oon 2012.12.04 */
	codec->s[0].channels = chls;

	/* set input rate */
	cx20707_stream3_src_config(codec, clk_idx&0x0f);
	/* enable stream3 */
    cx20707_stream3_enable(codec);
}

/* DONE */
static void
cx20707_stream7_config(mx27_codec_t* codec, unsigned char from)
{
	if (from == 0) {
		cx20707_stream7_disable(codec);
	} else {
        cx20707_stream7_from_config(codec, from);
	    cx20707_stream7_output_rate_48k(codec);
	    cx20707_stream7_enable(codec);
	}
}

/* DONE */
/*
static void
cx20707_lineout_config(mx27_codec_t* codec, int v)
{
	cx20707_outctl_manual(codec);
	if (v == 0) {
		cx20707_lineout_disable(codec);
	} else {
        cx20707_lineout_single_end(codec);
        cx20707_lineout_enable(codec);
    }
}
*/

//////////////////////////////////////////////////////////////////
/* on:0-disable linein,1-enable linein-diff, 2-enable linein-se */
/*
static void
cx20707_linein_config(mx27_codec_t *codec, int on)
{
    cx20707_linctl_manual(codec);
    if (on == 0) {
        cx20707_lin1_disable(codec);
    } else {
        if (on == 1) {
            cx20707_lin1_diff(codec);
        } else if (on == 2) {
            cx20707_lin1_single_end(codec);
        }
        cx20707_lin1_enable(codec);
        cx20707_lin1_unmute(codec);
        cx20707_lin_adc_unmuteL(codec);
        cx20707_lin_adc_unmuteR(codec);
        inmute = 0;
    }
}

static void
cx20707_stream1_config(mx27_codec_t *codec, int on)
{
	if (on) {
	    cx20707_stream1_mono_left(codec);
	    cx20707_stream1_input_rate_16k(codec);
	    cx20707_stream1_enable(codec);
	} else {
		cx20707_stream1_disable(codec);
	}
}*/
//////////////////////////////////////////////////////////////////

/* DONE */
static void
cx20707_mic_config(mx27_codec_t *codec)
{
//	cx20707_mic_always_on(codec);
//	cx20707_mic_bias_80p(codec);
	cx20707_mic_unmuteL(codec);
	cx20707_mic_unmuteR(codec);
	inmute = 0;
}

/* DONE */
static void
cx20707_stream2_config(mx27_codec_t *codec, int on)
{
	if (on) {
		if (on == 1) {
	    	cx20707_stream2_mono_left(codec);
	    } else {
	    	cx20707_stream2_stero(codec);
	    }
	    cx20707_stream2_input_rate_16k(codec);
	    cx20707_stream2_enable(codec);
	} else {
		cx20707_stream2_stero(codec);
		cx20707_stream2_disable(codec);
	}
}

/* DONE */
static void
cx20707_stream5_config(mx27_codec_t *codec, unsigned char from)
{
	if (from != 0) {
		cx20707_stream5_from_config(codec, from);
		cx20707_stream5_enable(codec);
	} else {
		cx20707_stream5_disable(codec);
	}
}

/* DONE */
#define cx20707_micin_open(codec, chls) \
    do { \
        cx20707_mic_config(codec);  \
        cx20707_stream2_config(codec, chls);  \
        cx20707_voice_from_config(codec, S2OUT);  \
    } while (0)

#define cx20707_micin_close(codec)   cx20707_stream2_config(codec, 0)

/*
#define cx20707_linein_open(codec, m) \
    do { \
        cx20707_linein_config(codec, m);  \
        cx20707_stream1_config(codec, 1); \
        cx20707_voice_from_config(codec, S1OUT);  \
    } while (0)

#define cx20707_linein_close(codec) \
    do { \
        cx20707_linein_config(codec, 0);  \
        cx20707_stream1_config(codec, 0); \
    } while (0)
*/

#define mxc_audio_mute(v)   mxc_set_gpio_dataout(MX27_PIN_CSPI1_SS1, (v))

#define mxc_speaker_close(codec) \
    do { \
    	mxc_set_gpio_dataout(MX27_PIN_CSPI1_SS1, 1); \
    	cx20707_classD_disable(codec); \
    } while (0)

/*
void
mxc_jack_unplugged(void)
{
	atomic_set(&g_jack_state, 0);
	mxc_audio_mute(0);
	if (g_iampl_act) {
	    cx20707_classD_enable(&cx_chip);
	    INFO("--- turn on ClassD ---\n");
	}
	INFO("--- HeadPhone unplugged ---\n");
}

void
mxc_jack_plugged(void)
{
	atomic_set(&g_jack_state, 1);
	if (g_jack_policy == 0) {
	    mxc_speaker_close(&cx_chip);
	    INFO("--- turn off speaker ---\n");
	}
	INFO("--- HeadPhone plugged ---\n");
}
*/

#if 0
}
#endif

//////////////////////////////////////////////
//             control interface            //
//////////////////////////////////////////////
#if 0
{
#endif

/* DONE */
static int
_snd_cx20707_put(struct snd_kcontrol *kcontrol, struct snd_ctl_elem_value *ucontrol, int newc)
{
	struct soc_mixer_control *mc = (struct soc_mixer_control *)kcontrol->private_value;
	mx27_codec_t *codec = snd_kcontrol_chip(kcontrol);
	unsigned int reg = mc->reg;
	unsigned int shift = mc->shift;
	unsigned int rshift = mc->rshift;
	int max = mc->max;
	unsigned int mask = (1 << fls(max)) - 1;
	unsigned int invert = mc->invert;
	unsigned int val, val2, val_mask, old, new;
	int change, ret = 0;

	INFO("--- PUT reg address: 0x%4x ---\n", cx20707_regs[reg].addr);
	val = (ucontrol->value.integer.value[0] & mask);
	if (invert) {
		val = max - val;
	}

	val_mask = mask << shift;
	val = val << shift;
	if (shift != rshift) {
		val2 = (ucontrol->value.integer.value[1] & mask);
		if (invert)
			val2 = max - val2;
		val_mask |= mask << rshift;
		val |= val2 << rshift;
	}

	old = cx20707_data[reg];
	new = (old & ~val_mask) | val;
	change = (old != new);
	if (change) {
		ret = cx20707_write(codec, reg, new);
		if (ret < 0)
			return ret;
		if (newc == 1) {
		    cx20707_dsp_newc(codec);
		}
	}
	return change;
}

/* DONE */
static int
snd_cx20707_info(struct snd_kcontrol *kcontrol, struct snd_ctl_elem_info *uinfo)
{
	struct soc_mixer_control *mc = (struct soc_mixer_control *)kcontrol->private_value;
	int platform_max;
	unsigned int shift = mc->shift;
	unsigned int rshift = mc->rshift;

	if (!mc->platform_max)
		mc->platform_max = mc->max;
	platform_max = mc->platform_max;

	if (platform_max == 1 && !strstr(kcontrol->id.name, " Volume"))
		uinfo->type = SNDRV_CTL_ELEM_TYPE_BOOLEAN;
	else
		uinfo->type = SNDRV_CTL_ELEM_TYPE_INTEGER;

	uinfo->count = (shift == rshift ? 1 : 2);
	uinfo->value.integer.min = 0;
	uinfo->value.integer.max = platform_max;
	return 0;
}

/* DONE */
static int
snd_cx20707_get(struct snd_kcontrol *kcontrol, struct snd_ctl_elem_value *ucontrol)
{
	struct soc_mixer_control *mc = (struct soc_mixer_control *)kcontrol->private_value;
	unsigned int reg = mc->reg;
	unsigned int shift = mc->shift;
	unsigned int rshift = mc->rshift;
	int max = mc->max;
	unsigned int mask = (1 << fls(max)) - 1;
	unsigned int invert = mc->invert;

	INFO("--- Get reg address: 0x%4x ---\n", cx20707_regs[reg].addr);
	ucontrol->value.integer.value[0] = (cx20707_data[reg] >> shift) & mask;
	if (shift != rshift) {
		ucontrol->value.integer.value[1] = (cx20707_data[reg] >> rshift) & mask;
	}

	if (invert) {
		ucontrol->value.integer.value[0] = max - ucontrol->value.integer.value[0];
		if (shift != rshift) {
			ucontrol->value.integer.value[1] = max - ucontrol->value.integer.value[1];
		}
	}

	return 0;
}

static int
snd_cx20707_put(struct snd_kcontrol *kcontrol, struct snd_ctl_elem_value *ucontrol)
{
	return _snd_cx20707_put(kcontrol, ucontrol, 0);
}

static int
snd_cx20707_put_newc(struct snd_kcontrol *kcontrol, struct snd_ctl_elem_value *ucontrol)
{
	return _snd_cx20707_put(kcontrol, ucontrol, 1);
}

static int
snd_cx20707_info2(struct snd_kcontrol *kcontrol, struct snd_ctl_elem_info *uinfo)
{
	struct soc_mixer_control *mc = (struct soc_mixer_control *)kcontrol->private_value;
	int platform_max;

	if (!mc->platform_max)
		mc->platform_max = mc->max;
	platform_max = mc->platform_max;

	if (platform_max == 1 && !strstr(kcontrol->id.name, " Volume"))
		uinfo->type = SNDRV_CTL_ELEM_TYPE_BOOLEAN;
	else
		uinfo->type = SNDRV_CTL_ELEM_TYPE_INTEGER;

	uinfo->count = 2;
	uinfo->value.integer.min = 0;
	uinfo->value.integer.max = platform_max;
	return 0;
}

static int
snd_cx20707_get2(struct snd_kcontrol *kcontrol, struct snd_ctl_elem_value *ucontrol)
{
	struct soc_mixer_control *mc = (struct soc_mixer_control *)kcontrol->private_value;
	unsigned int reg = mc->reg;
	unsigned int reg2 = mc->rreg;
	unsigned int shift = mc->shift;
	int max = mc->max;
	unsigned int mask = (1 << fls(max)) - 1;
	unsigned int invert = mc->invert;

	ucontrol->value.integer.value[0] = (cx20707_data[reg] >> shift) & mask;
	ucontrol->value.integer.value[1] = (cx20707_data[reg2] >> shift) & mask;
	if (invert) {
		ucontrol->value.integer.value[0] = max - ucontrol->value.integer.value[0];
		ucontrol->value.integer.value[1] = max - ucontrol->value.integer.value[1];
	}

	return 0;
}

static int
snd_cx20707_put2(struct snd_kcontrol *kcontrol, struct snd_ctl_elem_value *ucontrol)
{
	struct soc_mixer_control *mc = (struct soc_mixer_control *)kcontrol->private_value;
	mx27_codec_t *codec = snd_kcontrol_chip(kcontrol);
	unsigned int reg = mc->reg;
	unsigned int reg2 = mc->rreg;
	unsigned int shift = mc->shift;
	int max = mc->max;
	unsigned int mask = (1 << fls(max)) - 1;
	unsigned int invert = mc->invert;
	unsigned int val, val2, val_mask, old, new;
	int change, change2, ret = 0;

	val_mask = mask << shift;
	val = (ucontrol->value.integer.value[0] & mask);
	val2 = (ucontrol->value.integer.value[1] & mask);

	if (invert) {
		val = max - val;
		val2 = max - val2;
	}

	val = val << shift;
	val2 = val2 << shift;
	
	old = cx20707_data[reg];
	new = (old & ~val_mask) | val;
	change = old != new;
	if (change) {
		ret = cx20707_write(codec, reg, new);
		if (ret < 0)
			return ret;
	}

	old = cx20707_data[reg2];
	new = (old & ~val_mask) | val2;
	change2 = old != new;
	if (change2) {
		ret = cx20707_write(codec, reg2, new);
		if (ret < 0)
			return ret;
	}
	return (change||change2);
}

//#ifdef CX20707_DEBUG
/*
static int loopback = 0;

static int
cx20707_loopback_info(struct snd_kcontrol *kcontrol, struct snd_ctl_elem_info *uinfo)
{
	struct soc_mixer_control *mc = (struct soc_mixer_control *)kcontrol->private_value;
	int platform_max;

	if (!mc->platform_max)
		mc->platform_max = mc->max;
	platform_max = mc->platform_max;

	if (platform_max == 1 && !strstr(kcontrol->id.name, " Volume"))
		uinfo->type = SNDRV_CTL_ELEM_TYPE_BOOLEAN;
	else
		uinfo->type = SNDRV_CTL_ELEM_TYPE_INTEGER;

	uinfo->count = 1;
	uinfo->value.integer.min = 0;
	uinfo->value.integer.max = platform_max;
	return 0;
}

static int
cx20707_loopback_get(struct snd_kcontrol *kcontrol, struct snd_ctl_elem_value *ucontrol)
{
	ucontrol->value.integer.value[0] = loopback;
	return 0;
}

static int
cx20707_loopback_put1(struct snd_kcontrol *kcontrol, struct snd_ctl_elem_value *ucontrol)
{
	mx27_codec_t *codec = snd_kcontrol_chip(kcontrol);

	INFO("--- loopback: %ld ---\n", ucontrol->value.integer.value[0]);
	if (ucontrol->value.integer.value[0]) {
		if (loopback == 0) {
			cx20707_mic_config(codec);
			cx20707_stream2_config(codec, 1);		
			cx20707_stream7_config(codec, S2OUT);
			(*out_enable[out_id])(codec, 1);
			loopback = 1;
			INFO("--- DSP Loopback mode on ---\n");
			return 1;
		}
	} else {
		if (loopback == 1) {
			cx20707_stream2_config(codec, 0);		
			cx20707_stream7_config(codec, 0);
			(*out_enable[out_id])(codec, 0);
			loopback = 0;
			INFO("--- DSP Loopback mode off ---\n");
			return 1;
		}
	}

	return 0;
}

#define SOC_LOOPBACK1(xname, xmax) \
{	.iface = SNDRV_CTL_ELEM_IFACE_MIXER, .name = (xname), \
	.info = cx20707_loopback_info, \
	.get = cx20707_loopback_get, .put = cx20707_loopback_put1, \
	.private_value = (unsigned long)&(struct soc_mixer_control) \
		{.reg = 0, .rreg = 0, .shift = 0, \
			.max = xmax, .platform_max = xmax, .invert = 0} }
*/
//#endif

/*
static int
cx20707_inroute_get(struct snd_kcontrol *kcontrol, struct snd_ctl_elem_value *ucontrol)
{
	ucontrol->value.integer.value[0] = inroute;
	return 0;
}

static int
cx20707_inroute_put(struct snd_kcontrol *kcontrol, struct snd_ctl_elem_value *ucontrol)
{
	mx27_codec_t *codec = snd_kcontrol_chip(kcontrol);

	INFO("--- inroute: %ld ---\n", ucontrol->value.integer.value[0]);
	if (ucontrol->value.integer.value[0] == INROUTE_MIC) {
	    if (inroute != INROUTE_MIC) {
	        cx20707_linein_close(codec);
		    cx20707_micin_open(codec, 2);
		    inroute = INROUTE_MIC;
            INFO("--- INROUTE_MIC ---\n");
            return 1;
        }
	} else if (ucontrol->value.integer.value[0] == INROUTE_LINEIN_DIFF) {
	    if (inroute != INROUTE_LINEIN_DIFF) {
	        cx20707_micin_close(codec);
		    cx20707_linein_open(codec, INROUTE_LINEIN_DIFF);
		    inroute = INROUTE_LINEIN_DIFF;
            INFO("--- INROUTE_LINEIN_DIFF ---\n");
            return 1;
        }
	} else if (ucontrol->value.integer.value[0] == INROUTE_LINEIN_SE) {
	    if (inroute != INROUTE_LINEIN_SE) {
		    cx20707_micin_close(codec);
		    cx20707_linein_open(codec, INROUTE_LINEIN_SE);
		    inroute = INROUTE_LINEIN_SE;
            INFO("--- INROUTE_LINEIN_SE ---\n");
            return 1;
        }
	}

	return 0;
}
*/

/* DONE */
static int
cx20707_inmute_get(struct snd_kcontrol *kcontrol, struct snd_ctl_elem_value *ucontrol)
{
	ucontrol->value.integer.value[0] = inmute;
	return 0;
}

/* DONE */
static int
cx20707_inmute_put(struct snd_kcontrol *kcontrol, struct snd_ctl_elem_value *ucontrol)
{
	mx27_codec_t *codec = snd_kcontrol_chip(kcontrol);

	INFO("--- inmute: %ld ---\n", ucontrol->value.integer.value[0]);
	if (ucontrol->value.integer.value[0] == 0) {
	    if (inmute != 0) {
	        cx20707_mic_unmuteL(codec);
	        cx20707_mic_unmuteR(codec);
	        cx20707_lin_adc_unmuteL(codec);
	        cx20707_lin_adc_unmuteR(codec);
		    inmute = 0;
            INFO("--- CAPTURE MUTE ---\n");
            return 1;
        }
	} else if (ucontrol->value.integer.value[0] == 1) {
	    if (inmute != 1) {
	        cx20707_mic_muteL(codec);
	        cx20707_mic_muteR(codec);
	        cx20707_lin_adc_muteL(codec);
	        cx20707_lin_adc_muteR(codec);
		    inmute = 1;
            INFO("--- CAPTURE UNMUTE ---\n");
            return 1;
        }
	}

	return 0;
}

/* DONE */
static int
cx20707_outmute_get(struct snd_kcontrol *kcontrol, struct snd_ctl_elem_value *ucontrol)
{
	ucontrol->value.integer.value[0] = outmute;
	return 0;
}

/* DONE */
static int
cx20707_outmute_put(struct snd_kcontrol *kcontrol, struct snd_ctl_elem_value *ucontrol)
{
	mx27_codec_t *codec = snd_kcontrol_chip(kcontrol);

	INFO("--- inroute: %ld ---\n", ucontrol->value.integer.value[0]);
	if (ucontrol->value.integer.value[0] == 0) {
	    if (outmute != 0) {
	        cx20707_outp_unmuteL(codec);
	        cx20707_outp_unmuteR(codec);
		    outmute = 0;
            INFO("--- PLAYBACK MUTE ---\n");
            return 1;
        }
	} else if (ucontrol->value.integer.value[0] == 1) {
	    if (outmute != 1) {
	        cx20707_outp_muteL(codec);
	        cx20707_outp_muteR(codec);
		    outmute = 1;
            INFO("--- PLAYBACK UNMUTE ---\n");
            return 1;
        }
	}

	return 0;
}

/*
static int  ampl_mute = 0;

static int
cx20707_ampl_get(struct snd_kcontrol *kcontrol, struct snd_ctl_elem_value *ucontrol)
{
	ucontrol->value.integer.value[0] = ampl_mute;
	return 0;
}

static int
cx20707_ampl_put(struct snd_kcontrol *kcontrol, struct snd_ctl_elem_value *ucontrol)
{
	INFO("--- ampl mute: %ld ---\n", ucontrol->value.integer.value[0]);
	if (ucontrol->value.integer.value[0] == 0) {
	    mxc_audio_mute(0);
		ampl_mute = 0;
        INFO("--- AMPL PLAYBACK UNMUTE ---\n");
        return 1;
	} else if (ucontrol->value.integer.value[0] == 1) {
	    mxc_audio_mute(1);
		ampl_mute = 1;
        INFO("--- AMPL PLAYBACK MUTE ---\n");
        return 1;
	}

	return 0;
}
*/

/*
static int
cx20707_jackp_get(struct snd_kcontrol *kcontrol, struct snd_ctl_elem_value *ucontrol)
{
	ucontrol->value.integer.value[0] = g_jack_policy;
	return 0;
}

static int
cx20707_jackp_put(struct snd_kcontrol *kcontrol, struct snd_ctl_elem_value *ucontrol)
{
	INFO("--- ampl mute: %ld ---\n", ucontrol->value.integer.value[0]);
	if (ucontrol->value.integer.value[0] == 0) {
		g_jack_policy = 0;
        INFO("--- g_jack_policy: EXCLUSIVE ---\n");
        return 1;
	} else if (ucontrol->value.integer.value[0] == 1) {
		g_jack_policy = 1;
        INFO("--- g_jack_policy: COEXIST ---\n");
        return 1;
	}

	return 0;
}
*/

/*
static int
cx20707_iampl_get(struct snd_kcontrol *kcontrol, struct snd_ctl_elem_value *ucontrol)
{
	ucontrol->value.integer.value[0] = g_iampl_act;
	return 0;
}

static int
cx20707_iampl_put(struct snd_kcontrol *kcontrol, struct snd_ctl_elem_value *ucontrol)
{
	INFO("--- ampl mute: %ld ---\n", ucontrol->value.integer.value[0]);
	if (ucontrol->value.integer.value[0] == 0) {
		g_iampl_act = 0;
        INFO("--- ClassD Inactive ---\n");
        return 1;
	} else if (ucontrol->value.integer.value[0] == 1) {
		g_iampl_act = 1;
        INFO("--- ClassD Active ---\n");
        return 1;
	}

	return 0;
}
*/

#define SOC_SINGLE_VALUE(xreg, xshift, xshiftr, xmax, xinvert) \
	((unsigned long)&(struct soc_mixer_control) \
	{.reg = xreg, .shift = xshift, .rshift = xshiftr, .max = xmax, \
	.platform_max = xmax, .invert = xinvert})

/*
#define SOC_INROUTE(xname, xmax) \
{	.iface = SNDRV_CTL_ELEM_IFACE_MIXER, .name = (xname), \
	.info = snd_cx20707_info, \
	.get = cx20707_inroute_get, .put = cx20707_inroute_put, \
	.private_value = SOC_SINGLE_VALUE(0, 0, 0, xmax, 0) }
*/

#define SOC_SINGLE(xname, reg, shift, max, invert) \
{	.iface = SNDRV_CTL_ELEM_IFACE_MIXER, .name = xname, \
	.info = snd_cx20707_info, .get = snd_cx20707_get,\
	.put = snd_cx20707_put, \
	.private_value =  SOC_SINGLE_VALUE(reg, shift, shift, max, invert) }

#define SOC_DOUBLE_R(xname, reg_left, reg_right, xshift, xmax, xinvert) \
{	.iface = SNDRV_CTL_ELEM_IFACE_MIXER, .name = (xname), \
	.info = snd_cx20707_info2, \
	.get = snd_cx20707_get2, .put = snd_cx20707_put2, \
	.private_value = (unsigned long)&(struct soc_mixer_control) \
		{.reg = reg_left, .rreg = reg_right, .shift = xshift, \
		.max = xmax, .platform_max = xmax, .invert = xinvert} }

#define SOC_SINGLE_NEWC(xname, reg, shift, max, invert) \
{	.iface = SNDRV_CTL_ELEM_IFACE_MIXER, .name = xname, \
	.info = snd_cx20707_info, .get = snd_cx20707_get,\
	.put = snd_cx20707_put_newc, \
	.private_value =  SOC_SINGLE_VALUE(reg, shift, shift, max, invert) }

#define SOC_INMUTE(xname, xmax) \
{	.iface = SNDRV_CTL_ELEM_IFACE_MIXER, .name = (xname), \
	.info = snd_cx20707_info, \
	.get = cx20707_inmute_get, .put = cx20707_inmute_put, \
	.private_value = SOC_SINGLE_VALUE(0, 0, 0, xmax, 0) }

#define SOC_OUTMUTE(xname, xmax) \
{	.iface = SNDRV_CTL_ELEM_IFACE_MIXER, .name = (xname), \
	.info = snd_cx20707_info, \
	.get = cx20707_outmute_get, .put = cx20707_outmute_put, \
	.private_value = SOC_SINGLE_VALUE(0, 0, 0, xmax, 0) }
	
#define SOC_AMPLSW(xname, xmax) \
{	.iface = SNDRV_CTL_ELEM_IFACE_MIXER, .name = (xname), \
	.info = snd_cx20707_info, \
	.get = cx20707_ampl_get, .put = cx20707_ampl_put, \
	.private_value = SOC_SINGLE_VALUE(0, 0, 0, xmax, 0) }

/*
#define SOC_JACKPOLICY(xname, xmax) \
{	.iface = SNDRV_CTL_ELEM_IFACE_MIXER, .name = (xname), \
	.info = snd_cx20707_info, \
	.get = cx20707_jackp_get, .put = cx20707_jackp_put, \
	.private_value = SOC_SINGLE_VALUE(0, 0, 0, xmax, 0) }

#define SOC_CLASSDACT(xname, xmax) \
{	.iface = SNDRV_CTL_ELEM_IFACE_MIXER, .name = (xname), \
	.info = snd_cx20707_info, \
	.get = cx20707_iampl_get, .put = cx20707_iampl_put, \
	.private_value = SOC_SINGLE_VALUE(0, 0, 0, xmax, 0) }
*/

static const struct snd_kcontrol_new cx20707_snd_controls[] = {
    SOC_DOUBLE_R("PCM Playback Volume",	DAC1L, DAC2R, 0, 0x4F, 0),
    SOC_DOUBLE_R("Mic Capture Volume", MicADC2GainL, MicADC2GainR, 0, 0x4F, 0),

    SOC_SINGLE_NEWC("ClassD Playback Switch", OutCTL, 2, 1, 0),
    SOC_SINGLE_NEWC("Lineout Playback Switch", OutCTL, 1, 1, 0),

    /* capture route setting:0-mic, 1-linein_diff, 2-linein-se */
//    SOC_INROUTE("Route Capture Switch", INROUTE_LINEIN_SE),

    /* DONE */
    SOC_INMUTE("Mute Capture Switch", 1),
    SOC_OUTMUTE("Mute Playback Switch", 1),
    
    SOC_SINGLE("Mic Capture Boost", MicCTL, 0, 0x07, 0),   
    SOC_SINGLE("Mic AGC Switch", DSPEn1, 4, 0x01, 0),

    SOC_SINGLE("AEC Switch", DSPEn1, 0, 0x01, 0),
    SOC_SINGLE("LEC Switch", DSPEn1, 1, 0x01, 0),  
    SOC_SINGLE("Noise Reduction Switch", DSPEn1, 2, 0x01, 0),
    SOC_SINGLE("Beam Forming Switch", DSPEn1, 3, 0x01, 0),
    SOC_SINGLE("DRC Switch",	DSPEn2, 2, 0x01, 0),

/*
	SOC_JACKPOLICY("JackPolicy Playback Switch", 1),
	SOC_CLASSDACT("IAMPL Playback Switch", 1),
    SOC_AMPLSW("EAMPL Playback Switch", 1),
    SOC_SINGLE("Mic Capture Boost", MicCTL, 0, 0x07, 0),
    SOC_SINGLE("Mic Capture Bias", MicCTL, 3, 0x01, 0),
    SOC_SINGLE("Speaker Playback Gain",	ClassDGain, 0, 0x0f, 0),

    SOC_SINGLE("DRC Switch",	DSPEn2, 2, 0x01, 0),
    SOC_SINGLE("3DEffect Switch",	DSPEn2, 3, 0x01, 0),
    SOC_SINGLE("Loudness Switch",	DSPEn2, 4, 0x01, 0),
    SOC_SINGLE("Speaker Lineout Switch",	OutCTL, 1, 0x02, 0),
    SOC_SINGLE("Inbound Noice Reduction Switch",	DSPEn1, 5, 0x01, 0),
    SOC_SINGLE("Mic AGC Switch", DSPEn1, 4, 0x01, 0),
    SOC_SINGLE("Beam Forming Switch", DSPEn1, 3, 0x01, 0),
    SOC_SINGLE("Noise Reduction Switch", DSPEn1, 2, 0x01, 0),
    SOC_SINGLE("LEC Switch", DSPEn1, 1, 0x01, 0),
    SOC_SINGLE("AEC Switch", DSPEn1, 0, 0x01, 0),

    SOC_SINGLE("SideTone Capture Switch", DSPEn1, 7, 0x01, 0),
    SOC_SINGLE("SideTone Capture GainL", SideToneGainL, 0, 0xff, 0),
    SOC_SINGLE("SideTone Capture GainH", SideToneGainH, 0, 0x7f, 0),

    SOC_SINGLE("LineIn Left Mute Switch", Mute, 6, 0x01, 0),
    SOC_SINGLE("LineIn Right Mute Switch", Mute, 5, 0x01, 0),
    SOC_SINGLE("Mic Left Mute Switch", Mute, 4, 0x01, 0),
    SOC_SINGLE("Mic Right Mute Switch", Mute, 3, 0x01, 0),

#ifdef CX20707_DEBUG
    SOC_LOOPBACK1("DSP Loopback Switch", 1),
#endif
*/
};

static int
cx20707_dev_free(struct snd_device *device)
{
	struct i2c_client *clnt = device->device_data;
	struct i2c_driver *drv;
	drv = clnt->driver;
	i2c_del_driver(drv);
	return 0;
}

static int
snd_cx20707_mixer_new(mx27_codec_t *codec)
{
	static struct snd_device_ops ops = {
		.dev_free = cx20707_dev_free,
	};
	struct snd_card *card = codec->card;
	int err, i;

    if ((err = i2c_add_driver(&cx20707_i2c_driver)) < 0) {
        printk("---- %s: i2c_add_driver() failed ----\n", __func__);
        return err;
    }

    for (i = 0; i < ARRAY_SIZE(cx20707_snd_controls); i++) {
		err = snd_ctl_add(card, snd_ctl_new1(&cx20707_snd_controls[i], codec));
		if (err < 0) {
			i2c_del_driver(&cx20707_i2c_driver);
			return err;
		}
	}

	if ((err = snd_device_new(card, SNDRV_DEV_CODEC, &codec->iic, &ops)) < 0) {
		i2c_del_driver(&cx20707_i2c_driver);
		return err;
	}

	strncpy(card->mixername, "cx20707 Mixer", sizeof(card->mixername));

  return 0;
}

#if 0
}
#endif

//////////////////////////////////////////////
//              mx27 i2s config             //
//////////////////////////////////////////////
#if 0
{
#endif

#define TX_WATERMARK		0x4	/* set tx_fifo watermark */
#define RX_WATERMARK		0x6	/* set rx_fifo watermark */

static atomic_t ssi_ref = ATOMIC_INIT(0);
static struct clk *ssi_clk;

/* DONE */
static void
ssi_clock_enable(int ssi)
{
	if (ssi == SSI1) {
		ssi_clk = clk_get(NULL, "ssi_clk.0");
	} else if (ssi == SSI2) {
		ssi_clk = clk_get(NULL, "ssi_clk.1");
	}

	clk_enable(ssi_clk);
}

/* DONE */
static void
mxc_dam_config(int ssi)
{
	unsigned long reg = 0;
	unsigned long saddr;
	unsigned long daddr;
	unsigned long addr;
	unsigned long prt;

	addr = IO_ADDRESS(AUDMUX_BASE_ADDR);
	daddr = addr + 0x14;
	if (ssi == SSI1) {
		saddr = addr;
		prt = 0;
	} else if (ssi == SSI2) {
		saddr = addr + 0x04;
		prt = 1;
	}

	reg = (1 << 31) |	/* Fs to internal ssi1 */
	    (1 << 30) |		/* clk to internal ssi1 */
	    (4 << 26) |		/* fs/clk <-->external ssi2 */
	    (1 << 25) |		/* sync mode */
	    (1 << 24) | 
	    (4 << 20) | 
	    (4 << 13) |	/* data <--> external ssi2 */
	    (1 << 12);
	__raw_writel(reg, saddr);	/* set the host port1/2(internal ssi1/2) */

	/* set the peripheral port2 */
	reg = (0 << 31) |	/* Fs from the external ssi2 */
	    (0 << 30) |		/* clk from the external ssi2 */
	    (prt << 26) | 
	    (0 << 25) |
	    (0 << 24) | 
	    (prt << 20) | 
	    (prt << 13) |	/* data <-->internal ssi1/2 */
	    (1 << 12);
	__raw_writel(reg, daddr);	/* set the port5(external ssi2) */
}

static void
mxc_ssi_general_config(int ssi)
{
	INFO("--- config SSI%d ---\n", ssi+1);
	ssi_enable(ssi, 0);
	clk_disable(ssi_clk);
	/* Sync mode (SCR[4] =1) */
	ssi_synchronous_mode(ssi, 1);

	/* Tx/Rx shift direction: MSB transmitted first (STCR[4]=0)/(SRCR[4]=0) */
	ssi_tx_shift_direction(ssi, ssi_msb_first);
	/* Tx data clocked at falling edge of the clock (STCR[3]=1) */
	ssi_tx_clock_polarity(ssi, ssi_clock_on_falling_edge);
	/* Tx/Rx frame sync active low (STCR[2]=1)/(SRCR[2]=1) */
	ssi_tx_frame_sync_active(ssi, ssi_frame_sync_active_low);
	/* Tx/Rx frame sync initiated one bit before data is transmitted (STCR[0]=1)/(SRCR[0]=1) */
	ssi_tx_early_frame_sync(ssi, ssi_frame_sync_one_bit_before);
	//WL3-WL0 bit (STCCR[16-13])
	ssi_tx_word_length(ssi, ssi_16_bits);
	ssi_tx_fifo_enable(ssi, ssi_fifo_0, 1);
	ssi_tx_frame_rate(ssi, 2);

	/* Tx/Rx shift direction: MSB transmitted first (STCR[4]=0)/(SRCR[4]=0) */
	ssi_rx_shift_direction(ssi, ssi_msb_first);
	/* Rx data latched at rising edge of the clock (SRCR[3]=1) */
	ssi_rx_clock_polarity(ssi, ssi_clock_on_rising_edge);
	ssi_rx_frame_sync_active(ssi, ssi_frame_sync_active_low);
	ssi_rx_early_frame_sync(ssi, ssi_frame_sync_one_bit_before);
	ssi_rx_word_length(ssi, ssi_16_bits);
	ssi_rx_fifo_enable(ssi, ssi_fifo_0, 1);
	ssi_rx_frame_rate(ssi, 2);

	ssi_tx_clock_direction(ssi, ssi_tx_rx_externally);
	ssi_tx_frame_direction(ssi, ssi_tx_rx_externally);
	ssi_rx_clock_direction(ssi, ssi_tx_rx_externally);
	ssi_rx_frame_direction(ssi, ssi_tx_rx_externally);
	ssi_i2s_mode(ssi, i2s_slave);

	ssi_tx_fifo_empty_watermark(ssi, ssi_fifo_0, TX_WATERMARK);
	ssi_interrupt_enable(ssi, ssi_tx_dma_interrupt_enable);
	ssi_interrupt_disable(ssi, ssi_tx_fifo_0_empty);
	
	ssi_rx_fifo_full_watermark(ssi, ssi_fifo_0, RX_WATERMARK);
	ssi_interrupt_enable(ssi, ssi_rx_dma_interrupt_enable);
	ssi_interrupt_disable(ssi, ssi_rx_fifo_0_full);

	ssi_enable(ssi, 1);
	clk_enable(ssi_clk);
}

static void
mxc_ssi_tx_config(int ssi)
{
//    ssi_interrupt_enable(ssi, ssi_tx_dma_interrupt_enable);  
	ssi_interrupt_enable(ssi, ssi_tx_fifo_0_empty);
	ssi_transmit_enable(ssi, 1);
}

static void
mxc_ssi_rx_config(int ssi)
{
//    ssi_interrupt_enable(ssi, ssi_rx_dma_interrupt_enable);
	ssi_interrupt_enable(ssi, ssi_rx_fifo_0_full);
	ssi_receive_enable(ssi, 1);
}

/* DONE */
static void
mxc_ssi_i2s_uninit(int ssi)
{
	ssi_enable(ssi, 0);
	ssi_receive_enable(ssi, 0);
	ssi_transmit_enable(ssi, 0);
	ssi_interrupt_disable(ssi, ssi_rx_dma_interrupt_enable);
	ssi_interrupt_disable(ssi, ssi_rx_fifo_0_full);
	ssi_interrupt_disable(ssi, ssi_tx_dma_interrupt_enable);
	ssi_interrupt_disable(ssi, ssi_tx_fifo_0_empty);
	ssi_tx_fifo_enable(ssi, ssi_fifo_0, 0);
	ssi_rx_fifo_enable(ssi, ssi_fifo_0, 0);
}

#if 0
}
#endif

//////////////////////////////////////////////
//           driver implementation          //
//////////////////////////////////////////////
#if 0
{
#endif

#define MAX_BUFFER_SIZE  		(32*1024)
#define DMA_BUF_SIZE			(8*1024)
#define MIN_PERIOD_SIZE			64
#define MIN_PERIOD				2
#define MAX_PERIOD				255

static snd_pcm_hardware_t mx27_codec_hw[] = {
	{  
	.info             = ( SNDRV_PCM_INFO_INTERLEAVED
                      | SNDRV_PCM_INFO_BLOCK_TRANSFER
                      | SNDRV_PCM_INFO_MMAP
                      | SNDRV_PCM_INFO_MMAP_VALID ),
	.formats          = CX20707_FORMATS,
	.rates            = CX20707_RATES,
	.rate_min         = 8000,
	.rate_max         = 96000,
	.channels_min     = 1,
	.channels_max     = 2,
	.buffer_bytes_max = MAX_BUFFER_SIZE,
	.period_bytes_min = MIN_PERIOD_SIZE,
	.period_bytes_max = DMA_BUF_SIZE,
	.periods_min      = MIN_PERIOD,
	.periods_max      = MAX_PERIOD,
	.fifo_size        = 0,
	},
	{  
	.info             = ( SNDRV_PCM_INFO_INTERLEAVED   
                      | SNDRV_PCM_INFO_BLOCK_TRANSFER   
                      | SNDRV_PCM_INFO_MMAP  
                      | SNDRV_PCM_INFO_MMAP_VALID ),  
	.formats          = CX20707_FORMATS,
	.rates            = (SNDRV_PCM_RATE_8000 | SNDRV_PCM_RATE_16000),
	.rate_min         = 8000,
	.rate_max         = 16000,
	.channels_min     = 1,
	.channels_max     = 2,
	.buffer_bytes_max = MAX_BUFFER_SIZE,
	.period_bytes_min = MIN_PERIOD_SIZE,
	.period_bytes_max = DMA_BUF_SIZE,
	.periods_min      = MIN_PERIOD,
	.periods_max      = MAX_PERIOD,
	.fifo_size        = 0,
	},
};

static unsigned int playback_rates[] = {
	8000, 11025, 16000, 22050, 24000, 32000, 44100, 48000, 88200, 96000,
};

static unsigned int capture_rates[] = { 8000, 16000, };

static snd_pcm_hw_constraint_list_t hw_constraints_rates[] = {
	{
	.count = ARRAY_SIZE(playback_rates),
	.list = playback_rates,
	.mask = 0,
	},
	{
	.count = ARRAY_SIZE(capture_rates),
	.list = capture_rates,
	.mask = 0,
	}
};

/* DONE */
static unsigned int
adjust_speed(int speed)
{
	if (speed >= (88200 + 96000) / 2) {
		speed = 96000;
	} else if (speed >= (48000 + 88200) / 2) {
		speed = 88200;
	} else if (speed >= (44100 + 48000) / 2) {
		speed = 48000;
	} else if (speed >= (32000 + 44100) / 2) {
		speed = 44100;
	} else if (speed >= (24000 + 32000) / 2) {
		speed = 32000;
	} else if (speed >= (22050 + 24000) / 2) {
		speed = 24000;
	} else if (speed >= (16000 + 22050) / 2) {
		speed = 22050;
	} else if (speed >= (11025 + 16000) / 2) {
		speed = 16000;
	} else if (speed >= (8000 + 11025) / 2) {
		speed = 11025;
	} else {
		speed = 8000;
	}
	return speed;
}

/* DONE */
static int
cx20707_playback_config(mx27_codec_t* codec, snd_pcm_runtime_t *runtime)
{
	int idx;

	idx = clk_divider(runtime->rate);
	if (idx < 0) return -EINVAL;

#ifdef CX20707_DEBUG
	dbg_clk(idx);
#endif
	cx20707_i2s_config(codec, idx);
	cx20707_stream3_config(codec, idx, runtime->channels);
	cx20707_mixer0_from_config(codec, 0, 0);
	cx20707_mixer0_from_config(codec, 1, S3OUT);
	cx20707_mixer0_from_config(codec, 2, 0);
	cx20707_mixer0_from_config(codec, 3, 0);
	cx20707_stream7_config(codec, ScaleOut);

	cx20707_outp_unmuteL(codec);
	cx20707_outp_unmuteR(codec);
	outmute = 0;

	/* AEC初始化配置已经将OutCTL配置为自动 */
//    cx20707_lineout_config(codec, 1);
//    cx20707_headphone_enable(codec);
    mxc_audio_mute(0);

	return 0;
}

/* DONE */
static void
cx20707_playback_quit(mx27_codec_t *codec)
{
/*
	if (codec->s[0].channels == 1) {
		cx20707_stream3_stero(codec);
	} else {
		cx20707_stream3_mono_left(codec);
	}
*/
	cx20707_stream3_mono_right(codec);
	cx20707_headphone_disable(codec);
	cx20707_lineout_disable(codec);
	mxc_audio_mute(1);
//	cx20707_classD_stero(codec);
}

/* DONE */
/* revert: 1-setting order reverted */
static int
cx20707_capture_config(mx27_codec_t* codec, snd_pcm_runtime_t *runtime)
{
    int idx;

	idx = clk_divider(runtime->rate);
	if (idx != 0&&idx != 2) return -EINVAL;
#ifdef CX20707_DEBUG
	dbg_clk(idx);
#endif
	cx20707_i2s_config(codec, idx);

//	if (inroute == INROUTE_MIC) {
//	    cx20707_linein_close(codec);
	    cx20707_micin_open(codec, runtime->channels);
//	} else {
//	    cx20707_micin_close(codec);
//	    cx20707_linein_open(codec, inroute);
//	}
	cx20707_stream5_config(codec, VoiceOut0);

	return 0;
}

/* DONE */
static void
cx20707_capture_quit(mx27_codec_t *codec)
{
    cx20707_stream5_config(codec, 0);
//    cx20707_linein_close(codec);
    cx20707_micin_close(codec);
}

/* DONE */
static void
audio_stop_dma(audio_stream_t *s)
{
	unsigned long flags;
	snd_pcm_substream_t *substream;
	snd_pcm_runtime_t *runtime;
	unsigned int dma_size;
	unsigned int offset;

	substream = s->substream;
	runtime = substream->runtime;
	dma_size = frames_to_bytes(runtime, runtime->period_size);
	offset = dma_size * s->periods;

	spin_lock_irqsave(&s->dma_lock, flags);
	INFO("--- MXC : audio_stop_dma active = 0 ---\n");
	s->active = 0;
	s->period = 0;
	s->periods = 0;

	/* this stops the dma channel and clears the buffer ptrs */
	mxc_dma_disable(s->dma_channel);
	if (s->stream_id == 0)
	    dma_unmap_single(NULL, runtime->dma_addr + offset, dma_size, DMA_TO_DEVICE);
	else
		dma_unmap_single(NULL, runtime->dma_addr + offset, dma_size, DMA_FROM_DEVICE);
	spin_unlock_irqrestore(&s->dma_lock, flags);
}

/*! DONE
  * This function is called whenever a new audio block needs to be 
  * transferred to cx20707. The function receives the address and the size 
  * of the new block and start a new DMA transfer. 
  * 
  * @param  substream   pointer to the structure of the current stream. 
  * 
*/
static void
audio_playback_dma(audio_stream_t *s)
{
	snd_pcm_substream_t *substream;
	snd_pcm_runtime_t *runtime;
	unsigned int dma_size;
	unsigned int offset;
	int ret = 0;
	mxc_dma_requestbuf_t dma_request;

	substream = s->substream;
	runtime = substream->runtime;
	memset(&dma_request, 0, sizeof(mxc_dma_requestbuf_t));

	if (s->active) {
		if (ssi_get_status(s->ssi) & ssi_transmitter_underrun_0) {
			INFO("--- playback underrun ---\n");
			ssi_enable(s->ssi, 0);
			ssi_transmit_enable(s->ssi, 0);
			ssi_enable(s->ssi, 1);
		}

		dma_size = frames_to_bytes(runtime, runtime->period_size);
//		INFO("--- s->period (%x) runtime->periods (%d) ---\n", s->period, runtime->periods);
//		INFO("--- runtime->period_size (%d),dma_bytes (%d) ---\n", (unsigned int)runtime->period_size, runtime->dma_bytes);

		offset = dma_size * s->period;
		snd_assert(dma_size <= DMA_BUF_SIZE,);

		dma_request.src_addr = (dma_addr_t)(dma_map_single(NULL, runtime->dma_area+offset, dma_size, DMA_TO_DEVICE));
		dma_request.dst_addr = s->dma_dev_addr;
//		dma_request.dst_addr = (dma_addr_t)(SSI2_BASE_ADDR + MXC_SSI2STX0);
		dma_request.num_of_bytes = dma_size;

//		INFO("MXC: start DMA offset=%d size=%d\n", offset,runtime->dma_bytes);
//		INFO("dst=%x,src=%x\n", dma_request.dst_addr,dma_request.src_addr);

		mxc_dma_config(s->dma_channel, &dma_request, 1, MXC_DMA_MODE_WRITE);
		ret = mxc_dma_enable(s->dma_channel);
		ssi_transmit_enable(s->ssi, 1);
		if (ret) {
			printk(KERN_ERR "audio_process_dma: cannot queue DMA buffer (%i)/n", ret);
			return;
		}
		s->tx_spin = 1;
		s->period++;
		s->period %= runtime->periods;
	}
}

/*! DONE
  * This function is called whenever a new audio block needs to be 
  * transferred from cx20707. The function receives the address and the size 
  * of the block that will store the audio samples and start a new DMA transfer. 
  * 
  * @param  substream   pointer to the structure of the current stream. 
  * 
*/  
static void
audio_capture_dma(audio_stream_t *s)
{
	snd_pcm_substream_t *substream;
	snd_pcm_runtime_t *runtime;
	unsigned int dma_size;
	unsigned int offset;
	int ret = 0;
	mxc_dma_requestbuf_t dma_request;

	substream = s->substream;
	runtime = substream->runtime;
	memset(&dma_request, 0, sizeof(mxc_dma_requestbuf_t));

	if (s->active) {
		if (ssi_get_status(s->ssi) & ssi_receiver_overrun_0) {
			INFO("--- capature overrun ---\n");
			ssi_enable(s->ssi, 0);
			ssi_receive_enable(s->ssi, 0);
			ssi_enable(s->ssi, 1);
		}

		dma_size = frames_to_bytes(runtime, runtime->period_size);
//		INFO("--- s->period (%x) runtime->periods (%d) ---\r\n", s->period, runtime->periods);
//		INFO("--- runtime->period_size (%d),dma_bytes (%d) ---\r\n", (unsigned int)runtime->period_size, runtime->dma_bytes);

		offset = dma_size * s->period;
		snd_assert(dma_size <= DMA_BUF_SIZE,);
		dma_request.dst_addr = (dma_addr_t)(dma_map_single(NULL, runtime->dma_area + offset, dma_size, DMA_FROM_DEVICE));
		dma_request.src_addr = s->dma_dev_addr;
		dma_request.num_of_bytes = dma_size;

		mxc_dma_config(s->dma_channel, &dma_request, 1, MXC_DMA_MODE_READ);
		ret = mxc_dma_enable(s->dma_channel);
		if (ret) {
			printk(KERN_ERR "audio_process_dma: cannot queue DMA buffer (%i)\n", ret);
			return;
		}
		ssi_receive_enable(s->ssi, 1);
		s->tx_spin = 1;
		s->period++;
		s->period %= runtime->periods;
	}
}

/* DONE */
static void
audio_dma_callback(void *data, int error, unsigned int count)
{
	audio_stream_t *s;
	snd_pcm_substream_t *substream;
	snd_pcm_runtime_t *runtime;
	unsigned int dma_size;
	unsigned int previous_period;
	unsigned int offset;

	s = data;  
	substream = s->substream;  
	runtime = substream->runtime;  
	previous_period = s->periods;
	dma_size = frames_to_bytes(runtime, runtime->period_size);
	offset = dma_size * previous_period;
  
	s->tx_spin = 0;
	s->periods++;
	s->periods %= runtime->periods;
	/*
	 * Give back to the CPU the access to the non cached memory
	 */
	if (s->stream_id == 0)
	    dma_unmap_single(NULL, runtime->dma_addr + offset, dma_size, DMA_TO_DEVICE);
	else
		dma_unmap_single(NULL, runtime->dma_addr + offset, dma_size, DMA_FROM_DEVICE);

	/* 
   * If we are getting a callback for an active stream then we inform 
   * the PCM middle layer we've finished a period 
   */  
	if (s->active)
		snd_pcm_period_elapsed(s->substream);

	spin_lock(&s->dma_lock);
    s->dma_func(s);
	spin_unlock(&s->dma_lock);
}

/* DONE */
static int
configure_io_channel(audio_stream_t *s, mxc_dma_callback_t callback)
{
	int channel = -1;
	int ret;
	char *dma_name[] = {"ALSA TX DMA", "ALSA RX DMA"};

	mutex_lock(&codec_mutex);
	channel = mxc_dma_request(s->dma_dev, dma_name[s->stream_id]);
	if (channel < 0) {
		INFO(" --- failed to request a read dma channel --- \n");
		mutex_unlock(&codec_mutex);
		return -1;
	}

	ret = mxc_dma_callback_set(channel, callback, (void *)s);
	if (ret != 0) {
		mxc_dma_free(channel);
		INFO(" --- mxc_dma_callback_set failed --- \n");
		mutex_unlock(&codec_mutex);
		return -1;
	}
	s->dma_channel = channel;
	mutex_unlock(&codec_mutex);
	return 0;
}

/* DONE */
static int
mx27_codec_open(snd_pcm_substream_t *substream)  
{  
	mx27_codec_t *codec = snd_pcm_substream_chip(substream);
	snd_pcm_runtime_t  *runtime = substream->runtime;
	audio_stream_t *s;
	int stream_id = substream->pstr->stream;
	int err;

	if (stream_id > SNDRV_PCM_STREAM_CAPTURE) return -ENODEV;
	s = &codec->s[stream_id];
	if (!atomic_dec_and_test(&s->ref)) {
		atomic_inc(&s->ref);
		printk(KERN_ALERT "pcm%d device busy\n", stream_id);
		return -EBUSY; /* already open */
	}

	s->substream = substream;
	runtime->hw = mx27_codec_hw[stream_id];

	err = snd_pcm_hw_constraint_integer(runtime, SNDRV_PCM_HW_PARAM_PERIODS);
	if (err < 0) {
		atomic_inc(&s->ref);
		return err;
	}

	err = snd_pcm_hw_constraint_list(runtime, 0, SNDRV_PCM_HW_PARAM_RATE, &hw_constraints_rates[stream_id]);
	if (err < 0) {
		atomic_inc(&s->ref);
		return err;
	}

	msleep(50);
	err = configure_io_channel(s, audio_dma_callback);
	if (err < 0) {
		atomic_inc(&s->ref);
		return err;
	}

	if (atomic_inc_return(&ssi_ref) == 1) {
		printk(KERN_ALERT "--- mxc_ssi_general_config ---\n");
		mxc_ssi_general_config(INTERN_SSI);
	}

	return 0;
}

/* DONE */
static int
mx27_codec_close(snd_pcm_substream_t *substream)  
{
	mx27_codec_t* codec = snd_pcm_substream_chip(substream);
	int stream_id = substream->pstr->stream;
	audio_stream_t *s;

	mutex_lock(&codec_mutex);
	s = &codec->s[stream_id];
	mxc_dma_free(s->dma_channel);
	if (stream_id == SNDRV_PCM_STREAM_PLAYBACK) {
		ssi_interrupt_disable(s->ssi, ssi_tx_dma_interrupt_enable);  
		ssi_interrupt_disable(s->ssi, ssi_tx_fifo_0_empty);
		ssi_transmit_enable(s->ssi, 0);
		cx20707_playback_quit(codec);
		printk(KERN_ALERT "--- close playback ---\n");
	} else if (stream_id == SNDRV_PCM_STREAM_CAPTURE) {
		ssi_interrupt_disable(s->ssi, ssi_rx_dma_interrupt_enable);
		ssi_interrupt_disable(s->ssi, ssi_rx_fifo_0_full);
		ssi_receive_enable(s->ssi, 0);
		cx20707_capture_quit(codec);

		printk(KERN_ALERT "--- close capture ---\n");
	}

	s->substream = NULL;
	if (atomic_dec_return(&ssi_ref) == 0) {
//		cx20707_real_write(codec, CLOCK_DIVIDER, 0xff);
		rate_idx = -1;
//		cx20707_reset(codec);
	}
	atomic_inc(&s->ref);
	mutex_unlock(&codec_mutex);

	return 0;  
}

/* DONE */
static int
mx27_codec_hw_free(snd_pcm_substream_t* substream)
{
	return snd_pcm_lib_free_pages(substream);
}

/* DONE */
static int
mx27_codec_hw_params(snd_pcm_substream_t* substream, snd_pcm_hw_params_t* hw_params)
{
	snd_pcm_runtime_t *runtime;
	int ret;

	runtime = substream->runtime;
	ret = snd_pcm_lib_malloc_pages(substream, params_buffer_bytes(hw_params));
	if (ret < 0) return ret;

	runtime->dma_addr = virt_to_phys(runtime->dma_area);

	INFO(" --- %s: dma_area=%x ",__FUNCTION__, (int)substream->runtime->dma_area);
	INFO("dma_addr=%x,dma_bytes=%d --- \n", substream->runtime->dma_addr, substream->runtime->dma_bytes);

    return (ret);
}

/* DONE */
static int
mx27_codec_prepare(snd_pcm_substream_t* substream)
{
	mx27_codec_t *codec = snd_pcm_substream_chip(substream);
	snd_pcm_runtime_t *runtime = substream->runtime;
	audio_stream_t  *s;
	int id = substream->pstr->stream;

	s = &codec->s[id];
	s->period = 0;
	s->periods = 0;

    printk(KERN_ALERT "--- before adjusting: rate = %d, channels = %d ---\n", runtime->rate, runtime->channels);
	runtime->rate = adjust_speed(runtime->rate);

	mutex_lock(&codec_mutex);
	cx20707_dsp_mode(codec);
	
	/* AEC初始化配置已经将OutCTL配置为自动 */
//	cx20707_outctl_manual(codec);

	if (id == SNDRV_PCM_STREAM_PLAYBACK) {
		printk(KERN_ALERT "--- after adjusting: prepare playback, rate = %d, channels = %d ---\n", runtime->rate, runtime->channels);
		mxc_ssi_tx_config(s->ssi);
		cx20707_playback_config(codec, runtime);
	} else if (id == SNDRV_PCM_STREAM_CAPTURE) {
		printk(KERN_ALERT "--- after adjusting: prepare capture, rate = %d, channels = %d ---\n", runtime->rate, runtime->channels);
		mxc_ssi_rx_config(s->ssi);
		cx20707_capture_config(codec, runtime);
	}
	mutex_unlock(&codec_mutex);

	return 0;
}

/* DONE */
static int
mx27_codec_trigger(snd_pcm_substream_t* substream, int cmd)
{
	mx27_codec_t* codec;
	audio_stream_t* s;
    int result = 0;

	codec = snd_pcm_substream_chip(substream);
	s = &codec->s[substream->pstr->stream];
	printk(KERN_ALERT "--- %s: trigger id = %d, cmd = %d ---\n", __FUNCTION__, substream->pstr->stream, cmd);
	switch (cmd) {
	case SNDRV_PCM_TRIGGER_START:
		printk(KERN_ALERT "--- trigger start stream %d ---\n", substream->pstr->stream);
		s->tx_spin = 0;
		s->active = 1;
		s->dma_func(s);
		break;
	case SNDRV_PCM_TRIGGER_STOP:
		printk(KERN_ALERT "---- trigger stop stream %d ---\n", substream->pstr->stream);
		s->tx_spin = 0;
		s->active = 0;
		audio_stop_dma(s);
		break;
	case SNDRV_PCM_TRIGGER_SUSPEND:
		break;
	case SNDRV_PCM_TRIGGER_RESUME:
		break;
	case SNDRV_PCM_TRIGGER_PAUSE_PUSH:
		break;
	case SNDRV_PCM_TRIGGER_PAUSE_RELEASE:
		break;
	default:
		result = -EINVAL;
		break;
	}
	return result;
}

/* DONE */
static snd_pcm_uframes_t
mx27_codec_pointer(snd_pcm_substream_t *substream)
{
	mx27_codec_t *codec;
	audio_stream_t *s;
	snd_pcm_runtime_t *runtime;
	unsigned int offset;

	codec = snd_pcm_substream_chip(substream);
	runtime = substream->runtime;
//	offset = 0;
	s = &codec->s[substream->pstr->stream];
	
	offset = runtime->period_size * (s->periods);
	if (offset >= runtime->buffer_size)
		offset = 0;
 
 #if 0
  /* tx_spin value is used here to check if a transfert is active */  
	if (s->tx_spin) {
		offset = runtime->period_size * (s->periods) + 0;
		if (offset >= runtime->buffer_size)
			offset = 0;
//		INFO("--- MXC: audio_get_dma_pos offset: %d ---\n", offset);
	} else {
		offset = (runtime->period_size * (s->periods));
		if (offset >= runtime->buffer_size)
			offset = 0;
//		INFO("--- MXC: audio_get_dma_pos BIs offset: %d ---\n", offset);
	}
#endif

	return offset;
}

//////////////////////////////////////////////////////////
static snd_pcm_ops_t mx27_codec_playback_ops = {
	.open      = mx27_codec_open,
	.close     = mx27_codec_close,
	.ioctl     = snd_pcm_lib_ioctl,
	.hw_params = mx27_codec_hw_params,
	.hw_free   = mx27_codec_hw_free,
	.prepare   = mx27_codec_prepare,
	.trigger   = mx27_codec_trigger,
	.pointer   = mx27_codec_pointer,
};

static snd_pcm_ops_t mx27_codec_capture_ops = {
	.open      = mx27_codec_open,
	.close     = mx27_codec_close,
	.ioctl     = snd_pcm_lib_ioctl,
	.hw_params = mx27_codec_hw_params,
	.hw_free   = mx27_codec_hw_free,
	.prepare   = mx27_codec_prepare, 
	.trigger   = mx27_codec_trigger,  
	.pointer   = mx27_codec_pointer,
};

/* DONE */
static void
audio_stream_init(audio_stream_t *s, int ssi)
{
	spin_lock_init(&s[SNDRV_PCM_STREAM_PLAYBACK].dma_lock);
	spin_lock_init(&s[SNDRV_PCM_STREAM_CAPTURE].dma_lock);

	atomic_set(&s[SNDRV_PCM_STREAM_PLAYBACK].ref, 1);
	atomic_set(&s[SNDRV_PCM_STREAM_CAPTURE].ref, 1);

	s[SNDRV_PCM_STREAM_PLAYBACK].ssi = ssi;
	s[SNDRV_PCM_STREAM_CAPTURE].ssi = ssi;

	if (ssi == SSI1) {
		s[SNDRV_PCM_STREAM_PLAYBACK].dma_dev = MXC_DMA_SSI1_16BIT_TX0;
		s[SNDRV_PCM_STREAM_CAPTURE].dma_dev = MXC_DMA_SSI1_16BIT_RX0;
		s[SNDRV_PCM_STREAM_PLAYBACK].dma_dev_addr = (dma_addr_t)(SSI1_BASE_ADDR + MXC_SSISTX0);
		s[SNDRV_PCM_STREAM_CAPTURE].dma_dev_addr = (dma_addr_t)(SSI1_BASE_ADDR + MXC_SSISRX0);
	} else if (ssi == SSI2) {
		s[SNDRV_PCM_STREAM_PLAYBACK].dma_dev = MXC_DMA_SSI2_16BIT_TX0;
		s[SNDRV_PCM_STREAM_CAPTURE].dma_dev = MXC_DMA_SSI2_16BIT_RX0;
		s[SNDRV_PCM_STREAM_PLAYBACK].dma_dev_addr = (dma_addr_t)(SSI2_BASE_ADDR + MXC_SSISTX0);
		s[SNDRV_PCM_STREAM_CAPTURE].dma_dev_addr = (dma_addr_t)(SSI2_BASE_ADDR + MXC_SSISRX0);
	}

	s[SNDRV_PCM_STREAM_PLAYBACK].dma_func = audio_playback_dma;
	s[SNDRV_PCM_STREAM_CAPTURE].dma_func = audio_capture_dma;

	s[SNDRV_PCM_STREAM_PLAYBACK].id = "CX20707 Out";
    s[SNDRV_PCM_STREAM_PLAYBACK].stream_id = SNDRV_PCM_STREAM_PLAYBACK;

	s[SNDRV_PCM_STREAM_CAPTURE].id = "CX20707 In";
	s[SNDRV_PCM_STREAM_CAPTURE].stream_id = SNDRV_PCM_STREAM_CAPTURE;
}

/* DONE */
static int
snd_cx20707_pcm_new(mx27_codec_t* codec)  
{  
	snd_pcm_t* pcm;
	int result = 0;

	/* Create a new PCM instance with 1 capture stream and 1 playback substream */
	result = snd_pcm_new (codec->card, "mx27 PCM", 0, 1, 1, &pcm);
	if (result) goto done;

	snd_pcm_lib_preallocate_pages_for_all(pcm, SNDRV_DMA_TYPE_CONTINUOUS,
	                   snd_dma_continuous_data(GFP_KERNEL), MAX_BUFFER_SIZE*2, MAX_BUFFER_SIZE*2);

    snd_pcm_set_ops(pcm, SNDRV_PCM_STREAM_PLAYBACK, &mx27_codec_playback_ops);  
	snd_pcm_set_ops(pcm, SNDRV_PCM_STREAM_CAPTURE, &mx27_codec_capture_ops);

	pcm->private_data = codec;
	pcm->info_flags = 0;
	strncpy(pcm->name, "mx27 PCM", sizeof(pcm->name));
	audio_stream_init(codec->s, INTERN_SSI);
	codec->pcm = pcm;

 done:
	return result;
}

/* DONE */
static void
snd_cx20707_free(struct snd_card *card)
{
//	mx27_codec_t *chip = card->private_data;
	mxc_ssi_i2s_uninit(INTERN_SSI);
	cx_chip = NULL;
}

/* DONE */
static int __init
mxc_cx20707_probe(struct platform_device *devptr)
{
	int err;
	struct snd_card *card;
	mx27_codec_t *chip;

	card = snd_card_new(-1, CX20707_ID, THIS_MODULE, sizeof(mx27_codec_t));
	if (card == NULL)
		return -ENOMEM;

	INFO("--- snd_card_new() success ---\n" );

	cx_chip = chip = card->private_data;
	chip->card = card;
	card->private_free = snd_cx20707_free;

	if ((err = snd_cx20707_mixer_new(chip)))
		goto nodev;

	INFO("--- snd_cx20707_mixer_new() success ---\n" );

	if ((err = snd_cx20707_pcm_new(chip)) < 0)
		goto nodev;
       
	strncpy(card->driver, "mxc CX20707", sizeof(card->driver));
	strncpy(card->shortname, "cx20707", sizeof(card->shortname));
	strncpy(card->longname, "MX27 with CX20707", sizeof(card->longname));
   
	snd_card_set_dev(card, &devptr->dev);
	INFO("--- snd_cx20707_pcm_new() success ---\n" );

	if ((err = snd_card_register(card)) == 0) {
		INFO("mxc cx20707 support initialized\n" );
		cx20707_init(chip);
		platform_set_drvdata(devptr, card);
		return 0;
	}

 nodev:
 	INFO("mxc cx20707 support failed\n" );
	snd_card_free(card);
	return err;
}

/* DONE */
static int
mxc_cx20707_remove(struct platform_device *devptr)
{
	snd_card_free(platform_get_drvdata(devptr));
	platform_set_drvdata(devptr, NULL);
	return 0;
}

/* DONE */
static struct platform_driver cx20707_driver = {
  .probe = mxc_cx20707_probe,
  .remove = __devexit_p(mxc_cx20707_remove),
  .driver = {
    .name = CX20707_DRIVER,
    .owner = THIS_MODULE,
  },
};

#ifdef CX20707_DEBUG
extern void mxc_dump_clocks(void);
#endif

/* external amplifier mute control */
static void
gpio_audio_mute_active(void)
{
    gpio_request_mux(MX27_PIN_CSPI1_SS1, GPIO_MUX_GPIO);
    mxc_set_gpio_direction(MX27_PIN_CSPI1_SS1, 0);
//    gpio_set_puen(MX27_PIN_CSPI1_SS1, 0);
}

/* DONE */
static int __init
mxc_cx20707_init(void)
{
    int err;

	/* new add 2012.11.30 */
	gpio_request_mux(MX27_PIN_USBH1_RCV, GPIO_MUX_GPIO);
    mxc_set_gpio_direction(MX27_PIN_USBH1_RCV, 0);

	cx20707_hardware_reset();

	gpio_ssi_active(SSI2);
	gpio_audio_mute_active();
	ssi_clock_enable(INTERN_SSI);
	mxc_dam_config(INTERN_SSI);
	if ((err = platform_driver_register(&cx20707_driver)) < 0)
		return err;

	cx20707_device = platform_device_register_simple(CX20707_DRIVER, -1, NULL, 0);	
	if (!IS_ERR(cx20707_device)) {
		if (platform_get_drvdata(cx20707_device)) {
			INFO("---- sound platform driver and device registered ----\n");
			return 0;
		}
		platform_device_unregister(cx20707_device);
		err = -ENODEV;
	} else {
		err = PTR_ERR(cx20707_device);
	}

	platform_driver_unregister(&cx20707_driver);
	INFO("---- sound platform driver and device register failed ----\n");
	return err;
}

/* DONE */
static void __exit
mxc_cx20707_exit(void)
{
    platform_device_unregister(cx20707_device);
    platform_driver_unregister(&cx20707_driver);

	ssi_enable(INTERN_SSI, 0);
	clk_disable(ssi_clk);
	clk_put(ssi_clk);
	gpio_ssi_inactive(SSI2);
	gpio_free_mux(MX27_PIN_USBH1_RCV);
}

module_init(mxc_cx20707_init);
module_exit(mxc_cx20707_exit);
MODULE_LICENSE("GPL");

//EXPORT_SYMBOL(mxc_jack_plugged);
//EXPORT_SYMBOL(mxc_jack_unplugged);

#if 0
}
#endif

