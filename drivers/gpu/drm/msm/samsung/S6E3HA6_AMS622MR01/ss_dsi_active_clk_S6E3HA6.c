/*
 * Copyright (c) 2017 Samsung Electronics Co., Ltd.
 *	      http://www.samsung.com/
 *
 * Samsung's Active Clock Driver
 * Author: ChangJae Jang <cj1225.jang@samsung.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#include "ss_dsi_panel_common.h"
#include "ss_dsi_active_clk_S6E3HA6.h"
#include "ss_dsi_active_clk_img_S6E3HA6.h"

static int ss_dsi_update_clock_control_cmd(int time_update)
{
	char *pload = NULL;
	char *pload_d = NULL;
	struct act_clk_info *c_info;
	struct act_blink_info *b_info;
	struct act_drawer_info *d_info;
	struct samsung_display_driver_data *vdd = samsung_get_vdd();
	u32 xres;
	u32 yres;

	if (IS_ERR_OR_NULL(vdd)) {
		LCD_ERR("vdd is null or error\n");
		return -ENODEV;
	}

	c_info = &vdd->act_clock.clk_info;
	b_info = &vdd->act_clock.blink_info;
	d_info = &vdd->act_clock.draw_info;

	if (vdd->act_clock.act_clock_control_tx_cmds->cmds->msg.tx_buf) {
		pload = vdd->act_clock.act_clock_control_tx_cmds->cmds[1].msg.tx_buf;
		pload_d = vdd->act_clock.act_clock_control_tx_cmds->cmds[2].msg.tx_buf;
	} else {
		LCD_ERR("Clock Control Command Payload is NULL\n");
		return -ENODEV;
	}

	pload[1] = 0;

	xres = ss_get_xres(vdd);
	yres = ss_get_yres(vdd);

	if (c_info->en) {
		pload[2] = ACTIVE_CLK_ANA_CLOCK_EN;

		if (c_info->interval == 100) {
			/* INC_STEP = 1(0.6 degree), UPDATE_RATE=3 */
			pload[9] = 0x03; /* UPDATE_RATE */
			pload[10] = 0x41; /* COMP_EN + INC_STEP */
		} else {
			/* Default Value */
			/* INC_STEP = 10(6 degree), UPDATE_RATE=30 */
			pload[9] = 0x1E; /* UPDATE_RATE */
			pload[10] = 0x4A; /* COMP_EN + INC_STEP */
		}

		if (time_update)
			pload[10] |= BIT(7);
		else
			pload[10] &= ~BIT(7);

		pload[16] = c_info->time_hour % 12;
		pload[17] = c_info->time_min % 60;
		pload[18] = c_info->time_second % 60;
		pload[19] = c_info->time_ms;

		if (c_info->pos_x > xres)
			c_info->pos_x = xres;
		pload[20] = (c_info->pos_x >> 8) & 0xff;
		pload[21] = c_info->pos_x & 0xff;

		if (c_info->pos_x > yres)
			c_info->pos_y = yres;
		pload[22] = (c_info->pos_y >> 8) & 0xff;
		pload[23] = c_info->pos_y & 0xff;

		/* Update Self Drawer Command also */
		pload_d[2] = ACTIVE_CLK_SELF_DRAWER_EN;

		pload_d[30] = (c_info->pos_x >> 8) & 0xff;
		pload_d[31] = c_info->pos_x & 0xff;
		pload_d[32] = (c_info->pos_y >> 8) & 0xff;
		pload_d[33] = c_info->pos_y & 0xff;

		pload_d[40] = (unsigned char)(d_info->sd_line_color >> 16) & 0xff; // Inner Circle Color (R)
		pload_d[41] = (unsigned char)(d_info->sd_line_color >> 8) & 0xff; // Inner Circle Color (G)
		pload_d[42] = (unsigned char)(d_info->sd_line_color & 0xff); // Inner Circle Color (B)
		pload_d[43] = (unsigned char)(d_info->sd2_line_color >> 16) & 0xff; // Outer Circle Color (R)
		pload_d[44] = (unsigned char)(d_info->sd2_line_color >> 8) & 0xff; // Outer Circle Color (G)
		pload_d[45] = (unsigned char)(d_info->sd2_line_color & 0xff); // Outer Circle Color (B)
	} else if (b_info->en) {
		pload[2] = ACTIVE_CLK_BLINK_EN;

		pload[33] = b_info->interval;
		pload[34] = b_info->radius;
		/* blink color : Red */
		pload[35] = (unsigned char)(b_info->color >> 16) & 0xff;
		/* blink color : Blue */
		pload[36] = (unsigned char)(b_info->color >> 8) & 0xff;
		/* blink color : Green */
		pload[37] = (unsigned char)(b_info->color & 0xff);

		pload[38] = 0x05;

		if (b_info->pos1_x > xres)
			b_info->pos1_x = xres;
		if (b_info->pos1_y > yres)
			b_info->pos1_y = yres;

		pload[39] = (b_info->pos1_x >> 8) & 0xff;
		pload[40] = (b_info->pos1_x & 0xff);
		pload[41] = (b_info->pos1_y >> 8) & 0xff;
		pload[42] = (b_info->pos1_y & 0xff);

		if (b_info->pos2_x > xres)
			b_info->pos2_x = xres;
		if (b_info->pos2_x > xres)
			b_info->pos2_x = xres;

		pload[43] = (b_info->pos2_x >> 8) & 0xff;
		pload[44] = (b_info->pos2_x & 0xff);
		pload[45] = (b_info->pos2_y >> 8) & 0xff;
		pload[46] = (b_info->pos2_y & 0xff);
	} else {
		pload[2] = 0x00;
		pload_d[2] = 0x00;
	}

	return 0;
}

static int ss_dsi_set_active_clock_info(struct ioctl_active_clock *ioctl_info)
{
	int ret = 0;
	struct act_clk_info *c_info;
	struct samsung_display_driver_data *vdd = samsung_get_vdd();

	if (IS_ERR_OR_NULL(vdd)) {
		LCD_ERR("vdd is null or error\n");
		return -ENODEV;
	}

	c_info = &vdd->act_clock.clk_info;

	c_info->en = ioctl_info->en;
	c_info->interval = ioctl_info->interval;
	c_info->time_hour = ioctl_info->time_hour;
	c_info->time_min = ioctl_info->time_min;
	c_info->time_second = ioctl_info->time_second;
	c_info->time_ms = ioctl_info->time_ms;
	c_info->pos_x = ioctl_info->pos_x;
	c_info->pos_y = ioctl_info->pos_y;

	ret = ss_dsi_update_clock_control_cmd(1);
	if (ret)
		LCD_ERR("Failed to update clock control cmd\n");

	ret = ss_send_cmd(vdd, TX_ACT_CLOCK_ENABLE);
	if (ret)
		LCD_ERR("Failed to send clock control cmd\n");

	msleep(35);

	ret = ss_dsi_update_clock_control_cmd(0);
	if (ret)
		LCD_ERR("Failed to update clock control cmd\n");

	ret = ss_send_cmd(vdd, TX_ACT_CLOCK_ENABLE);
	if (ret)
		LCD_ERR("Failed to send clock control cmd\n");

	LCD_INFO("Active_Clock CLOCK en=%d, interval=%d, Time(%d:%d:%d.%d), pos(%d,%d)",
		c_info->en, c_info->interval, c_info->time_hour, c_info->time_min,
		c_info->time_second, c_info->time_ms, c_info->pos_x, c_info->pos_y);

	return ret;
}

static int ss_dsi_set_blink_clock_info(struct ioctl_blink_clock *ioctl_info)
{
	int ret = 0;
	struct act_blink_info *b_info;
	struct samsung_display_driver_data *vdd = samsung_get_vdd();

	if (IS_ERR_OR_NULL(vdd)) {
		LCD_ERR("vdd is null or error\n");
		return -ENODEV;
	}

	b_info = &vdd->act_clock.blink_info;

	b_info->en = ioctl_info->en;
	b_info->interval = ioctl_info->interval;
	b_info->radius = ioctl_info->radius;
	b_info->color = ioctl_info->color;
	b_info->line_width = ioctl_info->line_width;
	b_info->pos1_x = ioctl_info->pos1_x;
	b_info->pos1_y = ioctl_info->pos1_y;
	b_info->pos2_x = ioctl_info->pos2_x;
	b_info->pos2_y = ioctl_info->pos2_y;

	ret = ss_dsi_update_clock_control_cmd(0);
	if (ret)
		LCD_ERR("Failed to update clock control cmd\n");

	ret = ss_send_cmd(vdd, TX_ACT_CLOCK_ENABLE);
	if (ret)
		LCD_ERR("Failed to send clock control cmd\n");

	LCD_INFO("Active_Clock BLINK en=%d, interval=%d, radius=%d, color=%d, line_width=%d, pos1(%d,%d), pos2(%d,%d)",
		b_info->en, b_info->interval, b_info->radius, b_info->color,
		b_info->line_width, b_info->pos1_x, b_info->pos1_y, b_info->pos2_x, b_info->pos2_y);

	return ret;
}

static int ss_dsi_set_self_drawer_info(struct ioctl_self_drawer *ioctl_info)
{
	int ret = 0;
	struct act_drawer_info *d_info;
	struct samsung_display_driver_data *vdd = samsung_get_vdd();

	if (IS_ERR_OR_NULL(vdd)) {
		LCD_ERR("vdd is null or error\n");
		return -ENODEV;
	}

	d_info = &vdd->act_clock.draw_info;

	d_info->sd_line_color = ioctl_info->sd_line_color;
	d_info->sd2_line_color = ioctl_info->sd2_line_color;
	d_info->sd_radius = ioctl_info->sd_radius;

	ret = ss_dsi_update_clock_control_cmd(0);
	if (ret)
		LCD_ERR("Failed to update clock control cmd\n");

	ret = ss_send_cmd(vdd, TX_ACT_CLOCK_ENABLE);
	if (ret)
		LCD_ERR("Failed to send clock control cmd\n");

	LCD_INFO("Active_Clock SELF DRAW sd_line_color=0x%x, sd2_line_color=0x%x, sd_radius=%d",
		d_info->sd_line_color, d_info->sd2_line_color, d_info->sd_radius);

	return ret;
}

static int ss_dsi_update_clock_image_to_cmd(void)
{
	short copy_size = 0;
	u32 loop = 0, cursor = 0, img_buf_size = 0;
	char *p_load = NULL;
	char *img_buf = NULL;
	char *temp_buf = NULL;
	struct samsung_display_driver_data *vdd = samsung_get_vdd();

	if (IS_ERR_OR_NULL(vdd)) {
		LCD_ERR("vdd is null or error\n");
		return -ENODEV;
	}

	if (vdd->act_clock.clk_info.img_buf) {
		img_buf = vdd->act_clock.clk_info.img_buf;
		img_buf_size = vdd->act_clock.clk_info.img_buf_size;
	} else {
		LCD_ERR("Image Buffer is NULL\n");
		return -ENODEV;
	}

	LCD_INFO("++\n");
	mutex_lock(&vdd->vdd_act_clock_lock);

	/* Generate DSI Command From Image Buffer */
	for (loop = 0 ; loop < ARRAY_SIZE(DSI_CMD_ACTIVE_CLK_IMG_TO_CMD); loop++) {
		DSI_CMD_ACTIVE_CLK_IMG_TO_CMD[loop].msg.type = MIPI_DSI_GENERIC_LONG_WRITE;
		DSI_CMD_ACTIVE_CLK_IMG_TO_CMD[loop].last_command = true;
		DSI_CMD_ACTIVE_CLK_IMG_TO_CMD[loop].msg.tx_buf = active_clk_img_to_cmd[loop];
		DSI_CMD_ACTIVE_CLK_IMG_TO_CMD[loop].msg.tx_len = ACT_CLK_IMG_PAYLOAD_SIZE+1;
		p_load = DSI_CMD_ACTIVE_CLK_IMG_TO_CMD[loop].msg.tx_buf;
		copy_size = DSI_CMD_ACTIVE_CLK_IMG_TO_CMD[loop].msg.tx_len - 1;

		if (loop == 0)
			p_load[0] = 0x4C;
		else
			p_load[0] = 0x5C;

		/* Protection of Image Buffer Overflow */
		if (cursor + copy_size > img_buf_size) {
			copy_size = img_buf_size-cursor;
			DSI_CMD_ACTIVE_CLK_IMG_TO_CMD[loop].msg.tx_len = copy_size + 1;
		}

		temp_buf = memcpy(p_load+1, img_buf+cursor, copy_size);
		if (!temp_buf) {
			LCD_ERR("Image Command Update Fail\n");
			goto update_exit;
		}

		cursor += copy_size;
	}

update_exit:
	LCD_INFO("--\n");

	mutex_unlock(&vdd->vdd_act_clock_lock);

	return 0;
}

static long ss_dsi_active_clock_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{
	int ret = 0;
	struct samsung_display_driver_data *vdd = samsung_get_vdd();
	struct ioctl_active_clock ioctl_active;
	struct ioctl_blink_clock ioctl_blink;
	struct ioctl_self_drawer ioctl_drawer;

	if (IS_ERR_OR_NULL(vdd)) {
		LCD_ERR("vdd is null or error\n");
		return -ENODEV;
	}

	if (!vdd->act_clock.file_opend) {
		LCD_ERR("ioctl error(File is not opened)\n");
		return -EIO;
	}

	LCD_INFO("Active_Clock IOCTL CMD=%s\n", cmd == IOCTL_ACT_CLK?"IOCTL_ACT_CLK" :
			cmd == IOCTL_BLINK_CLK?"IOCTL_BLINK_CLK" :
			cmd == IOCTL_SELF_DRAWER_CLK?"IOCTL_SELF_DRAWER_CLK":"Invalid Cmd");

	mutex_lock(&vdd->vdd_act_clock_lock);

	if (ss_is_panel_on(vdd)) {
		LCD_INFO("Active_Clock IOCTL is blocked in Normal On(Unblank)\n");
		mutex_unlock(&vdd->vdd_act_clock_lock);
		return -EPERM;
	}

	switch (cmd) {
	case IOCTL_ACT_CLK:
		if (copy_from_user(&ioctl_active, (struct ioctl_active_clock __user *)arg,
			sizeof(struct ioctl_active_clock))) {
			LCD_ERR("failed to copy from user's active param\n");
			ret = -EFAULT;
			goto ioctl_exit;
		}
		ret = ss_dsi_set_active_clock_info(&ioctl_active);

		break;
	case IOCTL_BLINK_CLK:
		if (copy_from_user(&ioctl_blink, (struct ioctl_blink_clock __user *)arg,
			sizeof(struct ioctl_blink_clock))) {
			LCD_ERR("failed to copy from user's blink param\n");
			ret = -EFAULT;
			goto ioctl_exit;
		}
		ret = ss_dsi_set_blink_clock_info(&ioctl_blink);

		break;
	case IOCTL_SELF_DRAWER_CLK:
		if (copy_from_user(&ioctl_drawer, (struct ioctl_self_drawer __user *)arg,
			sizeof(struct ioctl_self_drawer))) {
			LCD_ERR("failed to copy from user's self drawer param\n");
			ret = -EFAULT;
			goto ioctl_exit;
		}
		ret = ss_dsi_set_self_drawer_info(&ioctl_drawer);
		break;
	default:
		LCD_ERR("invalid cmd : %d\n", cmd);
		ret = -EINVAL;
		goto ioctl_exit;
	}

ioctl_exit:
	mutex_unlock(&vdd->vdd_act_clock_lock);
	return ret;
}


static int ss_dsi_active_clock_open(struct inode *inode, struct file *file)
{
	int ret = 0;
	struct samsung_display_driver_data *vdd = samsung_get_vdd();

	if (IS_ERR_OR_NULL(vdd)) {
		LCD_ERR("vdd is null or error\n");
		return -ENODEV;
	}

	if (vdd->act_clock.file_opend) {
		LCD_ERR("already opend\n");
		return -EBUSY;
	}

	LCD_DEBUG("Active_Clock Open\n");

	mutex_lock(&vdd->vdd_act_clock_lock);
	vdd->act_clock.file_opend = 1;
	file->private_data = &vdd->act_clock.dev;
	mutex_unlock(&vdd->vdd_act_clock_lock);

	return ret;
}

static int ss_dsi_active_clock_release(struct inode *inode, struct file *file)
{
	int ret = 0;
	struct samsung_display_driver_data *vdd = samsung_get_vdd();

	mutex_lock(&vdd->vdd_act_clock_lock);
	vdd->act_clock.file_opend = 0;
	mutex_unlock(&vdd->vdd_act_clock_lock);

	LCD_DEBUG("Active_Clock Release\n");

	return ret;
}

static ssize_t ss_dsi_active_clock_write(struct file *file, const char __user *buf,
			 size_t count, loff_t *ppos)
{
	struct samsung_display_driver_data *vdd = samsung_get_vdd();
	static int acc_size;

	if (IS_ERR_OR_NULL(vdd)) {
		LCD_ERR("vdd is null or error\n");
		return -ENODEV;
	}

	if (!vdd->act_clock.clk_info.img_buf) {
		LCD_ERR("img buf is null : %d\n", (int)count);
		return -EIO;
	}

	if (count + acc_size > vdd->act_clock.clk_info.img_buf_size) {
		LCD_ERR("Buffer OverFlow Detected!! count=%d, acc_size=%d\n", (int)count, acc_size);
		acc_size = 0;
		return -EIO;
	}

	LCD_INFO("Active_Clock Write Start size : %d, ppos %d\n", (int)count, (int)*ppos);

	mutex_lock(&vdd->vdd_act_clock_lock);

	if (!vdd->act_clock.file_opend) {
		LCD_ERR("file is not opened\n");
		mutex_unlock(&vdd->vdd_act_clock_lock);
		return -ENOENT;
	}

	if (copy_from_user(vdd->act_clock.clk_info.img_buf + acc_size, buf, count)) {
		LCD_ERR("failed to copy from user data\n");
		mutex_unlock(&vdd->vdd_act_clock_lock);
		return -EFAULT;
	}

	/* Below Code is for self test(4K Write) via ADB */
	acc_size += count;
	if (acc_size >= vdd->act_clock.clk_info.img_buf_size)
		acc_size = 0;

	mutex_unlock(&vdd->vdd_act_clock_lock);

	ss_dsi_update_clock_image_to_cmd();

	LCD_INFO("Active_Clock Write Finish\n");

	return count;
}

static const struct file_operations act_clk_fops = {
	.owner = THIS_MODULE,
	.unlocked_ioctl = ss_dsi_active_clock_ioctl,
	.open = ss_dsi_active_clock_open,
	.release = ss_dsi_active_clock_release,
	.write = ss_dsi_active_clock_write,
};

int ss_dsi_active_clk_HA6_init(struct samsung_display_driver_data *vdd)
{
	int ret = 0;
	struct dsi_panel *panel = GET_DSI_PANEL(vdd);

	if (IS_ERR_OR_NULL(vdd)) {
		LCD_ERR("vdd is null or error\n");
		return -ENODEV;
	}

	if (IS_ERR_OR_NULL(panel)) {
		LCD_ERR("panel is null or error\n");
		return -ENODEV;
	}

	LCD_INFO("Active_Clock Init ++\n");

	vdd->act_clock.dev.minor = MISC_DYNAMIC_MINOR;
	vdd->act_clock.dev.name = "act_clk";
	vdd->act_clock.dev.fops = &act_clk_fops;
	vdd->act_clock.dev.parent = NULL;

	ret = misc_register(&vdd->act_clock.dev);

	if (ret) {
		LCD_ERR("failed to register driver : %d\n", ret);
		return ret;
	}

	vdd->act_clock.act_clock_img_data_tx_cmds = ss_get_cmds(vdd, TX_ACT_CLOCK_IMG_DATA);
	vdd->act_clock.act_clock_control_tx_cmds = ss_get_cmds(vdd, TX_ACT_CLOCK_ENABLE);
	vdd->act_clock.act_clock_disable_tx_cmds = ss_get_cmds(vdd, TX_ACT_CLOCK_DISABLE);

	vdd->act_clock.clk_info.en = 0;
	vdd->act_clock.clk_info.interval = 100;
	vdd->act_clock.clk_info.time_hour = 12;
	vdd->act_clock.clk_info.time_min = 0;
	vdd->act_clock.clk_info.time_second = 0;
	vdd->act_clock.clk_info.time_ms = 0;
	vdd->act_clock.clk_info.pos_x = 0;
	vdd->act_clock.clk_info.pos_y = 0;
	vdd->act_clock.clk_info.img_buf = active_clk_img_buf;
	vdd->act_clock.clk_info.img_buf_size = sizeof(active_clk_img_buf);
	vdd->act_clock.act_clock_img_data_tx_cmds->type = TX_ACT_CLOCK_IMG_DATA;
	vdd->act_clock.act_clock_img_data_tx_cmds->state = DSI_CMD_SET_STATE_HS;
	vdd->act_clock.act_clock_img_data_tx_cmds->cmds =
			DSI_CMD_ACTIVE_CLK_IMG_TO_CMD;
	vdd->act_clock.act_clock_img_data_tx_cmds->count =
			ARRAY_SIZE(DSI_CMD_ACTIVE_CLK_IMG_TO_CMD);
	vdd->act_clock.act_clock_control_tx_cmds->type = TX_ACT_CLOCK_ENABLE;
	vdd->act_clock.act_clock_control_tx_cmds->state = DSI_CMD_SET_STATE_HS;
	vdd->act_clock.act_clock_control_tx_cmds->cmds =
			DSI_CMD_ACTIVE_CLK_CONTROL_CMD;
	vdd->act_clock.act_clock_control_tx_cmds->count =
			ARRAY_SIZE(DSI_CMD_ACTIVE_CLK_CONTROL_CMD);
	vdd->act_clock.act_clock_disable_tx_cmds->type = TX_ACT_CLOCK_DISABLE;
	vdd->act_clock.act_clock_disable_tx_cmds->state = DSI_CMD_SET_STATE_HS;
	vdd->act_clock.act_clock_disable_tx_cmds->cmds =
			DSI_CMD_ACTIVE_CLK_CONTROL_DISABLE_CMD;
	vdd->act_clock.act_clock_disable_tx_cmds->count =
			ARRAY_SIZE(DSI_CMD_ACTIVE_CLK_CONTROL_DISABLE_CMD);

	vdd->act_clock.blink_info.en = 0;
	vdd->act_clock.blink_info.interval = 5;
	vdd->act_clock.blink_info.radius = 10;
	vdd->act_clock.blink_info.color = 0x000000;
	vdd->act_clock.blink_info.line_width = 0;
	vdd->act_clock.blink_info.pos1_x = 0;
	vdd->act_clock.blink_info.pos1_y = 0;
	vdd->act_clock.blink_info.pos2_x = 0;
	vdd->act_clock.blink_info.pos2_y = 0;

	vdd->act_clock.draw_info.sd_line_color = 0xF0F0F0;
	vdd->act_clock.draw_info.sd2_line_color = 0x000000;
	vdd->act_clock.draw_info.sd_radius = 0x0A;

	vdd->act_clock.file_opend = 0;

	ss_dsi_update_clock_image_to_cmd();

	LCD_INFO("Active_Clock Init --\n");
	return ret;
}
