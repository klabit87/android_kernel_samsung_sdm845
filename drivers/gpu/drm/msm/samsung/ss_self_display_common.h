/*
 * Copyright (c) 2017 Samsung Electronics Co., Ltd.
 *	      http://www.samsung.com/
 *
 * DDI operation : self clock, self mask, self icon.. etc.
 * Author: QC LCD driver <kr0124.cho@samsung.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#ifndef __SS_DDI_OP_COMMON_H__
#define __SS_DDI_OP_COMMON_H__

#include <linux/miscdevice.h>
#include <linux/fs.h>
#include <linux/file.h>
#include <linux/self_display/self_display.h>

/* payload should be 16-bytes aligned */
#define CMD_ALIGN 16

#define MAX_PAYLOAD_SIZE 200

struct self_display_op {
	bool select;
	bool on;
	u32 wpos;
	u64 wsize;

	char *img_buf;
	u32 img_size;

	int img_checksum;
};

struct self_display_debug {
	u32 SI_X_O;
	u32 SI_Y_O;
	u32 MEM_SUM_O;
	u32 SM_SUM_O;
	u32 MEM_WR_DONE_O;
	u32 mem_wr_icon_pk_err_o;
	u32 mem_wr_var_pk_err_o;
	u32 sv_dec_err_fg_o;
	u32 scd_dec_err_fg_o;
	u32 si_dec_err_fg_o;
	u32 CUR_HH_O;
	u32 CUR_MM_O;
	u32 CUR_MSS_O;
	u32 CUR_SS_O;
	u32 SM_WR_DONE_O;
	u32 SM_PK_ERR_O;
	u32 SM_DEC_ERR_O;
};

struct self_display {
	struct miscdevice dev;

	int is_support;
	int factory_support;
	int on;
	int file_open;
	int time_set;

	struct self_time_info st_info;
	struct self_icon_info si_info;
	struct self_grid_info sg_info;
	struct self_analog_clk_info sa_info;
	struct self_digital_clk_info sd_info;

	struct mutex vdd_self_move_lock;
	struct mutex vdd_self_mask_lock;
	struct mutex vdd_self_aclock_lock;
	struct mutex vdd_self_dclock_lock;
	struct mutex vdd_self_icon_grid_lock;

	struct self_display_op operation[FLAG_SELF_DISP_MAX];

	struct self_display_debug debug;
//	struct samsung_display_dtsi_data *dtsi_data;
};

enum dsi_cmd_set_type;
struct samsung_display_dtsi_data;

void self_move_on(int enable);

void self_mask_img_write(void);
void self_mask_on(int enable);

int self_time_set(int from_self_move);

int self_icon_set(void);
int self_grid_set(void);
void self_icon_img_write(void);

void self_aclock_on(int enable);
void self_aclock_img_write(void);
int self_aclock_set(void);

void self_dclock_on(int enable);
void self_dclock_img_write(void);
int self_dclock_set(void);

void self_blinking_on(int enable);

int self_display_aod_enter(void);
int self_display_aod_exit(void);

int self_display_debug(void);

void make_self_dispaly_img_cmds(enum dsi_cmd_set_type cmd, u32 flag);
int self_display_init(struct samsung_display_driver_data *vdd);

#endif // __SS_DDI_OP_COMMON_H__
