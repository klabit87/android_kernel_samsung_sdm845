/* Copyright (c) 2015-2017, The Linux Foundation. All rights reserved.
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
#if defined(CONFIG_CAMERA_DYNAMIC_MIPI)
#include <linux/platform_device.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/bsearch.h>
#include <linux/dev_ril_bridge.h>
#include "cam_sensor_mipi.h"
#include "imx320_mipi.h"
#include "cam_sensor_dev.h"

static struct cam_cp_noti_info g_cp_noti_info;
static struct mutex g_mipi_mutex;
static bool g_init_notifier;
extern char mipi_string[20];

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

//static const struct cam_mipi_sensor_mode *sensor_imx320_mipi_sensor_mode;
static u32 sensor_imx320_mipi_sensor_mode_size;
static const int *sensor_imx320_verify_sensor_mode;
static int sensor_imx320_verify_sensor_mode_size;

static int cam_mipi_ril_notifier(struct notifier_block *nb,
				unsigned long size, void *buf)
{
	struct cam_cp_noti_info *cp_noti_info;

	if (!g_init_notifier) {
		pr_err("%s: not init ril notifier\n", __func__);
		return NOTIFY_DONE;
	}

	pr_err("%s: [dynamic_mipi] ril notification size [%ld]\n", __func__, size);
	if (size == sizeof(struct cam_cp_noti_info)) {
		cp_noti_info = (struct cam_cp_noti_info *)buf;

		if (cp_noti_info->len == 0x0010 && cp_noti_info->main_cmd == 0x27 &&
			cp_noti_info->sub_cmd == 0x01 && cp_noti_info->cmd_type == 0x03) {
			mutex_lock(&g_mipi_mutex);
			memcpy(&g_cp_noti_info, buf, sizeof(struct cam_cp_noti_info));
			mutex_unlock(&g_mipi_mutex);

			pr_err("%s: [dynamic_mipi] update mipi channel [%d,%d,%d]\n",
				__func__, g_cp_noti_info.rat, g_cp_noti_info.band, g_cp_noti_info.channel);
			return NOTIFY_OK;
		}
	}

	return NOTIFY_DONE;
}

static struct notifier_block g_ril_notifier_block = {
	.notifier_call = cam_mipi_ril_notifier,
};

void cam_mipi_register_ril_notifier(void)
{
	if (!g_init_notifier) {
		pr_err("%s: [dynamic_mipi] register ril notifier\n", __func__);

		mutex_init(&g_mipi_mutex);
		memset(&g_cp_noti_info, 0, sizeof(struct cam_cp_noti_info));

		register_dev_ril_bridge_event_notifier(&g_ril_notifier_block);
		g_init_notifier = true;
	}
}

static void cam_mipi_get_rf_channel(struct cam_cp_noti_info *ch)
{
	if (!g_init_notifier) {
		pr_err("%s: not init ril notifier\n", __func__);
		memset(ch, 0, sizeof(struct cam_cp_noti_info));
		return;
	}

	mutex_lock(&g_mipi_mutex);
	memcpy(ch, &g_cp_noti_info, sizeof(struct cam_cp_noti_info));
	mutex_unlock(&g_mipi_mutex);
}

static int compare_rf_channel(const void *key, const void *element)
{
	struct cam_mipi_channel *k = ((struct cam_mipi_channel *)key);
	struct cam_mipi_channel *e = ((struct cam_mipi_channel *)element);

	if (k->rat_band < e->rat_band)
		return -1;
	else if (k->rat_band > e->rat_band)
		return 1;

	if (k->channel_max < e->channel_min)
		return -1;
	else if (k->channel_min > e->channel_max)
		return 1;

	return 0;
}

int cam_mipi_select_mipi_by_rf_channel(const struct cam_mipi_channel *channel_list, const int size)
{
	struct cam_mipi_channel *result = NULL;
	struct cam_mipi_channel key;
	struct cam_cp_noti_info input_ch;

	cam_mipi_get_rf_channel(&input_ch);

	key.rat_band = CAM_RAT_BAND(input_ch.rat, input_ch.band);
	key.channel_min = input_ch.channel;
	key.channel_max = input_ch.channel;

	pr_err("%s: [dynamic_mipi] searching rf channel s [%d,%d,%d]\n",
		__func__, input_ch.rat, input_ch.band, input_ch.channel);

	result = bsearch(&key,
			channel_list,
			size,
			sizeof(struct cam_mipi_channel),
			compare_rf_channel);

	if (result == NULL) {
		pr_err("%s: [dynamic_mipi] searching result : not found, use default mipi clock\n", __func__);
		return 0; /* return default mipi clock index = 0 */
	}

	pr_err("%s: [dynamic_mipi] searching result : [0x%x,(%d-%d)]->(%d)\n", __func__,
		result->rat_band, result->channel_min, result->channel_max, result->setting_index);

	return result->setting_index;
}

int cam_mipi_verify_mipi_channel(const struct cam_mipi_channel *channel_list, const int size)
{
	int i;
	u16 pre_rat;
	u16 pre_band;
	u32 pre_channel_min;
	u32 pre_channel_max;
	u16 cur_rat;
	u16 cur_band;
	u32 cur_channel_min;
	u32 cur_channel_max;

	if (channel_list == NULL || size < 2)
		return 0;

	pre_rat = CAM_GET_RAT(channel_list[0].rat_band);
	pre_band = CAM_GET_BAND(channel_list[0].rat_band);
	pre_channel_min = channel_list[0].channel_min;
	pre_channel_max = channel_list[0].channel_max;

	if (pre_channel_min > pre_channel_max) {
		panic("min is bigger than max : index=%d, min=%d, max=%d", 0, pre_channel_min, pre_channel_max);
		return -EINVAL;
	}

	for (i = 1; i < size; i++) {
		cur_rat = CAM_GET_RAT(channel_list[i].rat_band);
		cur_band = CAM_GET_BAND(channel_list[i].rat_band);
		cur_channel_min = channel_list[i].channel_min;
		cur_channel_max = channel_list[i].channel_max;

		if (cur_channel_min > cur_channel_max) {
			panic("min is bigger than max : index=%d, min=%d, max=%d", 0, cur_channel_min, cur_channel_max);
			return -EINVAL;
		}

		if (pre_rat > cur_rat) {
			panic("not sorted rat : index=%d, pre_rat=%d, cur_rat=%d", i, pre_rat, cur_rat);
			return -EINVAL;
		}

		if (pre_rat == cur_rat) {
			if (pre_band > cur_band) {
				panic("not sorted band : index=%d, pre_band=%d, cur_band=%d", i, pre_band, cur_band);
				return -EINVAL;
			}

			if (pre_band == cur_band) {
				if (pre_channel_max >= cur_channel_min) {
					panic("overlaped channel range : index=%d, pre_ch=[%d-%d], cur_ch=[%d-%d]",
						i, pre_channel_min, pre_channel_max, cur_channel_min, cur_channel_max);
					return -EINVAL;
				}
			}
		}

		pre_rat = cur_rat;
		pre_band = cur_band;
		pre_channel_min = cur_channel_min;
		pre_channel_max = cur_channel_max;
	}

	return 0;
}

void cam_mipi_init_setting(struct cam_sensor_ctrl_t *s_ctrl)
{
	const struct cam_mipi_sensor_mode *cur_mipi_sensor_mode;

	s_ctrl->mipi_info = sensor_imx320_setfile_A_mipi_sensor_mode;
	sensor_imx320_mipi_sensor_mode_size = ARRAY_SIZE(sensor_imx320_setfile_A_mipi_sensor_mode);
	sensor_imx320_verify_sensor_mode = sensor_imx320_setfile_A_verify_sensor_mode;
	sensor_imx320_verify_sensor_mode_size = ARRAY_SIZE(sensor_imx320_setfile_A_verify_sensor_mode);

	cur_mipi_sensor_mode = &(s_ctrl->mipi_info[0]);

	s_ctrl->mipi_clock_index_cur = CAM_MIPI_NOT_INITIALIZED;
	s_ctrl->mipi_clock_index_new = CAM_MIPI_NOT_INITIALIZED;

	pr_err("[dynamic_mipi] mipi_clock_index_cur : %d, mipi_clock_index_new : %d\n",
		s_ctrl->mipi_clock_index_cur, s_ctrl->mipi_clock_index_new);
}

void cam_mipi_update_info(struct cam_sensor_ctrl_t *s_ctrl)
{
	const struct cam_mipi_sensor_mode *cur_mipi_sensor_mode;
	int found = -1;

	cur_mipi_sensor_mode = &(s_ctrl->mipi_info[0]);

	pr_err("[dynamic_mipi] cur rat : %d\n", cur_mipi_sensor_mode->mipi_channel->rat_band);
	pr_err("[dynamic_mipi] cur channel_min : %d\n", cur_mipi_sensor_mode->mipi_channel->channel_min);
	pr_err("[dynamic_mipi] cur channel_max : %d\n", cur_mipi_sensor_mode->mipi_channel->channel_max);
	pr_err("[dynamic_mipi] cur setting_index : %d\n", cur_mipi_sensor_mode->mipi_channel->setting_index);

	found = cam_mipi_select_mipi_by_rf_channel(cur_mipi_sensor_mode->mipi_channel,
				cur_mipi_sensor_mode->mipi_channel_size);
	if (found != -1) {
		if (found < cur_mipi_sensor_mode->sensor_setting_size) {
			s_ctrl->mipi_clock_index_new = found;

			pr_err("[dynamic_mipi] mipi_clock_index_new : %d\n",
				s_ctrl->mipi_clock_index_new);

			//device->cfg->mipi_speed = cur_mipi_sensor_mode->sensor_setting[found].mipi_rate;
			//pr_err("%s - update mipi rate : %d\n", __func__, device->cfg->mipi_speed);
		} else {
			//prerr("sensor setting size is out of bound");
		}
	}
}

void cam_mipi_get_clock_string(struct cam_sensor_ctrl_t *s_ctrl)
{
	const struct cam_mipi_sensor_mode *cur_mipi_sensor_mode;

	cur_mipi_sensor_mode = &(s_ctrl->mipi_info[0]);

	sprintf(mipi_string, "%s",
		cur_mipi_sensor_mode->mipi_setting[s_ctrl->mipi_clock_index_new].str_mipi_clk);

    pr_err("[dynamic_mipi] cam_mipi_get_clock_string : %d\n", s_ctrl->mipi_clock_index_new);
	pr_err("[dynamic_mipi] mipi_string : %s\n", mipi_string);
}
#endif
