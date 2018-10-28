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

#ifndef _CAM_APERTURE_CORE_H_
#define _CAM_APERTURE_CORE_H_

#include "cam_aperture_dev.h"


int32_t cam_aperture_power_down(struct cam_aperture_ctrl_t *a_ctrl);

int32_t cam_aperture_power_up(struct cam_aperture_ctrl_t *a_ctrl);
int cam_aperture_low(struct camera_io_master *client);
int cam_aperture_high(struct camera_io_master *client);
int cam_aperture_init(struct camera_io_master *client);
int cam_aperture_init_fast(struct cam_aperture_ctrl_t *a_ctrl);

/**
 * @power_info: power setting info to control the power
 *
 * This API construct the default aperture power setting.
 *
 * @return Status of operation. Negative in case of error. Zero otherwise.
 */
int32_t cam_aperture_construct_default_power_setting(
	struct cam_sensor_power_ctrl_t *power_info);

/**
 * @apply: Req mgr structure for applying request
 *
 * This API applies the request that is mentioned
 */
int32_t cam_aperture_apply_request(struct cam_req_mgr_apply_request *apply);

/**
 * @info: Sub device info to req mgr
 *
 * This API publish the subdevice info to req mgr
 */
int32_t cam_aperture_publish_dev_info(struct cam_req_mgr_device_info *info);

/**
 * @flush: Req mgr structure for flushing request
 *
 * This API flushes the request that is mentioned
 */
int cam_aperture_flush_request(struct cam_req_mgr_flush_request *flush);


/**
 * @link: Link setup info
 *
 * This API establishes link aperture subdevice with req mgr
 */
int32_t cam_aperture_establish_link(
	struct cam_req_mgr_core_dev_link_setup *link);


/**
 * @a_ctrl: Aperture ctrl structure
 * @arg:    Camera control command argument
 *
 * This API handles the camera control argument reached to aperture
 */
int32_t cam_aperture_driver_cmd(struct cam_aperture_ctrl_t *a_ctrl, void *arg);

#if defined(CONFIG_SAMSUNG_OIS_RUMBA_S4) || defined(CONFIG_SAMSUNG_OIS_RUMBA_S6) || defined(CONFIG_SAMSUNG_OIS_MCU_STM32)
int cam_aperture_init_for_ois_test(struct cam_aperture_ctrl_t *a_ctrl);
#endif
int32_t cam_aperture_power_up(struct cam_aperture_ctrl_t *a_ctrl);
int32_t cam_aperture_power_down(struct cam_aperture_ctrl_t *a_ctrl);
/**
 * @a_ctrl: Aperture ctrl structure
 *
 * This API handles the shutdown ioctl/close
 */
void cam_aperture_shutdown(struct cam_aperture_ctrl_t *a_ctrl);

#endif /* _CAM_APERTURE_CORE_H_ */
