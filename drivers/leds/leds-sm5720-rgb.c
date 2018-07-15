/*
 *  leds-sm5720-rgb.c
 *  Samsung SM5720 RGB-LED Driver
 *
 *  Copyright (C) 2016 Samsung Electronics
 *
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#define DRIVER_NAME		"leds-sm5720-rgb"
#define pr_fmt(fmt)		DRIVER_NAME ": " fmt

#include <linux/leds.h>
#include <linux/debugfs.h>
#include <linux/platform_device.h>
#include <linux/mfd/sm5720.h>
#include <linux/mfd/sm5720-private.h>
#include <linux/leds-sm5720-rgb.h>
#include <linux/sec_class.h>

#define SEC_LED_SPECIFIC

#define DIMTT_TIME(time)	((time == 0) ? 0 :			\
				(time < 500) ? 1 :			\
				(time < 8000) ? time/500 : 15)

enum {
	RGBW_MODE_CC          = 0x0,
	RGBW_MODE_Dimming,
};

/*
 * SM5720 RGBW register control functions
 */
static u8 _calc_delay_time_offset_to_ms(unsigned int delay_ms)
{
	u8 offset;

	offset = DIMTT_TIME(delay_ms) & 0xf; /* step = 500ms */

	return offset;
}

static u8 _calc_step_time_offset_to_ms(unsigned int step_ms)
{
	u8 offset = (step_ms / 4) & 0xf; /* step = 4ms */

	return offset;
}

static int sm5720_RGBW_set_xLEDON(struct sm5720_rgb *rgb,
					int color_index, bool enable)
{
	if (WARN_ON(color_index < 0 || color_index > LED_MAX)) {
		pr_warn("%s: invalid color_index=%d\n",
				__func__, color_index);
		return -1;
	}
	sm5720_update_reg(rgb->i2c, SM5720_CHG_REG_RGBWCNTL1,
			((enable & 0x1) << (0 + color_index)),
			(0x1 << (0 + color_index)));

	return 0;
}

static u8 sm5720_RGBW_get_xLEDON(struct sm5720_rgb *rgb, int color_index)
{
	u8 reg = 0x0;

	if (WARN_ON(color_index < 0 || color_index > LED_MAX)) {
		pr_warn("%s: invalid color_index=%d\n",
				__func__, color_index);
		return -1;
	}
	sm5720_read_reg(rgb->i2c, SM5720_CHG_REG_RGBWCNTL1, &reg);

	return ((reg & (0x1 << color_index)) >> color_index);
}

static int sm5720_RGBW_set_xLEDMODE(struct sm5720_rgb *rgb,
		int color_index, bool mode)
{
	if (WARN_ON(color_index < 0 || color_index > LED_MAX)) {
		pr_warn("%s: invalid color_index=%d\n",
				__func__, color_index);
		return -1;
	}
	sm5720_update_reg(rgb->i2c, SM5720_CHG_REG_RGBWCNTL1,
			((mode & 0x1) << (4 + color_index)),
			(0x1 << (4 + color_index)));

	return 0;
}

static int sm5720_RGBW_set_xLEDCURRENT(struct sm5720_rgb *rgb,
		int color_index, u8 curr)
{
	if (WARN_ON(color_index < 0 || color_index > LED_MAX)) {
		pr_warn("%s: invalid color_index=%d\n",
				__func__, color_index);
		return -1;
	}
	sm5720_write_reg(rgb->i2c,
			SM5720_CHG_REG_RLEDCURRENT + color_index, curr);

	return 0;
}

static u8 sm5720_RGBW_get_xLEDCURRENT(struct sm5720_rgb *rgb, int color_index)
{
	u8 reg;

	if (WARN_ON(color_index < 0 || color_index > LED_MAX)) {
		pr_warn("%s: invalid color_index=%d\n",
				__func__, color_index);
		return -1;
	}
	sm5720_read_reg(rgb->i2c,
			SM5720_CHG_REG_RLEDCURRENT + color_index, &reg);

	return reg;
}

static int sm5720_RGBW_set_DIMSLPxLEDCNTL(struct sm5720_rgb *rgb,
		int color_index, unsigned int delay_on, unsigned int delay_off,
		unsigned int step_on, unsigned int step_off)
{
	u8 reg_value =
		((_calc_delay_time_offset_to_ms(delay_off) & 0xf) << 4) |
		(_calc_delay_time_offset_to_ms(delay_on) & 0xf);

	u8 step_on_reg_value =
		((_calc_step_time_offset_to_ms(step_on) & 0xf) << 4) |
		(_calc_step_time_offset_to_ms(step_on) & 0xf);

	u8 step_off_reg_value =
		((_calc_step_time_offset_to_ms(step_off) & 0xf) << 4) |
		(_calc_step_time_offset_to_ms(step_off) & 0xf);

	if (WARN_ON(color_index < 0 || color_index > LED_MAX)) {
		pr_warn("%s: invalid color_index=%d\n",
				__func__, color_index);
		return -1;
	}

	sm5720_write_reg(rgb->i2c, SM5720_CHG_REG_DIMSLPRLEDCNTL + color_index,
			reg_value);
	sm5720_write_reg(rgb->i2c, SM5720_CHG_REG_RLEDCNTL3 + (color_index * 4),
			step_on_reg_value);
	sm5720_write_reg(rgb->i2c, SM5720_CHG_REG_RLEDCNTL4 + (color_index * 4),
			step_off_reg_value);

	return 0;
}

static int sm5720_RGBW_print_reg(struct sm5720_rgb *rgb)
{
	u8 regs[SM5720_CHG_REG_END] = {0x0, };
	unsigned short cnt =
		SM5720_CHG_REG_HAPTICCNTL - SM5720_CHG_REG_RGBWCNTL1;
	int i;

	sm5720_bulk_read(rgb->i2c, SM5720_CHG_REG_RGBWCNTL1, cnt, regs);

	pr_info("");

	for (i = 0; i < cnt; ++i)
		pr_info("0x%x:0x%x ", SM5720_CHG_REG_RGBWCNTL1 + i, regs[i]);

	pr_info("\n");

	return 0;
}

static int sm5720_rgb_reset(struct sm5720_rgb *sm5720_rgb)
{
	sm5720_update_reg(sm5720_rgb->i2c,
			SM5720_CHG_REG_RGBWCNTL2, (0x1 << 4), (0x1 << 4));

	return 0;
}

static int sm5720_rgb_blink(struct sm5720_rgb *sm5720_rgb, int color_index,
		unsigned int delay_on_time_ms, unsigned int delay_off_time_ms,
		unsigned int step_on_time_ms, unsigned int step_off_time_ms)
{
	sm5720_RGBW_set_DIMSLPxLEDCNTL(sm5720_rgb, color_index,
					delay_on_time_ms, delay_off_time_ms,
					step_on_time_ms, step_off_time_ms);

	return 0;
}

static int sm5720_rgb_set_state(struct sm5720_rgb *sm5720_rgb, int color_index,
				u8 brightness, u8 mode)
{
	int value = brightness;

	pr_info("%s: color_index=%d, brightness=%d, mode=%d\n",
			__func__, color_index, brightness, mode);

	if (brightness != 0) {
		/* apply brightness ratio for optimize each led brightness*/
		if (en_lowpower_mode && led_power_mode == LOW_MODE)
			value = brightness * brightness_ratio_low[color_index];
		else
			value = brightness * brightness_ratio[color_index];

		/*
		 *  There is possibility that value becomes 0 even brightness>0 && ratio>0.
		 *  ex) brightness is 10 & brightness_ratio[RED] is 9
		 *  value = 10 * 9 / 100 = 0.9
		 *  value is inteager, so value is 0.
		 *  In this case, it is need to assign 1 of value.
		 */
		if (value != 0) {
			value /= 100;
			if (value == 0)
				value = 1;
			if (value > LED_MAX_CURRENT)
				value = LED_MAX_CURRENT;
		}
	}
	pr_info("LED[%d] CURRENT = %02d.%dmA\n", color_index,
			(value / 10), (value % 10));

	sm5720_RGBW_set_xLEDCURRENT(sm5720_rgb, color_index, value);

	switch (mode) {
	case LED_DISABLE:
		sm5720_RGBW_set_xLEDON(sm5720_rgb, color_index, 0);
		break;
	case LED_ALWAYS_ON:
		sm5720_RGBW_set_xLEDMODE(sm5720_rgb, color_index,
				RGBW_MODE_CC);
		sm5720_RGBW_set_xLEDON(sm5720_rgb, color_index, 1);
		break;
	case LED_BLINK:
		sm5720_RGBW_set_xLEDMODE(sm5720_rgb, color_index,
				RGBW_MODE_Dimming);
		sm5720_RGBW_set_xLEDON(sm5720_rgb, color_index, 1);
		break;
	default:
		return -EINVAL;
	}

	return 0;
}

/*
 * SM5720 RGBW SAMSUNG specific led device control functions
 */

#ifdef SEC_LED_SPECIFIC

static ssize_t store_led_r(struct device *dev,
			struct device_attribute *devattr,
				const char *buf, size_t count)
{
	struct sm5720_rgb *sm5720_rgb = dev_get_drvdata(dev);
	unsigned int brightness;
	int ret;

	ret = kstrtouint(buf, 0, &brightness);
	if (ret != 0) {
		dev_err(dev, "fail to get brightness.\n");
		goto out;
	}

	if (brightness > LED_MAX_CURRENT)
		brightness = LED_MAX_CURRENT;

	if (brightness != 0) {
		sm5720_rgb_set_state(sm5720_rgb, RED, brightness,
				LED_ALWAYS_ON);
	} else {
		sm5720_rgb_set_state(sm5720_rgb, RED, LED_OFF, LED_DISABLE);
	}
out:
	pr_info("%s\n", __func__);

	return count;
}
static ssize_t store_led_g(struct device *dev,
			struct device_attribute *devattr,
			const char *buf, size_t count)
{
	struct sm5720_rgb *sm5720_rgb = dev_get_drvdata(dev);
	unsigned int brightness;
	int ret;

	ret = kstrtouint(buf, 0, &brightness);
	if (ret != 0) {
		dev_err(dev, "fail to get brightness.\n");
		goto out;
	}

	if (brightness > LED_MAX_CURRENT)
		brightness = LED_MAX_CURRENT;

	if (brightness != 0) {
		sm5720_rgb_set_state(sm5720_rgb, GREEN, brightness,
				LED_ALWAYS_ON);
	} else {
		sm5720_rgb_set_state(sm5720_rgb, GREEN, LED_OFF, LED_DISABLE);
	}
out:
	pr_info("%s\n", __func__);
	return count;
}
static ssize_t store_led_b(struct device *dev,
		struct device_attribute *devattr,
		const char *buf, size_t count)
{
	struct sm5720_rgb *sm5720_rgb = dev_get_drvdata(dev);
	unsigned int brightness;
	int ret;

	ret = kstrtouint(buf, 0, &brightness);
	if (ret != 0) {
		dev_err(dev, "fail to get brightness.\n");
		goto out;
	}

	if (brightness > LED_MAX_CURRENT)
		brightness = LED_MAX_CURRENT;

	if (brightness != 0) {
		sm5720_rgb_set_state(sm5720_rgb, BLUE, brightness,
				LED_ALWAYS_ON);
	} else	{
		sm5720_rgb_set_state(sm5720_rgb, BLUE, LED_OFF, LED_DISABLE);
	}
out:
	pr_info("%s\n", __func__);

	return count;
}

static ssize_t store_sm5720_rgb_pattern(struct device *dev,
					struct device_attribute *devattr,
					const char *buf, size_t count)
{
	struct sm5720_rgb *sm5720_rgb = dev_get_drvdata(dev);
	unsigned int mode = 0;
	int ret;

	pr_info("%s: led_power_mode : %d\n", __func__, led_power_mode);

	ret = sscanf(buf, "%1d", &mode);
	if (ret == 0) {
		dev_err(dev, "fail to get led_pattern mode.\n");
		return count;
	}

	if (mode > POWERING)
		return count;

	/* Set all LEDs Off */
	sm5720_rgb_reset(sm5720_rgb);
	if (mode == PATTERN_OFF)
		return count;

	/* Set to low power consumption mode */
	if (led_power_mode == LOW_MODE)
		led_dynamic_current = low_powermode_current;
	else
		led_dynamic_current = normal_powermode_current;

	switch (mode) {
	case CHARGING:
		sm5720_rgb_set_state(sm5720_rgb, RED, led_dynamic_current,
				LED_ALWAYS_ON);
		break;
	case CHARGING_ERR:
		sm5720_rgb_blink(sm5720_rgb, RED, 500, 500, 0, 0);
		sm5720_rgb_set_state(sm5720_rgb, RED, led_dynamic_current,
				LED_BLINK);
		break;
	case MISSED_NOTI:
		sm5720_rgb_blink(sm5720_rgb, BLUE, 500, 5000, 0, 0);
		sm5720_rgb_set_state(sm5720_rgb, BLUE, led_dynamic_current,
				LED_BLINK);
		break;
	case LOW_BATTERY:
		sm5720_rgb_blink(sm5720_rgb, RED, 500, 5000, 0, 0);
		sm5720_rgb_set_state(sm5720_rgb, RED, led_dynamic_current,
				LED_BLINK);
		break;
	case FULLY_CHARGED:
		sm5720_rgb_set_state(sm5720_rgb, GREEN, led_dynamic_current,
				LED_ALWAYS_ON);
		break;
	case POWERING:
		sm5720_rgb_blink(sm5720_rgb, GREEN, 1000, 1000, 8, 8);
		sm5720_rgb_set_state(sm5720_rgb, GREEN, led_dynamic_current,
				LED_BLINK);
		sm5720_rgb_set_state(sm5720_rgb, BLUE, led_dynamic_current,
				LED_ALWAYS_ON);
		break;
	default:
		break;
	}

	pr_info("%s: mode=%d, led_dynamic_current=%d\n", __func__,
			mode, led_dynamic_current);
	sm5720_RGBW_print_reg(sm5720_rgb);

	return count;
}

static ssize_t store_sm5720_rgb_blink(struct device *dev,
					struct device_attribute *devattr,
					const char *buf, size_t count)
{
	struct sm5720_rgb *sm5720_rgb = dev_get_drvdata(dev);
	int led_brightness = 0;
	int delay_on_time = 0;
	int delay_off_time = 0;
	int step_on_time = 0;
	int step_off_time = 0;
	u8 led_r_brightness = 0;
	u8 led_g_brightness = 0;
	u8 led_b_brightness = 0;
	unsigned int led_total_br = 0;
	unsigned int led_max_br = 0;
	int ret;

	ret = sscanf(buf, "0x%8x %5d %5d %5d %5d", &led_brightness,
			&delay_on_time, &delay_off_time, &step_on_time,
			&step_off_time);
	if (ret == 0) {
		dev_err(dev, "fail to get led_blink value.\n");
		return count;
	}

	/* Set to low power consumption mode */
	led_dynamic_current = normal_powermode_current;

	/*Reset led*/
	sm5720_rgb_reset(sm5720_rgb);

	led_r_brightness = (led_brightness & LED_R_MASK) >> 16;
	led_g_brightness = (led_brightness & LED_G_MASK) >> 8;
	led_b_brightness = led_brightness & LED_B_MASK;

	/* In user case, LED current is restricted to less than tuning value */
	if (led_r_brightness != 0) {
		led_r_brightness = (led_r_brightness * led_dynamic_current) /
								LED_MAX_CURRENT;
		if (led_r_brightness == 0)
			led_r_brightness = 1;
	}
	if (led_g_brightness != 0) {
		led_g_brightness = (led_g_brightness * led_dynamic_current) /
								LED_MAX_CURRENT;
		if (led_g_brightness == 0)
			led_g_brightness = 1;
	}
	if (led_b_brightness != 0) {
		led_b_brightness = (led_b_brightness * led_dynamic_current) /
								LED_MAX_CURRENT;
		if (led_b_brightness == 0)
			led_b_brightness = 1;
	}

	led_total_br += led_r_brightness * brightness_ratio[RED] / 100;
	led_total_br += led_g_brightness * brightness_ratio[GREEN] / 100;
	led_total_br += led_b_brightness * brightness_ratio[BLUE] / 100;

	if (brightness_ratio[RED] >= brightness_ratio[GREEN] &&
		brightness_ratio[RED] >= brightness_ratio[BLUE]) {
		led_max_br =
			normal_powermode_current * brightness_ratio[RED] / 100;
	} else if (brightness_ratio[GREEN] >= brightness_ratio[RED] &&
		brightness_ratio[GREEN] >= brightness_ratio[BLUE]) {
		led_max_br =
			normal_powermode_current * brightness_ratio[GREEN] / 100;
	} else if (brightness_ratio[BLUE] >= brightness_ratio[RED] &&
		brightness_ratio[BLUE] >= brightness_ratio[GREEN]) {
		led_max_br =
			normal_powermode_current * brightness_ratio[BLUE] / 100;
	}

	/* Each color decreases according to the limit at the same rate. */
	if (led_total_br > led_max_br) {
		if (led_r_brightness != 0) {
			led_r_brightness =
				led_r_brightness * led_max_br / led_total_br;
			if (led_r_brightness == 0)
				led_r_brightness = 1;
		}
		if (led_g_brightness != 0) {
			led_g_brightness =
				led_g_brightness * led_max_br / led_total_br;
			if (led_g_brightness == 0)
				led_g_brightness = 1;
		}
		if (led_b_brightness != 0) {
			led_b_brightness =
				led_b_brightness * led_max_br / led_total_br;
			if (led_b_brightness == 0)
				led_b_brightness = 1;
		}
	}

	if (led_r_brightness) {
		sm5720_rgb_blink(sm5720_rgb, RED, delay_on_time,
				delay_off_time,	step_on_time, step_off_time);
		sm5720_rgb_set_state(sm5720_rgb, RED,
				led_r_brightness, LED_BLINK);
	}
	if (led_g_brightness) {
		sm5720_rgb_blink(sm5720_rgb, GREEN, delay_on_time,
				delay_off_time,	step_on_time, step_off_time);
		sm5720_rgb_set_state(sm5720_rgb, GREEN,
				led_g_brightness, LED_BLINK);
	}
	if (led_b_brightness) {
		sm5720_rgb_blink(sm5720_rgb, BLUE, delay_on_time,
				delay_off_time,	step_on_time, step_off_time);
		sm5720_rgb_set_state(sm5720_rgb, BLUE,
				led_b_brightness, LED_BLINK);
	}

	pr_info("%s, on_time= %x, off_time= %x, step_on_time= %x, step_off_time= %x\n",
			__func__, delay_on_time, delay_off_time,
			step_on_time, step_off_time);
	dev_dbg(dev, "led_blink is called, Color:0x%X Brightness:%i\n",
			led_brightness, led_dynamic_current);

	sm5720_RGBW_print_reg(sm5720_rgb);

	return count;
}

static ssize_t store_sm5720_rgb_power_mode(struct device *dev,
					struct device_attribute *devattr,
					const char *buf, size_t count)
{
	int ret;
	u8 _led_power_mode;

	ret = kstrtou8(buf, 0, &_led_power_mode);
	if (ret != 0) {
		dev_err(dev, "fail to get led_power_mode.\n");
		return -EINVAL;
	}

	led_power_mode = _led_power_mode;

	pr_info("led_power_mode set to %i\n", led_power_mode);

	return count;
}
static ssize_t store_sm5720_rgb_brightness(struct device *dev,
					struct device_attribute *devattr,
					const char *buf, size_t count)
{
	int ret;
	u8 brightness;

	pr_info("%s\n", __func__);

	ret = kstrtou8(buf, 0, &brightness);
	if (ret != 0) {
		dev_err(dev, "fail to get led_brightness.\n");
		return -EINVAL;
	}

	led_power_mode = NORMAL_MODE;

	if (brightness > LED_MAX_CURRENT)
		brightness = LED_MAX_CURRENT;

	led_dynamic_current = brightness;

	dev_dbg(dev, "led brightness set to %i\n", brightness);

	return count;
}

static ssize_t rgb_tuning_show(struct device *dev,
				struct device_attribute *attr,
				char *buf)
{
	return sprintf(buf, "%s: %s [RED: %d, GREEN: %d, BLUE: %d]\n",
			DRIVER_NAME, __func__, brightness_ratio[RED],
			brightness_ratio[GREEN], brightness_ratio[BLUE]);
}

static ssize_t rgb_tuning_store(struct device *dev,
			struct device_attribute *attr,
			const char *buf, size_t count)
{
	int ret, red, green, blue;

	ret = sscanf(buf, "%d %d %d", &red, &green, &blue);
	brightness_ratio[RED] = red;
	brightness_ratio[GREEN] = green;
	brightness_ratio[BLUE] = blue;

	return count;
}


/* below nodes is SAMSUNG specific nodes */
static DEVICE_ATTR(led_r, 0660, NULL, store_led_r);
static DEVICE_ATTR(led_g, 0660, NULL, store_led_g);
static DEVICE_ATTR(led_b, 0660, NULL, store_led_b);
/* led_pattern node permission is 222 */
/* To access sysfs node from other groups */
static DEVICE_ATTR(led_pattern, 0660, NULL, store_sm5720_rgb_pattern);
static DEVICE_ATTR(led_blink, 0660, NULL,  store_sm5720_rgb_blink);
static DEVICE_ATTR(led_brightness, 0660, NULL, store_sm5720_rgb_brightness);
static DEVICE_ATTR(led_power_mode, 0660, NULL,  store_sm5720_rgb_power_mode);
static DEVICE_ATTR(led_tuning, 0660, rgb_tuning_show, rgb_tuning_store);

static struct attribute *sec_led_attributes[] = {
	&dev_attr_led_r.attr,
	&dev_attr_led_g.attr,
	&dev_attr_led_b.attr,
	&dev_attr_led_pattern.attr,
	&dev_attr_led_blink.attr,
	&dev_attr_led_brightness.attr,
	&dev_attr_led_power_mode.attr,
	&dev_attr_led_tuning.attr,
	NULL,
};

static struct attribute_group sec_led_attr_group = {
	.attrs = sec_led_attributes,
};
#endif

/**
 * SM5720 RGBW common led-class device control functions
 */

static int sm5720_get_color_index_to_led_dev(struct sm5720_rgb *sm5720_rgb,
						struct led_classdev *led_cdev)
{
	int i;

	for (i = 0; i < LED_MAX; ++i) {
		if (&sm5720_rgb->led[i] == led_cdev)
			return i;
	}

	return -ENODEV;
}

static void sm5720_rgb_set(struct led_classdev *led_cdev,
				unsigned int brightness)
{
	const struct device *parent = led_cdev->dev->parent;
	struct sm5720_rgb *sm5720_rgb = dev_get_drvdata(parent);
	int color_index;

	color_index = sm5720_get_color_index_to_led_dev(sm5720_rgb, led_cdev);
	if (color_index < 0) {
		dev_err(led_cdev->dev,
			"sm5720_rgb_number() returns %d.\n", color_index);
		return;
	}

	if (brightness == LED_OFF) {
		sm5720_RGBW_set_xLEDON(sm5720_rgb, color_index, 0);
	} else {
		sm5720_RGBW_set_xLEDCURRENT(sm5720_rgb,
				color_index, brightness);
		sm5720_RGBW_set_xLEDMODE(sm5720_rgb, color_index, RGBW_MODE_CC);
		sm5720_RGBW_set_xLEDON(sm5720_rgb, color_index, 1);
		sm5720_RGBW_set_xLEDON(sm5720_rgb, color_index, 1);
	}

	pr_info("%s: color_index=%d, brightness=%d\n",
			__func__, color_index, brightness);
	sm5720_RGBW_print_reg(sm5720_rgb);
}

static unsigned int sm5720_rgb_get(struct led_classdev *led_cdev)
{
	const struct device *parent = led_cdev->dev->parent;
	struct sm5720_rgb *sm5720_rgb = dev_get_drvdata(parent);
	int color_index;
	u8 value;

	pr_info("%s\n", __func__);

	color_index = sm5720_get_color_index_to_led_dev(sm5720_rgb, led_cdev);
	if (color_index < 0) {
		dev_err(led_cdev->dev,
			"sm5720_rgb_number() returns %d.\n", color_index);
		return 0;
	}

	/* Get status */
	value = sm5720_RGBW_get_xLEDON(sm5720_rgb, color_index);
	if (!value)
		return LED_OFF;

	/* Get current */
	value = sm5720_RGBW_get_xLEDCURRENT(sm5720_rgb, color_index);

	return value;
}

static ssize_t led_delay_on_show(struct device *dev,
			struct device_attribute *attr, char *buf)
{
	struct sm5720_rgb *sm5720_rgb = dev_get_drvdata(dev);

	return sprintf(buf, "%d\n", sm5720_rgb->delay_on_times_ms);
}

static ssize_t led_delay_on_store(struct device *dev,
			struct device_attribute *attr,
			const char *buf, size_t count)
{
	struct sm5720_rgb *sm5720_rgb = dev_get_drvdata(dev);
	unsigned int time;

	if (kstrtouint(buf, 0, &time)) {
		dev_err(dev, "can not write led_delay_on\n");
		return -EINVAL;
	}

	sm5720_rgb->delay_on_times_ms = time;

	return count;
}

static ssize_t led_delay_off_show(struct device *dev,
			struct device_attribute *attr, char *buf)
{
	struct sm5720_rgb *sm5720_rgb = dev_get_drvdata(dev);

	return sprintf(buf, "%d\n", sm5720_rgb->delay_off_times_ms);
}

static ssize_t led_delay_off_store(struct device *dev,
			struct device_attribute *attr,
			const char *buf, size_t count)
{
	struct sm5720_rgb *sm5720_rgb = dev_get_drvdata(dev);
	unsigned int time;

	if (kstrtouint(buf, 0, &time)) {
		dev_err(dev, "can not write led_delay_off\n");
		return -EINVAL;
	}

	sm5720_rgb->delay_off_times_ms = time;

	return count;
}

static ssize_t led_blink_store(struct device *dev,
			struct device_attribute *attr,
			const char *buf, size_t count)
{
	struct sm5720_rgb *sm5720_rgb = dev_get_drvdata(dev);
	unsigned int blink_set;
	int n = 0;
	int i;

	if (kstrtouint(buf, 0, &blink_set)) {
		dev_err(dev, "can not write led_blink\n");
		return -EINVAL;
	}

	if (!blink_set) {
		sm5720_rgb->delay_on_times_ms = LED_OFF;
		sm5720_rgb->delay_off_times_ms = LED_OFF;
	}

	for (i = 0; i < LED_MAX; i++) {
		if (dev == sm5720_rgb->led[i].dev)
			n = i;
	}

	sm5720_rgb_blink(sm5720_rgb, n,
			sm5720_rgb->delay_on_times_ms,
			sm5720_rgb->delay_off_times_ms, 0, 0);
	sm5720_rgb_set_state(sm5720_rgb, n,
			led_dynamic_current,
			LED_BLINK);

	pr_info("%s\n", __func__);

	return count;
}

/* permission for sysfs node */
static DEVICE_ATTR(delay_on, 0640, led_delay_on_show, led_delay_on_store);
static DEVICE_ATTR(delay_off, 0640, led_delay_off_show, led_delay_off_store);
static DEVICE_ATTR(blink, 0640, NULL, led_blink_store);

static struct attribute *led_class_attrs[] = {
	&dev_attr_delay_on.attr,
	&dev_attr_delay_off.attr,
	&dev_attr_blink.attr,
	NULL,
};

static struct attribute_group common_led_attr_group = {
	.attrs = led_class_attrs,
};

/*
 * SM5720 RGBW Platform driver handling functions
 */

#ifdef CONFIG_OF
static struct sm5720_rgb_platform_data
			*sm5720_rgb_parse_dt(struct device *dev)
{
	struct sm5720_rgb_platform_data *pdata;
	struct device_node *np;
	int ret;
	int i;
	int temp;
	const char *octa_color;
	char br_ratio_r[23] = "br_ratio_r";
	char br_ratio_g[23] = "br_ratio_g";
	char br_ratio_b[23] = "br_ratio_b";
	char br_ratio_r_low[23] = "br_ratio_r_low";
	char br_ratio_g_low[23] = "br_ratio_g_low";
	char br_ratio_b_low[23] = "br_ratio_b_low";
	char normal_po_cur[29] = "normal_powermode_current";
	char low_po_cur[26] = "low_powermode_current";

	pr_info("%s\n", __func__);

	pdata = devm_kzalloc(dev, sizeof(*pdata), GFP_KERNEL);
	if (unlikely(pdata == NULL))
		return ERR_PTR(-ENOMEM);

	np = of_find_node_by_name(NULL, "rgb");
	if (unlikely(np == NULL)) {
		dev_err(dev, "rgb node not found\n");
		devm_kfree(dev, pdata);
		return ERR_PTR(-EINVAL);
	}

	for (i = 0; i < LED_MAX; i++)	{
		ret = of_property_read_string_index(np, "rgb-name", i,
						(const char **)&pdata->name[i]);

		pr_info("%s, %s\n", __func__, pdata->name[i]);

		if (ret < 0) {
			devm_kfree(dev, pdata);
			return ERR_PTR(ret);
		}
	}

	/* parse en_lowpower_mode */
	en_lowpower_mode = of_property_read_bool(np, "en-lowpower-mode");

	/* parse the octa_code */
	of_property_read_string_index(np, "octa-code", octa_code,
			(const char **)&octa_color);

	pr_info("%s: octa-color:%s, octa_code:%d\n", __func__, octa_color,
			octa_code);

	strcat(normal_po_cur, octa_color);
	strcat(low_po_cur, octa_color);
	strcat(br_ratio_r, octa_color);
	strcat(br_ratio_g, octa_color);
	strcat(br_ratio_b, octa_color);
	strcat(br_ratio_r_low, octa_color);
	strcat(br_ratio_g_low, octa_color);
	strcat(br_ratio_b_low, octa_color);

	pr_info("%s: en-lowpower-mode: %d, octa-color: %s(%d)\n",
			__func__, en_lowpower_mode, octa_color, octa_code);

	/* get normal_powermode_current value in dt */
	ret = of_property_read_u32(np, normal_po_cur, &temp);

	if (ret < 0)
		pr_info("%s, can't parse normal_powermode_current in dt\n", __func__);
	else
		normal_powermode_current = (u8)temp;

	pr_info("%s, normal_powermode_current = %x\n",
			__func__, normal_powermode_current);

	/* get low_powermode_current value in dt */
	ret = of_property_read_u32(np, low_po_cur, &temp);
	if (ret < 0)
		pr_info("%s, can't parse low_powermode_current in dt\n", __func__);
	else
		low_powermode_current = (u8)temp;
	pr_info("%s, low_powermode_current = %x\n",
			__func__, low_powermode_current);

	/* get led red brightness ratio */
	ret = of_property_read_u32(np, br_ratio_r, &temp);
	if (ret < 0)
		pr_info("%s, can't parse brightness_ratio[RED] in dt\n", __func__);
	else
		brightness_ratio[RED] = (int)temp;
	pr_info("%s, brightness_ratio[RED] = %x\n",
			__func__, brightness_ratio[RED]);

	/* get led green brightness ratio */
	ret = of_property_read_u32(np, br_ratio_g, &temp);
	if (ret < 0)
		pr_info("%s, can't parse brightness_ratio[GREEN] in dt\n", __func__);
	else
		brightness_ratio[GREEN] = (int)temp;
	pr_info("%s, brightness_ratio[GREEN] = %x\n",
			__func__, brightness_ratio[GREEN]);

	/* get led blue brightness ratio */
	ret = of_property_read_u32(np, br_ratio_b, &temp);
	if (ret < 0)
		pr_info("%s, can't parse brightness_ratio[BLUE] in dt\n", __func__);
	else
		brightness_ratio[BLUE] = (int)temp;
	pr_info("%s, brightness_ratio[BLUE] = %x\n",
			__func__, brightness_ratio[BLUE]);

	/* For Dream */
	if (en_lowpower_mode) {
		/* get led red brightness ratio lowpower */
		ret = of_property_read_u32(np, br_ratio_r_low, &temp);
		if (ret < 0)
			pr_info("%s, can't parse brightness_ratio_low[RED] in dt\n", __func__);
		else
			brightness_ratio_low[RED] = (int)temp;
		pr_info("%s, brightness_ratio_low[RED] = %x\n",
				__func__, brightness_ratio_low[RED]);

		/* get led green brightness ratio lowpower*/
		ret = of_property_read_u32(np, br_ratio_g_low, &temp);
		if (ret < 0)
			pr_info("%s, can't parse brightness_ratio_low[GREEN] in dt\n", __func__);
		else
			brightness_ratio_low[GREEN] = (int)temp;
		pr_info("%s, brightness_ratio_low[GREEN] = %x\n",
				__func__, brightness_ratio_low[GREEN]);

		/* get led blue brightness ratio lowpower */
		ret = of_property_read_u32(np, br_ratio_b_low, &temp);
		if (ret < 0)
			pr_info("%s, can't parse brightness_ratio_low[BLUE] in dt\n", __func__);
		else
			brightness_ratio_low[BLUE] = (int)temp;
		pr_info("%s, brightness_ratio_low[BLUE] = %x\n",
				__func__, brightness_ratio_low[BLUE]);
	}

	return pdata;
}
#endif

static int sm5720_rgb_probe(struct platform_device *pdev)
{
	struct device *dev = &pdev->dev;
	struct sm5720_rgb_platform_data *pdata;
	struct sm5720_rgb *sm5720_rgb;
	struct sm5720_dev *sm5720_dev = dev_get_drvdata(dev->parent);
	char temp_name[4][40] = {{0,},}, name[40] = {0,}, *p;
	int i, ret;

	pr_info("%s\n", __func__);

	/* FIXME: because the get_lcd_attached is not implemented yet,
	 * it is fixed to 0 which means black octa temporarily.
	 */
	//octa_code = (get_lcd_attached("GET") >> 16) & 0x0000000f;
	octa_code = 1;

#ifdef CONFIG_OF
	pdata = sm5720_rgb_parse_dt(dev);
	if (unlikely(IS_ERR(pdata)))
		return PTR_ERR(pdata);

	led_dynamic_current = normal_powermode_current;
#else
	pdata = dev_get_platdata(dev);
#endif

	sm5720_rgb = devm_kzalloc(dev, sizeof(struct sm5720_rgb), GFP_KERNEL);
	if (unlikely(!sm5720_rgb))
		return -ENOMEM;

	platform_set_drvdata(pdev, sm5720_rgb);
	sm5720_rgb->i2c = sm5720_dev->charger;

	for (i = 0; i < LED_MAX; i++) {
		ret = snprintf(name, 30, "%s", pdata->name[i])+1;
		if (ret < 1)
			goto alloc_err_flash;

		p = devm_kzalloc(dev, ret, GFP_KERNEL);
		if (unlikely(!p))
			goto alloc_err_flash;

		strcpy(p, name);
		strcpy(temp_name[i], name);
		sm5720_rgb->led[i].name = p;
		sm5720_rgb->led[i].brightness_set = sm5720_rgb_set;
		sm5720_rgb->led[i].brightness_get = sm5720_rgb_get;
		sm5720_rgb->led[i].max_brightness = LED_MAX_CURRENT;

		ret = led_classdev_register(dev, &sm5720_rgb->led[i]);
		if (ret < 0) {
			dev_err(dev, "unable to register RGB : %d\n", ret);
			goto alloc_err_flash_plus;
		}
		ret = sysfs_create_group(&sm5720_rgb->led[i].dev->kobj,
						&common_led_attr_group);
		if (ret < 0) {
			dev_err(dev, "can not register sysfs attribute\n");
			goto register_err_flash;
		}
	}

#ifdef SEC_LED_SPECIFIC
	sm5720_rgb->led_dev = sec_device_create(0, sm5720_rgb, "led");
	if (IS_ERR(sm5720_rgb->led_dev)) {
		dev_err(dev, "Failed to create device for samsung specific led\n");
		goto alloc_err_flash;
	}

	ret = sysfs_create_group(&sm5720_rgb->led_dev->kobj,
			&sec_led_attr_group);
	if (ret < 0) {
		dev_err(dev, "Failed to create sysfs group for samsung specific led\n");
		goto alloc_err_flash;
	}
#endif

	sm5720_rgb->dev = dev;

	pr_info("%s done\n", __func__);

	return 0;

register_err_flash:
	led_classdev_unregister(&sm5720_rgb->led[i]);
alloc_err_flash_plus:
	devm_kfree(dev, temp_name[i]);
alloc_err_flash:
	while (i--) {
		led_classdev_unregister(&sm5720_rgb->led[i]);
		devm_kfree(dev, temp_name[i]);
	}
	devm_kfree(dev, sm5720_rgb);
	return -ENOMEM;
}

static int sm5720_rgb_remove(struct platform_device *pdev)
{
	struct sm5720_rgb *sm5720_rgb = platform_get_drvdata(pdev);
	int i;

	for (i = 0; i < LED_MAX; i++)
		led_classdev_unregister(&sm5720_rgb->led[i]);

	return 0;
}

static void sm5720_rgb_shutdown(struct platform_device *pdev)
{
	struct sm5720_rgb *sm5720_rgb = platform_get_drvdata(pdev);
	int i;

	if (!sm5720_rgb->i2c)
		return;

	pr_info("entering %s\n", __func__);

	sm5720_rgb_reset(sm5720_rgb);

#ifdef SEC_LED_SPECIFIC
	sysfs_remove_group(&sm5720_rgb->led_dev->kobj, &sec_led_attr_group);
#endif

	for (i = 0; i < LED_MAX; i++) {
		sysfs_remove_group(&sm5720_rgb->led[i].dev->kobj,
						&common_led_attr_group);
		led_classdev_unregister(&sm5720_rgb->led[i]);
	}
	devm_kfree(&pdev->dev, sm5720_rgb);
}

static struct platform_driver sm5720_fled_driver = {
	.driver		= {
		.name	= DRIVER_NAME,
		.owner	= THIS_MODULE,
	},
	.probe		= sm5720_rgb_probe,
	.remove		= sm5720_rgb_remove,
	.shutdown       = sm5720_rgb_shutdown,
};

static int __init sm5720_rgb_init(void)
{
	pr_info("%s\n", __func__);
	return platform_driver_register(&sm5720_fled_driver);
}
module_init(sm5720_rgb_init);

static void __exit sm5720_rgb_exit(void)
{
	platform_driver_unregister(&sm5720_fled_driver);
}
module_exit(sm5720_rgb_exit);

MODULE_DESCRIPTION("Samsung SM5720 RGB-LED Driver");
MODULE_AUTHOR("Samsung Electronics");
MODULE_LICENSE("GPL");
