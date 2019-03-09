/*
 * Copyright (c) 2017 Samsung Electronics Co., Ltd.
 *	      http://www.samsung.com/
 *
 * COPR :
 * Author: QC LCD driver <kr0124.cho@samsung.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#ifndef __SS_COPR_COMMON_H__
#define __SS_COPR_COMMON_H__

#define MAX_COPR_CNT 36000

struct COPR_CMD {
	char CNT_RE;
	char COPR_ILC;
	char COPR_GAMMA;
	char COPR_EN;
	int COPR_ER;
	int COPR_EG;
	int COPR_EB;
	int COPR_ERC;
	int COPR_EGC;
	int COPR_EBC;
	int MAX_CNT;
	char ROI_ON;
	int ROI_X_S;
	int ROI_Y_S;
	int ROI_X_E;
	int ROI_Y_E;
};

struct COPR_ROI {
	int ROI_X_S;
	int ROI_Y_S;
	int ROI_X_E;
	int ROI_Y_E;
};

struct COPR_ROI_OPR {
	int R_OPR;
	int G_OPR;
	int B_OPR;
};

enum COPR_CD_INDEX {
	COPR_CD_INDEX_0,	/* for copr show - SSRM */
	COPR_CD_INDEX_1,	/* for brt_avg show - mDNIe */
	MAX_COPR_CD_INDEX,
};

struct COPR_CD {
	s64 cd_sum;
	int cd_avr;

	ktime_t cur_t;
	ktime_t last_t;
	s64 total_t;
};

struct COPR {
	int copr_on;

	char read_addr;
	int read_size;

	int copr_ready;
	int current_cnt;
	int current_copr;
	int avg_copr;
	int sliding_current_cnt;
	int sliding_avg_copr;
	int comp_copr;

/*
	s64 cd_sum;
	int cd_avr;

	ktime_t cur_t;
	ktime_t last_t;
	s64 total_t;
*/
	struct mutex copr_lock;
	struct mutex copr_val_lock;
	struct workqueue_struct *read_copr_wq;
	struct work_struct read_copr_work;

	struct COPR_CD copr_cd[MAX_COPR_CD_INDEX];
	struct COPR_CMD cmd;
	struct COPR_ROI roi[32];
	struct COPR_ROI_OPR roi_opr[32];
	int roi_cnt;
};

void ss_copr_set_cmd(struct COPR_CMD copr_cmd);
int ss_copr_get_cmd(struct samsung_display_driver_data *vdd);
void ss_copr_enable(struct samsung_display_driver_data *vdd, int enable);
int ss_copr_read(struct samsung_display_driver_data *vdd);
void ss_set_copr_sum(struct samsung_display_driver_data *vdd, enum COPR_CD_INDEX idx);
void ss_copr_reset_cnt(struct samsung_display_driver_data *vdd);
int ss_copr_get_roi_opr(void);

void ss_copr_init(void);

#endif // __SS_COPR_COMMON_H__
