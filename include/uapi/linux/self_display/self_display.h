/*
 * linux/drivers/video/fbdev/exynos/aod/aod_drv_ioctl.h
 *
 * Source file for AOD Driver
 *
 * Copyright (c) 2016 Samsung Electronics
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

/*
 * SELF DISLAY interface
 *
 * Author: Samsung display driver team
 * Company:  Samsung Electronics
 */

 #ifndef _UAPI_LINUX_SELF_DISPLAY_H_
 #define _UAPI_LINUX_SELF_DISPLAY_H_

 /*
  * ioctl
  */
#define SELF_DISPLAY_IOCTL_MAGIC		'S'
#define IOCTL_SELF_MOVE_EN		_IOW(SELF_DISPLAY_IOCTL_MAGIC, 1, int)
#define IOCTL_SELF_MOVE_OFF		_IOW(SELF_DISPLAY_IOCTL_MAGIC, 2, int)
#define IOCTL_SET_TIME			_IOW(SELF_DISPLAY_IOCTL_MAGIC, 11, struct self_time_info)
#define IOCTL_SET_ICON			_IOW(SELF_DISPLAY_IOCTL_MAGIC, 21, struct self_icon_info)
#define IOCTL_SET_GRID			_IOW(SELF_DISPLAY_IOCTL_MAGIC, 31, struct self_grid_info)
#define IOCTL_SET_ANALOG_CLK		_IOW(SELF_DISPLAY_IOCTL_MAGIC, 41, struct self_analog_clk_info)
#define IOCTL_SET_DIGITAL_CLK		_IOW(SELF_DISPLAY_IOCTL_MAGIC, 51, struct self_digital_clk_info)
#define IOCTL_SELF_MAX			60

#define IMAGE_HEADER_SIZE	2
#define IMAGE_HEADER_SELF_ICON	"IC"
#define IMAGE_HEADER_ANALOG_CLK	"AC"
#define IMAGE_HEADER_DIGITAL_CLK "DC"

#define ROTATE_0	0
#define ROTATE_90	1
#define ROTATE_180	2
#define ROTATE_270	3

#define INTERVAL_100	0
#define INTERVAL_200	1
#define INTERVAL_500	2
#define INTERVAL_1000	3
#define INTERVAL_DEBUG	999

/*
 * data flag
 * This flag is used to distinguish what data will be passed and added at first 2byte of data.
 * ex ) ioctl write data : flag (2byte) + data (bytes)
 */
enum self_display_data_flag {
	FLAG_SELF_MOVE = 1,
	FLAG_SELF_MASK = 2,
	FLAG_SELF_ICON = 3,
	FLAG_SELF_GRID = 4,
	FLAG_SELF_ACLK = 5,
	FLAG_SELF_DCLK = 6,
	FLAG_SELF_VIDEO = 7,
	FLAG_SELF_DISP_MAX,
};

struct self_time_info {
	int cur_h;
	int cur_m;
	int cur_s;
	int cur_ms;
	int disp_24h;
	int interval;
};

struct self_icon_info {
	int en;
	int pos_x;
	int pos_y;
	int width;
	int height;
};

struct self_grid_info {
	int en;
	int s_pos_x;
	int s_pos_y;
	int e_pos_x;
	int e_pos_y;
};

struct self_analog_clk_info {
	int en;
	int pos_x;
	int pos_y;
	int rotate;
};

struct self_digital_clk_info {
	int en;
	int en_hh;
	int en_mm;
	int pos1_x;
	int pos1_y;
	int pos2_x;
	int pos2_y;
	int pos3_x;
	int pos3_y;
	int pos4_x;
	int pos4_y;
	int img_width;
	int img_height;
	int b_en;
	int b1_pos_x;
	int b1_pos_y;
	int b2_pos_x;
	int b2_pos_y;
	int b_color;
	int b_radius;
#if 0
	int b1_color;
	int b2_color;
	int b1_radius;
	int b2_radius;
#endif
};
#endif
