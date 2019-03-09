/*
 *  leds-sm5720-rgb.h
 *  Samsung SM5720 RGB-LED Driver header file
 *
 *  Copyright (C) 2016 Samsung Electronics
 *
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#ifndef __LEDS_SM5720_RGB_H
#define __LEDS_SM5720_RGB_H __FILE__

#define LED_R_MASK		    0x00FF0000
#define LED_G_MASK		    0x0000FF00
#define LED_B_MASK		    0x000000FF
#define LED_MAX_CURRENT		    0xFF

#define BASE_DYNAMIC_LED_CURRENT    0x0A
#define BASE_LOW_POWER_CURRENT      0x02

enum sm5720_led_color {
	BLUE	= 0x0,
	RED,
	GREEN,
	WHITE,
	LED_MAX,
};

enum sm5720_led_mode {
	LED_DISABLE = 0x0,
	LED_ALWAYS_ON,
	LED_BLINK,
};

enum sm5720_led_pattern {
	PATTERN_OFF,
	CHARGING,
	CHARGING_ERR,
	MISSED_NOTI,
	LOW_BATTERY,
	FULLY_CHARGED,
	POWERING,
};

enum sm5720_led_powermode {
	NORMAL_MODE,
	LOW_MODE,
	MAX_POWERMODE,
};

/* helper function which is supported by LCD driver */
extern int get_lcd_attached(char *mode);

static u8 led_dynamic_current = BASE_DYNAMIC_LED_CURRENT;
static u8 normal_powermode_current = BASE_DYNAMIC_LED_CURRENT;
static u8 low_powermode_current = BASE_LOW_POWER_CURRENT;
static u8 led_power_mode;

static unsigned int octa_code;
static unsigned int brightness_ratio[LED_MAX] = {100, 100, 100, };
static unsigned int brightness_ratio_low[LED_MAX] = {20, 20, 20, };
static bool en_lowpower_mode;

struct sm5720_rgb {
	struct device *dev;
	struct device *led_dev;
	struct led_classdev led[LED_MAX];
	struct i2c_client *i2c;
	unsigned int delay_on_times_ms;
	unsigned int delay_off_times_ms;
};

struct sm5720_rgb_platform_data {
	char *name[LED_MAX];
};

#endif /* __LEDS_SM5720_RGB_H */

