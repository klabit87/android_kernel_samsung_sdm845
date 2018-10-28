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
<one line to give the program's name and a brief idea of what it does.>
Copyright (C) 2015, Samsung Electronics. All rights reserved.

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

static void ss_panel_recovery(struct samsung_display_driver_data *vdd);
static void ss_event_osc_te_fitting(
		struct samsung_display_driver_data *vdd, int event, void *arg);
static irqreturn_t samsung_te_check_handler(int irq, void *handle);
static void samsung_te_check_done_work(struct work_struct *work);
static void ss_event_esd_recovery_init(
		struct samsung_display_driver_data *vdd, int event, void *arg);
static void samsung_display_delay_disp_on_work(struct work_struct *work);
static void read_panel_data_work_fn(struct work_struct *work);

struct samsung_display_driver_data vdd_data[MAX_DISPLAY_NDX];

void __iomem *virt_mmss_gp_base;

LIST_HEAD(vdds_list);

char ss_panel_id0_get(struct samsung_display_driver_data *vdd)
{
	return (vdd->manufacture_id_dsi & 0xFF0000) >> 16;
}

char ss_panel_id1_get(struct samsung_display_driver_data *vdd)
{
	return (vdd->manufacture_id_dsi & 0xFF00) >> 8;
}

char ss_panel_id2_get(struct samsung_display_driver_data *vdd)
{
	return vdd->manufacture_id_dsi & 0xFF;
}

char ss_panel_rev_get(struct samsung_display_driver_data *vdd)
{
	return vdd->manufacture_id_dsi & 0x0F;
}

int ss_panel_attach_get(struct samsung_display_driver_data *vdd)
{
	return vdd->panel_attach_status;
}

int ss_panel_attach_set(struct samsung_display_driver_data *vdd, bool attach)
{
	/* 0bit->DSI0 1bit->DSI1 */
	/* check the lcd id for DISPLAY_1 and DISPLAY_2 */
	if (likely(ss_panel_attached(vdd->ndx) && attach))
		vdd->panel_attach_status = true;
	else
		vdd->panel_attach_status = false;

	LCD_INFO("panel_attach_status : %d\n", vdd->panel_attach_status);

	return vdd->panel_attach_status;
}

/*
 * Check the lcd id for DISPLAY_1 and DISPLAY_2 using the ndx
 */
int ss_panel_attached(int ndx)
{
	int lcd_id = 0;

	/*
	 * ndx 0 means DISPLAY_1 and ndx 1 means DISPLAY_2
	 */
	if (ndx == PRIMARY_DISPLAY_NDX)
		lcd_id = get_lcd_attached("GET");
	else if (ndx == SECONDARY_DISPLAY_NDX)
		lcd_id = get_lcd_attached_secondary("GET");

	/*
	 * The 0xFFFFFF is the id for PBA booting
	 * if the id is same with 0xFFFFFF, this function
	 * will return 0
	 */
	return !(lcd_id == PBA_ID);
}

static int ss_parse_panel_id(char *panel_id)
{
	char *pt;
	int lcd_id = 0;

	if (!IS_ERR_OR_NULL(panel_id))
		pt = panel_id;
	else
		return lcd_id;

	for (pt = panel_id; *pt != 0; pt++)  {
		lcd_id <<= 4;
		switch (*pt) {
		case '0' ... '9':
			lcd_id += *pt - '0';
			break;
		case 'a' ... 'f':
			lcd_id += 10 + *pt - 'a';
			break;
		case 'A' ... 'F':
			lcd_id += 10 + *pt - 'A';
			break;
		}
	}

	return lcd_id;
}

int get_lcd_attached(char *mode)
{
	static int lcd_id;

	LCD_DEBUG("%s", mode);

	if (mode == NULL)
		return true;

	if (!strncmp(mode, "GET", 3))
		goto end;
	else
		lcd_id = ss_parse_panel_id(mode);

	LCD_ERR("LCD_ID = 0x%X\n", lcd_id);
end:
	return lcd_id;
}
EXPORT_SYMBOL(get_lcd_attached);
__setup("lcd_id=0x", get_lcd_attached);

int get_lcd_attached_secondary(char *mode)
{
	static int lcd_id;

	LCD_DEBUG("%s", mode);

	if (mode == NULL)
		return true;

	if (!strncmp(mode, "GET", 3))
		goto end;
	else
		lcd_id = ss_parse_panel_id(mode);

	LCD_ERR("LCD_ID = 0x%X\n", lcd_id);
end:
	return lcd_id;
}
EXPORT_SYMBOL(get_lcd_attached_secondary);
__setup("lcd_id2=0x", get_lcd_attached_secondary);

static void ss_event_frame_update_pre(
		struct samsung_display_driver_data *vdd, void *arg)
{
	/* Block frame update during exclusive mode on.*/
	if (unlikely(vdd->exclusive_tx.enable)) {
		if (unlikely(!vdd->exclusive_tx.permit_frame_update)) {
			LCD_INFO("exclusive mode on, stop frame update\n");
			wait_event(vdd->exclusive_tx.ex_tx_waitq,
				!vdd->exclusive_tx.enable);
			LCD_INFO("continue frame update\n");
		}
	}
}

static void ss_event_frame_update(
		struct samsung_display_driver_data *vdd, int event, void *arg)
{
	struct panel_func *panel_func = NULL;
	static u8 frame_count = 1;

	panel_func = &vdd->panel_func;

	if (vdd->dtsi_data.samsung_osc_te_fitting &&
			!(vdd->te_fitting_info.status & TE_FITTING_DONE)) {
		if (panel_func->ss_event_osc_te_fitting)
			panel_func->ss_event_osc_te_fitting(vdd, event, arg);
	}

	if (vdd->display_status_dsi.wait_disp_on) {
		/* TODO: Add condition to check the susupend state
		 * insted of !msm_is_suspend_state(GET_DRM_DEV(vdd))
		 */

		/* Skip a number of frames to avoid garbage image output from wakeup */
		if (frame_count <= vdd->dtsi_data.samsung_delayed_display_on) {
			LCD_DEBUG("Skip %d frame\n", frame_count);
			frame_count++;
			goto skip_display_on;
		}
		frame_count = 1;

		ss_send_cmd(vdd, TX_DISPLAY_ON);
		vdd->display_status_dsi.wait_disp_on = false;

		if (vdd->panel_func.samsung_backlight_late_on)
			vdd->panel_func.samsung_backlight_late_on(vdd);

		if (vdd->dtsi_data.hmt_enabled &&
				ss_is_panel_on(vdd)) {
			if (vdd->hmt_stat.hmt_on) {
				LCD_INFO("hmt reset ..\n");
				vdd->hmt_stat.hmt_enable(vdd);
				vdd->hmt_stat.hmt_reverse_update(vdd, 1);
				vdd->hmt_stat.hmt_bright_update(vdd);
			}
		}

		if (ss_is_esd_check_enabled(vdd))
			vdd->esd_recovery.is_enabled_esd_recovery = true;

		/* For read flash gamma data before first brightness set */
		if (vdd->dtsi_data.flash_gamma_support && !vdd->panel_br_info.flash_data.init_done) {
			if (!work_busy(&vdd->flash_br_work.work)) {
				queue_delayed_work(vdd->flash_br_workqueue, &vdd->flash_br_work, msecs_to_jiffies(0));
			}
		}

		LCD_INFO("DISPLAY_ON\n");
	}

	/* copr - check actual display on (frame - copr > 0) debug */
	/* work thread will be stopped if copr is over 0 */
	if (vdd->display_status_dsi.wait_actual_disp_on && ss_is_panel_on(vdd)) {
		if (vdd->copr.read_copr_wq && vdd->copr.copr_on)
			queue_work(vdd->copr.read_copr_wq, &vdd->copr.read_copr_work);
	}
skip_display_on:
	return;
}

static void ss_send_esd_recovery_cmd(struct samsung_display_driver_data *vdd)
{
	static bool toggle;

	if (toggle)
		ss_send_cmd(vdd, TX_ESD_RECOVERY_1);
	else
		ss_send_cmd(vdd, TX_ESD_RECOVERY_2);
	toggle = !toggle;
}

static void ss_check_te(struct samsung_display_driver_data *vdd)
{
	unsigned int disp_te_gpio;
	int rc, te_count = 0;
	int te_max = 20000; /*sampling 200ms */

	disp_te_gpio = ss_get_te_gpio(vdd);

	LCD_INFO("============ start waiting for TE ============\n");
	if (gpio_is_valid(disp_te_gpio)) {
		for (te_count = 0 ; te_count < te_max ; te_count++) {
			rc = gpio_get_value(disp_te_gpio);
			if (rc == 1) {
				LCD_INFO("LDI generate TE within = %d.%dms\n", te_count/100, te_count%100);
				break;
			}
			/* usleep suspends the calling thread whereas udelay is a
			 * busy wait. Here the value of te_gpio is checked in a loop of
			 * max count = 250. If this loop has to iterate multiple
			 * times before the te_gpio is 1, the calling thread will end
			 * up in suspend/wakeup sequence multiple times if usleep is
			 * used, which is an overhead. So use udelay instead of usleep.
			 */
			udelay(10);
		}
		if (te_count == te_max) {
			LCD_ERR("LDI doesn't generate TE");
			SS_XLOG(0xbad);
			inc_dpui_u32_field(DPUI_KEY_QCT_NO_TE, 1);
		}
	} else
		LCD_ERR("disp_te_gpio is not valid\n");
	LCD_INFO("============ end waiting for TE ============\n");
}

static void ss_event_fb_event_callback(struct samsung_display_driver_data *vdd, int event, void *arg)
{
	struct panel_func *panel_func = NULL;

	panel_func = &vdd->panel_func;

	if (IS_ERR_OR_NULL(panel_func)) {
		LCD_ERR("Invalid data panel_func : 0x%zx\n",
				(size_t)panel_func);
		return;
	}

	if (panel_func->ss_event_esd_recovery_init)
		panel_func->ss_event_esd_recovery_init(vdd, event, arg);
}

static int _ss_dsi_panel_event_handler(
		struct samsung_display_driver_data *vdd,
		enum mdss_intf_events event, void *arg)
{
	struct panel_func *panel_func = NULL;

	panel_func = &vdd->panel_func;

	if (IS_ERR_OR_NULL(panel_func)) {
		LCD_ERR("Invalid data panel_func : 0x%zx\n", (size_t)panel_func);
		return -EINVAL;
	}

	switch (event) {
	case SS_EVENT_FRAME_UPDATE_POST:
		if (!IS_ERR_OR_NULL(panel_func->ss_event_frame_update))
			panel_func->ss_event_frame_update(vdd, event, arg);
		break;
	case SS_EVENT_FRAME_UPDATE_PRE:
		ss_event_frame_update_pre(vdd, arg);
		break;
	case SS_EVENT_FB_EVENT_CALLBACK:
		if (!IS_ERR_OR_NULL(panel_func->ss_event_fb_event_callback))
			panel_func->ss_event_fb_event_callback(vdd, event, arg);
		break;
	case SS_EVENT_PANEL_ON:
		if (likely(!vdd->is_factory_mode))
			ss_panel_lpm_ctrl(vdd, false);
		break;
	case SS_EVENT_PANEL_OFF:
		if (likely(!vdd->is_factory_mode))
			ss_panel_lpm_ctrl(vdd, true);
		break;
	case SS_EVENT_PANEL_RECOVERY:
		ss_panel_recovery(vdd);
		break;
	case SS_EVENT_PANEL_ESD_RECOVERY:
		if (vdd->send_esd_recovery)
			ss_send_esd_recovery_cmd(vdd);
		break;
	case SS_EVENT_CHECK_TE:
			ss_check_te(vdd);
		break;
	case SS_EVENT_SDE_HW_CATALOG_INIT:
		if (unlikely(vdd->panel_func.samsung_pba_config))
			vdd->panel_func.samsung_pba_config(vdd, arg);
		break;
	default:
		LCD_DEBUG("unhandled event=%d\n", event);
		break;
	}

	return 0;
}

int ss_dsi_panel_event_handler(
		struct drm_device *dev, enum mdss_intf_events event, void *arg)
{

#if 0
	struct msm_drm_private *priv = dev->dev_private;
	struct drm_connector *connector;
	struct sde_connector *c_conn;
	struct dsi_display *display;
	struct samsung_display_driver_data *vdd;
	int i;

	if (event >= SS_EVENT_MAX) {
		LCD_ERR("Unknown event(%d)\n", event);
		return -EINVAL;
	}

	// refer to msm_atomic_helper_commit_modeset_enables() or ...
	for (i = 0; i < priv->num_connectors; i++) {
		connector = priv->connectors[i];
		c_conn = to_sde_connector(connector);
		display = c_conn->display;

		if (!display || !display->panel ||
				!display->panel->panel_private)
			continue;

		vdd = display->panel->panel_private;
		_ss_dsi_panel_event_handler(vdd, event, arg);
	}
#else
	struct samsung_display_driver_data *vdd;

	vdd = &vdd_data[PRIMARY_DISPLAY_NDX];
	_ss_dsi_panel_event_handler(vdd, event, arg);
#endif

	return 0;
}

/* CP notity format (HEX raw format)
 * 10 00 AA BB 27 01 03 XX YY YY YY YY ZZ ZZ ZZ ZZ
 *
 * 00 10 (0x0010) - len
 * AA BB - not used
 * 27 - MAIN CMD (SYSTEM CMD : 0x27)
 * 01 - SUB CMD (CP Channel Info : 0x01)
 * 03 - NOTI CMD (0x03)
 * XX - RAT MODE
 * YY YY YY YY - BAND MODE
 * ZZ ZZ ZZ ZZ - FREQ INFO
 */

int ss_rf_info_notify_callback(struct notifier_block *nb,
				unsigned long size, void *data)
{
	struct rf_noti_info *info;
	struct samsung_display_driver_data *vdd = samsung_get_vdd();

	if (IS_ERR_OR_NULL(vdd)) {
		LCD_ERR("vdd is null\n");
		return -ENODEV;
	}
	LCD_INFO("RF Info Notifiy Callback, Size=%lu\n", size);

	if (size == sizeof(struct rf_noti_info)) {
		info = (struct rf_noti_info *)data;

		if (info->main_cmd == 0x27 && info->sub_cmd == 0x01 &&
			info->cmd_type == 0x03) {
			mutex_lock(&vdd->vdd_dyn_mipi_lock);
			vdd->rf_info.rat = info->rat;
			vdd->rf_info.band = info->band;
			vdd->rf_info.arfcn = info->arfcn;
			mutex_unlock(&vdd->vdd_dyn_mipi_lock);
		}
	}

	LCD_INFO("RF Info Notify RAT(%d), BAND(%d), ARFCN(%d)\n",
		vdd->rf_info.rat, vdd->rf_info.band, vdd->rf_info.arfcn);

	return 0;
}

static int ss_find_dyn_mipi_clk_timing_idx(struct samsung_display_driver_data *vdd)
{
	int idx = 0;
	int loop;
	int rat, band, arfcn;
	struct clk_sel_table sel_table = vdd->dyn_mipi_clk.clk_sel_table;

	if (!sel_table.tab_size) {
		LCD_ERR("Table is NULL");
		return 0;
	}

	rat = vdd->rf_info.rat;
	band = vdd->rf_info.band;
	arfcn = vdd->rf_info.arfcn;

	for (loop = 0 ; loop < sel_table.tab_size ; loop++) {
		if ((rat == sel_table.rat[loop]) && (band == sel_table.band[loop])) {
			if ((arfcn >= sel_table.from[loop]) && (arfcn <= sel_table.end[loop])) {
				idx = sel_table.target_clk_idx[loop];
				break;
			}
		}
	}

	LCD_INFO("RAT(%d), BAND(%d), ARFCN(%d), Clock Index(%d)\n",
		rat, band, arfcn, idx);

	return idx;

}

int ss_change_dyn_mipi_clk_timing(struct samsung_display_driver_data *vdd)
{
	int ret = 0;
	int idx = 0;
	size_t loop;
	u8 *ffc_pload = NULL;
	u8 *ffc_set_pload = NULL;
	struct dsi_panel_cmd_set *ffc_cmds = NULL;
	struct dsi_panel_cmd_set *ffc_set_cmds = NULL;
	struct clk_timing_table timing_table = vdd->dyn_mipi_clk.clk_timing_table;
	struct dsi_display *dsi_disp = GET_DSI_DISPLAY(vdd);
	struct dsi_mode_info *timing = &dsi_disp->config.video_timing;

	if (!vdd->dyn_mipi_clk.is_support)
		LCD_DEBUG("Dynamic MIPI Clock does not support\n");

	if (!timing_table.tab_size) {
		LCD_ERR("Table is NULL");
		return -ENODEV;
	}

	mutex_lock(&vdd->vdd_dyn_mipi_lock);

	idx = ss_find_dyn_mipi_clk_timing_idx(vdd);

	LCD_DEBUG("Timing IDX = %d\n", idx);

	if (!idx)
		LCD_ERR("Failed to find MIPI clock timing, idx=0\n");

	/* Timing Change */
	timing->refresh_rate = timing_table.fps[idx];
	timing->h_front_porch = timing_table.hfp[idx];
	timing->h_back_porch = timing_table.hbp[idx];
	timing->h_sync_width = timing_table.hsw[idx];
	timing->h_skew = timing_table.hss[idx];
	timing->v_front_porch = timing_table.vfp[idx];
	timing->v_back_porch = timing_table.vbp[idx];
	timing->v_sync_width = timing_table.vsw[idx];

	/* FFC Command Change */
	ffc_cmds = ss_get_cmds(vdd, TX_FFC);
	ffc_set_cmds = ss_get_cmds(vdd, TX_DYNAMIC_FFC_SET);

	ffc_pload = ffc_cmds->cmds[1].msg.tx_buf;
	ffc_set_pload = ffc_set_cmds->cmds[idx].msg.tx_buf;

	for (loop = 0; loop < ffc_cmds->cmds[1].msg.tx_len ; loop++)
		ffc_pload[loop] = ffc_set_pload[loop];

	mutex_unlock(&vdd->vdd_dyn_mipi_lock);

	LCD_INFO("FPS(%d), HFP(%d), HBP(%d), HSW(%d), HSKEW(%d), VFP(%d), VBP(%d), VSW(%d)\n",
		timing->refresh_rate, timing->h_front_porch,
		timing->h_back_porch, timing->h_sync_width,
		timing->h_skew, timing->v_front_porch,
		timing->v_back_porch, timing->v_sync_width);

	return ret;
}

int ss_parse_dyn_mipi_clk_timing_table(struct device_node *np,
		void *tbl, char *keystring)
{
	struct clk_timing_table *table = (struct clk_timing_table *) tbl;
	const __be32 *data;
	int len = 0, i = 0, data_offset = 0;
	int col_size = 0;

	data = of_get_property(np, keystring, &len);

	if (data)
		LCD_INFO("Success to read table %s\n", keystring);
	else {
		LCD_ERR("%d, Unable to read table %s ", __LINE__, keystring);
		return -EINVAL;
	}

	col_size = 8;

	if ((len % col_size) != 0) {
		LCD_ERR("%d, Incorrect table entries for %s , len : %d",
					__LINE__, keystring, len);
		return -EINVAL;
	}

	table->tab_size = len / (sizeof(int) * col_size);

	table->fps = kzalloc((sizeof(int) * table->tab_size), GFP_KERNEL);
	if (!table->fps)
		return -ENOMEM;
	table->hfp = kzalloc((sizeof(int) * table->tab_size), GFP_KERNEL);
	if (!table->hfp)
		goto error;
	table->hbp = kzalloc((sizeof(int) * table->tab_size), GFP_KERNEL);
	if (!table->hbp)
		goto error;
	table->hsw = kzalloc((sizeof(int) * table->tab_size), GFP_KERNEL);
	if (!table->hsw)
		goto error;
	table->hss = kzalloc((sizeof(int) * table->tab_size), GFP_KERNEL);
	if (!table->hss)
		goto error;
	table->vfp = kzalloc((sizeof(int) * table->tab_size), GFP_KERNEL);
	if (!table->vfp)
		goto error;
	table->vbp = kzalloc((sizeof(int) * table->tab_size), GFP_KERNEL);
	if (!table->vbp)
		goto error;
	table->vsw = kzalloc((sizeof(int) * table->tab_size), GFP_KERNEL);
	if (!table->vsw)
		goto error;

	for (i = 0 ; i < table->tab_size; i++) {
		table->fps[i] = be32_to_cpup(&data[data_offset++]);
		table->hfp[i] = be32_to_cpup(&data[data_offset++]);
		table->hbp[i] = be32_to_cpup(&data[data_offset++]);
		table->hsw[i] = be32_to_cpup(&data[data_offset++]);
		table->hss[i] = be32_to_cpup(&data[data_offset++]);
		table->vfp[i] = be32_to_cpup(&data[data_offset++]);
		table->vbp[i] = be32_to_cpup(&data[data_offset++]);
		table->vsw[i] = be32_to_cpup(&data[data_offset++]);
	}

	LCD_INFO("%s tab_size (%d)\n", keystring, table->tab_size);

	return 0;

error:
	LCD_ERR("Allocation Fail\n");
	kfree(table->fps);
	kfree(table->hfp);
	kfree(table->hbp);
	kfree(table->hsw);
	kfree(table->hss);
	kfree(table->vfp);
	kfree(table->vbp);
	kfree(table->vsw);

	return -ENOMEM;
}

int ss_parse_dyn_mipi_clk_sel_table(struct device_node *np,
		void *tbl, char *keystring)
{
	struct clk_sel_table *table = (struct clk_sel_table *) tbl;
	const __be32 *data;
	int len = 0, i = 0, data_offset = 0;
	int col_size = 0;

	data = of_get_property(np, keystring, &len);

	if (data)
		LCD_INFO("Success to read table %s\n", keystring);
	else {
		LCD_ERR("%d, Unable to read table %s ", __LINE__, keystring);
		return -EINVAL;
	}

	col_size = 5;

	if ((len % col_size) != 0) {
		LCD_ERR("%d, Incorrect table entries for %s , len : %d",
					__LINE__, keystring, len);
		return -EINVAL;
	}

	table->tab_size = len / (sizeof(int) * col_size);

	table->rat = kzalloc((sizeof(int) * table->tab_size), GFP_KERNEL);
	if (!table->rat)
		return -ENOMEM;
	table->band = kzalloc((sizeof(int) * table->tab_size), GFP_KERNEL);
	if (!table->band)
		goto error;
	table->from = kzalloc((sizeof(int) * table->tab_size), GFP_KERNEL);
	if (!table->from)
		goto error;
	table->end = kzalloc((sizeof(int) * table->tab_size), GFP_KERNEL);
	if (!table->end)
		goto error;
	table->target_clk_idx = kzalloc((sizeof(int) * table->tab_size), GFP_KERNEL);
	if (!table->target_clk_idx)
		goto error;

	for (i = 0 ; i < table->tab_size; i++) {
		table->rat[i] = be32_to_cpup(&data[data_offset++]);
		table->band[i] = be32_to_cpup(&data[data_offset++]);
		table->from[i] = be32_to_cpup(&data[data_offset++]);
		table->end[i] = be32_to_cpup(&data[data_offset++]);
		table->target_clk_idx[i] = be32_to_cpup(&data[data_offset++]);
		LCD_DEBUG("%dst : %d %d %d %d %d\n",
			i, table->rat[i], table->band[i], table->from[i],
			table->end[i], table->target_clk_idx[i]);
	}

	LCD_INFO("%s tab_size (%d)\n", keystring, table->tab_size);

	return 0;

error:
	LCD_ERR("Allocation Fail\n");
	kfree(table->rat);
	kfree(table->band);
	kfree(table->from);
	kfree(table->end);
	kfree(table->target_clk_idx);

	return -ENOMEM;
}

int ss_parse_candella_mapping_table(struct device_node *np,
		void *tbl, char *keystring)
{
	struct candela_map_table *table = (struct candela_map_table *) tbl;
	const __be32 *data;
	int len = 0, i = 0, data_offset = 0;
	int col_size = 0;

	data = of_get_property(np, keystring, &len);
	if (!data) {
		LCD_DEBUG("%d, Unable to read table %s ", __LINE__, keystring);
		return -EINVAL;
	} else
		LCD_ERR("Success to read table %s\n", keystring);

	col_size = 4;

	if ((len % col_size) != 0) {
		LCD_ERR("%d, Incorrect table entries for %s , len : %d",
					__LINE__, keystring, len);
		return -EINVAL;
	}

	table->tab_size = len / (sizeof(int) * col_size);

	table->cd = kzalloc((sizeof(int) * table->tab_size), GFP_KERNEL);
	if (!table->cd)
		goto error;
	table->idx = kzalloc((sizeof(int) * table->tab_size), GFP_KERNEL);
	if (!table->idx)
		goto error;
	table->from = kzalloc((sizeof(int) * table->tab_size), GFP_KERNEL);
	if (!table->from)
		goto error;
	table->end = kzalloc((sizeof(int) * table->tab_size), GFP_KERNEL);
	if (!table->end)
		goto error;

	for (i = 0 ; i < table->tab_size; i++) {
		table->idx[i] = be32_to_cpup(&data[data_offset++]);		/* field => <idx> */
		table->from[i] = be32_to_cpup(&data[data_offset++]);	/* field => <from> */
		table->end[i] = be32_to_cpup(&data[data_offset++]);		/* field => <end> */
		table->cd[i] = be32_to_cpup(&data[data_offset++]);		/* field => <cd> */
	}

	table->min_lv = table->from[0];
	table->max_lv = table->end[table->tab_size-1];

	LCD_INFO("tab_size (%d), hbm min/max lv (%d/%d)\n", table->tab_size, table->min_lv, table->max_lv);

	return 0;

error:
	kfree(table->cd);
	kfree(table->idx);
	kfree(table->from);
	kfree(table->end);

	return -ENOMEM;
}

int ss_parse_pac_candella_mapping_table(struct device_node *np,
		void *tbl, char *keystring)
{
	struct candela_map_table *table = (struct candela_map_table *) tbl;
	const __be32 *data;
	int len = 0, i = 0, data_offset = 0;
	int col_size = 0;

	data = of_get_property(np, keystring, &len);
	if (!data) {
		LCD_DEBUG("%d, Unable to read table %s ", __LINE__, keystring);
		return -EINVAL;
	} else
		LCD_ERR("Success to read table %s\n", keystring);

	col_size = 6;

	if ((len % col_size) != 0) {
		LCD_ERR("%d, Incorrect table entries for %s , len : %d",
					__LINE__, keystring, len);
		return -EINVAL;
	}

	table->tab_size = len / (sizeof(int) * col_size);

	table->interpolation_cd = kzalloc((sizeof(int) * table->tab_size), GFP_KERNEL);
	if (!table->interpolation_cd)
		return -ENOMEM;
	table->cd = kzalloc((sizeof(int) * table->tab_size), GFP_KERNEL);
	if (!table->cd)
		goto error;
	table->scaled_idx = kzalloc((sizeof(int) * table->tab_size), GFP_KERNEL);
	if (!table->scaled_idx)
		goto error;
	table->idx = kzalloc((sizeof(int) * table->tab_size), GFP_KERNEL);
	if (!table->idx)
		goto error;
	table->from = kzalloc((sizeof(int) * table->tab_size), GFP_KERNEL);
	if (!table->from)
		goto error;
	table->end = kzalloc((sizeof(int) * table->tab_size), GFP_KERNEL);
	if (!table->end)
		goto error;

	for (i = 0 ; i < table->tab_size; i++) {
		table->scaled_idx[i] = be32_to_cpup(&data[data_offset++]);	/* field => <scaeled idx> */
		table->idx[i] = be32_to_cpup(&data[data_offset++]);			/* field => <idx> */
		table->from[i] = be32_to_cpup(&data[data_offset++]);		/* field => <from> */
		table->end[i] = be32_to_cpup(&data[data_offset++]);			/* field => <end> */
		table->cd[i] = be32_to_cpup(&data[data_offset++]);			/* field => <cd> */
		table->interpolation_cd[i] = be32_to_cpup(&data[data_offset++]);	/* field => <interpolation cd> */
	}

	table->min_lv = table->from[0];
	table->max_lv = table->end[table->tab_size-1];

	LCD_INFO("tab_size (%d), hbm min/max lv (%d/%d)\n", table->tab_size, table->min_lv, table->max_lv);

	return 0;

error:
	kfree(table->interpolation_cd);
	kfree(table->cd);
	kfree(table->scaled_idx);
	kfree(table->idx);
	kfree(table->from);
	kfree(table->end);

	return -ENOMEM;
}

int ss_parse_hbm_candella_mapping_table(struct device_node *np,
		void *tbl, char *keystring)
{
	struct hbm_candela_map_table *table = (struct hbm_candela_map_table *) tbl;
	const __be32 *data;
	int data_offset = 0, len = 0, i = 0;

	data = of_get_property(np, keystring, &len);
	if (!data) {
		LCD_DEBUG("%d, Unable to read table %s ", __LINE__, keystring);
		return -EINVAL;
	} else
		LCD_ERR("Success to read table %s\n", keystring);

	if ((len % 4) != 0) {
		LCD_ERR("%d, Incorrect table entries for %s",
					__LINE__, keystring);
		return -EINVAL;
	}

	table->tab_size = len / (sizeof(int)*5);

	table->cd = kzalloc((sizeof(int) * table->tab_size), GFP_KERNEL);
	if (!table->cd)
		return -ENOMEM;
	table->idx = kzalloc((sizeof(int) * table->tab_size), GFP_KERNEL);
	if (!table->idx)
		goto error;
	table->from = kzalloc((sizeof(int) * table->tab_size), GFP_KERNEL);
	if (!table->from)
		goto error;
	table->end = kzalloc((sizeof(int) * table->tab_size), GFP_KERNEL);
	if (!table->end)
		goto error;
	table->auto_level = kzalloc((sizeof(int) * table->tab_size), GFP_KERNEL);
	if (!table->auto_level)
		goto error;

	for (i = 0 ; i < table->tab_size; i++) {
		table->idx[i] = be32_to_cpup(&data[data_offset++]);		/* 1st field => <idx> */
		table->from[i] = be32_to_cpup(&data[data_offset++]);		/* 2nd field => <from> */
		table->end[i] = be32_to_cpup(&data[data_offset++]);			/* 3rd field => <end> */
		table->cd[i] = be32_to_cpup(&data[data_offset++]);		/* 4th field => <candella> */
		table->auto_level[i] = be32_to_cpup(&data[data_offset++]);  /* 5th field => <auto brightness level> */
	}

	table->hbm_min_lv = table->from[0];
	table->hbm_max_lv = table->end[table->tab_size-1];

	LCD_INFO("tab_size (%d), hbm min/max lv (%d/%d)\n", table->tab_size, table->hbm_min_lv, table->hbm_max_lv);

	return 0;
error:
	kfree(table->cd);
	kfree(table->idx);
	kfree(table->from);
	kfree(table->end);
	kfree(table->auto_level);

	return -ENOMEM;
}

int ss_parse_panel_table(struct device_node *np,
		void *tbl, char *keystring)
{
	struct cmd_map *table = (struct cmd_map *) tbl;
	const __be32 *data;
	int  data_offset, len = 0, i = 0;

	data = of_get_property(np, keystring, &len);
	if (!data) {
		LCD_DEBUG("%d, Unable to read table %s\n", __LINE__, keystring);
		return -EINVAL;
	} else
		LCD_ERR("Success to read table %s\n", keystring);

	if ((len % 2) != 0) {
		LCD_ERR("%d, Incorrect table entries for %s",
					__LINE__, keystring);
		return -EINVAL;
	}

	table->size = len / (sizeof(int)*2);
	table->bl_level = kzalloc((sizeof(int) * table->size), GFP_KERNEL);
	if (!table->bl_level)
		return -ENOMEM;

	table->cmd_idx = kzalloc((sizeof(int) * table->size), GFP_KERNEL);
	if (!table->cmd_idx)
		goto error;

	data_offset = 0;
	for (i = 0 ; i < table->size; i++) {
		table->bl_level[i] = be32_to_cpup(&data[data_offset++]);
		table->cmd_idx[i] = be32_to_cpup(&data[data_offset++]);
	}

	return 0;
error:
	kfree(table->cmd_idx);

	return -ENOMEM;
}


int ss_send_cmd(struct samsung_display_driver_data *vdd,
		enum dsi_cmd_set_type type)
{
	struct dsi_panel *panel = GET_DSI_PANEL(vdd);
	struct dsi_display *dsi_display = GET_DSI_DISPLAY(vdd);
	struct dsi_panel_cmd_set *set;
	bool is_vdd_locked = false;
	int rc = 0;

	if (IS_ERR_OR_NULL(vdd)) {
		LCD_ERR("vdd is null\n");
		return -ENODEV;
	}

	if (!ss_panel_attach_get(vdd)) {
		LCD_ERR("ss_panel_attach_get(%d) : %d\n",
				vdd->ndx, ss_panel_attach_get(vdd));
		return -EAGAIN;
	}

	/* Skip to lock vdd_lock for commands that has exclusive_pass token
	 * to prevent deadlock between vdd_lock and ex_tx_waitq.
	 * cmd_lock guarantees exclusive tx cmds without vdd_lock.
	 */
	set = ss_get_cmds(vdd, type);
	if (likely(!vdd->exclusive_tx.enable || !set->exclusive_pass)) {
		mutex_lock(&vdd->vdd_lock);
		is_vdd_locked = true;
	}

	if (ss_is_panel_off(vdd) || (vdd->panel_func.samsung_lvds_write_reg)) {
		LCD_ERR("skip to tx cmd (%d), panel off (%d)\n",
				type, ss_is_panel_off(vdd));
		goto error;
	}

	if (vdd->debug_data->print_cmds)
		LCD_INFO("Send cmd(%d): %s ++\n", type, ss_get_cmd_name(type));
	else
		LCD_DEBUG("Send cmd(%d): %s ++\n", type, ss_get_cmd_name(type));

	/* case 03063186:
	 * To prevent deadlock between phandle->phandle_lock and panel->panel_lock,
	 * dsi_display_clk_ctrl() should be called without locking panel_lock.
	 */
	rc = dsi_display_clk_ctrl(dsi_display->dsi_clk_handle,
			DSI_ALL_CLKS, DSI_CLK_ON);
	if (rc) {
		pr_err("[%s] failed to enable DSI core clocks, rc=%d\n",
				dsi_display->name, rc);
		goto error;
	}

	dsi_panel_tx_cmd_set(panel, type);

	rc = dsi_display_clk_ctrl(dsi_display->dsi_clk_handle,
			DSI_ALL_CLKS, DSI_CLK_OFF);
	if (rc) {
		pr_err("[%s] failed to disable DSI core clocks, rc=%d\n",
				dsi_display->name, rc);
		goto error;
	}

	if (vdd->debug_data->print_cmds)
		LCD_INFO("Send cmd(%d): %s --\n", type, ss_get_cmd_name(type));
	else
		LCD_DEBUG("Send cmd(%d): %s --\n", type, ss_get_cmd_name(type));

error:
	if (likely(is_vdd_locked))
		mutex_unlock(&vdd->vdd_lock);

	return rc;
}

#include <linux/cpufreq.h>
#define CPUFREQ_MAX	100
#define CPUFREQ_ALT_HIGH	70
struct cluster_cpu_info {
	int cpu_man; /* cpu number that manages the policy */
	int max_freq;
};

static void __ss_set_cpufreq_cpu(struct samsung_display_driver_data *vdd,
				int enable, int cpu, int max_percent)
{
	struct cpufreq_policy *policy;
	static unsigned int org_min[NR_CPUS + 1];
	static unsigned int org_max[NR_CPUS + 1];

	policy = cpufreq_cpu_get(cpu);
	if (policy == NULL) {
		LCD_ERR("policy is null..\n");
		return;
	}

	if (enable) {
		org_min[cpu] = policy->min;
		org_max[cpu] = policy->max;

		/* max_freq's unit is kHz, and it should not cause overflow.
		 * After applying user_policy, cpufreq driver will find closest
		 * frequency in freq_table, and set the frequency.
		 */
		policy->user_policy.min = policy->user_policy.max =
			policy->cpuinfo.max_freq * max_percent / 100;
	} else {
		policy->user_policy.min = org_min[cpu];
		policy->user_policy.max = org_max[cpu];
	}

	cpufreq_update_policy(cpu);
	cpufreq_cpu_put(policy);

	LCD_INFO("en=%d, cpu=%d, per=%d, min=%d, org=(%d-%d)\n",
			enable, cpu, max_percent, policy->user_policy.min,
			org_min[cpu], org_max[cpu]);
}

void ss_set_max_cpufreq(struct samsung_display_driver_data *vdd,
				int enable, enum mdss_cpufreq_cluster cluster)
{
	struct cpufreq_policy *policy;
	int cpu;
	struct cluster_cpu_info cluster_info[NR_CPUS];
	int cnt_cluster;
	int big_cpu_man;
	int little_cpu_man;

	LCD_INFO("en=%d, cluster=%d\n", enable, cluster);
	get_online_cpus();

	/* find clusters */
	cnt_cluster = 0;
	for_each_online_cpu(cpu) {
		policy = cpufreq_cpu_get(cpu);
		/* In big-little octa core system,
		 * cpu0 ~ cpu3 has same policy (policy0) and
		 * policy->cpu would be 0. (cpu0 manages policy0)
		 * cpu4 ~ cpu7 has same policy (policy4) and
		 * policy->cpu would be 4. (cpu4 manages policy0)
		 * In this case, cnt_cluster would be two, and
		 * cluster_info[0].cpu_man = 0, cluster_info[1].cpu_man = 4.
		 */
		if (policy && policy->cpu == cpu) {
			cluster_info[cnt_cluster].cpu_man = cpu;
			cluster_info[cnt_cluster].max_freq =
				policy->cpuinfo.max_freq;
			cnt_cluster++;
		}
		cpufreq_cpu_put(policy);
	}

	if (!cnt_cluster || cnt_cluster > 2) {
		LCD_ERR("cluster count err (%d)\n", cnt_cluster);
		goto end;
	}

	if (cnt_cluster == 1) {
		/* single policy (none big-little case) */
		LCD_INFO("cluster count is one...\n");

		if (cluster == CPUFREQ_CLUSTER_ALL)
			/* set all cores' freq to max*/
			__ss_set_cpufreq_cpu(vdd, enable,
					cluster_info[0].cpu_man, CPUFREQ_MAX);
		else
			/* set all cores' freq to max * 70 percent */
			__ss_set_cpufreq_cpu(vdd, enable,
				cluster_info[0].cpu_man, CPUFREQ_ALT_HIGH);
		goto end;
	}

	/* big-little */
	if (cluster_info[0].max_freq > cluster_info[1].max_freq) {
		big_cpu_man = cluster_info[0].cpu_man;
		little_cpu_man = cluster_info[1].cpu_man;
	} else {
		big_cpu_man = cluster_info[1].cpu_man;
		little_cpu_man = cluster_info[0].cpu_man;
	}

	if (cluster == CPUFREQ_CLUSTER_BIG) {
		__ss_set_cpufreq_cpu(vdd, enable, big_cpu_man,
				CPUFREQ_MAX);
	} else if (cluster == CPUFREQ_CLUSTER_LITTLE) {
		__ss_set_cpufreq_cpu(vdd, enable, little_cpu_man,
				CPUFREQ_MAX);
	} else if  (cluster == CPUFREQ_CLUSTER_ALL) {
		__ss_set_cpufreq_cpu(vdd, enable, big_cpu_man,
				CPUFREQ_MAX);
		__ss_set_cpufreq_cpu(vdd, enable, little_cpu_man,
				CPUFREQ_MAX);
	}

end:
	put_online_cpus();
}

/* ss_set_exclusive_packet():
 * This shuold be called in ex_tx_lock lock.
 */
void ss_set_exclusive_tx_packet(
		struct samsung_display_driver_data *vdd,
		enum dsi_cmd_set_type cmd, int pass)
{
	struct dsi_panel_cmd_set *pcmds = NULL;

	pcmds = ss_get_cmds(vdd, cmd);
	pcmds->exclusive_pass = pass;
}

/*
 * ex_tx_lock should be locked before panel_lock if there is a dsi_panel_tx_cmd_set after panel_lock is locked.
 * because of there is a "ex_tx_lock -> panel lock" routine at the sequence of ss_gct_write.
 * So we need to add ex_tx_lock to protect all "dsi_panel_tx_cmd_set after panel_lock".
 */

void ss_set_exclusive_tx_lock_from_qct(struct samsung_display_driver_data *vdd, bool lock)
{
	if (lock)
		mutex_lock(&vdd->exclusive_tx.ex_tx_lock);
	else
		mutex_unlock(&vdd->exclusive_tx.ex_tx_lock);
}

#define _ALIGN_DOWN(addr, size)  ((addr)&(~((size)-1)))
//#define MAX_DSI_FIFO_SIZE_HS_MODE	(480) /* AP limitation */
/* TODO: find MAX_DSI_FIFO_SIZE_HS_MODE	for sdm845... if set to 480, cause fail of tx mipi */
#define MAX_DSI_FIFO_SIZE_HS_MODE	(50) /* AP limitation */
#define MAX_SIZE_IMG_PAYLOAD	_ALIGN_DOWN((MAX_DSI_FIFO_SIZE_HS_MODE - 1), 12)

#define GRAM_WR_MEM_START	0x2c
#define GRAM_WR_MEM_CONT	0x3c
#define SIDERAM_WR_MEM_START	0x4c
#define SIDERAM_WR_MEM_CONT	0x5c

int ss_write_ddi_ram(struct samsung_display_driver_data *vdd,
		int target, u8 *buffer, int len)
{
	u8 hdr_start;
	u8 hdr_continue;
	struct dsi_panel_cmd_set *set;
	struct dsi_cmd_desc *cmds;
	u8 *tx_buf;
	u8 *org_tx_buf;
	int loop;
	int remain;
	int psize;

	LCD_INFO("+\n");

	set = ss_get_cmds(vdd, TX_DDI_RAM_IMG_DATA);

	if (target == MIPI_TX_TYPE_GRAM) {
		hdr_start = GRAM_WR_MEM_START;
		hdr_continue = GRAM_WR_MEM_CONT;
	} else if (target == MIPI_TX_TYPE_SIDERAM) {
		hdr_start = SIDERAM_WR_MEM_START;
		hdr_continue = SIDERAM_WR_MEM_CONT;
	} else {
		LCD_ERR("invalid target (%d)\n", target);
		return -EINVAL;
	}

	set->state = DSI_CMD_SET_STATE_HS;
	set->count = DIV_ROUND_UP(len, MAX_SIZE_IMG_PAYLOAD);

	cmds = vzalloc(sizeof(struct dsi_cmd_desc) * set->count);
	tx_buf = vzalloc((MAX_SIZE_IMG_PAYLOAD + 1) * set->count);
	org_tx_buf = tx_buf;
	set->cmds = cmds;


	LCD_INFO("len=%d, cmds count=%d\n", len, set->count);

	/* split and fill image data to TX_DDI_RAM_IMG_DATA packet */
	loop = 0;
	remain = len;
	while (remain > 0) {
		cmds[loop].msg.type = MIPI_DSI_GENERIC_LONG_WRITE;
		cmds[loop].last_command = 1;
		cmds[loop].msg.tx_buf = tx_buf;

		if (loop == 0)
			*tx_buf = hdr_start;
		else
			*tx_buf = hdr_continue;
		tx_buf++;

		if (remain > MAX_SIZE_IMG_PAYLOAD)
			psize = MAX_SIZE_IMG_PAYLOAD;
		else
			psize = remain;

		memcpy(tx_buf, buffer, psize);
		cmds[loop].msg.tx_len = psize + 1;

		tx_buf += psize;
		buffer += psize;
		remain -= psize;
		loop++;
	}

	LCD_INFO("start tx gram\n");
	ss_send_cmd(vdd, TX_DDI_RAM_IMG_DATA);
	LCD_INFO("fin tx gram\n");

	vfree(org_tx_buf);
	vfree(cmds);
	LCD_INFO("-\n");

	return 0;
}

/**
 * controller have 4 registers can hold 16 bytes of rxed data
 * dcs packet: 4 bytes header + payload + 2 bytes crc
 * 1st read: 4 bytes header + 10 bytes payload + 2 crc
 * 2nd read: 14 bytes payload + 2 crc
 * 3rd read: 14 bytes payload + 2 crc
 */
#define MAX_LEN_RX_BUF	200
#define RX_SIZE_LIMIT	10
int ss_panel_data_read(struct samsung_display_driver_data *vdd,
		enum dsi_cmd_set_type type, u8 *buffer, int level_key)
{
	struct dsi_panel_cmd_set *set;
	struct dsi_panel_cmd_set *read_pos_cmd;
	static u8 rx_buffer[MAX_LEN_RX_BUF] = {0,};
	char show_buffer[MAX_LEN_RX_BUF] = {0,};
	char temp_buffer[MAX_LEN_RX_BUF] = {0,};
	int orig_rx_len = 0;
	int new_rx_len = 0;
	int orig_offset = 0;
	int new_offset = 0;
	int loop_limit = 0;
	int show_cnt = 0;
	int pos = 0;
	int i, j;

	if (!ss_panel_attach_get(vdd)) {
		LCD_ERR("ss_panel_attach_get(%d) : %d\n",
				vdd->ndx, ss_panel_attach_get(vdd));
		return -EPERM;
	}

	if (ss_is_panel_off(vdd)) {
		LCD_ERR("skip to rx cmd (%d), panel off (%d)\n",
				type, ss_is_panel_off(vdd));
		return -EPERM;
	}

	/* To block read operation at esd-recovery */
	if (vdd->panel_dead) {
		LCD_ERR("esd recovery, skip %s\n", ss_get_cmd_name(type));
		return -EPERM;
	}

	/* samsung mipi rx cmd feature supports only one command */
	set = ss_get_cmds(vdd, type);
	if (!ss_is_read_cmd(type) || set->count != 1) {
		LCD_ERR("invalid set(%d): %s, count (%d)\n",
				type, ss_get_cmd_name(type), set->count);
		return -EINVAL;
	}

	/* enable level key */
	if (level_key & LEVEL1_KEY)
		ss_send_cmd(vdd, TX_LEVEL1_KEY_ENABLE);
	if (level_key & LEVEL2_KEY)
		ss_send_cmd(vdd, TX_LEVEL2_KEY_ENABLE);

	set->cmds[0].msg.rx_buf = rx_buffer;

	read_pos_cmd = ss_get_cmds(vdd, TX_REG_READ_POS);

	orig_rx_len = set->cmds[0].msg.rx_len;
	orig_offset = new_offset = set->read_startoffset;

	loop_limit = (orig_rx_len + RX_SIZE_LIMIT - 1) / RX_SIZE_LIMIT;

	LCD_DEBUG("orig_rx_len (%d) , orig_offset (%d) loop_limit (%d)\n", orig_rx_len, orig_offset, loop_limit);

	for (i = 0; i < loop_limit; i++) {
		/* gPara */
		read_pos_cmd->cmds->msg.tx_buf[1] = new_offset;
		new_rx_len = ((orig_rx_len - new_offset + orig_offset) < RX_SIZE_LIMIT) ?
						(orig_rx_len - new_offset + orig_offset) : RX_SIZE_LIMIT;

		LCD_DEBUG("new_offset (%d) new_rx_len (%d) \n", new_offset, new_rx_len);

		ss_send_cmd(vdd, TX_REG_READ_POS);

		set->cmds[0].msg.rx_len = new_rx_len;

		/* RX */
		ss_send_cmd(vdd, type);

		/* oopy to buffer */
		memcpy(&buffer[show_cnt], rx_buffer, new_rx_len);

		/* snprint */
		memcpy(temp_buffer, set->cmds[0].msg.rx_buf, new_rx_len);
		for (j = 0; j < new_rx_len; j++, show_cnt++) {
			pos += snprintf(show_buffer + pos, sizeof(show_buffer) - pos, "%02x ",
				temp_buffer[j]);
		}

		/* increase gPara offset */
		new_offset += new_rx_len;

		if (new_offset - orig_offset >= orig_rx_len)
			break;
	}

	/* restore rx len */
	set->cmds[0].msg.rx_len = orig_rx_len;

	/* disable level key */
	if (level_key & LEVEL1_KEY)
		ss_send_cmd(vdd, TX_LEVEL1_KEY_DISABLE);
	if (level_key & LEVEL2_KEY)
		ss_send_cmd(vdd, TX_LEVEL2_KEY_DISABLE);

	LCD_INFO("[%d]%s, addr: 0x%x, off: %d, len: %d, buf: %s\n",
			type, ss_get_cmd_name(type),
			set->cmds[0].msg.tx_buf[0], orig_offset, orig_rx_len,
			show_buffer);

	return 0;
}

int ss_panel_on_pre(struct samsung_display_driver_data *vdd)
{
	int rddpm, rddsm, errfg, dsierror;

	/* At this time, it already enabled SDE clock/power and  display power.
	 * It is possible to send mipi comamnd to display.
	 * To send mipi command, like mdnie setting or brightness setting,
	 * change panel_state: PANEL_PWR_OFF -> PANEL_PWR_ON_READY, here.
	 */
	vdd->panel_state = PANEL_PWR_ON_READY;

	vdd->display_status_dsi.disp_on_pre = 1;

	if (vdd->other_line_panel_work_cnt)
		vdd->other_line_panel_work_cnt = 0; /*stop open otherline dat file*/

#if !defined(CONFIG_SEC_FACTORY)
	/* LCD ID read every wake_up time incase of factory binary */
	if (vdd->dtsi_data.tft_common_support)
		return false;
#endif

	if (!ss_panel_attach_get(vdd)) {
		LCD_ERR("ss_panel_attach_get NG\n");
		return false;
	}

	LCD_INFO("+\n");

	if (unlikely(vdd->is_factory_mode) &&
			vdd->dtsi_data.samsung_support_factory_panel_swap) {
		/* LCD ID read every wake_up time incase of factory binary */
		vdd->manufacture_id_dsi = PBA_ID;

		/* Factory Panel Swap*/
		vdd->manufacture_date_loaded_dsi = 0;
		vdd->ddi_id_loaded_dsi = 0;
		vdd->cell_id_loaded_dsi = 0;
		vdd->octa_id_loaded_dsi = 0;
		vdd->hbm_loaded_dsi = 0;
		vdd->mdnie_loaded_dsi = 0;
		vdd->smart_dimming_loaded_dsi = 0;
		vdd->smart_dimming_hmt_loaded_dsi = 0;
		vdd->table_interpolation_loaded = 0;
	}

	if (vdd->manufacture_id_dsi == PBA_ID) {
		u8 recv_buf[3];

		/*
		*	At this time, panel revision it not selected.
		*	So last index(SUPPORT_PANEL_REVISION-1) used.
		*/
		vdd->panel_revision = SUPPORT_PANEL_REVISION-1;

		/*
		*	Some panel needs to update register at init time to read ID & MTP
		*	Such as, dual-dsi-control or sleep-out so on.
		*/
		if (!(ss_get_cmds(vdd, RX_MANUFACTURE_ID)->count)) {
			ss_panel_data_read(vdd, RX_MANUFACTURE_ID,
					recv_buf, LEVEL1_KEY);
		} else {
			LCD_INFO("manufacture_read_pre_tx_cmds\n");
			ss_send_cmd(vdd, TX_MANUFACTURE_ID_READ_PRE);

			ss_panel_data_read(vdd, RX_MANUFACTURE_ID0,
					recv_buf, LEVEL1_KEY);
			ss_panel_data_read(vdd, RX_MANUFACTURE_ID1,
					recv_buf + 1, LEVEL1_KEY);
			ss_panel_data_read(vdd, RX_MANUFACTURE_ID2,
					recv_buf + 2, LEVEL1_KEY);
		}

		vdd->manufacture_id_dsi =
			(recv_buf[0] << 16) | (recv_buf[1] << 8) | recv_buf[2];
		LCD_INFO("manufacture id: 0x%x\n", vdd->manufacture_id_dsi);

		/* Panel revision selection */
		if (IS_ERR_OR_NULL(vdd->panel_func.samsung_panel_revision))
			LCD_ERR("no panel_revision_selection_error fucntion\n");
		else
			vdd->panel_func.samsung_panel_revision(vdd);

		LCD_INFO("Panel_Revision = %c %d\n", vdd->panel_revision + 'A', vdd->panel_revision);
	}

	/* Read panel status to check panel is ok from bootloader */
	if (!vdd->read_panel_status_from_lk) {
		rddpm = ss_read_rddpm(vdd);
		rddsm = ss_read_rddsm(vdd);
		errfg = ss_read_errfg(vdd);
		dsierror = ss_read_dsierr(vdd);
		ss_read_pps_data(vdd);

		SS_XLOG(rddpm, rddsm, errfg, dsierror);
		LCD_INFO("panel dbg: %x %x %x %x\n", rddpm, rddsm, errfg, dsierror);

		vdd->read_panel_status_from_lk = 1;
	}

	/* Manufacture date */
	if (!vdd->manufacture_date_loaded_dsi) {
		if (IS_ERR_OR_NULL(vdd->panel_func.samsung_manufacture_date_read))
			LCD_ERR("no samsung_manufacture_date_read function\n");
		else
			vdd->manufacture_date_loaded_dsi = vdd->panel_func.samsung_manufacture_date_read(vdd);
	}

	/* DDI ID */
	if (!vdd->ddi_id_loaded_dsi) {
		if (IS_ERR_OR_NULL(vdd->panel_func.samsung_ddi_id_read))
			LCD_ERR("no samsung_ddi_id_read function\n");
		else
			vdd->ddi_id_loaded_dsi = vdd->panel_func.samsung_ddi_id_read(vdd);
	}

	/* Panel Unique OCTA ID */
	if (!vdd->octa_id_loaded_dsi) {
		if (IS_ERR_OR_NULL(vdd->panel_func.samsung_octa_id_read))
			LCD_ERR("no samsung_octa_id_read function\n");
		else
			vdd->octa_id_loaded_dsi = vdd->panel_func.samsung_octa_id_read(vdd);
	}

	/* ELVSS read */
	if (!vdd->elvss_loaded_dsi) {
		if (IS_ERR_OR_NULL(vdd->panel_func.samsung_elvss_read))
			LCD_ERR("no samsung_elvss_read function\n");
		else
			vdd->elvss_loaded_dsi = vdd->panel_func.samsung_elvss_read(vdd);
	}

	/* IRC read */
	if (!vdd->irc_loaded_dsi) {
		if (IS_ERR_OR_NULL(vdd->panel_func.samsung_irc_read))
			LCD_ERR("no samsung_irc_read function\n");
		else
			vdd->irc_loaded_dsi = vdd->panel_func.samsung_irc_read(vdd);
	}

	/* HBM */
	if (!vdd->hbm_loaded_dsi) {
		if (IS_ERR_OR_NULL(vdd->panel_func.samsung_hbm_read))
			LCD_ERR("no samsung_hbm_read function\n");
		else
			vdd->hbm_loaded_dsi = vdd->panel_func.samsung_hbm_read(vdd);
	}

	/* MDNIE X,Y */
	if (!vdd->mdnie_loaded_dsi) {
		if (IS_ERR_OR_NULL(vdd->panel_func.samsung_mdnie_read))
			LCD_ERR("no samsung_mdnie_read function\n");
		else
			vdd->mdnie_loaded_dsi = vdd->panel_func.samsung_mdnie_read(vdd);
	}

	/* Panel Unique Cell ID */
	if (!vdd->cell_id_loaded_dsi) {
		if (IS_ERR_OR_NULL(vdd->panel_func.samsung_cell_id_read))
			LCD_ERR("no samsung_cell_id_read function\n");
		else
			vdd->cell_id_loaded_dsi = vdd->panel_func.samsung_cell_id_read(vdd);
	}

	/* Smart dimming*/
	if (!vdd->smart_dimming_loaded_dsi) {
		if (IS_ERR_OR_NULL(vdd->panel_func.samsung_smart_dimming_init))
			LCD_ERR("no samsung_smart_dimming_init function\n");
		else
			vdd->smart_dimming_loaded_dsi = vdd->panel_func.samsung_smart_dimming_init(vdd);
	}

	/* Smart dimming for hmt */
	if (vdd->dtsi_data.hmt_enabled) {
		if (!vdd->smart_dimming_hmt_loaded_dsi) {
			if (IS_ERR_OR_NULL(vdd->panel_func.samsung_smart_dimming_hmt_init))
				LCD_ERR("no samsung_smart_dimming_hmt_init function\n");
			else
				vdd->smart_dimming_hmt_loaded_dsi = vdd->panel_func.samsung_smart_dimming_hmt_init(vdd);
		}
	}

	/* self display */
	if (!vdd->self_display_loaded_dsi) {
		if (IS_ERR_OR_NULL(vdd->panel_func.samsung_self_display_init))
			LCD_ERR("no samsung_self_display_init function\n");
		else
			vdd->self_display_loaded_dsi = vdd->panel_func.samsung_self_display_init(vdd);
	}

	/* copr */
	if (!vdd->copr_load_init_cmd && vdd->copr.copr_on) {
		vdd->copr_load_init_cmd = ss_copr_get_cmd(vdd);
	}

	if (!vdd->table_interpolation_loaded) {
		if (!IS_ERR_OR_NULL(vdd->panel_func.samsung_interpolation_init)) {
			table_br_func(vdd);
			vdd->table_interpolation_loaded = vdd->panel_func.samsung_interpolation_init(vdd, TABLE_INTERPOLATION);
		}
	}

	if (!IS_ERR_OR_NULL(vdd->panel_func.samsung_panel_on_pre))
		vdd->panel_func.samsung_panel_on_pre(vdd);

	LCD_INFO("-\n");

	return true;
}

int ss_panel_on_post(struct samsung_display_driver_data *vdd)
{
	if (!ss_panel_attach_get(vdd)) {
		LCD_ERR("ss_panel_attach_get NG\n");
		return false;
	}

	LCD_INFO("+\n");

/* TODO: enable debug file and activate below...
#ifdef CONFIG_DISPLAY_USE_INFO
	ss_read_self_diag(vdd);
#endif
*/

	if (vdd->support_cabc && !vdd->auto_brightness)
		ss_cabc_update(vdd);
	else if (vdd->ss_panel_tft_outdoormode_update && vdd->auto_brightness)
		vdd->ss_panel_tft_outdoormode_update(vdd);
	else if (vdd->support_cabc && vdd->auto_brightness)
		ss_tft_autobrightness_cabc_update(vdd);

	if (!IS_ERR_OR_NULL(vdd->panel_func.samsung_panel_on_post))
		vdd->panel_func.samsung_panel_on_post(vdd);

	if ((vdd->panel_func.color_weakness_ccb_on_off) && vdd->color_weakness_mode)
		vdd->panel_func.color_weakness_ccb_on_off(vdd, vdd->color_weakness_mode);

	if (vdd->support_hall_ic) {
		/*
		* Brightenss cmds sent by samsung_display_hall_ic_status() at panel switching operation
		*/
		if (ss_is_bl_dcs(vdd) &&
			(vdd->hall_ic_mode_change_trigger == false))
			ss_brightness_dcs(vdd, vdd->bl_level);
	} else {
		if (ss_is_bl_dcs(vdd)) {
			struct backlight_device *bd = GET_SDE_BACKLIGHT_DEVICE(vdd);

			/* In case of backlight update in panel off,
			 * dsi_display_set_backlight() returns error
			 * without updating vdd->bl_level.
			 * Update bl_level from bd->props.brightness.
			 */
			if (bd && vdd->bl_level != bd->props.brightness) {
				LCD_INFO("update bl_level: %d -> %d\n",
					vdd->bl_level, bd->props.brightness);
				vdd->bl_level = bd->props.brightness;
			}

			ss_brightness_dcs(vdd, vdd->bl_level);
		}
	}

	if (vdd->support_mdnie_lite) {
		vdd->mdnie_lcd_on_notifiy = true;
		update_dsi_tcon_mdnie_register(vdd);
		if (vdd->support_mdnie_trans_dimming)
			vdd->mdnie_disable_trans_dimming = false;
	}

	/*
	 * Update Cover Control Status every Normal sleep & wakeup
	 * Do not update Cover_control at this point in case of AOD.
	 * Because, below update is done before entering AOD.
	 */
	if (vdd->panel_func.samsung_cover_control && vdd->cover_control
		&& !ss_is_panel_lpm(vdd))
		vdd->panel_func.samsung_cover_control(vdd);

	/* Work around: For folder model, the status bar resizes itself and old UI appears in sub-panel
	 * before premium watch or screen lock is on, so it needs to skip old UI.
	 */
	if (!vdd->lcd_flip_not_refresh &&
			vdd->support_hall_ic &&
			vdd->hall_ic_mode_change_trigger &&
			vdd->lcd_flip_delay_ms) {
		schedule_delayed_work(&vdd->delay_disp_on_work, msecs_to_jiffies(vdd->lcd_flip_delay_ms));
	} else
		vdd->display_status_dsi.wait_disp_on = true;

	vdd->display_status_dsi.wait_actual_disp_on = true;
	vdd->display_status_dsi.aod_delay = true;

	if (ss_is_esd_check_enabled(vdd)) {
		if (vdd->esd_recovery.esd_irq_enable)
			vdd->esd_recovery.esd_irq_enable(true, true, (void *)vdd);
	}

	if (vdd->dyn_mipi_clk.is_support)
		ss_send_cmd(vdd, TX_FFC);

	if (vdd->copr.copr_on)
		ss_send_cmd(vdd, TX_COPR_ENABLE);

	vdd->panel_state = PANEL_PWR_ON;

	vdd->te_check.te_cnt = 0;

	LCD_INFO("-\n");

	return true;
}

int ss_panel_off_pre(struct samsung_display_driver_data *vdd)
{
	int rddpm, rddsm, errfg, dsierror;
	int ret = 0;

	LCD_INFO("+\n");
#ifdef CONFIG_DISPLAY_USE_INFO
	rddpm = ss_read_rddpm(vdd);
	rddsm = ss_read_rddsm(vdd);
	errfg = ss_read_errfg(vdd);
	dsierror = ss_read_dsierr(vdd);
	ss_read_pps_data(vdd);
	SS_XLOG(rddpm, rddsm, errfg, dsierror);
	LCD_INFO("panel dbg: %x %x %x %x\n", rddpm, rddsm, errfg, dsierror);
#endif

	if (ss_is_esd_check_enabled(vdd)) {
		vdd->esd_recovery.is_wakeup_source = false;
		if (vdd->esd_recovery.esd_irq_enable)
			vdd->esd_recovery.esd_irq_enable(false, true, (void *)vdd);
		vdd->esd_recovery.is_enabled_esd_recovery = false;
	}

	if (!IS_ERR_OR_NULL(vdd->panel_func.samsung_panel_off_pre))
		vdd->panel_func.samsung_panel_off_pre(vdd);

	mutex_lock(&vdd->vdd_lock);
	vdd->panel_state = PANEL_PWR_OFF;
	mutex_unlock(&vdd->vdd_lock);
	vdd->panel_dead = false;

	LCD_INFO("-\n");

	return ret;
}

/*
 * Do not call ss_send_cmd() or ss_panel_data_read() here.
 * Any MIPI Tx/Rx can not be alowed in here.
 */
int ss_panel_off_post(struct samsung_display_driver_data *vdd)
{
	int ret = 0;

	LCD_INFO("+\n");

	if (vdd->support_mdnie_trans_dimming)
		vdd->mdnie_disable_trans_dimming = true;

	if (vdd->support_hall_ic && vdd->lcd_flip_delay_ms)
		cancel_delayed_work(&vdd->delay_disp_on_work);

	if (!IS_ERR_OR_NULL(vdd->panel_func.samsung_panel_off_post))
		vdd->panel_func.samsung_panel_off_post(vdd);

	/* gradual acl on/off */
	vdd->gradual_pre_acl_on = GRADUAL_ACL_UNSTABLE;

	/* Reset Self Display Status */
	vdd->self_disp.sa_info.en = false;
	vdd->self_disp.sd_info.en = false;
	vdd->self_disp.si_info.en = false;
	vdd->self_disp.sg_info.en = false;
	vdd->self_disp.time_set = false;
	vdd->self_disp.on = false;

	LCD_INFO("-\n");
	SS_XLOG(SS_XLOG_FINISH);

	return ret;
}

/*************************************************************
*
*		TFT BACKLIGHT GPIO FUNCTION BELOW.
*
**************************************************************/
int ss_backlight_tft_request_gpios(struct samsung_display_driver_data *vdd)
{
	int rc = 0, i;
	/*
	 * gpio_name[] named as gpio_name + num(recomend as 0)
	 * because of the num will increase depend on number of gpio
	 */
	char gpio_name[17] = "disp_bcklt_gpio0";
	static u8 gpio_request_status = -EINVAL;

	if (!gpio_request_status)
		goto end;

	if (IS_ERR_OR_NULL(vdd)) {
		LCD_ERR("Invalid data pinfo : 0x%zx\n", (size_t)vdd);
		goto end;
	}

	for (i = 0; i < MAX_BACKLIGHT_TFT_GPIO; i++) {
		if (gpio_is_valid(vdd->dtsi_data.backlight_tft_gpio[i])) {
			rc = gpio_request(vdd->dtsi_data.backlight_tft_gpio[i],
							gpio_name);
			if (rc) {
				LCD_ERR("request %s failed, rc=%d\n", gpio_name, rc);
				goto tft_backlight_gpio_err;
			}
		}
	}

	gpio_request_status = rc;
end:
	return rc;
tft_backlight_gpio_err:
	if (i) {
		do {
			if (gpio_is_valid(vdd->dtsi_data.backlight_tft_gpio[i]))
				gpio_free(vdd->dtsi_data.backlight_tft_gpio[i--]);
			LCD_ERR("i = %d\n", i);
		} while (i > 0);
	}

	return rc;
}

int ss_backlight_tft_gpio_config(struct samsung_display_driver_data *vdd, int enable)
{
	int ret = 0, i = 0, add_value = 1;

	if (IS_ERR_OR_NULL(vdd)) {
		LCD_ERR("Invalid data pinfo : 0x%zx\n",  (size_t)vdd);
		goto end;
	}

	LCD_INFO("++ enable(%d) ndx(%d)\n", enable, ss_get_display_ndx(vdd));

	if (ss_backlight_tft_request_gpios(vdd)) {
		LCD_ERR("fail to request tft backlight gpios");
		goto end;
	}

	LCD_DEBUG("%s tft backlight gpios\n", enable ? "enable" : "disable");

	/*
	 * The order of backlight_tft_gpio enable/disable
	 * 1. Enable : backlight_tft_gpio[0], [1], ... [MAX_BACKLIGHT_TFT_GPIO - 1]
	 * 2. Disable : backlight_tft_gpio[MAX_BACKLIGHT_TFT_GPIO - 1], ... [1], [0]
	 */
	if (!enable) {
		add_value = -1;
		i = MAX_BACKLIGHT_TFT_GPIO - 1;
	}

	do {
		if (gpio_is_valid(vdd->dtsi_data.backlight_tft_gpio[i])) {
			gpio_set_value(vdd->dtsi_data.backlight_tft_gpio[i], enable);
			LCD_DEBUG("set backlight tft gpio[%d] to %s\n",
						 vdd->dtsi_data.backlight_tft_gpio[i],
						enable ? "high" : "low");
			usleep_range(500, 500);
		}
	} while (((i += add_value) < MAX_BACKLIGHT_TFT_GPIO) && (i >= 0));

end:
	LCD_INFO("--\n");
	return ret;
}

/*************************************************************
*
*		ESD RECOVERY RELATED FUNCTION BELOW.
*
**************************************************************/

#if 0
static int ss_esd_check_status(struct samsung_display_driver_data *vdd)
{
	LCD_INFO("lcd esd - check_ststus\n");
	return 0;
}
static int ss_esd_read_status(struct mdss_dsi_ctrl_pdata *ctrl)
{
	LCD_INFO("lcd esd - check_read_status\n");
	// esd status must return 0 to go status_dead(blank->unblnk) in ss_check_dsi_ctrl_status.
	return 0;
}
#endif

/*
 * esd_irq_enable() - Enable or disable esd irq.
 *
 * @enable	: flag for enable or disabled
 * @nosync	: flag for disable irq with nosync
 * @data	: point ot struct ss_panel_info
 */
#define IRQS_PENDING	0x00000200
#define istate core_internal_state__do_not_mess_with_it
static void esd_irq_enable(bool enable, bool nosync, void *data)
{
	/* The irq will enabled when do the request_threaded_irq() */
	static bool is_enabled = true;
	static bool is_wakeup_source;
	int gpio;
	unsigned long flags;
	struct samsung_display_driver_data *vdd =
		(struct samsung_display_driver_data *)data;
	struct irq_desc *desc = NULL;
	u8 i = 0;

	if (IS_ERR_OR_NULL(vdd)) {
		LCD_ERR("Invalid data vdd : 0x%zx\n", (size_t)vdd);
		return;
	}

	spin_lock_irqsave(&vdd->esd_recovery.irq_lock, flags);

	if (enable == is_enabled) {
		LCD_INFO("ESD irq already %s\n",
				enable ? "enabled" : "disabled");
		goto config_wakeup_source;
	}


	for (i = 0; i < vdd->esd_recovery.num_of_gpio; i++) {
		gpio = vdd->esd_recovery.esd_gpio[i];
		desc = irq_to_desc(gpio_to_irq(gpio));
		if (enable) {
			/* clear esd irq triggered while it was disabled. */
			if (desc->istate & IRQS_PENDING) {
				LCD_INFO("clear esd irq pending status\n");
				desc->istate &= ~IRQS_PENDING;
			}
		}

		if (!gpio_is_valid(gpio)) {
			LCD_ERR("Invalid ESD_GPIO : %d\n", gpio);
			continue;
		}

		if (enable) {
			is_enabled = true;
			enable_irq(gpio_to_irq(gpio));
		} else {
			if (nosync)
				disable_irq_nosync(gpio_to_irq(gpio));
			else
				disable_irq(gpio_to_irq(gpio));
			is_enabled = false;
		}
	}

	/* TODO: Disable log if the esd function stable */
	LCD_DEBUG("ESD irq %s with %s\n",
				enable ? "enabled" : "disabled",
				nosync ? "nosync" : "sync");

config_wakeup_source:
	if (vdd->esd_recovery.is_wakeup_source == is_wakeup_source) {
		LCD_DEBUG("[ESD] IRQs are already irq_wake %s\n",
				is_wakeup_source ? "enabled" : "disabled");
		goto end;
	}

	for (i = 0; i < vdd->esd_recovery.num_of_gpio; i++) {
		gpio = vdd->esd_recovery.esd_gpio[i];

		if (!gpio_is_valid(gpio)) {
			LCD_ERR("Invalid ESD_GPIO : %d\n", gpio);
			continue;
		}

		is_wakeup_source =
			vdd->esd_recovery.is_wakeup_source;

		if (is_wakeup_source)
			enable_irq_wake(gpio_to_irq(gpio));
		else
			disable_irq_wake(gpio_to_irq(gpio));
	}

	LCD_DEBUG("[ESD] IRQs are set to irq_wake %s\n",
				is_wakeup_source ? "enabled" : "disabled");

end:
	spin_unlock_irqrestore(&vdd->esd_recovery.irq_lock, flags);

}

static irqreturn_t esd_irq_handler(int irq, void *handle)
{
	struct samsung_display_driver_data *vdd = samsung_get_vdd();
	struct sde_connector *conn = GET_SDE_CONNECTOR(vdd);

	if (!handle) {
		LCD_INFO("handle is null\n");
		return IRQ_HANDLED;
	}

	if (IS_ERR_OR_NULL(vdd)) {
		LCD_ERR("Invalid data vdd : 0x%zx\n", (size_t)vdd);
		goto end;
	}

	if (!vdd->esd_recovery.is_enabled_esd_recovery) {
		LCD_ERR("esd recovery is not enabled yet");
		goto end;
	}
	LCD_INFO("++\n");

	esd_irq_enable(false, true, (void *)vdd);

	vdd->panel_lpm.esd_recovery = true;

	schedule_work(&conn->status_work.work);

	LCD_INFO("--\n");

end:
	return IRQ_HANDLED;
}

// refer to dsi_display_res_init() and dsi_panel_get(&display->pdev->dev, display->panel_of);
static void ss_panel_parse_dt_esd(struct device_node *np,
		struct samsung_display_driver_data *vdd)
{
	int rc = 0;
	const char *data;
	char esd_irq_gpio[] = "samsung,esd-irq-gpio1";
	char esd_irqflag[] = "qcom,mdss-dsi-panel-status-irq-trigger1";
	struct esd_recovery *esd = NULL;
	struct dsi_panel *panel = NULL;
	struct drm_panel_esd_config *esd_config = NULL;
	u8 i = 0;

	panel = (struct dsi_panel *)vdd->msm_private;
	esd_config = &panel->esd_config;

	esd = &vdd->esd_recovery;
	esd->num_of_gpio = 0;

	esd_config->esd_enabled = of_property_read_bool(np,
		"qcom,esd-check-enabled");

	if (!esd_config->esd_enabled)
		goto end;

	for (i = 0; i < MAX_ESD_GPIO; i++) {
		esd_irq_gpio[strlen(esd_irq_gpio) - 1] = '1' + i;
		esd->esd_gpio[esd->num_of_gpio] = of_get_named_gpio(np,
				esd_irq_gpio, 0);

		if (gpio_is_valid(esd->esd_gpio[esd->num_of_gpio])) {
			LCD_INFO("[ESD] gpio : %d, irq : %d\n",
					esd->esd_gpio[esd->num_of_gpio],
					gpio_to_irq(esd->esd_gpio[esd->num_of_gpio]));
			esd->num_of_gpio++;
		}
	}

	rc = of_property_read_string(np, "qcom,mdss-dsi-panel-status-check-mode", &data);
	if (!rc) {
		if (!strcmp(data, "irq_check"))
			esd_config->status_mode = ESD_MODE_PANEL_IRQ;
		else
			LCD_ERR("No valid panel-status-check-mode string\n");
	}

	for (i = 0; i < esd->num_of_gpio; i++) {
		esd_irqflag[strlen(esd_irqflag) - 1] = '1' + i;
		rc = of_property_read_string(np, esd_irqflag, &data);
		if (!rc) {
			esd->irqflags[i] =
				IRQF_ONESHOT | IRQF_NO_SUSPEND;

			if (!strcmp(data, "rising"))
				esd->irqflags[i] |= IRQF_TRIGGER_RISING;
			else if (!strcmp(data, "falling"))
				esd->irqflags[i] |= IRQF_TRIGGER_FALLING;
			else if (!strcmp(data, "high"))
				esd->irqflags[i] |= IRQF_TRIGGER_HIGH;
			else if (!strcmp(data, "low"))
				esd->irqflags[i] |= IRQF_TRIGGER_LOW;
		}
	}

end:
	LCD_INFO("samsung esd %s mode (%d)\n",
		esd_config->esd_enabled ? "enabled" : "disabled",
		esd_config->status_mode);
	return;
}

static void ss_event_esd_recovery_init(
		struct samsung_display_driver_data *vdd, int event, void *arg)
{
	// TODO: implement after qcomm esd bsp appllied...
	int ret;
	u8 i;
	int gpio, irqflags;
	struct esd_recovery *esd = NULL;
	struct dsi_panel *panel = NULL;
	struct drm_panel_esd_config *esd_config = NULL;

	panel = (struct dsi_panel *)vdd->msm_private;
	esd_config = &panel->esd_config;

	esd = &vdd->esd_recovery;


	if (IS_ERR_OR_NULL(vdd)) {
		LCD_ERR("Invalid data vdd : 0x%zx\n", (size_t)vdd);
		return;
	}

	if (unlikely(!esd->esd_recovery_init)) {
		esd->esd_recovery_init = true;
		esd->esd_irq_enable = esd_irq_enable;
		if (esd_config->status_mode  == ESD_MODE_PANEL_IRQ) {
			for (i = 0; i < esd->num_of_gpio; i++) {
				/* Set gpio num and irqflags */
				gpio = esd->esd_gpio[i];
				irqflags = esd->irqflags[i];
				if (!gpio_is_valid(gpio)) {
					LCD_ERR("[ESD] Invalid GPIO : %d\n", gpio);
					continue;
				}

				gpio_request(gpio, "esd_recovery");
				ret = request_threaded_irq(
						gpio_to_irq(gpio),
						NULL,
						esd_irq_handler,
						irqflags,
						"esd_recovery",
						(void *)panel);
				if (ret)
					LCD_ERR("Failed to request_irq, ret=%d\n",
							ret);
			}
			esd_irq_enable(false, true, (void *)vdd);
		}
	}
}

static void ss_panel_recovery(struct samsung_display_driver_data *vdd)
{
	struct sde_connector *conn = GET_SDE_CONNECTOR(vdd);

	if (IS_ERR_OR_NULL(vdd)) {
		LCD_ERR("Invalid data vdd : 0x%zx\n", (size_t)vdd);
		return;
	}

	if (!vdd->esd_recovery.is_enabled_esd_recovery) {
		LCD_ERR("esd recovery is not enabled yet");
		return;
	}
	LCD_INFO("Panel Recovery, Trial Count = %d\n", vdd->panel_recovery_cnt++);
	SS_XLOG(vdd->panel_recovery_cnt);
	inc_dpui_u32_field(DPUI_KEY_QCT_RCV_CNT, 1);

	esd_irq_enable(false, true, (void *)vdd);
	vdd->panel_lpm.esd_recovery = true;
	schedule_work(&conn->status_work.work);

	LCD_INFO("Panel Recovery --\n");

}

/*************************************************************
*
*		OSC TE FITTING RELATED FUNCTION BELOW.
*
**************************************************************/
static void ss_event_osc_te_fitting(
		struct samsung_display_driver_data *vdd, int event, void *arg)
{
	struct osc_te_fitting_info *te_info = NULL;
	int ret, i, lut_count;
	unsigned int disp_te_gpio;

	te_info = &vdd->te_fitting_info;

	if (IS_ERR_OR_NULL(te_info)) {
		LCD_ERR("Invalid te data : 0x%zx\n",
				(size_t)te_info);
		return;
	}

	if (ss_is_seamless_mode(vdd)) {
		LCD_ERR("cont splash enabled\n");
		return;
	}

	if (!ss_is_vsync_enabled(vdd)) {
		LCD_DEBUG("vsync handler does not enabled yet\n");
		return;
	}

	te_info->status |= TE_FITTING_DONE;

	LCD_DEBUG("++\n");

	disp_te_gpio = ss_get_te_gpio(vdd);

	if (!(te_info->status & TE_FITTING_REQUEST_IRQ)) {
		te_info->status |= TE_FITTING_REQUEST_IRQ;


		ret = request_threaded_irq(
				gpio_to_irq(disp_te_gpio),
				samsung_te_check_handler,
				NULL,
				IRQF_TRIGGER_FALLING,
				"VSYNC_GPIO",
				(void *) vdd);
		if (ret)
			LCD_ERR("Failed to request_irq, ret=%d\n",
					ret);
		else
			disable_irq(gpio_to_irq(disp_te_gpio));
		te_info->te_time =
			kzalloc(sizeof(long long) * te_info->sampling_rate, GFP_KERNEL);
		INIT_WORK(&te_info->work, samsung_te_check_done_work);
	}

	for (lut_count = 0; lut_count < OSC_TE_FITTING_LUT_MAX; lut_count++) {
		init_completion(&te_info->te_check_comp);
		te_info->status |= TE_CHECK_ENABLE;
		te_info->te_duration = 0;

		LCD_DEBUG("osc_te_fitting _irq : %d\n",
				gpio_to_irq(disp_te_gpio));

		enable_irq(gpio_to_irq(disp_te_gpio));
		ret = wait_for_completion_timeout(
				&te_info->te_check_comp, 1000);

		if (ret <= 0)
			LCD_ERR("timeout\n");

		for (i = 0; i < te_info->sampling_rate; i++) {
			te_info->te_duration +=
				(i != 0 ? (te_info->te_time[i] - te_info->te_time[i-1]) : 0);
			LCD_DEBUG("vsync time : %lld, sum : %lld\n",
					te_info->te_time[i], te_info->te_duration);
		}
		do_div(te_info->te_duration, te_info->sampling_rate - 1);
		LCD_INFO("ave vsync time : %lld\n",
				te_info->te_duration);
		te_info->status &= ~TE_CHECK_ENABLE;

		if (vdd->panel_func.samsung_osc_te_fitting)
			ret = vdd->panel_func.samsung_osc_te_fitting(vdd);

		if (!ret)
			ss_send_cmd(vdd, TX_OSC_TE_FITTING);
		else
			break;
	}
	LCD_DEBUG("--\n");
}

static void samsung_te_check_done_work(struct work_struct *work)
{
	struct osc_te_fitting_info *te_info = NULL;

	te_info = container_of(work, struct osc_te_fitting_info, work);

	if (IS_ERR_OR_NULL(te_info)) {
		LCD_ERR("Invalid TE tuning data\n");
		return;
	}

	complete_all(&te_info->te_check_comp);
}

static irqreturn_t samsung_te_check_handler(int irq, void *handle)
{
	struct samsung_display_driver_data *vdd = NULL;
	struct osc_te_fitting_info *te_info = NULL;
	static bool skip_first_te = true;
	static u8 count;

	if (skip_first_te) {
		skip_first_te = false;
		goto end;
	}

	if (IS_ERR_OR_NULL(handle)) {
		LCD_ERR("handle is null\n");
		goto end;
	}

	te_info = &vdd->te_fitting_info;


	if (!(te_info->status & TE_CHECK_ENABLE))
		goto end;

	if (count < te_info->sampling_rate) {
		te_info->te_time[count++] =
			ktime_to_us(ktime_get());
	} else {
		disable_irq_nosync(gpio_to_irq(ss_get_te_gpio(vdd)));
		schedule_work(&te_info->work);
		skip_first_te = true;
		count = 0;
	}

end:
	return IRQ_HANDLED;
}

/*************************************************************
*
*		LDI FPS RELATED FUNCTION BELOW.
*
**************************************************************/
int ldi_fps(unsigned int input_fps)
{
	struct samsung_display_driver_data *vdd = samsung_get_vdd();
	int rc = 0;

	if (IS_ERR_OR_NULL(vdd)) {
		LCD_ERR("Invalid data vdd : 0x%zx\n", (size_t)vdd);
		return 0;
	}

	LCD_INFO("input_fps = %d\n", input_fps);

	if (!IS_ERR_OR_NULL(vdd->panel_func.samsung_change_ldi_fps))
		rc = vdd->panel_func.samsung_change_ldi_fps(vdd, input_fps);
	else {
		LCD_ERR("samsung_change_ldi_fps function is NULL\n");
		return 0;
	}

	if (rc)
		ss_send_cmd(vdd, TX_LDI_FPS_CHANGE);

	return rc;
}
EXPORT_SYMBOL(ldi_fps);

int ss_set_backlight(struct samsung_display_driver_data *vdd, u32 bl_lvl)
{
	struct dsi_panel *panel = NULL;
	int ret = 0;

	if (IS_ERR_OR_NULL(vdd)) {
		ret = -EINVAL;
		goto end;
	}

	panel = GET_DSI_PANEL(vdd);
	if (IS_ERR_OR_NULL(panel)) {
		LCD_ERR("panel is NULL\n");
		ret = -EINVAL;
		goto end;
	}

	ret = dsi_panel_set_backlight(panel, bl_lvl);

end:
	return ret;
}

/*************************************************************
*
*		HMT RELATED FUNCTION BELOW.
*
**************************************************************/
int hmt_bright_update(struct samsung_display_driver_data *vdd)
{
	if (vdd->hmt_stat.hmt_on) {
		ss_brightness_dcs_hmt(vdd, vdd->hmt_stat.hmt_bl_level);
	} else {
		ss_brightness_dcs(vdd, vdd->bl_level);
		LCD_INFO("hmt off state!\n");
	}

	return 0;
}

int hmt_enable(struct samsung_display_driver_data *vdd)
{
	LCD_INFO("[HMT] HMT %s\n", vdd->hmt_stat.hmt_on ? "ON" : "OFF");

	if (vdd->hmt_stat.hmt_on) {
		ss_send_cmd(vdd, TX_HMT_ENABLE);
	} else {
		ss_send_cmd(vdd, TX_HMT_DISABLE);
	}

	return 0;
}

int hmt_reverse_update(struct samsung_display_driver_data *vdd, int enable)
{
	LCD_INFO("[HMT] HMT %s\n", enable ? "REVERSE" : "FORWARD");

	if (enable)
		ss_send_cmd(vdd, TX_HMT_REVERSE);
	else
		ss_send_cmd(vdd, TX_HMT_FORWARD);

	return 0;
}

static void parse_dt_data(struct device_node *np, void *data, size_t size,
		char *cmd_string, char panel_rev,
		int (*fnc)(struct device_node *, void *, char *))
{
	char string[PARSE_STRING];
	int ret = 0;

	/* Generate string to parsing from DT */
	snprintf(string, PARSE_STRING, "%s%c", cmd_string, 'A' + panel_rev);

	ret = fnc(np, data, string);

	/* If there is no dtsi data for panel rev B ~ T,
	 * use valid previous panel rev dtsi data.
	 * TODO: Instead of copying all data from previous panel rev,
	 * copy only the pointer...
	 */
	if (ret && (panel_rev > 0))
		memcpy(data, (u8 *) data - size, size);
}

static void ss_panel_parse_dt_bright_tables(struct device_node *np,
		struct samsung_display_driver_data *vdd)
{
	struct samsung_display_dtsi_data *dtsi_data = &vdd->dtsi_data;
	int panel_rev;

	for (panel_rev = 0; panel_rev < SUPPORT_PANEL_REVISION; panel_rev++) {
		parse_dt_data(np, &dtsi_data->vint_map_table[panel_rev],
				sizeof(struct cmd_map),
				"samsung,vint_map_table_rev", panel_rev,
				ss_parse_panel_table); /* VINT TABLE */

		parse_dt_data(np, &dtsi_data->candela_map_table[panel_rev],
				sizeof(struct candela_map_table),
				"samsung,candela_map_table_rev", panel_rev,
				ss_parse_candella_mapping_table);

		parse_dt_data(np, &dtsi_data->aod_candela_map_table[panel_rev],
				sizeof(struct candela_map_table),
				"samsung,aod_candela_map_table_rev", panel_rev,
				ss_parse_candella_mapping_table);

		parse_dt_data(np, &dtsi_data->hbm_candela_map_table[panel_rev],
				sizeof(struct hbm_candela_map_table),
				"samsung,hbm_candela_map_table_rev", panel_rev,
				ss_parse_hbm_candella_mapping_table);

		parse_dt_data(np, &dtsi_data->pac_candela_map_table[panel_rev],
				sizeof(struct candela_map_table),
				"samsung,pac_candela_map_table_rev", panel_rev,
				ss_parse_pac_candella_mapping_table);

		parse_dt_data(np, &dtsi_data->pac_hbm_candela_map_table[panel_rev],
				sizeof(struct hbm_candela_map_table),
				"samsung,pac_hbm_candela_map_table_rev", panel_rev,
				ss_parse_hbm_candella_mapping_table);

		/* ACL */
		parse_dt_data(np, &dtsi_data->acl_map_table[panel_rev],
				sizeof(struct cmd_map),
				"samsung,acl_map_table_rev", panel_rev,
				ss_parse_panel_table); /* ACL TABLE */

		parse_dt_data(np, &dtsi_data->elvss_map_table[panel_rev],
				sizeof(struct cmd_map),
				"samsung,elvss_map_table_rev", panel_rev,
				ss_parse_panel_table); /* ELVSS TABLE */

		parse_dt_data(np, &dtsi_data->aid_map_table[panel_rev],
				sizeof(struct cmd_map),
				"samsung,aid_map_table_rev", panel_rev,
				ss_parse_panel_table); /* AID TABLE */

		parse_dt_data(np, &dtsi_data->smart_acl_elvss_map_table[panel_rev],
				sizeof(struct cmd_map),
				"samsung,smart_acl_elvss_map_table_rev", panel_rev,
				ss_parse_panel_table); /* TABLE */

		parse_dt_data(np, &dtsi_data->hmt_reverse_aid_map_table[panel_rev],
				sizeof(struct cmd_map),
				"samsung,hmt_reverse_aid_map_table_rev", panel_rev,
				ss_parse_panel_table); /* TABLE */

		parse_dt_data(np, &dtsi_data->hmt_candela_map_table[panel_rev],
				sizeof(struct candela_map_table),
				"samsung,hmt_candela_map_table_rev", panel_rev,
				ss_parse_candella_mapping_table);

		parse_dt_data(np, &dtsi_data->scaled_level_map_table[panel_rev],
				sizeof(struct candela_map_table),
				"samsung,scaled_level_map_table_rev", panel_rev,
				ss_parse_candella_mapping_table);
	}
}

static void ss_panel_pbaboot_config(struct device_node *np,
		struct samsung_display_driver_data *vdd)
{
	// TODO: set appropriate value to drm display members... not pinfo...
#if 0
	struct ss_panel_info *pinfo = NULL;
	struct samsung_display_driver_data *vdd = NULL;
	bool need_to_force_vidoe_mode = false;

	pinfo = &ctrl->panel_data.panel_info;
	vdd = check_valid_ctrl(ctrl);

	if (vdd->support_hall_ic) {
		/* check the lcd id for DISPLAY_1 and DISPLAY_2 */
		if (!ss_panel_attached(DISPLAY_1) && !ss_panel_attached(DISPLAY_2))
			need_to_force_vidoe_mode = true;
	} else {
		/* check the lcd id for DISPLAY_1 */
		if (!ss_panel_attached(DISPLAY_1))
			need_to_force_vidoe_mode = true;
	}

	/* Support PBA boot without lcd */
	if (need_to_force_vidoe_mode &&
			!IS_ERR_OR_NULL(pinfo) &&
			!IS_ERR_OR_NULL(vdd) &&
			(pinfo->mipi.mode == DSI_CMD_MODE)) {
		LCD_ERR("force VIDEO_MODE : %d\n", vdd->ndx);
		pinfo->type = MIPI_VIDEO_PANEL;
		pinfo->mipi.mode = DSI_VIDEO_MODE;
		pinfo->mipi.traffic_mode = DSI_BURST_MODE;
		pinfo->mipi.bllp_power_stop = true;
		pinfo->mipi.te_sel = 0;
		pinfo->mipi.vsync_enable = 0;
		pinfo->mipi.hw_vsync_mode = 0;
		pinfo->mipi.force_clk_lane_hs = true;
		pinfo->mipi.dst_format = DSI_VIDEO_DST_FORMAT_RGB888;

#if 0
		pinfo->cont_splash_enabled = false;
#else
		ss_set_seamless_mode(vdd);
#endif
		pinfo->mipi.lp11_init = false;

		vdd->support_mdnie_lite = false;
		vdd->mdnie_lcd_on_notifiy = false;
		vdd->support_mdnie_trans_dimming = false;
		vdd->mdnie_disable_trans_dimming = false;

		if (!IS_ERR_OR_NULL(vdd->panel_func.parsing_otherline_pdata) && ss_panel_attached(DISPLAY_1)) {
			vdd->panel_func.parsing_otherline_pdata = NULL;
			destroy_workqueue(vdd->other_line_panel_support_workq);
		}

		pinfo->esd_check_enabled = false;
		ctrl->on_cmds.link_state = DSI_LP_MODE;
		ctrl->off_cmds.link_state = DSI_LP_MODE;

#if 0
		/* To avoid underrun panic*/
		mdd->logd.xlog_enable = 0;
#else
#endif
		vdd->dtsi_data.samsung_osc_te_fitting = false;
	}
#endif
}

static void ss_panel_parse_dt(struct samsung_display_driver_data *vdd)
{
	int rc, i;
	u32 tmp[2];
	int len;
	char backlight_tft_gpio[] = "samsung,panel-backlight-tft-gpio1";
	const char *data;
	const __be32 *data_32;
	struct device_node *np;

	np = ss_get_panel_of(vdd);

	if (!np)
		return;

	/* Set LP11 init flag */
	vdd->dtsi_data.samsung_lp11_init = of_property_read_bool(np, "samsung,dsi-lp11-init");
	LCD_ERR("LP11 init %s\n",
		vdd->dtsi_data.samsung_lp11_init ? "enabled" : "disabled");

	rc = of_property_read_u32(np, "samsung,mdss-power-on-reset-delay-us", tmp);
	vdd->dtsi_data.samsung_power_on_reset_delay = (!rc ? tmp[0] : 0);

	rc = of_property_read_u32(np, "samsung,mdss-dsi-off-reset-delay-us", tmp);
	vdd->dtsi_data.samsung_dsi_off_reset_delay = (!rc ? tmp[0] : 0);

	/* Set esc clk 128M */
	vdd->dtsi_data.samsung_esc_clk_128M = of_property_read_bool(np, "samsung,esc-clk-128M");
	LCD_ERR("ESC CLK 128M %s\n",
		vdd->dtsi_data.samsung_esc_clk_128M ? "enabled" : "disabled");

	vdd->dtsi_data.panel_lpm_enable = of_property_read_bool(np, "samsung,panel-lpm-enable");
	LCD_ERR("alpm enable %s\n",
		vdd->dtsi_data.panel_lpm_enable ? "enabled" : "disabled");

	/* Set HALL IC */
	vdd->support_hall_ic  = of_property_read_bool(np, "samsung,mdss_dsi_hall_ic_enable");
	LCD_ERR("hall_ic %s\n", vdd->support_hall_ic ? "enabled" : "disabled");

	rc = of_property_read_u32(np, "samsung,mdss_dsi_lcd_flip_delay_ms", tmp);
		vdd->lcd_flip_delay_ms = (!rc ? tmp[0] : 0);
		LCD_ERR("lcd_flip_delay_ms %d\n", vdd->lcd_flip_delay_ms);

	/*Set OSC TE fitting flag */
	vdd->dtsi_data.samsung_osc_te_fitting =
		of_property_read_bool(np, "samsung,osc-te-fitting-enable");

	if (vdd->dtsi_data.samsung_osc_te_fitting) {
		rc = of_property_read_u32_array(np, "samsung,osc-te-fitting-cmd-index", tmp, 2);
		if (!rc) {
			vdd->dtsi_data.samsung_osc_te_fitting_cmd_index[0] =
				tmp[0];
			vdd->dtsi_data.samsung_osc_te_fitting_cmd_index[1] =
				tmp[1];
		}

		rc = of_property_read_u32(np, "samsung,osc-te-fitting-sampling-rate", tmp);

		vdd->te_fitting_info.sampling_rate = !rc ? tmp[0] : 2;

	}

	LCD_INFO("OSC TE fitting %s\n",
		vdd->dtsi_data.samsung_osc_te_fitting ? "enabled" : "disabled");

	/* Set HMT flag */
	vdd->dtsi_data.hmt_enabled = of_property_read_bool(np, "samsung,hmt_enabled");
	if (vdd->dtsi_data.hmt_enabled)
		vdd->dtsi_data.hmt_enabled = true;

	LCD_INFO("hmt %s\n",
		vdd->dtsi_data.hmt_enabled ? "enabled" : "disabled");

	/* TCON Clk On Support */
	vdd->dtsi_data.samsung_tcon_clk_on_support =
		of_property_read_bool(np, "samsung,tcon-clk-on-support");
	LCD_INFO("tcon clk on support: %s\n",
			vdd->dtsi_data.samsung_tcon_clk_on_support ?
			"enabled" : "disabled");

	/* Set TFT flag */
	vdd->mdnie_tuning_enable_tft = of_property_read_bool(np,
				"samsung,mdnie-tuning-enable-tft");
	vdd->dtsi_data.tft_common_support  = of_property_read_bool(np,
		"samsung,tft-common-support");

	LCD_INFO("tft_common_support %s\n",
	vdd->dtsi_data.tft_common_support ? "enabled" : "disabled");

	vdd->dtsi_data.tft_module_name = of_get_property(np,
		"samsung,tft-module-name", NULL);  /* for tft tablet */

	vdd->dtsi_data.panel_vendor = of_get_property(np,
		"samsung,panel-vendor", NULL);

	vdd->dtsi_data.disp_model = of_get_property(np,
		"samsung,disp-model", NULL);

	vdd->dtsi_data.backlight_gpio_config = of_property_read_bool(np,
		"samsung,backlight-gpio-config");

	LCD_INFO("backlight_gpio_config %s\n",
	vdd->dtsi_data.backlight_gpio_config ? "enabled" : "disabled");

	/* Factory Panel Swap*/
	vdd->dtsi_data.samsung_support_factory_panel_swap = of_property_read_bool(np,
		"samsung,support_factory_panel_swap");

	/* Set tft backlight gpio */
	for (i = 0; i < MAX_BACKLIGHT_TFT_GPIO; i++) {
		backlight_tft_gpio[strlen(backlight_tft_gpio) - 1] = '1' + i;
		vdd->dtsi_data.backlight_tft_gpio[i] =
				 of_get_named_gpio(np,
						backlight_tft_gpio, 0);
		if (!gpio_is_valid(vdd->dtsi_data.backlight_tft_gpio[i]))
			LCD_ERR("%d, backlight_tft_gpio gpio%d not specified\n",
							__LINE__, i+1);
		else
			LCD_ERR("tft gpio num : %d\n", vdd->dtsi_data.backlight_tft_gpio[i]);
	}

	/* Set Mdnie lite HBM_CE_TEXT_MDNIE mode used */
	vdd->dtsi_data.hbm_ce_text_mode_support = of_property_read_bool(np, "samsung,hbm_ce_text_mode_support");

	/* Set Backlight IC discharge time */
	rc = of_property_read_u32(np, "samsung,blic-discharging-delay-us", tmp);
	vdd->dtsi_data.blic_discharging_delay_tft = (!rc ? tmp[0] : 6);

	/* Set cabc delay time */
	rc = of_property_read_u32(np, "samsung,cabc-delay-us", tmp);
	vdd->dtsi_data.cabc_delay = (!rc ? tmp[0] : 6);

	/* IRC */
	vdd->samsung_support_irc = of_property_read_bool(np, "samsung,support_irc");

	/* Gram Checksum Test */
	vdd->gct.is_support = of_property_read_bool(np, "samsung,support_gct");
	LCD_DEBUG("vdd->gct.is_support = %d\n", vdd->gct.is_support);

	/* POC Driver */
	vdd->poc_driver.is_support = of_property_read_bool(np, "samsung,support_poc_driver");
	LCD_INFO("vdd->poc_drvier.is_support = %d\n", vdd->poc_driver.is_support);
	rc = of_property_read_u32(np, "samsung,poc_erase_delay_ms", tmp);
	vdd->poc_driver.erase_delay_ms = (!rc ? tmp[0] : 0);
	rc = of_property_read_u32(np, "samsung,poc_write_delay_us", tmp);
	vdd->poc_driver.write_delay_us = (!rc ? tmp[0] : 0);
	LCD_INFO("erase_delay_ms(%d) write_delay_us (%d)\n",
		vdd->poc_driver.erase_delay_ms, vdd->poc_driver.write_delay_us);

	/* PAC */
	vdd->pac = of_property_read_bool(np, "samsung,support_pac");
	LCD_INFO("vdd->pac = %d\n", vdd->pac);

	/* Set elvss_interpolation_temperature */
	data_32 = of_get_property(np, "samsung,elvss_interpolation_temperature", NULL);

	if (data_32)
		vdd->elvss_interpolation_temperature = (int)(be32_to_cpup(data_32));
	else
		vdd->elvss_interpolation_temperature = ELVSS_INTERPOLATION_TEMPERATURE;

	/* Set lux value for mdnie HBM */
	data_32 = of_get_property(np, "samsung,enter_hbm_lux", NULL);
	if (data_32)
		vdd->enter_hbm_lux = (int)(be32_to_cpup(data_32));
	else
		vdd->enter_hbm_lux = ENTER_HBM_LUX;

	/* Power Control for LPM */
	vdd->lpm_power_control = of_property_read_bool(np, "samsung,lpm-power-control");
	LCD_INFO("lpm_power_control %s\n", vdd->lpm_power_control ? "enabled" : "disabled");

	if (vdd->lpm_power_control) {
		rc = of_property_read_string(np, "samsung,lpm-power-control-supply-name", &data);
		if (rc)
			LCD_ERR("error reading lpm-power name. rc=%d\n", rc);
		else
			snprintf(vdd->lpm_power_control_supply_name,
				ARRAY_SIZE((vdd->lpm_power_control_supply_name)), "%s", data);

		data_32 = of_get_property(np, "samsung,lpm-power-control-supply-min-voltage", NULL);
		if (data_32)
			vdd->lpm_power_control_supply_min_voltage = (int)(be32_to_cpup(data_32));
		else
			LCD_ERR("error reading lpm-power min_voltage\n");

		data_32 = of_get_property(np, "samsung,lpm-power-control-supply-max-voltage", NULL);
		if (data_32)
			vdd->lpm_power_control_supply_max_voltage = (int)(be32_to_cpup(data_32));
		else
			LCD_ERR("error reading lpm-power max_voltage\n");

		LCD_INFO("lpm_power_control Supply Name=%s, Min=%d, Max=%d\n",
			vdd->lpm_power_control_supply_name, vdd->lpm_power_control_supply_min_voltage,
			vdd->lpm_power_control_supply_max_voltage);

		rc = of_property_read_string(np, "samsung,lpm-power-control-elvss-name", &data);
		if (rc)
			LCD_ERR("error reading lpm-power-elvss name. rc=%d\n", rc);
		else
			snprintf(vdd->lpm_power_control_elvss_name,
				ARRAY_SIZE((vdd->lpm_power_control_elvss_name)), "%s", data);

		data_32 = of_get_property(np, "samsung,lpm-power-control-elvss-lpm-voltage", NULL);
		if (data_32)
			vdd->lpm_power_control_elvss_lpm_voltage = (int)(be32_to_cpup(data_32));
		else
			LCD_ERR("error reading lpm-power-elvss lpm voltage\n");

		data_32 = of_get_property(np, "samsung,lpm-power-control-elvss-normal-voltage", NULL);
		if (data_32)
			vdd->lpm_power_control_elvss_normal_voltage = (int)(be32_to_cpup(data_32));
		else
			LCD_ERR("error reading lpm-power-elvss normal voltage\n");

		LCD_INFO("lpm_power_control ELVSS Name=%s, lpm=%d, normal=%d\n",
			vdd->lpm_power_control_elvss_name, vdd->lpm_power_control_elvss_lpm_voltage,
			vdd->lpm_power_control_elvss_normal_voltage);
	}

	/* Dynamic MIPI Clock */
	vdd->dyn_mipi_clk.is_support = of_property_read_bool(np, "samsung,support_dynamic_mipi_clk");
	LCD_INFO("vdd->dyn_mipi_clk.is_support = %d\n", vdd->dyn_mipi_clk.is_support);

	if (vdd->dyn_mipi_clk.is_support) {
		ss_parse_dyn_mipi_clk_sel_table(np, &vdd->dyn_mipi_clk.clk_sel_table,
					"samsung,dynamic_mipi_clk_sel_table");
		ss_parse_dyn_mipi_clk_timing_table(np, &vdd->dyn_mipi_clk.clk_timing_table,
					"samsung,dynamic_mipi_clk_timing_table");
		vdd->rf_info.notifier.priority = 0;
		vdd->rf_info.notifier.notifier_call = ss_rf_info_notify_callback;
		register_dev_ril_bridge_event_notifier(&vdd->rf_info.notifier);
	}

	ss_panel_parse_dt_bright_tables(np, vdd);
	ss_dsi_panel_parse_cmd_sets(vdd->dtsi_data.cmd_sets, np);

	if (vdd->support_hall_ic) {
		vdd->hall_ic_notifier_display.priority = 0; /* Tsp is 1, Touch key is 2 */
		vdd->hall_ic_notifier_display.notifier_call = samsung_display_hall_ic_status;
#if defined(CONFIG_FOLDER_HALL)
		hall_ic_register_notify(&vdd->hall_ic_notifier_display);
#endif
	}

	// LCD SELECT
	vdd->select_panel_gpio = of_get_named_gpio(np,
			"samsung,mdss_dsi_lcd_sel_gpio", 0);
	if (gpio_is_valid(vdd->select_panel_gpio))
		gpio_request(vdd->select_panel_gpio, "lcd_sel_gpio");

	vdd->select_panel_use_expander_gpio = of_property_read_bool(np, "samsung,mdss_dsi_lcd_sel_use_expander_gpio");

	/* Panel LPM */
	rc = of_property_read_u32(np, "samsung,lpm-init-delay-ms", tmp);
	vdd->dtsi_data.samsung_lpm_init_delay = (!rc ? tmp[0] : 0);

	rc = of_property_read_u32(np, "samsung,delayed-display-on", tmp);
	vdd->dtsi_data.samsung_delayed_display_on = (!rc ? tmp[0] : 0);

	ss_panel_parse_dt_esd(np, vdd);
	ss_panel_pbaboot_config(np, vdd);

	/* AOR & IRC INTERPOLATION (FLASH or NO FLAHS SUPPORT)*/
	data_32 = of_get_property(np, "samsung,hbm_brightness", &len);
			vdd->dtsi_data.hbm_brightness_step = (data_32 ? len/sizeof(len) : 0);
	data_32 = of_get_property(np, "samsung,normal_brightness", &len);
			vdd->dtsi_data.normal_brightness_step = (data_32 ? len/sizeof(len) : 0);
	data_32 = of_get_property(np, "samsung,hmd_brightness", &len);
			vdd->dtsi_data.hmd_brightness_step = (data_32 ? len/sizeof(len) : 0);
	LCD_INFO("gamma_step hbm: %d normal: %d hmt: %d\n",
		vdd->dtsi_data.hbm_brightness_step,
		vdd->dtsi_data.normal_brightness_step,
		vdd->dtsi_data.hmd_brightness_step);

	rc = of_property_read_u32(np, "samsung,gamma_size", tmp);
			vdd->dtsi_data.gamma_size = (!rc ? tmp[0] : 34);
	rc = of_property_read_u32(np, "samsung,aor_size", tmp);
			vdd->dtsi_data.aor_size = (!rc ? tmp[0] : 2);
	rc = of_property_read_u32(np, "samsung,vint_size", tmp);
			vdd->dtsi_data.vint_size = (!rc ? tmp[0] : 1);
	rc = of_property_read_u32(np, "samsung,elvss_size", tmp);
			vdd->dtsi_data.elvss_size = (!rc ? tmp[0] : 3);
	rc = of_property_read_u32(np, "samsung,irc_size", tmp);
			vdd->dtsi_data.irc_size = (!rc ? tmp[0] : 17);
	LCD_INFO("flash_gamma_size gamma:%d aor:%d vint:%d elvss:%d irc:%d\n",
		vdd->dtsi_data.gamma_size,
		vdd->dtsi_data.aor_size,
		vdd->dtsi_data.vint_size,
		vdd->dtsi_data.elvss_size,
		vdd->dtsi_data.irc_size);

	rc = of_property_read_u32(np, "samsung,flash_table_hbm_aor_offset", tmp);
	vdd->dtsi_data.flash_table_hbm_aor_offset = (!rc ? tmp[0] : 0x09D4);
	rc = of_property_read_u32(np, "samsung,flash_table_hbm_vint_offset", tmp);
	vdd->dtsi_data.flash_table_hbm_vint_offset = (!rc ? tmp[0] : 0x0A80);
	rc = of_property_read_u32(np, "samsung,flash_table_hbm_elvss_offset", tmp);
	vdd->dtsi_data.flash_table_hbm_elvss_offset = (!rc ? tmp[0] : 0x0AD6);
	rc = of_property_read_u32(np, "samsung,flash_table_hbm_irc_offset", tmp);
	vdd->dtsi_data.flash_table_hbm_irc_offset = (!rc ? tmp[0] : 0x0AD6);
	LCD_INFO("flash_table_hbm_addr aor: 0x%x vint:0x%x elvss 0x%x hbm:0x%x\n",
		vdd->dtsi_data.flash_table_hbm_aor_offset,
		vdd->dtsi_data.flash_table_hbm_vint_offset,
		vdd->dtsi_data.flash_table_hbm_elvss_offset,
		vdd->dtsi_data.flash_table_hbm_irc_offset);

	rc = of_property_read_u32(np, "samsung,flash_table_normal_gamma_offset", tmp);
	vdd->dtsi_data.flash_table_normal_gamma_offset = (!rc ? tmp[0] : 0x0000);
	rc = of_property_read_u32(np, "samsung,flash_table_normal_aor_offset", tmp);
	vdd->dtsi_data.flash_table_normal_aor_offset = (!rc ? tmp[0] : 0x09EC);
	rc = of_property_read_u32(np, "samsung,flash_table_normal_vint_offset", tmp);
	vdd->dtsi_data.flash_table_normal_vint_offset = (!rc ? tmp[0] : 0x0A8C);
	rc = of_property_read_u32(np, "samsung,flash_table_normal_elvss_offset", tmp);
	vdd->dtsi_data.flash_table_normal_elvss_offset = (!rc ? tmp[0] : 0x0AFA);
	rc = of_property_read_u32(np, "samsung,flash_table_normal_irc_offset", tmp);
	vdd->dtsi_data.flash_table_normal_irc_offset = (!rc ? tmp[0] : 0x0CA4);
	LCD_INFO("flash_table__normal_addr gamma:0x%x aor: 0x%x vint:0x%x elvss 0x%x hbm:0x%x\n",
		vdd->dtsi_data.flash_table_normal_gamma_offset,
		vdd->dtsi_data.flash_table_normal_aor_offset,
		vdd->dtsi_data.flash_table_normal_vint_offset,
		vdd->dtsi_data.flash_table_normal_elvss_offset,
		vdd->dtsi_data.flash_table_normal_irc_offset);

	rc = of_property_read_u32(np, "samsung,flash_table_hmd_gamma_offset", tmp);
	vdd->dtsi_data.flash_table_hmd_gamma_offset = (!rc ? tmp[0] : 0x118E);
	rc = of_property_read_u32(np, "samsung,flash_table_hmd_aor_offset", tmp);
	vdd->dtsi_data.flash_table_hmd_aor_offset = (!rc ? tmp[0] : 0x1678);
	LCD_INFO("flash_table_hmt_addr gamma:0x%x aor: 0x%x\n",
		vdd->dtsi_data.flash_table_hmd_gamma_offset,
		vdd->dtsi_data.flash_table_hmd_aor_offset);

	/* ONLY FLASH GAMMA */
	vdd->dtsi_data.flash_gamma_support = of_property_read_bool(np, "samsung,support_flash_gamma");

	if (vdd->dtsi_data.flash_gamma_support) {
		vdd->flash_br_workqueue = create_singlethread_workqueue("flash_br_workqueue");
		INIT_DELAYED_WORK(&vdd->flash_br_work, flash_br_work_func);

		rc = of_property_read_u32(np, "samsung,flash_gamma_write_check_addr", tmp);
		vdd->dtsi_data.flash_gamma_write_check_address = (!rc ? tmp[0] : 0x0A16C4);
		LCD_INFO("write_check_addr: 0x%x \n", vdd->dtsi_data.flash_gamma_write_check_address);

		data_32 = of_get_property(np, "samsung,flash_gamma_start_bank", &len);
		vdd->dtsi_data.flash_gamma_bank_start_len = (data_32 ? len/sizeof(len) : 3);
		data_32 = of_get_property(np, "samsung,flash_gamma_end_bank", &len);
		vdd->dtsi_data.flash_gamma_bank_end_len = (data_32 ? len/sizeof(len) : 3);

		vdd->dtsi_data.flash_gamma_bank_start = kzalloc(vdd->dtsi_data.flash_gamma_bank_start_len * sizeof(int *), GFP_KERNEL);
		vdd->dtsi_data.flash_gamma_bank_end = kzalloc(vdd->dtsi_data.flash_gamma_bank_end_len * sizeof(int *), GFP_KERNEL);
		rc = of_property_read_u32_array(np, "samsung,flash_gamma_start_bank",
					vdd->dtsi_data.flash_gamma_bank_start ,
					vdd->dtsi_data.flash_gamma_bank_start_len);
		if (rc)
			LCD_INFO("fail to get samsung,flash_gamma_start_bank\n");

		rc = of_property_read_u32_array(np, "samsung,flash_gamma_end_bank",
					vdd->dtsi_data.flash_gamma_bank_end,
					vdd->dtsi_data.flash_gamma_bank_end_len);
		if (rc)
			LCD_INFO("fail to get samsung,flash_gamma_end_bank\n");

		LCD_INFO("start_bank[0] : 0x%x end_bank[0]: 0x%x\n",
			vdd->dtsi_data.flash_gamma_bank_start[0],
			vdd->dtsi_data.flash_gamma_bank_end[0]);

		rc = of_property_read_u32(np, "samsung,flash_gamma_check_sum_start_offset", tmp);
		vdd->dtsi_data.flash_gamma_check_sum_start_offset = (!rc ? tmp[0] : 0x16C2);
		rc = of_property_read_u32(np, "samsung,flash_gamma_check_sum_end_offset", tmp);
		vdd->dtsi_data.flash_gamma_check_sum_end_offset = (!rc ? tmp[0] : 0x16C3);
		LCD_INFO("check_sum_start_offset : 0x%x check_sum_end_offset: 0x%x\n",
			vdd->dtsi_data.flash_gamma_check_sum_start_offset,
			vdd->dtsi_data.flash_gamma_check_sum_end_offset);

		rc = of_property_read_u32(np, "samsung,flash_gamma_0xc8_start_offset", tmp);
		vdd->dtsi_data.flash_gamma_0xc8_start_offset = (!rc ? tmp[0] : 0x2000);
		rc = of_property_read_u32(np, "samsung,flash_gamma_0xc8_end_offset", tmp);
		vdd->dtsi_data.flash_gamma_0xc8_end_offset = (!rc ? tmp[0] : 0x2021);
		rc = of_property_read_u32(np, "samsung,flash_gamma_0xc8_size", tmp);
		vdd->dtsi_data.flash_gamma_0xc8_size = (!rc ? tmp[0] : 34);
		rc = of_property_read_u32(np, "samsung,flash_gamma_0xc8_check_sum_start_addr", tmp);
		vdd->dtsi_data.flash_gamma_0xc8_check_sum_start_offset = (!rc ? tmp[0] : 0x2022);
		rc = of_property_read_u32(np, "samsung,flash_gamma_0xc8_check_sum_end_addr", tmp);
		vdd->dtsi_data.flash_gamma_0xc8_check_sum_end_offset = (!rc ? tmp[0] : 0x2023);
		LCD_INFO("flash_gamma 0xC8 start_addr:0x%x end_addr: 0x%x size: 0x%x check_sum_start : 0x%x check_sum_end: 0x%x\n",
			vdd->dtsi_data.flash_gamma_0xc8_start_offset,
			vdd->dtsi_data.flash_gamma_0xc8_end_offset,
			vdd->dtsi_data.flash_gamma_0xc8_size,
			vdd->dtsi_data.flash_gamma_0xc8_check_sum_start_offset,
			vdd->dtsi_data.flash_gamma_0xc8_check_sum_end_offset);

		rc = of_property_read_u32(np, "samsung,flash_MCD1_R_addr", tmp);
		vdd->dtsi_data.flash_MCD1_R_address = (!rc ? tmp[0] : 0xB8000);
		rc = of_property_read_u32(np, "samsung,flash_MCD2_R_addr", tmp);
		vdd->dtsi_data.flash_MCD2_R_address = (!rc ? tmp[0] : 0xB8001);
		rc = of_property_read_u32(np, "samsung,flash_MCD1_L_addr", tmp);
		vdd->dtsi_data.flash_MCD1_L_address = (!rc ? tmp[0] : 0xB8004);
		rc = of_property_read_u32(np, "samsung,flash_MCD2_L_addr", tmp);
		vdd->dtsi_data.flash_MCD2_L_address = (!rc ? tmp[0] : 0xB8005);
		LCD_INFO("flash_gamma MCD1_R:0x%x MCD2_R:0x%x MCD1_L:0x%x MCD2_L:0x%x\n",
			vdd->dtsi_data.flash_MCD1_R_address,
			vdd->dtsi_data.flash_MCD2_R_address,
			vdd->dtsi_data.flash_MCD1_L_address,
			vdd->dtsi_data.flash_MCD2_L_address);
	}
}


/***********/
/* A2 line */
/***********/

#define OTHER_PANEL_FILE "/efs/FactoryApp/a2_line.dat"

int read_line(char *src, char *buf, int *pos, int len)
{
	int idx = 0;

	LCD_DEBUG("(%d) ++\n", *pos);

	while (*(src + *pos) != 10 && *(src + *pos) != 13) {
		buf[idx] = *(src + *pos);

		idx++;
		(*pos)++;

		if (idx > MAX_READ_LINE_SIZE) {
			LCD_ERR("overflow!\n");
			return idx;
		}

		if (*pos >= len) {
			LCD_ERR("End of File (%d) / (%d)\n", *pos, len);
			return idx;
		}
	}

	while (*(src + *pos) == 10 || *(src + *pos) == 13)
		(*pos)++;

	LCD_DEBUG("--\n");

	return idx;
}

int ss_read_otherline_panel_data(struct samsung_display_driver_data *vdd)
{
	struct file *filp;
	char *dp;
	long l;
	loff_t pos;
	int ret = 0;
	mm_segment_t fs;

	fs = get_fs();
	set_fs(get_ds());

	filp = filp_open(OTHER_PANEL_FILE, O_RDONLY, 0);
	if (IS_ERR(filp)) {
		printk(KERN_ERR "%s File open failed\n", __func__);

		if (!IS_ERR_OR_NULL(vdd->panel_func.set_panel_fab_type))
			vdd->panel_func.set_panel_fab_type(BASIC_FB_PANLE_TYPE);/*to work as original line panel*/
		ret = -ENOENT;
		goto err;
	}

	l = filp->f_path.dentry->d_inode->i_size;
	LCD_INFO("Loading File Size : %ld(bytes)", l);

	dp = kmalloc(l + 10, GFP_KERNEL);
	if (dp == NULL) {
		LCD_INFO("Can't not alloc memory for tuning file load\n");
		filp_close(filp, current->files);
		ret = -1;
		goto err;
	}
	pos = 0;
	memset(dp, 0, l);

	LCD_INFO("before vfs_read()\n");
	ret = vfs_read(filp, (char __user *)dp, l, &pos);
	LCD_INFO("after vfs_read()\n");

	if (ret != l) {
		LCD_INFO("vfs_read() filed ret : %d\n", ret);
		kfree(dp);
		filp_close(filp, current->files);
		ret = -1;
		goto err;
	}

	if (!IS_ERR_OR_NULL(vdd->panel_func.parsing_otherline_pdata))
		ret = vdd->panel_func.parsing_otherline_pdata(filp, vdd, dp, l);

	filp_close(filp, current->files);

	set_fs(fs);

	kfree(dp);

	return ret;
err:
	set_fs(fs);
	return ret;
}

static void read_panel_data_work_fn(struct work_struct *work)
{
	struct samsung_display_driver_data *vdd =
		container_of(work, struct samsung_display_driver_data,
			other_line_panel_support_work.work);
	int ret = 1;

	ret = ss_read_otherline_panel_data(vdd);

	if (ret && vdd->other_line_panel_work_cnt) {
		queue_delayed_work(vdd->other_line_panel_support_workq,
				&vdd->other_line_panel_support_work,
				msecs_to_jiffies(OTHERLINE_WORKQ_DEALY));
		vdd->other_line_panel_work_cnt--;
	} else
		destroy_workqueue(vdd->other_line_panel_support_workq);

	if (vdd->other_line_panel_work_cnt == 0)
		LCD_ERR(" cnt (%d)\n", vdd->other_line_panel_work_cnt);
}

/*********************/
/* LPM control       */
/*********************/
void ss_panel_low_power_config(void *vdd_data, int enable)
{
	struct samsung_display_driver_data *vdd =
		(struct samsung_display_driver_data *)vdd_data;

	if (!vdd->dtsi_data.panel_lpm_enable) {
		LCD_INFO("[Panel LPM] LPM(ALPM/HLPM) is not supported\n");
		return;
	}

	ss_panel_lpm_power_ctrl(vdd, enable);

	ss_panel_lpm_ctrl(vdd, enable);

	if (enable) {
		vdd->esd_recovery.is_wakeup_source = true;
	} else {
		vdd->esd_recovery.is_wakeup_source = false;
	}

	if (vdd->esd_recovery.esd_irq_enable)
		vdd->esd_recovery.esd_irq_enable(true, true, (void *)vdd);
}

/*
 * ss_find_reg_offset()
 * This function find offset for reg value
 * reg_list[X][0] is reg value
 * reg_list[X][1] is offset for reg value
 * cmd_list is the target cmds for searching reg value
 */
int ss_find_reg_offset(int (*reg_list)[2],
		struct dsi_panel_cmd_set *cmd_list[], int list_size)
{
	struct dsi_panel_cmd_set *lpm_cmds = NULL;
	int i = 0, j = 0, max_cmd_cnt;

	if (IS_ERR_OR_NULL(reg_list) || IS_ERR_OR_NULL(cmd_list))
		goto end;

	for (i = 0; i < list_size; i++) {
		lpm_cmds = cmd_list[i];
		max_cmd_cnt = lpm_cmds->count;

		for (j = 0; j < max_cmd_cnt; j++) {
			if (lpm_cmds->cmds[j].msg.tx_buf &&
					lpm_cmds->cmds[j].msg.tx_buf[0] == reg_list[i][0]) {
				reg_list[i][1] = j;
				break;
			}
		}
	}

end:
	for (i = 0; i < list_size; i++)
		LCD_DEBUG("offset[%d] : %d\n", i, reg_list[i][1]);
	return 0;
}

static bool is_new_lpm_version(struct samsung_display_driver_data *vdd)
{
	if (vdd->panel_lpm.ver == LPM_VER1)
		return true;
	else
		return false;
}

static void set_lpm_br_values(struct samsung_display_driver_data *vdd)
{
	int from, end;
	int left, right, p = 0;
	int loop = 0;
	struct candela_map_table *table;
	int bl_level = vdd->bl_level;

	table = &vdd->dtsi_data.aod_candela_map_table[vdd->panel_revision];

	if (IS_ERR_OR_NULL(table->cd)) {
		LCD_ERR("No aod candela_map_table..\n");
		return;
	}

	LCD_DEBUG("table size (%d)\n", table->tab_size);

	if (bl_level > table->max_lv)
		bl_level = table->max_lv;

	left = 0;
	right = table->tab_size - 1;

	while (left <= right) {
		loop++;
		p = (left + right) / 2;
		from = table->from[p];
		end = table->end[p];
		LCD_DEBUG("[%d] from(%d) end(%d) / %d\n", p, from, end, bl_level);

		if (bl_level >= from && bl_level <= end)
			break;
		if (bl_level < from)
			right = p - 1;
		else
			left = p + 1;

		if (loop > table->tab_size) {
			pr_err("can not find (%d) level in table!\n", bl_level);
			p = table->tab_size - 1;
			break;
		}
	};
	vdd->panel_lpm.lpm_bl_level = table->cd[p];

	LCD_DEBUG("%s: (%d)->(%d)\n",
		__func__, vdd->bl_level, vdd->panel_lpm.lpm_bl_level);

}

int ss_panel_lpm_power_ctrl(struct samsung_display_driver_data *vdd, int enable)
{
	int i;
	int rc = 0;
	int get_voltage;
	struct regulator *elvss = NULL;
	struct dsi_panel *panel = NULL;
	struct dsi_vreg *target_vreg = NULL;
	struct dsi_regulator_info regs;

	if (!vdd->dtsi_data.panel_lpm_enable) {
		LCD_INFO("[Panel LPM] LPM(ALPM/HLPM) is not supported\n");
		return -ENODEV;
	}

	if (!vdd->lpm_power_control) {
		pr_err("%s: No panel power control for LPM\n", __func__);
		return -ENODEV;
	}

	panel = GET_DSI_PANEL(vdd);
	if (IS_ERR_OR_NULL(panel)) {
		pr_err("No Panel Data\n");
		return -ENODEV;
	}

	LCD_DEBUG("%s ++\n", enable == true ? "Enable" : "Disable");

	regs = panel->power_info;

	mutex_lock(&vdd->vdd_panel_lpm_lock);

	/* Find vreg for LPM setting */
	for (i = 0; i < regs.count; i++) {
		target_vreg = &regs.vregs[i];
		if (!strcmp(target_vreg->vreg_name, vdd->lpm_power_control_supply_name)) {
			LCD_INFO("Found Voltage(%d)\n", i);
			break;
		}
	}

	/* To check previous voltage */
	get_voltage = regulator_get_voltage(target_vreg->vreg);

	if (enable) { /* AOD ON(Enter) */
		if (get_voltage != vdd->lpm_power_control_supply_min_voltage) {
			rc = regulator_set_voltage(
					target_vreg->vreg,
					vdd->lpm_power_control_supply_min_voltage,
					vdd->lpm_power_control_supply_max_voltage);
			if (rc < 0) {
				LCD_ERR("Voltage Set Fail enable=%d voltage : %d rc : %d\n",
							enable, vdd->lpm_power_control_supply_min_voltage, rc);
			} else {
				get_voltage = regulator_get_voltage(target_vreg->vreg);
				LCD_INFO("enable=%d, current get_voltage=%d rc : %d\n", enable, get_voltage, rc);
			}

			if (strlen(vdd->lpm_power_control_elvss_name) > 0) {
				LCD_INFO("ELVSS Name(%s), Level(%d)\n", vdd->lpm_power_control_elvss_name,
								vdd->lpm_power_control_elvss_lpm_voltage);
				/* Elvss Regulator for Short Detection*/
				elvss = regulator_get(NULL, vdd->lpm_power_control_elvss_name);
				if (elvss) {
					rc = regulator_set_short_detection(elvss, true,
									vdd->lpm_power_control_elvss_lpm_voltage);
					if (rc < 0)
						LCD_ERR("Regulator Set for AOD Short Detection Fail\n");

					regulator_put(elvss);
				} else
					LCD_ERR("ELVSS Regulator Get Fail\n");
			} else
				LCD_ERR("No elvss name for lpm power control\n");
		} else
			LCD_DEBUG("enable=%d, previous voltage : %d\n", enable, get_voltage);

	} else { /* AOD OFF(Exit) */
		if (get_voltage != target_vreg->min_voltage) {
			rc = regulator_set_voltage(
						target_vreg->vreg,
						target_vreg->min_voltage,
						target_vreg->max_voltage);
			if (rc < 0) {
				LCD_ERR("Voltage Set Fail enable=%d voltage : %d rc : %d\n",
							enable, target_vreg->min_voltage, rc);
				panic("Voltage Set Fail to NORMAL");
			} else {
				get_voltage = regulator_get_voltage(target_vreg->vreg);
				LCD_INFO("enable=%d, current get_voltage=%d\n", enable, get_voltage);

				if (get_voltage != target_vreg->min_voltage)
					panic("Voltage Set Fail to NORMAL");
			}

			if (strlen(vdd->lpm_power_control_elvss_name) > 0) {
				LCD_INFO("ELVSS Name(%s), Level(%d)\n", vdd->lpm_power_control_elvss_name,
								vdd->lpm_power_control_elvss_normal_voltage);
				/* Elvss Regulator for Short Detection*/
				elvss = regulator_get(NULL, vdd->lpm_power_control_elvss_name);
				if (elvss) {
					rc = regulator_set_short_detection(elvss, true,
									vdd->lpm_power_control_elvss_normal_voltage);
					if (rc < 0)
						LCD_ERR("Regulator Set for Normal Short Detection Fail\n");

					regulator_put(elvss);
				} else
					LCD_ERR("ELVSS Regulator Get Fail\n");
			} else
				LCD_ERR("No elvss name for lpm power control\n");
		} else
			LCD_DEBUG("enable=%d, previous voltage : %d\n", enable, get_voltage);
	}

	mutex_unlock(&vdd->vdd_panel_lpm_lock);
	LCD_DEBUG("[Panel LPM] --\n");

	return rc;
}

void ss_panel_lpm_ctrl(struct samsung_display_driver_data *vdd, int enable)
{
	static int stored_bl_level; /* Used for factory mode only */
	int current_bl_level = 0;
	u32 lpm_init_delay = 0;

	LCD_INFO("[Panel LPM] ++\n");

	if (!vdd->dtsi_data.panel_lpm_enable) {
		LCD_INFO("[Panel LPM] LPM(ALPM/HLPM) is not supported\n");
		return;
	}

	if (ss_is_panel_off(vdd)) {
		LCD_INFO("[Panel LPM] Do not change mode\n");
		goto end;
	}

	lpm_init_delay = vdd->dtsi_data.samsung_lpm_init_delay;

	mutex_lock(&vdd->vdd_panel_lpm_lock);

	if (enable) { /* AOD ON(Enter) */
		if (unlikely(vdd->is_factory_mode)) {
			LCD_INFO("[Panel LPM] Set low brightness for factory mode (%d) \n", vdd->bl_level);
			stored_bl_level = vdd->bl_level;
			ss_brightness_dcs(vdd, 0);
		}

		if (!ss_is_panel_lpm(vdd)) {
			ss_send_cmd(vdd, TX_DISPLAY_OFF);
			LCD_INFO("[Panel LPM] Send panel DISPLAY_OFF cmds\n");
		} else {
			LCD_INFO("[Panel LPM] skip DISPLAY_OFF cmds\n");
		}

		if (vdd->panel_func.samsung_update_lpm_ctrl_cmd) {
			vdd->panel_func.samsung_update_lpm_ctrl_cmd(vdd);
			LCD_INFO("[Panel LPM] update lpm cmd done\n");
		}

		/* lpm init delay   */
		if (vdd->display_status_dsi.aod_delay == true) {
			vdd->display_status_dsi.aod_delay = false;
			if (lpm_init_delay)
				msleep(lpm_init_delay);
			LCD_INFO("%ums delay before turn on lpm mode",
					lpm_init_delay);
		}
		/* Self Display Setting */
		self_display_aod_enter();

		ss_send_cmd(vdd, TX_LPM_ON);
		LCD_INFO("[Panel LPM] Send panel LPM cmds\n");

		if (unlikely(vdd->is_factory_mode))
			ss_send_cmd(vdd, TX_DISPLAY_ON);
		else {
			/* The display_on cmd will be sent on next commit */
			vdd->display_status_dsi.wait_disp_on = true;
			vdd->display_status_dsi.wait_actual_disp_on = true;
			LCD_INFO("[Panel LPM] Set wait_disp_on to true\n");
		}

		vdd->panel_state = PANEL_PWR_LPM;

		/*
			Update mdnie to disable mdnie operation by scenario at AOD display status.
		*/
		if (vdd->support_mdnie_lite) {
			update_dsi_tcon_mdnie_register(vdd);
		}

	} else { /* AOD OFF(Exit) */
		/* Self Display Setting */
		self_display_aod_exit();

		/* Turn Off ALPM Mode */
		ss_send_cmd(vdd, TX_LPM_OFF);

		LCD_INFO("[Panel LPM] Send panel LPM off cmds\n");

		if (unlikely(vdd->is_factory_mode)) {
			LCD_INFO("[Panel LPM] restore bl_level for factory (%d) \n", stored_bl_level);
			current_bl_level = stored_bl_level;
		} else {
			current_bl_level = vdd->bl_level;
		}

		LCD_INFO("[Panel LPM] Restore brightness level (%d) \n", current_bl_level);

		vdd->panel_state = PANEL_PWR_ON;

		ss_brightness_dcs(vdd, current_bl_level);

		if (vdd->support_mdnie_lite) {
			vdd->mdnie_lcd_on_notifiy = true;
			update_dsi_tcon_mdnie_register(vdd);
			if (vdd->support_mdnie_trans_dimming)
				vdd->mdnie_disable_trans_dimming = false;
		}

		if (vdd->panel_func.samsung_cover_control && vdd->cover_control)
			vdd->panel_func.samsung_cover_control(vdd);

		if (unlikely(vdd->is_factory_mode)) {
			ss_send_cmd(vdd, TX_DISPLAY_ON);
			vdd->panel_state = PANEL_PWR_ON;
		} else {
			/* The display_on cmd will be sent on next commit */
			vdd->display_status_dsi.wait_disp_on = true;
			vdd->display_status_dsi.wait_actual_disp_on = true;
			LCD_INFO("[Panel LPM] Set wait_disp_on to true\n");
		}

		/* 1Frame Delay(33.4ms - 30FPS) Should be added */
		usleep_range(34*1000, 34*1000);
	}

	LCD_INFO("[Panel LPM] En/Dis : %s, LPM_MODE : %s, Hz : 30Hz, bl_level : %s\n",
				/* Enable / Disable */
				enable ? "Enable" : "Disable",
				/* Check LPM mode */
				vdd->panel_lpm.mode == ALPM_MODE_ON ? "ALPM" :
				vdd->panel_lpm.mode == HLPM_MODE_ON ? "HLPM" :
				vdd->panel_lpm.mode == LPM_MODE_OFF ? "MODE_OFF" : "UNKNOWN",
				/* Check current brightness level */
				vdd->panel_lpm.lpm_bl_level == LPM_2NIT ? "2NIT" :
				vdd->panel_lpm.lpm_bl_level == LPM_10NIT ? "10NIT" :
				vdd->panel_lpm.lpm_bl_level == LPM_30NIT ? "30NIT" :
				vdd->panel_lpm.lpm_bl_level == LPM_60NIT ? "60NIT" : "UNKNOWN");

	mutex_unlock(&vdd->vdd_panel_lpm_lock);
end:
	LCD_INFO("[Panel LPM] --\n");
}

// return primary vdd.. to be removed...
struct samsung_display_driver_data *samsung_get_vdd(void)
{
	return &vdd_data[PRIMARY_DISPLAY_NDX];
}

/*
 * @param:
 *	D0: hall_ic (the hall ic status, 0: folder open. 1: folder close)
 *	D8: flip_not_refresh (0: refresh after flipping. 1: don't refresh after flipping)
 */
int samsung_display_hall_ic_status(struct notifier_block *nb,
				unsigned long param, void *data)
{
	struct samsung_display_driver_data *vdd = samsung_get_vdd();
	bool hall_ic = (bool)(param & 0x1);
	//bool flip_not_refresh = (bool)(!!(param & LCD_FLIP_NOT_REFRESH));
	struct sde_connector *conn = GET_SDE_CONNECTOR(vdd);
	struct drm_event event;
	bool panel_dead = false;

	/*
		previous panel off -> current panel on
		foder open : 0, close : 1
	*/

	if (!vdd->support_hall_ic)
		return 0;

	LCD_ERR("mdss hall_ic : %s, start\n", hall_ic ? "CLOSE" : "OPEN");

#if 0
	if (ss_panel_attached(PRIMARY_DISPLAY_NDX) && ss_panel_attached(SECONDARY_DISPLAY_NDX)) {
		/* To check current blank mode */
		if ((ss_is_panel_on(vdd) ||
			ss_is_panel_lpm(vdd)) &&
			vdd->hall_ic_status != hall_ic) {

			/* set flag */
			vdd->hall_ic_mode_change_trigger = true;
			vdd->lcd_flip_not_refresh = flip_not_refresh;

			/* panel off */
			// TODO: off panel..
			// call msm_disable_outputs()..???
			LCD_ERR("should implement panel off...\n");

			/* set status */
			vdd->hall_ic_status = hall_ic;

			/* panel on */
			// TODO: off panel..
			// call complete_commit()..???
			LCD_ERR("should implement panel on...\n");

			/* clear flag */
			vdd->hall_ic_mode_change_trigger = false;
			vdd->lcd_flip_not_refresh = false;

			/* Brightness setting */
			// TODO: check if it controls right display in dual dsi or dual display...
			if (ss_is_bl_dcs(vdd))
				ss_brightness_dcs(vdd, vdd->bl_level);

			/* display on */
			if (ss_is_video_mode(vdd))
				ss_send_cmd(vdd, TX_DISPLAY_ON);

			/* refresh a frame to panel */
			if (ss_is_cmd_mode(vdd) && !flip_not_refresh) {
				/* TODO: refresh a frame to panel.. with drm msm code...
				   call complete_commit()..??
				fbi->fbops->fb_pan_display(&fbi->var, fbi);
				*/
				LCD_ERR("need to refresh a frame to panel.. with drm msm code...\n");
			}
		} else {
			vdd->hall_ic_status = hall_ic;
			LCD_ERR("mdss skip display changing\n");
		}
	} else {
#endif
	/* check the lcd id for DISPLAY_1 and DISPLAY_2 */
	if (ss_panel_attached(PRIMARY_DISPLAY_NDX) && ss_panel_attached(SECONDARY_DISPLAY_NDX)) {
		/* set status */
		vdd->hall_ic_status_unhandled = hall_ic;
		//vdd->panel_dead = true; panel switch, should not set panel_dead, else MTP is not read

		panel_dead = true;
		event.type = DRM_EVENT_PANEL_DEAD;
		event.length = sizeof(bool);
		msm_mode_object_event_notify(&conn->base.base,
			conn->base.dev, &event, (u8 *)&panel_dead);
		/* TODO: send flip_not_refresh to HAL, and let HAL to handle it. */
	} else {
		/* check the lcd id for DISPLAY_1 */
		if (ss_panel_attached(PRIMARY_DISPLAY_NDX))
			vdd->hall_ic_status = HALL_IC_OPEN;

		/* check the lcd id for DISPLAY_2 */
		if (ss_panel_attached(SECONDARY_DISPLAY_NDX))
			vdd->hall_ic_status = HALL_IC_CLOSE;
	}

	return 0;
}

static void samsung_display_delay_disp_on_work(struct work_struct *work)
{
	struct samsung_display_driver_data *vdd =
		container_of(work, struct samsung_display_driver_data,
				delay_disp_on_work.work);

	LCD_INFO("wait_disp_on is set\n");
	vdd->display_status_dsi.wait_disp_on = true;
	ss_send_cmd(vdd, TX_DISPLAY_ON);
}

int get_hall_ic_status(char *mode)
{
	struct samsung_display_driver_data *vdd = samsung_get_vdd();

	if (mode == NULL)
		return true;

	if (*mode - '0')
		vdd->hall_ic_status = HALL_IC_CLOSE;
	else
		vdd->hall_ic_status = HALL_IC_OPEN;

	LCD_ERR("hall_ic : %s\n", vdd->hall_ic_status ? "CLOSE" : "OPEN");

	return true;
}
EXPORT_SYMBOL(get_hall_ic_status);
__setup("hall_ic=0x", get_hall_ic_status);

/***************************************************************************************************
*		BRIGHTNESS RELATED FUNCTION.
****************************************************************************************************/
static void set_normal_br_values(struct samsung_display_driver_data *vdd)
{
	int from, end;
	int left, right, p = 0;
	int loop = 0;
	struct candela_map_table *table;

	if (vdd->pac)
		table = &vdd->dtsi_data.pac_candela_map_table[vdd->panel_revision];
	else
		table = &vdd->dtsi_data.candela_map_table[vdd->panel_revision];

	if (IS_ERR_OR_NULL(table->cd)) {
		LCD_ERR("No candela_map_table..\n");
		return;
	}

	LCD_DEBUG("table size (%d)\n", table->tab_size);

	if (vdd->bl_level > table->max_lv)
		vdd->bl_level = table->max_lv;

	left = 0;
	right = table->tab_size - 1;

	while (left <= right) {
		loop++;
		p = (left + right) / 2;
		from = table->from[p];
		end = table->end[p];
		LCD_DEBUG("[%d] from(%d) end(%d) / %d\n", p, from, end, vdd->bl_level);

		if (vdd->bl_level >= from && vdd->bl_level <= end)
			break;
		if (vdd->bl_level < from)
			right = p - 1;
		else
			left = p + 1;

		if (loop > table->tab_size) {
			pr_err("can not find (%d) level in table!\n", vdd->bl_level);
			p = table->tab_size - 1;
			break;
		}
	};

	// for elvess, vint etc.. which are using 74 steps.
	vdd->candela_level = table->cd[p];
	vdd->cd_idx = table->idx[p];

	if (vdd->pac) {
		vdd->pac_cd_idx = table->scaled_idx[p];
		vdd->interpolation_cd = table->interpolation_cd[p];
		LCD_INFO("pac_cd_idx (%d) cd_idx (%d) cd (%d) interpolation_cd (%d)\n",
			vdd->pac_cd_idx, vdd->cd_idx, vdd->candela_level, vdd->interpolation_cd);
	} else
		LCD_INFO("cd_idx (%d) candela_level (%d) \n", vdd->cd_idx, vdd->candela_level);

	return;
}

static void set_hbm_br_values(struct samsung_display_driver_data *vdd)
{
	int from, end;
	int left, right, p;
	int loop = 0;
	struct hbm_candela_map_table *table;

	if (vdd->pac)
		table = &vdd->dtsi_data.pac_hbm_candela_map_table[vdd->panel_revision];
	else
		table = &vdd->dtsi_data.hbm_candela_map_table[vdd->panel_revision];

	if (IS_ERR_OR_NULL(table->cd)) {
		LCD_ERR("No hbm_candela_map_table..\n");
		return;
	}

	if (vdd->bl_level > table->hbm_max_lv)
		vdd->bl_level = table->hbm_max_lv;

	left = 0;
	right = table->tab_size - 1;

	while (1) {
		loop++;
		p = (left + right) / 2;
		from = table->from[p];
		end = table->end[p];
		LCD_DEBUG("[%d] from(%d) end(%d) / %d\n", p, from, end, vdd->bl_level);

		if (vdd->bl_level >= from && vdd->bl_level <= end)
			break;
		if (vdd->bl_level < from)
			right = p - 1;
		else
			left = p + 1;
		LCD_DEBUG("left(%d) right(%d)\n", left, right);

		if (loop > table->tab_size) {
			pr_err("can not find (%d) level in table!\n", vdd->bl_level);
			p = table->tab_size - 1;
			break;
		}
	};

	// set values..
	vdd->interpolation_cd = vdd->candela_level = table->cd[p];
	vdd->auto_brightness = table->auto_level[p];

	LCD_INFO("[%d] candela_level (%d) auto_brightness (%d) \n", p, vdd->candela_level, vdd->auto_brightness);

	return;
}

static void ss_update_brightness_packet(struct dsi_cmd_desc *packet,
		int *count, struct dsi_panel_cmd_set *tx_cmd)
{
	int loop = 0;

	if (IS_ERR_OR_NULL(packet)) {
		LCD_ERR("%ps no packet\n", __builtin_return_address(0));
		return;
	}

	if (IS_ERR_OR_NULL(tx_cmd)) {
		LCD_ERR("%ps no tx_cmd\n", __builtin_return_address(0));
		return;
	}

	if (*count > (BRIGHTNESS_MAX_PACKET - 1))
		panic("over max brightness_packet size(%d).. !!",
				BRIGHTNESS_MAX_PACKET);

	for (loop = 0; loop < tx_cmd->count; loop++)
		packet[(*count)++] = tx_cmd->cmds[loop];
}

void update_packet_level_key_enable(struct samsung_display_driver_data *vdd,
		struct dsi_cmd_desc *packet, int *cmd_cnt, int level_key)
{
	if (!level_key)
		return;
	else {
		if (level_key & LEVEL0_KEY)
			ss_update_brightness_packet(packet, cmd_cnt, ss_get_cmds(vdd, TX_LEVEL0_KEY_ENABLE));

		if (level_key & LEVEL1_KEY)
			ss_update_brightness_packet(packet, cmd_cnt, ss_get_cmds(vdd, TX_LEVEL1_KEY_ENABLE));

		if (level_key & LEVEL2_KEY)
			ss_update_brightness_packet(packet, cmd_cnt, ss_get_cmds(vdd, TX_LEVEL2_KEY_ENABLE));
	}
}

void update_packet_level_key_disable(struct samsung_display_driver_data *vdd,
		struct dsi_cmd_desc *packet, int *cmd_cnt, int level_key)
{
	if (!level_key)
		return;
	else {
		if (level_key & LEVEL0_KEY)
			ss_update_brightness_packet(packet, cmd_cnt, ss_get_cmds(vdd, TX_LEVEL0_KEY_DISABLE));

		if (level_key & LEVEL1_KEY)
			ss_update_brightness_packet(packet, cmd_cnt, ss_get_cmds(vdd, TX_LEVEL1_KEY_DISABLE));

		if (level_key & LEVEL2_KEY)
			ss_update_brightness_packet(packet, cmd_cnt, ss_get_cmds(vdd, TX_LEVEL2_KEY_DISABLE));
	}
}

static int ss_hbm_brightness_packet_set(
		struct samsung_display_driver_data *vdd)
{
	int cmd_cnt = 0;
	int level_key = 0;
	struct dsi_panel_cmd_set *set;
	struct dsi_cmd_desc *packet = NULL;
	struct dsi_panel_cmd_set *tx_cmd = NULL;

	/* init packet */
	set = ss_get_cmds(vdd, TX_BRIGHT_CTRL);
	packet = set->cmds;

	set_hbm_br_values(vdd);

	/* IRC */
	if (!IS_ERR_OR_NULL(vdd->panel_func.samsung_hbm_irc)) {
		level_key = false;
		tx_cmd = vdd->panel_func.samsung_hbm_irc(vdd, &level_key);

		update_packet_level_key_enable(vdd, packet, &cmd_cnt, level_key);
		ss_update_brightness_packet(packet, &cmd_cnt, tx_cmd);
		update_packet_level_key_disable(vdd, packet, &cmd_cnt, level_key);
	}

	/* Gamma */
	if (ss_get_cmds(vdd, TX_HBM_GAMMA)->count) {
		if (!IS_ERR_OR_NULL(vdd->panel_func.samsung_hbm_gamma)) {
			level_key = false;
			tx_cmd = vdd->panel_func.samsung_hbm_gamma(vdd, &level_key);

			update_packet_level_key_enable(vdd, packet, &cmd_cnt, level_key);
			ss_update_brightness_packet(packet, &cmd_cnt, tx_cmd);
			update_packet_level_key_disable(vdd, packet, &cmd_cnt, level_key);
		}
	}

	/* hbm etc */
	if (ss_get_cmds(vdd, TX_HBM_ETC)->count) {
		if (!IS_ERR_OR_NULL(vdd->panel_func.samsung_hbm_etc)) {
			level_key = false;
			tx_cmd = vdd->panel_func.samsung_hbm_etc(vdd, &level_key);

			update_packet_level_key_enable(vdd, packet, &cmd_cnt, level_key);
			ss_update_brightness_packet(packet, &cmd_cnt, tx_cmd);
			update_packet_level_key_disable(vdd, packet, &cmd_cnt, level_key);
		}
	}

	return cmd_cnt;
}

static int ss_normal_brightness_packet_set(
		struct samsung_display_driver_data *vdd)
{
	int cmd_cnt = 0;
	int level_key = 0;
	struct dsi_panel_cmd_set *set;
	struct dsi_cmd_desc *packet = NULL;
	struct dsi_panel_cmd_set *tx_cmd = NULL;

	/* init packet */
	set = ss_get_cmds(vdd, TX_BRIGHT_CTRL);
	packet = set->cmds;

	vdd->auto_brightness = 0;

	set_normal_br_values(vdd);

	if (vdd->smart_dimming_loaded_dsi) { /* OCTA PANEL */
		/* hbm off */
		if (vdd->display_status_dsi.hbm_mode) {
			if (!IS_ERR_OR_NULL(vdd->panel_func.samsung_brightness_hbm_off)) {
				level_key = false;
				tx_cmd = vdd->panel_func.samsung_brightness_hbm_off(vdd, &level_key);

				update_packet_level_key_enable(vdd, packet, &cmd_cnt, level_key);
				ss_update_brightness_packet(packet, &cmd_cnt, tx_cmd);
				update_packet_level_key_disable(vdd, packet, &cmd_cnt, level_key);
			}
		}

		/* aid/aor */
		if (!IS_ERR_OR_NULL(vdd->panel_func.samsung_brightness_aid)) {
			level_key = false;
			tx_cmd = vdd->panel_func.samsung_brightness_aid(vdd, &level_key);

			update_packet_level_key_enable(vdd, packet, &cmd_cnt, level_key);
			ss_update_brightness_packet(packet, &cmd_cnt, tx_cmd);
			update_packet_level_key_disable(vdd, packet, &cmd_cnt, level_key);
		}

		/* acl */
		if (vdd->acl_status || vdd->siop_status) {
			if (!IS_ERR_OR_NULL(vdd->panel_func.samsung_brightness_acl_on)) {
				level_key = false;
				tx_cmd = vdd->panel_func.samsung_brightness_acl_on(vdd, &level_key);

				update_packet_level_key_enable(vdd, packet, &cmd_cnt, level_key);
				ss_update_brightness_packet(packet, &cmd_cnt, tx_cmd);
				update_packet_level_key_disable(vdd, packet, &cmd_cnt, level_key);
			}

			if (!IS_ERR_OR_NULL(vdd->panel_func.samsung_brightness_pre_acl_percent)) {
				level_key = false;
				tx_cmd = vdd->panel_func.samsung_brightness_pre_acl_percent(vdd, &level_key);

				update_packet_level_key_enable(vdd, packet, &cmd_cnt, level_key);
				ss_update_brightness_packet(packet, &cmd_cnt, tx_cmd);
				update_packet_level_key_disable(vdd, packet, &cmd_cnt, level_key);
			}

			if (!IS_ERR_OR_NULL(vdd->panel_func.samsung_brightness_acl_percent)) {

				level_key = false;
				tx_cmd = vdd->panel_func.samsung_brightness_acl_percent(vdd, &level_key);

				update_packet_level_key_enable(vdd, packet, &cmd_cnt, level_key);
				ss_update_brightness_packet(packet, &cmd_cnt, tx_cmd);
				update_packet_level_key_disable(vdd, packet, &cmd_cnt, level_key);
			}
		} else {
			if (!IS_ERR_OR_NULL(vdd->panel_func.samsung_brightness_acl_off)) {
				level_key = false;
				tx_cmd = vdd->panel_func.samsung_brightness_acl_off(vdd, &level_key);

				update_packet_level_key_enable(vdd, packet, &cmd_cnt, level_key);
				ss_update_brightness_packet(packet, &cmd_cnt, tx_cmd);
				update_packet_level_key_disable(vdd, packet, &cmd_cnt, level_key);
			}
		}

		/* elvss */
		if (!IS_ERR_OR_NULL(vdd->panel_func.samsung_brightness_elvss)) {
			if (!IS_ERR_OR_NULL(vdd->panel_func.samsung_brightness_pre_elvss)) {
				level_key = false;
				tx_cmd = vdd->panel_func.samsung_brightness_pre_elvss(vdd, &level_key);

				update_packet_level_key_enable(vdd, packet, &cmd_cnt, level_key);
				ss_update_brightness_packet(packet, &cmd_cnt, tx_cmd);
				update_packet_level_key_disable(vdd, packet, &cmd_cnt, level_key);
			}

			level_key = false;
			tx_cmd = vdd->panel_func.samsung_brightness_elvss(vdd, &level_key);

			update_packet_level_key_enable(vdd, packet, &cmd_cnt, level_key);
			ss_update_brightness_packet(packet, &cmd_cnt, tx_cmd);
			update_packet_level_key_disable(vdd, packet, &cmd_cnt, level_key);
		}

		/* temperature elvss */
		if (!IS_ERR_OR_NULL(vdd->panel_func.samsung_brightness_elvss_temperature1)) {
			level_key = false;
			tx_cmd = vdd->panel_func.samsung_brightness_elvss_temperature1(vdd, &level_key);

			update_packet_level_key_enable(vdd, packet, &cmd_cnt, level_key);
			ss_update_brightness_packet(packet, &cmd_cnt, tx_cmd);
			update_packet_level_key_disable(vdd, packet, &cmd_cnt, level_key);
		}

		if (!IS_ERR_OR_NULL(vdd->panel_func.samsung_brightness_elvss_temperature2)) {
			level_key = false;
			tx_cmd = vdd->panel_func.samsung_brightness_elvss_temperature2(vdd, &level_key);

			update_packet_level_key_enable(vdd, packet, &cmd_cnt, level_key);
			ss_update_brightness_packet(packet, &cmd_cnt, tx_cmd);
			update_packet_level_key_disable(vdd, packet, &cmd_cnt, level_key);
		}

		/* caps*/
		if (!IS_ERR_OR_NULL(vdd->panel_func.samsung_brightness_caps)) {
			if (!IS_ERR_OR_NULL(vdd->panel_func.samsung_brightness_pre_caps)) {
				level_key = false;
				tx_cmd = vdd->panel_func.samsung_brightness_pre_caps(vdd, &level_key);

				update_packet_level_key_enable(vdd, packet, &cmd_cnt, level_key);
				ss_update_brightness_packet(packet, &cmd_cnt, tx_cmd);
				update_packet_level_key_disable(vdd, packet, &cmd_cnt, level_key);
			}
			level_key = false;
			tx_cmd = vdd->panel_func.samsung_brightness_caps(vdd, &level_key);

			update_packet_level_key_enable(vdd, packet, &cmd_cnt, level_key);
			ss_update_brightness_packet(packet, &cmd_cnt, tx_cmd);
			update_packet_level_key_disable(vdd, packet, &cmd_cnt, level_key);
		}

		/* vint */
		if (!IS_ERR_OR_NULL(vdd->panel_func.samsung_brightness_vint)) {
			level_key = false;
			tx_cmd = vdd->panel_func.samsung_brightness_vint(vdd, &level_key);

			update_packet_level_key_enable(vdd, packet, &cmd_cnt, level_key);
			ss_update_brightness_packet(packet, &cmd_cnt, tx_cmd);
			update_packet_level_key_disable(vdd, packet, &cmd_cnt, level_key);
		}

		/* IRC */
		if (!IS_ERR_OR_NULL(vdd->panel_func.samsung_brightness_irc)) {
			level_key = false;
			tx_cmd = vdd->panel_func.samsung_brightness_irc(vdd, &level_key);

			update_packet_level_key_enable(vdd, packet, &cmd_cnt, level_key);
			ss_update_brightness_packet(packet, &cmd_cnt, tx_cmd);
			update_packet_level_key_disable(vdd, packet, &cmd_cnt, level_key);
		}

		/* gamma */
		if (!IS_ERR_OR_NULL(vdd->panel_func.samsung_brightness_gamma)) {
			level_key = false;
			tx_cmd = vdd->panel_func.samsung_brightness_gamma(vdd, &level_key);

			update_packet_level_key_enable(vdd, packet, &cmd_cnt, level_key);
			ss_update_brightness_packet(packet, &cmd_cnt, tx_cmd);
			update_packet_level_key_disable(vdd, packet, &cmd_cnt, level_key);
		}
	} else { /* TFT PANEL */
		if (!IS_ERR_OR_NULL(vdd->panel_func.samsung_brightness_tft_pwm_ldi)) {
			level_key = false;
			tx_cmd = vdd->panel_func.samsung_brightness_tft_pwm_ldi(vdd, &level_key);

			update_packet_level_key_enable(vdd, packet, &cmd_cnt, level_key);
			ss_update_brightness_packet(packet, &cmd_cnt, tx_cmd);
			update_packet_level_key_disable(vdd, packet, &cmd_cnt, level_key);
		}
	}

	return cmd_cnt;
}

int ss_single_transmission_packet(struct dsi_panel_cmd_set *cmds)
{
	int loop;
	struct dsi_cmd_desc *packet = cmds->cmds;
	int packet_cnt = cmds->count;

	for (loop = 0; (loop < packet_cnt) && (loop < BRIGHTNESS_MAX_PACKET); loop++) {
		if (packet[loop].msg.type == MIPI_DSI_DCS_LONG_WRITE ||
			packet[loop].msg.type == MIPI_DSI_GENERIC_LONG_WRITE)
			packet[loop].last_command = false;
		else {
			if (loop > 0)
				packet[loop - 1].last_command = true; /*To ensure previous single tx packet */

			packet[loop].last_command = true;
		}
	}

	if (loop == BRIGHTNESS_MAX_PACKET)
		return false;
	else {
		packet[loop - 1].last_command = true; /* To make last packet flag */
		return true;
	}
}

static bool is_hbm_level(struct samsung_display_driver_data *vdd)
{

	struct hbm_candela_map_table *table;

	if (vdd->pac)
		table = &vdd->dtsi_data.pac_hbm_candela_map_table[vdd->panel_revision];
	else
		table = &vdd->dtsi_data.hbm_candela_map_table[vdd->panel_revision];

	if (vdd->bl_level < table->hbm_min_lv)
		return false;

	if (vdd->bl_level > table->hbm_max_lv) {
		LCD_ERR("bl_level(%d) is over max_level (%d), force set to max\n", vdd->bl_level, table->hbm_max_lv);
		vdd->bl_level = table->hbm_max_lv;
	}

	return true;
}

/* ss_brightness_dcs() is called not in locking status.
 * Instead, calls ss_set_backlight() when you need to controll backlight
 * in locking status.
 */
int ss_brightness_dcs(struct samsung_display_driver_data *vdd, int level)
{
	int cmd_cnt = 0;
	int ret = 0;
	int need_lpm_lock = 0;
	struct dsi_panel_cmd_set *brightness_cmds = NULL;
	struct dsi_panel *panel = GET_DSI_PANEL(vdd);

	/* FC2 change: set panle mode in SurfaceFlinger initialization, instead of kenrel booting... */
	if (!panel->cur_mode) {
		LCD_ERR("err: no panel mode yet...\n");
		return false;
	}

	/*
	 * vdd_panel_lpm_lock should be locked
	 * to avoid brightness mis-handling (LPM brightness <-> normal brightness) from other thread
	 * but running ss_panel_lpm_ctrl thread
	*/
	if (ss_is_panel_lpm(vdd)) need_lpm_lock = 1;
	if (need_lpm_lock) mutex_lock(&vdd->vdd_panel_lpm_lock);

	// need bl_lock..
	mutex_lock(&vdd->bl_lock);

	vdd->bl_level = level;

	/* check the lcd id for DISPLAY_1 or DISPLAY_2 */
	if (!ss_panel_attached(vdd->ndx))
		goto skip_bl_update;

	if (vdd->dtsi_data.flash_gamma_support &&
		!vdd->panel_br_info.flash_data.init_done) {
		LCD_ERR("flash_gamme not ready\n");
		goto skip_bl_update;
	}

	if (vdd->dtsi_data.panel_lpm_enable && is_new_lpm_version(vdd)) {
		set_lpm_br_values(vdd);

		if (ss_is_panel_lpm(vdd)) {
			LCD_ERR("[Panel LPM]: set brightness.(%d)->(%d)\n", vdd->bl_level, vdd->panel_lpm.lpm_bl_level);

			if (vdd->panel_func.samsung_set_lpm_brightness)
				vdd->panel_func.samsung_set_lpm_brightness(vdd);
			goto skip_bl_update;
		}
	}

	if (vdd->dtsi_data.hmt_enabled && vdd->hmt_stat.hmt_on) {
		LCD_ERR("HMT is on. do not set normal brightness..(%d)\n", level);
		goto skip_bl_update;
	}

	if (ss_is_seamless_mode(vdd)) {
		LCD_ERR("splash is not done..\n");
		goto skip_bl_update;
	}

	if (is_hbm_level(vdd) && !vdd->dtsi_data.tft_common_support) {
		cmd_cnt = ss_hbm_brightness_packet_set(vdd);
		cmd_cnt > 0 ? vdd->display_status_dsi.hbm_mode = true : false;
	} else {
		cmd_cnt = ss_normal_brightness_packet_set(vdd);
		cmd_cnt > 0 ? vdd->display_status_dsi.hbm_mode = false : false;
	}

	if (cmd_cnt) {
		/* setting tx cmds cmt */
		brightness_cmds = ss_get_cmds(vdd, TX_BRIGHT_CTRL);
		brightness_cmds->count = cmd_cnt;

		/* generate single tx packet */
		ret = ss_single_transmission_packet(brightness_cmds);

		/* sending tx cmds */
		if (ret) {
			ss_send_cmd(vdd, TX_BRIGHT_CTRL);

			if (!IS_ERR_OR_NULL(ss_get_cmds(vdd, TX_BLIC_DIMMING)->cmds)) {
				if (vdd->bl_level == 0)
					ss_get_cmds(vdd, TX_BLIC_DIMMING)->cmds->msg.tx_buf[1] = 0x24;
				else
					ss_get_cmds(vdd, TX_BLIC_DIMMING)->cmds->msg.tx_buf[1] = 0x2C;

				ss_send_cmd(vdd, TX_BLIC_DIMMING);
			}

			// copr sum after changing brightness to calculate brightness avg.
			if (vdd->copr.copr_on) {
				ss_set_copr_sum(vdd, COPR_CD_INDEX_0);
				ss_set_copr_sum(vdd, COPR_CD_INDEX_1);
			}

			LCD_INFO("level : %d  candela : %dCD hbm : %d (%d) itp : %s\n",
				vdd->bl_level, vdd->candela_level, vdd->display_status_dsi.hbm_mode, vdd->auto_brightness,
				vdd->panel_br_info.itp_mode == 0 ? "TABLE" : "FLASH");
		} else
			LCD_INFO("single_transmission_fail error\n");
	} else
		LCD_INFO("level : %d skip\n", vdd->bl_level);

	if (vdd->auto_brightness >= HBM_CE_MODE &&
			!vdd->dtsi_data.tft_common_support &&
			vdd->support_mdnie_lite)
		update_dsi_tcon_mdnie_register(vdd);

skip_bl_update:
	mutex_unlock(&vdd->bl_lock);

	if (need_lpm_lock) mutex_unlock(&vdd->vdd_panel_lpm_lock);

	return 0;
}

// HMT brightness
static void set_hmt_br_values(struct samsung_display_driver_data *vdd)
{
	int from, end;
	int left, right, p = 0;
	struct candela_map_table *table;

	table = &vdd->dtsi_data.hmt_candela_map_table[vdd->panel_revision];

	if (IS_ERR_OR_NULL(table->cd)) {
		LCD_ERR("No candela_map_table..\n");
		return;
	}

	LCD_DEBUG("table size (%d)\n", table->tab_size);

	if (vdd->hmt_stat.hmt_bl_level > table->max_lv)
		vdd->hmt_stat.hmt_bl_level = table->max_lv;

	left = 0;
	right = table->tab_size - 1;

	while (left <= right) {
		p = (left + right) / 2;
		from = table->from[p];
		end = table->end[p];
		LCD_DEBUG("[%d] from(%d) end(%d) / %d\n", p, from, end, vdd->hmt_stat.hmt_bl_level);

		if (vdd->hmt_stat.hmt_bl_level >= from && vdd->hmt_stat.hmt_bl_level <= end)
			break;
		if (vdd->hmt_stat.hmt_bl_level < from)
			right = p - 1;
		else
			left = p + 1;
	};

	// for elvess, vint etc.. which are using 74 steps.
	vdd->interpolation_cd = vdd->hmt_stat.candela_level_hmt = table->cd[p];
	vdd->hmt_stat.cmd_idx_hmt = table->idx[p];

	LCD_INFO("cd_idx (%d) candela_level (%d) \n", vdd->hmt_stat.cmd_idx_hmt, vdd->hmt_stat.candela_level_hmt);

	return;
}

int ss_hmt_brightenss_packet_set(
		struct samsung_display_driver_data *vdd)
{
	int cmd_cnt = 0;
	int level_key = 0;
	struct dsi_panel_cmd_set *set;
	struct dsi_cmd_desc *packet = NULL;
	struct dsi_panel_cmd_set *tx_cmd = NULL;

	LCD_DEBUG("++\n");

	set_hmt_br_values(vdd);

	/* init packet */
	set = ss_get_cmds(vdd, TX_BRIGHT_CTRL);
	packet = set->cmds;

	if (vdd->smart_dimming_hmt_loaded_dsi) {
		/* aid/aor B2 */
		if (!IS_ERR_OR_NULL(vdd->panel_func.samsung_brightness_aid_hmt)) {
			level_key = false;
			tx_cmd = vdd->panel_func.samsung_brightness_aid_hmt(vdd, &level_key);

			update_packet_level_key_enable(vdd, packet, &cmd_cnt, level_key);
			ss_update_brightness_packet(packet, &cmd_cnt, tx_cmd);
			update_packet_level_key_disable(vdd, packet, &cmd_cnt, level_key);
		}

		/* elvss B5 */
		if (!IS_ERR_OR_NULL(vdd->panel_func.samsung_brightness_elvss_hmt)) {
			level_key = false;
			tx_cmd = vdd->panel_func.samsung_brightness_elvss_hmt(vdd, &level_key);

			update_packet_level_key_enable(vdd, packet, &cmd_cnt, level_key);
			ss_update_brightness_packet(packet, &cmd_cnt, tx_cmd);
			update_packet_level_key_disable(vdd, packet, &cmd_cnt, level_key);
		}

		/* vint F4 */
		if (!IS_ERR_OR_NULL(vdd->panel_func.samsung_brightness_vint_hmt)) {
			level_key = false;
			tx_cmd = vdd->panel_func.samsung_brightness_vint_hmt(vdd, &level_key);

			update_packet_level_key_enable(vdd, packet, &cmd_cnt, level_key);
			ss_update_brightness_packet(packet, &cmd_cnt, tx_cmd);
			update_packet_level_key_disable(vdd, packet, &cmd_cnt, level_key);
		}

		/* gamma CA */
		if (!IS_ERR_OR_NULL(vdd->panel_func.samsung_brightness_gamma_hmt)) {
			level_key = false;
			tx_cmd = vdd->panel_func.samsung_brightness_gamma_hmt(vdd, &level_key);

			update_packet_level_key_enable(vdd, packet, &cmd_cnt, level_key);
			ss_update_brightness_packet(packet, &cmd_cnt, tx_cmd);
			update_packet_level_key_disable(vdd, packet, &cmd_cnt, level_key);
		}
	}

	LCD_DEBUG("--\n");

	return cmd_cnt;
}

int ss_brightness_dcs_hmt(struct samsung_display_driver_data *vdd,
		int level)
{
	struct dsi_panel_cmd_set *brightness_cmds = NULL;
	int cmd_cnt;
	int ret = 0;

	brightness_cmds = ss_get_cmds(vdd, TX_BRIGHT_CTRL);

	vdd->hmt_stat.hmt_bl_level = level;
	LCD_ERR("[HMT] hmt_bl_level(%d)\n", vdd->hmt_stat.hmt_bl_level);

	cmd_cnt = ss_hmt_brightenss_packet_set(vdd);

	/* sending tx cmds */
	if (cmd_cnt) {
		/* setting tx cmds cmt */
		brightness_cmds->count = cmd_cnt;

		/* generate single tx packet */
		ret = ss_single_transmission_packet(brightness_cmds);

		if (ret) {
			ss_send_cmd(vdd, TX_BRIGHT_CTRL);

			LCD_INFO("idx(%d), candela_level(%d), hmt_bl_level(%d) itp : %s",
				vdd->hmt_stat.cmd_idx_hmt, vdd->hmt_stat.candela_level_hmt, vdd->hmt_stat.hmt_bl_level,
				vdd->panel_br_info.itp_mode == 0 ? "TABLE" : "FLASH'");
		} else
			LCD_DEBUG("single_transmission_fail error\n");
	} else
		LCD_INFO("level : %d skip\n", vdd->bl_level);

	return cmd_cnt;
}

// TFT brightness

void ss_brightness_tft_pwm(struct samsung_display_driver_data *vdd, int level)
{
	if (vdd == NULL) {
		LCD_ERR("no PWM\n");
		return;
	}

	if (ss_is_panel_off(vdd))
		return;

	vdd->bl_level = level;

	if (vdd->panel_func.samsung_brightness_tft_pwm)
		vdd->panel_func.samsung_brightness_tft_pwm(vdd, level);
}

void ss_tft_autobrightness_cabc_update(struct samsung_display_driver_data *vdd)
{
	LCD_INFO("\n");

	switch (vdd->auto_brightness) {
	case 0:
		ss_cabc_update(vdd);
		break;
	case 1:
	case 2:
	case 3:
	case 4:
		ss_send_cmd(vdd, TX_CABC_ON);
		break;
	case 5:
	case 6:
		ss_send_cmd(vdd, TX_CABC_OFF);
		break;
	}
}

/* Check lists in case of wrong TE period status
 * 1) read DDI command log buffer to check not intended TE setting.
 * TODO: add below items.
 * 2) check if MIPI clock frequency is stable.
 *    unstable mipi clock would cause unstable TE period.
 */
static void ss_inval_te_check_work(struct work_struct *work)
{
	struct te_period_check *te_check =
		container_of(work, struct te_period_check , te_work);
	struct samsung_display_driver_data *vdd =
		container_of(te_check, struct samsung_display_driver_data, te_check);
	int i;
	static int read_operation = 0;

	LCD_INFO("+++");

	if (read_operation) {
		/* 1) read DDI command log buffer to check not intended TE setting. */
		ss_read_ddi_cmd_log(vdd, te_check->cmd_log);

		for (i = 0; i < DDI_CMD_LOGBUF_SIZE; i++) {
			/* below commands info. is for HA8 */
			if (te_check->cmd_log[i] == 0x35 ||
				te_check->cmd_log[i] == 0xB9 ||
				te_check->cmd_log[i] == 0xC5)
				LCD_ERR("detected TE cmd: [%d]:0x%x\n", i, te_check->cmd_log[i]);
		}
	}
	/* 2) check if MIPI clock frequency is stable. */

	/* add delay before setting is_working flag, to prevent work flood */
	msleep(5000);
	te_check->is_working = false;
	te_check->te_cnt = 0;
	enable_irq(vdd->te_check.te_irq);

	LCD_INFO("---");
}

/* case of clearing pending irq: TE_CNT_STABLE=2 (desc->istate &= ~IRQS_PENDING)
 * case of not clearing pending irq: TE_CNT_STABLE=4
 */
#define TE_CNT_STABLE	2
static irqreturn_t ss_te_period_check_handler(int irq, void *handle)
{
	struct samsung_display_driver_data *vdd = handle;
	struct te_period_check *te_check = &vdd->te_check;
	static ktime_t prev_te_time;
	ktime_t new_te_time;
	ktime_t delta_t;
	s64 delta;

	if (!ss_is_panel_on(vdd)) {
		/* In AOD, reset te_cnt and do not check te period */
		te_check->te_cnt = 0;
		return IRQ_HANDLED;
	}

	new_te_time = ktime_get();

	if (unlikely(te_check->te_cnt < TE_CNT_STABLE)) {
		te_check->te_cnt++;
		goto end;
	}

	delta_t.tv64 = new_te_time.tv64 - prev_te_time.tv64;
	delta = ktime_to_ms(delta_t);

	/* Normal TE period range: 13 ~ 20ms or 31 ~ 37ms (To be fixed).
	 * In below cases, TE period be 33ms.
	 * case1) AP sends mipi packet in TE timing.
	 * case2) AOD.
	 * ...
	 */
	if (likely((delta > 12 && delta < 21) || (delta > 30 && delta < 38))) {
		LCD_DEBUG("TE period: %lld\n", delta);
	} else {
		LCD_ERR("invalid TE period (%lld ms), te_cnt: %d, is_working: %d\n",
				delta, te_check->te_cnt, te_check->is_working);
		if (!te_check->is_working) {
			te_check->is_working = true;
			disable_irq_nosync(vdd->te_check.te_irq);
			schedule_work(&te_check->te_work);
		}
	}

end:
	prev_te_time.tv64 = new_te_time.tv64;

	return IRQ_HANDLED;
}

void ss_check_inval_te_period_init(struct samsung_display_driver_data *vdd)
{
	struct dsi_panel *panel = GET_DSI_PANEL(vdd);
	unsigned int disp_te_gpio;
	int ret;

	disp_te_gpio = ss_get_te_gpio(vdd);

	/* In case of ESD_MODE_PANEL_TE, TE gpio irq will be used for ESD.
	 * case 1) status_mode = ESD_MODE_PANEL_TE:
	 *         register TE irq for ESD check and no TE period monitor
	 * case 2) status_mode != ESD_MODE_PANEL_TE:
	 *         no TE irq for ESD check and allow to monitor TE period
	 */
	LCD_INFO("status_mode = %d\n", panel->esd_config.status_mode);
	if (panel->esd_config.status_mode == ESD_MODE_PANEL_TE) {
		vdd->te_check.te_irq = -1;
		return;
	}

	ret = request_threaded_irq(
			gpio_to_irq(disp_te_gpio),
			ss_te_period_check_handler,
			NULL,
			IRQF_TRIGGER_RISING | IRQF_ONESHOT,
			"VSYNC_GPIO", vdd);
	if (ret) {
		LCD_ERR("Failed to TE request_irq, ret=%d\n", ret);
		vdd->te_check.te_irq = -1;
		return;
	} else {
		vdd->te_check.te_irq = gpio_to_irq(disp_te_gpio);
		disable_irq(vdd->te_check.te_irq);
	}

	INIT_WORK(&vdd->te_check.te_work, ss_inval_te_check_work);
}

void ss_panel_init(struct dsi_panel *panel)
{
	struct samsung_display_driver_data *vdd;
	enum ss_display_ndx ndx;
	char panel_name[MAX_CMDLINE_PARAM_LEN];
	char panel_secondary_name[MAX_CMDLINE_PARAM_LEN];

	/* compare panel name in command line and dsi_panel.
	 * primary panel: ndx = 0
	 * secondary panel: ndx = 1
	 */

	ss_get_primary_panel_name_cmdline(panel_name);
	ss_get_secondary_panel_name_cmdline(panel_secondary_name);

	if (!strcmp(panel->name, panel_name)) {
		ndx = PRIMARY_DISPLAY_NDX;
	} else if (!strcmp(panel->name, panel_secondary_name)) {
		ndx = SECONDARY_DISPLAY_NDX;
	} else {
		/* If it fails to find panel name, it cannot turn on display,
		 * and this is critical error case...
		 */
		WARN(1, "fail to find panel name, panel=%s, cmdline=%s\n",
				panel->name, panel_name);
		return;
	}

	/* TODO: after using component_bind in samsung_panel_init,
	 * it doesn't have to use vdd_data..
	 * Remove vdd_data, and allocate vdd memory here.
	 * vdds will be managed by vdds_list...
	 */
	vdd = &vdd_data[ndx];

	ss_set_display_ndx(vdd, ndx);

	panel->panel_private = vdd;
	vdd->msm_private = panel;
	list_add(&vdd->vdd_list, &vdds_list);

	if (ss_panel_debug_init(vdd))
		LCD_ERR("Fail to create debugfs\n");

	if (ss_smmu_debug_init(vdd))
		LCD_ERR("Fail to create smmu debug\n");

	mutex_init(&vdd->vdd_lock);
	mutex_init(&vdd->cmd_lock);
	mutex_init(&vdd->bl_lock);

	/* To guarantee ALPM ON or OFF mode change operation*/
	mutex_init(&vdd->vdd_panel_lpm_lock);

	/* To guarantee dynamic MIPI clock change*/
	mutex_init(&vdd->vdd_dyn_mipi_lock);

	if (ss_is_cmd_mode(vdd)) {
		vdd->panel_func.ss_event_osc_te_fitting =
			ss_event_osc_te_fitting;
	}

	vdd->panel_func.ss_event_frame_update =
		ss_event_frame_update;
	vdd->panel_func.ss_event_fb_event_callback =
		ss_event_fb_event_callback;
	vdd->panel_func.ss_event_esd_recovery_init =
		ss_event_esd_recovery_init;

	vdd->manufacture_id_dsi = PBA_ID;

	vdd->panel_dead = false;

	if (IS_ERR_OR_NULL(vdd->panel_func.samsung_panel_init))
		LCD_ERR("no samsung_panel_init fucn");
	else
		vdd->panel_func.samsung_panel_init(vdd);

	if (vdd->support_mdnie_lite ||
			vdd->support_cabc ||
			ss_panel_attached(ndx))
		vdd->mdnie_tune_state_dsi =
			init_dsi_tcon_mdnie_class(vdd);

	spin_lock_init(&vdd->esd_recovery.irq_lock);

	vdd->hmt_stat.hmt_enable = hmt_enable;
	vdd->hmt_stat.hmt_reverse_update = hmt_reverse_update;
	vdd->hmt_stat.hmt_bright_update = hmt_bright_update;

	ss_panel_attach_set(vdd, true);

	INIT_DELAYED_WORK(&vdd->delay_disp_on_work, samsung_display_delay_disp_on_work);

	/* Init Other line panel support */
	if (!IS_ERR_OR_NULL(vdd->panel_func.parsing_otherline_pdata) && ss_panel_attached(vdd->ndx)) {
		if (!IS_ERR_OR_NULL(vdd->panel_func.get_panel_fab_type)) {
			if (vdd->panel_func.get_panel_fab_type() == NEW_FB_PANLE_TYPE) {
				LCD_ERR("parsing_otherline_pdata (%d)\n", vdd->panel_func.get_panel_fab_type());

				INIT_DELAYED_WORK(&vdd->other_line_panel_support_work, read_panel_data_work_fn);
				vdd->other_line_panel_support_workq =
					create_singlethread_workqueue("other_line_panel_support_wq");

				if (vdd->other_line_panel_support_workq) {
					vdd->other_line_panel_work_cnt = OTHERLINE_WORKQ_CNT;
					queue_delayed_work(vdd->other_line_panel_support_workq,
							&vdd->other_line_panel_support_work,
							msecs_to_jiffies(OTHERLINE_WORKQ_DEALY));
				}
			}
		}
	}

	mutex_init(&vdd->exclusive_tx.ex_tx_lock);
	vdd->exclusive_tx.enable = 0;
	init_waitqueue_head(&vdd->exclusive_tx.ex_tx_waitq);

	ss_create_sysfs(vdd);

#if defined(CONFIG_SEC_FACTORY)
	vdd->is_factory_mode = true;
#endif

	/* parse display dtsi node */
	ss_panel_parse_dt(vdd);

	self_display_init(vdd);

	ss_check_inval_te_period_init(vdd);
}
