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
#include "cam_aperture_thread.h"
#include "cam_aperture_soc.h"
#include "cam_aperture_core.h"
#include "cam_sensor_util.h"
#include "cam_debug_util.h"

/**
 * cam_aperture_thread_add_msg - add msg to list
 * @a_ctrl:     ctrl structure
 * @msg:       Camera control command argument
 *
 * Returns success or failure
 */
int cam_aperture_thread_add_msg(
	struct cam_aperture_ctrl_t *a_ctrl,
	struct cam_aperture_thread_msg_t *msg)
{
	if (!a_ctrl) {
		CAM_ERR(CAM_APERTURE, "Invalid Args");
		return -EINVAL;
	}

	if (!a_ctrl->is_thread_started) {
		CAM_ERR(CAM_APERTURE, "Thread is not started");
		return -EINVAL;
	}

	mutex_lock(&(a_ctrl->aperture_thread_mutex));
	list_add_tail(&(msg->list),
		&(a_ctrl->list_head_thread.list));
	mutex_unlock(&(a_ctrl->aperture_thread_mutex));
	wake_up(&(a_ctrl->wait));

	return 0;
}

/**
 * cam_aperture_thread_func - create thread
 * @data:	  ctrl structure
 *
 * Returns success or failure
 */
static int cam_aperture_thread_func(void *data)
{
	int rc = 0;
	struct cam_aperture_ctrl_t *a_ctrl = NULL;
	struct cam_aperture_thread_msg_t *msg = NULL;

	CAM_INFO(CAM_APERTURE, "E");

	if (!data) {
		CAM_ERR(CAM_APERTURE, "Invalid Args");
		return -EINVAL;
	}

	a_ctrl = (struct cam_aperture_ctrl_t *)data;
	a_ctrl->is_thread_started = true;

	while (true) {
		wait_event_freezable(
				a_ctrl->wait,
				(!list_empty(&(a_ctrl->list_head_thread.list)))
				|| kthread_should_stop());

		if (!a_ctrl->is_thread_started) {
			CAM_INFO(CAM_APERTURE, "Thread is stopped");
			break;
		}

		mutex_lock(&(a_ctrl->aperture_thread_mutex));
		msg = list_first_entry_or_null(
				&a_ctrl->list_head_thread.list,
				struct cam_aperture_thread_msg_t, list);
		if (msg != NULL) {
			list_del(&(msg->list));
			mutex_unlock(&(a_ctrl->aperture_thread_mutex));
			if ((msg->msg_type >= 0) &&
				(msg->msg_type < CAM_APERTURE_THREAD_MSG_MAX)) {
				switch (msg->msg_type) {
				case CAM_APERTURE_THREAD_MSG_INIT:
					CAM_INFO(CAM_APERTURE, "CAM_APERTURE_THREAD_MSG_INIT");
					cam_aperture_init_fast(a_ctrl);
					break;
				}
			}
			kfree(msg);
			msg = NULL;
		} else {
			mutex_unlock(&(a_ctrl->aperture_thread_mutex));
		}
	}

	CAM_INFO(CAM_APERTURE, "X");

	return rc;
}

/**
 * cam_aperture_thread_create - create thread
 * @a_ctrl:  ctrl structure
 *
 * Returns success or failure
 */
int cam_aperture_thread_create(struct cam_aperture_ctrl_t *a_ctrl)
{
	int retries = 10;

	CAM_INFO(CAM_APERTURE, "E");

	if (!a_ctrl) {
		CAM_ERR(CAM_APERTURE, "Invalid Args");
		return -EINVAL;
	}

	if (a_ctrl->is_thread_started) {
		CAM_ERR(CAM_APERTURE, "Already started");
		return -EBUSY;
	}

	INIT_LIST_HEAD(&a_ctrl->list_head_thread.list);
	mutex_init(&(a_ctrl->aperture_thread_mutex));
	a_ctrl->is_thread_started = false;
	a_ctrl->aperture_thread = kthread_run(cam_aperture_thread_func, (void *)a_ctrl, "CAM_APERTURE");
	if (IS_ERR(a_ctrl->aperture_thread))
		return -EINVAL;

	while (a_ctrl->is_thread_started == FALSE) {
		usleep_range(2000, 3000);
		if (retries < 0) {
			CAM_ERR(CAM_APERTURE, "Fail to start thread");
			break;
		}
		retries--;
	}

	CAM_INFO(CAM_APERTURE, "X");
	return 0;
}

/**
 * cam_aperture_thread_destroy - destroy thread
 * @a_ctrl:  ctrl structure
 *
 * Returns success or failure
 */
int cam_aperture_thread_destroy(struct cam_aperture_ctrl_t *a_ctrl)
{
	struct cam_aperture_thread_msg_t *msg_list = NULL, *msg_next = NULL;

	CAM_INFO(CAM_APERTURE, "E");

	if (!a_ctrl) {
		CAM_ERR(CAM_APERTURE, "Invalid Args");
		return -EINVAL;
	}

	if (!a_ctrl->is_thread_started) {
		CAM_ERR(CAM_APERTURE, "Thread is not started");
		return 0;
	}

	a_ctrl->is_thread_started = false;
	if (a_ctrl->aperture_thread) {
		mutex_lock(&(a_ctrl->aperture_thread_mutex));
		list_for_each_entry_safe(msg_list, msg_next,
			&a_ctrl->list_head_thread.list, list) {
			list_del(&(msg_list->list));
			kfree(msg_list);
		}
		mutex_unlock(&(a_ctrl->aperture_thread_mutex));

		kthread_stop(a_ctrl->aperture_thread);
		wake_up(&a_ctrl->wait);
		a_ctrl->aperture_thread = NULL;
	}

	CAM_INFO(CAM_APERTURE, "X");

	return 0;
}
