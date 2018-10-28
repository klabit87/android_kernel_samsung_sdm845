/*
 * =================================================================
 *
 *
 *	Description:  samsung display common file
 *
 *	Author: jb09.kim
 *	Company:  Samsung Electronics
 *
 * ================================================================
 */
/*
 * <one line to give the program's name and a brief idea of what it does.>
 * Copyright (C) 2012, Samsung Electronics. All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 and
 * only version 2 as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#ifndef _SS_DSI_MDNIE_S6E3HA6_AMB577MQ01_H_
#define _SS_DSI_MDNIE_S6E3HA6_AMB577MQ01_H_

#include "ss_dsi_mdnie_lite_common.h"

#define MDNIE_COLOR_BLINDE_CMD_OFFSET 32

#define ADDRESS_SCR_WHITE_RED   0x32
#define ADDRESS_SCR_WHITE_GREEN 0x34
#define ADDRESS_SCR_WHITE_BLUE  0x36

#define MDNIE_STEP1_INDEX 1
#define MDNIE_STEP2_INDEX 2
#define MDNIE_STEP3_INDEX 3

#define MDNIE_TRANS_DIMMING_DATA_INDEX	17

/* TODO: This file came from ss_dsi_mdnie_S6E3HA6_AMS622MR01.h file.
 * It should be fixed the value...
 */
static char level_1_key_on[] = {
	0xF0,
	0x5A, 0x5A
};

static char level_1_key_off[] = {
	0xF0,
	0xA5, 0xA5
};

static char adjust_ldu_data_1[] = {
	0xff, 0xff, 0xff,
	0xf6, 0xfa, 0xff,
	0xf4, 0xf8, 0xff,
	0xe9, 0xf2, 0xff,
	0xe2, 0xef, 0xff,
	0xd4, 0xe8, 0xff,
};

static char adjust_ldu_data_2[] = {
	0xff, 0xfa, 0xf1,
	0xff, 0xfd, 0xf8,
	0xff, 0xfd, 0xfa,
	0xfa, 0xfd, 0xff,
	0xf5, 0xfb, 0xff,
	0xe5, 0xf3, 0xff,
};

static char *adjust_ldu_data[MAX_MODE] = {
	adjust_ldu_data_2,
	adjust_ldu_data_2,
	adjust_ldu_data_2,
	adjust_ldu_data_1,
	adjust_ldu_data_1,
};

static char night_mode_data[] = {
	0x00, 0xff, 0xfa, 0x00, 0xf1, 0x00, 0xff, 0x00, 0x00, 0xfa, 0xf1, 0x00, 0xff, 0x00, 0xfa, 0x00, 0x00, 0xf1, 0xff, 0x00, 0xfa, 0x00, 0xf1, 0x00, /* 6500K */
	0x00, 0xff, 0xf8, 0x00, 0xea, 0x00, 0xff, 0x00, 0x00, 0xf8, 0xea, 0x00, 0xff, 0x00, 0xf8, 0x00, 0x00, 0xea, 0xff, 0x00, 0xf8, 0x00, 0xea, 0x00, /* 6100K */
	0x00, 0xff, 0xf5, 0x00, 0xe2, 0x00, 0xff, 0x00, 0x00, 0xf5, 0xe2, 0x00, 0xff, 0x00, 0xf5, 0x00, 0x00, 0xe2, 0xff, 0x00, 0xf5, 0x00, 0xe2, 0x00, /* 5700K */
	0x00, 0xff, 0xf2, 0x00, 0xda, 0x00, 0xff, 0x00, 0x00, 0xf2, 0xda, 0x00, 0xff, 0x00, 0xf2, 0x00, 0x00, 0xda, 0xff, 0x00, 0xf2, 0x00, 0xda, 0x00, /* 5300K */
	0x00, 0xff, 0xee, 0x00, 0xd0, 0x00, 0xff, 0x00, 0x00, 0xee, 0xd0, 0x00, 0xff, 0x00, 0xee, 0x00, 0x00, 0xd0, 0xff, 0x00, 0xee, 0x00, 0xd0, 0x00, /* 4900K */
	0x00, 0xff, 0xea, 0x00, 0xc5, 0x00, 0xff, 0x00, 0x00, 0xea, 0xc5, 0x00, 0xff, 0x00, 0xea, 0x00, 0x00, 0xc5, 0xff, 0x00, 0xea, 0x00, 0xc5, 0x00, /* 4500K */
	0x00, 0xff, 0xe5, 0x00, 0xb7, 0x00, 0xff, 0x00, 0x00, 0xe5, 0xb7, 0x00, 0xff, 0x00, 0xe5, 0x00, 0x00, 0xb7, 0xff, 0x00, 0xe5, 0x00, 0xb7, 0x00, /* 4100K */
	0x00, 0xff, 0xde, 0x00, 0xa5, 0x00, 0xff, 0x00, 0x00, 0xde, 0xa5, 0x00, 0xff, 0x00, 0xde, 0x00, 0x00, 0xa5, 0xff, 0x00, 0xde, 0x00, 0xa5, 0x00, /* 3700K */
	0x00, 0xff, 0xd8, 0x00, 0x93, 0x00, 0xff, 0x00, 0x00, 0xd8, 0x93, 0x00, 0xff, 0x00, 0xd8, 0x00, 0x00, 0x93, 0xff, 0x00, 0xd8, 0x00, 0x93, 0x00, /* 3300K */
	0x00, 0xff, 0xcf, 0x00, 0x7f, 0x00, 0xff, 0x00, 0x00, 0xcf, 0x7f, 0x00, 0xff, 0x00, 0xcf, 0x00, 0x00, 0x7f, 0xff, 0x00, 0xcf, 0x00, 0x7f, 0x00, /* 2900K */
	0x00, 0xff, 0xc3, 0x00, 0x6a, 0x00, 0xff, 0x00, 0x00, 0xc3, 0x6a, 0x00, 0xff, 0x00, 0xc3, 0x00, 0x00, 0x6a, 0xff, 0x00, 0xc3, 0x00, 0x6a, 0x00, /* 2500K */
};

static char DSI_BYPASS_MDNIE_1[] = {
	//start
	0xDF,
	0x00, //linear_on ascr_skin_on 0000 0000
	0x6a, //ascr_skin_cb
	0x9a, //ascr_skin_cr
	0x25, //ascr_dist_up
	0x1a, //ascr_dist_down
	0x16, //ascr_dist_right
	0x2a, //ascr_dist_left
	0x00, //ascr_div_up 20
	0x37,
	0x5a,
	0x00, //ascr_div_down
	0x4e,
	0xc5,
	0x00, //ascr_div_right
	0x5d,
	0x17,
	0x00, //ascr_div_left
	0x30,
	0xc3,
	0xff, //ascr_skin_Rr
	0x00, //ascr_skin_Rg
	0x00, //ascr_skin_Rb
	0xff, //ascr_skin_Yr
	0xff, //ascr_skin_Yg
	0x00, //ascr_skin_Yb
	0xff, //ascr_skin_Mr
	0x00, //ascr_skin_Mg
	0xff, //ascr_skin_Mb
	0xff, //ascr_skin_Wr
	0xff, //ascr_skin_Wg
	0xff, //ascr_skin_Wb
	0x00, //ascr_Cr
	0xff, //ascr_Rr
	0xff, //ascr_Cg
	0x00, //ascr_Rg
	0xff, //ascr_Cb
	0x00, //ascr_Rb
	0xff, //ascr_Mr
	0x00, //ascr_Gr
	0x00, //ascr_Mg
	0xff, //ascr_Gg
	0xff, //ascr_Mb
	0x00, //ascr_Gb
	0xff, //ascr_Yr
	0x00, //ascr_Br
	0xff, //ascr_Yg
	0x00, //ascr_Bg
	0x00, //ascr_Yb
	0xff, //ascr_Bb
	0xff, //ascr_Wr
	0x00, //ascr_Kr
	0xff, //ascr_Wg
	0x00, //ascr_Kg
	0xff, //ascr_Wb
	0x00, //ascr_Kb
	0x0F, //edge_ctrl
	0x00, //edge_radius 12
	0x6F,
	0x05, //edge_aa_factor
	0x00, //edge_r
	0x00, //edge_g
	0x00, //edge_b
	0x00, //edge_x 11
	0x70,
	0x00, //edge_y 12
	0x70,
	0x44, //edge_coef_a edge_coef_b 0000 0000
};

static char DSI_BYPASS_MDNIE_2[] = {
	0xDE,
	0x00, //gamut_gamma_on gamut_blk_shift 0000 0000
	0x40, //gamut_scale
	0x04, //gamut_r1
	0x00,
	0x00, //gamut_r2
	0x00,
	0x00, //gamut_r3
	0x00,
	0x00, //gamut_g1
	0x00,
	0x04, //gamut_g2
	0x00,
	0x00, //gamut_g3
	0x00,
	0x00, //gamut_b1
	0x00,
	0x00, //gamut_b2
	0x00,
	0x04, //gamut_b3
	0x00,
	0x00, //slce_on cadrx_en 0000 0000
	0x00, //glare_on
	0x00, //lce_gain 000 0000
	0x24, //lce_color_gain 00 0000
	0x96, //lce_min_ref_offset
	0xb3, //lce_illum_gain
	0x01, //lce_ref_offset 9
	0x0e,
	0x01, //lce_ref_gain 9
	0x00,
	0x77, //lce_block_size h v 0000 0000
	0x03, //lce_dark_th 000
	0x07, //lce_reduct_slope 0000
	0x40, //lce_black cc0 cc1 color_th 0 0 0 0000
	0x08, //lce_large_w
	0x04, //lce_med_w
	0x04, //lce_small_w
	0x00, //cadrx_dit_en shadow_corr_en ref_ctrl_en 0 0 0
	0x00, //cadrx_gain
	0x0d, //cadrx_low_intensity_th
	0xe1, //cadrx_high_intensity_th
	0x07, //cadrx_ui_zerobincnt_th
	0x29, //cadrx_ui_ratio_th
	0x04,
	0x0f, //cadrx_entire_freq
	0xa0,
	0x00,
	0x03, //cadrx_peak_th_in_freq
	0x7a,
	0xa0,
	0x00, //cadrx_high_peak_th_in_freq
	0x25,
	0x1c,
	0x24, //cadrx_dit_color_gain
	0x00, //cadrx_dit_illum_a0
	0x48, //cadrx_dit_illum_a1
	0xea, //cadrx_dit_illum_a2
	0xfa, //cadrx_dit_illum_a3
	0x00, //cadrx_dit_illum_b0
	0x10,
	0x00, //cadrx_dit_illum_b1
	0x10,
	0x00, //cadrx_dit_illum_b2
	0x9c,
	0x01, //cadrx_dit_illum_b3
	0x00,
	0x00, //cadrx_dit_illum_slope1
	0x00,
	0x00, //cadrx_dit_illum_slope2
	0xdc,
	0x03, //cadrx_dit_illum_slope3
	0xff,
	0x40, //cadrx_ui_illum_a1
	0x80, //cadrx_ui_illum_a2
	0xc0, //cadrx_ui_illum_a3
	0x00, //cadrx_ui_illum_offset1
	0x9a,
	0x00, //cadrx_ui_illum_offset2
	0x9a,
	0x00, //cadrx_ui_illum_offset3
	0x9a,
	0x00, //cadrx_ui_illum_offset4
	0x9a,
	0x00, //cadrx_ui_illum_slope1
	0x00,
	0x00, //cadrx_ui_illum_slope2
	0x00,
	0x00, //cadrx_ui_illum_slope3
	0x00,
	0x00, //cadrx_ui_illum_slope4
	0x00,
	0x40, //cadrx_ui_ref_a1
	0x80, //cadrx_ui_ref_a2
	0xc0, //cadrx_ui_ref_a3
	0x01, //cadrx_ui_ref_offset1
	0x0e,
	0x01, //cadrx_ui_ref_offset2
	0x0e,
	0x01, //cadrx_ui_ref_offset3
	0x0e,
	0x01, //cadrx_ui_ref_offset4
	0x0e,
	0x00, //cadrx_ui_ref_slope1
	0x00,
	0x00, //cadrx_ui_ref_slope2
	0x00,
	0x00, //cadrx_ui_ref_slope3
	0x00,
	0x00, //cadrx_ui_ref_slope4
	0x00,
	0x01, //cadrx_reg_ref_c1_offset
	0x0e,
	0x01, //cadrx_reg_ref_c2_offset
	0x3a,
	0x01, //cadrx_reg_ref_c3_offset
	0x44,
	0x00, //cadrx_reg_ref_c1_slope
	0x01,
	0x00, //cadrx_reg_ref_c2_slope
	0x43,
	0x00, //cadrx_reg_ref_c3_slope
	0x4b,
	0x00, //cadrx_sc_reg_a1
	0x18,
	0x00, //cadrx_sc_reg_b1
	0x1c,
	0x01, //cadrx_sc_k1_int
	0xff,
	0x01, //cadrx_sc_k2_int
	0xff,
	0x03, //cadrx_sc_w1_int
	0xff,
	0xe8,
	0x03, //cadrx_sc_w2_int
	0xff,
	0xe8,
	0x00, //flicker_w_en
	0x14, //flicker_w
	0x01, //flicker_trans_en
	0x04, //flicker_trans_slope
	0x00, //bl_en
	0x03, //bl_hbs_th
	0x10, //bl_lbs_th
	0xC8, //bl_illum_gain
	0x01, //bl_reg_gain 9
	0x2C,
	0x00, //bl_le_diff
	0x00, //bl_cadrx_gain
	0x04, //bl_trans_slope
	0x00, //le_en
	0x40, //le_diff
	0x03, //le_lmax 10
	0xB6,
	0x23, //le_p
	0x24, //le_w_tmf
	0x00, //le_w_struct_diff 9
	0x72,
	0x01, //le_w_struct_diff_gain
	0x00, //le_w_luminance 9
	0x0D,
	0x00, //le_luminance_slope 10
	0x04,
	0x80, //glare_blk_avg_th 0000 0000
	0x00, //glare_luma_gain 9
	0x28,
	0xa0, //glare_luma_ctl_start 0000 0000
	0x01, //glare_luma_th 0000 0000
	0x00, //glare_luma_step 9
	0x01,
	0x00, //glare_uni_luma_gain 9
	0x01,
	0x00, //glare_blk_max_th 0000 0000
	0x00, //glare_blk_max_w 0000 0000
	0x10, //glare_blk_max_curve blk_max_sh 0 0000
	0x00, //glare_con_ext_max_th 0000 0000
	0x00, //glare_con_ext_max_w 0000 0000
	0x30, //glare_con_ext_max_sign con_ext_max_curve con_ext_max_sh 0 0 0000
	0x00, //glare_max_cnt_th 0000
	0x00, //glare_max_cnt_th 0000 0000
	0x00, //glare_max_cnt_w 0000 0000
	0x30, //glare_max_cnt_sign max_cnt_curve max_cnt_sh 0 0 0000
	0x00, //glare_y_avg_th 0000 0000
	0x00, //glare_y_avg_w 0000 0000
	0x30, //glare_y_avg_sign y_avg_curve y_avg_sh 0 0 0000
	0x00, //glare_max_cnt_th_dmc 0000
	0x00, //glare_max_cnt_th_dmc 0000 0000
	0x00, //nr fa de cs gamma 0 0000
	0xff, //nr_mask_th
	0x00, //de_gain 10
	0x00,
	0x02, //de_maxplus 11
	0x00,
	0x02, //de_maxminus 11
	0x00,
	0x14, //fa_edge_th
	0x00, //fa_step_p 13
	0x01,
	0x00, //fa_step_n 13
	0x01,
	0x00, //fa_max_de_gain 13
	0x10,
	0x00, //fa_pcl_ppi 14
	0x00,
	0x28, //fa_os_cnt_10_co
	0x3c, //fa_os_cnt_20_co
	0x04, //fa_edge_cnt_weight
	0x0f, //fa_avg_y_weight
	0x01, //cs_gain 10
	0x00,
	0x00, //curve_0
	0x10, //curve_1
	0x20, //curve_2
	0x30, //curve_3
	0x40, //curve_4
	0x50, //curve_5
	0x60, //curve_6
	0x70, //curve_7
	0x80, //curve_8
	0x90, //curve_9
	0xa0, //curve_10
	0xb0, //curve_11
	0xc0, //curve_12
	0xd0, //curve_13
	0xe0, //curve_14
	0xf0, //curve_15
	0x01, //curve_16
	0x00,
	0x00, //curve_offset 10
	0x00,
	0x00, //curve_low_x
	0x00, //blank
	0x00, //curve_low_y
};

static char DSI_BYPASS_MDNIE_3[] = {
	0xDD,
	0x01, //mdnie_en
	0x00, //mask 00 0000
	0x03, //edge 00
	0x00, //ascr algo aolce gamut 00 00 00 00
	0xff, //partial_en1
	0xff, //partial_en2
	0x00, //roi_en
	0x00, //roi_block
	0x00, //roi_sx
	0x00, //roi_sx
	0x00, //roi_sy
	0x00, //roi_sy
	0x00, //roi_ex
	0x00, //roi_ex
	0x00, //roi_ey
	0x00, //roi_ey
	0x00, //trans_on trans_block 0 000 0000
	0x00, //trans_slope
	0x00, //trans_interval
	//end
};

static char DSI_NEGATIVE_MDNIE_1[] = {
	//start
	0xDF,
	0x01, //linear_on ascr_skin_on 0000 0000
	0x6a, //ascr_skin_cb
	0x9a, //ascr_skin_cr
	0x25, //ascr_dist_up
	0x1a, //ascr_dist_down
	0x16, //ascr_dist_right
	0x2a, //ascr_dist_left
	0x00, //ascr_div_up 20
	0x37,
	0x5a,
	0x00, //ascr_div_down
	0x4e,
	0xc5,
	0x00, //ascr_div_right
	0x5d,
	0x17,
	0x00, //ascr_div_left
	0x30,
	0xc3,
	0x00, //ascr_skin_Rr
	0xff, //ascr_skin_Rg
	0xff, //ascr_skin_Rb
	0x00, //ascr_skin_Yr
	0x00, //ascr_skin_Yg
	0xff, //ascr_skin_Yb
	0x00, //ascr_skin_Mr
	0xff, //ascr_skin_Mg
	0x00, //ascr_skin_Mb
	0x00, //ascr_skin_Wr
	0x00, //ascr_skin_Wg
	0x00, //ascr_skin_Wb
	0xff, //ascr_Cr
	0x00, //ascr_Rr
	0x00, //ascr_Cg
	0xff, //ascr_Rg
	0x00, //ascr_Cb
	0xff, //ascr_Rb
	0x00, //ascr_Mr
	0xff, //ascr_Gr
	0xff, //ascr_Mg
	0x00, //ascr_Gg
	0x00, //ascr_Mb
	0xff, //ascr_Gb
	0x00, //ascr_Yr
	0xff, //ascr_Br
	0x00, //ascr_Yg
	0xff, //ascr_Bg
	0xff, //ascr_Yb
	0x00, //ascr_Bb
	0x00, //ascr_Wr
	0xff, //ascr_Kr
	0x00, //ascr_Wg
	0xff, //ascr_Kg
	0x00, //ascr_Wb
	0xff, //ascr_Kb
	0x0F, //edge_ctrl
	0x00, //edge_radius 12
	0x6F,
	0x05, //edge_aa_factor
	0x00, //edge_r
	0x00, //edge_g
	0x00, //edge_b
	0x00, //edge_x 11
	0x70,
	0x00, //edge_y 12
	0x70,
	0x44, //edge_coef_a edge_coef_b 0000 0000
};

static char DSI_NEGATIVE_MDNIE_2[] = {
	0xDE,
	0x00, //gamut_gamma_on gamut_blk_shift 0000 0000
	0x40, //gamut_scale
	0x04, //gamut_r1
	0x00,
	0x00, //gamut_r2
	0x00,
	0x00, //gamut_r3
	0x00,
	0x00, //gamut_g1
	0x00,
	0x04, //gamut_g2
	0x00,
	0x00, //gamut_g3
	0x00,
	0x00, //gamut_b1
	0x00,
	0x00, //gamut_b2
	0x00,
	0x04, //gamut_b3
	0x00,
	0x00, //slce_on cadrx_en 0000 0000
	0x00, //glare_on
	0x00, //lce_gain 000 0000
	0x24, //lce_color_gain 00 0000
	0x96, //lce_min_ref_offset
	0xb3, //lce_illum_gain
	0x01, //lce_ref_offset 9
	0x0e,
	0x01, //lce_ref_gain 9
	0x00,
	0x77, //lce_block_size h v 0000 0000
	0x03, //lce_dark_th 000
	0x07, //lce_reduct_slope 0000
	0x40, //lce_black cc0 cc1 color_th 0 0 0 0000
	0x08, //lce_large_w
	0x04, //lce_med_w
	0x04, //lce_small_w
	0x00, //cadrx_dit_en shadow_corr_en ref_ctrl_en 0 0 0
	0x00, //cadrx_gain
	0x0d, //cadrx_low_intensity_th
	0xe1, //cadrx_high_intensity_th
	0x07, //cadrx_ui_zerobincnt_th
	0x29, //cadrx_ui_ratio_th
	0x04,
	0x0f, //cadrx_entire_freq
	0xa0,
	0x00,
	0x03, //cadrx_peak_th_in_freq
	0x7a,
	0xa0,
	0x00, //cadrx_high_peak_th_in_freq
	0x25,
	0x1c,
	0x24, //cadrx_dit_color_gain
	0x00, //cadrx_dit_illum_a0
	0x48, //cadrx_dit_illum_a1
	0xea, //cadrx_dit_illum_a2
	0xfa, //cadrx_dit_illum_a3
	0x00, //cadrx_dit_illum_b0
	0x10,
	0x00, //cadrx_dit_illum_b1
	0x10,
	0x00, //cadrx_dit_illum_b2
	0x9c,
	0x01, //cadrx_dit_illum_b3
	0x00,
	0x00, //cadrx_dit_illum_slope1
	0x00,
	0x00, //cadrx_dit_illum_slope2
	0xdc,
	0x03, //cadrx_dit_illum_slope3
	0xff,
	0x40, //cadrx_ui_illum_a1
	0x80, //cadrx_ui_illum_a2
	0xc0, //cadrx_ui_illum_a3
	0x00, //cadrx_ui_illum_offset1
	0x9a,
	0x00, //cadrx_ui_illum_offset2
	0x9a,
	0x00, //cadrx_ui_illum_offset3
	0x9a,
	0x00, //cadrx_ui_illum_offset4
	0x9a,
	0x00, //cadrx_ui_illum_slope1
	0x00,
	0x00, //cadrx_ui_illum_slope2
	0x00,
	0x00, //cadrx_ui_illum_slope3
	0x00,
	0x00, //cadrx_ui_illum_slope4
	0x00,
	0x40, //cadrx_ui_ref_a1
	0x80, //cadrx_ui_ref_a2
	0xc0, //cadrx_ui_ref_a3
	0x01, //cadrx_ui_ref_offset1
	0x0e,
	0x01, //cadrx_ui_ref_offset2
	0x0e,
	0x01, //cadrx_ui_ref_offset3
	0x0e,
	0x01, //cadrx_ui_ref_offset4
	0x0e,
	0x00, //cadrx_ui_ref_slope1
	0x00,
	0x00, //cadrx_ui_ref_slope2
	0x00,
	0x00, //cadrx_ui_ref_slope3
	0x00,
	0x00, //cadrx_ui_ref_slope4
	0x00,
	0x01, //cadrx_reg_ref_c1_offset
	0x0e,
	0x01, //cadrx_reg_ref_c2_offset
	0x3a,
	0x01, //cadrx_reg_ref_c3_offset
	0x44,
	0x00, //cadrx_reg_ref_c1_slope
	0x01,
	0x00, //cadrx_reg_ref_c2_slope
	0x43,
	0x00, //cadrx_reg_ref_c3_slope
	0x4b,
	0x00, //cadrx_sc_reg_a1
	0x18,
	0x00, //cadrx_sc_reg_b1
	0x1c,
	0x01, //cadrx_sc_k1_int
	0xff,
	0x01, //cadrx_sc_k2_int
	0xff,
	0x03, //cadrx_sc_w1_int
	0xff,
	0xe8,
	0x03, //cadrx_sc_w2_int
	0xff,
	0xe8,
	0x00, //flicker_w_en
	0x14, //flicker_w
	0x01, //flicker_trans_en
	0x04, //flicker_trans_slope
	0x00, //bl_en
	0x03, //bl_hbs_th
	0x10, //bl_lbs_th
	0xC8, //bl_illum_gain
	0x01, //bl_reg_gain 9
	0x2C,
	0x00, //bl_le_diff
	0x00, //bl_cadrx_gain
	0x04, //bl_trans_slope
	0x00, //le_en
	0x40, //le_diff
	0x03, //le_lmax 10
	0xB6,
	0x23, //le_p
	0x24, //le_w_tmf
	0x00, //le_w_struct_diff 9
	0x72,
	0x01, //le_w_struct_diff_gain
	0x00, //le_w_luminance 9
	0x0D,
	0x00, //le_luminance_slope 10
	0x04,
	0x80, //glare_blk_avg_th 0000 0000
	0x00, //glare_luma_gain 9
	0x28,
	0xa0, //glare_luma_ctl_start 0000 0000
	0x01, //glare_luma_th 0000 0000
	0x00, //glare_luma_step 9
	0x01,
	0x00, //glare_uni_luma_gain 9
	0x01,
	0x00, //glare_blk_max_th 0000 0000
	0x00, //glare_blk_max_w 0000 0000
	0x10, //glare_blk_max_curve blk_max_sh 0 0000
	0x00, //glare_con_ext_max_th 0000 0000
	0x00, //glare_con_ext_max_w 0000 0000
	0x30, //glare_con_ext_max_sign con_ext_max_curve con_ext_max_sh 0 0 0000
	0x00, //glare_max_cnt_th 0000
	0x00, //glare_max_cnt_th 0000 0000
	0x00, //glare_max_cnt_w 0000 0000
	0x30, //glare_max_cnt_sign max_cnt_curve max_cnt_sh 0 0 0000
	0x00, //glare_y_avg_th 0000 0000
	0x00, //glare_y_avg_w 0000 0000
	0x30, //glare_y_avg_sign y_avg_curve y_avg_sh 0 0 0000
	0x00, //glare_max_cnt_th_dmc 0000
	0x00, //glare_max_cnt_th_dmc 0000 0000
	0x00, //nr fa de cs gamma 0 0000
	0xff, //nr_mask_th
	0x00, //de_gain 10
	0x00,
	0x02, //de_maxplus 11
	0x00,
	0x02, //de_maxminus 11
	0x00,
	0x14, //fa_edge_th
	0x00, //fa_step_p 13
	0x01,
	0x00, //fa_step_n 13
	0x01,
	0x00, //fa_max_de_gain 13
	0x10,
	0x00, //fa_pcl_ppi 14
	0x00,
	0x28, //fa_os_cnt_10_co
	0x3c, //fa_os_cnt_20_co
	0x04, //fa_edge_cnt_weight
	0x0f, //fa_avg_y_weight
	0x01, //cs_gain 10
	0x00,
	0x00, //curve_0
	0x10, //curve_1
	0x20, //curve_2
	0x30, //curve_3
	0x40, //curve_4
	0x50, //curve_5
	0x60, //curve_6
	0x70, //curve_7
	0x80, //curve_8
	0x90, //curve_9
	0xa0, //curve_10
	0xb0, //curve_11
	0xc0, //curve_12
	0xd0, //curve_13
	0xe0, //curve_14
	0xf0, //curve_15
	0x01, //curve_16
	0x00,
	0x00, //curve_offset 10
	0x00,
	0x00, //curve_low_x
	0x00, //blank
	0x00, //curve_low_y
};

static char DSI_NEGATIVE_MDNIE_3[] = {
	0xDD,
	0x01, //mdnie_en
	0x00, //mask 00 0000
	0x03, //edge 00
	0xf0, //ascr algo aolce gamut 00 00 00 00
	0xff, //partial_en1
	0xff, //partial_en2
	0x00, //roi_en
	0x00, //roi_block
	0x00, //roi_sx
	0x00, //roi_sx
	0x00, //roi_sy
	0x00, //roi_sy
	0x00, //roi_ex
	0x00, //roi_ex
	0x00, //roi_ey
	0x00, //roi_ey
	0xff, //trans_on trans_block 0 000 0000
	0x04, //trans_slope
	0x01, //trans_interval
	//end
};

static char DSI_GRAYSCALE_MDNIE_1[] = {
	//start
	0xDF,
	0x01, //linear_on ascr_skin_on 0000 0000
	0x6a, //ascr_skin_cb
	0x9a, //ascr_skin_cr
	0x25, //ascr_dist_up
	0x1a, //ascr_dist_down
	0x16, //ascr_dist_right
	0x2a, //ascr_dist_left
	0x00, //ascr_div_up 20
	0x37,
	0x5a,
	0x00, //ascr_div_down
	0x4e,
	0xc5,
	0x00, //ascr_div_right
	0x5d,
	0x17,
	0x00, //ascr_div_left
	0x30,
	0xc3,
	0x4c, //ascr_skin_Rr
	0x4c, //ascr_skin_Rg
	0x4c, //ascr_skin_Rb
	0xe2, //ascr_skin_Yr
	0xe2, //ascr_skin_Yg
	0xe2, //ascr_skin_Yb
	0x69, //ascr_skin_Mr
	0x69, //ascr_skin_Mg
	0x69, //ascr_skin_Mb
	0xff, //ascr_skin_Wr
	0xff, //ascr_skin_Wg
	0xff, //ascr_skin_Wb
	0xb3, //ascr_Cr
	0x4c, //ascr_Rr
	0xb3, //ascr_Cg
	0x4c, //ascr_Rg
	0xb3, //ascr_Cb
	0x4c, //ascr_Rb
	0x69, //ascr_Mr
	0x96, //ascr_Gr
	0x69, //ascr_Mg
	0x96, //ascr_Gg
	0x69, //ascr_Mb
	0x96, //ascr_Gb
	0xe2, //ascr_Yr
	0x1d, //ascr_Br
	0xe2, //ascr_Yg
	0x1d, //ascr_Bg
	0xe2, //ascr_Yb
	0x1d, //ascr_Bb
	0xff, //ascr_Wr
	0x00, //ascr_Kr
	0xff, //ascr_Wg
	0x00, //ascr_Kg
	0xff, //ascr_Wb
	0x00, //ascr_Kb
	0x0F, //edge_ctrl
	0x00, //edge_radius 12
	0x6F,
	0x05, //edge_aa_factor
	0x00, //edge_r
	0x00, //edge_g
	0x00, //edge_b
	0x00, //edge_x 11
	0x70,
	0x00, //edge_y 12
	0x70,
	0x44, //edge_coef_a edge_coef_b 0000 0000
};

static char DSI_GRAYSCALE_MDNIE_2[] = {
	0xDE,
	0x00, //gamut_gamma_on gamut_blk_shift 0000 0000
	0x40, //gamut_scale
	0x04, //gamut_r1
	0x00,
	0x00, //gamut_r2
	0x00,
	0x00, //gamut_r3
	0x00,
	0x00, //gamut_g1
	0x00,
	0x04, //gamut_g2
	0x00,
	0x00, //gamut_g3
	0x00,
	0x00, //gamut_b1
	0x00,
	0x00, //gamut_b2
	0x00,
	0x04, //gamut_b3
	0x00,
	0x00, //slce_on cadrx_en 0000 0000
	0x00, //glare_on
	0x00, //lce_gain 000 0000
	0x24, //lce_color_gain 00 0000
	0x96, //lce_min_ref_offset
	0xb3, //lce_illum_gain
	0x01, //lce_ref_offset 9
	0x0e,
	0x01, //lce_ref_gain 9
	0x00,
	0x77, //lce_block_size h v 0000 0000
	0x03, //lce_dark_th 000
	0x07, //lce_reduct_slope 0000
	0x40, //lce_black cc0 cc1 color_th 0 0 0 0000
	0x08, //lce_large_w
	0x04, //lce_med_w
	0x04, //lce_small_w
	0x00, //cadrx_dit_en shadow_corr_en ref_ctrl_en 0 0 0
	0x00, //cadrx_gain
	0x0d, //cadrx_low_intensity_th
	0xe1, //cadrx_high_intensity_th
	0x07, //cadrx_ui_zerobincnt_th
	0x29, //cadrx_ui_ratio_th
	0x04,
	0x0f, //cadrx_entire_freq
	0xa0,
	0x00,
	0x03, //cadrx_peak_th_in_freq
	0x7a,
	0xa0,
	0x00, //cadrx_high_peak_th_in_freq
	0x25,
	0x1c,
	0x24, //cadrx_dit_color_gain
	0x00, //cadrx_dit_illum_a0
	0x48, //cadrx_dit_illum_a1
	0xea, //cadrx_dit_illum_a2
	0xfa, //cadrx_dit_illum_a3
	0x00, //cadrx_dit_illum_b0
	0x10,
	0x00, //cadrx_dit_illum_b1
	0x10,
	0x00, //cadrx_dit_illum_b2
	0x9c,
	0x01, //cadrx_dit_illum_b3
	0x00,
	0x00, //cadrx_dit_illum_slope1
	0x00,
	0x00, //cadrx_dit_illum_slope2
	0xdc,
	0x03, //cadrx_dit_illum_slope3
	0xff,
	0x40, //cadrx_ui_illum_a1
	0x80, //cadrx_ui_illum_a2
	0xc0, //cadrx_ui_illum_a3
	0x00, //cadrx_ui_illum_offset1
	0x9a,
	0x00, //cadrx_ui_illum_offset2
	0x9a,
	0x00, //cadrx_ui_illum_offset3
	0x9a,
	0x00, //cadrx_ui_illum_offset4
	0x9a,
	0x00, //cadrx_ui_illum_slope1
	0x00,
	0x00, //cadrx_ui_illum_slope2
	0x00,
	0x00, //cadrx_ui_illum_slope3
	0x00,
	0x00, //cadrx_ui_illum_slope4
	0x00,
	0x40, //cadrx_ui_ref_a1
	0x80, //cadrx_ui_ref_a2
	0xc0, //cadrx_ui_ref_a3
	0x01, //cadrx_ui_ref_offset1
	0x0e,
	0x01, //cadrx_ui_ref_offset2
	0x0e,
	0x01, //cadrx_ui_ref_offset3
	0x0e,
	0x01, //cadrx_ui_ref_offset4
	0x0e,
	0x00, //cadrx_ui_ref_slope1
	0x00,
	0x00, //cadrx_ui_ref_slope2
	0x00,
	0x00, //cadrx_ui_ref_slope3
	0x00,
	0x00, //cadrx_ui_ref_slope4
	0x00,
	0x01, //cadrx_reg_ref_c1_offset
	0x0e,
	0x01, //cadrx_reg_ref_c2_offset
	0x3a,
	0x01, //cadrx_reg_ref_c3_offset
	0x44,
	0x00, //cadrx_reg_ref_c1_slope
	0x01,
	0x00, //cadrx_reg_ref_c2_slope
	0x43,
	0x00, //cadrx_reg_ref_c3_slope
	0x4b,
	0x00, //cadrx_sc_reg_a1
	0x18,
	0x00, //cadrx_sc_reg_b1
	0x1c,
	0x01, //cadrx_sc_k1_int
	0xff,
	0x01, //cadrx_sc_k2_int
	0xff,
	0x03, //cadrx_sc_w1_int
	0xff,
	0xe8,
	0x03, //cadrx_sc_w2_int
	0xff,
	0xe8,
	0x00, //flicker_w_en
	0x14, //flicker_w
	0x01, //flicker_trans_en
	0x04, //flicker_trans_slope
	0x00, //bl_en
	0x03, //bl_hbs_th
	0x10, //bl_lbs_th
	0xC8, //bl_illum_gain
	0x01, //bl_reg_gain 9
	0x2C,
	0x00, //bl_le_diff
	0x00, //bl_cadrx_gain
	0x04, //bl_trans_slope
	0x00, //le_en
	0x40, //le_diff
	0x03, //le_lmax 10
	0xB6,
	0x23, //le_p
	0x24, //le_w_tmf
	0x00, //le_w_struct_diff 9
	0x72,
	0x01, //le_w_struct_diff_gain
	0x00, //le_w_luminance 9
	0x0D,
	0x00, //le_luminance_slope 10
	0x04,
	0x80, //glare_blk_avg_th 0000 0000
	0x00, //glare_luma_gain 9
	0x28,
	0xa0, //glare_luma_ctl_start 0000 0000
	0x01, //glare_luma_th 0000 0000
	0x00, //glare_luma_step 9
	0x01,
	0x00, //glare_uni_luma_gain 9
	0x01,
	0x00, //glare_blk_max_th 0000 0000
	0x00, //glare_blk_max_w 0000 0000
	0x10, //glare_blk_max_curve blk_max_sh 0 0000
	0x00, //glare_con_ext_max_th 0000 0000
	0x00, //glare_con_ext_max_w 0000 0000
	0x30, //glare_con_ext_max_sign con_ext_max_curve con_ext_max_sh 0 0 0000
	0x00, //glare_max_cnt_th 0000
	0x00, //glare_max_cnt_th 0000 0000
	0x00, //glare_max_cnt_w 0000 0000
	0x30, //glare_max_cnt_sign max_cnt_curve max_cnt_sh 0 0 0000
	0x00, //glare_y_avg_th 0000 0000
	0x00, //glare_y_avg_w 0000 0000
	0x30, //glare_y_avg_sign y_avg_curve y_avg_sh 0 0 0000
	0x00, //glare_max_cnt_th_dmc 0000
	0x00, //glare_max_cnt_th_dmc 0000 0000
	0x00, //nr fa de cs gamma 0 0000
	0xff, //nr_mask_th
	0x00, //de_gain 10
	0x00,
	0x02, //de_maxplus 11
	0x00,
	0x02, //de_maxminus 11
	0x00,
	0x14, //fa_edge_th
	0x00, //fa_step_p 13
	0x01,
	0x00, //fa_step_n 13
	0x01,
	0x00, //fa_max_de_gain 13
	0x10,
	0x00, //fa_pcl_ppi 14
	0x00,
	0x28, //fa_os_cnt_10_co
	0x3c, //fa_os_cnt_20_co
	0x04, //fa_edge_cnt_weight
	0x0f, //fa_avg_y_weight
	0x01, //cs_gain 10
	0x00,
	0x00, //curve_0
	0x10, //curve_1
	0x20, //curve_2
	0x30, //curve_3
	0x40, //curve_4
	0x50, //curve_5
	0x60, //curve_6
	0x70, //curve_7
	0x80, //curve_8
	0x90, //curve_9
	0xa0, //curve_10
	0xb0, //curve_11
	0xc0, //curve_12
	0xd0, //curve_13
	0xe0, //curve_14
	0xf0, //curve_15
	0x01, //curve_16
	0x00,
	0x00, //curve_offset 10
	0x00,
	0x00, //curve_low_x
	0x00, //blank
	0x00, //curve_low_y
};

static char DSI_GRAYSCALE_MDNIE_3[] = {
	0xDD,
	0x01, //mdnie_en
	0x00, //mask 00 0000
	0x03, //edge 00
	0xf0, //ascr algo aolce gamut 00 00 00 00
	0xff, //partial_en1
	0xff, //partial_en2
	0x00, //roi_en
	0x00, //roi_block
	0x00, //roi_sx
	0x00, //roi_sx
	0x00, //roi_sy
	0x00, //roi_sy
	0x00, //roi_ex
	0x00, //roi_ex
	0x00, //roi_ey
	0x00, //roi_ey
	0xff, //trans_on trans_block 0 000 0000
	0x04, //trans_slope
	0x01, //trans_interval
	//end
};

static char DSI_GRAYSCALE_NEGATIVE_MDNIE_1[] = {
	//start
	0xDF,
	0x01, //linear_on ascr_skin_on 0000 0000
	0x6a, //ascr_skin_cb
	0x9a, //ascr_skin_cr
	0x25, //ascr_dist_up
	0x1a, //ascr_dist_down
	0x16, //ascr_dist_right
	0x2a, //ascr_dist_left
	0x00, //ascr_div_up 20
	0x37,
	0x5a,
	0x00, //ascr_div_down
	0x4e,
	0xc5,
	0x00, //ascr_div_right
	0x5d,
	0x17,
	0x00, //ascr_div_left
	0x30,
	0xc3,
	0xb3, //ascr_skin_Rr
	0xb3, //ascr_skin_Rg
	0xb3, //ascr_skin_Rb
	0x1d, //ascr_skin_Yr
	0x1d, //ascr_skin_Yg
	0x1d, //ascr_skin_Yb
	0x96, //ascr_skin_Mr
	0x96, //ascr_skin_Mg
	0x96, //ascr_skin_Mb
	0x00, //ascr_skin_Wr
	0x00, //ascr_skin_Wg
	0x00, //ascr_skin_Wb
	0x4c, //ascr_Cr
	0xb3, //ascr_Rr
	0x4c, //ascr_Cg
	0xb3, //ascr_Rg
	0x4c, //ascr_Cb
	0xb3, //ascr_Rb
	0x96, //ascr_Mr
	0x69, //ascr_Gr
	0x96, //ascr_Mg
	0x69, //ascr_Gg
	0x96, //ascr_Mb
	0x69, //ascr_Gb
	0x1d, //ascr_Yr
	0xe2, //ascr_Br
	0x1d, //ascr_Yg
	0xe2, //ascr_Bg
	0x1d, //ascr_Yb
	0xe2, //ascr_Bb
	0x00, //ascr_Wr
	0xff, //ascr_Kr
	0x00, //ascr_Wg
	0xff, //ascr_Kg
	0x00, //ascr_Wb
	0xff, //ascr_Kb
	0x0F, //edge_ctrl
	0x00, //edge_radius 12
	0x6F,
	0x05, //edge_aa_factor
	0x00, //edge_r
	0x00, //edge_g
	0x00, //edge_b
	0x00, //edge_x 11
	0x70,
	0x00, //edge_y 12
	0x70,
	0x44, //edge_coef_a edge_coef_b 0000 0000
};

static char DSI_GRAYSCALE_NEGATIVE_MDNIE_2[] = {
	0xDE,
	0x00, //gamut_gamma_on gamut_blk_shift 0000 0000
	0x40, //gamut_scale
	0x04, //gamut_r1
	0x00,
	0x00, //gamut_r2
	0x00,
	0x00, //gamut_r3
	0x00,
	0x00, //gamut_g1
	0x00,
	0x04, //gamut_g2
	0x00,
	0x00, //gamut_g3
	0x00,
	0x00, //gamut_b1
	0x00,
	0x00, //gamut_b2
	0x00,
	0x04, //gamut_b3
	0x00,
	0x00, //slce_on cadrx_en 0000 0000
	0x00, //glare_on
	0x00, //lce_gain 000 0000
	0x24, //lce_color_gain 00 0000
	0x96, //lce_min_ref_offset
	0xb3, //lce_illum_gain
	0x01, //lce_ref_offset 9
	0x0e,
	0x01, //lce_ref_gain 9
	0x00,
	0x77, //lce_block_size h v 0000 0000
	0x03, //lce_dark_th 000
	0x07, //lce_reduct_slope 0000
	0x40, //lce_black cc0 cc1 color_th 0 0 0 0000
	0x08, //lce_large_w
	0x04, //lce_med_w
	0x04, //lce_small_w
	0x00, //cadrx_dit_en shadow_corr_en ref_ctrl_en 0 0 0
	0x00, //cadrx_gain
	0x0d, //cadrx_low_intensity_th
	0xe1, //cadrx_high_intensity_th
	0x07, //cadrx_ui_zerobincnt_th
	0x29, //cadrx_ui_ratio_th
	0x04,
	0x0f, //cadrx_entire_freq
	0xa0,
	0x00,
	0x03, //cadrx_peak_th_in_freq
	0x7a,
	0xa0,
	0x00, //cadrx_high_peak_th_in_freq
	0x25,
	0x1c,
	0x24, //cadrx_dit_color_gain
	0x00, //cadrx_dit_illum_a0
	0x48, //cadrx_dit_illum_a1
	0xea, //cadrx_dit_illum_a2
	0xfa, //cadrx_dit_illum_a3
	0x00, //cadrx_dit_illum_b0
	0x10,
	0x00, //cadrx_dit_illum_b1
	0x10,
	0x00, //cadrx_dit_illum_b2
	0x9c,
	0x01, //cadrx_dit_illum_b3
	0x00,
	0x00, //cadrx_dit_illum_slope1
	0x00,
	0x00, //cadrx_dit_illum_slope2
	0xdc,
	0x03, //cadrx_dit_illum_slope3
	0xff,
	0x40, //cadrx_ui_illum_a1
	0x80, //cadrx_ui_illum_a2
	0xc0, //cadrx_ui_illum_a3
	0x00, //cadrx_ui_illum_offset1
	0x9a,
	0x00, //cadrx_ui_illum_offset2
	0x9a,
	0x00, //cadrx_ui_illum_offset3
	0x9a,
	0x00, //cadrx_ui_illum_offset4
	0x9a,
	0x00, //cadrx_ui_illum_slope1
	0x00,
	0x00, //cadrx_ui_illum_slope2
	0x00,
	0x00, //cadrx_ui_illum_slope3
	0x00,
	0x00, //cadrx_ui_illum_slope4
	0x00,
	0x40, //cadrx_ui_ref_a1
	0x80, //cadrx_ui_ref_a2
	0xc0, //cadrx_ui_ref_a3
	0x01, //cadrx_ui_ref_offset1
	0x0e,
	0x01, //cadrx_ui_ref_offset2
	0x0e,
	0x01, //cadrx_ui_ref_offset3
	0x0e,
	0x01, //cadrx_ui_ref_offset4
	0x0e,
	0x00, //cadrx_ui_ref_slope1
	0x00,
	0x00, //cadrx_ui_ref_slope2
	0x00,
	0x00, //cadrx_ui_ref_slope3
	0x00,
	0x00, //cadrx_ui_ref_slope4
	0x00,
	0x01, //cadrx_reg_ref_c1_offset
	0x0e,
	0x01, //cadrx_reg_ref_c2_offset
	0x3a,
	0x01, //cadrx_reg_ref_c3_offset
	0x44,
	0x00, //cadrx_reg_ref_c1_slope
	0x01,
	0x00, //cadrx_reg_ref_c2_slope
	0x43,
	0x00, //cadrx_reg_ref_c3_slope
	0x4b,
	0x00, //cadrx_sc_reg_a1
	0x18,
	0x00, //cadrx_sc_reg_b1
	0x1c,
	0x01, //cadrx_sc_k1_int
	0xff,
	0x01, //cadrx_sc_k2_int
	0xff,
	0x03, //cadrx_sc_w1_int
	0xff,
	0xe8,
	0x03, //cadrx_sc_w2_int
	0xff,
	0xe8,
	0x00, //flicker_w_en
	0x14, //flicker_w
	0x01, //flicker_trans_en
	0x04, //flicker_trans_slope
	0x00, //bl_en
	0x03, //bl_hbs_th
	0x10, //bl_lbs_th
	0xC8, //bl_illum_gain
	0x01, //bl_reg_gain 9
	0x2C,
	0x00, //bl_le_diff
	0x00, //bl_cadrx_gain
	0x04, //bl_trans_slope
	0x00, //le_en
	0x40, //le_diff
	0x03, //le_lmax 10
	0xB6,
	0x23, //le_p
	0x24, //le_w_tmf
	0x00, //le_w_struct_diff 9
	0x72,
	0x01, //le_w_struct_diff_gain
	0x00, //le_w_luminance 9
	0x0D,
	0x00, //le_luminance_slope 10
	0x04,
	0x80, //glare_blk_avg_th 0000 0000
	0x00, //glare_luma_gain 9
	0x28,
	0xa0, //glare_luma_ctl_start 0000 0000
	0x01, //glare_luma_th 0000 0000
	0x00, //glare_luma_step 9
	0x01,
	0x00, //glare_uni_luma_gain 9
	0x01,
	0x00, //glare_blk_max_th 0000 0000
	0x00, //glare_blk_max_w 0000 0000
	0x10, //glare_blk_max_curve blk_max_sh 0 0000
	0x00, //glare_con_ext_max_th 0000 0000
	0x00, //glare_con_ext_max_w 0000 0000
	0x30, //glare_con_ext_max_sign con_ext_max_curve con_ext_max_sh 0 0 0000
	0x00, //glare_max_cnt_th 0000
	0x00, //glare_max_cnt_th 0000 0000
	0x00, //glare_max_cnt_w 0000 0000
	0x30, //glare_max_cnt_sign max_cnt_curve max_cnt_sh 0 0 0000
	0x00, //glare_y_avg_th 0000 0000
	0x00, //glare_y_avg_w 0000 0000
	0x30, //glare_y_avg_sign y_avg_curve y_avg_sh 0 0 0000
	0x00, //glare_max_cnt_th_dmc 0000
	0x00, //glare_max_cnt_th_dmc 0000 0000
	0x00, //nr fa de cs gamma 0 0000
	0xff, //nr_mask_th
	0x00, //de_gain 10
	0x00,
	0x02, //de_maxplus 11
	0x00,
	0x02, //de_maxminus 11
	0x00,
	0x14, //fa_edge_th
	0x00, //fa_step_p 13
	0x01,
	0x00, //fa_step_n 13
	0x01,
	0x00, //fa_max_de_gain 13
	0x10,
	0x00, //fa_pcl_ppi 14
	0x00,
	0x28, //fa_os_cnt_10_co
	0x3c, //fa_os_cnt_20_co
	0x04, //fa_edge_cnt_weight
	0x0f, //fa_avg_y_weight
	0x01, //cs_gain 10
	0x00,
	0x00, //curve_0
	0x10, //curve_1
	0x20, //curve_2
	0x30, //curve_3
	0x40, //curve_4
	0x50, //curve_5
	0x60, //curve_6
	0x70, //curve_7
	0x80, //curve_8
	0x90, //curve_9
	0xa0, //curve_10
	0xb0, //curve_11
	0xc0, //curve_12
	0xd0, //curve_13
	0xe0, //curve_14
	0xf0, //curve_15
	0x01, //curve_16
	0x00,
	0x00, //curve_offset 10
	0x00,
	0x00, //curve_low_x
	0x00, //blank
	0x00, //curve_low_y
};

static char DSI_GRAYSCALE_NEGATIVE_MDNIE_3[] = {
	0xDD,
	0x01, //mdnie_en
	0x00, //mask 00 0000
	0x03, //edge 00
	0xf0, //ascr algo aolce gamut 00 00 00 00
	0xff, //partial_en1
	0xff, //partial_en2
	0x00, //roi_en
	0x00, //roi_block
	0x00, //roi_sx
	0x00, //roi_sx
	0x00, //roi_sy
	0x00, //roi_sy
	0x00, //roi_ex
	0x00, //roi_ex
	0x00, //roi_ey
	0x00, //roi_ey
	0xff, //trans_on trans_block 0 000 0000
	0x04, //trans_slope
	0x01, //trans_interval
	//end
};

static char DSI_COLOR_BLIND_MDNIE_1[] = {
	//start
	0xDF,
	0x00, //linear_on ascr_skin_on 0000 0000
	0x6a, //ascr_skin_cb
	0x9a, //ascr_skin_cr
	0x25, //ascr_dist_up
	0x1a, //ascr_dist_down
	0x16, //ascr_dist_right
	0x2a, //ascr_dist_left
	0x00, //ascr_div_up 20
	0x37,
	0x5a,
	0x00, //ascr_div_down
	0x4e,
	0xc5,
	0x00, //ascr_div_right
	0x5d,
	0x17,
	0x00, //ascr_div_left
	0x30,
	0xc3,
	0xff, //ascr_skin_Rr
	0x00, //ascr_skin_Rg
	0x00, //ascr_skin_Rb
	0xff, //ascr_skin_Yr
	0xff, //ascr_skin_Yg
	0x00, //ascr_skin_Yb
	0xff, //ascr_skin_Mr
	0x00, //ascr_skin_Mg
	0xff, //ascr_skin_Mb
	0xff, //ascr_skin_Wr
	0xff, //ascr_skin_Wg
	0xff, //ascr_skin_Wb
	0x00, //ascr_Cr
	0xff, //ascr_Rr
	0xff, //ascr_Cg
	0x00, //ascr_Rg
	0xff, //ascr_Cb
	0x00, //ascr_Rb
	0xff, //ascr_Mr
	0x00, //ascr_Gr
	0x00, //ascr_Mg
	0xff, //ascr_Gg
	0xff, //ascr_Mb
	0x00, //ascr_Gb
	0xff, //ascr_Yr
	0x00, //ascr_Br
	0xff, //ascr_Yg
	0x00, //ascr_Bg
	0x00, //ascr_Yb
	0xff, //ascr_Bb
	0xff, //ascr_Wr
	0x00, //ascr_Kr
	0xff, //ascr_Wg
	0x00, //ascr_Kg
	0xff, //ascr_Wb
	0x00, //ascr_Kb
	0x0F, //edge_ctrl
	0x00, //edge_radius 12
	0x6F,
	0x05, //edge_aa_factor
	0x00, //edge_r
	0x00, //edge_g
	0x00, //edge_b
	0x00, //edge_x 11
	0x70,
	0x00, //edge_y 12
	0x70,
	0x44, //edge_coef_a edge_coef_b 0000 0000
};

static char DSI_COLOR_BLIND_MDNIE_2[] = {
	0xDE,
	0x00, //gamut_gamma_on gamut_blk_shift 0000 0000
	0x40, //gamut_scale
	0x04, //gamut_r1
	0x00,
	0x00, //gamut_r2
	0x00,
	0x00, //gamut_r3
	0x00,
	0x00, //gamut_g1
	0x00,
	0x04, //gamut_g2
	0x00,
	0x00, //gamut_g3
	0x00,
	0x00, //gamut_b1
	0x00,
	0x00, //gamut_b2
	0x00,
	0x04, //gamut_b3
	0x00,
	0x00, //slce_on cadrx_en 0000 0000
	0x00, //glare_on
	0x00, //lce_gain 000 0000
	0x24, //lce_color_gain 00 0000
	0x96, //lce_min_ref_offset
	0xb3, //lce_illum_gain
	0x01, //lce_ref_offset 9
	0x0e,
	0x01, //lce_ref_gain 9
	0x00,
	0x77, //lce_block_size h v 0000 0000
	0x03, //lce_dark_th 000
	0x07, //lce_reduct_slope 0000
	0x40, //lce_black cc0 cc1 color_th 0 0 0 0000
	0x08, //lce_large_w
	0x04, //lce_med_w
	0x04, //lce_small_w
	0x00, //cadrx_dit_en shadow_corr_en ref_ctrl_en 0 0 0
	0x00, //cadrx_gain
	0x0d, //cadrx_low_intensity_th
	0xe1, //cadrx_high_intensity_th
	0x07, //cadrx_ui_zerobincnt_th
	0x29, //cadrx_ui_ratio_th
	0x04,
	0x0f, //cadrx_entire_freq
	0xa0,
	0x00,
	0x03, //cadrx_peak_th_in_freq
	0x7a,
	0xa0,
	0x00, //cadrx_high_peak_th_in_freq
	0x25,
	0x1c,
	0x24, //cadrx_dit_color_gain
	0x00, //cadrx_dit_illum_a0
	0x48, //cadrx_dit_illum_a1
	0xea, //cadrx_dit_illum_a2
	0xfa, //cadrx_dit_illum_a3
	0x00, //cadrx_dit_illum_b0
	0x10,
	0x00, //cadrx_dit_illum_b1
	0x10,
	0x00, //cadrx_dit_illum_b2
	0x9c,
	0x01, //cadrx_dit_illum_b3
	0x00,
	0x00, //cadrx_dit_illum_slope1
	0x00,
	0x00, //cadrx_dit_illum_slope2
	0xdc,
	0x03, //cadrx_dit_illum_slope3
	0xff,
	0x40, //cadrx_ui_illum_a1
	0x80, //cadrx_ui_illum_a2
	0xc0, //cadrx_ui_illum_a3
	0x00, //cadrx_ui_illum_offset1
	0x9a,
	0x00, //cadrx_ui_illum_offset2
	0x9a,
	0x00, //cadrx_ui_illum_offset3
	0x9a,
	0x00, //cadrx_ui_illum_offset4
	0x9a,
	0x00, //cadrx_ui_illum_slope1
	0x00,
	0x00, //cadrx_ui_illum_slope2
	0x00,
	0x00, //cadrx_ui_illum_slope3
	0x00,
	0x00, //cadrx_ui_illum_slope4
	0x00,
	0x40, //cadrx_ui_ref_a1
	0x80, //cadrx_ui_ref_a2
	0xc0, //cadrx_ui_ref_a3
	0x01, //cadrx_ui_ref_offset1
	0x0e,
	0x01, //cadrx_ui_ref_offset2
	0x0e,
	0x01, //cadrx_ui_ref_offset3
	0x0e,
	0x01, //cadrx_ui_ref_offset4
	0x0e,
	0x00, //cadrx_ui_ref_slope1
	0x00,
	0x00, //cadrx_ui_ref_slope2
	0x00,
	0x00, //cadrx_ui_ref_slope3
	0x00,
	0x00, //cadrx_ui_ref_slope4
	0x00,
	0x01, //cadrx_reg_ref_c1_offset
	0x0e,
	0x01, //cadrx_reg_ref_c2_offset
	0x3a,
	0x01, //cadrx_reg_ref_c3_offset
	0x44,
	0x00, //cadrx_reg_ref_c1_slope
	0x01,
	0x00, //cadrx_reg_ref_c2_slope
	0x43,
	0x00, //cadrx_reg_ref_c3_slope
	0x4b,
	0x00, //cadrx_sc_reg_a1
	0x18,
	0x00, //cadrx_sc_reg_b1
	0x1c,
	0x01, //cadrx_sc_k1_int
	0xff,
	0x01, //cadrx_sc_k2_int
	0xff,
	0x03, //cadrx_sc_w1_int
	0xff,
	0xe8,
	0x03, //cadrx_sc_w2_int
	0xff,
	0xe8,
	0x00, //flicker_w_en
	0x14, //flicker_w
	0x01, //flicker_trans_en
	0x04, //flicker_trans_slope
	0x00, //bl_en
	0x03, //bl_hbs_th
	0x10, //bl_lbs_th
	0xC8, //bl_illum_gain
	0x01, //bl_reg_gain 9
	0x2C,
	0x00, //bl_le_diff
	0x00, //bl_cadrx_gain
	0x04, //bl_trans_slope
	0x00, //le_en
	0x40, //le_diff
	0x03, //le_lmax 10
	0xB6,
	0x23, //le_p
	0x24, //le_w_tmf
	0x00, //le_w_struct_diff 9
	0x72,
	0x01, //le_w_struct_diff_gain
	0x00, //le_w_luminance 9
	0x0D,
	0x00, //le_luminance_slope 10
	0x04,
	0x80, //glare_blk_avg_th 0000 0000
	0x00, //glare_luma_gain 9
	0x28,
	0xa0, //glare_luma_ctl_start 0000 0000
	0x01, //glare_luma_th 0000 0000
	0x00, //glare_luma_step 9
	0x01,
	0x00, //glare_uni_luma_gain 9
	0x01,
	0x00, //glare_blk_max_th 0000 0000
	0x00, //glare_blk_max_w 0000 0000
	0x10, //glare_blk_max_curve blk_max_sh 0 0000
	0x00, //glare_con_ext_max_th 0000 0000
	0x00, //glare_con_ext_max_w 0000 0000
	0x30, //glare_con_ext_max_sign con_ext_max_curve con_ext_max_sh 0 0 0000
	0x00, //glare_max_cnt_th 0000
	0x00, //glare_max_cnt_th 0000 0000
	0x00, //glare_max_cnt_w 0000 0000
	0x30, //glare_max_cnt_sign max_cnt_curve max_cnt_sh 0 0 0000
	0x00, //glare_y_avg_th 0000 0000
	0x00, //glare_y_avg_w 0000 0000
	0x30, //glare_y_avg_sign y_avg_curve y_avg_sh 0 0 0000
	0x00, //glare_max_cnt_th_dmc 0000
	0x00, //glare_max_cnt_th_dmc 0000 0000
	0x00, //nr fa de cs gamma 0 0000
	0xff, //nr_mask_th
	0x00, //de_gain 10
	0x00,
	0x02, //de_maxplus 11
	0x00,
	0x02, //de_maxminus 11
	0x00,
	0x14, //fa_edge_th
	0x00, //fa_step_p 13
	0x01,
	0x00, //fa_step_n 13
	0x01,
	0x00, //fa_max_de_gain 13
	0x10,
	0x00, //fa_pcl_ppi 14
	0x00,
	0x28, //fa_os_cnt_10_co
	0x3c, //fa_os_cnt_20_co
	0x04, //fa_edge_cnt_weight
	0x0f, //fa_avg_y_weight
	0x01, //cs_gain 10
	0x00,
	0x00, //curve_0
	0x10, //curve_1
	0x20, //curve_2
	0x30, //curve_3
	0x40, //curve_4
	0x50, //curve_5
	0x60, //curve_6
	0x70, //curve_7
	0x80, //curve_8
	0x90, //curve_9
	0xa0, //curve_10
	0xb0, //curve_11
	0xc0, //curve_12
	0xd0, //curve_13
	0xe0, //curve_14
	0xf0, //curve_15
	0x01, //curve_16
	0x00,
	0x00, //curve_offset 10
	0x00,
	0x00, //curve_low_x
	0x00, //blank
	0x00, //curve_low_y
};

static char DSI_COLOR_BLIND_MDNIE_3[] = {
	0xDD,
	0x01, //mdnie_en
	0x00, //mask 00 0000
	0x03, //edge 00
	0xf0, //ascr algo aolce gamut 00 00 00 00
	0xff, //partial_en1
	0xff, //partial_en2
	0x00, //roi_en
	0x00, //roi_block
	0x00, //roi_sx
	0x00, //roi_sx
	0x00, //roi_sy
	0x00, //roi_sy
	0x00, //roi_ex
	0x00, //roi_ex
	0x00, //roi_ey
	0x00, //roi_ey
	0xff, //trans_on trans_block 0 000 0000
	0x04, //trans_slope
	0x01, //trans_interval
	//end
};

static char DSI_NIGHT_MODE_MDNIE_1[] = {
	//start
	0xDF,
	0x00, //linear_on ascr_skin_on 0000 0000
	0x6a, //ascr_skin_cb
	0x9a, //ascr_skin_cr
	0x25, //ascr_dist_up
	0x1a, //ascr_dist_down
	0x16, //ascr_dist_right
	0x2a, //ascr_dist_left
	0x00, //ascr_div_up 20
	0x37,
	0x5a,
	0x00, //ascr_div_down
	0x4e,
	0xc5,
	0x00, //ascr_div_right
	0x5d,
	0x17,
	0x00, //ascr_div_left
	0x30,
	0xc3,
	0xff, //ascr_skin_Rr
	0x00, //ascr_skin_Rg
	0x00, //ascr_skin_Rb
	0xff, //ascr_skin_Yr
	0xff, //ascr_skin_Yg
	0x00, //ascr_skin_Yb
	0xff, //ascr_skin_Mr
	0x00, //ascr_skin_Mg
	0xff, //ascr_skin_Mb
	0xff, //ascr_skin_Wr
	0xff, //ascr_skin_Wg
	0xff, //ascr_skin_Wb
	0x00, //ascr_Cr
	0xff, //ascr_Rr
	0xff, //ascr_Cg
	0x00, //ascr_Rg
	0xff, //ascr_Cb
	0x00, //ascr_Rb
	0xff, //ascr_Mr
	0x00, //ascr_Gr
	0x00, //ascr_Mg
	0xff, //ascr_Gg
	0xff, //ascr_Mb
	0x00, //ascr_Gb
	0xff, //ascr_Yr
	0x00, //ascr_Br
	0xff, //ascr_Yg
	0x00, //ascr_Bg
	0x00, //ascr_Yb
	0xff, //ascr_Bb
	0xff, //ascr_Wr
	0x00, //ascr_Kr
	0xff, //ascr_Wg
	0x00, //ascr_Kg
	0xff, //ascr_Wb
	0x00, //ascr_Kb
	0x0F, //edge_ctrl
	0x00, //edge_radius 12
	0x6F,
	0x05, //edge_aa_factor
	0x00, //edge_r
	0x00, //edge_g
	0x00, //edge_b
	0x00, //edge_x 11
	0x70,
	0x00, //edge_y 12
	0x70,
	0x44, //edge_coef_a edge_coef_b 0000 0000
};

static char DSI_NIGHT_MODE_MDNIE_2[] = {
	0xDE,
	0x00, //gamut_gamma_on gamut_blk_shift 0000 0000
	0x40, //gamut_scale
	0x04, //gamut_r1
	0x00,
	0x00, //gamut_r2
	0x00,
	0x00, //gamut_r3
	0x00,
	0x00, //gamut_g1
	0x00,
	0x04, //gamut_g2
	0x00,
	0x00, //gamut_g3
	0x00,
	0x00, //gamut_b1
	0x00,
	0x00, //gamut_b2
	0x00,
	0x04, //gamut_b3
	0x00,
	0x00, //slce_on cadrx_en 0000 0000
	0x00, //glare_on
	0x00, //lce_gain 000 0000
	0x24, //lce_color_gain 00 0000
	0x96, //lce_min_ref_offset
	0xb3, //lce_illum_gain
	0x01, //lce_ref_offset 9
	0x0e,
	0x01, //lce_ref_gain 9
	0x00,
	0x77, //lce_block_size h v 0000 0000
	0x03, //lce_dark_th 000
	0x07, //lce_reduct_slope 0000
	0x40, //lce_black cc0 cc1 color_th 0 0 0 0000
	0x08, //lce_large_w
	0x04, //lce_med_w
	0x04, //lce_small_w
	0x00, //cadrx_dit_en shadow_corr_en ref_ctrl_en 0 0 0
	0x00, //cadrx_gain
	0x0d, //cadrx_low_intensity_th
	0xe1, //cadrx_high_intensity_th
	0x07, //cadrx_ui_zerobincnt_th
	0x29, //cadrx_ui_ratio_th
	0x04,
	0x0f, //cadrx_entire_freq
	0xa0,
	0x00,
	0x03, //cadrx_peak_th_in_freq
	0x7a,
	0xa0,
	0x00, //cadrx_high_peak_th_in_freq
	0x25,
	0x1c,
	0x24, //cadrx_dit_color_gain
	0x00, //cadrx_dit_illum_a0
	0x48, //cadrx_dit_illum_a1
	0xea, //cadrx_dit_illum_a2
	0xfa, //cadrx_dit_illum_a3
	0x00, //cadrx_dit_illum_b0
	0x10,
	0x00, //cadrx_dit_illum_b1
	0x10,
	0x00, //cadrx_dit_illum_b2
	0x9c,
	0x01, //cadrx_dit_illum_b3
	0x00,
	0x00, //cadrx_dit_illum_slope1
	0x00,
	0x00, //cadrx_dit_illum_slope2
	0xdc,
	0x03, //cadrx_dit_illum_slope3
	0xff,
	0x40, //cadrx_ui_illum_a1
	0x80, //cadrx_ui_illum_a2
	0xc0, //cadrx_ui_illum_a3
	0x00, //cadrx_ui_illum_offset1
	0x9a,
	0x00, //cadrx_ui_illum_offset2
	0x9a,
	0x00, //cadrx_ui_illum_offset3
	0x9a,
	0x00, //cadrx_ui_illum_offset4
	0x9a,
	0x00, //cadrx_ui_illum_slope1
	0x00,
	0x00, //cadrx_ui_illum_slope2
	0x00,
	0x00, //cadrx_ui_illum_slope3
	0x00,
	0x00, //cadrx_ui_illum_slope4
	0x00,
	0x40, //cadrx_ui_ref_a1
	0x80, //cadrx_ui_ref_a2
	0xc0, //cadrx_ui_ref_a3
	0x01, //cadrx_ui_ref_offset1
	0x0e,
	0x01, //cadrx_ui_ref_offset2
	0x0e,
	0x01, //cadrx_ui_ref_offset3
	0x0e,
	0x01, //cadrx_ui_ref_offset4
	0x0e,
	0x00, //cadrx_ui_ref_slope1
	0x00,
	0x00, //cadrx_ui_ref_slope2
	0x00,
	0x00, //cadrx_ui_ref_slope3
	0x00,
	0x00, //cadrx_ui_ref_slope4
	0x00,
	0x01, //cadrx_reg_ref_c1_offset
	0x0e,
	0x01, //cadrx_reg_ref_c2_offset
	0x3a,
	0x01, //cadrx_reg_ref_c3_offset
	0x44,
	0x00, //cadrx_reg_ref_c1_slope
	0x01,
	0x00, //cadrx_reg_ref_c2_slope
	0x43,
	0x00, //cadrx_reg_ref_c3_slope
	0x4b,
	0x00, //cadrx_sc_reg_a1
	0x18,
	0x00, //cadrx_sc_reg_b1
	0x1c,
	0x01, //cadrx_sc_k1_int
	0xff,
	0x01, //cadrx_sc_k2_int
	0xff,
	0x03, //cadrx_sc_w1_int
	0xff,
	0xe8,
	0x03, //cadrx_sc_w2_int
	0xff,
	0xe8,
	0x00, //flicker_w_en
	0x14, //flicker_w
	0x01, //flicker_trans_en
	0x04, //flicker_trans_slope
	0x00, //bl_en
	0x03, //bl_hbs_th
	0x10, //bl_lbs_th
	0xC8, //bl_illum_gain
	0x01, //bl_reg_gain 9
	0x2C,
	0x00, //bl_le_diff
	0x00, //bl_cadrx_gain
	0x04, //bl_trans_slope
	0x00, //le_en
	0x40, //le_diff
	0x03, //le_lmax 10
	0xB6,
	0x23, //le_p
	0x24, //le_w_tmf
	0x00, //le_w_struct_diff 9
	0x72,
	0x01, //le_w_struct_diff_gain
	0x00, //le_w_luminance 9
	0x0D,
	0x00, //le_luminance_slope 10
	0x04,
	0x80, //glare_blk_avg_th 0000 0000
	0x00, //glare_luma_gain 9
	0x28,
	0xa0, //glare_luma_ctl_start 0000 0000
	0x01, //glare_luma_th 0000 0000
	0x00, //glare_luma_step 9
	0x01,
	0x00, //glare_uni_luma_gain 9
	0x01,
	0x00, //glare_blk_max_th 0000 0000
	0x00, //glare_blk_max_w 0000 0000
	0x10, //glare_blk_max_curve blk_max_sh 0 0000
	0x00, //glare_con_ext_max_th 0000 0000
	0x00, //glare_con_ext_max_w 0000 0000
	0x30, //glare_con_ext_max_sign con_ext_max_curve con_ext_max_sh 0 0 0000
	0x00, //glare_max_cnt_th 0000
	0x00, //glare_max_cnt_th 0000 0000
	0x00, //glare_max_cnt_w 0000 0000
	0x30, //glare_max_cnt_sign max_cnt_curve max_cnt_sh 0 0 0000
	0x00, //glare_y_avg_th 0000 0000
	0x00, //glare_y_avg_w 0000 0000
	0x30, //glare_y_avg_sign y_avg_curve y_avg_sh 0 0 0000
	0x00, //glare_max_cnt_th_dmc 0000
	0x00, //glare_max_cnt_th_dmc 0000 0000
	0x00, //nr fa de cs gamma 0 0000
	0xff, //nr_mask_th
	0x00, //de_gain 10
	0x00,
	0x02, //de_maxplus 11
	0x00,
	0x02, //de_maxminus 11
	0x00,
	0x14, //fa_edge_th
	0x00, //fa_step_p 13
	0x01,
	0x00, //fa_step_n 13
	0x01,
	0x00, //fa_max_de_gain 13
	0x10,
	0x00, //fa_pcl_ppi 14
	0x00,
	0x28, //fa_os_cnt_10_co
	0x3c, //fa_os_cnt_20_co
	0x04, //fa_edge_cnt_weight
	0x0f, //fa_avg_y_weight
	0x01, //cs_gain 10
	0x00,
	0x00, //curve_0
	0x10, //curve_1
	0x20, //curve_2
	0x30, //curve_3
	0x40, //curve_4
	0x50, //curve_5
	0x60, //curve_6
	0x70, //curve_7
	0x80, //curve_8
	0x90, //curve_9
	0xa0, //curve_10
	0xb0, //curve_11
	0xc0, //curve_12
	0xd0, //curve_13
	0xe0, //curve_14
	0xf0, //curve_15
	0x01, //curve_16
	0x00,
	0x00, //curve_offset 10
	0x00,
	0x00, //curve_low_x
	0x00, //blank
	0x00, //curve_low_y
};

static char DSI_NIGHT_MODE_MDNIE_3[] = {
	0xDD,
	0x01, //mdnie_en
	0x00, //mask 00 0000
	0x03, //edge 00
	0xf0, //ascr algo aolce gamut 00 00 00 00
	0xff, //partial_en1
	0xff, //partial_en2
	0x00, //roi_en
	0x00, //roi_block
	0x00, //roi_sx
	0x00, //roi_sx
	0x00, //roi_sy
	0x00, //roi_sy
	0x00, //roi_ex
	0x00, //roi_ex
	0x00, //roi_ey
	0x00, //roi_ey
	0xff, //trans_on trans_block 0 000 0000
	0x04, //trans_slope
	0x01, //trans_interval
	//end
};

static char DSI_HBM_CE_MDNIE_1[] = {
	//start
	0xDF,
	0x01, //linear_on ascr_skin_on 0000 0000
	0x6a, //ascr_skin_cb
	0x9a, //ascr_skin_cr
	0x25, //ascr_dist_up
	0x1a, //ascr_dist_down
	0x16, //ascr_dist_right
	0x2a, //ascr_dist_left
	0x00, //ascr_div_up 20
	0x37,
	0x5a,
	0x00, //ascr_div_down
	0x4e,
	0xc5,
	0x00, //ascr_div_right
	0x5d,
	0x17,
	0x00, //ascr_div_left
	0x30,
	0xc3,
	0xff, //ascr_skin_Rr
	0x00, //ascr_skin_Rg
	0x00, //ascr_skin_Rb
	0xff, //ascr_skin_Yr
	0xff, //ascr_skin_Yg
	0x00, //ascr_skin_Yb
	0xff, //ascr_skin_Mr
	0x00, //ascr_skin_Mg
	0xff, //ascr_skin_Mb
	0xff, //ascr_skin_Wr
	0xff, //ascr_skin_Wg
	0xff, //ascr_skin_Wb
	0x00, //ascr_Cr
	0xff, //ascr_Rr
	0xff, //ascr_Cg
	0x00, //ascr_Rg
	0xff, //ascr_Cb
	0x00, //ascr_Rb
	0xff, //ascr_Mr
	0x00, //ascr_Gr
	0x00, //ascr_Mg
	0xff, //ascr_Gg
	0xff, //ascr_Mb
	0x00, //ascr_Gb
	0xff, //ascr_Yr
	0x00, //ascr_Br
	0xff, //ascr_Yg
	0x00, //ascr_Bg
	0x00, //ascr_Yb
	0xff, //ascr_Bb
	0xff, //ascr_Wr
	0x00, //ascr_Kr
	0xff, //ascr_Wg
	0x00, //ascr_Kg
	0xff, //ascr_Wb
	0x00, //ascr_Kb
	0x0F, //edge_ctrl
	0x00, //edge_radius 12
	0x6F,
	0x05, //edge_aa_factor
	0x00, //edge_r
	0x00, //edge_g
	0x00, //edge_b
	0x00, //edge_x 11
	0x70,
	0x00, //edge_y 12
	0x70,
	0x44, //edge_coef_a edge_coef_b 0000 0000
};

static char DSI_HBM_CE_MDNIE_2[] = {
	0xDE,
	0x00, //gamut_gamma_on gamut_blk_shift 0000 0000
	0x40, //gamut_scale
	0x04, //gamut_r1
	0x00,
	0x00, //gamut_r2
	0x00,
	0x00, //gamut_r3
	0x00,
	0x00, //gamut_g1
	0x00,
	0x04, //gamut_g2
	0x00,
	0x00, //gamut_g3
	0x00,
	0x00, //gamut_b1
	0x00,
	0x00, //gamut_b2
	0x00,
	0x04, //gamut_b3
	0x00,
	0x10, //slce_on cadrx_en 0000 0000
	0x00, //glare_on
	0x08, //lce_gain 000 0000
	0x30, //lce_color_gain 00 0000
	0x40, //lce_min_ref_offset
	0xb0, //lce_illum_gain
	0x01, //lce_ref_offset 9
	0xbf,
	0x00, //lce_ref_gain 9
	0xd0,
	0x77, //lce_block_size h v 0000 0000
	0x00, //lce_dark_th 000
	0x07, //lce_reduct_slope 0000
	0x55, //lce_black cc0 cc1 color_th 0 0 0 0000
	0x00, //lce_large_w
	0x00, //lce_med_w
	0x10, //lce_small_w
	0x00, //cadrx_dit_en shadow_corr_en ref_ctrl_en 0 0 0
	0x00, //cadrx_gain
	0x0d, //cadrx_low_intensity_th
	0xe1, //cadrx_high_intensity_th
	0x07, //cadrx_ui_zerobincnt_th
	0x29, //cadrx_ui_ratio_th
	0x04,
	0x0f, //cadrx_entire_freq
	0xa0,
	0x00,
	0x03, //cadrx_peak_th_in_freq
	0x7a,
	0xa0,
	0x00, //cadrx_high_peak_th_in_freq
	0x25,
	0x1c,
	0x24, //cadrx_dit_color_gain
	0x00, //cadrx_dit_illum_a0
	0x48, //cadrx_dit_illum_a1
	0xea, //cadrx_dit_illum_a2
	0xfa, //cadrx_dit_illum_a3
	0x00, //cadrx_dit_illum_b0
	0x10,
	0x00, //cadrx_dit_illum_b1
	0x10,
	0x00, //cadrx_dit_illum_b2
	0x9c,
	0x01, //cadrx_dit_illum_b3
	0x00,
	0x00, //cadrx_dit_illum_slope1
	0x00,
	0x00, //cadrx_dit_illum_slope2
	0xdc,
	0x03, //cadrx_dit_illum_slope3
	0xff,
	0x40, //cadrx_ui_illum_a1
	0x80, //cadrx_ui_illum_a2
	0xc0, //cadrx_ui_illum_a3
	0x00, //cadrx_ui_illum_offset1
	0x9a,
	0x00, //cadrx_ui_illum_offset2
	0x9a,
	0x00, //cadrx_ui_illum_offset3
	0x9a,
	0x00, //cadrx_ui_illum_offset4
	0x9a,
	0x00, //cadrx_ui_illum_slope1
	0x00,
	0x00, //cadrx_ui_illum_slope2
	0x00,
	0x00, //cadrx_ui_illum_slope3
	0x00,
	0x00, //cadrx_ui_illum_slope4
	0x00,
	0x40, //cadrx_ui_ref_a1
	0x80, //cadrx_ui_ref_a2
	0xc0, //cadrx_ui_ref_a3
	0x01, //cadrx_ui_ref_offset1
	0x0e,
	0x01, //cadrx_ui_ref_offset2
	0x0e,
	0x01, //cadrx_ui_ref_offset3
	0x0e,
	0x01, //cadrx_ui_ref_offset4
	0x0e,
	0x00, //cadrx_ui_ref_slope1
	0x00,
	0x00, //cadrx_ui_ref_slope2
	0x00,
	0x00, //cadrx_ui_ref_slope3
	0x00,
	0x00, //cadrx_ui_ref_slope4
	0x00,
	0x01, //cadrx_reg_ref_c1_offset
	0x0e,
	0x01, //cadrx_reg_ref_c2_offset
	0x3a,
	0x01, //cadrx_reg_ref_c3_offset
	0x44,
	0x00, //cadrx_reg_ref_c1_slope
	0x01,
	0x00, //cadrx_reg_ref_c2_slope
	0x43,
	0x00, //cadrx_reg_ref_c3_slope
	0x4b,
	0x00, //cadrx_sc_reg_a1
	0x18,
	0x00, //cadrx_sc_reg_b1
	0x1c,
	0x01, //cadrx_sc_k1_int
	0xff,
	0x01, //cadrx_sc_k2_int
	0xff,
	0x03, //cadrx_sc_w1_int
	0xff,
	0xe8,
	0x03, //cadrx_sc_w2_int
	0xff,
	0xe8,
	0x00, //flicker_w_en
	0x14, //flicker_w
	0x01, //flicker_trans_en
	0x04, //flicker_trans_slope
	0x00, //bl_en
	0x03, //bl_hbs_th
	0x10, //bl_lbs_th
	0xC8, //bl_illum_gain
	0x01, //bl_reg_gain 9
	0x2C,
	0x00, //bl_le_diff
	0x00, //bl_cadrx_gain
	0x04, //bl_trans_slope
	0x00, //le_en
	0x40, //le_diff
	0x03, //le_lmax 10
	0xB6,
	0x23, //le_p
	0x24, //le_w_tmf
	0x00, //le_w_struct_diff 9
	0x72,
	0x01, //le_w_struct_diff_gain
	0x00, //le_w_luminance 9
	0x0D,
	0x00, //le_luminance_slope 10
	0x04,
	0x80, //glare_blk_avg_th 0000 0000
	0x00, //glare_luma_gain 9
	0x28,
	0xa0, //glare_luma_ctl_start 0000 0000
	0x01, //glare_luma_th 0000 0000
	0x00, //glare_luma_step 9
	0x01,
	0x00, //glare_uni_luma_gain 9
	0x01,
	0x00, //glare_blk_max_th 0000 0000
	0x00, //glare_blk_max_w 0000 0000
	0x10, //glare_blk_max_curve blk_max_sh 0 0000
	0x00, //glare_con_ext_max_th 0000 0000
	0x00, //glare_con_ext_max_w 0000 0000
	0x30, //glare_con_ext_max_sign con_ext_max_curve con_ext_max_sh 0 0 0000
	0x00, //glare_max_cnt_th 0000
	0x00, //glare_max_cnt_th 0000 0000
	0x00, //glare_max_cnt_w 0000 0000
	0x30, //glare_max_cnt_sign max_cnt_curve max_cnt_sh 0 0 0000
	0x00, //glare_y_avg_th 0000 0000
	0x00, //glare_y_avg_w 0000 0000
	0x30, //glare_y_avg_sign y_avg_curve y_avg_sh 0 0 0000
	0x00, //glare_max_cnt_th_dmc 0000
	0x00, //glare_max_cnt_th_dmc 0000 0000
	0x01, //nr fa de cs gamma 0 0000
	0xff, //nr_mask_th
	0x00, //de_gain 10
	0x00,
	0x02, //de_maxplus 11
	0x00,
	0x02, //de_maxminus 11
	0x00,
	0x14, //fa_edge_th
	0x00, //fa_step_p 13
	0x01,
	0x00, //fa_step_n 13
	0x01,
	0x00, //fa_max_de_gain 13
	0x10,
	0x00, //fa_pcl_ppi 14
	0x00,
	0x28, //fa_os_cnt_10_co
	0x3c, //fa_os_cnt_20_co
	0x04, //fa_edge_cnt_weight
	0x0f, //fa_avg_y_weight
	0x01, //cs_gain 10
	0x20,
	0x00, //curve_0
	0x21, //curve_1
	0x38, //curve_2
	0x4c, //curve_3
	0x5d, //curve_4
	0x6e, //curve_5
	0x7d, //curve_6
	0x8c, //curve_7
	0x9a, //curve_8
	0xa8, //curve_9
	0xb5, //curve_10
	0xc2, //curve_11
	0xcf, //curve_12
	0xdc, //curve_13
	0xe8, //curve_14
	0xf5, //curve_15
	0x01, //curve_16
	0x00,
	0x00, //curve_offset 10
	0x00,
	0x00, //curve_low_x
	0x00, //blank
	0x00, //curve_low_y
};

static char DSI_HBM_CE_MDNIE_3[] = {
	0xDD,
	0x01, //mdnie_en
	0x00, //mask 00 0000
	0x03, //edge 00
	0xfc, //ascr algo aolce gamut 00 00 00 00
	0xff, //partial_en1
	0xff, //partial_en2
	0x00, //roi_en
	0x00, //roi_block
	0x00, //roi_sx
	0x00, //roi_sx
	0x00, //roi_sy
	0x00, //roi_sy
	0x00, //roi_ex
	0x00, //roi_ex
	0x00, //roi_ey
	0x00, //roi_ey
	0xff, //trans_on trans_block 0 000 0000
	0x04, //trans_slope
	0x01, //trans_interval
	//end
};

static char DSI_HBM_CE_D65_MDNIE_1[] = {
	//start
	0xDF,
	0x01, //linear_on ascr_skin_on 0000 0000
	0x6a, //ascr_skin_cb
	0x9a, //ascr_skin_cr
	0x25, //ascr_dist_up
	0x1a, //ascr_dist_down
	0x16, //ascr_dist_right
	0x2a, //ascr_dist_left
	0x00, //ascr_div_up 20
	0x37,
	0x5a,
	0x00, //ascr_div_down
	0x4e,
	0xc5,
	0x00, //ascr_div_right
	0x5d,
	0x17,
	0x00, //ascr_div_left
	0x30,
	0xc3,
	0xff, //ascr_skin_Rr
	0x00, //ascr_skin_Rg
	0x00, //ascr_skin_Rb
	0xff, //ascr_skin_Yr
	0xff, //ascr_skin_Yg
	0x00, //ascr_skin_Yb
	0xff, //ascr_skin_Mr
	0x00, //ascr_skin_Mg
	0xff, //ascr_skin_Mb
	0xff, //ascr_skin_Wr
	0xfa, //ascr_skin_Wg
	0xf1, //ascr_skin_Wb
	0x00, //ascr_Cr
	0xff, //ascr_Rr
	0xff, //ascr_Cg
	0x00, //ascr_Rg
	0xff, //ascr_Cb
	0x00, //ascr_Rb
	0xff, //ascr_Mr
	0x00, //ascr_Gr
	0x00, //ascr_Mg
	0xff, //ascr_Gg
	0xff, //ascr_Mb
	0x00, //ascr_Gb
	0xff, //ascr_Yr
	0x00, //ascr_Br
	0xff, //ascr_Yg
	0x00, //ascr_Bg
	0x00, //ascr_Yb
	0xff, //ascr_Bb
	0xff, //ascr_Wr
	0x00, //ascr_Kr
	0xfa, //ascr_Wg
	0x00, //ascr_Kg
	0xf1, //ascr_Wb
	0x00, //ascr_Kb
	0x0F, //edge_ctrl
	0x00, //edge_radius 12
	0x6F,
	0x05, //edge_aa_factor
	0x00, //edge_r
	0x00, //edge_g
	0x00, //edge_b
	0x00, //edge_x 11
	0x70,
	0x00, //edge_y 12
	0x70,
	0x44, //edge_coef_a edge_coef_b 0000 0000
};

static char DSI_HBM_CE_D65_MDNIE_2[] = {
	0xDE,
	0x00, //gamut_gamma_on gamut_blk_shift 0000 0000
	0x40, //gamut_scale
	0x04, //gamut_r1
	0x00,
	0x00, //gamut_r2
	0x00,
	0x00, //gamut_r3
	0x00,
	0x00, //gamut_g1
	0x00,
	0x04, //gamut_g2
	0x00,
	0x00, //gamut_g3
	0x00,
	0x00, //gamut_b1
	0x00,
	0x00, //gamut_b2
	0x00,
	0x04, //gamut_b3
	0x00,
	0x10, //slce_on cadrx_en 0000 0000
	0x00, //glare_on
	0x08, //lce_gain 000 0000
	0x30, //lce_color_gain 00 0000
	0x40, //lce_min_ref_offset
	0xb0, //lce_illum_gain
	0x01, //lce_ref_offset 9
	0xbf,
	0x00, //lce_ref_gain 9
	0xd0,
	0x77, //lce_block_size h v 0000 0000
	0x00, //lce_dark_th 000
	0x07, //lce_reduct_slope 0000
	0x55, //lce_black cc0 cc1 color_th 0 0 0 0000
	0x00, //lce_large_w
	0x00, //lce_med_w
	0x10, //lce_small_w
	0x00, //cadrx_dit_en shadow_corr_en ref_ctrl_en 0 0 0
	0x00, //cadrx_gain
	0x0d, //cadrx_low_intensity_th
	0xe1, //cadrx_high_intensity_th
	0x07, //cadrx_ui_zerobincnt_th
	0x29, //cadrx_ui_ratio_th
	0x04,
	0x0f, //cadrx_entire_freq
	0xa0,
	0x00,
	0x03, //cadrx_peak_th_in_freq
	0x7a,
	0xa0,
	0x00, //cadrx_high_peak_th_in_freq
	0x25,
	0x1c,
	0x24, //cadrx_dit_color_gain
	0x00, //cadrx_dit_illum_a0
	0x48, //cadrx_dit_illum_a1
	0xea, //cadrx_dit_illum_a2
	0xfa, //cadrx_dit_illum_a3
	0x00, //cadrx_dit_illum_b0
	0x10,
	0x00, //cadrx_dit_illum_b1
	0x10,
	0x00, //cadrx_dit_illum_b2
	0x9c,
	0x01, //cadrx_dit_illum_b3
	0x00,
	0x00, //cadrx_dit_illum_slope1
	0x00,
	0x00, //cadrx_dit_illum_slope2
	0xdc,
	0x03, //cadrx_dit_illum_slope3
	0xff,
	0x40, //cadrx_ui_illum_a1
	0x80, //cadrx_ui_illum_a2
	0xc0, //cadrx_ui_illum_a3
	0x00, //cadrx_ui_illum_offset1
	0x9a,
	0x00, //cadrx_ui_illum_offset2
	0x9a,
	0x00, //cadrx_ui_illum_offset3
	0x9a,
	0x00, //cadrx_ui_illum_offset4
	0x9a,
	0x00, //cadrx_ui_illum_slope1
	0x00,
	0x00, //cadrx_ui_illum_slope2
	0x00,
	0x00, //cadrx_ui_illum_slope3
	0x00,
	0x00, //cadrx_ui_illum_slope4
	0x00,
	0x40, //cadrx_ui_ref_a1
	0x80, //cadrx_ui_ref_a2
	0xc0, //cadrx_ui_ref_a3
	0x01, //cadrx_ui_ref_offset1
	0x0e,
	0x01, //cadrx_ui_ref_offset2
	0x0e,
	0x01, //cadrx_ui_ref_offset3
	0x0e,
	0x01, //cadrx_ui_ref_offset4
	0x0e,
	0x00, //cadrx_ui_ref_slope1
	0x00,
	0x00, //cadrx_ui_ref_slope2
	0x00,
	0x00, //cadrx_ui_ref_slope3
	0x00,
	0x00, //cadrx_ui_ref_slope4
	0x00,
	0x01, //cadrx_reg_ref_c1_offset
	0x0e,
	0x01, //cadrx_reg_ref_c2_offset
	0x3a,
	0x01, //cadrx_reg_ref_c3_offset
	0x44,
	0x00, //cadrx_reg_ref_c1_slope
	0x01,
	0x00, //cadrx_reg_ref_c2_slope
	0x43,
	0x00, //cadrx_reg_ref_c3_slope
	0x4b,
	0x00, //cadrx_sc_reg_a1
	0x18,
	0x00, //cadrx_sc_reg_b1
	0x1c,
	0x01, //cadrx_sc_k1_int
	0xff,
	0x01, //cadrx_sc_k2_int
	0xff,
	0x03, //cadrx_sc_w1_int
	0xff,
	0xe8,
	0x03, //cadrx_sc_w2_int
	0xff,
	0xe8,
	0x00, //flicker_w_en
	0x14, //flicker_w
	0x01, //flicker_trans_en
	0x04, //flicker_trans_slope
	0x00, //bl_en
	0x03, //bl_hbs_th
	0x10, //bl_lbs_th
	0xC8, //bl_illum_gain
	0x01, //bl_reg_gain 9
	0x2C,
	0x00, //bl_le_diff
	0x00, //bl_cadrx_gain
	0x04, //bl_trans_slope
	0x00, //le_en
	0x40, //le_diff
	0x03, //le_lmax 10
	0xB6,
	0x23, //le_p
	0x24, //le_w_tmf
	0x00, //le_w_struct_diff 9
	0x72,
	0x01, //le_w_struct_diff_gain
	0x00, //le_w_luminance 9
	0x0D,
	0x00, //le_luminance_slope 10
	0x04,
	0x80, //glare_blk_avg_th 0000 0000
	0x00, //glare_luma_gain 9
	0x28,
	0xa0, //glare_luma_ctl_start 0000 0000
	0x01, //glare_luma_th 0000 0000
	0x00, //glare_luma_step 9
	0x01,
	0x00, //glare_uni_luma_gain 9
	0x01,
	0x00, //glare_blk_max_th 0000 0000
	0x00, //glare_blk_max_w 0000 0000
	0x10, //glare_blk_max_curve blk_max_sh 0 0000
	0x00, //glare_con_ext_max_th 0000 0000
	0x00, //glare_con_ext_max_w 0000 0000
	0x30, //glare_con_ext_max_sign con_ext_max_curve con_ext_max_sh 0 0 0000
	0x00, //glare_max_cnt_th 0000
	0x00, //glare_max_cnt_th 0000 0000
	0x00, //glare_max_cnt_w 0000 0000
	0x30, //glare_max_cnt_sign max_cnt_curve max_cnt_sh 0 0 0000
	0x00, //glare_y_avg_th 0000 0000
	0x00, //glare_y_avg_w 0000 0000
	0x30, //glare_y_avg_sign y_avg_curve y_avg_sh 0 0 0000
	0x00, //glare_max_cnt_th_dmc 0000
	0x00, //glare_max_cnt_th_dmc 0000 0000
	0x01, //nr fa de cs gamma 0 0000
	0xff, //nr_mask_th
	0x00, //de_gain 10
	0x00,
	0x02, //de_maxplus 11
	0x00,
	0x02, //de_maxminus 11
	0x00,
	0x14, //fa_edge_th
	0x00, //fa_step_p 13
	0x01,
	0x00, //fa_step_n 13
	0x01,
	0x00, //fa_max_de_gain 13
	0x10,
	0x00, //fa_pcl_ppi 14
	0x00,
	0x28, //fa_os_cnt_10_co
	0x3c, //fa_os_cnt_20_co
	0x04, //fa_edge_cnt_weight
	0x0f, //fa_avg_y_weight
	0x01, //cs_gain 10
	0x20,
	0x00, //curve_0
	0x21, //curve_1
	0x38, //curve_2
	0x4c, //curve_3
	0x5d, //curve_4
	0x6e, //curve_5
	0x7d, //curve_6
	0x8c, //curve_7
	0x9a, //curve_8
	0xa8, //curve_9
	0xb5, //curve_10
	0xc2, //curve_11
	0xcf, //curve_12
	0xdc, //curve_13
	0xe8, //curve_14
	0xf5, //curve_15
	0x01, //curve_16
	0x00,
	0x00, //curve_offset 10
	0x00,
	0x00, //curve_low_x
	0x00, //blank
	0x00, //curve_low_y
};

static char DSI_HBM_CE_D65_MDNIE_3[] = {
	0xDD,
	0x01, //mdnie_en
	0x00, //mask 00 0000
	0x03, //edge 00
	0xfc, //ascr algo aolce gamut 00 00 00 00
	0xff, //partial_en1
	0xff, //partial_en2
	0x00, //roi_en
	0x00, //roi_block
	0x00, //roi_sx
	0x00, //roi_sx
	0x00, //roi_sy
	0x00, //roi_sy
	0x00, //roi_ex
	0x00, //roi_ex
	0x00, //roi_ey
	0x00, //roi_ey
	0xff, //trans_on trans_block 0 000 0000
	0x04, //trans_slope
	0x01, //trans_interval
	//end
};

static char DSI_RGB_SENSOR_MDNIE_1[] = {
	//start
	0xDF,
	0x00, //linear_on ascr_skin_on 0000 0000
	0x6a, //ascr_skin_cb
	0x9a, //ascr_skin_cr
	0x25, //ascr_dist_up
	0x1a, //ascr_dist_down
	0x16, //ascr_dist_right
	0x2a, //ascr_dist_left
	0x00, //ascr_div_up 20
	0x37,
	0x5a,
	0x00, //ascr_div_down
	0x4e,
	0xc5,
	0x00, //ascr_div_right
	0x5d,
	0x17,
	0x00, //ascr_div_left
	0x30,
	0xc3,
	0xff, //ascr_skin_Rr
	0x00, //ascr_skin_Rg
	0x00, //ascr_skin_Rb
	0xff, //ascr_skin_Yr
	0xff, //ascr_skin_Yg
	0x00, //ascr_skin_Yb
	0xff, //ascr_skin_Mr
	0x00, //ascr_skin_Mg
	0xff, //ascr_skin_Mb
	0xff, //ascr_skin_Wr
	0xff, //ascr_skin_Wg
	0xff, //ascr_skin_Wb
	0x00, //ascr_Cr
	0xff, //ascr_Rr
	0xff, //ascr_Cg
	0x00, //ascr_Rg
	0xff, //ascr_Cb
	0x00, //ascr_Rb
	0xff, //ascr_Mr
	0x00, //ascr_Gr
	0x00, //ascr_Mg
	0xff, //ascr_Gg
	0xff, //ascr_Mb
	0x00, //ascr_Gb
	0xff, //ascr_Yr
	0x00, //ascr_Br
	0xff, //ascr_Yg
	0x00, //ascr_Bg
	0x00, //ascr_Yb
	0xff, //ascr_Bb
	0xff, //ascr_Wr
	0x00, //ascr_Kr
	0xff, //ascr_Wg
	0x00, //ascr_Kg
	0xff, //ascr_Wb
	0x00, //ascr_Kb
	0x0F, //edge_ctrl
	0x00, //edge_radius 12
	0x6F,
	0x05, //edge_aa_factor
	0x00, //edge_r
	0x00, //edge_g
	0x00, //edge_b
	0x00, //edge_x 11
	0x70,
	0x00, //edge_y 12
	0x70,
	0x44, //edge_coef_a edge_coef_b 0000 0000
};

static char DSI_RGB_SENSOR_MDNIE_2[] = {
	0xDE,
	0x00, //gamut_gamma_on gamut_blk_shift 0000 0000
	0x40, //gamut_scale
	0x04, //gamut_r1
	0x00,
	0x00, //gamut_r2
	0x00,
	0x00, //gamut_r3
	0x00,
	0x00, //gamut_g1
	0x00,
	0x04, //gamut_g2
	0x00,
	0x00, //gamut_g3
	0x00,
	0x00, //gamut_b1
	0x00,
	0x00, //gamut_b2
	0x00,
	0x04, //gamut_b3
	0x00,
	0x00, //slce_on cadrx_en 0000 0000
	0x00, //glare_on
	0x00, //lce_gain 000 0000
	0x24, //lce_color_gain 00 0000
	0x96, //lce_min_ref_offset
	0xb3, //lce_illum_gain
	0x01, //lce_ref_offset 9
	0x0e,
	0x01, //lce_ref_gain 9
	0x00,
	0x77, //lce_block_size h v 0000 0000
	0x03, //lce_dark_th 000
	0x07, //lce_reduct_slope 0000
	0x40, //lce_black cc0 cc1 color_th 0 0 0 0000
	0x08, //lce_large_w
	0x04, //lce_med_w
	0x04, //lce_small_w
	0x00, //cadrx_dit_en shadow_corr_en ref_ctrl_en 0 0 0
	0x00, //cadrx_gain
	0x0d, //cadrx_low_intensity_th
	0xe1, //cadrx_high_intensity_th
	0x07, //cadrx_ui_zerobincnt_th
	0x29, //cadrx_ui_ratio_th
	0x04,
	0x0f, //cadrx_entire_freq
	0xa0,
	0x00,
	0x03, //cadrx_peak_th_in_freq
	0x7a,
	0xa0,
	0x00, //cadrx_high_peak_th_in_freq
	0x25,
	0x1c,
	0x24, //cadrx_dit_color_gain
	0x00, //cadrx_dit_illum_a0
	0x48, //cadrx_dit_illum_a1
	0xea, //cadrx_dit_illum_a2
	0xfa, //cadrx_dit_illum_a3
	0x00, //cadrx_dit_illum_b0
	0x10,
	0x00, //cadrx_dit_illum_b1
	0x10,
	0x00, //cadrx_dit_illum_b2
	0x9c,
	0x01, //cadrx_dit_illum_b3
	0x00,
	0x00, //cadrx_dit_illum_slope1
	0x00,
	0x00, //cadrx_dit_illum_slope2
	0xdc,
	0x03, //cadrx_dit_illum_slope3
	0xff,
	0x40, //cadrx_ui_illum_a1
	0x80, //cadrx_ui_illum_a2
	0xc0, //cadrx_ui_illum_a3
	0x00, //cadrx_ui_illum_offset1
	0x9a,
	0x00, //cadrx_ui_illum_offset2
	0x9a,
	0x00, //cadrx_ui_illum_offset3
	0x9a,
	0x00, //cadrx_ui_illum_offset4
	0x9a,
	0x00, //cadrx_ui_illum_slope1
	0x00,
	0x00, //cadrx_ui_illum_slope2
	0x00,
	0x00, //cadrx_ui_illum_slope3
	0x00,
	0x00, //cadrx_ui_illum_slope4
	0x00,
	0x40, //cadrx_ui_ref_a1
	0x80, //cadrx_ui_ref_a2
	0xc0, //cadrx_ui_ref_a3
	0x01, //cadrx_ui_ref_offset1
	0x0e,
	0x01, //cadrx_ui_ref_offset2
	0x0e,
	0x01, //cadrx_ui_ref_offset3
	0x0e,
	0x01, //cadrx_ui_ref_offset4
	0x0e,
	0x00, //cadrx_ui_ref_slope1
	0x00,
	0x00, //cadrx_ui_ref_slope2
	0x00,
	0x00, //cadrx_ui_ref_slope3
	0x00,
	0x00, //cadrx_ui_ref_slope4
	0x00,
	0x01, //cadrx_reg_ref_c1_offset
	0x0e,
	0x01, //cadrx_reg_ref_c2_offset
	0x3a,
	0x01, //cadrx_reg_ref_c3_offset
	0x44,
	0x00, //cadrx_reg_ref_c1_slope
	0x01,
	0x00, //cadrx_reg_ref_c2_slope
	0x43,
	0x00, //cadrx_reg_ref_c3_slope
	0x4b,
	0x00, //cadrx_sc_reg_a1
	0x18,
	0x00, //cadrx_sc_reg_b1
	0x1c,
	0x01, //cadrx_sc_k1_int
	0xff,
	0x01, //cadrx_sc_k2_int
	0xff,
	0x03, //cadrx_sc_w1_int
	0xff,
	0xe8,
	0x03, //cadrx_sc_w2_int
	0xff,
	0xe8,
	0x00, //flicker_w_en
	0x14, //flicker_w
	0x01, //flicker_trans_en
	0x04, //flicker_trans_slope
	0x00, //bl_en
	0x03, //bl_hbs_th
	0x10, //bl_lbs_th
	0xC8, //bl_illum_gain
	0x01, //bl_reg_gain 9
	0x2C,
	0x00, //bl_le_diff
	0x00, //bl_cadrx_gain
	0x04, //bl_trans_slope
	0x00, //le_en
	0x40, //le_diff
	0x03, //le_lmax 10
	0xB6,
	0x23, //le_p
	0x24, //le_w_tmf
	0x00, //le_w_struct_diff 9
	0x72,
	0x01, //le_w_struct_diff_gain
	0x00, //le_w_luminance 9
	0x0D,
	0x00, //le_luminance_slope 10
	0x04,
	0x80, //glare_blk_avg_th 0000 0000
	0x00, //glare_luma_gain 9
	0x28,
	0xa0, //glare_luma_ctl_start 0000 0000
	0x01, //glare_luma_th 0000 0000
	0x00, //glare_luma_step 9
	0x01,
	0x00, //glare_uni_luma_gain 9
	0x01,
	0x00, //glare_blk_max_th 0000 0000
	0x00, //glare_blk_max_w 0000 0000
	0x10, //glare_blk_max_curve blk_max_sh 0 0000
	0x00, //glare_con_ext_max_th 0000 0000
	0x00, //glare_con_ext_max_w 0000 0000
	0x30, //glare_con_ext_max_sign con_ext_max_curve con_ext_max_sh 0 0 0000
	0x00, //glare_max_cnt_th 0000
	0x00, //glare_max_cnt_th 0000 0000
	0x00, //glare_max_cnt_w 0000 0000
	0x30, //glare_max_cnt_sign max_cnt_curve max_cnt_sh 0 0 0000
	0x00, //glare_y_avg_th 0000 0000
	0x00, //glare_y_avg_w 0000 0000
	0x30, //glare_y_avg_sign y_avg_curve y_avg_sh 0 0 0000
	0x00, //glare_max_cnt_th_dmc 0000
	0x00, //glare_max_cnt_th_dmc 0000 0000
	0x00, //nr fa de cs gamma 0 0000
	0xff, //nr_mask_th
	0x00, //de_gain 10
	0x00,
	0x02, //de_maxplus 11
	0x00,
	0x02, //de_maxminus 11
	0x00,
	0x14, //fa_edge_th
	0x00, //fa_step_p 13
	0x01,
	0x00, //fa_step_n 13
	0x01,
	0x00, //fa_max_de_gain 13
	0x10,
	0x00, //fa_pcl_ppi 14
	0x00,
	0x28, //fa_os_cnt_10_co
	0x3c, //fa_os_cnt_20_co
	0x04, //fa_edge_cnt_weight
	0x0f, //fa_avg_y_weight
	0x01, //cs_gain 10
	0x00,
	0x00, //curve_0
	0x10, //curve_1
	0x20, //curve_2
	0x30, //curve_3
	0x40, //curve_4
	0x50, //curve_5
	0x60, //curve_6
	0x70, //curve_7
	0x80, //curve_8
	0x90, //curve_9
	0xa0, //curve_10
	0xb0, //curve_11
	0xc0, //curve_12
	0xd0, //curve_13
	0xe0, //curve_14
	0xf0, //curve_15
	0x01, //curve_16
	0x00,
	0x00, //curve_offset 10
	0x00,
	0x00, //curve_low_x
	0x00, //blank
	0x00, //curve_low_y
};

static char DSI_RGB_SENSOR_MDNIE_3[] = {
	0xDD,
	0x01, //mdnie_en
	0x00, //mask 00 0000
	0x03, //edge 00
	0xf0, //ascr algo aolce gamut 00 00 00 00
	0xff, //partial_en1
	0xff, //partial_en2
	0x00, //roi_en
	0x00, //roi_block
	0x00, //roi_sx
	0x00, //roi_sx
	0x00, //roi_sy
	0x00, //roi_sy
	0x00, //roi_ex
	0x00, //roi_ex
	0x00, //roi_ey
	0x00, //roi_ey
	0xff, //trans_on trans_block 0 000 0000
	0x04, //trans_slope
	0x01, //trans_interval
	//end
};

static char DSI_SCREEN_CURTAIN_MDNIE_1[] = {
	//start
	0xDF,
	0x01, //linear_on ascr_skin_on 0000 0000
	0x6a, //ascr_skin_cb
	0x9a, //ascr_skin_cr
	0x25, //ascr_dist_up
	0x1a, //ascr_dist_down
	0x16, //ascr_dist_right
	0x2a, //ascr_dist_left
	0x00, //ascr_div_up 20
	0x37,
	0x5a,
	0x00, //ascr_div_down
	0x4e,
	0xc5,
	0x00, //ascr_div_right
	0x5d,
	0x17,
	0x00, //ascr_div_left
	0x30,
	0xc3,
	0x00, //ascr_skin_Rr
	0x00, //ascr_skin_Rg
	0x00, //ascr_skin_Rb
	0x00, //ascr_skin_Yr
	0x00, //ascr_skin_Yg
	0x00, //ascr_skin_Yb
	0x00, //ascr_skin_Mr
	0x00, //ascr_skin_Mg
	0x00, //ascr_skin_Mb
	0x00, //ascr_skin_Wr
	0x00, //ascr_skin_Wg
	0x00, //ascr_skin_Wb
	0x00, //ascr_Cr
	0x00, //ascr_Rr
	0x00, //ascr_Cg
	0x00, //ascr_Rg
	0x00, //ascr_Cb
	0x00, //ascr_Rb
	0x00, //ascr_Mr
	0x00, //ascr_Gr
	0x00, //ascr_Mg
	0x00, //ascr_Gg
	0x00, //ascr_Mb
	0x00, //ascr_Gb
	0x00, //ascr_Yr
	0x00, //ascr_Br
	0x00, //ascr_Yg
	0x00, //ascr_Bg
	0x00, //ascr_Yb
	0x00, //ascr_Bb
	0x00, //ascr_Wr
	0x00, //ascr_Kr
	0x00, //ascr_Wg
	0x00, //ascr_Kg
	0x00, //ascr_Wb
	0x00, //ascr_Kb
	0x0F, //edge_ctrl
	0x00, //edge_radius 12
	0x6F,
	0x05, //edge_aa_factor
	0x00, //edge_r
	0x00, //edge_g
	0x00, //edge_b
	0x00, //edge_x 11
	0x70,
	0x00, //edge_y 12
	0x70,
	0x44, //edge_coef_a edge_coef_b 0000 0000
};

static char DSI_SCREEN_CURTAIN_MDNIE_2[] = {
	0xDE,
	0x00, //gamut_gamma_on gamut_blk_shift 0000 0000
	0x40, //gamut_scale
	0x04, //gamut_r1
	0x00,
	0x00, //gamut_r2
	0x00,
	0x00, //gamut_r3
	0x00,
	0x00, //gamut_g1
	0x00,
	0x04, //gamut_g2
	0x00,
	0x00, //gamut_g3
	0x00,
	0x00, //gamut_b1
	0x00,
	0x00, //gamut_b2
	0x00,
	0x04, //gamut_b3
	0x00,
	0x00, //slce_on cadrx_en 0000 0000
	0x00, //glare_on
	0x00, //lce_gain 000 0000
	0x24, //lce_color_gain 00 0000
	0x96, //lce_min_ref_offset
	0xb3, //lce_illum_gain
	0x01, //lce_ref_offset 9
	0x0e,
	0x01, //lce_ref_gain 9
	0x00,
	0x77, //lce_block_size h v 0000 0000
	0x03, //lce_dark_th 000
	0x07, //lce_reduct_slope 0000
	0x40, //lce_black cc0 cc1 color_th 0 0 0 0000
	0x08, //lce_large_w
	0x04, //lce_med_w
	0x04, //lce_small_w
	0x00, //cadrx_dit_en shadow_corr_en ref_ctrl_en 0 0 0
	0x00, //cadrx_gain
	0x0d, //cadrx_low_intensity_th
	0xe1, //cadrx_high_intensity_th
	0x07, //cadrx_ui_zerobincnt_th
	0x29, //cadrx_ui_ratio_th
	0x04,
	0x0f, //cadrx_entire_freq
	0xa0,
	0x00,
	0x03, //cadrx_peak_th_in_freq
	0x7a,
	0xa0,
	0x00, //cadrx_high_peak_th_in_freq
	0x25,
	0x1c,
	0x24, //cadrx_dit_color_gain
	0x00, //cadrx_dit_illum_a0
	0x48, //cadrx_dit_illum_a1
	0xea, //cadrx_dit_illum_a2
	0xfa, //cadrx_dit_illum_a3
	0x00, //cadrx_dit_illum_b0
	0x10,
	0x00, //cadrx_dit_illum_b1
	0x10,
	0x00, //cadrx_dit_illum_b2
	0x9c,
	0x01, //cadrx_dit_illum_b3
	0x00,
	0x00, //cadrx_dit_illum_slope1
	0x00,
	0x00, //cadrx_dit_illum_slope2
	0xdc,
	0x03, //cadrx_dit_illum_slope3
	0xff,
	0x40, //cadrx_ui_illum_a1
	0x80, //cadrx_ui_illum_a2
	0xc0, //cadrx_ui_illum_a3
	0x00, //cadrx_ui_illum_offset1
	0x9a,
	0x00, //cadrx_ui_illum_offset2
	0x9a,
	0x00, //cadrx_ui_illum_offset3
	0x9a,
	0x00, //cadrx_ui_illum_offset4
	0x9a,
	0x00, //cadrx_ui_illum_slope1
	0x00,
	0x00, //cadrx_ui_illum_slope2
	0x00,
	0x00, //cadrx_ui_illum_slope3
	0x00,
	0x00, //cadrx_ui_illum_slope4
	0x00,
	0x40, //cadrx_ui_ref_a1
	0x80, //cadrx_ui_ref_a2
	0xc0, //cadrx_ui_ref_a3
	0x01, //cadrx_ui_ref_offset1
	0x0e,
	0x01, //cadrx_ui_ref_offset2
	0x0e,
	0x01, //cadrx_ui_ref_offset3
	0x0e,
	0x01, //cadrx_ui_ref_offset4
	0x0e,
	0x00, //cadrx_ui_ref_slope1
	0x00,
	0x00, //cadrx_ui_ref_slope2
	0x00,
	0x00, //cadrx_ui_ref_slope3
	0x00,
	0x00, //cadrx_ui_ref_slope4
	0x00,
	0x01, //cadrx_reg_ref_c1_offset
	0x0e,
	0x01, //cadrx_reg_ref_c2_offset
	0x3a,
	0x01, //cadrx_reg_ref_c3_offset
	0x44,
	0x00, //cadrx_reg_ref_c1_slope
	0x01,
	0x00, //cadrx_reg_ref_c2_slope
	0x43,
	0x00, //cadrx_reg_ref_c3_slope
	0x4b,
	0x00, //cadrx_sc_reg_a1
	0x18,
	0x00, //cadrx_sc_reg_b1
	0x1c,
	0x01, //cadrx_sc_k1_int
	0xff,
	0x01, //cadrx_sc_k2_int
	0xff,
	0x03, //cadrx_sc_w1_int
	0xff,
	0xe8,
	0x03, //cadrx_sc_w2_int
	0xff,
	0xe8,
	0x00, //flicker_w_en
	0x14, //flicker_w
	0x01, //flicker_trans_en
	0x04, //flicker_trans_slope
	0x00, //bl_en
	0x03, //bl_hbs_th
	0x10, //bl_lbs_th
	0xC8, //bl_illum_gain
	0x01, //bl_reg_gain 9
	0x2C,
	0x00, //bl_le_diff
	0x00, //bl_cadrx_gain
	0x04, //bl_trans_slope
	0x00, //le_en
	0x40, //le_diff
	0x03, //le_lmax 10
	0xB6,
	0x23, //le_p
	0x24, //le_w_tmf
	0x00, //le_w_struct_diff 9
	0x72,
	0x01, //le_w_struct_diff_gain
	0x00, //le_w_luminance 9
	0x0D,
	0x00, //le_luminance_slope 10
	0x04,
	0x80, //glare_blk_avg_th 0000 0000
	0x00, //glare_luma_gain 9
	0x28,
	0xa0, //glare_luma_ctl_start 0000 0000
	0x01, //glare_luma_th 0000 0000
	0x00, //glare_luma_step 9
	0x01,
	0x00, //glare_uni_luma_gain 9
	0x01,
	0x00, //glare_blk_max_th 0000 0000
	0x00, //glare_blk_max_w 0000 0000
	0x10, //glare_blk_max_curve blk_max_sh 0 0000
	0x00, //glare_con_ext_max_th 0000 0000
	0x00, //glare_con_ext_max_w 0000 0000
	0x30, //glare_con_ext_max_sign con_ext_max_curve con_ext_max_sh 0 0 0000
	0x00, //glare_max_cnt_th 0000
	0x00, //glare_max_cnt_th 0000 0000
	0x00, //glare_max_cnt_w 0000 0000
	0x30, //glare_max_cnt_sign max_cnt_curve max_cnt_sh 0 0 0000
	0x00, //glare_y_avg_th 0000 0000
	0x00, //glare_y_avg_w 0000 0000
	0x30, //glare_y_avg_sign y_avg_curve y_avg_sh 0 0 0000
	0x00, //glare_max_cnt_th_dmc 0000
	0x00, //glare_max_cnt_th_dmc 0000 0000
	0x00, //nr fa de cs gamma 0 0000
	0xff, //nr_mask_th
	0x00, //de_gain 10
	0x00,
	0x02, //de_maxplus 11
	0x00,
	0x02, //de_maxminus 11
	0x00,
	0x14, //fa_edge_th
	0x00, //fa_step_p 13
	0x01,
	0x00, //fa_step_n 13
	0x01,
	0x00, //fa_max_de_gain 13
	0x10,
	0x00, //fa_pcl_ppi 14
	0x00,
	0x28, //fa_os_cnt_10_co
	0x3c, //fa_os_cnt_20_co
	0x04, //fa_edge_cnt_weight
	0x0f, //fa_avg_y_weight
	0x01, //cs_gain 10
	0x00,
	0x00, //curve_0
	0x10, //curve_1
	0x20, //curve_2
	0x30, //curve_3
	0x40, //curve_4
	0x50, //curve_5
	0x60, //curve_6
	0x70, //curve_7
	0x80, //curve_8
	0x90, //curve_9
	0xa0, //curve_10
	0xb0, //curve_11
	0xc0, //curve_12
	0xd0, //curve_13
	0xe0, //curve_14
	0xf0, //curve_15
	0x01, //curve_16
	0x00,
	0x00, //curve_offset 10
	0x00,
	0x00, //curve_low_x
	0x00, //blank
	0x00, //curve_low_y
};

static char DSI_SCREEN_CURTAIN_MDNIE_3[] = {
	0xDD,
	0x01, //mdnie_en
	0x00, //mask 00 0000
	0x03, //edge 00
	0xf0, //ascr algo aolce gamut 00 00 00 00
	0xff, //partial_en1
	0xff, //partial_en2
	0x00, //roi_en
	0x00, //roi_block
	0x00, //roi_sx
	0x00, //roi_sx
	0x00, //roi_sy
	0x00, //roi_sy
	0x00, //roi_ex
	0x00, //roi_ex
	0x00, //roi_ey
	0x00, //roi_ey
	0xff, //trans_on trans_block 0 000 0000
	0x04, //trans_slope
	0x01, //trans_interval
	//end
};

static char DSI_LIGHT_NOTIFICATION_MDNIE_1[] = {
	//start
	0xDF,
	0x01, //linear_on ascr_skin_on 0000 0000
	0x6a, //ascr_skin_cb
	0x9a, //ascr_skin_cr
	0x25, //ascr_dist_up
	0x1a, //ascr_dist_down
	0x16, //ascr_dist_right
	0x2a, //ascr_dist_left
	0x00, //ascr_div_up 20
	0x37,
	0x5a,
	0x00, //ascr_div_down
	0x4e,
	0xc5,
	0x00, //ascr_div_right
	0x5d,
	0x17,
	0x00, //ascr_div_left
	0x30,
	0xc3,
	0xff, //ascr_skin_Rr
	0x00, //ascr_skin_Rg
	0x00, //ascr_skin_Rb
	0xff, //ascr_skin_Yr
	0xf9, //ascr_skin_Yg
	0x00, //ascr_skin_Yb
	0xff, //ascr_skin_Mr
	0x00, //ascr_skin_Mg
	0xac, //ascr_skin_Mb
	0xff, //ascr_skin_Wr
	0xf9, //ascr_skin_Wg
	0xac, //ascr_skin_Wb
	0x66, //ascr_Cr
	0xff, //ascr_Rr
	0xf9, //ascr_Cg
	0x60, //ascr_Rg
	0xac, //ascr_Cb
	0x13, //ascr_Rb
	0xff, //ascr_Mr
	0x66, //ascr_Gr
	0x60, //ascr_Mg
	0xf9, //ascr_Gg
	0xac, //ascr_Mb
	0x13, //ascr_Gb
	0xff, //ascr_Yr
	0x66, //ascr_Br
	0xf9, //ascr_Yg
	0x60, //ascr_Bg
	0x13, //ascr_Yb
	0xac, //ascr_Bb
	0xff, //ascr_Wr
	0x66, //ascr_Kr
	0xf9, //ascr_Wg
	0x60, //ascr_Kg
	0xac, //ascr_Wb
	0x13, //ascr_Kb
	0x0F, //edge_ctrl
	0x00, //edge_radius 12
	0x6F,
	0x05, //edge_aa_factor
	0x00, //edge_r
	0x00, //edge_g
	0x00, //edge_b
	0x00, //edge_x 11
	0x70,
	0x00, //edge_y 12
	0x70,
	0x44, //edge_coef_a edge_coef_b 0000 0000
};

static char DSI_LIGHT_NOTIFICATION_MDNIE_2[] = {
	0xDE,
	0x00, //gamut_gamma_on gamut_blk_shift 0000 0000
	0x40, //gamut_scale
	0x04, //gamut_r1
	0x00,
	0x00, //gamut_r2
	0x00,
	0x00, //gamut_r3
	0x00,
	0x00, //gamut_g1
	0x00,
	0x04, //gamut_g2
	0x00,
	0x00, //gamut_g3
	0x00,
	0x00, //gamut_b1
	0x00,
	0x00, //gamut_b2
	0x00,
	0x04, //gamut_b3
	0x00,
	0x00, //slce_on cadrx_en 0000 0000
	0x11, //glare_on
	0x00, //lce_gain 000 0000
	0x24, //lce_color_gain 00 0000
	0x96, //lce_min_ref_offset
	0xb3, //lce_illum_gain
	0x01, //lce_ref_offset 9
	0x0e,
	0x01, //lce_ref_gain 9
	0x00,
	0x77, //lce_block_size h v 0000 0000
	0x03, //lce_dark_th 000
	0x07, //lce_reduct_slope 0000
	0x40, //lce_black cc0 cc1 color_th 0 0 0 0000
	0x08, //lce_large_w
	0x04, //lce_med_w
	0x04, //lce_small_w
	0x00, //cadrx_dit_en shadow_corr_en ref_ctrl_en 0 0 0
	0x00, //cadrx_gain
	0x0d, //cadrx_low_intensity_th
	0xe1, //cadrx_high_intensity_th
	0x07, //cadrx_ui_zerobincnt_th
	0x29, //cadrx_ui_ratio_th
	0x04,
	0x0f, //cadrx_entire_freq
	0xa0,
	0x00,
	0x03, //cadrx_peak_th_in_freq
	0x7a,
	0xa0,
	0x00, //cadrx_high_peak_th_in_freq
	0x25,
	0x1c,
	0x24, //cadrx_dit_color_gain
	0x00, //cadrx_dit_illum_a0
	0x48, //cadrx_dit_illum_a1
	0xea, //cadrx_dit_illum_a2
	0xfa, //cadrx_dit_illum_a3
	0x00, //cadrx_dit_illum_b0
	0x10,
	0x00, //cadrx_dit_illum_b1
	0x10,
	0x00, //cadrx_dit_illum_b2
	0x9c,
	0x01, //cadrx_dit_illum_b3
	0x00,
	0x00, //cadrx_dit_illum_slope1
	0x00,
	0x00, //cadrx_dit_illum_slope2
	0xdc,
	0x03, //cadrx_dit_illum_slope3
	0xff,
	0x40, //cadrx_ui_illum_a1
	0x80, //cadrx_ui_illum_a2
	0xc0, //cadrx_ui_illum_a3
	0x00, //cadrx_ui_illum_offset1
	0x9a,
	0x00, //cadrx_ui_illum_offset2
	0x9a,
	0x00, //cadrx_ui_illum_offset3
	0x9a,
	0x00, //cadrx_ui_illum_offset4
	0x9a,
	0x00, //cadrx_ui_illum_slope1
	0x00,
	0x00, //cadrx_ui_illum_slope2
	0x00,
	0x00, //cadrx_ui_illum_slope3
	0x00,
	0x00, //cadrx_ui_illum_slope4
	0x00,
	0x40, //cadrx_ui_ref_a1
	0x80, //cadrx_ui_ref_a2
	0xc0, //cadrx_ui_ref_a3
	0x01, //cadrx_ui_ref_offset1
	0x0e,
	0x01, //cadrx_ui_ref_offset2
	0x0e,
	0x01, //cadrx_ui_ref_offset3
	0x0e,
	0x01, //cadrx_ui_ref_offset4
	0x0e,
	0x00, //cadrx_ui_ref_slope1
	0x00,
	0x00, //cadrx_ui_ref_slope2
	0x00,
	0x00, //cadrx_ui_ref_slope3
	0x00,
	0x00, //cadrx_ui_ref_slope4
	0x00,
	0x01, //cadrx_reg_ref_c1_offset
	0x0e,
	0x01, //cadrx_reg_ref_c2_offset
	0x3a,
	0x01, //cadrx_reg_ref_c3_offset
	0x44,
	0x00, //cadrx_reg_ref_c1_slope
	0x01,
	0x00, //cadrx_reg_ref_c2_slope
	0x43,
	0x00, //cadrx_reg_ref_c3_slope
	0x4b,
	0x00, //cadrx_sc_reg_a1
	0x18,
	0x00, //cadrx_sc_reg_b1
	0x1c,
	0x01, //cadrx_sc_k1_int
	0xff,
	0x01, //cadrx_sc_k2_int
	0xff,
	0x03, //cadrx_sc_w1_int
	0xff,
	0xe8,
	0x03, //cadrx_sc_w2_int
	0xff,
	0xe8,
	0x00, //flicker_w_en
	0x14, //flicker_w
	0x01, //flicker_trans_en
	0x04, //flicker_trans_slope
	0x00, //bl_en
	0x03, //bl_hbs_th
	0x10, //bl_lbs_th
	0xC8, //bl_illum_gain
	0x01, //bl_reg_gain 9
	0x2C,
	0x00, //bl_le_diff
	0x00, //bl_cadrx_gain
	0x04, //bl_trans_slope
	0x00, //le_en
	0x40, //le_diff
	0x03, //le_lmax 10
	0xB6,
	0x23, //le_p
	0x24, //le_w_tmf
	0x00, //le_w_struct_diff 9
	0x72,
	0x01, //le_w_struct_diff_gain
	0x00, //le_w_luminance 9
	0x0D,
	0x00, //le_luminance_slope 10
	0x04,
	0x80, //glare_blk_avg_th 0000 0000
	0x00, //glare_luma_gain 9
	0x00,
	0x00, //glare_luma_ctl_start 0000 0000
	0x05, //glare_luma_th 0000 0000
	0x00, //glare_luma_step 9
	0x01,
	0x01, //glare_uni_luma_gain 9
	0x00,
	0xf0, //glare_blk_max_th 0000 0000
	0x12, //glare_blk_max_w 0000 0000
	0x18, //glare_blk_max_curve blk_max_sh 0 0000
	0xf0, //glare_con_ext_max_th 0000 0000
	0x12, //glare_con_ext_max_w 0000 0000
	0x38, //glare_con_ext_max_sign con_ext_max_curve con_ext_max_sh 0 0 0000
	0x00, //glare_max_cnt_th 0000
	0x00, //glare_max_cnt_th 0000 0000
	0x02, //glare_max_cnt_w 0000 0000
	0x38, //glare_max_cnt_sign max_cnt_curve max_cnt_sh 0 0 0000
	0x00, //glare_y_avg_th 0000 0000
	0x18, //glare_y_avg_w 0000 0000
	0x38, //glare_y_avg_sign y_avg_curve y_avg_sh 0 0 0000
	0x00, //glare_max_cnt_th_dmc 0000
	0x05, //glare_max_cnt_th_dmc 0000 0000
	0x00, //nr fa de cs gamma 0 0000
	0xff, //nr_mask_th
	0x00, //de_gain 10
	0x00,
	0x02, //de_maxplus 11
	0x00,
	0x02, //de_maxminus 11
	0x00,
	0x14, //fa_edge_th
	0x00, //fa_step_p 13
	0x01,
	0x00, //fa_step_n 13
	0x01,
	0x00, //fa_max_de_gain 13
	0x10,
	0x00, //fa_pcl_ppi 14
	0x00,
	0x28, //fa_os_cnt_10_co
	0x3c, //fa_os_cnt_20_co
	0x04, //fa_edge_cnt_weight
	0x0f, //fa_avg_y_weight
	0x01, //cs_gain 10
	0x00,
	0x00, //curve_0
	0x10, //curve_1
	0x20, //curve_2
	0x30, //curve_3
	0x40, //curve_4
	0x50, //curve_5
	0x60, //curve_6
	0x70, //curve_7
	0x80, //curve_8
	0x90, //curve_9
	0xa0, //curve_10
	0xb0, //curve_11
	0xc0, //curve_12
	0xd0, //curve_13
	0xe0, //curve_14
	0xf0, //curve_15
	0x01, //curve_16
	0x00,
	0x00, //curve_offset 10
	0x00,
	0x00, //curve_low_x
	0x00, //blank
	0x00, //curve_low_y
};

static char DSI_LIGHT_NOTIFICATION_MDNIE_3[] = {
	0xDD,
	0x01, //mdnie_en
	0x00, //mask 00 0000
	0x03, //edge 00
	0xf0, //ascr algo aolce gamut 00 00 00 00
	0xff, //partial_en1
	0xff, //partial_en2
	0x00, //roi_en
	0x00, //roi_block
	0x00, //roi_sx
	0x00, //roi_sx
	0x00, //roi_sy
	0x00, //roi_sy
	0x00, //roi_ex
	0x00, //roi_ex
	0x00, //roi_ey
	0x00, //roi_ey
	0xff, //trans_on trans_block 0 000 0000
	0x04, //trans_slope
	0x01, //trans_interval
	//end
};

static unsigned char DSI_HDR_VIDEO_1_MDNIE_1[] = {
	//start
	0xDF,
	0x01, //linear_on ascr_skin_on 0000 0000
	0x67, //ascr_skin_cb
	0xa9, //ascr_skin_cr
	0x37, //ascr_dist_up
	0x29, //ascr_dist_down
	0x19, //ascr_dist_right
	0x47, //ascr_dist_left
	0x00, //ascr_div_up 20
	0x25,
	0x3d,
	0x00, //ascr_div_down
	0x31,
	0xf4,
	0x00, //ascr_div_right
	0x51,
	0xec,
	0x00, //ascr_div_left
	0x1c,
	0xd8,
	0xd0, //ascr_skin_Rr
	0x30, //ascr_skin_Rg
	0x50, //ascr_skin_Rb
	0xff, //ascr_skin_Yr
	0xd0, //ascr_skin_Yg
	0x00, //ascr_skin_Yb
	0xff, //ascr_skin_Mr
	0x00, //ascr_skin_Mg
	0xff, //ascr_skin_Mb
	0xff, //ascr_skin_Wr
	0xff, //ascr_skin_Wg
	0xff, //ascr_skin_Wb
	0x00, //ascr_Cr
	0xff, //ascr_Rr
	0xff, //ascr_Cg
	0x00, //ascr_Rg
	0xff, //ascr_Cb
	0x00, //ascr_Rb
	0xff, //ascr_Mr
	0x00, //ascr_Gr
	0x00, //ascr_Mg
	0xff, //ascr_Gg
	0xff, //ascr_Mb
	0x00, //ascr_Gb
	0xff, //ascr_Yr
	0x00, //ascr_Br
	0xff, //ascr_Yg
	0x00, //ascr_Bg
	0x00, //ascr_Yb
	0xff, //ascr_Bb
	0xff, //ascr_Wr
	0x00, //ascr_Kr
	0xf9, //ascr_Wg
	0x00, //ascr_Kg
	0xef, //ascr_Wb
	0x00, //ascr_Kb
	0x0F, //edge_ctrl
	0x00, //edge_radius 12
	0x6F,
	0x05, //edge_aa_factor
	0x00, //edge_r
	0x00, //edge_g
	0x00, //edge_b
	0x00, //edge_x 11
	0x70,
	0x00, //edge_y 12
	0x70,
	0x44, //edge_coef_a edge_coef_b 0000 0000
};

static unsigned char DSI_HDR_VIDEO_1_MDNIE_2[] = {
	0xDE,
	0x10, //gamut_gamma_on gamut_blk_shift 0000 0000
	0x40, //gamut_scale
	0x05, //gamut_r1
	0xf1,
	0xfe, //gamut_r2
	0xc3,
	0xff, //gamut_r3
	0x4c,
	0xff, //gamut_g1
	0xc3,
	0x03, //gamut_g2
	0xf1,
	0x00, //gamut_g3
	0x4c,
	0x00, //gamut_b1
	0x03,
	0xff, //gamut_b2
	0xec,
	0x04, //gamut_b3
	0x11,
	0x00, //slce_on cadrx_en 0000 0000
	0x11, //glare_on
	0x00, //lce_gain 000 0000
	0x24, //lce_color_gain 00 0000
	0x96, //lce_min_ref_offset
	0xb3, //lce_illum_gain
	0x01, //lce_ref_offset 9
	0x0e,
	0x01, //lce_ref_gain 9
	0x00,
	0x77, //lce_block_size h v 0000 0000
	0x03, //lce_dark_th 000
	0x07, //lce_reduct_slope 0000
	0x40, //lce_black cc0 cc1 color_th 0 0 0 0000
	0x08, //lce_large_w
	0x04, //lce_med_w
	0x04, //lce_small_w
	0x00, //cadrx_dit_en shadow_corr_en ref_ctrl_en 0 0 0
	0x00, //cadrx_gain
	0x0d, //cadrx_low_intensity_th
	0xe1, //cadrx_high_intensity_th
	0x07, //cadrx_ui_zerobincnt_th
	0x29, //cadrx_ui_ratio_th
	0x04,
	0x0f, //cadrx_entire_freq
	0xa0,
	0x00,
	0x03, //cadrx_peak_th_in_freq
	0x7a,
	0xa0,
	0x00, //cadrx_high_peak_th_in_freq
	0x25,
	0x1c,
	0x24, //cadrx_dit_color_gain
	0x00, //cadrx_dit_illum_a0
	0x48, //cadrx_dit_illum_a1
	0xea, //cadrx_dit_illum_a2
	0xfa, //cadrx_dit_illum_a3
	0x00, //cadrx_dit_illum_b0
	0x10,
	0x00, //cadrx_dit_illum_b1
	0x10,
	0x00, //cadrx_dit_illum_b2
	0x9c,
	0x01, //cadrx_dit_illum_b3
	0x00,
	0x00, //cadrx_dit_illum_slope1
	0x00,
	0x00, //cadrx_dit_illum_slope2
	0xdc,
	0x03, //cadrx_dit_illum_slope3
	0xff,
	0x40, //cadrx_ui_illum_a1
	0x80, //cadrx_ui_illum_a2
	0xc0, //cadrx_ui_illum_a3
	0x00, //cadrx_ui_illum_offset1
	0x9a,
	0x00, //cadrx_ui_illum_offset2
	0x9a,
	0x00, //cadrx_ui_illum_offset3
	0x9a,
	0x00, //cadrx_ui_illum_offset4
	0x9a,
	0x00, //cadrx_ui_illum_slope1
	0x00,
	0x00, //cadrx_ui_illum_slope2
	0x00,
	0x00, //cadrx_ui_illum_slope3
	0x00,
	0x00, //cadrx_ui_illum_slope4
	0x00,
	0x40, //cadrx_ui_ref_a1
	0x80, //cadrx_ui_ref_a2
	0xc0, //cadrx_ui_ref_a3
	0x01, //cadrx_ui_ref_offset1
	0x0e,
	0x01, //cadrx_ui_ref_offset2
	0x0e,
	0x01, //cadrx_ui_ref_offset3
	0x0e,
	0x01, //cadrx_ui_ref_offset4
	0x0e,
	0x00, //cadrx_ui_ref_slope1
	0x00,
	0x00, //cadrx_ui_ref_slope2
	0x00,
	0x00, //cadrx_ui_ref_slope3
	0x00,
	0x00, //cadrx_ui_ref_slope4
	0x00,
	0x01, //cadrx_reg_ref_c1_offset
	0x0e,
	0x01, //cadrx_reg_ref_c2_offset
	0x3a,
	0x01, //cadrx_reg_ref_c3_offset
	0x44,
	0x00, //cadrx_reg_ref_c1_slope
	0x01,
	0x00, //cadrx_reg_ref_c2_slope
	0x43,
	0x00, //cadrx_reg_ref_c3_slope
	0x4b,
	0x00, //cadrx_sc_reg_a1
	0x18,
	0x00, //cadrx_sc_reg_b1
	0x1c,
	0x01, //cadrx_sc_k1_int
	0xff,
	0x01, //cadrx_sc_k2_int
	0xff,
	0x03, //cadrx_sc_w1_int
	0xff,
	0xe8,
	0x03, //cadrx_sc_w2_int
	0xff,
	0xe8,
	0x00, //flicker_w_en
	0x14, //flicker_w
	0x01, //flicker_trans_en
	0x04, //flicker_trans_slope
	0x00, //bl_en
	0x03, //bl_hbs_th
	0x10, //bl_lbs_th
	0xC8, //bl_illum_gain
	0x01, //bl_reg_gain 9
	0x2C,
	0x00, //bl_le_diff
	0x00, //bl_cadrx_gain
	0x04, //bl_trans_slope
	0x00, //le_en
	0x40, //le_diff
	0x03, //le_lmax 10
	0xB6,
	0x23, //le_p
	0x24, //le_w_tmf
	0x00, //le_w_struct_diff 9
	0x72,
	0x01, //le_w_struct_diff_gain
	0x00, //le_w_luminance 9
	0x0D,
	0x00, //le_luminance_slope 10
	0x04,
	0x80, //glare_blk_avg_th 0000 0000
	0x00, //glare_luma_gain 9
	0x28,
	0xa0, //glare_luma_ctl_start 0000 0000
	0x01, //glare_luma_th 0000 0000
	0x00, //glare_luma_step 9
	0x01,
	0x00, //glare_uni_luma_gain 9
	0x01,
	0xf0, //glare_blk_max_th 0000 0000
	0x89, //glare_blk_max_w 0000 0000
	0x18, //glare_blk_max_curve blk_max_sh 0 0000
	0xf0, //glare_con_ext_max_th 0000 0000
	0x89, //glare_con_ext_max_w 0000 0000
	0x38, //glare_con_ext_max_sign con_ext_max_curve con_ext_max_sh 0 0 0000
	0x00, //glare_max_cnt_th 0000
	0x00, //glare_max_cnt_th 0000 0000
	0x0c, //glare_max_cnt_w 0000 0000
	0x38, //glare_max_cnt_sign max_cnt_curve max_cnt_sh 0 0 0000
	0xba, //glare_y_avg_th 0000 0000
	0x1e, //glare_y_avg_w 0000 0000
	0x38, //glare_y_avg_sign y_avg_curve y_avg_sh 0 0 0000
	0x00, //glare_max_cnt_th_dmc 0000
	0x05, //glare_max_cnt_th_dmc 0000 0000
	0x0f, //nr fa de cs gamma 0 0000
	0xff, //nr_mask_th
	0x00, //de_gain 10
	0x20,
	0x00, //de_maxplus 11
	0x40,
	0x00, //de_maxminus 11
	0x40,
	0x14, //fa_edge_th
	0x00, //fa_step_p 13
	0x01,
	0x00, //fa_step_n 13
	0x01,
	0x00, //fa_max_de_gain 13
	0x10,
	0x00, //fa_pcl_ppi 14
	0x00,
	0x28, //fa_os_cnt_10_co
	0x3c, //fa_os_cnt_20_co
	0x04, //fa_edge_cnt_weight
	0x0f, //fa_avg_y_weight
	0x01, //cs_gain 10
	0x60,
	0x00, //curve_0
	0x09, //curve_1
	0x0d, //curve_2
	0x16, //curve_3
	0x23, //curve_4
	0x31, //curve_5
	0x47, //curve_6
	0x60, //curve_7
	0x7f, //curve_8
	0xa3, //curve_9
	0xcf, //curve_10
	0xf6, //curve_11
	0xf9, //curve_12
	0xfc, //curve_13
	0xff, //curve_14
	0xff, //curve_15
	0x01, //curve_16
	0x00,
	0x00, //curve_offset 10
	0x00,
	0x05, //curve_low_x
	0x00, //blank
	0x00, //curve_low_y
};

static unsigned char DSI_HDR_VIDEO_1_MDNIE_3[] = {
	0xDD,
	0x01, //mdnie_en
	0x00, //mask 00 0000
	0x03, //edge 00
	0xf3, //ascr algo aolce gamut 00 00 00 00
	0xff, //partial_en1
	0xff, //partial_en2
	0x00, //roi_en
	0x00, //roi_block
	0x00, //roi_sx
	0x00, //roi_sx
	0x00, //roi_sy
	0x00, //roi_sy
	0x00, //roi_ex
	0x00, //roi_ex
	0x00, //roi_ey
	0x00, //roi_ey
	0xff, //trans_on trans_block 0 000 0000
	0x04, //trans_slope
	0x01, //trans_interval
	//end
};

static unsigned char DSI_HDR_VIDEO_2_MDNIE_1[] = {
	//start
	0xDF,
	0x01, //linear_on ascr_skin_on 0000 0000
	0x67, //ascr_skin_cb
	0xa9, //ascr_skin_cr
	0x37, //ascr_dist_up
	0x29, //ascr_dist_down
	0x19, //ascr_dist_right
	0x47, //ascr_dist_left
	0x00, //ascr_div_up 20
	0x25,
	0x3d,
	0x00, //ascr_div_down
	0x31,
	0xf4,
	0x00, //ascr_div_right
	0x51,
	0xec,
	0x00, //ascr_div_left
	0x1c,
	0xd8,
	0xf0, //ascr_skin_Rr
	0x38, //ascr_skin_Rg
	0x48, //ascr_skin_Rb
	0xff, //ascr_skin_Yr
	0xe0, //ascr_skin_Yg
	0x50, //ascr_skin_Yb
	0xff, //ascr_skin_Mr
	0x00, //ascr_skin_Mg
	0xff, //ascr_skin_Mb
	0xf4, //ascr_skin_Wr
	0xff, //ascr_skin_Wg
	0xf4, //ascr_skin_Wb
	0x00, //ascr_Cr
	0xff, //ascr_Rr
	0xff, //ascr_Cg
	0x00, //ascr_Rg
	0xd0, //ascr_Cb
	0x00, //ascr_Rb
	0xff, //ascr_Mr
	0x00, //ascr_Gr
	0x00, //ascr_Mg
	0xff, //ascr_Gg
	0xff, //ascr_Mb
	0x00, //ascr_Gb
	0xff, //ascr_Yr
	0x00, //ascr_Br
	0xe0, //ascr_Yg
	0x00, //ascr_Bg
	0x50, //ascr_Yb
	0xe0, //ascr_Bb
	0xff, //ascr_Wr
	0x00, //ascr_Kr
	0xfa, //ascr_Wg
	0x00, //ascr_Kg
	0xf1, //ascr_Wb
	0x00, //ascr_Kb
	0x0F, //edge_ctrl
	0x00, //edge_radius 12
	0x6F,
	0x05, //edge_aa_factor
	0x00, //edge_r
	0x00, //edge_g
	0x00, //edge_b
	0x00, //edge_x 11
	0x70,
	0x00, //edge_y 12
	0x70,
	0x44, //edge_coef_a edge_coef_b 0000 0000
};

static unsigned char DSI_HDR_VIDEO_2_MDNIE_2[] = {
	0xDE,
	0x00, //gamut_gamma_on gamut_blk_shift 0000 0000
	0x40, //gamut_scale
	0x04, //gamut_r1
	0x00,
	0x00, //gamut_r2
	0x00,
	0x00, //gamut_r3
	0x00,
	0x00, //gamut_g1
	0x00,
	0x04, //gamut_g2
	0x00,
	0x00, //gamut_g3
	0x00,
	0x00, //gamut_b1
	0x00,
	0x00, //gamut_b2
	0x00,
	0x04, //gamut_b3
	0x00,
	0x00, //slce_on cadrx_en 0000 0000
	0x11, //glare_on
	0x00, //lce_gain 000 0000
	0x24, //lce_color_gain 00 0000
	0x96, //lce_min_ref_offset
	0xb3, //lce_illum_gain
	0x01, //lce_ref_offset 9
	0x0e,
	0x01, //lce_ref_gain 9
	0x00,
	0x77, //lce_block_size h v 0000 0000
	0x03, //lce_dark_th 000
	0x07, //lce_reduct_slope 0000
	0x40, //lce_black cc0 cc1 color_th 0 0 0 0000
	0x08, //lce_large_w
	0x04, //lce_med_w
	0x04, //lce_small_w
	0x00, //cadrx_dit_en shadow_corr_en ref_ctrl_en 0 0 0
	0x00, //cadrx_gain
	0x0d, //cadrx_low_intensity_th
	0xe1, //cadrx_high_intensity_th
	0x07, //cadrx_ui_zerobincnt_th
	0x29, //cadrx_ui_ratio_th
	0x04,
	0x0f, //cadrx_entire_freq
	0xa0,
	0x00,
	0x03, //cadrx_peak_th_in_freq
	0x7a,
	0xa0,
	0x00, //cadrx_high_peak_th_in_freq
	0x25,
	0x1c,
	0x24, //cadrx_dit_color_gain
	0x00, //cadrx_dit_illum_a0
	0x48, //cadrx_dit_illum_a1
	0xea, //cadrx_dit_illum_a2
	0xfa, //cadrx_dit_illum_a3
	0x00, //cadrx_dit_illum_b0
	0x10,
	0x00, //cadrx_dit_illum_b1
	0x10,
	0x00, //cadrx_dit_illum_b2
	0x9c,
	0x01, //cadrx_dit_illum_b3
	0x00,
	0x00, //cadrx_dit_illum_slope1
	0x00,
	0x00, //cadrx_dit_illum_slope2
	0xdc,
	0x03, //cadrx_dit_illum_slope3
	0xff,
	0x40, //cadrx_ui_illum_a1
	0x80, //cadrx_ui_illum_a2
	0xc0, //cadrx_ui_illum_a3
	0x00, //cadrx_ui_illum_offset1
	0x9a,
	0x00, //cadrx_ui_illum_offset2
	0x9a,
	0x00, //cadrx_ui_illum_offset3
	0x9a,
	0x00, //cadrx_ui_illum_offset4
	0x9a,
	0x00, //cadrx_ui_illum_slope1
	0x00,
	0x00, //cadrx_ui_illum_slope2
	0x00,
	0x00, //cadrx_ui_illum_slope3
	0x00,
	0x00, //cadrx_ui_illum_slope4
	0x00,
	0x40, //cadrx_ui_ref_a1
	0x80, //cadrx_ui_ref_a2
	0xc0, //cadrx_ui_ref_a3
	0x01, //cadrx_ui_ref_offset1
	0x0e,
	0x01, //cadrx_ui_ref_offset2
	0x0e,
	0x01, //cadrx_ui_ref_offset3
	0x0e,
	0x01, //cadrx_ui_ref_offset4
	0x0e,
	0x00, //cadrx_ui_ref_slope1
	0x00,
	0x00, //cadrx_ui_ref_slope2
	0x00,
	0x00, //cadrx_ui_ref_slope3
	0x00,
	0x00, //cadrx_ui_ref_slope4
	0x00,
	0x01, //cadrx_reg_ref_c1_offset
	0x0e,
	0x01, //cadrx_reg_ref_c2_offset
	0x3a,
	0x01, //cadrx_reg_ref_c3_offset
	0x44,
	0x00, //cadrx_reg_ref_c1_slope
	0x01,
	0x00, //cadrx_reg_ref_c2_slope
	0x43,
	0x00, //cadrx_reg_ref_c3_slope
	0x4b,
	0x00, //cadrx_sc_reg_a1
	0x18,
	0x00, //cadrx_sc_reg_b1
	0x1c,
	0x01, //cadrx_sc_k1_int
	0xff,
	0x01, //cadrx_sc_k2_int
	0xff,
	0x03, //cadrx_sc_w1_int
	0xff,
	0xe8,
	0x03, //cadrx_sc_w2_int
	0xff,
	0xe8,
	0x00, //flicker_w_en
	0x14, //flicker_w
	0x01, //flicker_trans_en
	0x04, //flicker_trans_slope
	0x00, //bl_en
	0x03, //bl_hbs_th
	0x10, //bl_lbs_th
	0xC8, //bl_illum_gain
	0x01, //bl_reg_gain 9
	0x2C,
	0x00, //bl_le_diff
	0x00, //bl_cadrx_gain
	0x04, //bl_trans_slope
	0x00, //le_en
	0x40, //le_diff
	0x03, //le_lmax 10
	0xB6,
	0x23, //le_p
	0x24, //le_w_tmf
	0x00, //le_w_struct_diff 9
	0x72,
	0x01, //le_w_struct_diff_gain
	0x00, //le_w_luminance 9
	0x0D,
	0x00, //le_luminance_slope 10
	0x04,
	0x80, //glare_blk_avg_th 0000 0000
	0x00, //glare_luma_gain 9
	0x28,
	0xa0, //glare_luma_ctl_start 0000 0000
	0x01, //glare_luma_th 0000 0000
	0x00, //glare_luma_step 9
	0x01,
	0x00, //glare_uni_luma_gain 9
	0x01,
	0xf0, //glare_blk_max_th 0000 0000
	0x89, //glare_blk_max_w 0000 0000
	0x18, //glare_blk_max_curve blk_max_sh 0 0000
	0xf0, //glare_con_ext_max_th 0000 0000
	0x89, //glare_con_ext_max_w 0000 0000
	0x38, //glare_con_ext_max_sign con_ext_max_curve con_ext_max_sh 0 0 0000
	0x00, //glare_max_cnt_th 0000
	0x00, //glare_max_cnt_th 0000 0000
	0x0c, //glare_max_cnt_w 0000 0000
	0x38, //glare_max_cnt_sign max_cnt_curve max_cnt_sh 0 0 0000
	0xba, //glare_y_avg_th 0000 0000
	0x1e, //glare_y_avg_w 0000 0000
	0x38, //glare_y_avg_sign y_avg_curve y_avg_sh 0 0 0000
	0x00, //glare_max_cnt_th_dmc 0000
	0x05, //glare_max_cnt_th_dmc 0000 0000
	0x0f, //nr fa de cs gamma 0 0000
	0xff, //nr_mask_th
	0x00, //de_gain 10
	0x10,
	0x00, //de_maxplus 11
	0x40,
	0x00, //de_maxminus 11
	0x40,
	0x14, //fa_edge_th
	0x00, //fa_step_p 13
	0x01,
	0x00, //fa_step_n 13
	0x01,
	0x00, //fa_max_de_gain 13
	0x10,
	0x00, //fa_pcl_ppi 14
	0x00,
	0x28, //fa_os_cnt_10_co
	0x3c, //fa_os_cnt_20_co
	0x04, //fa_edge_cnt_weight
	0x0f, //fa_avg_y_weight
	0x01, //cs_gain 10
	0xd0,
	0x00, //curve_0
	0x0d, //curve_1
	0x19, //curve_2
	0x24, //curve_3
	0x35, //curve_4
	0x4e, //curve_5
	0x6d, //curve_6
	0x8f, //curve_7
	0xb2, //curve_8
	0xce, //curve_9
	0xe2, //curve_10
	0xf2, //curve_11
	0xff, //curve_12
	0xff, //curve_13
	0xff, //curve_14
	0xff, //curve_15
	0x01, //curve_16
	0x00,
	0x00, //curve_offset 10
	0x00,
	0x00, //curve_low_x
	0x00, //blank
	0x00, //curve_low_y
};

static unsigned char DSI_HDR_VIDEO_2_MDNIE_3[] = {
	0xDD,
	0x01, //mdnie_en
	0x00, //mask 00 0000
	0x03, //edge 00
	0xf0, //ascr algo aolce gamut 00 00 00 00
	0xff, //partial_en1
	0xff, //partial_en2
	0x00, //roi_en
	0x00, //roi_block
	0x00, //roi_sx
	0x00, //roi_sx
	0x00, //roi_sy
	0x00, //roi_sy
	0x00, //roi_ex
	0x00, //roi_ex
	0x00, //roi_ey
	0x00, //roi_ey
	0xff, //trans_on trans_block 0 000 0000
	0x04, //trans_slope
	0x01, //trans_interval
	//end
};

static unsigned char DSI_HDR_VIDEO_3_MDNIE_1[] = {
	//start
	0xDF,
	0x01, //linear_on ascr_skin_on 0000 0000
	0x67, //ascr_skin_cb
	0xa9, //ascr_skin_cr
	0x37, //ascr_dist_up
	0x29, //ascr_dist_down
	0x19, //ascr_dist_right
	0x47, //ascr_dist_left
	0x00, //ascr_div_up 20
	0x25,
	0x3d,
	0x00, //ascr_div_down
	0x31,
	0xf4,
	0x00, //ascr_div_right
	0x51,
	0xec,
	0x00, //ascr_div_left
	0x1c,
	0xd8,
	0xd0, //ascr_skin_Rr
	0x30, //ascr_skin_Rg
	0x50, //ascr_skin_Rb
	0xff, //ascr_skin_Yr
	0xd0, //ascr_skin_Yg
	0x00, //ascr_skin_Yb
	0xff, //ascr_skin_Mr
	0x00, //ascr_skin_Mg
	0xff, //ascr_skin_Mb
	0xff, //ascr_skin_Wr
	0xff, //ascr_skin_Wg
	0xff, //ascr_skin_Wb
	0x00, //ascr_Cr
	0xff, //ascr_Rr
	0xff, //ascr_Cg
	0x00, //ascr_Rg
	0xff, //ascr_Cb
	0x00, //ascr_Rb
	0xff, //ascr_Mr
	0x00, //ascr_Gr
	0x00, //ascr_Mg
	0xff, //ascr_Gg
	0xff, //ascr_Mb
	0x00, //ascr_Gb
	0xff, //ascr_Yr
	0x00, //ascr_Br
	0xff, //ascr_Yg
	0x00, //ascr_Bg
	0x00, //ascr_Yb
	0xff, //ascr_Bb
	0xff, //ascr_Wr
	0x00, //ascr_Kr
	0xf9, //ascr_Wg
	0x00, //ascr_Kg
	0xef, //ascr_Wb
	0x00, //ascr_Kb
	0x0F, //edge_ctrl
	0x00, //edge_radius 12
	0x6F,
	0x05, //edge_aa_factor
	0x00, //edge_r
	0x00, //edge_g
	0x00, //edge_b
	0x00, //edge_x 11
	0x70,
	0x00, //edge_y 12
	0x70,
	0x44, //edge_coef_a edge_coef_b 0000 0000
};

static unsigned char DSI_HDR_VIDEO_3_MDNIE_2[] = {
	0xDE,
	0x10, //gamut_gamma_on gamut_blk_shift 0000 0000
	0x40, //gamut_scale
	0x05, //gamut_r1
	0xf1,
	0xfe, //gamut_r2
	0xc3,
	0xff, //gamut_r3
	0x4c,
	0xff, //gamut_g1
	0xc3,
	0x03, //gamut_g2
	0xf1,
	0x00, //gamut_g3
	0x4c,
	0x00, //gamut_b1
	0x03,
	0xff, //gamut_b2
	0xec,
	0x04, //gamut_b3
	0x11,
	0x00, //slce_on cadrx_en 0000 0000
	0x11, //glare_on
	0x00, //lce_gain 000 0000
	0x24, //lce_color_gain 00 0000
	0x96, //lce_min_ref_offset
	0xb3, //lce_illum_gain
	0x01, //lce_ref_offset 9
	0x0e,
	0x01, //lce_ref_gain 9
	0x00,
	0x77, //lce_block_size h v 0000 0000
	0x03, //lce_dark_th 000
	0x07, //lce_reduct_slope 0000
	0x40, //lce_black cc0 cc1 color_th 0 0 0 0000
	0x08, //lce_large_w
	0x04, //lce_med_w
	0x04, //lce_small_w
	0x00, //cadrx_dit_en shadow_corr_en ref_ctrl_en 0 0 0
	0x00, //cadrx_gain
	0x0d, //cadrx_low_intensity_th
	0xe1, //cadrx_high_intensity_th
	0x07, //cadrx_ui_zerobincnt_th
	0x29, //cadrx_ui_ratio_th
	0x04,
	0x0f, //cadrx_entire_freq
	0xa0,
	0x00,
	0x03, //cadrx_peak_th_in_freq
	0x7a,
	0xa0,
	0x00, //cadrx_high_peak_th_in_freq
	0x25,
	0x1c,
	0x24, //cadrx_dit_color_gain
	0x00, //cadrx_dit_illum_a0
	0x48, //cadrx_dit_illum_a1
	0xea, //cadrx_dit_illum_a2
	0xfa, //cadrx_dit_illum_a3
	0x00, //cadrx_dit_illum_b0
	0x10,
	0x00, //cadrx_dit_illum_b1
	0x10,
	0x00, //cadrx_dit_illum_b2
	0x9c,
	0x01, //cadrx_dit_illum_b3
	0x00,
	0x00, //cadrx_dit_illum_slope1
	0x00,
	0x00, //cadrx_dit_illum_slope2
	0xdc,
	0x03, //cadrx_dit_illum_slope3
	0xff,
	0x40, //cadrx_ui_illum_a1
	0x80, //cadrx_ui_illum_a2
	0xc0, //cadrx_ui_illum_a3
	0x00, //cadrx_ui_illum_offset1
	0x9a,
	0x00, //cadrx_ui_illum_offset2
	0x9a,
	0x00, //cadrx_ui_illum_offset3
	0x9a,
	0x00, //cadrx_ui_illum_offset4
	0x9a,
	0x00, //cadrx_ui_illum_slope1
	0x00,
	0x00, //cadrx_ui_illum_slope2
	0x00,
	0x00, //cadrx_ui_illum_slope3
	0x00,
	0x00, //cadrx_ui_illum_slope4
	0x00,
	0x40, //cadrx_ui_ref_a1
	0x80, //cadrx_ui_ref_a2
	0xc0, //cadrx_ui_ref_a3
	0x01, //cadrx_ui_ref_offset1
	0x0e,
	0x01, //cadrx_ui_ref_offset2
	0x0e,
	0x01, //cadrx_ui_ref_offset3
	0x0e,
	0x01, //cadrx_ui_ref_offset4
	0x0e,
	0x00, //cadrx_ui_ref_slope1
	0x00,
	0x00, //cadrx_ui_ref_slope2
	0x00,
	0x00, //cadrx_ui_ref_slope3
	0x00,
	0x00, //cadrx_ui_ref_slope4
	0x00,
	0x01, //cadrx_reg_ref_c1_offset
	0x0e,
	0x01, //cadrx_reg_ref_c2_offset
	0x3a,
	0x01, //cadrx_reg_ref_c3_offset
	0x44,
	0x00, //cadrx_reg_ref_c1_slope
	0x01,
	0x00, //cadrx_reg_ref_c2_slope
	0x43,
	0x00, //cadrx_reg_ref_c3_slope
	0x4b,
	0x00, //cadrx_sc_reg_a1
	0x18,
	0x00, //cadrx_sc_reg_b1
	0x1c,
	0x01, //cadrx_sc_k1_int
	0xff,
	0x01, //cadrx_sc_k2_int
	0xff,
	0x03, //cadrx_sc_w1_int
	0xff,
	0xe8,
	0x03, //cadrx_sc_w2_int
	0xff,
	0xe8,
	0x00, //flicker_w_en
	0x14, //flicker_w
	0x01, //flicker_trans_en
	0x04, //flicker_trans_slope
	0x00, //bl_en
	0x03, //bl_hbs_th
	0x10, //bl_lbs_th
	0xC8, //bl_illum_gain
	0x01, //bl_reg_gain 9
	0x2C,
	0x00, //bl_le_diff
	0x00, //bl_cadrx_gain
	0x04, //bl_trans_slope
	0x00, //le_en
	0x40, //le_diff
	0x03, //le_lmax 10
	0xB6,
	0x23, //le_p
	0x24, //le_w_tmf
	0x00, //le_w_struct_diff 9
	0x72,
	0x01, //le_w_struct_diff_gain
	0x00, //le_w_luminance 9
	0x0D,
	0x00, //le_luminance_slope 10
	0x04,
	0x80, //glare_blk_avg_th 0000 0000
	0x00, //glare_luma_gain 9
	0x28,
	0xa0, //glare_luma_ctl_start 0000 0000
	0x01, //glare_luma_th 0000 0000
	0x00, //glare_luma_step 9
	0x01,
	0x00, //glare_uni_luma_gain 9
	0x01,
	0xf0, //glare_blk_max_th 0000 0000
	0x89, //glare_blk_max_w 0000 0000
	0x18, //glare_blk_max_curve blk_max_sh 0 0000
	0xf0, //glare_con_ext_max_th 0000 0000
	0x89, //glare_con_ext_max_w 0000 0000
	0x38, //glare_con_ext_max_sign con_ext_max_curve con_ext_max_sh 0 0 0000
	0x00, //glare_max_cnt_th 0000
	0x00, //glare_max_cnt_th 0000 0000
	0x0c, //glare_max_cnt_w 0000 0000
	0x38, //glare_max_cnt_sign max_cnt_curve max_cnt_sh 0 0 0000
	0xba, //glare_y_avg_th 0000 0000
	0x1e, //glare_y_avg_w 0000 0000
	0x38, //glare_y_avg_sign y_avg_curve y_avg_sh 0 0 0000
	0x00, //glare_max_cnt_th_dmc 0000
	0x05, //glare_max_cnt_th_dmc 0000 0000
	0x0f, //nr fa de cs gamma 0 0000
	0xff, //nr_mask_th
	0x00, //de_gain 10
	0x20,
	0x00, //de_maxplus 11
	0x40,
	0x00, //de_maxminus 11
	0x40,
	0x14, //fa_edge_th
	0x00, //fa_step_p 13
	0x01,
	0x00, //fa_step_n 13
	0x01,
	0x00, //fa_max_de_gain 13
	0x10,
	0x00, //fa_pcl_ppi 14
	0x00,
	0x28, //fa_os_cnt_10_co
	0x3c, //fa_os_cnt_20_co
	0x04, //fa_edge_cnt_weight
	0x0f, //fa_avg_y_weight
	0x01, //cs_gain 10
	0x60,
	0x00, //curve_0
	0x09, //curve_1
	0x0d, //curve_2
	0x16, //curve_3
	0x25, //curve_4
	0x34, //curve_5
	0x49, //curve_6
	0x65, //curve_7
	0x84, //curve_8
	0xaa, //curve_9
	0xd9, //curve_10
	0xf6, //curve_11
	0xf9, //curve_12
	0xfc, //curve_13
	0xff, //curve_14
	0xff, //curve_15
	0x01, //curve_16
	0x00,
	0x00, //curve_offset 10
	0x00,
	0x05, //curve_low_x
	0x00, //blank
	0x00, //curve_low_y
};

static unsigned char DSI_HDR_VIDEO_3_MDNIE_3[] = {
	0xDD,
	0x01, //mdnie_en
	0x00, //mask 00 0000
	0x03, //edge 00
	0xf3, //ascr algo aolce gamut 00 00 00 00
	0xff, //partial_en1
	0xff, //partial_en2
	0x00, //roi_en
	0x00, //roi_block
	0x00, //roi_sx
	0x00, //roi_sx
	0x00, //roi_sy
	0x00, //roi_sy
	0x00, //roi_ex
	0x00, //roi_ex
	0x00, //roi_ey
	0x00, //roi_ey
	0xff, //trans_on trans_block 0 000 0000
	0x04, //trans_slope
	0x01, //trans_interval
	//end
};

static unsigned char DSI_VIDEO_ENHANCER_D65_MDNIE_1[] = {
	//start
	0xDF,
	0x01, //linear_on ascr_skin_on 0000 0000
	0x67, //ascr_skin_cb
	0xa9, //ascr_skin_cr
	0x37, //ascr_dist_up
	0x29, //ascr_dist_down
	0x19, //ascr_dist_right
	0x47, //ascr_dist_left
	0x00, //ascr_div_up 20
	0x25,
	0x3d,
	0x00, //ascr_div_down
	0x31,
	0xf4,
	0x00, //ascr_div_right
	0x51,
	0xec,
	0x00, //ascr_div_left
	0x1c,
	0xd8,
	0xff, //ascr_skin_Rr
	0x64, //ascr_skin_Rg
	0x70, //ascr_skin_Rb
	0xff, //ascr_skin_Yr
	0xff, //ascr_skin_Yg
	0x00, //ascr_skin_Yb
	0xff, //ascr_skin_Mr
	0x00, //ascr_skin_Mg
	0xff, //ascr_skin_Mb
	0xff, //ascr_skin_Wr
	0xfa, //ascr_skin_Wg
	0xf1, //ascr_skin_Wb
	0x00, //ascr_Cr
	0xff, //ascr_Rr
	0xff, //ascr_Cg
	0x00, //ascr_Rg
	0xff, //ascr_Cb
	0x00, //ascr_Rb
	0xff, //ascr_Mr
	0x00, //ascr_Gr
	0x00, //ascr_Mg
	0xff, //ascr_Gg
	0xff, //ascr_Mb
	0x00, //ascr_Gb
	0xff, //ascr_Yr
	0x00, //ascr_Br
	0xff, //ascr_Yg
	0x00, //ascr_Bg
	0x00, //ascr_Yb
	0xff, //ascr_Bb
	0xff, //ascr_Wr
	0x00, //ascr_Kr
	0xfa, //ascr_Wg
	0x00, //ascr_Kg
	0xf1, //ascr_Wb
	0x00, //ascr_Kb
	0x0F, //edge_ctrl
	0x00, //edge_radius 12
	0x6F,
	0x05, //edge_aa_factor
	0x00, //edge_r
	0x00, //edge_g
	0x00, //edge_b
	0x00, //edge_x 11
	0x70,
	0x00, //edge_y 12
	0x70,
	0x44, //edge_coef_a edge_coef_b 0000 0000
};

static unsigned char DSI_VIDEO_ENHANCER_D65_MDNIE_2[] = {
	0xDE,
	0x00, //gamut_gamma_on gamut_blk_shift 0000 0000
	0x40, //gamut_scale
	0x04, //gamut_r1
	0x00,
	0x00, //gamut_r2
	0x00,
	0x00, //gamut_r3
	0x00,
	0x00, //gamut_g1
	0x00,
	0x04, //gamut_g2
	0x00,
	0x00, //gamut_g3
	0x00,
	0x00, //gamut_b1
	0x00,
	0x00, //gamut_b2
	0x00,
	0x04, //gamut_b3
	0x00,
	0x11, //slce_on cadrx_en 0000 0000
	0x00, //glare_on
	0x08, //lce_gain 000 0000
	0x18, //lce_color_gain 00 0000
	0xff, //lce_min_ref_offset
	0xa0, //lce_illum_gain
	0x01, //lce_ref_offset 9
	0x40,
	0x01, //lce_ref_gain 9
	0x00,
	0x66, //lce_block_size h v 0000 0000
	0x05, //lce_dark_th 000
	0x01, //lce_reduct_slope 0000
	0x46, //lce_black cc0 cc1 color_th 0 0 0 0000
	0x04, //lce_large_w
	0x06, //lce_med_w
	0x06, //lce_small_w
	0x05, //cadrx_dit_en shadow_corr_en ref_ctrl_en 0 0 0
	0x1b, //cadrx_gain
	0x0d, //cadrx_low_intensity_th
	0xe1, //cadrx_high_intensity_th
	0x00, //cadrx_ui_zerobincnt_th
	0x24, //cadrx_ui_ratio_th
	0xea,
	0x0e, //cadrx_entire_freq
	0x10,
	0x00,
	0x03, //cadrx_peak_th_in_freq
	0x21,
	0x90,
	0x00, //cadrx_high_peak_th_in_freq
	0x21,
	0x66,
	0x24, //cadrx_dit_color_gain
	0x00, //cadrx_dit_illum_a0
	0x48, //cadrx_dit_illum_a1
	0xea, //cadrx_dit_illum_a2
	0xfa, //cadrx_dit_illum_a3
	0x00, //cadrx_dit_illum_b0
	0x10,
	0x00, //cadrx_dit_illum_b1
	0x10,
	0x00, //cadrx_dit_illum_b2
	0x9c,
	0x01, //cadrx_dit_illum_b3
	0x00,
	0x00, //cadrx_dit_illum_slope1
	0x00,
	0x00, //cadrx_dit_illum_slope2
	0xdd,
	0x03, //cadrx_dit_illum_slope3
	0xff,
	0x20, //cadrx_ui_illum_a1
	0x40, //cadrx_ui_illum_a2
	0x60, //cadrx_ui_illum_a3
	0x00, //cadrx_ui_illum_offset1
	0xd0,
	0x00, //cadrx_ui_illum_offset2
	0xc0,
	0x00, //cadrx_ui_illum_offset3
	0xb0,
	0x00, //cadrx_ui_illum_offset4
	0xa0,
	0x07, //cadrx_ui_illum_slope1
	0x80,
	0x07, //cadrx_ui_illum_slope2
	0x80,
	0x07, //cadrx_ui_illum_slope3
	0x80,
	0x00, //cadrx_ui_illum_slope4
	0x00,
	0x20, //cadrx_ui_ref_a1
	0x40, //cadrx_ui_ref_a2
	0x60, //cadrx_ui_ref_a3
	0x01, //cadrx_ui_ref_offset1
	0x60,
	0x01, //cadrx_ui_ref_offset2
	0x50,
	0x01, //cadrx_ui_ref_offset3
	0x40,
	0x01, //cadrx_ui_ref_offset4
	0x30,
	0x07, //cadrx_ui_ref_slope1
	0x80,
	0x07, //cadrx_ui_ref_slope2
	0x80,
	0x07, //cadrx_ui_ref_slope3
	0x80,
	0x00, //cadrx_ui_ref_slope4
	0x00,
	0x00, //cadrx_reg_ref_c1_offset
	0x80,
	0x00, //cadrx_reg_ref_c2_offset
	0xac,
	0x00, //cadrx_reg_ref_c3_offset
	0xb6,
	0x00, //cadrx_reg_ref_c1_slope
	0x01,
	0x00, //cadrx_reg_ref_c2_slope
	0x40,
	0x00, //cadrx_reg_ref_c3_slope
	0x48,
	0x00, //cadrx_sc_reg_a1
	0x18,
	0x00, //cadrx_sc_reg_b1
	0x1c,
	0x01, //cadrx_sc_k1_int
	0xff,
	0x01, //cadrx_sc_k2_int
	0xff,
	0x03, //cadrx_sc_w1_int
	0xff,
	0xe8,
	0x03, //cadrx_sc_w2_int
	0xff,
	0xe8,
	0x00, //flicker_w_en
	0x14, //flicker_w
	0x00, //flicker_trans_en
	0x04, //flicker_trans_slope
	0x00, //bl_en
	0x03, //bl_hbs_th
	0x10, //bl_lbs_th
	0xC8, //bl_illum_gain
	0x01, //bl_reg_gain 9
	0x2C,
	0x00, //bl_le_diff
	0x00, //bl_cadrx_gain
	0x04, //bl_trans_slope
	0x01, //le_en
	0x40, //le_diff
	0x03, //le_lmax 10
	0x84,
	0x3c, //le_p
	0x28, //le_w_tmf
	0x00, //le_w_struct_diff 9
	0x40,
	0xff, //le_w_struct_diff_gain
	0x00, //le_w_luminance 9
	0x80,
	0x01, //le_luminance_slope 10
	0x00,
	0x80, //glare_blk_avg_th 0000 0000
	0x00, //glare_luma_gain 9
	0x28,
	0xa0, //glare_luma_ctl_start 0000 0000
	0x01, //glare_luma_th 0000 0000
	0x00, //glare_luma_step 9
	0x01,
	0x00, //glare_uni_luma_gain 9
	0x01,
	0x00, //glare_blk_max_th 0000 0000
	0x00, //glare_blk_max_w 0000 0000
	0x10, //glare_blk_max_curve blk_max_sh 0 0000
	0x00, //glare_con_ext_max_th 0000 0000
	0x00, //glare_con_ext_max_w 0000 0000
	0x30, //glare_con_ext_max_sign con_ext_max_curve con_ext_max_sh 0 0 0000
	0x00, //glare_max_cnt_th 0000
	0x00, //glare_max_cnt_th 0000 0000
	0x00, //glare_max_cnt_w 0000 0000
	0x30, //glare_max_cnt_sign max_cnt_curve max_cnt_sh 0 0 0000
	0x00, //glare_y_avg_th 0000 0000
	0x00, //glare_y_avg_w 0000 0000
	0x30, //glare_y_avg_sign y_avg_curve y_avg_sh 0 0 0000
	0x00, //glare_max_cnt_th_dmc 0000
	0x00, //glare_max_cnt_th_dmc 0000 0000
	0x07, //nr fa de cs gamma 0 0000
	0xff, //nr_mask_th
	0x00, //de_gain 10
	0x10,
	0x00, //de_maxplus 11
	0x40,
	0x00, //de_maxminus 11
	0x40,
	0x14, //fa_edge_th
	0x00, //fa_step_p 13
	0x01,
	0x00, //fa_step_n 13
	0x01,
	0x00, //fa_max_de_gain 13
	0x10,
	0x00, //fa_pcl_ppi 14
	0x00,
	0x28, //fa_os_cnt_10_co
	0x3c, //fa_os_cnt_20_co
	0x04, //fa_edge_cnt_weight
	0x0f, //fa_avg_y_weight
	0x01, //cs_gain 10
	0x00,
	0x00, //curve_0
	0x0a, //curve_1
	0x17, //curve_2
	0x26, //curve_3
	0x36, //curve_4
	0x49, //curve_5
	0x5c, //curve_6
	0x6f, //curve_7
	0x82, //curve_8
	0x95, //curve_9
	0xaa, //curve_10
	0xbe, //curve_11
	0xcf, //curve_12
	0xdf, //curve_13
	0xee, //curve_14
	0xf9, //curve_15
	0x01, //curve_16
	0x00,
	0x00, //curve_offset 10
	0x00,
	0x00, //curve_low_x
	0x00, //blank
	0x00, //curve_low_y
};

static unsigned char DSI_VIDEO_ENHANCER_D65_MDNIE_3[] = {
	0xDD,
	0x01, //mdnie_en
	0x00, //mask 00 0000
	0x03, //edge 00
	0xfc, //ascr algo aolce gamut 00 00 00 00
	0xff, //partial_en1
	0xff, //partial_en2
	0x00, //roi_en
	0x00, //roi_block
	0x00, //roi_sx
	0x00, //roi_sx
	0x00, //roi_sy
	0x00, //roi_sy
	0x00, //roi_ex
	0x00, //roi_ex
	0x00, //roi_ey
	0x00, //roi_ey
	0xff, //trans_on trans_block 0 000 0000
	0x04, //trans_slope
	0x01, //trans_interval
	//end
};

static unsigned char DSI_VIDEO_ENHANCER_AUTO_MDNIE_1[] = {
	//start
	0xDF,
	0x01, //linear_on ascr_skin_on 0000 0000
	0x67, //ascr_skin_cb
	0xa9, //ascr_skin_cr
	0x37, //ascr_dist_up
	0x29, //ascr_dist_down
	0x19, //ascr_dist_right
	0x47, //ascr_dist_left
	0x00, //ascr_div_up 20
	0x25,
	0x3d,
	0x00, //ascr_div_down
	0x31,
	0xf4,
	0x00, //ascr_div_right
	0x51,
	0xec,
	0x00, //ascr_div_left
	0x1c,
	0xd8,
	0xff, //ascr_skin_Rr
	0x64, //ascr_skin_Rg
	0x70, //ascr_skin_Rb
	0xff, //ascr_skin_Yr
	0xff, //ascr_skin_Yg
	0x00, //ascr_skin_Yb
	0xff, //ascr_skin_Mr
	0x00, //ascr_skin_Mg
	0xff, //ascr_skin_Mb
	0xff, //ascr_skin_Wr
	0xfd, //ascr_skin_Wg
	0xff, //ascr_skin_Wb
	0x00, //ascr_Cr
	0xff, //ascr_Rr
	0xff, //ascr_Cg
	0x00, //ascr_Rg
	0xff, //ascr_Cb
	0x00, //ascr_Rb
	0xff, //ascr_Mr
	0x00, //ascr_Gr
	0x00, //ascr_Mg
	0xff, //ascr_Gg
	0xff, //ascr_Mb
	0x00, //ascr_Gb
	0xff, //ascr_Yr
	0x00, //ascr_Br
	0xff, //ascr_Yg
	0x00, //ascr_Bg
	0x00, //ascr_Yb
	0xff, //ascr_Bb
	0xff, //ascr_Wr
	0x00, //ascr_Kr
	0xff, //ascr_Wg
	0x00, //ascr_Kg
	0xff, //ascr_Wb
	0x00, //ascr_Kb
	0x0F, //edge_ctrl
	0x00, //edge_radius 12
	0x6F,
	0x05, //edge_aa_factor
	0x00, //edge_r
	0x00, //edge_g
	0x00, //edge_b
	0x00, //edge_x 11
	0x70,
	0x00, //edge_y 12
	0x70,
	0x44, //edge_coef_a edge_coef_b 0000 0000
};

static unsigned char DSI_VIDEO_ENHANCER_AUTO_MDNIE_2[] = {
	0xDE,
	0x00, //gamut_gamma_on gamut_blk_shift 0000 0000
	0x40, //gamut_scale
	0x04, //gamut_r1
	0x00,
	0x00, //gamut_r2
	0x00,
	0x00, //gamut_r3
	0x00,
	0x00, //gamut_g1
	0x00,
	0x04, //gamut_g2
	0x00,
	0x00, //gamut_g3
	0x00,
	0x00, //gamut_b1
	0x00,
	0x00, //gamut_b2
	0x00,
	0x04, //gamut_b3
	0x00,
	0x11, //slce_on cadrx_en 0000 0000
	0x00, //glare_on
	0x08, //lce_gain 000 0000
	0x18, //lce_color_gain 00 0000
	0xff, //lce_min_ref_offset
	0xa0, //lce_illum_gain
	0x01, //lce_ref_offset 9
	0x40,
	0x01, //lce_ref_gain 9
	0x00,
	0x66, //lce_block_size h v 0000 0000
	0x05, //lce_dark_th 000
	0x01, //lce_reduct_slope 0000
	0x46, //lce_black cc0 cc1 color_th 0 0 0 0000
	0x04, //lce_large_w
	0x06, //lce_med_w
	0x06, //lce_small_w
	0x05, //cadrx_dit_en shadow_corr_en ref_ctrl_en 0 0 0
	0x1b, //cadrx_gain
	0x0d, //cadrx_low_intensity_th
	0xe1, //cadrx_high_intensity_th
	0x00, //cadrx_ui_zerobincnt_th
	0x24, //cadrx_ui_ratio_th
	0xea,
	0x0e, //cadrx_entire_freq
	0x10,
	0x00,
	0x03, //cadrx_peak_th_in_freq
	0x21,
	0x90,
	0x00, //cadrx_high_peak_th_in_freq
	0x21,
	0x66,
	0x24, //cadrx_dit_color_gain
	0x00, //cadrx_dit_illum_a0
	0x48, //cadrx_dit_illum_a1
	0xea, //cadrx_dit_illum_a2
	0xfa, //cadrx_dit_illum_a3
	0x00, //cadrx_dit_illum_b0
	0x10,
	0x00, //cadrx_dit_illum_b1
	0x10,
	0x00, //cadrx_dit_illum_b2
	0x9c,
	0x01, //cadrx_dit_illum_b3
	0x00,
	0x00, //cadrx_dit_illum_slope1
	0x00,
	0x00, //cadrx_dit_illum_slope2
	0xdd,
	0x03, //cadrx_dit_illum_slope3
	0xff,
	0x20, //cadrx_ui_illum_a1
	0x40, //cadrx_ui_illum_a2
	0x60, //cadrx_ui_illum_a3
	0x00, //cadrx_ui_illum_offset1
	0xd0,
	0x00, //cadrx_ui_illum_offset2
	0xc0,
	0x00, //cadrx_ui_illum_offset3
	0xb0,
	0x00, //cadrx_ui_illum_offset4
	0xa0,
	0x07, //cadrx_ui_illum_slope1
	0x80,
	0x07, //cadrx_ui_illum_slope2
	0x80,
	0x07, //cadrx_ui_illum_slope3
	0x80,
	0x00, //cadrx_ui_illum_slope4
	0x00,
	0x20, //cadrx_ui_ref_a1
	0x40, //cadrx_ui_ref_a2
	0x60, //cadrx_ui_ref_a3
	0x01, //cadrx_ui_ref_offset1
	0x60,
	0x01, //cadrx_ui_ref_offset2
	0x50,
	0x01, //cadrx_ui_ref_offset3
	0x40,
	0x01, //cadrx_ui_ref_offset4
	0x30,
	0x07, //cadrx_ui_ref_slope1
	0x80,
	0x07, //cadrx_ui_ref_slope2
	0x80,
	0x07, //cadrx_ui_ref_slope3
	0x80,
	0x00, //cadrx_ui_ref_slope4
	0x00,
	0x00, //cadrx_reg_ref_c1_offset
	0x80,
	0x00, //cadrx_reg_ref_c2_offset
	0xac,
	0x00, //cadrx_reg_ref_c3_offset
	0xb6,
	0x00, //cadrx_reg_ref_c1_slope
	0x01,
	0x00, //cadrx_reg_ref_c2_slope
	0x40,
	0x00, //cadrx_reg_ref_c3_slope
	0x48,
	0x00, //cadrx_sc_reg_a1
	0x18,
	0x00, //cadrx_sc_reg_b1
	0x1c,
	0x01, //cadrx_sc_k1_int
	0xff,
	0x01, //cadrx_sc_k2_int
	0xff,
	0x03, //cadrx_sc_w1_int
	0xff,
	0xe8,
	0x03, //cadrx_sc_w2_int
	0xff,
	0xe8,
	0x00, //flicker_w_en
	0x14, //flicker_w
	0x00, //flicker_trans_en
	0x04, //flicker_trans_slope
	0x00, //bl_en
	0x03, //bl_hbs_th
	0x10, //bl_lbs_th
	0xC8, //bl_illum_gain
	0x01, //bl_reg_gain 9
	0x2C,
	0x00, //bl_le_diff
	0x00, //bl_cadrx_gain
	0x04, //bl_trans_slope
	0x01, //le_en
	0x40, //le_diff
	0x03, //le_lmax 10
	0x84,
	0x3c, //le_p
	0x28, //le_w_tmf
	0x00, //le_w_struct_diff 9
	0x40,
	0xff, //le_w_struct_diff_gain
	0x00, //le_w_luminance 9
	0x80,
	0x01, //le_luminance_slope 10
	0x00,
	0x80, //glare_blk_avg_th 0000 0000
	0x00, //glare_luma_gain 9
	0x28,
	0xa0, //glare_luma_ctl_start 0000 0000
	0x01, //glare_luma_th 0000 0000
	0x00, //glare_luma_step 9
	0x01,
	0x00, //glare_uni_luma_gain 9
	0x01,
	0x00, //glare_blk_max_th 0000 0000
	0x00, //glare_blk_max_w 0000 0000
	0x10, //glare_blk_max_curve blk_max_sh 0 0000
	0x00, //glare_con_ext_max_th 0000 0000
	0x00, //glare_con_ext_max_w 0000 0000
	0x30, //glare_con_ext_max_sign con_ext_max_curve con_ext_max_sh 0 0 0000
	0x00, //glare_max_cnt_th 0000
	0x00, //glare_max_cnt_th 0000 0000
	0x00, //glare_max_cnt_w 0000 0000
	0x30, //glare_max_cnt_sign max_cnt_curve max_cnt_sh 0 0 0000
	0x00, //glare_y_avg_th 0000 0000
	0x00, //glare_y_avg_w 0000 0000
	0x30, //glare_y_avg_sign y_avg_curve y_avg_sh 0 0 0000
	0x00, //glare_max_cnt_th_dmc 0000
	0x00, //glare_max_cnt_th_dmc 0000 0000
	0x07, //nr fa de cs gamma 0 0000
	0xff, //nr_mask_th
	0x00, //de_gain 10
	0x10,
	0x00, //de_maxplus 11
	0x40,
	0x00, //de_maxminus 11
	0x40,
	0x14, //fa_edge_th
	0x00, //fa_step_p 13
	0x01,
	0x00, //fa_step_n 13
	0x01,
	0x00, //fa_max_de_gain 13
	0x10,
	0x00, //fa_pcl_ppi 14
	0x00,
	0x28, //fa_os_cnt_10_co
	0x3c, //fa_os_cnt_20_co
	0x04, //fa_edge_cnt_weight
	0x0f, //fa_avg_y_weight
	0x01, //cs_gain 10
	0x00,
	0x00, //curve_0
	0x0a, //curve_1
	0x17, //curve_2
	0x26, //curve_3
	0x36, //curve_4
	0x49, //curve_5
	0x5c, //curve_6
	0x6f, //curve_7
	0x82, //curve_8
	0x95, //curve_9
	0xaa, //curve_10
	0xbe, //curve_11
	0xcf, //curve_12
	0xdf, //curve_13
	0xee, //curve_14
	0xf9, //curve_15
	0x01, //curve_16
	0x00,
	0x00, //curve_offset 10
	0x00,
	0x00, //curve_low_x
	0x00, //blank
	0x00, //curve_low_y
};

static unsigned char DSI_VIDEO_ENHANCER_AUTO_MDNIE_3[] = {
	0xDD,
	0x01, //mdnie_en
	0x00, //mask 00 0000
	0x03, //edge 00
	0xfc, //ascr algo aolce gamut 00 00 00 00
	0xff, //partial_en1
	0xff, //partial_en2
	0x00, //roi_en
	0x00, //roi_block
	0x00, //roi_sx
	0x00, //roi_sx
	0x00, //roi_sy
	0x00, //roi_sy
	0x00, //roi_ex
	0x00, //roi_ex
	0x00, //roi_ey
	0x00, //roi_ey
	0xff, //trans_on trans_block 0 000 0000
	0x04, //trans_slope
	0x01, //trans_interval
	//end
};

static unsigned char DSI_VIDEO_ENHANCER_THIRD_D65_MDNIE_1[] = {
	//start
	0xDF,
	0x01, //linear_on ascr_skin_on 0000 0000
	0x67, //ascr_skin_cb
	0xa9, //ascr_skin_cr
	0x37, //ascr_dist_up
	0x29, //ascr_dist_down
	0x19, //ascr_dist_right
	0x47, //ascr_dist_left
	0x00, //ascr_div_up 20
	0x25,
	0x3d,
	0x00, //ascr_div_down
	0x31,
	0xf4,
	0x00, //ascr_div_right
	0x51,
	0xec,
	0x00, //ascr_div_left
	0x1c,
	0xd8,
	0xff, //ascr_skin_Rr
	0x64, //ascr_skin_Rg
	0x70, //ascr_skin_Rb
	0xff, //ascr_skin_Yr
	0xff, //ascr_skin_Yg
	0x00, //ascr_skin_Yb
	0xff, //ascr_skin_Mr
	0x00, //ascr_skin_Mg
	0xff, //ascr_skin_Mb
	0xff, //ascr_skin_Wr
	0xfa, //ascr_skin_Wg
	0xf1, //ascr_skin_Wb
	0x00, //ascr_Cr
	0xff, //ascr_Rr
	0xff, //ascr_Cg
	0x00, //ascr_Rg
	0xff, //ascr_Cb
	0x00, //ascr_Rb
	0xff, //ascr_Mr
	0x00, //ascr_Gr
	0x00, //ascr_Mg
	0xff, //ascr_Gg
	0xff, //ascr_Mb
	0x00, //ascr_Gb
	0xff, //ascr_Yr
	0x00, //ascr_Br
	0xff, //ascr_Yg
	0x00, //ascr_Bg
	0x00, //ascr_Yb
	0xff, //ascr_Bb
	0xff, //ascr_Wr
	0x00, //ascr_Kr
	0xfa, //ascr_Wg
	0x00, //ascr_Kg
	0xf1, //ascr_Wb
	0x00, //ascr_Kb
	0x0F, //edge_ctrl
	0x00, //edge_radius 12
	0x6F,
	0x05, //edge_aa_factor
	0x00, //edge_r
	0x00, //edge_g
	0x00, //edge_b
	0x00, //edge_x 11
	0x70,
	0x00, //edge_y 12
	0x70,
	0x44, //edge_coef_a edge_coef_b 0000 0000
};

static unsigned char DSI_VIDEO_ENHANCER_THIRD_D65_MDNIE_2[] = {
	0xDE,
	0x00, //gamut_gamma_on gamut_blk_shift 0000 0000
	0x40, //gamut_scale
	0x04, //gamut_r1
	0x00,
	0x00, //gamut_r2
	0x00,
	0x00, //gamut_r3
	0x00,
	0x00, //gamut_g1
	0x00,
	0x04, //gamut_g2
	0x00,
	0x00, //gamut_g3
	0x00,
	0x00, //gamut_b1
	0x00,
	0x00, //gamut_b2
	0x00,
	0x04, //gamut_b3
	0x00,
	0x11, //slce_on cadrx_en 0000 0000
	0x00, //glare_on
	0x01, //lce_gain 000 0000
	0x18, //lce_color_gain 00 0000
	0xff, //lce_min_ref_offset
	0xa0, //lce_illum_gain
	0x01, //lce_ref_offset 9
	0x40,
	0x01, //lce_ref_gain 9
	0x00,
	0x66, //lce_block_size h v 0000 0000
	0x05, //lce_dark_th 000
	0x01, //lce_reduct_slope 0000
	0x46, //lce_black cc0 cc1 color_th 0 0 0 0000
	0x04, //lce_large_w
	0x06, //lce_med_w
	0x06, //lce_small_w
	0x05, //cadrx_dit_en shadow_corr_en ref_ctrl_en 0 0 0
	0x1b, //cadrx_gain
	0x0d, //cadrx_low_intensity_th
	0xe1, //cadrx_high_intensity_th
	0x00, //cadrx_ui_zerobincnt_th
	0x24, //cadrx_ui_ratio_th
	0xea,
	0x0e, //cadrx_entire_freq
	0x10,
	0x00,
	0x03, //cadrx_peak_th_in_freq
	0x21,
	0x90,
	0x00, //cadrx_high_peak_th_in_freq
	0x21,
	0x66,
	0x24, //cadrx_dit_color_gain
	0x00, //cadrx_dit_illum_a0
	0x48, //cadrx_dit_illum_a1
	0xea, //cadrx_dit_illum_a2
	0xfa, //cadrx_dit_illum_a3
	0x00, //cadrx_dit_illum_b0
	0x10,
	0x00, //cadrx_dit_illum_b1
	0x10,
	0x00, //cadrx_dit_illum_b2
	0x9c,
	0x01, //cadrx_dit_illum_b3
	0x00,
	0x00, //cadrx_dit_illum_slope1
	0x00,
	0x00, //cadrx_dit_illum_slope2
	0xdd,
	0x03, //cadrx_dit_illum_slope3
	0xff,
	0x20, //cadrx_ui_illum_a1
	0x40, //cadrx_ui_illum_a2
	0x60, //cadrx_ui_illum_a3
	0x00, //cadrx_ui_illum_offset1
	0xd0,
	0x00, //cadrx_ui_illum_offset2
	0xc0,
	0x00, //cadrx_ui_illum_offset3
	0xb0,
	0x00, //cadrx_ui_illum_offset4
	0xa0,
	0x07, //cadrx_ui_illum_slope1
	0x80,
	0x07, //cadrx_ui_illum_slope2
	0x80,
	0x07, //cadrx_ui_illum_slope3
	0x80,
	0x00, //cadrx_ui_illum_slope4
	0x00,
	0x20, //cadrx_ui_ref_a1
	0x40, //cadrx_ui_ref_a2
	0x60, //cadrx_ui_ref_a3
	0x01, //cadrx_ui_ref_offset1
	0x60,
	0x01, //cadrx_ui_ref_offset2
	0x50,
	0x01, //cadrx_ui_ref_offset3
	0x40,
	0x01, //cadrx_ui_ref_offset4
	0x30,
	0x07, //cadrx_ui_ref_slope1
	0x80,
	0x07, //cadrx_ui_ref_slope2
	0x80,
	0x07, //cadrx_ui_ref_slope3
	0x80,
	0x00, //cadrx_ui_ref_slope4
	0x00,
	0x00, //cadrx_reg_ref_c1_offset
	0x80,
	0x00, //cadrx_reg_ref_c2_offset
	0xac,
	0x00, //cadrx_reg_ref_c3_offset
	0xb6,
	0x00, //cadrx_reg_ref_c1_slope
	0x01,
	0x00, //cadrx_reg_ref_c2_slope
	0x40,
	0x00, //cadrx_reg_ref_c3_slope
	0x48,
	0x00, //cadrx_sc_reg_a1
	0x18,
	0x00, //cadrx_sc_reg_b1
	0x1c,
	0x01, //cadrx_sc_k1_int
	0xff,
	0x01, //cadrx_sc_k2_int
	0xff,
	0x03, //cadrx_sc_w1_int
	0xff,
	0xe8,
	0x03, //cadrx_sc_w2_int
	0xff,
	0xe8,
	0x00, //flicker_w_en
	0x14, //flicker_w
	0x00, //flicker_trans_en
	0x04, //flicker_trans_slope
	0x00, //bl_en
	0x03, //bl_hbs_th
	0x10, //bl_lbs_th
	0xC8, //bl_illum_gain
	0x01, //bl_reg_gain 9
	0x2C,
	0x00, //bl_le_diff
	0x00, //bl_cadrx_gain
	0x04, //bl_trans_slope
	0x01, //le_en
	0x40, //le_diff
	0x03, //le_lmax 10
	0x84,
	0x3c, //le_p
	0x28, //le_w_tmf
	0x00, //le_w_struct_diff 9
	0x40,
	0xff, //le_w_struct_diff_gain
	0x00, //le_w_luminance 9
	0x80,
	0x01, //le_luminance_slope 10
	0x00,
	0x80, //glare_blk_avg_th 0000 0000
	0x00, //glare_luma_gain 9
	0x28,
	0xa0, //glare_luma_ctl_start 0000 0000
	0x01, //glare_luma_th 0000 0000
	0x00, //glare_luma_step 9
	0x01,
	0x00, //glare_uni_luma_gain 9
	0x01,
	0x00, //glare_blk_max_th 0000 0000
	0x00, //glare_blk_max_w 0000 0000
	0x10, //glare_blk_max_curve blk_max_sh 0 0000
	0x00, //glare_con_ext_max_th 0000 0000
	0x00, //glare_con_ext_max_w 0000 0000
	0x30, //glare_con_ext_max_sign con_ext_max_curve con_ext_max_sh 0 0 0000
	0x00, //glare_max_cnt_th 0000
	0x00, //glare_max_cnt_th 0000 0000
	0x00, //glare_max_cnt_w 0000 0000
	0x30, //glare_max_cnt_sign max_cnt_curve max_cnt_sh 0 0 0000
	0x00, //glare_y_avg_th 0000 0000
	0x00, //glare_y_avg_w 0000 0000
	0x30, //glare_y_avg_sign y_avg_curve y_avg_sh 0 0 0000
	0x00, //glare_max_cnt_th_dmc 0000
	0x00, //glare_max_cnt_th_dmc 0000 0000
	0x07, //nr fa de cs gamma 0 0000
	0xff, //nr_mask_th
	0x00, //de_gain 10
	0x10,
	0x00, //de_maxplus 11
	0x40,
	0x00, //de_maxminus 11
	0x40,
	0x14, //fa_edge_th
	0x00, //fa_step_p 13
	0x01,
	0x00, //fa_step_n 13
	0x01,
	0x00, //fa_max_de_gain 13
	0x10,
	0x00, //fa_pcl_ppi 14
	0x00,
	0x28, //fa_os_cnt_10_co
	0x3c, //fa_os_cnt_20_co
	0x04, //fa_edge_cnt_weight
	0x0f, //fa_avg_y_weight
	0x01, //cs_gain 10
	0x20,
	0x00, //curve_0
	0x10, //curve_1
	0x20, //curve_2
	0x30, //curve_3
	0x40, //curve_4
	0x50, //curve_5
	0x60, //curve_6
	0x70, //curve_7
	0x82, //curve_8
	0x95, //curve_9
	0xaa, //curve_10
	0xbe, //curve_11
	0xcf, //curve_12
	0xdf, //curve_13
	0xee, //curve_14
	0xf9, //curve_15
	0x01, //curve_16
	0x00,
	0x00, //curve_offset 10
	0x00,
	0x00, //curve_low_x
	0x00, //blank
	0x00, //curve_low_y
};

static unsigned char DSI_VIDEO_ENHANCER_THIRD_D65_MDNIE_3[] = {
	0xDD,
	0x01, //mdnie_en
	0x00, //mask 00 0000
	0x03, //edge 00
	0xfc, //ascr algo aolce gamut 00 00 00 00
	0xff, //partial_en1
	0xff, //partial_en2
	0x00, //roi_en
	0x00, //roi_block
	0x00, //roi_sx
	0x00, //roi_sx
	0x00, //roi_sy
	0x00, //roi_sy
	0x00, //roi_ex
	0x00, //roi_ex
	0x00, //roi_ey
	0x00, //roi_ey
	0xff, //trans_on trans_block 0 000 0000
	0x04, //trans_slope
	0x01, //trans_interval
	//end
};

static unsigned char DSI_VIDEO_ENHANCER_THIRD_AUTO_MDNIE_1[] = {
	//start
	0xDF,
	0x01, //linear_on ascr_skin_on 0000 0000
	0x67, //ascr_skin_cb
	0xa9, //ascr_skin_cr
	0x37, //ascr_dist_up
	0x29, //ascr_dist_down
	0x19, //ascr_dist_right
	0x47, //ascr_dist_left
	0x00, //ascr_div_up 20
	0x25,
	0x3d,
	0x00, //ascr_div_down
	0x31,
	0xf4,
	0x00, //ascr_div_right
	0x51,
	0xec,
	0x00, //ascr_div_left
	0x1c,
	0xd8,
	0xff, //ascr_skin_Rr
	0x64, //ascr_skin_Rg
	0x70, //ascr_skin_Rb
	0xff, //ascr_skin_Yr
	0xff, //ascr_skin_Yg
	0x00, //ascr_skin_Yb
	0xff, //ascr_skin_Mr
	0x00, //ascr_skin_Mg
	0xff, //ascr_skin_Mb
	0xff, //ascr_skin_Wr
	0xfd, //ascr_skin_Wg
	0xff, //ascr_skin_Wb
	0x00, //ascr_Cr
	0xff, //ascr_Rr
	0xff, //ascr_Cg
	0x00, //ascr_Rg
	0xff, //ascr_Cb
	0x00, //ascr_Rb
	0xff, //ascr_Mr
	0x00, //ascr_Gr
	0x00, //ascr_Mg
	0xff, //ascr_Gg
	0xff, //ascr_Mb
	0x00, //ascr_Gb
	0xff, //ascr_Yr
	0x00, //ascr_Br
	0xff, //ascr_Yg
	0x00, //ascr_Bg
	0x00, //ascr_Yb
	0xff, //ascr_Bb
	0xff, //ascr_Wr
	0x00, //ascr_Kr
	0xff, //ascr_Wg
	0x00, //ascr_Kg
	0xff, //ascr_Wb
	0x00, //ascr_Kb
	0x0F, //edge_ctrl
	0x00, //edge_radius 12
	0x6F,
	0x05, //edge_aa_factor
	0x00, //edge_r
	0x00, //edge_g
	0x00, //edge_b
	0x00, //edge_x 11
	0x70,
	0x00, //edge_y 12
	0x70,
	0x44, //edge_coef_a edge_coef_b 0000 0000
};

static unsigned char DSI_VIDEO_ENHANCER_THIRD_AUTO_MDNIE_2[] = {
	0xDE,
	0x00, //gamut_gamma_on gamut_blk_shift 0000 0000
	0x40, //gamut_scale
	0x04, //gamut_r1
	0x00,
	0x00, //gamut_r2
	0x00,
	0x00, //gamut_r3
	0x00,
	0x00, //gamut_g1
	0x00,
	0x04, //gamut_g2
	0x00,
	0x00, //gamut_g3
	0x00,
	0x00, //gamut_b1
	0x00,
	0x00, //gamut_b2
	0x00,
	0x04, //gamut_b3
	0x00,
	0x11, //slce_on cadrx_en 0000 0000
	0x00, //glare_on
	0x01, //lce_gain 000 0000
	0x18, //lce_color_gain 00 0000
	0xff, //lce_min_ref_offset
	0xa0, //lce_illum_gain
	0x01, //lce_ref_offset 9
	0x40,
	0x01, //lce_ref_gain 9
	0x00,
	0x66, //lce_block_size h v 0000 0000
	0x05, //lce_dark_th 000
	0x01, //lce_reduct_slope 0000
	0x46, //lce_black cc0 cc1 color_th 0 0 0 0000
	0x04, //lce_large_w
	0x06, //lce_med_w
	0x06, //lce_small_w
	0x05, //cadrx_dit_en shadow_corr_en ref_ctrl_en 0 0 0
	0x1b, //cadrx_gain
	0x0d, //cadrx_low_intensity_th
	0xe1, //cadrx_high_intensity_th
	0x00, //cadrx_ui_zerobincnt_th
	0x24, //cadrx_ui_ratio_th
	0xea,
	0x0e, //cadrx_entire_freq
	0x10,
	0x00,
	0x03, //cadrx_peak_th_in_freq
	0x21,
	0x90,
	0x00, //cadrx_high_peak_th_in_freq
	0x21,
	0x66,
	0x24, //cadrx_dit_color_gain
	0x00, //cadrx_dit_illum_a0
	0x48, //cadrx_dit_illum_a1
	0xea, //cadrx_dit_illum_a2
	0xfa, //cadrx_dit_illum_a3
	0x00, //cadrx_dit_illum_b0
	0x10,
	0x00, //cadrx_dit_illum_b1
	0x10,
	0x00, //cadrx_dit_illum_b2
	0x9c,
	0x01, //cadrx_dit_illum_b3
	0x00,
	0x00, //cadrx_dit_illum_slope1
	0x00,
	0x00, //cadrx_dit_illum_slope2
	0xdd,
	0x03, //cadrx_dit_illum_slope3
	0xff,
	0x20, //cadrx_ui_illum_a1
	0x40, //cadrx_ui_illum_a2
	0x60, //cadrx_ui_illum_a3
	0x00, //cadrx_ui_illum_offset1
	0xd0,
	0x00, //cadrx_ui_illum_offset2
	0xc0,
	0x00, //cadrx_ui_illum_offset3
	0xb0,
	0x00, //cadrx_ui_illum_offset4
	0xa0,
	0x07, //cadrx_ui_illum_slope1
	0x80,
	0x07, //cadrx_ui_illum_slope2
	0x80,
	0x07, //cadrx_ui_illum_slope3
	0x80,
	0x00, //cadrx_ui_illum_slope4
	0x00,
	0x20, //cadrx_ui_ref_a1
	0x40, //cadrx_ui_ref_a2
	0x60, //cadrx_ui_ref_a3
	0x01, //cadrx_ui_ref_offset1
	0x60,
	0x01, //cadrx_ui_ref_offset2
	0x50,
	0x01, //cadrx_ui_ref_offset3
	0x40,
	0x01, //cadrx_ui_ref_offset4
	0x30,
	0x07, //cadrx_ui_ref_slope1
	0x80,
	0x07, //cadrx_ui_ref_slope2
	0x80,
	0x07, //cadrx_ui_ref_slope3
	0x80,
	0x00, //cadrx_ui_ref_slope4
	0x00,
	0x00, //cadrx_reg_ref_c1_offset
	0x80,
	0x00, //cadrx_reg_ref_c2_offset
	0xac,
	0x00, //cadrx_reg_ref_c3_offset
	0xb6,
	0x00, //cadrx_reg_ref_c1_slope
	0x01,
	0x00, //cadrx_reg_ref_c2_slope
	0x40,
	0x00, //cadrx_reg_ref_c3_slope
	0x48,
	0x00, //cadrx_sc_reg_a1
	0x18,
	0x00, //cadrx_sc_reg_b1
	0x1c,
	0x01, //cadrx_sc_k1_int
	0xff,
	0x01, //cadrx_sc_k2_int
	0xff,
	0x03, //cadrx_sc_w1_int
	0xff,
	0xe8,
	0x03, //cadrx_sc_w2_int
	0xff,
	0xe8,
	0x00, //flicker_w_en
	0x14, //flicker_w
	0x00, //flicker_trans_en
	0x04, //flicker_trans_slope
	0x00, //bl_en
	0x03, //bl_hbs_th
	0x10, //bl_lbs_th
	0xC8, //bl_illum_gain
	0x01, //bl_reg_gain 9
	0x2C,
	0x00, //bl_le_diff
	0x00, //bl_cadrx_gain
	0x04, //bl_trans_slope
	0x01, //le_en
	0x40, //le_diff
	0x03, //le_lmax 10
	0x84,
	0x3c, //le_p
	0x28, //le_w_tmf
	0x00, //le_w_struct_diff 9
	0x40,
	0xff, //le_w_struct_diff_gain
	0x00, //le_w_luminance 9
	0x80,
	0x01, //le_luminance_slope 10
	0x00,
	0x80, //glare_blk_avg_th 0000 0000
	0x00, //glare_luma_gain 9
	0x28,
	0xa0, //glare_luma_ctl_start 0000 0000
	0x01, //glare_luma_th 0000 0000
	0x00, //glare_luma_step 9
	0x01,
	0x00, //glare_uni_luma_gain 9
	0x01,
	0x00, //glare_blk_max_th 0000 0000
	0x00, //glare_blk_max_w 0000 0000
	0x10, //glare_blk_max_curve blk_max_sh 0 0000
	0x00, //glare_con_ext_max_th 0000 0000
	0x00, //glare_con_ext_max_w 0000 0000
	0x30, //glare_con_ext_max_sign con_ext_max_curve con_ext_max_sh 0 0 0000
	0x00, //glare_max_cnt_th 0000
	0x00, //glare_max_cnt_th 0000 0000
	0x00, //glare_max_cnt_w 0000 0000
	0x30, //glare_max_cnt_sign max_cnt_curve max_cnt_sh 0 0 0000
	0x00, //glare_y_avg_th 0000 0000
	0x00, //glare_y_avg_w 0000 0000
	0x30, //glare_y_avg_sign y_avg_curve y_avg_sh 0 0 0000
	0x00, //glare_max_cnt_th_dmc 0000
	0x00, //glare_max_cnt_th_dmc 0000 0000
	0x07, //nr fa de cs gamma 0 0000
	0xff, //nr_mask_th
	0x00, //de_gain 10
	0x10,
	0x00, //de_maxplus 11
	0x40,
	0x00, //de_maxminus 11
	0x40,
	0x14, //fa_edge_th
	0x00, //fa_step_p 13
	0x01,
	0x00, //fa_step_n 13
	0x01,
	0x00, //fa_max_de_gain 13
	0x10,
	0x00, //fa_pcl_ppi 14
	0x00,
	0x28, //fa_os_cnt_10_co
	0x3c, //fa_os_cnt_20_co
	0x04, //fa_edge_cnt_weight
	0x0f, //fa_avg_y_weight
	0x01, //cs_gain 10
	0x20,
	0x00, //curve_0
	0x10, //curve_1
	0x20, //curve_2
	0x30, //curve_3
	0x40, //curve_4
	0x50, //curve_5
	0x60, //curve_6
	0x70, //curve_7
	0x82, //curve_8
	0x95, //curve_9
	0xaa, //curve_10
	0xbe, //curve_11
	0xcf, //curve_12
	0xdf, //curve_13
	0xee, //curve_14
	0xf9, //curve_15
	0x01, //curve_16
	0x00,
	0x00, //curve_offset 10
	0x00,
	0x00, //curve_low_x
	0x00, //blank
	0x00, //curve_low_y
};

static unsigned char DSI_VIDEO_ENHANCER_THIRD_AUTO_MDNIE_3[] = {
	0xDD,
	0x01, //mdnie_en
	0x00, //mask 00 0000
	0x03, //edge 00
	0xfc, //ascr algo aolce gamut 00 00 00 00
	0xff, //partial_en1
	0xff, //partial_en2
	0x00, //roi_en
	0x00, //roi_block
	0x00, //roi_sx
	0x00, //roi_sx
	0x00, //roi_sy
	0x00, //roi_sy
	0x00, //roi_ex
	0x00, //roi_ex
	0x00, //roi_ey
	0x00, //roi_ey
	0xff, //trans_on trans_block 0 000 0000
	0x04, //trans_slope
	0x01, //trans_interval
	//end
};

static char DSI_UI_DYNAMIC_MDNIE_1[] = {
	//start
	0xDF,
	0x11, //linear_on ascr_skin_on 0000 0000
	0x67, //ascr_skin_cb
	0xa9, //ascr_skin_cr
	0x37, //ascr_dist_up
	0x29, //ascr_dist_down
	0x19, //ascr_dist_right
	0x47, //ascr_dist_left
	0x00, //ascr_div_up 20
	0x25,
	0x3d,
	0x00, //ascr_div_down
	0x31,
	0xf4,
	0x00, //ascr_div_right
	0x51,
	0xec,
	0x00, //ascr_div_left
	0x1c,
	0xd8,
	0xe1, //ascr_skin_Rr
	0x25, //ascr_skin_Rg
	0x00, //ascr_skin_Rb
	0xee, //ascr_skin_Yr
	0xea, //ascr_skin_Yg
	0x00, //ascr_skin_Yb
	0xf0, //ascr_skin_Mr
	0x33, //ascr_skin_Mg
	0xe8, //ascr_skin_Mb
	0xff, //ascr_skin_Wr
	0xfa, //ascr_skin_Wg
	0xf1, //ascr_skin_Wb
	0x5c, //ascr_Cr
	0xe1, //ascr_Rr
	0xef, //ascr_Cg
	0x25, //ascr_Rg
	0xe8, //ascr_Cb
	0x00, //ascr_Rb
	0xf0, //ascr_Mr
	0x49, //ascr_Gr
	0x33, //ascr_Mg
	0xdf, //ascr_Gg
	0xe8, //ascr_Mb
	0x00, //ascr_Gb
	0xee, //ascr_Yr
	0x37, //ascr_Br
	0xea, //ascr_Yg
	0x21, //ascr_Bg
	0x00, //ascr_Yb
	0xdf, //ascr_Bb
	0xff, //ascr_Wr
	0x00, //ascr_Kr
	0xfa, //ascr_Wg
	0x00, //ascr_Kg
	0xf1, //ascr_Wb
	0x00, //ascr_Kb
	0x0F, //edge_ctrl
	0x00, //edge_radius 12
	0x6F,
	0x05, //edge_aa_factor
	0x00, //edge_r
	0x00, //edge_g
	0x00, //edge_b
	0x00, //edge_x 11
	0x70,
	0x00, //edge_y 12
	0x70,
	0x44, //edge_coef_a edge_coef_b 0000 0000
};

static char DSI_UI_DYNAMIC_MDNIE_2[] = {
	0xDE,
	0x00, //gamut_gamma_on gamut_blk_shift 0000 0000
	0x40, //gamut_scale
	0x04, //gamut_r1
	0x00,
	0x00, //gamut_r2
	0x00,
	0x00, //gamut_r3
	0x00,
	0x00, //gamut_g1
	0x00,
	0x04, //gamut_g2
	0x00,
	0x00, //gamut_g3
	0x00,
	0x00, //gamut_b1
	0x00,
	0x00, //gamut_b2
	0x00,
	0x04, //gamut_b3
	0x00,
	0x00, //slce_on cadrx_en 0000 0000
	0x00, //glare_on
	0x00, //lce_gain 000 0000
	0x24, //lce_color_gain 00 0000
	0x96, //lce_min_ref_offset
	0xb3, //lce_illum_gain
	0x01, //lce_ref_offset 9
	0x0e,
	0x01, //lce_ref_gain 9
	0x00,
	0x77, //lce_block_size h v 0000 0000
	0x03, //lce_dark_th 000
	0x07, //lce_reduct_slope 0000
	0x40, //lce_black cc0 cc1 color_th 0 0 0 0000
	0x08, //lce_large_w
	0x04, //lce_med_w
	0x04, //lce_small_w
	0x00, //cadrx_dit_en shadow_corr_en ref_ctrl_en 0 0 0
	0x00, //cadrx_gain
	0x0d, //cadrx_low_intensity_th
	0xe1, //cadrx_high_intensity_th
	0x07, //cadrx_ui_zerobincnt_th
	0x29, //cadrx_ui_ratio_th
	0x04,
	0x0f, //cadrx_entire_freq
	0xa0,
	0x00,
	0x03, //cadrx_peak_th_in_freq
	0x7a,
	0xa0,
	0x00, //cadrx_high_peak_th_in_freq
	0x25,
	0x1c,
	0x24, //cadrx_dit_color_gain
	0x00, //cadrx_dit_illum_a0
	0x48, //cadrx_dit_illum_a1
	0xea, //cadrx_dit_illum_a2
	0xfa, //cadrx_dit_illum_a3
	0x00, //cadrx_dit_illum_b0
	0x10,
	0x00, //cadrx_dit_illum_b1
	0x10,
	0x00, //cadrx_dit_illum_b2
	0x9c,
	0x01, //cadrx_dit_illum_b3
	0x00,
	0x00, //cadrx_dit_illum_slope1
	0x00,
	0x00, //cadrx_dit_illum_slope2
	0xdc,
	0x03, //cadrx_dit_illum_slope3
	0xff,
	0x40, //cadrx_ui_illum_a1
	0x80, //cadrx_ui_illum_a2
	0xc0, //cadrx_ui_illum_a3
	0x00, //cadrx_ui_illum_offset1
	0x9a,
	0x00, //cadrx_ui_illum_offset2
	0x9a,
	0x00, //cadrx_ui_illum_offset3
	0x9a,
	0x00, //cadrx_ui_illum_offset4
	0x9a,
	0x00, //cadrx_ui_illum_slope1
	0x00,
	0x00, //cadrx_ui_illum_slope2
	0x00,
	0x00, //cadrx_ui_illum_slope3
	0x00,
	0x00, //cadrx_ui_illum_slope4
	0x00,
	0x40, //cadrx_ui_ref_a1
	0x80, //cadrx_ui_ref_a2
	0xc0, //cadrx_ui_ref_a3
	0x01, //cadrx_ui_ref_offset1
	0x0e,
	0x01, //cadrx_ui_ref_offset2
	0x0e,
	0x01, //cadrx_ui_ref_offset3
	0x0e,
	0x01, //cadrx_ui_ref_offset4
	0x0e,
	0x00, //cadrx_ui_ref_slope1
	0x00,
	0x00, //cadrx_ui_ref_slope2
	0x00,
	0x00, //cadrx_ui_ref_slope3
	0x00,
	0x00, //cadrx_ui_ref_slope4
	0x00,
	0x01, //cadrx_reg_ref_c1_offset
	0x0e,
	0x01, //cadrx_reg_ref_c2_offset
	0x3a,
	0x01, //cadrx_reg_ref_c3_offset
	0x44,
	0x00, //cadrx_reg_ref_c1_slope
	0x01,
	0x00, //cadrx_reg_ref_c2_slope
	0x43,
	0x00, //cadrx_reg_ref_c3_slope
	0x4b,
	0x00, //cadrx_sc_reg_a1
	0x18,
	0x00, //cadrx_sc_reg_b1
	0x1c,
	0x01, //cadrx_sc_k1_int
	0xff,
	0x01, //cadrx_sc_k2_int
	0xff,
	0x03, //cadrx_sc_w1_int
	0xff,
	0xe8,
	0x03, //cadrx_sc_w2_int
	0xff,
	0xe8,
	0x00, //flicker_w_en
	0x14, //flicker_w
	0x01, //flicker_trans_en
	0x04, //flicker_trans_slope
	0x00, //bl_en
	0x03, //bl_hbs_th
	0x10, //bl_lbs_th
	0xC8, //bl_illum_gain
	0x01, //bl_reg_gain 9
	0x2C,
	0x00, //bl_le_diff
	0x00, //bl_cadrx_gain
	0x04, //bl_trans_slope
	0x00, //le_en
	0x40, //le_diff
	0x03, //le_lmax 10
	0xB6,
	0x23, //le_p
	0x24, //le_w_tmf
	0x00, //le_w_struct_diff 9
	0x72,
	0x01, //le_w_struct_diff_gain
	0x00, //le_w_luminance 9
	0x0D,
	0x00, //le_luminance_slope 10
	0x04,
	0x80, //glare_blk_avg_th 0000 0000
	0x00, //glare_luma_gain 9
	0x28,
	0xa0, //glare_luma_ctl_start 0000 0000
	0x01, //glare_luma_th 0000 0000
	0x00, //glare_luma_step 9
	0x01,
	0x00, //glare_uni_luma_gain 9
	0x01,
	0x00, //glare_blk_max_th 0000 0000
	0x00, //glare_blk_max_w 0000 0000
	0x10, //glare_blk_max_curve blk_max_sh 0 0000
	0x00, //glare_con_ext_max_th 0000 0000
	0x00, //glare_con_ext_max_w 0000 0000
	0x30, //glare_con_ext_max_sign con_ext_max_curve con_ext_max_sh 0 0 0000
	0x00, //glare_max_cnt_th 0000
	0x00, //glare_max_cnt_th 0000 0000
	0x00, //glare_max_cnt_w 0000 0000
	0x30, //glare_max_cnt_sign max_cnt_curve max_cnt_sh 0 0 0000
	0x00, //glare_y_avg_th 0000 0000
	0x00, //glare_y_avg_w 0000 0000
	0x30, //glare_y_avg_sign y_avg_curve y_avg_sh 0 0 0000
	0x00, //glare_max_cnt_th_dmc 0000
	0x00, //glare_max_cnt_th_dmc 0000 0000
	0x00, //nr fa de cs gamma 0 0000
	0xff, //nr_mask_th
	0x00, //de_gain 10
	0x00,
	0x02, //de_maxplus 11
	0x00,
	0x02, //de_maxminus 11
	0x00,
	0x14, //fa_edge_th
	0x00, //fa_step_p 13
	0x01,
	0x00, //fa_step_n 13
	0x01,
	0x00, //fa_max_de_gain 13
	0x10,
	0x00, //fa_pcl_ppi 14
	0x00,
	0x28, //fa_os_cnt_10_co
	0x3c, //fa_os_cnt_20_co
	0x04, //fa_edge_cnt_weight
	0x0f, //fa_avg_y_weight
	0x01, //cs_gain 10
	0x00,
	0x00, //curve_0
	0x10, //curve_1
	0x20, //curve_2
	0x30, //curve_3
	0x40, //curve_4
	0x50, //curve_5
	0x60, //curve_6
	0x70, //curve_7
	0x80, //curve_8
	0x90, //curve_9
	0xa0, //curve_10
	0xb0, //curve_11
	0xc0, //curve_12
	0xd0, //curve_13
	0xe0, //curve_14
	0xf0, //curve_15
	0x01, //curve_16
	0x00,
	0x00, //curve_offset 10
	0x00,
	0x00, //curve_low_x
	0x00, //blank
	0x00, //curve_low_y
};

static char DSI_UI_DYNAMIC_MDNIE_3[] = {
	0xDD,
	0x01, //mdnie_en
	0x00, //mask 00 0000
	0x03, //edge 00
	0xf0, //ascr algo aolce gamut 00 00 00 00
	0xff, //partial_en1
	0xff, //partial_en2
	0x00, //roi_en
	0x00, //roi_block
	0x00, //roi_sx
	0x00, //roi_sx
	0x00, //roi_sy
	0x00, //roi_sy
	0x00, //roi_ex
	0x00, //roi_ex
	0x00, //roi_ey
	0x00, //roi_ey
	0xff, //trans_on trans_block 0 000 0000
	0x04, //trans_slope
	0x01, //trans_interval
	//end
};

static char DSI_UI_STANDARD_MDNIE_1[] = {
	//start
	0xDF,
	0x11, //linear_on ascr_skin_on 0000 0000
	0x6a, //ascr_skin_cb
	0x9a, //ascr_skin_cr
	0x25, //ascr_dist_up
	0x1a, //ascr_dist_down
	0x16, //ascr_dist_right
	0x2a, //ascr_dist_left
	0x00, //ascr_div_up 20
	0x37,
	0x5a,
	0x00, //ascr_div_down
	0x4e,
	0xc5,
	0x00, //ascr_div_right
	0x5d,
	0x17,
	0x00, //ascr_div_left
	0x30,
	0xc3,
	0xee, //ascr_skin_Rr
	0x4b, //ascr_skin_Rg
	0x30, //ascr_skin_Rb
	0xef, //ascr_skin_Yr
	0xeb, //ascr_skin_Yg
	0x47, //ascr_skin_Yb
	0xfc, //ascr_skin_Mr
	0x57, //ascr_skin_Mg
	0xe7, //ascr_skin_Mb
	0xff, //ascr_skin_Wr
	0xfa, //ascr_skin_Wg
	0xf1, //ascr_skin_Wb
	0x00, //ascr_Cr
	0xee, //ascr_Rr
	0xe7, //ascr_Cg
	0x4b, //ascr_Rg
	0xe5, //ascr_Cb
	0x30, //ascr_Rb
	0xfc, //ascr_Mr
	0x00, //ascr_Gr
	0x57, //ascr_Mg
	0xd9, //ascr_Gg
	0xe7, //ascr_Mb
	0x34, //ascr_Gb
	0xef, //ascr_Yr
	0x34, //ascr_Br
	0xeb, //ascr_Yg
	0x20, //ascr_Bg
	0x47, //ascr_Yb
	0xdb, //ascr_Bb
	0xff, //ascr_Wr
	0x00, //ascr_Kr
	0xfa, //ascr_Wg
	0x00, //ascr_Kg
	0xf1, //ascr_Wb
	0x00, //ascr_Kb
	0x0F, //edge_ctrl
	0x00, //edge_radius 12
	0x6F,
	0x05, //edge_aa_factor
	0x00, //edge_r
	0x00, //edge_g
	0x00, //edge_b
	0x00, //edge_x 11
	0x70,
	0x00, //edge_y 12
	0x70,
	0x44, //edge_coef_a edge_coef_b 0000 0000
};

static char DSI_UI_STANDARD_MDNIE_2[] = {
	0xDE,
	0x00, //gamut_gamma_on gamut_blk_shift 0000 0000
	0x40, //gamut_scale
	0x04, //gamut_r1
	0x00,
	0x00, //gamut_r2
	0x00,
	0x00, //gamut_r3
	0x00,
	0x00, //gamut_g1
	0x00,
	0x04, //gamut_g2
	0x00,
	0x00, //gamut_g3
	0x00,
	0x00, //gamut_b1
	0x00,
	0x00, //gamut_b2
	0x00,
	0x04, //gamut_b3
	0x00,
	0x00, //slce_on cadrx_en 0000 0000
	0x00, //glare_on
	0x00, //lce_gain 000 0000
	0x24, //lce_color_gain 00 0000
	0x96, //lce_min_ref_offset
	0xb3, //lce_illum_gain
	0x01, //lce_ref_offset 9
	0x0e,
	0x01, //lce_ref_gain 9
	0x00,
	0x77, //lce_block_size h v 0000 0000
	0x03, //lce_dark_th 000
	0x07, //lce_reduct_slope 0000
	0x40, //lce_black cc0 cc1 color_th 0 0 0 0000
	0x08, //lce_large_w
	0x04, //lce_med_w
	0x04, //lce_small_w
	0x00, //cadrx_dit_en shadow_corr_en ref_ctrl_en 0 0 0
	0x00, //cadrx_gain
	0x0d, //cadrx_low_intensity_th
	0xe1, //cadrx_high_intensity_th
	0x07, //cadrx_ui_zerobincnt_th
	0x29, //cadrx_ui_ratio_th
	0x04,
	0x0f, //cadrx_entire_freq
	0xa0,
	0x00,
	0x03, //cadrx_peak_th_in_freq
	0x7a,
	0xa0,
	0x00, //cadrx_high_peak_th_in_freq
	0x25,
	0x1c,
	0x24, //cadrx_dit_color_gain
	0x00, //cadrx_dit_illum_a0
	0x48, //cadrx_dit_illum_a1
	0xea, //cadrx_dit_illum_a2
	0xfa, //cadrx_dit_illum_a3
	0x00, //cadrx_dit_illum_b0
	0x10,
	0x00, //cadrx_dit_illum_b1
	0x10,
	0x00, //cadrx_dit_illum_b2
	0x9c,
	0x01, //cadrx_dit_illum_b3
	0x00,
	0x00, //cadrx_dit_illum_slope1
	0x00,
	0x00, //cadrx_dit_illum_slope2
	0xdc,
	0x03, //cadrx_dit_illum_slope3
	0xff,
	0x40, //cadrx_ui_illum_a1
	0x80, //cadrx_ui_illum_a2
	0xc0, //cadrx_ui_illum_a3
	0x00, //cadrx_ui_illum_offset1
	0x9a,
	0x00, //cadrx_ui_illum_offset2
	0x9a,
	0x00, //cadrx_ui_illum_offset3
	0x9a,
	0x00, //cadrx_ui_illum_offset4
	0x9a,
	0x00, //cadrx_ui_illum_slope1
	0x00,
	0x00, //cadrx_ui_illum_slope2
	0x00,
	0x00, //cadrx_ui_illum_slope3
	0x00,
	0x00, //cadrx_ui_illum_slope4
	0x00,
	0x40, //cadrx_ui_ref_a1
	0x80, //cadrx_ui_ref_a2
	0xc0, //cadrx_ui_ref_a3
	0x01, //cadrx_ui_ref_offset1
	0x0e,
	0x01, //cadrx_ui_ref_offset2
	0x0e,
	0x01, //cadrx_ui_ref_offset3
	0x0e,
	0x01, //cadrx_ui_ref_offset4
	0x0e,
	0x00, //cadrx_ui_ref_slope1
	0x00,
	0x00, //cadrx_ui_ref_slope2
	0x00,
	0x00, //cadrx_ui_ref_slope3
	0x00,
	0x00, //cadrx_ui_ref_slope4
	0x00,
	0x01, //cadrx_reg_ref_c1_offset
	0x0e,
	0x01, //cadrx_reg_ref_c2_offset
	0x3a,
	0x01, //cadrx_reg_ref_c3_offset
	0x44,
	0x00, //cadrx_reg_ref_c1_slope
	0x01,
	0x00, //cadrx_reg_ref_c2_slope
	0x43,
	0x00, //cadrx_reg_ref_c3_slope
	0x4b,
	0x00, //cadrx_sc_reg_a1
	0x18,
	0x00, //cadrx_sc_reg_b1
	0x1c,
	0x01, //cadrx_sc_k1_int
	0xff,
	0x01, //cadrx_sc_k2_int
	0xff,
	0x03, //cadrx_sc_w1_int
	0xff,
	0xe8,
	0x03, //cadrx_sc_w2_int
	0xff,
	0xe8,
	0x00, //flicker_w_en
	0x14, //flicker_w
	0x01, //flicker_trans_en
	0x04, //flicker_trans_slope
	0x00, //bl_en
	0x03, //bl_hbs_th
	0x10, //bl_lbs_th
	0xC8, //bl_illum_gain
	0x01, //bl_reg_gain 9
	0x2C,
	0x00, //bl_le_diff
	0x00, //bl_cadrx_gain
	0x04, //bl_trans_slope
	0x00, //le_en
	0x40, //le_diff
	0x03, //le_lmax 10
	0xB6,
	0x23, //le_p
	0x24, //le_w_tmf
	0x00, //le_w_struct_diff 9
	0x72,
	0x01, //le_w_struct_diff_gain
	0x00, //le_w_luminance 9
	0x0D,
	0x00, //le_luminance_slope 10
	0x04,
	0x80, //glare_blk_avg_th 0000 0000
	0x00, //glare_luma_gain 9
	0x28,
	0xa0, //glare_luma_ctl_start 0000 0000
	0x01, //glare_luma_th 0000 0000
	0x00, //glare_luma_step 9
	0x01,
	0x00, //glare_uni_luma_gain 9
	0x01,
	0x00, //glare_blk_max_th 0000 0000
	0x00, //glare_blk_max_w 0000 0000
	0x10, //glare_blk_max_curve blk_max_sh 0 0000
	0x00, //glare_con_ext_max_th 0000 0000
	0x00, //glare_con_ext_max_w 0000 0000
	0x30, //glare_con_ext_max_sign con_ext_max_curve con_ext_max_sh 0 0 0000
	0x00, //glare_max_cnt_th 0000
	0x00, //glare_max_cnt_th 0000 0000
	0x00, //glare_max_cnt_w 0000 0000
	0x30, //glare_max_cnt_sign max_cnt_curve max_cnt_sh 0 0 0000
	0x00, //glare_y_avg_th 0000 0000
	0x00, //glare_y_avg_w 0000 0000
	0x30, //glare_y_avg_sign y_avg_curve y_avg_sh 0 0 0000
	0x00, //glare_max_cnt_th_dmc 0000
	0x00, //glare_max_cnt_th_dmc 0000 0000
	0x02, //nr fa de cs gamma 0 0000
	0xff, //nr_mask_th
	0x00, //de_gain 10
	0x00,
	0x02, //de_maxplus 11
	0x00,
	0x02, //de_maxminus 11
	0x00,
	0x14, //fa_edge_th
	0x00, //fa_step_p 13
	0x01,
	0x00, //fa_step_n 13
	0x01,
	0x00, //fa_max_de_gain 13
	0x10,
	0x00, //fa_pcl_ppi 14
	0x00,
	0x28, //fa_os_cnt_10_co
	0x3c, //fa_os_cnt_20_co
	0x04, //fa_edge_cnt_weight
	0x0f, //fa_avg_y_weight
	0x01, //cs_gain 10
	0x18,
	0x00, //curve_0
	0x10, //curve_1
	0x20, //curve_2
	0x30, //curve_3
	0x40, //curve_4
	0x50, //curve_5
	0x60, //curve_6
	0x70, //curve_7
	0x80, //curve_8
	0x90, //curve_9
	0xa0, //curve_10
	0xb0, //curve_11
	0xc0, //curve_12
	0xd0, //curve_13
	0xe0, //curve_14
	0xf0, //curve_15
	0x01, //curve_16
	0x00,
	0x00, //curve_offset 10
	0x00,
	0x00, //curve_low_x
	0x00, //blank
	0x00, //curve_low_y
};

static char DSI_UI_STANDARD_MDNIE_3[] = {
	0xDD,
	0x01, //mdnie_en
	0x00, //mask 00 0000
	0x03, //edge 00
	0xf0, //ascr algo aolce gamut 00 00 00 00
	0xff, //partial_en1
	0xff, //partial_en2
	0x00, //roi_en
	0x00, //roi_block
	0x00, //roi_sx
	0x00, //roi_sx
	0x00, //roi_sy
	0x00, //roi_sy
	0x00, //roi_ex
	0x00, //roi_ex
	0x00, //roi_ey
	0x00, //roi_ey
	0xff, //trans_on trans_block 0 000 0000
	0x04, //trans_slope
	0x01, //trans_interval
	//end
};

static char DSI_UI_NATURAL_MDNIE_1[] = {
	//start
	0xDF,
	0x11, //linear_on ascr_skin_on 0000 0000
	0x6a, //ascr_skin_cb
	0x9a, //ascr_skin_cr
	0x25, //ascr_dist_up
	0x1a, //ascr_dist_down
	0x16, //ascr_dist_right
	0x2a, //ascr_dist_left
	0x00, //ascr_div_up 20
	0x37,
	0x5a,
	0x00, //ascr_div_down
	0x4e,
	0xc5,
	0x00, //ascr_div_right
	0x5d,
	0x17,
	0x00, //ascr_div_left
	0x30,
	0xc3,
	0xd3, //ascr_skin_Rr
	0x40, //ascr_skin_Rg
	0x29, //ascr_skin_Rb
	0xf0, //ascr_skin_Yr
	0xec, //ascr_skin_Yg
	0x5b, //ascr_skin_Yb
	0xdf, //ascr_skin_Mr
	0x4a, //ascr_skin_Mg
	0xdf, //ascr_skin_Mb
	0xff, //ascr_skin_Wr
	0xfa, //ascr_skin_Wg
	0xf1, //ascr_skin_Wb
	0x93, //ascr_Cr
	0xd3, //ascr_Rr
	0xed, //ascr_Cg
	0x40, //ascr_Rg
	0xe7, //ascr_Cb
	0x29, //ascr_Rb
	0xdf, //ascr_Mr
	0x85, //ascr_Gr
	0x4a, //ascr_Mg
	0xe0, //ascr_Gg
	0xdf, //ascr_Mb
	0x4f, //ascr_Gb
	0xf0, //ascr_Yr
	0x32, //ascr_Br
	0xec, //ascr_Yg
	0x1f, //ascr_Bg
	0x5b, //ascr_Yb
	0xd8, //ascr_Bb
	0xff, //ascr_Wr
	0x00, //ascr_Kr
	0xfa, //ascr_Wg
	0x00, //ascr_Kg
	0xf1, //ascr_Wb
	0x00, //ascr_Kb
	0x0F, //edge_ctrl
	0x00, //edge_radius 12
	0x6F,
	0x05, //edge_aa_factor
	0x00, //edge_r
	0x00, //edge_g
	0x00, //edge_b
	0x00, //edge_x 11
	0x70,
	0x00, //edge_y 12
	0x70,
	0x44, //edge_coef_a edge_coef_b 0000 0000
};

static char DSI_UI_NATURAL_MDNIE_2[] = {
	0xDE,
	0x00, //gamut_gamma_on gamut_blk_shift 0000 0000
	0x40, //gamut_scale
	0x04, //gamut_r1
	0x00,
	0x00, //gamut_r2
	0x00,
	0x00, //gamut_r3
	0x00,
	0x00, //gamut_g1
	0x00,
	0x04, //gamut_g2
	0x00,
	0x00, //gamut_g3
	0x00,
	0x00, //gamut_b1
	0x00,
	0x00, //gamut_b2
	0x00,
	0x04, //gamut_b3
	0x00,
	0x00, //slce_on cadrx_en 0000 0000
	0x00, //glare_on
	0x00, //lce_gain 000 0000
	0x24, //lce_color_gain 00 0000
	0x96, //lce_min_ref_offset
	0xb3, //lce_illum_gain
	0x01, //lce_ref_offset 9
	0x0e,
	0x01, //lce_ref_gain 9
	0x00,
	0x77, //lce_block_size h v 0000 0000
	0x03, //lce_dark_th 000
	0x07, //lce_reduct_slope 0000
	0x40, //lce_black cc0 cc1 color_th 0 0 0 0000
	0x08, //lce_large_w
	0x04, //lce_med_w
	0x04, //lce_small_w
	0x00, //cadrx_dit_en shadow_corr_en ref_ctrl_en 0 0 0
	0x00, //cadrx_gain
	0x0d, //cadrx_low_intensity_th
	0xe1, //cadrx_high_intensity_th
	0x07, //cadrx_ui_zerobincnt_th
	0x29, //cadrx_ui_ratio_th
	0x04,
	0x0f, //cadrx_entire_freq
	0xa0,
	0x00,
	0x03, //cadrx_peak_th_in_freq
	0x7a,
	0xa0,
	0x00, //cadrx_high_peak_th_in_freq
	0x25,
	0x1c,
	0x24, //cadrx_dit_color_gain
	0x00, //cadrx_dit_illum_a0
	0x48, //cadrx_dit_illum_a1
	0xea, //cadrx_dit_illum_a2
	0xfa, //cadrx_dit_illum_a3
	0x00, //cadrx_dit_illum_b0
	0x10,
	0x00, //cadrx_dit_illum_b1
	0x10,
	0x00, //cadrx_dit_illum_b2
	0x9c,
	0x01, //cadrx_dit_illum_b3
	0x00,
	0x00, //cadrx_dit_illum_slope1
	0x00,
	0x00, //cadrx_dit_illum_slope2
	0xdc,
	0x03, //cadrx_dit_illum_slope3
	0xff,
	0x40, //cadrx_ui_illum_a1
	0x80, //cadrx_ui_illum_a2
	0xc0, //cadrx_ui_illum_a3
	0x00, //cadrx_ui_illum_offset1
	0x9a,
	0x00, //cadrx_ui_illum_offset2
	0x9a,
	0x00, //cadrx_ui_illum_offset3
	0x9a,
	0x00, //cadrx_ui_illum_offset4
	0x9a,
	0x00, //cadrx_ui_illum_slope1
	0x00,
	0x00, //cadrx_ui_illum_slope2
	0x00,
	0x00, //cadrx_ui_illum_slope3
	0x00,
	0x00, //cadrx_ui_illum_slope4
	0x00,
	0x40, //cadrx_ui_ref_a1
	0x80, //cadrx_ui_ref_a2
	0xc0, //cadrx_ui_ref_a3
	0x01, //cadrx_ui_ref_offset1
	0x0e,
	0x01, //cadrx_ui_ref_offset2
	0x0e,
	0x01, //cadrx_ui_ref_offset3
	0x0e,
	0x01, //cadrx_ui_ref_offset4
	0x0e,
	0x00, //cadrx_ui_ref_slope1
	0x00,
	0x00, //cadrx_ui_ref_slope2
	0x00,
	0x00, //cadrx_ui_ref_slope3
	0x00,
	0x00, //cadrx_ui_ref_slope4
	0x00,
	0x01, //cadrx_reg_ref_c1_offset
	0x0e,
	0x01, //cadrx_reg_ref_c2_offset
	0x3a,
	0x01, //cadrx_reg_ref_c3_offset
	0x44,
	0x00, //cadrx_reg_ref_c1_slope
	0x01,
	0x00, //cadrx_reg_ref_c2_slope
	0x43,
	0x00, //cadrx_reg_ref_c3_slope
	0x4b,
	0x00, //cadrx_sc_reg_a1
	0x18,
	0x00, //cadrx_sc_reg_b1
	0x1c,
	0x01, //cadrx_sc_k1_int
	0xff,
	0x01, //cadrx_sc_k2_int
	0xff,
	0x03, //cadrx_sc_w1_int
	0xff,
	0xe8,
	0x03, //cadrx_sc_w2_int
	0xff,
	0xe8,
	0x00, //flicker_w_en
	0x14, //flicker_w
	0x01, //flicker_trans_en
	0x04, //flicker_trans_slope
	0x00, //bl_en
	0x03, //bl_hbs_th
	0x10, //bl_lbs_th
	0xC8, //bl_illum_gain
	0x01, //bl_reg_gain 9
	0x2C,
	0x00, //bl_le_diff
	0x00, //bl_cadrx_gain
	0x04, //bl_trans_slope
	0x00, //le_en
	0x40, //le_diff
	0x03, //le_lmax 10
	0xB6,
	0x23, //le_p
	0x24, //le_w_tmf
	0x00, //le_w_struct_diff 9
	0x72,
	0x01, //le_w_struct_diff_gain
	0x00, //le_w_luminance 9
	0x0D,
	0x00, //le_luminance_slope 10
	0x04,
	0x80, //glare_blk_avg_th 0000 0000
	0x00, //glare_luma_gain 9
	0x28,
	0xa0, //glare_luma_ctl_start 0000 0000
	0x01, //glare_luma_th 0000 0000
	0x00, //glare_luma_step 9
	0x01,
	0x00, //glare_uni_luma_gain 9
	0x01,
	0x00, //glare_blk_max_th 0000 0000
	0x00, //glare_blk_max_w 0000 0000
	0x10, //glare_blk_max_curve blk_max_sh 0 0000
	0x00, //glare_con_ext_max_th 0000 0000
	0x00, //glare_con_ext_max_w 0000 0000
	0x30, //glare_con_ext_max_sign con_ext_max_curve con_ext_max_sh 0 0 0000
	0x00, //glare_max_cnt_th 0000
	0x00, //glare_max_cnt_th 0000 0000
	0x00, //glare_max_cnt_w 0000 0000
	0x30, //glare_max_cnt_sign max_cnt_curve max_cnt_sh 0 0 0000
	0x00, //glare_y_avg_th 0000 0000
	0x00, //glare_y_avg_w 0000 0000
	0x30, //glare_y_avg_sign y_avg_curve y_avg_sh 0 0 0000
	0x00, //glare_max_cnt_th_dmc 0000
	0x00, //glare_max_cnt_th_dmc 0000 0000
	0x00, //nr fa de cs gamma 0 0000
	0xff, //nr_mask_th
	0x00, //de_gain 10
	0x00,
	0x02, //de_maxplus 11
	0x00,
	0x02, //de_maxminus 11
	0x00,
	0x14, //fa_edge_th
	0x00, //fa_step_p 13
	0x01,
	0x00, //fa_step_n 13
	0x01,
	0x00, //fa_max_de_gain 13
	0x10,
	0x00, //fa_pcl_ppi 14
	0x00,
	0x28, //fa_os_cnt_10_co
	0x3c, //fa_os_cnt_20_co
	0x04, //fa_edge_cnt_weight
	0x0f, //fa_avg_y_weight
	0x01, //cs_gain 10
	0x00,
	0x00, //curve_0
	0x10, //curve_1
	0x20, //curve_2
	0x30, //curve_3
	0x40, //curve_4
	0x50, //curve_5
	0x60, //curve_6
	0x70, //curve_7
	0x80, //curve_8
	0x90, //curve_9
	0xa0, //curve_10
	0xb0, //curve_11
	0xc0, //curve_12
	0xd0, //curve_13
	0xe0, //curve_14
	0xf0, //curve_15
	0x01, //curve_16
	0x00,
	0x00, //curve_offset 10
	0x00,
	0x00, //curve_low_x
	0x00, //blank
	0x00, //curve_low_y
};

static char DSI_UI_NATURAL_MDNIE_3[] = {
	0xDD,
	0x01, //mdnie_en
	0x00, //mask 00 0000
	0x03, //edge 00
	0xf0, //ascr algo aolce gamut 00 00 00 00
	0xff, //partial_en1
	0xff, //partial_en2
	0x00, //roi_en
	0x00, //roi_block
	0x00, //roi_sx
	0x00, //roi_sx
	0x00, //roi_sy
	0x00, //roi_sy
	0x00, //roi_ex
	0x00, //roi_ex
	0x00, //roi_ey
	0x00, //roi_ey
	0xff, //trans_on trans_block 0 000 0000
	0x04, //trans_slope
	0x01, //trans_interval
	//end
};

static char DSI_UI_AUTO_MDNIE_1[] = {
	//start
	0xDF,
	0x01, //linear_on ascr_skin_on 0000 0000
	0x6a, //ascr_skin_cb
	0x9a, //ascr_skin_cr
	0x25, //ascr_dist_up
	0x1a, //ascr_dist_down
	0x16, //ascr_dist_right
	0x2a, //ascr_dist_left
	0x00, //ascr_div_up 20
	0x37,
	0x5a,
	0x00, //ascr_div_down
	0x4e,
	0xc5,
	0x00, //ascr_div_right
	0x5d,
	0x17,
	0x00, //ascr_div_left
	0x30,
	0xc3,
	0xff, //ascr_skin_Rr
	0x00, //ascr_skin_Rg
	0x00, //ascr_skin_Rb
	0xff, //ascr_skin_Yr
	0xff, //ascr_skin_Yg
	0x00, //ascr_skin_Yb
	0xff, //ascr_skin_Mr
	0x00, //ascr_skin_Mg
	0xff, //ascr_skin_Mb
	0xff, //ascr_skin_Wr
	0xff, //ascr_skin_Wg
	0xff, //ascr_skin_Wb
	0x00, //ascr_Cr
	0xff, //ascr_Rr
	0xff, //ascr_Cg
	0x00, //ascr_Rg
	0xff, //ascr_Cb
	0x00, //ascr_Rb
	0xff, //ascr_Mr
	0x00, //ascr_Gr
	0x00, //ascr_Mg
	0xff, //ascr_Gg
	0xff, //ascr_Mb
	0x00, //ascr_Gb
	0xff, //ascr_Yr
	0x00, //ascr_Br
	0xff, //ascr_Yg
	0x00, //ascr_Bg
	0x00, //ascr_Yb
	0xff, //ascr_Bb
	0xff, //ascr_Wr
	0x00, //ascr_Kr
	0xff, //ascr_Wg
	0x00, //ascr_Kg
	0xff, //ascr_Wb
	0x00, //ascr_Kb
	0x0F, //edge_ctrl
	0x00, //edge_radius 12
	0x6F,
	0x05, //edge_aa_factor
	0x00, //edge_r
	0x00, //edge_g
	0x00, //edge_b
	0x00, //edge_x 11
	0x70,
	0x00, //edge_y 12
	0x70,
	0x44, //edge_coef_a edge_coef_b 0000 0000
};

static char DSI_UI_AUTO_MDNIE_2[] = {
	0xDE,
	0x00, //gamut_gamma_on gamut_blk_shift 0000 0000
	0x40, //gamut_scale
	0x04, //gamut_r1
	0x00,
	0x00, //gamut_r2
	0x00,
	0x00, //gamut_r3
	0x00,
	0x00, //gamut_g1
	0x00,
	0x04, //gamut_g2
	0x00,
	0x00, //gamut_g3
	0x00,
	0x00, //gamut_b1
	0x00,
	0x00, //gamut_b2
	0x00,
	0x04, //gamut_b3
	0x00,
	0x00, //slce_on cadrx_en 0000 0000
	0x00, //glare_on
	0x00, //lce_gain 000 0000
	0x24, //lce_color_gain 00 0000
	0x96, //lce_min_ref_offset
	0xb3, //lce_illum_gain
	0x01, //lce_ref_offset 9
	0x0e,
	0x01, //lce_ref_gain 9
	0x00,
	0x77, //lce_block_size h v 0000 0000
	0x03, //lce_dark_th 000
	0x07, //lce_reduct_slope 0000
	0x40, //lce_black cc0 cc1 color_th 0 0 0 0000
	0x08, //lce_large_w
	0x04, //lce_med_w
	0x04, //lce_small_w
	0x00, //cadrx_dit_en shadow_corr_en ref_ctrl_en 0 0 0
	0x00, //cadrx_gain
	0x0d, //cadrx_low_intensity_th
	0xe1, //cadrx_high_intensity_th
	0x07, //cadrx_ui_zerobincnt_th
	0x29, //cadrx_ui_ratio_th
	0x04,
	0x0f, //cadrx_entire_freq
	0xa0,
	0x00,
	0x03, //cadrx_peak_th_in_freq
	0x7a,
	0xa0,
	0x00, //cadrx_high_peak_th_in_freq
	0x25,
	0x1c,
	0x24, //cadrx_dit_color_gain
	0x00, //cadrx_dit_illum_a0
	0x48, //cadrx_dit_illum_a1
	0xea, //cadrx_dit_illum_a2
	0xfa, //cadrx_dit_illum_a3
	0x00, //cadrx_dit_illum_b0
	0x10,
	0x00, //cadrx_dit_illum_b1
	0x10,
	0x00, //cadrx_dit_illum_b2
	0x9c,
	0x01, //cadrx_dit_illum_b3
	0x00,
	0x00, //cadrx_dit_illum_slope1
	0x00,
	0x00, //cadrx_dit_illum_slope2
	0xdc,
	0x03, //cadrx_dit_illum_slope3
	0xff,
	0x40, //cadrx_ui_illum_a1
	0x80, //cadrx_ui_illum_a2
	0xc0, //cadrx_ui_illum_a3
	0x00, //cadrx_ui_illum_offset1
	0x9a,
	0x00, //cadrx_ui_illum_offset2
	0x9a,
	0x00, //cadrx_ui_illum_offset3
	0x9a,
	0x00, //cadrx_ui_illum_offset4
	0x9a,
	0x00, //cadrx_ui_illum_slope1
	0x00,
	0x00, //cadrx_ui_illum_slope2
	0x00,
	0x00, //cadrx_ui_illum_slope3
	0x00,
	0x00, //cadrx_ui_illum_slope4
	0x00,
	0x40, //cadrx_ui_ref_a1
	0x80, //cadrx_ui_ref_a2
	0xc0, //cadrx_ui_ref_a3
	0x01, //cadrx_ui_ref_offset1
	0x0e,
	0x01, //cadrx_ui_ref_offset2
	0x0e,
	0x01, //cadrx_ui_ref_offset3
	0x0e,
	0x01, //cadrx_ui_ref_offset4
	0x0e,
	0x00, //cadrx_ui_ref_slope1
	0x00,
	0x00, //cadrx_ui_ref_slope2
	0x00,
	0x00, //cadrx_ui_ref_slope3
	0x00,
	0x00, //cadrx_ui_ref_slope4
	0x00,
	0x01, //cadrx_reg_ref_c1_offset
	0x0e,
	0x01, //cadrx_reg_ref_c2_offset
	0x3a,
	0x01, //cadrx_reg_ref_c3_offset
	0x44,
	0x00, //cadrx_reg_ref_c1_slope
	0x01,
	0x00, //cadrx_reg_ref_c2_slope
	0x43,
	0x00, //cadrx_reg_ref_c3_slope
	0x4b,
	0x00, //cadrx_sc_reg_a1
	0x18,
	0x00, //cadrx_sc_reg_b1
	0x1c,
	0x01, //cadrx_sc_k1_int
	0xff,
	0x01, //cadrx_sc_k2_int
	0xff,
	0x03, //cadrx_sc_w1_int
	0xff,
	0xe8,
	0x03, //cadrx_sc_w2_int
	0xff,
	0xe8,
	0x00, //flicker_w_en
	0x14, //flicker_w
	0x01, //flicker_trans_en
	0x04, //flicker_trans_slope
	0x00, //bl_en
	0x03, //bl_hbs_th
	0x10, //bl_lbs_th
	0xC8, //bl_illum_gain
	0x01, //bl_reg_gain 9
	0x2C,
	0x00, //bl_le_diff
	0x00, //bl_cadrx_gain
	0x04, //bl_trans_slope
	0x00, //le_en
	0x40, //le_diff
	0x03, //le_lmax 10
	0xB6,
	0x23, //le_p
	0x24, //le_w_tmf
	0x00, //le_w_struct_diff 9
	0x72,
	0x01, //le_w_struct_diff_gain
	0x00, //le_w_luminance 9
	0x0D,
	0x00, //le_luminance_slope 10
	0x04,
	0x80, //glare_blk_avg_th 0000 0000
	0x00, //glare_luma_gain 9
	0x28,
	0xa0, //glare_luma_ctl_start 0000 0000
	0x01, //glare_luma_th 0000 0000
	0x00, //glare_luma_step 9
	0x01,
	0x00, //glare_uni_luma_gain 9
	0x01,
	0xf0, //glare_blk_max_th 0000 0000
	0x00, //glare_blk_max_w 0000 0000
	0x18, //glare_blk_max_curve blk_max_sh 0 0000
	0xf0, //glare_con_ext_max_th 0000 0000
	0x00, //glare_con_ext_max_w 0000 0000
	0x38, //glare_con_ext_max_sign con_ext_max_curve con_ext_max_sh 0 0 0000
	0x00, //glare_max_cnt_th 0000
	0x00, //glare_max_cnt_th 0000 0000
	0x00, //glare_max_cnt_w 0000 0000
	0x38, //glare_max_cnt_sign max_cnt_curve max_cnt_sh 0 0 0000
	0xba, //glare_y_avg_th 0000 0000
	0x00, //glare_y_avg_w 0000 0000
	0x38, //glare_y_avg_sign y_avg_curve y_avg_sh 0 0 0000
	0x00, //glare_max_cnt_th_dmc 0000
	0x05, //glare_max_cnt_th_dmc 0000 0000
	0x00, //nr fa de cs gamma 0 0000
	0xff, //nr_mask_th
	0x00, //de_gain 10
	0x00,
	0x02, //de_maxplus 11
	0x00,
	0x02, //de_maxminus 11
	0x00,
	0x14, //fa_edge_th
	0x00, //fa_step_p 13
	0x01,
	0x00, //fa_step_n 13
	0x01,
	0x00, //fa_max_de_gain 13
	0x10,
	0x00, //fa_pcl_ppi 14
	0x00,
	0x28, //fa_os_cnt_10_co
	0x3c, //fa_os_cnt_20_co
	0x04, //fa_edge_cnt_weight
	0x0f, //fa_avg_y_weight
	0x01, //cs_gain 10
	0x00,
	0x00, //curve_0
	0x10, //curve_1
	0x20, //curve_2
	0x30, //curve_3
	0x40, //curve_4
	0x50, //curve_5
	0x60, //curve_6
	0x70, //curve_7
	0x80, //curve_8
	0x90, //curve_9
	0xa0, //curve_10
	0xb0, //curve_11
	0xc0, //curve_12
	0xd0, //curve_13
	0xe0, //curve_14
	0xf0, //curve_15
	0x01, //curve_16
	0x00,
	0x00, //curve_offset 10
	0x00,
	0x00, //curve_low_x
	0x00, //blank
	0x00, //curve_low_y
};

static char DSI_UI_AUTO_MDNIE_3[] = {
	0xDD,
	0x01, //mdnie_en
	0x00, //mask 00 0000
	0x03, //edge 00
	0xf0, //ascr algo aolce gamut 00 00 00 00
	0xff, //partial_en1
	0xff, //partial_en2
	0x00, //roi_en
	0x00, //roi_block
	0x00, //roi_sx
	0x00, //roi_sx
	0x00, //roi_sy
	0x00, //roi_sy
	0x00, //roi_ex
	0x00, //roi_ex
	0x00, //roi_ey
	0x00, //roi_ey
	0xff, //trans_on trans_block 0 000 0000
	0x04, //trans_slope
	0x01, //trans_interval
	//end
};

static char DSI_VIDEO_DYNAMIC_MDNIE_1[] = {
	//start
	0xDF,
	0x11, //linear_on ascr_skin_on 0000 0000
	0x67, //ascr_skin_cb
	0xa9, //ascr_skin_cr
	0x37, //ascr_dist_up
	0x29, //ascr_dist_down
	0x19, //ascr_dist_right
	0x47, //ascr_dist_left
	0x00, //ascr_div_up 20
	0x25,
	0x3d,
	0x00, //ascr_div_down
	0x31,
	0xf4,
	0x00, //ascr_div_right
	0x51,
	0xec,
	0x00, //ascr_div_left
	0x1c,
	0xd8,
	0xe1, //ascr_skin_Rr
	0x25, //ascr_skin_Rg
	0x00, //ascr_skin_Rb
	0xee, //ascr_skin_Yr
	0xea, //ascr_skin_Yg
	0x00, //ascr_skin_Yb
	0xf0, //ascr_skin_Mr
	0x33, //ascr_skin_Mg
	0xe8, //ascr_skin_Mb
	0xff, //ascr_skin_Wr
	0xfa, //ascr_skin_Wg
	0xf1, //ascr_skin_Wb
	0x5c, //ascr_Cr
	0xe1, //ascr_Rr
	0xef, //ascr_Cg
	0x25, //ascr_Rg
	0xe8, //ascr_Cb
	0x00, //ascr_Rb
	0xf0, //ascr_Mr
	0x49, //ascr_Gr
	0x33, //ascr_Mg
	0xdf, //ascr_Gg
	0xe8, //ascr_Mb
	0x00, //ascr_Gb
	0xee, //ascr_Yr
	0x37, //ascr_Br
	0xea, //ascr_Yg
	0x21, //ascr_Bg
	0x00, //ascr_Yb
	0xdf, //ascr_Bb
	0xff, //ascr_Wr
	0x00, //ascr_Kr
	0xfa, //ascr_Wg
	0x00, //ascr_Kg
	0xf1, //ascr_Wb
	0x00, //ascr_Kb
	0x0F, //edge_ctrl
	0x00, //edge_radius 12
	0x6F,
	0x05, //edge_aa_factor
	0x00, //edge_r
	0x00, //edge_g
	0x00, //edge_b
	0x00, //edge_x 11
	0x70,
	0x00, //edge_y 12
	0x70,
	0x44, //edge_coef_a edge_coef_b 0000 0000
};

static char DSI_VIDEO_DYNAMIC_MDNIE_2[] = {
	0xDE,
	0x00, //gamut_gamma_on gamut_blk_shift 0000 0000
	0x40, //gamut_scale
	0x04, //gamut_r1
	0x00,
	0x00, //gamut_r2
	0x00,
	0x00, //gamut_r3
	0x00,
	0x00, //gamut_g1
	0x00,
	0x04, //gamut_g2
	0x00,
	0x00, //gamut_g3
	0x00,
	0x00, //gamut_b1
	0x00,
	0x00, //gamut_b2
	0x00,
	0x04, //gamut_b3
	0x00,
	0x00, //slce_on cadrx_en 0000 0000
	0x00, //glare_on
	0x00, //lce_gain 000 0000
	0x24, //lce_color_gain 00 0000
	0x96, //lce_min_ref_offset
	0xb3, //lce_illum_gain
	0x01, //lce_ref_offset 9
	0x0e,
	0x01, //lce_ref_gain 9
	0x00,
	0x77, //lce_block_size h v 0000 0000
	0x03, //lce_dark_th 000
	0x07, //lce_reduct_slope 0000
	0x40, //lce_black cc0 cc1 color_th 0 0 0 0000
	0x08, //lce_large_w
	0x04, //lce_med_w
	0x04, //lce_small_w
	0x00, //cadrx_dit_en shadow_corr_en ref_ctrl_en 0 0 0
	0x00, //cadrx_gain
	0x0d, //cadrx_low_intensity_th
	0xe1, //cadrx_high_intensity_th
	0x07, //cadrx_ui_zerobincnt_th
	0x29, //cadrx_ui_ratio_th
	0x04,
	0x0f, //cadrx_entire_freq
	0xa0,
	0x00,
	0x03, //cadrx_peak_th_in_freq
	0x7a,
	0xa0,
	0x00, //cadrx_high_peak_th_in_freq
	0x25,
	0x1c,
	0x24, //cadrx_dit_color_gain
	0x00, //cadrx_dit_illum_a0
	0x48, //cadrx_dit_illum_a1
	0xea, //cadrx_dit_illum_a2
	0xfa, //cadrx_dit_illum_a3
	0x00, //cadrx_dit_illum_b0
	0x10,
	0x00, //cadrx_dit_illum_b1
	0x10,
	0x00, //cadrx_dit_illum_b2
	0x9c,
	0x01, //cadrx_dit_illum_b3
	0x00,
	0x00, //cadrx_dit_illum_slope1
	0x00,
	0x00, //cadrx_dit_illum_slope2
	0xdc,
	0x03, //cadrx_dit_illum_slope3
	0xff,
	0x40, //cadrx_ui_illum_a1
	0x80, //cadrx_ui_illum_a2
	0xc0, //cadrx_ui_illum_a3
	0x00, //cadrx_ui_illum_offset1
	0x9a,
	0x00, //cadrx_ui_illum_offset2
	0x9a,
	0x00, //cadrx_ui_illum_offset3
	0x9a,
	0x00, //cadrx_ui_illum_offset4
	0x9a,
	0x00, //cadrx_ui_illum_slope1
	0x00,
	0x00, //cadrx_ui_illum_slope2
	0x00,
	0x00, //cadrx_ui_illum_slope3
	0x00,
	0x00, //cadrx_ui_illum_slope4
	0x00,
	0x40, //cadrx_ui_ref_a1
	0x80, //cadrx_ui_ref_a2
	0xc0, //cadrx_ui_ref_a3
	0x01, //cadrx_ui_ref_offset1
	0x0e,
	0x01, //cadrx_ui_ref_offset2
	0x0e,
	0x01, //cadrx_ui_ref_offset3
	0x0e,
	0x01, //cadrx_ui_ref_offset4
	0x0e,
	0x00, //cadrx_ui_ref_slope1
	0x00,
	0x00, //cadrx_ui_ref_slope2
	0x00,
	0x00, //cadrx_ui_ref_slope3
	0x00,
	0x00, //cadrx_ui_ref_slope4
	0x00,
	0x01, //cadrx_reg_ref_c1_offset
	0x0e,
	0x01, //cadrx_reg_ref_c2_offset
	0x3a,
	0x01, //cadrx_reg_ref_c3_offset
	0x44,
	0x00, //cadrx_reg_ref_c1_slope
	0x01,
	0x00, //cadrx_reg_ref_c2_slope
	0x43,
	0x00, //cadrx_reg_ref_c3_slope
	0x4b,
	0x00, //cadrx_sc_reg_a1
	0x18,
	0x00, //cadrx_sc_reg_b1
	0x1c,
	0x01, //cadrx_sc_k1_int
	0xff,
	0x01, //cadrx_sc_k2_int
	0xff,
	0x03, //cadrx_sc_w1_int
	0xff,
	0xe8,
	0x03, //cadrx_sc_w2_int
	0xff,
	0xe8,
	0x00, //flicker_w_en
	0x14, //flicker_w
	0x01, //flicker_trans_en
	0x04, //flicker_trans_slope
	0x00, //bl_en
	0x03, //bl_hbs_th
	0x10, //bl_lbs_th
	0xC8, //bl_illum_gain
	0x01, //bl_reg_gain 9
	0x2C,
	0x00, //bl_le_diff
	0x00, //bl_cadrx_gain
	0x04, //bl_trans_slope
	0x00, //le_en
	0x40, //le_diff
	0x03, //le_lmax 10
	0xB6,
	0x23, //le_p
	0x24, //le_w_tmf
	0x00, //le_w_struct_diff 9
	0x72,
	0x01, //le_w_struct_diff_gain
	0x00, //le_w_luminance 9
	0x0D,
	0x00, //le_luminance_slope 10
	0x04,
	0x80, //glare_blk_avg_th 0000 0000
	0x00, //glare_luma_gain 9
	0x28,
	0xa0, //glare_luma_ctl_start 0000 0000
	0x01, //glare_luma_th 0000 0000
	0x00, //glare_luma_step 9
	0x01,
	0x00, //glare_uni_luma_gain 9
	0x01,
	0x00, //glare_blk_max_th 0000 0000
	0x00, //glare_blk_max_w 0000 0000
	0x10, //glare_blk_max_curve blk_max_sh 0 0000
	0x00, //glare_con_ext_max_th 0000 0000
	0x00, //glare_con_ext_max_w 0000 0000
	0x30, //glare_con_ext_max_sign con_ext_max_curve con_ext_max_sh 0 0 0000
	0x00, //glare_max_cnt_th 0000
	0x00, //glare_max_cnt_th 0000 0000
	0x00, //glare_max_cnt_w 0000 0000
	0x30, //glare_max_cnt_sign max_cnt_curve max_cnt_sh 0 0 0000
	0x00, //glare_y_avg_th 0000 0000
	0x00, //glare_y_avg_w 0000 0000
	0x30, //glare_y_avg_sign y_avg_curve y_avg_sh 0 0 0000
	0x00, //glare_max_cnt_th_dmc 0000
	0x00, //glare_max_cnt_th_dmc 0000 0000
	0x04, //nr fa de cs gamma 0 0000
	0xff, //nr_mask_th
	0x00, //de_gain 10
	0x10,
	0x00, //de_maxplus 11
	0x40,
	0x00, //de_maxminus 11
	0x40,
	0x14, //fa_edge_th
	0x00, //fa_step_p 13
	0x01,
	0x00, //fa_step_n 13
	0x01,
	0x00, //fa_max_de_gain 13
	0x10,
	0x00, //fa_pcl_ppi 14
	0x00,
	0x28, //fa_os_cnt_10_co
	0x3c, //fa_os_cnt_20_co
	0x04, //fa_edge_cnt_weight
	0x0f, //fa_avg_y_weight
	0x01, //cs_gain 10
	0x00,
	0x00, //curve_0
	0x10, //curve_1
	0x20, //curve_2
	0x30, //curve_3
	0x40, //curve_4
	0x50, //curve_5
	0x60, //curve_6
	0x70, //curve_7
	0x80, //curve_8
	0x90, //curve_9
	0xa0, //curve_10
	0xb0, //curve_11
	0xc0, //curve_12
	0xd0, //curve_13
	0xe0, //curve_14
	0xf0, //curve_15
	0x01, //curve_16
	0x00,
	0x00, //curve_offset 10
	0x00,
	0x00, //curve_low_x
	0x00, //blank
	0x00, //curve_low_y
};

static char DSI_VIDEO_DYNAMIC_MDNIE_3[] = {
	0xDD,
	0x01, //mdnie_en
	0x00, //mask 00 0000
	0x03, //edge 00
	0xf0, //ascr algo aolce gamut 00 00 00 00
	0xff, //partial_en1
	0xff, //partial_en2
	0x00, //roi_en
	0x00, //roi_block
	0x00, //roi_sx
	0x00, //roi_sx
	0x00, //roi_sy
	0x00, //roi_sy
	0x00, //roi_ex
	0x00, //roi_ex
	0x00, //roi_ey
	0x00, //roi_ey
	0xff, //trans_on trans_block 0 000 0000
	0x04, //trans_slope
	0x01, //trans_interval
	//end
};

static char DSI_VIDEO_STANDARD_MDNIE_1[] = {
	//start
	0xDF,
	0x11, //linear_on ascr_skin_on 0000 0000
	0x6a, //ascr_skin_cb
	0x9a, //ascr_skin_cr
	0x25, //ascr_dist_up
	0x1a, //ascr_dist_down
	0x16, //ascr_dist_right
	0x2a, //ascr_dist_left
	0x00, //ascr_div_up 20
	0x37,
	0x5a,
	0x00, //ascr_div_down
	0x4e,
	0xc5,
	0x00, //ascr_div_right
	0x5d,
	0x17,
	0x00, //ascr_div_left
	0x30,
	0xc3,
	0xee, //ascr_skin_Rr
	0x4b, //ascr_skin_Rg
	0x30, //ascr_skin_Rb
	0xef, //ascr_skin_Yr
	0xeb, //ascr_skin_Yg
	0x47, //ascr_skin_Yb
	0xfc, //ascr_skin_Mr
	0x57, //ascr_skin_Mg
	0xe7, //ascr_skin_Mb
	0xff, //ascr_skin_Wr
	0xfa, //ascr_skin_Wg
	0xf1, //ascr_skin_Wb
	0x00, //ascr_Cr
	0xee, //ascr_Rr
	0xe7, //ascr_Cg
	0x4b, //ascr_Rg
	0xe5, //ascr_Cb
	0x30, //ascr_Rb
	0xfc, //ascr_Mr
	0x00, //ascr_Gr
	0x57, //ascr_Mg
	0xd9, //ascr_Gg
	0xe7, //ascr_Mb
	0x34, //ascr_Gb
	0xef, //ascr_Yr
	0x34, //ascr_Br
	0xeb, //ascr_Yg
	0x20, //ascr_Bg
	0x47, //ascr_Yb
	0xdb, //ascr_Bb
	0xff, //ascr_Wr
	0x00, //ascr_Kr
	0xfa, //ascr_Wg
	0x00, //ascr_Kg
	0xf1, //ascr_Wb
	0x00, //ascr_Kb
	0x0F, //edge_ctrl
	0x00, //edge_radius 12
	0x6F,
	0x05, //edge_aa_factor
	0x00, //edge_r
	0x00, //edge_g
	0x00, //edge_b
	0x00, //edge_x 11
	0x70,
	0x00, //edge_y 12
	0x70,
	0x44, //edge_coef_a edge_coef_b 0000 0000
};

static char DSI_VIDEO_STANDARD_MDNIE_2[] = {
	0xDE,
	0x00, //gamut_gamma_on gamut_blk_shift 0000 0000
	0x40, //gamut_scale
	0x04, //gamut_r1
	0x00,
	0x00, //gamut_r2
	0x00,
	0x00, //gamut_r3
	0x00,
	0x00, //gamut_g1
	0x00,
	0x04, //gamut_g2
	0x00,
	0x00, //gamut_g3
	0x00,
	0x00, //gamut_b1
	0x00,
	0x00, //gamut_b2
	0x00,
	0x04, //gamut_b3
	0x00,
	0x00, //slce_on cadrx_en 0000 0000
	0x00, //glare_on
	0x00, //lce_gain 000 0000
	0x24, //lce_color_gain 00 0000
	0x96, //lce_min_ref_offset
	0xb3, //lce_illum_gain
	0x01, //lce_ref_offset 9
	0x0e,
	0x01, //lce_ref_gain 9
	0x00,
	0x77, //lce_block_size h v 0000 0000
	0x03, //lce_dark_th 000
	0x07, //lce_reduct_slope 0000
	0x40, //lce_black cc0 cc1 color_th 0 0 0 0000
	0x08, //lce_large_w
	0x04, //lce_med_w
	0x04, //lce_small_w
	0x00, //cadrx_dit_en shadow_corr_en ref_ctrl_en 0 0 0
	0x00, //cadrx_gain
	0x0d, //cadrx_low_intensity_th
	0xe1, //cadrx_high_intensity_th
	0x07, //cadrx_ui_zerobincnt_th
	0x29, //cadrx_ui_ratio_th
	0x04,
	0x0f, //cadrx_entire_freq
	0xa0,
	0x00,
	0x03, //cadrx_peak_th_in_freq
	0x7a,
	0xa0,
	0x00, //cadrx_high_peak_th_in_freq
	0x25,
	0x1c,
	0x24, //cadrx_dit_color_gain
	0x00, //cadrx_dit_illum_a0
	0x48, //cadrx_dit_illum_a1
	0xea, //cadrx_dit_illum_a2
	0xfa, //cadrx_dit_illum_a3
	0x00, //cadrx_dit_illum_b0
	0x10,
	0x00, //cadrx_dit_illum_b1
	0x10,
	0x00, //cadrx_dit_illum_b2
	0x9c,
	0x01, //cadrx_dit_illum_b3
	0x00,
	0x00, //cadrx_dit_illum_slope1
	0x00,
	0x00, //cadrx_dit_illum_slope2
	0xdc,
	0x03, //cadrx_dit_illum_slope3
	0xff,
	0x40, //cadrx_ui_illum_a1
	0x80, //cadrx_ui_illum_a2
	0xc0, //cadrx_ui_illum_a3
	0x00, //cadrx_ui_illum_offset1
	0x9a,
	0x00, //cadrx_ui_illum_offset2
	0x9a,
	0x00, //cadrx_ui_illum_offset3
	0x9a,
	0x00, //cadrx_ui_illum_offset4
	0x9a,
	0x00, //cadrx_ui_illum_slope1
	0x00,
	0x00, //cadrx_ui_illum_slope2
	0x00,
	0x00, //cadrx_ui_illum_slope3
	0x00,
	0x00, //cadrx_ui_illum_slope4
	0x00,
	0x40, //cadrx_ui_ref_a1
	0x80, //cadrx_ui_ref_a2
	0xc0, //cadrx_ui_ref_a3
	0x01, //cadrx_ui_ref_offset1
	0x0e,
	0x01, //cadrx_ui_ref_offset2
	0x0e,
	0x01, //cadrx_ui_ref_offset3
	0x0e,
	0x01, //cadrx_ui_ref_offset4
	0x0e,
	0x00, //cadrx_ui_ref_slope1
	0x00,
	0x00, //cadrx_ui_ref_slope2
	0x00,
	0x00, //cadrx_ui_ref_slope3
	0x00,
	0x00, //cadrx_ui_ref_slope4
	0x00,
	0x01, //cadrx_reg_ref_c1_offset
	0x0e,
	0x01, //cadrx_reg_ref_c2_offset
	0x3a,
	0x01, //cadrx_reg_ref_c3_offset
	0x44,
	0x00, //cadrx_reg_ref_c1_slope
	0x01,
	0x00, //cadrx_reg_ref_c2_slope
	0x43,
	0x00, //cadrx_reg_ref_c3_slope
	0x4b,
	0x00, //cadrx_sc_reg_a1
	0x18,
	0x00, //cadrx_sc_reg_b1
	0x1c,
	0x01, //cadrx_sc_k1_int
	0xff,
	0x01, //cadrx_sc_k2_int
	0xff,
	0x03, //cadrx_sc_w1_int
	0xff,
	0xe8,
	0x03, //cadrx_sc_w2_int
	0xff,
	0xe8,
	0x00, //flicker_w_en
	0x14, //flicker_w
	0x01, //flicker_trans_en
	0x04, //flicker_trans_slope
	0x00, //bl_en
	0x03, //bl_hbs_th
	0x10, //bl_lbs_th
	0xC8, //bl_illum_gain
	0x01, //bl_reg_gain 9
	0x2C,
	0x00, //bl_le_diff
	0x00, //bl_cadrx_gain
	0x04, //bl_trans_slope
	0x00, //le_en
	0x40, //le_diff
	0x03, //le_lmax 10
	0xB6,
	0x23, //le_p
	0x24, //le_w_tmf
	0x00, //le_w_struct_diff 9
	0x72,
	0x01, //le_w_struct_diff_gain
	0x00, //le_w_luminance 9
	0x0D,
	0x00, //le_luminance_slope 10
	0x04,
	0x80, //glare_blk_avg_th 0000 0000
	0x00, //glare_luma_gain 9
	0x28,
	0xa0, //glare_luma_ctl_start 0000 0000
	0x01, //glare_luma_th 0000 0000
	0x00, //glare_luma_step 9
	0x01,
	0x00, //glare_uni_luma_gain 9
	0x01,
	0x00, //glare_blk_max_th 0000 0000
	0x00, //glare_blk_max_w 0000 0000
	0x10, //glare_blk_max_curve blk_max_sh 0 0000
	0x00, //glare_con_ext_max_th 0000 0000
	0x00, //glare_con_ext_max_w 0000 0000
	0x30, //glare_con_ext_max_sign con_ext_max_curve con_ext_max_sh 0 0 0000
	0x00, //glare_max_cnt_th 0000
	0x00, //glare_max_cnt_th 0000 0000
	0x00, //glare_max_cnt_w 0000 0000
	0x30, //glare_max_cnt_sign max_cnt_curve max_cnt_sh 0 0 0000
	0x00, //glare_y_avg_th 0000 0000
	0x00, //glare_y_avg_w 0000 0000
	0x30, //glare_y_avg_sign y_avg_curve y_avg_sh 0 0 0000
	0x00, //glare_max_cnt_th_dmc 0000
	0x00, //glare_max_cnt_th_dmc 0000 0000
	0x06, //nr fa de cs gamma 0 0000
	0xff, //nr_mask_th
	0x00, //de_gain 10
	0x10,
	0x00, //de_maxplus 11
	0x40,
	0x00, //de_maxminus 11
	0x40,
	0x14, //fa_edge_th
	0x00, //fa_step_p 13
	0x01,
	0x00, //fa_step_n 13
	0x01,
	0x00, //fa_max_de_gain 13
	0x10,
	0x00, //fa_pcl_ppi 14
	0x00,
	0x28, //fa_os_cnt_10_co
	0x3c, //fa_os_cnt_20_co
	0x04, //fa_edge_cnt_weight
	0x0f, //fa_avg_y_weight
	0x01, //cs_gain 10
	0x18,
	0x00, //curve_0
	0x10, //curve_1
	0x20, //curve_2
	0x30, //curve_3
	0x40, //curve_4
	0x50, //curve_5
	0x60, //curve_6
	0x70, //curve_7
	0x80, //curve_8
	0x90, //curve_9
	0xa0, //curve_10
	0xb0, //curve_11
	0xc0, //curve_12
	0xd0, //curve_13
	0xe0, //curve_14
	0xf0, //curve_15
	0x01, //curve_16
	0x00,
	0x00, //curve_offset 10
	0x00,
	0x00, //curve_low_x
	0x00, //blank
	0x00, //curve_low_y
};

static char DSI_VIDEO_STANDARD_MDNIE_3[] = {
	0xDD,
	0x01, //mdnie_en
	0x00, //mask 00 0000
	0x03, //edge 00
	0xf0, //ascr algo aolce gamut 00 00 00 00
	0xff, //partial_en1
	0xff, //partial_en2
	0x00, //roi_en
	0x00, //roi_block
	0x00, //roi_sx
	0x00, //roi_sx
	0x00, //roi_sy
	0x00, //roi_sy
	0x00, //roi_ex
	0x00, //roi_ex
	0x00, //roi_ey
	0x00, //roi_ey
	0xff, //trans_on trans_block 0 000 0000
	0x04, //trans_slope
	0x01, //trans_interval
	//end
};

static char DSI_VIDEO_NATURAL_MDNIE_1[] = {
	//start
	0xDF,
	0x11, //linear_on ascr_skin_on 0000 0000
	0x6a, //ascr_skin_cb
	0x9a, //ascr_skin_cr
	0x25, //ascr_dist_up
	0x1a, //ascr_dist_down
	0x16, //ascr_dist_right
	0x2a, //ascr_dist_left
	0x00, //ascr_div_up 20
	0x37,
	0x5a,
	0x00, //ascr_div_down
	0x4e,
	0xc5,
	0x00, //ascr_div_right
	0x5d,
	0x17,
	0x00, //ascr_div_left
	0x30,
	0xc3,
	0xd3, //ascr_skin_Rr
	0x40, //ascr_skin_Rg
	0x29, //ascr_skin_Rb
	0xf0, //ascr_skin_Yr
	0xec, //ascr_skin_Yg
	0x5b, //ascr_skin_Yb
	0xdf, //ascr_skin_Mr
	0x4a, //ascr_skin_Mg
	0xdf, //ascr_skin_Mb
	0xff, //ascr_skin_Wr
	0xfa, //ascr_skin_Wg
	0xf1, //ascr_skin_Wb
	0x93, //ascr_Cr
	0xd3, //ascr_Rr
	0xed, //ascr_Cg
	0x40, //ascr_Rg
	0xe7, //ascr_Cb
	0x29, //ascr_Rb
	0xdf, //ascr_Mr
	0x85, //ascr_Gr
	0x4a, //ascr_Mg
	0xe0, //ascr_Gg
	0xdf, //ascr_Mb
	0x4f, //ascr_Gb
	0xf0, //ascr_Yr
	0x32, //ascr_Br
	0xec, //ascr_Yg
	0x1f, //ascr_Bg
	0x5b, //ascr_Yb
	0xd8, //ascr_Bb
	0xff, //ascr_Wr
	0x00, //ascr_Kr
	0xfa, //ascr_Wg
	0x00, //ascr_Kg
	0xf1, //ascr_Wb
	0x00, //ascr_Kb
	0x0F, //edge_ctrl
	0x00, //edge_radius 12
	0x6F,
	0x05, //edge_aa_factor
	0x00, //edge_r
	0x00, //edge_g
	0x00, //edge_b
	0x00, //edge_x 11
	0x70,
	0x00, //edge_y 12
	0x70,
	0x44, //edge_coef_a edge_coef_b 0000 0000
};

static char DSI_VIDEO_NATURAL_MDNIE_2[] = {
	0xDE,
	0x00, //gamut_gamma_on gamut_blk_shift 0000 0000
	0x40, //gamut_scale
	0x04, //gamut_r1
	0x00,
	0x00, //gamut_r2
	0x00,
	0x00, //gamut_r3
	0x00,
	0x00, //gamut_g1
	0x00,
	0x04, //gamut_g2
	0x00,
	0x00, //gamut_g3
	0x00,
	0x00, //gamut_b1
	0x00,
	0x00, //gamut_b2
	0x00,
	0x04, //gamut_b3
	0x00,
	0x00, //slce_on cadrx_en 0000 0000
	0x00, //glare_on
	0x00, //lce_gain 000 0000
	0x24, //lce_color_gain 00 0000
	0x96, //lce_min_ref_offset
	0xb3, //lce_illum_gain
	0x01, //lce_ref_offset 9
	0x0e,
	0x01, //lce_ref_gain 9
	0x00,
	0x77, //lce_block_size h v 0000 0000
	0x03, //lce_dark_th 000
	0x07, //lce_reduct_slope 0000
	0x40, //lce_black cc0 cc1 color_th 0 0 0 0000
	0x08, //lce_large_w
	0x04, //lce_med_w
	0x04, //lce_small_w
	0x00, //cadrx_dit_en shadow_corr_en ref_ctrl_en 0 0 0
	0x00, //cadrx_gain
	0x0d, //cadrx_low_intensity_th
	0xe1, //cadrx_high_intensity_th
	0x07, //cadrx_ui_zerobincnt_th
	0x29, //cadrx_ui_ratio_th
	0x04,
	0x0f, //cadrx_entire_freq
	0xa0,
	0x00,
	0x03, //cadrx_peak_th_in_freq
	0x7a,
	0xa0,
	0x00, //cadrx_high_peak_th_in_freq
	0x25,
	0x1c,
	0x24, //cadrx_dit_color_gain
	0x00, //cadrx_dit_illum_a0
	0x48, //cadrx_dit_illum_a1
	0xea, //cadrx_dit_illum_a2
	0xfa, //cadrx_dit_illum_a3
	0x00, //cadrx_dit_illum_b0
	0x10,
	0x00, //cadrx_dit_illum_b1
	0x10,
	0x00, //cadrx_dit_illum_b2
	0x9c,
	0x01, //cadrx_dit_illum_b3
	0x00,
	0x00, //cadrx_dit_illum_slope1
	0x00,
	0x00, //cadrx_dit_illum_slope2
	0xdc,
	0x03, //cadrx_dit_illum_slope3
	0xff,
	0x40, //cadrx_ui_illum_a1
	0x80, //cadrx_ui_illum_a2
	0xc0, //cadrx_ui_illum_a3
	0x00, //cadrx_ui_illum_offset1
	0x9a,
	0x00, //cadrx_ui_illum_offset2
	0x9a,
	0x00, //cadrx_ui_illum_offset3
	0x9a,
	0x00, //cadrx_ui_illum_offset4
	0x9a,
	0x00, //cadrx_ui_illum_slope1
	0x00,
	0x00, //cadrx_ui_illum_slope2
	0x00,
	0x00, //cadrx_ui_illum_slope3
	0x00,
	0x00, //cadrx_ui_illum_slope4
	0x00,
	0x40, //cadrx_ui_ref_a1
	0x80, //cadrx_ui_ref_a2
	0xc0, //cadrx_ui_ref_a3
	0x01, //cadrx_ui_ref_offset1
	0x0e,
	0x01, //cadrx_ui_ref_offset2
	0x0e,
	0x01, //cadrx_ui_ref_offset3
	0x0e,
	0x01, //cadrx_ui_ref_offset4
	0x0e,
	0x00, //cadrx_ui_ref_slope1
	0x00,
	0x00, //cadrx_ui_ref_slope2
	0x00,
	0x00, //cadrx_ui_ref_slope3
	0x00,
	0x00, //cadrx_ui_ref_slope4
	0x00,
	0x01, //cadrx_reg_ref_c1_offset
	0x0e,
	0x01, //cadrx_reg_ref_c2_offset
	0x3a,
	0x01, //cadrx_reg_ref_c3_offset
	0x44,
	0x00, //cadrx_reg_ref_c1_slope
	0x01,
	0x00, //cadrx_reg_ref_c2_slope
	0x43,
	0x00, //cadrx_reg_ref_c3_slope
	0x4b,
	0x00, //cadrx_sc_reg_a1
	0x18,
	0x00, //cadrx_sc_reg_b1
	0x1c,
	0x01, //cadrx_sc_k1_int
	0xff,
	0x01, //cadrx_sc_k2_int
	0xff,
	0x03, //cadrx_sc_w1_int
	0xff,
	0xe8,
	0x03, //cadrx_sc_w2_int
	0xff,
	0xe8,
	0x00, //flicker_w_en
	0x14, //flicker_w
	0x01, //flicker_trans_en
	0x04, //flicker_trans_slope
	0x00, //bl_en
	0x03, //bl_hbs_th
	0x10, //bl_lbs_th
	0xC8, //bl_illum_gain
	0x01, //bl_reg_gain 9
	0x2C,
	0x00, //bl_le_diff
	0x00, //bl_cadrx_gain
	0x04, //bl_trans_slope
	0x00, //le_en
	0x40, //le_diff
	0x03, //le_lmax 10
	0xB6,
	0x23, //le_p
	0x24, //le_w_tmf
	0x00, //le_w_struct_diff 9
	0x72,
	0x01, //le_w_struct_diff_gain
	0x00, //le_w_luminance 9
	0x0D,
	0x00, //le_luminance_slope 10
	0x04,
	0x80, //glare_blk_avg_th 0000 0000
	0x00, //glare_luma_gain 9
	0x28,
	0xa0, //glare_luma_ctl_start 0000 0000
	0x01, //glare_luma_th 0000 0000
	0x00, //glare_luma_step 9
	0x01,
	0x00, //glare_uni_luma_gain 9
	0x01,
	0x00, //glare_blk_max_th 0000 0000
	0x00, //glare_blk_max_w 0000 0000
	0x10, //glare_blk_max_curve blk_max_sh 0 0000
	0x00, //glare_con_ext_max_th 0000 0000
	0x00, //glare_con_ext_max_w 0000 0000
	0x30, //glare_con_ext_max_sign con_ext_max_curve con_ext_max_sh 0 0 0000
	0x00, //glare_max_cnt_th 0000
	0x00, //glare_max_cnt_th 0000 0000
	0x00, //glare_max_cnt_w 0000 0000
	0x30, //glare_max_cnt_sign max_cnt_curve max_cnt_sh 0 0 0000
	0x00, //glare_y_avg_th 0000 0000
	0x00, //glare_y_avg_w 0000 0000
	0x30, //glare_y_avg_sign y_avg_curve y_avg_sh 0 0 0000
	0x00, //glare_max_cnt_th_dmc 0000
	0x00, //glare_max_cnt_th_dmc 0000 0000
	0x04, //nr fa de cs gamma 0 0000
	0xff, //nr_mask_th
	0x00, //de_gain 10
	0x10,
	0x00, //de_maxplus 11
	0x40,
	0x00, //de_maxminus 11
	0x40,
	0x14, //fa_edge_th
	0x00, //fa_step_p 13
	0x01,
	0x00, //fa_step_n 13
	0x01,
	0x00, //fa_max_de_gain 13
	0x10,
	0x00, //fa_pcl_ppi 14
	0x00,
	0x28, //fa_os_cnt_10_co
	0x3c, //fa_os_cnt_20_co
	0x04, //fa_edge_cnt_weight
	0x0f, //fa_avg_y_weight
	0x01, //cs_gain 10
	0x00,
	0x00, //curve_0
	0x10, //curve_1
	0x20, //curve_2
	0x30, //curve_3
	0x40, //curve_4
	0x50, //curve_5
	0x60, //curve_6
	0x70, //curve_7
	0x80, //curve_8
	0x90, //curve_9
	0xa0, //curve_10
	0xb0, //curve_11
	0xc0, //curve_12
	0xd0, //curve_13
	0xe0, //curve_14
	0xf0, //curve_15
	0x01, //curve_16
	0x00,
	0x00, //curve_offset 10
	0x00,
	0x00, //curve_low_x
	0x00, //blank
	0x00, //curve_low_y
};

static char DSI_VIDEO_NATURAL_MDNIE_3[] = {
	0xDD,
	0x01, //mdnie_en
	0x00, //mask 00 0000
	0x03, //edge 00
	0xf0, //ascr algo aolce gamut 00 00 00 00
	0xff, //partial_en1
	0xff, //partial_en2
	0x00, //roi_en
	0x00, //roi_block
	0x00, //roi_sx
	0x00, //roi_sx
	0x00, //roi_sy
	0x00, //roi_sy
	0x00, //roi_ex
	0x00, //roi_ex
	0x00, //roi_ey
	0x00, //roi_ey
	0xff, //trans_on trans_block 0 000 0000
	0x04, //trans_slope
	0x01, //trans_interval
	//end
};

static char DSI_VIDEO_AUTO_MDNIE_1[] = {
	//start
	0xDF,
	0x01, //linear_on ascr_skin_on 0000 0000
	0x67, //ascr_skin_cb
	0xa9, //ascr_skin_cr
	0x37, //ascr_dist_up
	0x29, //ascr_dist_down
	0x19, //ascr_dist_right
	0x47, //ascr_dist_left
	0x00, //ascr_div_up 20
	0x25,
	0x3d,
	0x00, //ascr_div_down
	0x31,
	0xf4,
	0x00, //ascr_div_right
	0x51,
	0xec,
	0x00, //ascr_div_left
	0x1c,
	0xd8,
	0xff, //ascr_skin_Rr
	0x64, //ascr_skin_Rg
	0x70, //ascr_skin_Rb
	0xff, //ascr_skin_Yr
	0xff, //ascr_skin_Yg
	0x00, //ascr_skin_Yb
	0xff, //ascr_skin_Mr
	0x00, //ascr_skin_Mg
	0xff, //ascr_skin_Mb
	0xff, //ascr_skin_Wr
	0xfd, //ascr_skin_Wg
	0xff, //ascr_skin_Wb
	0x00, //ascr_Cr
	0xff, //ascr_Rr
	0xff, //ascr_Cg
	0x00, //ascr_Rg
	0xff, //ascr_Cb
	0x00, //ascr_Rb
	0xff, //ascr_Mr
	0x00, //ascr_Gr
	0x00, //ascr_Mg
	0xff, //ascr_Gg
	0xff, //ascr_Mb
	0x00, //ascr_Gb
	0xff, //ascr_Yr
	0x00, //ascr_Br
	0xff, //ascr_Yg
	0x00, //ascr_Bg
	0x00, //ascr_Yb
	0xff, //ascr_Bb
	0xff, //ascr_Wr
	0x00, //ascr_Kr
	0xff, //ascr_Wg
	0x00, //ascr_Kg
	0xff, //ascr_Wb
	0x00, //ascr_Kb
	0x0F, //edge_ctrl
	0x00, //edge_radius 12
	0x6F,
	0x05, //edge_aa_factor
	0x00, //edge_r
	0x00, //edge_g
	0x00, //edge_b
	0x00, //edge_x 11
	0x70,
	0x00, //edge_y 12
	0x70,
	0x44, //edge_coef_a edge_coef_b 0000 0000
};

static char DSI_VIDEO_AUTO_MDNIE_2[] = {
	0xDE,
	0x00, //gamut_gamma_on gamut_blk_shift 0000 0000
	0x40, //gamut_scale
	0x04, //gamut_r1
	0x00,
	0x00, //gamut_r2
	0x00,
	0x00, //gamut_r3
	0x00,
	0x00, //gamut_g1
	0x00,
	0x04, //gamut_g2
	0x00,
	0x00, //gamut_g3
	0x00,
	0x00, //gamut_b1
	0x00,
	0x00, //gamut_b2
	0x00,
	0x04, //gamut_b3
	0x00,
	0x00, //slce_on cadrx_en 0000 0000
	0x00, //glare_on
	0x00, //lce_gain 000 0000
	0x24, //lce_color_gain 00 0000
	0x96, //lce_min_ref_offset
	0xb3, //lce_illum_gain
	0x01, //lce_ref_offset 9
	0x0e,
	0x01, //lce_ref_gain 9
	0x00,
	0x77, //lce_block_size h v 0000 0000
	0x03, //lce_dark_th 000
	0x07, //lce_reduct_slope 0000
	0x40, //lce_black cc0 cc1 color_th 0 0 0 0000
	0x08, //lce_large_w
	0x04, //lce_med_w
	0x04, //lce_small_w
	0x00, //cadrx_dit_en shadow_corr_en ref_ctrl_en 0 0 0
	0x00, //cadrx_gain
	0x0d, //cadrx_low_intensity_th
	0xe1, //cadrx_high_intensity_th
	0x07, //cadrx_ui_zerobincnt_th
	0x29, //cadrx_ui_ratio_th
	0x04,
	0x0f, //cadrx_entire_freq
	0xa0,
	0x00,
	0x03, //cadrx_peak_th_in_freq
	0x7a,
	0xa0,
	0x00, //cadrx_high_peak_th_in_freq
	0x25,
	0x1c,
	0x24, //cadrx_dit_color_gain
	0x00, //cadrx_dit_illum_a0
	0x48, //cadrx_dit_illum_a1
	0xea, //cadrx_dit_illum_a2
	0xfa, //cadrx_dit_illum_a3
	0x00, //cadrx_dit_illum_b0
	0x10,
	0x00, //cadrx_dit_illum_b1
	0x10,
	0x00, //cadrx_dit_illum_b2
	0x9c,
	0x01, //cadrx_dit_illum_b3
	0x00,
	0x00, //cadrx_dit_illum_slope1
	0x00,
	0x00, //cadrx_dit_illum_slope2
	0xdc,
	0x03, //cadrx_dit_illum_slope3
	0xff,
	0x40, //cadrx_ui_illum_a1
	0x80, //cadrx_ui_illum_a2
	0xc0, //cadrx_ui_illum_a3
	0x00, //cadrx_ui_illum_offset1
	0x9a,
	0x00, //cadrx_ui_illum_offset2
	0x9a,
	0x00, //cadrx_ui_illum_offset3
	0x9a,
	0x00, //cadrx_ui_illum_offset4
	0x9a,
	0x00, //cadrx_ui_illum_slope1
	0x00,
	0x00, //cadrx_ui_illum_slope2
	0x00,
	0x00, //cadrx_ui_illum_slope3
	0x00,
	0x00, //cadrx_ui_illum_slope4
	0x00,
	0x40, //cadrx_ui_ref_a1
	0x80, //cadrx_ui_ref_a2
	0xc0, //cadrx_ui_ref_a3
	0x01, //cadrx_ui_ref_offset1
	0x0e,
	0x01, //cadrx_ui_ref_offset2
	0x0e,
	0x01, //cadrx_ui_ref_offset3
	0x0e,
	0x01, //cadrx_ui_ref_offset4
	0x0e,
	0x00, //cadrx_ui_ref_slope1
	0x00,
	0x00, //cadrx_ui_ref_slope2
	0x00,
	0x00, //cadrx_ui_ref_slope3
	0x00,
	0x00, //cadrx_ui_ref_slope4
	0x00,
	0x01, //cadrx_reg_ref_c1_offset
	0x0e,
	0x01, //cadrx_reg_ref_c2_offset
	0x3a,
	0x01, //cadrx_reg_ref_c3_offset
	0x44,
	0x00, //cadrx_reg_ref_c1_slope
	0x01,
	0x00, //cadrx_reg_ref_c2_slope
	0x43,
	0x00, //cadrx_reg_ref_c3_slope
	0x4b,
	0x00, //cadrx_sc_reg_a1
	0x18,
	0x00, //cadrx_sc_reg_b1
	0x1c,
	0x01, //cadrx_sc_k1_int
	0xff,
	0x01, //cadrx_sc_k2_int
	0xff,
	0x03, //cadrx_sc_w1_int
	0xff,
	0xe8,
	0x03, //cadrx_sc_w2_int
	0xff,
	0xe8,
	0x00, //flicker_w_en
	0x14, //flicker_w
	0x01, //flicker_trans_en
	0x04, //flicker_trans_slope
	0x00, //bl_en
	0x03, //bl_hbs_th
	0x10, //bl_lbs_th
	0xC8, //bl_illum_gain
	0x01, //bl_reg_gain 9
	0x2C,
	0x00, //bl_le_diff
	0x00, //bl_cadrx_gain
	0x04, //bl_trans_slope
	0x00, //le_en
	0x40, //le_diff
	0x03, //le_lmax 10
	0xB6,
	0x23, //le_p
	0x24, //le_w_tmf
	0x00, //le_w_struct_diff 9
	0x72,
	0x01, //le_w_struct_diff_gain
	0x00, //le_w_luminance 9
	0x0D,
	0x00, //le_luminance_slope 10
	0x04,
	0x80, //glare_blk_avg_th 0000 0000
	0x00, //glare_luma_gain 9
	0x28,
	0xa0, //glare_luma_ctl_start 0000 0000
	0x01, //glare_luma_th 0000 0000
	0x00, //glare_luma_step 9
	0x01,
	0x00, //glare_uni_luma_gain 9
	0x01,
	0xf0, //glare_blk_max_th 0000 0000
	0x00, //glare_blk_max_w 0000 0000
	0x18, //glare_blk_max_curve blk_max_sh 0 0000
	0xf0, //glare_con_ext_max_th 0000 0000
	0x00, //glare_con_ext_max_w 0000 0000
	0x38, //glare_con_ext_max_sign con_ext_max_curve con_ext_max_sh 0 0 0000
	0x00, //glare_max_cnt_th 0000
	0x00, //glare_max_cnt_th 0000 0000
	0x00, //glare_max_cnt_w 0000 0000
	0x38, //glare_max_cnt_sign max_cnt_curve max_cnt_sh 0 0 0000
	0xba, //glare_y_avg_th 0000 0000
	0x00, //glare_y_avg_w 0000 0000
	0x38, //glare_y_avg_sign y_avg_curve y_avg_sh 0 0 0000
	0x00, //glare_max_cnt_th_dmc 0000
	0x05, //glare_max_cnt_th_dmc 0000 0000
	0x0f, //nr fa de cs gamma 0 0000
	0xff, //nr_mask_th
	0x00, //de_gain 10
	0x10,
	0x00, //de_maxplus 11
	0x40,
	0x00, //de_maxminus 11
	0x40,
	0x14, //fa_edge_th
	0x00, //fa_step_p 13
	0x01,
	0x00, //fa_step_n 13
	0x01,
	0x00, //fa_max_de_gain 13
	0x10,
	0x00, //fa_pcl_ppi 14
	0x00,
	0x28, //fa_os_cnt_10_co
	0x3c, //fa_os_cnt_20_co
	0x04, //fa_edge_cnt_weight
	0x0f, //fa_avg_y_weight
	0x01, //cs_gain 10
	0x20,
	0x00, //curve_0
	0x0a, //curve_1
	0x17, //curve_2
	0x26, //curve_3
	0x36, //curve_4
	0x49, //curve_5
	0x5c, //curve_6
	0x6f, //curve_7
	0x82, //curve_8
	0x95, //curve_9
	0xa8, //curve_10
	0xbb, //curve_11
	0xcb, //curve_12
	0xdb, //curve_13
	0xeb, //curve_14
	0xf8, //curve_15
	0x01, //curve_16
	0x00,
	0x00, //curve_offset 10
	0x00,
	0x00, //curve_low_x
	0x00, //blank
	0x00, //curve_low_y
};

static char DSI_VIDEO_AUTO_MDNIE_3[] = {
	0xDD,
	0x01, //mdnie_en
	0x00, //mask 00 0000
	0x03, //edge 00
	0xf0, //ascr algo aolce gamut 00 00 00 00
	0xff, //partial_en1
	0xff, //partial_en2
	0x00, //roi_en
	0x00, //roi_block
	0x00, //roi_sx
	0x00, //roi_sx
	0x00, //roi_sy
	0x00, //roi_sy
	0x00, //roi_ex
	0x00, //roi_ex
	0x00, //roi_ey
	0x00, //roi_ey
	0xff, //trans_on trans_block 0 000 0000
	0x04, //trans_slope
	0x01, //trans_interval
	//end
};

static char DSI_CAMERA_DYNAMIC_MDNIE_1[] = {
	//start
	0xDF,
	0x11, //linear_on ascr_skin_on 0000 0000
	0x67, //ascr_skin_cb
	0xa9, //ascr_skin_cr
	0x37, //ascr_dist_up
	0x29, //ascr_dist_down
	0x19, //ascr_dist_right
	0x47, //ascr_dist_left
	0x00, //ascr_div_up 20
	0x25,
	0x3d,
	0x00, //ascr_div_down
	0x31,
	0xf4,
	0x00, //ascr_div_right
	0x51,
	0xec,
	0x00, //ascr_div_left
	0x1c,
	0xd8,
	0xe1, //ascr_skin_Rr
	0x25, //ascr_skin_Rg
	0x00, //ascr_skin_Rb
	0xee, //ascr_skin_Yr
	0xea, //ascr_skin_Yg
	0x00, //ascr_skin_Yb
	0xf0, //ascr_skin_Mr
	0x33, //ascr_skin_Mg
	0xe8, //ascr_skin_Mb
	0xff, //ascr_skin_Wr
	0xfa, //ascr_skin_Wg
	0xf1, //ascr_skin_Wb
	0x5c, //ascr_Cr
	0xe1, //ascr_Rr
	0xef, //ascr_Cg
	0x25, //ascr_Rg
	0xe8, //ascr_Cb
	0x00, //ascr_Rb
	0xf0, //ascr_Mr
	0x49, //ascr_Gr
	0x33, //ascr_Mg
	0xdf, //ascr_Gg
	0xe8, //ascr_Mb
	0x00, //ascr_Gb
	0xee, //ascr_Yr
	0x37, //ascr_Br
	0xea, //ascr_Yg
	0x21, //ascr_Bg
	0x00, //ascr_Yb
	0xdf, //ascr_Bb
	0xff, //ascr_Wr
	0x00, //ascr_Kr
	0xfa, //ascr_Wg
	0x00, //ascr_Kg
	0xf1, //ascr_Wb
	0x00, //ascr_Kb
	0x0F, //edge_ctrl
	0x00, //edge_radius 12
	0x6F,
	0x05, //edge_aa_factor
	0x00, //edge_r
	0x00, //edge_g
	0x00, //edge_b
	0x00, //edge_x 11
	0x70,
	0x00, //edge_y 12
	0x70,
	0x44, //edge_coef_a edge_coef_b 0000 0000
};

static char DSI_CAMERA_DYNAMIC_MDNIE_2[] = {
	0xDE,
	0x00, //gamut_gamma_on gamut_blk_shift 0000 0000
	0x40, //gamut_scale
	0x04, //gamut_r1
	0x00,
	0x00, //gamut_r2
	0x00,
	0x00, //gamut_r3
	0x00,
	0x00, //gamut_g1
	0x00,
	0x04, //gamut_g2
	0x00,
	0x00, //gamut_g3
	0x00,
	0x00, //gamut_b1
	0x00,
	0x00, //gamut_b2
	0x00,
	0x04, //gamut_b3
	0x00,
	0x00, //slce_on cadrx_en 0000 0000
	0x00, //glare_on
	0x00, //lce_gain 000 0000
	0x24, //lce_color_gain 00 0000
	0x96, //lce_min_ref_offset
	0xb3, //lce_illum_gain
	0x01, //lce_ref_offset 9
	0x0e,
	0x01, //lce_ref_gain 9
	0x00,
	0x77, //lce_block_size h v 0000 0000
	0x03, //lce_dark_th 000
	0x07, //lce_reduct_slope 0000
	0x40, //lce_black cc0 cc1 color_th 0 0 0 0000
	0x08, //lce_large_w
	0x04, //lce_med_w
	0x04, //lce_small_w
	0x00, //cadrx_dit_en shadow_corr_en ref_ctrl_en 0 0 0
	0x00, //cadrx_gain
	0x0d, //cadrx_low_intensity_th
	0xe1, //cadrx_high_intensity_th
	0x07, //cadrx_ui_zerobincnt_th
	0x29, //cadrx_ui_ratio_th
	0x04,
	0x0f, //cadrx_entire_freq
	0xa0,
	0x00,
	0x03, //cadrx_peak_th_in_freq
	0x7a,
	0xa0,
	0x00, //cadrx_high_peak_th_in_freq
	0x25,
	0x1c,
	0x24, //cadrx_dit_color_gain
	0x00, //cadrx_dit_illum_a0
	0x48, //cadrx_dit_illum_a1
	0xea, //cadrx_dit_illum_a2
	0xfa, //cadrx_dit_illum_a3
	0x00, //cadrx_dit_illum_b0
	0x10,
	0x00, //cadrx_dit_illum_b1
	0x10,
	0x00, //cadrx_dit_illum_b2
	0x9c,
	0x01, //cadrx_dit_illum_b3
	0x00,
	0x00, //cadrx_dit_illum_slope1
	0x00,
	0x00, //cadrx_dit_illum_slope2
	0xdc,
	0x03, //cadrx_dit_illum_slope3
	0xff,
	0x40, //cadrx_ui_illum_a1
	0x80, //cadrx_ui_illum_a2
	0xc0, //cadrx_ui_illum_a3
	0x00, //cadrx_ui_illum_offset1
	0x9a,
	0x00, //cadrx_ui_illum_offset2
	0x9a,
	0x00, //cadrx_ui_illum_offset3
	0x9a,
	0x00, //cadrx_ui_illum_offset4
	0x9a,
	0x00, //cadrx_ui_illum_slope1
	0x00,
	0x00, //cadrx_ui_illum_slope2
	0x00,
	0x00, //cadrx_ui_illum_slope3
	0x00,
	0x00, //cadrx_ui_illum_slope4
	0x00,
	0x40, //cadrx_ui_ref_a1
	0x80, //cadrx_ui_ref_a2
	0xc0, //cadrx_ui_ref_a3
	0x01, //cadrx_ui_ref_offset1
	0x0e,
	0x01, //cadrx_ui_ref_offset2
	0x0e,
	0x01, //cadrx_ui_ref_offset3
	0x0e,
	0x01, //cadrx_ui_ref_offset4
	0x0e,
	0x00, //cadrx_ui_ref_slope1
	0x00,
	0x00, //cadrx_ui_ref_slope2
	0x00,
	0x00, //cadrx_ui_ref_slope3
	0x00,
	0x00, //cadrx_ui_ref_slope4
	0x00,
	0x01, //cadrx_reg_ref_c1_offset
	0x0e,
	0x01, //cadrx_reg_ref_c2_offset
	0x3a,
	0x01, //cadrx_reg_ref_c3_offset
	0x44,
	0x00, //cadrx_reg_ref_c1_slope
	0x01,
	0x00, //cadrx_reg_ref_c2_slope
	0x43,
	0x00, //cadrx_reg_ref_c3_slope
	0x4b,
	0x00, //cadrx_sc_reg_a1
	0x18,
	0x00, //cadrx_sc_reg_b1
	0x1c,
	0x01, //cadrx_sc_k1_int
	0xff,
	0x01, //cadrx_sc_k2_int
	0xff,
	0x03, //cadrx_sc_w1_int
	0xff,
	0xe8,
	0x03, //cadrx_sc_w2_int
	0xff,
	0xe8,
	0x00, //flicker_w_en
	0x14, //flicker_w
	0x01, //flicker_trans_en
	0x04, //flicker_trans_slope
	0x00, //bl_en
	0x03, //bl_hbs_th
	0x10, //bl_lbs_th
	0xC8, //bl_illum_gain
	0x01, //bl_reg_gain 9
	0x2C,
	0x00, //bl_le_diff
	0x00, //bl_cadrx_gain
	0x04, //bl_trans_slope
	0x00, //le_en
	0x40, //le_diff
	0x03, //le_lmax 10
	0xB6,
	0x23, //le_p
	0x24, //le_w_tmf
	0x00, //le_w_struct_diff 9
	0x72,
	0x01, //le_w_struct_diff_gain
	0x00, //le_w_luminance 9
	0x0D,
	0x00, //le_luminance_slope 10
	0x04,
	0x80, //glare_blk_avg_th 0000 0000
	0x00, //glare_luma_gain 9
	0x28,
	0xa0, //glare_luma_ctl_start 0000 0000
	0x01, //glare_luma_th 0000 0000
	0x00, //glare_luma_step 9
	0x01,
	0x00, //glare_uni_luma_gain 9
	0x01,
	0x00, //glare_blk_max_th 0000 0000
	0x00, //glare_blk_max_w 0000 0000
	0x10, //glare_blk_max_curve blk_max_sh 0 0000
	0x00, //glare_con_ext_max_th 0000 0000
	0x00, //glare_con_ext_max_w 0000 0000
	0x30, //glare_con_ext_max_sign con_ext_max_curve con_ext_max_sh 0 0 0000
	0x00, //glare_max_cnt_th 0000
	0x00, //glare_max_cnt_th 0000 0000
	0x00, //glare_max_cnt_w 0000 0000
	0x30, //glare_max_cnt_sign max_cnt_curve max_cnt_sh 0 0 0000
	0x00, //glare_y_avg_th 0000 0000
	0x00, //glare_y_avg_w 0000 0000
	0x30, //glare_y_avg_sign y_avg_curve y_avg_sh 0 0 0000
	0x00, //glare_max_cnt_th_dmc 0000
	0x00, //glare_max_cnt_th_dmc 0000 0000
	0x00, //nr fa de cs gamma 0 0000
	0xff, //nr_mask_th
	0x00, //de_gain 10
	0x00,
	0x02, //de_maxplus 11
	0x00,
	0x02, //de_maxminus 11
	0x00,
	0x14, //fa_edge_th
	0x00, //fa_step_p 13
	0x01,
	0x00, //fa_step_n 13
	0x01,
	0x00, //fa_max_de_gain 13
	0x10,
	0x00, //fa_pcl_ppi 14
	0x00,
	0x28, //fa_os_cnt_10_co
	0x3c, //fa_os_cnt_20_co
	0x04, //fa_edge_cnt_weight
	0x0f, //fa_avg_y_weight
	0x01, //cs_gain 10
	0x00,
	0x00, //curve_0
	0x10, //curve_1
	0x20, //curve_2
	0x30, //curve_3
	0x40, //curve_4
	0x50, //curve_5
	0x60, //curve_6
	0x70, //curve_7
	0x80, //curve_8
	0x90, //curve_9
	0xa0, //curve_10
	0xb0, //curve_11
	0xc0, //curve_12
	0xd0, //curve_13
	0xe0, //curve_14
	0xf0, //curve_15
	0x01, //curve_16
	0x00,
	0x00, //curve_offset 10
	0x00,
	0x00, //curve_low_x
	0x00, //blank
	0x00, //curve_low_y
};

static char DSI_CAMERA_DYNAMIC_MDNIE_3[] = {
	0xDD,
	0x01, //mdnie_en
	0x00, //mask 00 0000
	0x03, //edge 00
	0xf0, //ascr algo aolce gamut 00 00 00 00
	0xff, //partial_en1
	0xff, //partial_en2
	0x00, //roi_en
	0x00, //roi_block
	0x00, //roi_sx
	0x00, //roi_sx
	0x00, //roi_sy
	0x00, //roi_sy
	0x00, //roi_ex
	0x00, //roi_ex
	0x00, //roi_ey
	0x00, //roi_ey
	0xff, //trans_on trans_block 0 000 0000
	0x04, //trans_slope
	0x01, //trans_interval
	//end
};

static char DSI_CAMERA_STANDARD_MDNIE_1[] = {
	//start
	0xDF,
	0x11, //linear_on ascr_skin_on 0000 0000
	0x6a, //ascr_skin_cb
	0x9a, //ascr_skin_cr
	0x25, //ascr_dist_up
	0x1a, //ascr_dist_down
	0x16, //ascr_dist_right
	0x2a, //ascr_dist_left
	0x00, //ascr_div_up 20
	0x37,
	0x5a,
	0x00, //ascr_div_down
	0x4e,
	0xc5,
	0x00, //ascr_div_right
	0x5d,
	0x17,
	0x00, //ascr_div_left
	0x30,
	0xc3,
	0xee, //ascr_skin_Rr
	0x4b, //ascr_skin_Rg
	0x30, //ascr_skin_Rb
	0xef, //ascr_skin_Yr
	0xeb, //ascr_skin_Yg
	0x47, //ascr_skin_Yb
	0xfc, //ascr_skin_Mr
	0x57, //ascr_skin_Mg
	0xe7, //ascr_skin_Mb
	0xff, //ascr_skin_Wr
	0xfa, //ascr_skin_Wg
	0xf1, //ascr_skin_Wb
	0x00, //ascr_Cr
	0xee, //ascr_Rr
	0xe7, //ascr_Cg
	0x4b, //ascr_Rg
	0xe5, //ascr_Cb
	0x30, //ascr_Rb
	0xfc, //ascr_Mr
	0x00, //ascr_Gr
	0x57, //ascr_Mg
	0xd9, //ascr_Gg
	0xe7, //ascr_Mb
	0x34, //ascr_Gb
	0xef, //ascr_Yr
	0x34, //ascr_Br
	0xeb, //ascr_Yg
	0x20, //ascr_Bg
	0x47, //ascr_Yb
	0xdb, //ascr_Bb
	0xff, //ascr_Wr
	0x00, //ascr_Kr
	0xfa, //ascr_Wg
	0x00, //ascr_Kg
	0xf1, //ascr_Wb
	0x00, //ascr_Kb
	0x0F, //edge_ctrl
	0x00, //edge_radius 12
	0x6F,
	0x05, //edge_aa_factor
	0x00, //edge_r
	0x00, //edge_g
	0x00, //edge_b
	0x00, //edge_x 11
	0x70,
	0x00, //edge_y 12
	0x70,
	0x44, //edge_coef_a edge_coef_b 0000 0000
};

static char DSI_CAMERA_STANDARD_MDNIE_2[] = {
	0xDE,
	0x00, //gamut_gamma_on gamut_blk_shift 0000 0000
	0x40, //gamut_scale
	0x04, //gamut_r1
	0x00,
	0x00, //gamut_r2
	0x00,
	0x00, //gamut_r3
	0x00,
	0x00, //gamut_g1
	0x00,
	0x04, //gamut_g2
	0x00,
	0x00, //gamut_g3
	0x00,
	0x00, //gamut_b1
	0x00,
	0x00, //gamut_b2
	0x00,
	0x04, //gamut_b3
	0x00,
	0x00, //slce_on cadrx_en 0000 0000
	0x00, //glare_on
	0x00, //lce_gain 000 0000
	0x24, //lce_color_gain 00 0000
	0x96, //lce_min_ref_offset
	0xb3, //lce_illum_gain
	0x01, //lce_ref_offset 9
	0x0e,
	0x01, //lce_ref_gain 9
	0x00,
	0x77, //lce_block_size h v 0000 0000
	0x03, //lce_dark_th 000
	0x07, //lce_reduct_slope 0000
	0x40, //lce_black cc0 cc1 color_th 0 0 0 0000
	0x08, //lce_large_w
	0x04, //lce_med_w
	0x04, //lce_small_w
	0x00, //cadrx_dit_en shadow_corr_en ref_ctrl_en 0 0 0
	0x00, //cadrx_gain
	0x0d, //cadrx_low_intensity_th
	0xe1, //cadrx_high_intensity_th
	0x07, //cadrx_ui_zerobincnt_th
	0x29, //cadrx_ui_ratio_th
	0x04,
	0x0f, //cadrx_entire_freq
	0xa0,
	0x00,
	0x03, //cadrx_peak_th_in_freq
	0x7a,
	0xa0,
	0x00, //cadrx_high_peak_th_in_freq
	0x25,
	0x1c,
	0x24, //cadrx_dit_color_gain
	0x00, //cadrx_dit_illum_a0
	0x48, //cadrx_dit_illum_a1
	0xea, //cadrx_dit_illum_a2
	0xfa, //cadrx_dit_illum_a3
	0x00, //cadrx_dit_illum_b0
	0x10,
	0x00, //cadrx_dit_illum_b1
	0x10,
	0x00, //cadrx_dit_illum_b2
	0x9c,
	0x01, //cadrx_dit_illum_b3
	0x00,
	0x00, //cadrx_dit_illum_slope1
	0x00,
	0x00, //cadrx_dit_illum_slope2
	0xdc,
	0x03, //cadrx_dit_illum_slope3
	0xff,
	0x40, //cadrx_ui_illum_a1
	0x80, //cadrx_ui_illum_a2
	0xc0, //cadrx_ui_illum_a3
	0x00, //cadrx_ui_illum_offset1
	0x9a,
	0x00, //cadrx_ui_illum_offset2
	0x9a,
	0x00, //cadrx_ui_illum_offset3
	0x9a,
	0x00, //cadrx_ui_illum_offset4
	0x9a,
	0x00, //cadrx_ui_illum_slope1
	0x00,
	0x00, //cadrx_ui_illum_slope2
	0x00,
	0x00, //cadrx_ui_illum_slope3
	0x00,
	0x00, //cadrx_ui_illum_slope4
	0x00,
	0x40, //cadrx_ui_ref_a1
	0x80, //cadrx_ui_ref_a2
	0xc0, //cadrx_ui_ref_a3
	0x01, //cadrx_ui_ref_offset1
	0x0e,
	0x01, //cadrx_ui_ref_offset2
	0x0e,
	0x01, //cadrx_ui_ref_offset3
	0x0e,
	0x01, //cadrx_ui_ref_offset4
	0x0e,
	0x00, //cadrx_ui_ref_slope1
	0x00,
	0x00, //cadrx_ui_ref_slope2
	0x00,
	0x00, //cadrx_ui_ref_slope3
	0x00,
	0x00, //cadrx_ui_ref_slope4
	0x00,
	0x01, //cadrx_reg_ref_c1_offset
	0x0e,
	0x01, //cadrx_reg_ref_c2_offset
	0x3a,
	0x01, //cadrx_reg_ref_c3_offset
	0x44,
	0x00, //cadrx_reg_ref_c1_slope
	0x01,
	0x00, //cadrx_reg_ref_c2_slope
	0x43,
	0x00, //cadrx_reg_ref_c3_slope
	0x4b,
	0x00, //cadrx_sc_reg_a1
	0x18,
	0x00, //cadrx_sc_reg_b1
	0x1c,
	0x01, //cadrx_sc_k1_int
	0xff,
	0x01, //cadrx_sc_k2_int
	0xff,
	0x03, //cadrx_sc_w1_int
	0xff,
	0xe8,
	0x03, //cadrx_sc_w2_int
	0xff,
	0xe8,
	0x00, //flicker_w_en
	0x14, //flicker_w
	0x01, //flicker_trans_en
	0x04, //flicker_trans_slope
	0x00, //bl_en
	0x03, //bl_hbs_th
	0x10, //bl_lbs_th
	0xC8, //bl_illum_gain
	0x01, //bl_reg_gain 9
	0x2C,
	0x00, //bl_le_diff
	0x00, //bl_cadrx_gain
	0x04, //bl_trans_slope
	0x00, //le_en
	0x40, //le_diff
	0x03, //le_lmax 10
	0xB6,
	0x23, //le_p
	0x24, //le_w_tmf
	0x00, //le_w_struct_diff 9
	0x72,
	0x01, //le_w_struct_diff_gain
	0x00, //le_w_luminance 9
	0x0D,
	0x00, //le_luminance_slope 10
	0x04,
	0x80, //glare_blk_avg_th 0000 0000
	0x00, //glare_luma_gain 9
	0x28,
	0xa0, //glare_luma_ctl_start 0000 0000
	0x01, //glare_luma_th 0000 0000
	0x00, //glare_luma_step 9
	0x01,
	0x00, //glare_uni_luma_gain 9
	0x01,
	0x00, //glare_blk_max_th 0000 0000
	0x00, //glare_blk_max_w 0000 0000
	0x10, //glare_blk_max_curve blk_max_sh 0 0000
	0x00, //glare_con_ext_max_th 0000 0000
	0x00, //glare_con_ext_max_w 0000 0000
	0x30, //glare_con_ext_max_sign con_ext_max_curve con_ext_max_sh 0 0 0000
	0x00, //glare_max_cnt_th 0000
	0x00, //glare_max_cnt_th 0000 0000
	0x00, //glare_max_cnt_w 0000 0000
	0x30, //glare_max_cnt_sign max_cnt_curve max_cnt_sh 0 0 0000
	0x00, //glare_y_avg_th 0000 0000
	0x00, //glare_y_avg_w 0000 0000
	0x30, //glare_y_avg_sign y_avg_curve y_avg_sh 0 0 0000
	0x00, //glare_max_cnt_th_dmc 0000
	0x00, //glare_max_cnt_th_dmc 0000 0000
	0x02, //nr fa de cs gamma 0 0000
	0xff, //nr_mask_th
	0x00, //de_gain 10
	0x00,
	0x02, //de_maxplus 11
	0x00,
	0x02, //de_maxminus 11
	0x00,
	0x14, //fa_edge_th
	0x00, //fa_step_p 13
	0x01,
	0x00, //fa_step_n 13
	0x01,
	0x00, //fa_max_de_gain 13
	0x10,
	0x00, //fa_pcl_ppi 14
	0x00,
	0x28, //fa_os_cnt_10_co
	0x3c, //fa_os_cnt_20_co
	0x04, //fa_edge_cnt_weight
	0x0f, //fa_avg_y_weight
	0x01, //cs_gain 10
	0x18,
	0x00, //curve_0
	0x10, //curve_1
	0x20, //curve_2
	0x30, //curve_3
	0x40, //curve_4
	0x50, //curve_5
	0x60, //curve_6
	0x70, //curve_7
	0x80, //curve_8
	0x90, //curve_9
	0xa0, //curve_10
	0xb0, //curve_11
	0xc0, //curve_12
	0xd0, //curve_13
	0xe0, //curve_14
	0xf0, //curve_15
	0x01, //curve_16
	0x00,
	0x00, //curve_offset 10
	0x00,
	0x00, //curve_low_x
	0x00, //blank
	0x00, //curve_low_y
};

static char DSI_CAMERA_STANDARD_MDNIE_3[] = {
	0xDD,
	0x01, //mdnie_en
	0x00, //mask 00 0000
	0x03, //edge 00
	0xf0, //ascr algo aolce gamut 00 00 00 00
	0xff, //partial_en1
	0xff, //partial_en2
	0x00, //roi_en
	0x00, //roi_block
	0x00, //roi_sx
	0x00, //roi_sx
	0x00, //roi_sy
	0x00, //roi_sy
	0x00, //roi_ex
	0x00, //roi_ex
	0x00, //roi_ey
	0x00, //roi_ey
	0xff, //trans_on trans_block 0 000 0000
	0x04, //trans_slope
	0x01, //trans_interval
	//end
};

static char DSI_CAMERA_NATURAL_MDNIE_1[] = {
	//start
	0xDF,
	0x11, //linear_on ascr_skin_on 0000 0000
	0x6a, //ascr_skin_cb
	0x9a, //ascr_skin_cr
	0x25, //ascr_dist_up
	0x1a, //ascr_dist_down
	0x16, //ascr_dist_right
	0x2a, //ascr_dist_left
	0x00, //ascr_div_up 20
	0x37,
	0x5a,
	0x00, //ascr_div_down
	0x4e,
	0xc5,
	0x00, //ascr_div_right
	0x5d,
	0x17,
	0x00, //ascr_div_left
	0x30,
	0xc3,
	0xd3, //ascr_skin_Rr
	0x40, //ascr_skin_Rg
	0x29, //ascr_skin_Rb
	0xf0, //ascr_skin_Yr
	0xec, //ascr_skin_Yg
	0x5b, //ascr_skin_Yb
	0xdf, //ascr_skin_Mr
	0x4a, //ascr_skin_Mg
	0xdf, //ascr_skin_Mb
	0xff, //ascr_skin_Wr
	0xfa, //ascr_skin_Wg
	0xf1, //ascr_skin_Wb
	0x93, //ascr_Cr
	0xd3, //ascr_Rr
	0xed, //ascr_Cg
	0x40, //ascr_Rg
	0xe7, //ascr_Cb
	0x29, //ascr_Rb
	0xdf, //ascr_Mr
	0x85, //ascr_Gr
	0x4a, //ascr_Mg
	0xe0, //ascr_Gg
	0xdf, //ascr_Mb
	0x4f, //ascr_Gb
	0xf0, //ascr_Yr
	0x32, //ascr_Br
	0xec, //ascr_Yg
	0x1f, //ascr_Bg
	0x5b, //ascr_Yb
	0xd8, //ascr_Bb
	0xff, //ascr_Wr
	0x00, //ascr_Kr
	0xfa, //ascr_Wg
	0x00, //ascr_Kg
	0xf1, //ascr_Wb
	0x00, //ascr_Kb
	0x0F, //edge_ctrl
	0x00, //edge_radius 12
	0x6F,
	0x05, //edge_aa_factor
	0x00, //edge_r
	0x00, //edge_g
	0x00, //edge_b
	0x00, //edge_x 11
	0x70,
	0x00, //edge_y 12
	0x70,
	0x44, //edge_coef_a edge_coef_b 0000 0000
};

static char DSI_CAMERA_NATURAL_MDNIE_2[] = {
	0xDE,
	0x00, //gamut_gamma_on gamut_blk_shift 0000 0000
	0x40, //gamut_scale
	0x04, //gamut_r1
	0x00,
	0x00, //gamut_r2
	0x00,
	0x00, //gamut_r3
	0x00,
	0x00, //gamut_g1
	0x00,
	0x04, //gamut_g2
	0x00,
	0x00, //gamut_g3
	0x00,
	0x00, //gamut_b1
	0x00,
	0x00, //gamut_b2
	0x00,
	0x04, //gamut_b3
	0x00,
	0x00, //slce_on cadrx_en 0000 0000
	0x00, //glare_on
	0x00, //lce_gain 000 0000
	0x24, //lce_color_gain 00 0000
	0x96, //lce_min_ref_offset
	0xb3, //lce_illum_gain
	0x01, //lce_ref_offset 9
	0x0e,
	0x01, //lce_ref_gain 9
	0x00,
	0x77, //lce_block_size h v 0000 0000
	0x03, //lce_dark_th 000
	0x07, //lce_reduct_slope 0000
	0x40, //lce_black cc0 cc1 color_th 0 0 0 0000
	0x08, //lce_large_w
	0x04, //lce_med_w
	0x04, //lce_small_w
	0x00, //cadrx_dit_en shadow_corr_en ref_ctrl_en 0 0 0
	0x00, //cadrx_gain
	0x0d, //cadrx_low_intensity_th
	0xe1, //cadrx_high_intensity_th
	0x07, //cadrx_ui_zerobincnt_th
	0x29, //cadrx_ui_ratio_th
	0x04,
	0x0f, //cadrx_entire_freq
	0xa0,
	0x00,
	0x03, //cadrx_peak_th_in_freq
	0x7a,
	0xa0,
	0x00, //cadrx_high_peak_th_in_freq
	0x25,
	0x1c,
	0x24, //cadrx_dit_color_gain
	0x00, //cadrx_dit_illum_a0
	0x48, //cadrx_dit_illum_a1
	0xea, //cadrx_dit_illum_a2
	0xfa, //cadrx_dit_illum_a3
	0x00, //cadrx_dit_illum_b0
	0x10,
	0x00, //cadrx_dit_illum_b1
	0x10,
	0x00, //cadrx_dit_illum_b2
	0x9c,
	0x01, //cadrx_dit_illum_b3
	0x00,
	0x00, //cadrx_dit_illum_slope1
	0x00,
	0x00, //cadrx_dit_illum_slope2
	0xdc,
	0x03, //cadrx_dit_illum_slope3
	0xff,
	0x40, //cadrx_ui_illum_a1
	0x80, //cadrx_ui_illum_a2
	0xc0, //cadrx_ui_illum_a3
	0x00, //cadrx_ui_illum_offset1
	0x9a,
	0x00, //cadrx_ui_illum_offset2
	0x9a,
	0x00, //cadrx_ui_illum_offset3
	0x9a,
	0x00, //cadrx_ui_illum_offset4
	0x9a,
	0x00, //cadrx_ui_illum_slope1
	0x00,
	0x00, //cadrx_ui_illum_slope2
	0x00,
	0x00, //cadrx_ui_illum_slope3
	0x00,
	0x00, //cadrx_ui_illum_slope4
	0x00,
	0x40, //cadrx_ui_ref_a1
	0x80, //cadrx_ui_ref_a2
	0xc0, //cadrx_ui_ref_a3
	0x01, //cadrx_ui_ref_offset1
	0x0e,
	0x01, //cadrx_ui_ref_offset2
	0x0e,
	0x01, //cadrx_ui_ref_offset3
	0x0e,
	0x01, //cadrx_ui_ref_offset4
	0x0e,
	0x00, //cadrx_ui_ref_slope1
	0x00,
	0x00, //cadrx_ui_ref_slope2
	0x00,
	0x00, //cadrx_ui_ref_slope3
	0x00,
	0x00, //cadrx_ui_ref_slope4
	0x00,
	0x01, //cadrx_reg_ref_c1_offset
	0x0e,
	0x01, //cadrx_reg_ref_c2_offset
	0x3a,
	0x01, //cadrx_reg_ref_c3_offset
	0x44,
	0x00, //cadrx_reg_ref_c1_slope
	0x01,
	0x00, //cadrx_reg_ref_c2_slope
	0x43,
	0x00, //cadrx_reg_ref_c3_slope
	0x4b,
	0x00, //cadrx_sc_reg_a1
	0x18,
	0x00, //cadrx_sc_reg_b1
	0x1c,
	0x01, //cadrx_sc_k1_int
	0xff,
	0x01, //cadrx_sc_k2_int
	0xff,
	0x03, //cadrx_sc_w1_int
	0xff,
	0xe8,
	0x03, //cadrx_sc_w2_int
	0xff,
	0xe8,
	0x00, //flicker_w_en
	0x14, //flicker_w
	0x01, //flicker_trans_en
	0x04, //flicker_trans_slope
	0x00, //bl_en
	0x03, //bl_hbs_th
	0x10, //bl_lbs_th
	0xC8, //bl_illum_gain
	0x01, //bl_reg_gain 9
	0x2C,
	0x00, //bl_le_diff
	0x00, //bl_cadrx_gain
	0x04, //bl_trans_slope
	0x00, //le_en
	0x40, //le_diff
	0x03, //le_lmax 10
	0xB6,
	0x23, //le_p
	0x24, //le_w_tmf
	0x00, //le_w_struct_diff 9
	0x72,
	0x01, //le_w_struct_diff_gain
	0x00, //le_w_luminance 9
	0x0D,
	0x00, //le_luminance_slope 10
	0x04,
	0x80, //glare_blk_avg_th 0000 0000
	0x00, //glare_luma_gain 9
	0x28,
	0xa0, //glare_luma_ctl_start 0000 0000
	0x01, //glare_luma_th 0000 0000
	0x00, //glare_luma_step 9
	0x01,
	0x00, //glare_uni_luma_gain 9
	0x01,
	0x00, //glare_blk_max_th 0000 0000
	0x00, //glare_blk_max_w 0000 0000
	0x10, //glare_blk_max_curve blk_max_sh 0 0000
	0x00, //glare_con_ext_max_th 0000 0000
	0x00, //glare_con_ext_max_w 0000 0000
	0x30, //glare_con_ext_max_sign con_ext_max_curve con_ext_max_sh 0 0 0000
	0x00, //glare_max_cnt_th 0000
	0x00, //glare_max_cnt_th 0000 0000
	0x00, //glare_max_cnt_w 0000 0000
	0x30, //glare_max_cnt_sign max_cnt_curve max_cnt_sh 0 0 0000
	0x00, //glare_y_avg_th 0000 0000
	0x00, //glare_y_avg_w 0000 0000
	0x30, //glare_y_avg_sign y_avg_curve y_avg_sh 0 0 0000
	0x00, //glare_max_cnt_th_dmc 0000
	0x00, //glare_max_cnt_th_dmc 0000 0000
	0x00, //nr fa de cs gamma 0 0000
	0xff, //nr_mask_th
	0x00, //de_gain 10
	0x00,
	0x02, //de_maxplus 11
	0x00,
	0x02, //de_maxminus 11
	0x00,
	0x14, //fa_edge_th
	0x00, //fa_step_p 13
	0x01,
	0x00, //fa_step_n 13
	0x01,
	0x00, //fa_max_de_gain 13
	0x10,
	0x00, //fa_pcl_ppi 14
	0x00,
	0x28, //fa_os_cnt_10_co
	0x3c, //fa_os_cnt_20_co
	0x04, //fa_edge_cnt_weight
	0x0f, //fa_avg_y_weight
	0x01, //cs_gain 10
	0x00,
	0x00, //curve_0
	0x10, //curve_1
	0x20, //curve_2
	0x30, //curve_3
	0x40, //curve_4
	0x50, //curve_5
	0x60, //curve_6
	0x70, //curve_7
	0x80, //curve_8
	0x90, //curve_9
	0xa0, //curve_10
	0xb0, //curve_11
	0xc0, //curve_12
	0xd0, //curve_13
	0xe0, //curve_14
	0xf0, //curve_15
	0x01, //curve_16
	0x00,
	0x00, //curve_offset 10
	0x00,
	0x00, //curve_low_x
	0x00, //blank
	0x00, //curve_low_y
};

static char DSI_CAMERA_NATURAL_MDNIE_3[] = {
	0xDD,
	0x01, //mdnie_en
	0x00, //mask 00 0000
	0x03, //edge 00
	0xf0, //ascr algo aolce gamut 00 00 00 00
	0xff, //partial_en1
	0xff, //partial_en2
	0x00, //roi_en
	0x00, //roi_block
	0x00, //roi_sx
	0x00, //roi_sx
	0x00, //roi_sy
	0x00, //roi_sy
	0x00, //roi_ex
	0x00, //roi_ex
	0x00, //roi_ey
	0x00, //roi_ey
	0xff, //trans_on trans_block 0 000 0000
	0x04, //trans_slope
	0x01, //trans_interval
	//end
};

static char DSI_CAMERA_AUTO_MDNIE_1[] = {
	//start
	0xDF,
	0x01, //linear_on ascr_skin_on 0000 0000
	0x6a, //ascr_skin_cb
	0x9a, //ascr_skin_cr
	0x25, //ascr_dist_up
	0x1a, //ascr_dist_down
	0x16, //ascr_dist_right
	0x2a, //ascr_dist_left
	0x00, //ascr_div_up 20
	0x37,
	0x5a,
	0x00, //ascr_div_down
	0x4e,
	0xc5,
	0x00, //ascr_div_right
	0x5d,
	0x17,
	0x00, //ascr_div_left
	0x30,
	0xc3,
	0xff, //ascr_skin_Rr
	0x48, //ascr_skin_Rg
	0x58, //ascr_skin_Rb
	0xff, //ascr_skin_Yr
	0xf8, //ascr_skin_Yg
	0x00, //ascr_skin_Yb
	0xfe, //ascr_skin_Mr
	0x00, //ascr_skin_Mg
	0xff, //ascr_skin_Mb
	0xff, //ascr_skin_Wr
	0xff, //ascr_skin_Wg
	0xff, //ascr_skin_Wb
	0x00, //ascr_Cr
	0xf0, //ascr_Rr
	0xff, //ascr_Cg
	0x00, //ascr_Rg
	0xfa, //ascr_Cb
	0x00, //ascr_Rb
	0xfe, //ascr_Mr
	0x00, //ascr_Gr
	0x00, //ascr_Mg
	0xff, //ascr_Gg
	0xff, //ascr_Mb
	0x00, //ascr_Gb
	0xff, //ascr_Yr
	0x00, //ascr_Br
	0xf8, //ascr_Yg
	0x00, //ascr_Bg
	0x00, //ascr_Yb
	0xff, //ascr_Bb
	0xff, //ascr_Wr
	0x00, //ascr_Kr
	0xff, //ascr_Wg
	0x00, //ascr_Kg
	0xff, //ascr_Wb
	0x00, //ascr_Kb
	0x0F, //edge_ctrl
	0x00, //edge_radius 12
	0x6F,
	0x05, //edge_aa_factor
	0x00, //edge_r
	0x00, //edge_g
	0x00, //edge_b
	0x00, //edge_x 11
	0x70,
	0x00, //edge_y 12
	0x70,
	0x44, //edge_coef_a edge_coef_b 0000 0000
};

static char DSI_CAMERA_AUTO_MDNIE_2[] = {
	0xDE,
	0x00, //gamut_gamma_on gamut_blk_shift 0000 0000
	0x40, //gamut_scale
	0x04, //gamut_r1
	0x00,
	0x00, //gamut_r2
	0x00,
	0x00, //gamut_r3
	0x00,
	0x00, //gamut_g1
	0x00,
	0x04, //gamut_g2
	0x00,
	0x00, //gamut_g3
	0x00,
	0x00, //gamut_b1
	0x00,
	0x00, //gamut_b2
	0x00,
	0x04, //gamut_b3
	0x00,
	0x00, //slce_on cadrx_en 0000 0000
	0x00, //glare_on
	0x00, //lce_gain 000 0000
	0x24, //lce_color_gain 00 0000
	0x96, //lce_min_ref_offset
	0xb3, //lce_illum_gain
	0x01, //lce_ref_offset 9
	0x0e,
	0x01, //lce_ref_gain 9
	0x00,
	0x77, //lce_block_size h v 0000 0000
	0x03, //lce_dark_th 000
	0x07, //lce_reduct_slope 0000
	0x40, //lce_black cc0 cc1 color_th 0 0 0 0000
	0x08, //lce_large_w
	0x04, //lce_med_w
	0x04, //lce_small_w
	0x00, //cadrx_dit_en shadow_corr_en ref_ctrl_en 0 0 0
	0x00, //cadrx_gain
	0x0d, //cadrx_low_intensity_th
	0xe1, //cadrx_high_intensity_th
	0x07, //cadrx_ui_zerobincnt_th
	0x29, //cadrx_ui_ratio_th
	0x04,
	0x0f, //cadrx_entire_freq
	0xa0,
	0x00,
	0x03, //cadrx_peak_th_in_freq
	0x7a,
	0xa0,
	0x00, //cadrx_high_peak_th_in_freq
	0x25,
	0x1c,
	0x24, //cadrx_dit_color_gain
	0x00, //cadrx_dit_illum_a0
	0x48, //cadrx_dit_illum_a1
	0xea, //cadrx_dit_illum_a2
	0xfa, //cadrx_dit_illum_a3
	0x00, //cadrx_dit_illum_b0
	0x10,
	0x00, //cadrx_dit_illum_b1
	0x10,
	0x00, //cadrx_dit_illum_b2
	0x9c,
	0x01, //cadrx_dit_illum_b3
	0x00,
	0x00, //cadrx_dit_illum_slope1
	0x00,
	0x00, //cadrx_dit_illum_slope2
	0xdc,
	0x03, //cadrx_dit_illum_slope3
	0xff,
	0x40, //cadrx_ui_illum_a1
	0x80, //cadrx_ui_illum_a2
	0xc0, //cadrx_ui_illum_a3
	0x00, //cadrx_ui_illum_offset1
	0x9a,
	0x00, //cadrx_ui_illum_offset2
	0x9a,
	0x00, //cadrx_ui_illum_offset3
	0x9a,
	0x00, //cadrx_ui_illum_offset4
	0x9a,
	0x00, //cadrx_ui_illum_slope1
	0x00,
	0x00, //cadrx_ui_illum_slope2
	0x00,
	0x00, //cadrx_ui_illum_slope3
	0x00,
	0x00, //cadrx_ui_illum_slope4
	0x00,
	0x40, //cadrx_ui_ref_a1
	0x80, //cadrx_ui_ref_a2
	0xc0, //cadrx_ui_ref_a3
	0x01, //cadrx_ui_ref_offset1
	0x0e,
	0x01, //cadrx_ui_ref_offset2
	0x0e,
	0x01, //cadrx_ui_ref_offset3
	0x0e,
	0x01, //cadrx_ui_ref_offset4
	0x0e,
	0x00, //cadrx_ui_ref_slope1
	0x00,
	0x00, //cadrx_ui_ref_slope2
	0x00,
	0x00, //cadrx_ui_ref_slope3
	0x00,
	0x00, //cadrx_ui_ref_slope4
	0x00,
	0x01, //cadrx_reg_ref_c1_offset
	0x0e,
	0x01, //cadrx_reg_ref_c2_offset
	0x3a,
	0x01, //cadrx_reg_ref_c3_offset
	0x44,
	0x00, //cadrx_reg_ref_c1_slope
	0x01,
	0x00, //cadrx_reg_ref_c2_slope
	0x43,
	0x00, //cadrx_reg_ref_c3_slope
	0x4b,
	0x00, //cadrx_sc_reg_a1
	0x18,
	0x00, //cadrx_sc_reg_b1
	0x1c,
	0x01, //cadrx_sc_k1_int
	0xff,
	0x01, //cadrx_sc_k2_int
	0xff,
	0x03, //cadrx_sc_w1_int
	0xff,
	0xe8,
	0x03, //cadrx_sc_w2_int
	0xff,
	0xe8,
	0x00, //flicker_w_en
	0x14, //flicker_w
	0x01, //flicker_trans_en
	0x04, //flicker_trans_slope
	0x00, //bl_en
	0x03, //bl_hbs_th
	0x10, //bl_lbs_th
	0xC8, //bl_illum_gain
	0x01, //bl_reg_gain 9
	0x2C,
	0x00, //bl_le_diff
	0x00, //bl_cadrx_gain
	0x04, //bl_trans_slope
	0x00, //le_en
	0x40, //le_diff
	0x03, //le_lmax 10
	0xB6,
	0x23, //le_p
	0x24, //le_w_tmf
	0x00, //le_w_struct_diff 9
	0x72,
	0x01, //le_w_struct_diff_gain
	0x00, //le_w_luminance 9
	0x0D,
	0x00, //le_luminance_slope 10
	0x04,
	0x80, //glare_blk_avg_th 0000 0000
	0x00, //glare_luma_gain 9
	0x28,
	0xa0, //glare_luma_ctl_start 0000 0000
	0x01, //glare_luma_th 0000 0000
	0x00, //glare_luma_step 9
	0x01,
	0x00, //glare_uni_luma_gain 9
	0x01,
	0xf0, //glare_blk_max_th 0000 0000
	0x00, //glare_blk_max_w 0000 0000
	0x18, //glare_blk_max_curve blk_max_sh 0 0000
	0xf0, //glare_con_ext_max_th 0000 0000
	0x00, //glare_con_ext_max_w 0000 0000
	0x38, //glare_con_ext_max_sign con_ext_max_curve con_ext_max_sh 0 0 0000
	0x00, //glare_max_cnt_th 0000
	0x00, //glare_max_cnt_th 0000 0000
	0x00, //glare_max_cnt_w 0000 0000
	0x38, //glare_max_cnt_sign max_cnt_curve max_cnt_sh 0 0 0000
	0xba, //glare_y_avg_th 0000 0000
	0x00, //glare_y_avg_w 0000 0000
	0x38, //glare_y_avg_sign y_avg_curve y_avg_sh 0 0 0000
	0x00, //glare_max_cnt_th_dmc 0000
	0x05, //glare_max_cnt_th_dmc 0000 0000
	0x00, //nr fa de cs gamma 0 0000
	0xff, //nr_mask_th
	0x00, //de_gain 10
	0x00,
	0x02, //de_maxplus 11
	0x00,
	0x02, //de_maxminus 11
	0x00,
	0x14, //fa_edge_th
	0x00, //fa_step_p 13
	0x01,
	0x00, //fa_step_n 13
	0x01,
	0x00, //fa_max_de_gain 13
	0x10,
	0x00, //fa_pcl_ppi 14
	0x00,
	0x28, //fa_os_cnt_10_co
	0x3c, //fa_os_cnt_20_co
	0x04, //fa_edge_cnt_weight
	0x0f, //fa_avg_y_weight
	0x01, //cs_gain 10
	0x00,
	0x00, //curve_0
	0x10, //curve_1
	0x20, //curve_2
	0x30, //curve_3
	0x40, //curve_4
	0x50, //curve_5
	0x60, //curve_6
	0x70, //curve_7
	0x80, //curve_8
	0x90, //curve_9
	0xa0, //curve_10
	0xb0, //curve_11
	0xc0, //curve_12
	0xd0, //curve_13
	0xe0, //curve_14
	0xf0, //curve_15
	0x01, //curve_16
	0x00,
	0x00, //curve_offset 10
	0x00,
	0x00, //curve_low_x
	0x00, //blank
	0x00, //curve_low_y
};

static char DSI_CAMERA_AUTO_MDNIE_3[] = {
	0xDD,
	0x01, //mdnie_en
	0x00, //mask 00 0000
	0x03, //edge 00
	0xf0, //ascr algo aolce gamut 00 00 00 00
	0xff, //partial_en1
	0xff, //partial_en2
	0x00, //roi_en
	0x00, //roi_block
	0x00, //roi_sx
	0x00, //roi_sx
	0x00, //roi_sy
	0x00, //roi_sy
	0x00, //roi_ex
	0x00, //roi_ex
	0x00, //roi_ey
	0x00, //roi_ey
	0xff, //trans_on trans_block 0 000 0000
	0x04, //trans_slope
	0x01, //trans_interval
	//end
};

static char DSI_GALLERY_DYNAMIC_MDNIE_1[] = {
	//start
	0xDF,
	0x11, //linear_on ascr_skin_on 0000 0000
	0x67, //ascr_skin_cb
	0xa9, //ascr_skin_cr
	0x37, //ascr_dist_up
	0x29, //ascr_dist_down
	0x19, //ascr_dist_right
	0x47, //ascr_dist_left
	0x00, //ascr_div_up 20
	0x25,
	0x3d,
	0x00, //ascr_div_down
	0x31,
	0xf4,
	0x00, //ascr_div_right
	0x51,
	0xec,
	0x00, //ascr_div_left
	0x1c,
	0xd8,
	0xe1, //ascr_skin_Rr
	0x25, //ascr_skin_Rg
	0x00, //ascr_skin_Rb
	0xee, //ascr_skin_Yr
	0xea, //ascr_skin_Yg
	0x00, //ascr_skin_Yb
	0xf0, //ascr_skin_Mr
	0x33, //ascr_skin_Mg
	0xe8, //ascr_skin_Mb
	0xff, //ascr_skin_Wr
	0xfa, //ascr_skin_Wg
	0xf1, //ascr_skin_Wb
	0x5c, //ascr_Cr
	0xe1, //ascr_Rr
	0xef, //ascr_Cg
	0x25, //ascr_Rg
	0xe8, //ascr_Cb
	0x00, //ascr_Rb
	0xf0, //ascr_Mr
	0x49, //ascr_Gr
	0x33, //ascr_Mg
	0xdf, //ascr_Gg
	0xe8, //ascr_Mb
	0x00, //ascr_Gb
	0xee, //ascr_Yr
	0x37, //ascr_Br
	0xea, //ascr_Yg
	0x21, //ascr_Bg
	0x00, //ascr_Yb
	0xdf, //ascr_Bb
	0xff, //ascr_Wr
	0x00, //ascr_Kr
	0xfa, //ascr_Wg
	0x00, //ascr_Kg
	0xf1, //ascr_Wb
	0x00, //ascr_Kb
	0x0F, //edge_ctrl
	0x00, //edge_radius 12
	0x6F,
	0x05, //edge_aa_factor
	0x00, //edge_r
	0x00, //edge_g
	0x00, //edge_b
	0x00, //edge_x 11
	0x70,
	0x00, //edge_y 12
	0x70,
	0x44, //edge_coef_a edge_coef_b 0000 0000
};

static char DSI_GALLERY_DYNAMIC_MDNIE_2[] = {
	0xDE,
	0x00, //gamut_gamma_on gamut_blk_shift 0000 0000
	0x40, //gamut_scale
	0x04, //gamut_r1
	0x00,
	0x00, //gamut_r2
	0x00,
	0x00, //gamut_r3
	0x00,
	0x00, //gamut_g1
	0x00,
	0x04, //gamut_g2
	0x00,
	0x00, //gamut_g3
	0x00,
	0x00, //gamut_b1
	0x00,
	0x00, //gamut_b2
	0x00,
	0x04, //gamut_b3
	0x00,
	0x00, //slce_on cadrx_en 0000 0000
	0x00, //glare_on
	0x00, //lce_gain 000 0000
	0x24, //lce_color_gain 00 0000
	0x96, //lce_min_ref_offset
	0xb3, //lce_illum_gain
	0x01, //lce_ref_offset 9
	0x0e,
	0x01, //lce_ref_gain 9
	0x00,
	0x77, //lce_block_size h v 0000 0000
	0x03, //lce_dark_th 000
	0x07, //lce_reduct_slope 0000
	0x40, //lce_black cc0 cc1 color_th 0 0 0 0000
	0x08, //lce_large_w
	0x04, //lce_med_w
	0x04, //lce_small_w
	0x00, //cadrx_dit_en shadow_corr_en ref_ctrl_en 0 0 0
	0x00, //cadrx_gain
	0x0d, //cadrx_low_intensity_th
	0xe1, //cadrx_high_intensity_th
	0x07, //cadrx_ui_zerobincnt_th
	0x29, //cadrx_ui_ratio_th
	0x04,
	0x0f, //cadrx_entire_freq
	0xa0,
	0x00,
	0x03, //cadrx_peak_th_in_freq
	0x7a,
	0xa0,
	0x00, //cadrx_high_peak_th_in_freq
	0x25,
	0x1c,
	0x24, //cadrx_dit_color_gain
	0x00, //cadrx_dit_illum_a0
	0x48, //cadrx_dit_illum_a1
	0xea, //cadrx_dit_illum_a2
	0xfa, //cadrx_dit_illum_a3
	0x00, //cadrx_dit_illum_b0
	0x10,
	0x00, //cadrx_dit_illum_b1
	0x10,
	0x00, //cadrx_dit_illum_b2
	0x9c,
	0x01, //cadrx_dit_illum_b3
	0x00,
	0x00, //cadrx_dit_illum_slope1
	0x00,
	0x00, //cadrx_dit_illum_slope2
	0xdc,
	0x03, //cadrx_dit_illum_slope3
	0xff,
	0x40, //cadrx_ui_illum_a1
	0x80, //cadrx_ui_illum_a2
	0xc0, //cadrx_ui_illum_a3
	0x00, //cadrx_ui_illum_offset1
	0x9a,
	0x00, //cadrx_ui_illum_offset2
	0x9a,
	0x00, //cadrx_ui_illum_offset3
	0x9a,
	0x00, //cadrx_ui_illum_offset4
	0x9a,
	0x00, //cadrx_ui_illum_slope1
	0x00,
	0x00, //cadrx_ui_illum_slope2
	0x00,
	0x00, //cadrx_ui_illum_slope3
	0x00,
	0x00, //cadrx_ui_illum_slope4
	0x00,
	0x40, //cadrx_ui_ref_a1
	0x80, //cadrx_ui_ref_a2
	0xc0, //cadrx_ui_ref_a3
	0x01, //cadrx_ui_ref_offset1
	0x0e,
	0x01, //cadrx_ui_ref_offset2
	0x0e,
	0x01, //cadrx_ui_ref_offset3
	0x0e,
	0x01, //cadrx_ui_ref_offset4
	0x0e,
	0x00, //cadrx_ui_ref_slope1
	0x00,
	0x00, //cadrx_ui_ref_slope2
	0x00,
	0x00, //cadrx_ui_ref_slope3
	0x00,
	0x00, //cadrx_ui_ref_slope4
	0x00,
	0x01, //cadrx_reg_ref_c1_offset
	0x0e,
	0x01, //cadrx_reg_ref_c2_offset
	0x3a,
	0x01, //cadrx_reg_ref_c3_offset
	0x44,
	0x00, //cadrx_reg_ref_c1_slope
	0x01,
	0x00, //cadrx_reg_ref_c2_slope
	0x43,
	0x00, //cadrx_reg_ref_c3_slope
	0x4b,
	0x00, //cadrx_sc_reg_a1
	0x18,
	0x00, //cadrx_sc_reg_b1
	0x1c,
	0x01, //cadrx_sc_k1_int
	0xff,
	0x01, //cadrx_sc_k2_int
	0xff,
	0x03, //cadrx_sc_w1_int
	0xff,
	0xe8,
	0x03, //cadrx_sc_w2_int
	0xff,
	0xe8,
	0x00, //flicker_w_en
	0x14, //flicker_w
	0x01, //flicker_trans_en
	0x04, //flicker_trans_slope
	0x00, //bl_en
	0x03, //bl_hbs_th
	0x10, //bl_lbs_th
	0xC8, //bl_illum_gain
	0x01, //bl_reg_gain 9
	0x2C,
	0x00, //bl_le_diff
	0x00, //bl_cadrx_gain
	0x04, //bl_trans_slope
	0x00, //le_en
	0x40, //le_diff
	0x03, //le_lmax 10
	0xB6,
	0x23, //le_p
	0x24, //le_w_tmf
	0x00, //le_w_struct_diff 9
	0x72,
	0x01, //le_w_struct_diff_gain
	0x00, //le_w_luminance 9
	0x0D,
	0x00, //le_luminance_slope 10
	0x04,
	0x80, //glare_blk_avg_th 0000 0000
	0x00, //glare_luma_gain 9
	0x28,
	0xa0, //glare_luma_ctl_start 0000 0000
	0x01, //glare_luma_th 0000 0000
	0x00, //glare_luma_step 9
	0x01,
	0x00, //glare_uni_luma_gain 9
	0x01,
	0x00, //glare_blk_max_th 0000 0000
	0x00, //glare_blk_max_w 0000 0000
	0x10, //glare_blk_max_curve blk_max_sh 0 0000
	0x00, //glare_con_ext_max_th 0000 0000
	0x00, //glare_con_ext_max_w 0000 0000
	0x30, //glare_con_ext_max_sign con_ext_max_curve con_ext_max_sh 0 0 0000
	0x00, //glare_max_cnt_th 0000
	0x00, //glare_max_cnt_th 0000 0000
	0x00, //glare_max_cnt_w 0000 0000
	0x30, //glare_max_cnt_sign max_cnt_curve max_cnt_sh 0 0 0000
	0x00, //glare_y_avg_th 0000 0000
	0x00, //glare_y_avg_w 0000 0000
	0x30, //glare_y_avg_sign y_avg_curve y_avg_sh 0 0 0000
	0x00, //glare_max_cnt_th_dmc 0000
	0x00, //glare_max_cnt_th_dmc 0000 0000
	0x04, //nr fa de cs gamma 0 0000
	0xff, //nr_mask_th
	0x00, //de_gain 10
	0x10,
	0x02, //de_maxplus 11
	0x00,
	0x02, //de_maxminus 11
	0x00,
	0x14, //fa_edge_th
	0x00, //fa_step_p 13
	0x01,
	0x00, //fa_step_n 13
	0x01,
	0x00, //fa_max_de_gain 13
	0x10,
	0x00, //fa_pcl_ppi 14
	0x00,
	0x28, //fa_os_cnt_10_co
	0x3c, //fa_os_cnt_20_co
	0x04, //fa_edge_cnt_weight
	0x0f, //fa_avg_y_weight
	0x01, //cs_gain 10
	0x00,
	0x00, //curve_0
	0x10, //curve_1
	0x20, //curve_2
	0x30, //curve_3
	0x40, //curve_4
	0x50, //curve_5
	0x60, //curve_6
	0x70, //curve_7
	0x80, //curve_8
	0x90, //curve_9
	0xa0, //curve_10
	0xb0, //curve_11
	0xc0, //curve_12
	0xd0, //curve_13
	0xe0, //curve_14
	0xf0, //curve_15
	0x01, //curve_16
	0x00,
	0x00, //curve_offset 10
	0x00,
	0x00, //curve_low_x
	0x00, //blank
	0x00, //curve_low_y
};

static char DSI_GALLERY_DYNAMIC_MDNIE_3[] = {
	0xDD,
	0x01, //mdnie_en
	0x00, //mask 00 0000
	0x03, //edge 00
	0xf0, //ascr algo aolce gamut 00 00 00 00
	0xff, //partial_en1
	0xff, //partial_en2
	0x00, //roi_en
	0x00, //roi_block
	0x00, //roi_sx
	0x00, //roi_sx
	0x00, //roi_sy
	0x00, //roi_sy
	0x00, //roi_ex
	0x00, //roi_ex
	0x00, //roi_ey
	0x00, //roi_ey
	0xff, //trans_on trans_block 0 000 0000
	0x04, //trans_slope
	0x01, //trans_interval
	//end
};

static char DSI_GALLERY_STANDARD_MDNIE_1[] = {
	//start
	0xDF,
	0x11, //linear_on ascr_skin_on 0000 0000
	0x6a, //ascr_skin_cb
	0x9a, //ascr_skin_cr
	0x25, //ascr_dist_up
	0x1a, //ascr_dist_down
	0x16, //ascr_dist_right
	0x2a, //ascr_dist_left
	0x00, //ascr_div_up 20
	0x37,
	0x5a,
	0x00, //ascr_div_down
	0x4e,
	0xc5,
	0x00, //ascr_div_right
	0x5d,
	0x17,
	0x00, //ascr_div_left
	0x30,
	0xc3,
	0xee, //ascr_skin_Rr
	0x4b, //ascr_skin_Rg
	0x30, //ascr_skin_Rb
	0xef, //ascr_skin_Yr
	0xeb, //ascr_skin_Yg
	0x47, //ascr_skin_Yb
	0xfc, //ascr_skin_Mr
	0x57, //ascr_skin_Mg
	0xe7, //ascr_skin_Mb
	0xff, //ascr_skin_Wr
	0xfa, //ascr_skin_Wg
	0xf1, //ascr_skin_Wb
	0x00, //ascr_Cr
	0xee, //ascr_Rr
	0xe7, //ascr_Cg
	0x4b, //ascr_Rg
	0xe5, //ascr_Cb
	0x30, //ascr_Rb
	0xfc, //ascr_Mr
	0x00, //ascr_Gr
	0x57, //ascr_Mg
	0xd9, //ascr_Gg
	0xe7, //ascr_Mb
	0x34, //ascr_Gb
	0xef, //ascr_Yr
	0x34, //ascr_Br
	0xeb, //ascr_Yg
	0x20, //ascr_Bg
	0x47, //ascr_Yb
	0xdb, //ascr_Bb
	0xff, //ascr_Wr
	0x00, //ascr_Kr
	0xfa, //ascr_Wg
	0x00, //ascr_Kg
	0xf1, //ascr_Wb
	0x00, //ascr_Kb
	0x0F, //edge_ctrl
	0x00, //edge_radius 12
	0x6F,
	0x05, //edge_aa_factor
	0x00, //edge_r
	0x00, //edge_g
	0x00, //edge_b
	0x00, //edge_x 11
	0x70,
	0x00, //edge_y 12
	0x70,
	0x44, //edge_coef_a edge_coef_b 0000 0000
};

static char DSI_GALLERY_STANDARD_MDNIE_2[] = {
	0xDE,
	0x00, //gamut_gamma_on gamut_blk_shift 0000 0000
	0x40, //gamut_scale
	0x04, //gamut_r1
	0x00,
	0x00, //gamut_r2
	0x00,
	0x00, //gamut_r3
	0x00,
	0x00, //gamut_g1
	0x00,
	0x04, //gamut_g2
	0x00,
	0x00, //gamut_g3
	0x00,
	0x00, //gamut_b1
	0x00,
	0x00, //gamut_b2
	0x00,
	0x04, //gamut_b3
	0x00,
	0x00, //slce_on cadrx_en 0000 0000
	0x00, //glare_on
	0x00, //lce_gain 000 0000
	0x24, //lce_color_gain 00 0000
	0x96, //lce_min_ref_offset
	0xb3, //lce_illum_gain
	0x01, //lce_ref_offset 9
	0x0e,
	0x01, //lce_ref_gain 9
	0x00,
	0x77, //lce_block_size h v 0000 0000
	0x03, //lce_dark_th 000
	0x07, //lce_reduct_slope 0000
	0x40, //lce_black cc0 cc1 color_th 0 0 0 0000
	0x08, //lce_large_w
	0x04, //lce_med_w
	0x04, //lce_small_w
	0x00, //cadrx_dit_en shadow_corr_en ref_ctrl_en 0 0 0
	0x00, //cadrx_gain
	0x0d, //cadrx_low_intensity_th
	0xe1, //cadrx_high_intensity_th
	0x07, //cadrx_ui_zerobincnt_th
	0x29, //cadrx_ui_ratio_th
	0x04,
	0x0f, //cadrx_entire_freq
	0xa0,
	0x00,
	0x03, //cadrx_peak_th_in_freq
	0x7a,
	0xa0,
	0x00, //cadrx_high_peak_th_in_freq
	0x25,
	0x1c,
	0x24, //cadrx_dit_color_gain
	0x00, //cadrx_dit_illum_a0
	0x48, //cadrx_dit_illum_a1
	0xea, //cadrx_dit_illum_a2
	0xfa, //cadrx_dit_illum_a3
	0x00, //cadrx_dit_illum_b0
	0x10,
	0x00, //cadrx_dit_illum_b1
	0x10,
	0x00, //cadrx_dit_illum_b2
	0x9c,
	0x01, //cadrx_dit_illum_b3
	0x00,
	0x00, //cadrx_dit_illum_slope1
	0x00,
	0x00, //cadrx_dit_illum_slope2
	0xdc,
	0x03, //cadrx_dit_illum_slope3
	0xff,
	0x40, //cadrx_ui_illum_a1
	0x80, //cadrx_ui_illum_a2
	0xc0, //cadrx_ui_illum_a3
	0x00, //cadrx_ui_illum_offset1
	0x9a,
	0x00, //cadrx_ui_illum_offset2
	0x9a,
	0x00, //cadrx_ui_illum_offset3
	0x9a,
	0x00, //cadrx_ui_illum_offset4
	0x9a,
	0x00, //cadrx_ui_illum_slope1
	0x00,
	0x00, //cadrx_ui_illum_slope2
	0x00,
	0x00, //cadrx_ui_illum_slope3
	0x00,
	0x00, //cadrx_ui_illum_slope4
	0x00,
	0x40, //cadrx_ui_ref_a1
	0x80, //cadrx_ui_ref_a2
	0xc0, //cadrx_ui_ref_a3
	0x01, //cadrx_ui_ref_offset1
	0x0e,
	0x01, //cadrx_ui_ref_offset2
	0x0e,
	0x01, //cadrx_ui_ref_offset3
	0x0e,
	0x01, //cadrx_ui_ref_offset4
	0x0e,
	0x00, //cadrx_ui_ref_slope1
	0x00,
	0x00, //cadrx_ui_ref_slope2
	0x00,
	0x00, //cadrx_ui_ref_slope3
	0x00,
	0x00, //cadrx_ui_ref_slope4
	0x00,
	0x01, //cadrx_reg_ref_c1_offset
	0x0e,
	0x01, //cadrx_reg_ref_c2_offset
	0x3a,
	0x01, //cadrx_reg_ref_c3_offset
	0x44,
	0x00, //cadrx_reg_ref_c1_slope
	0x01,
	0x00, //cadrx_reg_ref_c2_slope
	0x43,
	0x00, //cadrx_reg_ref_c3_slope
	0x4b,
	0x00, //cadrx_sc_reg_a1
	0x18,
	0x00, //cadrx_sc_reg_b1
	0x1c,
	0x01, //cadrx_sc_k1_int
	0xff,
	0x01, //cadrx_sc_k2_int
	0xff,
	0x03, //cadrx_sc_w1_int
	0xff,
	0xe8,
	0x03, //cadrx_sc_w2_int
	0xff,
	0xe8,
	0x00, //flicker_w_en
	0x14, //flicker_w
	0x01, //flicker_trans_en
	0x04, //flicker_trans_slope
	0x00, //bl_en
	0x03, //bl_hbs_th
	0x10, //bl_lbs_th
	0xC8, //bl_illum_gain
	0x01, //bl_reg_gain 9
	0x2C,
	0x00, //bl_le_diff
	0x00, //bl_cadrx_gain
	0x04, //bl_trans_slope
	0x00, //le_en
	0x40, //le_diff
	0x03, //le_lmax 10
	0xB6,
	0x23, //le_p
	0x24, //le_w_tmf
	0x00, //le_w_struct_diff 9
	0x72,
	0x01, //le_w_struct_diff_gain
	0x00, //le_w_luminance 9
	0x0D,
	0x00, //le_luminance_slope 10
	0x04,
	0x80, //glare_blk_avg_th 0000 0000
	0x00, //glare_luma_gain 9
	0x28,
	0xa0, //glare_luma_ctl_start 0000 0000
	0x01, //glare_luma_th 0000 0000
	0x00, //glare_luma_step 9
	0x01,
	0x00, //glare_uni_luma_gain 9
	0x01,
	0x00, //glare_blk_max_th 0000 0000
	0x00, //glare_blk_max_w 0000 0000
	0x10, //glare_blk_max_curve blk_max_sh 0 0000
	0x00, //glare_con_ext_max_th 0000 0000
	0x00, //glare_con_ext_max_w 0000 0000
	0x30, //glare_con_ext_max_sign con_ext_max_curve con_ext_max_sh 0 0 0000
	0x00, //glare_max_cnt_th 0000
	0x00, //glare_max_cnt_th 0000 0000
	0x00, //glare_max_cnt_w 0000 0000
	0x30, //glare_max_cnt_sign max_cnt_curve max_cnt_sh 0 0 0000
	0x00, //glare_y_avg_th 0000 0000
	0x00, //glare_y_avg_w 0000 0000
	0x30, //glare_y_avg_sign y_avg_curve y_avg_sh 0 0 0000
	0x00, //glare_max_cnt_th_dmc 0000
	0x00, //glare_max_cnt_th_dmc 0000 0000
	0x06, //nr fa de cs gamma 0 0000
	0xff, //nr_mask_th
	0x00, //de_gain 10
	0x10,
	0x02, //de_maxplus 11
	0x00,
	0x02, //de_maxminus 11
	0x00,
	0x14, //fa_edge_th
	0x00, //fa_step_p 13
	0x01,
	0x00, //fa_step_n 13
	0x01,
	0x00, //fa_max_de_gain 13
	0x10,
	0x00, //fa_pcl_ppi 14
	0x00,
	0x28, //fa_os_cnt_10_co
	0x3c, //fa_os_cnt_20_co
	0x04, //fa_edge_cnt_weight
	0x0f, //fa_avg_y_weight
	0x01, //cs_gain 10
	0x18,
	0x00, //curve_0
	0x10, //curve_1
	0x20, //curve_2
	0x30, //curve_3
	0x40, //curve_4
	0x50, //curve_5
	0x60, //curve_6
	0x70, //curve_7
	0x80, //curve_8
	0x90, //curve_9
	0xa0, //curve_10
	0xb0, //curve_11
	0xc0, //curve_12
	0xd0, //curve_13
	0xe0, //curve_14
	0xf0, //curve_15
	0x01, //curve_16
	0x00,
	0x00, //curve_offset 10
	0x00,
	0x00, //curve_low_x
	0x00, //blank
	0x00, //curve_low_y
};

static char DSI_GALLERY_STANDARD_MDNIE_3[] = {
	0xDD,
	0x01, //mdnie_en
	0x00, //mask 00 0000
	0x03, //edge 00
	0xf0, //ascr algo aolce gamut 00 00 00 00
	0xff, //partial_en1
	0xff, //partial_en2
	0x00, //roi_en
	0x00, //roi_block
	0x00, //roi_sx
	0x00, //roi_sx
	0x00, //roi_sy
	0x00, //roi_sy
	0x00, //roi_ex
	0x00, //roi_ex
	0x00, //roi_ey
	0x00, //roi_ey
	0xff, //trans_on trans_block 0 000 0000
	0x04, //trans_slope
	0x01, //trans_interval
	//end
};

static char DSI_GALLERY_NATURAL_MDNIE_1[] = {
	//start
	0xDF,
	0x11, //linear_on ascr_skin_on 0000 0000
	0x6a, //ascr_skin_cb
	0x9a, //ascr_skin_cr
	0x25, //ascr_dist_up
	0x1a, //ascr_dist_down
	0x16, //ascr_dist_right
	0x2a, //ascr_dist_left
	0x00, //ascr_div_up 20
	0x37,
	0x5a,
	0x00, //ascr_div_down
	0x4e,
	0xc5,
	0x00, //ascr_div_right
	0x5d,
	0x17,
	0x00, //ascr_div_left
	0x30,
	0xc3,
	0xd3, //ascr_skin_Rr
	0x40, //ascr_skin_Rg
	0x29, //ascr_skin_Rb
	0xf0, //ascr_skin_Yr
	0xec, //ascr_skin_Yg
	0x5b, //ascr_skin_Yb
	0xdf, //ascr_skin_Mr
	0x4a, //ascr_skin_Mg
	0xdf, //ascr_skin_Mb
	0xff, //ascr_skin_Wr
	0xfa, //ascr_skin_Wg
	0xf1, //ascr_skin_Wb
	0x93, //ascr_Cr
	0xd3, //ascr_Rr
	0xed, //ascr_Cg
	0x40, //ascr_Rg
	0xe7, //ascr_Cb
	0x29, //ascr_Rb
	0xdf, //ascr_Mr
	0x85, //ascr_Gr
	0x4a, //ascr_Mg
	0xe0, //ascr_Gg
	0xdf, //ascr_Mb
	0x4f, //ascr_Gb
	0xf0, //ascr_Yr
	0x32, //ascr_Br
	0xec, //ascr_Yg
	0x1f, //ascr_Bg
	0x5b, //ascr_Yb
	0xd8, //ascr_Bb
	0xff, //ascr_Wr
	0x00, //ascr_Kr
	0xfa, //ascr_Wg
	0x00, //ascr_Kg
	0xf1, //ascr_Wb
	0x00, //ascr_Kb
	0x0F, //edge_ctrl
	0x00, //edge_radius 12
	0x6F,
	0x05, //edge_aa_factor
	0x00, //edge_r
	0x00, //edge_g
	0x00, //edge_b
	0x00, //edge_x 11
	0x70,
	0x00, //edge_y 12
	0x70,
	0x44, //edge_coef_a edge_coef_b 0000 0000
};

static char DSI_GALLERY_NATURAL_MDNIE_2[] = {
	0xDE,
	0x00, //gamut_gamma_on gamut_blk_shift 0000 0000
	0x40, //gamut_scale
	0x04, //gamut_r1
	0x00,
	0x00, //gamut_r2
	0x00,
	0x00, //gamut_r3
	0x00,
	0x00, //gamut_g1
	0x00,
	0x04, //gamut_g2
	0x00,
	0x00, //gamut_g3
	0x00,
	0x00, //gamut_b1
	0x00,
	0x00, //gamut_b2
	0x00,
	0x04, //gamut_b3
	0x00,
	0x00, //slce_on cadrx_en 0000 0000
	0x00, //glare_on
	0x00, //lce_gain 000 0000
	0x24, //lce_color_gain 00 0000
	0x96, //lce_min_ref_offset
	0xb3, //lce_illum_gain
	0x01, //lce_ref_offset 9
	0x0e,
	0x01, //lce_ref_gain 9
	0x00,
	0x77, //lce_block_size h v 0000 0000
	0x03, //lce_dark_th 000
	0x07, //lce_reduct_slope 0000
	0x40, //lce_black cc0 cc1 color_th 0 0 0 0000
	0x08, //lce_large_w
	0x04, //lce_med_w
	0x04, //lce_small_w
	0x00, //cadrx_dit_en shadow_corr_en ref_ctrl_en 0 0 0
	0x00, //cadrx_gain
	0x0d, //cadrx_low_intensity_th
	0xe1, //cadrx_high_intensity_th
	0x07, //cadrx_ui_zerobincnt_th
	0x29, //cadrx_ui_ratio_th
	0x04,
	0x0f, //cadrx_entire_freq
	0xa0,
	0x00,
	0x03, //cadrx_peak_th_in_freq
	0x7a,
	0xa0,
	0x00, //cadrx_high_peak_th_in_freq
	0x25,
	0x1c,
	0x24, //cadrx_dit_color_gain
	0x00, //cadrx_dit_illum_a0
	0x48, //cadrx_dit_illum_a1
	0xea, //cadrx_dit_illum_a2
	0xfa, //cadrx_dit_illum_a3
	0x00, //cadrx_dit_illum_b0
	0x10,
	0x00, //cadrx_dit_illum_b1
	0x10,
	0x00, //cadrx_dit_illum_b2
	0x9c,
	0x01, //cadrx_dit_illum_b3
	0x00,
	0x00, //cadrx_dit_illum_slope1
	0x00,
	0x00, //cadrx_dit_illum_slope2
	0xdc,
	0x03, //cadrx_dit_illum_slope3
	0xff,
	0x40, //cadrx_ui_illum_a1
	0x80, //cadrx_ui_illum_a2
	0xc0, //cadrx_ui_illum_a3
	0x00, //cadrx_ui_illum_offset1
	0x9a,
	0x00, //cadrx_ui_illum_offset2
	0x9a,
	0x00, //cadrx_ui_illum_offset3
	0x9a,
	0x00, //cadrx_ui_illum_offset4
	0x9a,
	0x00, //cadrx_ui_illum_slope1
	0x00,
	0x00, //cadrx_ui_illum_slope2
	0x00,
	0x00, //cadrx_ui_illum_slope3
	0x00,
	0x00, //cadrx_ui_illum_slope4
	0x00,
	0x40, //cadrx_ui_ref_a1
	0x80, //cadrx_ui_ref_a2
	0xc0, //cadrx_ui_ref_a3
	0x01, //cadrx_ui_ref_offset1
	0x0e,
	0x01, //cadrx_ui_ref_offset2
	0x0e,
	0x01, //cadrx_ui_ref_offset3
	0x0e,
	0x01, //cadrx_ui_ref_offset4
	0x0e,
	0x00, //cadrx_ui_ref_slope1
	0x00,
	0x00, //cadrx_ui_ref_slope2
	0x00,
	0x00, //cadrx_ui_ref_slope3
	0x00,
	0x00, //cadrx_ui_ref_slope4
	0x00,
	0x01, //cadrx_reg_ref_c1_offset
	0x0e,
	0x01, //cadrx_reg_ref_c2_offset
	0x3a,
	0x01, //cadrx_reg_ref_c3_offset
	0x44,
	0x00, //cadrx_reg_ref_c1_slope
	0x01,
	0x00, //cadrx_reg_ref_c2_slope
	0x43,
	0x00, //cadrx_reg_ref_c3_slope
	0x4b,
	0x00, //cadrx_sc_reg_a1
	0x18,
	0x00, //cadrx_sc_reg_b1
	0x1c,
	0x01, //cadrx_sc_k1_int
	0xff,
	0x01, //cadrx_sc_k2_int
	0xff,
	0x03, //cadrx_sc_w1_int
	0xff,
	0xe8,
	0x03, //cadrx_sc_w2_int
	0xff,
	0xe8,
	0x00, //flicker_w_en
	0x14, //flicker_w
	0x01, //flicker_trans_en
	0x04, //flicker_trans_slope
	0x00, //bl_en
	0x03, //bl_hbs_th
	0x10, //bl_lbs_th
	0xC8, //bl_illum_gain
	0x01, //bl_reg_gain 9
	0x2C,
	0x00, //bl_le_diff
	0x00, //bl_cadrx_gain
	0x04, //bl_trans_slope
	0x00, //le_en
	0x40, //le_diff
	0x03, //le_lmax 10
	0xB6,
	0x23, //le_p
	0x24, //le_w_tmf
	0x00, //le_w_struct_diff 9
	0x72,
	0x01, //le_w_struct_diff_gain
	0x00, //le_w_luminance 9
	0x0D,
	0x00, //le_luminance_slope 10
	0x04,
	0x80, //glare_blk_avg_th 0000 0000
	0x00, //glare_luma_gain 9
	0x28,
	0xa0, //glare_luma_ctl_start 0000 0000
	0x01, //glare_luma_th 0000 0000
	0x00, //glare_luma_step 9
	0x01,
	0x00, //glare_uni_luma_gain 9
	0x01,
	0x00, //glare_blk_max_th 0000 0000
	0x00, //glare_blk_max_w 0000 0000
	0x10, //glare_blk_max_curve blk_max_sh 0 0000
	0x00, //glare_con_ext_max_th 0000 0000
	0x00, //glare_con_ext_max_w 0000 0000
	0x30, //glare_con_ext_max_sign con_ext_max_curve con_ext_max_sh 0 0 0000
	0x00, //glare_max_cnt_th 0000
	0x00, //glare_max_cnt_th 0000 0000
	0x00, //glare_max_cnt_w 0000 0000
	0x30, //glare_max_cnt_sign max_cnt_curve max_cnt_sh 0 0 0000
	0x00, //glare_y_avg_th 0000 0000
	0x00, //glare_y_avg_w 0000 0000
	0x30, //glare_y_avg_sign y_avg_curve y_avg_sh 0 0 0000
	0x00, //glare_max_cnt_th_dmc 0000
	0x00, //glare_max_cnt_th_dmc 0000 0000
	0x04, //nr fa de cs gamma 0 0000
	0xff, //nr_mask_th
	0x00, //de_gain 10
	0x10,
	0x02, //de_maxplus 11
	0x00,
	0x02, //de_maxminus 11
	0x00,
	0x14, //fa_edge_th
	0x00, //fa_step_p 13
	0x01,
	0x00, //fa_step_n 13
	0x01,
	0x00, //fa_max_de_gain 13
	0x10,
	0x00, //fa_pcl_ppi 14
	0x00,
	0x28, //fa_os_cnt_10_co
	0x3c, //fa_os_cnt_20_co
	0x04, //fa_edge_cnt_weight
	0x0f, //fa_avg_y_weight
	0x01, //cs_gain 10
	0x00,
	0x00, //curve_0
	0x10, //curve_1
	0x20, //curve_2
	0x30, //curve_3
	0x40, //curve_4
	0x50, //curve_5
	0x60, //curve_6
	0x70, //curve_7
	0x80, //curve_8
	0x90, //curve_9
	0xa0, //curve_10
	0xb0, //curve_11
	0xc0, //curve_12
	0xd0, //curve_13
	0xe0, //curve_14
	0xf0, //curve_15
	0x01, //curve_16
	0x00,
	0x00, //curve_offset 10
	0x00,
	0x00, //curve_low_x
	0x00, //blank
	0x00, //curve_low_y
};

static char DSI_GALLERY_NATURAL_MDNIE_3[] = {
	0xDD,
	0x01, //mdnie_en
	0x00, //mask 00 0000
	0x03, //edge 00
	0xf0, //ascr algo aolce gamut 00 00 00 00
	0xff, //partial_en1
	0xff, //partial_en2
	0x00, //roi_en
	0x00, //roi_block
	0x00, //roi_sx
	0x00, //roi_sx
	0x00, //roi_sy
	0x00, //roi_sy
	0x00, //roi_ex
	0x00, //roi_ex
	0x00, //roi_ey
	0x00, //roi_ey
	0xff, //trans_on trans_block 0 000 0000
	0x04, //trans_slope
	0x01, //trans_interval
	//end
};

static char DSI_GALLERY_AUTO_MDNIE_1[] = {
	//start
	0xDF,
	0x01, //linear_on ascr_skin_on 0000 0000
	0x6a, //ascr_skin_cb
	0x9a, //ascr_skin_cr
	0x25, //ascr_dist_up
	0x1a, //ascr_dist_down
	0x16, //ascr_dist_right
	0x2a, //ascr_dist_left
	0x00, //ascr_div_up 20
	0x37,
	0x5a,
	0x00, //ascr_div_down
	0x4e,
	0xc5,
	0x00, //ascr_div_right
	0x5d,
	0x17,
	0x00, //ascr_div_left
	0x30,
	0xc3,
	0xff, //ascr_skin_Rr
	0x48, //ascr_skin_Rg
	0x58, //ascr_skin_Rb
	0xff, //ascr_skin_Yr
	0xf8, //ascr_skin_Yg
	0x00, //ascr_skin_Yb
	0xfe, //ascr_skin_Mr
	0x00, //ascr_skin_Mg
	0xff, //ascr_skin_Mb
	0xff, //ascr_skin_Wr
	0xff, //ascr_skin_Wg
	0xff, //ascr_skin_Wb
	0x00, //ascr_Cr
	0xf0, //ascr_Rr
	0xff, //ascr_Cg
	0x00, //ascr_Rg
	0xfa, //ascr_Cb
	0x00, //ascr_Rb
	0xfe, //ascr_Mr
	0x00, //ascr_Gr
	0x00, //ascr_Mg
	0xff, //ascr_Gg
	0xff, //ascr_Mb
	0x00, //ascr_Gb
	0xff, //ascr_Yr
	0x00, //ascr_Br
	0xf8, //ascr_Yg
	0x00, //ascr_Bg
	0x00, //ascr_Yb
	0xff, //ascr_Bb
	0xff, //ascr_Wr
	0x00, //ascr_Kr
	0xff, //ascr_Wg
	0x00, //ascr_Kg
	0xff, //ascr_Wb
	0x00, //ascr_Kb
	0x0F, //edge_ctrl
	0x00, //edge_radius 12
	0x6F,
	0x05, //edge_aa_factor
	0x00, //edge_r
	0x00, //edge_g
	0x00, //edge_b
	0x00, //edge_x 11
	0x70,
	0x00, //edge_y 12
	0x70,
	0x44, //edge_coef_a edge_coef_b 0000 0000
};

static char DSI_GALLERY_AUTO_MDNIE_2[] = {
	0xDE,
	0x00, //gamut_gamma_on gamut_blk_shift 0000 0000
	0x40, //gamut_scale
	0x04, //gamut_r1
	0x00,
	0x00, //gamut_r2
	0x00,
	0x00, //gamut_r3
	0x00,
	0x00, //gamut_g1
	0x00,
	0x04, //gamut_g2
	0x00,
	0x00, //gamut_g3
	0x00,
	0x00, //gamut_b1
	0x00,
	0x00, //gamut_b2
	0x00,
	0x04, //gamut_b3
	0x00,
	0x00, //slce_on cadrx_en 0000 0000
	0x00, //glare_on
	0x00, //lce_gain 000 0000
	0x24, //lce_color_gain 00 0000
	0x96, //lce_min_ref_offset
	0xb3, //lce_illum_gain
	0x01, //lce_ref_offset 9
	0x0e,
	0x01, //lce_ref_gain 9
	0x00,
	0x77, //lce_block_size h v 0000 0000
	0x03, //lce_dark_th 000
	0x07, //lce_reduct_slope 0000
	0x40, //lce_black cc0 cc1 color_th 0 0 0 0000
	0x08, //lce_large_w
	0x04, //lce_med_w
	0x04, //lce_small_w
	0x00, //cadrx_dit_en shadow_corr_en ref_ctrl_en 0 0 0
	0x00, //cadrx_gain
	0x0d, //cadrx_low_intensity_th
	0xe1, //cadrx_high_intensity_th
	0x07, //cadrx_ui_zerobincnt_th
	0x29, //cadrx_ui_ratio_th
	0x04,
	0x0f, //cadrx_entire_freq
	0xa0,
	0x00,
	0x03, //cadrx_peak_th_in_freq
	0x7a,
	0xa0,
	0x00, //cadrx_high_peak_th_in_freq
	0x25,
	0x1c,
	0x24, //cadrx_dit_color_gain
	0x00, //cadrx_dit_illum_a0
	0x48, //cadrx_dit_illum_a1
	0xea, //cadrx_dit_illum_a2
	0xfa, //cadrx_dit_illum_a3
	0x00, //cadrx_dit_illum_b0
	0x10,
	0x00, //cadrx_dit_illum_b1
	0x10,
	0x00, //cadrx_dit_illum_b2
	0x9c,
	0x01, //cadrx_dit_illum_b3
	0x00,
	0x00, //cadrx_dit_illum_slope1
	0x00,
	0x00, //cadrx_dit_illum_slope2
	0xdc,
	0x03, //cadrx_dit_illum_slope3
	0xff,
	0x40, //cadrx_ui_illum_a1
	0x80, //cadrx_ui_illum_a2
	0xc0, //cadrx_ui_illum_a3
	0x00, //cadrx_ui_illum_offset1
	0x9a,
	0x00, //cadrx_ui_illum_offset2
	0x9a,
	0x00, //cadrx_ui_illum_offset3
	0x9a,
	0x00, //cadrx_ui_illum_offset4
	0x9a,
	0x00, //cadrx_ui_illum_slope1
	0x00,
	0x00, //cadrx_ui_illum_slope2
	0x00,
	0x00, //cadrx_ui_illum_slope3
	0x00,
	0x00, //cadrx_ui_illum_slope4
	0x00,
	0x40, //cadrx_ui_ref_a1
	0x80, //cadrx_ui_ref_a2
	0xc0, //cadrx_ui_ref_a3
	0x01, //cadrx_ui_ref_offset1
	0x0e,
	0x01, //cadrx_ui_ref_offset2
	0x0e,
	0x01, //cadrx_ui_ref_offset3
	0x0e,
	0x01, //cadrx_ui_ref_offset4
	0x0e,
	0x00, //cadrx_ui_ref_slope1
	0x00,
	0x00, //cadrx_ui_ref_slope2
	0x00,
	0x00, //cadrx_ui_ref_slope3
	0x00,
	0x00, //cadrx_ui_ref_slope4
	0x00,
	0x01, //cadrx_reg_ref_c1_offset
	0x0e,
	0x01, //cadrx_reg_ref_c2_offset
	0x3a,
	0x01, //cadrx_reg_ref_c3_offset
	0x44,
	0x00, //cadrx_reg_ref_c1_slope
	0x01,
	0x00, //cadrx_reg_ref_c2_slope
	0x43,
	0x00, //cadrx_reg_ref_c3_slope
	0x4b,
	0x00, //cadrx_sc_reg_a1
	0x18,
	0x00, //cadrx_sc_reg_b1
	0x1c,
	0x01, //cadrx_sc_k1_int
	0xff,
	0x01, //cadrx_sc_k2_int
	0xff,
	0x03, //cadrx_sc_w1_int
	0xff,
	0xe8,
	0x03, //cadrx_sc_w2_int
	0xff,
	0xe8,
	0x00, //flicker_w_en
	0x14, //flicker_w
	0x01, //flicker_trans_en
	0x04, //flicker_trans_slope
	0x00, //bl_en
	0x03, //bl_hbs_th
	0x10, //bl_lbs_th
	0xC8, //bl_illum_gain
	0x01, //bl_reg_gain 9
	0x2C,
	0x00, //bl_le_diff
	0x00, //bl_cadrx_gain
	0x04, //bl_trans_slope
	0x00, //le_en
	0x40, //le_diff
	0x03, //le_lmax 10
	0xB6,
	0x23, //le_p
	0x24, //le_w_tmf
	0x00, //le_w_struct_diff 9
	0x72,
	0x01, //le_w_struct_diff_gain
	0x00, //le_w_luminance 9
	0x0D,
	0x00, //le_luminance_slope 10
	0x04,
	0x80, //glare_blk_avg_th 0000 0000
	0x00, //glare_luma_gain 9
	0x28,
	0xa0, //glare_luma_ctl_start 0000 0000
	0x01, //glare_luma_th 0000 0000
	0x00, //glare_luma_step 9
	0x01,
	0x00, //glare_uni_luma_gain 9
	0x01,
	0xf0, //glare_blk_max_th 0000 0000
	0x00, //glare_blk_max_w 0000 0000
	0x18, //glare_blk_max_curve blk_max_sh 0 0000
	0xf0, //glare_con_ext_max_th 0000 0000
	0x00, //glare_con_ext_max_w 0000 0000
	0x38, //glare_con_ext_max_sign con_ext_max_curve con_ext_max_sh 0 0 0000
	0x00, //glare_max_cnt_th 0000
	0x00, //glare_max_cnt_th 0000 0000
	0x00, //glare_max_cnt_w 0000 0000
	0x38, //glare_max_cnt_sign max_cnt_curve max_cnt_sh 0 0 0000
	0xba, //glare_y_avg_th 0000 0000
	0x00, //glare_y_avg_w 0000 0000
	0x38, //glare_y_avg_sign y_avg_curve y_avg_sh 0 0 0000
	0x00, //glare_max_cnt_th_dmc 0000
	0x05, //glare_max_cnt_th_dmc 0000 0000
	0x0c, //nr fa de cs gamma 0 0000
	0xff, //nr_mask_th
	0x00, //de_gain 10
	0x10,
	0x02, //de_maxplus 11
	0x00,
	0x02, //de_maxminus 11
	0x00,
	0x14, //fa_edge_th
	0x00, //fa_step_p 13
	0x01,
	0x00, //fa_step_n 13
	0x01,
	0x00, //fa_max_de_gain 13
	0x10,
	0x00, //fa_pcl_ppi 14
	0x00,
	0x28, //fa_os_cnt_10_co
	0x3c, //fa_os_cnt_20_co
	0x04, //fa_edge_cnt_weight
	0x0f, //fa_avg_y_weight
	0x01, //cs_gain 10
	0x00,
	0x00, //curve_0
	0x10, //curve_1
	0x20, //curve_2
	0x30, //curve_3
	0x40, //curve_4
	0x50, //curve_5
	0x60, //curve_6
	0x70, //curve_7
	0x80, //curve_8
	0x90, //curve_9
	0xa0, //curve_10
	0xb0, //curve_11
	0xc0, //curve_12
	0xd0, //curve_13
	0xe0, //curve_14
	0xf0, //curve_15
	0x01, //curve_16
	0x00,
	0x00, //curve_offset 10
	0x00,
	0x00, //curve_low_x
	0x00, //blank
	0x00, //curve_low_y
};

static char DSI_GALLERY_AUTO_MDNIE_3[] = {
	0xDD,
	0x01, //mdnie_en
	0x00, //mask 00 0000
	0x03, //edge 00
	0xf0, //ascr algo aolce gamut 00 00 00 00
	0xff, //partial_en1
	0xff, //partial_en2
	0x00, //roi_en
	0x00, //roi_block
	0x00, //roi_sx
	0x00, //roi_sx
	0x00, //roi_sy
	0x00, //roi_sy
	0x00, //roi_ex
	0x00, //roi_ex
	0x00, //roi_ey
	0x00, //roi_ey
	0xff, //trans_on trans_block 0 000 0000
	0x04, //trans_slope
	0x01, //trans_interval
	//end
};

static char DSI_BROWSER_DYNAMIC_MDNIE_1[] = {
	//start
	0xDF,
	0x11, //linear_on ascr_skin_on 0000 0000
	0x67, //ascr_skin_cb
	0xa9, //ascr_skin_cr
	0x37, //ascr_dist_up
	0x29, //ascr_dist_down
	0x19, //ascr_dist_right
	0x47, //ascr_dist_left
	0x00, //ascr_div_up 20
	0x25,
	0x3d,
	0x00, //ascr_div_down
	0x31,
	0xf4,
	0x00, //ascr_div_right
	0x51,
	0xec,
	0x00, //ascr_div_left
	0x1c,
	0xd8,
	0xe1, //ascr_skin_Rr
	0x25, //ascr_skin_Rg
	0x00, //ascr_skin_Rb
	0xee, //ascr_skin_Yr
	0xea, //ascr_skin_Yg
	0x00, //ascr_skin_Yb
	0xf0, //ascr_skin_Mr
	0x33, //ascr_skin_Mg
	0xe8, //ascr_skin_Mb
	0xff, //ascr_skin_Wr
	0xfa, //ascr_skin_Wg
	0xf1, //ascr_skin_Wb
	0x5c, //ascr_Cr
	0xe1, //ascr_Rr
	0xef, //ascr_Cg
	0x25, //ascr_Rg
	0xe8, //ascr_Cb
	0x00, //ascr_Rb
	0xf0, //ascr_Mr
	0x49, //ascr_Gr
	0x33, //ascr_Mg
	0xdf, //ascr_Gg
	0xe8, //ascr_Mb
	0x00, //ascr_Gb
	0xee, //ascr_Yr
	0x37, //ascr_Br
	0xea, //ascr_Yg
	0x21, //ascr_Bg
	0x00, //ascr_Yb
	0xdf, //ascr_Bb
	0xff, //ascr_Wr
	0x00, //ascr_Kr
	0xfa, //ascr_Wg
	0x00, //ascr_Kg
	0xf1, //ascr_Wb
	0x00, //ascr_Kb
	0x0F, //edge_ctrl
	0x00, //edge_radius 12
	0x6F,
	0x05, //edge_aa_factor
	0x00, //edge_r
	0x00, //edge_g
	0x00, //edge_b
	0x00, //edge_x 11
	0x70,
	0x00, //edge_y 12
	0x70,
	0x44, //edge_coef_a edge_coef_b 0000 0000
};

static char DSI_BROWSER_DYNAMIC_MDNIE_2[] = {
	0xDE,
	0x00, //gamut_gamma_on gamut_blk_shift 0000 0000
	0x40, //gamut_scale
	0x04, //gamut_r1
	0x00,
	0x00, //gamut_r2
	0x00,
	0x00, //gamut_r3
	0x00,
	0x00, //gamut_g1
	0x00,
	0x04, //gamut_g2
	0x00,
	0x00, //gamut_g3
	0x00,
	0x00, //gamut_b1
	0x00,
	0x00, //gamut_b2
	0x00,
	0x04, //gamut_b3
	0x00,
	0x00, //slce_on cadrx_en 0000 0000
	0x00, //glare_on
	0x00, //lce_gain 000 0000
	0x24, //lce_color_gain 00 0000
	0x96, //lce_min_ref_offset
	0xb3, //lce_illum_gain
	0x01, //lce_ref_offset 9
	0x0e,
	0x01, //lce_ref_gain 9
	0x00,
	0x77, //lce_block_size h v 0000 0000
	0x03, //lce_dark_th 000
	0x07, //lce_reduct_slope 0000
	0x40, //lce_black cc0 cc1 color_th 0 0 0 0000
	0x08, //lce_large_w
	0x04, //lce_med_w
	0x04, //lce_small_w
	0x00, //cadrx_dit_en shadow_corr_en ref_ctrl_en 0 0 0
	0x00, //cadrx_gain
	0x0d, //cadrx_low_intensity_th
	0xe1, //cadrx_high_intensity_th
	0x07, //cadrx_ui_zerobincnt_th
	0x29, //cadrx_ui_ratio_th
	0x04,
	0x0f, //cadrx_entire_freq
	0xa0,
	0x00,
	0x03, //cadrx_peak_th_in_freq
	0x7a,
	0xa0,
	0x00, //cadrx_high_peak_th_in_freq
	0x25,
	0x1c,
	0x24, //cadrx_dit_color_gain
	0x00, //cadrx_dit_illum_a0
	0x48, //cadrx_dit_illum_a1
	0xea, //cadrx_dit_illum_a2
	0xfa, //cadrx_dit_illum_a3
	0x00, //cadrx_dit_illum_b0
	0x10,
	0x00, //cadrx_dit_illum_b1
	0x10,
	0x00, //cadrx_dit_illum_b2
	0x9c,
	0x01, //cadrx_dit_illum_b3
	0x00,
	0x00, //cadrx_dit_illum_slope1
	0x00,
	0x00, //cadrx_dit_illum_slope2
	0xdc,
	0x03, //cadrx_dit_illum_slope3
	0xff,
	0x40, //cadrx_ui_illum_a1
	0x80, //cadrx_ui_illum_a2
	0xc0, //cadrx_ui_illum_a3
	0x00, //cadrx_ui_illum_offset1
	0x9a,
	0x00, //cadrx_ui_illum_offset2
	0x9a,
	0x00, //cadrx_ui_illum_offset3
	0x9a,
	0x00, //cadrx_ui_illum_offset4
	0x9a,
	0x00, //cadrx_ui_illum_slope1
	0x00,
	0x00, //cadrx_ui_illum_slope2
	0x00,
	0x00, //cadrx_ui_illum_slope3
	0x00,
	0x00, //cadrx_ui_illum_slope4
	0x00,
	0x40, //cadrx_ui_ref_a1
	0x80, //cadrx_ui_ref_a2
	0xc0, //cadrx_ui_ref_a3
	0x01, //cadrx_ui_ref_offset1
	0x0e,
	0x01, //cadrx_ui_ref_offset2
	0x0e,
	0x01, //cadrx_ui_ref_offset3
	0x0e,
	0x01, //cadrx_ui_ref_offset4
	0x0e,
	0x00, //cadrx_ui_ref_slope1
	0x00,
	0x00, //cadrx_ui_ref_slope2
	0x00,
	0x00, //cadrx_ui_ref_slope3
	0x00,
	0x00, //cadrx_ui_ref_slope4
	0x00,
	0x01, //cadrx_reg_ref_c1_offset
	0x0e,
	0x01, //cadrx_reg_ref_c2_offset
	0x3a,
	0x01, //cadrx_reg_ref_c3_offset
	0x44,
	0x00, //cadrx_reg_ref_c1_slope
	0x01,
	0x00, //cadrx_reg_ref_c2_slope
	0x43,
	0x00, //cadrx_reg_ref_c3_slope
	0x4b,
	0x00, //cadrx_sc_reg_a1
	0x18,
	0x00, //cadrx_sc_reg_b1
	0x1c,
	0x01, //cadrx_sc_k1_int
	0xff,
	0x01, //cadrx_sc_k2_int
	0xff,
	0x03, //cadrx_sc_w1_int
	0xff,
	0xe8,
	0x03, //cadrx_sc_w2_int
	0xff,
	0xe8,
	0x00, //flicker_w_en
	0x14, //flicker_w
	0x01, //flicker_trans_en
	0x04, //flicker_trans_slope
	0x00, //bl_en
	0x03, //bl_hbs_th
	0x10, //bl_lbs_th
	0xC8, //bl_illum_gain
	0x01, //bl_reg_gain 9
	0x2C,
	0x00, //bl_le_diff
	0x00, //bl_cadrx_gain
	0x04, //bl_trans_slope
	0x00, //le_en
	0x40, //le_diff
	0x03, //le_lmax 10
	0xB6,
	0x23, //le_p
	0x24, //le_w_tmf
	0x00, //le_w_struct_diff 9
	0x72,
	0x01, //le_w_struct_diff_gain
	0x00, //le_w_luminance 9
	0x0D,
	0x00, //le_luminance_slope 10
	0x04,
	0x80, //glare_blk_avg_th 0000 0000
	0x00, //glare_luma_gain 9
	0x28,
	0xa0, //glare_luma_ctl_start 0000 0000
	0x01, //glare_luma_th 0000 0000
	0x00, //glare_luma_step 9
	0x01,
	0x00, //glare_uni_luma_gain 9
	0x01,
	0x00, //glare_blk_max_th 0000 0000
	0x00, //glare_blk_max_w 0000 0000
	0x10, //glare_blk_max_curve blk_max_sh 0 0000
	0x00, //glare_con_ext_max_th 0000 0000
	0x00, //glare_con_ext_max_w 0000 0000
	0x30, //glare_con_ext_max_sign con_ext_max_curve con_ext_max_sh 0 0 0000
	0x00, //glare_max_cnt_th 0000
	0x00, //glare_max_cnt_th 0000 0000
	0x00, //glare_max_cnt_w 0000 0000
	0x30, //glare_max_cnt_sign max_cnt_curve max_cnt_sh 0 0 0000
	0x00, //glare_y_avg_th 0000 0000
	0x00, //glare_y_avg_w 0000 0000
	0x30, //glare_y_avg_sign y_avg_curve y_avg_sh 0 0 0000
	0x00, //glare_max_cnt_th_dmc 0000
	0x00, //glare_max_cnt_th_dmc 0000 0000
	0x00, //nr fa de cs gamma 0 0000
	0xff, //nr_mask_th
	0x00, //de_gain 10
	0x00,
	0x02, //de_maxplus 11
	0x00,
	0x02, //de_maxminus 11
	0x00,
	0x14, //fa_edge_th
	0x00, //fa_step_p 13
	0x01,
	0x00, //fa_step_n 13
	0x01,
	0x00, //fa_max_de_gain 13
	0x10,
	0x00, //fa_pcl_ppi 14
	0x00,
	0x28, //fa_os_cnt_10_co
	0x3c, //fa_os_cnt_20_co
	0x04, //fa_edge_cnt_weight
	0x0f, //fa_avg_y_weight
	0x01, //cs_gain 10
	0x00,
	0x00, //curve_0
	0x10, //curve_1
	0x20, //curve_2
	0x30, //curve_3
	0x40, //curve_4
	0x50, //curve_5
	0x60, //curve_6
	0x70, //curve_7
	0x80, //curve_8
	0x90, //curve_9
	0xa0, //curve_10
	0xb0, //curve_11
	0xc0, //curve_12
	0xd0, //curve_13
	0xe0, //curve_14
	0xf0, //curve_15
	0x01, //curve_16
	0x00,
	0x00, //curve_offset 10
	0x00,
	0x00, //curve_low_x
	0x00, //blank
	0x00, //curve_low_y
};

static char DSI_BROWSER_DYNAMIC_MDNIE_3[] = {
	0xDD,
	0x01, //mdnie_en
	0x00, //mask 00 0000
	0x03, //edge 00
	0xf0, //ascr algo aolce gamut 00 00 00 00
	0xff, //partial_en1
	0xff, //partial_en2
	0x00, //roi_en
	0x00, //roi_block
	0x00, //roi_sx
	0x00, //roi_sx
	0x00, //roi_sy
	0x00, //roi_sy
	0x00, //roi_ex
	0x00, //roi_ex
	0x00, //roi_ey
	0x00, //roi_ey
	0xff, //trans_on trans_block 0 000 0000
	0x04, //trans_slope
	0x01, //trans_interval
	//end
};

static char DSI_BROWSER_STANDARD_MDNIE_1[] = {
	//start
	0xDF,
	0x11, //linear_on ascr_skin_on 0000 0000
	0x6a, //ascr_skin_cb
	0x9a, //ascr_skin_cr
	0x25, //ascr_dist_up
	0x1a, //ascr_dist_down
	0x16, //ascr_dist_right
	0x2a, //ascr_dist_left
	0x00, //ascr_div_up 20
	0x37,
	0x5a,
	0x00, //ascr_div_down
	0x4e,
	0xc5,
	0x00, //ascr_div_right
	0x5d,
	0x17,
	0x00, //ascr_div_left
	0x30,
	0xc3,
	0xee, //ascr_skin_Rr
	0x4b, //ascr_skin_Rg
	0x30, //ascr_skin_Rb
	0xef, //ascr_skin_Yr
	0xeb, //ascr_skin_Yg
	0x47, //ascr_skin_Yb
	0xfc, //ascr_skin_Mr
	0x57, //ascr_skin_Mg
	0xe7, //ascr_skin_Mb
	0xff, //ascr_skin_Wr
	0xfa, //ascr_skin_Wg
	0xf1, //ascr_skin_Wb
	0x00, //ascr_Cr
	0xee, //ascr_Rr
	0xe7, //ascr_Cg
	0x4b, //ascr_Rg
	0xe5, //ascr_Cb
	0x30, //ascr_Rb
	0xfc, //ascr_Mr
	0x00, //ascr_Gr
	0x57, //ascr_Mg
	0xd9, //ascr_Gg
	0xe7, //ascr_Mb
	0x34, //ascr_Gb
	0xef, //ascr_Yr
	0x34, //ascr_Br
	0xeb, //ascr_Yg
	0x20, //ascr_Bg
	0x47, //ascr_Yb
	0xdb, //ascr_Bb
	0xff, //ascr_Wr
	0x00, //ascr_Kr
	0xfa, //ascr_Wg
	0x00, //ascr_Kg
	0xf1, //ascr_Wb
	0x00, //ascr_Kb
	0x0F, //edge_ctrl
	0x00, //edge_radius 12
	0x6F,
	0x05, //edge_aa_factor
	0x00, //edge_r
	0x00, //edge_g
	0x00, //edge_b
	0x00, //edge_x 11
	0x70,
	0x00, //edge_y 12
	0x70,
	0x44, //edge_coef_a edge_coef_b 0000 0000
};

static char DSI_BROWSER_STANDARD_MDNIE_2[] = {
	0xDE,
	0x00, //gamut_gamma_on gamut_blk_shift 0000 0000
	0x40, //gamut_scale
	0x04, //gamut_r1
	0x00,
	0x00, //gamut_r2
	0x00,
	0x00, //gamut_r3
	0x00,
	0x00, //gamut_g1
	0x00,
	0x04, //gamut_g2
	0x00,
	0x00, //gamut_g3
	0x00,
	0x00, //gamut_b1
	0x00,
	0x00, //gamut_b2
	0x00,
	0x04, //gamut_b3
	0x00,
	0x00, //slce_on cadrx_en 0000 0000
	0x00, //glare_on
	0x00, //lce_gain 000 0000
	0x24, //lce_color_gain 00 0000
	0x96, //lce_min_ref_offset
	0xb3, //lce_illum_gain
	0x01, //lce_ref_offset 9
	0x0e,
	0x01, //lce_ref_gain 9
	0x00,
	0x77, //lce_block_size h v 0000 0000
	0x03, //lce_dark_th 000
	0x07, //lce_reduct_slope 0000
	0x40, //lce_black cc0 cc1 color_th 0 0 0 0000
	0x08, //lce_large_w
	0x04, //lce_med_w
	0x04, //lce_small_w
	0x00, //cadrx_dit_en shadow_corr_en ref_ctrl_en 0 0 0
	0x00, //cadrx_gain
	0x0d, //cadrx_low_intensity_th
	0xe1, //cadrx_high_intensity_th
	0x07, //cadrx_ui_zerobincnt_th
	0x29, //cadrx_ui_ratio_th
	0x04,
	0x0f, //cadrx_entire_freq
	0xa0,
	0x00,
	0x03, //cadrx_peak_th_in_freq
	0x7a,
	0xa0,
	0x00, //cadrx_high_peak_th_in_freq
	0x25,
	0x1c,
	0x24, //cadrx_dit_color_gain
	0x00, //cadrx_dit_illum_a0
	0x48, //cadrx_dit_illum_a1
	0xea, //cadrx_dit_illum_a2
	0xfa, //cadrx_dit_illum_a3
	0x00, //cadrx_dit_illum_b0
	0x10,
	0x00, //cadrx_dit_illum_b1
	0x10,
	0x00, //cadrx_dit_illum_b2
	0x9c,
	0x01, //cadrx_dit_illum_b3
	0x00,
	0x00, //cadrx_dit_illum_slope1
	0x00,
	0x00, //cadrx_dit_illum_slope2
	0xdc,
	0x03, //cadrx_dit_illum_slope3
	0xff,
	0x40, //cadrx_ui_illum_a1
	0x80, //cadrx_ui_illum_a2
	0xc0, //cadrx_ui_illum_a3
	0x00, //cadrx_ui_illum_offset1
	0x9a,
	0x00, //cadrx_ui_illum_offset2
	0x9a,
	0x00, //cadrx_ui_illum_offset3
	0x9a,
	0x00, //cadrx_ui_illum_offset4
	0x9a,
	0x00, //cadrx_ui_illum_slope1
	0x00,
	0x00, //cadrx_ui_illum_slope2
	0x00,
	0x00, //cadrx_ui_illum_slope3
	0x00,
	0x00, //cadrx_ui_illum_slope4
	0x00,
	0x40, //cadrx_ui_ref_a1
	0x80, //cadrx_ui_ref_a2
	0xc0, //cadrx_ui_ref_a3
	0x01, //cadrx_ui_ref_offset1
	0x0e,
	0x01, //cadrx_ui_ref_offset2
	0x0e,
	0x01, //cadrx_ui_ref_offset3
	0x0e,
	0x01, //cadrx_ui_ref_offset4
	0x0e,
	0x00, //cadrx_ui_ref_slope1
	0x00,
	0x00, //cadrx_ui_ref_slope2
	0x00,
	0x00, //cadrx_ui_ref_slope3
	0x00,
	0x00, //cadrx_ui_ref_slope4
	0x00,
	0x01, //cadrx_reg_ref_c1_offset
	0x0e,
	0x01, //cadrx_reg_ref_c2_offset
	0x3a,
	0x01, //cadrx_reg_ref_c3_offset
	0x44,
	0x00, //cadrx_reg_ref_c1_slope
	0x01,
	0x00, //cadrx_reg_ref_c2_slope
	0x43,
	0x00, //cadrx_reg_ref_c3_slope
	0x4b,
	0x00, //cadrx_sc_reg_a1
	0x18,
	0x00, //cadrx_sc_reg_b1
	0x1c,
	0x01, //cadrx_sc_k1_int
	0xff,
	0x01, //cadrx_sc_k2_int
	0xff,
	0x03, //cadrx_sc_w1_int
	0xff,
	0xe8,
	0x03, //cadrx_sc_w2_int
	0xff,
	0xe8,
	0x00, //flicker_w_en
	0x14, //flicker_w
	0x01, //flicker_trans_en
	0x04, //flicker_trans_slope
	0x00, //bl_en
	0x03, //bl_hbs_th
	0x10, //bl_lbs_th
	0xC8, //bl_illum_gain
	0x01, //bl_reg_gain 9
	0x2C,
	0x00, //bl_le_diff
	0x00, //bl_cadrx_gain
	0x04, //bl_trans_slope
	0x00, //le_en
	0x40, //le_diff
	0x03, //le_lmax 10
	0xB6,
	0x23, //le_p
	0x24, //le_w_tmf
	0x00, //le_w_struct_diff 9
	0x72,
	0x01, //le_w_struct_diff_gain
	0x00, //le_w_luminance 9
	0x0D,
	0x00, //le_luminance_slope 10
	0x04,
	0x80, //glare_blk_avg_th 0000 0000
	0x00, //glare_luma_gain 9
	0x28,
	0xa0, //glare_luma_ctl_start 0000 0000
	0x01, //glare_luma_th 0000 0000
	0x00, //glare_luma_step 9
	0x01,
	0x00, //glare_uni_luma_gain 9
	0x01,
	0x00, //glare_blk_max_th 0000 0000
	0x00, //glare_blk_max_w 0000 0000
	0x10, //glare_blk_max_curve blk_max_sh 0 0000
	0x00, //glare_con_ext_max_th 0000 0000
	0x00, //glare_con_ext_max_w 0000 0000
	0x30, //glare_con_ext_max_sign con_ext_max_curve con_ext_max_sh 0 0 0000
	0x00, //glare_max_cnt_th 0000
	0x00, //glare_max_cnt_th 0000 0000
	0x00, //glare_max_cnt_w 0000 0000
	0x30, //glare_max_cnt_sign max_cnt_curve max_cnt_sh 0 0 0000
	0x00, //glare_y_avg_th 0000 0000
	0x00, //glare_y_avg_w 0000 0000
	0x30, //glare_y_avg_sign y_avg_curve y_avg_sh 0 0 0000
	0x00, //glare_max_cnt_th_dmc 0000
	0x00, //glare_max_cnt_th_dmc 0000 0000
	0x02, //nr fa de cs gamma 0 0000
	0xff, //nr_mask_th
	0x00, //de_gain 10
	0x00,
	0x02, //de_maxplus 11
	0x00,
	0x02, //de_maxminus 11
	0x00,
	0x14, //fa_edge_th
	0x00, //fa_step_p 13
	0x01,
	0x00, //fa_step_n 13
	0x01,
	0x00, //fa_max_de_gain 13
	0x10,
	0x00, //fa_pcl_ppi 14
	0x00,
	0x28, //fa_os_cnt_10_co
	0x3c, //fa_os_cnt_20_co
	0x04, //fa_edge_cnt_weight
	0x0f, //fa_avg_y_weight
	0x01, //cs_gain 10
	0x18,
	0x00, //curve_0
	0x10, //curve_1
	0x20, //curve_2
	0x30, //curve_3
	0x40, //curve_4
	0x50, //curve_5
	0x60, //curve_6
	0x70, //curve_7
	0x80, //curve_8
	0x90, //curve_9
	0xa0, //curve_10
	0xb0, //curve_11
	0xc0, //curve_12
	0xd0, //curve_13
	0xe0, //curve_14
	0xf0, //curve_15
	0x01, //curve_16
	0x00,
	0x00, //curve_offset 10
	0x00,
	0x00, //curve_low_x
	0x00, //blank
	0x00, //curve_low_y
};

static char DSI_BROWSER_STANDARD_MDNIE_3[] = {
	0xDD,
	0x01, //mdnie_en
	0x00, //mask 00 0000
	0x03, //edge 00
	0xf0, //ascr algo aolce gamut 00 00 00 00
	0xff, //partial_en1
	0xff, //partial_en2
	0x00, //roi_en
	0x00, //roi_block
	0x00, //roi_sx
	0x00, //roi_sx
	0x00, //roi_sy
	0x00, //roi_sy
	0x00, //roi_ex
	0x00, //roi_ex
	0x00, //roi_ey
	0x00, //roi_ey
	0xff, //trans_on trans_block 0 000 0000
	0x04, //trans_slope
	0x01, //trans_interval
	//end
};

static char DSI_BROWSER_NATURAL_MDNIE_1[] = {
	//start
	0xDF,
	0x11, //linear_on ascr_skin_on 0000 0000
	0x6a, //ascr_skin_cb
	0x9a, //ascr_skin_cr
	0x25, //ascr_dist_up
	0x1a, //ascr_dist_down
	0x16, //ascr_dist_right
	0x2a, //ascr_dist_left
	0x00, //ascr_div_up 20
	0x37,
	0x5a,
	0x00, //ascr_div_down
	0x4e,
	0xc5,
	0x00, //ascr_div_right
	0x5d,
	0x17,
	0x00, //ascr_div_left
	0x30,
	0xc3,
	0xd3, //ascr_skin_Rr
	0x40, //ascr_skin_Rg
	0x29, //ascr_skin_Rb
	0xf0, //ascr_skin_Yr
	0xec, //ascr_skin_Yg
	0x5b, //ascr_skin_Yb
	0xdf, //ascr_skin_Mr
	0x4a, //ascr_skin_Mg
	0xdf, //ascr_skin_Mb
	0xff, //ascr_skin_Wr
	0xfa, //ascr_skin_Wg
	0xf1, //ascr_skin_Wb
	0x93, //ascr_Cr
	0xd3, //ascr_Rr
	0xed, //ascr_Cg
	0x40, //ascr_Rg
	0xe7, //ascr_Cb
	0x29, //ascr_Rb
	0xdf, //ascr_Mr
	0x85, //ascr_Gr
	0x4a, //ascr_Mg
	0xe0, //ascr_Gg
	0xdf, //ascr_Mb
	0x4f, //ascr_Gb
	0xf0, //ascr_Yr
	0x32, //ascr_Br
	0xec, //ascr_Yg
	0x1f, //ascr_Bg
	0x5b, //ascr_Yb
	0xd8, //ascr_Bb
	0xff, //ascr_Wr
	0x00, //ascr_Kr
	0xfa, //ascr_Wg
	0x00, //ascr_Kg
	0xf1, //ascr_Wb
	0x00, //ascr_Kb
	0x0F, //edge_ctrl
	0x00, //edge_radius 12
	0x6F,
	0x05, //edge_aa_factor
	0x00, //edge_r
	0x00, //edge_g
	0x00, //edge_b
	0x00, //edge_x 11
	0x70,
	0x00, //edge_y 12
	0x70,
	0x44, //edge_coef_a edge_coef_b 0000 0000
};

static char DSI_BROWSER_NATURAL_MDNIE_2[] = {
	0xDE,
	0x00, //gamut_gamma_on gamut_blk_shift 0000 0000
	0x40, //gamut_scale
	0x04, //gamut_r1
	0x00,
	0x00, //gamut_r2
	0x00,
	0x00, //gamut_r3
	0x00,
	0x00, //gamut_g1
	0x00,
	0x04, //gamut_g2
	0x00,
	0x00, //gamut_g3
	0x00,
	0x00, //gamut_b1
	0x00,
	0x00, //gamut_b2
	0x00,
	0x04, //gamut_b3
	0x00,
	0x00, //slce_on cadrx_en 0000 0000
	0x00, //glare_on
	0x00, //lce_gain 000 0000
	0x24, //lce_color_gain 00 0000
	0x96, //lce_min_ref_offset
	0xb3, //lce_illum_gain
	0x01, //lce_ref_offset 9
	0x0e,
	0x01, //lce_ref_gain 9
	0x00,
	0x77, //lce_block_size h v 0000 0000
	0x03, //lce_dark_th 000
	0x07, //lce_reduct_slope 0000
	0x40, //lce_black cc0 cc1 color_th 0 0 0 0000
	0x08, //lce_large_w
	0x04, //lce_med_w
	0x04, //lce_small_w
	0x00, //cadrx_dit_en shadow_corr_en ref_ctrl_en 0 0 0
	0x00, //cadrx_gain
	0x0d, //cadrx_low_intensity_th
	0xe1, //cadrx_high_intensity_th
	0x07, //cadrx_ui_zerobincnt_th
	0x29, //cadrx_ui_ratio_th
	0x04,
	0x0f, //cadrx_entire_freq
	0xa0,
	0x00,
	0x03, //cadrx_peak_th_in_freq
	0x7a,
	0xa0,
	0x00, //cadrx_high_peak_th_in_freq
	0x25,
	0x1c,
	0x24, //cadrx_dit_color_gain
	0x00, //cadrx_dit_illum_a0
	0x48, //cadrx_dit_illum_a1
	0xea, //cadrx_dit_illum_a2
	0xfa, //cadrx_dit_illum_a3
	0x00, //cadrx_dit_illum_b0
	0x10,
	0x00, //cadrx_dit_illum_b1
	0x10,
	0x00, //cadrx_dit_illum_b2
	0x9c,
	0x01, //cadrx_dit_illum_b3
	0x00,
	0x00, //cadrx_dit_illum_slope1
	0x00,
	0x00, //cadrx_dit_illum_slope2
	0xdc,
	0x03, //cadrx_dit_illum_slope3
	0xff,
	0x40, //cadrx_ui_illum_a1
	0x80, //cadrx_ui_illum_a2
	0xc0, //cadrx_ui_illum_a3
	0x00, //cadrx_ui_illum_offset1
	0x9a,
	0x00, //cadrx_ui_illum_offset2
	0x9a,
	0x00, //cadrx_ui_illum_offset3
	0x9a,
	0x00, //cadrx_ui_illum_offset4
	0x9a,
	0x00, //cadrx_ui_illum_slope1
	0x00,
	0x00, //cadrx_ui_illum_slope2
	0x00,
	0x00, //cadrx_ui_illum_slope3
	0x00,
	0x00, //cadrx_ui_illum_slope4
	0x00,
	0x40, //cadrx_ui_ref_a1
	0x80, //cadrx_ui_ref_a2
	0xc0, //cadrx_ui_ref_a3
	0x01, //cadrx_ui_ref_offset1
	0x0e,
	0x01, //cadrx_ui_ref_offset2
	0x0e,
	0x01, //cadrx_ui_ref_offset3
	0x0e,
	0x01, //cadrx_ui_ref_offset4
	0x0e,
	0x00, //cadrx_ui_ref_slope1
	0x00,
	0x00, //cadrx_ui_ref_slope2
	0x00,
	0x00, //cadrx_ui_ref_slope3
	0x00,
	0x00, //cadrx_ui_ref_slope4
	0x00,
	0x01, //cadrx_reg_ref_c1_offset
	0x0e,
	0x01, //cadrx_reg_ref_c2_offset
	0x3a,
	0x01, //cadrx_reg_ref_c3_offset
	0x44,
	0x00, //cadrx_reg_ref_c1_slope
	0x01,
	0x00, //cadrx_reg_ref_c2_slope
	0x43,
	0x00, //cadrx_reg_ref_c3_slope
	0x4b,
	0x00, //cadrx_sc_reg_a1
	0x18,
	0x00, //cadrx_sc_reg_b1
	0x1c,
	0x01, //cadrx_sc_k1_int
	0xff,
	0x01, //cadrx_sc_k2_int
	0xff,
	0x03, //cadrx_sc_w1_int
	0xff,
	0xe8,
	0x03, //cadrx_sc_w2_int
	0xff,
	0xe8,
	0x00, //flicker_w_en
	0x14, //flicker_w
	0x01, //flicker_trans_en
	0x04, //flicker_trans_slope
	0x00, //bl_en
	0x03, //bl_hbs_th
	0x10, //bl_lbs_th
	0xC8, //bl_illum_gain
	0x01, //bl_reg_gain 9
	0x2C,
	0x00, //bl_le_diff
	0x00, //bl_cadrx_gain
	0x04, //bl_trans_slope
	0x00, //le_en
	0x40, //le_diff
	0x03, //le_lmax 10
	0xB6,
	0x23, //le_p
	0x24, //le_w_tmf
	0x00, //le_w_struct_diff 9
	0x72,
	0x01, //le_w_struct_diff_gain
	0x00, //le_w_luminance 9
	0x0D,
	0x00, //le_luminance_slope 10
	0x04,
	0x80, //glare_blk_avg_th 0000 0000
	0x00, //glare_luma_gain 9
	0x28,
	0xa0, //glare_luma_ctl_start 0000 0000
	0x01, //glare_luma_th 0000 0000
	0x00, //glare_luma_step 9
	0x01,
	0x00, //glare_uni_luma_gain 9
	0x01,
	0x00, //glare_blk_max_th 0000 0000
	0x00, //glare_blk_max_w 0000 0000
	0x10, //glare_blk_max_curve blk_max_sh 0 0000
	0x00, //glare_con_ext_max_th 0000 0000
	0x00, //glare_con_ext_max_w 0000 0000
	0x30, //glare_con_ext_max_sign con_ext_max_curve con_ext_max_sh 0 0 0000
	0x00, //glare_max_cnt_th 0000
	0x00, //glare_max_cnt_th 0000 0000
	0x00, //glare_max_cnt_w 0000 0000
	0x30, //glare_max_cnt_sign max_cnt_curve max_cnt_sh 0 0 0000
	0x00, //glare_y_avg_th 0000 0000
	0x00, //glare_y_avg_w 0000 0000
	0x30, //glare_y_avg_sign y_avg_curve y_avg_sh 0 0 0000
	0x00, //glare_max_cnt_th_dmc 0000
	0x00, //glare_max_cnt_th_dmc 0000 0000
	0x00, //nr fa de cs gamma 0 0000
	0xff, //nr_mask_th
	0x00, //de_gain 10
	0x00,
	0x02, //de_maxplus 11
	0x00,
	0x02, //de_maxminus 11
	0x00,
	0x14, //fa_edge_th
	0x00, //fa_step_p 13
	0x01,
	0x00, //fa_step_n 13
	0x01,
	0x00, //fa_max_de_gain 13
	0x10,
	0x00, //fa_pcl_ppi 14
	0x00,
	0x28, //fa_os_cnt_10_co
	0x3c, //fa_os_cnt_20_co
	0x04, //fa_edge_cnt_weight
	0x0f, //fa_avg_y_weight
	0x01, //cs_gain 10
	0x00,
	0x00, //curve_0
	0x10, //curve_1
	0x20, //curve_2
	0x30, //curve_3
	0x40, //curve_4
	0x50, //curve_5
	0x60, //curve_6
	0x70, //curve_7
	0x80, //curve_8
	0x90, //curve_9
	0xa0, //curve_10
	0xb0, //curve_11
	0xc0, //curve_12
	0xd0, //curve_13
	0xe0, //curve_14
	0xf0, //curve_15
	0x01, //curve_16
	0x00,
	0x00, //curve_offset 10
	0x00,
	0x00, //curve_low_x
	0x00, //blank
	0x00, //curve_low_y
};

static char DSI_BROWSER_NATURAL_MDNIE_3[] = {
	0xDD,
	0x01, //mdnie_en
	0x00, //mask 00 0000
	0x03, //edge 00
	0xf0, //ascr algo aolce gamut 00 00 00 00
	0xff, //partial_en1
	0xff, //partial_en2
	0x00, //roi_en
	0x00, //roi_block
	0x00, //roi_sx
	0x00, //roi_sx
	0x00, //roi_sy
	0x00, //roi_sy
	0x00, //roi_ex
	0x00, //roi_ex
	0x00, //roi_ey
	0x00, //roi_ey
	0xff, //trans_on trans_block 0 000 0000
	0x04, //trans_slope
	0x01, //trans_interval
	//end
};

static char DSI_BROWSER_AUTO_MDNIE_1[] = {
	//start
	0xDF,
	0x01, //linear_on ascr_skin_on 0000 0000
	0x6a, //ascr_skin_cb
	0x9a, //ascr_skin_cr
	0x25, //ascr_dist_up
	0x1a, //ascr_dist_down
	0x16, //ascr_dist_right
	0x2a, //ascr_dist_left
	0x00, //ascr_div_up 20
	0x37,
	0x5a,
	0x00, //ascr_div_down
	0x4e,
	0xc5,
	0x00, //ascr_div_right
	0x5d,
	0x17,
	0x00, //ascr_div_left
	0x30,
	0xc3,
	0xff, //ascr_skin_Rr
	0x48, //ascr_skin_Rg
	0x48, //ascr_skin_Rb
	0xff, //ascr_skin_Yr
	0xff, //ascr_skin_Yg
	0x00, //ascr_skin_Yb
	0xff, //ascr_skin_Mr
	0x00, //ascr_skin_Mg
	0xff, //ascr_skin_Mb
	0xff, //ascr_skin_Wr
	0xff, //ascr_skin_Wg
	0xff, //ascr_skin_Wb
	0x00, //ascr_Cr
	0xff, //ascr_Rr
	0xff, //ascr_Cg
	0x00, //ascr_Rg
	0xff, //ascr_Cb
	0x00, //ascr_Rb
	0xff, //ascr_Mr
	0x00, //ascr_Gr
	0x00, //ascr_Mg
	0xff, //ascr_Gg
	0xff, //ascr_Mb
	0x00, //ascr_Gb
	0xff, //ascr_Yr
	0x00, //ascr_Br
	0xff, //ascr_Yg
	0x00, //ascr_Bg
	0x00, //ascr_Yb
	0xff, //ascr_Bb
	0xff, //ascr_Wr
	0x00, //ascr_Kr
	0xff, //ascr_Wg
	0x00, //ascr_Kg
	0xff, //ascr_Wb
	0x00, //ascr_Kb
	0x0F, //edge_ctrl
	0x00, //edge_radius 12
	0x6F,
	0x05, //edge_aa_factor
	0x00, //edge_r
	0x00, //edge_g
	0x00, //edge_b
	0x00, //edge_x 11
	0x70,
	0x00, //edge_y 12
	0x70,
	0x44, //edge_coef_a edge_coef_b 0000 0000
};

static char DSI_BROWSER_AUTO_MDNIE_2[] = {
	0xDE,
	0x00, //gamut_gamma_on gamut_blk_shift 0000 0000
	0x40, //gamut_scale
	0x04, //gamut_r1
	0x00,
	0x00, //gamut_r2
	0x00,
	0x00, //gamut_r3
	0x00,
	0x00, //gamut_g1
	0x00,
	0x04, //gamut_g2
	0x00,
	0x00, //gamut_g3
	0x00,
	0x00, //gamut_b1
	0x00,
	0x00, //gamut_b2
	0x00,
	0x04, //gamut_b3
	0x00,
	0x00, //slce_on cadrx_en 0000 0000
	0x11, //glare_on
	0x00, //lce_gain 000 0000
	0x24, //lce_color_gain 00 0000
	0x96, //lce_min_ref_offset
	0xb3, //lce_illum_gain
	0x01, //lce_ref_offset 9
	0x0e,
	0x01, //lce_ref_gain 9
	0x00,
	0x77, //lce_block_size h v 0000 0000
	0x03, //lce_dark_th 000
	0x07, //lce_reduct_slope 0000
	0x40, //lce_black cc0 cc1 color_th 0 0 0 0000
	0x08, //lce_large_w
	0x04, //lce_med_w
	0x04, //lce_small_w
	0x00, //cadrx_dit_en shadow_corr_en ref_ctrl_en 0 0 0
	0x00, //cadrx_gain
	0x0d, //cadrx_low_intensity_th
	0xe1, //cadrx_high_intensity_th
	0x07, //cadrx_ui_zerobincnt_th
	0x29, //cadrx_ui_ratio_th
	0x04,
	0x0f, //cadrx_entire_freq
	0xa0,
	0x00,
	0x03, //cadrx_peak_th_in_freq
	0x7a,
	0xa0,
	0x00, //cadrx_high_peak_th_in_freq
	0x25,
	0x1c,
	0x24, //cadrx_dit_color_gain
	0x00, //cadrx_dit_illum_a0
	0x48, //cadrx_dit_illum_a1
	0xea, //cadrx_dit_illum_a2
	0xfa, //cadrx_dit_illum_a3
	0x00, //cadrx_dit_illum_b0
	0x10,
	0x00, //cadrx_dit_illum_b1
	0x10,
	0x00, //cadrx_dit_illum_b2
	0x9c,
	0x01, //cadrx_dit_illum_b3
	0x00,
	0x00, //cadrx_dit_illum_slope1
	0x00,
	0x00, //cadrx_dit_illum_slope2
	0xdc,
	0x03, //cadrx_dit_illum_slope3
	0xff,
	0x40, //cadrx_ui_illum_a1
	0x80, //cadrx_ui_illum_a2
	0xc0, //cadrx_ui_illum_a3
	0x00, //cadrx_ui_illum_offset1
	0x9a,
	0x00, //cadrx_ui_illum_offset2
	0x9a,
	0x00, //cadrx_ui_illum_offset3
	0x9a,
	0x00, //cadrx_ui_illum_offset4
	0x9a,
	0x00, //cadrx_ui_illum_slope1
	0x00,
	0x00, //cadrx_ui_illum_slope2
	0x00,
	0x00, //cadrx_ui_illum_slope3
	0x00,
	0x00, //cadrx_ui_illum_slope4
	0x00,
	0x40, //cadrx_ui_ref_a1
	0x80, //cadrx_ui_ref_a2
	0xc0, //cadrx_ui_ref_a3
	0x01, //cadrx_ui_ref_offset1
	0x0e,
	0x01, //cadrx_ui_ref_offset2
	0x0e,
	0x01, //cadrx_ui_ref_offset3
	0x0e,
	0x01, //cadrx_ui_ref_offset4
	0x0e,
	0x00, //cadrx_ui_ref_slope1
	0x00,
	0x00, //cadrx_ui_ref_slope2
	0x00,
	0x00, //cadrx_ui_ref_slope3
	0x00,
	0x00, //cadrx_ui_ref_slope4
	0x00,
	0x01, //cadrx_reg_ref_c1_offset
	0x0e,
	0x01, //cadrx_reg_ref_c2_offset
	0x3a,
	0x01, //cadrx_reg_ref_c3_offset
	0x44,
	0x00, //cadrx_reg_ref_c1_slope
	0x01,
	0x00, //cadrx_reg_ref_c2_slope
	0x43,
	0x00, //cadrx_reg_ref_c3_slope
	0x4b,
	0x00, //cadrx_sc_reg_a1
	0x18,
	0x00, //cadrx_sc_reg_b1
	0x1c,
	0x01, //cadrx_sc_k1_int
	0xff,
	0x01, //cadrx_sc_k2_int
	0xff,
	0x03, //cadrx_sc_w1_int
	0xff,
	0xe8,
	0x03, //cadrx_sc_w2_int
	0xff,
	0xe8,
	0x00, //flicker_w_en
	0x14, //flicker_w
	0x01, //flicker_trans_en
	0x04, //flicker_trans_slope
	0x00, //bl_en
	0x03, //bl_hbs_th
	0x10, //bl_lbs_th
	0xC8, //bl_illum_gain
	0x01, //bl_reg_gain 9
	0x2C,
	0x00, //bl_le_diff
	0x00, //bl_cadrx_gain
	0x04, //bl_trans_slope
	0x00, //le_en
	0x40, //le_diff
	0x03, //le_lmax 10
	0xB6,
	0x23, //le_p
	0x24, //le_w_tmf
	0x00, //le_w_struct_diff 9
	0x72,
	0x01, //le_w_struct_diff_gain
	0x00, //le_w_luminance 9
	0x0D,
	0x00, //le_luminance_slope 10
	0x04,
	0x80, //glare_blk_avg_th 0000 0000
	0x00, //glare_luma_gain 9
	0x00,
	0x00, //glare_luma_ctl_start 0000 0000
	0x05, //glare_luma_th 0000 0000
	0x00, //glare_luma_step 9
	0x01,
	0x01, //glare_uni_luma_gain 9
	0x00,
	0xf0, //glare_blk_max_th 0000 0000
	0x12, //glare_blk_max_w 0000 0000
	0x18, //glare_blk_max_curve blk_max_sh 0 0000
	0xf0, //glare_con_ext_max_th 0000 0000
	0x12, //glare_con_ext_max_w 0000 0000
	0x38, //glare_con_ext_max_sign con_ext_max_curve con_ext_max_sh 0 0 0000
	0x00, //glare_max_cnt_th 0000
	0x00, //glare_max_cnt_th 0000 0000
	0x02, //glare_max_cnt_w 0000 0000
	0x38, //glare_max_cnt_sign max_cnt_curve max_cnt_sh 0 0 0000
	0x00, //glare_y_avg_th 0000 0000
	0x18, //glare_y_avg_w 0000 0000
	0x38, //glare_y_avg_sign y_avg_curve y_avg_sh 0 0 0000
	0x00, //glare_max_cnt_th_dmc 0000
	0x05, //glare_max_cnt_th_dmc 0000 0000
	0x00, //nr fa de cs gamma 0 0000
	0xff, //nr_mask_th
	0x00, //de_gain 10
	0x00,
	0x02, //de_maxplus 11
	0x00,
	0x02, //de_maxminus 11
	0x00,
	0x14, //fa_edge_th
	0x00, //fa_step_p 13
	0x01,
	0x00, //fa_step_n 13
	0x01,
	0x00, //fa_max_de_gain 13
	0x10,
	0x00, //fa_pcl_ppi 14
	0x00,
	0x28, //fa_os_cnt_10_co
	0x3c, //fa_os_cnt_20_co
	0x04, //fa_edge_cnt_weight
	0x0f, //fa_avg_y_weight
	0x01, //cs_gain 10
	0x00,
	0x00, //curve_0
	0x10, //curve_1
	0x20, //curve_2
	0x30, //curve_3
	0x40, //curve_4
	0x50, //curve_5
	0x60, //curve_6
	0x70, //curve_7
	0x80, //curve_8
	0x90, //curve_9
	0xa0, //curve_10
	0xb0, //curve_11
	0xc0, //curve_12
	0xd0, //curve_13
	0xe0, //curve_14
	0xf0, //curve_15
	0x01, //curve_16
	0x00,
	0x00, //curve_offset 10
	0x00,
	0x00, //curve_low_x
	0x00, //blank
	0x00, //curve_low_y
};

static char DSI_BROWSER_AUTO_MDNIE_3[] = {
	0xDD,
	0x01, //mdnie_en
	0x00, //mask 00 0000
	0x03, //edge 00
	0xf0, //ascr algo aolce gamut 00 00 00 00
	0xff, //partial_en1
	0xff, //partial_en2
	0x00, //roi_en
	0x00, //roi_block
	0x00, //roi_sx
	0x00, //roi_sx
	0x00, //roi_sy
	0x00, //roi_sy
	0x00, //roi_ex
	0x00, //roi_ex
	0x00, //roi_ey
	0x00, //roi_ey
	0xff, //trans_on trans_block 0 000 0000
	0x04, //trans_slope
	0x01, //trans_interval
	//end
};

static char DSI_EBOOK_DYNAMIC_MDNIE_1[] = {
	//start
	0xDF,
	0x11, //linear_on ascr_skin_on 0000 0000
	0x67, //ascr_skin_cb
	0xa9, //ascr_skin_cr
	0x37, //ascr_dist_up
	0x29, //ascr_dist_down
	0x19, //ascr_dist_right
	0x47, //ascr_dist_left
	0x00, //ascr_div_up 20
	0x25,
	0x3d,
	0x00, //ascr_div_down
	0x31,
	0xf4,
	0x00, //ascr_div_right
	0x51,
	0xec,
	0x00, //ascr_div_left
	0x1c,
	0xd8,
	0xe1, //ascr_skin_Rr
	0x25, //ascr_skin_Rg
	0x00, //ascr_skin_Rb
	0xee, //ascr_skin_Yr
	0xea, //ascr_skin_Yg
	0x00, //ascr_skin_Yb
	0xf0, //ascr_skin_Mr
	0x33, //ascr_skin_Mg
	0xe8, //ascr_skin_Mb
	0xff, //ascr_skin_Wr
	0xfa, //ascr_skin_Wg
	0xf1, //ascr_skin_Wb
	0x5c, //ascr_Cr
	0xe1, //ascr_Rr
	0xef, //ascr_Cg
	0x25, //ascr_Rg
	0xe8, //ascr_Cb
	0x00, //ascr_Rb
	0xf0, //ascr_Mr
	0x49, //ascr_Gr
	0x33, //ascr_Mg
	0xdf, //ascr_Gg
	0xe8, //ascr_Mb
	0x00, //ascr_Gb
	0xee, //ascr_Yr
	0x37, //ascr_Br
	0xea, //ascr_Yg
	0x21, //ascr_Bg
	0x00, //ascr_Yb
	0xdf, //ascr_Bb
	0xff, //ascr_Wr
	0x00, //ascr_Kr
	0xfa, //ascr_Wg
	0x00, //ascr_Kg
	0xf1, //ascr_Wb
	0x00, //ascr_Kb
	0x0F, //edge_ctrl
	0x00, //edge_radius 12
	0x6F,
	0x05, //edge_aa_factor
	0x00, //edge_r
	0x00, //edge_g
	0x00, //edge_b
	0x00, //edge_x 11
	0x70,
	0x00, //edge_y 12
	0x70,
	0x44, //edge_coef_a edge_coef_b 0000 0000
};

static char DSI_EBOOK_DYNAMIC_MDNIE_2[] = {
	0xDE,
	0x00, //gamut_gamma_on gamut_blk_shift 0000 0000
	0x40, //gamut_scale
	0x04, //gamut_r1
	0x00,
	0x00, //gamut_r2
	0x00,
	0x00, //gamut_r3
	0x00,
	0x00, //gamut_g1
	0x00,
	0x04, //gamut_g2
	0x00,
	0x00, //gamut_g3
	0x00,
	0x00, //gamut_b1
	0x00,
	0x00, //gamut_b2
	0x00,
	0x04, //gamut_b3
	0x00,
	0x00, //slce_on cadrx_en 0000 0000
	0x00, //glare_on
	0x00, //lce_gain 000 0000
	0x24, //lce_color_gain 00 0000
	0x96, //lce_min_ref_offset
	0xb3, //lce_illum_gain
	0x01, //lce_ref_offset 9
	0x0e,
	0x01, //lce_ref_gain 9
	0x00,
	0x77, //lce_block_size h v 0000 0000
	0x03, //lce_dark_th 000
	0x07, //lce_reduct_slope 0000
	0x40, //lce_black cc0 cc1 color_th 0 0 0 0000
	0x08, //lce_large_w
	0x04, //lce_med_w
	0x04, //lce_small_w
	0x00, //cadrx_dit_en shadow_corr_en ref_ctrl_en 0 0 0
	0x00, //cadrx_gain
	0x0d, //cadrx_low_intensity_th
	0xe1, //cadrx_high_intensity_th
	0x07, //cadrx_ui_zerobincnt_th
	0x29, //cadrx_ui_ratio_th
	0x04,
	0x0f, //cadrx_entire_freq
	0xa0,
	0x00,
	0x03, //cadrx_peak_th_in_freq
	0x7a,
	0xa0,
	0x00, //cadrx_high_peak_th_in_freq
	0x25,
	0x1c,
	0x24, //cadrx_dit_color_gain
	0x00, //cadrx_dit_illum_a0
	0x48, //cadrx_dit_illum_a1
	0xea, //cadrx_dit_illum_a2
	0xfa, //cadrx_dit_illum_a3
	0x00, //cadrx_dit_illum_b0
	0x10,
	0x00, //cadrx_dit_illum_b1
	0x10,
	0x00, //cadrx_dit_illum_b2
	0x9c,
	0x01, //cadrx_dit_illum_b3
	0x00,
	0x00, //cadrx_dit_illum_slope1
	0x00,
	0x00, //cadrx_dit_illum_slope2
	0xdc,
	0x03, //cadrx_dit_illum_slope3
	0xff,
	0x40, //cadrx_ui_illum_a1
	0x80, //cadrx_ui_illum_a2
	0xc0, //cadrx_ui_illum_a3
	0x00, //cadrx_ui_illum_offset1
	0x9a,
	0x00, //cadrx_ui_illum_offset2
	0x9a,
	0x00, //cadrx_ui_illum_offset3
	0x9a,
	0x00, //cadrx_ui_illum_offset4
	0x9a,
	0x00, //cadrx_ui_illum_slope1
	0x00,
	0x00, //cadrx_ui_illum_slope2
	0x00,
	0x00, //cadrx_ui_illum_slope3
	0x00,
	0x00, //cadrx_ui_illum_slope4
	0x00,
	0x40, //cadrx_ui_ref_a1
	0x80, //cadrx_ui_ref_a2
	0xc0, //cadrx_ui_ref_a3
	0x01, //cadrx_ui_ref_offset1
	0x0e,
	0x01, //cadrx_ui_ref_offset2
	0x0e,
	0x01, //cadrx_ui_ref_offset3
	0x0e,
	0x01, //cadrx_ui_ref_offset4
	0x0e,
	0x00, //cadrx_ui_ref_slope1
	0x00,
	0x00, //cadrx_ui_ref_slope2
	0x00,
	0x00, //cadrx_ui_ref_slope3
	0x00,
	0x00, //cadrx_ui_ref_slope4
	0x00,
	0x01, //cadrx_reg_ref_c1_offset
	0x0e,
	0x01, //cadrx_reg_ref_c2_offset
	0x3a,
	0x01, //cadrx_reg_ref_c3_offset
	0x44,
	0x00, //cadrx_reg_ref_c1_slope
	0x01,
	0x00, //cadrx_reg_ref_c2_slope
	0x43,
	0x00, //cadrx_reg_ref_c3_slope
	0x4b,
	0x00, //cadrx_sc_reg_a1
	0x18,
	0x00, //cadrx_sc_reg_b1
	0x1c,
	0x01, //cadrx_sc_k1_int
	0xff,
	0x01, //cadrx_sc_k2_int
	0xff,
	0x03, //cadrx_sc_w1_int
	0xff,
	0xe8,
	0x03, //cadrx_sc_w2_int
	0xff,
	0xe8,
	0x00, //flicker_w_en
	0x14, //flicker_w
	0x01, //flicker_trans_en
	0x04, //flicker_trans_slope
	0x00, //bl_en
	0x03, //bl_hbs_th
	0x10, //bl_lbs_th
	0xC8, //bl_illum_gain
	0x01, //bl_reg_gain 9
	0x2C,
	0x00, //bl_le_diff
	0x00, //bl_cadrx_gain
	0x04, //bl_trans_slope
	0x00, //le_en
	0x40, //le_diff
	0x03, //le_lmax 10
	0xB6,
	0x23, //le_p
	0x24, //le_w_tmf
	0x00, //le_w_struct_diff 9
	0x72,
	0x01, //le_w_struct_diff_gain
	0x00, //le_w_luminance 9
	0x0D,
	0x00, //le_luminance_slope 10
	0x04,
	0x80, //glare_blk_avg_th 0000 0000
	0x00, //glare_luma_gain 9
	0x28,
	0xa0, //glare_luma_ctl_start 0000 0000
	0x01, //glare_luma_th 0000 0000
	0x00, //glare_luma_step 9
	0x01,
	0x00, //glare_uni_luma_gain 9
	0x01,
	0x00, //glare_blk_max_th 0000 0000
	0x00, //glare_blk_max_w 0000 0000
	0x10, //glare_blk_max_curve blk_max_sh 0 0000
	0x00, //glare_con_ext_max_th 0000 0000
	0x00, //glare_con_ext_max_w 0000 0000
	0x30, //glare_con_ext_max_sign con_ext_max_curve con_ext_max_sh 0 0 0000
	0x00, //glare_max_cnt_th 0000
	0x00, //glare_max_cnt_th 0000 0000
	0x00, //glare_max_cnt_w 0000 0000
	0x30, //glare_max_cnt_sign max_cnt_curve max_cnt_sh 0 0 0000
	0x00, //glare_y_avg_th 0000 0000
	0x00, //glare_y_avg_w 0000 0000
	0x30, //glare_y_avg_sign y_avg_curve y_avg_sh 0 0 0000
	0x00, //glare_max_cnt_th_dmc 0000
	0x00, //glare_max_cnt_th_dmc 0000 0000
	0x00, //nr fa de cs gamma 0 0000
	0xff, //nr_mask_th
	0x00, //de_gain 10
	0x00,
	0x02, //de_maxplus 11
	0x00,
	0x02, //de_maxminus 11
	0x00,
	0x14, //fa_edge_th
	0x00, //fa_step_p 13
	0x01,
	0x00, //fa_step_n 13
	0x01,
	0x00, //fa_max_de_gain 13
	0x10,
	0x00, //fa_pcl_ppi 14
	0x00,
	0x28, //fa_os_cnt_10_co
	0x3c, //fa_os_cnt_20_co
	0x04, //fa_edge_cnt_weight
	0x0f, //fa_avg_y_weight
	0x01, //cs_gain 10
	0x00,
	0x00, //curve_0
	0x10, //curve_1
	0x20, //curve_2
	0x30, //curve_3
	0x40, //curve_4
	0x50, //curve_5
	0x60, //curve_6
	0x70, //curve_7
	0x80, //curve_8
	0x90, //curve_9
	0xa0, //curve_10
	0xb0, //curve_11
	0xc0, //curve_12
	0xd0, //curve_13
	0xe0, //curve_14
	0xf0, //curve_15
	0x01, //curve_16
	0x00,
	0x00, //curve_offset 10
	0x00,
	0x00, //curve_low_x
	0x00, //blank
	0x00, //curve_low_y
};

static char DSI_EBOOK_DYNAMIC_MDNIE_3[] = {
	0xDD,
	0x01, //mdnie_en
	0x00, //mask 00 0000
	0x03, //edge 00
	0xf0, //ascr algo aolce gamut 00 00 00 00
	0xff, //partial_en1
	0xff, //partial_en2
	0x00, //roi_en
	0x00, //roi_block
	0x00, //roi_sx
	0x00, //roi_sx
	0x00, //roi_sy
	0x00, //roi_sy
	0x00, //roi_ex
	0x00, //roi_ex
	0x00, //roi_ey
	0x00, //roi_ey
	0xff, //trans_on trans_block 0 000 0000
	0x04, //trans_slope
	0x01, //trans_interval
	//end
};

static char DSI_EBOOK_STANDARD_MDNIE_1[] = {
	//start
	0xDF,
	0x11, //linear_on ascr_skin_on 0000 0000
	0x6a, //ascr_skin_cb
	0x9a, //ascr_skin_cr
	0x25, //ascr_dist_up
	0x1a, //ascr_dist_down
	0x16, //ascr_dist_right
	0x2a, //ascr_dist_left
	0x00, //ascr_div_up 20
	0x37,
	0x5a,
	0x00, //ascr_div_down
	0x4e,
	0xc5,
	0x00, //ascr_div_right
	0x5d,
	0x17,
	0x00, //ascr_div_left
	0x30,
	0xc3,
	0xee, //ascr_skin_Rr
	0x4b, //ascr_skin_Rg
	0x30, //ascr_skin_Rb
	0xef, //ascr_skin_Yr
	0xeb, //ascr_skin_Yg
	0x47, //ascr_skin_Yb
	0xfc, //ascr_skin_Mr
	0x57, //ascr_skin_Mg
	0xe7, //ascr_skin_Mb
	0xff, //ascr_skin_Wr
	0xfa, //ascr_skin_Wg
	0xf1, //ascr_skin_Wb
	0x00, //ascr_Cr
	0xee, //ascr_Rr
	0xe7, //ascr_Cg
	0x4b, //ascr_Rg
	0xe5, //ascr_Cb
	0x30, //ascr_Rb
	0xfc, //ascr_Mr
	0x00, //ascr_Gr
	0x57, //ascr_Mg
	0xd9, //ascr_Gg
	0xe7, //ascr_Mb
	0x34, //ascr_Gb
	0xef, //ascr_Yr
	0x34, //ascr_Br
	0xeb, //ascr_Yg
	0x20, //ascr_Bg
	0x47, //ascr_Yb
	0xdb, //ascr_Bb
	0xff, //ascr_Wr
	0x00, //ascr_Kr
	0xfa, //ascr_Wg
	0x00, //ascr_Kg
	0xf1, //ascr_Wb
	0x00, //ascr_Kb
	0x0F, //edge_ctrl
	0x00, //edge_radius 12
	0x6F,
	0x05, //edge_aa_factor
	0x00, //edge_r
	0x00, //edge_g
	0x00, //edge_b
	0x00, //edge_x 11
	0x70,
	0x00, //edge_y 12
	0x70,
	0x44, //edge_coef_a edge_coef_b 0000 0000
};

static char DSI_EBOOK_STANDARD_MDNIE_2[] = {
	0xDE,
	0x00, //gamut_gamma_on gamut_blk_shift 0000 0000
	0x40, //gamut_scale
	0x04, //gamut_r1
	0x00,
	0x00, //gamut_r2
	0x00,
	0x00, //gamut_r3
	0x00,
	0x00, //gamut_g1
	0x00,
	0x04, //gamut_g2
	0x00,
	0x00, //gamut_g3
	0x00,
	0x00, //gamut_b1
	0x00,
	0x00, //gamut_b2
	0x00,
	0x04, //gamut_b3
	0x00,
	0x00, //slce_on cadrx_en 0000 0000
	0x00, //glare_on
	0x00, //lce_gain 000 0000
	0x24, //lce_color_gain 00 0000
	0x96, //lce_min_ref_offset
	0xb3, //lce_illum_gain
	0x01, //lce_ref_offset 9
	0x0e,
	0x01, //lce_ref_gain 9
	0x00,
	0x77, //lce_block_size h v 0000 0000
	0x03, //lce_dark_th 000
	0x07, //lce_reduct_slope 0000
	0x40, //lce_black cc0 cc1 color_th 0 0 0 0000
	0x08, //lce_large_w
	0x04, //lce_med_w
	0x04, //lce_small_w
	0x00, //cadrx_dit_en shadow_corr_en ref_ctrl_en 0 0 0
	0x00, //cadrx_gain
	0x0d, //cadrx_low_intensity_th
	0xe1, //cadrx_high_intensity_th
	0x07, //cadrx_ui_zerobincnt_th
	0x29, //cadrx_ui_ratio_th
	0x04,
	0x0f, //cadrx_entire_freq
	0xa0,
	0x00,
	0x03, //cadrx_peak_th_in_freq
	0x7a,
	0xa0,
	0x00, //cadrx_high_peak_th_in_freq
	0x25,
	0x1c,
	0x24, //cadrx_dit_color_gain
	0x00, //cadrx_dit_illum_a0
	0x48, //cadrx_dit_illum_a1
	0xea, //cadrx_dit_illum_a2
	0xfa, //cadrx_dit_illum_a3
	0x00, //cadrx_dit_illum_b0
	0x10,
	0x00, //cadrx_dit_illum_b1
	0x10,
	0x00, //cadrx_dit_illum_b2
	0x9c,
	0x01, //cadrx_dit_illum_b3
	0x00,
	0x00, //cadrx_dit_illum_slope1
	0x00,
	0x00, //cadrx_dit_illum_slope2
	0xdc,
	0x03, //cadrx_dit_illum_slope3
	0xff,
	0x40, //cadrx_ui_illum_a1
	0x80, //cadrx_ui_illum_a2
	0xc0, //cadrx_ui_illum_a3
	0x00, //cadrx_ui_illum_offset1
	0x9a,
	0x00, //cadrx_ui_illum_offset2
	0x9a,
	0x00, //cadrx_ui_illum_offset3
	0x9a,
	0x00, //cadrx_ui_illum_offset4
	0x9a,
	0x00, //cadrx_ui_illum_slope1
	0x00,
	0x00, //cadrx_ui_illum_slope2
	0x00,
	0x00, //cadrx_ui_illum_slope3
	0x00,
	0x00, //cadrx_ui_illum_slope4
	0x00,
	0x40, //cadrx_ui_ref_a1
	0x80, //cadrx_ui_ref_a2
	0xc0, //cadrx_ui_ref_a3
	0x01, //cadrx_ui_ref_offset1
	0x0e,
	0x01, //cadrx_ui_ref_offset2
	0x0e,
	0x01, //cadrx_ui_ref_offset3
	0x0e,
	0x01, //cadrx_ui_ref_offset4
	0x0e,
	0x00, //cadrx_ui_ref_slope1
	0x00,
	0x00, //cadrx_ui_ref_slope2
	0x00,
	0x00, //cadrx_ui_ref_slope3
	0x00,
	0x00, //cadrx_ui_ref_slope4
	0x00,
	0x01, //cadrx_reg_ref_c1_offset
	0x0e,
	0x01, //cadrx_reg_ref_c2_offset
	0x3a,
	0x01, //cadrx_reg_ref_c3_offset
	0x44,
	0x00, //cadrx_reg_ref_c1_slope
	0x01,
	0x00, //cadrx_reg_ref_c2_slope
	0x43,
	0x00, //cadrx_reg_ref_c3_slope
	0x4b,
	0x00, //cadrx_sc_reg_a1
	0x18,
	0x00, //cadrx_sc_reg_b1
	0x1c,
	0x01, //cadrx_sc_k1_int
	0xff,
	0x01, //cadrx_sc_k2_int
	0xff,
	0x03, //cadrx_sc_w1_int
	0xff,
	0xe8,
	0x03, //cadrx_sc_w2_int
	0xff,
	0xe8,
	0x00, //flicker_w_en
	0x14, //flicker_w
	0x01, //flicker_trans_en
	0x04, //flicker_trans_slope
	0x00, //bl_en
	0x03, //bl_hbs_th
	0x10, //bl_lbs_th
	0xC8, //bl_illum_gain
	0x01, //bl_reg_gain 9
	0x2C,
	0x00, //bl_le_diff
	0x00, //bl_cadrx_gain
	0x04, //bl_trans_slope
	0x00, //le_en
	0x40, //le_diff
	0x03, //le_lmax 10
	0xB6,
	0x23, //le_p
	0x24, //le_w_tmf
	0x00, //le_w_struct_diff 9
	0x72,
	0x01, //le_w_struct_diff_gain
	0x00, //le_w_luminance 9
	0x0D,
	0x00, //le_luminance_slope 10
	0x04,
	0x80, //glare_blk_avg_th 0000 0000
	0x00, //glare_luma_gain 9
	0x28,
	0xa0, //glare_luma_ctl_start 0000 0000
	0x01, //glare_luma_th 0000 0000
	0x00, //glare_luma_step 9
	0x01,
	0x00, //glare_uni_luma_gain 9
	0x01,
	0x00, //glare_blk_max_th 0000 0000
	0x00, //glare_blk_max_w 0000 0000
	0x10, //glare_blk_max_curve blk_max_sh 0 0000
	0x00, //glare_con_ext_max_th 0000 0000
	0x00, //glare_con_ext_max_w 0000 0000
	0x30, //glare_con_ext_max_sign con_ext_max_curve con_ext_max_sh 0 0 0000
	0x00, //glare_max_cnt_th 0000
	0x00, //glare_max_cnt_th 0000 0000
	0x00, //glare_max_cnt_w 0000 0000
	0x30, //glare_max_cnt_sign max_cnt_curve max_cnt_sh 0 0 0000
	0x00, //glare_y_avg_th 0000 0000
	0x00, //glare_y_avg_w 0000 0000
	0x30, //glare_y_avg_sign y_avg_curve y_avg_sh 0 0 0000
	0x00, //glare_max_cnt_th_dmc 0000
	0x00, //glare_max_cnt_th_dmc 0000 0000
	0x02, //nr fa de cs gamma 0 0000
	0xff, //nr_mask_th
	0x00, //de_gain 10
	0x00,
	0x02, //de_maxplus 11
	0x00,
	0x02, //de_maxminus 11
	0x00,
	0x14, //fa_edge_th
	0x00, //fa_step_p 13
	0x01,
	0x00, //fa_step_n 13
	0x01,
	0x00, //fa_max_de_gain 13
	0x10,
	0x00, //fa_pcl_ppi 14
	0x00,
	0x28, //fa_os_cnt_10_co
	0x3c, //fa_os_cnt_20_co
	0x04, //fa_edge_cnt_weight
	0x0f, //fa_avg_y_weight
	0x01, //cs_gain 10
	0x18,
	0x00, //curve_0
	0x10, //curve_1
	0x20, //curve_2
	0x30, //curve_3
	0x40, //curve_4
	0x50, //curve_5
	0x60, //curve_6
	0x70, //curve_7
	0x80, //curve_8
	0x90, //curve_9
	0xa0, //curve_10
	0xb0, //curve_11
	0xc0, //curve_12
	0xd0, //curve_13
	0xe0, //curve_14
	0xf0, //curve_15
	0x01, //curve_16
	0x00,
	0x00, //curve_offset 10
	0x00,
	0x00, //curve_low_x
	0x00, //blank
	0x00, //curve_low_y
};

static char DSI_EBOOK_STANDARD_MDNIE_3[] = {
	0xDD,
	0x01, //mdnie_en
	0x00, //mask 00 0000
	0x03, //edge 00
	0xf0, //ascr algo aolce gamut 00 00 00 00
	0xff, //partial_en1
	0xff, //partial_en2
	0x00, //roi_en
	0x00, //roi_block
	0x00, //roi_sx
	0x00, //roi_sx
	0x00, //roi_sy
	0x00, //roi_sy
	0x00, //roi_ex
	0x00, //roi_ex
	0x00, //roi_ey
	0x00, //roi_ey
	0xff, //trans_on trans_block 0 000 0000
	0x04, //trans_slope
	0x01, //trans_interval
	//end
};

static char DSI_EBOOK_NATURAL_MDNIE_1[] = {
	//start
	0xDF,
	0x11, //linear_on ascr_skin_on 0000 0000
	0x6a, //ascr_skin_cb
	0x9a, //ascr_skin_cr
	0x25, //ascr_dist_up
	0x1a, //ascr_dist_down
	0x16, //ascr_dist_right
	0x2a, //ascr_dist_left
	0x00, //ascr_div_up 20
	0x37,
	0x5a,
	0x00, //ascr_div_down
	0x4e,
	0xc5,
	0x00, //ascr_div_right
	0x5d,
	0x17,
	0x00, //ascr_div_left
	0x30,
	0xc3,
	0xd3, //ascr_skin_Rr
	0x40, //ascr_skin_Rg
	0x29, //ascr_skin_Rb
	0xf0, //ascr_skin_Yr
	0xec, //ascr_skin_Yg
	0x5b, //ascr_skin_Yb
	0xdf, //ascr_skin_Mr
	0x4a, //ascr_skin_Mg
	0xdf, //ascr_skin_Mb
	0xff, //ascr_skin_Wr
	0xfa, //ascr_skin_Wg
	0xf1, //ascr_skin_Wb
	0x93, //ascr_Cr
	0xd3, //ascr_Rr
	0xed, //ascr_Cg
	0x40, //ascr_Rg
	0xe7, //ascr_Cb
	0x29, //ascr_Rb
	0xdf, //ascr_Mr
	0x85, //ascr_Gr
	0x4a, //ascr_Mg
	0xe0, //ascr_Gg
	0xdf, //ascr_Mb
	0x4f, //ascr_Gb
	0xf0, //ascr_Yr
	0x32, //ascr_Br
	0xec, //ascr_Yg
	0x1f, //ascr_Bg
	0x5b, //ascr_Yb
	0xd8, //ascr_Bb
	0xff, //ascr_Wr
	0x00, //ascr_Kr
	0xfa, //ascr_Wg
	0x00, //ascr_Kg
	0xf1, //ascr_Wb
	0x00, //ascr_Kb
	0x0F, //edge_ctrl
	0x00, //edge_radius 12
	0x6F,
	0x05, //edge_aa_factor
	0x00, //edge_r
	0x00, //edge_g
	0x00, //edge_b
	0x00, //edge_x 11
	0x70,
	0x00, //edge_y 12
	0x70,
	0x44, //edge_coef_a edge_coef_b 0000 0000
};

static char DSI_EBOOK_NATURAL_MDNIE_2[] = {
	0xDE,
	0x00, //gamut_gamma_on gamut_blk_shift 0000 0000
	0x40, //gamut_scale
	0x04, //gamut_r1
	0x00,
	0x00, //gamut_r2
	0x00,
	0x00, //gamut_r3
	0x00,
	0x00, //gamut_g1
	0x00,
	0x04, //gamut_g2
	0x00,
	0x00, //gamut_g3
	0x00,
	0x00, //gamut_b1
	0x00,
	0x00, //gamut_b2
	0x00,
	0x04, //gamut_b3
	0x00,
	0x00, //slce_on cadrx_en 0000 0000
	0x00, //glare_on
	0x00, //lce_gain 000 0000
	0x24, //lce_color_gain 00 0000
	0x96, //lce_min_ref_offset
	0xb3, //lce_illum_gain
	0x01, //lce_ref_offset 9
	0x0e,
	0x01, //lce_ref_gain 9
	0x00,
	0x77, //lce_block_size h v 0000 0000
	0x03, //lce_dark_th 000
	0x07, //lce_reduct_slope 0000
	0x40, //lce_black cc0 cc1 color_th 0 0 0 0000
	0x08, //lce_large_w
	0x04, //lce_med_w
	0x04, //lce_small_w
	0x00, //cadrx_dit_en shadow_corr_en ref_ctrl_en 0 0 0
	0x00, //cadrx_gain
	0x0d, //cadrx_low_intensity_th
	0xe1, //cadrx_high_intensity_th
	0x07, //cadrx_ui_zerobincnt_th
	0x29, //cadrx_ui_ratio_th
	0x04,
	0x0f, //cadrx_entire_freq
	0xa0,
	0x00,
	0x03, //cadrx_peak_th_in_freq
	0x7a,
	0xa0,
	0x00, //cadrx_high_peak_th_in_freq
	0x25,
	0x1c,
	0x24, //cadrx_dit_color_gain
	0x00, //cadrx_dit_illum_a0
	0x48, //cadrx_dit_illum_a1
	0xea, //cadrx_dit_illum_a2
	0xfa, //cadrx_dit_illum_a3
	0x00, //cadrx_dit_illum_b0
	0x10,
	0x00, //cadrx_dit_illum_b1
	0x10,
	0x00, //cadrx_dit_illum_b2
	0x9c,
	0x01, //cadrx_dit_illum_b3
	0x00,
	0x00, //cadrx_dit_illum_slope1
	0x00,
	0x00, //cadrx_dit_illum_slope2
	0xdc,
	0x03, //cadrx_dit_illum_slope3
	0xff,
	0x40, //cadrx_ui_illum_a1
	0x80, //cadrx_ui_illum_a2
	0xc0, //cadrx_ui_illum_a3
	0x00, //cadrx_ui_illum_offset1
	0x9a,
	0x00, //cadrx_ui_illum_offset2
	0x9a,
	0x00, //cadrx_ui_illum_offset3
	0x9a,
	0x00, //cadrx_ui_illum_offset4
	0x9a,
	0x00, //cadrx_ui_illum_slope1
	0x00,
	0x00, //cadrx_ui_illum_slope2
	0x00,
	0x00, //cadrx_ui_illum_slope3
	0x00,
	0x00, //cadrx_ui_illum_slope4
	0x00,
	0x40, //cadrx_ui_ref_a1
	0x80, //cadrx_ui_ref_a2
	0xc0, //cadrx_ui_ref_a3
	0x01, //cadrx_ui_ref_offset1
	0x0e,
	0x01, //cadrx_ui_ref_offset2
	0x0e,
	0x01, //cadrx_ui_ref_offset3
	0x0e,
	0x01, //cadrx_ui_ref_offset4
	0x0e,
	0x00, //cadrx_ui_ref_slope1
	0x00,
	0x00, //cadrx_ui_ref_slope2
	0x00,
	0x00, //cadrx_ui_ref_slope3
	0x00,
	0x00, //cadrx_ui_ref_slope4
	0x00,
	0x01, //cadrx_reg_ref_c1_offset
	0x0e,
	0x01, //cadrx_reg_ref_c2_offset
	0x3a,
	0x01, //cadrx_reg_ref_c3_offset
	0x44,
	0x00, //cadrx_reg_ref_c1_slope
	0x01,
	0x00, //cadrx_reg_ref_c2_slope
	0x43,
	0x00, //cadrx_reg_ref_c3_slope
	0x4b,
	0x00, //cadrx_sc_reg_a1
	0x18,
	0x00, //cadrx_sc_reg_b1
	0x1c,
	0x01, //cadrx_sc_k1_int
	0xff,
	0x01, //cadrx_sc_k2_int
	0xff,
	0x03, //cadrx_sc_w1_int
	0xff,
	0xe8,
	0x03, //cadrx_sc_w2_int
	0xff,
	0xe8,
	0x00, //flicker_w_en
	0x14, //flicker_w
	0x01, //flicker_trans_en
	0x04, //flicker_trans_slope
	0x00, //bl_en
	0x03, //bl_hbs_th
	0x10, //bl_lbs_th
	0xC8, //bl_illum_gain
	0x01, //bl_reg_gain 9
	0x2C,
	0x00, //bl_le_diff
	0x00, //bl_cadrx_gain
	0x04, //bl_trans_slope
	0x00, //le_en
	0x40, //le_diff
	0x03, //le_lmax 10
	0xB6,
	0x23, //le_p
	0x24, //le_w_tmf
	0x00, //le_w_struct_diff 9
	0x72,
	0x01, //le_w_struct_diff_gain
	0x00, //le_w_luminance 9
	0x0D,
	0x00, //le_luminance_slope 10
	0x04,
	0x80, //glare_blk_avg_th 0000 0000
	0x00, //glare_luma_gain 9
	0x28,
	0xa0, //glare_luma_ctl_start 0000 0000
	0x01, //glare_luma_th 0000 0000
	0x00, //glare_luma_step 9
	0x01,
	0x00, //glare_uni_luma_gain 9
	0x01,
	0x00, //glare_blk_max_th 0000 0000
	0x00, //glare_blk_max_w 0000 0000
	0x10, //glare_blk_max_curve blk_max_sh 0 0000
	0x00, //glare_con_ext_max_th 0000 0000
	0x00, //glare_con_ext_max_w 0000 0000
	0x30, //glare_con_ext_max_sign con_ext_max_curve con_ext_max_sh 0 0 0000
	0x00, //glare_max_cnt_th 0000
	0x00, //glare_max_cnt_th 0000 0000
	0x00, //glare_max_cnt_w 0000 0000
	0x30, //glare_max_cnt_sign max_cnt_curve max_cnt_sh 0 0 0000
	0x00, //glare_y_avg_th 0000 0000
	0x00, //glare_y_avg_w 0000 0000
	0x30, //glare_y_avg_sign y_avg_curve y_avg_sh 0 0 0000
	0x00, //glare_max_cnt_th_dmc 0000
	0x00, //glare_max_cnt_th_dmc 0000 0000
	0x00, //nr fa de cs gamma 0 0000
	0xff, //nr_mask_th
	0x00, //de_gain 10
	0x00,
	0x02, //de_maxplus 11
	0x00,
	0x02, //de_maxminus 11
	0x00,
	0x14, //fa_edge_th
	0x00, //fa_step_p 13
	0x01,
	0x00, //fa_step_n 13
	0x01,
	0x00, //fa_max_de_gain 13
	0x10,
	0x00, //fa_pcl_ppi 14
	0x00,
	0x28, //fa_os_cnt_10_co
	0x3c, //fa_os_cnt_20_co
	0x04, //fa_edge_cnt_weight
	0x0f, //fa_avg_y_weight
	0x01, //cs_gain 10
	0x00,
	0x00, //curve_0
	0x10, //curve_1
	0x20, //curve_2
	0x30, //curve_3
	0x40, //curve_4
	0x50, //curve_5
	0x60, //curve_6
	0x70, //curve_7
	0x80, //curve_8
	0x90, //curve_9
	0xa0, //curve_10
	0xb0, //curve_11
	0xc0, //curve_12
	0xd0, //curve_13
	0xe0, //curve_14
	0xf0, //curve_15
	0x01, //curve_16
	0x00,
	0x00, //curve_offset 10
	0x00,
	0x00, //curve_low_x
	0x00, //blank
	0x00, //curve_low_y
};

static char DSI_EBOOK_NATURAL_MDNIE_3[] = {
	0xDD,
	0x01, //mdnie_en
	0x00, //mask 00 0000
	0x03, //edge 00
	0xf0, //ascr algo aolce gamut 00 00 00 00
	0xff, //partial_en1
	0xff, //partial_en2
	0x00, //roi_en
	0x00, //roi_block
	0x00, //roi_sx
	0x00, //roi_sx
	0x00, //roi_sy
	0x00, //roi_sy
	0x00, //roi_ex
	0x00, //roi_ex
	0x00, //roi_ey
	0x00, //roi_ey
	0xff, //trans_on trans_block 0 000 0000
	0x04, //trans_slope
	0x01, //trans_interval
	//end
};

static char DSI_EBOOK_AUTO_MDNIE_1[] = {
	//start
	0xDF,
	0x01, //linear_on ascr_skin_on 0000 0000
	0x6a, //ascr_skin_cb
	0x9a, //ascr_skin_cr
	0x25, //ascr_dist_up
	0x1a, //ascr_dist_down
	0x16, //ascr_dist_right
	0x2a, //ascr_dist_left
	0x00, //ascr_div_up 20
	0x37,
	0x5a,
	0x00, //ascr_div_down
	0x4e,
	0xc5,
	0x00, //ascr_div_right
	0x5d,
	0x17,
	0x00, //ascr_div_left
	0x30,
	0xc3,
	0xff, //ascr_skin_Rr
	0x00, //ascr_skin_Rg
	0x00, //ascr_skin_Rb
	0xff, //ascr_skin_Yr
	0xff, //ascr_skin_Yg
	0x00, //ascr_skin_Yb
	0xff, //ascr_skin_Mr
	0x00, //ascr_skin_Mg
	0xff, //ascr_skin_Mb
	0xff, //ascr_skin_Wr
	0xf9, //ascr_skin_Wg
	0xe9, //ascr_skin_Wb
	0x00, //ascr_Cr
	0xff, //ascr_Rr
	0xff, //ascr_Cg
	0x00, //ascr_Rg
	0xff, //ascr_Cb
	0x00, //ascr_Rb
	0xff, //ascr_Mr
	0x00, //ascr_Gr
	0x00, //ascr_Mg
	0xff, //ascr_Gg
	0xff, //ascr_Mb
	0x00, //ascr_Gb
	0xff, //ascr_Yr
	0x00, //ascr_Br
	0xff, //ascr_Yg
	0x00, //ascr_Bg
	0x00, //ascr_Yb
	0xff, //ascr_Bb
	0xff, //ascr_Wr
	0x00, //ascr_Kr
	0xf8, //ascr_Wg
	0x00, //ascr_Kg
	0xe7, //ascr_Wb
	0x00, //ascr_Kb
	0x0F, //edge_ctrl
	0x00, //edge_radius 12
	0x6F,
	0x05, //edge_aa_factor
	0x00, //edge_r
	0x00, //edge_g
	0x00, //edge_b
	0x00, //edge_x 11
	0x70,
	0x00, //edge_y 12
	0x70,
	0x44, //edge_coef_a edge_coef_b 0000 0000
};

static char DSI_EBOOK_AUTO_MDNIE_2[] = {
	0xDE,
	0x00, //gamut_gamma_on gamut_blk_shift 0000 0000
	0x40, //gamut_scale
	0x04, //gamut_r1
	0x00,
	0x00, //gamut_r2
	0x00,
	0x00, //gamut_r3
	0x00,
	0x00, //gamut_g1
	0x00,
	0x04, //gamut_g2
	0x00,
	0x00, //gamut_g3
	0x00,
	0x00, //gamut_b1
	0x00,
	0x00, //gamut_b2
	0x00,
	0x04, //gamut_b3
	0x00,
	0x00, //slce_on cadrx_en 0000 0000
	0x11, //glare_on
	0x00, //lce_gain 000 0000
	0x24, //lce_color_gain 00 0000
	0x96, //lce_min_ref_offset
	0xb3, //lce_illum_gain
	0x01, //lce_ref_offset 9
	0x0e,
	0x01, //lce_ref_gain 9
	0x00,
	0x77, //lce_block_size h v 0000 0000
	0x03, //lce_dark_th 000
	0x07, //lce_reduct_slope 0000
	0x40, //lce_black cc0 cc1 color_th 0 0 0 0000
	0x08, //lce_large_w
	0x04, //lce_med_w
	0x04, //lce_small_w
	0x00, //cadrx_dit_en shadow_corr_en ref_ctrl_en 0 0 0
	0x00, //cadrx_gain
	0x0d, //cadrx_low_intensity_th
	0xe1, //cadrx_high_intensity_th
	0x07, //cadrx_ui_zerobincnt_th
	0x29, //cadrx_ui_ratio_th
	0x04,
	0x0f, //cadrx_entire_freq
	0xa0,
	0x00,
	0x03, //cadrx_peak_th_in_freq
	0x7a,
	0xa0,
	0x00, //cadrx_high_peak_th_in_freq
	0x25,
	0x1c,
	0x24, //cadrx_dit_color_gain
	0x00, //cadrx_dit_illum_a0
	0x48, //cadrx_dit_illum_a1
	0xea, //cadrx_dit_illum_a2
	0xfa, //cadrx_dit_illum_a3
	0x00, //cadrx_dit_illum_b0
	0x10,
	0x00, //cadrx_dit_illum_b1
	0x10,
	0x00, //cadrx_dit_illum_b2
	0x9c,
	0x01, //cadrx_dit_illum_b3
	0x00,
	0x00, //cadrx_dit_illum_slope1
	0x00,
	0x00, //cadrx_dit_illum_slope2
	0xdc,
	0x03, //cadrx_dit_illum_slope3
	0xff,
	0x40, //cadrx_ui_illum_a1
	0x80, //cadrx_ui_illum_a2
	0xc0, //cadrx_ui_illum_a3
	0x00, //cadrx_ui_illum_offset1
	0x9a,
	0x00, //cadrx_ui_illum_offset2
	0x9a,
	0x00, //cadrx_ui_illum_offset3
	0x9a,
	0x00, //cadrx_ui_illum_offset4
	0x9a,
	0x00, //cadrx_ui_illum_slope1
	0x00,
	0x00, //cadrx_ui_illum_slope2
	0x00,
	0x00, //cadrx_ui_illum_slope3
	0x00,
	0x00, //cadrx_ui_illum_slope4
	0x00,
	0x40, //cadrx_ui_ref_a1
	0x80, //cadrx_ui_ref_a2
	0xc0, //cadrx_ui_ref_a3
	0x01, //cadrx_ui_ref_offset1
	0x0e,
	0x01, //cadrx_ui_ref_offset2
	0x0e,
	0x01, //cadrx_ui_ref_offset3
	0x0e,
	0x01, //cadrx_ui_ref_offset4
	0x0e,
	0x00, //cadrx_ui_ref_slope1
	0x00,
	0x00, //cadrx_ui_ref_slope2
	0x00,
	0x00, //cadrx_ui_ref_slope3
	0x00,
	0x00, //cadrx_ui_ref_slope4
	0x00,
	0x01, //cadrx_reg_ref_c1_offset
	0x0e,
	0x01, //cadrx_reg_ref_c2_offset
	0x3a,
	0x01, //cadrx_reg_ref_c3_offset
	0x44,
	0x00, //cadrx_reg_ref_c1_slope
	0x01,
	0x00, //cadrx_reg_ref_c2_slope
	0x43,
	0x00, //cadrx_reg_ref_c3_slope
	0x4b,
	0x00, //cadrx_sc_reg_a1
	0x18,
	0x00, //cadrx_sc_reg_b1
	0x1c,
	0x01, //cadrx_sc_k1_int
	0xff,
	0x01, //cadrx_sc_k2_int
	0xff,
	0x03, //cadrx_sc_w1_int
	0xff,
	0xe8,
	0x03, //cadrx_sc_w2_int
	0xff,
	0xe8,
	0x00, //flicker_w_en
	0x14, //flicker_w
	0x01, //flicker_trans_en
	0x04, //flicker_trans_slope
	0x00, //bl_en
	0x03, //bl_hbs_th
	0x10, //bl_lbs_th
	0xC8, //bl_illum_gain
	0x01, //bl_reg_gain 9
	0x2C,
	0x00, //bl_le_diff
	0x00, //bl_cadrx_gain
	0x04, //bl_trans_slope
	0x00, //le_en
	0x40, //le_diff
	0x03, //le_lmax 10
	0xB6,
	0x23, //le_p
	0x24, //le_w_tmf
	0x00, //le_w_struct_diff 9
	0x72,
	0x01, //le_w_struct_diff_gain
	0x00, //le_w_luminance 9
	0x0D,
	0x00, //le_luminance_slope 10
	0x04,
	0x80, //glare_blk_avg_th 0000 0000
	0x00, //glare_luma_gain 9
	0x00,
	0x00, //glare_luma_ctl_start 0000 0000
	0x05, //glare_luma_th 0000 0000
	0x00, //glare_luma_step 9
	0x01,
	0x01, //glare_uni_luma_gain 9
	0x00,
	0xf0, //glare_blk_max_th 0000 0000
	0x12, //glare_blk_max_w 0000 0000
	0x18, //glare_blk_max_curve blk_max_sh 0 0000
	0xf0, //glare_con_ext_max_th 0000 0000
	0x12, //glare_con_ext_max_w 0000 0000
	0x38, //glare_con_ext_max_sign con_ext_max_curve con_ext_max_sh 0 0 0000
	0x00, //glare_max_cnt_th 0000
	0x00, //glare_max_cnt_th 0000 0000
	0x02, //glare_max_cnt_w 0000 0000
	0x38, //glare_max_cnt_sign max_cnt_curve max_cnt_sh 0 0 0000
	0x00, //glare_y_avg_th 0000 0000
	0x18, //glare_y_avg_w 0000 0000
	0x38, //glare_y_avg_sign y_avg_curve y_avg_sh 0 0 0000
	0x00, //glare_max_cnt_th_dmc 0000
	0x05, //glare_max_cnt_th_dmc 0000 0000
	0x00, //nr fa de cs gamma 0 0000
	0xff, //nr_mask_th
	0x00, //de_gain 10
	0x00,
	0x02, //de_maxplus 11
	0x00,
	0x02, //de_maxminus 11
	0x00,
	0x14, //fa_edge_th
	0x00, //fa_step_p 13
	0x01,
	0x00, //fa_step_n 13
	0x01,
	0x00, //fa_max_de_gain 13
	0x10,
	0x00, //fa_pcl_ppi 14
	0x00,
	0x28, //fa_os_cnt_10_co
	0x3c, //fa_os_cnt_20_co
	0x04, //fa_edge_cnt_weight
	0x0f, //fa_avg_y_weight
	0x01, //cs_gain 10
	0x00,
	0x00, //curve_0
	0x10, //curve_1
	0x20, //curve_2
	0x30, //curve_3
	0x40, //curve_4
	0x50, //curve_5
	0x60, //curve_6
	0x70, //curve_7
	0x80, //curve_8
	0x90, //curve_9
	0xa0, //curve_10
	0xb0, //curve_11
	0xc0, //curve_12
	0xd0, //curve_13
	0xe0, //curve_14
	0xf0, //curve_15
	0x01, //curve_16
	0x00,
	0x00, //curve_offset 10
	0x00,
	0x00, //curve_low_x
	0x00, //blank
	0x00, //curve_low_y
};

static char DSI_EBOOK_AUTO_MDNIE_3[] = {
	0xDD,
	0x01, //mdnie_en
	0x00, //mask 00 0000
	0x03, //edge 00
	0xf0, //ascr algo aolce gamut 00 00 00 00
	0xff, //partial_en1
	0xff, //partial_en2
	0x00, //roi_en
	0x00, //roi_block
	0x00, //roi_sx
	0x00, //roi_sx
	0x00, //roi_sy
	0x00, //roi_sy
	0x00, //roi_ex
	0x00, //roi_ex
	0x00, //roi_ey
	0x00, //roi_ey
	0xff, //trans_on trans_block 0 000 0000
	0x04, //trans_slope
	0x01, //trans_interval
	//end
};

static char DSI_EMAIL_AUTO_MDNIE_1[] = {
	//start
	0xDF,
	0x01, //linear_on ascr_skin_on 0000 0000
	0x6a, //ascr_skin_cb
	0x9a, //ascr_skin_cr
	0x25, //ascr_dist_up
	0x1a, //ascr_dist_down
	0x16, //ascr_dist_right
	0x2a, //ascr_dist_left
	0x00, //ascr_div_up 20
	0x37,
	0x5a,
	0x00, //ascr_div_down
	0x4e,
	0xc5,
	0x00, //ascr_div_right
	0x5d,
	0x17,
	0x00, //ascr_div_left
	0x30,
	0xc3,
	0xff, //ascr_skin_Rr
	0x00, //ascr_skin_Rg
	0x00, //ascr_skin_Rb
	0xff, //ascr_skin_Yr
	0xff, //ascr_skin_Yg
	0x00, //ascr_skin_Yb
	0xff, //ascr_skin_Mr
	0x00, //ascr_skin_Mg
	0xff, //ascr_skin_Mb
	0xff, //ascr_skin_Wr
	0xfb, //ascr_skin_Wg
	0xee, //ascr_skin_Wb
	0x00, //ascr_Cr
	0xff, //ascr_Rr
	0xff, //ascr_Cg
	0x00, //ascr_Rg
	0xff, //ascr_Cb
	0x00, //ascr_Rb
	0xff, //ascr_Mr
	0x00, //ascr_Gr
	0x00, //ascr_Mg
	0xff, //ascr_Gg
	0xff, //ascr_Mb
	0x00, //ascr_Gb
	0xff, //ascr_Yr
	0x00, //ascr_Br
	0xff, //ascr_Yg
	0x00, //ascr_Bg
	0x00, //ascr_Yb
	0xff, //ascr_Bb
	0xff, //ascr_Wr
	0x00, //ascr_Kr
	0xfb, //ascr_Wg
	0x00, //ascr_Kg
	0xee, //ascr_Wb
	0x00, //ascr_Kb
	0x0F, //edge_ctrl
	0x00, //edge_radius 12
	0x6F,
	0x05, //edge_aa_factor
	0x00, //edge_r
	0x00, //edge_g
	0x00, //edge_b
	0x00, //edge_x 11
	0x70,
	0x00, //edge_y 12
	0x70,
	0x44, //edge_coef_a edge_coef_b 0000 0000
};

static char DSI_EMAIL_AUTO_MDNIE_2[] = {
	0xDE,
	0x00, //gamut_gamma_on gamut_blk_shift 0000 0000
	0x40, //gamut_scale
	0x04, //gamut_r1
	0x00,
	0x00, //gamut_r2
	0x00,
	0x00, //gamut_r3
	0x00,
	0x00, //gamut_g1
	0x00,
	0x04, //gamut_g2
	0x00,
	0x00, //gamut_g3
	0x00,
	0x00, //gamut_b1
	0x00,
	0x00, //gamut_b2
	0x00,
	0x04, //gamut_b3
	0x00,
	0x00, //slce_on cadrx_en 0000 0000
	0x00, //glare_on
	0x00, //lce_gain 000 0000
	0x24, //lce_color_gain 00 0000
	0x96, //lce_min_ref_offset
	0xb3, //lce_illum_gain
	0x01, //lce_ref_offset 9
	0x0e,
	0x01, //lce_ref_gain 9
	0x00,
	0x77, //lce_block_size h v 0000 0000
	0x03, //lce_dark_th 000
	0x07, //lce_reduct_slope 0000
	0x40, //lce_black cc0 cc1 color_th 0 0 0 0000
	0x08, //lce_large_w
	0x04, //lce_med_w
	0x04, //lce_small_w
	0x00, //cadrx_dit_en shadow_corr_en ref_ctrl_en 0 0 0
	0x00, //cadrx_gain
	0x0d, //cadrx_low_intensity_th
	0xe1, //cadrx_high_intensity_th
	0x07, //cadrx_ui_zerobincnt_th
	0x29, //cadrx_ui_ratio_th
	0x04,
	0x0f, //cadrx_entire_freq
	0xa0,
	0x00,
	0x03, //cadrx_peak_th_in_freq
	0x7a,
	0xa0,
	0x00, //cadrx_high_peak_th_in_freq
	0x25,
	0x1c,
	0x24, //cadrx_dit_color_gain
	0x00, //cadrx_dit_illum_a0
	0x48, //cadrx_dit_illum_a1
	0xea, //cadrx_dit_illum_a2
	0xfa, //cadrx_dit_illum_a3
	0x00, //cadrx_dit_illum_b0
	0x10,
	0x00, //cadrx_dit_illum_b1
	0x10,
	0x00, //cadrx_dit_illum_b2
	0x9c,
	0x01, //cadrx_dit_illum_b3
	0x00,
	0x00, //cadrx_dit_illum_slope1
	0x00,
	0x00, //cadrx_dit_illum_slope2
	0xdc,
	0x03, //cadrx_dit_illum_slope3
	0xff,
	0x40, //cadrx_ui_illum_a1
	0x80, //cadrx_ui_illum_a2
	0xc0, //cadrx_ui_illum_a3
	0x00, //cadrx_ui_illum_offset1
	0x9a,
	0x00, //cadrx_ui_illum_offset2
	0x9a,
	0x00, //cadrx_ui_illum_offset3
	0x9a,
	0x00, //cadrx_ui_illum_offset4
	0x9a,
	0x00, //cadrx_ui_illum_slope1
	0x00,
	0x00, //cadrx_ui_illum_slope2
	0x00,
	0x00, //cadrx_ui_illum_slope3
	0x00,
	0x00, //cadrx_ui_illum_slope4
	0x00,
	0x40, //cadrx_ui_ref_a1
	0x80, //cadrx_ui_ref_a2
	0xc0, //cadrx_ui_ref_a3
	0x01, //cadrx_ui_ref_offset1
	0x0e,
	0x01, //cadrx_ui_ref_offset2
	0x0e,
	0x01, //cadrx_ui_ref_offset3
	0x0e,
	0x01, //cadrx_ui_ref_offset4
	0x0e,
	0x00, //cadrx_ui_ref_slope1
	0x00,
	0x00, //cadrx_ui_ref_slope2
	0x00,
	0x00, //cadrx_ui_ref_slope3
	0x00,
	0x00, //cadrx_ui_ref_slope4
	0x00,
	0x01, //cadrx_reg_ref_c1_offset
	0x0e,
	0x01, //cadrx_reg_ref_c2_offset
	0x3a,
	0x01, //cadrx_reg_ref_c3_offset
	0x44,
	0x00, //cadrx_reg_ref_c1_slope
	0x01,
	0x00, //cadrx_reg_ref_c2_slope
	0x43,
	0x00, //cadrx_reg_ref_c3_slope
	0x4b,
	0x00, //cadrx_sc_reg_a1
	0x18,
	0x00, //cadrx_sc_reg_b1
	0x1c,
	0x01, //cadrx_sc_k1_int
	0xff,
	0x01, //cadrx_sc_k2_int
	0xff,
	0x03, //cadrx_sc_w1_int
	0xff,
	0xe8,
	0x03, //cadrx_sc_w2_int
	0xff,
	0xe8,
	0x00, //flicker_w_en
	0x14, //flicker_w
	0x01, //flicker_trans_en
	0x04, //flicker_trans_slope
	0x00, //bl_en
	0x03, //bl_hbs_th
	0x10, //bl_lbs_th
	0xC8, //bl_illum_gain
	0x01, //bl_reg_gain 9
	0x2C,
	0x00, //bl_le_diff
	0x00, //bl_cadrx_gain
	0x04, //bl_trans_slope
	0x00, //le_en
	0x40, //le_diff
	0x03, //le_lmax 10
	0xB6,
	0x23, //le_p
	0x24, //le_w_tmf
	0x00, //le_w_struct_diff 9
	0x72,
	0x01, //le_w_struct_diff_gain
	0x00, //le_w_luminance 9
	0x0D,
	0x00, //le_luminance_slope 10
	0x04,
	0x80, //glare_blk_avg_th 0000 0000
	0x00, //glare_luma_gain 9
	0x28,
	0xa0, //glare_luma_ctl_start 0000 0000
	0x01, //glare_luma_th 0000 0000
	0x00, //glare_luma_step 9
	0x01,
	0x00, //glare_uni_luma_gain 9
	0x01,
	0x00, //glare_blk_max_th 0000 0000
	0x00, //glare_blk_max_w 0000 0000
	0x10, //glare_blk_max_curve blk_max_sh 0 0000
	0x00, //glare_con_ext_max_th 0000 0000
	0x00, //glare_con_ext_max_w 0000 0000
	0x30, //glare_con_ext_max_sign con_ext_max_curve con_ext_max_sh 0 0 0000
	0x00, //glare_max_cnt_th 0000
	0x00, //glare_max_cnt_th 0000 0000
	0x00, //glare_max_cnt_w 0000 0000
	0x30, //glare_max_cnt_sign max_cnt_curve max_cnt_sh 0 0 0000
	0x00, //glare_y_avg_th 0000 0000
	0x00, //glare_y_avg_w 0000 0000
	0x30, //glare_y_avg_sign y_avg_curve y_avg_sh 0 0 0000
	0x00, //glare_max_cnt_th_dmc 0000
	0x00, //glare_max_cnt_th_dmc 0000 0000
	0x00, //nr fa de cs gamma 0 0000
	0xff, //nr_mask_th
	0x00, //de_gain 10
	0x00,
	0x02, //de_maxplus 11
	0x00,
	0x02, //de_maxminus 11
	0x00,
	0x14, //fa_edge_th
	0x00, //fa_step_p 13
	0x01,
	0x00, //fa_step_n 13
	0x01,
	0x00, //fa_max_de_gain 13
	0x10,
	0x00, //fa_pcl_ppi 14
	0x00,
	0x28, //fa_os_cnt_10_co
	0x3c, //fa_os_cnt_20_co
	0x04, //fa_edge_cnt_weight
	0x0f, //fa_avg_y_weight
	0x01, //cs_gain 10
	0x00,
	0x00, //curve_0
	0x10, //curve_1
	0x20, //curve_2
	0x30, //curve_3
	0x40, //curve_4
	0x50, //curve_5
	0x60, //curve_6
	0x70, //curve_7
	0x80, //curve_8
	0x90, //curve_9
	0xa0, //curve_10
	0xb0, //curve_11
	0xc0, //curve_12
	0xd0, //curve_13
	0xe0, //curve_14
	0xf0, //curve_15
	0x01, //curve_16
	0x00,
	0x00, //curve_offset 10
	0x00,
	0x00, //curve_low_x
	0x00, //blank
	0x00, //curve_low_y
};

static char DSI_EMAIL_AUTO_MDNIE_3[] = {
	0xDD,
	0x01, //mdnie_en
	0x00, //mask 00 0000
	0x03, //edge 00
	0xf0, //ascr algo aolce gamut 00 00 00 00
	0xff, //partial_en1
	0xff, //partial_en2
	0x00, //roi_en
	0x00, //roi_block
	0x00, //roi_sx
	0x00, //roi_sx
	0x00, //roi_sy
	0x00, //roi_sy
	0x00, //roi_ex
	0x00, //roi_ex
	0x00, //roi_ey
	0x00, //roi_ey
	0xff, //trans_on trans_block 0 000 0000
	0x04, //trans_slope
	0x01, //trans_interval
	//end
};

static char DSI_TDMB_STANDARD_MDNIE_1[] = {
	//start
	0xDF,
	0x11, //linear_on ascr_skin_on 0000 0000
	0x6a, //ascr_skin_cb
	0x9a, //ascr_skin_cr
	0x25, //ascr_dist_up
	0x1a, //ascr_dist_down
	0x16, //ascr_dist_right
	0x2a, //ascr_dist_left
	0x00, //ascr_div_up 20
	0x37,
	0x5a,
	0x00, //ascr_div_down
	0x4e,
	0xc5,
	0x00, //ascr_div_right
	0x5d,
	0x17,
	0x00, //ascr_div_left
	0x30,
	0xc3,
	0xee, //ascr_skin_Rr
	0x4b, //ascr_skin_Rg
	0x30, //ascr_skin_Rb
	0xef, //ascr_skin_Yr
	0xeb, //ascr_skin_Yg
	0x47, //ascr_skin_Yb
	0xfc, //ascr_skin_Mr
	0x57, //ascr_skin_Mg
	0xe7, //ascr_skin_Mb
	0xff, //ascr_skin_Wr
	0xfa, //ascr_skin_Wg
	0xf1, //ascr_skin_Wb
	0x00, //ascr_Cr
	0xee, //ascr_Rr
	0xe7, //ascr_Cg
	0x4b, //ascr_Rg
	0xe5, //ascr_Cb
	0x30, //ascr_Rb
	0xfc, //ascr_Mr
	0x00, //ascr_Gr
	0x57, //ascr_Mg
	0xd9, //ascr_Gg
	0xe7, //ascr_Mb
	0x34, //ascr_Gb
	0xef, //ascr_Yr
	0x34, //ascr_Br
	0xeb, //ascr_Yg
	0x20, //ascr_Bg
	0x47, //ascr_Yb
	0xdb, //ascr_Bb
	0xff, //ascr_Wr
	0x00, //ascr_Kr
	0xfa, //ascr_Wg
	0x00, //ascr_Kg
	0xf1, //ascr_Wb
	0x00, //ascr_Kb
	0x0F, //edge_ctrl
	0x00, //edge_radius 12
	0x6F,
	0x05, //edge_aa_factor
	0x00, //edge_r
	0x00, //edge_g
	0x00, //edge_b
	0x00, //edge_x 11
	0x70,
	0x00, //edge_y 12
	0x70,
	0x44, //edge_coef_a edge_coef_b 0000 0000
};

static char DSI_TDMB_STANDARD_MDNIE_2[] = {
	0xDE,
	0x00, //gamut_gamma_on gamut_blk_shift 0000 0000
	0x40, //gamut_scale
	0x04, //gamut_r1
	0x00,
	0x00, //gamut_r2
	0x00,
	0x00, //gamut_r3
	0x00,
	0x00, //gamut_g1
	0x00,
	0x04, //gamut_g2
	0x00,
	0x00, //gamut_g3
	0x00,
	0x00, //gamut_b1
	0x00,
	0x00, //gamut_b2
	0x00,
	0x04, //gamut_b3
	0x00,
	0x00, //slce_on cadrx_en 0000 0000
	0x00, //glare_on
	0x00, //lce_gain 000 0000
	0x24, //lce_color_gain 00 0000
	0x96, //lce_min_ref_offset
	0xb3, //lce_illum_gain
	0x01, //lce_ref_offset 9
	0x0e,
	0x01, //lce_ref_gain 9
	0x00,
	0x77, //lce_block_size h v 0000 0000
	0x03, //lce_dark_th 000
	0x07, //lce_reduct_slope 0000
	0x40, //lce_black cc0 cc1 color_th 0 0 0 0000
	0x08, //lce_large_w
	0x04, //lce_med_w
	0x04, //lce_small_w
	0x00, //cadrx_dit_en shadow_corr_en ref_ctrl_en 0 0 0
	0x00, //cadrx_gain
	0x0d, //cadrx_low_intensity_th
	0xe1, //cadrx_high_intensity_th
	0x07, //cadrx_ui_zerobincnt_th
	0x29, //cadrx_ui_ratio_th
	0x04,
	0x0f, //cadrx_entire_freq
	0xa0,
	0x00,
	0x03, //cadrx_peak_th_in_freq
	0x7a,
	0xa0,
	0x00, //cadrx_high_peak_th_in_freq
	0x25,
	0x1c,
	0x24, //cadrx_dit_color_gain
	0x00, //cadrx_dit_illum_a0
	0x48, //cadrx_dit_illum_a1
	0xea, //cadrx_dit_illum_a2
	0xfa, //cadrx_dit_illum_a3
	0x00, //cadrx_dit_illum_b0
	0x10,
	0x00, //cadrx_dit_illum_b1
	0x10,
	0x00, //cadrx_dit_illum_b2
	0x9c,
	0x01, //cadrx_dit_illum_b3
	0x00,
	0x00, //cadrx_dit_illum_slope1
	0x00,
	0x00, //cadrx_dit_illum_slope2
	0xdc,
	0x03, //cadrx_dit_illum_slope3
	0xff,
	0x40, //cadrx_ui_illum_a1
	0x80, //cadrx_ui_illum_a2
	0xc0, //cadrx_ui_illum_a3
	0x00, //cadrx_ui_illum_offset1
	0x9a,
	0x00, //cadrx_ui_illum_offset2
	0x9a,
	0x00, //cadrx_ui_illum_offset3
	0x9a,
	0x00, //cadrx_ui_illum_offset4
	0x9a,
	0x00, //cadrx_ui_illum_slope1
	0x00,
	0x00, //cadrx_ui_illum_slope2
	0x00,
	0x00, //cadrx_ui_illum_slope3
	0x00,
	0x00, //cadrx_ui_illum_slope4
	0x00,
	0x40, //cadrx_ui_ref_a1
	0x80, //cadrx_ui_ref_a2
	0xc0, //cadrx_ui_ref_a3
	0x01, //cadrx_ui_ref_offset1
	0x0e,
	0x01, //cadrx_ui_ref_offset2
	0x0e,
	0x01, //cadrx_ui_ref_offset3
	0x0e,
	0x01, //cadrx_ui_ref_offset4
	0x0e,
	0x00, //cadrx_ui_ref_slope1
	0x00,
	0x00, //cadrx_ui_ref_slope2
	0x00,
	0x00, //cadrx_ui_ref_slope3
	0x00,
	0x00, //cadrx_ui_ref_slope4
	0x00,
	0x01, //cadrx_reg_ref_c1_offset
	0x0e,
	0x01, //cadrx_reg_ref_c2_offset
	0x3a,
	0x01, //cadrx_reg_ref_c3_offset
	0x44,
	0x00, //cadrx_reg_ref_c1_slope
	0x01,
	0x00, //cadrx_reg_ref_c2_slope
	0x43,
	0x00, //cadrx_reg_ref_c3_slope
	0x4b,
	0x00, //cadrx_sc_reg_a1
	0x18,
	0x00, //cadrx_sc_reg_b1
	0x1c,
	0x01, //cadrx_sc_k1_int
	0xff,
	0x01, //cadrx_sc_k2_int
	0xff,
	0x03, //cadrx_sc_w1_int
	0xff,
	0xe8,
	0x03, //cadrx_sc_w2_int
	0xff,
	0xe8,
	0x00, //flicker_w_en
	0x14, //flicker_w
	0x01, //flicker_trans_en
	0x04, //flicker_trans_slope
	0x00, //bl_en
	0x03, //bl_hbs_th
	0x10, //bl_lbs_th
	0xC8, //bl_illum_gain
	0x01, //bl_reg_gain 9
	0x2C,
	0x00, //bl_le_diff
	0x00, //bl_cadrx_gain
	0x04, //bl_trans_slope
	0x00, //le_en
	0x40, //le_diff
	0x03, //le_lmax 10
	0xB6,
	0x23, //le_p
	0x24, //le_w_tmf
	0x00, //le_w_struct_diff 9
	0x72,
	0x01, //le_w_struct_diff_gain
	0x00, //le_w_luminance 9
	0x0D,
	0x00, //le_luminance_slope 10
	0x04,
	0x80, //glare_blk_avg_th 0000 0000
	0x00, //glare_luma_gain 9
	0x28,
	0xa0, //glare_luma_ctl_start 0000 0000
	0x01, //glare_luma_th 0000 0000
	0x00, //glare_luma_step 9
	0x01,
	0x00, //glare_uni_luma_gain 9
	0x01,
	0x00, //glare_blk_max_th 0000 0000
	0x00, //glare_blk_max_w 0000 0000
	0x10, //glare_blk_max_curve blk_max_sh 0 0000
	0x00, //glare_con_ext_max_th 0000 0000
	0x00, //glare_con_ext_max_w 0000 0000
	0x30, //glare_con_ext_max_sign con_ext_max_curve con_ext_max_sh 0 0 0000
	0x00, //glare_max_cnt_th 0000
	0x00, //glare_max_cnt_th 0000 0000
	0x00, //glare_max_cnt_w 0000 0000
	0x30, //glare_max_cnt_sign max_cnt_curve max_cnt_sh 0 0 0000
	0x00, //glare_y_avg_th 0000 0000
	0x00, //glare_y_avg_w 0000 0000
	0x30, //glare_y_avg_sign y_avg_curve y_avg_sh 0 0 0000
	0x00, //glare_max_cnt_th_dmc 0000
	0x00, //glare_max_cnt_th_dmc 0000 0000
	0x06, //nr fa de cs gamma 0 0000
	0xff, //nr_mask_th
	0x00, //de_gain 10
	0x10,
	0x02, //de_maxplus 11
	0x00,
	0x02, //de_maxminus 11
	0x00,
	0x14, //fa_edge_th
	0x00, //fa_step_p 13
	0x01,
	0x00, //fa_step_n 13
	0x01,
	0x00, //fa_max_de_gain 13
	0x10,
	0x00, //fa_pcl_ppi 14
	0x00,
	0x28, //fa_os_cnt_10_co
	0x3c, //fa_os_cnt_20_co
	0x04, //fa_edge_cnt_weight
	0x0f, //fa_avg_y_weight
	0x01, //cs_gain 10
	0x18,
	0x00, //curve_0
	0x10, //curve_1
	0x20, //curve_2
	0x30, //curve_3
	0x40, //curve_4
	0x50, //curve_5
	0x60, //curve_6
	0x70, //curve_7
	0x80, //curve_8
	0x90, //curve_9
	0xa0, //curve_10
	0xb0, //curve_11
	0xc0, //curve_12
	0xd0, //curve_13
	0xe0, //curve_14
	0xf0, //curve_15
	0x01, //curve_16
	0x00,
	0x00, //curve_offset 10
	0x00,
	0x00, //curve_low_x
	0x00, //blank
	0x00, //curve_low_y
};

static char DSI_TDMB_STANDARD_MDNIE_3[] = {
	0xDD,
	0x01, //mdnie_en
	0x00, //mask 00 0000
	0x03, //edge 00
	0xf0, //ascr algo aolce gamut 00 00 00 00
	0xff, //partial_en1
	0xff, //partial_en2
	0x00, //roi_en
	0x00, //roi_block
	0x00, //roi_sx
	0x00, //roi_sx
	0x00, //roi_sy
	0x00, //roi_sy
	0x00, //roi_ex
	0x00, //roi_ex
	0x00, //roi_ey
	0x00, //roi_ey
	0xff, //trans_on trans_block 0 000 0000
	0x04, //trans_slope
	0x01, //trans_interval
	//end
};

static char DSI_TDMB_NATURAL_MDNIE_1[] = {
	//start
	0xDF,
	0x11, //linear_on ascr_skin_on 0000 0000
	0x6a, //ascr_skin_cb
	0x9a, //ascr_skin_cr
	0x25, //ascr_dist_up
	0x1a, //ascr_dist_down
	0x16, //ascr_dist_right
	0x2a, //ascr_dist_left
	0x00, //ascr_div_up 20
	0x37,
	0x5a,
	0x00, //ascr_div_down
	0x4e,
	0xc5,
	0x00, //ascr_div_right
	0x5d,
	0x17,
	0x00, //ascr_div_left
	0x30,
	0xc3,
	0xd3, //ascr_skin_Rr
	0x40, //ascr_skin_Rg
	0x29, //ascr_skin_Rb
	0xf0, //ascr_skin_Yr
	0xec, //ascr_skin_Yg
	0x5b, //ascr_skin_Yb
	0xdf, //ascr_skin_Mr
	0x4a, //ascr_skin_Mg
	0xdf, //ascr_skin_Mb
	0xff, //ascr_skin_Wr
	0xfa, //ascr_skin_Wg
	0xf1, //ascr_skin_Wb
	0x93, //ascr_Cr
	0xd3, //ascr_Rr
	0xed, //ascr_Cg
	0x40, //ascr_Rg
	0xe7, //ascr_Cb
	0x29, //ascr_Rb
	0xdf, //ascr_Mr
	0x85, //ascr_Gr
	0x4a, //ascr_Mg
	0xe0, //ascr_Gg
	0xdf, //ascr_Mb
	0x4f, //ascr_Gb
	0xf0, //ascr_Yr
	0x32, //ascr_Br
	0xec, //ascr_Yg
	0x1f, //ascr_Bg
	0x5b, //ascr_Yb
	0xd8, //ascr_Bb
	0xff, //ascr_Wr
	0x00, //ascr_Kr
	0xfa, //ascr_Wg
	0x00, //ascr_Kg
	0xf1, //ascr_Wb
	0x00, //ascr_Kb
	0x0F, //edge_ctrl
	0x00, //edge_radius 12
	0x6F,
	0x05, //edge_aa_factor
	0x00, //edge_r
	0x00, //edge_g
	0x00, //edge_b
	0x00, //edge_x 11
	0x70,
	0x00, //edge_y 12
	0x70,
	0x44, //edge_coef_a edge_coef_b 0000 0000
};

static char DSI_TDMB_NATURAL_MDNIE_2[] = {
	0xDE,
	0x00, //gamut_gamma_on gamut_blk_shift 0000 0000
	0x40, //gamut_scale
	0x04, //gamut_r1
	0x00,
	0x00, //gamut_r2
	0x00,
	0x00, //gamut_r3
	0x00,
	0x00, //gamut_g1
	0x00,
	0x04, //gamut_g2
	0x00,
	0x00, //gamut_g3
	0x00,
	0x00, //gamut_b1
	0x00,
	0x00, //gamut_b2
	0x00,
	0x04, //gamut_b3
	0x00,
	0x00, //slce_on cadrx_en 0000 0000
	0x00, //glare_on
	0x00, //lce_gain 000 0000
	0x24, //lce_color_gain 00 0000
	0x96, //lce_min_ref_offset
	0xb3, //lce_illum_gain
	0x01, //lce_ref_offset 9
	0x0e,
	0x01, //lce_ref_gain 9
	0x00,
	0x77, //lce_block_size h v 0000 0000
	0x03, //lce_dark_th 000
	0x07, //lce_reduct_slope 0000
	0x40, //lce_black cc0 cc1 color_th 0 0 0 0000
	0x08, //lce_large_w
	0x04, //lce_med_w
	0x04, //lce_small_w
	0x00, //cadrx_dit_en shadow_corr_en ref_ctrl_en 0 0 0
	0x00, //cadrx_gain
	0x0d, //cadrx_low_intensity_th
	0xe1, //cadrx_high_intensity_th
	0x07, //cadrx_ui_zerobincnt_th
	0x29, //cadrx_ui_ratio_th
	0x04,
	0x0f, //cadrx_entire_freq
	0xa0,
	0x00,
	0x03, //cadrx_peak_th_in_freq
	0x7a,
	0xa0,
	0x00, //cadrx_high_peak_th_in_freq
	0x25,
	0x1c,
	0x24, //cadrx_dit_color_gain
	0x00, //cadrx_dit_illum_a0
	0x48, //cadrx_dit_illum_a1
	0xea, //cadrx_dit_illum_a2
	0xfa, //cadrx_dit_illum_a3
	0x00, //cadrx_dit_illum_b0
	0x10,
	0x00, //cadrx_dit_illum_b1
	0x10,
	0x00, //cadrx_dit_illum_b2
	0x9c,
	0x01, //cadrx_dit_illum_b3
	0x00,
	0x00, //cadrx_dit_illum_slope1
	0x00,
	0x00, //cadrx_dit_illum_slope2
	0xdc,
	0x03, //cadrx_dit_illum_slope3
	0xff,
	0x40, //cadrx_ui_illum_a1
	0x80, //cadrx_ui_illum_a2
	0xc0, //cadrx_ui_illum_a3
	0x00, //cadrx_ui_illum_offset1
	0x9a,
	0x00, //cadrx_ui_illum_offset2
	0x9a,
	0x00, //cadrx_ui_illum_offset3
	0x9a,
	0x00, //cadrx_ui_illum_offset4
	0x9a,
	0x00, //cadrx_ui_illum_slope1
	0x00,
	0x00, //cadrx_ui_illum_slope2
	0x00,
	0x00, //cadrx_ui_illum_slope3
	0x00,
	0x00, //cadrx_ui_illum_slope4
	0x00,
	0x40, //cadrx_ui_ref_a1
	0x80, //cadrx_ui_ref_a2
	0xc0, //cadrx_ui_ref_a3
	0x01, //cadrx_ui_ref_offset1
	0x0e,
	0x01, //cadrx_ui_ref_offset2
	0x0e,
	0x01, //cadrx_ui_ref_offset3
	0x0e,
	0x01, //cadrx_ui_ref_offset4
	0x0e,
	0x00, //cadrx_ui_ref_slope1
	0x00,
	0x00, //cadrx_ui_ref_slope2
	0x00,
	0x00, //cadrx_ui_ref_slope3
	0x00,
	0x00, //cadrx_ui_ref_slope4
	0x00,
	0x01, //cadrx_reg_ref_c1_offset
	0x0e,
	0x01, //cadrx_reg_ref_c2_offset
	0x3a,
	0x01, //cadrx_reg_ref_c3_offset
	0x44,
	0x00, //cadrx_reg_ref_c1_slope
	0x01,
	0x00, //cadrx_reg_ref_c2_slope
	0x43,
	0x00, //cadrx_reg_ref_c3_slope
	0x4b,
	0x00, //cadrx_sc_reg_a1
	0x18,
	0x00, //cadrx_sc_reg_b1
	0x1c,
	0x01, //cadrx_sc_k1_int
	0xff,
	0x01, //cadrx_sc_k2_int
	0xff,
	0x03, //cadrx_sc_w1_int
	0xff,
	0xe8,
	0x03, //cadrx_sc_w2_int
	0xff,
	0xe8,
	0x00, //flicker_w_en
	0x14, //flicker_w
	0x01, //flicker_trans_en
	0x04, //flicker_trans_slope
	0x00, //bl_en
	0x03, //bl_hbs_th
	0x10, //bl_lbs_th
	0xC8, //bl_illum_gain
	0x01, //bl_reg_gain 9
	0x2C,
	0x00, //bl_le_diff
	0x00, //bl_cadrx_gain
	0x04, //bl_trans_slope
	0x00, //le_en
	0x40, //le_diff
	0x03, //le_lmax 10
	0xB6,
	0x23, //le_p
	0x24, //le_w_tmf
	0x00, //le_w_struct_diff 9
	0x72,
	0x01, //le_w_struct_diff_gain
	0x00, //le_w_luminance 9
	0x0D,
	0x00, //le_luminance_slope 10
	0x04,
	0x80, //glare_blk_avg_th 0000 0000
	0x00, //glare_luma_gain 9
	0x28,
	0xa0, //glare_luma_ctl_start 0000 0000
	0x01, //glare_luma_th 0000 0000
	0x00, //glare_luma_step 9
	0x01,
	0x00, //glare_uni_luma_gain 9
	0x01,
	0x00, //glare_blk_max_th 0000 0000
	0x00, //glare_blk_max_w 0000 0000
	0x10, //glare_blk_max_curve blk_max_sh 0 0000
	0x00, //glare_con_ext_max_th 0000 0000
	0x00, //glare_con_ext_max_w 0000 0000
	0x30, //glare_con_ext_max_sign con_ext_max_curve con_ext_max_sh 0 0 0000
	0x00, //glare_max_cnt_th 0000
	0x00, //glare_max_cnt_th 0000 0000
	0x00, //glare_max_cnt_w 0000 0000
	0x30, //glare_max_cnt_sign max_cnt_curve max_cnt_sh 0 0 0000
	0x00, //glare_y_avg_th 0000 0000
	0x00, //glare_y_avg_w 0000 0000
	0x30, //glare_y_avg_sign y_avg_curve y_avg_sh 0 0 0000
	0x00, //glare_max_cnt_th_dmc 0000
	0x00, //glare_max_cnt_th_dmc 0000 0000
	0x04, //nr fa de cs gamma 0 0000
	0xff, //nr_mask_th
	0x00, //de_gain 10
	0x10,
	0x02, //de_maxplus 11
	0x00,
	0x02, //de_maxminus 11
	0x00,
	0x14, //fa_edge_th
	0x00, //fa_step_p 13
	0x01,
	0x00, //fa_step_n 13
	0x01,
	0x00, //fa_max_de_gain 13
	0x10,
	0x00, //fa_pcl_ppi 14
	0x00,
	0x28, //fa_os_cnt_10_co
	0x3c, //fa_os_cnt_20_co
	0x04, //fa_edge_cnt_weight
	0x0f, //fa_avg_y_weight
	0x01, //cs_gain 10
	0x00,
	0x00, //curve_0
	0x10, //curve_1
	0x20, //curve_2
	0x30, //curve_3
	0x40, //curve_4
	0x50, //curve_5
	0x60, //curve_6
	0x70, //curve_7
	0x80, //curve_8
	0x90, //curve_9
	0xa0, //curve_10
	0xb0, //curve_11
	0xc0, //curve_12
	0xd0, //curve_13
	0xe0, //curve_14
	0xf0, //curve_15
	0x01, //curve_16
	0x00,
	0x00, //curve_offset 10
	0x00,
	0x00, //curve_low_x
	0x00, //blank
	0x00, //curve_low_y
};

static char DSI_TDMB_NATURAL_MDNIE_3[] = {
	0xDD,
	0x01, //mdnie_en
	0x00, //mask 00 0000
	0x03, //edge 00
	0xf0, //ascr algo aolce gamut 00 00 00 00
	0xff, //partial_en1
	0xff, //partial_en2
	0x00, //roi_en
	0x00, //roi_block
	0x00, //roi_sx
	0x00, //roi_sx
	0x00, //roi_sy
	0x00, //roi_sy
	0x00, //roi_ex
	0x00, //roi_ex
	0x00, //roi_ey
	0x00, //roi_ey
	0xff, //trans_on trans_block 0 000 0000
	0x04, //trans_slope
	0x01, //trans_interval
	//end
};

static char DSI_TDMB_DYNAMIC_MDNIE_1[] = {
	//start
	0xDF,
	0x11, //linear_on ascr_skin_on 0000 0000
	0x67, //ascr_skin_cb
	0xa9, //ascr_skin_cr
	0x37, //ascr_dist_up
	0x29, //ascr_dist_down
	0x19, //ascr_dist_right
	0x47, //ascr_dist_left
	0x00, //ascr_div_up 20
	0x25,
	0x3d,
	0x00, //ascr_div_down
	0x31,
	0xf4,
	0x00, //ascr_div_right
	0x51,
	0xec,
	0x00, //ascr_div_left
	0x1c,
	0xd8,
	0xe1, //ascr_skin_Rr
	0x25, //ascr_skin_Rg
	0x00, //ascr_skin_Rb
	0xee, //ascr_skin_Yr
	0xea, //ascr_skin_Yg
	0x00, //ascr_skin_Yb
	0xf0, //ascr_skin_Mr
	0x33, //ascr_skin_Mg
	0xe8, //ascr_skin_Mb
	0xff, //ascr_skin_Wr
	0xfa, //ascr_skin_Wg
	0xf1, //ascr_skin_Wb
	0x5c, //ascr_Cr
	0xe1, //ascr_Rr
	0xef, //ascr_Cg
	0x25, //ascr_Rg
	0xe8, //ascr_Cb
	0x00, //ascr_Rb
	0xf0, //ascr_Mr
	0x49, //ascr_Gr
	0x33, //ascr_Mg
	0xdf, //ascr_Gg
	0xe8, //ascr_Mb
	0x00, //ascr_Gb
	0xee, //ascr_Yr
	0x37, //ascr_Br
	0xea, //ascr_Yg
	0x21, //ascr_Bg
	0x00, //ascr_Yb
	0xdf, //ascr_Bb
	0xff, //ascr_Wr
	0x00, //ascr_Kr
	0xfa, //ascr_Wg
	0x00, //ascr_Kg
	0xf1, //ascr_Wb
	0x00, //ascr_Kb
	0x0F, //edge_ctrl
	0x00, //edge_radius 12
	0x6F,
	0x05, //edge_aa_factor
	0x00, //edge_r
	0x00, //edge_g
	0x00, //edge_b
	0x00, //edge_x 11
	0x70,
	0x00, //edge_y 12
	0x70,
	0x44, //edge_coef_a edge_coef_b 0000 0000
};

static char DSI_TDMB_DYNAMIC_MDNIE_2[] = {
	0xDE,
	0x00, //gamut_gamma_on gamut_blk_shift 0000 0000
	0x40, //gamut_scale
	0x04, //gamut_r1
	0x00,
	0x00, //gamut_r2
	0x00,
	0x00, //gamut_r3
	0x00,
	0x00, //gamut_g1
	0x00,
	0x04, //gamut_g2
	0x00,
	0x00, //gamut_g3
	0x00,
	0x00, //gamut_b1
	0x00,
	0x00, //gamut_b2
	0x00,
	0x04, //gamut_b3
	0x00,
	0x00, //slce_on cadrx_en 0000 0000
	0x00, //glare_on
	0x00, //lce_gain 000 0000
	0x24, //lce_color_gain 00 0000
	0x96, //lce_min_ref_offset
	0xb3, //lce_illum_gain
	0x01, //lce_ref_offset 9
	0x0e,
	0x01, //lce_ref_gain 9
	0x00,
	0x77, //lce_block_size h v 0000 0000
	0x03, //lce_dark_th 000
	0x07, //lce_reduct_slope 0000
	0x40, //lce_black cc0 cc1 color_th 0 0 0 0000
	0x08, //lce_large_w
	0x04, //lce_med_w
	0x04, //lce_small_w
	0x00, //cadrx_dit_en shadow_corr_en ref_ctrl_en 0 0 0
	0x00, //cadrx_gain
	0x0d, //cadrx_low_intensity_th
	0xe1, //cadrx_high_intensity_th
	0x07, //cadrx_ui_zerobincnt_th
	0x29, //cadrx_ui_ratio_th
	0x04,
	0x0f, //cadrx_entire_freq
	0xa0,
	0x00,
	0x03, //cadrx_peak_th_in_freq
	0x7a,
	0xa0,
	0x00, //cadrx_high_peak_th_in_freq
	0x25,
	0x1c,
	0x24, //cadrx_dit_color_gain
	0x00, //cadrx_dit_illum_a0
	0x48, //cadrx_dit_illum_a1
	0xea, //cadrx_dit_illum_a2
	0xfa, //cadrx_dit_illum_a3
	0x00, //cadrx_dit_illum_b0
	0x10,
	0x00, //cadrx_dit_illum_b1
	0x10,
	0x00, //cadrx_dit_illum_b2
	0x9c,
	0x01, //cadrx_dit_illum_b3
	0x00,
	0x00, //cadrx_dit_illum_slope1
	0x00,
	0x00, //cadrx_dit_illum_slope2
	0xdc,
	0x03, //cadrx_dit_illum_slope3
	0xff,
	0x40, //cadrx_ui_illum_a1
	0x80, //cadrx_ui_illum_a2
	0xc0, //cadrx_ui_illum_a3
	0x00, //cadrx_ui_illum_offset1
	0x9a,
	0x00, //cadrx_ui_illum_offset2
	0x9a,
	0x00, //cadrx_ui_illum_offset3
	0x9a,
	0x00, //cadrx_ui_illum_offset4
	0x9a,
	0x00, //cadrx_ui_illum_slope1
	0x00,
	0x00, //cadrx_ui_illum_slope2
	0x00,
	0x00, //cadrx_ui_illum_slope3
	0x00,
	0x00, //cadrx_ui_illum_slope4
	0x00,
	0x40, //cadrx_ui_ref_a1
	0x80, //cadrx_ui_ref_a2
	0xc0, //cadrx_ui_ref_a3
	0x01, //cadrx_ui_ref_offset1
	0x0e,
	0x01, //cadrx_ui_ref_offset2
	0x0e,
	0x01, //cadrx_ui_ref_offset3
	0x0e,
	0x01, //cadrx_ui_ref_offset4
	0x0e,
	0x00, //cadrx_ui_ref_slope1
	0x00,
	0x00, //cadrx_ui_ref_slope2
	0x00,
	0x00, //cadrx_ui_ref_slope3
	0x00,
	0x00, //cadrx_ui_ref_slope4
	0x00,
	0x01, //cadrx_reg_ref_c1_offset
	0x0e,
	0x01, //cadrx_reg_ref_c2_offset
	0x3a,
	0x01, //cadrx_reg_ref_c3_offset
	0x44,
	0x00, //cadrx_reg_ref_c1_slope
	0x01,
	0x00, //cadrx_reg_ref_c2_slope
	0x43,
	0x00, //cadrx_reg_ref_c3_slope
	0x4b,
	0x00, //cadrx_sc_reg_a1
	0x18,
	0x00, //cadrx_sc_reg_b1
	0x1c,
	0x01, //cadrx_sc_k1_int
	0xff,
	0x01, //cadrx_sc_k2_int
	0xff,
	0x03, //cadrx_sc_w1_int
	0xff,
	0xe8,
	0x03, //cadrx_sc_w2_int
	0xff,
	0xe8,
	0x00, //flicker_w_en
	0x14, //flicker_w
	0x01, //flicker_trans_en
	0x04, //flicker_trans_slope
	0x00, //bl_en
	0x03, //bl_hbs_th
	0x10, //bl_lbs_th
	0xC8, //bl_illum_gain
	0x01, //bl_reg_gain 9
	0x2C,
	0x00, //bl_le_diff
	0x00, //bl_cadrx_gain
	0x04, //bl_trans_slope
	0x00, //le_en
	0x40, //le_diff
	0x03, //le_lmax 10
	0xB6,
	0x23, //le_p
	0x24, //le_w_tmf
	0x00, //le_w_struct_diff 9
	0x72,
	0x01, //le_w_struct_diff_gain
	0x00, //le_w_luminance 9
	0x0D,
	0x00, //le_luminance_slope 10
	0x04,
	0x80, //glare_blk_avg_th 0000 0000
	0x00, //glare_luma_gain 9
	0x28,
	0xa0, //glare_luma_ctl_start 0000 0000
	0x01, //glare_luma_th 0000 0000
	0x00, //glare_luma_step 9
	0x01,
	0x00, //glare_uni_luma_gain 9
	0x01,
	0x00, //glare_blk_max_th 0000 0000
	0x00, //glare_blk_max_w 0000 0000
	0x10, //glare_blk_max_curve blk_max_sh 0 0000
	0x00, //glare_con_ext_max_th 0000 0000
	0x00, //glare_con_ext_max_w 0000 0000
	0x30, //glare_con_ext_max_sign con_ext_max_curve con_ext_max_sh 0 0 0000
	0x00, //glare_max_cnt_th 0000
	0x00, //glare_max_cnt_th 0000 0000
	0x00, //glare_max_cnt_w 0000 0000
	0x30, //glare_max_cnt_sign max_cnt_curve max_cnt_sh 0 0 0000
	0x00, //glare_y_avg_th 0000 0000
	0x00, //glare_y_avg_w 0000 0000
	0x30, //glare_y_avg_sign y_avg_curve y_avg_sh 0 0 0000
	0x00, //glare_max_cnt_th_dmc 0000
	0x00, //glare_max_cnt_th_dmc 0000 0000
	0x04, //nr fa de cs gamma 0 0000
	0xff, //nr_mask_th
	0x00, //de_gain 10
	0x10,
	0x02, //de_maxplus 11
	0x00,
	0x02, //de_maxminus 11
	0x00,
	0x14, //fa_edge_th
	0x00, //fa_step_p 13
	0x01,
	0x00, //fa_step_n 13
	0x01,
	0x00, //fa_max_de_gain 13
	0x10,
	0x00, //fa_pcl_ppi 14
	0x00,
	0x28, //fa_os_cnt_10_co
	0x3c, //fa_os_cnt_20_co
	0x04, //fa_edge_cnt_weight
	0x0f, //fa_avg_y_weight
	0x01, //cs_gain 10
	0x00,
	0x00, //curve_0
	0x10, //curve_1
	0x20, //curve_2
	0x30, //curve_3
	0x40, //curve_4
	0x50, //curve_5
	0x60, //curve_6
	0x70, //curve_7
	0x80, //curve_8
	0x90, //curve_9
	0xa0, //curve_10
	0xb0, //curve_11
	0xc0, //curve_12
	0xd0, //curve_13
	0xe0, //curve_14
	0xf0, //curve_15
	0x01, //curve_16
	0x00,
	0x00, //curve_offset 10
	0x00,
	0x00, //curve_low_x
	0x00, //blank
	0x00, //curve_low_y
};

static char DSI_TDMB_DYNAMIC_MDNIE_3[] = {
	0xDD,
	0x01, //mdnie_en
	0x00, //mask 00 0000
	0x03, //edge 00
	0xf0, //ascr algo aolce gamut 00 00 00 00
	0xff, //partial_en1
	0xff, //partial_en2
	0x00, //roi_en
	0x00, //roi_block
	0x00, //roi_sx
	0x00, //roi_sx
	0x00, //roi_sy
	0x00, //roi_sy
	0x00, //roi_ex
	0x00, //roi_ex
	0x00, //roi_ey
	0x00, //roi_ey
	0xff, //trans_on trans_block 0 000 0000
	0x04, //trans_slope
	0x01, //trans_interval
	//end
};

static char DSI_TDMB_AUTO_MDNIE_1[] = {
	//start
	0xDF,
	0x01, //linear_on ascr_skin_on 0000 0000
	0x6a, //ascr_skin_cb
	0x9a, //ascr_skin_cr
	0x25, //ascr_dist_up
	0x1a, //ascr_dist_down
	0x16, //ascr_dist_right
	0x2a, //ascr_dist_left
	0x00, //ascr_div_up 20
	0x37,
	0x5a,
	0x00, //ascr_div_down
	0x4e,
	0xc5,
	0x00, //ascr_div_right
	0x5d,
	0x17,
	0x00, //ascr_div_left
	0x30,
	0xc3,
	0xff, //ascr_skin_Rr
	0x48, //ascr_skin_Rg
	0x58, //ascr_skin_Rb
	0xff, //ascr_skin_Yr
	0xf8, //ascr_skin_Yg
	0x00, //ascr_skin_Yb
	0xfe, //ascr_skin_Mr
	0x00, //ascr_skin_Mg
	0xff, //ascr_skin_Mb
	0xff, //ascr_skin_Wr
	0xff, //ascr_skin_Wg
	0xff, //ascr_skin_Wb
	0x00, //ascr_Cr
	0xf0, //ascr_Rr
	0xff, //ascr_Cg
	0x00, //ascr_Rg
	0xfa, //ascr_Cb
	0x00, //ascr_Rb
	0xfe, //ascr_Mr
	0x00, //ascr_Gr
	0x00, //ascr_Mg
	0xff, //ascr_Gg
	0xff, //ascr_Mb
	0x00, //ascr_Gb
	0xff, //ascr_Yr
	0x00, //ascr_Br
	0xf8, //ascr_Yg
	0x00, //ascr_Bg
	0x00, //ascr_Yb
	0xff, //ascr_Bb
	0xff, //ascr_Wr
	0x00, //ascr_Kr
	0xff, //ascr_Wg
	0x00, //ascr_Kg
	0xff, //ascr_Wb
	0x00, //ascr_Kb
	0x0F, //edge_ctrl
	0x00, //edge_radius 12
	0x6F,
	0x05, //edge_aa_factor
	0x00, //edge_r
	0x00, //edge_g
	0x00, //edge_b
	0x00, //edge_x 11
	0x70,
	0x00, //edge_y 12
	0x70,
	0x44, //edge_coef_a edge_coef_b 0000 0000
};

static char DSI_TDMB_AUTO_MDNIE_2[] = {
	0xDE,
	0x00, //gamut_gamma_on gamut_blk_shift 0000 0000
	0x40, //gamut_scale
	0x04, //gamut_r1
	0x00,
	0x00, //gamut_r2
	0x00,
	0x00, //gamut_r3
	0x00,
	0x00, //gamut_g1
	0x00,
	0x04, //gamut_g2
	0x00,
	0x00, //gamut_g3
	0x00,
	0x00, //gamut_b1
	0x00,
	0x00, //gamut_b2
	0x00,
	0x04, //gamut_b3
	0x00,
	0x00, //slce_on cadrx_en 0000 0000
	0x00, //glare_on
	0x00, //lce_gain 000 0000
	0x24, //lce_color_gain 00 0000
	0x96, //lce_min_ref_offset
	0xb3, //lce_illum_gain
	0x01, //lce_ref_offset 9
	0x0e,
	0x01, //lce_ref_gain 9
	0x00,
	0x77, //lce_block_size h v 0000 0000
	0x03, //lce_dark_th 000
	0x07, //lce_reduct_slope 0000
	0x40, //lce_black cc0 cc1 color_th 0 0 0 0000
	0x08, //lce_large_w
	0x04, //lce_med_w
	0x04, //lce_small_w
	0x00, //cadrx_dit_en shadow_corr_en ref_ctrl_en 0 0 0
	0x00, //cadrx_gain
	0x0d, //cadrx_low_intensity_th
	0xe1, //cadrx_high_intensity_th
	0x07, //cadrx_ui_zerobincnt_th
	0x29, //cadrx_ui_ratio_th
	0x04,
	0x0f, //cadrx_entire_freq
	0xa0,
	0x00,
	0x03, //cadrx_peak_th_in_freq
	0x7a,
	0xa0,
	0x00, //cadrx_high_peak_th_in_freq
	0x25,
	0x1c,
	0x24, //cadrx_dit_color_gain
	0x00, //cadrx_dit_illum_a0
	0x48, //cadrx_dit_illum_a1
	0xea, //cadrx_dit_illum_a2
	0xfa, //cadrx_dit_illum_a3
	0x00, //cadrx_dit_illum_b0
	0x10,
	0x00, //cadrx_dit_illum_b1
	0x10,
	0x00, //cadrx_dit_illum_b2
	0x9c,
	0x01, //cadrx_dit_illum_b3
	0x00,
	0x00, //cadrx_dit_illum_slope1
	0x00,
	0x00, //cadrx_dit_illum_slope2
	0xdc,
	0x03, //cadrx_dit_illum_slope3
	0xff,
	0x40, //cadrx_ui_illum_a1
	0x80, //cadrx_ui_illum_a2
	0xc0, //cadrx_ui_illum_a3
	0x00, //cadrx_ui_illum_offset1
	0x9a,
	0x00, //cadrx_ui_illum_offset2
	0x9a,
	0x00, //cadrx_ui_illum_offset3
	0x9a,
	0x00, //cadrx_ui_illum_offset4
	0x9a,
	0x00, //cadrx_ui_illum_slope1
	0x00,
	0x00, //cadrx_ui_illum_slope2
	0x00,
	0x00, //cadrx_ui_illum_slope3
	0x00,
	0x00, //cadrx_ui_illum_slope4
	0x00,
	0x40, //cadrx_ui_ref_a1
	0x80, //cadrx_ui_ref_a2
	0xc0, //cadrx_ui_ref_a3
	0x01, //cadrx_ui_ref_offset1
	0x0e,
	0x01, //cadrx_ui_ref_offset2
	0x0e,
	0x01, //cadrx_ui_ref_offset3
	0x0e,
	0x01, //cadrx_ui_ref_offset4
	0x0e,
	0x00, //cadrx_ui_ref_slope1
	0x00,
	0x00, //cadrx_ui_ref_slope2
	0x00,
	0x00, //cadrx_ui_ref_slope3
	0x00,
	0x00, //cadrx_ui_ref_slope4
	0x00,
	0x01, //cadrx_reg_ref_c1_offset
	0x0e,
	0x01, //cadrx_reg_ref_c2_offset
	0x3a,
	0x01, //cadrx_reg_ref_c3_offset
	0x44,
	0x00, //cadrx_reg_ref_c1_slope
	0x01,
	0x00, //cadrx_reg_ref_c2_slope
	0x43,
	0x00, //cadrx_reg_ref_c3_slope
	0x4b,
	0x00, //cadrx_sc_reg_a1
	0x18,
	0x00, //cadrx_sc_reg_b1
	0x1c,
	0x01, //cadrx_sc_k1_int
	0xff,
	0x01, //cadrx_sc_k2_int
	0xff,
	0x03, //cadrx_sc_w1_int
	0xff,
	0xe8,
	0x03, //cadrx_sc_w2_int
	0xff,
	0xe8,
	0x00, //flicker_w_en
	0x14, //flicker_w
	0x01, //flicker_trans_en
	0x04, //flicker_trans_slope
	0x00, //bl_en
	0x03, //bl_hbs_th
	0x10, //bl_lbs_th
	0xC8, //bl_illum_gain
	0x01, //bl_reg_gain 9
	0x2C,
	0x00, //bl_le_diff
	0x00, //bl_cadrx_gain
	0x04, //bl_trans_slope
	0x00, //le_en
	0x40, //le_diff
	0x03, //le_lmax 10
	0xB6,
	0x23, //le_p
	0x24, //le_w_tmf
	0x00, //le_w_struct_diff 9
	0x72,
	0x01, //le_w_struct_diff_gain
	0x00, //le_w_luminance 9
	0x0D,
	0x00, //le_luminance_slope 10
	0x04,
	0x80, //glare_blk_avg_th 0000 0000
	0x00, //glare_luma_gain 9
	0x28,
	0xa0, //glare_luma_ctl_start 0000 0000
	0x01, //glare_luma_th 0000 0000
	0x00, //glare_luma_step 9
	0x01,
	0x00, //glare_uni_luma_gain 9
	0x01,
	0xf0, //glare_blk_max_th 0000 0000
	0x00, //glare_blk_max_w 0000 0000
	0x18, //glare_blk_max_curve blk_max_sh 0 0000
	0xf0, //glare_con_ext_max_th 0000 0000
	0x00, //glare_con_ext_max_w 0000 0000
	0x38, //glare_con_ext_max_sign con_ext_max_curve con_ext_max_sh 0 0 0000
	0x00, //glare_max_cnt_th 0000
	0x00, //glare_max_cnt_th 0000 0000
	0x00, //glare_max_cnt_w 0000 0000
	0x38, //glare_max_cnt_sign max_cnt_curve max_cnt_sh 0 0 0000
	0xba, //glare_y_avg_th 0000 0000
	0x00, //glare_y_avg_w 0000 0000
	0x38, //glare_y_avg_sign y_avg_curve y_avg_sh 0 0 0000
	0x00, //glare_max_cnt_th_dmc 0000
	0x05, //glare_max_cnt_th_dmc 0000 0000
	0x0c, //nr fa de cs gamma 0 0000
	0xff, //nr_mask_th
	0x00, //de_gain 10
	0x10,
	0x02, //de_maxplus 11
	0x00,
	0x02, //de_maxminus 11
	0x00,
	0x14, //fa_edge_th
	0x00, //fa_step_p 13
	0x01,
	0x00, //fa_step_n 13
	0x01,
	0x00, //fa_max_de_gain 13
	0x10,
	0x00, //fa_pcl_ppi 14
	0x00,
	0x28, //fa_os_cnt_10_co
	0x3c, //fa_os_cnt_20_co
	0x04, //fa_edge_cnt_weight
	0x0f, //fa_avg_y_weight
	0x01, //cs_gain 10
	0x00,
	0x00, //curve_0
	0x10, //curve_1
	0x20, //curve_2
	0x30, //curve_3
	0x40, //curve_4
	0x50, //curve_5
	0x60, //curve_6
	0x70, //curve_7
	0x80, //curve_8
	0x90, //curve_9
	0xa0, //curve_10
	0xb0, //curve_11
	0xc0, //curve_12
	0xd0, //curve_13
	0xe0, //curve_14
	0xf0, //curve_15
	0x01, //curve_16
	0x00,
	0x00, //curve_offset 10
	0x00,
	0x00, //curve_low_x
	0x00, //blank
	0x00, //curve_low_y
};

static char DSI_TDMB_AUTO_MDNIE_3[] = {
	0xDD,
	0x01, //mdnie_en
	0x00, //mask 00 0000
	0x03, //edge 00
	0xf0, //ascr algo aolce gamut 00 00 00 00
	0xff, //partial_en1
	0xff, //partial_en2
	0x00, //roi_en
	0x00, //roi_block
	0x00, //roi_sx
	0x00, //roi_sx
	0x00, //roi_sy
	0x00, //roi_sy
	0x00, //roi_ex
	0x00, //roi_ex
	0x00, //roi_ey
	0x00, //roi_ey
	0xff, //trans_on trans_block 0 000 0000
	0x04, //trans_slope
	0x01, //trans_interval
	//end
};

static char DSI_HMT_COLOR_TEMP_3000K_MDNIE_1[] = {
	//start
	0xDF,
	0x01, //linear_on ascr_skin_on 0000 0000
	0x6a, //ascr_skin_cb
	0x9a, //ascr_skin_cr
	0x25, //ascr_dist_up
	0x1a, //ascr_dist_down
	0x16, //ascr_dist_right
	0x2a, //ascr_dist_left
	0x00, //ascr_div_up 20
	0x37,
	0x5a,
	0x00, //ascr_div_down
	0x4e,
	0xc5,
	0x00, //ascr_div_right
	0x5d,
	0x17,
	0x00, //ascr_div_left
	0x30,
	0xc3,
	0xff, //ascr_skin_Rr
	0x00, //ascr_skin_Rg
	0x00, //ascr_skin_Rb
	0xff, //ascr_skin_Yr
	0xff, //ascr_skin_Yg
	0x00, //ascr_skin_Yb
	0xff, //ascr_skin_Mr
	0x00, //ascr_skin_Mg
	0xff, //ascr_skin_Mb
	0xff, //ascr_skin_Wr
	0xcf, //ascr_skin_Wg
	0x7f, //ascr_skin_Wb
	0x00, //ascr_Cr
	0xff, //ascr_Rr
	0xff, //ascr_Cg
	0x00, //ascr_Rg
	0xff, //ascr_Cb
	0x00, //ascr_Rb
	0xff, //ascr_Mr
	0x00, //ascr_Gr
	0x00, //ascr_Mg
	0xff, //ascr_Gg
	0xff, //ascr_Mb
	0x00, //ascr_Gb
	0xff, //ascr_Yr
	0x00, //ascr_Br
	0xff, //ascr_Yg
	0x00, //ascr_Bg
	0x00, //ascr_Yb
	0xff, //ascr_Bb
	0xff, //ascr_Wr
	0x00, //ascr_Kr
	0xcf, //ascr_Wg
	0x00, //ascr_Kg
	0x7f, //ascr_Wb
	0x00, //ascr_Kb
	0x0F, //edge_ctrl
	0x00, //edge_radius 12
	0x6F,
	0x05, //edge_aa_factor
	0x00, //edge_r
	0x00, //edge_g
	0x00, //edge_b
	0x00, //edge_x 11
	0x70,
	0x00, //edge_y 12
	0x70,
	0x44, //edge_coef_a edge_coef_b 0000 0000
};

static char DSI_HMT_COLOR_TEMP_3000K_MDNIE_2[] = {
	0xDE,
	0x00, //gamut_gamma_on gamut_blk_shift 0000 0000
	0x40, //gamut_scale
	0x04, //gamut_r1
	0x00,
	0x00, //gamut_r2
	0x00,
	0x00, //gamut_r3
	0x00,
	0x00, //gamut_g1
	0x00,
	0x04, //gamut_g2
	0x00,
	0x00, //gamut_g3
	0x00,
	0x00, //gamut_b1
	0x00,
	0x00, //gamut_b2
	0x00,
	0x04, //gamut_b3
	0x00,
	0x00, //slce_on cadrx_en 0000 0000
	0x11, //glare_on
	0x00, //lce_gain 000 0000
	0x24, //lce_color_gain 00 0000
	0x96, //lce_min_ref_offset
	0xb3, //lce_illum_gain
	0x01, //lce_ref_offset 9
	0x0e,
	0x01, //lce_ref_gain 9
	0x00,
	0x77, //lce_block_size h v 0000 0000
	0x03, //lce_dark_th 000
	0x07, //lce_reduct_slope 0000
	0x40, //lce_black cc0 cc1 color_th 0 0 0 0000
	0x08, //lce_large_w
	0x04, //lce_med_w
	0x04, //lce_small_w
	0x00, //cadrx_dit_en shadow_corr_en ref_ctrl_en 0 0 0
	0x00, //cadrx_gain
	0x0d, //cadrx_low_intensity_th
	0xe1, //cadrx_high_intensity_th
	0x07, //cadrx_ui_zerobincnt_th
	0x29, //cadrx_ui_ratio_th
	0x04,
	0x0f, //cadrx_entire_freq
	0xa0,
	0x00,
	0x03, //cadrx_peak_th_in_freq
	0x7a,
	0xa0,
	0x00, //cadrx_high_peak_th_in_freq
	0x25,
	0x1c,
	0x24, //cadrx_dit_color_gain
	0x00, //cadrx_dit_illum_a0
	0x48, //cadrx_dit_illum_a1
	0xea, //cadrx_dit_illum_a2
	0xfa, //cadrx_dit_illum_a3
	0x00, //cadrx_dit_illum_b0
	0x10,
	0x00, //cadrx_dit_illum_b1
	0x10,
	0x00, //cadrx_dit_illum_b2
	0x9c,
	0x01, //cadrx_dit_illum_b3
	0x00,
	0x00, //cadrx_dit_illum_slope1
	0x00,
	0x00, //cadrx_dit_illum_slope2
	0xdc,
	0x03, //cadrx_dit_illum_slope3
	0xff,
	0x40, //cadrx_ui_illum_a1
	0x80, //cadrx_ui_illum_a2
	0xc0, //cadrx_ui_illum_a3
	0x00, //cadrx_ui_illum_offset1
	0x9a,
	0x00, //cadrx_ui_illum_offset2
	0x9a,
	0x00, //cadrx_ui_illum_offset3
	0x9a,
	0x00, //cadrx_ui_illum_offset4
	0x9a,
	0x00, //cadrx_ui_illum_slope1
	0x00,
	0x00, //cadrx_ui_illum_slope2
	0x00,
	0x00, //cadrx_ui_illum_slope3
	0x00,
	0x00, //cadrx_ui_illum_slope4
	0x00,
	0x40, //cadrx_ui_ref_a1
	0x80, //cadrx_ui_ref_a2
	0xc0, //cadrx_ui_ref_a3
	0x01, //cadrx_ui_ref_offset1
	0x0e,
	0x01, //cadrx_ui_ref_offset2
	0x0e,
	0x01, //cadrx_ui_ref_offset3
	0x0e,
	0x01, //cadrx_ui_ref_offset4
	0x0e,
	0x00, //cadrx_ui_ref_slope1
	0x00,
	0x00, //cadrx_ui_ref_slope2
	0x00,
	0x00, //cadrx_ui_ref_slope3
	0x00,
	0x00, //cadrx_ui_ref_slope4
	0x00,
	0x01, //cadrx_reg_ref_c1_offset
	0x0e,
	0x01, //cadrx_reg_ref_c2_offset
	0x3a,
	0x01, //cadrx_reg_ref_c3_offset
	0x44,
	0x00, //cadrx_reg_ref_c1_slope
	0x01,
	0x00, //cadrx_reg_ref_c2_slope
	0x43,
	0x00, //cadrx_reg_ref_c3_slope
	0x4b,
	0x00, //cadrx_sc_reg_a1
	0x18,
	0x00, //cadrx_sc_reg_b1
	0x1c,
	0x01, //cadrx_sc_k1_int
	0xff,
	0x01, //cadrx_sc_k2_int
	0xff,
	0x03, //cadrx_sc_w1_int
	0xff,
	0xe8,
	0x03, //cadrx_sc_w2_int
	0xff,
	0xe8,
	0x00, //flicker_w_en
	0x14, //flicker_w
	0x01, //flicker_trans_en
	0x04, //flicker_trans_slope
	0x00, //bl_en
	0x03, //bl_hbs_th
	0x10, //bl_lbs_th
	0xC8, //bl_illum_gain
	0x01, //bl_reg_gain 9
	0x2C,
	0x00, //bl_le_diff
	0x00, //bl_cadrx_gain
	0x04, //bl_trans_slope
	0x00, //le_en
	0x40, //le_diff
	0x03, //le_lmax 10
	0xB6,
	0x23, //le_p
	0x24, //le_w_tmf
	0x00, //le_w_struct_diff 9
	0x72,
	0x01, //le_w_struct_diff_gain
	0x00, //le_w_luminance 9
	0x0D,
	0x00, //le_luminance_slope 10
	0x04,
	0x80, //glare_blk_avg_th 0000 0000
	0x00, //glare_luma_gain 9
	0x00,
	0x00, //glare_luma_ctl_start 0000 0000
	0x05, //glare_luma_th 0000 0000
	0x00, //glare_luma_step 9
	0x01,
	0x01, //glare_uni_luma_gain 9
	0x00,
	0xf0, //glare_blk_max_th 0000 0000
	0x12, //glare_blk_max_w 0000 0000
	0x18, //glare_blk_max_curve blk_max_sh 0 0000
	0xf0, //glare_con_ext_max_th 0000 0000
	0x12, //glare_con_ext_max_w 0000 0000
	0x38, //glare_con_ext_max_sign con_ext_max_curve con_ext_max_sh 0 0 0000
	0x00, //glare_max_cnt_th 0000
	0x00, //glare_max_cnt_th 0000 0000
	0x02, //glare_max_cnt_w 0000 0000
	0x38, //glare_max_cnt_sign max_cnt_curve max_cnt_sh 0 0 0000
	0x00, //glare_y_avg_th 0000 0000
	0x18, //glare_y_avg_w 0000 0000
	0x38, //glare_y_avg_sign y_avg_curve y_avg_sh 0 0 0000
	0x00, //glare_max_cnt_th_dmc 0000
	0x05, //glare_max_cnt_th_dmc 0000 0000
	0x02, //nr fa de cs gamma 0 0000
	0xff, //nr_mask_th
	0x00, //de_gain 10
	0x00,
	0x02, //de_maxplus 11
	0x00,
	0x02, //de_maxminus 11
	0x00,
	0x14, //fa_edge_th
	0x00, //fa_step_p 13
	0x01,
	0x00, //fa_step_n 13
	0x01,
	0x00, //fa_max_de_gain 13
	0x10,
	0x00, //fa_pcl_ppi 14
	0x00,
	0x28, //fa_os_cnt_10_co
	0x3c, //fa_os_cnt_20_co
	0x04, //fa_edge_cnt_weight
	0x0f, //fa_avg_y_weight
	0x01, //cs_gain 10
	0x40,
	0x00, //curve_0
	0x10, //curve_1
	0x20, //curve_2
	0x30, //curve_3
	0x40, //curve_4
	0x50, //curve_5
	0x60, //curve_6
	0x70, //curve_7
	0x80, //curve_8
	0x90, //curve_9
	0xa0, //curve_10
	0xb0, //curve_11
	0xc0, //curve_12
	0xd0, //curve_13
	0xe0, //curve_14
	0xf0, //curve_15
	0x01, //curve_16
	0x00,
	0x00, //curve_offset 10
	0x00,
	0x00, //curve_low_x
	0x00, //blank
	0x00, //curve_low_y
};

static char DSI_HMT_COLOR_TEMP_3000K_MDNIE_3[] = {
	0xDD,
	0x01, //mdnie_en
	0x00, //mask 00 0000
	0x03, //edge 00
	0xf0, //ascr algo aolce gamut 00 00 00 00
	0xff, //partial_en1
	0xff, //partial_en2
	0x00, //roi_en
	0x00, //roi_block
	0x00, //roi_sx
	0x00, //roi_sx
	0x00, //roi_sy
	0x00, //roi_sy
	0x00, //roi_ex
	0x00, //roi_ex
	0x00, //roi_ey
	0x00, //roi_ey
	0xff, //trans_on trans_block 0 000 0000
	0x04, //trans_slope
	0x01, //trans_interval
	//end
};

static char DSI_HMT_COLOR_TEMP_4000K_MDNIE_1[] = {
	//start
	0xDF,
	0x01, //linear_on ascr_skin_on 0000 0000
	0x6a, //ascr_skin_cb
	0x9a, //ascr_skin_cr
	0x25, //ascr_dist_up
	0x1a, //ascr_dist_down
	0x16, //ascr_dist_right
	0x2a, //ascr_dist_left
	0x00, //ascr_div_up 20
	0x37,
	0x5a,
	0x00, //ascr_div_down
	0x4e,
	0xc5,
	0x00, //ascr_div_right
	0x5d,
	0x17,
	0x00, //ascr_div_left
	0x30,
	0xc3,
	0xff, //ascr_skin_Rr
	0x00, //ascr_skin_Rg
	0x00, //ascr_skin_Rb
	0xff, //ascr_skin_Yr
	0xff, //ascr_skin_Yg
	0x00, //ascr_skin_Yb
	0xff, //ascr_skin_Mr
	0x00, //ascr_skin_Mg
	0xff, //ascr_skin_Mb
	0xff, //ascr_skin_Wr
	0xe2, //ascr_skin_Wg
	0xb0, //ascr_skin_Wb
	0x00, //ascr_Cr
	0xff, //ascr_Rr
	0xff, //ascr_Cg
	0x00, //ascr_Rg
	0xff, //ascr_Cb
	0x00, //ascr_Rb
	0xff, //ascr_Mr
	0x00, //ascr_Gr
	0x00, //ascr_Mg
	0xff, //ascr_Gg
	0xff, //ascr_Mb
	0x00, //ascr_Gb
	0xff, //ascr_Yr
	0x00, //ascr_Br
	0xff, //ascr_Yg
	0x00, //ascr_Bg
	0x00, //ascr_Yb
	0xff, //ascr_Bb
	0xff, //ascr_Wr
	0x00, //ascr_Kr
	0xe2, //ascr_Wg
	0x00, //ascr_Kg
	0xb0, //ascr_Wb
	0x00, //ascr_Kb
	0x0F, //edge_ctrl
	0x00, //edge_radius 12
	0x6F,
	0x05, //edge_aa_factor
	0x00, //edge_r
	0x00, //edge_g
	0x00, //edge_b
	0x00, //edge_x 11
	0x70,
	0x00, //edge_y 12
	0x70,
	0x44, //edge_coef_a edge_coef_b 0000 0000
};

static char DSI_HMT_COLOR_TEMP_4000K_MDNIE_2[] = {
	0xDE,
	0x00, //gamut_gamma_on gamut_blk_shift 0000 0000
	0x40, //gamut_scale
	0x04, //gamut_r1
	0x00,
	0x00, //gamut_r2
	0x00,
	0x00, //gamut_r3
	0x00,
	0x00, //gamut_g1
	0x00,
	0x04, //gamut_g2
	0x00,
	0x00, //gamut_g3
	0x00,
	0x00, //gamut_b1
	0x00,
	0x00, //gamut_b2
	0x00,
	0x04, //gamut_b3
	0x00,
	0x00, //slce_on cadrx_en 0000 0000
	0x11, //glare_on
	0x00, //lce_gain 000 0000
	0x24, //lce_color_gain 00 0000
	0x96, //lce_min_ref_offset
	0xb3, //lce_illum_gain
	0x01, //lce_ref_offset 9
	0x0e,
	0x01, //lce_ref_gain 9
	0x00,
	0x77, //lce_block_size h v 0000 0000
	0x03, //lce_dark_th 000
	0x07, //lce_reduct_slope 0000
	0x40, //lce_black cc0 cc1 color_th 0 0 0 0000
	0x08, //lce_large_w
	0x04, //lce_med_w
	0x04, //lce_small_w
	0x00, //cadrx_dit_en shadow_corr_en ref_ctrl_en 0 0 0
	0x00, //cadrx_gain
	0x0d, //cadrx_low_intensity_th
	0xe1, //cadrx_high_intensity_th
	0x07, //cadrx_ui_zerobincnt_th
	0x29, //cadrx_ui_ratio_th
	0x04,
	0x0f, //cadrx_entire_freq
	0xa0,
	0x00,
	0x03, //cadrx_peak_th_in_freq
	0x7a,
	0xa0,
	0x00, //cadrx_high_peak_th_in_freq
	0x25,
	0x1c,
	0x24, //cadrx_dit_color_gain
	0x00, //cadrx_dit_illum_a0
	0x48, //cadrx_dit_illum_a1
	0xea, //cadrx_dit_illum_a2
	0xfa, //cadrx_dit_illum_a3
	0x00, //cadrx_dit_illum_b0
	0x10,
	0x00, //cadrx_dit_illum_b1
	0x10,
	0x00, //cadrx_dit_illum_b2
	0x9c,
	0x01, //cadrx_dit_illum_b3
	0x00,
	0x00, //cadrx_dit_illum_slope1
	0x00,
	0x00, //cadrx_dit_illum_slope2
	0xdc,
	0x03, //cadrx_dit_illum_slope3
	0xff,
	0x40, //cadrx_ui_illum_a1
	0x80, //cadrx_ui_illum_a2
	0xc0, //cadrx_ui_illum_a3
	0x00, //cadrx_ui_illum_offset1
	0x9a,
	0x00, //cadrx_ui_illum_offset2
	0x9a,
	0x00, //cadrx_ui_illum_offset3
	0x9a,
	0x00, //cadrx_ui_illum_offset4
	0x9a,
	0x00, //cadrx_ui_illum_slope1
	0x00,
	0x00, //cadrx_ui_illum_slope2
	0x00,
	0x00, //cadrx_ui_illum_slope3
	0x00,
	0x00, //cadrx_ui_illum_slope4
	0x00,
	0x40, //cadrx_ui_ref_a1
	0x80, //cadrx_ui_ref_a2
	0xc0, //cadrx_ui_ref_a3
	0x01, //cadrx_ui_ref_offset1
	0x0e,
	0x01, //cadrx_ui_ref_offset2
	0x0e,
	0x01, //cadrx_ui_ref_offset3
	0x0e,
	0x01, //cadrx_ui_ref_offset4
	0x0e,
	0x00, //cadrx_ui_ref_slope1
	0x00,
	0x00, //cadrx_ui_ref_slope2
	0x00,
	0x00, //cadrx_ui_ref_slope3
	0x00,
	0x00, //cadrx_ui_ref_slope4
	0x00,
	0x01, //cadrx_reg_ref_c1_offset
	0x0e,
	0x01, //cadrx_reg_ref_c2_offset
	0x3a,
	0x01, //cadrx_reg_ref_c3_offset
	0x44,
	0x00, //cadrx_reg_ref_c1_slope
	0x01,
	0x00, //cadrx_reg_ref_c2_slope
	0x43,
	0x00, //cadrx_reg_ref_c3_slope
	0x4b,
	0x00, //cadrx_sc_reg_a1
	0x18,
	0x00, //cadrx_sc_reg_b1
	0x1c,
	0x01, //cadrx_sc_k1_int
	0xff,
	0x01, //cadrx_sc_k2_int
	0xff,
	0x03, //cadrx_sc_w1_int
	0xff,
	0xe8,
	0x03, //cadrx_sc_w2_int
	0xff,
	0xe8,
	0x00, //flicker_w_en
	0x14, //flicker_w
	0x01, //flicker_trans_en
	0x04, //flicker_trans_slope
	0x00, //bl_en
	0x03, //bl_hbs_th
	0x10, //bl_lbs_th
	0xC8, //bl_illum_gain
	0x01, //bl_reg_gain 9
	0x2C,
	0x00, //bl_le_diff
	0x00, //bl_cadrx_gain
	0x04, //bl_trans_slope
	0x00, //le_en
	0x40, //le_diff
	0x03, //le_lmax 10
	0xB6,
	0x23, //le_p
	0x24, //le_w_tmf
	0x00, //le_w_struct_diff 9
	0x72,
	0x01, //le_w_struct_diff_gain
	0x00, //le_w_luminance 9
	0x0D,
	0x00, //le_luminance_slope 10
	0x04,
	0x80, //glare_blk_avg_th 0000 0000
	0x00, //glare_luma_gain 9
	0x00,
	0x00, //glare_luma_ctl_start 0000 0000
	0x05, //glare_luma_th 0000 0000
	0x00, //glare_luma_step 9
	0x01,
	0x01, //glare_uni_luma_gain 9
	0x00,
	0xf0, //glare_blk_max_th 0000 0000
	0x12, //glare_blk_max_w 0000 0000
	0x18, //glare_blk_max_curve blk_max_sh 0 0000
	0xf0, //glare_con_ext_max_th 0000 0000
	0x12, //glare_con_ext_max_w 0000 0000
	0x38, //glare_con_ext_max_sign con_ext_max_curve con_ext_max_sh 0 0 0000
	0x00, //glare_max_cnt_th 0000
	0x00, //glare_max_cnt_th 0000 0000
	0x02, //glare_max_cnt_w 0000 0000
	0x38, //glare_max_cnt_sign max_cnt_curve max_cnt_sh 0 0 0000
	0x00, //glare_y_avg_th 0000 0000
	0x18, //glare_y_avg_w 0000 0000
	0x38, //glare_y_avg_sign y_avg_curve y_avg_sh 0 0 0000
	0x00, //glare_max_cnt_th_dmc 0000
	0x05, //glare_max_cnt_th_dmc 0000 0000
	0x02, //nr fa de cs gamma 0 0000
	0xff, //nr_mask_th
	0x00, //de_gain 10
	0x00,
	0x02, //de_maxplus 11
	0x00,
	0x02, //de_maxminus 11
	0x00,
	0x14, //fa_edge_th
	0x00, //fa_step_p 13
	0x01,
	0x00, //fa_step_n 13
	0x01,
	0x00, //fa_max_de_gain 13
	0x10,
	0x00, //fa_pcl_ppi 14
	0x00,
	0x28, //fa_os_cnt_10_co
	0x3c, //fa_os_cnt_20_co
	0x04, //fa_edge_cnt_weight
	0x0f, //fa_avg_y_weight
	0x01, //cs_gain 10
	0x40,
	0x00, //curve_0
	0x10, //curve_1
	0x20, //curve_2
	0x30, //curve_3
	0x40, //curve_4
	0x50, //curve_5
	0x60, //curve_6
	0x70, //curve_7
	0x80, //curve_8
	0x90, //curve_9
	0xa0, //curve_10
	0xb0, //curve_11
	0xc0, //curve_12
	0xd0, //curve_13
	0xe0, //curve_14
	0xf0, //curve_15
	0x01, //curve_16
	0x00,
	0x00, //curve_offset 10
	0x00,
	0x00, //curve_low_x
	0x00, //blank
	0x00, //curve_low_y
};

static char DSI_HMT_COLOR_TEMP_4000K_MDNIE_3[] = {
	0xDD,
	0x01, //mdnie_en
	0x00, //mask 00 0000
	0x03, //edge 00
	0xf0, //ascr algo aolce gamut 00 00 00 00
	0xff, //partial_en1
	0xff, //partial_en2
	0x00, //roi_en
	0x00, //roi_block
	0x00, //roi_sx
	0x00, //roi_sx
	0x00, //roi_sy
	0x00, //roi_sy
	0x00, //roi_ex
	0x00, //roi_ex
	0x00, //roi_ey
	0x00, //roi_ey
	0xff, //trans_on trans_block 0 000 0000
	0x04, //trans_slope
	0x01, //trans_interval
	//end
};

static char DSI_HMT_COLOR_TEMP_5000K_MDNIE_1[] = {
	//start
	0xDF,
	0x01, //linear_on ascr_skin_on 0000 0000
	0x6a, //ascr_skin_cb
	0x9a, //ascr_skin_cr
	0x25, //ascr_dist_up
	0x1a, //ascr_dist_down
	0x16, //ascr_dist_right
	0x2a, //ascr_dist_left
	0x00, //ascr_div_up 20
	0x37,
	0x5a,
	0x00, //ascr_div_down
	0x4e,
	0xc5,
	0x00, //ascr_div_right
	0x5d,
	0x17,
	0x00, //ascr_div_left
	0x30,
	0xc3,
	0xff, //ascr_skin_Rr
	0x00, //ascr_skin_Rg
	0x00, //ascr_skin_Rb
	0xff, //ascr_skin_Yr
	0xff, //ascr_skin_Yg
	0x00, //ascr_skin_Yb
	0xff, //ascr_skin_Mr
	0x00, //ascr_skin_Mg
	0xff, //ascr_skin_Mb
	0xff, //ascr_skin_Wr
	0xee, //ascr_skin_Wg
	0xd0, //ascr_skin_Wb
	0x00, //ascr_Cr
	0xff, //ascr_Rr
	0xff, //ascr_Cg
	0x00, //ascr_Rg
	0xff, //ascr_Cb
	0x00, //ascr_Rb
	0xff, //ascr_Mr
	0x00, //ascr_Gr
	0x00, //ascr_Mg
	0xff, //ascr_Gg
	0xff, //ascr_Mb
	0x00, //ascr_Gb
	0xff, //ascr_Yr
	0x00, //ascr_Br
	0xff, //ascr_Yg
	0x00, //ascr_Bg
	0x00, //ascr_Yb
	0xff, //ascr_Bb
	0xff, //ascr_Wr
	0x00, //ascr_Kr
	0xee, //ascr_Wg
	0x00, //ascr_Kg
	0xd0, //ascr_Wb
	0x00, //ascr_Kb
	0x0F, //edge_ctrl
	0x00, //edge_radius 12
	0x6F,
	0x05, //edge_aa_factor
	0x00, //edge_r
	0x00, //edge_g
	0x00, //edge_b
	0x00, //edge_x 11
	0x70,
	0x00, //edge_y 12
	0x70,
	0x44, //edge_coef_a edge_coef_b 0000 0000
};

static char DSI_HMT_COLOR_TEMP_5000K_MDNIE_2[] = {
	0xDE,
	0x00, //gamut_gamma_on gamut_blk_shift 0000 0000
	0x40, //gamut_scale
	0x04, //gamut_r1
	0x00,
	0x00, //gamut_r2
	0x00,
	0x00, //gamut_r3
	0x00,
	0x00, //gamut_g1
	0x00,
	0x04, //gamut_g2
	0x00,
	0x00, //gamut_g3
	0x00,
	0x00, //gamut_b1
	0x00,
	0x00, //gamut_b2
	0x00,
	0x04, //gamut_b3
	0x00,
	0x00, //slce_on cadrx_en 0000 0000
	0x11, //glare_on
	0x00, //lce_gain 000 0000
	0x24, //lce_color_gain 00 0000
	0x96, //lce_min_ref_offset
	0xb3, //lce_illum_gain
	0x01, //lce_ref_offset 9
	0x0e,
	0x01, //lce_ref_gain 9
	0x00,
	0x77, //lce_block_size h v 0000 0000
	0x03, //lce_dark_th 000
	0x07, //lce_reduct_slope 0000
	0x40, //lce_black cc0 cc1 color_th 0 0 0 0000
	0x08, //lce_large_w
	0x04, //lce_med_w
	0x04, //lce_small_w
	0x00, //cadrx_dit_en shadow_corr_en ref_ctrl_en 0 0 0
	0x00, //cadrx_gain
	0x0d, //cadrx_low_intensity_th
	0xe1, //cadrx_high_intensity_th
	0x07, //cadrx_ui_zerobincnt_th
	0x29, //cadrx_ui_ratio_th
	0x04,
	0x0f, //cadrx_entire_freq
	0xa0,
	0x00,
	0x03, //cadrx_peak_th_in_freq
	0x7a,
	0xa0,
	0x00, //cadrx_high_peak_th_in_freq
	0x25,
	0x1c,
	0x24, //cadrx_dit_color_gain
	0x00, //cadrx_dit_illum_a0
	0x48, //cadrx_dit_illum_a1
	0xea, //cadrx_dit_illum_a2
	0xfa, //cadrx_dit_illum_a3
	0x00, //cadrx_dit_illum_b0
	0x10,
	0x00, //cadrx_dit_illum_b1
	0x10,
	0x00, //cadrx_dit_illum_b2
	0x9c,
	0x01, //cadrx_dit_illum_b3
	0x00,
	0x00, //cadrx_dit_illum_slope1
	0x00,
	0x00, //cadrx_dit_illum_slope2
	0xdc,
	0x03, //cadrx_dit_illum_slope3
	0xff,
	0x40, //cadrx_ui_illum_a1
	0x80, //cadrx_ui_illum_a2
	0xc0, //cadrx_ui_illum_a3
	0x00, //cadrx_ui_illum_offset1
	0x9a,
	0x00, //cadrx_ui_illum_offset2
	0x9a,
	0x00, //cadrx_ui_illum_offset3
	0x9a,
	0x00, //cadrx_ui_illum_offset4
	0x9a,
	0x00, //cadrx_ui_illum_slope1
	0x00,
	0x00, //cadrx_ui_illum_slope2
	0x00,
	0x00, //cadrx_ui_illum_slope3
	0x00,
	0x00, //cadrx_ui_illum_slope4
	0x00,
	0x40, //cadrx_ui_ref_a1
	0x80, //cadrx_ui_ref_a2
	0xc0, //cadrx_ui_ref_a3
	0x01, //cadrx_ui_ref_offset1
	0x0e,
	0x01, //cadrx_ui_ref_offset2
	0x0e,
	0x01, //cadrx_ui_ref_offset3
	0x0e,
	0x01, //cadrx_ui_ref_offset4
	0x0e,
	0x00, //cadrx_ui_ref_slope1
	0x00,
	0x00, //cadrx_ui_ref_slope2
	0x00,
	0x00, //cadrx_ui_ref_slope3
	0x00,
	0x00, //cadrx_ui_ref_slope4
	0x00,
	0x01, //cadrx_reg_ref_c1_offset
	0x0e,
	0x01, //cadrx_reg_ref_c2_offset
	0x3a,
	0x01, //cadrx_reg_ref_c3_offset
	0x44,
	0x00, //cadrx_reg_ref_c1_slope
	0x01,
	0x00, //cadrx_reg_ref_c2_slope
	0x43,
	0x00, //cadrx_reg_ref_c3_slope
	0x4b,
	0x00, //cadrx_sc_reg_a1
	0x18,
	0x00, //cadrx_sc_reg_b1
	0x1c,
	0x01, //cadrx_sc_k1_int
	0xff,
	0x01, //cadrx_sc_k2_int
	0xff,
	0x03, //cadrx_sc_w1_int
	0xff,
	0xe8,
	0x03, //cadrx_sc_w2_int
	0xff,
	0xe8,
	0x00, //flicker_w_en
	0x14, //flicker_w
	0x01, //flicker_trans_en
	0x04, //flicker_trans_slope
	0x00, //bl_en
	0x03, //bl_hbs_th
	0x10, //bl_lbs_th
	0xC8, //bl_illum_gain
	0x01, //bl_reg_gain 9
	0x2C,
	0x00, //bl_le_diff
	0x00, //bl_cadrx_gain
	0x04, //bl_trans_slope
	0x00, //le_en
	0x40, //le_diff
	0x03, //le_lmax 10
	0xB6,
	0x23, //le_p
	0x24, //le_w_tmf
	0x00, //le_w_struct_diff 9
	0x72,
	0x01, //le_w_struct_diff_gain
	0x00, //le_w_luminance 9
	0x0D,
	0x00, //le_luminance_slope 10
	0x04,
	0x80, //glare_blk_avg_th 0000 0000
	0x00, //glare_luma_gain 9
	0x00,
	0x00, //glare_luma_ctl_start 0000 0000
	0x05, //glare_luma_th 0000 0000
	0x00, //glare_luma_step 9
	0x01,
	0x01, //glare_uni_luma_gain 9
	0x00,
	0xf0, //glare_blk_max_th 0000 0000
	0x12, //glare_blk_max_w 0000 0000
	0x18, //glare_blk_max_curve blk_max_sh 0 0000
	0xf0, //glare_con_ext_max_th 0000 0000
	0x12, //glare_con_ext_max_w 0000 0000
	0x38, //glare_con_ext_max_sign con_ext_max_curve con_ext_max_sh 0 0 0000
	0x00, //glare_max_cnt_th 0000
	0x00, //glare_max_cnt_th 0000 0000
	0x02, //glare_max_cnt_w 0000 0000
	0x38, //glare_max_cnt_sign max_cnt_curve max_cnt_sh 0 0 0000
	0x00, //glare_y_avg_th 0000 0000
	0x18, //glare_y_avg_w 0000 0000
	0x38, //glare_y_avg_sign y_avg_curve y_avg_sh 0 0 0000
	0x00, //glare_max_cnt_th_dmc 0000
	0x05, //glare_max_cnt_th_dmc 0000 0000
	0x02, //nr fa de cs gamma 0 0000
	0xff, //nr_mask_th
	0x00, //de_gain 10
	0x00,
	0x02, //de_maxplus 11
	0x00,
	0x02, //de_maxminus 11
	0x00,
	0x14, //fa_edge_th
	0x00, //fa_step_p 13
	0x01,
	0x00, //fa_step_n 13
	0x01,
	0x00, //fa_max_de_gain 13
	0x10,
	0x00, //fa_pcl_ppi 14
	0x00,
	0x28, //fa_os_cnt_10_co
	0x3c, //fa_os_cnt_20_co
	0x04, //fa_edge_cnt_weight
	0x0f, //fa_avg_y_weight
	0x01, //cs_gain 10
	0x40,
	0x00, //curve_0
	0x10, //curve_1
	0x20, //curve_2
	0x30, //curve_3
	0x40, //curve_4
	0x50, //curve_5
	0x60, //curve_6
	0x70, //curve_7
	0x80, //curve_8
	0x90, //curve_9
	0xa0, //curve_10
	0xb0, //curve_11
	0xc0, //curve_12
	0xd0, //curve_13
	0xe0, //curve_14
	0xf0, //curve_15
	0x01, //curve_16
	0x00,
	0x00, //curve_offset 10
	0x00,
	0x00, //curve_low_x
	0x00, //blank
	0x00, //curve_low_y
};

static char DSI_HMT_COLOR_TEMP_5000K_MDNIE_3[] = {
	0xDD,
	0x01, //mdnie_en
	0x00, //mask 00 0000
	0x03, //edge 00
	0xf0, //ascr algo aolce gamut 00 00 00 00
	0xff, //partial_en1
	0xff, //partial_en2
	0x00, //roi_en
	0x00, //roi_block
	0x00, //roi_sx
	0x00, //roi_sx
	0x00, //roi_sy
	0x00, //roi_sy
	0x00, //roi_ex
	0x00, //roi_ex
	0x00, //roi_ey
	0x00, //roi_ey
	0xff, //trans_on trans_block 0 000 0000
	0x04, //trans_slope
	0x01, //trans_interval
	//end
};

static char DSI_HMT_COLOR_TEMP_6500K_MDNIE_1[] = {
	//start
	0xDF,
	0x01, //linear_on ascr_skin_on 0000 0000
	0x6a, //ascr_skin_cb
	0x9a, //ascr_skin_cr
	0x25, //ascr_dist_up
	0x1a, //ascr_dist_down
	0x16, //ascr_dist_right
	0x2a, //ascr_dist_left
	0x00, //ascr_div_up 20
	0x37,
	0x5a,
	0x00, //ascr_div_down
	0x4e,
	0xc5,
	0x00, //ascr_div_right
	0x5d,
	0x17,
	0x00, //ascr_div_left
	0x30,
	0xc3,
	0xff, //ascr_skin_Rr
	0x00, //ascr_skin_Rg
	0x00, //ascr_skin_Rb
	0xff, //ascr_skin_Yr
	0xff, //ascr_skin_Yg
	0x00, //ascr_skin_Yb
	0xff, //ascr_skin_Mr
	0x00, //ascr_skin_Mg
	0xff, //ascr_skin_Mb
	0xff, //ascr_skin_Wr
	0xfa, //ascr_skin_Wg
	0xf1, //ascr_skin_Wb
	0x00, //ascr_Cr
	0xff, //ascr_Rr
	0xff, //ascr_Cg
	0x00, //ascr_Rg
	0xff, //ascr_Cb
	0x00, //ascr_Rb
	0xff, //ascr_Mr
	0x00, //ascr_Gr
	0x00, //ascr_Mg
	0xff, //ascr_Gg
	0xff, //ascr_Mb
	0x00, //ascr_Gb
	0xff, //ascr_Yr
	0x00, //ascr_Br
	0xff, //ascr_Yg
	0x00, //ascr_Bg
	0x00, //ascr_Yb
	0xff, //ascr_Bb
	0xff, //ascr_Wr
	0x00, //ascr_Kr
	0xfa, //ascr_Wg
	0x00, //ascr_Kg
	0xf1, //ascr_Wb
	0x00, //ascr_Kb
	0x0F, //edge_ctrl
	0x00, //edge_radius 12
	0x6F,
	0x05, //edge_aa_factor
	0x00, //edge_r
	0x00, //edge_g
	0x00, //edge_b
	0x00, //edge_x 11
	0x70,
	0x00, //edge_y 12
	0x70,
	0x44, //edge_coef_a edge_coef_b 0000 0000
};

static char DSI_HMT_COLOR_TEMP_6500K_MDNIE_2[] = {
	0xDE,
	0x00, //gamut_gamma_on gamut_blk_shift 0000 0000
	0x40, //gamut_scale
	0x04, //gamut_r1
	0x00,
	0x00, //gamut_r2
	0x00,
	0x00, //gamut_r3
	0x00,
	0x00, //gamut_g1
	0x00,
	0x04, //gamut_g2
	0x00,
	0x00, //gamut_g3
	0x00,
	0x00, //gamut_b1
	0x00,
	0x00, //gamut_b2
	0x00,
	0x04, //gamut_b3
	0x00,
	0x00, //slce_on cadrx_en 0000 0000
	0x11, //glare_on
	0x00, //lce_gain 000 0000
	0x24, //lce_color_gain 00 0000
	0x96, //lce_min_ref_offset
	0xb3, //lce_illum_gain
	0x01, //lce_ref_offset 9
	0x0e,
	0x01, //lce_ref_gain 9
	0x00,
	0x77, //lce_block_size h v 0000 0000
	0x03, //lce_dark_th 000
	0x07, //lce_reduct_slope 0000
	0x40, //lce_black cc0 cc1 color_th 0 0 0 0000
	0x08, //lce_large_w
	0x04, //lce_med_w
	0x04, //lce_small_w
	0x00, //cadrx_dit_en shadow_corr_en ref_ctrl_en 0 0 0
	0x00, //cadrx_gain
	0x0d, //cadrx_low_intensity_th
	0xe1, //cadrx_high_intensity_th
	0x07, //cadrx_ui_zerobincnt_th
	0x29, //cadrx_ui_ratio_th
	0x04,
	0x0f, //cadrx_entire_freq
	0xa0,
	0x00,
	0x03, //cadrx_peak_th_in_freq
	0x7a,
	0xa0,
	0x00, //cadrx_high_peak_th_in_freq
	0x25,
	0x1c,
	0x24, //cadrx_dit_color_gain
	0x00, //cadrx_dit_illum_a0
	0x48, //cadrx_dit_illum_a1
	0xea, //cadrx_dit_illum_a2
	0xfa, //cadrx_dit_illum_a3
	0x00, //cadrx_dit_illum_b0
	0x10,
	0x00, //cadrx_dit_illum_b1
	0x10,
	0x00, //cadrx_dit_illum_b2
	0x9c,
	0x01, //cadrx_dit_illum_b3
	0x00,
	0x00, //cadrx_dit_illum_slope1
	0x00,
	0x00, //cadrx_dit_illum_slope2
	0xdc,
	0x03, //cadrx_dit_illum_slope3
	0xff,
	0x40, //cadrx_ui_illum_a1
	0x80, //cadrx_ui_illum_a2
	0xc0, //cadrx_ui_illum_a3
	0x00, //cadrx_ui_illum_offset1
	0x9a,
	0x00, //cadrx_ui_illum_offset2
	0x9a,
	0x00, //cadrx_ui_illum_offset3
	0x9a,
	0x00, //cadrx_ui_illum_offset4
	0x9a,
	0x00, //cadrx_ui_illum_slope1
	0x00,
	0x00, //cadrx_ui_illum_slope2
	0x00,
	0x00, //cadrx_ui_illum_slope3
	0x00,
	0x00, //cadrx_ui_illum_slope4
	0x00,
	0x40, //cadrx_ui_ref_a1
	0x80, //cadrx_ui_ref_a2
	0xc0, //cadrx_ui_ref_a3
	0x01, //cadrx_ui_ref_offset1
	0x0e,
	0x01, //cadrx_ui_ref_offset2
	0x0e,
	0x01, //cadrx_ui_ref_offset3
	0x0e,
	0x01, //cadrx_ui_ref_offset4
	0x0e,
	0x00, //cadrx_ui_ref_slope1
	0x00,
	0x00, //cadrx_ui_ref_slope2
	0x00,
	0x00, //cadrx_ui_ref_slope3
	0x00,
	0x00, //cadrx_ui_ref_slope4
	0x00,
	0x01, //cadrx_reg_ref_c1_offset
	0x0e,
	0x01, //cadrx_reg_ref_c2_offset
	0x3a,
	0x01, //cadrx_reg_ref_c3_offset
	0x44,
	0x00, //cadrx_reg_ref_c1_slope
	0x01,
	0x00, //cadrx_reg_ref_c2_slope
	0x43,
	0x00, //cadrx_reg_ref_c3_slope
	0x4b,
	0x00, //cadrx_sc_reg_a1
	0x18,
	0x00, //cadrx_sc_reg_b1
	0x1c,
	0x01, //cadrx_sc_k1_int
	0xff,
	0x01, //cadrx_sc_k2_int
	0xff,
	0x03, //cadrx_sc_w1_int
	0xff,
	0xe8,
	0x03, //cadrx_sc_w2_int
	0xff,
	0xe8,
	0x00, //flicker_w_en
	0x14, //flicker_w
	0x01, //flicker_trans_en
	0x04, //flicker_trans_slope
	0x00, //bl_en
	0x03, //bl_hbs_th
	0x10, //bl_lbs_th
	0xC8, //bl_illum_gain
	0x01, //bl_reg_gain 9
	0x2C,
	0x00, //bl_le_diff
	0x00, //bl_cadrx_gain
	0x04, //bl_trans_slope
	0x00, //le_en
	0x40, //le_diff
	0x03, //le_lmax 10
	0xB6,
	0x23, //le_p
	0x24, //le_w_tmf
	0x00, //le_w_struct_diff 9
	0x72,
	0x01, //le_w_struct_diff_gain
	0x00, //le_w_luminance 9
	0x0D,
	0x00, //le_luminance_slope 10
	0x04,
	0x80, //glare_blk_avg_th 0000 0000
	0x00, //glare_luma_gain 9
	0x00,
	0x00, //glare_luma_ctl_start 0000 0000
	0x05, //glare_luma_th 0000 0000
	0x00, //glare_luma_step 9
	0x01,
	0x01, //glare_uni_luma_gain 9
	0x00,
	0xf0, //glare_blk_max_th 0000 0000
	0x12, //glare_blk_max_w 0000 0000
	0x18, //glare_blk_max_curve blk_max_sh 0 0000
	0xf0, //glare_con_ext_max_th 0000 0000
	0x12, //glare_con_ext_max_w 0000 0000
	0x38, //glare_con_ext_max_sign con_ext_max_curve con_ext_max_sh 0 0 0000
	0x00, //glare_max_cnt_th 0000
	0x00, //glare_max_cnt_th 0000 0000
	0x02, //glare_max_cnt_w 0000 0000
	0x38, //glare_max_cnt_sign max_cnt_curve max_cnt_sh 0 0 0000
	0x00, //glare_y_avg_th 0000 0000
	0x18, //glare_y_avg_w 0000 0000
	0x38, //glare_y_avg_sign y_avg_curve y_avg_sh 0 0 0000
	0x00, //glare_max_cnt_th_dmc 0000
	0x05, //glare_max_cnt_th_dmc 0000 0000
	0x02, //nr fa de cs gamma 0 0000
	0xff, //nr_mask_th
	0x00, //de_gain 10
	0x00,
	0x02, //de_maxplus 11
	0x00,
	0x02, //de_maxminus 11
	0x00,
	0x14, //fa_edge_th
	0x00, //fa_step_p 13
	0x01,
	0x00, //fa_step_n 13
	0x01,
	0x00, //fa_max_de_gain 13
	0x10,
	0x00, //fa_pcl_ppi 14
	0x00,
	0x28, //fa_os_cnt_10_co
	0x3c, //fa_os_cnt_20_co
	0x04, //fa_edge_cnt_weight
	0x0f, //fa_avg_y_weight
	0x01, //cs_gain 10
	0x40,
	0x00, //curve_0
	0x10, //curve_1
	0x20, //curve_2
	0x30, //curve_3
	0x40, //curve_4
	0x50, //curve_5
	0x60, //curve_6
	0x70, //curve_7
	0x80, //curve_8
	0x90, //curve_9
	0xa0, //curve_10
	0xb0, //curve_11
	0xc0, //curve_12
	0xd0, //curve_13
	0xe0, //curve_14
	0xf0, //curve_15
	0x01, //curve_16
	0x00,
	0x00, //curve_offset 10
	0x00,
	0x00, //curve_low_x
	0x00, //blank
	0x00, //curve_low_y
};

static char DSI_HMT_COLOR_TEMP_6500K_MDNIE_3[] = {
	0xDD,
	0x01, //mdnie_en
	0x00, //mask 00 0000
	0x03, //edge 00
	0xf0, //ascr algo aolce gamut 00 00 00 00
	0xff, //partial_en1
	0xff, //partial_en2
	0x00, //roi_en
	0x00, //roi_block
	0x00, //roi_sx
	0x00, //roi_sx
	0x00, //roi_sy
	0x00, //roi_sy
	0x00, //roi_ex
	0x00, //roi_ex
	0x00, //roi_ey
	0x00, //roi_ey
	0xff, //trans_on trans_block 0 000 0000
	0x04, //trans_slope
	0x01, //trans_interval
	//end
};

static char DSI_HMT_COLOR_TEMP_7500K_MDNIE_1[] = {
	//start
	0xDF,
	0x01, //linear_on ascr_skin_on 0000 0000
	0x6a, //ascr_skin_cb
	0x9a, //ascr_skin_cr
	0x25, //ascr_dist_up
	0x1a, //ascr_dist_down
	0x16, //ascr_dist_right
	0x2a, //ascr_dist_left
	0x00, //ascr_div_up 20
	0x37,
	0x5a,
	0x00, //ascr_div_down
	0x4e,
	0xc5,
	0x00, //ascr_div_right
	0x5d,
	0x17,
	0x00, //ascr_div_left
	0x30,
	0xc3,
	0xff, //ascr_skin_Rr
	0x00, //ascr_skin_Rg
	0x00, //ascr_skin_Rb
	0xff, //ascr_skin_Yr
	0xff, //ascr_skin_Yg
	0x00, //ascr_skin_Yb
	0xff, //ascr_skin_Mr
	0x00, //ascr_skin_Mg
	0xff, //ascr_skin_Mb
	0xff, //ascr_skin_Wr
	0xff, //ascr_skin_Wg
	0xff, //ascr_skin_Wb
	0x00, //ascr_Cr
	0xff, //ascr_Rr
	0xff, //ascr_Cg
	0x00, //ascr_Rg
	0xff, //ascr_Cb
	0x00, //ascr_Rb
	0xff, //ascr_Mr
	0x00, //ascr_Gr
	0x00, //ascr_Mg
	0xff, //ascr_Gg
	0xff, //ascr_Mb
	0x00, //ascr_Gb
	0xff, //ascr_Yr
	0x00, //ascr_Br
	0xff, //ascr_Yg
	0x00, //ascr_Bg
	0x00, //ascr_Yb
	0xff, //ascr_Bb
	0xff, //ascr_Wr
	0x00, //ascr_Kr
	0xff, //ascr_Wg
	0x00, //ascr_Kg
	0xff, //ascr_Wb
	0x00, //ascr_Kb
	0x0F, //edge_ctrl
	0x00, //edge_radius 12
	0x6F,
	0x05, //edge_aa_factor
	0x00, //edge_r
	0x00, //edge_g
	0x00, //edge_b
	0x00, //edge_x 11
	0x70,
	0x00, //edge_y 12
	0x70,
	0x44, //edge_coef_a edge_coef_b 0000 0000
};

static char DSI_HMT_COLOR_TEMP_7500K_MDNIE_2[] = {
	0xDE,
	0x00, //gamut_gamma_on gamut_blk_shift 0000 0000
	0x40, //gamut_scale
	0x04, //gamut_r1
	0x00,
	0x00, //gamut_r2
	0x00,
	0x00, //gamut_r3
	0x00,
	0x00, //gamut_g1
	0x00,
	0x04, //gamut_g2
	0x00,
	0x00, //gamut_g3
	0x00,
	0x00, //gamut_b1
	0x00,
	0x00, //gamut_b2
	0x00,
	0x04, //gamut_b3
	0x00,
	0x00, //slce_on cadrx_en 0000 0000
	0x11, //glare_on
	0x00, //lce_gain 000 0000
	0x24, //lce_color_gain 00 0000
	0x96, //lce_min_ref_offset
	0xb3, //lce_illum_gain
	0x01, //lce_ref_offset 9
	0x0e,
	0x01, //lce_ref_gain 9
	0x00,
	0x77, //lce_block_size h v 0000 0000
	0x03, //lce_dark_th 000
	0x07, //lce_reduct_slope 0000
	0x40, //lce_black cc0 cc1 color_th 0 0 0 0000
	0x08, //lce_large_w
	0x04, //lce_med_w
	0x04, //lce_small_w
	0x00, //cadrx_dit_en shadow_corr_en ref_ctrl_en 0 0 0
	0x00, //cadrx_gain
	0x0d, //cadrx_low_intensity_th
	0xe1, //cadrx_high_intensity_th
	0x07, //cadrx_ui_zerobincnt_th
	0x29, //cadrx_ui_ratio_th
	0x04,
	0x0f, //cadrx_entire_freq
	0xa0,
	0x00,
	0x03, //cadrx_peak_th_in_freq
	0x7a,
	0xa0,
	0x00, //cadrx_high_peak_th_in_freq
	0x25,
	0x1c,
	0x24, //cadrx_dit_color_gain
	0x00, //cadrx_dit_illum_a0
	0x48, //cadrx_dit_illum_a1
	0xea, //cadrx_dit_illum_a2
	0xfa, //cadrx_dit_illum_a3
	0x00, //cadrx_dit_illum_b0
	0x10,
	0x00, //cadrx_dit_illum_b1
	0x10,
	0x00, //cadrx_dit_illum_b2
	0x9c,
	0x01, //cadrx_dit_illum_b3
	0x00,
	0x00, //cadrx_dit_illum_slope1
	0x00,
	0x00, //cadrx_dit_illum_slope2
	0xdc,
	0x03, //cadrx_dit_illum_slope3
	0xff,
	0x40, //cadrx_ui_illum_a1
	0x80, //cadrx_ui_illum_a2
	0xc0, //cadrx_ui_illum_a3
	0x00, //cadrx_ui_illum_offset1
	0x9a,
	0x00, //cadrx_ui_illum_offset2
	0x9a,
	0x00, //cadrx_ui_illum_offset3
	0x9a,
	0x00, //cadrx_ui_illum_offset4
	0x9a,
	0x00, //cadrx_ui_illum_slope1
	0x00,
	0x00, //cadrx_ui_illum_slope2
	0x00,
	0x00, //cadrx_ui_illum_slope3
	0x00,
	0x00, //cadrx_ui_illum_slope4
	0x00,
	0x40, //cadrx_ui_ref_a1
	0x80, //cadrx_ui_ref_a2
	0xc0, //cadrx_ui_ref_a3
	0x01, //cadrx_ui_ref_offset1
	0x0e,
	0x01, //cadrx_ui_ref_offset2
	0x0e,
	0x01, //cadrx_ui_ref_offset3
	0x0e,
	0x01, //cadrx_ui_ref_offset4
	0x0e,
	0x00, //cadrx_ui_ref_slope1
	0x00,
	0x00, //cadrx_ui_ref_slope2
	0x00,
	0x00, //cadrx_ui_ref_slope3
	0x00,
	0x00, //cadrx_ui_ref_slope4
	0x00,
	0x01, //cadrx_reg_ref_c1_offset
	0x0e,
	0x01, //cadrx_reg_ref_c2_offset
	0x3a,
	0x01, //cadrx_reg_ref_c3_offset
	0x44,
	0x00, //cadrx_reg_ref_c1_slope
	0x01,
	0x00, //cadrx_reg_ref_c2_slope
	0x43,
	0x00, //cadrx_reg_ref_c3_slope
	0x4b,
	0x00, //cadrx_sc_reg_a1
	0x18,
	0x00, //cadrx_sc_reg_b1
	0x1c,
	0x01, //cadrx_sc_k1_int
	0xff,
	0x01, //cadrx_sc_k2_int
	0xff,
	0x03, //cadrx_sc_w1_int
	0xff,
	0xe8,
	0x03, //cadrx_sc_w2_int
	0xff,
	0xe8,
	0x00, //flicker_w_en
	0x14, //flicker_w
	0x01, //flicker_trans_en
	0x04, //flicker_trans_slope
	0x00, //bl_en
	0x03, //bl_hbs_th
	0x10, //bl_lbs_th
	0xC8, //bl_illum_gain
	0x01, //bl_reg_gain 9
	0x2C,
	0x00, //bl_le_diff
	0x00, //bl_cadrx_gain
	0x04, //bl_trans_slope
	0x00, //le_en
	0x40, //le_diff
	0x03, //le_lmax 10
	0xB6,
	0x23, //le_p
	0x24, //le_w_tmf
	0x00, //le_w_struct_diff 9
	0x72,
	0x01, //le_w_struct_diff_gain
	0x00, //le_w_luminance 9
	0x0D,
	0x00, //le_luminance_slope 10
	0x04,
	0x80, //glare_blk_avg_th 0000 0000
	0x00, //glare_luma_gain 9
	0x00,
	0x00, //glare_luma_ctl_start 0000 0000
	0x05, //glare_luma_th 0000 0000
	0x00, //glare_luma_step 9
	0x01,
	0x01, //glare_uni_luma_gain 9
	0x00,
	0xf0, //glare_blk_max_th 0000 0000
	0x12, //glare_blk_max_w 0000 0000
	0x18, //glare_blk_max_curve blk_max_sh 0 0000
	0xf0, //glare_con_ext_max_th 0000 0000
	0x12, //glare_con_ext_max_w 0000 0000
	0x38, //glare_con_ext_max_sign con_ext_max_curve con_ext_max_sh 0 0 0000
	0x00, //glare_max_cnt_th 0000
	0x00, //glare_max_cnt_th 0000 0000
	0x02, //glare_max_cnt_w 0000 0000
	0x38, //glare_max_cnt_sign max_cnt_curve max_cnt_sh 0 0 0000
	0x00, //glare_y_avg_th 0000 0000
	0x18, //glare_y_avg_w 0000 0000
	0x38, //glare_y_avg_sign y_avg_curve y_avg_sh 0 0 0000
	0x00, //glare_max_cnt_th_dmc 0000
	0x05, //glare_max_cnt_th_dmc 0000 0000
	0x02, //nr fa de cs gamma 0 0000
	0xff, //nr_mask_th
	0x00, //de_gain 10
	0x00,
	0x02, //de_maxplus 11
	0x00,
	0x02, //de_maxminus 11
	0x00,
	0x14, //fa_edge_th
	0x00, //fa_step_p 13
	0x01,
	0x00, //fa_step_n 13
	0x01,
	0x00, //fa_max_de_gain 13
	0x10,
	0x00, //fa_pcl_ppi 14
	0x00,
	0x28, //fa_os_cnt_10_co
	0x3c, //fa_os_cnt_20_co
	0x04, //fa_edge_cnt_weight
	0x0f, //fa_avg_y_weight
	0x01, //cs_gain 10
	0x40,
	0x00, //curve_0
	0x10, //curve_1
	0x20, //curve_2
	0x30, //curve_3
	0x40, //curve_4
	0x50, //curve_5
	0x60, //curve_6
	0x70, //curve_7
	0x80, //curve_8
	0x90, //curve_9
	0xa0, //curve_10
	0xb0, //curve_11
	0xc0, //curve_12
	0xd0, //curve_13
	0xe0, //curve_14
	0xf0, //curve_15
	0x01, //curve_16
	0x00,
	0x00, //curve_offset 10
	0x00,
	0x00, //curve_low_x
	0x00, //blank
	0x00, //curve_low_y
};

static char DSI_HMT_COLOR_TEMP_7500K_MDNIE_3[] = {
	0xDD,
	0x01, //mdnie_en
	0x00, //mask 00 0000
	0x03, //edge 00
	0xf0, //ascr algo aolce gamut 00 00 00 00
	0xff, //partial_en1
	0xff, //partial_en2
	0x00, //roi_en
	0x00, //roi_block
	0x00, //roi_sx
	0x00, //roi_sx
	0x00, //roi_sy
	0x00, //roi_sy
	0x00, //roi_ex
	0x00, //roi_ex
	0x00, //roi_ey
	0x00, //roi_ey
	0xff, //trans_on trans_block 0 000 0000
	0x04, //trans_slope
	0x01, //trans_interval
	//end
};

static struct dsi_cmd_desc DSI_BYPASS_MDNIE[] = {
	{{0, MIPI_DSI_DCS_LONG_WRITE, 0, 0, sizeof(level_1_key_on), level_1_key_on, 0, NULL}, false, 0},
	{{0, MIPI_DSI_DCS_LONG_WRITE, 0, 0, sizeof(DSI_BYPASS_MDNIE_1), DSI_BYPASS_MDNIE_1, 0, NULL}, false, 0},
	{{0, MIPI_DSI_DCS_LONG_WRITE, 0, 0, sizeof(DSI_BYPASS_MDNIE_2), DSI_BYPASS_MDNIE_2, 0, NULL}, false, 0},
	{{0, MIPI_DSI_DCS_LONG_WRITE, 0, 0, sizeof(DSI_BYPASS_MDNIE_3), DSI_BYPASS_MDNIE_3, 0, NULL}, false, 0},
	{{0, MIPI_DSI_DCS_LONG_WRITE, 0, 0, sizeof(level_1_key_off), level_1_key_off, 0, NULL}, true, 0},
};

static struct dsi_cmd_desc DSI_NEGATIVE_MDNIE[] = {
	{{0, MIPI_DSI_DCS_LONG_WRITE, 0, 0, sizeof(level_1_key_on), level_1_key_on, 0, NULL}, false, 0},
	{{0, MIPI_DSI_DCS_LONG_WRITE, 0, 0, sizeof(DSI_NEGATIVE_MDNIE_1), DSI_NEGATIVE_MDNIE_1, 0, NULL}, false, 0},
	{{0, MIPI_DSI_DCS_LONG_WRITE, 0, 0, sizeof(DSI_NEGATIVE_MDNIE_2), DSI_NEGATIVE_MDNIE_2, 0, NULL}, false, 0},
	{{0, MIPI_DSI_DCS_LONG_WRITE, 0, 0, sizeof(DSI_NEGATIVE_MDNIE_3), DSI_NEGATIVE_MDNIE_3, 0, NULL}, false, 0},
	{{0, MIPI_DSI_DCS_LONG_WRITE, 0, 0, sizeof(level_1_key_off), level_1_key_off, 0, NULL}, true, 0},
};

static struct dsi_cmd_desc DSI_GRAYSCALE_MDNIE[] = {
	{{0, MIPI_DSI_DCS_LONG_WRITE, 0, 0, sizeof(level_1_key_on), level_1_key_on, 0, NULL}, false, 0},
	{{0, MIPI_DSI_DCS_LONG_WRITE, 0, 0, sizeof(DSI_GRAYSCALE_MDNIE_1), DSI_GRAYSCALE_MDNIE_1, 0, NULL}, false, 0},
	{{0, MIPI_DSI_DCS_LONG_WRITE, 0, 0, sizeof(DSI_GRAYSCALE_MDNIE_2), DSI_GRAYSCALE_MDNIE_2, 0, NULL}, false, 0},
	{{0, MIPI_DSI_DCS_LONG_WRITE, 0, 0, sizeof(DSI_GRAYSCALE_MDNIE_3), DSI_GRAYSCALE_MDNIE_3, 0, NULL}, false, 0},
	{{0, MIPI_DSI_DCS_LONG_WRITE, 0, 0, sizeof(level_1_key_off), level_1_key_off, 0, NULL}, true, 0},
};

static struct dsi_cmd_desc DSI_GRAYSCALE_NEGATIVE_MDNIE[] = {
	{{0, MIPI_DSI_DCS_LONG_WRITE, 0, 0, sizeof(level_1_key_on), level_1_key_on, 0, NULL}, false, 0},
	{{0, MIPI_DSI_DCS_LONG_WRITE, 0, 0, sizeof(DSI_GRAYSCALE_NEGATIVE_MDNIE_1), DSI_GRAYSCALE_NEGATIVE_MDNIE_1, 0, NULL}, false, 0},
	{{0, MIPI_DSI_DCS_LONG_WRITE, 0, 0, sizeof(DSI_GRAYSCALE_NEGATIVE_MDNIE_2), DSI_GRAYSCALE_NEGATIVE_MDNIE_2, 0, NULL}, false, 0},
	{{0, MIPI_DSI_DCS_LONG_WRITE, 0, 0, sizeof(DSI_GRAYSCALE_NEGATIVE_MDNIE_3), DSI_GRAYSCALE_NEGATIVE_MDNIE_3, 0, NULL}, false, 0},
	{{0, MIPI_DSI_DCS_LONG_WRITE, 0, 0, sizeof(level_1_key_off), level_1_key_off, 0, NULL}, true, 0},
};

static struct dsi_cmd_desc DSI_COLOR_BLIND_MDNIE[] = {
	{{0, MIPI_DSI_DCS_LONG_WRITE, 0, 0, sizeof(level_1_key_on), level_1_key_on, 0, NULL}, false, 0},
	{{0, MIPI_DSI_DCS_LONG_WRITE, 0, 0, sizeof(DSI_COLOR_BLIND_MDNIE_1), DSI_COLOR_BLIND_MDNIE_1, 0, NULL}, false, 0},
	{{0, MIPI_DSI_DCS_LONG_WRITE, 0, 0, sizeof(DSI_COLOR_BLIND_MDNIE_2), DSI_COLOR_BLIND_MDNIE_2, 0, NULL}, false, 0},
	{{0, MIPI_DSI_DCS_LONG_WRITE, 0, 0, sizeof(DSI_COLOR_BLIND_MDNIE_3), DSI_COLOR_BLIND_MDNIE_3, 0, NULL}, false, 0},
	{{0, MIPI_DSI_DCS_LONG_WRITE, 0, 0, sizeof(level_1_key_off), level_1_key_off, 0, NULL}, true, 0},
};

static struct dsi_cmd_desc DSI_NIGHT_MODE_MDNIE[] = {
	{{0, MIPI_DSI_DCS_LONG_WRITE, 0, 0, sizeof(level_1_key_on), level_1_key_on, 0, NULL}, false, 0},
	{{0, MIPI_DSI_DCS_LONG_WRITE, 0, 0, sizeof(DSI_NIGHT_MODE_MDNIE_1), DSI_NIGHT_MODE_MDNIE_1, 0, NULL}, false, 0},
	{{0, MIPI_DSI_DCS_LONG_WRITE, 0, 0, sizeof(DSI_NIGHT_MODE_MDNIE_2), DSI_NIGHT_MODE_MDNIE_2, 0, NULL}, false, 0},
	{{0, MIPI_DSI_DCS_LONG_WRITE, 0, 0, sizeof(DSI_NIGHT_MODE_MDNIE_3), DSI_NIGHT_MODE_MDNIE_3, 0, NULL}, false, 0},
	{{0, MIPI_DSI_DCS_LONG_WRITE, 0, 0, sizeof(level_1_key_off), level_1_key_off, 0, NULL}, true, 0},
};

static struct dsi_cmd_desc DSI_HBM_CE_MDNIE[] = {
	{{0, MIPI_DSI_DCS_LONG_WRITE, 0, 0, sizeof(level_1_key_on), level_1_key_on, 0, NULL}, false, 0},
	{{0, MIPI_DSI_DCS_LONG_WRITE, 0, 0, sizeof(DSI_HBM_CE_MDNIE_1), DSI_HBM_CE_MDNIE_1, 0, NULL}, false, 0},
	{{0, MIPI_DSI_DCS_LONG_WRITE, 0, 0, sizeof(DSI_HBM_CE_MDNIE_2), DSI_HBM_CE_MDNIE_2, 0, NULL}, false, 0},
	{{0, MIPI_DSI_DCS_LONG_WRITE, 0, 0, sizeof(DSI_HBM_CE_MDNIE_3), DSI_HBM_CE_MDNIE_3, 0, NULL}, false, 0},
	{{0, MIPI_DSI_DCS_LONG_WRITE, 0, 0, sizeof(level_1_key_off), level_1_key_off, 0, NULL}, true, 0},
};

static struct dsi_cmd_desc DSI_HBM_CE_D65_MDNIE[] = {
	{{0, MIPI_DSI_DCS_LONG_WRITE, 0, 0, sizeof(level_1_key_on), level_1_key_on, 0, NULL}, false, 0},
	{{0, MIPI_DSI_DCS_LONG_WRITE, 0, 0, sizeof(DSI_HBM_CE_D65_MDNIE_1), DSI_HBM_CE_D65_MDNIE_1, 0, NULL}, false, 0},
	{{0, MIPI_DSI_DCS_LONG_WRITE, 0, 0, sizeof(DSI_HBM_CE_D65_MDNIE_2), DSI_HBM_CE_D65_MDNIE_2, 0, NULL}, false, 0},
	{{0, MIPI_DSI_DCS_LONG_WRITE, 0, 0, sizeof(DSI_HBM_CE_D65_MDNIE_3), DSI_HBM_CE_D65_MDNIE_3, 0, NULL}, false, 0},
	{{0, MIPI_DSI_DCS_LONG_WRITE, 0, 0, sizeof(level_1_key_off), level_1_key_off, 0, NULL}, true, 0},
};

static struct dsi_cmd_desc DSI_RGB_SENSOR_MDNIE[] = {
	{{0, MIPI_DSI_DCS_LONG_WRITE, 0, 0, sizeof(level_1_key_on), level_1_key_on, 0, NULL}, false, 0},
	{{0, MIPI_DSI_DCS_LONG_WRITE, 0, 0, sizeof(DSI_RGB_SENSOR_MDNIE_1), DSI_RGB_SENSOR_MDNIE_1, 0, NULL}, false, 0},
	{{0, MIPI_DSI_DCS_LONG_WRITE, 0, 0, sizeof(DSI_RGB_SENSOR_MDNIE_2), DSI_RGB_SENSOR_MDNIE_2, 0, NULL}, false, 0},
	{{0, MIPI_DSI_DCS_LONG_WRITE, 0, 0, sizeof(DSI_RGB_SENSOR_MDNIE_3), DSI_RGB_SENSOR_MDNIE_3, 0, NULL}, false, 0},
	{{0, MIPI_DSI_DCS_LONG_WRITE, 0, 0, sizeof(level_1_key_off), level_1_key_off, 0, NULL}, true, 0},
};

static struct dsi_cmd_desc DSI_SCREEN_CURTAIN_MDNIE[] = {
	{{0, MIPI_DSI_DCS_LONG_WRITE, 0, 0, sizeof(level_1_key_on), level_1_key_on, 0, NULL}, false, 0},
	{{0, MIPI_DSI_DCS_LONG_WRITE, 0, 0, sizeof(DSI_SCREEN_CURTAIN_MDNIE_1), DSI_SCREEN_CURTAIN_MDNIE_1, 0, NULL}, false, 0},
	{{0, MIPI_DSI_DCS_LONG_WRITE, 0, 0, sizeof(DSI_SCREEN_CURTAIN_MDNIE_2), DSI_SCREEN_CURTAIN_MDNIE_2, 0, NULL}, false, 0},
	{{0, MIPI_DSI_DCS_LONG_WRITE, 0, 0, sizeof(DSI_SCREEN_CURTAIN_MDNIE_3), DSI_SCREEN_CURTAIN_MDNIE_3, 0, NULL}, false, 0},
	{{0, MIPI_DSI_DCS_LONG_WRITE, 0, 0, sizeof(level_1_key_off), level_1_key_off, 0, NULL}, true, 0},
};

static struct dsi_cmd_desc DSI_LIGHT_NOTIFICATION_MDNIE[] = {
	{{0, MIPI_DSI_DCS_LONG_WRITE, 0, 0, sizeof(level_1_key_on), level_1_key_on, 0, NULL}, false, 0},
	{{0, MIPI_DSI_DCS_LONG_WRITE, 0, 0, sizeof(DSI_LIGHT_NOTIFICATION_MDNIE_1), DSI_LIGHT_NOTIFICATION_MDNIE_1, 0, NULL}, false, 0},
	{{0, MIPI_DSI_DCS_LONG_WRITE, 0, 0, sizeof(DSI_LIGHT_NOTIFICATION_MDNIE_2), DSI_LIGHT_NOTIFICATION_MDNIE_2, 0, NULL}, false, 0},
	{{0, MIPI_DSI_DCS_LONG_WRITE, 0, 0, sizeof(DSI_LIGHT_NOTIFICATION_MDNIE_3), DSI_LIGHT_NOTIFICATION_MDNIE_3, 0, NULL}, false, 0},
	{{0, MIPI_DSI_DCS_LONG_WRITE, 0, 0, sizeof(level_1_key_off), level_1_key_off, 0, NULL}, true, 0},
};

static struct dsi_cmd_desc DSI_HDR_VIDEO_1_MDNIE[] = {
	{{0, MIPI_DSI_DCS_LONG_WRITE, 0, 0, sizeof(level_1_key_on), level_1_key_on, 0, NULL}, false, 0},
	{{0, MIPI_DSI_DCS_LONG_WRITE, 0, 0, sizeof(DSI_HDR_VIDEO_1_MDNIE_1), DSI_HDR_VIDEO_1_MDNIE_1, 0, NULL}, false, 0},
	{{0, MIPI_DSI_DCS_LONG_WRITE, 0, 0, sizeof(DSI_HDR_VIDEO_1_MDNIE_2), DSI_HDR_VIDEO_1_MDNIE_2, 0, NULL}, false, 0},
	{{0, MIPI_DSI_DCS_LONG_WRITE, 0, 0, sizeof(DSI_HDR_VIDEO_1_MDNIE_3), DSI_HDR_VIDEO_1_MDNIE_3, 0, NULL}, false, 0},
	{{0, MIPI_DSI_DCS_LONG_WRITE, 0, 0, sizeof(level_1_key_off), level_1_key_off, 0, NULL}, true, 0},
};

static struct dsi_cmd_desc DSI_HDR_VIDEO_2_MDNIE[] = {
	{{0, MIPI_DSI_DCS_LONG_WRITE, 0, 0, sizeof(level_1_key_on), level_1_key_on, 0, NULL}, false, 0},
	{{0, MIPI_DSI_DCS_LONG_WRITE, 0, 0, sizeof(DSI_HDR_VIDEO_2_MDNIE_1), DSI_HDR_VIDEO_2_MDNIE_1, 0, NULL}, false, 0},
	{{0, MIPI_DSI_DCS_LONG_WRITE, 0, 0, sizeof(DSI_HDR_VIDEO_2_MDNIE_2), DSI_HDR_VIDEO_2_MDNIE_2, 0, NULL}, false, 0},
	{{0, MIPI_DSI_DCS_LONG_WRITE, 0, 0, sizeof(DSI_HDR_VIDEO_2_MDNIE_3), DSI_HDR_VIDEO_2_MDNIE_3, 0, NULL}, false, 0},
	{{0, MIPI_DSI_DCS_LONG_WRITE, 0, 0, sizeof(level_1_key_off), level_1_key_off, 0, NULL}, true, 0},
};

static struct dsi_cmd_desc DSI_HDR_VIDEO_3_MDNIE[] = {
	{{0, MIPI_DSI_DCS_LONG_WRITE, 0, 0, sizeof(level_1_key_on), level_1_key_on, 0, NULL}, false, 0},
	{{0, MIPI_DSI_DCS_LONG_WRITE, 0, 0, sizeof(DSI_HDR_VIDEO_3_MDNIE_1), DSI_HDR_VIDEO_3_MDNIE_1, 0, NULL}, false, 0},
	{{0, MIPI_DSI_DCS_LONG_WRITE, 0, 0, sizeof(DSI_HDR_VIDEO_3_MDNIE_2), DSI_HDR_VIDEO_3_MDNIE_2, 0, NULL}, false, 0},
	{{0, MIPI_DSI_DCS_LONG_WRITE, 0, 0, sizeof(DSI_HDR_VIDEO_3_MDNIE_3), DSI_HDR_VIDEO_3_MDNIE_3, 0, NULL}, false, 0},
	{{0, MIPI_DSI_DCS_LONG_WRITE, 0, 0, sizeof(level_1_key_off), level_1_key_off, 0, NULL}, true, 0},
};

static struct dsi_cmd_desc DSI_VIDEO_ENHANCER_D65_MDNIE[] = {
	{{0, MIPI_DSI_DCS_LONG_WRITE, 0, 0, sizeof(level_1_key_on), level_1_key_on, 0, NULL}, false, 0},
	{{0, MIPI_DSI_DCS_LONG_WRITE, 0, 0, sizeof(DSI_VIDEO_ENHANCER_D65_MDNIE_1), DSI_VIDEO_ENHANCER_D65_MDNIE_1, 0, NULL}, false, 0},
	{{0, MIPI_DSI_DCS_LONG_WRITE, 0, 0, sizeof(DSI_VIDEO_ENHANCER_D65_MDNIE_2), DSI_VIDEO_ENHANCER_D65_MDNIE_2, 0, NULL}, false, 0},
	{{0, MIPI_DSI_DCS_LONG_WRITE, 0, 0, sizeof(DSI_VIDEO_ENHANCER_D65_MDNIE_3), DSI_VIDEO_ENHANCER_D65_MDNIE_3, 0, NULL}, false, 0},
	{{0, MIPI_DSI_DCS_LONG_WRITE, 0, 0, sizeof(level_1_key_off), level_1_key_off, 0, NULL}, true, 0},
};

static struct dsi_cmd_desc DSI_VIDEO_ENHANCER_AUTO_MDNIE[] = {
	{{0, MIPI_DSI_DCS_LONG_WRITE, 0, 0, sizeof(level_1_key_on), level_1_key_on, 0, NULL}, false, 0},
	{{0, MIPI_DSI_DCS_LONG_WRITE, 0, 0, sizeof(DSI_VIDEO_ENHANCER_AUTO_MDNIE_1), DSI_VIDEO_ENHANCER_AUTO_MDNIE_1, 0, NULL}, false, 0},
	{{0, MIPI_DSI_DCS_LONG_WRITE, 0, 0, sizeof(DSI_VIDEO_ENHANCER_AUTO_MDNIE_2), DSI_VIDEO_ENHANCER_AUTO_MDNIE_2, 0, NULL}, false, 0},
	{{0, MIPI_DSI_DCS_LONG_WRITE, 0, 0, sizeof(DSI_VIDEO_ENHANCER_AUTO_MDNIE_3), DSI_VIDEO_ENHANCER_AUTO_MDNIE_3, 0, NULL}, false, 0},
	{{0, MIPI_DSI_DCS_LONG_WRITE, 0, 0, sizeof(level_1_key_off), level_1_key_off, 0, NULL}, true, 0},
};

static struct dsi_cmd_desc DSI_VIDEO_ENHANCER_THIRD_D65_MDNIE[] = {
	{{0, MIPI_DSI_DCS_LONG_WRITE, 0, 0, sizeof(level_1_key_on), level_1_key_on, 0, NULL}, false, 0},
	{{0, MIPI_DSI_DCS_LONG_WRITE, 0, 0, sizeof(DSI_VIDEO_ENHANCER_THIRD_D65_MDNIE_1), DSI_VIDEO_ENHANCER_THIRD_D65_MDNIE_1, 0, NULL}, false, 0},
	{{0, MIPI_DSI_DCS_LONG_WRITE, 0, 0, sizeof(DSI_VIDEO_ENHANCER_THIRD_D65_MDNIE_2), DSI_VIDEO_ENHANCER_THIRD_D65_MDNIE_2, 0, NULL}, false, 0},
	{{0, MIPI_DSI_DCS_LONG_WRITE, 0, 0, sizeof(DSI_VIDEO_ENHANCER_THIRD_D65_MDNIE_3), DSI_VIDEO_ENHANCER_THIRD_D65_MDNIE_3, 0, NULL}, false, 0},
	{{0, MIPI_DSI_DCS_LONG_WRITE, 0, 0, sizeof(level_1_key_off), level_1_key_off, 0, NULL}, true, 0},
};

static struct dsi_cmd_desc DSI_VIDEO_ENHANCER_THIRD_AUTO_MDNIE[] = {
	{{0, MIPI_DSI_DCS_LONG_WRITE, 0, 0, sizeof(level_1_key_on), level_1_key_on, 0, NULL}, false, 0},
	{{0, MIPI_DSI_DCS_LONG_WRITE, 0, 0, sizeof(DSI_VIDEO_ENHANCER_THIRD_AUTO_MDNIE_1), DSI_VIDEO_ENHANCER_THIRD_AUTO_MDNIE_1, 0, NULL}, false, 0},
	{{0, MIPI_DSI_DCS_LONG_WRITE, 0, 0, sizeof(DSI_VIDEO_ENHANCER_THIRD_AUTO_MDNIE_2), DSI_VIDEO_ENHANCER_THIRD_AUTO_MDNIE_2, 0, NULL}, false, 0},
	{{0, MIPI_DSI_DCS_LONG_WRITE, 0, 0, sizeof(DSI_VIDEO_ENHANCER_THIRD_AUTO_MDNIE_3), DSI_VIDEO_ENHANCER_THIRD_AUTO_MDNIE_3, 0, NULL}, false, 0},
	{{0, MIPI_DSI_DCS_LONG_WRITE, 0, 0, sizeof(level_1_key_off), level_1_key_off, 0, NULL}, true, 0},
};

///////////////////////////////////////////////////////////////////////////////////

static struct dsi_cmd_desc DSI_UI_DYNAMIC_MDNIE[] = {
	{{0, MIPI_DSI_DCS_LONG_WRITE, 0, 0, sizeof(level_1_key_on), level_1_key_on, 0, NULL}, false, 0},
	{{0, MIPI_DSI_DCS_LONG_WRITE, 0, 0, sizeof(DSI_UI_DYNAMIC_MDNIE_1), DSI_UI_DYNAMIC_MDNIE_1, 0, NULL}, false, 0},
	{{0, MIPI_DSI_DCS_LONG_WRITE, 0, 0, sizeof(DSI_UI_DYNAMIC_MDNIE_2), DSI_UI_DYNAMIC_MDNIE_2, 0, NULL}, false, 0},
	{{0, MIPI_DSI_DCS_LONG_WRITE, 0, 0, sizeof(DSI_UI_DYNAMIC_MDNIE_3), DSI_UI_DYNAMIC_MDNIE_3, 0, NULL}, false, 0},
	{{0, MIPI_DSI_DCS_LONG_WRITE, 0, 0, sizeof(level_1_key_off), level_1_key_off, 0, NULL}, true, 0},
};

static struct dsi_cmd_desc DSI_UI_STANDARD_MDNIE[] = {
	{{0, MIPI_DSI_DCS_LONG_WRITE, 0, 0, sizeof(level_1_key_on), level_1_key_on, 0, NULL}, false, 0},
	{{0, MIPI_DSI_DCS_LONG_WRITE, 0, 0, sizeof(DSI_UI_STANDARD_MDNIE_1), DSI_UI_STANDARD_MDNIE_1, 0, NULL}, false, 0},
	{{0, MIPI_DSI_DCS_LONG_WRITE, 0, 0, sizeof(DSI_UI_STANDARD_MDNIE_2), DSI_UI_STANDARD_MDNIE_2, 0, NULL}, false, 0},
	{{0, MIPI_DSI_DCS_LONG_WRITE, 0, 0, sizeof(DSI_UI_STANDARD_MDNIE_3), DSI_UI_STANDARD_MDNIE_3, 0, NULL}, false, 0},
	{{0, MIPI_DSI_DCS_LONG_WRITE, 0, 0, sizeof(level_1_key_off), level_1_key_off, 0, NULL}, true, 0},
};

static struct dsi_cmd_desc DSI_UI_NATURAL_MDNIE[] = {
	{{0, MIPI_DSI_DCS_LONG_WRITE, 0, 0, sizeof(level_1_key_on), level_1_key_on, 0, NULL}, false, 0},
	{{0, MIPI_DSI_DCS_LONG_WRITE, 0, 0, sizeof(DSI_UI_NATURAL_MDNIE_1), DSI_UI_NATURAL_MDNIE_1, 0, NULL}, false, 0},
	{{0, MIPI_DSI_DCS_LONG_WRITE, 0, 0, sizeof(DSI_UI_NATURAL_MDNIE_2), DSI_UI_NATURAL_MDNIE_2, 0, NULL}, false, 0},
	{{0, MIPI_DSI_DCS_LONG_WRITE, 0, 0, sizeof(DSI_UI_NATURAL_MDNIE_3), DSI_UI_NATURAL_MDNIE_3, 0, NULL}, false, 0},
	{{0, MIPI_DSI_DCS_LONG_WRITE, 0, 0, sizeof(level_1_key_off), level_1_key_off, 0, NULL}, true, 0},
};

static struct dsi_cmd_desc DSI_UI_AUTO_MDNIE[] = {
	{{0, MIPI_DSI_DCS_LONG_WRITE, 0, 0, sizeof(level_1_key_on), level_1_key_on, 0, NULL}, false, 0},
	{{0, MIPI_DSI_DCS_LONG_WRITE, 0, 0, sizeof(DSI_UI_AUTO_MDNIE_1), DSI_UI_AUTO_MDNIE_1, 0, NULL}, false, 0},
	{{0, MIPI_DSI_DCS_LONG_WRITE, 0, 0, sizeof(DSI_UI_AUTO_MDNIE_2), DSI_UI_AUTO_MDNIE_2, 0, NULL}, false, 0},
	{{0, MIPI_DSI_DCS_LONG_WRITE, 0, 0, sizeof(DSI_UI_AUTO_MDNIE_3), DSI_UI_AUTO_MDNIE_3, 0, NULL}, false, 0},
	{{0, MIPI_DSI_DCS_LONG_WRITE, 0, 0, sizeof(level_1_key_off), level_1_key_off, 0, NULL}, true, 0},
};

static struct dsi_cmd_desc DSI_VIDEO_DYNAMIC_MDNIE[] = {
	{{0, MIPI_DSI_DCS_LONG_WRITE, 0, 0, sizeof(level_1_key_on), level_1_key_on, 0, NULL}, false, 0},
	{{0, MIPI_DSI_DCS_LONG_WRITE, 0, 0, sizeof(DSI_VIDEO_DYNAMIC_MDNIE_1), DSI_VIDEO_DYNAMIC_MDNIE_1, 0, NULL}, false, 0},
	{{0, MIPI_DSI_DCS_LONG_WRITE, 0, 0, sizeof(DSI_VIDEO_DYNAMIC_MDNIE_2), DSI_VIDEO_DYNAMIC_MDNIE_2, 0, NULL}, false, 0},
	{{0, MIPI_DSI_DCS_LONG_WRITE, 0, 0, sizeof(DSI_VIDEO_DYNAMIC_MDNIE_3), DSI_VIDEO_DYNAMIC_MDNIE_3, 0, NULL}, false, 0},
	{{0, MIPI_DSI_DCS_LONG_WRITE, 0, 0, sizeof(level_1_key_off), level_1_key_off, 0, NULL}, true, 0},
};

static struct dsi_cmd_desc DSI_VIDEO_STANDARD_MDNIE[] = {
	{{0, MIPI_DSI_DCS_LONG_WRITE, 0, 0, sizeof(level_1_key_on), level_1_key_on, 0, NULL}, false, 0},
	{{0, MIPI_DSI_DCS_LONG_WRITE, 0, 0, sizeof(DSI_VIDEO_STANDARD_MDNIE_1), DSI_VIDEO_STANDARD_MDNIE_1, 0, NULL}, false, 0},
	{{0, MIPI_DSI_DCS_LONG_WRITE, 0, 0, sizeof(DSI_VIDEO_STANDARD_MDNIE_2), DSI_VIDEO_STANDARD_MDNIE_2, 0, NULL}, false, 0},
	{{0, MIPI_DSI_DCS_LONG_WRITE, 0, 0, sizeof(DSI_VIDEO_STANDARD_MDNIE_3), DSI_VIDEO_STANDARD_MDNIE_3, 0, NULL}, false, 0},
	{{0, MIPI_DSI_DCS_LONG_WRITE, 0, 0, sizeof(level_1_key_off), level_1_key_off, 0, NULL}, true, 0},
};

static struct dsi_cmd_desc DSI_VIDEO_NATURAL_MDNIE[] = {
	{{0, MIPI_DSI_DCS_LONG_WRITE, 0, 0, sizeof(level_1_key_on), level_1_key_on, 0, NULL}, false, 0},
	{{0, MIPI_DSI_DCS_LONG_WRITE, 0, 0, sizeof(DSI_VIDEO_NATURAL_MDNIE_1), DSI_VIDEO_NATURAL_MDNIE_1, 0, NULL}, false, 0},
	{{0, MIPI_DSI_DCS_LONG_WRITE, 0, 0, sizeof(DSI_VIDEO_NATURAL_MDNIE_2), DSI_VIDEO_NATURAL_MDNIE_2, 0, NULL}, false, 0},
	{{0, MIPI_DSI_DCS_LONG_WRITE, 0, 0, sizeof(DSI_VIDEO_NATURAL_MDNIE_3), DSI_VIDEO_NATURAL_MDNIE_3, 0, NULL}, false, 0},
	{{0, MIPI_DSI_DCS_LONG_WRITE, 0, 0, sizeof(level_1_key_off), level_1_key_off, 0, NULL}, true, 0},
};

static struct dsi_cmd_desc DSI_VIDEO_AUTO_MDNIE[] = {
	{{0, MIPI_DSI_DCS_LONG_WRITE, 0, 0, sizeof(level_1_key_on), level_1_key_on, 0, NULL}, false, 0},
	{{0, MIPI_DSI_DCS_LONG_WRITE, 0, 0, sizeof(DSI_VIDEO_AUTO_MDNIE_1), DSI_VIDEO_AUTO_MDNIE_1, 0, NULL}, false, 0},
	{{0, MIPI_DSI_DCS_LONG_WRITE, 0, 0, sizeof(DSI_VIDEO_AUTO_MDNIE_2), DSI_VIDEO_AUTO_MDNIE_2, 0, NULL}, false, 0},
	{{0, MIPI_DSI_DCS_LONG_WRITE, 0, 0, sizeof(DSI_VIDEO_AUTO_MDNIE_3), DSI_VIDEO_AUTO_MDNIE_3, 0, NULL}, false, 0},
	{{0, MIPI_DSI_DCS_LONG_WRITE, 0, 0, sizeof(level_1_key_off), level_1_key_off, 0, NULL}, true, 0},
};

static struct dsi_cmd_desc DSI_CAMERA_DYNAMIC_MDNIE[] = {
	{{0, MIPI_DSI_DCS_LONG_WRITE, 0, 0, sizeof(level_1_key_on), level_1_key_on, 0, NULL}, false, 0},
	{{0, MIPI_DSI_DCS_LONG_WRITE, 0, 0, sizeof(DSI_CAMERA_DYNAMIC_MDNIE_1), DSI_CAMERA_DYNAMIC_MDNIE_1, 0, NULL}, false, 0},
	{{0, MIPI_DSI_DCS_LONG_WRITE, 0, 0, sizeof(DSI_CAMERA_DYNAMIC_MDNIE_2), DSI_CAMERA_DYNAMIC_MDNIE_2, 0, NULL}, false, 0},
	{{0, MIPI_DSI_DCS_LONG_WRITE, 0, 0, sizeof(DSI_CAMERA_DYNAMIC_MDNIE_3), DSI_CAMERA_DYNAMIC_MDNIE_3, 0, NULL}, false, 0},
	{{0, MIPI_DSI_DCS_LONG_WRITE, 0, 0, sizeof(level_1_key_off), level_1_key_off, 0, NULL}, true, 0},
};

static struct dsi_cmd_desc DSI_CAMERA_STANDARD_MDNIE[] = {
	{{0, MIPI_DSI_DCS_LONG_WRITE, 0, 0, sizeof(level_1_key_on), level_1_key_on, 0, NULL}, false, 0},
	{{0, MIPI_DSI_DCS_LONG_WRITE, 0, 0, sizeof(DSI_CAMERA_STANDARD_MDNIE_1), DSI_CAMERA_STANDARD_MDNIE_1, 0, NULL}, false, 0},
	{{0, MIPI_DSI_DCS_LONG_WRITE, 0, 0, sizeof(DSI_CAMERA_STANDARD_MDNIE_2), DSI_CAMERA_STANDARD_MDNIE_2, 0, NULL}, false, 0},
	{{0, MIPI_DSI_DCS_LONG_WRITE, 0, 0, sizeof(DSI_CAMERA_STANDARD_MDNIE_3), DSI_CAMERA_STANDARD_MDNIE_3, 0, NULL}, false, 0},
	{{0, MIPI_DSI_DCS_LONG_WRITE, 0, 0, sizeof(level_1_key_off), level_1_key_off, 0, NULL}, true, 0},
};

static struct dsi_cmd_desc DSI_CAMERA_NATURAL_MDNIE[] = {
	{{0, MIPI_DSI_DCS_LONG_WRITE, 0, 0, sizeof(level_1_key_on), level_1_key_on, 0, NULL}, false, 0},
	{{0, MIPI_DSI_DCS_LONG_WRITE, 0, 0, sizeof(DSI_CAMERA_NATURAL_MDNIE_1), DSI_CAMERA_NATURAL_MDNIE_1, 0, NULL}, false, 0},
	{{0, MIPI_DSI_DCS_LONG_WRITE, 0, 0, sizeof(DSI_CAMERA_NATURAL_MDNIE_2), DSI_CAMERA_NATURAL_MDNIE_2, 0, NULL}, false, 0},
	{{0, MIPI_DSI_DCS_LONG_WRITE, 0, 0, sizeof(DSI_CAMERA_NATURAL_MDNIE_3), DSI_CAMERA_NATURAL_MDNIE_3, 0, NULL}, false, 0},
	{{0, MIPI_DSI_DCS_LONG_WRITE, 0, 0, sizeof(level_1_key_off), level_1_key_off, 0, NULL}, true, 0},
};

static struct dsi_cmd_desc DSI_CAMERA_AUTO_MDNIE[] = {
	{{0, MIPI_DSI_DCS_LONG_WRITE, 0, 0, sizeof(level_1_key_on), level_1_key_on, 0, NULL}, false, 0},
	{{0, MIPI_DSI_DCS_LONG_WRITE, 0, 0, sizeof(DSI_CAMERA_AUTO_MDNIE_1), DSI_CAMERA_AUTO_MDNIE_1, 0, NULL}, false, 0},
	{{0, MIPI_DSI_DCS_LONG_WRITE, 0, 0, sizeof(DSI_CAMERA_AUTO_MDNIE_2), DSI_CAMERA_AUTO_MDNIE_2, 0, NULL}, false, 0},
	{{0, MIPI_DSI_DCS_LONG_WRITE, 0, 0, sizeof(DSI_CAMERA_AUTO_MDNIE_3), DSI_CAMERA_AUTO_MDNIE_3, 0, NULL}, false, 0},
	{{0, MIPI_DSI_DCS_LONG_WRITE, 0, 0, sizeof(level_1_key_off), level_1_key_off, 0, NULL}, true, 0},
};

static struct dsi_cmd_desc DSI_GALLERY_DYNAMIC_MDNIE[] = {
	{{0, MIPI_DSI_DCS_LONG_WRITE, 0, 0, sizeof(level_1_key_on), level_1_key_on, 0, NULL}, false, 0},
	{{0, MIPI_DSI_DCS_LONG_WRITE, 0, 0, sizeof(DSI_GALLERY_DYNAMIC_MDNIE_1), DSI_GALLERY_DYNAMIC_MDNIE_1, 0, NULL}, false, 0},
	{{0, MIPI_DSI_DCS_LONG_WRITE, 0, 0, sizeof(DSI_GALLERY_DYNAMIC_MDNIE_2), DSI_GALLERY_DYNAMIC_MDNIE_2, 0, NULL}, false, 0},
	{{0, MIPI_DSI_DCS_LONG_WRITE, 0, 0, sizeof(DSI_GALLERY_DYNAMIC_MDNIE_3), DSI_GALLERY_DYNAMIC_MDNIE_3, 0, NULL}, false, 0},
	{{0, MIPI_DSI_DCS_LONG_WRITE, 0, 0, sizeof(level_1_key_off), level_1_key_off, 0, NULL}, true, 0},
};

static struct dsi_cmd_desc DSI_GALLERY_STANDARD_MDNIE[] = {
	{{0, MIPI_DSI_DCS_LONG_WRITE, 0, 0, sizeof(level_1_key_on), level_1_key_on, 0, NULL}, false, 0},
	{{0, MIPI_DSI_DCS_LONG_WRITE, 0, 0, sizeof(DSI_GALLERY_STANDARD_MDNIE_1), DSI_GALLERY_STANDARD_MDNIE_1, 0, NULL}, false, 0},
	{{0, MIPI_DSI_DCS_LONG_WRITE, 0, 0, sizeof(DSI_GALLERY_STANDARD_MDNIE_2), DSI_GALLERY_STANDARD_MDNIE_2, 0, NULL}, false, 0},
	{{0, MIPI_DSI_DCS_LONG_WRITE, 0, 0, sizeof(DSI_GALLERY_STANDARD_MDNIE_3), DSI_GALLERY_STANDARD_MDNIE_3, 0, NULL}, false, 0},
	{{0, MIPI_DSI_DCS_LONG_WRITE, 0, 0, sizeof(level_1_key_off), level_1_key_off, 0, NULL}, true, 0},
};

static struct dsi_cmd_desc DSI_GALLERY_NATURAL_MDNIE[] = {
	{{0, MIPI_DSI_DCS_LONG_WRITE, 0, 0, sizeof(level_1_key_on), level_1_key_on, 0, NULL}, false, 0},
	{{0, MIPI_DSI_DCS_LONG_WRITE, 0, 0, sizeof(DSI_GALLERY_NATURAL_MDNIE_1), DSI_GALLERY_NATURAL_MDNIE_1, 0, NULL}, false, 0},
	{{0, MIPI_DSI_DCS_LONG_WRITE, 0, 0, sizeof(DSI_GALLERY_NATURAL_MDNIE_2), DSI_GALLERY_NATURAL_MDNIE_2, 0, NULL}, false, 0},
	{{0, MIPI_DSI_DCS_LONG_WRITE, 0, 0, sizeof(DSI_GALLERY_NATURAL_MDNIE_3), DSI_GALLERY_NATURAL_MDNIE_3, 0, NULL}, false, 0},
	{{0, MIPI_DSI_DCS_LONG_WRITE, 0, 0, sizeof(level_1_key_off), level_1_key_off, 0, NULL}, true, 0},
};

static struct dsi_cmd_desc DSI_GALLERY_AUTO_MDNIE[] = {
	{{0, MIPI_DSI_DCS_LONG_WRITE, 0, 0, sizeof(level_1_key_on), level_1_key_on, 0, NULL}, false, 0},
	{{0, MIPI_DSI_DCS_LONG_WRITE, 0, 0, sizeof(DSI_GALLERY_AUTO_MDNIE_1), DSI_GALLERY_AUTO_MDNIE_1, 0, NULL}, false, 0},
	{{0, MIPI_DSI_DCS_LONG_WRITE, 0, 0, sizeof(DSI_GALLERY_AUTO_MDNIE_2), DSI_GALLERY_AUTO_MDNIE_2, 0, NULL}, false, 0},
	{{0, MIPI_DSI_DCS_LONG_WRITE, 0, 0, sizeof(DSI_GALLERY_AUTO_MDNIE_3), DSI_GALLERY_AUTO_MDNIE_3, 0, NULL}, false, 0},
	{{0, MIPI_DSI_DCS_LONG_WRITE, 0, 0, sizeof(level_1_key_off), level_1_key_off, 0, NULL}, true, 0},
};

static struct dsi_cmd_desc DSI_BROWSER_DYNAMIC_MDNIE[] = {
	{{0, MIPI_DSI_DCS_LONG_WRITE, 0, 0, sizeof(level_1_key_on), level_1_key_on, 0, NULL}, false, 0},
	{{0, MIPI_DSI_DCS_LONG_WRITE, 0, 0, sizeof(DSI_BROWSER_DYNAMIC_MDNIE_1), DSI_BROWSER_DYNAMIC_MDNIE_1, 0, NULL}, false, 0},
	{{0, MIPI_DSI_DCS_LONG_WRITE, 0, 0, sizeof(DSI_BROWSER_DYNAMIC_MDNIE_2), DSI_BROWSER_DYNAMIC_MDNIE_2, 0, NULL}, false, 0},
	{{0, MIPI_DSI_DCS_LONG_WRITE, 0, 0, sizeof(DSI_BROWSER_DYNAMIC_MDNIE_3), DSI_BROWSER_DYNAMIC_MDNIE_3, 0, NULL}, false, 0},
	{{0, MIPI_DSI_DCS_LONG_WRITE, 0, 0, sizeof(level_1_key_off), level_1_key_off, 0, NULL}, true, 0},
};

static struct dsi_cmd_desc DSI_BROWSER_STANDARD_MDNIE[] = {
	{{0, MIPI_DSI_DCS_LONG_WRITE, 0, 0, sizeof(level_1_key_on), level_1_key_on, 0, NULL}, false, 0},
	{{0, MIPI_DSI_DCS_LONG_WRITE, 0, 0, sizeof(DSI_BROWSER_STANDARD_MDNIE_1), DSI_BROWSER_STANDARD_MDNIE_1, 0, NULL}, false, 0},
	{{0, MIPI_DSI_DCS_LONG_WRITE, 0, 0, sizeof(DSI_BROWSER_STANDARD_MDNIE_2), DSI_BROWSER_STANDARD_MDNIE_2, 0, NULL}, false, 0},
	{{0, MIPI_DSI_DCS_LONG_WRITE, 0, 0, sizeof(DSI_BROWSER_STANDARD_MDNIE_3), DSI_BROWSER_STANDARD_MDNIE_3, 0, NULL}, false, 0},
	{{0, MIPI_DSI_DCS_LONG_WRITE, 0, 0, sizeof(level_1_key_off), level_1_key_off, 0, NULL}, true, 0},
};

static struct dsi_cmd_desc DSI_BROWSER_NATURAL_MDNIE[] = {
	{{0, MIPI_DSI_DCS_LONG_WRITE, 0, 0, sizeof(level_1_key_on), level_1_key_on, 0, NULL}, false, 0},
	{{0, MIPI_DSI_DCS_LONG_WRITE, 0, 0, sizeof(DSI_BROWSER_NATURAL_MDNIE_1), DSI_BROWSER_NATURAL_MDNIE_1, 0, NULL}, false, 0},
	{{0, MIPI_DSI_DCS_LONG_WRITE, 0, 0, sizeof(DSI_BROWSER_NATURAL_MDNIE_2), DSI_BROWSER_NATURAL_MDNIE_2, 0, NULL}, false, 0},
	{{0, MIPI_DSI_DCS_LONG_WRITE, 0, 0, sizeof(DSI_BROWSER_NATURAL_MDNIE_3), DSI_BROWSER_NATURAL_MDNIE_3, 0, NULL}, false, 0},
	{{0, MIPI_DSI_DCS_LONG_WRITE, 0, 0, sizeof(level_1_key_off), level_1_key_off, 0, NULL}, true, 0},
};

static struct dsi_cmd_desc DSI_BROWSER_AUTO_MDNIE[] = {
	{{0, MIPI_DSI_DCS_LONG_WRITE, 0, 0, sizeof(level_1_key_on), level_1_key_on, 0, NULL}, false, 0},
	{{0, MIPI_DSI_DCS_LONG_WRITE, 0, 0, sizeof(DSI_BROWSER_AUTO_MDNIE_1), DSI_BROWSER_AUTO_MDNIE_1, 0, NULL}, false, 0},
	{{0, MIPI_DSI_DCS_LONG_WRITE, 0, 0, sizeof(DSI_BROWSER_AUTO_MDNIE_2), DSI_BROWSER_AUTO_MDNIE_2, 0, NULL}, false, 0},
	{{0, MIPI_DSI_DCS_LONG_WRITE, 0, 0, sizeof(DSI_BROWSER_AUTO_MDNIE_3), DSI_BROWSER_AUTO_MDNIE_3, 0, NULL}, false, 0},
	{{0, MIPI_DSI_DCS_LONG_WRITE, 0, 0, sizeof(level_1_key_off), level_1_key_off, 0, NULL}, true, 0},
};

static struct dsi_cmd_desc DSI_EBOOK_DYNAMIC_MDNIE[] = {
	{{0, MIPI_DSI_DCS_LONG_WRITE, 0, 0, sizeof(level_1_key_on), level_1_key_on, 0, NULL}, false, 0},
	{{0, MIPI_DSI_DCS_LONG_WRITE, 0, 0, sizeof(DSI_EBOOK_DYNAMIC_MDNIE_1), DSI_EBOOK_DYNAMIC_MDNIE_1, 0, NULL}, false, 0},
	{{0, MIPI_DSI_DCS_LONG_WRITE, 0, 0, sizeof(DSI_EBOOK_DYNAMIC_MDNIE_2), DSI_EBOOK_DYNAMIC_MDNIE_2, 0, NULL}, false, 0},
	{{0, MIPI_DSI_DCS_LONG_WRITE, 0, 0, sizeof(DSI_EBOOK_DYNAMIC_MDNIE_3), DSI_EBOOK_DYNAMIC_MDNIE_3, 0, NULL}, false, 0},
	{{0, MIPI_DSI_DCS_LONG_WRITE, 0, 0, sizeof(level_1_key_off), level_1_key_off, 0, NULL}, true, 0},
};

static struct dsi_cmd_desc DSI_EBOOK_STANDARD_MDNIE[] = {
	{{0, MIPI_DSI_DCS_LONG_WRITE, 0, 0, sizeof(level_1_key_on), level_1_key_on, 0, NULL}, false, 0},
	{{0, MIPI_DSI_DCS_LONG_WRITE, 0, 0, sizeof(DSI_EBOOK_STANDARD_MDNIE_1), DSI_EBOOK_STANDARD_MDNIE_1, 0, NULL}, false, 0},
	{{0, MIPI_DSI_DCS_LONG_WRITE, 0, 0, sizeof(DSI_EBOOK_STANDARD_MDNIE_2), DSI_EBOOK_STANDARD_MDNIE_2, 0, NULL}, false, 0},
	{{0, MIPI_DSI_DCS_LONG_WRITE, 0, 0, sizeof(DSI_EBOOK_STANDARD_MDNIE_3), DSI_EBOOK_STANDARD_MDNIE_3, 0, NULL}, false, 0},
	{{0, MIPI_DSI_DCS_LONG_WRITE, 0, 0, sizeof(level_1_key_off), level_1_key_off, 0, NULL}, true, 0},
};

static struct dsi_cmd_desc DSI_EBOOK_NATURAL_MDNIE[] = {
	{{0, MIPI_DSI_DCS_LONG_WRITE, 0, 0, sizeof(level_1_key_on), level_1_key_on, 0, NULL}, false, 0},
	{{0, MIPI_DSI_DCS_LONG_WRITE, 0, 0, sizeof(DSI_EBOOK_NATURAL_MDNIE_1), DSI_EBOOK_NATURAL_MDNIE_1, 0, NULL}, false, 0},
	{{0, MIPI_DSI_DCS_LONG_WRITE, 0, 0, sizeof(DSI_EBOOK_NATURAL_MDNIE_2), DSI_EBOOK_NATURAL_MDNIE_2, 0, NULL}, false, 0},
	{{0, MIPI_DSI_DCS_LONG_WRITE, 0, 0, sizeof(DSI_EBOOK_NATURAL_MDNIE_3), DSI_EBOOK_NATURAL_MDNIE_3, 0, NULL}, false, 0},
	{{0, MIPI_DSI_DCS_LONG_WRITE, 0, 0, sizeof(level_1_key_off), level_1_key_off, 0, NULL}, true, 0},
};

static struct dsi_cmd_desc DSI_EBOOK_AUTO_MDNIE[] = {
	{{0, MIPI_DSI_DCS_LONG_WRITE, 0, 0, sizeof(level_1_key_on), level_1_key_on, 0, NULL}, false, 0},
	{{0, MIPI_DSI_DCS_LONG_WRITE, 0, 0, sizeof(DSI_EBOOK_AUTO_MDNIE_1), DSI_EBOOK_AUTO_MDNIE_1, 0, NULL}, false, 0},
	{{0, MIPI_DSI_DCS_LONG_WRITE, 0, 0, sizeof(DSI_EBOOK_AUTO_MDNIE_2), DSI_EBOOK_AUTO_MDNIE_2, 0, NULL}, false, 0},
	{{0, MIPI_DSI_DCS_LONG_WRITE, 0, 0, sizeof(DSI_EBOOK_AUTO_MDNIE_3), DSI_EBOOK_AUTO_MDNIE_3, 0, NULL}, false, 0},
	{{0, MIPI_DSI_DCS_LONG_WRITE, 0, 0, sizeof(level_1_key_off), level_1_key_off, 0, NULL}, true, 0},
};

static struct dsi_cmd_desc DSI_EMAIL_AUTO_MDNIE[] = {
	{{0, MIPI_DSI_DCS_LONG_WRITE, 0, 0, sizeof(level_1_key_on), level_1_key_on, 0, NULL}, false, 0},
	{{0, MIPI_DSI_DCS_LONG_WRITE, 0, 0, sizeof(DSI_EMAIL_AUTO_MDNIE_1), DSI_EMAIL_AUTO_MDNIE_1, 0, NULL}, false, 0},
	{{0, MIPI_DSI_DCS_LONG_WRITE, 0, 0, sizeof(DSI_EMAIL_AUTO_MDNIE_2), DSI_EMAIL_AUTO_MDNIE_2, 0, NULL}, false, 0},
	{{0, MIPI_DSI_DCS_LONG_WRITE, 0, 0, sizeof(DSI_EMAIL_AUTO_MDNIE_3), DSI_EMAIL_AUTO_MDNIE_3, 0, NULL}, false, 0},
	{{0, MIPI_DSI_DCS_LONG_WRITE, 0, 0, sizeof(level_1_key_off), level_1_key_off, 0, NULL}, true, 0},
};

static struct dsi_cmd_desc DSI_GAME_LOW_MDNIE[] = {
	{{0, MIPI_DSI_DCS_LONG_WRITE, 0, 0, sizeof(level_1_key_on), level_1_key_on, 0, NULL}, false, 0},
	{{0, MIPI_DSI_DCS_LONG_WRITE, 0, 0, sizeof(DSI_BYPASS_MDNIE_1), DSI_BYPASS_MDNIE_1, 0, NULL}, false, 0},
	{{0, MIPI_DSI_DCS_LONG_WRITE, 0, 0, sizeof(DSI_BYPASS_MDNIE_2), DSI_BYPASS_MDNIE_2, 0, NULL}, false, 0},
	{{0, MIPI_DSI_DCS_LONG_WRITE, 0, 0, sizeof(DSI_BYPASS_MDNIE_3), DSI_BYPASS_MDNIE_3, 0, NULL}, false, 0},
	{{0, MIPI_DSI_DCS_LONG_WRITE, 0, 0, sizeof(level_1_key_off), level_1_key_off, 0, NULL}, true, 0},
};

static struct dsi_cmd_desc DSI_GAME_MID_MDNIE[] = {
	{{0, MIPI_DSI_DCS_LONG_WRITE, 0, 0, sizeof(level_1_key_on), level_1_key_on, 0, NULL}, false, 0},
	{{0, MIPI_DSI_DCS_LONG_WRITE, 0, 0, sizeof(DSI_BYPASS_MDNIE_1), DSI_BYPASS_MDNIE_1, 0, NULL}, false, 0},
	{{0, MIPI_DSI_DCS_LONG_WRITE, 0, 0, sizeof(DSI_BYPASS_MDNIE_2), DSI_BYPASS_MDNIE_2, 0, NULL}, false, 0},
	{{0, MIPI_DSI_DCS_LONG_WRITE, 0, 0, sizeof(DSI_BYPASS_MDNIE_3), DSI_BYPASS_MDNIE_3, 0, NULL}, false, 0},
	{{0, MIPI_DSI_DCS_LONG_WRITE, 0, 0, sizeof(level_1_key_off), level_1_key_off, 0, NULL}, true, 0},
};

static struct dsi_cmd_desc DSI_GAME_HIGH_MDNIE[] = {
	{{0, MIPI_DSI_DCS_LONG_WRITE, 0, 0, sizeof(level_1_key_on), level_1_key_on, 0, NULL}, false, 0},
	{{0, MIPI_DSI_DCS_LONG_WRITE, 0, 0, sizeof(DSI_BYPASS_MDNIE_1), DSI_BYPASS_MDNIE_1, 0, NULL}, false, 0},
	{{0, MIPI_DSI_DCS_LONG_WRITE, 0, 0, sizeof(DSI_BYPASS_MDNIE_2), DSI_BYPASS_MDNIE_2, 0, NULL}, false, 0},
	{{0, MIPI_DSI_DCS_LONG_WRITE, 0, 0, sizeof(DSI_BYPASS_MDNIE_3), DSI_BYPASS_MDNIE_3, 0, NULL}, false, 0},
	{{0, MIPI_DSI_DCS_LONG_WRITE, 0, 0, sizeof(level_1_key_off), level_1_key_off, 0, NULL}, true, 0},
};

static struct dsi_cmd_desc DSI_TDMB_DYNAMIC_MDNIE[] = {
	{{0, MIPI_DSI_DCS_LONG_WRITE, 0, 0, sizeof(level_1_key_on), level_1_key_on, 0, NULL}, false, 0},
	{{0, MIPI_DSI_DCS_LONG_WRITE, 0, 0, sizeof(DSI_TDMB_DYNAMIC_MDNIE_1), DSI_TDMB_DYNAMIC_MDNIE_1, 0, NULL}, false, 0},
	{{0, MIPI_DSI_DCS_LONG_WRITE, 0, 0, sizeof(DSI_TDMB_DYNAMIC_MDNIE_2), DSI_TDMB_DYNAMIC_MDNIE_2, 0, NULL}, false, 0},
	{{0, MIPI_DSI_DCS_LONG_WRITE, 0, 0, sizeof(DSI_TDMB_DYNAMIC_MDNIE_3), DSI_TDMB_DYNAMIC_MDNIE_3, 0, NULL}, false, 0},
	{{0, MIPI_DSI_DCS_LONG_WRITE, 0, 0, sizeof(level_1_key_off), level_1_key_off, 0, NULL}, true, 0},
};

static struct dsi_cmd_desc DSI_TDMB_STANDARD_MDNIE[] = {
	{{0, MIPI_DSI_DCS_LONG_WRITE, 0, 0, sizeof(level_1_key_on), level_1_key_on, 0, NULL}, false, 0},
	{{0, MIPI_DSI_DCS_LONG_WRITE, 0, 0, sizeof(DSI_TDMB_STANDARD_MDNIE_1), DSI_TDMB_STANDARD_MDNIE_1, 0, NULL}, false, 0},
	{{0, MIPI_DSI_DCS_LONG_WRITE, 0, 0, sizeof(DSI_TDMB_STANDARD_MDNIE_2), DSI_TDMB_STANDARD_MDNIE_2, 0, NULL}, false, 0},
	{{0, MIPI_DSI_DCS_LONG_WRITE, 0, 0, sizeof(DSI_TDMB_STANDARD_MDNIE_3), DSI_TDMB_STANDARD_MDNIE_3, 0, NULL}, false, 0},
	{{0, MIPI_DSI_DCS_LONG_WRITE, 0, 0, sizeof(level_1_key_off), level_1_key_off, 0, NULL}, true, 0},
};

static struct dsi_cmd_desc DSI_TDMB_NATURAL_MDNIE[] = {
	{{0, MIPI_DSI_DCS_LONG_WRITE, 0, 0, sizeof(level_1_key_on), level_1_key_on, 0, NULL}, false, 0},
	{{0, MIPI_DSI_DCS_LONG_WRITE, 0, 0, sizeof(DSI_TDMB_NATURAL_MDNIE_1), DSI_TDMB_NATURAL_MDNIE_1, 0, NULL}, false, 0},
	{{0, MIPI_DSI_DCS_LONG_WRITE, 0, 0, sizeof(DSI_TDMB_NATURAL_MDNIE_2), DSI_TDMB_NATURAL_MDNIE_2, 0, NULL}, false, 0},
	{{0, MIPI_DSI_DCS_LONG_WRITE, 0, 0, sizeof(DSI_TDMB_NATURAL_MDNIE_3), DSI_TDMB_NATURAL_MDNIE_3, 0, NULL}, false, 0},
	{{0, MIPI_DSI_DCS_LONG_WRITE, 0, 0, sizeof(level_1_key_off), level_1_key_off, 0, NULL}, true, 0},
};

static struct dsi_cmd_desc DSI_TDMB_AUTO_MDNIE[] = {
	{{0, MIPI_DSI_DCS_LONG_WRITE, 0, 0, sizeof(level_1_key_on), level_1_key_on, 0, NULL}, false, 0},
	{{0, MIPI_DSI_DCS_LONG_WRITE, 0, 0, sizeof(DSI_TDMB_AUTO_MDNIE_1), DSI_TDMB_AUTO_MDNIE_1, 0, NULL}, false, 0},
	{{0, MIPI_DSI_DCS_LONG_WRITE, 0, 0, sizeof(DSI_TDMB_AUTO_MDNIE_2), DSI_TDMB_AUTO_MDNIE_2, 0, NULL}, false, 0},
	{{0, MIPI_DSI_DCS_LONG_WRITE, 0, 0, sizeof(DSI_TDMB_AUTO_MDNIE_3), DSI_TDMB_AUTO_MDNIE_3, 0, NULL}, false, 0},
	{{0, MIPI_DSI_DCS_LONG_WRITE, 0, 0, sizeof(level_1_key_off), level_1_key_off, 0, NULL}, true, 0},
};

static struct dsi_cmd_desc *mdnie_tune_value_dsi[MAX_APP_MODE][MAX_MODE][MAX_OUTDOOR_MODE] = {
	/*
		DYNAMIC_MODE
		STANDARD_MODE
		NATURAL_MODE
		MOVIE_MODE
		AUTO_MODE
		READING_MODE
	*/
	// UI_APP
	{
		{DSI_UI_DYNAMIC_MDNIE,	NULL},
		{DSI_UI_STANDARD_MDNIE,	NULL},
		{DSI_UI_NATURAL_MDNIE,	NULL},
		{NULL,	NULL},
		{DSI_UI_AUTO_MDNIE,	NULL},
		{DSI_EBOOK_AUTO_MDNIE,	NULL},
	},
	// VIDEO_APP
	{
		{DSI_VIDEO_DYNAMIC_MDNIE,	NULL},
		{DSI_VIDEO_STANDARD_MDNIE,	NULL},
		{DSI_VIDEO_NATURAL_MDNIE,	NULL},
		{NULL,	NULL},
		{DSI_VIDEO_AUTO_MDNIE,	NULL},
		{DSI_EBOOK_AUTO_MDNIE,	NULL},
	},
	// VIDEO_WARM_APP
	{
		{NULL,	NULL},
		{NULL,	NULL},
		{NULL,	NULL},
		{NULL,	NULL},
		{NULL,	NULL},
		{NULL,	NULL},
	},
	// VIDEO_COLD_APP
	{
		{NULL,	NULL},
		{NULL,	NULL},
		{NULL,	NULL},
		{NULL,	NULL},
		{NULL,	NULL},
		{NULL,	NULL},
	},
	// CAMERA_APP
	{
		{DSI_CAMERA_DYNAMIC_MDNIE,	NULL},
		{DSI_CAMERA_STANDARD_MDNIE,	NULL},
		{DSI_CAMERA_NATURAL_MDNIE,	NULL},
		{NULL,	NULL},
		{DSI_CAMERA_AUTO_MDNIE,	NULL},
		{DSI_EBOOK_AUTO_MDNIE,	NULL},
	},
	// NAVI_APP
	{
		{NULL,	NULL},
		{NULL,	NULL},
		{NULL,	NULL},
		{NULL,	NULL},
		{NULL,	NULL},
		{NULL,	NULL},
	},
	// GALLERY_APP
	{
		{DSI_GALLERY_DYNAMIC_MDNIE,	NULL},
		{DSI_GALLERY_STANDARD_MDNIE,	NULL},
		{DSI_GALLERY_NATURAL_MDNIE,	NULL},
		{NULL,	NULL},
		{DSI_GALLERY_AUTO_MDNIE,	NULL},
		{DSI_EBOOK_AUTO_MDNIE,	NULL},
	},
	// VT_APP
	{
		{NULL,	NULL},
		{NULL,	NULL},
		{NULL,	NULL},
		{NULL,	NULL},
		{NULL,	NULL},
		{NULL,	NULL},
	},
	// BROWSER_APP
	{
		{DSI_BROWSER_DYNAMIC_MDNIE,	NULL},
		{DSI_BROWSER_STANDARD_MDNIE,	NULL},
		{DSI_BROWSER_NATURAL_MDNIE,	NULL},
		{NULL,	NULL},
		{DSI_BROWSER_AUTO_MDNIE,	NULL},
		{DSI_EBOOK_AUTO_MDNIE,	NULL},
	},
	// eBOOK_APP
	{
		{DSI_EBOOK_DYNAMIC_MDNIE,	NULL},
		{DSI_EBOOK_STANDARD_MDNIE,	NULL},
		{DSI_EBOOK_NATURAL_MDNIE,	NULL},
		{NULL,	NULL},
		{DSI_EBOOK_AUTO_MDNIE,	NULL},
		{DSI_EBOOK_AUTO_MDNIE,	NULL},
	},
	// EMAIL_APP
	{
		{DSI_EMAIL_AUTO_MDNIE,	NULL},
		{DSI_EMAIL_AUTO_MDNIE,	NULL},
		{DSI_EMAIL_AUTO_MDNIE,	NULL},
		{NULL,	NULL},
		{DSI_EMAIL_AUTO_MDNIE,	NULL},
		{DSI_EBOOK_AUTO_MDNIE,	NULL},
	},
	// GAME_LOW_APP
	{
		{DSI_GAME_LOW_MDNIE,	NULL},
		{DSI_GAME_LOW_MDNIE,	NULL},
		{DSI_GAME_LOW_MDNIE,	NULL},
		{NULL,	NULL},
		{DSI_GAME_LOW_MDNIE,	NULL},
		{DSI_GAME_LOW_MDNIE,	NULL},
	},
	// GAME_MID_APP
	{
		{DSI_GAME_MID_MDNIE,	NULL},
		{DSI_GAME_MID_MDNIE,	NULL},
		{DSI_GAME_MID_MDNIE,	NULL},
		{NULL,	NULL},
		{DSI_GAME_MID_MDNIE,	NULL},
		{DSI_GAME_MID_MDNIE,	NULL},
	},
	// GAME_HIGH_APP
	{
		{DSI_GAME_HIGH_MDNIE,	NULL},
		{DSI_GAME_HIGH_MDNIE,	NULL},
		{DSI_GAME_HIGH_MDNIE,	NULL},
		{NULL,	NULL},
		{DSI_GAME_HIGH_MDNIE,	NULL},
		{DSI_GAME_HIGH_MDNIE,	NULL},
	},
	// VIDEO_ENHANCER_APP
	{
		{DSI_VIDEO_ENHANCER_D65_MDNIE,	NULL},
		{DSI_VIDEO_ENHANCER_D65_MDNIE,	NULL},
		{DSI_VIDEO_ENHANCER_D65_MDNIE,	NULL},
		{NULL,	NULL},
		{DSI_VIDEO_ENHANCER_AUTO_MDNIE, NULL},
		{DSI_EBOOK_AUTO_MDNIE,	NULL},
	},
	// VIDEO_ENHANCER_THIRD_APP
	{
		{DSI_VIDEO_ENHANCER_THIRD_D65_MDNIE,	NULL},
		{DSI_VIDEO_ENHANCER_THIRD_D65_MDNIE,	NULL},
		{DSI_VIDEO_ENHANCER_THIRD_D65_MDNIE,	NULL},
		{NULL,	NULL},
		{DSI_VIDEO_ENHANCER_THIRD_AUTO_MDNIE,	NULL},
		{DSI_EBOOK_AUTO_MDNIE,	NULL},
	},
	// TDMB_APP
	{
		{DSI_TDMB_DYNAMIC_MDNIE,       NULL},
		{DSI_TDMB_STANDARD_MDNIE,      NULL},
		{DSI_TDMB_NATURAL_MDNIE,       NULL},
		{NULL,         NULL},
		{DSI_TDMB_AUTO_MDNIE,          NULL},
		{DSI_EBOOK_AUTO_MDNIE,	NULL},
	},
};

static struct dsi_cmd_desc DSI_HMT_COLOR_TEMP_3000K_MDNIE[] = {
	{{0, MIPI_DSI_DCS_LONG_WRITE, 0, 0, sizeof(level_1_key_on), level_1_key_on, 0, NULL}, false, 0},
	{{0, MIPI_DSI_DCS_LONG_WRITE, 0, 0, sizeof(DSI_HMT_COLOR_TEMP_3000K_MDNIE_1), DSI_HMT_COLOR_TEMP_3000K_MDNIE_1, 0, NULL}, false, 0},
	{{0, MIPI_DSI_DCS_LONG_WRITE, 0, 0, sizeof(DSI_HMT_COLOR_TEMP_3000K_MDNIE_2), DSI_HMT_COLOR_TEMP_3000K_MDNIE_2, 0, NULL}, false, 0},
	{{0, MIPI_DSI_DCS_LONG_WRITE, 0, 0, sizeof(DSI_HMT_COLOR_TEMP_3000K_MDNIE_3), DSI_HMT_COLOR_TEMP_3000K_MDNIE_3, 0, NULL}, false, 0},
	{{0, MIPI_DSI_DCS_LONG_WRITE, 0, 0, sizeof(level_1_key_off), level_1_key_off, 0, NULL}, true, 0},
};

static struct dsi_cmd_desc DSI_HMT_COLOR_TEMP_4000K_MDNIE[] = {
	{{0, MIPI_DSI_DCS_LONG_WRITE, 0, 0, sizeof(level_1_key_on), level_1_key_on, 0, NULL}, false, 0},
	{{0, MIPI_DSI_DCS_LONG_WRITE, 0, 0, sizeof(DSI_HMT_COLOR_TEMP_4000K_MDNIE_1), DSI_HMT_COLOR_TEMP_4000K_MDNIE_1, 0, NULL}, false, 0},
	{{0, MIPI_DSI_DCS_LONG_WRITE, 0, 0, sizeof(DSI_HMT_COLOR_TEMP_4000K_MDNIE_2), DSI_HMT_COLOR_TEMP_4000K_MDNIE_2, 0, NULL}, false, 0},
	{{0, MIPI_DSI_DCS_LONG_WRITE, 0, 0, sizeof(DSI_HMT_COLOR_TEMP_4000K_MDNIE_3), DSI_HMT_COLOR_TEMP_4000K_MDNIE_3, 0, NULL}, false, 0},
	{{0, MIPI_DSI_DCS_LONG_WRITE, 0, 0, sizeof(level_1_key_off), level_1_key_off, 0, NULL}, true, 0},
};

static struct dsi_cmd_desc DSI_HMT_COLOR_TEMP_5000K_MDNIE[] = {
	{{0, MIPI_DSI_DCS_LONG_WRITE, 0, 0, sizeof(level_1_key_on), level_1_key_on, 0, NULL}, false, 0},
	{{0, MIPI_DSI_DCS_LONG_WRITE, 0, 0, sizeof(DSI_HMT_COLOR_TEMP_5000K_MDNIE_1), DSI_HMT_COLOR_TEMP_5000K_MDNIE_1, 0, NULL}, false, 0},
	{{0, MIPI_DSI_DCS_LONG_WRITE, 0, 0, sizeof(DSI_HMT_COLOR_TEMP_5000K_MDNIE_2), DSI_HMT_COLOR_TEMP_5000K_MDNIE_2, 0, NULL}, false, 0},
	{{0, MIPI_DSI_DCS_LONG_WRITE, 0, 0, sizeof(DSI_HMT_COLOR_TEMP_5000K_MDNIE_3), DSI_HMT_COLOR_TEMP_5000K_MDNIE_3, 0, NULL}, false, 0},
	{{0, MIPI_DSI_DCS_LONG_WRITE, 0, 0, sizeof(level_1_key_off), level_1_key_off, 0, NULL}, true, 0},
};

static struct dsi_cmd_desc DSI_HMT_COLOR_TEMP_6500K_MDNIE[] = {
	{{0, MIPI_DSI_DCS_LONG_WRITE, 0, 0, sizeof(level_1_key_on), level_1_key_on, 0, NULL}, false, 0},
	{{0, MIPI_DSI_DCS_LONG_WRITE, 0, 0, sizeof(DSI_HMT_COLOR_TEMP_6500K_MDNIE_1), DSI_HMT_COLOR_TEMP_6500K_MDNIE_1, 0, NULL}, false, 0},
	{{0, MIPI_DSI_DCS_LONG_WRITE, 0, 0, sizeof(DSI_HMT_COLOR_TEMP_6500K_MDNIE_2), DSI_HMT_COLOR_TEMP_6500K_MDNIE_2, 0, NULL}, false, 0},
	{{0, MIPI_DSI_DCS_LONG_WRITE, 0, 0, sizeof(DSI_HMT_COLOR_TEMP_6500K_MDNIE_3), DSI_HMT_COLOR_TEMP_6500K_MDNIE_3, 0, NULL}, false, 0},
	{{0, MIPI_DSI_DCS_LONG_WRITE, 0, 0, sizeof(level_1_key_off), level_1_key_off, 0, NULL}, true, 0},
};

static struct dsi_cmd_desc DSI_HMT_COLOR_TEMP_7500K_MDNIE[] = {
	{{0, MIPI_DSI_DCS_LONG_WRITE, 0, 0, sizeof(level_1_key_on), level_1_key_on, 0, NULL}, false, 0},
	{{0, MIPI_DSI_DCS_LONG_WRITE, 0, 0, sizeof(DSI_HMT_COLOR_TEMP_7500K_MDNIE_1), DSI_HMT_COLOR_TEMP_7500K_MDNIE_1, 0, NULL}, false, 0},
	{{0, MIPI_DSI_DCS_LONG_WRITE, 0, 0, sizeof(DSI_HMT_COLOR_TEMP_7500K_MDNIE_2), DSI_HMT_COLOR_TEMP_7500K_MDNIE_2, 0, NULL}, false, 0},
	{{0, MIPI_DSI_DCS_LONG_WRITE, 0, 0, sizeof(DSI_HMT_COLOR_TEMP_7500K_MDNIE_3), DSI_HMT_COLOR_TEMP_7500K_MDNIE_3, 0, NULL}, false, 0},
	{{0, MIPI_DSI_DCS_LONG_WRITE, 0, 0, sizeof(level_1_key_off), level_1_key_off, 0, NULL}, true, 0},
};

static struct dsi_cmd_desc *hmt_color_temperature_tune_value_dsi[HMT_COLOR_TEMP_MAX] = {
	/*
		HMT_COLOR_TEMP_3000K, // 3000K
		HMT_COLOR_TEMP_4000K, // 4000K
		HMT_COLOR_TEMP_5000K, // 5000K
		HMT_COLOR_TEMP_6500K, // 6500K
		HMT_COLOR_TEMP_7500K, // 7500K
	*/
	NULL,
	DSI_HMT_COLOR_TEMP_3000K_MDNIE,
	DSI_HMT_COLOR_TEMP_4000K_MDNIE,
	DSI_HMT_COLOR_TEMP_5000K_MDNIE,
	DSI_HMT_COLOR_TEMP_6500K_MDNIE,
	DSI_HMT_COLOR_TEMP_7500K_MDNIE,
};

static struct dsi_cmd_desc *light_notification_tune_value_dsi[LIGHT_NOTIFICATION_MAX] = {
	NULL,
	DSI_LIGHT_NOTIFICATION_MDNIE,
};

static struct dsi_cmd_desc *hdr_tune_value_dsi[HDR_MAX] = {
	NULL,
	DSI_HDR_VIDEO_1_MDNIE,
	DSI_HDR_VIDEO_2_MDNIE,
	DSI_HDR_VIDEO_3_MDNIE,
};

#define DSI_RGB_SENSOR_MDNIE_1_SIZE ARRAY_SIZE(DSI_RGB_SENSOR_MDNIE_1)
#define DSI_RGB_SENSOR_MDNIE_2_SIZE ARRAY_SIZE(DSI_RGB_SENSOR_MDNIE_2)
#define DSI_RGB_SENSOR_MDNIE_3_SIZE ARRAY_SIZE(DSI_RGB_SENSOR_MDNIE_3)

#endif
