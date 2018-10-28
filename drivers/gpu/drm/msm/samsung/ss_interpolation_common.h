 /* =================================================================
 *
 *
 *	Description:  samsung display common file
 *
 *	Author:  jb09.kim@samsung.com
 *	Company:  Samsung Electronics
 *
 * ================================================================
 */
/*
<one line to give the program's name and a brief idea of what it does.>
Copyright (C) 2012, Samsung Electronics. All rights reserved.

 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 and
 * only version 2 as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 */

#ifndef _SS_INTERPOLATION_COMMON_H_
#define _SS_INTERPOLATION_COMMON_H_

#include "ss_dsi_panel_common.h"

#define AOR_HEX_STRING_CNT (sizeof(unsigned int))

#define IRC_START_VERSION_1 8

#define V255_START (1)
#define V0_VT_BYTE (3)

enum GEN_INTERPOLATION_EVENT {
	GEN_HBM_GAMMA,
	GEN_NORMAL_GAMMA,
	GEN_HMD_GAMMA,
	GEN_NORMAL_INTERPOLATION_GAMMA,
	GEN_HBM_INTERPOLATION_GAMMA,

	GEN_HBM_AOR,
	GEN_NORMAL_AOR,
	GEN_HMD_AOR,
	GEN_NORMAL_INTERPOLATION_AOR,
	GEN_HBM_INTERPOLATION_AOR,

	GEN_HBM_VINT,
	GEN_NORMAL_VINT,
	GEN_HMD_VINT,
	GEN_NORMAL_INTERPOLATION_VINT,
	GEN_HBM_INTERPOLATION_VINT,

	GEN_HBM_ELVSS,
	GEN_NORMAL_ELVSS,
	GEN_HMD_ELVSS,
	GEN_NORMAL_INTERPOLATION_ELVSS,
	GEN_HBM_INTERPOLATION_ELVSS,

	GEN_HBM_IRC,
	GEN_NORMAL_IRC,
	GEN_HMD_IRC,
	GEN_NORMAL_INTERPOLATION_IRC,
	GEN_HBM_INTERPOLATION_IRC,

	GEN_INTERPOLATION_EVENT_MAX,
};

enum INTERPOLATION_ELVSS_TEMPERATURE {
	INTERPOLATION_ELVSS_HIGH_TEMP,
	INTERPOLATION_ELVSS_MID_TEMP,
	INTERPOLATION_ELVSS_LOW_TEMP,
	INTERPOLATION_ELVSS_MAX_TEMP
};

enum INTERPOLATION_MODE {
	TABLE_INTERPOLATION,
	FLASH_INTERPOLATION,
	INTERPOLATION_MAX,
};

enum IRC_COMPENSATION_V1 {
	IRC_64_V1,
	IRC_128_V1,
	IRC_192_V1,
	IRC_V1_MAX,
};

enum {
	V255_BLUE_BIT8,
	V255_GREEN_BIT8,
	V255_RED_BIT8,
	V255_SIGN_BIT_MAX,
};

enum {
	RED_ORDER,
	GREEN_ORDER,
	BLUE_ORDER,
	COLOR_ORDER_MAX,
};

enum ss_dimming_mode {
	SS_FLASH_DIMMING_MODE,
	SS_S_DIMMING_MODE,
	SS_S_DIMMING_EXIT_MODE_1,
	SS_S_DIMMING_EXIT_MODE_2,
	SS_A_DIMMING_MODE,
	DIMMING_MODE_MAX,
};

struct ss_interpolation_brightness_table {
	/* THIS IS DISPLAY LAB SERVE TABLE */
	unsigned int platform;
	unsigned int steps;
	unsigned int lux_mode; /* candela */
};

struct ss_hbm_interpolation_br_table {
	unsigned int platform_level_x10000;
	unsigned int lux_mode; /* (interpolation_br_x10000 / 1000) */
	unsigned int interpolation_br_x10000; /* real interpolation candela(real candela * 1000) */
};

struct ss_hbm_interpolation {
	/*
		It support only HBM interpolation gamma.
		Other thing(aor, vint, elvss, irc) are set by flash gamma data or interpolation table data
	*/
	unsigned int brightness_step; /* total interpolation step size */
	struct hbm_candela_map_table *map_table; /* To update pac_hbm_candela_map_table */

	/*
	*	interpolation  output
	*/
	struct ss_hbm_interpolation_br_table *br_table;
	unsigned char **gamma;
	unsigned char **irc;
};

struct ss_normal_interpolation_br_aor_table {
	unsigned int platform_level_x10000;
	unsigned int lux_mode; /* base candela for calculating interpolation not real candela*/
	unsigned int interpolation_br_x10000; /* real interpolation candela(real candela * 1000) */

	unsigned int aor_percent_x10000;

	/*
		Currently 2 byte(15536) aor resolution.
		4byte(32bit, 64bit system)resolution is sufficient to support more detail aor resolution.
	*/
	unsigned int aor_hex;
	unsigned char aor_hex_string[AOR_HEX_STRING_CNT];

	enum ss_dimming_mode dimming_mode;
};

struct ss_normal_interpolation {
	unsigned int brightness_step;	/* total interpolation step size */
	struct candela_map_table *map_table; /* To update pac_candela_map_table */

	/*
	*	interpolation  output
	*/
	struct ss_normal_interpolation_br_aor_table *br_aor_table;
	unsigned char **irc;
};

struct ss_interpolation {
	struct ss_normal_interpolation normal;
	struct ss_hbm_interpolation hbm;
};

void set_up_interpolation(struct samsung_display_driver_data *vdd,
	struct ss_interpolation_brightness_table *normal_table, int normal_table_size,
	struct ss_interpolation_brightness_table *hbm_table, int hbm_table_size);

void debug_interpolation_log(struct samsung_display_driver_data *vdd);

#endif
