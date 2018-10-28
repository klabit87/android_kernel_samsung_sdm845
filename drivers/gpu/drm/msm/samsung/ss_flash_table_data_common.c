 /* =================================================================
 *
 *
 *	Description:  samsung display common file
 *
 *	Author: jb09.kim@samsung.com
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

#include "ss_dsi_panel_common.h"
#include "ss_flash_table_data_common.h"

static void init_br_info(struct samsung_display_driver_data *vdd)
{
	int gamma_size = vdd->dtsi_data.gamma_size;
	int aor_size = vdd->dtsi_data.aor_size;
	int vint_size = vdd->dtsi_data.vint_size;
	int elvss_size = vdd->dtsi_data.elvss_size;
	int irc_size = vdd->dtsi_data.irc_size;

	int hbm_size = vdd->dtsi_data.hbm_brightness_step * (aor_size + vint_size + elvss_size + irc_size);
	int normal_size = vdd->dtsi_data.normal_brightness_step * (gamma_size + aor_size + vint_size + elvss_size + irc_size);
	int hmd_size = vdd->dtsi_data.hmd_brightness_step * (gamma_size + aor_size);

	vdd->panel_br_info.br_data_size = hbm_size + normal_size + hmd_size;

	/*
		free alloc memory : test purpose for slab memory integrity
	*/
	if (!IS_ERR_OR_NULL(vdd->panel_br_info.br_data_raw)) {
		kfree(vdd->panel_br_info.br_data_raw);
		vdd->panel_br_info.br_data_raw = NULL;
	}

	vdd->panel_br_info.br_data_raw = kzalloc(vdd->panel_br_info.br_data_size * sizeof(char), GFP_KERNEL);

	LCD_INFO("alloc size : %d\n", vdd->panel_br_info.br_data_size);
}

static void init_br_info_hbm(struct samsung_display_driver_data *vdd)
{
	int column;
	int hbm_brightness_step = vdd->dtsi_data.hbm_brightness_step;
	int gamma_size = vdd->dtsi_data.gamma_size;
	int aor_size = vdd->dtsi_data.aor_size;
	int vint_size = vdd->dtsi_data.vint_size;
	int elvss_size = vdd->dtsi_data.elvss_size;
	int irc_size = vdd->dtsi_data.irc_size;

	/*
		free alloc memory : test purpose for slab memory integrity
	*/
	if (!IS_ERR_OR_NULL(vdd->panel_br_info.hbm.candela_table)) {
		kfree(vdd->panel_br_info.hbm.candela_table);
		vdd->panel_br_info.hbm.candela_table = NULL;
	}

	if (!IS_ERR_OR_NULL(vdd->panel_br_info.hbm.gamma)) {
		for (column = 0; column < hbm_brightness_step; column++)
			kfree(vdd->panel_br_info.hbm.gamma[column]);
		kfree(vdd->panel_br_info.hbm.gamma);
		vdd->panel_br_info.hbm.gamma = NULL;
	}

	if (!IS_ERR_OR_NULL(vdd->panel_br_info.hbm.aor)) {
		for (column = 0; column < hbm_brightness_step; column++)
			kfree(vdd->panel_br_info.hbm.aor[column]);
		kfree(vdd->panel_br_info.hbm.aor);
		vdd->panel_br_info.hbm.aor = NULL;
	}

	if (!IS_ERR_OR_NULL(vdd->panel_br_info.hbm.vint)) {
		for (column = 0; column < hbm_brightness_step; column++)
			kfree(vdd->panel_br_info.hbm.vint[column]);
		kfree(vdd->panel_br_info.hbm.vint);
		vdd->panel_br_info.hbm.vint = NULL;
	}

	if (!IS_ERR_OR_NULL(vdd->panel_br_info.hbm.elvss)) {
		for (column = 0; column < hbm_brightness_step; column++)
			kfree(vdd->panel_br_info.hbm.elvss[column]);
		kfree(vdd->panel_br_info.hbm.elvss);
		vdd->panel_br_info.hbm.elvss = NULL;

	}

	if (!IS_ERR_OR_NULL(vdd->panel_br_info.hbm.irc)) {
		for (column = 0; column < hbm_brightness_step; column++)
			kfree(vdd->panel_br_info.hbm.irc[column]);
		kfree(vdd->panel_br_info.hbm.irc);
		vdd->panel_br_info.hbm.irc = NULL;
	}

	/*alloc 2 dimension matrix */
	if (!vdd->panel_br_info.hbm.candela_table)
		vdd->panel_br_info.hbm.candela_table = kzalloc(hbm_brightness_step * sizeof(int), GFP_KERNEL);

	if (test_bit(BR_HBM_GAMMA, vdd->panel_br_info.br_info_select)) {
		if (!vdd->panel_br_info.hbm.gamma) {
			vdd->panel_br_info.hbm.gamma = kzalloc(hbm_brightness_step * sizeof(void *), GFP_KERNEL);
			for (column = 0; column < hbm_brightness_step; column++)
				vdd->panel_br_info.hbm.gamma[column] = kzalloc(gamma_size, GFP_KERNEL);
		}
	}

	if (test_bit(BR_HBM_AOR, vdd->panel_br_info.br_info_select)) {
		if (!vdd->panel_br_info.hbm.aor) {
			vdd->panel_br_info.hbm.aor = kzalloc(hbm_brightness_step * sizeof(void *), GFP_KERNEL);
			for (column = 0; column < hbm_brightness_step; column++)
				vdd->panel_br_info.hbm.aor[column] = kzalloc(aor_size, GFP_KERNEL);
		}
	}

	if (test_bit(BR_HBM_VINT, vdd->panel_br_info.br_info_select)) {
		if (!vdd->panel_br_info.hbm.vint) {
			vdd->panel_br_info.hbm.vint = kzalloc(hbm_brightness_step * sizeof(void *), GFP_KERNEL);
			for (column = 0; column < hbm_brightness_step; column++)
				vdd->panel_br_info.hbm.vint[column] = kzalloc(vint_size, GFP_KERNEL);
		}
	}

	if (test_bit(BR_HBM_ELVSS, vdd->panel_br_info.br_info_select)) {
		if (!vdd->panel_br_info.hbm.elvss) {
			vdd->panel_br_info.hbm.elvss = kzalloc(hbm_brightness_step * sizeof(void *), GFP_KERNEL);
			for (column = 0; column < hbm_brightness_step; column++)
				vdd->panel_br_info.hbm.elvss[column] = kzalloc(elvss_size, GFP_KERNEL);
		}
	}

	if (test_bit(BR_HBM_IRC, vdd->panel_br_info.br_info_select)) {
		if (!vdd->panel_br_info.hbm.irc) {
			vdd->panel_br_info.hbm.irc = kzalloc(hbm_brightness_step * sizeof(void *), GFP_KERNEL);
			for (column = 0; column < hbm_brightness_step; column++)
				vdd->panel_br_info.hbm.irc[column] = kzalloc(irc_size, GFP_KERNEL);
		}
	}
}

static void init_br_info_normal(struct samsung_display_driver_data *vdd)
{
	int column;

	int normal_brightness_step = vdd->dtsi_data.normal_brightness_step;
	int gamma_size = vdd->dtsi_data.gamma_size;
	int aor_size = vdd->dtsi_data.aor_size;
	int vint_size = vdd->dtsi_data.vint_size;
	int elvss_size = vdd->dtsi_data.elvss_size;
	int irc_size = vdd->dtsi_data.irc_size;

	/*
		free alloc memory : test purpose for slab memory integrity
	*/
	if (!IS_ERR_OR_NULL(vdd->panel_br_info.normal.candela_table)) {
		kfree(vdd->panel_br_info.normal.candela_table);
		vdd->panel_br_info.normal.candela_table = NULL;
	}

	if (!IS_ERR_OR_NULL(vdd->panel_br_info.normal.gamma)) {
		for (column = 0; column < normal_brightness_step; column++)
			kfree(vdd->panel_br_info.normal.gamma[column]);
		kfree(vdd->panel_br_info.normal.gamma);
		vdd->panel_br_info.normal.gamma = NULL;
	}

	if (!IS_ERR_OR_NULL(vdd->panel_br_info.normal.aor)) {
		for (column = 0; column < normal_brightness_step; column++)
			kfree(vdd->panel_br_info.normal.aor[column]);
		kfree(vdd->panel_br_info.normal.aor);
		vdd->panel_br_info.normal.aor = NULL;
	}

	if (!IS_ERR_OR_NULL(vdd->panel_br_info.normal.vint)) {
		for (column = 0; column < normal_brightness_step; column++)
			kfree(vdd->panel_br_info.normal.vint[column]);
		kfree(vdd->panel_br_info.normal.vint);
		vdd->panel_br_info.normal.vint = NULL;
	}

	if (!IS_ERR_OR_NULL(vdd->panel_br_info.normal.elvss)) {
		for (column = 0; column < normal_brightness_step; column++)
			kfree(vdd->panel_br_info.normal.elvss[column]);
		kfree(vdd->panel_br_info.normal.elvss);
		vdd->panel_br_info.normal.elvss = NULL;
	}

	if (!IS_ERR_OR_NULL(vdd->panel_br_info.normal.irc)) {
		for (column = 0; column < normal_brightness_step; column++)
			kfree(vdd->panel_br_info.normal.irc[column]);
		kfree(vdd->panel_br_info.normal.irc);
		vdd->panel_br_info.normal.irc = NULL;
	}

	/*alloc 2 dimension matrix */
	if (!vdd->panel_br_info.normal.candela_table)
		vdd->panel_br_info.normal.candela_table = kzalloc(normal_brightness_step * sizeof(int), GFP_KERNEL);

	if (test_bit(BR_NORMAL_GAMMA, vdd->panel_br_info.br_info_select)) {
		if (!vdd->panel_br_info.normal.gamma) {
			vdd->panel_br_info.normal.gamma = kzalloc(normal_brightness_step * sizeof(void *), GFP_KERNEL);
			for (column = 0; column < normal_brightness_step; column++)
				vdd->panel_br_info.normal.gamma[column] = kzalloc(gamma_size, GFP_KERNEL);
		}
	}

	if (test_bit(BR_NORMAL_AOR, vdd->panel_br_info.br_info_select)) {
		if (!vdd->panel_br_info.normal.aor) {
			vdd->panel_br_info.normal.aor = kzalloc(normal_brightness_step * sizeof(void *), GFP_KERNEL);
			for (column = 0; column < normal_brightness_step; column++)
				vdd->panel_br_info.normal.aor[column] = kzalloc(aor_size, GFP_KERNEL);
		}
	}

	if (test_bit(BR_NORMAL_VINT, vdd->panel_br_info.br_info_select)) {
		if (!vdd->panel_br_info.normal.vint) {
			vdd->panel_br_info.normal.vint = kzalloc(normal_brightness_step * sizeof(void *), GFP_KERNEL);
			for (column = 0; column < normal_brightness_step; column++)
				vdd->panel_br_info.normal.vint[column] = kzalloc(vint_size, GFP_KERNEL);
		}
	}

	if (test_bit(BR_NORMAL_ELVSS, vdd->panel_br_info.br_info_select)) {
		if (!vdd->panel_br_info.normal.elvss) {
			vdd->panel_br_info.normal.elvss = kzalloc(normal_brightness_step * sizeof(void *), GFP_KERNEL);
			for (column = 0; column < normal_brightness_step; column++)
				vdd->panel_br_info.normal.elvss[column] = kzalloc(elvss_size, GFP_KERNEL);
		}
	}

	if (test_bit(BR_NORMAL_IRC, vdd->panel_br_info.br_info_select)) {
		if (!vdd->panel_br_info.normal.irc) {
			vdd->panel_br_info.normal.irc = kzalloc(normal_brightness_step * sizeof(void *), GFP_KERNEL);
			for (column = 0; column < normal_brightness_step; column++)
				vdd->panel_br_info.normal.irc[column] = kzalloc(irc_size, GFP_KERNEL);
		}
	}
}

static void init_br_info_hmd(struct samsung_display_driver_data *vdd)
{
	int column;

	int hmd_brightness_step = vdd->dtsi_data.hmd_brightness_step;
	int gamma_size = vdd->dtsi_data.gamma_size;
	int aor_size = vdd->dtsi_data.aor_size;

	/*
		free alloc memory : test purpose for slab memory integrity
	*/
	if (!IS_ERR_OR_NULL(vdd->panel_br_info.hmd.candela_table)) {
		kfree(vdd->panel_br_info.hmd.candela_table);
		vdd->panel_br_info.hmd.candela_table = NULL;
	}

	if (!IS_ERR_OR_NULL(vdd->panel_br_info.hmd.gamma)) {
		for (column = 0; column < hmd_brightness_step; column++)
			kfree(vdd->panel_br_info.hmd.gamma[column]);
		kfree(vdd->panel_br_info.hmd.gamma);
		vdd->panel_br_info.hmd.gamma = NULL;
	}

	if (!IS_ERR_OR_NULL(vdd->panel_br_info.hmd.aor)) {
		for (column = 0; column < hmd_brightness_step; column++)
			kfree(vdd->panel_br_info.hmd.aor[column]);
		kfree(vdd->panel_br_info.hmd.aor);
		vdd->panel_br_info.hmd.aor = NULL;
	}

	/*alloc 2 dimension matrix */
	if (!vdd->panel_br_info.hmd.candela_table)
		vdd->panel_br_info.hmd.candela_table = kzalloc(hmd_brightness_step * sizeof(int), GFP_KERNEL);

	if (test_bit(BR_HMD_GAMMA, vdd->panel_br_info.br_info_select)) {
		if (!vdd->panel_br_info.hmd.gamma) {
			vdd->panel_br_info.hmd.gamma = kzalloc(hmd_brightness_step * sizeof(void *), GFP_KERNEL);
			for (column = 0; column < hmd_brightness_step; column++)
				vdd->panel_br_info.hmd.gamma[column] = kzalloc(gamma_size, GFP_KERNEL);
		}
	}

	if (test_bit(BR_HMD_AOR, vdd->panel_br_info.br_info_select)) {
		if (!vdd->panel_br_info.hmd.aor) {
			vdd->panel_br_info.hmd.aor = kzalloc(hmd_brightness_step * sizeof(void *), GFP_KERNEL);
			for (column = 0; column < hmd_brightness_step; column++)
				vdd->panel_br_info.hmd.aor[column] = kzalloc(aor_size, GFP_KERNEL);
		}
	}
}

static void update_br_info_hbm(struct samsung_display_driver_data *vdd)
{
	int column, data_cnt;

	char *flash_data = vdd->panel_br_info.br_data_raw;
	//char **gamma = vdd->panel_br_info.hbm.gamma;
	unsigned char **aor = vdd->panel_br_info.hbm.aor;
	unsigned char **vint = 	vdd->panel_br_info.hbm.vint;
	unsigned char **elvss = vdd->panel_br_info.hbm.elvss;
	unsigned char **irc = vdd->panel_br_info.hbm.irc;

	int aor_index = vdd->dtsi_data.flash_table_hbm_aor_offset;
	int vint_index =  vdd->dtsi_data.flash_table_hbm_vint_offset;
	int elvss_index =  vdd->dtsi_data.flash_table_hbm_elvss_offset;
	int irc_inedx =  vdd->dtsi_data.flash_table_hbm_irc_offset;

	int hbm_brightness_step = vdd->dtsi_data.hbm_brightness_step;
	int aor_size = vdd->dtsi_data.aor_size;
	int vint_size = vdd->dtsi_data.vint_size;
	int elvss_size = vdd->dtsi_data.elvss_size;
	int irc_size = vdd->dtsi_data.irc_size;

	int rc;
	struct device_node *np;
	np = ss_get_panel_of(vdd);

	for (column = 0; column < hbm_brightness_step; column++) {
		if (test_bit(BR_HBM_GAMMA, vdd->panel_br_info.br_info_select)) {
			LCD_INFO("update for other project\n");
		}

		if (test_bit(BR_HBM_AOR, vdd->panel_br_info.br_info_select)) {
			for (data_cnt = 0; data_cnt < aor_size; data_cnt++)
				aor[column][data_cnt] = flash_data[aor_index++];
		}

		if (test_bit(BR_HBM_VINT, vdd->panel_br_info.br_info_select)) {
			for (data_cnt = 0; data_cnt < vint_size; data_cnt++)
				vint[column][data_cnt] = flash_data[vint_index++];
		}

		if (test_bit(BR_HBM_ELVSS, vdd->panel_br_info.br_info_select)) {
			for (data_cnt = 0; data_cnt < elvss_size; data_cnt++)
				elvss[column][data_cnt] = flash_data[elvss_index++];
		}

		if (test_bit(BR_HBM_IRC, vdd->panel_br_info.br_info_select)) {
			for (data_cnt = 0; data_cnt < irc_size; data_cnt++)
				irc[column][data_cnt] = flash_data[irc_inedx++];
		}
	}

	rc = of_property_read_u32_array(np,
				"samsung,hbm_brightness",
				vdd->panel_br_info.hbm.candela_table,
				hbm_brightness_step);
	if (rc)
		LCD_INFO("fail to get samsung,hbm_brightness\n");
}

static void update_br_info_normal(struct samsung_display_driver_data *vdd)
{
	int column, data_cnt;

	unsigned char *flash_data = vdd->panel_br_info.br_data_raw;
	unsigned char **gamma = vdd->panel_br_info.normal.gamma;
	unsigned char **aor = vdd->panel_br_info.normal.aor;
	unsigned char **vint = 	vdd->panel_br_info.normal.vint;
	unsigned char **elvss = vdd->panel_br_info.normal.elvss;
	unsigned char **irc = vdd->panel_br_info.normal.irc;

	int gamma_index = vdd->dtsi_data.flash_table_normal_gamma_offset;
	int aor_index = vdd->dtsi_data.flash_table_normal_aor_offset;
	int vint_index = vdd->dtsi_data.flash_table_normal_vint_offset;
	int elvss_index = vdd->dtsi_data.flash_table_normal_elvss_offset;
	int irc_inedx = vdd->dtsi_data.flash_table_normal_irc_offset;

	int normal_brightness_step = vdd->dtsi_data.normal_brightness_step;
	int gamma_size = vdd->dtsi_data.gamma_size;
	int aor_size = vdd->dtsi_data.aor_size;
	int vint_size = vdd->dtsi_data.vint_size;
	int elvss_size = vdd->dtsi_data.elvss_size;
	int irc_size = vdd->dtsi_data.irc_size;

	int rc;
	struct device_node *np;
	np = ss_get_panel_of(vdd);

	for (column = 0; column < normal_brightness_step; column++) {
		if (test_bit(BR_NORMAL_GAMMA, vdd->panel_br_info.br_info_select)) {
			for (data_cnt = 0; data_cnt < gamma_size; data_cnt++)
				gamma[column][data_cnt] = flash_data[gamma_index++];
		}

		if (test_bit(BR_NORMAL_AOR, vdd->panel_br_info.br_info_select)) {
			for (data_cnt = 0; data_cnt < aor_size; data_cnt++)
				aor[column][data_cnt] = flash_data[aor_index++];
		}

		if (test_bit(BR_NORMAL_VINT, vdd->panel_br_info.br_info_select)) {
			for (data_cnt = 0; data_cnt < vint_size; data_cnt++)
				vint[column][data_cnt] = flash_data[vint_index++];
		}

		if (test_bit(BR_NORMAL_ELVSS, vdd->panel_br_info.br_info_select)) {
			for (data_cnt = 0; data_cnt < elvss_size; data_cnt++)
				elvss[column][data_cnt] = flash_data[elvss_index++];
		}

		if (test_bit(BR_NORMAL_IRC, vdd->panel_br_info.br_info_select)) {
			for (data_cnt = 0; data_cnt < irc_size; data_cnt++)
				irc[column][data_cnt] = flash_data[irc_inedx++];
		}
	}

	rc = of_property_read_u32_array(np,
				"samsung,normal_brightness",
				vdd->panel_br_info.normal.candela_table,
				normal_brightness_step);
	if (rc)
		LCD_INFO("fail to get samsung,normal_brightness\n");
}

static void update_br_info_hmd(struct samsung_display_driver_data *vdd)
{
	int column, data_cnt;

	unsigned char *flash_data = vdd->panel_br_info.br_data_raw;
	unsigned char **gamma = vdd->panel_br_info.hmd.gamma;
	unsigned char **aor = vdd->panel_br_info.hmd.aor;

	int gamma_index = vdd->dtsi_data.flash_table_hmd_gamma_offset;
	int aor_index = vdd->dtsi_data.flash_table_hmd_aor_offset;

	int hmd_brightness_step = vdd->dtsi_data.hmd_brightness_step;
	int gamma_size = vdd->dtsi_data.gamma_size;
	int aor_size = vdd->dtsi_data.aor_size;

	int rc;
	struct device_node *np;
	np = ss_get_panel_of(vdd);

	for (column = 0; column < hmd_brightness_step; column++) {
		if (test_bit(BR_HMD_GAMMA, vdd->panel_br_info.br_info_select)) {
			for (data_cnt = 0; data_cnt < gamma_size; data_cnt++)
				gamma[column][data_cnt] = flash_data[gamma_index++];
		}

		if (test_bit(BR_HMD_AOR, vdd->panel_br_info.br_info_select)) {
			for (data_cnt = 0; data_cnt < aor_size; data_cnt++)
				aor[column][data_cnt] = flash_data[aor_index++];
		}
	}

	rc = of_property_read_u32_array(np,
				"samsung,hmd_brightness",
				vdd->panel_br_info.hmd.candela_table,
				hmd_brightness_step);
	if (rc)
		LCD_INFO("fail to get samsung,hmd_brightness\n");
}

void set_up_br_info(struct samsung_display_driver_data *vdd)
{
	init_br_info_hbm(vdd);
	init_br_info_normal(vdd);
	init_br_info_hmd(vdd);

	update_br_info_hbm(vdd);
	update_br_info_normal(vdd);
	update_br_info_hmd(vdd);
}

#define read_buf_size 64
char flash_read_one_byte(struct samsung_display_driver_data *vdd, int addr)
{
	struct dsi_panel_cmd_set *flash_gamma_cmds = ss_get_cmds(vdd, TX_FLASH_GAMMA);
	struct dsi_panel_cmd_set *rx_gamma_cmds = ss_get_cmds(vdd, RX_FLASH_GAMMA);
	char gamma_buf[read_buf_size];

	/* for seding direct rx cmd  */
	rx_gamma_cmds->cmds[0].msg.rx_buf = gamma_buf;
	rx_gamma_cmds->state = DSI_CMD_SET_STATE_HS;

	flash_gamma_cmds->cmds[0].msg.tx_buf[6] = (char)((addr & 0xFF0000) >> 16);
	flash_gamma_cmds->cmds[0].msg.tx_buf[7] = (char)((addr & 0xFF00) >> 8);
	flash_gamma_cmds->cmds[0].msg.tx_buf[8] = (char)(addr & 0xFF);

	ss_send_cmd(vdd, TX_FLASH_GAMMA);

	usleep_range(250, 250);

	/* read data */
	memset(gamma_buf, 0x00, sizeof(gamma_buf));
	ss_send_cmd(vdd, RX_FLASH_GAMMA);

	return gamma_buf[0];
}

void flash_write_check_read(struct samsung_display_driver_data *vdd)
{
	int addr, loop_cnt, start_addr, end_addr;
	char read_buf = 0;

	start_addr = vdd->dtsi_data.flash_gamma_write_check_address;
	end_addr = vdd->dtsi_data.flash_gamma_write_check_address;

	for (loop_cnt = 0, addr = start_addr; addr <= end_addr ; addr++) {
		read_buf = flash_read_one_byte(vdd, addr);
		vdd->panel_br_info.flash_data.write_check =
			vdd->panel_br_info.flash_data.write_check << (loop_cnt++ * 8) | read_buf;
	}
}

void flash_data_read(struct samsung_display_driver_data *vdd, int bank_index)
{
	int addr, loop_cnt, start_addr, end_addr;
	char read_buf = 0;

	start_addr = vdd->dtsi_data.flash_gamma_bank_start[bank_index];
	end_addr = vdd->dtsi_data.flash_gamma_bank_end[bank_index];

	for (loop_cnt = 0, addr = start_addr; addr <= end_addr ; addr++) {
		read_buf = flash_read_one_byte(vdd, addr);

		vdd->panel_br_info.br_data_raw[loop_cnt++] = read_buf;
		vdd->panel_br_info.flash_data.check_sum_cal_data += read_buf;
	}

	/* 16bit sum check */
	vdd->panel_br_info.flash_data.check_sum_cal_data &= ERASED_MMC_16BIT;
}

void flash_checksum_read(struct samsung_display_driver_data *vdd, int bank_index)
{
	int addr, loop_cnt, bank_addr, start_addr, end_addr;
	char read_buf = 0;

	/* init check sum data */
	vdd->panel_br_info.flash_data.check_sum_flash_data = MMC_CHECK_SUM_INIT;
	vdd->panel_br_info.flash_data.check_sum_cal_data = MMC_CHECK_SUM_INIT;

	bank_addr = vdd->dtsi_data.flash_gamma_bank_start[bank_index];
	start_addr =  bank_addr + vdd->dtsi_data.flash_gamma_check_sum_start_offset;
	end_addr = bank_addr + vdd->dtsi_data.flash_gamma_check_sum_end_offset;

	for (loop_cnt = 0, addr = start_addr; addr <= end_addr ; addr++) {
		read_buf = flash_read_one_byte(vdd, addr);

		vdd->panel_br_info.flash_data.check_sum_flash_data =
			vdd->panel_br_info.flash_data.check_sum_flash_data << (loop_cnt++ * 8) | read_buf;
	}
}

void flash_0xc8_read(struct samsung_display_driver_data *vdd, int bank_index)
{
	int addr, loop_cnt, bank_addr,start_addr, end_addr;
	char read_buf = 0;

	/* init check sum data */
	vdd->panel_br_info.flash_data.c8_register.check_sum_mtp_data = MMC_CHECK_SUM_INIT;
	vdd->panel_br_info.flash_data.c8_register.check_sum_flash_data = MMC_CHECK_SUM_INIT;
	vdd->panel_br_info.flash_data.c8_register.check_sum_cal_data = MMC_CHECK_SUM_INIT;

	/* check sum value check */
	bank_addr = vdd->dtsi_data.flash_gamma_bank_start[bank_index];
	start_addr = bank_addr + vdd->dtsi_data.flash_gamma_0xc8_check_sum_start_offset;
	end_addr = bank_addr + vdd->dtsi_data.flash_gamma_0xc8_check_sum_end_offset;

	for (loop_cnt = 0, addr = start_addr; addr <= end_addr ; addr++) {
		read_buf = flash_read_one_byte(vdd, addr);

		vdd->panel_br_info.flash_data.c8_register.check_sum_flash_data =
			vdd->panel_br_info.flash_data.c8_register.check_sum_flash_data << (loop_cnt++ * 8) | read_buf;

	}

	/* read real flash gamma data */
	start_addr = vdd->dtsi_data.flash_gamma_0xc8_start_offset + bank_addr;
	end_addr = vdd->dtsi_data.flash_gamma_0xc8_end_offset + bank_addr;
	for (loop_cnt = 0, addr = start_addr; addr <= end_addr ; addr++) {
		read_buf = flash_read_one_byte(vdd, addr);

		vdd->panel_br_info.flash_data.c8_register.flash_data[loop_cnt++] = read_buf;
		vdd->panel_br_info.flash_data.c8_register.check_sum_cal_data += read_buf;
	}

	/* check ddi 0xc8 register value */
	for (loop_cnt = 0; loop_cnt <= vdd->dtsi_data.flash_gamma_0xc8_size; loop_cnt++)
		vdd->panel_br_info.flash_data.c8_register.check_sum_mtp_data += vdd->panel_br_info.flash_data.c8_register.mtp_data[loop_cnt];


	/* 16bit sum check */
	vdd->panel_br_info.flash_data.c8_register.check_sum_mtp_data &= ERASED_MMC_16BIT;
	vdd->panel_br_info.flash_data.c8_register.check_sum_cal_data &= ERASED_MMC_16BIT;


	LCD_INFO("read 0xC8 mpt_check_sum : 0x%x flash_check_sum : 0x%x cal_check_sum : 0x%x\n",
			vdd->panel_br_info.flash_data.c8_register.check_sum_mtp_data,
			vdd->panel_br_info.flash_data.c8_register.check_sum_flash_data,
			vdd->panel_br_info.flash_data.c8_register.check_sum_cal_data);
}


void flash_mcd_read(struct samsung_display_driver_data *vdd)
{
	char read_buf = 0;

	read_buf = flash_read_one_byte(vdd, vdd->dtsi_data.flash_MCD1_R_address);
	vdd->panel_br_info.flash_data.mcd.flash_MCD1_R = read_buf;

	read_buf = flash_read_one_byte(vdd, vdd->dtsi_data.flash_MCD2_R_address);
	vdd->panel_br_info.flash_data.mcd.flash_MCD2_R = read_buf;

	read_buf = flash_read_one_byte(vdd, vdd->dtsi_data.flash_MCD1_L_address);
	vdd->panel_br_info.flash_data.mcd.flash_MCD1_L = read_buf;

	read_buf = flash_read_one_byte(vdd, vdd->dtsi_data.flash_MCD2_L_address);
	vdd->panel_br_info.flash_data.mcd.flash_MCD2_L = read_buf;
}

void br_basic_register_read(struct samsung_display_driver_data *vdd)
{
	/* Read mtp for hbm gamma */
	ss_panel_data_read(vdd, RX_HBM, vdd->panel_br_info.hbm_read_buffer, LEVEL1_KEY);

	/* Read 0xC8 */
	ss_panel_data_read(vdd, RX_SMART_DIM_MTP, vdd->panel_br_info.flash_data.c8_register.mtp_data, LEVEL1_KEY);
}

int flash_gamma_support_check(struct samsung_display_driver_data *vdd)
{
	/* check force update mode */
	if (vdd->panel_br_info.flash_data.force_update == true) {
		vdd->panel_br_info.flash_data.force_update = false;
		return true;
	}

	/* check panel support */
	if (!IS_ERR_OR_NULL(vdd->panel_func.samsung_flash_gamma_support)) {
		if (!vdd->panel_func.samsung_flash_gamma_support(vdd)) {
			LCD_ERR("FLASH_GAMMA_NOT_SUPPORT\n");
			return false;
		}
	}

	return true;
}

int flash_checksum_check(struct samsung_display_driver_data *vdd)
{
	if (vdd->panel_br_info.flash_data.write_check != FLASH_GAMMA_BURN_WRITE) {
		LCD_ERR("FLASH_GAMMA_BURN_EMPTY\n");
		return false;
	} else if (vdd->panel_br_info.flash_data.check_sum_cal_data != vdd->panel_br_info.flash_data.check_sum_flash_data) {
		LCD_ERR("CHECK_SUM_FALSH_ERROR\n");
		return false;
	} else if (vdd->panel_br_info.flash_data.c8_register.check_sum_flash_data != vdd->panel_br_info.flash_data.c8_register.check_sum_cal_data) {
		LCD_ERR("CHECK_SUM_0xC8_ERROR_1\n");
		return false;
	} else if (vdd->panel_br_info.flash_data.c8_register.check_sum_flash_data != vdd->panel_br_info.flash_data.c8_register.check_sum_mtp_data) {
		LCD_ERR("CHECK_SUM_0xC8_ERROR_2\n");
		return false;
	} else if (vdd->panel_br_info.flash_data.c8_register.check_sum_cal_data != vdd->panel_br_info.flash_data.c8_register.check_sum_mtp_data) {
		LCD_ERR("CHECK_SUM_0xC8_ERROR_3\n");
		return false;
	} else
		return true;
}

void flash_br_work_func(struct work_struct *work)
{
	struct delayed_work * dwork = to_delayed_work(work);
	struct samsung_display_driver_data *vdd = container_of(dwork, struct samsung_display_driver_data, flash_br_work);
	struct dsi_display *display = NULL;
	struct msm_drm_private *priv = NULL;
	struct sde_kms *sde_kms = NULL;
	u64 sde_mnoc_ab, sde_mnoc_ib;
	int rc = 0;

	if (IS_ERR_OR_NULL(vdd)) {
		LCD_ERR("no vdd");
		goto end;
	}

	display = GET_DSI_DISPLAY(vdd);
	if (IS_ERR_OR_NULL(display)) {
		LCD_ERR("no display");
		goto end;
	}

	if (!vdd->dtsi_data.flash_gamma_support) {
		LCD_ERR("flash_gamma_support not support");
		goto end;
	}

	if (!flash_gamma_support_check(vdd))
		goto end;

	LCD_INFO("\n");

	priv = display->drm_dev->dev_private;
	sde_kms = (struct sde_kms *)priv->kms;
	sde_mnoc_ab = sde_kms->core_client->ab[SDE_POWER_HANDLE_DATA_BUS_CLIENT_RT];
	sde_mnoc_ib = sde_kms->core_client->ib[SDE_POWER_HANDLE_DATA_BUS_CLIENT_RT];

	br_basic_register_read(vdd); /* hbm & smart-dimming register read */

	mutex_lock(&vdd->exclusive_tx.ex_tx_lock);
	vdd->exclusive_tx.permit_frame_update = 1;
	vdd->exclusive_tx.enable = 1;

	if (test_bit(BOOST_CPU, vdd->panel_br_info.flash_br_boosting)) {
		/* max cpu freq */
		LCD_INFO("CPU\n");
		ss_set_max_cpufreq(vdd, true, CPUFREQ_CLUSTER_ALL);
	}

	if (test_bit(BOOST_MNOC, vdd->panel_br_info.flash_br_boosting)) {
		LCD_INFO("MNOC\n");
		/* update only mnoc bw*/
		sde_power_data_bus_set_quota(&priv->phandle,
			sde_kms->core_client,
			SDE_POWER_HANDLE_DATA_BUS_CLIENT_RT,
			SDE_POWER_HANDLE_DBUS_ID_MNOC,
			SDE_POWER_HANDLE_CONT_SPLASH_BUS_AB_QUOTA,
			SDE_POWER_HANDLE_CONT_SPLASH_BUS_IB_QUOTA);
	}

	if (test_bit(BOOST_DSI_CLK, vdd->panel_br_info.flash_br_boosting)) {
		/* enable dsi clock*/
		LCD_INFO("CLK\n");
		rc = dsi_display_clk_ctrl(display->dsi_clk_handle,
				DSI_ALL_CLKS, DSI_CLK_ON);
		if (rc) {
			LCD_ERR("[%s] failed to enable DSI core clocks, rc=%d\n",
					display->name, rc);
			goto error;
		}
	}

	ss_set_exclusive_tx_packet(vdd, TX_FLASH_GAMMA_PRE, 1);
	ss_set_exclusive_tx_packet(vdd, TX_FLASH_GAMMA, 1);
	ss_set_exclusive_tx_packet(vdd, TX_FLASH_GAMMA_POST, 1);
	ss_set_exclusive_tx_packet(vdd, RX_FLASH_GAMMA, 1);

	ss_send_cmd(vdd, TX_FLASH_GAMMA_PRE);

	/* 1st */
	flash_write_check_read(vdd);

	/* 2st */
	flash_checksum_read(vdd, 0);

	/* 3 st */
	init_br_info(vdd);

	/*4 st */
	flash_data_read(vdd, 0);

	/* 5 st extra 0xC8 bank */
	flash_0xc8_read(vdd, 0);

	/* 6 st flash_data.mcd flash value */
	//flash_flash_data.mcd_read(vdd);

	LCD_INFO("read write_check : 0x%x flash_check_sum : 0x%x cal_check_sum : 0x%x\n",
		vdd->panel_br_info.flash_data.write_check,
		vdd->panel_br_info.flash_data.check_sum_flash_data,
		vdd->panel_br_info.flash_data.check_sum_cal_data);

	if (flash_checksum_check(vdd)) {
		if (!IS_ERR_OR_NULL(vdd->panel_func.samsung_interpolation_init))
			vdd->panel_func.samsung_interpolation_init(vdd, FLASH_INTERPOLATION);
	}

	ss_send_cmd(vdd, TX_FLASH_GAMMA_POST);

	if (test_and_clear_bit(BOOST_DSI_CLK, vdd->panel_br_info.flash_br_boosting)) {
		rc = dsi_display_clk_ctrl(display->dsi_clk_handle,
				DSI_ALL_CLKS, DSI_CLK_OFF);
		if (rc) {
			LCD_ERR("[%s] failed to disable DSI core clocks, rc=%d\n",
					display->name, rc);
		}
	}

	ss_set_exclusive_tx_packet(vdd, TX_FLASH_GAMMA_PRE, 0);
	ss_set_exclusive_tx_packet(vdd, TX_FLASH_GAMMA, 0);
	ss_set_exclusive_tx_packet(vdd, TX_FLASH_GAMMA_POST, 0);
	ss_set_exclusive_tx_packet(vdd, RX_FLASH_GAMMA, 0);

error:
	if (test_and_clear_bit(BOOST_MNOC, vdd->panel_br_info.flash_br_boosting)) {
		/* restore bw */
		sde_power_data_bus_set_quota(&priv->phandle,
			sde_kms->core_client,
			SDE_POWER_HANDLE_DATA_BUS_CLIENT_RT, SDE_POWER_HANDLE_DBUS_ID_MNOC,
			sde_mnoc_ab, sde_mnoc_ib);
	}

	if (test_and_clear_bit(BOOST_CPU, vdd->panel_br_info.flash_br_boosting)) {
		/* release max cpu freq */
		ss_set_max_cpufreq(vdd, false, CPUFREQ_CLUSTER_ALL);
	}

	vdd->exclusive_tx.permit_frame_update = 0;
	vdd->exclusive_tx.enable = 0;
	wake_up_all(&vdd->exclusive_tx.ex_tx_waitq);
	mutex_unlock(&vdd->exclusive_tx.ex_tx_lock);

end:
	/* update init_done */
	vdd->panel_br_info.flash_data.init_done = true;

	/* update brighntess */
	ss_brightness_dcs(vdd, vdd->bl_level);
}

void table_br_func(struct samsung_display_driver_data *vdd)
{
	/* 1 st */
	init_br_info(vdd);

	/* 2 st */
	br_basic_register_read(vdd); /* hbm & smart-dimming register read */
}

static void debug_br_info_hbm(struct samsung_display_driver_data *vdd)
{
	char buf[FLASH_GAMMA_DBG_BUF_SIZE];
	int column, data_cnt;
	unsigned char **gamma, **aor, **vint, **elvss, **irc;
	int brightness_step, gamm_size, aor_size, vint_size, elvss_size, irc_size;

	memset(buf, '\n', sizeof(buf));

	brightness_step = vdd->dtsi_data.hbm_brightness_step;
	gamm_size = vdd->dtsi_data.gamma_size;
	aor_size = vdd->dtsi_data.aor_size;
	vint_size = vdd->dtsi_data.vint_size;
	elvss_size = vdd->dtsi_data.elvss_size;
	irc_size = vdd->dtsi_data.irc_size;

	gamma = vdd->panel_br_info.hbm.gamma;
	aor = vdd->panel_br_info.hbm.aor;
	vint = vdd->panel_br_info.hbm.vint;
	elvss = vdd->panel_br_info.hbm.elvss;
	irc = vdd->panel_br_info.hbm.irc;

	for (column =  0; column < brightness_step; column++) {
		snprintf(buf, FLASH_GAMMA_DBG_BUF_SIZE, "hbm lux : %d", vdd->panel_br_info.hbm.candela_table[column]);
		LCD_INFO("%s\n", buf);
		memset(buf, '\n', strlen(buf));

		/* gamma */
		if (!IS_ERR_OR_NULL(gamma)) {
			snprintf(buf, FLASH_GAMMA_DBG_BUF_SIZE, "gamma : ");
			for (data_cnt = 0; data_cnt < gamm_size; data_cnt++)
				snprintf(buf + strlen(buf), FLASH_GAMMA_DBG_BUF_SIZE - strlen(buf), "%02x ", gamma[column][data_cnt]);
			LCD_INFO("%s\n", buf);
			memset(buf, '\n', strlen(buf));
		}

		/* AOR */
		if (!IS_ERR_OR_NULL(aor)) {
			snprintf(buf, FLASH_GAMMA_DBG_BUF_SIZE, "aor : ");
			for (data_cnt = 0; data_cnt < aor_size; data_cnt++)
				snprintf(buf + strlen(buf), FLASH_GAMMA_DBG_BUF_SIZE - strlen(buf), "%02x ", aor[column][data_cnt]);
			LCD_INFO("%s\n", buf);
			memset(buf, '\n', strlen(buf));
		}

		/* VINT */
		if (!IS_ERR_OR_NULL(vint)) {
			snprintf(buf, FLASH_GAMMA_DBG_BUF_SIZE, "vint : ");
			for (data_cnt = 0; data_cnt < vint_size; data_cnt++)
				snprintf(buf + strlen(buf), FLASH_GAMMA_DBG_BUF_SIZE - strlen(buf), "%02x ", vint[column][data_cnt]);
			LCD_INFO("%s\n", buf);
			memset(buf, '\n', strlen(buf));
		}

		/* ELVSS */
		if (!IS_ERR_OR_NULL(elvss)) {
			snprintf(buf, FLASH_GAMMA_DBG_BUF_SIZE, "elvss : ");
			for (data_cnt = 0; data_cnt < elvss_size; data_cnt++)
				snprintf(buf + strlen(buf), FLASH_GAMMA_DBG_BUF_SIZE - strlen(buf), "%02x ", elvss[column][data_cnt]);
			LCD_INFO("%s\n", buf);
			memset(buf, '\n', strlen(buf));
		}

		/* IRC */
		if (!IS_ERR_OR_NULL(irc)) {
			snprintf(buf, FLASH_GAMMA_DBG_BUF_SIZE, "irc : ");
			for (data_cnt = 0; data_cnt < irc_size; data_cnt++)
				snprintf(buf + strlen(buf), FLASH_GAMMA_DBG_BUF_SIZE - strlen(buf), "%02x ", irc[column][data_cnt]);
			LCD_INFO("%s\n", buf);
			memset(buf, '\n', strlen(buf));
		}
	}
}

static void debug_br_info_normal(struct samsung_display_driver_data *vdd)
{
	char buf[FLASH_GAMMA_DBG_BUF_SIZE];
	int column, data_cnt;
	unsigned char **gamma, **aor, **vint, **elvss, **irc;
	int brightness_step, gamm_size, aor_size, vint_size, elvss_size, irc_size;

	memset(buf, '\n', sizeof(buf));

	brightness_step = vdd->dtsi_data.normal_brightness_step;
	gamm_size = vdd->dtsi_data.gamma_size;
	aor_size = vdd->dtsi_data.aor_size;
	vint_size = vdd->dtsi_data.vint_size;
	elvss_size = vdd->dtsi_data.elvss_size;
	irc_size = vdd->dtsi_data.irc_size;

	gamma = vdd->panel_br_info.normal.gamma;
	aor = vdd->panel_br_info.normal.aor;
	vint = vdd->panel_br_info.normal.vint;
	elvss = vdd->panel_br_info.normal.elvss;
	irc = vdd->panel_br_info.normal.irc;

	for (column =  0; column < brightness_step; column++) {
		snprintf(buf, FLASH_GAMMA_DBG_BUF_SIZE, "normal lux : %d", vdd->panel_br_info.normal.candela_table[column]);
		LCD_INFO("%s\n", buf);
		memset(buf, '\n', strlen(buf));

		/* gamma */
		if (!IS_ERR_OR_NULL(gamma)) {
			snprintf(buf, FLASH_GAMMA_DBG_BUF_SIZE, "gamma : ");
			for (data_cnt = 0; data_cnt < gamm_size; data_cnt++)
				snprintf(buf + strlen(buf), FLASH_GAMMA_DBG_BUF_SIZE - strlen(buf), "%02x ", gamma[column][data_cnt]);
			LCD_INFO("%s\n", buf);
			memset(buf, '\n', strlen(buf));
		}

		/* AOR */
		if (!IS_ERR_OR_NULL(aor)) {
			snprintf(buf, FLASH_GAMMA_DBG_BUF_SIZE, "aor : ");
			for (data_cnt = 0; data_cnt < aor_size; data_cnt++)
				snprintf(buf + strlen(buf), FLASH_GAMMA_DBG_BUF_SIZE - strlen(buf), "%02x ", aor[column][data_cnt]);
			LCD_INFO("%s\n", buf);
			memset(buf, '\n', strlen(buf));
		}

		/* VINT */
		if (!IS_ERR_OR_NULL(vint)) {
			snprintf(buf, FLASH_GAMMA_DBG_BUF_SIZE, "vint : ");
			for (data_cnt = 0; data_cnt < vint_size; data_cnt++)
				snprintf(buf + strlen(buf), FLASH_GAMMA_DBG_BUF_SIZE - strlen(buf), "%02x ", vint[column][data_cnt]);
			LCD_INFO("%s\n", buf);
			memset(buf, '\n', strlen(buf));
		}

		/* ELVSS */
		if (!IS_ERR_OR_NULL(elvss)) {
			snprintf(buf, FLASH_GAMMA_DBG_BUF_SIZE, "elvss : ");
			for (data_cnt = 0; data_cnt < elvss_size; data_cnt++)
				snprintf(buf + strlen(buf), FLASH_GAMMA_DBG_BUF_SIZE - strlen(buf), "%02x ", elvss[column][data_cnt]);
			LCD_INFO("%s\n", buf);
			memset(buf, '\n', strlen(buf));
		}

		/* IRC */
		if (!IS_ERR_OR_NULL(irc)) {
			snprintf(buf, FLASH_GAMMA_DBG_BUF_SIZE, "irc : ");
			for (data_cnt = 0; data_cnt < irc_size; data_cnt++)
				snprintf(buf + strlen(buf), FLASH_GAMMA_DBG_BUF_SIZE - strlen(buf), "%02x ", irc[column][data_cnt]);
			LCD_INFO("%s\n", buf);
			memset(buf, '\n', strlen(buf));
		}
	}
}

static void debug_br_info_hmd(struct samsung_display_driver_data *vdd)
{
	char buf[FLASH_GAMMA_DBG_BUF_SIZE];
	int column, data_cnt;
	unsigned char **gamma, **aor;
	int brightness_step, gamm_size, aor_size;

	memset(buf, '\n', sizeof(buf));

	brightness_step = vdd->dtsi_data.hmd_brightness_step;
	gamm_size = vdd->dtsi_data.gamma_size;
	aor_size = vdd->dtsi_data.aor_size;

	gamma = vdd->panel_br_info.hmd.gamma;
	aor = vdd->panel_br_info.hmd.aor;

	for (column =  0; column < brightness_step; column++) {
		snprintf(buf, FLASH_GAMMA_DBG_BUF_SIZE, "hmd lux : %d", vdd->panel_br_info.hmd.candela_table[column]);
		LCD_INFO("%s\n", buf);
		memset(buf, '\n', strlen(buf));

		/* gamma */
		if (!IS_ERR_OR_NULL(gamma)) {
			snprintf(buf, FLASH_GAMMA_DBG_BUF_SIZE, "gamma : ");
			for (data_cnt = 0; data_cnt < gamm_size; data_cnt++)
				snprintf(buf + strlen(buf), FLASH_GAMMA_DBG_BUF_SIZE - strlen(buf), "%02x ", gamma[column][data_cnt]);
			LCD_INFO("%s\n", buf);
			memset(buf, '\n', strlen(buf));
		}

		/* AOR */
		if (!IS_ERR_OR_NULL(aor)) {
			snprintf(buf, FLASH_GAMMA_DBG_BUF_SIZE, "aor : ");
			for (data_cnt = 0; data_cnt < aor_size; data_cnt++)
				snprintf(buf + strlen(buf), FLASH_GAMMA_DBG_BUF_SIZE - strlen(buf), "%02x ", aor[column][data_cnt]);
			LCD_INFO("%s\n", buf);
			memset(buf, '\n', strlen(buf));
		}
	}
}

void debug_br_info_log(struct samsung_display_driver_data *vdd)
{
	debug_br_info_hbm(vdd);
	debug_br_info_normal(vdd);
	debug_br_info_hmd(vdd);
}

