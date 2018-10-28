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

#ifndef _SS_FLASH_TABLE_DATA_COMMON_H_
#define _SS_FLASH_TABLE_DATA_COMMON_H_

#define ERASED_MMC_8BIT 0xFF
#define ERASED_MMC_16BIT 0xFFFF
#define MMC_CHECK_SUM_INIT 0x00
#define DIMMING_MODE_DEBUG_STRING 40

#define FLASH_GAMMA_DBG_BUF_SIZE 256

enum FLASH_OPERATION_BOOST{
	BOOST_DSI_CLK,
	BOOST_MNOC,
	BOOST_CPU, /* use only at factory checking, kernel booting use performance mode*/
	BOOST_MAX,
};

enum BR_FLASH_TABLE_INFO {
	BR_HBM_GAMMA,
	BR_HBM_AOR,
	BR_HBM_VINT,
	BR_HBM_ELVSS,
	BR_HBM_IRC,

	BR_NORMAL_GAMMA,
	BR_NORMAL_AOR,
	BR_NORMAL_VINT,
	BR_NORMAL_ELVSS,
	BR_NORMAL_IRC,

	BR_HMD_GAMMA,
	BR_HMD_AOR,
	BR_HMD_VINT,
	BR_HMD_ELVSS,
	BR_HMD_IRC,

	BR_INFO_MAX,
};

enum FLASH_GAMMA_BURN {
	FLASH_GAMMA_BURN_EMPTY,
	FLASH_GAMMA_BURN_WRITE,
	FLASH_GAMMA_BURN_ERASED = 0xFFFF,
};

struct flash_0xC8_register {
	unsigned char mtp_data[SZ_64]; /* 0xc8 ddi register read data */
	unsigned char flash_data[SZ_64]; /* flash read data */
	unsigned int check_sum_mtp_data; /* based on mtp_0xc8_data */
	unsigned int check_sum_flash_data; /* flash read data */
	unsigned int check_sum_cal_data; /* based on flash_0xc8_data */
};

struct flash_mcd_value {
	int flash_MCD1_R;
	int flash_MCD2_R;
	int flash_MCD1_L;
	int flash_MCD2_L;
};

struct hbm_info {
	int *candela_table; /* dtsi table */

	unsigned char **gamma;
	unsigned char **aor;
	unsigned char **vint;
	unsigned char **elvss;
	unsigned char **irc;
};

struct normal_info {
	int *candela_table; /* dtsi table */

	unsigned char **gamma;
	unsigned char **aor;
	unsigned char **vint;
	unsigned char **elvss;
	unsigned char **irc;
};

struct hmd_info {
	int *candela_table; /* dtsi table */

	unsigned char **gamma;
	unsigned char **aor;
};

struct flash_only_data {
	enum FLASH_GAMMA_BURN write_check;

	int force_update;
	int init_done; 	/* To check reading operation done */

	unsigned int check_sum_flash_data;
	unsigned int check_sum_cal_data;

	struct flash_0xC8_register c8_register; /* 0xC8 register related flash data */

	struct flash_mcd_value mcd; /*mcd flash data */
};

#include "ss_interpolation_common.h"

struct brightness_data_info {
	/*
		Currently only HBM(12 step) NORMAL(74 step) are available.
		Consider HBM(more 60 step) NORMAL(512 step) could be supported.
	*/
	enum INTERPOLATION_MODE itp_mode;

	int vfp, vbp, resolution;

	int br_data_size;
	unsigned char *br_data_raw;

	unsigned char hbm_read_buffer[SZ_64]; /* normally 0xB3 register */

	/* brightness_raw data parsing information */
	struct hbm_info hbm;
	struct normal_info normal;
	struct hmd_info hmd;

	struct flash_only_data flash_data;

	/* dsi, mnoc, cpu boosting */
	DECLARE_BITMAP(br_info_select, BR_INFO_MAX);

	/* dsi, mnoc, cpu boosting */
	DECLARE_BITMAP(flash_br_boosting, BOOST_MAX);
};

void flash_br_work_func(struct work_struct *work); /* For read flash data */
void table_br_func(struct samsung_display_driver_data *vdd); /* For table data */

char flash_read_one_byte(struct samsung_display_driver_data *vdd, int addr);

void set_up_br_info(struct samsung_display_driver_data *vdd);

int br_interpolation_generate_event(struct samsung_display_driver_data *vdd, enum GEN_INTERPOLATION_EVENT event, char *buf);
void debug_br_info_log(struct samsung_display_driver_data *vdd);

#endif
