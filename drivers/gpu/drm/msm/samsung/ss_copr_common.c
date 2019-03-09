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

#include "ss_dsi_panel_common.h"
#include "ss_ddi_spi_common.h"
#include "ss_copr_common.h"

void ss_copr_enable(struct samsung_display_driver_data *vdd, int enable)
{
	if (vdd->copr.copr_on == enable) {
		LCD_ERR("copr already %d..\n", vdd->copr.copr_on);
		return;
	}

	mutex_lock(&vdd->copr.copr_lock);

	if (enable) {
		/* enable COPR IP */
		ss_send_cmd(vdd, TX_COPR_ENABLE);
	} else {
		/* disable COPR IP */
		ss_send_cmd(vdd, TX_COPR_DISABLE);

		vdd->copr.copr_cd[COPR_CD_INDEX_0].cd_sum = 0;
		vdd->copr.copr_cd[COPR_CD_INDEX_0].total_t = 0;
	}

	vdd->copr.copr_on = enable;

	LCD_INFO("copr %s .. \n", vdd->copr.copr_on?"enabled..":"disabled..");

	mutex_unlock(&vdd->copr.copr_lock);

	return;
}

/**
 * ss_copr_set_cnt_re - set Counter Reset Trigger Signal values to copr cmds
 */
void ss_copr_set_cnt_re(int reset)
{
	struct samsung_display_driver_data *vdd = samsung_get_vdd();

	vdd->copr.cmd.CNT_RE = !!reset;
}

/**
 * ss_copr_set_en - set copr power on/off control values to copr cmds
 */
void ss_copr_set_en(int enable)
{
	struct samsung_display_driver_data *vdd = samsung_get_vdd();

	vdd->copr.cmd.COPR_EN = !!enable;
}

/**
 * ss_copr_set_ilc - set Illuminance Compensated COPR values to copr cmds
 */
void ss_copr_set_ilc(int enable)
{
	struct samsung_display_driver_data *vdd = samsung_get_vdd();

	vdd->copr.cmd.COPR_ILC = !!enable;
}

/**
 * ss_copr_set_gamma - set Gamma Selection values to copr cmds
 */
void ss_copr_set_gamma(int val)
{
	struct samsung_display_driver_data *vdd = samsung_get_vdd();

	vdd->copr.cmd.COPR_GAMMA = !!val;
}

/**
 * ss_copr_set_max_cnt - set Max Count values to copr cmds
 */
void ss_copr_set_max_cnt(int cnt)
{
	struct samsung_display_driver_data *vdd = samsung_get_vdd();

	if (cnt > MAX_COPR_CNT)
		cnt = MAX_COPR_CNT;

	vdd->copr.cmd.MAX_CNT = cnt;
}

/**
 * ss_copr_set_e - set Efficiency values to copr cmds
 */
void ss_copr_set_e(int r, int g, int b)
{
	struct samsung_display_driver_data *vdd = samsung_get_vdd();

	vdd->copr.cmd.COPR_ER = r;
	vdd->copr.cmd.COPR_EG = g;
	vdd->copr.cmd.COPR_EB = b;
}

/**
 * ss_copr_set_ec - set Efficiency for Illuminance Compensation values to copr cmds
 */
void ss_copr_set_ec(int r, int g, int b)
{
	struct samsung_display_driver_data *vdd = samsung_get_vdd();

	vdd->copr.cmd.COPR_ERC = r;
	vdd->copr.cmd.COPR_EGC = g;
	vdd->copr.cmd.COPR_EBC = b;
}

/**
 * ss_copr_set_roi - set roi values in cmds
 */
void ss_copr_set_roi(struct COPR_ROI *roi)
{
	struct samsung_display_driver_data *vdd = samsung_get_vdd();

	vdd->copr.cmd.ROI_X_S = roi->ROI_X_S;
	vdd->copr.cmd.ROI_Y_S = roi->ROI_Y_S;
	vdd->copr.cmd.ROI_X_E = roi->ROI_X_E;
	vdd->copr.cmd.ROI_Y_E = roi->ROI_Y_E;

	if (roi->ROI_X_S || roi->ROI_Y_S || roi->ROI_X_E || roi->ROI_Y_E)
		vdd->copr.cmd.ROI_ON = 1;
	else
		vdd->copr.cmd.ROI_ON = 0;
}

/**
 * ss_copr_set_cmd - set copr mipi cmd using COPR_CMD para.
 *
 * copr_cmd : new copr cmd to update.
 */
void ss_copr_set_cmd(struct COPR_CMD copr_cmd)
{
	struct samsung_display_driver_data *vdd = samsung_get_vdd();
	struct dsi_panel_cmd_set *pcmds = NULL;
	u8 *cmd_pload;
	char buf[256];
	int i, len = 0;

	pcmds = ss_get_cmds(vdd, TX_COPR_ENABLE);
	cmd_pload = pcmds->cmds[1].msg.tx_buf;

	cmd_pload[1] = copr_cmd.COPR_EN;
	cmd_pload[1] |= (copr_cmd.COPR_GAMMA << 1);
	cmd_pload[1] |= (copr_cmd.COPR_ILC << 2);
	cmd_pload[1] |= (copr_cmd.CNT_RE << 3);

	cmd_pload[2] = ((copr_cmd.COPR_ER & 0x300) >> 4);
	cmd_pload[2] |= ((copr_cmd.COPR_EG & 0x300) >> 6);
	cmd_pload[2] |= ((copr_cmd.COPR_EB & 0x300) >> 8);

	cmd_pload[3] = ((copr_cmd.COPR_ERC & 0x300) >> 4);
	cmd_pload[3] |= ((copr_cmd.COPR_EGC & 0x300) >> 6);
	cmd_pload[3] |= ((copr_cmd.COPR_EBC & 0x300) >> 8);

	cmd_pload[4] = (copr_cmd.COPR_ER & 0xFF);
	cmd_pload[5] = (copr_cmd.COPR_EG & 0xFF);
	cmd_pload[6] = (copr_cmd.COPR_EB & 0xFF);

	cmd_pload[7] = (copr_cmd.COPR_ERC & 0xFF);
	cmd_pload[8] = (copr_cmd.COPR_EGC & 0xFF);
	cmd_pload[9] = (copr_cmd.COPR_EBC & 0xFF);

	cmd_pload[10] = ((copr_cmd.MAX_CNT & 0xFF00) >> 8);
	cmd_pload[11] = (copr_cmd.MAX_CNT & 0xFF);

	cmd_pload[12] = (copr_cmd.ROI_ON << 3);

	cmd_pload[12] |= ((copr_cmd.ROI_X_S & 0x700) >> 8);
	cmd_pload[13] = (copr_cmd.ROI_X_S & 0xFF);

	cmd_pload[14] = ((copr_cmd.ROI_Y_S & 0xF00) >> 8);
	cmd_pload[15] = (copr_cmd.ROI_Y_S & 0xFF);

	cmd_pload[16] = ((copr_cmd.ROI_X_E & 0x700) >> 8);
	cmd_pload[17] = (copr_cmd.ROI_X_E & 0xFF);

	cmd_pload[18] = ((copr_cmd.ROI_Y_E & 0xF00) >> 8);
	cmd_pload[19] = (copr_cmd.ROI_Y_E & 0xFF);

	for (i = 0; i <= 19; i++)
		len += snprintf(buf + len, sizeof(buf) - len,
						"%02x ", cmd_pload[i]);
	LCD_DEBUG("cmd : %s\n", buf);

	return;
}

/**
 * ss_copr_get_cmd - get copr cmd from panel dtsi (current copr cmd).
 */
int ss_copr_get_cmd(struct samsung_display_driver_data *vdd)
{
	struct dsi_panel_cmd_set *pcmds = NULL;
	struct COPR_CMD *cmd = &vdd->copr.cmd;
	u8 *cmd_pload;

	pcmds = ss_get_cmds(vdd, TX_COPR_ENABLE);
	if (!pcmds) {
		LCD_ERR("No COPR cmds..\n");
		return 1;
	}

	cmd_pload = pcmds->cmds[1].msg.tx_buf;

	cmd->COPR_EN = (cmd_pload[1] & 0x01) ? 1 : 0;
	cmd->COPR_GAMMA = (cmd_pload[1] & 0x02) ? 1 : 0;
	cmd->COPR_ILC = (cmd_pload[1] & 0x04) ? 1 : 0;
	cmd->CNT_RE = (cmd_pload[1] & 0x08) ? 1 : 0;

	cmd->COPR_ER = ((cmd_pload[2] & 0x30) << 4) | (cmd_pload[4] & 0xFF);
	cmd->COPR_EG = ((cmd_pload[2] & 0x0C) << 6) | (cmd_pload[5] & 0xFF);
	cmd->COPR_EB = ((cmd_pload[2] & 0x03) << 8) | (cmd_pload[6] & 0xFF);

	cmd->COPR_ERC = ((cmd_pload[3] & 0x30) << 4) | (cmd_pload[7] & 0xFF);
	cmd->COPR_EGC = ((cmd_pload[3] & 0x0C) << 6) | (cmd_pload[8] & 0xFF);
	cmd->COPR_EBC = ((cmd_pload[3] & 0x03) << 8) | (cmd_pload[9] & 0xFF);

	cmd->MAX_CNT = (cmd_pload[10] << 8) | cmd_pload[11];
	cmd->ROI_ON = (cmd_pload[12] & 0x08) ? 1 : 0;

	cmd->ROI_X_S = ((cmd_pload[12] & 0x07) << 8) | (cmd_pload[13] & 0xFF);
	cmd->ROI_Y_S = ((cmd_pload[14] & 0x0F) << 8) | (cmd_pload[15] & 0xFF);
	cmd->ROI_X_E = ((cmd_pload[16] & 0x07) << 8) | (cmd_pload[17] & 0xFF);
	cmd->ROI_Y_E = ((cmd_pload[18] & 0x0F) << 8) | (cmd_pload[19] & 0xFF);

	LCD_INFO("EN(%d) GAMMA(%d) ILC(%d) RE(%d)\n",
		cmd->COPR_EN, cmd->COPR_GAMMA, cmd->COPR_ILC, cmd->CNT_RE);
	LCD_INFO("ER(%d) EG(%d) EB(%d) ERC(%d) EGC(%d) EBC(%d)\n",
		cmd->COPR_ER, cmd->COPR_EG, cmd->COPR_EB,
		cmd->COPR_ERC, cmd->COPR_EGC, cmd->COPR_EBC);
	LCD_INFO("MAX_CNT(%d) ROI_EN(%d)\n", cmd->MAX_CNT, cmd->ROI_ON);
	LCD_INFO("X_S(%d) Y_S(%d) X_E(%d) Y_E(%d)\n",
		cmd->ROI_X_S, cmd->ROI_Y_S, cmd->ROI_X_E, cmd->ROI_Y_E);

	return 1;
}

/**
 * ss_copr_get_roi_opr - get opr values of all roi's r/g/b.
 *
 * Get copr snapshot for each roi's r/g/b.
 * This function returns 0 if the average is valid.
 * If not this function returns -ERR.
 */
int ss_copr_get_roi_opr(void)
{
	struct samsung_display_driver_data *vdd = samsung_get_vdd();
	int ret = 0;
	int i;
	struct COPR_CMD cmd_backup;

	LCD_DEBUG("++ (%d)\n", vdd->copr.roi_cnt);

	/* backup copr cmd */
	memcpy(&cmd_backup, &vdd->copr.cmd, sizeof(cmd_backup));

	ss_copr_set_en(1);
	ss_copr_set_cnt_re(0);
	ss_copr_set_max_cnt(1);
	ss_copr_set_gamma(1);

	/**
	 * roi[0] - top
	 * roi[1] - mid
	 * roi[2] - bottom
	 * roi[3] - all
	 */
	for (i = 0; i < vdd->copr.roi_cnt; i++) {

		ss_copr_set_roi(&vdd->copr.roi[i]);

		/* R / G --------------------------------------- */
		ss_copr_set_ilc(1);
		ss_copr_set_e(0x300, 0x0, 0x0);
		ss_copr_set_ec(0x0, 0x300, 0x0);

		ss_copr_set_cmd(vdd->copr.cmd);
		ss_send_cmd(vdd, TX_COPR_ENABLE);

		/* sleep 34ms (wait 2 frame : roi cmd write -> vsync -> image write -> opr read) */
		usleep_range(34000, 34000);

		ret = ss_copr_read(vdd);
		if (ret)
			goto err;

		vdd->copr.roi_opr[i].R_OPR = vdd->copr.current_copr;
		vdd->copr.roi_opr[i].G_OPR = vdd->copr.comp_copr;
		/* --------------------------------------------- */

		/* B ------------------------------------------- */
		ss_copr_set_ilc(0);
		ss_copr_set_e(0x0, 0x0, 0x300);
		ss_copr_set_ec(0x0, 0x0, 0x0);

		ss_copr_set_cmd(vdd->copr.cmd);
		ss_send_cmd(vdd, TX_COPR_ENABLE);

		/* sleep 34ms (wait 2 frame : roi cmd write -> vsync -> image write -> opr read) */
		usleep_range(34000, 34000);

		ret = ss_copr_read(vdd);
		if (ret)
			goto err;

		vdd->copr.roi_opr[i].B_OPR = vdd->copr.current_copr;
		/* --------------------------------------------- */

		LCD_INFO("R (%d) G (%d) B (%d)\n",
			vdd->copr.roi_opr[i].R_OPR,
			vdd->copr.roi_opr[i].G_OPR,
			vdd->copr.roi_opr[i].B_OPR);
	}

err:
	/* restore copr cmd */
	memcpy(&vdd->copr.cmd, &cmd_backup, sizeof(cmd_backup));
	ss_copr_set_cmd(vdd->copr.cmd);

	LCD_DEBUG("--\n");

	return ret;
}

void ss_set_copr_sum(struct samsung_display_driver_data *vdd, enum COPR_CD_INDEX idx)
{
	s64 delta;

	mutex_lock(&vdd->copr.copr_val_lock);
	vdd->copr.copr_cd[idx].last_t = vdd->copr.copr_cd[idx].cur_t;
	vdd->copr.copr_cd[idx].cur_t = ktime_get();
	delta = ktime_ms_delta(vdd->copr.copr_cd[idx].cur_t, vdd->copr.copr_cd[idx].last_t);
	vdd->copr.copr_cd[idx].total_t += delta;
	vdd->copr.copr_cd[idx].cd_sum += (vdd->interpolation_cd * delta);
	mutex_unlock(&vdd->copr.copr_val_lock);

	LCD_DEBUG("[%d ]cd(%d) delta (%lld) cd_sum (%lld) total_t (%lld)\n", idx,
			vdd->interpolation_cd, delta, vdd->copr.copr_cd[idx].cd_sum, vdd->copr.copr_cd[idx].total_t);
}

/**
 * ss_copr_reset_cnt - reset copr frame cnt using CNT_RE reg.
 */
void ss_copr_reset_cnt(struct samsung_display_driver_data *vdd)
{
	struct dsi_panel_cmd_set *pcmds;
	u8 *cmd_pload;

	pcmds = ss_get_cmds(vdd, TX_COPR_ENABLE);
	cmd_pload = pcmds->cmds[1].msg.tx_buf;

	/* CNT_RE = 1 */
	cmd_pload[1] = 0x9;
	ss_send_cmd(vdd, TX_COPR_ENABLE);

	/* sleep 20ms (wait te) */
	usleep_range(20000, 20000);

	/* CNT_RE = 0 */
	cmd_pload[1] = 0x3;
	ss_send_cmd(vdd, TX_COPR_ENABLE);

	return;
}

void ss_copr_parse_spi_data(struct samsung_display_driver_data *vdd, u8 *rxbuf)
{
	/* parse copr data (2.0) */
	vdd->copr.copr_ready = (rxbuf[0] & 0x80) >> 7;
	vdd->copr.current_cnt = ((rxbuf[0] & 0x7F) << 9) | (rxbuf[1] << 1) | (rxbuf[2] & 0x80 >> 7) ;
	vdd->copr.current_copr = ((rxbuf[2] & 0x7F) << 2) | ((rxbuf[3] & 0xC0) >> 6);
	vdd->copr.avg_copr = ((rxbuf[3] & 0x3F) << 3) | ((rxbuf[4] & 0xE0) >> 5);
	vdd->copr.sliding_current_cnt = ((rxbuf[4] & 0x1F) << 11) | (rxbuf[5] << 3) | ((rxbuf[6] & 0xE0) >> 5);
	vdd->copr.sliding_avg_copr = ((rxbuf[6] & 0x1F) << 4) | ((rxbuf[7] & 0xF0) >> 4);
	vdd->copr.comp_copr = ((rxbuf[8] & 0xF8) >> 3) | ((rxbuf[7] & 0x0F) << 5);
}

/**
 * ss_copr_read()
 *
 * read copr registers via SPI interface
 * This function returns zero on success, else a negative error code.
 */
int ss_copr_read(struct samsung_display_driver_data *vdd)
{
	u8 read_addr;
	u8 *rxbuf;
	int read_size;
	int ret = 0;
	int i;

	LCD_DEBUG("%s ++ \n", __func__);

	if (!ss_is_panel_on(vdd)) {
		LCD_ERR("panel stste (%d) \n", vdd->panel_state);
		return -ENODEV;
	}

	/* COPR read addr */
	read_addr = vdd->copr.read_addr;
	read_size = vdd->copr.read_size;

	rxbuf = kzalloc(read_size, GFP_KERNEL);
	if (rxbuf == NULL) {
		LCD_ERR("fail to kzalloc for rxbuf \n");
		ret = -ENOMEM;
		goto err;
	}

	ret = ss_spi_read(vdd->spi_dev, read_addr, rxbuf, read_size);
	if (ret) {
		LCD_ERR("[SDE SPI] %s : spi read fail..(%x)\n", __func__, read_addr);
		goto err;
	}

	for (i = 0; i < read_size; i++)
		LCD_DEBUG("%x \n", rxbuf[i]);

	ss_copr_parse_spi_data(vdd, rxbuf);

	LCD_DEBUG("current_copr (%d), avg_copr (%d) , comp_copr(%d)\n",
		vdd->copr.current_copr, vdd->copr.avg_copr, vdd->copr.comp_copr);

	/* If current_copr is over 0, copr work thread will be stopped */
	if (vdd->copr.current_copr && vdd->display_status_dsi.wait_actual_disp_on) {
		LCD_INFO("ACTUAL_DISPLAY_ON\n");
		vdd->display_status_dsi.wait_actual_disp_on = false;
	}

	LCD_DEBUG("%s -- data (%d)\n", __func__, vdd->copr.current_copr);

err:
	kfree(rxbuf);
	return ret;
}

/**
 * ss_read_copr_work()
 *
 * COPR 1.0 - Need to run work thread to get copr per frame.
 * COPR 2.0 - Do not need to run work thread to get copr avg.
 *            but for the debugging purpose, this work is used.
 */
static void ss_read_copr_work(struct work_struct *work)
{
	struct samsung_display_driver_data *vdd = NULL;
	struct COPR *copr;

	copr = container_of(work, struct COPR, read_copr_work);
	vdd = container_of(copr, struct samsung_display_driver_data, copr);

	LCD_DEBUG("copr_calc work!!\n");

	mutex_lock(&vdd->copr.copr_lock);

	//ss_set_copr_sum(vdd);
	ss_copr_read(vdd);

	LCD_DEBUG("COPR : %02x (%d) \n", vdd->copr.current_copr, vdd->copr.current_copr);

	mutex_unlock(&vdd->copr.copr_lock);

	return;
}

void ss_copr_init(void)
{
	struct samsung_display_driver_data *vdd = samsung_get_vdd();

	if (IS_ERR_OR_NULL(vdd)) {
		LCD_ERR("vdd is null or error\n");
		return;
	}

	LCD_INFO("++ \n");

	mutex_init(&vdd->copr.copr_val_lock);
	mutex_init(&vdd->copr.copr_lock);

	vdd->copr.read_copr_wq = create_singlethread_workqueue("read_copr_wq");
	if (vdd->copr.read_copr_wq == NULL) {
		LCD_ERR("failed to create read copr workqueue..\n");
		return;
	}

	INIT_WORK(&vdd->copr.read_copr_work, (work_func_t)ss_read_copr_work);

	vdd->copr.copr_on = 1;

	LCD_INFO("read_addr (%x) read_size (%d) on (%d)\n",
		vdd->copr.read_addr, vdd->copr.read_size, vdd->copr.copr_on);

	LCD_INFO("Success to initialized copr ..\n");

	return;
}
