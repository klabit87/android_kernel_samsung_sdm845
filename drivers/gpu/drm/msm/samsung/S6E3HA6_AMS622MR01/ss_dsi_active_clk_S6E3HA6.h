/*
 * Copyright (c) 2016 Samsung Electronics Co., Ltd.
 *	      http://www.samsung.com/
 *
 * Samsung's Live Clock Driver
 * Author: Minwoo Kim <minwoo7945.kim@samsung.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#ifndef __SS_DSI_ACTIVE_CLK_S6E3HA6_H__
#define __SS_DSI_ACTIVE_CLK_S6E3HA6_H__

#include <linux/kernel.h>
#include <linux/poll.h>
#include <linux/miscdevice.h>
#include <linux/fs.h>
#include <linux/file.h>
#include <linux/err.h>
#include <linux/mutex.h>

#define IOCTL_ACT_CLK 	_IOW('A', 100, struct ioctl_active_clock)
#define IOCTL_BLINK_CLK _IOW('A', 101, struct ioctl_blink_clock)
#define IOCTL_SELF_DRAWER_CLK _IOW('A', 102, struct ioctl_self_drawer)

#define ACTIVE_CLK_ANA_CLOCK_EN	0x10
#define ACTIVE_CLK_BLINK_EN	0x01
#define ACTIVE_CLK_SELF_DRAWER_EN 0x01

struct ioctl_active_clock {
	u32 en;
	u32 interval;
	u32 time_hour;
	u32 time_min;
	u32 time_second;
	u32 time_ms;
	u32 pos_x;
	u32 pos_y;
};

struct ioctl_blink_clock {
	u32 en;
	u32 interval;
	u32 radius;
	u32 color;
	u32 line_width;
	u32 pos1_x;
	u32 pos1_y;
	u32 pos2_x;
	u32 pos2_y;
};

struct ioctl_self_drawer {
	u32 sd_line_color;
	u32 sd2_line_color;
	u32 sd_radius;
};

int ss_dsi_active_clk_HA6_init(struct samsung_display_driver_data *vdd);

#endif
//__LIVE_CLOCK_H__
