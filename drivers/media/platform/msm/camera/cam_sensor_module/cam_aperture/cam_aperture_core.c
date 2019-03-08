/* Copyright (c) 2017, The Linux Foundation. All rights reserved.
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

#include <linux/module.h>
#include <cam_sensor_cmn_header.h>
#include "cam_aperture_core.h"
#include "cam_sensor_util.h"
#include "cam_trace.h"

#if defined(CONFIG_SAMSUNG_OIS_RUMBA_S4)
#include "cam_ois_core.h"
#include "cam_ois_rumba_s4.h"
#endif
#if defined(CONFIG_SAMSUNG_OIS_RUMBA_S6)
#include "cam_ois_core.h"
#include "cam_ois_rumba_s6.h"
#endif
#if defined(CONFIG_SAMSUNG_OIS_MCU_STM32)
#include "cam_ois_core.h"
#include "cam_ois_mcu_stm32g.h"
#endif


#ifdef APERTURE_THREAD
#include "cam_aperture_thread.h"
#include <linux/slab.h>
#endif

#if defined(CONFIG_SAMSUNG_OIS_RUMBA_S4) || defined(CONFIG_SAMSUNG_OIS_RUMBA_S6) || defined(CONFIG_SAMSUNG_OIS_MCU_STM32)
extern struct cam_ois_ctrl_t *g_o_ctrl;
#endif

int32_t cam_aperture_construct_default_power_setting(
	struct cam_sensor_power_ctrl_t *power_info)
{
	int rc = 0;

	power_info->power_setting_size = 2;
	power_info->power_setting =
		(struct cam_sensor_power_setting *)
		kzalloc(sizeof(struct cam_sensor_power_setting) * MAX_POWER_CONFIG,
			GFP_KERNEL);
	if (!power_info->power_setting)
		return -ENOMEM;

	power_info->power_setting[0].seq_type = SENSOR_VAF;
	power_info->power_setting[0].seq_val = CAM_VAF;
	power_info->power_setting[0].config_val = 1;

	power_info->power_setting[1].seq_type = SENSOR_VIO;
	power_info->power_setting[1].seq_val = CAM_VIO;
	power_info->power_setting[1].config_val = 1;

	power_info->power_down_setting_size = 2;
	power_info->power_down_setting =
		(struct cam_sensor_power_setting *)
		kzalloc(sizeof(struct cam_sensor_power_setting) * MAX_POWER_CONFIG,
			GFP_KERNEL);
	if (!power_info->power_down_setting) {
		rc = -ENOMEM;
		goto free_power_settings;
	}
	power_info->power_down_setting[0].seq_type = SENSOR_VAF;
	power_info->power_down_setting[0].seq_val = CAM_VAF;
	power_info->power_down_setting[0].config_val = 0;

	power_info->power_down_setting[1].seq_type = SENSOR_VIO;
	power_info->power_down_setting[1].seq_val = CAM_VIO;
	power_info->power_down_setting[1].config_val = 0;

	return rc;

free_power_settings:
	kfree(power_info->power_setting);
	return rc;
}

int32_t cam_aperture_power_down(struct cam_aperture_ctrl_t *a_ctrl)
{
	int32_t rc = 0;
	struct cam_sensor_power_ctrl_t *power_info;
	struct cam_hw_soc_info *soc_info = &a_ctrl->soc_info;
	struct cam_aperture_soc_private  *soc_private;

	if (!a_ctrl) {
		CAM_ERR(CAM_APERTURE, "failed: e_ctrl %pK", a_ctrl);
		return -EINVAL;
	}

	CAM_INFO(CAM_APERTURE, "E");

	soc_private =
		(struct cam_aperture_soc_private *)a_ctrl->soc_info.soc_private;
	power_info = &soc_private->power_info;
	soc_info = &a_ctrl->soc_info;

	if (!power_info) {
		CAM_ERR(CAM_APERTURE, "failed: power_info %pK", power_info);
		return -EINVAL;
	}
	rc = msm_camera_power_down(power_info, soc_info, 0);
	if (rc) {
		CAM_ERR(CAM_APERTURE, "power down the core is failed:%d", rc);
		return rc;
	}

	camera_io_release(&a_ctrl->io_master_info);

	a_ctrl->is_initialized = false;

	CAM_INFO(CAM_APERTURE, "X");

	return rc;
}

int32_t cam_aperture_power_up(struct cam_aperture_ctrl_t *a_ctrl)
{
	int rc = 0;
	struct cam_hw_soc_info  *soc_info =
		&a_ctrl->soc_info;

	struct cam_aperture_soc_private  *soc_private;
	struct cam_sensor_power_ctrl_t *power_info;

	CAM_INFO(CAM_APERTURE, "E");

	soc_private =
		(struct cam_aperture_soc_private *)a_ctrl->soc_info.soc_private;
	power_info = &soc_private->power_info;

	if ((power_info->power_setting == NULL) &&
		(power_info->power_down_setting == NULL)) {
		CAM_INFO(CAM_APERTURE,
				"Using default power settings");
		rc = cam_aperture_construct_default_power_setting(power_info);
		if (rc < 0) {
			CAM_ERR(CAM_APERTURE,
				"Construct default aperture power setting failed.");
			return rc;
		}
	}

	/* Parse and fill vreg params for power up settings */
	rc = msm_camera_fill_vreg_params(
		&a_ctrl->soc_info,
		power_info->power_setting,
		power_info->power_setting_size);
	if (rc) {
		CAM_ERR(CAM_APERTURE,
			"failed to fill vreg params for power up rc:%d", rc);
		return rc;
	}

	/* Parse and fill vreg params for power down settings*/
	rc = msm_camera_fill_vreg_params(
		&a_ctrl->soc_info,
		power_info->power_down_setting,
		power_info->power_down_setting_size);
	if (rc) {
		CAM_ERR(CAM_APERTURE,
		"failed to fill vreg params power down rc:%d", rc);
		return rc;
	}

	power_info->dev = soc_info->dev;

	// Need to i2c lock, otherwise can make ois i2c fail
	i2c_lock_adapter(a_ctrl->io_master_info.client->adapter);
	rc = cam_sensor_core_power_up(power_info, soc_info);
	if (rc) {
		CAM_ERR(CAM_APERTURE,
			"failed in aperture power up rc %d", rc);
		i2c_unlock_adapter(a_ctrl->io_master_info.client->adapter);
		return rc;
	}
	i2c_unlock_adapter(a_ctrl->io_master_info.client->adapter);

	/* VREG needs some delay to power up */
	usleep_range(2000, 2050);

	rc = camera_io_init(&a_ctrl->io_master_info);
	if (rc < 0)
		CAM_ERR(CAM_APERTURE, "cci_init failed");

	CAM_INFO(CAM_APERTURE, "X");
	return rc;
}
static int32_t cam_aperture_i2c_modes_util(
	struct camera_io_master *io_master_info,
	struct i2c_settings_list *i2c_list)
{
	int32_t rc = 0;
	uint32_t i, size;

	if (i2c_list->op_code == CAM_SENSOR_I2C_WRITE_RANDOM) {
		rc = camera_io_dev_write(io_master_info,
			&(i2c_list->i2c_settings));
		if (rc < 0) {
			CAM_ERR(CAM_APERTURE,
				"Failed to random write I2C settings: %d",
				rc);
			return rc;
		}
	} else if (i2c_list->op_code == CAM_SENSOR_I2C_WRITE_SEQ) {
		rc = camera_io_dev_write_continuous(
			io_master_info,
			&(i2c_list->i2c_settings),
			0);
		if (rc < 0) {
			CAM_ERR(CAM_APERTURE,
				"Failed to seq write I2C settings: %d",
				rc);
			return rc;
			}
	} else if (i2c_list->op_code == CAM_SENSOR_I2C_WRITE_BURST) {
		rc = camera_io_dev_write_continuous(
			io_master_info,
			&(i2c_list->i2c_settings),
			1);
		if (rc < 0) {
			CAM_ERR(CAM_APERTURE,
				"Failed to burst write I2C settings: %d",
				rc);
			return rc;
		}
	} else if (i2c_list->op_code == CAM_SENSOR_I2C_POLL) {
		size = i2c_list->i2c_settings.size;
		for (i = 0; i < size; i++) {
			rc = camera_io_dev_poll(
			io_master_info,
			i2c_list->i2c_settings.
				reg_setting[i].reg_addr,
			i2c_list->i2c_settings.
				reg_setting[i].reg_data,
			i2c_list->i2c_settings.
				reg_setting[i].data_mask,
			i2c_list->i2c_settings.data_type,
			i2c_list->i2c_settings.addr_type,
			i2c_list->i2c_settings.
				reg_setting[i].delay);
			if (rc < 0) {
				CAM_ERR(CAM_APERTURE,
					"i2c poll apply setting Fail: %d", rc);
				return rc;
			}
		}
	}

	return rc;
}


int32_t cam_aperture_slaveInfo_pkt_parser(struct cam_aperture_ctrl_t *a_ctrl,
	uint32_t *cmd_buf)
{
	int32_t rc = 0;
	struct cam_cmd_i2c_info *i2c_info;

	if (!a_ctrl || !cmd_buf) {
		CAM_ERR(CAM_APERTURE, "Invalid Args");
		return -EINVAL;
	}

	i2c_info = (struct cam_cmd_i2c_info *)cmd_buf;
	if (a_ctrl->io_master_info.master_type == CCI_MASTER) {
		a_ctrl->io_master_info.cci_client->i2c_freq_mode =
			i2c_info->i2c_freq_mode;
		a_ctrl->io_master_info.cci_client->sid =
			i2c_info->slave_addr >> 1;
		CAM_DBG(CAM_APERTURE, "!!!! Slave addr: 0x%x Freq Mode: %d",
			i2c_info->slave_addr, i2c_info->i2c_freq_mode);
	} else if (a_ctrl->io_master_info.master_type == I2C_MASTER) {
		a_ctrl->io_master_info.client->addr =
			i2c_info->slave_addr;
		CAM_DBG(CAM_APERTURE, "2222 Slave addr: 0x%x Freq Mode: %d",
			i2c_info->slave_addr, i2c_info->i2c_freq_mode);
	} else {
		CAM_ERR(CAM_SENSOR, "Invalid Comm. Master:%d",
			a_ctrl->io_master_info.master_type);
		return -EINVAL;
	}

	return rc;
}

int32_t cam_aperture_apply_settings(struct cam_aperture_ctrl_t *a_ctrl,
	struct i2c_settings_array *i2c_set)
{
	struct i2c_settings_list *i2c_list;
	int32_t rc = 0;

	if (a_ctrl == NULL || i2c_set == NULL) {
		CAM_ERR(CAM_APERTURE, "Invalid Args");
		return -EINVAL;
	}

	if (i2c_set->is_settings_valid != 1) {
		CAM_ERR(CAM_APERTURE, " Invalid settings");
		return -EINVAL;
	}

	list_for_each_entry(i2c_list,
		&(i2c_set->list_head), list) {
		rc = cam_aperture_i2c_modes_util(
					&(a_ctrl->io_master_info),
			i2c_list);
				if (rc < 0) {
					CAM_ERR(CAM_APERTURE,
				"Failed to apply settings: %d",
				rc);
					return rc;
				}

	}

	return rc;
}

int32_t cam_aperture_apply_request(struct cam_req_mgr_apply_request *apply)
{
	int32_t rc = 0, request_id, del_req_id;
	struct cam_aperture_ctrl_t *a_ctrl = NULL;

	if (!apply) {
		CAM_ERR(CAM_APERTURE, "Invalid Input Args");
		return -EINVAL;
	}

	a_ctrl = (struct cam_aperture_ctrl_t *)
		cam_get_device_priv(apply->dev_hdl);
	if (!a_ctrl) {
		CAM_ERR(CAM_APERTURE, "Device data is NULL");
		return -EINVAL;
	}
	request_id = apply->request_id % MAX_PER_FRAME_ARRAY;

	trace_cam_apply_req("Aperture", (uint64_t)apply);

	CAM_DBG(CAM_APERTURE, "Request Id: %lld", apply->request_id);

	if ((apply->request_id ==
		a_ctrl->i2c_data.per_frame[request_id].request_id) &&
		(a_ctrl->i2c_data.per_frame[request_id].is_settings_valid)
		== 1) {
		rc = cam_aperture_apply_settings(a_ctrl,
			&a_ctrl->i2c_data.per_frame[request_id]);
		if (rc < 0) {
			CAM_ERR(CAM_APERTURE,
				"Failed in applying the request: %lld\n",
				apply->request_id);
			return rc;
		}
	}
	del_req_id = (request_id +
		MAX_PER_FRAME_ARRAY - MAX_SYSTEM_PIPELINE_DELAY) %
		MAX_PER_FRAME_ARRAY;

	if (apply->request_id >
		a_ctrl->i2c_data.per_frame[del_req_id].request_id) {
		a_ctrl->i2c_data.per_frame[del_req_id].request_id = 0;
		rc = delete_request(&a_ctrl->i2c_data.per_frame[del_req_id]);
		if (rc < 0) {
			CAM_ERR(CAM_APERTURE,
				"Fail deleting the req: %d err: %d\n",
				del_req_id, rc);
			return rc;
		}
	} else {
		CAM_DBG(CAM_APERTURE, "No Valid Req to clean Up");
	}

	return rc;
}

int32_t cam_aperture_establish_link(
	struct cam_req_mgr_core_dev_link_setup *link)
{
	struct cam_aperture_ctrl_t *a_ctrl = NULL;

	if (!link) {
		CAM_ERR(CAM_APERTURE, "Invalid Args");
		return -EINVAL;
	}

	a_ctrl = (struct cam_aperture_ctrl_t *)
		cam_get_device_priv(link->dev_hdl);
	if (!a_ctrl) {
		CAM_ERR(CAM_APERTURE, "Device data is NULL");
		return -EINVAL;
	}
	if (link->link_enable) {
		a_ctrl->bridge_intf.link_hdl = link->link_hdl;
		a_ctrl->bridge_intf.crm_cb = link->crm_cb;
	} else {
		a_ctrl->bridge_intf.link_hdl = -1;
		a_ctrl->bridge_intf.crm_cb = NULL;
	}

	return 0;
}

int32_t cam_aperture_publish_dev_info(struct cam_req_mgr_device_info *info)
{
	if (!info) {
		CAM_ERR(CAM_APERTURE, "Invalid Args");
		return -EINVAL;
	}

	info->dev_id = CAM_REQ_MGR_DEVICE_APERTURE;
	strlcpy(info->name, CAM_APERTURE_NAME, sizeof(info->name));
	info->p_delay = 0;

	return 0;
}

int32_t cam_aperture_i2c_pkt_parse(struct cam_aperture_ctrl_t *a_ctrl,
	void *arg)
{
	int32_t  rc = 0;
	int32_t  i = 0;
	uint32_t total_cmd_buf_in_bytes = 0;
	size_t   len_of_buff = 0;
	uint32_t *offset = NULL;
	uint32_t *cmd_buf = NULL;
	uint64_t generic_ptr;
	struct common_header      *cmm_hdr = NULL;
	struct cam_control        *ioctl_ctrl = NULL;
	struct cam_packet         *csl_packet = NULL;
	struct cam_config_dev_cmd config;
#if 0
	struct i2c_data_settings  *i2c_data = NULL;
#endif
	struct i2c_settings_array *i2c_reg_settings = NULL;
	struct cam_cmd_buf_desc   *cmd_desc = NULL;
	struct cam_req_mgr_add_request  add_req;
	struct cam_aperture_soc_private *soc_private = NULL;
	struct cam_sensor_power_ctrl_t  *power_info = NULL;
#ifdef APERTURE_THREAD
	struct cam_aperture_thread_msg_t    *msg = NULL;
#endif

	if (!a_ctrl || !arg) {
		CAM_ERR(CAM_APERTURE, "Invalid Args");
		return -EINVAL;
	}

	soc_private =
		(struct cam_aperture_soc_private *)a_ctrl->soc_info.soc_private;

	power_info = &soc_private->power_info;

	ioctl_ctrl = (struct cam_control *)arg;
	if (copy_from_user(&config, (void __user *) ioctl_ctrl->handle,
		sizeof(config)))
		return -EFAULT;
	rc = cam_mem_get_cpu_buf(config.packet_handle,
		(uint64_t *)&generic_ptr, &len_of_buff);
	if (rc < 0) {
		CAM_ERR(CAM_APERTURE, "Error in converting command Handle %d",
			rc);
		return rc;
	}

	if (config.offset > len_of_buff) {
		CAM_ERR(CAM_APERTURE,
			"offset is out of bounds: offset: %lld len: %zu",
			config.offset, len_of_buff);
		return -EINVAL;
	}

	csl_packet = (struct cam_packet *)(generic_ptr + config.offset);
	CAM_DBG(CAM_APERTURE, "Pkt opcode: %d", csl_packet->header.op_code);

	switch (csl_packet->header.op_code & 0xFFFFFF) {
	case CAM_APERTURE_PACKET_OPCODE_INIT:
		if (a_ctrl->is_initialized)
			return rc;

		offset = (uint32_t *)&csl_packet->payload;
		offset += (csl_packet->cmd_buf_offset / sizeof(uint32_t));
		cmd_desc = (struct cam_cmd_buf_desc *)(offset);

		/* Loop through multiple command buffers */
		for (i = 0; i < csl_packet->num_cmd_buf; i++) {
			total_cmd_buf_in_bytes = cmd_desc[i].length;
			if (!total_cmd_buf_in_bytes)
				continue;
			rc = cam_mem_get_cpu_buf(cmd_desc[i].mem_handle,
					(uint64_t *)&generic_ptr, &len_of_buff);
			if (rc < 0) {
				CAM_ERR(CAM_APERTURE, "Failed to get cpu buf");
				return rc;
			}
			cmd_buf = (uint32_t *)generic_ptr;
			if (!cmd_buf) {
				CAM_ERR(CAM_APERTURE, "invalid cmd buf");
				return -EINVAL;
			}
			cmd_buf += cmd_desc[i].offset / sizeof(uint32_t);
			cmm_hdr = (struct common_header *)cmd_buf;

			switch (cmm_hdr->cmd_type) {
			case CAMERA_SENSOR_CMD_TYPE_I2C_INFO:
				CAM_DBG(CAM_APERTURE,
					"Received slave info buffer");
				rc = cam_aperture_slaveInfo_pkt_parser(
					a_ctrl, cmd_buf);
				if (rc < 0) {
					CAM_ERR(CAM_APERTURE,
					"Failed to parse slave info: %d", rc);
					return rc;
				}
				break;
			case CAMERA_SENSOR_CMD_TYPE_PWR_UP:
			case CAMERA_SENSOR_CMD_TYPE_PWR_DOWN:
				CAM_DBG(CAM_APERTURE,
					"Received power settings buffer");
				rc = cam_sensor_update_power_settings(
					cmd_buf,
					total_cmd_buf_in_bytes,
					power_info);
				if (rc) {
					CAM_ERR(CAM_APERTURE,
					"Failed:parse power settings: %d",
					rc);
					return rc;
				}
				break;
			default:
#if 0
				CAM_DBG(CAM_APERTURE,
					"Received initSettings buffer");
				i2c_data = &(a_ctrl->i2c_data);
				i2c_reg_settings =
					&i2c_data->init_settings;

				i2c_reg_settings->request_id = 0;
				i2c_reg_settings->is_settings_valid = 1;
				rc = cam_sensor_i2c_command_parser(
					i2c_reg_settings,
					&cmd_desc[i], 1);
				if (rc < 0) {
					CAM_ERR(CAM_APERTURE,
					"Failed:parse init settings: %d",
					rc);
					return rc;
				}
#endif
				break;
			}
		}

		if (a_ctrl->cam_act_state == CAM_APERTURE_ACQUIRE) {
			rc = cam_aperture_power_up(a_ctrl);
			if (rc < 0) {
				CAM_ERR(CAM_APERTURE,
					"Aperture Power up failed");
				return rc;
			}
			a_ctrl->cam_act_state = CAM_APERTURE_CONFIG;
		}
#if 0
		rc = cam_aperture_apply_settings(a_ctrl,
			&a_ctrl->i2c_data.init_settings);
		if (rc < 0) {
			CAM_ERR(CAM_APERTURE, "Cannot apply Init settings");
			return rc;
		}

		/* Delete the request even if the apply is failed */
		rc = delete_request(&a_ctrl->i2c_data.init_settings);
		if (rc < 0) {
			CAM_WARN(CAM_APERTURE,
				"Fail in deleting the Init settings");
			rc = 0;
		}
#endif
#ifdef APERTURE_THREAD
		msg = kmalloc(sizeof(struct cam_aperture_thread_msg_t), GFP_KERNEL);
		if (msg == NULL) {
			CAM_ERR(CAM_APERTURE, "Failed alloc memory for msg, Out of memory");
			return -ENOMEM;
		}

		memset(msg, 0, sizeof(struct cam_aperture_thread_msg_t));
		msg->i2c_reg_settings = i2c_reg_settings;
		msg->msg_type = CAM_APERTURE_THREAD_MSG_INIT;
		rc = cam_aperture_thread_add_msg(a_ctrl, msg);
		if (rc < 0)
			CAM_ERR(CAM_APERTURE, "Failed add msg to APERTURE thread");
#else
		cam_aperture_init(&a_ctrl->io_master_info);
#endif
		a_ctrl->cam_aper_mode = CAM_APERTURE_2_4;
		a_ctrl->is_initialized = true;
		break;
	case CAM_APERTURE_PACKET_AUTO_MOVE_LENS:
		if (a_ctrl->cam_act_state < CAM_APERTURE_CONFIG) {
			rc = -EINVAL;
			CAM_WARN(CAM_APERTURE,
				"Not in right state to aperture high: %d",
				a_ctrl->cam_act_state);
			return rc;
		}

		cam_aperture_high(&a_ctrl->io_master_info); //close aperture mode
		a_ctrl->cam_aper_mode = CAM_APERTURE_1_5;
		break;
	case CAM_APERTURE_PACKET_MANUAL_MOVE_LENS:
		if (a_ctrl->cam_act_state < CAM_APERTURE_CONFIG) {
			rc = -EINVAL;
			CAM_WARN(CAM_APERTURE,
				"Not in right state to aperture low: %d",
				a_ctrl->cam_act_state);
			return rc;
		}
		cam_aperture_low(&a_ctrl->io_master_info); //open aperture mode
		a_ctrl->cam_aper_mode = CAM_APERTURE_2_4;
		break;
	}

	if ((csl_packet->header.op_code & 0xFFFFFF) !=
		CAM_APERTURE_PACKET_OPCODE_INIT) {
		add_req.link_hdl = a_ctrl->bridge_intf.link_hdl;
		add_req.req_id = csl_packet->header.request_id;
		add_req.dev_hdl = a_ctrl->bridge_intf.device_hdl;
		if (a_ctrl->bridge_intf.crm_cb &&
			a_ctrl->bridge_intf.crm_cb->add_req)
			a_ctrl->bridge_intf.crm_cb->add_req(&add_req);
		CAM_DBG(CAM_APERTURE, "Req Id: %lld added to Bridge",
			add_req.req_id);
	}

	rc = camera_io_init(&a_ctrl->io_master_info);
	if (rc < 0)
		CAM_ERR(CAM_APERTURE, "cci_init failed");

	return rc;
}

void cam_aperture_shutdown(struct cam_aperture_ctrl_t *a_ctrl)
{
	int rc = 0;
	struct cam_aperture_soc_private  *soc_private =
		(struct cam_aperture_soc_private *)a_ctrl->soc_info.soc_private;
	struct cam_sensor_power_ctrl_t *power_info =
		&soc_private->power_info;

	if (a_ctrl->cam_act_state == CAM_APERTURE_INIT)
		return;

	if (a_ctrl->cam_act_state >= CAM_APERTURE_CONFIG) {

		rc = cam_aperture_power_down(a_ctrl);
		if (rc < 0)
			CAM_ERR(CAM_APERTURE, "Aperture Power down failed");
		a_ctrl->cam_act_state = CAM_APERTURE_ACQUIRE;
	}

	if (a_ctrl->cam_act_state >= CAM_APERTURE_ACQUIRE) {
#ifdef APERTURE_THREAD
		cam_aperture_thread_destroy(a_ctrl);
#endif
		rc = cam_destroy_device_hdl(a_ctrl->bridge_intf.device_hdl);
		if (rc < 0)
			CAM_ERR(CAM_APERTURE, "destroying  dhdl failed");
		a_ctrl->bridge_intf.device_hdl = -1;
		a_ctrl->bridge_intf.link_hdl = -1;
		a_ctrl->bridge_intf.session_hdl = -1;
	}

	if(NULL != power_info->power_setting) {
		kfree(power_info->power_setting);
		power_info->power_setting = NULL;
	}

	if(NULL != power_info->power_down_setting) {
		kfree(power_info->power_down_setting);
		power_info->power_down_setting = NULL;
	}
	a_ctrl->cam_act_state = CAM_APERTURE_INIT;

	a_ctrl->is_initialized = false;
}

int32_t cam_aperture_driver_cmd(struct cam_aperture_ctrl_t *a_ctrl,
	void *arg)
{
	int rc = 0;
	struct cam_control *cmd = (struct cam_control *)arg;

	if (!a_ctrl || !cmd) {
		CAM_ERR(CAM_APERTURE, " Invalid Args");
		return -EINVAL;
	}
	CAM_DBG(CAM_APERTURE, "E");

	CAM_DBG(CAM_APERTURE, "Opcode to Aperture : 0x%x", cmd->op_code);

	mutex_lock(&(a_ctrl->aperture_mutex));
	switch (cmd->op_code) {
	case CAM_ACQUIRE_DEV: {
		struct cam_sensor_acquire_dev aperture_acq_dev;
		struct cam_create_dev_hdl bridge_params;

		if (a_ctrl->bridge_intf.device_hdl != -1) {
			CAM_ERR(CAM_APERTURE, "Device is already acquired");
			rc = -EINVAL;
			goto release_mutex;
		}
		rc = copy_from_user(&aperture_acq_dev,
			(void __user *) cmd->handle,
			sizeof(aperture_acq_dev));
		if (rc < 0) {
			CAM_ERR(CAM_APERTURE, "Failed Copying from user\n");
			goto release_mutex;
		}

		bridge_params.session_hdl = aperture_acq_dev.session_handle;
		bridge_params.ops = &a_ctrl->bridge_intf.ops;
		bridge_params.v4l2_sub_dev_flag = 0;
		bridge_params.media_entity_flag = 0;
		bridge_params.priv = a_ctrl;

		aperture_acq_dev.device_handle =
			cam_create_device_hdl(&bridge_params);
		a_ctrl->bridge_intf.device_hdl = aperture_acq_dev.device_handle;
		a_ctrl->bridge_intf.session_hdl =
			aperture_acq_dev.session_handle;

		CAM_DBG(CAM_APERTURE, "Device Handle: %d",
			aperture_acq_dev.device_handle);
		if (copy_to_user((void __user *) cmd->handle, &aperture_acq_dev,
			sizeof(struct cam_sensor_acquire_dev))) {
			CAM_ERR(CAM_APERTURE, "Failed Copy to User");
			rc = -EFAULT;
			goto release_mutex;
		}

#ifdef APERTURE_THREAD
		rc = cam_aperture_thread_create(a_ctrl);
		if (rc < 0) {
			CAM_ERR(CAM_APERTURE, "Failed create APERTURE thread");
			goto release_mutex;
		}
#endif
		a_ctrl->cam_act_state = CAM_APERTURE_ACQUIRE;

	}
		break;
	case CAM_RELEASE_DEV: {
		if (a_ctrl->cam_act_state == CAM_APERTURE_START) {
			rc = -EINVAL;
			CAM_WARN(CAM_APERTURE,
			"Not in right state to release : %d",
			a_ctrl->cam_act_state);
			goto release_mutex;
		}
#ifdef APERTURE_THREAD
		cam_aperture_thread_destroy(a_ctrl);
#endif
		if (a_ctrl->cam_act_state == CAM_APERTURE_CONFIG) {
			if (a_ctrl->cam_aper_mode == CAM_APERTURE_2_4) {
				CAM_INFO(CAM_APERTURE, "set aperture mode (1.5)");
				cam_aperture_high(&a_ctrl->io_master_info); //1.5
				a_ctrl->cam_aper_mode = CAM_APERTURE_1_5;
			}

			rc = cam_aperture_power_down(a_ctrl);
			if (rc < 0) {
				CAM_ERR(CAM_APERTURE, "Aperture Power down failed");
				goto release_mutex;
			}
		}
		if (a_ctrl->bridge_intf.device_hdl == -1) {
			CAM_ERR(CAM_APERTURE, "link hdl: %d device hdl: %d",
				a_ctrl->bridge_intf.device_hdl,
				a_ctrl->bridge_intf.link_hdl);
			rc = -EINVAL;
			goto release_mutex;
		}


		rc = cam_destroy_device_hdl(a_ctrl->bridge_intf.device_hdl);
		if (rc < 0)
			CAM_ERR(CAM_APERTURE, "destroying the device hdl");
		a_ctrl->bridge_intf.device_hdl = -1;
		a_ctrl->bridge_intf.link_hdl = -1;
		a_ctrl->bridge_intf.session_hdl = -1;
		a_ctrl->cam_act_state = CAM_APERTURE_INIT;

	}
		break;
	case CAM_QUERY_CAP: {
		struct cam_aperture_query_cap aperture_cap = {0};

		aperture_cap.slot_info = a_ctrl->id;
		if (copy_to_user((void __user *) cmd->handle, &aperture_cap,
			sizeof(struct cam_aperture_query_cap))) {
			CAM_ERR(CAM_APERTURE, "Failed Copy to User");
			rc = -EFAULT;
			goto release_mutex;
		}
	}
		break;
	case CAM_START_DEV: {
		if (a_ctrl->cam_act_state != CAM_APERTURE_CONFIG) {
			rc = -EINVAL;
			CAM_WARN(CAM_APERTURE,
			"Not in right state to start : %d",
			a_ctrl->cam_act_state);
			goto release_mutex;
		}
		a_ctrl->cam_act_state = CAM_APERTURE_START;
	}
		break;
	case CAM_STOP_DEV: {
		if (a_ctrl->cam_act_state != CAM_APERTURE_START) {
			rc = -EINVAL;
			CAM_WARN(CAM_APERTURE,
				"Not in right state to stop : %d",
				a_ctrl->cam_act_state);
			goto release_mutex;
		}
		a_ctrl->cam_act_state = CAM_APERTURE_CONFIG;
	}
		break;
	case CAM_CONFIG_DEV: {
		a_ctrl->setting_apply_state =
			APERTURE_APPLY_SETTINGS_LATER;
		rc = cam_aperture_i2c_pkt_parse(a_ctrl, arg);
		if (rc < 0)
			CAM_ERR(CAM_APERTURE, "Failed in aperture Parsing");

		if (a_ctrl->setting_apply_state ==
			APERTURE_APPLY_SETTINGS_NOW) {
#if 0
			rc = cam_aperture_apply_settings(a_ctrl,
				&a_ctrl->i2c_data.init_settings);
			if (rc < 0)
				CAM_ERR(CAM_APERTURE,
					"Cannot apply Update settings");

			/* Delete the request even if the apply is failed */
			rc = delete_request(&a_ctrl->i2c_data.init_settings);
			if (rc < 0) {
				CAM_ERR(CAM_APERTURE,
					"Failed in Deleting the Init Pkt: %d",
					rc);
				goto release_mutex;
			}
#else
			cam_aperture_init(&a_ctrl->io_master_info);
			a_ctrl->cam_aper_mode = CAM_APERTURE_2_4;
#endif
		}
	}
		break;
	default:
		CAM_ERR(CAM_APERTURE, "Invalid Opcode %d", cmd->op_code);
	}

release_mutex:
	mutex_unlock(&(a_ctrl->aperture_mutex));

	CAM_DBG(CAM_APERTURE, "X");
	return rc;
}


int cam_aperture_init(struct camera_io_master *client)
{
	int rc = 0;
	int pos = 0;
	u8 data[2] = {0, };
	uint32_t size;
	struct cam_sensor_i2c_reg_array reg_setting;
#if defined(CONFIG_SAMSUNG_OIS_RUMBA_S4) || defined(CONFIG_SAMSUNG_OIS_RUMBA_S6) || defined(CONFIG_SAMSUNG_OIS_MCU_STM32)
	uint16_t ois_mode = 0;
#endif

	CAM_INFO(CAM_APERTURE, "E");

#if defined(CONFIG_SAMSUNG_OIS_RUMBA_S4) || defined(CONFIG_SAMSUNG_OIS_RUMBA_S6) || defined(CONFIG_SAMSUNG_OIS_MCU_STM32)
	// OIS Cetering Mode
	if (g_o_ctrl != NULL) {
		mutex_lock(&(g_o_ctrl->ois_mode_mutex));
		rc = cam_ois_get_ois_mode(g_o_ctrl, &ois_mode);
		if (rc >= 0) {
			if (ois_mode != 0x05) {
				cam_ois_set_ois_mode(g_o_ctrl, 0x05); // OIS centering
				usleep_range(40000, 41000);
				CAM_INFO(CAM_APERTURE, "OIS centering, previous OIS mode %d", ois_mode);
			} else {
				CAM_INFO(CAM_APERTURE, "previous OIS mode %d", ois_mode);
			}
		}
	}
#endif

	size = 1;

	reg_setting.data_mask = 1;//needed check !!
	reg_setting.delay = 0;

	reg_setting.reg_addr = 0xAE;
	reg_setting.reg_data = 0x3B; //setting mode

	client->client->addr = 0x98;

	rc = cam_qup_i2c_write(client, &reg_setting, CAMERA_SENSOR_I2C_TYPE_BYTE, CAMERA_SENSOR_I2C_TYPE_BYTE);

	pos = 511;
	data[0] = pos >> 2 & 0xFF;
	data[1] = pos << 6 & 0xC0; // 511 code


	reg_setting.reg_addr = 0x00;
	reg_setting.reg_data = data[0];

	rc |= cam_qup_i2c_write(client, &reg_setting, CAMERA_SENSOR_I2C_TYPE_BYTE, CAMERA_SENSOR_I2C_TYPE_BYTE);

	reg_setting.reg_addr = 0x01;
	reg_setting.reg_data = data[1];

	rc |= cam_qup_i2c_write(client, &reg_setting, CAMERA_SENSOR_I2C_TYPE_BYTE, CAMERA_SENSOR_I2C_TYPE_BYTE);

	reg_setting.reg_addr = 0x02;
	reg_setting.reg_data = 0x00;

	rc |= cam_qup_i2c_write(client, &reg_setting, CAMERA_SENSOR_I2C_TYPE_BYTE, CAMERA_SENSOR_I2C_TYPE_BYTE);

	usleep_range(10000, 10050);

#if defined(CONFIG_SAMSUNG_OIS_RUMBA_S4) || defined(CONFIG_SAMSUNG_OIS_RUMBA_S6) || defined(CONFIG_SAMSUNG_OIS_MCU_STM32)
	if (g_o_ctrl != NULL) {
		// OIS CMD(Fixed Aperture)
		//rc |= cam_ois_fixed_aperture(g_o_ctrl);
		//usleep_range(15000, 15050);
	}
#endif

	reg_setting.reg_addr = 0xA6;
	reg_setting.reg_data = 0x7B;

	rc |= cam_qup_i2c_write(client, &reg_setting, CAMERA_SENSOR_I2C_TYPE_BYTE, CAMERA_SENSOR_I2C_TYPE_BYTE);

	pos = 1023;
	data[0] = pos >> 2 & 0xFF;
	data[1] = pos << 6 & 0xC0;

	reg_setting.reg_addr = 0x00;
	reg_setting.reg_data = data[0];

	rc |= cam_qup_i2c_write(client, &reg_setting, CAMERA_SENSOR_I2C_TYPE_BYTE, CAMERA_SENSOR_I2C_TYPE_BYTE);

	reg_setting.reg_addr = 0x01;
	reg_setting.reg_data = data[1];

	rc |= cam_qup_i2c_write(client, &reg_setting, CAMERA_SENSOR_I2C_TYPE_BYTE, CAMERA_SENSOR_I2C_TYPE_BYTE);

	usleep_range(10000, 11000);

	reg_setting.reg_addr = 0xA6;
	reg_setting.reg_data = 0x00;

	rc |= cam_qup_i2c_write(client, &reg_setting, CAMERA_SENSOR_I2C_TYPE_BYTE, CAMERA_SENSOR_I2C_TYPE_BYTE);

	pos = 511;
	data[0] = pos >> 2 & 0xFF;
	data[1] = pos << 6 & 0xC0;

	reg_setting.reg_addr = 0x00;
	reg_setting.reg_data = data[0];

	rc |= cam_qup_i2c_write(client, &reg_setting, CAMERA_SENSOR_I2C_TYPE_BYTE, CAMERA_SENSOR_I2C_TYPE_BYTE);

	reg_setting.reg_addr = 0x01;
	reg_setting.reg_data = data[1];

	rc |= cam_qup_i2c_write(client, &reg_setting, CAMERA_SENSOR_I2C_TYPE_BYTE, CAMERA_SENSOR_I2C_TYPE_BYTE);
	usleep_range(15000, 15050);

#if defined(CONFIG_SAMSUNG_OIS_RUMBA_S4) || defined(CONFIG_SAMSUNG_OIS_RUMBA_S6) || defined(CONFIG_SAMSUNG_OIS_MCU_STM32)
	if (g_o_ctrl != NULL) {
		cam_ois_set_ois_mode(g_o_ctrl, 0x05); // OIS centering
		msleep(20);
		cam_ois_set_ois_mode(g_o_ctrl, ois_mode); // OIS
		CAM_INFO(CAM_APERTURE, "restore OIS mode %d", ois_mode);
		mutex_unlock(&(g_o_ctrl->ois_mode_mutex));
	}
#endif

	usleep_range(30000, 31000);

	if (rc < 0)
		CAM_ERR(CAM_APERTURE, "i2c fail occurred.");

	CAM_INFO(CAM_APERTURE, "X");

	return rc;
}

int cam_aperture_init_fast(struct cam_aperture_ctrl_t *a_ctrl)
{
	int rc = 0;
	int pos = 0;
	u8 data[2] = {0, };
	uint32_t size;
	struct cam_sensor_i2c_reg_array reg_setting;
        struct camera_io_master *client = &a_ctrl->io_master_info;
#if defined(CONFIG_SAMSUNG_OIS_RUMBA_S4) || defined(CONFIG_SAMSUNG_OIS_RUMBA_S6) || defined(CONFIG_SAMSUNG_OIS_MCU_STM32)
	uint16_t ois_mode = 0;
#endif
	CAM_INFO(CAM_APERTURE, "E");

	mutex_lock(&(a_ctrl->aperture_mutex));
#if defined(CONFIG_SAMSUNG_OIS_RUMBA_S4) || defined(CONFIG_SAMSUNG_OIS_RUMBA_S6) || defined(CONFIG_SAMSUNG_OIS_MCU_STM32)
	// OIS Cetering Mode
	if (g_o_ctrl != NULL) {
		mutex_lock(&(g_o_ctrl->ois_mode_mutex));
		rc = cam_ois_get_ois_mode(g_o_ctrl, &ois_mode);
		if (rc >= 0) {
			if (ois_mode != 0x05) {
				cam_ois_set_ois_mode(g_o_ctrl, 0x05); // OIS centering
				usleep_range(40000, 41000);
				CAM_INFO(CAM_APERTURE, "OIS centering, previous OIS mode %d", ois_mode);
			} else {
				CAM_INFO(CAM_APERTURE, "previous OIS mode %d", ois_mode);
			}
		}
	}
#endif

	size = 1;

	reg_setting.data_mask = 1;//needed check !!
	reg_setting.delay = 0;

	reg_setting.reg_addr = 0xAE;
	reg_setting.reg_data = 0x3B; //setting mode

	client->client->addr = 0x98;

	rc = cam_qup_i2c_write(client, &reg_setting, CAMERA_SENSOR_I2C_TYPE_BYTE, CAMERA_SENSOR_I2C_TYPE_BYTE);

	pos = 511;
	data[0] = pos >> 2 & 0xFF;
	data[1] = pos << 6 & 0xC0; // 511 code


	reg_setting.reg_addr = 0x00;
	reg_setting.reg_data = data[0];

	rc |= cam_qup_i2c_write(client, &reg_setting, CAMERA_SENSOR_I2C_TYPE_BYTE, CAMERA_SENSOR_I2C_TYPE_BYTE);

	reg_setting.reg_addr = 0x01;
	reg_setting.reg_data = data[1];

	rc |= cam_qup_i2c_write(client, &reg_setting, CAMERA_SENSOR_I2C_TYPE_BYTE, CAMERA_SENSOR_I2C_TYPE_BYTE);

	reg_setting.reg_addr = 0x02;
	reg_setting.reg_data = 0x00;

	rc |= cam_qup_i2c_write(client, &reg_setting, CAMERA_SENSOR_I2C_TYPE_BYTE, CAMERA_SENSOR_I2C_TYPE_BYTE);

	usleep_range(10000, 10050);

#if defined(CONFIG_SAMSUNG_OIS_RUMBA_S4) || defined(CONFIG_SAMSUNG_OIS_RUMBA_S6) || defined(CONFIG_SAMSUNG_OIS_MCU_STM32)
	if (g_o_ctrl != NULL) {
		// OIS CMD(Fixed Aperture)
		//rc |= cam_ois_fixed_aperture(g_o_ctrl);
		//usleep_range(15000, 15050);
	}
#endif

	reg_setting.reg_addr = 0xA6;
	reg_setting.reg_data = 0x7B;

	rc |= cam_qup_i2c_write(client, &reg_setting, CAMERA_SENSOR_I2C_TYPE_BYTE, CAMERA_SENSOR_I2C_TYPE_BYTE);

	pos = 1023;
	data[0] = pos >> 2 & 0xFF;
	data[1] = pos << 6 & 0xC0;

	reg_setting.reg_addr = 0x00;
	reg_setting.reg_data = data[0];

	rc |= cam_qup_i2c_write(client, &reg_setting, CAMERA_SENSOR_I2C_TYPE_BYTE, CAMERA_SENSOR_I2C_TYPE_BYTE);

	reg_setting.reg_addr = 0x01;
	reg_setting.reg_data = data[1];

	rc |= cam_qup_i2c_write(client, &reg_setting, CAMERA_SENSOR_I2C_TYPE_BYTE, CAMERA_SENSOR_I2C_TYPE_BYTE);

	usleep_range(10000, 11000);

	reg_setting.reg_addr = 0xA6;
	reg_setting.reg_data = 0x00;

	rc |= cam_qup_i2c_write(client, &reg_setting, CAMERA_SENSOR_I2C_TYPE_BYTE, CAMERA_SENSOR_I2C_TYPE_BYTE);

	pos = 511;
	data[0] = pos >> 2 & 0xFF;
	data[1] = pos << 6 & 0xC0;

	reg_setting.reg_addr = 0x00;
	reg_setting.reg_data = data[0];

	rc |= cam_qup_i2c_write(client, &reg_setting, CAMERA_SENSOR_I2C_TYPE_BYTE, CAMERA_SENSOR_I2C_TYPE_BYTE);

	reg_setting.reg_addr = 0x01;
	reg_setting.reg_data = data[1];

	rc |= cam_qup_i2c_write(client, &reg_setting, CAMERA_SENSOR_I2C_TYPE_BYTE, CAMERA_SENSOR_I2C_TYPE_BYTE);

	mutex_unlock(&(a_ctrl->aperture_mutex)); // minimize the stream on start time for fastAE operation

	usleep_range(15000, 15050);

#if defined(CONFIG_SAMSUNG_OIS_RUMBA_S4) || defined(CONFIG_SAMSUNG_OIS_RUMBA_S6) || defined(CONFIG_SAMSUNG_OIS_MCU_STM32)
	if (g_o_ctrl != NULL) {
		cam_ois_set_ois_mode(g_o_ctrl, 0x05); // OIS centering
		msleep(20);
		cam_ois_set_ois_mode(g_o_ctrl, ois_mode); // OIS
		CAM_INFO(CAM_APERTURE, "restore OIS mode %d", ois_mode);
		mutex_unlock(&(g_o_ctrl->ois_mode_mutex));
	}
#endif
	if (rc < 0)
		CAM_ERR(CAM_APERTURE, "i2c fail occurred.");

	CAM_INFO(CAM_APERTURE, "X");

	return rc;
}

int cam_aperture_low(struct camera_io_master *client)
{
	int rc = 0;
	int pos = 0;
	u8 data[2] = {0, };
	uint32_t size;
	struct cam_sensor_i2c_reg_array reg_setting;
#if defined(CONFIG_SAMSUNG_OIS_RUMBA_S4) || defined(CONFIG_SAMSUNG_OIS_RUMBA_S6) || defined(CONFIG_SAMSUNG_OIS_MCU_STM32)
	uint16_t ois_mode = 0;
#endif

	CAM_INFO(CAM_APERTURE, "E");

#if defined(CONFIG_SAMSUNG_OIS_RUMBA_S4) || defined(CONFIG_SAMSUNG_OIS_RUMBA_S6) || defined(CONFIG_SAMSUNG_OIS_MCU_STM32)
	if (g_o_ctrl != NULL) {
		mutex_lock(&(g_o_ctrl->ois_mode_mutex));
		rc = cam_ois_get_ois_mode(g_o_ctrl, &ois_mode);
		if (rc >= 0) {
			cam_ois_set_ois_mode(g_o_ctrl, 0x05); // OIS centering
			usleep_range(10000, 11000);
			CAM_INFO(CAM_APERTURE, "OIS centering, previous OIS mode %d", ois_mode);
		}

		// OIS CMD(Fixed Aperture)
		//rc |= cam_ois_fixed_aperture(g_o_ctrl);
		//usleep_range(15000, 15050);
	}
#endif

	size = 1;

	reg_setting.data_mask = 1;//needed check !!
	reg_setting.delay = 0;

	reg_setting.reg_addr = 0xA6;
	reg_setting.reg_data = 0x7B;

	client->client->addr = 0x98;

	rc = cam_qup_i2c_write(client, &reg_setting, CAMERA_SENSOR_I2C_TYPE_BYTE, CAMERA_SENSOR_I2C_TYPE_BYTE);

	pos = 1023;
	data[0] = pos >> 2 & 0xFF;
	data[1] = pos << 6 & 0xC0;

	reg_setting.reg_addr = 0x00;
	reg_setting.reg_data = data[0];

	rc |= cam_qup_i2c_write(client, &reg_setting, CAMERA_SENSOR_I2C_TYPE_BYTE, CAMERA_SENSOR_I2C_TYPE_BYTE);

	reg_setting.reg_addr = 0x01;
	reg_setting.reg_data = data[1];

	rc |= cam_qup_i2c_write(client, &reg_setting, CAMERA_SENSOR_I2C_TYPE_BYTE, CAMERA_SENSOR_I2C_TYPE_BYTE);

	usleep_range(10000, 11000);

	reg_setting.reg_addr = 0xA6;
	reg_setting.reg_data = 0x00;

	rc |= cam_qup_i2c_write(client, &reg_setting, CAMERA_SENSOR_I2C_TYPE_BYTE, CAMERA_SENSOR_I2C_TYPE_BYTE);

	pos = 511;
	data[0] = pos >> 2 & 0xFF;
	data[1] = pos << 6 & 0xC0;

	reg_setting.reg_addr = 0x00;
	reg_setting.reg_data = data[0];

	rc |= cam_qup_i2c_write(client, &reg_setting, CAMERA_SENSOR_I2C_TYPE_BYTE, CAMERA_SENSOR_I2C_TYPE_BYTE);

	reg_setting.reg_addr = 0x01;
	reg_setting.reg_data = data[1];

	rc |= cam_qup_i2c_write(client, &reg_setting, CAMERA_SENSOR_I2C_TYPE_BYTE, CAMERA_SENSOR_I2C_TYPE_BYTE);
	usleep_range(15000, 15050);

#if defined(CONFIG_SAMSUNG_OIS_RUMBA_S4) || defined(CONFIG_SAMSUNG_OIS_RUMBA_S6) || defined(CONFIG_SAMSUNG_OIS_MCU_STM32)
	if (g_o_ctrl != NULL) {
		cam_ois_set_ois_mode(g_o_ctrl, 0x05); // OIS
		msleep(20);
		cam_ois_set_ois_mode(g_o_ctrl, ois_mode); // OIS
		CAM_INFO(CAM_APERTURE, "restore OIS mode %d", ois_mode);
		mutex_unlock(&(g_o_ctrl->ois_mode_mutex));
	}
#endif

	usleep_range(10000, 11000);

	if (rc < 0)
		CAM_ERR(CAM_APERTURE, "i2c fail occurred.");

	CAM_INFO(CAM_APERTURE, "X");

	return rc;
}
int cam_aperture_high(struct camera_io_master *client)
{
	int rc = 0;
	int pos = 0;
	u8 data[2] = {0, };
	uint32_t size;
	struct cam_sensor_i2c_reg_array reg_setting;
#if defined(CONFIG_SAMSUNG_OIS_RUMBA_S4) || defined(CONFIG_SAMSUNG_OIS_RUMBA_S6) || defined(CONFIG_SAMSUNG_OIS_MCU_STM32)
	uint16_t ois_mode = 0;
#endif

	CAM_INFO(CAM_APERTURE, "E");

#if defined(CONFIG_SAMSUNG_OIS_RUMBA_S4) || defined(CONFIG_SAMSUNG_OIS_RUMBA_S6) || defined(CONFIG_SAMSUNG_OIS_MCU_STM32)
	if (g_o_ctrl != NULL) {
		mutex_lock(&(g_o_ctrl->ois_mode_mutex));
		rc = cam_ois_get_ois_mode(g_o_ctrl, &ois_mode);
		if (rc >= 0) {
			cam_ois_set_ois_mode(g_o_ctrl, 0x05); // OIS centering
			usleep_range(10000, 11000);
			CAM_INFO(CAM_APERTURE, "OIS centering, previous OIS mode %d", ois_mode);
		}

		// OIS CMD(Fixed Aperture)
		//rc |= cam_ois_fixed_aperture(g_o_ctrl);
		//usleep_range(15000, 15050);
	}
#endif

	size = 1;

	reg_setting.data_mask = 1;
	reg_setting.delay = 0;

	reg_setting.reg_addr = 0xA6;
	reg_setting.reg_data = 0x7B;

	client->client->addr = 0x98;

	rc = cam_qup_i2c_write(client, &reg_setting, CAMERA_SENSOR_I2C_TYPE_BYTE, CAMERA_SENSOR_I2C_TYPE_BYTE);

	pos = 0;
	data[0] = pos >> 2 & 0xFF;
	data[1] = pos << 6 & 0xC0;

	reg_setting.reg_addr = 0x00;
	reg_setting.reg_data = data[0];

	rc |= cam_qup_i2c_write(client, &reg_setting, CAMERA_SENSOR_I2C_TYPE_BYTE, CAMERA_SENSOR_I2C_TYPE_BYTE);

	reg_setting.reg_addr = 0x01;
	reg_setting.reg_data = data[1];

	rc |= cam_qup_i2c_write(client, &reg_setting, CAMERA_SENSOR_I2C_TYPE_BYTE, CAMERA_SENSOR_I2C_TYPE_BYTE);

	usleep_range(10000, 11000);

	reg_setting.reg_addr = 0xA6;
	reg_setting.reg_data = 0x00;

	rc |= cam_qup_i2c_write(client, &reg_setting, CAMERA_SENSOR_I2C_TYPE_BYTE, CAMERA_SENSOR_I2C_TYPE_BYTE);

	pos = 511;
	data[0] = pos >> 2 & 0xFF;
	data[1] = pos << 6 & 0xC0;

	reg_setting.reg_addr = 0x00;
	reg_setting.reg_data = data[0];

	rc |= cam_qup_i2c_write(client, &reg_setting, CAMERA_SENSOR_I2C_TYPE_BYTE, CAMERA_SENSOR_I2C_TYPE_BYTE);

	reg_setting.reg_addr = 0x01;
	reg_setting.reg_data = data[1];

	rc |= cam_qup_i2c_write(client, &reg_setting, CAMERA_SENSOR_I2C_TYPE_BYTE, CAMERA_SENSOR_I2C_TYPE_BYTE);
	usleep_range(15000, 15050);

#if defined(CONFIG_SAMSUNG_OIS_RUMBA_S4) || defined(CONFIG_SAMSUNG_OIS_RUMBA_S6) || defined(CONFIG_SAMSUNG_OIS_MCU_STM32)
	if (g_o_ctrl != NULL) {
		cam_ois_set_ois_mode(g_o_ctrl, 0x05); // OIS
		msleep(20);
		cam_ois_set_ois_mode(g_o_ctrl, ois_mode); // OIS
		CAM_INFO(CAM_APERTURE, "restore OIS mode %d", ois_mode);
		mutex_unlock(&(g_o_ctrl->ois_mode_mutex));
	}
#endif

	usleep_range(10000, 11000);

	if (rc < 0)
		CAM_ERR(CAM_APERTURE, "i2c fail occurred.");

	CAM_INFO(CAM_APERTURE, "X");

	return rc;
}
int32_t cam_aperture_flush_request(struct cam_req_mgr_flush_request *flush_req)
{
	int32_t rc = 0, i;
	uint32_t cancel_req_id_found = 0;
	struct cam_aperture_ctrl_t *a_ctrl = NULL;
	struct i2c_settings_array *i2c_set = NULL;

	if (!flush_req)
		return -EINVAL;

	a_ctrl = (struct cam_aperture_ctrl_t *)
		cam_get_device_priv(flush_req->dev_hdl);
	if (!a_ctrl) {
		CAM_ERR(CAM_APERTURE, "Device data is NULL");
		return -EINVAL;
	}

	for (i = 0; i < MAX_PER_FRAME_ARRAY; i++) {
		i2c_set = &(a_ctrl->i2c_data.per_frame[i]);

		if ((flush_req->type == CAM_REQ_MGR_FLUSH_TYPE_CANCEL_REQ)
				&& (i2c_set->request_id != flush_req->req_id))
			continue;

		if (i2c_set->is_settings_valid == 1) {
			rc = delete_request(i2c_set);
			if (rc < 0)
				CAM_ERR(CAM_APERTURE,
					"delete request: %lld rc: %d",
					i2c_set->request_id, rc);

			if (flush_req->type ==
				CAM_REQ_MGR_FLUSH_TYPE_CANCEL_REQ) {
				cancel_req_id_found = 1;
				break;
			}
		}
	}

	if (flush_req->type == CAM_REQ_MGR_FLUSH_TYPE_CANCEL_REQ &&
		!cancel_req_id_found)
		CAM_DBG(CAM_APERTURE,
			"Flush request id:%lld not found in the pending list",
			flush_req->req_id);
	return rc;
}

#if defined(CONFIG_SAMSUNG_OIS_RUMBA_S4) || defined(CONFIG_SAMSUNG_OIS_RUMBA_S6) || defined(CONFIG_SAMSUNG_OIS_MCU_STM32)
int cam_aperture_i2c_byte_write(struct cam_aperture_ctrl_t *a_ctrl, uint32_t addr, uint16_t data)
{
	int rc = 0;
	struct cam_sensor_i2c_reg_array reg_setting;

	reg_setting.reg_addr = addr;
	reg_setting.reg_data = data;

	rc = cam_qup_i2c_write(&a_ctrl->io_master_info, &reg_setting,
		CAMERA_SENSOR_I2C_TYPE_BYTE, CAMERA_SENSOR_I2C_TYPE_BYTE);

	if (rc < 0) {
		CAM_DBG(CAM_APERTURE, "aperture i2c byte write failed addr : 0x%x data : 0x%x", addr, data);
		return rc;
	}

	CAM_DBG(CAM_APERTURE, "addr = 0x%x data: 0x%x", addr, data);
	return rc;
}

int cam_aperture_init_for_ois_test(struct cam_aperture_ctrl_t *a_ctrl)
{
	int rc = 0;

	if (a_ctrl == NULL) {
		CAM_ERR(CAM_APERTURE, "failed. a_ctrl is NULL");
		return -EINVAL;
	}

	if (a_ctrl->io_master_info.client == NULL) {
		CAM_ERR(CAM_APERTURE, "failed. i2c client is NULL");
		return -EINVAL;
	}

	a_ctrl->io_master_info.client->addr = 0x98;

	cam_aperture_i2c_byte_write(a_ctrl, 0xAE, 0x3B);
	cam_aperture_i2c_byte_write(a_ctrl, 0x00, 0x7F);
	cam_aperture_i2c_byte_write(a_ctrl, 0x01, 0xC0);
	cam_aperture_i2c_byte_write(a_ctrl, 0x02, 0x00);
	cam_aperture_i2c_byte_write(a_ctrl, 0xA6, 0x7B);
	cam_aperture_i2c_byte_write(a_ctrl, 0x00, 0xFF);
	cam_aperture_i2c_byte_write(a_ctrl, 0x01, 0xC0);
	usleep_range(10000, 10050); // > 10ms
	cam_aperture_i2c_byte_write(a_ctrl, 0xA6, 0x00);
	cam_aperture_i2c_byte_write(a_ctrl, 0x00, 0x7F);
	cam_aperture_i2c_byte_write(a_ctrl, 0x01, 0xC0);
	usleep_range(15000, 15050); // > 15ms

	return rc;
}
#endif
