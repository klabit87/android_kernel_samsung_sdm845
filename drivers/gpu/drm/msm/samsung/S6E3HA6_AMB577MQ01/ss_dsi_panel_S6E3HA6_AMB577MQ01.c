/*
 * =================================================================
 *
 *
 *	Description:  samsung display panel file
 *
 *	Author: jb09.kim
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
*/
#include "ss_dsi_panel_S6E3HA6_AMB577MQ01.h"
#include "ss_dsi_mdnie_S6E3HA6_AMB577MQ01.h"

/* AOD Mode status on AOD Service */

enum {
	ALPM_CTRL_2NIT,
	ALPM_CTRL_60NIT,
	HLPM_CTRL_2NIT,
	HLPM_CTRL_60NIT,
	MAX_LPM_CTRL,
};

/* Register to control brightness level */
#define ALPM_REG	0x53
/* Register to cnotrol ALPM/HLPM mode */
#define ALPM_CTRL_REG	0xBB
#define LUMINANCE_MAX 74

static int samsung_panel_on_pre(struct samsung_display_driver_data *vdd)
{
	int on_reg_list[1][2] = { {POC_CTRL_REG, -EINVAL} };
	struct dsi_panel_cmd_set *on_cmd_list[1];
	char poc_buffer[4] = {0,};
	static unsigned int i_poc_buffer[4] = {0,};
	int MAX_POC = 4;
	int loop;

	on_cmd_list[0] = ss_get_cmds(vdd, DSI_CMD_SET_ON);

	LCD_INFO("+: ndx=%d\n", vdd->ndx);
	ss_panel_attach_set(vdd, true);

	if (!vdd->poc_driver.is_support) {
		LCD_DEBUG("Not Support POC Function \n");
		goto end;
	}

	/* Read Panel POC (EBh 1nd~4th) */
	if (ss_get_cmds(vdd, RX_POC_STATUS)->count) {
		memset(poc_buffer, 0x00, sizeof(poc_buffer[0]) * MAX_POC);

		if (unlikely(vdd->is_factory_mode) &&
				vdd->dtsi_data.samsung_support_factory_panel_swap) {
			memset(i_poc_buffer, 0x00, sizeof(i_poc_buffer[0]) * MAX_POC);
		}

		if (i_poc_buffer[3] == 0) {
			ss_panel_data_read(vdd, RX_POC_STATUS,
					poc_buffer, LEVEL1_KEY);

			for (loop = 0; loop < MAX_POC; loop++)
				i_poc_buffer[loop] = (unsigned int)poc_buffer[loop];
		}

		LCD_DEBUG("[POC] DSI%d: %02x %02x %02x %02x\n",
				vdd->ndx,
				i_poc_buffer[0],
				i_poc_buffer[1],
				i_poc_buffer[2],
				i_poc_buffer[3]);

		/*
		 * Update REBh 4th param to 0xFF or 0x64
		 */
		ss_find_reg_offset(on_reg_list, on_cmd_list,
				sizeof(on_cmd_list) / sizeof(on_cmd_list[0]));

		if ((on_reg_list[0][1] != -EINVAL) &&\
			(vdd->octa_id_dsi[1] == 0x1)) {		/* POC CHECK  C9 3th Para */
			if (i_poc_buffer[3] == 0x33)
				i_poc_buffer[3] = 0x64;

			on_cmd_list[0]->cmds[on_reg_list[0][1]].msg.tx_buf[4] =
				i_poc_buffer[3];

			LCD_DEBUG("Update POC register, 0x%02x\n",
					i_poc_buffer[3]);
		}

		LCD_DEBUG("[POC] DSI%d: octa_id:%d, poc_buffer:%02x, index:%d\n",
				vdd->ndx,
				vdd->octa_id_dsi[1],
				i_poc_buffer[3],
				on_reg_list[0][1]);
	} else {
		LCD_ERR("DSI%d no poc_rx_cmds cmd\n", vdd->ndx);
	}

end:
	LCD_INFO("-: ndx=%d \n", vdd->ndx);

	return true;
}

static int samsung_panel_on_post(struct samsung_display_driver_data *vdd)
{
	return true;
}

static char ss_panel_revision(struct samsung_display_driver_data *vdd)
{
	if (vdd->manufacture_id_dsi == PBA_ID)
		ss_panel_attach_set(vdd, false);
	else
		ss_panel_attach_set(vdd, true);

	switch (ss_panel_rev_get(vdd)) {
	case 0x00:
		/* Rev.Pre */
	case 0x01:
		/* Rev.A */
		vdd->panel_revision = 'A';
		break;
	case 0x02:
		/* Rev.B */
		vdd->panel_revision = 'B';
		break;
	case 0x03:
		/* Rev.C */
		vdd->panel_revision = 'C';
		break;
	case 0x04:
		/* Rev.D / E / F */
		/* vdd->panel_revision = 'D'; */
		/* vdd->panel_revision = 'E'; */
		vdd->panel_revision = 'F';
		break;
	case 0x05:
		/* Rev.G / H */
		/* vdd->panel_revision = 'G'; */
		vdd->panel_revision = 'H';
		break;
	default:
		vdd->panel_revision = 'H';
		LCD_ERR("Invalid panel_rev(default rev : %c)\n",
				vdd->panel_revision);
		break;
	}

	vdd->panel_revision -= 'A';

	LCD_INFO_ONCE("panel_revision = %c %d \n",
					vdd->panel_revision + 'A', vdd->panel_revision);

	return (vdd->panel_revision + 'A');
}

static int ss_manufacture_date_read(struct samsung_display_driver_data *vdd)
{
	unsigned char date[4];
	int year, month, day;
	int hour, min;

	if (IS_ERR_OR_NULL(vdd)) {
		LCD_ERR("Invalid data vdd : 0x%zx", (size_t)vdd);
		return false;
	}

	/* Read mtp (A1h 5,6 th) for manufacture date */
	if (ss_get_cmds(vdd, RX_MANUFACTURE_DATE)->count) {
		ss_panel_data_read(vdd, RX_MANUFACTURE_DATE, date, LEVEL1_KEY);

		year = date[0] & 0xf0;
		year >>= 4;
		year += 2011; // 0 = 2011 year
		month = date[0] & 0x0f;
		day = date[1] & 0x1f;
		hour = date[2] & 0x0f;
		min = date[3] & 0x1f;

		vdd->manufacture_date_dsi = year * 10000 + month * 100 + day;
		vdd->manufacture_time_dsi = hour * 100 + min;

		LCD_ERR("manufacture_date DSI%d = (%d%04d) - year(%d) month(%d) day(%d) hour(%d) min(%d)\n",
			vdd->ndx, vdd->manufacture_date_dsi, vdd->manufacture_time_dsi,
			year, month, day, hour, min);

	} else {
		LCD_ERR("DSI%d no manufacture_date_rx_cmds cmds(%d)", vdd->ndx, vdd->panel_revision);
		return false;
	}

	return true;
}

static int ss_ddi_id_read(struct samsung_display_driver_data *vdd)
{
	char ddi_id[5];
	int loop;

	if (IS_ERR_OR_NULL(vdd)) {
		LCD_ERR("Invalid data vdd : 0x%zx", (size_t)vdd);
		return false;
	}

	/* Read mtp (D6h 1~5th) for ddi id */
	if (ss_get_cmds(vdd, RX_DDI_ID)->count) {
		ss_panel_data_read(vdd, RX_DDI_ID, ddi_id, LEVEL1_KEY);

		for (loop = 0; loop < 5; loop++)
			vdd->ddi_id_dsi[loop] = ddi_id[loop];

		LCD_INFO("DSI%d : %02x %02x %02x %02x %02x\n", vdd->ndx,
			vdd->ddi_id_dsi[0], vdd->ddi_id_dsi[1],
			vdd->ddi_id_dsi[2], vdd->ddi_id_dsi[3],
			vdd->ddi_id_dsi[4]);
	} else {
		LCD_ERR("DSI%d no ddi_id_rx_cmds cmds", vdd->ndx);
		return false;
	}

	return true;
}

static int get_hbm_candela_value(int level)
{
	if (level == 13)
		return 443;
	else if (level == 6)
		return 465;
	else if (level == 7)
		return 488;
	else if (level == 8)
		return 510;
	else if (level == 9)
		return 533;
	else if (level == 10)
		return 555;
	else if (level == 11)
		return 578;
	else if (level == 12)
		return 600;
	else
		return 600;
}

static struct dsi_panel_cmd_set *ss_hbm_gamma(struct samsung_display_driver_data *vdd, int *level_key)
{
	struct dsi_panel_cmd_set *hbm_gamma_cmds = ss_get_cmds(vdd, TX_HBM_GAMMA);

	if (IS_ERR_OR_NULL(vdd) || IS_ERR_OR_NULL(hbm_gamma_cmds)) {
		LCD_ERR("Invalid data  vdd : 0x%zx cmd : 0x%zx", (size_t)vdd, (size_t)hbm_gamma_cmds);
		return NULL;
	}

	if (IS_ERR_OR_NULL(vdd->smart_dimming_dsi->generate_hbm_gamma)) {
		LCD_ERR("generate_hbm_gamma is NULL error");
		return NULL;
	} else {
		vdd->smart_dimming_dsi->generate_hbm_gamma(
			vdd->smart_dimming_dsi,
			vdd->auto_brightness,
			&hbm_gamma_cmds->cmds[0].msg.tx_buf[1]);

		*level_key = LEVEL1_KEY;
		return hbm_gamma_cmds;
	}
}

static struct dsi_panel_cmd_set *ss_hbm_etc(struct samsung_display_driver_data *vdd, int *level_key)
{
	struct dsi_panel_cmd_set *hbm_etc_cmds = ss_get_cmds(vdd, TX_HBM_ETC);

	char elvss_3th_val, elvss_24th_val, elvss_25th_val;
	char elvss_443_offset, elvss_465_offset, elvss_488_offset, elvss_510_offset, elvss_533_offset;
	char elvss_555_offset, elvss_578_offset, elvss_600_offset;

	if (IS_ERR_OR_NULL(vdd) || IS_ERR_OR_NULL(hbm_etc_cmds)) {
		LCD_ERR("Invalid data  vdd : 0x%zx cmd : 0x%zx", (size_t)vdd, (size_t)hbm_etc_cmds);
		return NULL;
	}

	elvss_3th_val = elvss_24th_val = elvss_25th_val = 0;

	/* OTP value - B5 25th */
	elvss_24th_val = vdd->display_status_dsi.elvss_value1;
	elvss_25th_val = vdd->display_status_dsi.elvss_value2;

	/* ELVSS 0xB5 3th*/
	elvss_443_offset = 0x15;
	elvss_465_offset = 0x13;
	elvss_488_offset = 0x12;
	elvss_510_offset = 0x10;
	elvss_533_offset = 0x0E;
	elvss_555_offset = 0x0D;
	elvss_578_offset = 0x0B;
	elvss_600_offset = 0x0A;

	/* ELVSS 0xB5 3th*/
	if (vdd->auto_brightness == HBM_MODE) /* 465CD */
		elvss_3th_val = elvss_465_offset;
	else if (vdd->auto_brightness == HBM_MODE + 1) /* 488CD */
		elvss_3th_val = elvss_488_offset;
	else if (vdd->auto_brightness == HBM_MODE + 2) /* 510CD */
		elvss_3th_val = elvss_510_offset;
	else if (vdd->auto_brightness == HBM_MODE + 3) /* 533CD */
		elvss_3th_val = elvss_533_offset;
	else if (vdd->auto_brightness == HBM_MODE + 4) /* 555CD */
		elvss_3th_val = elvss_555_offset;
	else if (vdd->auto_brightness == HBM_MODE + 5) /* 578CD */
		elvss_3th_val = elvss_578_offset;
	else if (vdd->auto_brightness == HBM_MODE + 6) /* 600CD */
		elvss_3th_val = elvss_600_offset;
	else if (vdd->auto_brightness == HBM_MODE + 7) /* 443CD */
		elvss_3th_val = elvss_443_offset;

	if (vdd->bl_level == 366) {
		elvss_3th_val = 0x00;
		LCD_INFO("bl_level is (%d), so elvss_3th : 0x00 for FingerPrint\n", vdd->bl_level);
	}

	/* 0xB5 2nd temperature */
	hbm_etc_cmds->cmds[1].msg.tx_buf[1] =
			vdd->temperature > 0 ? vdd->temperature : 0x80|(-1*vdd->temperature);

	/* ELVSS 0xB5 3th, elvss_24th_val */
	hbm_etc_cmds->cmds[1].msg.tx_buf[3] = elvss_3th_val;
	hbm_etc_cmds->cmds[3].msg.tx_buf[1] = elvss_25th_val;

	*level_key = LEVEL1_KEY;

	LCD_INFO("0xB5 3th: 0x%x 0xB5 elvss_25th_val(elvss val) 0x%x\n", elvss_3th_val, elvss_25th_val);

	return hbm_etc_cmds;
}

static int ss_elvss_read(struct samsung_display_driver_data *vdd)
{
	char elvss_b5[2];

	if (IS_ERR_OR_NULL(vdd)) {
		LCD_ERR("Invalid data vdd : 0x%zx", (size_t)vdd);
		return false;
	}

	/* Read mtp (B5h 24th,25th) for elvss*/
	ss_panel_data_read(vdd, RX_ELVSS, elvss_b5, LEVEL1_KEY);
	vdd->display_status_dsi.elvss_value1 = elvss_b5[0]; /*0xB5 24th OTP value*/
	vdd->display_status_dsi.elvss_value2 = elvss_b5[1]; /*0xB5 25th */

	return true;
}

static int ss_hbm_read(struct samsung_display_driver_data *vdd)
{
	char hbm_buffer1[33];
	struct dsi_panel_cmd_set *hbm_gamma_cmds = ss_get_cmds(vdd, TX_HBM_GAMMA);
	int loop;

	if (IS_ERR_OR_NULL(vdd) || IS_ERR_OR_NULL(hbm_gamma_cmds)) {
		LCD_ERR("Invalid data vdd : 0x%zx cmds : 0x%zx", (size_t)vdd, (size_t)hbm_gamma_cmds);
		return false;
	}

	/* Read mtp (B3h 3~35th) for hbm gamma */
	ss_panel_data_read(vdd, RX_HBM, hbm_buffer1, LEVEL1_KEY);

	for (loop = 0; loop < 33; loop++) {
		if (loop == 0) {
			/* V255 RGB MSB */
			hbm_gamma_cmds->cmds[0].msg.tx_buf[1] = (hbm_buffer1[loop] & 0x04) >> 2;
			hbm_gamma_cmds->cmds[0].msg.tx_buf[3] = (hbm_buffer1[loop] & 0x02) >> 1;
			hbm_gamma_cmds->cmds[0].msg.tx_buf[5] = hbm_buffer1[loop] & 0x01;
		} else if (loop == 1) {
			/* V255 R LSB */
			hbm_gamma_cmds->cmds[0].msg.tx_buf[2] = hbm_buffer1[loop];
		} else if (loop == 2) {
			/* V255 G LSB */
			hbm_gamma_cmds->cmds[0].msg.tx_buf[4] = hbm_buffer1[loop];
		} else if (loop == 3) {
			/* V255 B LSB */
			hbm_gamma_cmds->cmds[0].msg.tx_buf[6] = hbm_buffer1[loop];
		} else {
			/* +3 means V255 RGB MSB */
			hbm_gamma_cmds->cmds[0].msg.tx_buf[loop + 3] = hbm_buffer1[loop];
		}
	}

	return true;
}

#define COORDINATE_DATA_SIZE 6
#define MDNIE_SCR_WR_ADDR	0x32
#define RGB_INDEX_SIZE 4
#define COEFFICIENT_DATA_SIZE 8

#define F1(x, y) (((y << 10) - (((x << 10) * 49) / 46) + (39 << 10)) >> 10)
#define F2(x, y) (((y << 10) + (((x << 10) * 104) / 29) - (13876 << 10)) >> 10)

static char coordinate_data_1[][COORDINATE_DATA_SIZE] = {
	{0xff, 0x00, 0xff, 0x00, 0xff, 0x00}, /* dummy */
	{0xff, 0x00, 0xfe, 0x00, 0xfa, 0x00}, /* Tune_1 */
	{0xff, 0x00, 0xff, 0x00, 0xff, 0x00}, /* Tune_2 */
	{0xf9, 0x00, 0xfb, 0x00, 0xff, 0x00}, /* Tune_3 */
	{0xff, 0x00, 0xfe, 0x00, 0xfa, 0x00}, /* Tune_4 */
	{0xff, 0x00, 0xff, 0x00, 0xff, 0x00}, /* Tune_5 */
	{0xf9, 0x00, 0xfb, 0x00, 0xff, 0x00}, /* Tune_6 */
	{0xfb, 0x00, 0xff, 0x00, 0xf8, 0x00}, /* Tune_7 */
	{0xfa, 0x00, 0xff, 0x00, 0xfb, 0x00}, /* Tune_8 */
	{0xf8, 0x00, 0xff, 0x00, 0xff, 0x00}, /* Tune_9 */
};

static char coordinate_data_2[][COORDINATE_DATA_SIZE] = {
	{0xff, 0x00, 0xff, 0x00, 0xff, 0x00}, /* dummy */
	{0xff, 0x00, 0xf5, 0x00, 0xec, 0x00}, /* Tune_1 */
	{0xff, 0x00, 0xf6, 0x00, 0xef, 0x00}, /* Tune_2 */
	{0xff, 0x00, 0xf7, 0x00, 0xf4, 0x00}, /* Tune_3 */
	{0xff, 0x00, 0xf8, 0x00, 0xe9, 0x00}, /* Tune_4 */
	{0xff, 0x00, 0xfa, 0x00, 0xf1, 0x00}, /* Tune_5 */
	{0xff, 0x00, 0xfc, 0x00, 0xf5, 0x00}, /* Tune_6 */
	{0xff, 0x00, 0xfe, 0x00, 0xee, 0x00}, /* Tune_7 */
	{0xff, 0x00, 0xff, 0x00, 0xf0, 0x00}, /* Tune_8 */
	{0xfe, 0x00, 0xff, 0x00, 0xf4, 0x00}, /* Tune_9 */
};

static char (*coordinate_data[MAX_MODE])[COORDINATE_DATA_SIZE] = {
	coordinate_data_2,
	coordinate_data_2,
	coordinate_data_2,
	coordinate_data_1,
	coordinate_data_1,
	coordinate_data_1
};

static int rgb_index[][RGB_INDEX_SIZE] = {
	{ 5, 5, 5, 5 }, /* dummy */
	{ 5, 2, 6, 3 },
	{ 5, 2, 4, 1 },
	{ 5, 8, 4, 7 },
	{ 5, 8, 6, 9 },
};

static int coefficient[][COEFFICIENT_DATA_SIZE] = {
	{       0,      0,      0,      0,      0,      0,      0,      0 }, /* dummy */
	{   95580,  78227, -25243, -30003,  29939,  59065, -14830, -13912 },
	{ -102381, -84518,  27387,  32046, -53136, -18756,  11837,  10913 },
	{  -95339, -79478,  25370,  30208, -48165, -77998,  21447,  19240 },
	{   91071,  75568, -24062, -28903,  18369, -13395,   -718,   -617 },
};

static int mdnie_coordinate_index(int x, int y)
{
	int tune_number = 0;

	if (F1(x, y) > 0) {
		if (F2(x, y) > 0) {
			tune_number = 1;
		} else {
			tune_number = 2;
		}
	} else {
		if (F2(x, y) > 0) {
			tune_number = 4;
		} else {
			tune_number = 3;
		}
	}

	return tune_number;
}

static int mdnie_coordinate_x(int x, int y, int index)
{
	int result = 0;

	result = (coefficient[index][0] * x) + (coefficient[index][1] * y) + (((coefficient[index][2] * x + 512) >> 10) * y) + (coefficient[index][3] * 10000);

	result = (result + 512) >> 10;

	if (result < 0)
		result = 0;
	if (result > 1024)
		result = 1024;

	return result;
}

static int mdnie_coordinate_y(int x, int y, int index)
{
	int result = 0;

	result = (coefficient[index][4] * x) + (coefficient[index][5] * y) + (((coefficient[index][6] * x + 512) >> 10) * y) + (coefficient[index][7] * 10000);

	result = (result + 512) >> 10;

	if (result < 0)
		result = 0;
	if (result > 1024)
		result = 1024;

	return result;
}

static int ss_mdnie_read(struct samsung_display_driver_data *vdd)
{
	char x_y_location[4];
	int x, y;
	int mdnie_tune_index = 0;

	if (IS_ERR_OR_NULL(vdd)) {
		LCD_ERR("Invalid data vdd : 0x%zx", (size_t)vdd);
		return false;
	}

	/* Read mtp (A1h 1~5th) for ddi id */
	if (ss_get_cmds(vdd, RX_MDNIE)->count) {
		ss_panel_data_read(vdd, RX_MDNIE, x_y_location, LEVEL1_KEY);

		vdd->mdnie_x = x_y_location[0] << 8 | x_y_location[1];	/* X */
		vdd->mdnie_y = x_y_location[2] << 8 | x_y_location[3];	/* Y */

		mdnie_tune_index = mdnie_coordinate_index(vdd->mdnie_x, vdd->mdnie_y);

		if (((vdd->mdnie_x - 2991) * (vdd->mdnie_x - 2991) + (vdd->mdnie_y - 3148) * (vdd->mdnie_y - 3148)) <= 225) {
			x = 0;
			y = 0;
		} else {
			x = mdnie_coordinate_x(vdd->mdnie_x, vdd->mdnie_y, mdnie_tune_index);
			y = mdnie_coordinate_y(vdd->mdnie_x, vdd->mdnie_y, mdnie_tune_index);
		}

		coordinate_tunning_calculate(vdd, x, y, coordinate_data,
				rgb_index[mdnie_tune_index],
				MDNIE_SCR_WR_ADDR, COORDINATE_DATA_SIZE);

		LCD_INFO("X-%d Y-%d\n", vdd->mdnie_x, vdd->mdnie_y);
	} else {
		LCD_ERR("DSI%d no mdnie_read_rx_cmds cmds", vdd->ndx);
		return false;
	}

	return true;
}

static int ss_samart_dimming_init(struct samsung_display_driver_data *vdd)
{
	struct dsi_panel_cmd_set *pcmds;

	if (IS_ERR_OR_NULL(vdd)) {
		LCD_ERR("Invalid data vdd : 0x%zx", (size_t)vdd);
		return false;
	}

	vdd->smart_dimming_dsi = vdd->panel_func.samsung_smart_get_conf();
	if (IS_ERR_OR_NULL(vdd->smart_dimming_dsi)) {
		LCD_ERR("DSI%d smart_dimming_dsi is null", vdd->ndx);
		return false;
	}

	ss_panel_data_read(vdd, RX_SMART_DIM_MTP,
			vdd->smart_dimming_dsi->mtp_buffer, LEVEL1_KEY);

	/* Initialize smart dimming related things here */
	/* lux_tab setting for 350cd */
	vdd->smart_dimming_dsi->lux_tab = vdd->dtsi_data.candela_map_table[vdd->panel_revision].cd;
	vdd->smart_dimming_dsi->lux_tabsize = vdd->dtsi_data.candela_map_table[vdd->panel_revision].tab_size;
	vdd->smart_dimming_dsi->man_id = vdd->manufacture_id_dsi;
	if (vdd->panel_func.samsung_panel_revision)
		vdd->smart_dimming_dsi->panel_revision = vdd->panel_func.samsung_panel_revision(vdd);

	/* copy hbm gamma payload for hbm interpolation calc */
	pcmds = ss_get_cmds(vdd, TX_HBM_GAMMA);
	vdd->smart_dimming_dsi->hbm_payload = &pcmds->cmds[0].msg.tx_buf[1];

	/* Just a safety check to ensure smart dimming data is initialised well */
	vdd->smart_dimming_dsi->init(vdd->smart_dimming_dsi);

	vdd->temperature = 20; // default temperature

	vdd->smart_dimming_loaded_dsi = true;

	LCD_INFO("DSI%d : --\n", vdd->ndx);

	return true;
}

static int ss_cell_id_read(struct samsung_display_driver_data *vdd)
{
	char cell_id_buffer[MAX_CELL_ID] = {0,};
	int loop;

	if (IS_ERR_OR_NULL(vdd)) {
		LCD_ERR("Invalid data vdd : 0x%zx", (size_t)vdd);
		return false;
	}

	/* Read Panel Unique Cell ID (C8h 41~51th) */
	if (ss_get_cmds(vdd, RX_CELL_ID)->count) {
		memset(cell_id_buffer, 0x00, MAX_CELL_ID);

		ss_panel_data_read(vdd, RX_CELL_ID, cell_id_buffer, LEVEL1_KEY);

		for (loop = 0; loop < MAX_CELL_ID; loop++)
			vdd->cell_id_dsi[loop] = cell_id_buffer[loop];

		LCD_INFO("DSI%d: %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x\n",
			vdd->ndx, vdd->cell_id_dsi[0],
			vdd->cell_id_dsi[1],	vdd->cell_id_dsi[2],
			vdd->cell_id_dsi[3],	vdd->cell_id_dsi[4],
			vdd->cell_id_dsi[5],	vdd->cell_id_dsi[6],
			vdd->cell_id_dsi[7],	vdd->cell_id_dsi[8],
			vdd->cell_id_dsi[9],	vdd->cell_id_dsi[10]);

	} else {
		LCD_ERR("DSI%d no cell_id_rx_cmds cmd\n", vdd->ndx);
		return false;
	}

	return true;
}

static int ss_octa_id_read(struct samsung_display_driver_data *vdd)
{
	if (IS_ERR_OR_NULL(vdd)) {
		LCD_ERR("Invalid data vdd : 0x%zx", (size_t)vdd);
		return false;
	}

	/* Read Panel Unique OCTA ID (C9h 2nd~21th) */
	if (ss_get_cmds(vdd, RX_OCTA_ID)->count) {
		memset(vdd->octa_id_dsi, 0x00, MAX_OCTA_ID);

		ss_panel_data_read(vdd, RX_OCTA_ID,
				vdd->octa_id_dsi, LEVEL1_KEY);

		LCD_INFO("octa id: %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x\n",
			vdd->octa_id_dsi[0], vdd->octa_id_dsi[1],
			vdd->octa_id_dsi[2], vdd->octa_id_dsi[3],
			vdd->octa_id_dsi[4], vdd->octa_id_dsi[5],
			vdd->octa_id_dsi[6], vdd->octa_id_dsi[7],
			vdd->octa_id_dsi[8], vdd->octa_id_dsi[9],
			vdd->octa_id_dsi[10], vdd->octa_id_dsi[11],
			vdd->octa_id_dsi[12], vdd->octa_id_dsi[13],
			vdd->octa_id_dsi[14], vdd->octa_id_dsi[15],
			vdd->octa_id_dsi[16], vdd->octa_id_dsi[17],
			vdd->octa_id_dsi[18], vdd->octa_id_dsi[19]);

	} else {
		LCD_ERR("DSI%d no octa_id_rx_cmds cmd\n", vdd->ndx);
		return false;
	}

	return true;
}

static struct dsi_panel_cmd_set aid_cmd;
static struct dsi_panel_cmd_set *ss_aid(struct samsung_display_driver_data *vdd, int *level_key)
{
	int cd_index = 0;

	if (IS_ERR_OR_NULL(vdd)) {
		LCD_ERR("Invalid data vdd : 0x%zx", (size_t)vdd);
		return NULL;
	}

	if (vdd->pac)
		cd_index = vdd->pac_cd_idx;
	else
		cd_index = vdd->bl_level;

	aid_cmd.count = 1;
	aid_cmd.cmds = &(ss_get_cmds(vdd, TX_AID_SUBDIVISION)->cmds[cd_index]);
	LCD_DEBUG("[%d] level(%d), aid(%x %x)\n",
			cd_index, vdd->bl_level,
			aid_cmd.cmds->msg.tx_buf[1],
			aid_cmd.cmds->msg.tx_buf[2]);

	*level_key = LEVEL1_KEY;

	return &aid_cmd;
}

static struct dsi_panel_cmd_set *ss_acl_on(struct samsung_display_driver_data *vdd, int *level_key)
{
	struct dsi_panel_cmd_set *pcmds;

	if (IS_ERR_OR_NULL(vdd)) {
		LCD_ERR("Invalid data vdd : 0x%zx", (size_t)vdd);
		return NULL;
	}

	*level_key = LEVEL1_KEY;

	if (vdd->gradual_acl_val) {
		pcmds = ss_get_cmds(vdd, TX_ACL_ON);
		pcmds->cmds[0].msg.tx_buf[7] = vdd->gradual_acl_val;
	}

	return ss_get_cmds(vdd, TX_ACL_ON);
}

static struct dsi_panel_cmd_set *ss_acl_off(struct samsung_display_driver_data *vdd, int *level_key)
{
	if (IS_ERR_OR_NULL(vdd)) {
		LCD_ERR("Invalid data vdd : 0x%zx", (size_t)vdd);
		return NULL;
	}

	*level_key = LEVEL1_KEY;

	return ss_get_cmds(vdd, TX_ACL_OFF);
}

static struct dsi_panel_cmd_set elvss_cmd;
static struct dsi_panel_cmd_set *ss_elvss(struct samsung_display_driver_data *vdd, int *level_key)
{
	struct dsi_panel_cmd_set *elvss_cmds = ss_get_cmds(vdd, TX_ELVSS);
	int cd_index = 0;
	int cmd_idx = 0;
	char elvss_3th_val;
	char elvss_3th_val_array[SUPPORT_LOWTEMP_ELVSS][MAX_TEMP];
	char panel_revision = 'A';

	if (IS_ERR_OR_NULL(vdd) || IS_ERR_OR_NULL(elvss_cmds)) {
		LCD_ERR("Invalid data vdd : 0x%zx cmds : 0x%zx", (size_t)vdd, (size_t)elvss_cmds);
		return NULL;
	}

	panel_revision += vdd->panel_revision;

	elvss_3th_val_array[14][HIGH_TEMP] = 0x15;
	elvss_3th_val_array[14][MID_TEMP] = 0x14;
	elvss_3th_val_array[14][LOW_TEMP] = 0x17;
	elvss_3th_val_array[13][HIGH_TEMP] = 0x13;
	elvss_3th_val_array[13][MID_TEMP] = 0x14;
	elvss_3th_val_array[13][LOW_TEMP] = 0x17;
	elvss_3th_val_array[12][HIGH_TEMP] = 0x11;
	elvss_3th_val_array[12][MID_TEMP] = 0x14;
	elvss_3th_val_array[12][LOW_TEMP] = 0x17;
	elvss_3th_val_array[11][HIGH_TEMP] = 0x0F;
	elvss_3th_val_array[11][MID_TEMP] = 0x14;
	elvss_3th_val_array[11][LOW_TEMP] = 0x17;
	elvss_3th_val_array[10][HIGH_TEMP] = 0x0D;
	elvss_3th_val_array[10][MID_TEMP] = 0x14;
	elvss_3th_val_array[10][LOW_TEMP] = 0x17;
	elvss_3th_val_array[9][HIGH_TEMP] = 0x0C;
	elvss_3th_val_array[9][MID_TEMP] = 0x14;
	elvss_3th_val_array[9][LOW_TEMP] = 0x17;
	elvss_3th_val_array[8][HIGH_TEMP] = 0x0B;
	elvss_3th_val_array[8][MID_TEMP] = 0x10;
	elvss_3th_val_array[8][LOW_TEMP] = 0x17;
	elvss_3th_val_array[7][HIGH_TEMP] = 0x0A;
	elvss_3th_val_array[7][MID_TEMP] = 0x10;
	elvss_3th_val_array[7][LOW_TEMP] = 0x17;
	elvss_3th_val_array[6][HIGH_TEMP] = 0x09;
	elvss_3th_val_array[6][MID_TEMP] = 0x10;
	elvss_3th_val_array[6][LOW_TEMP] = 0x17;
	elvss_3th_val_array[5][HIGH_TEMP] = 0x08;
	elvss_3th_val_array[5][MID_TEMP] = 0x10;
	elvss_3th_val_array[5][LOW_TEMP] = 0x17;
	elvss_3th_val_array[4][HIGH_TEMP] = 0x08;
	elvss_3th_val_array[4][MID_TEMP] = 0x10;
	elvss_3th_val_array[4][LOW_TEMP] = 0x17;
	elvss_3th_val_array[3][HIGH_TEMP] = 0x07;
	elvss_3th_val_array[3][MID_TEMP] = 0x10;
	elvss_3th_val_array[3][LOW_TEMP] = 0x17;
	elvss_3th_val_array[2][HIGH_TEMP] = 0x07;
	elvss_3th_val_array[2][MID_TEMP] = 0x10;
	elvss_3th_val_array[2][LOW_TEMP] = 0x17;

	/* 1CD and 0CD is not used */
	elvss_3th_val_array[1][HIGH_TEMP] = 0x07;
	elvss_3th_val_array[1][MID_TEMP] = 0x10;
	elvss_3th_val_array[1][LOW_TEMP] = 0x17;
	elvss_3th_val_array[0][HIGH_TEMP] = 0x07;
	elvss_3th_val_array[0][MID_TEMP] = 0x10;
	elvss_3th_val_array[0][LOW_TEMP] = 0x17;

	cd_index  = vdd->cd_idx;
	LCD_DEBUG("cd_index (%d)\n", cd_index);

	if (!vdd->dtsi_data.smart_acl_elvss_map_table[vdd->panel_revision].size ||
		cd_index > vdd->dtsi_data.smart_acl_elvss_map_table[vdd->panel_revision].size)
		goto end;

	cmd_idx = vdd->dtsi_data.smart_acl_elvss_map_table[vdd->panel_revision].cmd_idx[cd_index];

	/* ELVSS Compensation for Low Temperature & Low Birghtness*/
	if ((vdd->temperature <= 0) && (vdd->candela_level <= 14)) {
		if (vdd->temperature > vdd->elvss_interpolation_temperature)
			elvss_3th_val =
				elvss_3th_val_array[vdd->candela_level][MID_TEMP];
		else
			elvss_3th_val =
				elvss_3th_val_array[vdd->candela_level][LOW_TEMP];

		LCD_DEBUG("temperature(%d) level(%d):B5 3th (0x%x)\n", vdd->temperature, vdd->candela_level, elvss_3th_val);
		elvss_cmds->cmds[cmd_idx].msg.tx_buf[3] = elvss_3th_val;
	} else if ((vdd->temperature > 0) && (vdd->candela_level <= 14)) {
		elvss_3th_val =
				elvss_3th_val_array[vdd->candela_level][HIGH_TEMP];
		LCD_DEBUG("temperature(%d) level(%d):B5 3th (0x%x)\n", vdd->temperature, vdd->candela_level, elvss_3th_val);
		elvss_cmds->cmds[cmd_idx].msg.tx_buf[3] = elvss_3th_val;
	}

	elvss_cmd.cmds = &(elvss_cmds->cmds[cmd_idx]);
	elvss_cmd.count = 1;
	*level_key = LEVEL1_KEY;

	return &elvss_cmd;

end:
	LCD_ERR("error");
	return NULL;
}

static struct dsi_panel_cmd_set *ss_elvss_temperature1(struct samsung_display_driver_data *vdd, int *level_key)
{
	struct dsi_panel_cmd_set *cmds = ss_get_cmds(vdd, TX_ELVSS_LOWTEMP);
	char elvss_24th_val;

	if (IS_ERR_OR_NULL(vdd) || IS_ERR_OR_NULL(cmds)) {
		LCD_ERR("Invalid data vdd : 0x%zx cmds : 0x%zx", (size_t)vdd, (size_t)cmds);
		return NULL;
	}

	/* OTP value - B5 24th */
	elvss_24th_val = vdd->display_status_dsi.elvss_value1;

	LCD_DEBUG("OTP val %x\n", elvss_24th_val);

	/* 0xB5 1th TSET */
	cmds->cmds[0].msg.tx_buf[1] =
		vdd->temperature > 0 ? vdd->temperature : 0x80|(-1*vdd->temperature);

	/* 0xB5 elvss_24th_val elvss_offset */
	cmds->cmds[2].msg.tx_buf[1] = elvss_24th_val;

	LCD_DEBUG("acl : %d, interpolation_temp : %d temp : %d, cd : %d, B5 1st :0x%x, B5 elvss_24th_val :0x%x\n",
		vdd->acl_status, vdd->elvss_interpolation_temperature, vdd->temperature, vdd->candela_level,
		cmds->cmds[0].msg.tx_buf[1], cmds->cmds[2].msg.tx_buf[1]);

	*level_key = LEVEL1_KEY;

	return cmds;
}

static struct dsi_panel_cmd_set vint_cmd;
static struct dsi_panel_cmd_set *ss_vint(struct samsung_display_driver_data *vdd, int *level_key)
{
	struct dsi_panel_cmd_set *vint_cmds = ss_get_cmds(vdd, TX_VINT);
	int cd_index = 0;
	int cmd_idx = 0;

	if (IS_ERR_OR_NULL(vdd) || IS_ERR_OR_NULL(vint_cmds)) {
		LCD_ERR("Invalid data vdd : 0x%zx cmds : 0x%zx", (size_t)vdd, (size_t)vint_cmds);
		return NULL;
	}

	cd_index = vdd->cd_idx;
	LCD_DEBUG("cd_index (%d)\n", cd_index);

	if (!vdd->dtsi_data.vint_map_table[vdd->panel_revision].size ||
		cd_index > vdd->dtsi_data.vint_map_table[vdd->panel_revision].size)
		goto end;

	cmd_idx = vdd->dtsi_data.vint_map_table[vdd->panel_revision].cmd_idx[cd_index];

	if (vdd->temperature > vdd->elvss_interpolation_temperature)
		vint_cmd.cmds = &vint_cmds->cmds[cmd_idx];
	else
		vint_cmd.cmds = &vint_cmds->cmds[0];

	if (vdd->xtalk_mode)
		vint_cmd.cmds[0].msg.tx_buf[1] = 0x6B;	// VGH 6.2 V
	else
		vint_cmd.cmds[0].msg.tx_buf[1] = 0xEB;	// VGH 7.0 V

	vint_cmd.count = 1;
	*level_key = LEVEL1_KEY;

	return &vint_cmd;

end:
	LCD_ERR("error");
	return NULL;
}

static struct dsi_panel_cmd_set irc_cmd;
static struct dsi_panel_cmd_set *ss_irc(struct samsung_display_driver_data *vdd, int *level_key)
{
	struct dsi_panel_cmd_set *irc_cmds = ss_get_cmds(vdd, TX_IRC_SUBDIVISION);
	int cd_index = 0;

	if (IS_ERR_OR_NULL(vdd) || IS_ERR_OR_NULL(irc_cmds)) {
		LCD_ERR("Invalid data vdd : 0x%zx cmds : 0x%zx", (size_t)vdd, (size_t)irc_cmds);
		return NULL;
	}

	if (IS_ERR_OR_NULL(irc_cmds->cmds)) {
		LCD_ERR("No irc_subdivision_tx_cmds\n");
		return NULL;
	}

	if (!vdd->samsung_support_irc)
		return NULL;

	/* IRC Subdivision works like as AID Subdivision */
	if (vdd->pac)
		cd_index = vdd->pac_cd_idx;
	else
		cd_index = vdd->bl_level;

	LCD_DEBUG("irc idx (%d)\n", cd_index);

	irc_cmd.cmds = &(irc_cmds->cmds[cd_index]);
	irc_cmd.count = 1;
	*level_key = LEVEL1_KEY;

	return &irc_cmd;
}

static char irc_hbm_para_revD[8][20] = {
	{0x0D, 0xF0, 0x3B, 0x33, 0x5F, 0xD4, 0x33, 0x69, 0x12, 0x7A, 0xA5, 0x7F, 0x81, 0x7F, 0x2B, 0x2A, 0x2B, 0x29, 0x2B, 0x29}, // 600 ( 365 ~ 365 )
	{0x0D, 0xF0, 0x3B, 0x33, 0x5F, 0xD4, 0x33, 0x69, 0x12, 0x7A, 0xA5, 0x7A, 0x7C, 0x7A, 0x2A, 0x29, 0x2A, 0x28, 0x29, 0x28}, // 578 ( 351 ~ 364 )
	{0x0D, 0xF0, 0x3B, 0x33, 0x5F, 0xD4, 0x33, 0x69, 0x12, 0x7A, 0xA5, 0x76, 0x77, 0x76, 0x27, 0x28, 0x27, 0x27, 0x27, 0x27}, // 555 ( 337 ~ 350 )
	{0x0D, 0xF0, 0x3B, 0x33, 0x5F, 0xD4, 0x33, 0x69, 0x12, 0x7A, 0xA5, 0x71, 0x72, 0x71, 0x26, 0x26, 0x26, 0x25, 0x26, 0x25}, // 533 ( 324 ~ 336 )
	{0x0D, 0xF0, 0x3B, 0x33, 0x5F, 0xD4, 0x33, 0x69, 0x12, 0x7A, 0xA5, 0x6C, 0x6D, 0x6C, 0x25, 0x25, 0x25, 0x23, 0x24, 0x23}, // 510 ( 310 ~ 323 )
	{0x0D, 0xF0, 0x3B, 0x33, 0x5F, 0xD4, 0x33, 0x69, 0x12, 0x7A, 0xA5, 0x67, 0x69, 0x67, 0x23, 0x22, 0x23, 0x22, 0x23, 0x22}, // 488 ( 296 ~ 309 )
	{0x0D, 0xF0, 0x3B, 0x33, 0x5F, 0xD4, 0x33, 0x69, 0x12, 0x7A, 0xA5, 0x63, 0x64, 0x63, 0x21, 0x21, 0x21, 0x20, 0x21, 0x20}, // 465 ( 282 ~ 295 )
	{0x0D, 0xF0, 0x3B, 0x33, 0x5F, 0xD4, 0x33, 0x69, 0x12, 0x7A, 0xA5, 0x5E, 0x5F, 0x5E, 0x20, 0x20, 0x20, 0x1E, 0x1F, 0x1E}, // 443 ( 256 ~ 281 )
};

static struct dsi_panel_cmd_set *ss_hbm_irc(struct samsung_display_driver_data *vdd, int *level_key)
{
	struct dsi_panel_cmd_set *hbm_irc_cmds = ss_get_cmds(vdd, TX_HBM_IRC);
	int para_idx = 0;

	if (IS_ERR_OR_NULL(vdd) || IS_ERR_OR_NULL(hbm_irc_cmds)) {
		LCD_ERR("Invalid data vdd : 0x%zx cmds : 0x%zx", (size_t)vdd, (size_t)hbm_irc_cmds);
		return NULL;
	}

	if (IS_ERR_OR_NULL(hbm_irc_cmds->cmds)) {
		LCD_ERR("No irc_tx_cmds\n");
		return NULL;
	}

	if (!vdd->samsung_support_irc)
		return NULL;

	*level_key = LEVEL1_KEY;

	/*auto_brightness is 13 to use 443cd of hbm step on color weakness mode*/
	if (vdd->auto_brightness == HBM_MODE + 7)
		para_idx = 7;
	else
		para_idx = vdd->auto_brightness_level - vdd->auto_brightness;

	/* copy irc default setting */
	memcpy(&hbm_irc_cmds->cmds[0].msg.tx_buf[1], irc_hbm_para_revD[para_idx], 20);

	return hbm_irc_cmds;
}

static struct dsi_panel_cmd_set *ss_gamma(struct samsung_display_driver_data *vdd, int *level_key)
{
	struct dsi_panel_cmd_set  *gamma_cmds = ss_get_cmds(vdd, TX_GAMMA);

	if (IS_ERR_OR_NULL(vdd) || IS_ERR_OR_NULL(gamma_cmds)) {
		LCD_ERR("Invalid data vdd : 0x%zx cmds : 0x%zx", (size_t)vdd, (size_t)gamma_cmds);
		return NULL;
	}

	LCD_DEBUG("bl_level : %d candela : %dCD\n", vdd->bl_level, vdd->candela_level);

	if (IS_ERR_OR_NULL(vdd->smart_dimming_dsi->generate_gamma)) {
		LCD_ERR("generate_gamma is NULL error");
		return NULL;
	} else {
		vdd->smart_dimming_dsi->generate_gamma(
			vdd->smart_dimming_dsi,
			vdd->candela_level,
			&gamma_cmds->cmds[0].msg.tx_buf[1]);

		*level_key = LEVEL1_KEY;

		return gamma_cmds;
	}
}

static int samsung_panel_off_pre(struct samsung_display_driver_data *vdd)
{
	int rc = 0;
	return rc;
}

static int samsung_panel_off_post(struct samsung_display_driver_data *vdd)
{
	int rc = 0;
	return rc;
}

// ========================
//			HMT
// ========================
static struct dsi_panel_cmd_set *ss_gamma_hmt(struct samsung_display_driver_data *vdd, int *level_key)
{
	struct dsi_panel_cmd_set  *hmt_gamma_cmds = ss_get_cmds(vdd, TX_HMT_GAMMA);

	if (IS_ERR_OR_NULL(hmt_gamma_cmds)) {
		LCD_ERR("Invalid data vdd : 0x%zx cmds : 0x%zx", (size_t)vdd, (size_t)hmt_gamma_cmds);
		return NULL;
	}

	LCD_DEBUG("hmt_bl_level : %d candela : %dCD\n", vdd->hmt_stat.hmt_bl_level, vdd->hmt_stat.candela_level_hmt);

	if (IS_ERR_OR_NULL(vdd->smart_dimming_dsi_hmt->generate_gamma)) {
		LCD_ERR("generate_gamma is NULL");
		return NULL;
	} else {
		vdd->smart_dimming_dsi_hmt->generate_gamma(
			vdd->smart_dimming_dsi_hmt,
			vdd->hmt_stat.candela_level_hmt,
			&hmt_gamma_cmds->cmds[0].msg.tx_buf[1]);

		*level_key = LEVEL1_KEY;

		return hmt_gamma_cmds;
	}
}

static struct dsi_panel_cmd_set hmt_aid_cmd;
static struct dsi_panel_cmd_set *ss_aid_hmt(
		struct samsung_display_driver_data *vdd, int *level_key)
{
	struct dsi_panel_cmd_set  *hmt_aid_cmds = ss_get_cmds(vdd, TX_HMT_AID);
	int cmd_idx = 0;

	if (!vdd->dtsi_data.hmt_reverse_aid_map_table[vdd->panel_revision].size ||
		vdd->hmt_stat.cmd_idx_hmt > vdd->dtsi_data.hmt_reverse_aid_map_table[vdd->panel_revision].size)
		goto end;

	cmd_idx = vdd->dtsi_data.hmt_reverse_aid_map_table[vdd->panel_revision].cmd_idx[vdd->hmt_stat.cmd_idx_hmt];

	hmt_aid_cmd.cmds = &hmt_aid_cmds->cmds[cmd_idx];
	hmt_aid_cmd.count = 1;

	*level_key = LEVEL1_KEY;

	return &hmt_aid_cmd;

end:
	LCD_ERR("error");
	return NULL;
}

static struct dsi_panel_cmd_set *ss_elvss_hmt(struct samsung_display_driver_data *vdd, int *level_key)
{
	struct dsi_panel_cmd_set *pcmds;

	pcmds = ss_get_cmds(vdd, TX_HMT_ELVSS);

	/* 0xB5 1th TSET */
	pcmds->cmds[0].msg.tx_buf[1] =
		vdd->temperature > 0 ? vdd->temperature : 0x80|(-1*vdd->temperature);

	/* ELVSS(MPS_CON) setting condition is equal to normal birghtness */ // B5 2nd para : MPS_CON
	if (vdd->hmt_stat.candela_level_hmt > 40)
		pcmds->cmds[0].msg.tx_buf[2] = 0xDC;
	else
		pcmds->cmds[0].msg.tx_buf[2] = 0xCC;

	*level_key = LEVEL1_KEY;

	return pcmds;
}

static void ss_make_sdimconf_hmt(struct samsung_display_driver_data *vdd)
{
	/* Set the mtp read buffer pointer and read the NVM value*/
	ss_panel_data_read(vdd, RX_SMART_DIM_MTP,
			vdd->smart_dimming_dsi_hmt->mtp_buffer, LEVEL1_KEY);

	/* Initialize smart dimming related things here */
	/* lux_tab setting for 350cd */
	vdd->smart_dimming_dsi_hmt->lux_tab = vdd->dtsi_data.hmt_candela_map_table[vdd->panel_revision].cd;
	vdd->smart_dimming_dsi_hmt->lux_tabsize = vdd->dtsi_data.hmt_candela_map_table[vdd->panel_revision].tab_size;
	vdd->smart_dimming_dsi_hmt->man_id = vdd->manufacture_id_dsi;
	if (vdd->panel_func.samsung_panel_revision)
			vdd->smart_dimming_dsi_hmt->panel_revision = vdd->panel_func.samsung_panel_revision(vdd);

	/* Just a safety check to ensure smart dimming data is initialised well */
	vdd->smart_dimming_dsi_hmt->init(vdd->smart_dimming_dsi_hmt);

	LCD_INFO("[HMT] smart dimming done!\n");
}

static int ss_samart_dimming_init_hmt(struct samsung_display_driver_data *vdd)
{
	LCD_INFO("DSI%d : ++\n", vdd->ndx);

	if (IS_ERR_OR_NULL(vdd)) {
		LCD_ERR("Invalid data vdd : 0x%zx", (size_t)vdd);
		return false;
	}

	vdd->smart_dimming_dsi_hmt = vdd->panel_func.samsung_smart_get_conf_hmt();

	if (IS_ERR_OR_NULL(vdd->smart_dimming_dsi_hmt)) {
		LCD_ERR("DSI%d error", vdd->ndx);
		return false;
	} else {
		vdd->hmt_stat.hmt_on = 0;
		vdd->hmt_stat.hmt_bl_level = 0;
		vdd->hmt_stat.hmt_reverse = 0;
		vdd->hmt_stat.hmt_is_first = 1;

		ss_make_sdimconf_hmt(vdd);

		vdd->smart_dimming_hmt_loaded_dsi = true;
	}

	LCD_INFO("DSI%d : --\n", vdd->ndx);

	return true;
}

static void ss_set_panel_lpm_brightness(struct samsung_display_driver_data *vdd)
{
	struct dsi_panel_cmd_set *alpm_brightness[LPM_BRIGHTNESS_MAX_IDX] = {NULL, };
	struct dsi_panel_cmd_set *alpm_ctrl[MAX_LPM_CTRL] = {NULL, };
	struct dsi_panel_cmd_set *cmd_list[2];

	/*default*/
	int mode = ALPM_MODE_ON;
	int ctrl_index = ALPM_CTRL_2NIT;
	int bl_index = LPM_2NIT_IDX;

	/*
	 * Init reg_list and cmd list
	 * reg_list[X][0] is reg value
	 * reg_list[X][1] is offset for reg value
	 * cmd_list is the target cmds for searching reg value
	 */
	static int reg_list[2][2] = {
		{ALPM_REG, -EINVAL},
		{ALPM_CTRL_REG, -EINVAL} };

	LCD_DEBUG("%s++\n", __func__);

	cmd_list[0] = ss_get_cmds(vdd, TX_LPM_BL_CMD);
	cmd_list[1] = ss_get_cmds(vdd, TX_LPM_BL_CMD);

	/* Init alpm_brightness and alpm_ctrl cmds */
	alpm_brightness[LPM_2NIT_IDX] = ss_get_cmds(vdd, TX_LPM_2NIT_CMD);
	alpm_brightness[LPM_10NIT_IDX] = NULL;	//ss_get_cmds(vdd, TX_LPM_10NIT_CMD);
	alpm_brightness[LPM_30NIT_IDX] = NULL;	//ss_get_cmds(vdd, TX_LPM_30NIT_CMD);
	alpm_brightness[LPM_60NIT_IDX] = ss_get_cmds(vdd, TX_LPM_60NIT_CMD);

	alpm_ctrl[ALPM_CTRL_2NIT] = ss_get_cmds(vdd, TX_ALPM_2NIT_CMD);
	alpm_ctrl[ALPM_CTRL_60NIT] = ss_get_cmds(vdd, TX_ALPM_60NIT_CMD);
	alpm_ctrl[HLPM_CTRL_2NIT] = ss_get_cmds(vdd, TX_HLPM_2NIT_CMD);
	alpm_ctrl[HLPM_CTRL_60NIT] = ss_get_cmds(vdd, TX_HLPM_60NIT_CMD);

	mode = vdd->panel_lpm.mode;

	switch (vdd->panel_lpm.lpm_bl_level) {
	case LPM_60NIT:
		ctrl_index = (mode == ALPM_MODE_ON) ? ALPM_CTRL_60NIT :
			(mode == HLPM_MODE_ON) ? HLPM_CTRL_60NIT : ALPM_CTRL_60NIT;
		bl_index = LPM_60NIT_IDX;
		break;
	case LPM_2NIT:
	default:
		ctrl_index = (mode == ALPM_MODE_ON) ? ALPM_CTRL_2NIT :
			(mode == HLPM_MODE_ON) ? HLPM_CTRL_2NIT : ALPM_CTRL_2NIT;
		bl_index = LPM_2NIT_IDX;
		break;
	}

	LCD_DEBUG("[Panel LPM]bl_index %d, ctrl_index %d, mode %d\n",
			 bl_index, ctrl_index, mode);

	/*
	 * Find offset for alpm_reg and alpm_ctrl_reg
	 * alpm_reg  : Control register for ALPM/HLPM on/off
	 * alpm_ctrl_reg : Control register for changing ALPM/HLPM mode
	 */
	if (unlikely((reg_list[0][1] == -EINVAL) ||
				(reg_list[1][1] == -EINVAL)))
		ss_find_reg_offset(reg_list, cmd_list, ARRAY_SIZE(cmd_list));

	if (reg_list[0][1] != -EINVAL) {
		/* Update parameter for ALPM_REG */
		memcpy(cmd_list[0]->cmds[reg_list[0][1]].msg.tx_buf,
				alpm_brightness[bl_index]->cmds[0].msg.tx_buf,
				sizeof(char) * cmd_list[0]->cmds[reg_list[0][1]].msg.tx_len);

		LCD_DEBUG("[Panel LPM] change brightness cmd : %x, %x\n",
				cmd_list[0]->cmds[reg_list[0][1]].msg.tx_buf[1],
				alpm_brightness[bl_index]->cmds[0].msg.tx_buf[1]);
	}

	if (reg_list[1][1] != -EINVAL) {
		/* Initialize ALPM/HLPM cmds */
		/* Update parameter for ALPM_CTRL_REG */
		memcpy(cmd_list[1]->cmds[reg_list[1][1]].msg.tx_buf,
				alpm_ctrl[ctrl_index]->cmds[0].msg.tx_buf,
				sizeof(char) * cmd_list[1]->cmds[reg_list[1][1]].msg.tx_len);

		LCD_DEBUG("[Panel LPM] update alpm ctrl reg\n");
	}

	//send lpm bl cmd
	ss_send_cmd(vdd, TX_LPM_BL_CMD);

	LCD_INFO("[Panel LPM] bl_level : %s\n",
				/* Check current brightness level */
				vdd->panel_lpm.lpm_bl_level == LPM_2NIT ? "2NIT" :
				vdd->panel_lpm.lpm_bl_level == LPM_60NIT ? "60NIT" : "UNKNOWN");

	LCD_DEBUG("%s--\n", __func__);
	return;
}

/*
 * This function will update parameters for ALPM_REG/ALPM_CTRL_REG
 * Parameter for ALPM_REG : Control brightness for panel LPM
 * Parameter for ALPM_CTRL_REG : Change panel LPM mode for ALPM/HLPM
 * mode, brightness, hz are updated here.
 */
static void ss_update_panel_lpm_ctrl_cmd(struct samsung_display_driver_data *vdd)
{
	struct dsi_panel_cmd_set *alpm_brightness[LPM_BRIGHTNESS_MAX_IDX] = {NULL, };
	struct dsi_panel_cmd_set *alpm_ctrl[MAX_LPM_CTRL] = {NULL, };
	struct dsi_panel_cmd_set *alpm_off_ctrl[MAX_LPM_MODE] = {NULL, };
	struct dsi_panel_cmd_set *cmd_list[2];
	struct dsi_panel_cmd_set *off_cmd_list[1];

	/*default*/
	int mode = ALPM_MODE_ON;
	int ctrl_index = ALPM_CTRL_2NIT;
	int bl_index = LPM_2NIT_IDX;

	/*
	 * Init reg_list and cmd list
	 * reg_list[X][0] is reg value
	 * reg_list[X][1] is offset for reg value
	 * cmd_list is the target cmds for searching reg value
	 */
	static int reg_list[2][2] = {
		{ALPM_REG, -EINVAL},
		{ALPM_CTRL_REG, -EINVAL} };

	static int off_reg_list[1][2] = {
		{ALPM_CTRL_REG, -EINVAL} };

	cmd_list[0] = ss_get_cmds(vdd, TX_LPM_ON);
	cmd_list[1] = ss_get_cmds(vdd, TX_LPM_ON);
	off_cmd_list[0] = ss_get_cmds(vdd, TX_LPM_OFF);

	/* Init alpm_brightness and alpm_ctrl cmds */
	alpm_brightness[LPM_2NIT_IDX] = ss_get_cmds(vdd, TX_LPM_2NIT_CMD);
	alpm_brightness[LPM_10NIT_IDX] = NULL;	//ss_get_cmds(vdd, TX_LPM_10NIT_CMD);
	alpm_brightness[LPM_30NIT_IDX] = NULL;	//ss_get_cmds(vdd, TX_LPM_30NIT_CMD);
	alpm_brightness[LPM_60NIT_IDX] = ss_get_cmds(vdd, TX_LPM_60NIT_CMD);

	alpm_ctrl[ALPM_CTRL_2NIT] = ss_get_cmds(vdd, TX_ALPM_2NIT_CMD);
	alpm_ctrl[ALPM_CTRL_60NIT] = ss_get_cmds(vdd, TX_ALPM_60NIT_CMD);
	alpm_ctrl[HLPM_CTRL_2NIT] = ss_get_cmds(vdd, TX_HLPM_2NIT_CMD);
	alpm_ctrl[HLPM_CTRL_60NIT] = ss_get_cmds(vdd, TX_HLPM_60NIT_CMD);

	alpm_off_ctrl[ALPM_MODE_ON] = ss_get_cmds(vdd, TX_ALPM_OFF);
	alpm_off_ctrl[HLPM_MODE_ON] = ss_get_cmds(vdd, TX_HLPM_OFF);

	mode = vdd->panel_lpm.mode;

	switch (vdd->panel_lpm.lpm_bl_level) {
	case LPM_60NIT:
		ctrl_index = (mode == ALPM_MODE_ON) ? ALPM_CTRL_60NIT :
			(mode == HLPM_MODE_ON) ? HLPM_CTRL_60NIT : ALPM_CTRL_60NIT;
		bl_index = LPM_60NIT_IDX;
		break;
	case LPM_2NIT:
	default:
		ctrl_index = (mode == ALPM_MODE_ON) ? ALPM_CTRL_2NIT :
			(mode == HLPM_MODE_ON) ? HLPM_CTRL_2NIT : ALPM_CTRL_2NIT;
		bl_index = LPM_2NIT_IDX;
		break;
	}

	/*
	 * Find offset for alpm_reg and alpm_ctrl_reg
	 * alpm_reg	 : Control register for ALPM/HLPM on/off
	 * alpm_ctrl_reg : Control register for changing ALPM/HLPM mode
	 */

	if (unlikely((reg_list[0][1] == -EINVAL) ||
				(reg_list[1][1] == -EINVAL)))
		ss_find_reg_offset(reg_list, cmd_list, ARRAY_SIZE(cmd_list));

	if (unlikely(off_reg_list[0][1] == -EINVAL))
		ss_find_reg_offset(off_reg_list, off_cmd_list,
						ARRAY_SIZE(off_cmd_list));

	if (reg_list[0][1] != -EINVAL) {
		/* Update parameter for ALPM_REG */
		memcpy(cmd_list[0]->cmds[reg_list[0][1]].msg.tx_buf,
				alpm_brightness[bl_index]->cmds[0].msg.tx_buf,
				sizeof(char) * cmd_list[0]->cmds[reg_list[0][1]].msg.tx_len);

		LCD_ERR("[Panel LPM] change brightness cmd : %x, %x\n",
				cmd_list[0]->cmds[reg_list[0][1]].msg.tx_buf[1],
				alpm_brightness[bl_index]->cmds[0].msg.tx_buf[1]);
	}

	if (reg_list[1][1] != -EINVAL) {
		/* Initialize ALPM/HLPM cmds */
		/* Update parameter for ALPM_CTRL_REG */
		memcpy(cmd_list[1]->cmds[reg_list[1][1]].msg.tx_buf,
				alpm_ctrl[ctrl_index]->cmds[0].msg.tx_buf,
				sizeof(char) * cmd_list[0]->cmds[reg_list[1][1]].msg.tx_len);

		LCD_ERR("[Panel LPM] update alpm ctrl reg(%d)\n", mode);
	}

	if ((off_reg_list[0][1] != -EINVAL) &&\
			(mode != LPM_MODE_OFF)) {
		/* Update parameter for ALPM_CTRL_REG */
		memcpy(off_cmd_list[0]->cmds[off_reg_list[0][1]].msg.tx_buf,
				alpm_off_ctrl[mode]->cmds[0].msg.tx_buf,
				sizeof(char) * off_cmd_list[0]->cmds[off_reg_list[0][1]].msg.tx_len);
	}

	return;
}

static int ddi_hw_cursor(struct samsung_display_driver_data *vdd, int *input)
{
	struct dsi_panel_cmd_set *pcmds;
	u8 *payload;

	if (IS_ERR_OR_NULL(vdd)) {
		LCD_ERR("Invalid data vdd : 0x%zx", (size_t)vdd);
		return 0;
	}

	if (IS_ERR_OR_NULL(input)) {
		LCD_ERR("input is NULL\n");
		return 0;
	}

	pcmds = ss_get_cmds(vdd, TX_HW_CURSOR);
	payload = pcmds->cmds[0].msg.tx_buf;
	if (IS_ERR_OR_NULL(payload)) {
		LCD_ERR("hw_cursor_tx_cmds is NULL\n");
		return 0;
	}


	LCD_INFO("On/Off:(%d), Por/Land:(%d), On_Select:(%d), X:(%d), Y:(%d), Width:(%d), Length:(%d), Color:(0x%x), Period:(%x), TR_TIME(%x)\n",
		input[0], input[1], input[2], input[3], input[4], input[5],
		input[6], input[7], input[8], input[9]);

	/* Cursor On&Off (0:Off, 1:On) */
	payload[1] = input[0] & 0x1;

	/* 3rd bit : CURSOR_ON_SEL, 2nd bit : Port/Land, 1st bit : BLINK_ON(Default On)*/
	payload[2] = (input[2] & 0x1) << 2 | (input[1] & 0x1) << 1 | 0x1;

	/* Start X address */
	payload[3] = (input[3] & 0x700) >> 8;
	payload[4] = input[3] & 0xFF;

	/* Start Y address */
	payload[5] = (input[4] & 0x700) >> 8;
	payload[6] = input[4] & 0xFF;

	/* Width */
	payload[7] = input[5] & 0xF;

	/* Length */
	payload[8] = (input[6] & 0x100) >> 8;
	payload[9] = input[6] & 0xFF;

	/* Color */
	payload[10] = (input[7] & 0xFF0000) >> 16;
	payload[11] = (input[7] & 0xFF00) >> 8;
	payload[12] = input[7] & 0xFF;

	/* Period */
	payload[13] = input[8] & 0xFF;

	/* TR_TIME */
	payload[14] = input[9] & 0xFF;

	ss_send_cmd(vdd, TX_LEVEL1_KEY_ENABLE);
	ss_send_cmd(vdd, TX_HW_CURSOR);
	ss_send_cmd(vdd, TX_LEVEL1_KEY_DISABLE);

	return 1;
}

static void ss_send_colorweakness_ccb_cmd(struct samsung_display_driver_data *vdd, int mode)
{
	struct dsi_panel_cmd_set *pcmds;

	LCD_INFO("mode (%d) color_weakness_value (%x) \n", mode, vdd->color_weakness_value);

	if (mode) {
		pcmds = ss_get_cmds(vdd, TX_COLOR_WEAKNESS_ENABLE);
		pcmds->cmds[1].msg.tx_buf[1] = vdd->color_weakness_value;
		ss_send_cmd(vdd, TX_COLOR_WEAKNESS_ENABLE);
	} else {
		ss_send_cmd(vdd, TX_COLOR_WEAKNESS_DISABLE);
	}
}

static int dsi_update_mdnie_data(struct samsung_display_driver_data *vdd)
{
	struct mdnie_lite_tune_data *mdnie_data;

	mdnie_data = kzalloc(sizeof(struct mdnie_lite_tune_data), GFP_KERNEL);
	if (!mdnie_data) {
		LCD_ERR("fail to allocate mdnie_data memory\n");
		return -ENOMEM;
	}

	/* Update mdnie command */
	mdnie_data->DSI_COLOR_BLIND_MDNIE_1 = DSI_COLOR_BLIND_MDNIE_1;
	mdnie_data->DSI_RGB_SENSOR_MDNIE_1 = DSI_RGB_SENSOR_MDNIE_1;
	mdnie_data->DSI_RGB_SENSOR_MDNIE_2 = DSI_RGB_SENSOR_MDNIE_2;
	mdnie_data->DSI_RGB_SENSOR_MDNIE_3 = DSI_RGB_SENSOR_MDNIE_3;
	mdnie_data->DSI_TRANS_DIMMING_MDNIE = DSI_RGB_SENSOR_MDNIE_3;
	mdnie_data->DSI_UI_DYNAMIC_MDNIE_2 = DSI_UI_DYNAMIC_MDNIE_2;
	mdnie_data->DSI_UI_STANDARD_MDNIE_2 = DSI_UI_STANDARD_MDNIE_2;
	mdnie_data->DSI_UI_AUTO_MDNIE_2 = DSI_UI_AUTO_MDNIE_2;
	mdnie_data->DSI_VIDEO_DYNAMIC_MDNIE_2 = DSI_VIDEO_DYNAMIC_MDNIE_2;
	mdnie_data->DSI_VIDEO_STANDARD_MDNIE_2 = DSI_VIDEO_STANDARD_MDNIE_2;
	mdnie_data->DSI_VIDEO_AUTO_MDNIE_2 = DSI_VIDEO_AUTO_MDNIE_2;
	mdnie_data->DSI_CAMERA_AUTO_MDNIE_2 = DSI_CAMERA_AUTO_MDNIE_2;
	mdnie_data->DSI_GALLERY_DYNAMIC_MDNIE_2 = DSI_GALLERY_DYNAMIC_MDNIE_2;
	mdnie_data->DSI_GALLERY_STANDARD_MDNIE_2 = DSI_GALLERY_STANDARD_MDNIE_2;
	mdnie_data->DSI_GALLERY_AUTO_MDNIE_2 = DSI_GALLERY_AUTO_MDNIE_2;
	mdnie_data->DSI_BROWSER_DYNAMIC_MDNIE_2 = DSI_BROWSER_DYNAMIC_MDNIE_2;
	mdnie_data->DSI_BROWSER_STANDARD_MDNIE_2 = DSI_BROWSER_STANDARD_MDNIE_2;
	mdnie_data->DSI_BROWSER_AUTO_MDNIE_2 = DSI_BROWSER_AUTO_MDNIE_2;
	mdnie_data->DSI_EBOOK_DYNAMIC_MDNIE_2 = DSI_EBOOK_DYNAMIC_MDNIE_2;
	mdnie_data->DSI_EBOOK_STANDARD_MDNIE_2 = DSI_EBOOK_STANDARD_MDNIE_2;
	mdnie_data->DSI_EBOOK_AUTO_MDNIE_2 = DSI_EBOOK_AUTO_MDNIE_2;
	mdnie_data->DSI_TDMB_DYNAMIC_MDNIE_2 = DSI_TDMB_DYNAMIC_MDNIE_2;
	mdnie_data->DSI_TDMB_STANDARD_MDNIE_2 = DSI_TDMB_STANDARD_MDNIE_2;
	mdnie_data->DSI_TDMB_AUTO_MDNIE_2 = DSI_TDMB_AUTO_MDNIE_2;

	mdnie_data->DSI_BYPASS_MDNIE = DSI_BYPASS_MDNIE;
	mdnie_data->DSI_NEGATIVE_MDNIE = DSI_NEGATIVE_MDNIE;
	mdnie_data->DSI_COLOR_BLIND_MDNIE = DSI_COLOR_BLIND_MDNIE;
	mdnie_data->DSI_HBM_CE_MDNIE = DSI_HBM_CE_MDNIE;
	mdnie_data->DSI_HBM_CE_D65_MDNIE = DSI_HBM_CE_D65_MDNIE;
	mdnie_data->DSI_RGB_SENSOR_MDNIE = DSI_RGB_SENSOR_MDNIE;
	mdnie_data->DSI_UI_DYNAMIC_MDNIE = DSI_UI_DYNAMIC_MDNIE;
	mdnie_data->DSI_UI_STANDARD_MDNIE = DSI_UI_STANDARD_MDNIE;
	mdnie_data->DSI_UI_NATURAL_MDNIE = DSI_UI_NATURAL_MDNIE;
	mdnie_data->DSI_UI_AUTO_MDNIE = DSI_UI_AUTO_MDNIE;
	mdnie_data->DSI_VIDEO_DYNAMIC_MDNIE = DSI_VIDEO_DYNAMIC_MDNIE;
	mdnie_data->DSI_VIDEO_STANDARD_MDNIE = DSI_VIDEO_STANDARD_MDNIE;
	mdnie_data->DSI_VIDEO_NATURAL_MDNIE = DSI_VIDEO_NATURAL_MDNIE;
	mdnie_data->DSI_VIDEO_AUTO_MDNIE = DSI_VIDEO_AUTO_MDNIE;
	mdnie_data->DSI_CAMERA_AUTO_MDNIE = DSI_CAMERA_AUTO_MDNIE;
	mdnie_data->DSI_GALLERY_DYNAMIC_MDNIE = DSI_GALLERY_DYNAMIC_MDNIE;
	mdnie_data->DSI_GALLERY_STANDARD_MDNIE = DSI_GALLERY_STANDARD_MDNIE;
	mdnie_data->DSI_GALLERY_NATURAL_MDNIE = DSI_GALLERY_NATURAL_MDNIE;
	mdnie_data->DSI_GALLERY_AUTO_MDNIE = DSI_GALLERY_AUTO_MDNIE;
	mdnie_data->DSI_BROWSER_DYNAMIC_MDNIE = DSI_BROWSER_DYNAMIC_MDNIE;
	mdnie_data->DSI_BROWSER_STANDARD_MDNIE = DSI_BROWSER_STANDARD_MDNIE;
	mdnie_data->DSI_BROWSER_NATURAL_MDNIE = DSI_BROWSER_NATURAL_MDNIE;
	mdnie_data->DSI_BROWSER_AUTO_MDNIE = DSI_BROWSER_AUTO_MDNIE;
	mdnie_data->DSI_EBOOK_DYNAMIC_MDNIE = DSI_EBOOK_DYNAMIC_MDNIE;
	mdnie_data->DSI_EBOOK_STANDARD_MDNIE = DSI_EBOOK_STANDARD_MDNIE;
	mdnie_data->DSI_EBOOK_NATURAL_MDNIE = DSI_EBOOK_NATURAL_MDNIE;
	mdnie_data->DSI_EBOOK_AUTO_MDNIE = DSI_EBOOK_AUTO_MDNIE;
	mdnie_data->DSI_EMAIL_AUTO_MDNIE = DSI_EMAIL_AUTO_MDNIE;
	mdnie_data->DSI_GAME_LOW_MDNIE = DSI_GAME_LOW_MDNIE;
	mdnie_data->DSI_GAME_MID_MDNIE = DSI_GAME_MID_MDNIE;
	mdnie_data->DSI_GAME_HIGH_MDNIE = DSI_GAME_HIGH_MDNIE;
	mdnie_data->DSI_TDMB_DYNAMIC_MDNIE = DSI_TDMB_DYNAMIC_MDNIE;
	mdnie_data->DSI_TDMB_STANDARD_MDNIE = DSI_TDMB_STANDARD_MDNIE;
	mdnie_data->DSI_TDMB_NATURAL_MDNIE = DSI_TDMB_NATURAL_MDNIE;
	mdnie_data->DSI_TDMB_AUTO_MDNIE = DSI_TDMB_AUTO_MDNIE;
	mdnie_data->DSI_GRAYSCALE_MDNIE = DSI_GRAYSCALE_MDNIE;
	mdnie_data->DSI_GRAYSCALE_NEGATIVE_MDNIE = DSI_GRAYSCALE_NEGATIVE_MDNIE;
	mdnie_data->DSI_CURTAIN = DSI_SCREEN_CURTAIN_MDNIE;
	mdnie_data->DSI_NIGHT_MODE_MDNIE = DSI_NIGHT_MODE_MDNIE;
	mdnie_data->DSI_NIGHT_MODE_MDNIE_1 = DSI_NIGHT_MODE_MDNIE_1;
	mdnie_data->DSI_COLOR_BLIND_MDNIE_SCR = DSI_COLOR_BLIND_MDNIE_1;
	mdnie_data->DSI_RGB_SENSOR_MDNIE_SCR = DSI_RGB_SENSOR_MDNIE_1;
	mdnie_data->DSI_AFC = NULL;
	mdnie_data->DSI_AFC_ON = NULL;
	mdnie_data->DSI_AFC_OFF = NULL;

	mdnie_data->mdnie_tune_value_dsi = mdnie_tune_value_dsi;
	mdnie_data->hmt_color_temperature_tune_value_dsi = hmt_color_temperature_tune_value_dsi;

	mdnie_data->hdr_tune_value_dsi = hdr_tune_value_dsi;

	mdnie_data->light_notification_tune_value_dsi = light_notification_tune_value_dsi;

	/* Update MDNIE data related with size, offset or index */
	mdnie_data->dsi_bypass_mdnie_size = ARRAY_SIZE(DSI_BYPASS_MDNIE);
	mdnie_data->mdnie_color_blinde_cmd_offset = MDNIE_COLOR_BLINDE_CMD_OFFSET;
	mdnie_data->mdnie_step_index[MDNIE_STEP1] = MDNIE_STEP1_INDEX;
	mdnie_data->mdnie_step_index[MDNIE_STEP2] = MDNIE_STEP2_INDEX;
	mdnie_data->mdnie_step_index[MDNIE_STEP3] = MDNIE_STEP3_INDEX;
	mdnie_data->address_scr_white[ADDRESS_SCR_WHITE_RED_OFFSET] = ADDRESS_SCR_WHITE_RED;
	mdnie_data->address_scr_white[ADDRESS_SCR_WHITE_GREEN_OFFSET] = ADDRESS_SCR_WHITE_GREEN;
	mdnie_data->address_scr_white[ADDRESS_SCR_WHITE_BLUE_OFFSET] = ADDRESS_SCR_WHITE_BLUE;
	mdnie_data->dsi_rgb_sensor_mdnie_1_size = DSI_RGB_SENSOR_MDNIE_1_SIZE;
	mdnie_data->dsi_rgb_sensor_mdnie_2_size = DSI_RGB_SENSOR_MDNIE_2_SIZE;
	mdnie_data->dsi_rgb_sensor_mdnie_3_size = DSI_RGB_SENSOR_MDNIE_3_SIZE;

	mdnie_data->dsi_trans_dimming_data_index = MDNIE_TRANS_DIMMING_DATA_INDEX;

	mdnie_data->dsi_adjust_ldu_table = adjust_ldu_data;
	mdnie_data->dsi_max_adjust_ldu = 6;
	mdnie_data->dsi_night_mode_table = night_mode_data;
	mdnie_data->dsi_max_night_mode_index = 11;
	mdnie_data->dsi_white_default_r = 0xff;
	mdnie_data->dsi_white_default_g = 0xff;
	mdnie_data->dsi_white_default_b = 0xff;
	mdnie_data->dsi_white_balanced_r = 0;
	mdnie_data->dsi_white_balanced_g = 0;
	mdnie_data->dsi_white_balanced_b = 0;
	mdnie_data->dsi_scr_step_index = MDNIE_STEP1_INDEX;
	mdnie_data->dsi_afc_size = 0;
	mdnie_data->dsi_afc_index = 0;

	vdd->mdnie_data = mdnie_data;

	return 0;
}

static void samsung_panel_init(struct samsung_display_driver_data *vdd)
{
	LCD_ERR("%s\n", ss_get_panel_name(vdd));

	/* Default Panel Power Status is OFF */
	vdd->panel_state = PANEL_PWR_OFF;

	/* ON/OFF */
	vdd->panel_func.samsung_panel_on_pre = samsung_panel_on_pre;
	vdd->panel_func.samsung_panel_on_post = samsung_panel_on_post;
	vdd->panel_func.samsung_panel_off_pre = samsung_panel_off_pre;
	vdd->panel_func.samsung_panel_off_post = samsung_panel_off_post;

	/* DDI RX */
	vdd->panel_func.samsung_panel_revision = ss_panel_revision;
	vdd->panel_func.samsung_manufacture_date_read = ss_manufacture_date_read;
	vdd->panel_func.samsung_ddi_id_read = ss_ddi_id_read;

	vdd->panel_func.samsung_elvss_read = ss_elvss_read;
	vdd->panel_func.samsung_hbm_read = ss_hbm_read;
	vdd->panel_func.samsung_mdnie_read = ss_mdnie_read;
	vdd->panel_func.samsung_smart_dimming_init = ss_samart_dimming_init;
	vdd->panel_func.samsung_smart_get_conf = smart_get_conf_S6E3HA6_AMB577MQ01;

	vdd->panel_func.samsung_cell_id_read = ss_cell_id_read;
	vdd->panel_func.samsung_octa_id_read = ss_octa_id_read;

	/* Brightness */
	vdd->panel_func.samsung_brightness_hbm_off = NULL;
	vdd->panel_func.samsung_brightness_aid = ss_aid;
	vdd->panel_func.samsung_brightness_acl_on = ss_acl_on;
	vdd->panel_func.samsung_brightness_acl_percent = NULL;
	vdd->panel_func.samsung_brightness_acl_off = ss_acl_off;
	vdd->panel_func.samsung_brightness_elvss = ss_elvss;
	vdd->panel_func.samsung_brightness_elvss_temperature1 = ss_elvss_temperature1;
	vdd->panel_func.samsung_brightness_elvss_temperature2 = NULL;
	vdd->panel_func.samsung_brightness_vint = ss_vint;
	vdd->panel_func.samsung_brightness_irc = ss_irc;
	vdd->panel_func.samsung_brightness_gamma = ss_gamma;

	/* HBM */
	vdd->panel_func.samsung_hbm_gamma = ss_hbm_gamma;
	vdd->panel_func.samsung_hbm_etc = ss_hbm_etc;
	vdd->panel_func.samsung_hbm_irc = ss_hbm_irc;
	vdd->panel_func.get_hbm_candela_value = get_hbm_candela_value;

	/* Event */
	vdd->panel_func.samsung_change_ldi_fps = NULL;

	/* HMT */
	vdd->panel_func.samsung_brightness_gamma_hmt = ss_gamma_hmt;
	vdd->panel_func.samsung_brightness_aid_hmt = ss_aid_hmt;
	vdd->panel_func.samsung_brightness_elvss_hmt = ss_elvss_hmt;
	vdd->panel_func.samsung_brightness_vint_hmt = NULL;
	vdd->panel_func.samsung_smart_dimming_hmt_init = ss_samart_dimming_init_hmt;
	vdd->panel_func.samsung_smart_get_conf_hmt = smart_get_conf_S6E3HA6_AMB577MQ01_hmt;

	/* Panel LPM */
	vdd->panel_func.samsung_update_lpm_ctrl_cmd = ss_update_panel_lpm_ctrl_cmd;
	vdd->panel_func.samsung_set_lpm_brightness = ss_set_panel_lpm_brightness;

	/* default brightness */
	vdd->bl_level = 25500;

	/* mdnie */
	vdd->support_mdnie_lite = true;
	vdd->support_mdnie_trans_dimming = true;
	vdd->mdnie_tune_size1 = sizeof(DSI_BYPASS_MDNIE_1);
	vdd->mdnie_tune_size2 = sizeof(DSI_BYPASS_MDNIE_2);
	vdd->mdnie_tune_size3 = sizeof(DSI_BYPASS_MDNIE_3);
	dsi_update_mdnie_data(vdd);

	/* send recovery pck before sending image date (for ESD recovery) */
	vdd->send_esd_recovery = false;

	vdd->auto_brightness_level = 12;

	/* Enable panic on first pingpong timeout */
//	vdd->debug_data->panic_on_pptimeout = true;

	/* Set IRC init value */
	vdd->irc_mode = IRC_MODERATO_MODE;

	/* COLOR WEAKNESS */
	vdd->panel_func.color_weakness_ccb_on_off =  ss_send_colorweakness_ccb_cmd;

	/* Support DDI HW CURSOR */
	vdd->panel_func.ddi_hw_cursor = ddi_hw_cursor;

	/* COVER Open/Close */
	vdd->panel_func.samsung_cover_control = NULL;

	/* ACL default ON */
	vdd->acl_status = 1;

	/* Gram Checksum Test */
	vdd->panel_func.samsung_gct_write = NULL;
	vdd->panel_func.samsung_gct_read = NULL;
}

static int __init samsung_panel_initialize(void)
{
	struct samsung_display_driver_data *vdd;
	enum ss_display_ndx ndx;
	char panel_string[] = "ss_dsi_panel_S6E3HA6_AMB577MQ01_WQHD";
	char panel_name[MAX_CMDLINE_PARAM_LEN];
	char panel_secondary_name[MAX_CMDLINE_PARAM_LEN];

	ss_get_primary_panel_name_cmdline(panel_name);
	ss_get_secondary_panel_name_cmdline(panel_secondary_name);

	/* TODO: use component_bind with panel_func
	 * and match by panel_string, instead.
	 */
	if (!strncmp(panel_string, panel_name, strlen(panel_string)))
		ndx = PRIMARY_DISPLAY_NDX;
	else if (!strncmp(panel_string, panel_secondary_name,
				strlen(panel_string)))
		ndx = SECONDARY_DISPLAY_NDX;
	else
		return 0;

	vdd = &vdd_data[ndx];
	vdd->panel_func.samsung_panel_init = samsung_panel_init;

	return 0;
}

early_initcall(samsung_panel_initialize);
