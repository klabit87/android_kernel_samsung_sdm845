/*
 * =================================================================
 *
 *
 *	Description:  samsung display common file
 *
 *	Author: wu.deng
 *	Company:  Samsung Electronics
 *
 * ================================================================
 */
/*
<one line to give the program's name and a brief idea of what it does.>
Copyright (C) 2017, Samsung Electronics. All rights reserved.

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
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
 * 02110-1301, USA.
 *
*/
#ifndef _SS_DSI_MDNIE_S6E3FA5_AMS420QW01_H_
#define _SS_DSI_MDNIE_S6E3FA5_AMS420QW01_H_

#include "ss_dsi_mdnie_lite_common.h"

#define MDNIE_COLOR_BLINDE_CMD_OFFSET 32

#define ADDRESS_SCR_WHITE_RED   0x32
#define ADDRESS_SCR_WHITE_GREEN 0x34
#define ADDRESS_SCR_WHITE_BLUE  0x36

#define MDNIE_STEP1_INDEX 1
#define MDNIE_STEP2_INDEX 2
#define MDNIE_STEP3_INDEX 3

#define MDNIE_TRANS_DIMMING_DATA_INDEX	13

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
	0x00, 0xff, 0xf9, 0x00, 0xed, 0x00, 0xff, 0x00, 0x00, 0xf9, 0xed, 0x00, 0xff, 0x00, 0xf9, 0x00, 0x00, 0xed, 0xff, 0x00, 0xf9, 0x00, 0xed, 0x00, /* 6500K */
	0x00, 0xff, 0xf6, 0x00, 0xe5, 0x00, 0xff, 0x00, 0x00, 0xf6, 0xe5, 0x00, 0xff, 0x00, 0xf6, 0x00, 0x00, 0xe5, 0xff, 0x00, 0xf6, 0x00, 0xe5, 0x00, /* 6100K */
	0x00, 0xff, 0xf2, 0x00, 0xdb, 0x00, 0xff, 0x00, 0x00, 0xf2, 0xdb, 0x00, 0xff, 0x00, 0xf2, 0x00, 0x00, 0xdb, 0xff, 0x00, 0xf2, 0x00, 0xdb, 0x00, /* 5700K */
	0x00, 0xff, 0xee, 0x00, 0xd1, 0x00, 0xff, 0x00, 0x00, 0xee, 0xd1, 0x00, 0xff, 0x00, 0xee, 0x00, 0x00, 0xd1, 0xff, 0x00, 0xee, 0x00, 0xd1, 0x00, /* 5300K */
	0x00, 0xff, 0xea, 0x00, 0xc6, 0x00, 0xff, 0x00, 0x00, 0xea, 0xc6, 0x00, 0xff, 0x00, 0xea, 0x00, 0x00, 0xc6, 0xff, 0x00, 0xea, 0x00, 0xc6, 0x00, /* 4900K */
	0x00, 0xff, 0xe4, 0x00, 0xb7, 0x00, 0xff, 0x00, 0x00, 0xe4, 0xb7, 0x00, 0xff, 0x00, 0xe4, 0x00, 0x00, 0xb7, 0xff, 0x00, 0xe4, 0x00, 0xb7, 0x00, /* 4500K */
	0x00, 0xff, 0xde, 0x00, 0xa7, 0x00, 0xff, 0x00, 0x00, 0xde, 0xa7, 0x00, 0xff, 0x00, 0xde, 0x00, 0x00, 0xa7, 0xff, 0x00, 0xde, 0x00, 0xa7, 0x00, /* 4100K */
	0x00, 0xff, 0xd7, 0x00, 0x95, 0x00, 0xff, 0x00, 0x00, 0xd7, 0x95, 0x00, 0xff, 0x00, 0xd7, 0x00, 0x00, 0x95, 0xff, 0x00, 0xd7, 0x00, 0x95, 0x00, /* 3700K */
	0x00, 0xff, 0xce, 0x00, 0x82, 0x00, 0xff, 0x00, 0x00, 0xce, 0x82, 0x00, 0xff, 0x00, 0xce, 0x00, 0x00, 0x82, 0xff, 0x00, 0xce, 0x00, 0x82, 0x00, /* 3300K */
	0x00, 0xff, 0xc3, 0x00, 0x6d, 0x00, 0xff, 0x00, 0x00, 0xc3, 0x6d, 0x00, 0xff, 0x00, 0xc3, 0x00, 0x00, 0x6d, 0xff, 0x00, 0xc3, 0x00, 0x6d, 0x00, /* 2900K */
	0x00, 0xff, 0xb2, 0x00, 0x56, 0x00, 0xff, 0x00, 0x00, 0xb2, 0x56, 0x00, 0xff, 0x00, 0xb2, 0x00, 0x00, 0x56, 0xff, 0x00, 0xb2, 0x00, 0x56, 0x00, /* 2500K */
};

static char DSI_BYPASS_MDNIE_1[] = {
	//start
	0xDF,
	0x00, //linear_on ascr_skin_on 00
	0x6a, //ascr_skin_cb
	0x9a, //ascr_skin_cr
	0x25, //ascr_dist_up
	0x1a, //ascr_dist_down
	0x2a, //ascr_dist_left
	0x16, //ascr_dist_right
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
};

static char DSI_BYPASS_MDNIE_2[] = {
	0xDE,
	0x00, //de cs gamma 000
	0x00, //de_gain 10
	0x00,
	0x02, //de_maxplus 11
	0x00,
	0x02, //de_maxminus 11
	0x00,
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
	0x00, //curve_offset
	0x00,
	0x00, //curve_low_x
	0x00, //curve_low_y
};

static char DSI_BYPASS_MDNIE_3[] = {
	0xDD,
	0x01, //mdnie_en
	0x00, //mask 000
	0x00, //ascr algo 0000
	0x00, //roi_en
	0x00, //roi_sx
	0x00, //roi_sx
	0x00, //roi_sy
	0x00, //roi_sy
	0x00, //roi_ex
	0x00, //roi_ex
	0x00, //roi_ey
	0x00, //roi_ey
	0x00, //trans_on trans_slope 0 0000
	0x00, //trans_interval
	//end
};

static char DSI_NEGATIVE_MDNIE_1[] = {
	//start
	0xDF,
	0x00, //linear_on ascr_skin_on 00
	0x6a, //ascr_skin_cb
	0x9a, //ascr_skin_cr
	0x25, //ascr_dist_up
	0x1a, //ascr_dist_down
	0x2a, //ascr_dist_left
	0x16, //ascr_dist_right
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
	0x00, //ascr_skin_Wr
	0x00, //ascr_skin_Wg
	0x00, //ascr_skin_Wb
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
	0x00, //ascr_Wr
	0xff, //ascr_Kr
	0x00, //ascr_Wg
	0xff, //ascr_Kg
	0x00, //ascr_Wb
	0xff, //ascr_Kb
};

static char DSI_NEGATIVE_MDNIE_2[] = {
	0xDE,
	0x07, //de cs gamma 000
	0x00, //de_gain 10
	0x00,
	0x02, //de_maxplus 11
	0x00,
	0x02, //de_maxminus 11
	0x00,
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
	0x00, //curve_offset
	0x00,
	0x00, //curve_low_x
	0x00, //curve_low_y
};

static char DSI_NEGATIVE_MDNIE_3[] = {
	0xDD,
	0x01, //mdnie_en
	0x00, //mask 000
	0x0f, //ascr algo 0000
	0x00, //roi_en
	0x00, //roi_sx
	0x00, //roi_sx
	0x00, //roi_sy
	0x00, //roi_sy
	0x00, //roi_ex
	0x00, //roi_ex
	0x00, //roi_ey
	0x00, //roi_ey
	0x14, //trans_on trans_slope 0 0000
	0x01, //trans_interval
	//end
};

static char DSI_GRAYSCALE_MDNIE_1[] = {
	//start
	0xDF,
	0x00, //linear_on ascr_skin_on 00
	0x6a, //ascr_skin_cb
	0x9a, //ascr_skin_cr
	0x25, //ascr_dist_up
	0x1a, //ascr_dist_down
	0x2a, //ascr_dist_left
	0x16, //ascr_dist_right
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
};

static char DSI_GRAYSCALE_MDNIE_2[] = {
	0xDE,
	0x07, //de cs gamma 000
	0x00, //de_gain 10
	0x00,
	0x02, //de_maxplus 11
	0x00,
	0x02, //de_maxminus 11
	0x00,
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
	0x00, //curve_offset
	0x00,
	0x00, //curve_low_x
	0x00, //curve_low_y
};

static char DSI_GRAYSCALE_MDNIE_3[] = {
	0xDD,
	0x01, //mdnie_en
	0x00, //mask 000
	0x0f, //ascr algo 0000
	0x00, //roi_en
	0x00, //roi_sx
	0x00, //roi_sx
	0x00, //roi_sy
	0x00, //roi_sy
	0x00, //roi_ex
	0x00, //roi_ex
	0x00, //roi_ey
	0x00, //roi_ey
	0x14, //trans_on trans_slope 0 0000
	0x01, //trans_interval
	//end
};

static char DSI_GRAYSCALE_NEGATIVE_MDNIE_1[] = {
	//start
	0xDF,
	0x00, //linear_on ascr_skin_on 00
	0x6a, //ascr_skin_cb
	0x9a, //ascr_skin_cr
	0x25, //ascr_dist_up
	0x1a, //ascr_dist_down
	0x2a, //ascr_dist_left
	0x16, //ascr_dist_right
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
	0x00, //ascr_skin_Wr
	0x00, //ascr_skin_Wg
	0x00, //ascr_skin_Wb
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
	0x00, //ascr_Wr
	0xff, //ascr_Kr
	0x00, //ascr_Wg
	0xff, //ascr_Kg
	0x00, //ascr_Wb
	0xff, //ascr_Kb
};

static char DSI_GRAYSCALE_NEGATIVE_MDNIE_2[] = {
	0xDE,
	0x07, //de cs gamma 000
	0x00, //de_gain 10
	0x00,
	0x02, //de_maxplus 11
	0x00,
	0x02, //de_maxminus 11
	0x00,
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
	0x00, //curve_offset
	0x00,
	0x00, //curve_low_x
	0x00, //curve_low_y
};

static char DSI_GRAYSCALE_NEGATIVE_MDNIE_3[] = {
	0xDD,
	0x01, //mdnie_en
	0x00, //mask 000
	0x0f, //ascr algo 0000
	0x00, //roi_en
	0x00, //roi_sx
	0x00, //roi_sx
	0x00, //roi_sy
	0x00, //roi_sy
	0x00, //roi_ex
	0x00, //roi_ex
	0x00, //roi_ey
	0x00, //roi_ey
	0x14, //trans_on trans_slope 0 0000
	0x01, //trans_interval
	//end
};

static char DSI_COLOR_BLIND_MDNIE_1[] = {
	//start
	0xDF,
	0x00, //linear_on ascr_skin_on 00
	0x6a, //ascr_skin_cb
	0x9a, //ascr_skin_cr
	0x25, //ascr_dist_up
	0x1a, //ascr_dist_down
	0x2a, //ascr_dist_left
	0x16, //ascr_dist_right
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
};

static char DSI_COLOR_BLIND_MDNIE_2[] = {
	0xDE,
	0x07, //de cs gamma 000
	0x00, //de_gain 10
	0x00,
	0x02, //de_maxplus 11
	0x00,
	0x02, //de_maxminus 11
	0x00,
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
	0x00, //curve_offset
	0x00,
	0x00, //curve_low_x
	0x00, //curve_low_y
};

static char DSI_COLOR_BLIND_MDNIE_3[] = {
	0xDD,
	0x01, //mdnie_en
	0x00, //mask 000
	0x0f, //ascr algo 0000
	0x00, //roi_en
	0x00, //roi_sx
	0x00, //roi_sx
	0x00, //roi_sy
	0x00, //roi_sy
	0x00, //roi_ex
	0x00, //roi_ex
	0x00, //roi_ey
	0x00, //roi_ey
	0x14, //trans_on trans_slope 0 0000
	0x01, //trans_interval
	//end
};

static char DSI_NIGHT_MODE_MDNIE_1[] = {
	//start
	0xDF,
	0x00, //linear_on ascr_skin_on 00
	0x6a, //ascr_skin_cb
	0x9a, //ascr_skin_cr
	0x25, //ascr_dist_up
	0x1a, //ascr_dist_down
	0x2a, //ascr_dist_left
	0x16, //ascr_dist_right
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
};

static char DSI_NIGHT_MODE_MDNIE_2[] = {
	0xDE,
	0x07, //de cs gamma 000
	0x00, //de_gain 10
	0x00,
	0x02, //de_maxplus 11
	0x00,
	0x02, //de_maxminus 11
	0x00,
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
	0x00, //curve_offset
	0x00,
	0x00, //curve_low_x
	0x00, //curve_low_y
};

static char DSI_NIGHT_MODE_MDNIE_3[] = {
	0xDD,
	0x01, //mdnie_en
	0x00, //mask 000
	0x0f, //ascr algo 0000
	0x00, //roi_en
	0x00, //roi_sx
	0x00, //roi_sx
	0x00, //roi_sy
	0x00, //roi_sy
	0x00, //roi_ex
	0x00, //roi_ex
	0x00, //roi_ey
	0x00, //roi_ey
	0x14, //trans_on trans_slope 0 0000
	0x01, //trans_interval
	//end
};

static char DSI_HBM_CE_MDNIE_1[] = {
	//start
	0xDF,
	0x00, //linear_on ascr_skin_on 00
	0x6a, //ascr_skin_cb
	0x9a, //ascr_skin_cr
	0x25, //ascr_dist_up
	0x1a, //ascr_dist_down
	0x2a, //ascr_dist_left
	0x16, //ascr_dist_right
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
};

static char DSI_HBM_CE_MDNIE_2[] = {
	0xDE,
	0x07, //de cs gamma 000
	0x00, //de_gain 10
	0x00,
	0x02, //de_maxplus 11
	0x00,
	0x02, //de_maxminus 11
	0x00,
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
	0x00, //curve_offset
	0x00,
	0x00, //curve_low_x
	0x00, //curve_low_y
};

static char DSI_HBM_CE_MDNIE_3[] = {
	0xDD,
	0x01, //mdnie_en
	0x00, //mask 000
	0x0f, //ascr algo 0000
	0x00, //roi_en
	0x00, //roi_sx
	0x00, //roi_sx
	0x00, //roi_sy
	0x00, //roi_sy
	0x00, //roi_ex
	0x00, //roi_ex
	0x00, //roi_ey
	0x00, //roi_ey
	0x14, //trans_on trans_slope 0 0000
	0x01, //trans_interval
	//end
};

static char DSI_RGB_SENSOR_MDNIE_1[] = {
	//start
	0xDF,
	0x00, //linear_on ascr_skin_on 00
	0x6a, //ascr_skin_cb
	0x9a, //ascr_skin_cr
	0x25, //ascr_dist_up
	0x1a, //ascr_dist_down
	0x2a, //ascr_dist_left
	0x16, //ascr_dist_right
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
};

static char DSI_RGB_SENSOR_MDNIE_2[] ={
	0xDE,
	0x07, //de cs gamma 000
	0x00, //de_gain 10
	0x00,
	0x02, //de_maxplus 11
	0x00,
	0x02, //de_maxminus 11
	0x00,
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
	0x00, //curve_offset
	0x00,
	0x00, //curve_low_x
	0x00, //curve_low_y
};

static char DSI_RGB_SENSOR_MDNIE_3[] ={
	0xDD,
	0x01, //mdnie_en
	0x00, //mask 000
	0x0f, //ascr algo 0000
	0x00, //roi_en
	0x00, //roi_sx
	0x00, //roi_sx
	0x00, //roi_sy
	0x00, //roi_sy
	0x00, //roi_ex
	0x00, //roi_ex
	0x00, //roi_ey
	0x00, //roi_ey
	0x14, //trans_on trans_slope 0 0000
	0x01, //trans_interval
	//end
};

static char DSI_SCREEN_CURTAIN_MDNIE_1[] = {
	//start
	0xDF,
	0x00, //linear_on ascr_skin_on 00
	0x6a, //ascr_skin_cb
	0x9a, //ascr_skin_cr
	0x25, //ascr_dist_up
	0x1a, //ascr_dist_down
	0x2a, //ascr_dist_left
	0x16, //ascr_dist_right
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
};

static char DSI_SCREEN_CURTAIN_MDNIE_2[] ={
	0xDE,
	0x07, //de cs gamma 000
	0x00, //de_gain 10
	0x00,
	0x02, //de_maxplus 11
	0x00,
	0x02, //de_maxminus 11
	0x00,
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
	0x00, //curve_offset
	0x00,
	0x00, //curve_low_x
	0x00, //curve_low_y
};

static char DSI_SCREEN_CURTAIN_MDNIE_3[] ={
	0xDD,
	0x01, //mdnie_en
	0x00, //mask 000
	0x0f, //ascr algo 0000
	0x00, //roi_en
	0x00, //roi_sx
	0x00, //roi_sx
	0x00, //roi_sy
	0x00, //roi_sy
	0x00, //roi_ex
	0x00, //roi_ex
	0x00, //roi_ey
	0x00, //roi_ey
	0x14, //trans_on trans_slope 0 0000
	0x01, //trans_interval
	//end
};

static char DSI_LIGHT_NOTIFICATION_MDNIE_1[] = {
	//start
	0xDF,
	0x01, //linear_on ascr_skin_on 00
	0x6a, //ascr_skin_cb
	0x9a, //ascr_skin_cr
	0x25, //ascr_dist_up
	0x1a, //ascr_dist_down
	0x2a, //ascr_dist_left
	0x16, //ascr_dist_right
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
	0x60, //ascr_skin_Rg
	0x13, //ascr_skin_Rb
	0xff, //ascr_skin_Yr
	0xf9, //ascr_skin_Yg
	0x13, //ascr_skin_Yb
	0xff, //ascr_skin_Mr
	0x60, //ascr_skin_Mg
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
};

static char DSI_LIGHT_NOTIFICATION_MDNIE_2[] = {
	0xDE,
	0x07, //de cs gamma 000
	0x00, //de_gain 10
	0x00,
	0x02, //de_maxplus 11
	0x00,
	0x02, //de_maxminus 11
	0x00,
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
	0x00, //curve_offset
	0x00,
	0x00, //curve_low_x
	0x00, //curve_low_y
};

static char DSI_LIGHT_NOTIFICATION_MDNIE_3[] = {
	0xDD,
	0x01, //mdnie_en
	0x00, //mask 000
	0x0f, //ascr algo 0000
	0x00, //roi_en
	0x00, //roi_sx
	0x00, //roi_sx
	0x00, //roi_sy
	0x00, //roi_sy
	0x00, //roi_ex
	0x00, //roi_ex
	0x00, //roi_ey
	0x00, //roi_ey
	0x14, //trans_on trans_slope 0 0000
	0x01, //trans_interval
	//end
};

static unsigned char DSI_VIDEO_ENHANCER_MDNIE_1[] = {
	//start
	0xDF,
	0x01, //linear_on ascr_skin_on 00
	0x67, //ascr_skin_cb
	0xa9, //ascr_skin_cr
	0x37, //ascr_dist_up
	0x29, //ascr_dist_down
	0x47, //ascr_dist_left
	0x19, //ascr_dist_right
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
	0x48, //ascr_skin_Rg
	0x58, //ascr_skin_Rb
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
};

static unsigned char DSI_VIDEO_ENHANCER_MDNIE_2[] = {
	0xDE,
	0x07, //de cs gamma 000
	0x00, //de_gain 10
	0x10,
	0x00, //de_maxplus 11
	0x40,
	0x00, //de_maxminus 11
	0x40,
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
	0xa8, //curve_10
	0xbb, //curve_11
	0xcb, //curve_12
	0xdb, //curve_13
	0xeb, //curve_14
	0xf8, //curve_15
	0x01, //curve_16
	0x00,
	0x00, //curve_offset
	0x00,
	0x00, //curve_low_x
	0x00, //curve_low_y
};

static unsigned char DSI_VIDEO_ENHANCER_MDNIE_3[] = {
	0xDD,
	0x01, //mdnie_en
	0x00, //mask 000
	0x0f, //ascr algo 0000
	0x00, //roi_en
	0x00, //roi_sx
	0x00, //roi_sx
	0x00, //roi_sy
	0x00, //roi_sy
	0x00, //roi_ex
	0x00, //roi_ex
	0x00, //roi_ey
	0x00, //roi_ey
	0x14, //trans_on trans_slope 0 0000
	0x01, //trans_interval
	//end
};

static unsigned char DSI_VIDEO_ENHANCER_THIRD_MDNIE_1[] = {
	//start
	0xDF,
	0x01, //linear_on ascr_skin_on 00
	0x67, //ascr_skin_cb
	0xa9, //ascr_skin_cr
	0x37, //ascr_dist_up
	0x29, //ascr_dist_down
	0x47, //ascr_dist_left
	0x19, //ascr_dist_right
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
	0x48, //ascr_skin_Rg
	0x58, //ascr_skin_Rb
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
};

static unsigned char DSI_VIDEO_ENHANCER_THIRD_MDNIE_2[] = {
	0xDE,
	0x07, //de cs gamma 000
	0x00, //de_gain 10
	0x10,
	0x00, //de_maxplus 11
	0x40,
	0x00, //de_maxminus 11
	0x40,
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
	0xa8, //curve_10
	0xbb, //curve_11
	0xcb, //curve_12
	0xdb, //curve_13
	0xeb, //curve_14
	0xf8, //curve_15
	0x01, //curve_16
	0x00,
	0x00, //curve_offset
	0x00,
	0x00, //curve_low_x
	0x00, //curve_low_y
};

static unsigned char DSI_VIDEO_ENHANCER_THIRD_MDNIE_3[] = {
	0xDD,
	0x01, //mdnie_en
	0x00, //mask 000
	0x0f, //ascr algo 0000
	0x00, //roi_en
	0x00, //roi_sx
	0x00, //roi_sx
	0x00, //roi_sy
	0x00, //roi_sy
	0x00, //roi_ex
	0x00, //roi_ex
	0x00, //roi_ey
	0x00, //roi_ey
	0x14, //trans_on trans_slope 0 0000
	0x01, //trans_interval
	//end
};

static char DSI_UI_DYNAMIC_MDNIE_1[] = {
	//start
	0xDF,
	0x02, //linear_on ascr_skin_on 00
	0x67, //ascr_skin_cb
	0xa9, //ascr_skin_cr
	0x37, //ascr_dist_up
	0x29, //ascr_dist_down
	0x47, //ascr_dist_left
	0x19, //ascr_dist_right
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
	0xe8, //ascr_skin_Rr
	0x00, //ascr_skin_Rg
	0x00, //ascr_skin_Rb
	0xf7, //ascr_skin_Yr
	0xed, //ascr_skin_Yg
	0x00, //ascr_skin_Yb
	0xf2, //ascr_skin_Mr
	0x21, //ascr_skin_Mg
	0xea, //ascr_skin_Mb
	0xff, //ascr_skin_Wr
	0xf7, //ascr_skin_Wg
	0xef, //ascr_skin_Wb
	0x5d, //ascr_Cr
	0xe8, //ascr_Rr
	0xf7, //ascr_Cg
	0x00, //ascr_Rg
	0xec, //ascr_Cb
	0x00, //ascr_Rb
	0xf2, //ascr_Mr
	0x56, //ascr_Gr
	0x21, //ascr_Mg
	0xee, //ascr_Gg
	0xea, //ascr_Mb
	0x00, //ascr_Gb
	0xf7, //ascr_Yr
	0x22, //ascr_Br
	0xed, //ascr_Yg
	0x37, //ascr_Bg
	0x00, //ascr_Yb
	0xea, //ascr_Bb
	0xff, //ascr_Wr
	0x00, //ascr_Kr
	0xf7, //ascr_Wg
	0x00, //ascr_Kg
	0xef, //ascr_Wb
	0x00, //ascr_Kb
};

static char DSI_UI_DYNAMIC_MDNIE_2[] ={
	0xDE,
	0x07, //de cs gamma 000
	0x00, //de_gain 10
	0x00,
	0x02, //de_maxplus 11
	0x00,
	0x02, //de_maxminus 11
	0x00,
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
	0x00, //curve_offset
	0x00,
	0x00, //curve_low_x
	0x00, //curve_low_y
};

static char DSI_UI_DYNAMIC_MDNIE_3[] ={
	0xDD,
	0x01, //mdnie_en
	0x00, //mask 000
	0x0f, //ascr algo 0000
	0x00, //roi_en
	0x00, //roi_sx
	0x00, //roi_sx
	0x00, //roi_sy
	0x00, //roi_sy
	0x00, //roi_ex
	0x00, //roi_ex
	0x00, //roi_ey
	0x00, //roi_ey
	0x14, //trans_on trans_slope 0 0000
	0x01, //trans_interval
	//end
};

static char DSI_UI_STANDARD_MDNIE_1[] = {
	//start
	0xDF,
	0x02, //linear_on ascr_skin_on 00
	0x6a, //ascr_skin_cb
	0x9a, //ascr_skin_cr
	0x25, //ascr_dist_up
	0x1a, //ascr_dist_down
	0x2a, //ascr_dist_left
	0x16, //ascr_dist_right
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
	0xfd, //ascr_skin_Rr
	0x2e, //ascr_skin_Rg
	0x2e, //ascr_skin_Rb
	0xf8, //ascr_skin_Yr
	0xee, //ascr_skin_Yg
	0x40, //ascr_skin_Yb
	0xff, //ascr_skin_Mr
	0x4a, //ascr_skin_Mg
	0xe9, //ascr_skin_Mb
	0xff, //ascr_skin_Wr
	0xf7, //ascr_skin_Wg
	0xef, //ascr_skin_Wb
	0x00, //ascr_Cr
	0xfd, //ascr_Rr
	0xf0, //ascr_Cg
	0x2e, //ascr_Rg
	0xe9, //ascr_Cb
	0x2e, //ascr_Rb
	0xff, //ascr_Mr
	0x00, //ascr_Gr
	0x4a, //ascr_Mg
	0xe6, //ascr_Gg
	0xe9, //ascr_Mb
	0x2e, //ascr_Gb
	0xf8, //ascr_Yr
	0x27, //ascr_Br
	0xee, //ascr_Yg
	0x35, //ascr_Bg
	0x40, //ascr_Yb
	0xe4, //ascr_Bb
	0xff, //ascr_Wr
	0x00, //ascr_Kr
	0xf7, //ascr_Wg
	0x00, //ascr_Kg
	0xef, //ascr_Wb
	0x00, //ascr_Kb
};

static char DSI_UI_STANDARD_MDNIE_2[] = {
	0xDE,
	0x07, //de cs gamma 000
	0x00, //de_gain 10
	0x00,
	0x02, //de_maxplus 11
	0x00,
	0x02, //de_maxminus 11
	0x00,
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
	0x00, //curve_offset
	0x00,
	0x00, //curve_low_x
	0x00, //curve_low_y
};

static char DSI_UI_STANDARD_MDNIE_3[] = {
	0xDD,
	0x01, //mdnie_en
	0x00, //mask 000
	0x0f, //ascr algo 0000
	0x00, //roi_en
	0x00, //roi_sx
	0x00, //roi_sx
	0x00, //roi_sy
	0x00, //roi_sy
	0x00, //roi_ex
	0x00, //roi_ex
	0x00, //roi_ey
	0x00, //roi_ey
	0x14, //trans_on trans_slope 0 0000
	0x01, //trans_interval
	//end
};

static char DSI_UI_NATURAL_MDNIE_1[] = {
	//start
	0xDF,
	0x02, //linear_on ascr_skin_on 00
	0x6a, //ascr_skin_cb
	0x9a, //ascr_skin_cr
	0x25, //ascr_dist_up
	0x1a, //ascr_dist_down
	0x2a, //ascr_dist_left
	0x16, //ascr_dist_right
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
	0xde, //ascr_skin_Rr
	0x24, //ascr_skin_Rg
	0x28, //ascr_skin_Rb
	0xf8, //ascr_skin_Yr
	0xef, //ascr_skin_Yg
	0x53, //ascr_skin_Yb
	0xe1, //ascr_skin_Mr
	0x41, //ascr_skin_Mg
	0xe3, //ascr_skin_Mb
	0xff, //ascr_skin_Wr
	0xf7, //ascr_skin_Wg
	0xef, //ascr_skin_Wb
	0x91, //ascr_Cr
	0xde, //ascr_Rr
	0xf3, //ascr_Cg
	0x24, //ascr_Rg
	0xea, //ascr_Cb
	0x28, //ascr_Rb
	0xe1, //ascr_Mr
	0x8a, //ascr_Gr
	0x41, //ascr_Mg
	0xec, //ascr_Gg
	0xe3, //ascr_Mb
	0x4a, //ascr_Gb
	0xf8, //ascr_Yr
	0x24, //ascr_Br
	0xef, //ascr_Yg
	0x34, //ascr_Bg
	0x53, //ascr_Yb
	0xe0, //ascr_Bb
	0xff, //ascr_Wr
	0x00, //ascr_Kr
	0xf7, //ascr_Wg
	0x00, //ascr_Kg
	0xef, //ascr_Wb
	0x00, //ascr_Kb
};

static char DSI_UI_NATURAL_MDNIE_2[] = {
	0xDE,
	0x07, //de cs gamma 000
	0x00, //de_gain 10
	0x00,
	0x02, //de_maxplus 11
	0x00,
	0x02, //de_maxminus 11
	0x00,
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
	0x00, //curve_offset
	0x00,
	0x00, //curve_low_x
	0x00, //curve_low_y
};

static char DSI_UI_NATURAL_MDNIE_3[] = {
	0xDD,
	0x01, //mdnie_en
	0x00, //mask 000
	0x0f, //ascr algo 0000
	0x00, //roi_en
	0x00, //roi_sx
	0x00, //roi_sx
	0x00, //roi_sy
	0x00, //roi_sy
	0x00, //roi_ex
	0x00, //roi_ex
	0x00, //roi_ey
	0x00, //roi_ey
	0x14, //trans_on trans_slope 0 0000
	0x01, //trans_interval
	//end
};

static char DSI_UI_MOVIE_MDNIE_1[] = {
	//start
	0xDF,
	0x01, //linear_on ascr_skin_on 00
	0x67, //ascr_skin_cb
	0xa9, //ascr_skin_cr
	0x0c, //ascr_dist_up
	0x0c, //ascr_dist_down
	0x0c, //ascr_dist_left
	0x0c, //ascr_dist_right
	0x00, //ascr_div_up 20
	0xaa,
	0xab,
	0x00, //ascr_div_down
	0xaa,
	0xab,
	0x00, //ascr_div_right
	0xaa,
	0xab,
	0x00, //ascr_div_left
	0xaa,
	0xab,
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
};

static char DSI_UI_MOVIE_MDNIE_2[] = {
	0xDE,
	0x00, //de cs gamma 000
	0x00, //de_gain 10
	0x00,
	0x07, //de_maxplus 11
	0xff,
	0x07, //de_maxminus 11
	0xff,
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
	0x00, //curve_offset
	0x00,
	0x00, //curve_low_x
	0x00, //curve_low_y
};

static char DSI_UI_MOVIE_MDNIE_3[] = {
	0xDD,
	0x01, //mdnie_en
	0x00, //mask 000
	0x0c, //ascr algo 0000
	0x00, //roi_en
	0x00, //roi_sx
	0x00, //roi_sx
	0x00, //roi_sy
	0x00, //roi_sy
	0x00, //roi_ex
	0x00, //roi_ex
	0x00, //roi_ey
	0x00, //roi_ey
	0x00, //trans_on trans_slope 0 0000
	0x00, //trans_interval
	//end
};

static char DSI_UI_AUTO_MDNIE_1[] = {
	//start
	0xDF,
	0x00, //linear_on ascr_skin_on 00
	0x6a, //ascr_skin_cb
	0x9a, //ascr_skin_cr
	0x25, //ascr_dist_up
	0x1a, //ascr_dist_down
	0x2a, //ascr_dist_left
	0x16, //ascr_dist_right
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
	0x2c, //ascr_skin_Rg
	0x40, //ascr_skin_Rb
	0xff, //ascr_skin_Yr
	0xf0, //ascr_skin_Yg
	0x00, //ascr_skin_Yb
	0xd8, //ascr_skin_Mr
	0x00, //ascr_skin_Mg
	0xd9, //ascr_skin_Mb
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
};

static char DSI_UI_AUTO_MDNIE_2[] = {
	0xDE,
	0x07, //de cs gamma 000
	0x00, //de_gain 10
	0x00,
	0x02, //de_maxplus 11
	0x00,
	0x02, //de_maxminus 11
	0x00,
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
	0x00, //curve_offset
	0x00,
	0x00, //curve_low_x
	0x00, //curve_low_y
};

static char DSI_UI_AUTO_MDNIE_3[] = {
	0xDD,
	0x01, //mdnie_en
	0x00, //mask 000
	0x0f, //ascr algo 0000
	0x00, //roi_en
	0x00, //roi_sx
	0x00, //roi_sx
	0x00, //roi_sy
	0x00, //roi_sy
	0x00, //roi_ex
	0x00, //roi_ex
	0x00, //roi_ey
	0x00, //roi_ey
	0x14, //trans_on trans_slope 0 0000
	0x01, //trans_interval
	//end
};

static char DSI_VIDEO_OUTDOOR_MDNIE_1[] = {
	//start
	0xDF,
	0x01, //linear_on ascr_skin_on 00
	0x67, //ascr_skin_cb
	0xa9, //ascr_skin_cr
	0x0c, //ascr_dist_up
	0x0c, //ascr_dist_down
	0x0c, //ascr_dist_left
	0x0c, //ascr_dist_right
	0x00, //ascr_div_up 20
	0xaa,
	0xab,
	0x00, //ascr_div_down
	0xaa,
	0xab,
	0x00, //ascr_div_right
	0xaa,
	0xab,
	0x00, //ascr_div_left
	0xaa,
	0xab,
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
};

static char DSI_VIDEO_OUTDOOR_MDNIE_2[] = {
	0xDE,
	0x00, //de cs gamma 000
	0x00, //de_gain 10
	0x00,
	0x07, //de_maxplus 11
	0xff,
	0x07, //de_maxminus 11
	0xff,
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
	0x00, //curve_offset
	0x00,
	0x00, //curve_low_x
	0x00, //curve_low_y
};

static char DSI_VIDEO_OUTDOOR_MDNIE_3[] = {
	0xDD,
	0x01, //mdnie_en
	0x00, //mask 000
	0x0c, //ascr algo 0000
	0x00, //roi_en
	0x00, //roi_sx
	0x00, //roi_sx
	0x00, //roi_sy
	0x00, //roi_sy
	0x00, //roi_ex
	0x00, //roi_ex
	0x00, //roi_ey
	0x00, //roi_ey
	0x00, //trans_on trans_slope 0 0000
	0x00, //trans_interval
	//end
};

static char DSI_VIDEO_DYNAMIC_MDNIE_1[] = {
	//start
	0xDF,
	0x02, //linear_on ascr_skin_on 00
	0x67, //ascr_skin_cb
	0xa9, //ascr_skin_cr
	0x37, //ascr_dist_up
	0x29, //ascr_dist_down
	0x47, //ascr_dist_left
	0x19, //ascr_dist_right
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
	0xe8, //ascr_skin_Rr
	0x00, //ascr_skin_Rg
	0x00, //ascr_skin_Rb
	0xf7, //ascr_skin_Yr
	0xed, //ascr_skin_Yg
	0x00, //ascr_skin_Yb
	0xf2, //ascr_skin_Mr
	0x21, //ascr_skin_Mg
	0xea, //ascr_skin_Mb
	0xff, //ascr_skin_Wr
	0xf7, //ascr_skin_Wg
	0xef, //ascr_skin_Wb
	0x5d, //ascr_Cr
	0xe8, //ascr_Rr
	0xf7, //ascr_Cg
	0x00, //ascr_Rg
	0xec, //ascr_Cb
	0x00, //ascr_Rb
	0xf2, //ascr_Mr
	0x56, //ascr_Gr
	0x21, //ascr_Mg
	0xee, //ascr_Gg
	0xea, //ascr_Mb
	0x00, //ascr_Gb
	0xf7, //ascr_Yr
	0x22, //ascr_Br
	0xed, //ascr_Yg
	0x37, //ascr_Bg
	0x00, //ascr_Yb
	0xea, //ascr_Bb
	0xff, //ascr_Wr
	0x00, //ascr_Kr
	0xf7, //ascr_Wg
	0x00, //ascr_Kg
	0xef, //ascr_Wb
	0x00, //ascr_Kb
};

static char DSI_VIDEO_DYNAMIC_MDNIE_2[] = {
	0xDE,
	0x07, //de cs gamma 000
	0x00, //de_gain 10
	0x10,
	0x00, //de_maxplus 11
	0x40,
	0x00, //de_maxminus 11
	0x40,
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
	0x00, //curve_offset
	0x00,
	0x00, //curve_low_x
	0x00, //curve_low_y
};

static char DSI_VIDEO_DYNAMIC_MDNIE_3[] = {
	0xDD,
	0x01, //mdnie_en
	0x00, //mask 000
	0x0f, //ascr algo 0000
	0x00, //roi_en
	0x00, //roi_sx
	0x00, //roi_sx
	0x00, //roi_sy
	0x00, //roi_sy
	0x00, //roi_ex
	0x00, //roi_ex
	0x00, //roi_ey
	0x00, //roi_ey
	0x14, //trans_on trans_slope 0 0000
	0x01, //trans_interval
	//end
};

static char DSI_VIDEO_STANDARD_MDNIE_1[] = {
	//start
	0xDF,
	0x02, //linear_on ascr_skin_on 00
	0x6a, //ascr_skin_cb
	0x9a, //ascr_skin_cr
	0x25, //ascr_dist_up
	0x1a, //ascr_dist_down
	0x2a, //ascr_dist_left
	0x16, //ascr_dist_right
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
	0xfd, //ascr_skin_Rr
	0x2e, //ascr_skin_Rg
	0x2e, //ascr_skin_Rb
	0xf8, //ascr_skin_Yr
	0xee, //ascr_skin_Yg
	0x40, //ascr_skin_Yb
	0xff, //ascr_skin_Mr
	0x4a, //ascr_skin_Mg
	0xe9, //ascr_skin_Mb
	0xff, //ascr_skin_Wr
	0xf7, //ascr_skin_Wg
	0xef, //ascr_skin_Wb
	0x00, //ascr_Cr
	0xfd, //ascr_Rr
	0xf0, //ascr_Cg
	0x2e, //ascr_Rg
	0xe9, //ascr_Cb
	0x2e, //ascr_Rb
	0xff, //ascr_Mr
	0x00, //ascr_Gr
	0x4a, //ascr_Mg
	0xe6, //ascr_Gg
	0xe9, //ascr_Mb
	0x2e, //ascr_Gb
	0xf8, //ascr_Yr
	0x27, //ascr_Br
	0xee, //ascr_Yg
	0x35, //ascr_Bg
	0x40, //ascr_Yb
	0xe4, //ascr_Bb
	0xff, //ascr_Wr
	0x00, //ascr_Kr
	0xf7, //ascr_Wg
	0x00, //ascr_Kg
	0xef, //ascr_Wb
	0x00, //ascr_Kb
};

static char DSI_VIDEO_STANDARD_MDNIE_2[] = {
	0xDE,
	0x07, //de cs gamma 000
	0x00, //de_gain 10
	0x00,
	0x02, //de_maxplus 11
	0x00,
	0x02, //de_maxminus 11
	0x00,
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
	0x00, //curve_offset
	0x00,
	0x00, //curve_low_x
	0x00, //curve_low_y
};

static char DSI_VIDEO_STANDARD_MDNIE_3[] = {
	0xDD,
	0x01, //mdnie_en
	0x00, //mask 000
	0x0f, //ascr algo 0000
	0x00, //roi_en
	0x00, //roi_sx
	0x00, //roi_sx
	0x00, //roi_sy
	0x00, //roi_sy
	0x00, //roi_ex
	0x00, //roi_ex
	0x00, //roi_ey
	0x00, //roi_ey
	0x14, //trans_on trans_slope 0 0000
	0x01, //trans_interval
	//end
};

static char DSI_VIDEO_NATURAL_MDNIE_1[] = {
	//start
	0xDF,
	0x02, //linear_on ascr_skin_on 00
	0x6a, //ascr_skin_cb
	0x9a, //ascr_skin_cr
	0x25, //ascr_dist_up
	0x1a, //ascr_dist_down
	0x2a, //ascr_dist_left
	0x16, //ascr_dist_right
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
	0xde, //ascr_skin_Rr
	0x24, //ascr_skin_Rg
	0x28, //ascr_skin_Rb
	0xf8, //ascr_skin_Yr
	0xef, //ascr_skin_Yg
	0x53, //ascr_skin_Yb
	0xe1, //ascr_skin_Mr
	0x41, //ascr_skin_Mg
	0xe3, //ascr_skin_Mb
	0xff, //ascr_skin_Wr
	0xf7, //ascr_skin_Wg
	0xef, //ascr_skin_Wb
	0x91, //ascr_Cr
	0xde, //ascr_Rr
	0xf3, //ascr_Cg
	0x24, //ascr_Rg
	0xea, //ascr_Cb
	0x28, //ascr_Rb
	0xe1, //ascr_Mr
	0x8a, //ascr_Gr
	0x41, //ascr_Mg
	0xec, //ascr_Gg
	0xe3, //ascr_Mb
	0x4a, //ascr_Gb
	0xf8, //ascr_Yr
	0x24, //ascr_Br
	0xef, //ascr_Yg
	0x34, //ascr_Bg
	0x53, //ascr_Yb
	0xe0, //ascr_Bb
	0xff, //ascr_Wr
	0x00, //ascr_Kr
	0xf7, //ascr_Wg
	0x00, //ascr_Kg
	0xef, //ascr_Wb
	0x00, //ascr_Kb
};

static char DSI_VIDEO_NATURAL_MDNIE_2[] = {
	0xDE,
	0x07, //de cs gamma 000
	0x00, //de_gain 10
	0x00,
	0x02, //de_maxplus 11
	0x00,
	0x02, //de_maxminus 11
	0x00,
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
	0x00, //curve_offset
	0x00,
	0x00, //curve_low_x
	0x00, //curve_low_y
};

static char DSI_VIDEO_NATURAL_MDNIE_3[] = {
	0xDD,
	0x01, //mdnie_en
	0x00, //mask 000
	0x0f, //ascr algo 0000
	0x00, //roi_en
	0x00, //roi_sx
	0x00, //roi_sx
	0x00, //roi_sy
	0x00, //roi_sy
	0x00, //roi_ex
	0x00, //roi_ex
	0x00, //roi_ey
	0x00, //roi_ey
	0x14, //trans_on trans_slope 0 0000
	0x01, //trans_interval
	//end
};

static char DSI_VIDEO_MOVIE_MDNIE_1[] = {
	//start
	0xDF,
	0x01, //linear_on ascr_skin_on 00
	0x67, //ascr_skin_cb
	0xa9, //ascr_skin_cr
	0x0c, //ascr_dist_up
	0x0c, //ascr_dist_down
	0x0c, //ascr_dist_left
	0x0c, //ascr_dist_right
	0x00, //ascr_div_up 20
	0xaa,
	0xab,
	0x00, //ascr_div_down
	0xaa,
	0xab,
	0x00, //ascr_div_right
	0xaa,
	0xab,
	0x00, //ascr_div_left
	0xaa,
	0xab,
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
};

static char DSI_VIDEO_MOVIE_MDNIE_2[] = {
	0xDE,
	0x00, //de cs gamma 000
	0x00, //de_gain 10
	0x00,
	0x07, //de_maxplus 11
	0xff,
	0x07, //de_maxminus 11
	0xff,
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
	0x00, //curve_offset
	0x00,
	0x00, //curve_low_x
	0x00, //curve_low_y
};

static char DSI_VIDEO_MOVIE_MDNIE_3[] = {
	0xDD,
	0x01, //mdnie_en
	0x00, //mask 000
	0x0c, //ascr algo 0000
	0x00, //roi_en
	0x00, //roi_sx
	0x00, //roi_sx
	0x00, //roi_sy
	0x00, //roi_sy
	0x00, //roi_ex
	0x00, //roi_ex
	0x00, //roi_ey
	0x00, //roi_ey
	0x00, //trans_on trans_slope 0 0000
	0x00, //trans_interval
	//end
};

static char DSI_VIDEO_AUTO_MDNIE_1[] = {
	//start
	0xDF,
	0x01, //linear_on ascr_skin_on 00
	0x67, //ascr_skin_cb
	0xa9, //ascr_skin_cr
	0x37, //ascr_dist_up
	0x29, //ascr_dist_down
	0x47, //ascr_dist_left
	0x19, //ascr_dist_right
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
	0x60, //ascr_skin_Rg
	0x70, //ascr_skin_Rb
	0xff, //ascr_skin_Yr
	0xff, //ascr_skin_Yg
	0x00, //ascr_skin_Yb
	0xff, //ascr_skin_Mr
	0x00, //ascr_skin_Mg
	0xff, //ascr_skin_Mb
	0xff, //ascr_skin_Wr
	0xf4, //ascr_skin_Wg
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
};

static char DSI_VIDEO_AUTO_MDNIE_2[] = {
	0xDE,
	0x07, //de cs gamma 000
	0x00, //de_gain 10
	0x10,
	0x00, //de_maxplus 11
	0x40,
	0x00, //de_maxminus 11
	0x40,
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
	0x00, //curve_offset
	0x00,
	0x00, //curve_low_x
	0x00, //curve_low_y
};

static char DSI_VIDEO_AUTO_MDNIE_3[] = {
	0xDD,
	0x01, //mdnie_en
	0x00, //mask 000
	0x0f, //ascr algo 0000
	0x00, //roi_en
	0x00, //roi_sx
	0x00, //roi_sx
	0x00, //roi_sy
	0x00, //roi_sy
	0x00, //roi_ex
	0x00, //roi_ex
	0x00, //roi_ey
	0x00, //roi_ey
	0x14, //trans_on trans_slope 0 0000
	0x01, //trans_interval
	//end
};

static char DSI_VIDEO_WARM_OUTDOOR_MDNIE_1[] = {
	//start
	0xDF,
	0x01, //linear_on ascr_skin_on 00
	0x67, //ascr_skin_cb
	0xa9, //ascr_skin_cr
	0x0c, //ascr_dist_up
	0x0c, //ascr_dist_down
	0x0c, //ascr_dist_left
	0x0c, //ascr_dist_right
	0x00, //ascr_div_up 20
	0xaa,
	0xab,
	0x00, //ascr_div_down
	0xaa,
	0xab,
	0x00, //ascr_div_right
	0xaa,
	0xab,
	0x00, //ascr_div_left
	0xaa,
	0xab,
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
};

static char DSI_VIDEO_WARM_OUTDOOR_MDNIE_2[] = {
	0xDE,
	0x00, //de cs gamma 000
	0x00, //de_gain 10
	0x00,
	0x07, //de_maxplus 11
	0xff,
	0x07, //de_maxminus 11
	0xff,
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
	0x00, //curve_offset
	0x00,
	0x00, //curve_low_x
	0x00, //curve_low_y
};

static char DSI_VIDEO_WARM_OUTDOOR_MDNIE_3[] = {
	0xDD,
	0x01, //mdnie_en
	0x00, //mask 000
	0x0c, //ascr algo 0000
	0x00, //roi_en
	0x00, //roi_sx
	0x00, //roi_sx
	0x00, //roi_sy
	0x00, //roi_sy
	0x00, //roi_ex
	0x00, //roi_ex
	0x00, //roi_ey
	0x00, //roi_ey
	0x00, //trans_on trans_slope 0 0000
	0x00, //trans_interval
	//end
};

static char DSI_VIDEO_WARM_MDNIE_1[] = {
	//start
	0xDF,
	0x01, //linear_on ascr_skin_on 00
	0x67, //ascr_skin_cb
	0xa9, //ascr_skin_cr
	0x0c, //ascr_dist_up
	0x0c, //ascr_dist_down
	0x0c, //ascr_dist_left
	0x0c, //ascr_dist_right
	0x00, //ascr_div_up 20
	0xaa,
	0xab,
	0x00, //ascr_div_down
	0xaa,
	0xab,
	0x00, //ascr_div_right
	0xaa,
	0xab,
	0x00, //ascr_div_left
	0xaa,
	0xab,
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
};

static char DSI_VIDEO_WARM_MDNIE_2[] = {
	0xDE,
	0x00, //de cs gamma 000
	0x00, //de_gain 10
	0x00,
	0x07, //de_maxplus 11
	0xff,
	0x07, //de_maxminus 11
	0xff,
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
	0x00, //curve_offset
	0x00,
	0x00, //curve_low_x
	0x00, //curve_low_y
};

static char DSI_VIDEO_WARM_MDNIE_3[] = {
	0xDD,
	0x01, //mdnie_en
	0x00, //mask 000
	0x0c, //ascr algo 0000
	0x00, //roi_en
	0x00, //roi_sx
	0x00, //roi_sx
	0x00, //roi_sy
	0x00, //roi_sy
	0x00, //roi_ex
	0x00, //roi_ex
	0x00, //roi_ey
	0x00, //roi_ey
	0x00, //trans_on trans_slope 0 0000
	0x00, //trans_interval
	//end
};

static char DSI_VIDEO_COLD_OUTDOOR_MDNIE_1[] = {
	//start
	0xDF,
	0x01, //linear_on ascr_skin_on 00
	0x67, //ascr_skin_cb
	0xa9, //ascr_skin_cr
	0x0c, //ascr_dist_up
	0x0c, //ascr_dist_down
	0x0c, //ascr_dist_left
	0x0c, //ascr_dist_right
	0x00, //ascr_div_up 20
	0xaa,
	0xab,
	0x00, //ascr_div_down
	0xaa,
	0xab,
	0x00, //ascr_div_right
	0xaa,
	0xab,
	0x00, //ascr_div_left
	0xaa,
	0xab,
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
};

static char DSI_VIDEO_COLD_OUTDOOR_MDNIE_2[] = {
	0xDE,
	0x00, //de cs gamma 000
	0x00, //de_gain 10
	0x00,
	0x07, //de_maxplus 11
	0xff,
	0x07, //de_maxminus 11
	0xff,
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
	0x00, //curve_offset
	0x00,
	0x00, //curve_low_x
	0x00, //curve_low_y
};

static char DSI_VIDEO_COLD_OUTDOOR_MDNIE_3[] = {
	0xDD,
	0x01, //mdnie_en
	0x00, //mask 000
	0x0c, //ascr algo 0000
	0x00, //roi_en
	0x00, //roi_sx
	0x00, //roi_sx
	0x00, //roi_sy
	0x00, //roi_sy
	0x00, //roi_ex
	0x00, //roi_ex
	0x00, //roi_ey
	0x00, //roi_ey
	0x00, //trans_on trans_slope 0 0000
	0x00, //trans_interval
	//end
};

static char DSI_VIDEO_COLD_MDNIE_1[] ={
	//start
	0xDF,
	0x01, //linear_on ascr_skin_on 00
	0x67, //ascr_skin_cb
	0xa9, //ascr_skin_cr
	0x0c, //ascr_dist_up
	0x0c, //ascr_dist_down
	0x0c, //ascr_dist_left
	0x0c, //ascr_dist_right
	0x00, //ascr_div_up 20
	0xaa,
	0xab,
	0x00, //ascr_div_down
	0xaa,
	0xab,
	0x00, //ascr_div_right
	0xaa,
	0xab,
	0x00, //ascr_div_left
	0xaa,
	0xab,
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
};

static char DSI_VIDEO_COLD_MDNIE_2[] ={
	0xDE,
	0x00, //de cs gamma 000
	0x00, //de_gain 10
	0x00,
	0x07, //de_maxplus 11
	0xff,
	0x07, //de_maxminus 11
	0xff,
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
	0x00, //curve_offset
	0x00,
	0x00, //curve_low_x
	0x00, //curve_low_y
};

static char DSI_VIDEO_COLD_MDNIE_3[] ={
	0xDD,
	0x01, //mdnie_en
	0x00, //mask 000
	0x0c, //ascr algo 0000
	0x00, //roi_en
	0x00, //roi_sx
	0x00, //roi_sx
	0x00, //roi_sy
	0x00, //roi_sy
	0x00, //roi_ex
	0x00, //roi_ex
	0x00, //roi_ey
	0x00, //roi_ey
	0x00, //trans_on trans_slope 0 0000
	0x00, //trans_interval
	//end
};

static char DSI_CAMERA_OUTDOOR_MDNIE_1[] ={
	//start
	0xDF,
	0x01, //linear_on ascr_skin_on 00
	0x67, //ascr_skin_cb
	0xa9, //ascr_skin_cr
	0x0c, //ascr_dist_up
	0x0c, //ascr_dist_down
	0x0c, //ascr_dist_left
	0x0c, //ascr_dist_right
	0x00, //ascr_div_up 20
	0xaa,
	0xab,
	0x00, //ascr_div_down
	0xaa,
	0xab,
	0x00, //ascr_div_right
	0xaa,
	0xab,
	0x00, //ascr_div_left
	0xaa,
	0xab,
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
};

static char DSI_CAMERA_OUTDOOR_MDNIE_2[] ={
	0xDE,
	0x00, //de cs gamma 000
	0x00, //de_gain 10
	0x00,
	0x07, //de_maxplus 11
	0xff,
	0x07, //de_maxminus 11
	0xff,
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
	0x00, //curve_offset
	0x00,
	0x00, //curve_low_x
	0x00, //curve_low_y
};

static char DSI_CAMERA_OUTDOOR_MDNIE_3[] ={
	0xDD,
	0x01, //mdnie_en
	0x00, //mask 000
	0x0c, //ascr algo 0000
	0x00, //roi_en
	0x00, //roi_sx
	0x00, //roi_sx
	0x00, //roi_sy
	0x00, //roi_sy
	0x00, //roi_ex
	0x00, //roi_ex
	0x00, //roi_ey
	0x00, //roi_ey
	0x00, //trans_on trans_slope 0 0000
	0x00, //trans_interval
	//end
};

static char DSI_CAMERA_MDNIE_1[] ={
	//start
	0xDF,
	0x01, //linear_on ascr_skin_on 00
	0x67, //ascr_skin_cb
	0xa9, //ascr_skin_cr
	0x0c, //ascr_dist_up
	0x0c, //ascr_dist_down
	0x0c, //ascr_dist_left
	0x0c, //ascr_dist_right
	0x00, //ascr_div_up 20
	0xaa,
	0xab,
	0x00, //ascr_div_down
	0xaa,
	0xab,
	0x00, //ascr_div_right
	0xaa,
	0xab,
	0x00, //ascr_div_left
	0xaa,
	0xab,
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
};

static char DSI_CAMERA_MDNIE_2[] ={
	0xDE,
	0x00, //de cs gamma 000
	0x00, //de_gain 10
	0x00,
	0x07, //de_maxplus 11
	0xff,
	0x07, //de_maxminus 11
	0xff,
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
	0x00, //curve_offset
	0x00,
	0x00, //curve_low_x
	0x00, //curve_low_y
};

static char DSI_CAMERA_MDNIE_3[] ={
	0xDD,
	0x01, //mdnie_en
	0x00, //mask 000
	0x0c, //ascr algo 0000
	0x00, //roi_en
	0x00, //roi_sx
	0x00, //roi_sx
	0x00, //roi_sy
	0x00, //roi_sy
	0x00, //roi_ex
	0x00, //roi_ex
	0x00, //roi_ey
	0x00, //roi_ey
	0x00, //trans_on trans_slope 0 0000
	0x00, //trans_interval
	//end
};

static char DSI_CAMERA_DYNAMIC_MDNIE_1[] ={
	//start
	0xDF,
	0x02, //linear_on ascr_skin_on 00
	0x67, //ascr_skin_cb
	0xa9, //ascr_skin_cr
	0x37, //ascr_dist_up
	0x29, //ascr_dist_down
	0x47, //ascr_dist_left
	0x19, //ascr_dist_right
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
	0xe8, //ascr_skin_Rr
	0x00, //ascr_skin_Rg
	0x00, //ascr_skin_Rb
	0xf7, //ascr_skin_Yr
	0xed, //ascr_skin_Yg
	0x00, //ascr_skin_Yb
	0xf2, //ascr_skin_Mr
	0x21, //ascr_skin_Mg
	0xea, //ascr_skin_Mb
	0xff, //ascr_skin_Wr
	0xf7, //ascr_skin_Wg
	0xef, //ascr_skin_Wb
	0x5d, //ascr_Cr
	0xe8, //ascr_Rr
	0xf7, //ascr_Cg
	0x00, //ascr_Rg
	0xec, //ascr_Cb
	0x00, //ascr_Rb
	0xf2, //ascr_Mr
	0x56, //ascr_Gr
	0x21, //ascr_Mg
	0xee, //ascr_Gg
	0xea, //ascr_Mb
	0x00, //ascr_Gb
	0xf7, //ascr_Yr
	0x22, //ascr_Br
	0xed, //ascr_Yg
	0x37, //ascr_Bg
	0x00, //ascr_Yb
	0xea, //ascr_Bb
	0xff, //ascr_Wr
	0x00, //ascr_Kr
	0xf7, //ascr_Wg
	0x00, //ascr_Kg
	0xef, //ascr_Wb
	0x00, //ascr_Kb
};

static char DSI_CAMERA_DYNAMIC_MDNIE_2[] = {
	0xDE,
	0x07, //de cs gamma 000
	0x00, //de_gain 10
	0x00,
	0x02, //de_maxplus 11
	0x00,
	0x02, //de_maxminus 11
	0x00,
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
	0x00, //curve_offset
	0x00,
	0x00, //curve_low_x
	0x00, //curve_low_y
};

static char DSI_CAMERA_DYNAMIC_MDNIE_3[] = {
	0xDD,
	0x01, //mdnie_en
	0x00, //mask 000
	0x0f, //ascr algo 0000
	0x00, //roi_en
	0x00, //roi_sx
	0x00, //roi_sx
	0x00, //roi_sy
	0x00, //roi_sy
	0x00, //roi_ex
	0x00, //roi_ex
	0x00, //roi_ey
	0x00, //roi_ey
	0x14, //trans_on trans_slope 0 0000
	0x01, //trans_interval
	//end
};

static char DSI_CAMERA_STANDARD_MDNIE_1[] ={
	//start
	0xDF,
	0x02, //linear_on ascr_skin_on 00
	0x6a, //ascr_skin_cb
	0x9a, //ascr_skin_cr
	0x25, //ascr_dist_up
	0x1a, //ascr_dist_down
	0x2a, //ascr_dist_left
	0x16, //ascr_dist_right
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
	0xfd, //ascr_skin_Rr
	0x2e, //ascr_skin_Rg
	0x2e, //ascr_skin_Rb
	0xf8, //ascr_skin_Yr
	0xee, //ascr_skin_Yg
	0x40, //ascr_skin_Yb
	0xff, //ascr_skin_Mr
	0x4a, //ascr_skin_Mg
	0xe9, //ascr_skin_Mb
	0xff, //ascr_skin_Wr
	0xf7, //ascr_skin_Wg
	0xef, //ascr_skin_Wb
	0x00, //ascr_Cr
	0xfd, //ascr_Rr
	0xf0, //ascr_Cg
	0x2e, //ascr_Rg
	0xe9, //ascr_Cb
	0x2e, //ascr_Rb
	0xff, //ascr_Mr
	0x00, //ascr_Gr
	0x4a, //ascr_Mg
	0xe6, //ascr_Gg
	0xe9, //ascr_Mb
	0x2e, //ascr_Gb
	0xf8, //ascr_Yr
	0x27, //ascr_Br
	0xee, //ascr_Yg
	0x35, //ascr_Bg
	0x40, //ascr_Yb
	0xe4, //ascr_Bb
	0xff, //ascr_Wr
	0x00, //ascr_Kr
	0xf7, //ascr_Wg
	0x00, //ascr_Kg
	0xef, //ascr_Wb
	0x00, //ascr_Kb
};

static char DSI_CAMERA_STANDARD_MDNIE_2[] = {
	0xDE,
	0x07, //de cs gamma 000
	0x00, //de_gain 10
	0x00,
	0x02, //de_maxplus 11
	0x00,
	0x02, //de_maxminus 11
	0x00,
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
	0x00, //curve_offset
	0x00,
	0x00, //curve_low_x
	0x00, //curve_low_y
};

static char DSI_CAMERA_STANDARD_MDNIE_3[] = {
	0xDD,
	0x01, //mdnie_en
	0x00, //mask 000
	0x0f, //ascr algo 0000
	0x00, //roi_en
	0x00, //roi_sx
	0x00, //roi_sx
	0x00, //roi_sy
	0x00, //roi_sy
	0x00, //roi_ex
	0x00, //roi_ex
	0x00, //roi_ey
	0x00, //roi_ey
	0x14, //trans_on trans_slope 0 0000
	0x01, //trans_interval
	//end
};

static char DSI_CAMERA_NATURAL_MDNIE_1[] ={
	//start
	0xDF,
	0x02, //linear_on ascr_skin_on 00
	0x6a, //ascr_skin_cb
	0x9a, //ascr_skin_cr
	0x25, //ascr_dist_up
	0x1a, //ascr_dist_down
	0x2a, //ascr_dist_left
	0x16, //ascr_dist_right
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
	0xde, //ascr_skin_Rr
	0x24, //ascr_skin_Rg
	0x28, //ascr_skin_Rb
	0xf8, //ascr_skin_Yr
	0xef, //ascr_skin_Yg
	0x53, //ascr_skin_Yb
	0xe1, //ascr_skin_Mr
	0x41, //ascr_skin_Mg
	0xe3, //ascr_skin_Mb
	0xff, //ascr_skin_Wr
	0xf7, //ascr_skin_Wg
	0xef, //ascr_skin_Wb
	0x91, //ascr_Cr
	0xde, //ascr_Rr
	0xf3, //ascr_Cg
	0x24, //ascr_Rg
	0xea, //ascr_Cb
	0x28, //ascr_Rb
	0xe1, //ascr_Mr
	0x8a, //ascr_Gr
	0x41, //ascr_Mg
	0xec, //ascr_Gg
	0xe3, //ascr_Mb
	0x4a, //ascr_Gb
	0xf8, //ascr_Yr
	0x24, //ascr_Br
	0xef, //ascr_Yg
	0x34, //ascr_Bg
	0x53, //ascr_Yb
	0xe0, //ascr_Bb
	0xff, //ascr_Wr
	0x00, //ascr_Kr
	0xf7, //ascr_Wg
	0x00, //ascr_Kg
	0xef, //ascr_Wb
	0x00, //ascr_Kb
};

static char DSI_CAMERA_NATURAL_MDNIE_2[] = {
	0xDE,
	0x07, //de cs gamma 000
	0x00, //de_gain 10
	0x00,
	0x02, //de_maxplus 11
	0x00,
	0x02, //de_maxminus 11
	0x00,
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
	0x00, //curve_offset
	0x00,
	0x00, //curve_low_x
	0x00, //curve_low_y
};

static char DSI_CAMERA_NATURAL_MDNIE_3[] = {
	0xDD,
	0x01, //mdnie_en
	0x00, //mask 000
	0x0f, //ascr algo 0000
	0x00, //roi_en
	0x00, //roi_sx
	0x00, //roi_sx
	0x00, //roi_sy
	0x00, //roi_sy
	0x00, //roi_ex
	0x00, //roi_ex
	0x00, //roi_ey
	0x00, //roi_ey
	0x14, //trans_on trans_slope 0 0000
	0x01, //trans_interval
	//end
};

static char DSI_CAMERA_MOVIE_MDNIE_1[] ={
	//start
	0xDF,
	0x01, //linear_on ascr_skin_on 00
	0x67, //ascr_skin_cb
	0xa9, //ascr_skin_cr
	0x0c, //ascr_dist_up
	0x0c, //ascr_dist_down
	0x0c, //ascr_dist_left
	0x0c, //ascr_dist_right
	0x00, //ascr_div_up 20
	0xaa,
	0xab,
	0x00, //ascr_div_down
	0xaa,
	0xab,
	0x00, //ascr_div_right
	0xaa,
	0xab,
	0x00, //ascr_div_left
	0xaa,
	0xab,
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
};

static char DSI_CAMERA_MOVIE_MDNIE_2[] = {
	0xDE,
	0x00, //de cs gamma 000
	0x00, //de_gain 10
	0x00,
	0x07, //de_maxplus 11
	0xff,
	0x07, //de_maxminus 11
	0xff,
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
	0x00, //curve_offset
	0x00,
	0x00, //curve_low_x
	0x00, //curve_low_y
};

static char DSI_CAMERA_MOVIE_MDNIE_3[] = {
	0xDD,
	0x01, //mdnie_en
	0x00, //mask 000
	0x0c, //ascr algo 0000
	0x00, //roi_en
	0x00, //roi_sx
	0x00, //roi_sx
	0x00, //roi_sy
	0x00, //roi_sy
	0x00, //roi_ex
	0x00, //roi_ex
	0x00, //roi_ey
	0x00, //roi_ey
	0x00, //trans_on trans_slope 0 0000
	0x00, //trans_interval
	//end
};

static char DSI_CAMERA_AUTO_MDNIE_1[] ={
	//start
	0xDF,
	0x01, //linear_on ascr_skin_on 00
	0x6a, //ascr_skin_cb
	0x9a, //ascr_skin_cr
	0x25, //ascr_dist_up
	0x1a, //ascr_dist_down
	0x2a, //ascr_dist_left
	0x16, //ascr_dist_right
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
	0x2c, //ascr_skin_Rg
	0x40, //ascr_skin_Rb
	0xff, //ascr_skin_Yr
	0xf0, //ascr_skin_Yg
	0x00, //ascr_skin_Yb
	0xd8, //ascr_skin_Mr
	0x00, //ascr_skin_Mg
	0xd9, //ascr_skin_Mb
	0xff, //ascr_skin_Wr
	0xff, //ascr_skin_Wg
	0xff, //ascr_skin_Wb
	0x00, //ascr_Cr
	0xe0, //ascr_Rr
	0xff, //ascr_Cg
	0x00, //ascr_Rg
	0xf6, //ascr_Cb
	0x00, //ascr_Rb
	0xd8, //ascr_Mr
	0x3b, //ascr_Gr
	0x00, //ascr_Mg
	0xff, //ascr_Gg
	0xd9, //ascr_Mb
	0x00, //ascr_Gb
	0xff, //ascr_Yr
	0x14, //ascr_Br
	0xf9, //ascr_Yg
	0x00, //ascr_Bg
	0x00, //ascr_Yb
	0xff, //ascr_Bb
	0xff, //ascr_Wr
	0x00, //ascr_Kr
	0xff, //ascr_Wg
	0x00, //ascr_Kg
	0xff, //ascr_Wb
	0x00, //ascr_Kb
};

static char DSI_CAMERA_AUTO_MDNIE_2[] = {
	0xDE,
	0x07, //de cs gamma 000
	0x00, //de_gain 10
	0x00,
	0x02, //de_maxplus 11
	0x00,
	0x02, //de_maxminus 11
	0x00,
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
	0x00, //curve_offset
	0x00,
	0x00, //curve_low_x
	0x00, //curve_low_y
};

static char DSI_CAMERA_AUTO_MDNIE_3[] = {
	0xDD,
	0x01, //mdnie_en
	0x00, //mask 000
	0x0f, //ascr algo 0000
	0x00, //roi_en
	0x00, //roi_sx
	0x00, //roi_sx
	0x00, //roi_sy
	0x00, //roi_sy
	0x00, //roi_ex
	0x00, //roi_ex
	0x00, //roi_ey
	0x00, //roi_ey
	0x14, //trans_on trans_slope 0 0000
	0x01, //trans_interval
	//end
};

static char DSI_GALLERY_DYNAMIC_MDNIE_1[] ={
	//start
	0xDF,
	0x02, //linear_on ascr_skin_on 00
	0x67, //ascr_skin_cb
	0xa9, //ascr_skin_cr
	0x37, //ascr_dist_up
	0x29, //ascr_dist_down
	0x47, //ascr_dist_left
	0x19, //ascr_dist_right
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
	0xe8, //ascr_skin_Rr
	0x00, //ascr_skin_Rg
	0x00, //ascr_skin_Rb
	0xf7, //ascr_skin_Yr
	0xed, //ascr_skin_Yg
	0x00, //ascr_skin_Yb
	0xf2, //ascr_skin_Mr
	0x21, //ascr_skin_Mg
	0xea, //ascr_skin_Mb
	0xff, //ascr_skin_Wr
	0xf7, //ascr_skin_Wg
	0xef, //ascr_skin_Wb
	0x5d, //ascr_Cr
	0xe8, //ascr_Rr
	0xf7, //ascr_Cg
	0x00, //ascr_Rg
	0xec, //ascr_Cb
	0x00, //ascr_Rb
	0xf2, //ascr_Mr
	0x56, //ascr_Gr
	0x21, //ascr_Mg
	0xee, //ascr_Gg
	0xea, //ascr_Mb
	0x00, //ascr_Gb
	0xf7, //ascr_Yr
	0x22, //ascr_Br
	0xed, //ascr_Yg
	0x37, //ascr_Bg
	0x00, //ascr_Yb
	0xea, //ascr_Bb
	0xff, //ascr_Wr
	0x00, //ascr_Kr
	0xf7, //ascr_Wg
	0x00, //ascr_Kg
	0xef, //ascr_Wb
	0x00, //ascr_Kb
};

static char DSI_GALLERY_DYNAMIC_MDNIE_2[] = {
	0xDE,
	0x07, //de cs gamma 000
	0x00, //de_gain 10
	0x10,
	0x02, //de_maxplus 11
	0x00,
	0x02, //de_maxminus 11
	0x00,
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
	0x00, //curve_offset
	0x00,
	0x00, //curve_low_x
	0x00, //curve_low_y
};

static char DSI_GALLERY_DYNAMIC_MDNIE_3[] = {
	0xDD,
	0x01, //mdnie_en
	0x00, //mask 000
	0x0f, //ascr algo 0000
	0x00, //roi_en
	0x00, //roi_sx
	0x00, //roi_sx
	0x00, //roi_sy
	0x00, //roi_sy
	0x00, //roi_ex
	0x00, //roi_ex
	0x00, //roi_ey
	0x00, //roi_ey
	0x14, //trans_on trans_slope 0 0000
	0x01, //trans_interval
	//end
};

static char DSI_GALLERY_STANDARD_MDNIE_1[] ={
	//start
	0xDF,
	0x02, //linear_on ascr_skin_on 00
	0x6a, //ascr_skin_cb
	0x9a, //ascr_skin_cr
	0x25, //ascr_dist_up
	0x1a, //ascr_dist_down
	0x2a, //ascr_dist_left
	0x16, //ascr_dist_right
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
	0xfd, //ascr_skin_Rr
	0x2e, //ascr_skin_Rg
	0x2e, //ascr_skin_Rb
	0xf8, //ascr_skin_Yr
	0xee, //ascr_skin_Yg
	0x40, //ascr_skin_Yb
	0xff, //ascr_skin_Mr
	0x4a, //ascr_skin_Mg
	0xe9, //ascr_skin_Mb
	0xff, //ascr_skin_Wr
	0xf7, //ascr_skin_Wg
	0xef, //ascr_skin_Wb
	0x00, //ascr_Cr
	0xfd, //ascr_Rr
	0xf0, //ascr_Cg
	0x2e, //ascr_Rg
	0xe9, //ascr_Cb
	0x2e, //ascr_Rb
	0xff, //ascr_Mr
	0x00, //ascr_Gr
	0x4a, //ascr_Mg
	0xe6, //ascr_Gg
	0xe9, //ascr_Mb
	0x2e, //ascr_Gb
	0xf8, //ascr_Yr
	0x27, //ascr_Br
	0xee, //ascr_Yg
	0x35, //ascr_Bg
	0x40, //ascr_Yb
	0xe4, //ascr_Bb
	0xff, //ascr_Wr
	0x00, //ascr_Kr
	0xf7, //ascr_Wg
	0x00, //ascr_Kg
	0xef, //ascr_Wb
	0x00, //ascr_Kb
};

static char DSI_GALLERY_STANDARD_MDNIE_2[] = {
	0xDE,
	0x07, //de cs gamma 000
	0x00, //de_gain 10
	0x00,
	0x02, //de_maxplus 11
	0x00,
	0x02, //de_maxminus 11
	0x00,
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
	0x00, //curve_offset
	0x00,
	0x00, //curve_low_x
	0x00, //curve_low_y
};

static char DSI_GALLERY_STANDARD_MDNIE_3[] = {
	0xDD,
	0x01, //mdnie_en
	0x00, //mask 000
	0x0f, //ascr algo 0000
	0x00, //roi_en
	0x00, //roi_sx
	0x00, //roi_sx
	0x00, //roi_sy
	0x00, //roi_sy
	0x00, //roi_ex
	0x00, //roi_ex
	0x00, //roi_ey
	0x00, //roi_ey
	0x14, //trans_on trans_slope 0 0000
	0x01, //trans_interval
	//end
};

static char DSI_GALLERY_NATURAL_MDNIE_1[] = {
	//start
	0xDF,
	0x02, //linear_on ascr_skin_on 00
	0x6a, //ascr_skin_cb
	0x9a, //ascr_skin_cr
	0x25, //ascr_dist_up
	0x1a, //ascr_dist_down
	0x2a, //ascr_dist_left
	0x16, //ascr_dist_right
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
	0xde, //ascr_skin_Rr
	0x24, //ascr_skin_Rg
	0x28, //ascr_skin_Rb
	0xf8, //ascr_skin_Yr
	0xef, //ascr_skin_Yg
	0x53, //ascr_skin_Yb
	0xe1, //ascr_skin_Mr
	0x41, //ascr_skin_Mg
	0xe3, //ascr_skin_Mb
	0xff, //ascr_skin_Wr
	0xf7, //ascr_skin_Wg
	0xef, //ascr_skin_Wb
	0x91, //ascr_Cr
	0xde, //ascr_Rr
	0xf3, //ascr_Cg
	0x24, //ascr_Rg
	0xea, //ascr_Cb
	0x28, //ascr_Rb
	0xe1, //ascr_Mr
	0x8a, //ascr_Gr
	0x41, //ascr_Mg
	0xec, //ascr_Gg
	0xe3, //ascr_Mb
	0x4a, //ascr_Gb
	0xf8, //ascr_Yr
	0x24, //ascr_Br
	0xef, //ascr_Yg
	0x34, //ascr_Bg
	0x53, //ascr_Yb
	0xe0, //ascr_Bb
	0xff, //ascr_Wr
	0x00, //ascr_Kr
	0xf7, //ascr_Wg
	0x00, //ascr_Kg
	0xef, //ascr_Wb
	0x00, //ascr_Kb
};

static char DSI_GALLERY_NATURAL_MDNIE_2[] = {
	0xDE,
	0x07, //de cs gamma 000
	0x00, //de_gain 10
	0x00,
	0x02, //de_maxplus 11
	0x00,
	0x02, //de_maxminus 11
	0x00,
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
	0x00, //curve_offset
	0x00,
	0x00, //curve_low_x
	0x00, //curve_low_y
};

static char DSI_GALLERY_NATURAL_MDNIE_3[] = {
	0xDD,
	0x01, //mdnie_en
	0x00, //mask 000
	0x0f, //ascr algo 0000
	0x00, //roi_en
	0x00, //roi_sx
	0x00, //roi_sx
	0x00, //roi_sy
	0x00, //roi_sy
	0x00, //roi_ex
	0x00, //roi_ex
	0x00, //roi_ey
	0x00, //roi_ey
	0x14, //trans_on trans_slope 0 0000
	0x01, //trans_interval
	//end
};

static char DSI_GALLERY_MOVIE_MDNIE_1[] = {
	//start
	0xDF,
	0x01, //linear_on ascr_skin_on 00
	0x67, //ascr_skin_cb
	0xa9, //ascr_skin_cr
	0x0c, //ascr_dist_up
	0x0c, //ascr_dist_down
	0x0c, //ascr_dist_left
	0x0c, //ascr_dist_right
	0x00, //ascr_div_up 20
	0xaa,
	0xab,
	0x00, //ascr_div_down
	0xaa,
	0xab,
	0x00, //ascr_div_right
	0xaa,
	0xab,
	0x00, //ascr_div_left
	0xaa,
	0xab,
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
};

static char DSI_GALLERY_MOVIE_MDNIE_2[] = {
	0xDE,
	0x00, //de cs gamma 000
	0x00, //de_gain 10
	0x00,
	0x07, //de_maxplus 11
	0xff,
	0x07, //de_maxminus 11
	0xff,
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
	0x00, //curve_offset
	0x00,
	0x00, //curve_low_x
	0x00, //curve_low_y
};

static char DSI_GALLERY_MOVIE_MDNIE_3[] = {
	0xDD,
	0x01, //mdnie_en
	0x00, //mask 000
	0x0c, //ascr algo 0000
	0x00, //roi_en
	0x00, //roi_sx
	0x00, //roi_sx
	0x00, //roi_sy
	0x00, //roi_sy
	0x00, //roi_ex
	0x00, //roi_ex
	0x00, //roi_ey
	0x00, //roi_ey
	0x00, //trans_on trans_slope 0 0000
	0x00, //trans_interval
	//end
};

static char DSI_GALLERY_AUTO_MDNIE_1[] = {
	//start
	0xDF,
	0x01, //linear_on ascr_skin_on 00
	0x6a, //ascr_skin_cb
	0x9a, //ascr_skin_cr
	0x25, //ascr_dist_up
	0x1a, //ascr_dist_down
	0x2a, //ascr_dist_left
	0x16, //ascr_dist_right
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
	0x18, //ascr_skin_Rg
	0x30, //ascr_skin_Rb
	0xff, //ascr_skin_Yr
	0xf0, //ascr_skin_Yg
	0x00, //ascr_skin_Yb
	0xd8, //ascr_skin_Mr
	0x00, //ascr_skin_Mg
	0xd9, //ascr_skin_Mb
	0xff, //ascr_skin_Wr
	0xff, //ascr_skin_Wg
	0xff, //ascr_skin_Wb
	0x00, //ascr_Cr
	0xe0, //ascr_Rr
	0xff, //ascr_Cg
	0x00, //ascr_Rg
	0xf6, //ascr_Cb
	0x00, //ascr_Rb
	0xd8, //ascr_Mr
	0x3b, //ascr_Gr
	0x00, //ascr_Mg
	0xff, //ascr_Gg
	0xd9, //ascr_Mb
	0x00, //ascr_Gb
	0xff, //ascr_Yr
	0x14, //ascr_Br
	0xf9, //ascr_Yg
	0x00, //ascr_Bg
	0x00, //ascr_Yb
	0xff, //ascr_Bb
	0xff, //ascr_Wr
	0x00, //ascr_Kr
	0xff, //ascr_Wg
	0x00, //ascr_Kg
	0xff, //ascr_Wb
	0x00, //ascr_Kb
};

static char DSI_GALLERY_AUTO_MDNIE_2[] = {
	0xDE,
	0x07, //de cs gamma 000
	0x00, //de_gain 10
	0x10,
	0x02, //de_maxplus 11
	0x00,
	0x02, //de_maxminus 11
	0x00,
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
	0x00, //curve_offset
	0x00,
	0x00, //curve_low_x
	0x00, //curve_low_y
};

static char DSI_GALLERY_AUTO_MDNIE_3[] = {
	0xDD,
	0x01, //mdnie_en
	0x00, //mask 000
	0x0f, //ascr algo 0000
	0x00, //roi_en
	0x00, //roi_sx
	0x00, //roi_sx
	0x00, //roi_sy
	0x00, //roi_sy
	0x00, //roi_ex
	0x00, //roi_ex
	0x00, //roi_ey
	0x00, //roi_ey
	0x14, //trans_on trans_slope 0 0000
	0x01, //trans_interval
	//end
};

static char DSI_VT_DYNAMIC_MDNIE_1[] = {
	//start
	0xDF,
	0x02, //linear_on ascr_skin_on 00
	0x67, //ascr_skin_cb
	0xa9, //ascr_skin_cr
	0x37, //ascr_dist_up
	0x29, //ascr_dist_down
	0x47, //ascr_dist_left
	0x19, //ascr_dist_right
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
	0xe8, //ascr_skin_Rr
	0x00, //ascr_skin_Rg
	0x00, //ascr_skin_Rb
	0xf7, //ascr_skin_Yr
	0xed, //ascr_skin_Yg
	0x00, //ascr_skin_Yb
	0xf2, //ascr_skin_Mr
	0x21, //ascr_skin_Mg
	0xea, //ascr_skin_Mb
	0xff, //ascr_skin_Wr
	0xf7, //ascr_skin_Wg
	0xef, //ascr_skin_Wb
	0x5d, //ascr_Cr
	0xe8, //ascr_Rr
	0xf7, //ascr_Cg
	0x00, //ascr_Rg
	0xec, //ascr_Cb
	0x00, //ascr_Rb
	0xf2, //ascr_Mr
	0x56, //ascr_Gr
	0x21, //ascr_Mg
	0xee, //ascr_Gg
	0xea, //ascr_Mb
	0x00, //ascr_Gb
	0xf7, //ascr_Yr
	0x22, //ascr_Br
	0xed, //ascr_Yg
	0x37, //ascr_Bg
	0x00, //ascr_Yb
	0xea, //ascr_Bb
	0xff, //ascr_Wr
	0x00, //ascr_Kr
	0xf7, //ascr_Wg
	0x00, //ascr_Kg
	0xef, //ascr_Wb
	0x00, //ascr_Kb
};

static char DSI_VT_DYNAMIC_MDNIE_2[] = {
	0xDE,
	0x07, //de cs gamma 000
	0x00, //de_gain 10
	0x00,
	0x02, //de_maxplus 11
	0x00,
	0x02, //de_maxminus 11
	0x00,
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
	0x00, //curve_offset
	0x00,
	0x00, //curve_low_x
	0x00, //curve_low_y
};

static char DSI_VT_DYNAMIC_MDNIE_3[] = {
	0xDD,
	0x01, //mdnie_en
	0x00, //mask 000
	0x0f, //ascr algo 0000
	0x00, //roi_en
	0x00, //roi_sx
	0x00, //roi_sx
	0x00, //roi_sy
	0x00, //roi_sy
	0x00, //roi_ex
	0x00, //roi_ex
	0x00, //roi_ey
	0x00, //roi_ey
	0x14, //trans_on trans_slope 0 0000
	0x01, //trans_interval
	//end
};

static char DSI_VT_STANDARD_MDNIE_1[] = {
	//start
	0xDF,
	0x02, //linear_on ascr_skin_on 00
	0x6a, //ascr_skin_cb
	0x9a, //ascr_skin_cr
	0x25, //ascr_dist_up
	0x1a, //ascr_dist_down
	0x2a, //ascr_dist_left
	0x16, //ascr_dist_right
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
	0xfd, //ascr_skin_Rr
	0x2e, //ascr_skin_Rg
	0x2e, //ascr_skin_Rb
	0xf8, //ascr_skin_Yr
	0xee, //ascr_skin_Yg
	0x40, //ascr_skin_Yb
	0xff, //ascr_skin_Mr
	0x4a, //ascr_skin_Mg
	0xe9, //ascr_skin_Mb
	0xff, //ascr_skin_Wr
	0xf7, //ascr_skin_Wg
	0xef, //ascr_skin_Wb
	0x00, //ascr_Cr
	0xfd, //ascr_Rr
	0xf0, //ascr_Cg
	0x2e, //ascr_Rg
	0xe9, //ascr_Cb
	0x2e, //ascr_Rb
	0xff, //ascr_Mr
	0x00, //ascr_Gr
	0x4a, //ascr_Mg
	0xe6, //ascr_Gg
	0xe9, //ascr_Mb
	0x2e, //ascr_Gb
	0xf8, //ascr_Yr
	0x27, //ascr_Br
	0xee, //ascr_Yg
	0x35, //ascr_Bg
	0x40, //ascr_Yb
	0xe4, //ascr_Bb
	0xff, //ascr_Wr
	0x00, //ascr_Kr
	0xf7, //ascr_Wg
	0x00, //ascr_Kg
	0xef, //ascr_Wb
	0x00, //ascr_Kb
};

static char DSI_VT_STANDARD_MDNIE_2[] = {
	0xDE,
	0x07, //de cs gamma 000
	0x00, //de_gain 10
	0x00,
	0x02, //de_maxplus 11
	0x00,
	0x02, //de_maxminus 11
	0x00,
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
	0x00, //curve_offset
	0x00,
	0x00, //curve_low_x
	0x00, //curve_low_y
};

static char DSI_VT_STANDARD_MDNIE_3[] = {
	0xDD,
	0x01, //mdnie_en
	0x00, //mask 000
	0x0f, //ascr algo 0000
	0x00, //roi_en
	0x00, //roi_sx
	0x00, //roi_sx
	0x00, //roi_sy
	0x00, //roi_sy
	0x00, //roi_ex
	0x00, //roi_ex
	0x00, //roi_ey
	0x00, //roi_ey
	0x14, //trans_on trans_slope 0 0000
	0x01, //trans_interval
	//end
};

static char DSI_VT_NATURAL_MDNIE_1[] = {
	//start
	0xDF,
	0x02, //linear_on ascr_skin_on 00
	0x6a, //ascr_skin_cb
	0x9a, //ascr_skin_cr
	0x25, //ascr_dist_up
	0x1a, //ascr_dist_down
	0x2a, //ascr_dist_left
	0x16, //ascr_dist_right
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
	0xde, //ascr_skin_Rr
	0x24, //ascr_skin_Rg
	0x28, //ascr_skin_Rb
	0xf8, //ascr_skin_Yr
	0xef, //ascr_skin_Yg
	0x53, //ascr_skin_Yb
	0xe1, //ascr_skin_Mr
	0x41, //ascr_skin_Mg
	0xe3, //ascr_skin_Mb
	0xff, //ascr_skin_Wr
	0xf7, //ascr_skin_Wg
	0xef, //ascr_skin_Wb
	0x91, //ascr_Cr
	0xde, //ascr_Rr
	0xf3, //ascr_Cg
	0x24, //ascr_Rg
	0xea, //ascr_Cb
	0x28, //ascr_Rb
	0xe1, //ascr_Mr
	0x8a, //ascr_Gr
	0x41, //ascr_Mg
	0xec, //ascr_Gg
	0xe3, //ascr_Mb
	0x4a, //ascr_Gb
	0xf8, //ascr_Yr
	0x24, //ascr_Br
	0xef, //ascr_Yg
	0x34, //ascr_Bg
	0x53, //ascr_Yb
	0xe0, //ascr_Bb
	0xff, //ascr_Wr
	0x00, //ascr_Kr
	0xf7, //ascr_Wg
	0x00, //ascr_Kg
	0xef, //ascr_Wb
	0x00, //ascr_Kb
};

static char DSI_VT_NATURAL_MDNIE_2[] = {
	0xDE,
	0x07, //de cs gamma 000
	0x00, //de_gain 10
	0x00,
	0x02, //de_maxplus 11
	0x00,
	0x02, //de_maxminus 11
	0x00,
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
	0x00, //curve_offset
	0x00,
	0x00, //curve_low_x
	0x00, //curve_low_y
};

static char DSI_VT_NATURAL_MDNIE_3[] = {
	0xDD,
	0x01, //mdnie_en
	0x00, //mask 000
	0x0f, //ascr algo 0000
	0x00, //roi_en
	0x00, //roi_sx
	0x00, //roi_sx
	0x00, //roi_sy
	0x00, //roi_sy
	0x00, //roi_ex
	0x00, //roi_ex
	0x00, //roi_ey
	0x00, //roi_ey
	0x14, //trans_on trans_slope 0 0000
	0x01, //trans_interval
	//end
};

static char DSI_VT_MOVIE_MDNIE_1[] = {
	//start
	0xDF,
	0x01, //linear_on ascr_skin_on 00
	0x67, //ascr_skin_cb
	0xa9, //ascr_skin_cr
	0x0c, //ascr_dist_up
	0x0c, //ascr_dist_down
	0x0c, //ascr_dist_left
	0x0c, //ascr_dist_right
	0x00, //ascr_div_up 20
	0xaa,
	0xab,
	0x00, //ascr_div_down
	0xaa,
	0xab,
	0x00, //ascr_div_right
	0xaa,
	0xab,
	0x00, //ascr_div_left
	0xaa,
	0xab,
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
};

static char DSI_VT_MOVIE_MDNIE_2[] = {
	0xDE,
	0x00, //de cs gamma 000
	0x00, //de_gain 10
	0x00,
	0x07, //de_maxplus 11
	0xff,
	0x07, //de_maxminus 11
	0xff,
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
	0x00, //curve_offset
	0x00,
	0x00, //curve_low_x
	0x00, //curve_low_y
};

static char DSI_VT_MOVIE_MDNIE_3[] = {
	0xDD,
	0x01, //mdnie_en
	0x00, //mask 000
	0x0c, //ascr algo 0000
	0x00, //roi_en
	0x00, //roi_sx
	0x00, //roi_sx
	0x00, //roi_sy
	0x00, //roi_sy
	0x00, //roi_ex
	0x00, //roi_ex
	0x00, //roi_ey
	0x00, //roi_ey
	0x00, //trans_on trans_slope 0 0000
	0x00, //trans_interval
	//end
};

static char DSI_VT_AUTO_MDNIE_1[] = {
	//start
	0xDF,
	0x00, //linear_on ascr_skin_on 00
	0x6a, //ascr_skin_cb
	0x9a, //ascr_skin_cr
	0x25, //ascr_dist_up
	0x1a, //ascr_dist_down
	0x2a, //ascr_dist_left
	0x16, //ascr_dist_right
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
};

static char DSI_VT_AUTO_MDNIE_2[] = {
	0xDE,
	0x07, //de cs gamma 000
	0x00, //de_gain 10
	0x00,
	0x02, //de_maxplus 11
	0x00,
	0x02, //de_maxminus 11
	0x00,
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
	0x00, //curve_offset
	0x00,
	0x00, //curve_low_x
	0x00, //curve_low_y
};

static char DSI_VT_AUTO_MDNIE_3[] = {
	0xDD,
	0x01, //mdnie_en
	0x00, //mask 000
	0x0f, //ascr algo 0000
	0x00, //roi_en
	0x00, //roi_sx
	0x00, //roi_sx
	0x00, //roi_sy
	0x00, //roi_sy
	0x00, //roi_ex
	0x00, //roi_ex
	0x00, //roi_ey
	0x00, //roi_ey
	0x14, //trans_on trans_slope 0 0000
	0x01, //trans_interval
	//end
};

static char DSI_BROWSER_DYNAMIC_MDNIE_1[] = {
	//start
	0xDF,
	0x02, //linear_on ascr_skin_on 00
	0x67, //ascr_skin_cb
	0xa9, //ascr_skin_cr
	0x37, //ascr_dist_up
	0x29, //ascr_dist_down
	0x47, //ascr_dist_left
	0x19, //ascr_dist_right
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
	0xe8, //ascr_skin_Rr
	0x00, //ascr_skin_Rg
	0x00, //ascr_skin_Rb
	0xf7, //ascr_skin_Yr
	0xed, //ascr_skin_Yg
	0x00, //ascr_skin_Yb
	0xf2, //ascr_skin_Mr
	0x21, //ascr_skin_Mg
	0xea, //ascr_skin_Mb
	0xff, //ascr_skin_Wr
	0xf7, //ascr_skin_Wg
	0xef, //ascr_skin_Wb
	0x5d, //ascr_Cr
	0xe8, //ascr_Rr
	0xf7, //ascr_Cg
	0x00, //ascr_Rg
	0xec, //ascr_Cb
	0x00, //ascr_Rb
	0xf2, //ascr_Mr
	0x56, //ascr_Gr
	0x21, //ascr_Mg
	0xee, //ascr_Gg
	0xea, //ascr_Mb
	0x00, //ascr_Gb
	0xf7, //ascr_Yr
	0x22, //ascr_Br
	0xed, //ascr_Yg
	0x37, //ascr_Bg
	0x00, //ascr_Yb
	0xea, //ascr_Bb
	0xff, //ascr_Wr
	0x00, //ascr_Kr
	0xf7, //ascr_Wg
	0x00, //ascr_Kg
	0xef, //ascr_Wb
	0x00, //ascr_Kb
};

static char DSI_BROWSER_DYNAMIC_MDNIE_2[] = {
	0xDE,
	0x07, //de cs gamma 000
	0x00, //de_gain 10
	0x00,
	0x02, //de_maxplus 11
	0x00,
	0x02, //de_maxminus 11
	0x00,
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
	0x00, //curve_offset
	0x00,
	0x00, //curve_low_x
	0x00, //curve_low_y
};

static char DSI_BROWSER_DYNAMIC_MDNIE_3[] = {
	0xDD,
	0x01, //mdnie_en
	0x00, //mask 000
	0x0f, //ascr algo 0000
	0x00, //roi_en
	0x00, //roi_sx
	0x00, //roi_sx
	0x00, //roi_sy
	0x00, //roi_sy
	0x00, //roi_ex
	0x00, //roi_ex
	0x00, //roi_ey
	0x00, //roi_ey
	0x14, //trans_on trans_slope 0 0000
	0x01, //trans_interval
	//end
};

static char DSI_BROWSER_STANDARD_MDNIE_1[] = {
	//start
	0xDF,
	0x02, //linear_on ascr_skin_on 00
	0x6a, //ascr_skin_cb
	0x9a, //ascr_skin_cr
	0x25, //ascr_dist_up
	0x1a, //ascr_dist_down
	0x2a, //ascr_dist_left
	0x16, //ascr_dist_right
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
	0xfd, //ascr_skin_Rr
	0x2e, //ascr_skin_Rg
	0x2e, //ascr_skin_Rb
	0xf8, //ascr_skin_Yr
	0xee, //ascr_skin_Yg
	0x40, //ascr_skin_Yb
	0xff, //ascr_skin_Mr
	0x4a, //ascr_skin_Mg
	0xe9, //ascr_skin_Mb
	0xff, //ascr_skin_Wr
	0xf7, //ascr_skin_Wg
	0xef, //ascr_skin_Wb
	0x00, //ascr_Cr
	0xfd, //ascr_Rr
	0xf0, //ascr_Cg
	0x2e, //ascr_Rg
	0xe9, //ascr_Cb
	0x2e, //ascr_Rb
	0xff, //ascr_Mr
	0x00, //ascr_Gr
	0x4a, //ascr_Mg
	0xe6, //ascr_Gg
	0xe9, //ascr_Mb
	0x2e, //ascr_Gb
	0xf8, //ascr_Yr
	0x27, //ascr_Br
	0xee, //ascr_Yg
	0x35, //ascr_Bg
	0x40, //ascr_Yb
	0xe4, //ascr_Bb
	0xff, //ascr_Wr
	0x00, //ascr_Kr
	0xf7, //ascr_Wg
	0x00, //ascr_Kg
	0xef, //ascr_Wb
	0x00, //ascr_Kb
};

static char DSI_BROWSER_STANDARD_MDNIE_2[] = {
	0xDE,
	0x07, //de cs gamma 000
	0x00, //de_gain 10
	0x00,
	0x02, //de_maxplus 11
	0x00,
	0x02, //de_maxminus 11
	0x00,
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
	0x00, //curve_offset
	0x00,
	0x00, //curve_low_x
	0x00, //curve_low_y
};

static char DSI_BROWSER_STANDARD_MDNIE_3[] = {
	0xDD,
	0x01, //mdnie_en
	0x00, //mask 000
	0x0f, //ascr algo 0000
	0x00, //roi_en
	0x00, //roi_sx
	0x00, //roi_sx
	0x00, //roi_sy
	0x00, //roi_sy
	0x00, //roi_ex
	0x00, //roi_ex
	0x00, //roi_ey
	0x00, //roi_ey
	0x14, //trans_on trans_slope 0 0000
	0x01, //trans_interval
	//end
};

static char DSI_BROWSER_NATURAL_MDNIE_1[] = {
	//start
	0xDF,
	0x02, //linear_on ascr_skin_on 00
	0x6a, //ascr_skin_cb
	0x9a, //ascr_skin_cr
	0x25, //ascr_dist_up
	0x1a, //ascr_dist_down
	0x2a, //ascr_dist_left
	0x16, //ascr_dist_right
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
	0xde, //ascr_skin_Rr
	0x24, //ascr_skin_Rg
	0x28, //ascr_skin_Rb
	0xf8, //ascr_skin_Yr
	0xef, //ascr_skin_Yg
	0x53, //ascr_skin_Yb
	0xe1, //ascr_skin_Mr
	0x41, //ascr_skin_Mg
	0xe3, //ascr_skin_Mb
	0xff, //ascr_skin_Wr
	0xf7, //ascr_skin_Wg
	0xef, //ascr_skin_Wb
	0x91, //ascr_Cr
	0xde, //ascr_Rr
	0xf3, //ascr_Cg
	0x24, //ascr_Rg
	0xea, //ascr_Cb
	0x28, //ascr_Rb
	0xe1, //ascr_Mr
	0x8a, //ascr_Gr
	0x41, //ascr_Mg
	0xec, //ascr_Gg
	0xe3, //ascr_Mb
	0x4a, //ascr_Gb
	0xf8, //ascr_Yr
	0x24, //ascr_Br
	0xef, //ascr_Yg
	0x34, //ascr_Bg
	0x53, //ascr_Yb
	0xe0, //ascr_Bb
	0xff, //ascr_Wr
	0x00, //ascr_Kr
	0xf7, //ascr_Wg
	0x00, //ascr_Kg
	0xef, //ascr_Wb
	0x00, //ascr_Kb
};

static char DSI_BROWSER_NATURAL_MDNIE_2[] = {
	0xDE,
	0x07, //de cs gamma 000
	0x00, //de_gain 10
	0x00,
	0x02, //de_maxplus 11
	0x00,
	0x02, //de_maxminus 11
	0x00,
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
	0x00, //curve_offset
	0x00,
	0x00, //curve_low_x
	0x00, //curve_low_y
};

static char DSI_BROWSER_NATURAL_MDNIE_3[] = {
	0xDD,
	0x01, //mdnie_en
	0x00, //mask 000
	0x0f, //ascr algo 0000
	0x00, //roi_en
	0x00, //roi_sx
	0x00, //roi_sx
	0x00, //roi_sy
	0x00, //roi_sy
	0x00, //roi_ex
	0x00, //roi_ex
	0x00, //roi_ey
	0x00, //roi_ey
	0x14, //trans_on trans_slope 0 0000
	0x01, //trans_interval
	//end
};

static char DSI_BROWSER_MOVIE_MDNIE_1[] = {
	//start
	0xDF,
	0x01, //linear_on ascr_skin_on 00
	0x67, //ascr_skin_cb
	0xa9, //ascr_skin_cr
	0x0c, //ascr_dist_up
	0x0c, //ascr_dist_down
	0x0c, //ascr_dist_left
	0x0c, //ascr_dist_right
	0x00, //ascr_div_up 20
	0xaa,
	0xab,
	0x00, //ascr_div_down
	0xaa,
	0xab,
	0x00, //ascr_div_right
	0xaa,
	0xab,
	0x00, //ascr_div_left
	0xaa,
	0xab,
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
};

static char DSI_BROWSER_MOVIE_MDNIE_2[] = {
	0xDE,
	0x00, //de cs gamma 000
	0x00, //de_gain 10
	0x00,
	0x07, //de_maxplus 11
	0xff,
	0x07, //de_maxminus 11
	0xff,
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
	0x00, //curve_offset
	0x00,
	0x00, //curve_low_x
	0x00, //curve_low_y
};

static char DSI_BROWSER_MOVIE_MDNIE_3[] = {
	0xDD,
	0x01, //mdnie_en
	0x00, //mask 000
	0x0c, //ascr algo 0000
	0x00, //roi_en
	0x00, //roi_sx
	0x00, //roi_sx
	0x00, //roi_sy
	0x00, //roi_sy
	0x00, //roi_ex
	0x00, //roi_ex
	0x00, //roi_ey
	0x00, //roi_ey
	0x00, //trans_on trans_slope 0 0000
	0x00, //trans_interval
	//end
};

static char DSI_BROWSER_AUTO_MDNIE_1[] = {
	//start
	0xDF,
	0x01, //linear_on ascr_skin_on 00
	0x6a, //ascr_skin_cb
	0x9a, //ascr_skin_cr
	0x25, //ascr_dist_up
	0x1a, //ascr_dist_down
	0x2a, //ascr_dist_left
	0x16, //ascr_dist_right
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
	0x34, //ascr_skin_Rg
	0x44, //ascr_skin_Rb
	0xff, //ascr_skin_Yr
	0xff, //ascr_skin_Yg
	0x00, //ascr_skin_Yb
	0xff, //ascr_skin_Mr
	0x00, //ascr_skin_Mg
	0xff, //ascr_skin_Mb
	0xff, //ascr_skin_Wr
	0xf8, //ascr_skin_Wg
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
};

static char DSI_BROWSER_AUTO_MDNIE_2[] = {
	0xDE,
	0x07, //de cs gamma 000
	0x00, //de_gain 10
	0x00,
	0x02, //de_maxplus 11
	0x00,
	0x02, //de_maxminus 11
	0x00,
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
	0x00, //curve_offset
	0x00,
	0x00, //curve_low_x
	0x00, //curve_low_y
};

static char DSI_BROWSER_AUTO_MDNIE_3[] = {
	0xDD,
	0x01, //mdnie_en
	0x00, //mask 000
	0x0f, //ascr algo 0000
	0x00, //roi_en
	0x00, //roi_sx
	0x00, //roi_sx
	0x00, //roi_sy
	0x00, //roi_sy
	0x00, //roi_ex
	0x00, //roi_ex
	0x00, //roi_ey
	0x00, //roi_ey
	0x14, //trans_on trans_slope 0 0000
	0x01, //trans_interval
	//end
};

static char DSI_EBOOK_DYNAMIC_MDNIE_1[] = {
	//start
	0xDF,
	0x02, //linear_on ascr_skin_on 00
	0x67, //ascr_skin_cb
	0xa9, //ascr_skin_cr
	0x37, //ascr_dist_up
	0x29, //ascr_dist_down
	0x47, //ascr_dist_left
	0x19, //ascr_dist_right
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
	0xe8, //ascr_skin_Rr
	0x00, //ascr_skin_Rg
	0x00, //ascr_skin_Rb
	0xf7, //ascr_skin_Yr
	0xed, //ascr_skin_Yg
	0x00, //ascr_skin_Yb
	0xf2, //ascr_skin_Mr
	0x21, //ascr_skin_Mg
	0xea, //ascr_skin_Mb
	0xff, //ascr_skin_Wr
	0xf7, //ascr_skin_Wg
	0xef, //ascr_skin_Wb
	0x5d, //ascr_Cr
	0xe8, //ascr_Rr
	0xf7, //ascr_Cg
	0x00, //ascr_Rg
	0xec, //ascr_Cb
	0x00, //ascr_Rb
	0xf2, //ascr_Mr
	0x56, //ascr_Gr
	0x21, //ascr_Mg
	0xee, //ascr_Gg
	0xea, //ascr_Mb
	0x00, //ascr_Gb
	0xf7, //ascr_Yr
	0x22, //ascr_Br
	0xed, //ascr_Yg
	0x37, //ascr_Bg
	0x00, //ascr_Yb
	0xea, //ascr_Bb
	0xff, //ascr_Wr
	0x00, //ascr_Kr
	0xf7, //ascr_Wg
	0x00, //ascr_Kg
	0xef, //ascr_Wb
	0x00, //ascr_Kb
};

static char DSI_EBOOK_DYNAMIC_MDNIE_2[] = {
	0xDE,
	0x07, //de cs gamma 000
	0x00, //de_gain 10
	0x00,
	0x02, //de_maxplus 11
	0x00,
	0x02, //de_maxminus 11
	0x00,
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
	0x00, //curve_offset
	0x00,
	0x00, //curve_low_x
	0x00, //curve_low_y
};

static char DSI_EBOOK_DYNAMIC_MDNIE_3[] = {
	0xDD,
	0x01, //mdnie_en
	0x00, //mask 000
	0x0f, //ascr algo 0000
	0x00, //roi_en
	0x00, //roi_sx
	0x00, //roi_sx
	0x00, //roi_sy
	0x00, //roi_sy
	0x00, //roi_ex
	0x00, //roi_ex
	0x00, //roi_ey
	0x00, //roi_ey
	0x14, //trans_on trans_slope 0 0000
	0x01, //trans_interval
	//end
};

static char DSI_EBOOK_STANDARD_MDNIE_1[] = {
	//start
	0xDF,
	0x02, //linear_on ascr_skin_on 00
	0x6a, //ascr_skin_cb
	0x9a, //ascr_skin_cr
	0x25, //ascr_dist_up
	0x1a, //ascr_dist_down
	0x2a, //ascr_dist_left
	0x16, //ascr_dist_right
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
	0xfd, //ascr_skin_Rr
	0x2e, //ascr_skin_Rg
	0x2e, //ascr_skin_Rb
	0xf8, //ascr_skin_Yr
	0xee, //ascr_skin_Yg
	0x40, //ascr_skin_Yb
	0xff, //ascr_skin_Mr
	0x4a, //ascr_skin_Mg
	0xe9, //ascr_skin_Mb
	0xff, //ascr_skin_Wr
	0xf7, //ascr_skin_Wg
	0xef, //ascr_skin_Wb
	0x00, //ascr_Cr
	0xfd, //ascr_Rr
	0xf0, //ascr_Cg
	0x2e, //ascr_Rg
	0xe9, //ascr_Cb
	0x2e, //ascr_Rb
	0xff, //ascr_Mr
	0x00, //ascr_Gr
	0x4a, //ascr_Mg
	0xe6, //ascr_Gg
	0xe9, //ascr_Mb
	0x2e, //ascr_Gb
	0xf8, //ascr_Yr
	0x27, //ascr_Br
	0xee, //ascr_Yg
	0x35, //ascr_Bg
	0x40, //ascr_Yb
	0xe4, //ascr_Bb
	0xff, //ascr_Wr
	0x00, //ascr_Kr
	0xf7, //ascr_Wg
	0x00, //ascr_Kg
	0xef, //ascr_Wb
	0x00, //ascr_Kb
};

static char DSI_EBOOK_STANDARD_MDNIE_2[] = {
	0xDE,
	0x07, //de cs gamma 000
	0x00, //de_gain 10
	0x00,
	0x02, //de_maxplus 11
	0x00,
	0x02, //de_maxminus 11
	0x00,
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
	0x00, //curve_offset
	0x00,
	0x00, //curve_low_x
	0x00, //curve_low_y
};

static char DSI_EBOOK_STANDARD_MDNIE_3[] = {
	0xDD,
	0x01, //mdnie_en
	0x00, //mask 000
	0x0f, //ascr algo 0000
	0x00, //roi_en
	0x00, //roi_sx
	0x00, //roi_sx
	0x00, //roi_sy
	0x00, //roi_sy
	0x00, //roi_ex
	0x00, //roi_ex
	0x00, //roi_ey
	0x00, //roi_ey
	0x14, //trans_on trans_slope 0 0000
	0x01, //trans_interval
	//end
};

static char DSI_EBOOK_NATURAL_MDNIE_1[] = {
	//start
	0xDF,
	0x02, //linear_on ascr_skin_on 00
	0x6a, //ascr_skin_cb
	0x9a, //ascr_skin_cr
	0x25, //ascr_dist_up
	0x1a, //ascr_dist_down
	0x2a, //ascr_dist_left
	0x16, //ascr_dist_right
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
	0xde, //ascr_skin_Rr
	0x24, //ascr_skin_Rg
	0x28, //ascr_skin_Rb
	0xf8, //ascr_skin_Yr
	0xef, //ascr_skin_Yg
	0x53, //ascr_skin_Yb
	0xe1, //ascr_skin_Mr
	0x41, //ascr_skin_Mg
	0xe3, //ascr_skin_Mb
	0xff, //ascr_skin_Wr
	0xf7, //ascr_skin_Wg
	0xef, //ascr_skin_Wb
	0x91, //ascr_Cr
	0xde, //ascr_Rr
	0xf3, //ascr_Cg
	0x24, //ascr_Rg
	0xea, //ascr_Cb
	0x28, //ascr_Rb
	0xe1, //ascr_Mr
	0x8a, //ascr_Gr
	0x41, //ascr_Mg
	0xec, //ascr_Gg
	0xe3, //ascr_Mb
	0x4a, //ascr_Gb
	0xf8, //ascr_Yr
	0x24, //ascr_Br
	0xef, //ascr_Yg
	0x34, //ascr_Bg
	0x53, //ascr_Yb
	0xe0, //ascr_Bb
	0xff, //ascr_Wr
	0x00, //ascr_Kr
	0xf7, //ascr_Wg
	0x00, //ascr_Kg
	0xef, //ascr_Wb
	0x00, //ascr_Kb
};

static char DSI_EBOOK_NATURAL_MDNIE_2[] = {
	0xDE,
	0x07, //de cs gamma 000
	0x00, //de_gain 10
	0x00,
	0x02, //de_maxplus 11
	0x00,
	0x02, //de_maxminus 11
	0x00,
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
	0x00, //curve_offset
	0x00,
	0x00, //curve_low_x
	0x00, //curve_low_y
};

static char DSI_EBOOK_NATURAL_MDNIE_3[] = {
	0xDD,
	0x01, //mdnie_en
	0x00, //mask 000
	0x0f, //ascr algo 0000
	0x00, //roi_en
	0x00, //roi_sx
	0x00, //roi_sx
	0x00, //roi_sy
	0x00, //roi_sy
	0x00, //roi_ex
	0x00, //roi_ex
	0x00, //roi_ey
	0x00, //roi_ey
	0x14, //trans_on trans_slope 0 0000
	0x01, //trans_interval
	//end
};

static char DSI_EBOOK_MOVIE_MDNIE_1[] = {
	//start
	0xDF,
	0x01, //linear_on ascr_skin_on 00
	0x67, //ascr_skin_cb
	0xa9, //ascr_skin_cr
	0x0c, //ascr_dist_up
	0x0c, //ascr_dist_down
	0x0c, //ascr_dist_left
	0x0c, //ascr_dist_right
	0x00, //ascr_div_up 20
	0xaa,
	0xab,
	0x00, //ascr_div_down
	0xaa,
	0xab,
	0x00, //ascr_div_right
	0xaa,
	0xab,
	0x00, //ascr_div_left
	0xaa,
	0xab,
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
};

static char DSI_EBOOK_MOVIE_MDNIE_2[] = {
	0xDE,
	0x00, //de cs gamma 000
	0x00, //de_gain 10
	0x00,
	0x07, //de_maxplus 11
	0xff,
	0x07, //de_maxminus 11
	0xff,
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
	0x00, //curve_offset
	0x00,
	0x00, //curve_low_x
	0x00, //curve_low_y
};

static char DSI_EBOOK_MOVIE_MDNIE_3[] = {
	0xDD,
	0x01, //mdnie_en
	0x00, //mask 000
	0x0c, //ascr algo 0000
	0x00, //roi_en
	0x00, //roi_sx
	0x00, //roi_sx
	0x00, //roi_sy
	0x00, //roi_sy
	0x00, //roi_ex
	0x00, //roi_ex
	0x00, //roi_ey
	0x00, //roi_ey
	0x00, //trans_on trans_slope 0 0000
	0x00, //trans_interval
	//end
};

static char DSI_EBOOK_AUTO_MDNIE_1[] = {
	//start
	0xDF,
	0x00, //linear_on ascr_skin_on 00
	0x6a, //ascr_skin_cb
	0x9a, //ascr_skin_cr
	0x25, //ascr_dist_up
	0x1a, //ascr_dist_down
	0x2a, //ascr_dist_left
	0x16, //ascr_dist_right
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
	0xf6, //ascr_Wg
	0x00, //ascr_Kg
	0xe6, //ascr_Wb
	0x00, //ascr_Kb
};

static char DSI_EBOOK_AUTO_MDNIE_2[] = {
	0xDE,
	0x07, //de cs gamma 000
	0x00, //de_gain 10
	0x00,
	0x02, //de_maxplus 11
	0x00,
	0x02, //de_maxminus 11
	0x00,
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
	0x00, //curve_offset
	0x00,
	0x00, //curve_low_x
	0x00, //curve_low_y
};

static char DSI_EBOOK_AUTO_MDNIE_3[] = {
	0xDD,
	0x01, //mdnie_en
	0x00, //mask 000
	0x0f, //ascr algo 0000
	0x00, //roi_en
	0x00, //roi_sx
	0x00, //roi_sx
	0x00, //roi_sy
	0x00, //roi_sy
	0x00, //roi_ex
	0x00, //roi_ex
	0x00, //roi_ey
	0x00, //roi_ey
	0x14, //trans_on trans_slope 0 0000
	0x01, //trans_interval
	//end
};

static char DSI_EMAIL_AUTO_MDNIE_1[] = {
	//start
	0xDF,
	0x00, //linear_on ascr_skin_on 00
	0x6a, //ascr_skin_cb
	0x9a, //ascr_skin_cr
	0x25, //ascr_dist_up
	0x1a, //ascr_dist_down
	0x2a, //ascr_dist_left
	0x16, //ascr_dist_right
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
	0xfb, //ascr_Wg
	0x00, //ascr_Kg
	0xee, //ascr_Wb
	0x00, //ascr_Kb
};

static char DSI_EMAIL_AUTO_MDNIE_2[] = {
	0xDE,
	0x07, //de cs gamma 000
	0x00, //de_gain 10
	0x00,
	0x02, //de_maxplus 11
	0x00,
	0x02, //de_maxminus 11
	0x00,
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
	0x00, //curve_offset
	0x00,
	0x00, //curve_low_x
	0x00, //curve_low_y
};

static char DSI_EMAIL_AUTO_MDNIE_3[] = {
	0xDD,
	0x01, //mdnie_en
	0x00, //mask 000
	0x0f, //ascr algo 0000
	0x00, //roi_en
	0x00, //roi_sx
	0x00, //roi_sx
	0x00, //roi_sy
	0x00, //roi_sy
	0x00, //roi_ex
	0x00, //roi_ex
	0x00, //roi_ey
	0x00, //roi_ey
	0x14, //trans_on trans_slope 0 0000
	0x01, //trans_interval
	//end
};

static unsigned char DSI_GAME_LOW_MDNIE_1[] = {
	0xDF,
	0x00, //linear_on ascr_skin_on 00
	0x6a, //ascr_skin_cb
	0x9a, //ascr_skin_cr
	0x25, //ascr_dist_up
	0x1a, //ascr_dist_down
	0x2a, //ascr_dist_left
	0x16, //ascr_dist_right
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
};

static unsigned char DSI_GAME_LOW_MDNIE_2[] = {
	0xDE,
	0x01, //de cs gamma 000
	0x00, //de_gain 10
	0x00,
	0x00, //de_maxplus 11
	0x00,
	0x00, //de_maxminus 11
	0x00,
	0x01, //cs_gain 10
	0x00,
	0x00, //curve_0
	0x04, //curve_1
	0x0a, //curve_2
	0x0f, //curve_3
	0x1d, //curve_4
	0x2b, //curve_5
	0x3a, //curve_6
	0x49, //curve_7
	0x58, //curve_8
	0x67, //curve_9
	0x76, //curve_10
	0x85, //curve_11
	0x94, //curve_12
	0xa3, //curve_13
	0xb2, //curve_14
	0xc1, //curve_15
	0x00, //curve_16
	0xd0,
	0x00, //curve_offset
	0x00,
	0x00, //curve_low_x
	0x00, //curve_low_y
};

static unsigned char DSI_GAME_LOW_MDNIE_3[] = {
	0xDD,
	0x01, //mdnie_en
	0x00, //mask 000
	0x0f, //ascr algo aolce 00 00 00
	0x00, //roi_en
	0x00, //roi_sx
	0x00, //roi_sx
	0x00, //roi_sy
	0x00, //roi_sy
	0x00, //roi_ex
	0x00, //roi_ex
	0x00, //roi_ey
	0x00, //roi_ey
	0x00, //trans_on trans_slope 0 0000
	0x00, //trans_interval
};

static unsigned char DSI_GAME_MID_MDNIE_1[] = {
	0xDF,
	0x00, //linear_on ascr_skin_on 00
	0x6a, //ascr_skin_cb
	0x9a, //ascr_skin_cr
	0x25, //ascr_dist_up
	0x1a, //ascr_dist_down
	0x2a, //ascr_dist_left
	0x16, //ascr_dist_right
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
};

static unsigned char DSI_GAME_MID_MDNIE_2[] = {
	0xDE,
	0x01, //de cs gamma 000
	0x00, //de_gain 10
	0x00,
	0x00, //de_maxplus 11
	0x00,
	0x00, //de_maxminus 11
	0x00,
	0x01, //cs_gain 10
	0x00,
	0x00, //curve_0
	0x04, //curve_1
	0x0a, //curve_2
	0x0f, //curve_3
	0x1d, //curve_4
	0x2b, //curve_5
	0x3a, //curve_6
	0x49, //curve_7
	0x58, //curve_8
	0x67, //curve_9
	0x76, //curve_10
	0x85, //curve_11
	0x94, //curve_12
	0xa3, //curve_13
	0xb2, //curve_14
	0xc1, //curve_15
	0x00, //curve_16
	0xd0,
	0x00, //curve_offset
	0x00,
	0x00, //curve_low_x
	0x00, //curve_low_y
};

static unsigned char DSI_GAME_MID_MDNIE_3[] = {
	0xDD,
	0x01, //mdnie_en
	0x00, //mask 000
	0x0f, //ascr algo aolce 00 00 00
	0x00, //roi_en
	0x00, //roi_sx
	0x00, //roi_sx
	0x00, //roi_sy
	0x00, //roi_sy
	0x00, //roi_ex
	0x00, //roi_ex
	0x00, //roi_ey
	0x00, //roi_ey
	0x00, //trans_on trans_slope 0 0000
	0x00, //trans_interval
};

static unsigned char DSI_GAME_HIGH_MDNIE_1[] = {
	0xDF,
	0x00, //linear_on ascr_skin_on 00
	0x6a, //ascr_skin_cb
	0x9a, //ascr_skin_cr
	0x25, //ascr_dist_up
	0x1a, //ascr_dist_down
	0x2a, //ascr_dist_left
	0x16, //ascr_dist_right
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
};

static unsigned char DSI_GAME_HIGH_MDNIE_2[] = {
	0xDE,
	0x01, //de cs gamma 000
	0x00, //de_gain 10
	0x00,
	0x00, //de_maxplus 11
	0x00,
	0x00, //de_maxminus 11
	0x00,
	0x01, //cs_gain 10
	0x00,
	0x00, //curve_0
	0x06, //curve_1
	0x14, //curve_2
	0x1d, //curve_3
	0x26, //curve_4
	0x32, //curve_5
	0x42, //curve_6
	0x52, //curve_7
	0x62, //curve_8
	0x72, //curve_9
	0x82, //curve_10
	0x92, //curve_11
	0xa2, //curve_12
	0xb2, //curve_13
	0xc2, //curve_14
	0xd2, //curve_15
	0x00, //curve_16
	0xe2,
	0x00, //curve_offset
	0x00,
	0x00, //curve_low_x
	0x00, //curve_low_y
};

static unsigned char DSI_GAME_HIGH_MDNIE_3[] = {
	0xDD,
	0x01, //mdnie_en
	0x00, //mask 000
	0x0f, //ascr algo aolce 00 00 00
	0x00, //roi_en
	0x00, //roi_sx
	0x00, //roi_sx
	0x00, //roi_sy
	0x00, //roi_sy
	0x00, //roi_ex
	0x00, //roi_ex
	0x00, //roi_ey
	0x00, //roi_ey
	0x00, //trans_on trans_slope 0 0000
	0x00, //trans_interval
};

static char DSI_TDMB_STANDARD_MDNIE_1[] ={
	//start
	0xDF,
	0x02, //linear_on ascr_skin_on 00
	0x6a, //ascr_skin_cb
	0x9a, //ascr_skin_cr
	0x25, //ascr_dist_up
	0x1a, //ascr_dist_down
	0x2a, //ascr_dist_left
	0x16, //ascr_dist_right
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
	0xfd, //ascr_skin_Rr
	0x2e, //ascr_skin_Rg
	0x2e, //ascr_skin_Rb
	0xf8, //ascr_skin_Yr
	0xee, //ascr_skin_Yg
	0x40, //ascr_skin_Yb
	0xff, //ascr_skin_Mr
	0x4a, //ascr_skin_Mg
	0xe9, //ascr_skin_Mb
	0xff, //ascr_skin_Wr
	0xf7, //ascr_skin_Wg
	0xef, //ascr_skin_Wb
	0x00, //ascr_Cr
	0xfd, //ascr_Rr
	0xf0, //ascr_Cg
	0x2e, //ascr_Rg
	0xe9, //ascr_Cb
	0x2e, //ascr_Rb
	0xff, //ascr_Mr
	0x00, //ascr_Gr
	0x4a, //ascr_Mg
	0xe6, //ascr_Gg
	0xe9, //ascr_Mb
	0x2e, //ascr_Gb
	0xf8, //ascr_Yr
	0x27, //ascr_Br
	0xee, //ascr_Yg
	0x35, //ascr_Bg
	0x40, //ascr_Yb
	0xe4, //ascr_Bb
	0xff, //ascr_Wr
	0x00, //ascr_Kr
	0xf7, //ascr_Wg
	0x00, //ascr_Kg
	0xef, //ascr_Wb
	0x00, //ascr_Kb
};

static char DSI_TDMB_STANDARD_MDNIE_2[] = {
	0xDE,
	0x07, //de cs gamma 000
	0x00, //de_gain 10
	0x00,
	0x02, //de_maxplus 11
	0x00,
	0x02, //de_maxminus 11
	0x00,
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
	0x00, //curve_offset
	0x00,
	0x00, //curve_low_x
	0x00, //curve_low_y
};

static char DSI_TDMB_STANDARD_MDNIE_3[] = {
	0xDD,
	0x01, //mdnie_en
	0x00, //mask 000
	0x0f, //ascr algo 0000
	0x00, //roi_en
	0x00, //roi_sx
	0x00, //roi_sx
	0x00, //roi_sy
	0x00, //roi_sy
	0x00, //roi_ex
	0x00, //roi_ex
	0x00, //roi_ey
	0x00, //roi_ey
	0x14, //trans_on trans_slope 0 0000
	0x01, //trans_interval
	//end
};

static char DSI_TDMB_NATURAL_MDNIE_1[] = {
	//start
	0xDF,
	0x02, //linear_on ascr_skin_on 00
	0x6a, //ascr_skin_cb
	0x9a, //ascr_skin_cr
	0x25, //ascr_dist_up
	0x1a, //ascr_dist_down
	0x2a, //ascr_dist_left
	0x16, //ascr_dist_right
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
	0xde, //ascr_skin_Rr
	0x24, //ascr_skin_Rg
	0x28, //ascr_skin_Rb
	0xf8, //ascr_skin_Yr
	0xef, //ascr_skin_Yg
	0x53, //ascr_skin_Yb
	0xe1, //ascr_skin_Mr
	0x41, //ascr_skin_Mg
	0xe3, //ascr_skin_Mb
	0xff, //ascr_skin_Wr
	0xf7, //ascr_skin_Wg
	0xef, //ascr_skin_Wb
	0x91, //ascr_Cr
	0xde, //ascr_Rr
	0xf3, //ascr_Cg
	0x24, //ascr_Rg
	0xea, //ascr_Cb
	0x28, //ascr_Rb
	0xe1, //ascr_Mr
	0x8a, //ascr_Gr
	0x41, //ascr_Mg
	0xec, //ascr_Gg
	0xe3, //ascr_Mb
	0x4a, //ascr_Gb
	0xf8, //ascr_Yr
	0x24, //ascr_Br
	0xef, //ascr_Yg
	0x34, //ascr_Bg
	0x53, //ascr_Yb
	0xe0, //ascr_Bb
	0xff, //ascr_Wr
	0x00, //ascr_Kr
	0xf7, //ascr_Wg
	0x00, //ascr_Kg
	0xef, //ascr_Wb
	0x00, //ascr_Kb
};

static char DSI_TDMB_NATURAL_MDNIE_2[] = {
	0xDE,
	0x07, //de cs gamma 000
	0x00, //de_gain 10
	0x00,
	0x02, //de_maxplus 11
	0x00,
	0x02, //de_maxminus 11
	0x00,
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
	0x00, //curve_offset
	0x00,
	0x00, //curve_low_x
	0x00, //curve_low_y
};

static char DSI_TDMB_NATURAL_MDNIE_3[] = {
	0xDD,
	0x01, //mdnie_en
	0x00, //mask 000
	0x0f, //ascr algo 0000
	0x00, //roi_en
	0x00, //roi_sx
	0x00, //roi_sx
	0x00, //roi_sy
	0x00, //roi_sy
	0x00, //roi_ex
	0x00, //roi_ex
	0x00, //roi_ey
	0x00, //roi_ey
	0x14, //trans_on trans_slope 0 0000
	0x01, //trans_interval
	//end
};

static char DSI_TDMB_DYNAMIC_MDNIE_1[] ={
	//start
	0xDF,
	0x02, //linear_on ascr_skin_on 00
	0x67, //ascr_skin_cb
	0xa9, //ascr_skin_cr
	0x37, //ascr_dist_up
	0x29, //ascr_dist_down
	0x47, //ascr_dist_left
	0x19, //ascr_dist_right
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
	0xe8, //ascr_skin_Rr
	0x00, //ascr_skin_Rg
	0x00, //ascr_skin_Rb
	0xf7, //ascr_skin_Yr
	0xed, //ascr_skin_Yg
	0x00, //ascr_skin_Yb
	0xf2, //ascr_skin_Mr
	0x21, //ascr_skin_Mg
	0xea, //ascr_skin_Mb
	0xff, //ascr_skin_Wr
	0xf7, //ascr_skin_Wg
	0xef, //ascr_skin_Wb
	0x5d, //ascr_Cr
	0xe8, //ascr_Rr
	0xf7, //ascr_Cg
	0x00, //ascr_Rg
	0xec, //ascr_Cb
	0x00, //ascr_Rb
	0xf2, //ascr_Mr
	0x56, //ascr_Gr
	0x21, //ascr_Mg
	0xee, //ascr_Gg
	0xea, //ascr_Mb
	0x00, //ascr_Gb
	0xf7, //ascr_Yr
	0x22, //ascr_Br
	0xed, //ascr_Yg
	0x37, //ascr_Bg
	0x00, //ascr_Yb
	0xea, //ascr_Bb
	0xff, //ascr_Wr
	0x00, //ascr_Kr
	0xf7, //ascr_Wg
	0x00, //ascr_Kg
	0xef, //ascr_Wb
	0x00, //ascr_Kb
};

static char DSI_TDMB_DYNAMIC_MDNIE_2[] = {
	0xDE,
	0x07, //de cs gamma 000
	0x00, //de_gain 10
	0x10,
	0x02, //de_maxplus 11
	0x00,
	0x02, //de_maxminus 11
	0x00,
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
	0x00, //curve_offset
	0x00,
	0x00, //curve_low_x
	0x00, //curve_low_y
};

static char DSI_TDMB_DYNAMIC_MDNIE_3[] = {
	0xDD,
	0x01, //mdnie_en
	0x00, //mask 000
	0x0f, //ascr algo 0000
	0x00, //roi_en
	0x00, //roi_sx
	0x00, //roi_sx
	0x00, //roi_sy
	0x00, //roi_sy
	0x00, //roi_ex
	0x00, //roi_ex
	0x00, //roi_ey
	0x00, //roi_ey
	0x14, //trans_on trans_slope 0 0000
	0x01, //trans_interval
	//end
};

static char DSI_TDMB_MOVIE_MDNIE_1[] = {
	//start
	0xDF,
	0x01, //linear_on ascr_skin_on 00
	0x67, //ascr_skin_cb
	0xa9, //ascr_skin_cr
	0x0c, //ascr_dist_up
	0x0c, //ascr_dist_down
	0x0c, //ascr_dist_left
	0x0c, //ascr_dist_right
	0x00, //ascr_div_up 20
	0xaa,
	0xab,
	0x00, //ascr_div_down
	0xaa,
	0xab,
	0x00, //ascr_div_right
	0xaa,
	0xab,
	0x00, //ascr_div_left
	0xaa,
	0xab,
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
};

static char DSI_TDMB_MOVIE_MDNIE_2[] = {
	0xDE,
	0x00, //de cs gamma 000
	0x00, //de_gain 10
	0x00,
	0x07, //de_maxplus 11
	0xff,
	0x07, //de_maxminus 11
	0xff,
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
	0x00, //curve_offset
	0x00,
	0x00, //curve_low_x
	0x00, //curve_low_y
};

static char DSI_TDMB_MOVIE_MDNIE_3[] = {
	0xDD,
	0x01, //mdnie_en
	0x00, //mask 000
	0x0c, //ascr algo 0000
	0x00, //roi_en
	0x00, //roi_sx
	0x00, //roi_sx
	0x00, //roi_sy
	0x00, //roi_sy
	0x00, //roi_ex
	0x00, //roi_ex
	0x00, //roi_ey
	0x00, //roi_ey
	0x00, //trans_on trans_slope 0 0000
	0x00, //trans_interval
	//end
};

static char DSI_TDMB_AUTO_MDNIE_1[] = {
	//start
	0xDF,
	0x01, //linear_on ascr_skin_on 00
	0x6a, //ascr_skin_cb
	0x9a, //ascr_skin_cr
	0x25, //ascr_dist_up
	0x1a, //ascr_dist_down
	0x2a, //ascr_dist_left
	0x16, //ascr_dist_right
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
	0x2c, //ascr_skin_Rg
	0x40, //ascr_skin_Rb
	0xff, //ascr_skin_Yr
	0xf0, //ascr_skin_Yg
	0x00, //ascr_skin_Yb
	0xd8, //ascr_skin_Mr
	0x00, //ascr_skin_Mg
	0xd9, //ascr_skin_Mb
	0xff, //ascr_skin_Wr
	0xff, //ascr_skin_Wg
	0xff, //ascr_skin_Wb
	0x00, //ascr_Cr
	0xe0, //ascr_Rr
	0xff, //ascr_Cg
	0x00, //ascr_Rg
	0xf6, //ascr_Cb
	0x00, //ascr_Rb
	0xd8, //ascr_Mr
	0x3b, //ascr_Gr
	0x00, //ascr_Mg
	0xff, //ascr_Gg
	0xd9, //ascr_Mb
	0x00, //ascr_Gb
	0xff, //ascr_Yr
	0x14, //ascr_Br
	0xf9, //ascr_Yg
	0x00, //ascr_Bg
	0x00, //ascr_Yb
	0xff, //ascr_Bb
	0xff, //ascr_Wr
	0x00, //ascr_Kr
	0xff, //ascr_Wg
	0x00, //ascr_Kg
	0xff, //ascr_Wb
	0x00, //ascr_Kb
};

static char DSI_TDMB_AUTO_MDNIE_2[] = {
	0xDE,
	0x07, //de cs gamma 000
	0x00, //de_gain 10
	0x10,
	0x02, //de_maxplus 11
	0x00,
	0x02, //de_maxminus 11
	0x00,
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
	0x00, //curve_offset
	0x00,
	0x00, //curve_low_x
	0x00, //curve_low_y
};

static char DSI_TDMB_AUTO_MDNIE_3[] = {
	0xDD,
	0x01, //mdnie_en
	0x00, //mask 000
	0x0f, //ascr algo 0000
	0x00, //roi_en
	0x00, //roi_sx
	0x00, //roi_sx
	0x00, //roi_sy
	0x00, //roi_sy
	0x00, //roi_ex
	0x00, //roi_ex
	0x00, //roi_ey
	0x00, //roi_ey
	0x14, //trans_on trans_slope 0 0000
	0x01, //trans_interval
	//end
};

static char DSI_HMT_COLOR_TEMP_3000K_MDNIE_1[] = {
	//start
	0xDF,
	0x00, //linear_on ascr_skin_on 00
	0x6a, //ascr_skin_cb
	0x9a, //ascr_skin_cr
	0x25, //ascr_dist_up
	0x1a, //ascr_dist_down
	0x2a, //ascr_dist_left
	0x16, //ascr_dist_right
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
};

static char DSI_HMT_COLOR_TEMP_3000K_MDNIE_2[] = {
	0xDE,
	0x00, //de cs gamma 000
	0x00, //de_gain 10
	0x00,
	0x02, //de_maxplus 11
	0x00,
	0x02, //de_maxminus 11
	0x00,
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
	0x00, //curve_offset
	0x00,
	0x00, //curve_low_x
	0x00, //curve_low_y
};

static char DSI_HMT_COLOR_TEMP_3000K_MDNIE_3[] = {
	0xDD,
	0x01, //mdnie_en
	0x00, //mask 000
	0x00, //ascr algo 0000
	0x00, //roi_en
	0x00, //roi_sx
	0x00, //roi_sx
	0x00, //roi_sy
	0x00, //roi_sy
	0x00, //roi_ex
	0x00, //roi_ex
	0x00, //roi_ey
	0x00, //roi_ey
	0x00, //trans_on trans_slope 0 0000
	0x00, //trans_interval
	//end
};

static char DSI_HMT_COLOR_TEMP_4000K_MDNIE_1[] = {
	//start
	0xDF,
	0x00, //linear_on ascr_skin_on 00
	0x6a, //ascr_skin_cb
	0x9a, //ascr_skin_cr
	0x25, //ascr_dist_up
	0x1a, //ascr_dist_down
	0x2a, //ascr_dist_left
	0x16, //ascr_dist_right
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
};

static char DSI_HMT_COLOR_TEMP_4000K_MDNIE_2[] = {
	0xDE,
	0x00, //de cs gamma 000
	0x00, //de_gain 10
	0x00,
	0x02, //de_maxplus 11
	0x00,
	0x02, //de_maxminus 11
	0x00,
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
	0x00, //curve_offset
	0x00,
	0x00, //curve_low_x
	0x00, //curve_low_y
};

static char DSI_HMT_COLOR_TEMP_4000K_MDNIE_3[] = {
	0xDD,
	0x01, //mdnie_en
	0x00, //mask 000
	0x00, //ascr algo 0000
	0x00, //roi_en
	0x00, //roi_sx
	0x00, //roi_sx
	0x00, //roi_sy
	0x00, //roi_sy
	0x00, //roi_ex
	0x00, //roi_ex
	0x00, //roi_ey
	0x00, //roi_ey
	0x00, //trans_on trans_slope 0 0000
	0x00, //trans_interval
	//end
};

static char DSI_HMT_COLOR_TEMP_5000K_MDNIE_1[] = {
	//start
	0xDF,
	0x00, //linear_on ascr_skin_on 00
	0x6a, //ascr_skin_cb
	0x9a, //ascr_skin_cr
	0x25, //ascr_dist_up
	0x1a, //ascr_dist_down
	0x2a, //ascr_dist_left
	0x16, //ascr_dist_right
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
};

static char DSI_HMT_COLOR_TEMP_5000K_MDNIE_2[] = {
	0xDE,
	0x00, //de cs gamma 000
	0x00, //de_gain 10
	0x00,
	0x02, //de_maxplus 11
	0x00,
	0x02, //de_maxminus 11
	0x00,
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
	0x00, //curve_offset
	0x00,
	0x00, //curve_low_x
	0x00, //curve_low_y
};

static char DSI_HMT_COLOR_TEMP_5000K_MDNIE_3[] = {
	0xDD,
	0x01, //mdnie_en
	0x00, //mask 000
	0x00, //ascr algo 0000
	0x00, //roi_en
	0x00, //roi_sx
	0x00, //roi_sx
	0x00, //roi_sy
	0x00, //roi_sy
	0x00, //roi_ex
	0x00, //roi_ex
	0x00, //roi_ey
	0x00, //roi_ey
	0x00, //trans_on trans_slope 0 0000
	0x00, //trans_interval
	//end
};

static char DSI_HMT_COLOR_TEMP_6500K_MDNIE_1[] = {
	//start
	0xDF,
	0x00, //linear_on ascr_skin_on 00
	0x6a, //ascr_skin_cb
	0x9a, //ascr_skin_cr
	0x25, //ascr_dist_up
	0x1a, //ascr_dist_down
	0x2a, //ascr_dist_left
	0x16, //ascr_dist_right
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
	0xfa, //ascr_Wg
	0x00, //ascr_Kg
	0xf1, //ascr_Wb
	0x00, //ascr_Kb
};

static char DSI_HMT_COLOR_TEMP_6500K_MDNIE_2[] = {
	0xDE,
	0x07, //de cs gamma 000
	0x00, //de_gain 10
	0x00,
	0x02, //de_maxplus 11
	0x00,
	0x02, //de_maxminus 11
	0x00,
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
	0x00, //curve_offset
	0x00,
	0x00, //curve_low_x
	0x00, //curve_low_y
};

static char DSI_HMT_COLOR_TEMP_6500K_MDNIE_3[] = {
	0xDD,
	0x01, //mdnie_en
	0x00, //mask 000
	0x0f, //ascr algo 0000
	0x00, //roi_en
	0x00, //roi_sx
	0x00, //roi_sx
	0x00, //roi_sy
	0x00, //roi_sy
	0x00, //roi_ex
	0x00, //roi_ex
	0x00, //roi_ey
	0x00, //roi_ey
	0x14, //trans_on trans_slope 0 0000
	0x01, //trans_interval
	//end
};

static char DSI_HMT_COLOR_TEMP_7500K_MDNIE_1[] = {
	//start
	0xDF,
	0x00, //linear_on ascr_skin_on 00
	0x6a, //ascr_skin_cb
	0x9a, //ascr_skin_cr
	0x25, //ascr_dist_up
	0x1a, //ascr_dist_down
	0x2a, //ascr_dist_left
	0x16, //ascr_dist_right
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
};

static char DSI_HMT_COLOR_TEMP_7500K_MDNIE_2[] = {
	0xDE,
	0x00, //de cs gamma 000
	0x00, //de_gain 10
	0x00,
	0x02, //de_maxplus 11
	0x00,
	0x02, //de_maxminus 11
	0x00,
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
	0x00, //curve_offset
	0x00,
	0x00, //curve_low_x
	0x00, //curve_low_y
};

static char DSI_HMT_COLOR_TEMP_7500K_MDNIE_3[] = {
	0xDD,
	0x01, //mdnie_en
	0x00, //mask 000
	0x00, //ascr algo 0000
	0x00, //roi_en
	0x00, //roi_sx
	0x00, //roi_sx
	0x00, //roi_sy
	0x00, //roi_sy
	0x00, //roi_ex
	0x00, //roi_ex
	0x00, //roi_ey
	0x00, //roi_ey
	0x00, //trans_on trans_slope 0 0000
	0x00, //trans_interval
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
	{{0, MIPI_DSI_DCS_LONG_WRITE, 0, 0, sizeof(DSI_BYPASS_MDNIE_1), DSI_BYPASS_MDNIE_1, 0, NULL}, false, 0},
	{{0, MIPI_DSI_DCS_LONG_WRITE, 0, 0, sizeof(DSI_BYPASS_MDNIE_2), DSI_BYPASS_MDNIE_2, 0, NULL}, false, 0},
	{{0, MIPI_DSI_DCS_LONG_WRITE, 0, 0, sizeof(DSI_BYPASS_MDNIE_3), DSI_BYPASS_MDNIE_3, 0, NULL}, false, 0},
	{{0, MIPI_DSI_DCS_LONG_WRITE, 0, 0, sizeof(level_1_key_off), level_1_key_off, 0, NULL}, true, 0},
};

static struct dsi_cmd_desc DSI_HDR_VIDEO_2_MDNIE[] = {
	{{0, MIPI_DSI_DCS_LONG_WRITE, 0, 0, sizeof(level_1_key_on), level_1_key_on, 0, NULL}, false, 0},
	{{0, MIPI_DSI_DCS_LONG_WRITE, 0, 0, sizeof(DSI_BYPASS_MDNIE_1), DSI_BYPASS_MDNIE_1, 0, NULL}, false, 0},
	{{0, MIPI_DSI_DCS_LONG_WRITE, 0, 0, sizeof(DSI_BYPASS_MDNIE_2), DSI_BYPASS_MDNIE_2, 0, NULL}, false, 0},
	{{0, MIPI_DSI_DCS_LONG_WRITE, 0, 0, sizeof(DSI_BYPASS_MDNIE_3), DSI_BYPASS_MDNIE_3, 0, NULL}, false, 0},
	{{0, MIPI_DSI_DCS_LONG_WRITE, 0, 0, sizeof(level_1_key_off), level_1_key_off, 0, NULL}, true, 0},
};

static struct dsi_cmd_desc DSI_HDR_VIDEO_3_MDNIE[] = {
	{{0, MIPI_DSI_DCS_LONG_WRITE, 0, 0, sizeof(level_1_key_on), level_1_key_on, 0, NULL}, false, 0},
	{{0, MIPI_DSI_DCS_LONG_WRITE, 0, 0, sizeof(DSI_BYPASS_MDNIE_1), DSI_BYPASS_MDNIE_1, 0, NULL}, false, 0},
	{{0, MIPI_DSI_DCS_LONG_WRITE, 0, 0, sizeof(DSI_BYPASS_MDNIE_2), DSI_BYPASS_MDNIE_2, 0, NULL}, false, 0},
	{{0, MIPI_DSI_DCS_LONG_WRITE, 0, 0, sizeof(DSI_BYPASS_MDNIE_3), DSI_BYPASS_MDNIE_3, 0, NULL}, false, 0},
	{{0, MIPI_DSI_DCS_LONG_WRITE, 0, 0, sizeof(level_1_key_off), level_1_key_off, 0, NULL}, true, 0},
};

static struct dsi_cmd_desc DSI_HDR_VIDEO_4_MDNIE[] = {
	{{0, MIPI_DSI_DCS_LONG_WRITE, 0, 0, sizeof(level_1_key_on), level_1_key_on, 0, NULL}, false, 0},
	{{0, MIPI_DSI_DCS_LONG_WRITE, 0, 0, sizeof(DSI_BYPASS_MDNIE_1), DSI_BYPASS_MDNIE_1, 0, NULL}, false, 0},
	{{0, MIPI_DSI_DCS_LONG_WRITE, 0, 0, sizeof(DSI_BYPASS_MDNIE_2), DSI_BYPASS_MDNIE_2, 0, NULL}, false, 0},
	{{0, MIPI_DSI_DCS_LONG_WRITE, 0, 0, sizeof(DSI_BYPASS_MDNIE_3), DSI_BYPASS_MDNIE_3, 0, NULL}, false, 0},
	{{0, MIPI_DSI_DCS_LONG_WRITE, 0, 0, sizeof(level_1_key_off), level_1_key_off, 0, NULL}, true, 0},
};

static struct dsi_cmd_desc DSI_HDR_VIDEO_5_MDNIE[] = {
	{{0, MIPI_DSI_DCS_LONG_WRITE, 0, 0, sizeof(level_1_key_on), level_1_key_on, 0, NULL}, false, 0},
	{{0, MIPI_DSI_DCS_LONG_WRITE, 0, 0, sizeof(DSI_BYPASS_MDNIE_1), DSI_BYPASS_MDNIE_1, 0, NULL}, false, 0},
	{{0, MIPI_DSI_DCS_LONG_WRITE, 0, 0, sizeof(DSI_BYPASS_MDNIE_2), DSI_BYPASS_MDNIE_2, 0, NULL}, false, 0},
	{{0, MIPI_DSI_DCS_LONG_WRITE, 0, 0, sizeof(DSI_BYPASS_MDNIE_3), DSI_BYPASS_MDNIE_3, 0, NULL}, false, 0},
	{{0, MIPI_DSI_DCS_LONG_WRITE, 0, 0, sizeof(level_1_key_off), level_1_key_off, 0, NULL}, true, 0},
};

static struct dsi_cmd_desc DSI_VIDEO_ENHANCER_MDNIE[] = {
	{{0, MIPI_DSI_DCS_LONG_WRITE, 0, 0, sizeof(level_1_key_on), level_1_key_on, 0, NULL}, false, 0},
	{{0, MIPI_DSI_DCS_LONG_WRITE, 0, 0, sizeof(DSI_VIDEO_ENHANCER_MDNIE_1), DSI_VIDEO_ENHANCER_MDNIE_1, 0, NULL}, false, 0},
	{{0, MIPI_DSI_DCS_LONG_WRITE, 0, 0, sizeof(DSI_VIDEO_ENHANCER_MDNIE_2), DSI_VIDEO_ENHANCER_MDNIE_2, 0, NULL}, false, 0},
	{{0, MIPI_DSI_DCS_LONG_WRITE, 0, 0, sizeof(DSI_VIDEO_ENHANCER_MDNIE_3), DSI_VIDEO_ENHANCER_MDNIE_3, 0, NULL}, false, 0},
	{{0, MIPI_DSI_DCS_LONG_WRITE, 0, 0, sizeof(level_1_key_off), level_1_key_off, 0, NULL}, true, 0},
};

static struct dsi_cmd_desc DSI_VIDEO_ENHANCER_THIRD_MDNIE[] = {
	{{0, MIPI_DSI_DCS_LONG_WRITE, 0, 0, sizeof(level_1_key_on), level_1_key_on, 0, NULL}, false, 0},
	{{0, MIPI_DSI_DCS_LONG_WRITE, 0, 0, sizeof(DSI_VIDEO_ENHANCER_THIRD_MDNIE_1), DSI_VIDEO_ENHANCER_THIRD_MDNIE_1, 0, NULL}, false, 0},
	{{0, MIPI_DSI_DCS_LONG_WRITE, 0, 0, sizeof(DSI_VIDEO_ENHANCER_THIRD_MDNIE_2), DSI_VIDEO_ENHANCER_THIRD_MDNIE_2, 0, NULL}, false, 0},
	{{0, MIPI_DSI_DCS_LONG_WRITE, 0, 0, sizeof(DSI_VIDEO_ENHANCER_THIRD_MDNIE_3), DSI_VIDEO_ENHANCER_THIRD_MDNIE_3, 0, NULL}, false, 0},
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

static struct dsi_cmd_desc DSI_UI_MOVIE_MDNIE[] = {
	{{0, MIPI_DSI_DCS_LONG_WRITE, 0, 0, sizeof(level_1_key_on), level_1_key_on, 0, NULL}, false, 0},
	{{0, MIPI_DSI_DCS_LONG_WRITE, 0, 0, sizeof(DSI_UI_MOVIE_MDNIE_1), DSI_UI_MOVIE_MDNIE_1, 0, NULL}, false, 0},
	{{0, MIPI_DSI_DCS_LONG_WRITE, 0, 0, sizeof(DSI_UI_MOVIE_MDNIE_2), DSI_UI_MOVIE_MDNIE_2, 0, NULL}, false, 0},
	{{0, MIPI_DSI_DCS_LONG_WRITE, 0, 0, sizeof(DSI_UI_MOVIE_MDNIE_3), DSI_UI_MOVIE_MDNIE_3, 0, NULL}, false, 0},
	{{0, MIPI_DSI_DCS_LONG_WRITE, 0, 0, sizeof(level_1_key_off), level_1_key_off, 0, NULL}, true, 0},
};

static struct dsi_cmd_desc DSI_UI_AUTO_MDNIE[] = {
	{{0, MIPI_DSI_DCS_LONG_WRITE, 0, 0, sizeof(level_1_key_on), level_1_key_on, 0, NULL}, false, 0},
	{{0, MIPI_DSI_DCS_LONG_WRITE, 0, 0, sizeof(DSI_UI_AUTO_MDNIE_1), DSI_UI_AUTO_MDNIE_1, 0, NULL}, false, 0},
	{{0, MIPI_DSI_DCS_LONG_WRITE, 0, 0, sizeof(DSI_UI_AUTO_MDNIE_2), DSI_UI_AUTO_MDNIE_2, 0, NULL}, false, 0},
	{{0, MIPI_DSI_DCS_LONG_WRITE, 0, 0, sizeof(DSI_UI_AUTO_MDNIE_3), DSI_UI_AUTO_MDNIE_3, 0, NULL}, false, 0},
	{{0, MIPI_DSI_DCS_LONG_WRITE, 0, 0, sizeof(level_1_key_off), level_1_key_off, 0, NULL}, true, 0},
};

static struct dsi_cmd_desc DSI_VIDEO_OUTDOOR_MDNIE[] = {
	{{0, MIPI_DSI_DCS_LONG_WRITE, 0, 0, sizeof(level_1_key_on), level_1_key_on, 0, NULL}, false, 0},
	{{0, MIPI_DSI_DCS_LONG_WRITE, 0, 0, sizeof(DSI_VIDEO_OUTDOOR_MDNIE_1), DSI_VIDEO_OUTDOOR_MDNIE_1, 0, NULL}, false, 0},
	{{0, MIPI_DSI_DCS_LONG_WRITE, 0, 0, sizeof(DSI_VIDEO_OUTDOOR_MDNIE_2), DSI_VIDEO_OUTDOOR_MDNIE_2, 0, NULL}, false, 0},
	{{0, MIPI_DSI_DCS_LONG_WRITE, 0, 0, sizeof(DSI_VIDEO_OUTDOOR_MDNIE_3), DSI_VIDEO_OUTDOOR_MDNIE_3, 0, NULL}, false, 0},
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

static struct dsi_cmd_desc DSI_VIDEO_MOVIE_MDNIE[] = {
	{{0, MIPI_DSI_DCS_LONG_WRITE, 0, 0, sizeof(level_1_key_on), level_1_key_on, 0, NULL}, false, 0},
	{{0, MIPI_DSI_DCS_LONG_WRITE, 0, 0, sizeof(DSI_VIDEO_MOVIE_MDNIE_1), DSI_VIDEO_MOVIE_MDNIE_1, 0, NULL}, false, 0},
	{{0, MIPI_DSI_DCS_LONG_WRITE, 0, 0, sizeof(DSI_VIDEO_MOVIE_MDNIE_2), DSI_VIDEO_MOVIE_MDNIE_2, 0, NULL}, false, 0},
	{{0, MIPI_DSI_DCS_LONG_WRITE, 0, 0, sizeof(DSI_VIDEO_MOVIE_MDNIE_3), DSI_VIDEO_MOVIE_MDNIE_3, 0, NULL}, false, 0},
	{{0, MIPI_DSI_DCS_LONG_WRITE, 0, 0, sizeof(level_1_key_off), level_1_key_off, 0, NULL}, true, 0},
};

static struct dsi_cmd_desc DSI_VIDEO_AUTO_MDNIE[] = {
	{{0, MIPI_DSI_DCS_LONG_WRITE, 0, 0, sizeof(level_1_key_on), level_1_key_on, 0, NULL}, false, 0},
	{{0, MIPI_DSI_DCS_LONG_WRITE, 0, 0, sizeof(DSI_VIDEO_AUTO_MDNIE_1), DSI_VIDEO_AUTO_MDNIE_1, 0, NULL}, false, 0},
	{{0, MIPI_DSI_DCS_LONG_WRITE, 0, 0, sizeof(DSI_VIDEO_AUTO_MDNIE_2), DSI_VIDEO_AUTO_MDNIE_2, 0, NULL}, false, 0},
	{{0, MIPI_DSI_DCS_LONG_WRITE, 0, 0, sizeof(DSI_VIDEO_AUTO_MDNIE_3), DSI_VIDEO_AUTO_MDNIE_3, 0, NULL}, false, 0},
	{{0, MIPI_DSI_DCS_LONG_WRITE, 0, 0, sizeof(level_1_key_off), level_1_key_off, 0, NULL}, true, 0},
};

static struct dsi_cmd_desc DSI_VIDEO_WARM_OUTDOOR_MDNIE[] = {
	{{0, MIPI_DSI_DCS_LONG_WRITE, 0, 0, sizeof(level_1_key_on), level_1_key_on, 0, NULL}, false, 0},
	{{0, MIPI_DSI_DCS_LONG_WRITE, 0, 0, sizeof(DSI_VIDEO_WARM_OUTDOOR_MDNIE_1), DSI_VIDEO_WARM_OUTDOOR_MDNIE_1, 0, NULL}, false, 0},
	{{0, MIPI_DSI_DCS_LONG_WRITE, 0, 0, sizeof(DSI_VIDEO_WARM_OUTDOOR_MDNIE_2), DSI_VIDEO_WARM_OUTDOOR_MDNIE_2, 0, NULL}, false, 0},
	{{0, MIPI_DSI_DCS_LONG_WRITE, 0, 0, sizeof(DSI_VIDEO_WARM_OUTDOOR_MDNIE_3), DSI_VIDEO_WARM_OUTDOOR_MDNIE_3, 0, NULL}, false, 0},
	{{0, MIPI_DSI_DCS_LONG_WRITE, 0, 0, sizeof(level_1_key_off), level_1_key_off, 0, NULL}, true, 0},
};

static struct dsi_cmd_desc DSI_VIDEO_WARM_MDNIE[] = {
	{{0, MIPI_DSI_DCS_LONG_WRITE, 0, 0, sizeof(level_1_key_on), level_1_key_on, 0, NULL}, false, 0},
	{{0, MIPI_DSI_DCS_LONG_WRITE, 0, 0, sizeof(DSI_VIDEO_WARM_MDNIE_1), DSI_VIDEO_WARM_MDNIE_1, 0, NULL}, false, 0},
	{{0, MIPI_DSI_DCS_LONG_WRITE, 0, 0, sizeof(DSI_VIDEO_WARM_MDNIE_2), DSI_VIDEO_WARM_MDNIE_2, 0, NULL}, false, 0},
	{{0, MIPI_DSI_DCS_LONG_WRITE, 0, 0, sizeof(DSI_VIDEO_WARM_MDNIE_3), DSI_VIDEO_WARM_MDNIE_3, 0, NULL}, false, 0},
	{{0, MIPI_DSI_DCS_LONG_WRITE, 0, 0, sizeof(level_1_key_off), level_1_key_off, 0, NULL}, true, 0},
};

static struct dsi_cmd_desc DSI_VIDEO_COLD_OUTDOOR_MDNIE[] = {
	{{0, MIPI_DSI_DCS_LONG_WRITE, 0, 0, sizeof(level_1_key_on), level_1_key_on, 0, NULL}, false, 0},
	{{0, MIPI_DSI_DCS_LONG_WRITE, 0, 0, sizeof(DSI_VIDEO_COLD_OUTDOOR_MDNIE_1), DSI_VIDEO_COLD_OUTDOOR_MDNIE_1, 0, NULL}, false, 0},
	{{0, MIPI_DSI_DCS_LONG_WRITE, 0, 0, sizeof(DSI_VIDEO_COLD_OUTDOOR_MDNIE_2), DSI_VIDEO_COLD_OUTDOOR_MDNIE_2, 0, NULL}, false, 0},
	{{0, MIPI_DSI_DCS_LONG_WRITE, 0, 0, sizeof(DSI_VIDEO_COLD_OUTDOOR_MDNIE_3), DSI_VIDEO_COLD_OUTDOOR_MDNIE_3, 0, NULL}, false, 0},
	{{0, MIPI_DSI_DCS_LONG_WRITE, 0, 0, sizeof(level_1_key_off), level_1_key_off, 0, NULL}, true, 0},
};

static struct dsi_cmd_desc DSI_VIDEO_COLD_MDNIE[] = {
	{{0, MIPI_DSI_DCS_LONG_WRITE, 0, 0, sizeof(level_1_key_on), level_1_key_on, 0, NULL}, false, 0},
	{{0, MIPI_DSI_DCS_LONG_WRITE, 0, 0, sizeof(DSI_VIDEO_COLD_MDNIE_1), DSI_VIDEO_COLD_MDNIE_1, 0, NULL}, false, 0},
	{{0, MIPI_DSI_DCS_LONG_WRITE, 0, 0, sizeof(DSI_VIDEO_COLD_MDNIE_2), DSI_VIDEO_COLD_MDNIE_2, 0, NULL}, false, 0},
	{{0, MIPI_DSI_DCS_LONG_WRITE, 0, 0, sizeof(DSI_VIDEO_COLD_MDNIE_3), DSI_VIDEO_COLD_MDNIE_3, 0, NULL}, false, 0},
	{{0, MIPI_DSI_DCS_LONG_WRITE, 0, 0, sizeof(level_1_key_off), level_1_key_off, 0, NULL}, true, 0},
};

static struct dsi_cmd_desc DSI_CAMERA_OUTDOOR_MDNIE[] = {
	{{0, MIPI_DSI_DCS_LONG_WRITE, 0, 0, sizeof(level_1_key_on), level_1_key_on, 0, NULL}, false, 0},
	{{0, MIPI_DSI_DCS_LONG_WRITE, 0, 0, sizeof(DSI_CAMERA_OUTDOOR_MDNIE_1), DSI_CAMERA_OUTDOOR_MDNIE_1, 0, NULL}, false, 0},
	{{0, MIPI_DSI_DCS_LONG_WRITE, 0, 0, sizeof(DSI_CAMERA_OUTDOOR_MDNIE_2), DSI_CAMERA_OUTDOOR_MDNIE_2, 0, NULL}, false, 0},
	{{0, MIPI_DSI_DCS_LONG_WRITE, 0, 0, sizeof(DSI_CAMERA_OUTDOOR_MDNIE_3), DSI_CAMERA_OUTDOOR_MDNIE_3, 0, NULL}, false, 0},
	{{0, MIPI_DSI_DCS_LONG_WRITE, 0, 0, sizeof(level_1_key_off), level_1_key_off, 0, NULL}, true, 0},
};

static struct dsi_cmd_desc DSI_CAMERA_MDNIE[] = {
	{{0, MIPI_DSI_DCS_LONG_WRITE, 0, 0, sizeof(level_1_key_on), level_1_key_on, 0, NULL}, false, 0},
	{{0, MIPI_DSI_DCS_LONG_WRITE, 0, 0, sizeof(DSI_CAMERA_MDNIE_1), DSI_CAMERA_MDNIE_1, 0, NULL}, false, 0},
	{{0, MIPI_DSI_DCS_LONG_WRITE, 0, 0, sizeof(DSI_CAMERA_MDNIE_2), DSI_CAMERA_MDNIE_2, 0, NULL}, false, 0},
	{{0, MIPI_DSI_DCS_LONG_WRITE, 0, 0, sizeof(DSI_CAMERA_MDNIE_3), DSI_CAMERA_MDNIE_3, 0, NULL}, false, 0},
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

static struct dsi_cmd_desc DSI_CAMERA_MOVIE_MDNIE[] = {
	{{0, MIPI_DSI_DCS_LONG_WRITE, 0, 0, sizeof(level_1_key_on), level_1_key_on, 0, NULL}, false, 0},
	{{0, MIPI_DSI_DCS_LONG_WRITE, 0, 0, sizeof(DSI_CAMERA_MOVIE_MDNIE_1), DSI_CAMERA_MOVIE_MDNIE_1, 0, NULL}, false, 0},
	{{0, MIPI_DSI_DCS_LONG_WRITE, 0, 0, sizeof(DSI_CAMERA_MOVIE_MDNIE_2), DSI_CAMERA_MOVIE_MDNIE_2, 0, NULL}, false, 0},
	{{0, MIPI_DSI_DCS_LONG_WRITE, 0, 0, sizeof(DSI_CAMERA_MOVIE_MDNIE_3), DSI_CAMERA_MOVIE_MDNIE_3, 0, NULL}, false, 0},
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

static struct dsi_cmd_desc DSI_GALLERY_MOVIE_MDNIE[] = {
	{{0, MIPI_DSI_DCS_LONG_WRITE, 0, 0, sizeof(level_1_key_on), level_1_key_on, 0, NULL}, false, 0},
	{{0, MIPI_DSI_DCS_LONG_WRITE, 0, 0, sizeof(DSI_GALLERY_MOVIE_MDNIE_1), DSI_GALLERY_MOVIE_MDNIE_1, 0, NULL}, false, 0},
	{{0, MIPI_DSI_DCS_LONG_WRITE, 0, 0, sizeof(DSI_GALLERY_MOVIE_MDNIE_2), DSI_GALLERY_MOVIE_MDNIE_2, 0, NULL}, false, 0},
	{{0, MIPI_DSI_DCS_LONG_WRITE, 0, 0, sizeof(DSI_GALLERY_MOVIE_MDNIE_3), DSI_GALLERY_MOVIE_MDNIE_3, 0, NULL}, false, 0},
	{{0, MIPI_DSI_DCS_LONG_WRITE, 0, 0, sizeof(level_1_key_off), level_1_key_off, 0, NULL}, true, 0},
};

static struct dsi_cmd_desc DSI_GALLERY_AUTO_MDNIE[] = {
	{{0, MIPI_DSI_DCS_LONG_WRITE, 0, 0, sizeof(level_1_key_on), level_1_key_on, 0, NULL}, false, 0},
	{{0, MIPI_DSI_DCS_LONG_WRITE, 0, 0, sizeof(DSI_GALLERY_AUTO_MDNIE_1), DSI_GALLERY_AUTO_MDNIE_1, 0, NULL}, false, 0},
	{{0, MIPI_DSI_DCS_LONG_WRITE, 0, 0, sizeof(DSI_GALLERY_AUTO_MDNIE_2), DSI_GALLERY_AUTO_MDNIE_2, 0, NULL}, false, 0},
	{{0, MIPI_DSI_DCS_LONG_WRITE, 0, 0, sizeof(DSI_GALLERY_AUTO_MDNIE_3), DSI_GALLERY_AUTO_MDNIE_3, 0, NULL}, false, 0},
	{{0, MIPI_DSI_DCS_LONG_WRITE, 0, 0, sizeof(level_1_key_off), level_1_key_off, 0, NULL}, true, 0},
};

static struct dsi_cmd_desc DSI_VT_DYNAMIC_MDNIE[] = {
	{{0, MIPI_DSI_DCS_LONG_WRITE, 0, 0, sizeof(level_1_key_on), level_1_key_on, 0, NULL}, false, 0},
	{{0, MIPI_DSI_DCS_LONG_WRITE, 0, 0, sizeof(DSI_VT_DYNAMIC_MDNIE_1), DSI_VT_DYNAMIC_MDNIE_1, 0, NULL}, false, 0},
	{{0, MIPI_DSI_DCS_LONG_WRITE, 0, 0, sizeof(DSI_VT_DYNAMIC_MDNIE_2), DSI_VT_DYNAMIC_MDNIE_2, 0, NULL}, false, 0},
	{{0, MIPI_DSI_DCS_LONG_WRITE, 0, 0, sizeof(DSI_VT_DYNAMIC_MDNIE_3), DSI_VT_DYNAMIC_MDNIE_3, 0, NULL}, false, 0},
	{{0, MIPI_DSI_DCS_LONG_WRITE, 0, 0, sizeof(level_1_key_off), level_1_key_off, 0, NULL}, true, 0},
};

static struct dsi_cmd_desc DSI_VT_STANDARD_MDNIE[] = {
	{{0, MIPI_DSI_DCS_LONG_WRITE, 0, 0, sizeof(level_1_key_on), level_1_key_on, 0, NULL}, false, 0},
	{{0, MIPI_DSI_DCS_LONG_WRITE, 0, 0, sizeof(DSI_VT_STANDARD_MDNIE_1), DSI_VT_STANDARD_MDNIE_1, 0, NULL}, false, 0},
	{{0, MIPI_DSI_DCS_LONG_WRITE, 0, 0, sizeof(DSI_VT_STANDARD_MDNIE_2), DSI_VT_STANDARD_MDNIE_2, 0, NULL}, false, 0},
	{{0, MIPI_DSI_DCS_LONG_WRITE, 0, 0, sizeof(DSI_VT_STANDARD_MDNIE_3), DSI_VT_STANDARD_MDNIE_3, 0, NULL}, false, 0},
	{{0, MIPI_DSI_DCS_LONG_WRITE, 0, 0, sizeof(level_1_key_off), level_1_key_off, 0, NULL}, true, 0},
};

static struct dsi_cmd_desc DSI_VT_NATURAL_MDNIE[] = {
	{{0, MIPI_DSI_DCS_LONG_WRITE, 0, 0, sizeof(level_1_key_on), level_1_key_on, 0, NULL}, false, 0},
	{{0, MIPI_DSI_DCS_LONG_WRITE, 0, 0, sizeof(DSI_VT_NATURAL_MDNIE_1), DSI_VT_NATURAL_MDNIE_1, 0, NULL}, false, 0},
	{{0, MIPI_DSI_DCS_LONG_WRITE, 0, 0, sizeof(DSI_VT_NATURAL_MDNIE_2), DSI_VT_NATURAL_MDNIE_2, 0, NULL}, false, 0},
	{{0, MIPI_DSI_DCS_LONG_WRITE, 0, 0, sizeof(DSI_VT_NATURAL_MDNIE_3), DSI_VT_NATURAL_MDNIE_3, 0, NULL}, false, 0},
	{{0, MIPI_DSI_DCS_LONG_WRITE, 0, 0, sizeof(level_1_key_off), level_1_key_off, 0, NULL}, true, 0},
};

static struct dsi_cmd_desc DSI_VT_MOVIE_MDNIE[] = {
	{{0, MIPI_DSI_DCS_LONG_WRITE, 0, 0, sizeof(level_1_key_on), level_1_key_on, 0, NULL}, false, 0},
	{{0, MIPI_DSI_DCS_LONG_WRITE, 0, 0, sizeof(DSI_VT_MOVIE_MDNIE_1), DSI_VT_MOVIE_MDNIE_1, 0, NULL}, false, 0},
	{{0, MIPI_DSI_DCS_LONG_WRITE, 0, 0, sizeof(DSI_VT_MOVIE_MDNIE_2), DSI_VT_MOVIE_MDNIE_2, 0, NULL}, false, 0},
	{{0, MIPI_DSI_DCS_LONG_WRITE, 0, 0, sizeof(DSI_VT_MOVIE_MDNIE_3), DSI_VT_MOVIE_MDNIE_3, 0, NULL}, false, 0},
	{{0, MIPI_DSI_DCS_LONG_WRITE, 0, 0, sizeof(level_1_key_off), level_1_key_off, 0, NULL}, true, 0},
};

static struct dsi_cmd_desc DSI_VT_AUTO_MDNIE[] = {
	{{0, MIPI_DSI_DCS_LONG_WRITE, 0, 0, sizeof(level_1_key_on), level_1_key_on, 0, NULL}, false, 0},
	{{0, MIPI_DSI_DCS_LONG_WRITE, 0, 0, sizeof(DSI_VT_AUTO_MDNIE_1), DSI_VT_AUTO_MDNIE_1, 0, NULL}, false, 0},
	{{0, MIPI_DSI_DCS_LONG_WRITE, 0, 0, sizeof(DSI_VT_AUTO_MDNIE_2), DSI_VT_AUTO_MDNIE_2, 0, NULL}, false, 0},
	{{0, MIPI_DSI_DCS_LONG_WRITE, 0, 0, sizeof(DSI_VT_AUTO_MDNIE_3), DSI_VT_AUTO_MDNIE_3, 0, NULL}, false, 0},
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

static struct dsi_cmd_desc DSI_BROWSER_MOVIE_MDNIE[] = {
	{{0, MIPI_DSI_DCS_LONG_WRITE, 0, 0, sizeof(level_1_key_on), level_1_key_on, 0, NULL}, false, 0},
	{{0, MIPI_DSI_DCS_LONG_WRITE, 0, 0, sizeof(DSI_BROWSER_MOVIE_MDNIE_1), DSI_BROWSER_MOVIE_MDNIE_1, 0, NULL}, false, 0},
	{{0, MIPI_DSI_DCS_LONG_WRITE, 0, 0, sizeof(DSI_BROWSER_MOVIE_MDNIE_2), DSI_BROWSER_MOVIE_MDNIE_2, 0, NULL}, false, 0},
	{{0, MIPI_DSI_DCS_LONG_WRITE, 0, 0, sizeof(DSI_BROWSER_MOVIE_MDNIE_3), DSI_BROWSER_MOVIE_MDNIE_3, 0, NULL}, false, 0},
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

static struct dsi_cmd_desc DSI_EBOOK_MOVIE_MDNIE[] = {
	{{0, MIPI_DSI_DCS_LONG_WRITE, 0, 0, sizeof(level_1_key_on), level_1_key_on, 0, NULL}, false, 0},
	{{0, MIPI_DSI_DCS_LONG_WRITE, 0, 0, sizeof(DSI_EBOOK_MOVIE_MDNIE_1), DSI_EBOOK_MOVIE_MDNIE_1, 0, NULL}, false, 0},
	{{0, MIPI_DSI_DCS_LONG_WRITE, 0, 0, sizeof(DSI_EBOOK_MOVIE_MDNIE_2), DSI_EBOOK_MOVIE_MDNIE_2, 0, NULL}, false, 0},
	{{0, MIPI_DSI_DCS_LONG_WRITE, 0, 0, sizeof(DSI_EBOOK_MOVIE_MDNIE_3), DSI_EBOOK_MOVIE_MDNIE_3, 0, NULL}, false, 0},
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
	{{0, MIPI_DSI_DCS_LONG_WRITE, 0, 0, sizeof(DSI_GAME_LOW_MDNIE_1), DSI_GAME_LOW_MDNIE_1, 0, NULL}, false, 0},
	{{0, MIPI_DSI_DCS_LONG_WRITE, 0, 0, sizeof(DSI_GAME_LOW_MDNIE_2), DSI_GAME_LOW_MDNIE_2, 0, NULL}, false, 0},
	{{0, MIPI_DSI_DCS_LONG_WRITE, 0, 0, sizeof(DSI_GAME_LOW_MDNIE_3), DSI_GAME_LOW_MDNIE_3, 0, NULL}, false, 0},
	{{0, MIPI_DSI_DCS_LONG_WRITE, 0, 0, sizeof(level_1_key_off), level_1_key_off, 0, NULL}, true, 0},
};

static struct dsi_cmd_desc DSI_GAME_MID_MDNIE[] = {
	{{0, MIPI_DSI_DCS_LONG_WRITE, 0, 0, sizeof(level_1_key_on), level_1_key_on, 0, NULL}, false, 0},
	{{0, MIPI_DSI_DCS_LONG_WRITE, 0, 0, sizeof(DSI_GAME_MID_MDNIE_1), DSI_GAME_MID_MDNIE_1, 0, NULL}, false, 0},
	{{0, MIPI_DSI_DCS_LONG_WRITE, 0, 0, sizeof(DSI_GAME_MID_MDNIE_2), DSI_GAME_MID_MDNIE_2, 0, NULL}, false, 0},
	{{0, MIPI_DSI_DCS_LONG_WRITE, 0, 0, sizeof(DSI_GAME_MID_MDNIE_3), DSI_GAME_MID_MDNIE_3, 0, NULL}, false, 0},
	{{0, MIPI_DSI_DCS_LONG_WRITE, 0, 0, sizeof(level_1_key_off), level_1_key_off, 0, NULL}, true, 0},
};

static struct dsi_cmd_desc DSI_GAME_HIGH_MDNIE[] = {
	{{0, MIPI_DSI_DCS_LONG_WRITE, 0, 0, sizeof(level_1_key_on), level_1_key_on, 0, NULL}, false, 0},
	{{0, MIPI_DSI_DCS_LONG_WRITE, 0, 0, sizeof(DSI_GAME_HIGH_MDNIE_1), DSI_GAME_HIGH_MDNIE_1, 0, NULL}, false, 0},
	{{0, MIPI_DSI_DCS_LONG_WRITE, 0, 0, sizeof(DSI_GAME_HIGH_MDNIE_2), DSI_GAME_HIGH_MDNIE_2, 0, NULL}, false, 0},
	{{0, MIPI_DSI_DCS_LONG_WRITE, 0, 0, sizeof(DSI_GAME_HIGH_MDNIE_3), DSI_GAME_HIGH_MDNIE_3, 0, NULL}, false, 0},
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

static struct dsi_cmd_desc DSI_TDMB_MOVIE_MDNIE[] = {
	{{0, MIPI_DSI_DCS_LONG_WRITE, 0, 0, sizeof(level_1_key_on), level_1_key_on, 0, NULL}, false, 0},
	{{0, MIPI_DSI_DCS_LONG_WRITE, 0, 0, sizeof(DSI_TDMB_MOVIE_MDNIE_1), DSI_TDMB_MOVIE_MDNIE_1, 0, NULL}, false, 0},
	{{0, MIPI_DSI_DCS_LONG_WRITE, 0, 0, sizeof(DSI_TDMB_MOVIE_MDNIE_2), DSI_TDMB_MOVIE_MDNIE_2, 0, NULL}, false, 0},
	{{0, MIPI_DSI_DCS_LONG_WRITE, 0, 0, sizeof(DSI_TDMB_MOVIE_MDNIE_3), DSI_TDMB_MOVIE_MDNIE_3, 0, NULL}, false, 0},
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
			{DSI_UI_MOVIE_MDNIE,	NULL},
			{DSI_UI_AUTO_MDNIE,	NULL},
			{DSI_EBOOK_AUTO_MDNIE,	NULL},
		},
		// VIDEO_APP
		{
			{DSI_VIDEO_DYNAMIC_MDNIE,	DSI_VIDEO_OUTDOOR_MDNIE},
			{DSI_VIDEO_STANDARD_MDNIE,	DSI_VIDEO_OUTDOOR_MDNIE},
			{DSI_VIDEO_NATURAL_MDNIE,	DSI_VIDEO_OUTDOOR_MDNIE},
			{DSI_VIDEO_MOVIE_MDNIE,	DSI_VIDEO_OUTDOOR_MDNIE},
			{DSI_VIDEO_AUTO_MDNIE,	DSI_VIDEO_OUTDOOR_MDNIE},
			{DSI_EBOOK_AUTO_MDNIE,	DSI_VIDEO_OUTDOOR_MDNIE},
		},
		// VIDEO_WARM_APP
		{
			{DSI_VIDEO_WARM_MDNIE,	DSI_VIDEO_WARM_OUTDOOR_MDNIE},
			{DSI_VIDEO_WARM_MDNIE,	DSI_VIDEO_WARM_OUTDOOR_MDNIE},
			{DSI_VIDEO_WARM_MDNIE,	DSI_VIDEO_WARM_OUTDOOR_MDNIE},
			{DSI_VIDEO_WARM_MDNIE,	DSI_VIDEO_WARM_OUTDOOR_MDNIE},
			{DSI_VIDEO_WARM_MDNIE,	DSI_VIDEO_WARM_OUTDOOR_MDNIE},
			{DSI_EBOOK_AUTO_MDNIE,	DSI_VIDEO_WARM_OUTDOOR_MDNIE},
		},
		// VIDEO_COLD_APP
		{
			{DSI_VIDEO_COLD_MDNIE,	DSI_VIDEO_COLD_OUTDOOR_MDNIE},
			{DSI_VIDEO_COLD_MDNIE,	DSI_VIDEO_COLD_OUTDOOR_MDNIE},
			{DSI_VIDEO_COLD_MDNIE,	DSI_VIDEO_COLD_OUTDOOR_MDNIE},
			{DSI_VIDEO_COLD_MDNIE,	DSI_VIDEO_COLD_OUTDOOR_MDNIE},
			{DSI_VIDEO_COLD_MDNIE,	DSI_VIDEO_COLD_OUTDOOR_MDNIE},
			{DSI_EBOOK_AUTO_MDNIE,	DSI_VIDEO_COLD_OUTDOOR_MDNIE},
		},
		// CAMERA_APP
		{
			{DSI_CAMERA_DYNAMIC_MDNIE,	DSI_CAMERA_OUTDOOR_MDNIE},
			{DSI_CAMERA_STANDARD_MDNIE,	DSI_CAMERA_OUTDOOR_MDNIE},
			{DSI_CAMERA_NATURAL_MDNIE,	DSI_CAMERA_OUTDOOR_MDNIE},
			{DSI_CAMERA_MOVIE_MDNIE,	DSI_CAMERA_OUTDOOR_MDNIE},
			{DSI_CAMERA_AUTO_MDNIE,	DSI_CAMERA_OUTDOOR_MDNIE},
			{DSI_EBOOK_AUTO_MDNIE,	DSI_CAMERA_OUTDOOR_MDNIE},
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
			{DSI_GALLERY_MOVIE_MDNIE,	NULL},
			{DSI_GALLERY_AUTO_MDNIE,	NULL},
			{DSI_EBOOK_AUTO_MDNIE,	NULL},
		},
		// VT_APP
		{
			{DSI_VT_DYNAMIC_MDNIE,	NULL},
			{DSI_VT_STANDARD_MDNIE,	NULL},
			{DSI_VT_NATURAL_MDNIE,	NULL},
			{DSI_VT_MOVIE_MDNIE,	NULL},
			{DSI_VT_AUTO_MDNIE,	NULL},
			{DSI_EBOOK_AUTO_MDNIE,	NULL},
		},
		// BROWSER_APP
		{
			{DSI_BROWSER_DYNAMIC_MDNIE,	NULL},
			{DSI_BROWSER_STANDARD_MDNIE,	NULL},
			{DSI_BROWSER_NATURAL_MDNIE,	NULL},
			{DSI_BROWSER_MOVIE_MDNIE,	NULL},
			{DSI_BROWSER_AUTO_MDNIE,	NULL},
			{DSI_EBOOK_AUTO_MDNIE,	NULL},
		},
		// eBOOK_APP
		{
			{DSI_EBOOK_DYNAMIC_MDNIE,	NULL},
			{DSI_EBOOK_STANDARD_MDNIE,NULL},
			{DSI_EBOOK_NATURAL_MDNIE,	NULL},
			{DSI_EBOOK_MOVIE_MDNIE,	NULL},
			{DSI_EBOOK_AUTO_MDNIE,	NULL},
			{DSI_EBOOK_AUTO_MDNIE,	NULL},
		},
		// EMAIL_APP
		{
			{DSI_EMAIL_AUTO_MDNIE,	NULL},
			{DSI_EMAIL_AUTO_MDNIE,	NULL},
			{DSI_EMAIL_AUTO_MDNIE,	NULL},
			{DSI_EMAIL_AUTO_MDNIE,	NULL},
			{DSI_EMAIL_AUTO_MDNIE,	NULL},
			{DSI_EBOOK_AUTO_MDNIE,	NULL},
		},
		// GAME_LOW_APP
		{
			{DSI_GAME_LOW_MDNIE,	NULL},
			{DSI_GAME_LOW_MDNIE,	NULL},
			{DSI_GAME_LOW_MDNIE,	NULL},
			{DSI_GAME_LOW_MDNIE,	NULL},
			{DSI_GAME_LOW_MDNIE,	NULL},
			{DSI_GAME_LOW_MDNIE,	NULL},
		},
		// GAME_MID_APP
		{
			{DSI_GAME_MID_MDNIE,	NULL},
			{DSI_GAME_MID_MDNIE,	NULL},
			{DSI_GAME_MID_MDNIE,	NULL},
			{DSI_GAME_MID_MDNIE,	NULL},
			{DSI_GAME_MID_MDNIE,	NULL},
			{DSI_GAME_MID_MDNIE,	NULL},
		},
		// GAME_HIGH_APP
		{
			{DSI_GAME_HIGH_MDNIE,	NULL},
			{DSI_GAME_HIGH_MDNIE,	NULL},
			{DSI_GAME_HIGH_MDNIE,	NULL},
			{DSI_GAME_HIGH_MDNIE,	NULL},
			{DSI_GAME_HIGH_MDNIE,	NULL},
			{DSI_GAME_HIGH_MDNIE,	NULL},
		},
		// VIDEO_ENHANCER_APP
		{
			{DSI_VIDEO_ENHANCER_MDNIE,	NULL},
			{DSI_VIDEO_ENHANCER_MDNIE,	NULL},
			{DSI_VIDEO_ENHANCER_MDNIE,	NULL},
			{DSI_VIDEO_ENHANCER_MDNIE,	NULL},
			{DSI_VIDEO_ENHANCER_MDNIE,	NULL},
			{DSI_VIDEO_ENHANCER_MDNIE,	NULL},
		},
		// VIDEO_ENHANCER_THIRD_APP
		{
			{DSI_VIDEO_ENHANCER_THIRD_MDNIE,	NULL},
			{DSI_VIDEO_ENHANCER_THIRD_MDNIE,	NULL},
			{DSI_VIDEO_ENHANCER_THIRD_MDNIE,	NULL},
			{DSI_VIDEO_ENHANCER_THIRD_MDNIE,	NULL},
			{DSI_VIDEO_ENHANCER_THIRD_MDNIE,	NULL},
			{DSI_VIDEO_ENHANCER_THIRD_MDNIE,	NULL},
		},
		// TDMB_APP
		{
			{DSI_TDMB_DYNAMIC_MDNIE,       NULL},
			{DSI_TDMB_STANDARD_MDNIE,      NULL},
			{DSI_TDMB_NATURAL_MDNIE,       NULL},
			{DSI_TDMB_MOVIE_MDNIE,         NULL},
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
		HMT_COLOR_TEMP_6500K, // 6500K + gamma 2.2
		HMT_COLOR_TEMP_7500K, // 7500K + gamma 2.2
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
	DSI_HDR_VIDEO_4_MDNIE,
	DSI_HDR_VIDEO_5_MDNIE,
};

#define DSI_RGB_SENSOR_MDNIE_1_SIZE ARRAY_SIZE(DSI_RGB_SENSOR_MDNIE_1)
#define DSI_RGB_SENSOR_MDNIE_2_SIZE ARRAY_SIZE(DSI_RGB_SENSOR_MDNIE_2)
#define DSI_RGB_SENSOR_MDNIE_3_SIZE ARRAY_SIZE(DSI_RGB_SENSOR_MDNIE_3)


#endif
