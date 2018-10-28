/*
 * Copyright (C) ST-Ericsson SA 2010
 *
 * License Terms: GNU General Public License, version 2
 * Author: Rabin Vincent <rabin.vincent@stericsson.com> for ST-Ericsson
 */

#include <linux/module.h>
#include <linux/slab.h>
#include <linux/input.h>
#include <linux/interrupt.h>
#include <linux/platform_device.h>
#include <linux/input/matrix_keypad.h>
#include <linux/mfd/stmpe.h>
#include <linux/delay.h>
#include <linux/regulator/consumer.h>
#include <linux/wakelock.h>

#if defined(CONFIG_DRV_SAMSUNG)
#include <linux/input/sec_cmd.h>
#endif

#define STMPE1801_KEY_FIFO_LENGTH		10

/* These are at the same addresses in all STMPE variants */
#define STMPE_ICR_LSB					0x04
#define STMPE_INT_EN_MASK                         0x06
#define STMPE_KPC_ROW			0x30
#define STMPE_KPC_COL_LSB		0x31
#define STMPE_KPC_COL_MSB		0x32
#define STMPE_KPC_CTRL_LSB		0x33
#define STMPE_KPC_CTRL_MSB		0x34
#define STMPE_KPC_CTRL_HSB      0x35
#define STMPE_KPC_CTRL_CMD			0x36
#define STMPE_KPC_COMBI_KEY_0		0x37
#define STMPE_KPC_COMBI_KEY_1		0x38
#define STMPE_KPC_COMBI_KEY_2		0x39
#define STMPE_KPC_DATA_BYTE0		0x3a
#define STMPE_KPC_DATA_BYTE1		0x3b
#define STMPE_KPC_DATA_BYTE2		0x3c
#define STMPE_KPC_DATA_BYTE3		0x3d
#define STMPE_KPC_DATA_BYTE4		0x3e

#define STMPE_KPC_CTRL_LSB_SCAN		(0x1 << 0)
#define STMPE_KPC_CTRL_LSB_DEBOUNCE	(0x7f << 1)
#define STMPE_KPC_CTRL_MSB_SCAN_COUNT	(0xf << 4)

#define STMPE_KPC_ROW_MSB_ROWS		0xff

#define STMPE_KPC_DATA_UP		(0x1 << 7)
#define STMPE_KPC_DATA_ROW		(0xf << 3)
#define STMPE_KPC_DATA_COL		(0x7 << 0)
#define STMPE_KPC_DATA_NOKEY_MASK	0x78

#define STMPE_KEYPAD_MAX_DEBOUNCE	127
#define STMPE_KEYPAD_MAX_SCAN_COUNT	15

#define STMPE_KEYPAD_MAX_ROWS		8
#define STMPE_KEYPAD_MAX_COLS		8
#define STMPE_KEYPAD_ROW_SHIFT		3
#define STMPE_KEYPAD_KEYMAP_MAX_SIZE \
	(STMPE_KEYPAD_MAX_ROWS * STMPE_KEYPAD_MAX_COLS)

/* STMPE1801 KEYPAD variants */
#define STMPE1801_KPC_DATA_UP			(0x80)
#define STMPE1801_KPC_DATA_COL			(0x78)
#define STMPE1801_KPC_DATA_ROW			(0x07)
#define STMPE1801_KPC_DATA_NOKEY		(0x0f)

#if defined(CONFIG_DRV_SAMSUNG)
#else
extern struct class *sec_class;
#endif

#define SEC_CLASS_DEVT_KPDLED        13

/**
 * struct stmpe_keypad_variant - model-specific attributes
 * @auto_increment: whether the KPC_DATA_BYTE register address
 *		    auto-increments on multiple read
 * @set_pullup: whether the pins need to have their pull-ups set
 * @num_data: number of data bytes
 * @num_normal_data: number of normal keys' data bytes
 * @max_cols: maximum number of columns supported
 * @max_rows: maximum number of rows supported
 * @col_gpios: bitmask of gpios which can be used for columns
 * @row_gpios: bitmask of gpios which can be used for rows
 */
struct stmpe_keypad_variant {
	bool		auto_increment;
	bool		set_pullup;
	int		num_data;
	int		num_normal_data;
	int		max_cols;
	int		max_rows;
	unsigned int	col_gpios;
	unsigned int	row_gpios;
};

union stmpe1801_kpc_data {
	uint64_t	value;
	uint8_t		array[8];
};

static const struct stmpe_keypad_variant stmpe_keypad_variants[] = {
	[STMPE1601] = {
		.auto_increment		= true,
		.num_data		= 5,
		.num_normal_data	= 3,
		.max_cols		= 8,
		.max_rows		= 8,
		.col_gpios		= 0x000ff,	/* GPIO 0 - 7 */
		.row_gpios		= 0x0ff00,	/* GPIO 8 - 15 */
	},
	[STMPE1801] = {
		.auto_increment		= true,
		.num_data		= 5,
		.num_normal_data	= 3,
		.max_cols		= 10,
		.max_rows		= 8,
		.col_gpios		= 0x0001f,	/* GPIO 0 - 4 */
		.row_gpios		= 0x01f00,	/* GPIO 8 - 12 */
	},
	[STMPE2401] = {
		.auto_increment		= false,
		.set_pullup		= true,
		.num_data		= 3,
		.num_normal_data	= 2,
		.max_cols		= 8,
		.max_rows		= 12,
		.col_gpios		= 0x0000ff,	/* GPIO 0 - 7*/
		.row_gpios		= 0x1f7f00,	/* GPIO 8-14, 16-20 */
	},
	[STMPE2403] = {
		.auto_increment		= true,
		.set_pullup		= true,
		.num_data		= 5,
		.num_normal_data	= 3,
		.max_cols		= 8,
		.max_rows		= 12,
		.col_gpios		= 0x0000ff,	/* GPIO 0 - 7*/
		.row_gpios		= 0x1fef00,	/* GPIO 8-14, 16-20 */
	},
};

/**
 * struct stmpe_keypad - STMPE keypad state container
 * @stmpe: pointer to parent STMPE device
 * @input: spawned input device
 * @variant: STMPE variant
 * @debounce_ms: debounce interval, in ms.  Maximum is
 *		 %STMPE_KEYPAD_MAX_DEBOUNCE.
 * @scan_count: number of key scanning cycles to confirm key data.
 *		Maximum is %STMPE_KEYPAD_MAX_SCAN_COUNT.
 * @no_autorepeat: disable key autorepeat
 * @rows: bitmask for the rows
 * @cols: bitmask for the columns
 * @keymap: the keymap
 */
struct stmpe_keypad {
	struct stmpe *stmpe;
	struct input_dev *input;
	const struct stmpe_keypad_variant *variant;
	unsigned int debounce_ms;
	unsigned int scan_count;
	bool no_autorepeat;
	unsigned int rows;
	unsigned int cols;
	unsigned short keymap[STMPE_KEYPAD_KEYMAP_MAX_SIZE];
	struct matrix_keymap_data *keymap_data;
	int	key_state[5];	
	struct device	 *sec_keypad;
	struct mutex				dev_lock;
	struct wake_lock			stmpe_wake_lock;
};

static irqreturn_t stmpe_keypad_irq(int irq, void *dev)
{
	struct stmpe_keypad *keypad = dev;
	union stmpe1801_kpc_data key_values;
	int ret;
	int i,j;
	int data, col, row, code;
	bool up;

	mutex_lock(&keypad->dev_lock);
	wake_lock(&keypad->stmpe_wake_lock);

	// disable chip global int
	ret = stmpe_reg_write(keypad->stmpe, 0x04,  0);
		
	// get int src
	ret = stmpe_reg_read(keypad->stmpe, 0x08);
	if (ret < 0) goto out_proc;
	pr_info("[STMKEY]:%s get int src: %d\n", __func__, ret);
	if((ret & (0x06)) > 0) { // INT
		for (i = 0; i < STMPE1801_KEY_FIFO_LENGTH; i++) {
			ret = stmpe_block_read(keypad->stmpe, STMPE_KPC_DATA_BYTE0, 5, key_values.array);
			pr_info("[STMKEY]: %s data byte0: 0x%llx(%x,%x,%x,%x,%x,%x,%x,%x)\n", __func__, key_values.value, key_values.array[0], key_values.array[1], key_values.array[2], key_values.array[3], key_values.array[4],
					key_values.array[5], key_values.array[6], key_values.array[7]);		
			if (ret > 0) {
				if (key_values.value != 0xffff8f8f8LL) {
					for (j = 0; j < 3; j++) {
						data = key_values.array[j];
						col = (data & STMPE1801_KPC_DATA_COL) >> 3;
						row = data & STMPE1801_KPC_DATA_ROW;
						code = MATRIX_SCAN_CODE(row, col, STMPE_KEYPAD_ROW_SHIFT);
						up = data & STMPE_KPC_DATA_UP ? 1 : 0;

						if (col == STMPE1801_KPC_DATA_NOKEY)
							continue;

						if (up) {	/* Release */
							keypad->key_state[row] &= ~(1 << col);
						} else {	/* Press */
							keypad->key_state[row] |= (1 << col);
						}
#if !defined(CONFIG_SAMSUNG_PRODUCT_SHIP)
						pr_info("[STMKEY]: %s code(0x%X|%d) R:C(%d:%d)\n",
								!up ? "P" : "R", keypad->keymap[code], keypad->keymap[code], row, col);
#else
						pr_info("[STMKEY]:%s (%d:%d)\n",
								!up ? "P" : "R", row, col);
#endif
						input_event(keypad->input, EV_MSC, MSC_SCAN, code);
						input_report_key(keypad->input, keypad->keymap[code], !up);
						input_sync(keypad->input);
					}
				}
			}
		}
	}
out_proc:
	// enable chip global int
	stmpe_reg_write(keypad->stmpe, 0x04,  1);

	wake_unlock(&keypad->stmpe_wake_lock);
	mutex_unlock(&keypad->dev_lock);

	return IRQ_HANDLED;
}

static int stmpe_keypad_altfunc_init(struct stmpe_keypad *keypad)
{
	const struct stmpe_keypad_variant *variant = keypad->variant;
	unsigned int col_gpios = variant->col_gpios;
	unsigned int row_gpios = variant->row_gpios;
	struct stmpe *stmpe = keypad->stmpe;
	u8 pureg = stmpe->regs[STMPE_IDX_GPPUR_LSB];
	unsigned int pins = 0;
	unsigned int pu_pins = 0;
	int ret;
	int i;

	/*
	 * Figure out which pins need to be set to the keypad alternate
	 * function.
	 *
	 * {cols,rows}_gpios are bitmasks of which pins on the chip can be used
	 * for the keypad.
	 *
	 * keypad->{cols,rows} are a bitmask of which pins (of the ones useable
	 * for the keypad) are used on the board.
	 */

	for (i = 0; i < variant->max_cols; i++) {
		int num = __ffs(col_gpios);

		if (keypad->cols & (1 << i)) {
			pins |= 1 << num;
			pu_pins |= 1 << num;
		}

		col_gpios &= ~(1 << num);
	}

	for (i = 0; i < variant->max_rows; i++) {
		int num = __ffs(row_gpios);

		if (keypad->rows & (1 << i))
			pins |= 1 << num;

		row_gpios &= ~(1 << num);
	}

	ret = stmpe_set_altfunc(stmpe, pins, STMPE_BLOCK_KEYPAD);
	if (ret)
		return ret;

	/*
	 * On STMPE24xx, set pin bias to pull-up on all keypad input
	 * pins (columns), this incidentally happen to be maximum 8 pins
	 * and placed at GPIO0-7 so only the LSB of the pull up register
	 * ever needs to be written.
	 */
	if (variant->set_pullup) {
		u8 val;

		ret = stmpe_reg_read(stmpe, pureg);
		if (ret)
			return ret;

		/* Do not touch unused pins, may be used for GPIO */
		val = ret & ~pu_pins;
		val |= pu_pins;

		ret = stmpe_reg_write(stmpe, pureg, val);
	}

	return 0;
}

static int stmpe_keypad_chip_init(struct stmpe_keypad *keypad)
{
	const struct stmpe_keypad_variant *variant = keypad->variant;
	struct stmpe *stmpe = keypad->stmpe;
	int ret;

	if (keypad->debounce_ms > STMPE_KEYPAD_MAX_DEBOUNCE)
		return -EINVAL;

	if (keypad->scan_count > STMPE_KEYPAD_MAX_SCAN_COUNT)
		return -EINVAL;

	ret = stmpe_enable(stmpe, STMPE_BLOCK_KEYPAD);
	if (ret < 0)
		return ret;

	ret = stmpe_keypad_altfunc_init(keypad);
	if (ret < 0)
		return ret;

	ret = stmpe_reg_write(stmpe, STMPE_KPC_ROW, keypad->cols);
	if (ret < 0)
		return ret;

	ret = stmpe_reg_write(stmpe, STMPE_KPC_COL_LSB, keypad->rows);
	if (ret < 0)
		return ret;

	if (variant->max_rows > 8) {
		ret = stmpe_set_bits(stmpe, STMPE_KPC_COL_MSB,
				     STMPE_KPC_ROW_MSB_ROWS,
				     keypad->rows >> 8);
		if (ret < 0)
			return ret;
	}

	ret = stmpe_set_bits(stmpe, STMPE_KPC_CTRL_LSB,
			     STMPE_KPC_CTRL_MSB_SCAN_COUNT,
			     keypad->scan_count << 4);
	if (ret < 0)
		return ret;

	ret = stmpe_set_bits(stmpe, STMPE_KPC_CTRL_MSB,
			      STMPE_KPC_CTRL_LSB_SCAN |
			      STMPE_KPC_CTRL_LSB_DEBOUNCE,
			      STMPE_KPC_CTRL_LSB_SCAN |
			      (keypad->debounce_ms << 1));
	if (ret < 0)
		return ret;
			      
	//0001 0110 -- enable combination key interrupt; key fifo overflow interrupt; key interrupt;
	ret = stmpe_reg_write(stmpe, STMPE_INT_EN_MASK,  0x16);
	if (ret < 0)
		return ret;
	mdelay(20);

	//0000 0001 -- level interrupt, Low active; allow interruption to host;	
	ret = stmpe_reg_write(stmpe, STMPE_ICR_LSB,  0x01);
		if (ret < 0)
			return ret;
	mdelay(20);
	
	//0000 0001-- no stop mode, start to scan;
	ret = stmpe_reg_write(stmpe, STMPE_KPC_CTRL_CMD, 0x01);
	if (ret < 0)
		return ret;
	mdelay(20);

	return ret;		 
}

static void stmpe_keypad_fill_used_pins(struct stmpe_keypad *keypad,
					u32 used_rows, u32 used_cols)
{
	int row, col;

	for (row = 0; row < used_rows; row++) {
		for (col = 0; col < used_cols; col++) {
			int code = MATRIX_SCAN_CODE(row, col,
						    STMPE_KEYPAD_ROW_SHIFT);
			if (keypad->keymap[code] != KEY_RESERVED) {
				keypad->rows |= 1 << row;
				keypad->cols |= 1 << col;
			}
		}
	}
}

static void sec_keypad_dump_reg(struct stmpe_keypad *keypad)
{
	union stmpe1801_kpc_data key_values;
	int ret;
	int i;

	// get global int status
	ret = stmpe_reg_read(keypad->stmpe, 0x04);
	if (ret < 0) {
		pr_err("[STMKEY]:%s read I2C fail %d\n", __func__, ret);
		return;
	}
	pr_info("[STMKEY]:%s get global int status: %d\n", __func__, ret);

	// get int src
	ret = stmpe_reg_read(keypad->stmpe, 0x08);
	if (ret < 0) {
		pr_err("[STMKEY]:%s read I2C fail %d\n", __func__, ret);
		return;
	}
	pr_info("[STMKEY]:%s get int src: %d\n", __func__, ret);

	for (i = 0; i < STMPE1801_KEY_FIFO_LENGTH; i++) {
		ret = stmpe_block_read(keypad->stmpe, STMPE_KPC_DATA_BYTE0, 5, key_values.array);
		pr_info("[STMKEY]: %s data byte0: 0x%llx(%x,%x,%x,%x,%x,%x,%x,%x)\n", __func__, key_values.value, key_values.array[0], key_values.array[1], key_values.array[2], key_values.array[3], key_values.array[4],
				key_values.array[5], key_values.array[6], key_values.array[7]);		
	}

	ret = stmpe_reg_read(keypad->stmpe, STMPE_KPC_CTRL_LSB);
	if (ret < 0) {
		pr_err("[STMKEY]:%s read I2C fail %d\n", __func__, ret);
		return;
	}
	pr_info("[STMKEY]:%s get ctrl lsb: %d\n", __func__, ret);
	msleep(5);

	ret = stmpe_reg_read(keypad->stmpe, STMPE_KPC_CTRL_MSB);
	if (ret < 0) {
		pr_err("[STMKEY]:%s read I2C fail %d\n", __func__, ret);
		return;
	}
	pr_info("[STMKEY]:%s get ctrl msb: %d\n", __func__, ret);
	msleep(5);

	ret = stmpe_reg_read(keypad->stmpe, STMPE_INT_EN_MASK);
	if (ret < 0) {
		pr_err("[STMKEY]:%s read I2C fail %d\n", __func__, ret);
		return;
	}
	pr_info("[STMKEY]:%s get int en mask: %d\n", __func__, ret);
	msleep(5);

	ret = stmpe_reg_read(keypad->stmpe, STMPE_ICR_LSB);
	if (ret < 0) {
		pr_err("[STMKEY]:%s read I2C fail %d\n", __func__, ret);
		return;
	}
	pr_info("[STMKEY]:%s get icr lsb: %d\n", __func__, ret);
	msleep(5);

	ret = stmpe_reg_read(keypad->stmpe, STMPE_KPC_CTRL_CMD);
	if (ret < 0) {
		pr_err("[STMKEY]:%s read I2C fail %d\n", __func__, ret);
		return;
	}
	pr_info("[STMKEY]:%s get ctrl cmd: %d\n", __func__, ret);
}

static ssize_t  sec_keypad_dump_reg_show(struct device *dev,
        struct device_attribute *attr, char *buf)
{
	struct stmpe_keypad *keypad = dev_get_drvdata(dev);

	sec_keypad_dump_reg(keypad);

	return 1;
}

static ssize_t  sysfs_key_onoff_show(struct device *dev,
        struct device_attribute *attr, char *buf)
{
    struct stmpe_keypad *keypad = dev_get_drvdata(dev);
    int state = 0;
    int i;

    for (i = 0; i < 5; i++) {
        state |= keypad->key_state[i];
    }

    dev_info(keypad->stmpe->dev,
        "%s: key state:%d\n", __func__, !!state);

    return snprintf(buf, 5, "%d\n", !!state);
}

static DEVICE_ATTR(sec_key_pressed, 0444 , sysfs_key_onoff_show, NULL);
static DEVICE_ATTR(sec_keypad_dump_reg, 0444 , sec_keypad_dump_reg_show, NULL);


static ssize_t key_led_onoff_show(struct device *dev,
		struct device_attribute *attr, char *buf)
{
	struct stmpe_keypad *device_data = dev_get_drvdata(dev);
	int state;

	state = regulator_is_enabled(device_data->stmpe->ledvddo);

	dev_info(dev, "%s: key_led_%s\n", __func__, state ? "on" : "off");

	return snprintf(buf, 5, "%d\n", state);
}

static ssize_t key_led_onoff(struct device *dev,
				 struct device_attribute *attr, const char *buf,
				 size_t size)
{
	int data;
	int retval;
	struct stmpe_keypad *device_data = dev_get_drvdata(dev);

	sscanf(buf, "%d\n", &data);

/*
Test Manufacturing Part want that after function test(except SUB KEY Test) and then assemble keypad.
if back light is ON before keypad assemble, it is not good for eyes.so factory bin echo 2 to brighten.
*/
#ifdef CONFIG_SEC_FACTORY
	if(data==2){
		dev_err(dev, "%s: factory bin brighten the keypad led\n", __func__);
		data=1;
	}else
		data=0;
#endif
	
 if(data){
 	if(regulator_is_enabled(device_data->stmpe->ledvddo)){
			dev_err(dev, "%s: ledvddo is already enabled\n", __func__);
		} else {
			retval = regulator_enable(device_data->stmpe->ledvddo);
			if (retval) {
				dev_err(dev, "%s: Fail to enable regulator ledvddo[%d]\n",
						__func__, retval);
			}
			dev_err(dev, "%s: ledvddo is enabled[OK]\n", __func__);
		}
 	}else{
 	if (regulator_is_enabled(device_data->stmpe->ledvddo)) {
			retval = regulator_disable(device_data->stmpe->ledvddo);
			if (retval) {
				dev_err(dev, "%s: Fail to disable regulator vddo[%d]\n",
						__func__, retval);

			}
			dev_err(dev, "%s: vddo is disabled[OK]\n", __func__);
		} else {
			dev_err(dev, "%s: vddo is already disabled\n", __func__);
		}
 	}
	dev_err(dev, "%s: key_led_%s: %s now\n", __func__,
		(!!data) ? "on" : "off",
		regulator_is_enabled(device_data->stmpe->ledvddo) ? "on" : "off");
	return size;
}

static DEVICE_ATTR(brightness, S_IRUGO | S_IWUSR | S_IWGRP,
		key_led_onoff_show, key_led_onoff);

static struct attribute *key_attributes[] = {
	&dev_attr_sec_key_pressed.attr,
	&dev_attr_sec_keypad_dump_reg.attr,
	&dev_attr_brightness.attr,
	NULL,
};

static struct attribute_group key_attr_group = {
	.attrs = key_attributes,
};


static int stmpe_keypad_probe(struct platform_device *pdev)
{
	struct stmpe *stmpe = dev_get_drvdata(pdev->dev.parent);
	struct device_node *np = pdev->dev.of_node;
	struct stmpe_keypad *keypad;
	struct input_dev *input;
	u32 rows;
	u32 cols;
	struct matrix_keymap_data *keymap_data;
	int  keymap_len, i;
	u32 *keymap;
	const __be32 *map;
	int ret;

	if( stmpe->partnum != 1 )
		stmpe->partnum = 1;

	keypad = devm_kzalloc(&pdev->dev, sizeof(struct stmpe_keypad),
			      GFP_KERNEL);
	if (!keypad){
		dev_err(&pdev->dev,"keypad have not alloc keypad\n");	
		return -ENOMEM;
	}

	keypad->stmpe = stmpe;
	keypad->variant = &stmpe_keypad_variants[stmpe->partnum];

	ret=of_property_read_u32(np, "debounce-interval", &keypad->debounce_ms);
	if (ret) {
		dev_err(&pdev->dev,"keypad have not specified debounce_ms\n");	
		keypad->debounce_ms = 6;
		ret = 0;
	}
	
	ret=of_property_read_u32(np, "st,scan-count", &keypad->scan_count);
	if (ret) {
		dev_err(&pdev->dev,"keypad have not specified scan-count\n");	
		keypad->scan_count = 2;
		ret = 0;
	}

	keypad->no_autorepeat = of_property_read_bool(np, "st,no-autorepeat");

	input = devm_input_allocate_device(&pdev->dev);
	if (!input){
		dev_err(&pdev->dev,"keypad have not alloc input\n");	
		ret = -ENOMEM;
		goto err_input_alloc;
	}
	
	mutex_init(&keypad->dev_lock);
	wake_lock_init(&keypad->stmpe_wake_lock, WAKE_LOCK_SUSPEND, "stmpe_key wake lock");

	input->name = "sec_keypad_3x4-keypad";
	input->id.bustype = BUS_I2C;
	input->dev.parent = &pdev->dev;

	ret = matrix_keypad_parse_of_params(&pdev->dev, &rows, &cols);
	if (ret<0){
		dev_err(&pdev->dev,"keypad fail get the keypad rows and cols from the dts\n");	
		goto err_config;
	}
	
	dev_info(&pdev->dev,"[STMKEY]:stmpe_keypad rows %d cols %d \n", rows,cols );


	map = of_get_property(np, "linux,keymap", &keymap_len);

	if (!map) {
		dev_err(&pdev->dev,"[STMKEY]:Keymap not specified\n");
		ret = -EINVAL;
		goto err_config;
	}

	keymap_data = devm_kzalloc(&pdev->dev,
				sizeof(*keymap_data), GFP_KERNEL);
	if (!keymap_data) {
		dev_err(&pdev->dev,"[STMKEY]:Unable to allocate memory\n");
		ret = -ENOMEM;
		goto err_keymap_alloc;
	}

	keymap_data->keymap_size = keymap_len / sizeof(u32);

	keymap = devm_kzalloc(&pdev->dev,
		sizeof(uint32_t) * keymap_data->keymap_size, GFP_KERNEL);
	if (!keymap) {
		dev_err(&pdev->dev,"[STMKEY]:could not allocate memory for keymap\n");
		ret = -ENOMEM;
		goto err_map_alloc;
	}

	for (i = 0; i < keymap_data->keymap_size; i++) {
		unsigned int key = be32_to_cpup(map + i);
		int keycode, row, col;

		row = (key >> 24) & 0xff;
		col = (key >> 16) & 0xff;
		keycode = key & 0xffff;
		keymap[i] = KEY(row, col, keycode);
	}
	keymap_data->keymap = keymap;
	keypad->keymap_data = keymap_data;

	matrix_keypad_build_keymap(keymap_data, NULL,
		rows, cols, keypad->keymap, input);

	input_set_capability(input, EV_MSC, MSC_SCAN);
	if (!keypad->no_autorepeat)
		__set_bit(EV_REP, input->evbit);

	stmpe_keypad_fill_used_pins(keypad, rows, cols);

	keypad->input = input;

	ret = stmpe_keypad_chip_init(keypad);
	if (ret < 0){
		dev_err(&pdev->dev,"[STMKEY]:chip init fail \n");
		goto err_chipinit;
	}
	
	ret = devm_request_threaded_irq(&pdev->dev, stmpe->irq,
					  NULL, stmpe_keypad_irq,
					  IRQF_TRIGGER_LOW |IRQF_ONESHOT, "stmpe-keypad", keypad);
	if (ret) {
		dev_err(&pdev->dev, "unable to get irq: %d\n", ret);
		goto err_inturrupt;
	}

	ret = input_register_device(input);
	if (ret) {
		dev_err(&pdev->dev,"unable to register input device: %d\n", ret);
		goto err_input;
	}


#if defined(CONFIG_DRV_SAMSUNG)
	keypad->sec_keypad = sec_device_create(SEC_CLASS_DEVT_KPDLED, keypad, "sec_keypad");
#else
	keypad->sec_keypad = device_create(sec_class,
			NULL, SEC_CLASS_DEVT_KPDLED, keypad, "sec_keypad");
#endif

	if (IS_ERR(keypad->sec_keypad))
		dev_err(&pdev->dev, "Failed to create sec_key device\n");
	ret = sysfs_create_group(&keypad->sec_keypad->kobj, &key_attr_group);
	if (ret) {
		dev_err(&pdev->dev, "Failed to create the test sysfs: %d\n",
			ret);
	}

	enable_irq_wake(stmpe->irq);

	platform_set_drvdata(pdev, keypad);
	return ret;
	
err_input:	
err_inturrupt:
err_chipinit:
	kfree(keymap);
err_map_alloc:
	kfree(keymap_data);
err_keymap_alloc:
err_config:
	input_free_device(input);
	mutex_destroy(&keypad->dev_lock);
	wake_lock_destroy(&keypad->stmpe_wake_lock);
err_input_alloc:	
	kfree(keypad);
	dev_err(&pdev->dev, "Error at stmpe_keypad_probe\n");

	return ret;
}

static int stmpe_keypad_remove(struct platform_device *pdev)
{
	struct stmpe_keypad *keypad = platform_get_drvdata(pdev);

	disable_irq_wake(keypad->stmpe->irq);
	wake_lock_destroy(&keypad->stmpe_wake_lock);

	input_unregister_device(keypad->input);
	input_free_device(keypad->input);
	
	stmpe_disable(keypad->stmpe, STMPE_BLOCK_KEYPAD);
	kfree(keypad);

	return 0;
}

static struct platform_driver stmpe_keypad_driver = {
	.driver.name	= "stmpe-keypad",
	.driver.owner	= THIS_MODULE,
	.probe		= stmpe_keypad_probe,
	.remove		= stmpe_keypad_remove,
};
module_platform_driver(stmpe_keypad_driver);

MODULE_LICENSE("GPL v2");
MODULE_DESCRIPTION("STMPExxxx keypad driver");
MODULE_AUTHOR("Rabin Vincent <rabin.vincent@stericsson.com>");
