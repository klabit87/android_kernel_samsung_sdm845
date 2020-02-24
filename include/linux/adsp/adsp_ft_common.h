/*
 *  Copyright (C) 2012, Samsung Electronics Co. Ltd. All Rights Reserved.
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 */
#ifndef __ADSP_FT_COMMON_H__
#define __ADSP_FT_COMMON_H__

#define PID 20000
#define NETLINK_ADSP_FAC 23
#define MAX_REG_NUM 128

/* max size of each sensor's msg_buf */
#define MSG_TYPE_SIZE_ZERO	0
#define MSG_ACCEL_MAX	16
#define MSG_GYRO_MAX	16
#define MSG_MAG_MAX	10
#define MSG_LIGHT_MAX	6
#define MSG_PROX_MAX	12
#define MSG_MOBEAM_MAX	1
#define MSG_GYRO_TEMP_MAX	3
#define MSG_PRESSURE_TEMP_MAX	1

enum {
	MSG_ACCEL,
	MSG_GYRO,
	MSG_MAG,
	MSG_PRESSURE,// MSG_TYPE_SIZE_ZERO
	MSG_LIGHT,
	MSG_PROX,
	MSG_MOBEAM,
	MSG_GYRO_TEMP,
	MSG_PRESSURE_TEMP,
	MSG_MAG_CAL,//MSG_TYPE_SIZE_ZERO
	MSG_HH_HOLE,//MSG_TYPE_SIZE_ZERO
	MSG_FACTORY_INIT_CMD,//MSG_TYPE_SIZE_ZERO
	MSG_SSC_CORE,//MSG_TYPE_SIZE_ZERO
	MSG_SENSOR_MAX
};

/* Netlink ENUMS Message Protocols */
enum {
	MSG_TYPE_GET_RAW_DATA,
	MSG_TYPE_ST_SHOW_DATA,
	MSG_TYPE_SET_ACCEL_LPF,
	MSG_TYPE_SET_ACCEL_MOTOR,
	MSG_TYPE_GET_THRESHOLD,
	MSG_TYPE_SET_THRESHOLD,
	MSG_TYPE_SET_HIDDEN_HOLE_DATA,
	MSG_TYPE_GET_REGISTER,
	MSG_TYPE_SET_REGISTER,
	MSG_TYPE_GET_DUMP_REGISTER,
	MSG_TYPE_GET_CAL_DATA,
	MSG_TYPE_SET_CAL_DATA,
	MSG_TYPE_GET_DHR_INFO,
	MSG_TYPE_FACTORY_ENABLE,
	MSG_TYPE_FACTORY_DISABLE,
	MSG_TYPE_DUMPSTATE,
	MSG_TYPE_MAX
};

#endif
