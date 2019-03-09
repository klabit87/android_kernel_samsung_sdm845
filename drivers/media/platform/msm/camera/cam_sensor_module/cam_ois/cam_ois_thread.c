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
#include <linux/firmware.h>
#include <cam_sensor_cmn_header.h>
#include "cam_ois_thread.h"
#include "cam_ois_soc.h"
#include "cam_ois_core.h"
#include "cam_sensor_util.h"
#include "cam_debug_util.h"
#if defined(CONFIG_SAMSUNG_OIS_RUMBA_S4)
#include "cam_ois_rumba_s4.h"
#endif
#if defined(CONFIG_SAMSUNG_OIS_RUMBA_S6)
#include "cam_ois_rumba_s6.h"
#endif
#if defined(CONFIG_SAMSUNG_OIS_MCU_STM32)
#include "cam_ois_mcu_stm32g.h"
#endif

/**
 * cam_ois_thread_add_msg - add msg to list
 * @o_ctrl:     ctrl structure
 * @msg:       Camera control command argument
 *
 * Returns success or failure
 */
int cam_ois_thread_add_msg(
	struct cam_ois_ctrl_t *o_ctrl,
	struct cam_ois_thread_msg_t *msg)
{
	if (!o_ctrl) {
		CAM_ERR(CAM_OIS, "Invalid Args");
		return -EINVAL;
	}

	if (!o_ctrl->is_thread_started) {
		CAM_ERR(CAM_OIS, "Thread is not started");
		return -EINVAL;
	}

	mutex_lock(&(o_ctrl->thread_mutex));
	list_add_tail(&(msg->list),
		&(o_ctrl->list_head_thread.list));
	mutex_unlock(&(o_ctrl->thread_mutex));
	wake_up(&(o_ctrl->wait));

	return 0;
}

/**
 * cam_ois_thread_func - create thread
 * @data:	  ctrl structure
 *
 * Returns success or failure
 */
static int cam_ois_thread_func(void *data)
{
	int rc = 0;
	struct cam_ois_ctrl_t *o_ctrl = NULL;
	struct cam_ois_thread_msg_t *msg = NULL;

	CAM_INFO(CAM_OIS, "E");

	if (!data) {
		CAM_ERR(CAM_OIS, "Invalid Args");
		return -EINVAL;
	}

	o_ctrl = (struct cam_ois_ctrl_t *)data;
	o_ctrl->is_thread_started = true;

	while (true) {
		wait_event_freezable(
				o_ctrl->wait,
				(!list_empty(&(o_ctrl->list_head_thread.list)))
				|| kthread_should_stop());

		if (!o_ctrl->is_thread_started) {
			CAM_INFO(CAM_OIS, "Thread is stopped");
			break;
		}

		mutex_lock(&(o_ctrl->thread_mutex));
		msg = list_first_entry_or_null(
				&o_ctrl->list_head_thread.list,
				struct cam_ois_thread_msg_t, list);
		if (msg != NULL) {
			list_del(&(msg->list));
			mutex_unlock(&(o_ctrl->thread_mutex));
			if ((msg->msg_type >= 0) &&
				(msg->msg_type < CAM_OIS_THREAD_MSG_MAX)) {
				switch (msg->msg_type) {
				case CAM_OIS_THREAD_MSG_START:
					mutex_lock(&(o_ctrl->ois_mode_mutex));
					CAM_DBG(CAM_OIS, "CAM_OIS_THREAD_MSG_START");
					usleep_range(5000, 5050);
					rc = cam_ois_init(o_ctrl);
					if (rc < 0)
						CAM_ERR(CAM_OIS, "OIS init failed %d", rc);

					// OIS centering
					cam_ois_set_ois_mode(o_ctrl, 0x05);
					if (rc < 0)
						CAM_ERR(CAM_OIS, "OIS centering failed %d", rc);
					msleep(40);
					mutex_unlock(&(o_ctrl->ois_mode_mutex));
					break;
				case CAM_OIS_THREAD_MSG_APPLY_SETTING:
					mutex_lock(&(o_ctrl->ois_mode_mutex));
					CAM_DBG(CAM_OIS, "CAM_OIS_THREAD_MSG_APPLY_SETTING");

					mutex_lock(&(o_ctrl->i2c_mode_data_mutex));
					rc = cam_ois_apply_settings(o_ctrl, msg->i2c_reg_settings);
					if (rc < 0)
						CAM_ERR(CAM_OIS, "Cannot apply settings");
					if (msg->i2c_reg_settings->is_settings_valid == 1) {
						rc = delete_request(msg->i2c_reg_settings);
						if (rc < 0)
							CAM_ERR(CAM_OIS,
								"delete request: %lld rc: %d",
								msg->i2c_reg_settings->request_id, rc);
					}
					mutex_unlock(&(o_ctrl->i2c_mode_data_mutex));
					mutex_unlock(&(o_ctrl->ois_mode_mutex));
					break;
				}
			}
			kfree(msg);
			msg = NULL;
		} else {
			mutex_unlock(&(o_ctrl->thread_mutex));
		}
	}

	CAM_INFO(CAM_OIS, "X");

	return rc;
}

/**
 * cam_ois_thread_create - create thread
 * @o_ctrl:  ctrl structure
 *
 * Returns success or failure
 */
int cam_ois_thread_create(struct cam_ois_ctrl_t *o_ctrl)
{
	int retries = 10;

	CAM_INFO(CAM_OIS, "E");

	if (!o_ctrl) {
		CAM_ERR(CAM_OIS, "Invalid Args");
		return -EINVAL;
	}

	if (o_ctrl->is_thread_started) {
		CAM_ERR(CAM_OIS, "Already started");
		return -EBUSY;
	}

	INIT_LIST_HEAD(&o_ctrl->list_head_thread.list);
	mutex_init(&(o_ctrl->thread_mutex));
	o_ctrl->is_thread_started = false;
	o_ctrl->ois_thread = kthread_run(cam_ois_thread_func, (void *)o_ctrl, "CAM_OIS");
	if (IS_ERR(o_ctrl->ois_thread))
		return -EINVAL;

	while (o_ctrl->is_thread_started == FALSE) {
		usleep_range(2000, 3000);
		if (retries < 0) {
			CAM_ERR(CAM_OIS, "Fail to start thread");
			break;
		}
		retries--;
	}

	CAM_INFO(CAM_OIS, "X");
	return 0;
}

/**
 * cam_ois_thread_destroy - destroy thread
 * @o_ctrl:  ctrl structure
 *
 * Returns success or failure
 */
int cam_ois_thread_destroy(struct cam_ois_ctrl_t *o_ctrl)
{
	struct cam_ois_thread_msg_t *msg_list = NULL, *msg_next = NULL;

	CAM_INFO(CAM_OIS, "E");

	if (!o_ctrl) {
		CAM_ERR(CAM_OIS, "Invalid Args");
		return -EINVAL;
	}

	if (!o_ctrl->is_thread_started) {
		CAM_ERR(CAM_OIS, "Thread is not started");
		return 0;
	}

	o_ctrl->is_thread_started = false;
	if (o_ctrl->ois_thread) {
		mutex_lock(&(o_ctrl->thread_mutex));
		list_for_each_entry_safe(msg_list, msg_next,
			&o_ctrl->list_head_thread.list, list) {
			list_del(&(msg_list->list));
			kfree(msg_list);
		}
		mutex_unlock(&(o_ctrl->thread_mutex));

		kthread_stop(o_ctrl->ois_thread);
		wake_up(&o_ctrl->wait);
		o_ctrl->ois_thread = NULL;
	}

	CAM_INFO(CAM_OIS, "X");

	return 0;
}
