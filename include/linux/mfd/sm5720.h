/*
 *  sm5720.h
 *  Samsung mfd core driver header file for the SM5720.
 *
 *  Copyright (C) 2016 Samsung Electronics
 *
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#ifndef __SM5720_H__
#define __SM5720_H__
#include <linux/module.h>
#include <linux/init.h>
#include <linux/slab.h>
#include <linux/platform_device.h>
#include <linux/regmap.h>

#define MFD_DEV_NAME "sm5720"

#if defined(CONFIG_SS_VIBRATOR)

struct sm5720_haptic_platform_data
{
    int mode;
    int divisor;    /* PWM Frequency Divisor. 32, 64, 128 or 256 */
};

#endif

struct sm5720_platform_data {
	/* IRQ */
	int irq_base;
	int irq_gpio;
	bool wakeup;

    struct muic_platform_data *muic_pdata;

#if defined(CONFIG_SS_VIBRATOR)
	/* haptic motor data */
	struct sm5720_haptic_platform_data *haptic_data;
#endif
	struct mfd_cell *sub_devices;
	int num_subdevs;
};

#endif /* __SM5720_H__ */

