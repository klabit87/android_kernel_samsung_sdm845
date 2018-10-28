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
#include <linux/platform_device.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include "cam_ois_core.h"
#include "cam_eeprom_dev.h"
#include "cam_actuator_core.h"
#include "cam_aperture_core.h"
#if defined(CONFIG_SAMSUNG_OIS_RUMBA_S4)
#include "cam_ois_rumba_s4.h"
#endif
#if defined(CONFIG_SAMSUNG_OIS_RUMBA_S6)
#include "cam_ois_rumba_s6.h"
#endif
#if defined(CONFIG_CAMERA_DYNAMIC_MIPI)
#include "cam_sensor_mipi.h"
#endif
#if defined(CONFIG_SAMSUNG_OIS_MCU_STM32)
#include "cam_ois_mcu_stm32g.h"
#endif

#if defined(CONFIG_USE_CAMERA_HW_BIG_DATA)
#include "cam_sensor_cmn_header.h"
#include "cam_debug_util.h"
#endif

#if defined(CONFIG_CAMERA_SSM_I2C_ENV)
extern void cam_sensor_ssm_i2c_read(uint32_t addr, uint32_t *data,
	enum camera_sensor_i2c_type addr_type,
	enum camera_sensor_i2c_type data_type);
#endif

extern struct kset *devices_kset;
struct class *camera_class;

#define SYSFS_FW_VER_SIZE       40
#define SYSFS_MODULE_INFO_SIZE  96
/* #define FORCE_CAL_LOAD */
#define SYSFS_MAX_READ_SIZE     4096

#if defined(CONFIG_CAMERA_SSM_I2C_ENV)
static ssize_t back_camera_ssm_frame_id_show(struct device *dev,
					 struct device_attribute *attr, char *buf)
{
	uint32_t read_data = -1;

	cam_sensor_ssm_i2c_read(0x304C, &read_data, CAMERA_SENSOR_I2C_TYPE_WORD, CAMERA_SENSOR_I2C_TYPE_BYTE);

	return sprintf(buf, "%x\n", read_data);
}

static ssize_t back_camera_ssm_frame_id_store(struct device *dev,
					  struct device_attribute *attr, const char *buf, size_t size)
{
	int value = -1;

	if (buf == NULL || kstrtouint(buf, 10, &value))
		return -1;

	return size;
}

static ssize_t back_camera_ssm_flicker_max_r_show(struct device *dev,
					 struct device_attribute *attr, char *buf)
{
	uint32_t read_data[2] = {-1, };
	uint32_t flicker_max_r = 0;

	cam_sensor_ssm_i2c_read(0x6318, &read_data[0], CAMERA_SENSOR_I2C_TYPE_WORD, CAMERA_SENSOR_I2C_TYPE_BYTE);  // Fliker MAX R
	cam_sensor_ssm_i2c_read(0x6319, &read_data[1], CAMERA_SENSOR_I2C_TYPE_WORD, CAMERA_SENSOR_I2C_TYPE_BYTE);  // Fliker MAX R

	flicker_max_r = read_data[0]*256 + read_data[1];

	return sprintf(buf, "%d\n", flicker_max_r);
}

static ssize_t back_camera_ssm_flicker_max_r_store(struct device *dev,
					  struct device_attribute *attr, const char *buf, size_t size)
{
	int value = -1;

	if (buf == NULL || kstrtouint(buf, 10, &value))
		return -1;

	return size;
}

static ssize_t back_camera_ssm_flicker_max_g_show(struct device *dev,
					 struct device_attribute *attr, char *buf)
{
	uint32_t read_data[2] = {-1, };
	uint32_t flicker_max_g = 0;

	cam_sensor_ssm_i2c_read(0x631A, &read_data[0], CAMERA_SENSOR_I2C_TYPE_WORD, CAMERA_SENSOR_I2C_TYPE_BYTE);  // Fliker MAX G
	cam_sensor_ssm_i2c_read(0x631B, &read_data[1], CAMERA_SENSOR_I2C_TYPE_WORD, CAMERA_SENSOR_I2C_TYPE_BYTE);  // Fliker MAX G

	flicker_max_g = read_data[0]*256 + read_data[1];

	return sprintf(buf, "%d\n", flicker_max_g);
}

static ssize_t back_camera_ssm_flicker_max_g_store(struct device *dev,
					  struct device_attribute *attr, const char *buf, size_t size)
{
	int value = -1;

	if (buf == NULL || kstrtouint(buf, 10, &value))
		return -1;

	return size;
}

static ssize_t back_camera_ssm_flicker_max_b_show(struct device *dev,
					 struct device_attribute *attr, char *buf)
{
	uint32_t read_data[2] = {-1, };
	uint32_t flicker_max_b = 0;

	cam_sensor_ssm_i2c_read(0x631C, &read_data[0], CAMERA_SENSOR_I2C_TYPE_WORD, CAMERA_SENSOR_I2C_TYPE_BYTE);  // Fliker MAX B
	cam_sensor_ssm_i2c_read(0x631D, &read_data[1], CAMERA_SENSOR_I2C_TYPE_WORD, CAMERA_SENSOR_I2C_TYPE_BYTE);  // Fliker MAX B

	flicker_max_b = read_data[0]*256 + read_data[1];

	return sprintf(buf, "%d\n", flicker_max_b);
}

static ssize_t back_camera_ssm_flicker_max_b_store(struct device *dev,
					  struct device_attribute *attr, const char *buf, size_t size)
{
	int value = -1;

	if (buf == NULL || kstrtouint(buf, 10, &value))
		return -1;

	return size;
}

static ssize_t back_camera_ssm_flicker_coeff_show(struct device *dev,
					 struct device_attribute *attr, char *buf)
{
	uint32_t read_data[2] = {-1, };
	uint32_t flicker_coeff = 0;

	cam_sensor_ssm_i2c_read(0x6278, &read_data[0], CAMERA_SENSOR_I2C_TYPE_WORD, CAMERA_SENSOR_I2C_TYPE_BYTE);  // Fliker MAX B
	cam_sensor_ssm_i2c_read(0x6279, &read_data[1], CAMERA_SENSOR_I2C_TYPE_WORD, CAMERA_SENSOR_I2C_TYPE_BYTE);  // Fliker MAX B

	flicker_coeff = read_data[0]*256 + read_data[1];

	return sprintf(buf, "%d\n", flicker_coeff);

}

static ssize_t back_camera_ssm_flicker_coeff_store(struct device *dev,
					  struct device_attribute *attr, const char *buf, size_t size)
{
	int value = -1;

	if (buf == NULL || kstrtouint(buf, 10, &value))
		return -1;

	return size;
}

static ssize_t back_camera_ssm_diff_max_g_show(struct device *dev,
					 struct device_attribute *attr, char *buf)
{
	uint32_t read_data[2] = {-1, };
	uint32_t diff_max_g = 0;

	cam_sensor_ssm_i2c_read(0x633E, &read_data[0], CAMERA_SENSOR_I2C_TYPE_WORD, CAMERA_SENSOR_I2C_TYPE_BYTE);  // Fliker MAX G
	cam_sensor_ssm_i2c_read(0x633F, &read_data[1], CAMERA_SENSOR_I2C_TYPE_WORD, CAMERA_SENSOR_I2C_TYPE_BYTE);  // Fliker MAX G

	diff_max_g = read_data[0]*256 + read_data[1];

	return sprintf(buf, "%d\n", diff_max_g);
}

static ssize_t back_camera_ssm_diff_max_g_store(struct device *dev,
					  struct device_attribute *attr, const char *buf, size_t size)
{
	int value = -1;

	if (buf == NULL || kstrtouint(buf, 10, &value))
		return -1;

	return size;
}
#endif

char cam_fw_ver[SYSFS_FW_VER_SIZE] = "NULL NULL\n";//multi module
static ssize_t back_camera_firmware_show(struct device *dev,
			struct device_attribute *attr, char *buf)
{
	pr_info("[FW_DBG] cam_fw_ver : %s\n", cam_fw_ver);
	return snprintf(buf, sizeof(cam_fw_ver), "%s", cam_fw_ver);
}

static ssize_t back_camera_firmware_store(struct device *dev,
		struct device_attribute *attr, const char *buf, size_t size)
{
	pr_info("[FW_DBG] buf : %s\n", buf);
	snprintf(cam_fw_ver, sizeof(cam_fw_ver), "%s", buf);

	return size;
}

static ssize_t back_camera_type_show(struct device *dev,
			struct device_attribute *attr, char *buf)
{
	char cam_type_lsi[] = "SLSI_S5K2L2SA\n";
	char cam_type_sony[] = "SONY_IMX345\n";

	if (cam_fw_ver[4] == 'L')
		return snprintf(buf, sizeof(cam_type_lsi), "%s", cam_type_lsi);
	else
		return snprintf(buf, sizeof(cam_type_sony), "%s", cam_type_sony);
}

static ssize_t front_camera_type_show(struct device *dev,
			struct device_attribute *attr, char *buf)
{
#if defined(CONFIG_SEC_KELLYLTE_PROJECT)
	char cam_type[] = "LSI_S5K5E3YX\n";
#else
	char cam_type[] = "SONY_IMX320\n";
#endif
	return snprintf(buf, sizeof(cam_type), "%s", cam_type);
}

char cam_fw_user_ver[SYSFS_FW_VER_SIZE] = "NULL\n";//multi module
static ssize_t back_camera_firmware_user_show(struct device *dev,
					 struct device_attribute *attr, char *buf)
{
	pr_info("[FW_DBG] cam_fw_ver : %s\n", cam_fw_user_ver);
	return snprintf(buf, sizeof(cam_fw_user_ver), "%s", cam_fw_user_ver);
}

static ssize_t back_camera_firmware_user_store(struct device *dev,
					  struct device_attribute *attr, const char *buf, size_t size)
{
	pr_info("[FW_DBG] buf : %s\n", buf);
	snprintf(cam_fw_user_ver, sizeof(cam_fw_user_ver), "%s", buf);

	return size;
}

char cam_fw_factory_ver[SYSFS_FW_VER_SIZE] = "NULL\n";//multi module
static ssize_t back_camera_firmware_factory_show(struct device *dev,
					 struct device_attribute *attr, char *buf)
{
	pr_info("[FW_DBG] cam_fw_ver : %s\n", cam_fw_factory_ver);
	return snprintf(buf, sizeof(cam_fw_factory_ver), "%s", cam_fw_factory_ver);
}

static ssize_t back_camera_firmware_factory_store(struct device *dev,
					  struct device_attribute *attr, const char *buf, size_t size)
{
	pr_info("[FW_DBG] buf : %s\n", buf);
	snprintf(cam_fw_factory_ver, sizeof(cam_fw_factory_ver), "%s", buf);

	return size;
}

char cam_fw_full_ver[SYSFS_FW_VER_SIZE] = "NULL NULL NULL\n";//multi module
static ssize_t back_camera_firmware_full_show(struct device *dev,
					 struct device_attribute *attr, char *buf)
{
	pr_info("[FW_DBG] cam_fw_ver : %s\n", cam_fw_full_ver);
	return snprintf(buf, sizeof(cam_fw_full_ver), "%s", cam_fw_full_ver);
}

static ssize_t back_camera_firmware_full_store(struct device *dev,
					  struct device_attribute *attr, const char *buf, size_t size)
{
	pr_info("[FW_DBG] buf : %s\n", buf);
	snprintf(cam_fw_full_ver, sizeof(cam_fw_full_ver), "%s", buf);

	return size;
}

char cam_load_fw[SYSFS_FW_VER_SIZE] = "NULL\n";
static ssize_t back_camera_firmware_load_show(struct device *dev,
			struct device_attribute *attr, char *buf)
{
	pr_info("[FW_DBG] cam_load_fw : %s\n", cam_load_fw);
	return snprintf(buf, sizeof(cam_load_fw), "%s", cam_load_fw);
}

static ssize_t back_camera_firmware_load_store(struct device *dev,
		struct device_attribute *attr, const char *buf, size_t size)
{
	pr_info("[FW_DBG] buf : %s\n", buf);
	snprintf(cam_load_fw, sizeof(cam_load_fw), "%s\n", buf);
	return size;
}

char cal_crc[SYSFS_FW_VER_SIZE] = "NULL NULL\n";
static ssize_t back_cal_data_check_show(struct device *dev,
			struct device_attribute *attr, char *buf)
{
	pr_info("[FW_DBG] cal_crc : %s\n", cal_crc);
	return snprintf(buf, sizeof(cal_crc), "%s", cal_crc);
}

static ssize_t back_cal_data_check_store(struct device *dev,
		struct device_attribute *attr, const char *buf, size_t size)
{
	pr_info("[FW_DBG] buf : %s\n", buf);
	snprintf(cal_crc, sizeof(cal_crc), "%s", buf);

	return size;
}

char module_info[SYSFS_MODULE_INFO_SIZE] = "NULL\n";
static ssize_t back_module_info_show(struct device *dev,
			struct device_attribute *attr, char *buf)
{
	pr_info("[FW_DBG] module_info : %s\n", module_info);
	return snprintf(buf, sizeof(module_info), "%s", module_info);
}

static ssize_t back_module_info_store(struct device *dev,
		struct device_attribute *attr, const char *buf, size_t size)
{
	pr_info("[FW_DBG] buf : %s\n", buf);
	snprintf(module_info, sizeof(module_info), "%s", buf);

	return size;
}

char front_module_info[SYSFS_MODULE_INFO_SIZE] = "NULL\n";
static ssize_t front_module_info_show(struct device *dev,
			struct device_attribute *attr, char *buf)
{
	pr_info("[FW_DBG] front_module_info : %s\n", front_module_info);
	return snprintf(buf, sizeof(front_module_info), "%s", front_module_info);
}

static ssize_t front_module_info_store(struct device *dev,
		struct device_attribute *attr, const char *buf, size_t size)
{
	pr_info("[FW_DBG] buf : %s\n", buf);
	snprintf(front_module_info, sizeof(front_module_info), "%s", buf);

	return size;
}

char isp_core[10];
static ssize_t back_isp_core_check_show(struct device *dev,
			struct device_attribute *attr, char *buf)
{
#if 0// Power binning is used
	char cam_isp_core[] = "0.8V\n";

	return snprintf(buf, sizeof(cam_isp_core), "%s", cam_isp_core);
#else
	pr_info("[FW_DBG] isp_core : %s\n", isp_core);
	return snprintf(buf, sizeof(isp_core), "%s\n", isp_core);
#endif
}

static ssize_t back_isp_core_check_store(struct device *dev,
		struct device_attribute *attr, const char *buf, size_t size)
{
	pr_info("[FW_DBG] buf : %s\n", buf);
	snprintf(isp_core, sizeof(isp_core), "%s", buf);

	return size;
}


#define FROM_REAR_AF_CAL_SIZE	 10
int rear_af_cal[FROM_REAR_AF_CAL_SIZE + 1] = {0,};
static ssize_t back_camera_afcal_show(struct device *dev,
			struct device_attribute *attr, char *buf)
{
#if defined(CONFIG_SAMSUNG_MULTI_CAMERA)
	char tempbuf[10];
	char N[] = "N ";

	strncat(buf, "10 ", strlen("10 "));

#ifdef	FROM_REAR_AF_CAL_D10_ADDR
	sprintf(tempbuf, "%d ", rear_af_cal[1]);
#else
	sprintf(tempbuf, "%s", N);
#endif
	strncat(buf, tempbuf, strlen(tempbuf));

#ifdef	FROM_REAR_AF_CAL_D20_ADDR
	sprintf(tempbuf, "%d ", rear_af_cal[2]);
#else
	sprintf(tempbuf, "%s", N);
#endif
	strncat(buf, tempbuf, strlen(tempbuf));

#ifdef	FROM_REAR_AF_CAL_D30_ADDR
	sprintf(tempbuf, "%d ", rear_af_cal[3]);
#else
	sprintf(tempbuf, "%s", N);
#endif
	strncat(buf, tempbuf, strlen(tempbuf));

#ifdef	FROM_REAR_AF_CAL_D40_ADDR
	sprintf(tempbuf, "%d ", rear_af_cal[4]);
#else
	sprintf(tempbuf, "%s", N);
#endif
	strncat(buf, tempbuf, strlen(tempbuf));

#ifdef	FROM_REAR_AF_CAL_D50_ADDR
	sprintf(tempbuf, "%d ", rear_af_cal[5]);
#else
	sprintf(tempbuf, "%s", N);
#endif
	strncat(buf, tempbuf, strlen(tempbuf));

#ifdef	FROM_REAR_AF_CAL_D60_ADDR
	sprintf(tempbuf, "%d ", rear_af_cal[6]);
#else
	sprintf(tempbuf, "%s", N);
#endif
	strncat(buf, tempbuf, strlen(tempbuf));

#ifdef	FROM_REAR_AF_CAL_D70_ADDR
	sprintf(tempbuf, "%d ", rear_af_cal[7]);
#else
	sprintf(tempbuf, "%s", N);
#endif
	strncat(buf, tempbuf, strlen(tempbuf));

#ifdef	FROM_REAR_AF_CAL_D80_ADDR
	sprintf(tempbuf, "%d ", rear_af_cal[8]);
#else
	sprintf(tempbuf, "%s", N);
#endif
	strncat(buf, tempbuf, strlen(tempbuf));

#ifdef	FROM_REAR_AF_CAL_PAN_ADDR
	sprintf(tempbuf, "%d ", rear_af_cal[9]);
#else
	sprintf(tempbuf, "%s", N);
#endif
	strncat(buf, tempbuf, strlen(tempbuf));

	return strlen(buf);
#else
	return sprintf(buf, "1 %d %d\n", rear_af_cal[0], rear_af_cal[9]);
#endif
}

char rear_paf_cal_data_far[REAR_PAF_CAL_INFO_SIZE] = {0,};
char rear_paf_cal_data_mid[REAR_PAF_CAL_INFO_SIZE] = {0,};

static ssize_t rear_paf_offset_mid_show(struct device *dev,
					 struct device_attribute *attr, char *buf)
{
	int rc = 0;

	pr_info("rear_paf_cal_data : %s\n", rear_paf_cal_data_mid);
	rc = snprintf(buf, sizeof(rear_paf_cal_data_mid), "%s", rear_paf_cal_data_mid);
	if (rc) {
		pr_info("data size %d\n", rc);
		return rc;
	}
	return 0;
}
static ssize_t rear_paf_offset_far_show(struct device *dev,
					 struct device_attribute *attr, char *buf)
{
	int rc = 0;

	pr_info("rear_paf_cal_data : %s\n", rear_paf_cal_data_far);
	rc = snprintf(buf, sizeof(rear_paf_cal_data_far), "%s", rear_paf_cal_data_far);
	if (rc) {
		pr_info("data size %d\n", rc);
		return rc;
	}
	return 0;
}

uint32_t paf_err_data_result = 0xFFFFFFFF;
static ssize_t rear_paf_cal_check_show(struct device *dev,
		struct device_attribute *attr, char *buf)
{
	pr_info("paf_cal_check : %u\n", paf_err_data_result);
	return sprintf(buf, "%08X\n", paf_err_data_result);
}

char rear_f2_paf_cal_data_far[REAR_PAF_CAL_INFO_SIZE] = {0,};
char rear_f2_paf_cal_data_mid[REAR_PAF_CAL_INFO_SIZE] = {0,};
static ssize_t rear_f2_paf_offset_mid_show(struct device *dev,
					 struct device_attribute *attr, char *buf)
{
	int rc = 0;

	pr_info("rear_f2_paf_cal_data : %s\n", rear_f2_paf_cal_data_mid);
	rc = snprintf(buf, sizeof(rear_f2_paf_cal_data_mid), "%s", rear_f2_paf_cal_data_mid);
	if (rc) {
		pr_info("data size %d\n", rc);
		return rc;
	}
	return 0;
}
static ssize_t rear_f2_paf_offset_far_show(struct device *dev,
					 struct device_attribute *attr, char *buf)
{
	int rc = 0;

	pr_info("rear_f2_paf_cal_data : %s\n", rear_f2_paf_cal_data_far);
	rc = snprintf(buf, sizeof(rear_f2_paf_cal_data_far), "%s", rear_f2_paf_cal_data_far);
	if (rc) {
		pr_info("data size %d\n", rc);
		return rc;
	}
	return 0;
}

uint32_t f2_paf_err_data_result = 0xFFFFFFFF;
static ssize_t rear_f2_paf_cal_check_show(struct device *dev,
		struct device_attribute *attr, char *buf)
{
	pr_info("f2_paf_cal_check : %u\n", f2_paf_err_data_result);
	return sprintf(buf, "%08X\n", f2_paf_err_data_result);
}

uint32_t front_af_cal_pan;
uint32_t front_af_cal_macro;
static ssize_t front_camera_afcal_show(struct device *dev,
			struct device_attribute *attr, char *buf)
{
	pr_info("[FW_DBG] front_af_cal_pan: %d, front_af_cal_macro: %d\n", front_af_cal_pan, front_af_cal_macro);
	return sprintf(buf, "1 %d %d\n", front_af_cal_macro, front_af_cal_pan);
}

#if defined(CONFIG_SAMSUNG_FRONT_EEPROM)
char front_cam_fw_ver[SYSFS_FW_VER_SIZE] = "NULL NULL\n";
#else
char front_cam_fw_ver[SYSFS_FW_VER_SIZE] = "IMX320 N\n";
#endif
static ssize_t front_camera_firmware_show(struct device *dev,
			struct device_attribute *attr, char *buf)
{
	pr_info("[FW_DBG] front_cam_fw_ver : %s\n", front_cam_fw_ver);
	return snprintf(buf, sizeof(front_cam_fw_ver), "%s", front_cam_fw_ver);
}

static ssize_t front_camera_firmware_store(struct device *dev,
		struct device_attribute *attr, const char *buf, size_t size)
{
	pr_info("[FW_DBG] buf : %s\n", buf);
	snprintf(front_cam_fw_ver, sizeof(front_cam_fw_ver), "%s", buf);

	return size;
}

#if defined(CONFIG_SAMSUNG_FRONT_EEPROM)
char front_cam_fw_full_ver[SYSFS_FW_VER_SIZE] = "NULL NULL NULL\n";
#else
char front_cam_fw_full_ver[SYSFS_FW_VER_SIZE] = "IMX320 N N\n";
#endif
static ssize_t front_camera_firmware_full_show(struct device *dev,
			struct device_attribute *attr, char *buf)
{
	pr_info("[FW_DBG] front_cam_fw_full_ver : %s\n", front_cam_fw_full_ver);
	return snprintf(buf, sizeof(front_cam_fw_full_ver), "%s", front_cam_fw_full_ver);
}
static ssize_t front_camera_firmware_full_store(struct device *dev,
			struct device_attribute *attr, const char *buf, size_t size)
{
	pr_info("[FW_DBG] buf : %s\n", buf);
	snprintf(front_cam_fw_full_ver, sizeof(front_cam_fw_full_ver), "%s", buf);
	return size;
}

#if defined(CONFIG_SAMSUNG_FRONT_EEPROM)
char front_cam_fw_user_ver[SYSFS_FW_VER_SIZE] = "NULL\n";
#else
char front_cam_fw_user_ver[SYSFS_FW_VER_SIZE] = "OK\n";
#endif
static ssize_t front_camera_firmware_user_show(struct device *dev,
					 struct device_attribute *attr, char *buf)
{
	pr_info("[FW_DBG] cam_fw_ver : %s\n", front_cam_fw_user_ver);
	return snprintf(buf, sizeof(front_cam_fw_user_ver), "%s", front_cam_fw_user_ver);
}

static ssize_t front_camera_firmware_user_store(struct device *dev,
					  struct device_attribute *attr, const char *buf, size_t size)
{
	pr_info("[FW_DBG] buf : %s\n", buf);
	snprintf(front_cam_fw_user_ver, sizeof(front_cam_fw_user_ver), "%s", buf);

	return size;
}

#if defined(CONFIG_SAMSUNG_FRONT_EEPROM)
char front_cam_fw_factory_ver[SYSFS_FW_VER_SIZE] = "NULL\n";
#else
char front_cam_fw_factory_ver[SYSFS_FW_VER_SIZE] = "OK\n";
#endif
static ssize_t front_camera_firmware_factory_show(struct device *dev,
					 struct device_attribute *attr, char *buf)
{
	pr_info("[FW_DBG] cam_fw_ver : %s\n", front_cam_fw_factory_ver);
	return snprintf(buf, sizeof(front_cam_fw_factory_ver), "%s", front_cam_fw_factory_ver);
}

static ssize_t front_camera_firmware_factory_store(struct device *dev,
					  struct device_attribute *attr, const char *buf, size_t size)
{
	pr_info("[FW_DBG] buf : %s\n", buf);
	snprintf(front_cam_fw_factory_ver, sizeof(front_cam_fw_factory_ver), "%s", buf);

	return size;
}

#if defined(CONFIG_CAMERA_SYSFS_V2)
char rear_cam_info[150] = "NULL\n";	//camera_info
static ssize_t rear_camera_info_show(struct device *dev,
					 struct device_attribute *attr, char *buf)
{
	pr_info("[FW_DBG] cam_info : %s\n", rear_cam_info);
	return snprintf(buf, sizeof(rear_cam_info), "%s", rear_cam_info);
}

static ssize_t rear_camera_info_store(struct device *dev,
					  struct device_attribute *attr, const char *buf, size_t size)
{
	pr_info("[FW_DBG] buf : %s\n", buf);
//	snprintf(rear_cam_info, sizeof(rear_cam_info), "%s", buf);

	return size;
}

char front_cam_info[150] = "NULL\n";	//camera_info
static ssize_t front_camera_info_show(struct device *dev,
					 struct device_attribute *attr, char *buf)
{
	pr_info("[FW_DBG] cam_info : %s\n", front_cam_info);
	return snprintf(buf, sizeof(front_cam_info), "%s", front_cam_info);
}

static ssize_t front_camera_info_store(struct device *dev,
					  struct device_attribute *attr, const char *buf, size_t size)
{
	pr_info("[FW_DBG] buf : %s\n", buf);
//	snprintf(front_cam_info, sizeof(front_cam_info), "%s", buf);

	return size;
}
#endif

#if defined(CONFIG_SAMSUNG_SECURE_CAMERA)
char iris_cam_fw_ver[SYSFS_FW_VER_SIZE] = "UNKNOWN N\n";
static ssize_t iris_camera_firmware_show(struct device *dev,
					 struct device_attribute *attr, char *buf)
{
	pr_info("[FW_DBG] iris_cam_fw_ver : %s\n", iris_cam_fw_ver);
	return snprintf(buf, sizeof(iris_cam_fw_ver), "%s", iris_cam_fw_ver);
}

static ssize_t iris_camera_firmware_store(struct device *dev,
					 struct device_attribute *attr, const char *buf, size_t size)
{
	pr_info("[FW_DBG] buf : %s\n", buf);
	snprintf(iris_cam_fw_ver, sizeof(iris_cam_fw_ver), "%s", buf);
	return size;
}

char iris_cam_fw_full_ver[SYSFS_FW_VER_SIZE] = "UNKNOWN N N\n";
static ssize_t iris_camera_firmware_full_show(struct device *dev,
					 struct device_attribute *attr, char *buf)
{
	pr_info("[FW_DBG] iris_cam_fw_full_ver : %s\n", iris_cam_fw_full_ver);
	return snprintf(buf, sizeof(iris_cam_fw_full_ver), "%s", iris_cam_fw_full_ver);
}

static ssize_t iris_camera_firmware_full_store(struct device *dev,
					  struct device_attribute *attr, const char *buf, size_t size)
{
	pr_info("[FW_DBG] buf : %s\n", buf);
	snprintf(iris_cam_fw_full_ver, sizeof(iris_cam_fw_full_ver), "%s", buf);
	return size;
}

char iris_cam_fw_user_ver[SYSFS_FW_VER_SIZE] = "NG\n";
static ssize_t iris_camera_firmware_user_show(struct device *dev,
					 struct device_attribute *attr, char *buf)
{
	pr_info("[FW_DBG] iris_cam_fw_user_ver : %s\n", iris_cam_fw_user_ver);
	return snprintf(buf, sizeof(iris_cam_fw_user_ver), "%s", iris_cam_fw_user_ver);
}

static ssize_t iris_camera_firmware_user_store(struct device *dev,
					  struct device_attribute *attr, const char *buf, size_t size)
{
	pr_info("[FW_DBG] buf : %s\n", buf);
	snprintf(iris_cam_fw_user_ver, sizeof(iris_cam_fw_user_ver), "%s", buf);
	return size;
}

char iris_cam_fw_factory_ver[SYSFS_FW_VER_SIZE] = "NG_VER\n";
static ssize_t iris_camera_firmware_factory_show(struct device *dev,
					 struct device_attribute *attr, char *buf)
{
	pr_info("[FW_DBG] iris_cam_fw_factory_ver : %s\n", iris_cam_fw_factory_ver);
	return snprintf(buf, sizeof(iris_cam_fw_factory_ver), "%s", iris_cam_fw_factory_ver);
}

static ssize_t iris_camera_firmware_factory_store(struct device *dev,
					  struct device_attribute *attr, const char *buf, size_t size)
{
	pr_info("[FW_DBG] buf : %s\n", buf);
	snprintf(iris_cam_fw_factory_ver, sizeof(iris_cam_fw_factory_ver), "%s", buf);
	return size;
}

#if defined(CONFIG_CAMERA_SYSFS_V2)
char iris_cam_info[150] = "NULL\n";	//camera_info
static ssize_t iris_camera_info_show(struct device *dev,
					 struct device_attribute *attr, char *buf)
{
	pr_info("[FW_DBG] iris_cam_info : %s\n", iris_cam_info);
	return snprintf(buf, sizeof(iris_cam_info), "%s", iris_cam_info);
}

static ssize_t iris_camera_info_store(struct device *dev,
					  struct device_attribute *attr, const char *buf, size_t size)
{
	pr_info("[FW_DBG] buf : %s\n", buf);
//	snprintf(front_cam_info, sizeof(front_cam_info), "%s", buf);
	return size;
}
#endif
#endif

#define FROM_SENSOR_ID_SIZE 16
char rear_sensor_id[FROM_SENSOR_ID_SIZE + 1] = "\0";
static ssize_t rear_sensorid_exif_show(struct device *dev,
					 struct device_attribute *attr, char *buf)
{
	void *ret = NULL;

	pr_info("[FW_DBG] rear_sensor_id : %s\n", rear_sensor_id);

	ret = memcpy(buf, rear_sensor_id, sizeof(rear_sensor_id));
	if (ret)
		return sizeof(rear_sensor_id);
	return 0;
}

static ssize_t rear_sensorid_exif_store(struct device *dev,
					  struct device_attribute *attr, const char *buf, size_t size)
{
	pr_info("[FW_DBG] buf : %s\n", buf);
//	snprintf(rear_sensor_id, sizeof(rear_sensor_id), "%s", buf);

	return size;
}

char front_sensor_id[FROM_SENSOR_ID_SIZE + 1] = "\0";
static ssize_t front_sensorid_exif_show(struct device *dev,
					 struct device_attribute *attr, char *buf)
{
	void *ret = NULL;

	pr_info("[FW_DBG] front_sensor_id : %s\n", front_sensor_id);

	ret = memcpy(buf, front_sensor_id, sizeof(front_sensor_id));
	if (ret)
		return sizeof(front_sensor_id);
	return 0;
}

static ssize_t front_sensorid_exif_store(struct device *dev,
					  struct device_attribute *attr, const char *buf, size_t size)
{
	pr_info("[FW_DBG] buf : %s\n", buf);
//	snprintf(front_sensor_id, sizeof(front_sensor_id), "%s", buf);

	return size;
}

#define FROM_MTF_SIZE 54
char front_mtf_exif[FROM_MTF_SIZE + 1] = "\0";
static ssize_t front_mtf_exif_show(struct device *dev,
					 struct device_attribute *attr, char *buf)
{
	void *ret = NULL;

	pr_info("[FW_DBG] front_mtf_exif : %s\n", front_mtf_exif);

	ret = memcpy(buf, front_mtf_exif, sizeof(front_mtf_exif));
	if (ret)
		return sizeof(front_mtf_exif);
	return 0;
}

static ssize_t front_mtf_exif_store(struct device *dev,
					  struct device_attribute *attr, const char *buf, size_t size)
{
	pr_info("[FW_DBG] buf : %s\n", buf);
//	snprintf(front_mtf_exif, sizeof(front_mtf_exif), "%s", buf);

	return size;
}

char rear_mtf_exif[FROM_MTF_SIZE + 1] = "\0";
static ssize_t rear_mtf_exif_show(struct device *dev,
					 struct device_attribute *attr, char *buf)
{
	void *ret = NULL;

	pr_info("[FW_DBG] rear_mtf_exif : %s\n", rear_mtf_exif);

	ret = memcpy(buf, rear_mtf_exif, sizeof(rear_mtf_exif));
	if (ret)
		return sizeof(rear_mtf_exif);
	return 0;
}

static ssize_t rear_mtf_exif_store(struct device *dev,
					  struct device_attribute *attr, const char *buf, size_t size)
{
	pr_info("[FW_DBG] buf : %s\n", buf);
//	snprintf(rear_mtf_exif, sizeof(rear_mtf_exif), "%s", buf);

	return size;
}

char rear_mtf2_exif[FROM_MTF_SIZE + 1] = "\0";
static ssize_t rear_mtf2_exif_show(struct device *dev,
					 struct device_attribute *attr, char *buf)
{
	void *ret = NULL;

	pr_info("[FW_DBG] rear_mtf2_exif : %s\n", rear_mtf2_exif);

	ret = memcpy(buf, rear_mtf2_exif, sizeof(rear_mtf2_exif));
	if (ret)
		return sizeof(rear_mtf2_exif);
	return 0;
}

static ssize_t rear_mtf2_exif_store(struct device *dev,
					  struct device_attribute *attr, const char *buf, size_t size)
{
	pr_info("[FW_DBG] buf : %s\n", buf);
//	snprintf(rear_mtf_exif, sizeof(rear_mtf_exif), "%s", buf);

	return size;
}

uint8_t rear_module_id[FROM_MODULE_ID_SIZE + 1] = "\0";
static ssize_t back_camera_moduleid_show(struct device *dev,
			struct device_attribute *attr, char *buf)
{
	pr_info("[FW_DBG] rear_module_id : %c%c%c%c%c%02X%02X%02X%02X%02X\n",
	  rear_module_id[0], rear_module_id[1], rear_module_id[2], rear_module_id[3], rear_module_id[4],
	  rear_module_id[5], rear_module_id[6], rear_module_id[7], rear_module_id[8], rear_module_id[9]);
	return sprintf(buf, "%c%c%c%c%c%02X%02X%02X%02X%02X\n",
	  rear_module_id[0], rear_module_id[1], rear_module_id[2], rear_module_id[3], rear_module_id[4],
	  rear_module_id[5], rear_module_id[6], rear_module_id[7], rear_module_id[8], rear_module_id[9]);
}

uint8_t front_module_id[FROM_MODULE_ID_SIZE + 1] = "\0";
static ssize_t front_camera_moduleid_show(struct device *dev,
			struct device_attribute *attr, char *buf)
{
	pr_info("[FW_DBG] front_module_id : %c%c%c%c%c%02X%02X%02X%02X%02X\n",
	  front_module_id[0], front_module_id[1], front_module_id[2], front_module_id[3], front_module_id[4],
	  front_module_id[5], front_module_id[6], front_module_id[7], front_module_id[8], front_module_id[9]);
	return sprintf(buf, "%c%c%c%c%c%02X%02X%02X%02X%02X\n",
	  front_module_id[0], front_module_id[1], front_module_id[2], front_module_id[3], front_module_id[4],
	  front_module_id[5], front_module_id[6], front_module_id[7], front_module_id[8], front_module_id[9]);
}

#define SSRM_CAMERA_INFO_SIZE 256
char ssrm_camera_info[SSRM_CAMERA_INFO_SIZE + 1] = "\0";
static ssize_t ssrm_camera_info_show(struct device *dev,
					 struct device_attribute *attr, char *buf)
{
	int rc = 0;

	pr_info("ssrm_camera_info : %s\n", ssrm_camera_info);

	rc = scnprintf(buf, PAGE_SIZE, "%s", ssrm_camera_info);
	if (rc)
		return rc;
	return 0;
}

static ssize_t ssrm_camera_info_store(struct device *dev,
					  struct device_attribute *attr, const char *buf, size_t size)
{
	pr_info("ssrm_camera_info buf : %s\n", buf);
	scnprintf(ssrm_camera_info, sizeof(ssrm_camera_info), "%s", buf);

	return size;
}

#if defined(CONFIG_SAMSUNG_MULTI_CAMERA)
char cam2_fw_ver[SYSFS_FW_VER_SIZE] = "NULL NULL\n";//multi module
static ssize_t back_camera2_firmware_show(struct device *dev,
			struct device_attribute *attr, char *buf)
{
	pr_info("[FW_DBG] cam2_fw_ver : %s\n", cam2_fw_ver);
	return snprintf(buf, sizeof(cam2_fw_ver), "%s", cam2_fw_ver);
}

static ssize_t back_camera2_firmware_store(struct device *dev,
		struct device_attribute *attr, const char *buf, size_t size)
{
	pr_info("[FW_DBG] buf : %s\n", buf);
	snprintf(cam2_fw_ver, sizeof(cam2_fw_ver), "%s", buf);

	return size;
}
char cam2_fw_full_ver[SYSFS_FW_VER_SIZE] = "NULL NULL NULL\n";//multi module
static ssize_t back_camera2_firmware_full_show(struct device *dev,
					 struct device_attribute *attr, char *buf)
{
	pr_info("[FW_DBG] cam2_fw_ver : %s\n", cam2_fw_full_ver);
	return snprintf(buf, sizeof(cam2_fw_full_ver), "%s", cam2_fw_full_ver);
}

static ssize_t back_camera2_firmware_full_store(struct device *dev,
					  struct device_attribute *attr, const char *buf, size_t size)
{
	pr_info("[FW_DBG] buf : %s\n", buf);
	snprintf(cam2_fw_full_ver, sizeof(cam2_fw_full_ver), "%s", buf);

	return size;
}

int rear2_af_cal[FROM_REAR_AF_CAL_SIZE + 1] = {0,};
static ssize_t back_camera2_afcal_show(struct device *dev,
			struct device_attribute *attr, char *buf)
{
	char tempbuf[10];
	char N[] = "N ";

	strncat(buf, "10 ", strlen("10 "));

#ifdef	FROM_REAR2_AF_CAL_D10_ADDR
	sprintf(tempbuf, "%d ", rear2_af_cal[1]);
#else
	sprintf(tempbuf, "%s", N);
#endif
	strncat(buf, tempbuf, strlen(tempbuf));

#ifdef	FROM_REAR2_AF_CAL_D20_ADDR
	sprintf(tempbuf, "%d ", rear2_af_cal[2]);
#else
	sprintf(tempbuf, "%s", N);
#endif
	strncat(buf, tempbuf, strlen(tempbuf));

#ifdef	FROM_REAR2_AF_CAL_D30_ADDR
	sprintf(tempbuf, "%d ", rear2_af_cal[3]);
#else
	sprintf(tempbuf, "%s", N);
#endif
	strncat(buf, tempbuf, strlen(tempbuf));

#ifdef	FROM_REAR2_AF_CAL_D40_ADDR
	sprintf(tempbuf, "%d ", rear2_af_cal[4]);
#else
	sprintf(tempbuf, "%s", N);
#endif
	strncat(buf, tempbuf, strlen(tempbuf));

#ifdef	FROM_REAR2_AF_CAL_D50_ADDR
	sprintf(tempbuf, "%d ", rear2_af_cal[5]);
#else
	sprintf(tempbuf, "%s", N);
#endif
	strncat(buf, tempbuf, strlen(tempbuf));

#ifdef	FROM_REAR2_AF_CAL_D60_ADDR
	sprintf(tempbuf, "%d ", rear2_af_cal[6]);
#else
	sprintf(tempbuf, "%s", N);
#endif
	strncat(buf, tempbuf, strlen(tempbuf));

#ifdef	FROM_REAR2_AF_CAL_D70_ADDR
	sprintf(tempbuf, "%d ", rear2_af_cal[7]);
#else
	sprintf(tempbuf, "%s", N);
#endif
	strncat(buf, tempbuf, strlen(tempbuf));

#ifdef	FROM_REAR2_AF_CAL_D80_ADDR
	sprintf(tempbuf, "%d ", rear2_af_cal[8]);
#else
	sprintf(tempbuf, "%s", N);
#endif
	strncat(buf, tempbuf, strlen(tempbuf));

#ifdef	FROM_REAR2_AF_CAL_PAN_ADDR
	sprintf(tempbuf, "%d ", rear2_af_cal[9]);
#else
	sprintf(tempbuf, "%s", N);
#endif
	strncat(buf, tempbuf, strlen(tempbuf));

	return strlen(buf);
}

char rear2_cam_info[150] = "NULL\n";	//camera_info
static ssize_t rear2_camera_info_show(struct device *dev,
					 struct device_attribute *attr, char *buf)
{
	pr_info("[FW_DBG] cam_info : %s\n", rear2_cam_info);
	return snprintf(buf, sizeof(rear2_cam_info), "%s", rear2_cam_info);
}

static ssize_t rear2_camera_info_store(struct device *dev,
					  struct device_attribute *attr, const char *buf, size_t size)
{
	pr_info("[FW_DBG] buf : %s\n", buf);
//	snprintf(rear2_cam_info, sizeof(rear2_cam_info), "%s", buf);

	return size;
}

static ssize_t back_camera2_type_show(struct device *dev,
			struct device_attribute *attr, char *buf)
{
	char cam_type[] = "SLSI_S5K3M3\n";

	return snprintf(buf, sizeof(cam_type), "%s", cam_type);
}

char rear2_mtf_exif[FROM_MTF_SIZE + 1] = "\0";
static ssize_t rear2_mtf_exif_show(struct device *dev,
					 struct device_attribute *attr, char *buf)
{
	void *ret = NULL;

	pr_info("[FW_DBG] rear2_mtf_exif : %s\n", rear2_mtf_exif);

	ret = memcpy(buf, rear2_mtf_exif, sizeof(rear2_mtf_exif));
	if (ret)
		return sizeof(rear2_mtf_exif);
	return 0;
}

static ssize_t rear2_mtf_exif_store(struct device *dev,
					  struct device_attribute *attr, const char *buf, size_t size)
{
	pr_info("[FW_DBG] buf : %s\n", buf);
//	snprintf(rear2_mtf_exif, sizeof(rear2_mtf_exif), "%s", buf);

	return size;
}

char rear2_sensor_id[FROM_SENSOR_ID_SIZE + 1] = "\0";
static ssize_t rear2_sensorid_exif_show(struct device *dev,
					 struct device_attribute *attr, char *buf)
{
	void *ret = NULL;

	pr_info("[FW_DBG] rear2_sensor_id : %s\n", rear2_sensor_id);

	ret = memcpy(buf, rear2_sensor_id, sizeof(rear2_sensor_id));
	if (ret)
		return sizeof(rear2_sensor_id);
	return 0;
}

static ssize_t rear2_sensorid_exif_store(struct device *dev,
					  struct device_attribute *attr, const char *buf, size_t size)
{
	pr_info("[FW_DBG] buf : %s\n", buf);
//	snprintf(rear2_sensor_id, sizeof(rear2_sensor_id), "%s", buf);

	return size;
}

#define FROM_REAR_DUAL_CAL_SIZE 4112//512
uint8_t rear_dual_cal[FROM_REAR_DUAL_CAL_SIZE + 1] = "\0";
static ssize_t rear_dual_cal_show(struct device *dev,
					 struct device_attribute *attr, char *buf)
{
	void *ret = NULL;
	int copy_size = 0;

	pr_info("[FW_DBG] rear_dual_cal : %s\n", rear_dual_cal);

	if (FROM_REAR_DUAL_CAL_SIZE > SYSFS_MAX_READ_SIZE)
		copy_size = SYSFS_MAX_READ_SIZE;
	else
		copy_size = FROM_REAR_DUAL_CAL_SIZE;

	ret = memcpy(buf, rear_dual_cal, copy_size);

	if (ret)
		return copy_size;

	return 0;

}
static ssize_t rear_dual_cal_extra_show(struct device *dev,
					 struct device_attribute *attr, char *buf)
{
	void *ret = NULL;
	int copy_size = 0;

	pr_info("[FW_DBG] rear_dual_cal_extra : %s\n", rear_dual_cal);

	copy_size = FROM_REAR_DUAL_CAL_SIZE - SYSFS_MAX_READ_SIZE;

	ret = memcpy(buf, &rear_dual_cal[SYSFS_MAX_READ_SIZE], copy_size);

	if (ret)
		return copy_size;

	return 0;

}

uint32_t rear_dual_cal_size = FROM_REAR_DUAL_CAL_SIZE;
static ssize_t rear_dual_cal_size_show(struct device *dev,
					 struct device_attribute *attr, char *buf)
{
	pr_info("[FW_DBG] rear_dual_cal_size : %d\n", rear_dual_cal_size);
	return sprintf(buf, "%d\n", rear_dual_cal_size);
}

int rear2_dual_tilt_x;
int rear2_dual_tilt_y;
int rear2_dual_tilt_z;
int rear2_dual_tilt_sx;
int rear2_dual_tilt_sy;
int rear2_dual_tilt_range;
int rear2_dual_tilt_max_err;
int rear2_dual_tilt_avg_err;
int rear2_dual_tilt_dll_ver;
static ssize_t rear2_tilt_show(struct device *dev,
		struct device_attribute *attr, char *buf)
{
	pr_info("[FW_DBG] rear dual tilt x = %d, y = %d, z = %d, sx = %d, sy = %d, range = %d, max_err = %d, avg_err = %d, dll_ver = %d\n",
		rear2_dual_tilt_x, rear2_dual_tilt_y, rear2_dual_tilt_z, rear2_dual_tilt_sx, rear2_dual_tilt_sy,
		rear2_dual_tilt_range, rear2_dual_tilt_max_err, rear2_dual_tilt_avg_err, rear2_dual_tilt_dll_ver);

	return sprintf(buf, "1 %d %d %d %d %d %d %d %d %d\n", rear2_dual_tilt_x, rear2_dual_tilt_y,
			rear2_dual_tilt_z, rear2_dual_tilt_sx, rear2_dual_tilt_sy, rear2_dual_tilt_range,
			rear2_dual_tilt_max_err, rear2_dual_tilt_avg_err, rear2_dual_tilt_dll_ver);
}
#endif

extern struct cam_ois_ctrl_t *g_o_ctrl;
extern struct cam_actuator_ctrl_t *g_a_ctrls[2];
extern struct cam_aperture_ctrl_t *g_cam_aperture_t;
uint32_t ois_autotest_threshold = 150;
static ssize_t ois_autotest_show(struct device *dev,
					 struct device_attribute *attr, char *buf)
{
	bool ret = false;
	int result = 0;
	bool x1_result = true, y1_result = true;
	int cnt = 0;
	struct cam_ois_sinewave_t sinewave[1];

	if (g_a_ctrls[0] != NULL)
		cam_actuator_power_up(g_a_ctrls[0]);
	msleep(100);

	cam_actuator_move_for_ois_test(g_a_ctrls[0]);

	cam_ois_i2c_byte_write(g_o_ctrl, 0x0002, 0x05); //OIS Centering Mode
	cam_ois_i2c_byte_write(g_o_ctrl, 0x0000, 0x01); //OIS Servo On
	msleep(100);

	cam_aperture_init_for_ois_test(g_cam_aperture_t);

	cam_ois_i2c_byte_write(g_o_ctrl, 0x0000, 0x00); //OIS Servo Off
	msleep(100);

	ret = cam_ois_sine_wavecheck(g_o_ctrl, ois_autotest_threshold, sinewave, &result, 1);
	if (ret)
		x1_result = y1_result = true;
	else {
		if (result & 0x01) {
			// Module#1 X Axis Fail
			x1_result = false;
		}
		if (result & 0x02) {
			// Module#1 Y Axis Fail
			y1_result = false;
		}
	}

	cnt = sprintf(buf, "%s, %d, %s, %d",
		(x1_result ? "pass" : "fail"), (x1_result ? 0 : sinewave[0].sin_x),
		(y1_result ? "pass" : "fail"), (y1_result ? 0 : sinewave[0].sin_y));
	pr_info("%s: result : %s\n", __func__, buf);

	if (g_a_ctrls[0] != NULL)
		cam_actuator_power_down(g_a_ctrls[0]);

	return cnt;
}

static ssize_t ois_autotest_store(struct device *dev,
					  struct device_attribute *attr, const char *buf, size_t size)
{
	uint32_t value = 0;

	if (buf == NULL || kstrtouint(buf, 10, &value))
		return -1;
	ois_autotest_threshold = value;
	return size;
}

#if defined(CONFIG_SAMSUNG_OIS_RUMBA_S6)
static ssize_t ois_autotest_2nd_show(struct device *dev,
					 struct device_attribute *attr, char *buf)
{
	bool ret = false;
	int result = 0;
	bool x1_result = true, y1_result = true, x2_result = true, y2_result = true;
	int cnt = 0, i = 0;
	struct cam_ois_sinewave_t sinewave[2];

	for (i = 0; i < 2; i++) {
		if (g_a_ctrls[i] != NULL) {
			cam_actuator_power_up(g_a_ctrls[i]);
			msleep(5);
			cam_actuator_move_for_ois_test(g_a_ctrls[i]);
		}
	}
	msleep(100);

	cam_ois_i2c_byte_write(g_o_ctrl, 0x0002, 0x05); //OIS Centering Mode
	cam_ois_i2c_byte_write(g_o_ctrl, 0x0000, 0x01); //OIS Servo On
	msleep(100);

	cam_aperture_init_for_ois_test(g_cam_aperture_t);

	cam_ois_i2c_byte_write(g_o_ctrl, 0x0000, 0x00); //OIS Servo Off
	msleep(100);

	ret = cam_ois_sine_wavecheck(g_o_ctrl, ois_autotest_threshold, sinewave, &result, 2); //two at once
	if (ret)
		x1_result = y1_result = x2_result = y2_result = true;
	else {
		if (result & 0x01) {
			// Module#1 X Axis Fail
			x1_result = false;
		}
		if (result & 0x02) {
			// Module#1 Y Axis Fail
			y1_result = false;
		}
		if ((result >> 4) & 0x01) {
			// Module#2 X Axis Fail
			x2_result = false;
		}
		if ((result >> 4) & 0x02) {
			// Module#2 Y Axis Fail
			y2_result = false;
		}
	}
	cnt = sprintf(buf, "%s, %d, %s, %d",
		(x1_result ? "pass" : "fail"), (x1_result ? 0 : sinewave[0].sin_x),
		(y1_result ? "pass" : "fail"), (y1_result ? 0 : sinewave[0].sin_y));
	cnt += sprintf(buf + cnt, ", %s, %d, %s, %d",
		(x2_result ? "pass" : "fail"), (x2_result ? 0 : sinewave[1].sin_x),
		(y2_result ? "pass" : "fail"), (y2_result ? 0 : sinewave[1].sin_y));
	pr_info("%s: result : %s\n", __func__, buf);

	for (i = 0; i < 2; i++) {
		if (g_a_ctrls[i] != NULL)
			cam_actuator_power_down(g_a_ctrls[i]);
	}

	return cnt;
}

static ssize_t ois_autotest_2nd_store(struct device *dev,
					  struct device_attribute *attr, const char *buf, size_t size)
{
	uint32_t value = 0;

	if (buf == NULL || kstrtouint(buf, 10, &value))
		return -1;
	ois_autotest_threshold = value;
	return size;
}
#endif

static ssize_t ois_power_store(struct device *dev, struct device_attribute *attr, const char *buf, size_t size)
{
	if (g_o_ctrl == NULL || g_o_ctrl->io_master_info.client == NULL)
		return size;

	switch (buf[0]) {
	case '0':
		cam_ois_power_down(g_o_ctrl);
		pr_info("%s: power down", __func__);
		break;
	case '1':
		cam_ois_power_up(g_o_ctrl);
		msleep(200);
		pr_info("%s: power up", __func__);
		break;

	default:
		break;
	}
	return size;
}

static ssize_t gyro_selftest_show(struct device *dev, struct device_attribute *attr, char *buf)
{
	int result_total = 0;
	bool result_offset = 0, result_selftest = 0;
	uint32_t selftest_ret = 0;
	long raw_data_x = 0, raw_data_y = 0;

	cam_ois_offset_test(g_o_ctrl, &raw_data_x, &raw_data_y, 1);
	msleep(50);
	selftest_ret = cam_ois_self_test(g_o_ctrl);

	if (selftest_ret == 0x0)
		result_selftest = true;
	else
		result_selftest = false;

	if (abs(raw_data_x) > 30000 || abs(raw_data_y) > 30000)
		result_offset = false;
	else
		result_offset = true;

	if (result_offset && result_selftest)
		result_total = 0;
	else if (!result_offset && !result_selftest)
		result_total = 3;
	else if (!result_offset)
		result_total = 1;
	else if (!result_selftest)
		result_total = 2;

	pr_info("%s: Result : 0 (success), 1 (offset fail), 2 (selftest fail) , 3 (both fail)\n", __func__);
	sprintf(buf, "Result : %d, result x = %ld.%03ld, result y = %ld.%03ld\n",
		result_total, raw_data_x / 1000, (long int)abs(raw_data_x % 1000),
		raw_data_y / 1000, (long int)abs(raw_data_y % 1000));
	pr_info("%s", buf);

	if (raw_data_x < 0 && raw_data_y < 0) {
		return sprintf(buf, "%d,-%ld.%03ld,-%ld.%03ld\n", result_total,
			(long int)abs(raw_data_x / 1000), (long int)abs(raw_data_x % 1000),
			(long int)abs(raw_data_y / 1000), (long int)abs(raw_data_y % 1000));
	} else if (raw_data_x < 0) {
		return sprintf(buf, "%d,-%ld.%03ld,%ld.%03ld\n", result_total,
			(long int)abs(raw_data_x / 1000), (long int)abs(raw_data_x % 1000),
			raw_data_y / 1000, raw_data_y % 1000);
	} else if (raw_data_y < 0) {
		return sprintf(buf, "%d,%ld.%03ld,-%ld.%03ld\n", result_total,
			raw_data_x / 1000, raw_data_x % 1000,
			(long int)abs(raw_data_y / 1000), (long int)abs(raw_data_y % 1000));
	} else {
		return sprintf(buf, "%d,%ld.%03ld,%ld.%03ld\n",
			result_total, raw_data_x / 1000, raw_data_x % 1000,
			raw_data_y / 1000, raw_data_y % 1000);
	}
}

static ssize_t gyro_rawdata_test_show(struct device *dev,
			struct device_attribute *attr, char *buf)
{
	long raw_data_x = 0, raw_data_y = 0;

	cam_ois_offset_test(g_o_ctrl, &raw_data_x, &raw_data_y, 0);

	pr_info("%s: raw data x = %ld.%03ld, raw data y = %ld.%03ld\n", __func__,
		raw_data_x / 1000, raw_data_x % 1000,
		raw_data_y / 1000, raw_data_y % 1000);

	if (raw_data_x < 0 && raw_data_y < 0) {
		return sprintf(buf, "-%ld.%03ld,-%ld.%03ld\n",
			(long int)abs(raw_data_x / 1000), (long int)abs(raw_data_x % 1000),
			(long int)abs(raw_data_y / 1000), (long int)abs(raw_data_y % 1000));
	} else if (raw_data_x < 0) {
		return sprintf(buf, "-%ld.%03ld,%ld.%03ld\n",
			(long int)abs(raw_data_x / 1000), (long int)abs(raw_data_x % 1000),
			raw_data_y / 1000, raw_data_y % 1000);
	} else if (raw_data_y < 0) {
		return sprintf(buf, "%ld.%03ld,-%ld.%03ld\n",
			raw_data_x / 1000, raw_data_x % 1000,
			(long int)abs(raw_data_y / 1000), (long int)abs(raw_data_y % 1000));
	} else {
		return sprintf(buf, "%ld.%03ld,%ld.%03ld\n",
			raw_data_x / 1000, raw_data_x % 1000,
			raw_data_y / 1000, raw_data_y % 1000);
	}
}

char ois_fw_full[SYSFS_FW_VER_SIZE] = "NULL NULL\n";
static ssize_t ois_fw_full_show(struct device *dev,
	struct device_attribute *attr, char *buf)
{
	pr_info("[FW_DBG] OIS_fw_ver : %s\n", ois_fw_full);
	return snprintf(buf, sizeof(ois_fw_full), "%s", ois_fw_full);
}

static ssize_t ois_fw_full_store(struct device *dev,
	struct device_attribute *attr, const char *buf, size_t size)
{
	pr_info("[FW_DBG] buf : %s\n", buf);
	snprintf(ois_fw_full, sizeof(ois_fw_full), "%s", buf);

	return size;
}

char ois_debug[40] = "NULL NULL NULL\n";
static ssize_t ois_exif_show(struct device *dev,
	struct device_attribute *attr, char *buf)
{
	pr_info("[FW_DBG] ois_debug : %s\n", ois_debug);
	return snprintf(buf, sizeof(ois_debug), "%s", ois_debug);
}

static ssize_t ois_exif_store(struct device *dev,
	struct device_attribute *attr, const char *buf, size_t size)
{
	pr_info("[FW_DBG] buf: %s\n", buf);
	snprintf(ois_debug, sizeof(ois_debug), "%s", buf);

	return size;
}

#if defined(CONFIG_CAMERA_DYNAMIC_MIPI)
char mipi_string[20] = {0, };
static ssize_t front_camera_mipi_clock_show(struct device *dev,
		struct device_attribute *attr, char *buf)
{
	pr_info("[FW_DBG] ois_debug : %s\n", mipi_string);
	return sprintf(buf, "%s\n", mipi_string);
}
#endif

#if defined(CONFIG_CAMERA_FRS_DRAM_TEST)
extern uint8_t rear_frs_test_mode;
static ssize_t rear_camera_frs_test_show(struct device *dev,
		struct device_attribute *attr, char *buf)
{
	pr_info("[FRS_DBG] rear_frs_test_mode : %d\n", rear_frs_test_mode);
	return sprintf(buf, "%d\n", rear_frs_test_mode);
}

static ssize_t rear_camera_frs_test_shore(struct device *dev,
					  struct device_attribute *attr, const char *buf, size_t size)
{
	pr_info("[FRS_DBG] rear_frs_test_mode : %c\n", buf[0]);
	if (buf[0] == '1')
		rear_frs_test_mode = 1;
	else
		rear_frs_test_mode = 0;

	return size;
}
#endif

#if defined(CONFIG_CAMERA_FAC_LN_TEST) // Factory Low Noise Test
extern uint8_t factory_ln_test;
static ssize_t cam_factory_ln_test_show(struct device *dev,
		struct device_attribute *attr, char *buf)
{
	pr_info("[LN_TEST] factory_ln_test : %d\n", factory_ln_test);
	return sprintf(buf, "%d\n", factory_ln_test);
}
static ssize_t cam_factory_ln_test_store(struct device *dev,
					  struct device_attribute *attr, const char *buf, size_t size)
{
	pr_info("[LN_TEST] factory_ln_test : %c\n", buf[0]);
	if (buf[0] == '1')
		factory_ln_test = 1;
	else
		factory_ln_test = 0;

	return size;
}
#endif

#if defined(CONFIG_USE_CAMERA_HW_BIG_DATA)
static int16_t is_hw_param_valid_module_id(char *moduleid)
{
	int i = 0;
	int32_t moduleid_cnt = 0;
	int16_t rc = HW_PARAMS_MI_VALID;

	if (moduleid == NULL) {
		CAM_ERR(CAM_HWB, "MI_INVALID\n");
		return HW_PARAMS_MI_INVALID;
	}

	for (i = 0; i < FROM_MODULE_ID_SIZE; i++) {
		if (moduleid[i] == '\0') {
			moduleid_cnt = moduleid_cnt + 1;
		} else if ((i < 5)
				&& (!((moduleid[i] > 47 && moduleid[i] < 58)	// 0 to 9
						|| (moduleid[i] > 64 && moduleid[i] < 91)))) { // A to Z
			CAM_ERR(CAM_HWB, "MIR_ERR_1\n");
			rc = HW_PARAMS_MIR_ERR_1;
			break;
		}
	}

	if (moduleid_cnt == FROM_MODULE_ID_SIZE) {
		CAM_ERR(CAM_HWB, "MIR_ERR_0\n");
		rc = HW_PARAMS_MIR_ERR_0;
	}

	return rc;
}

static ssize_t rear_camera_hw_param_show(struct device *dev,
		struct device_attribute *attr, char *buf)
{
	ssize_t rc = 0;
	int16_t moduelid_chk = 0;
	struct cam_hw_param *ec_param = NULL;

	msm_is_sec_get_rear_hw_param(&ec_param);

	if (ec_param != NULL) {
		moduelid_chk = is_hw_param_valid_module_id(rear_module_id);
		switch (moduelid_chk) {
		case HW_PARAMS_MI_VALID:
			rc = sprintf(buf, "\"CAMIR_ID\":\"%c%c%c%c%cXX%02X%02X%02X\",\"I2CR_AF\":\"%d\",\"I2CR_COM\":\"%d\",\"I2CR_OIS\":\"%d\","
					"\"I2CR_SEN\":\"%d\",\"MIPIR_COM\":\"%d\",\"MIPIR_SEN\":\"%d\"\n",
					rear_module_id[0], rear_module_id[1], rear_module_id[2], rear_module_id[3],
					rear_module_id[4], rear_module_id[7], rear_module_id[8], rear_module_id[9],
					ec_param->i2c_af_err_cnt, ec_param->i2c_comp_err_cnt, ec_param->i2c_ois_err_cnt,
					ec_param->i2c_sensor_err_cnt, ec_param->mipi_comp_err_cnt,
					ec_param->mipi_sensor_err_cnt);
			break;

		case HW_PARAMS_MIR_ERR_1:
			rc = sprintf(buf, "\"CAMIR_ID\":\"MIR_ERR\",\"I2CR_AF\":\"%d\",\"I2CR_COM\":\"%d\",\"I2CR_OIS\":\"%d\","
					"\"I2CR_SEN\":\"%d\",\"MIPIR_COM\":\"%d\",\"MIPIR_SEN\":\"%d\"\n",
					ec_param->i2c_af_err_cnt, ec_param->i2c_comp_err_cnt, ec_param->i2c_ois_err_cnt,
					ec_param->i2c_sensor_err_cnt, ec_param->mipi_comp_err_cnt,
					ec_param->mipi_sensor_err_cnt);
			break;

		default:
			rc = sprintf(buf, "\"CAMIR_ID\":\"MI_NO\",\"I2CR_AF\":\"%d\",\"I2CR_COM\":\"%d\",\"I2CR_OIS\":\"%d\","
					"\"I2CR_SEN\":\"%d\",\"MIPIR_COM\":\"%d\",\"MIPIR_SEN\":\"%d\"\n",
					ec_param->i2c_af_err_cnt, ec_param->i2c_comp_err_cnt, ec_param->i2c_ois_err_cnt,
					ec_param->i2c_sensor_err_cnt, ec_param->mipi_comp_err_cnt,
					ec_param->mipi_sensor_err_cnt);
			break;
		}
	}

	return rc;
}

static ssize_t rear_camera_hw_param_store(struct device *dev,
		struct device_attribute *attr, const char *buf, size_t size)
{
	struct cam_hw_param *ec_param = NULL;

	CAM_DBG(CAM_HWB, "[R] buf : %s\n", buf);

	if (!strncmp(buf, "c", 1)) {
		msm_is_sec_get_rear_hw_param(&ec_param);
		if (ec_param != NULL) {
			msm_is_sec_init_err_cnt_file(ec_param);
		}
	}

	return size;
}

static ssize_t front_camera_hw_param_show(struct device *dev,
		struct device_attribute *attr, char *buf)
{
	ssize_t rc = 0;
	int16_t moduelid_chk = 0;
	struct cam_hw_param *ec_param = NULL;

	msm_is_sec_get_front_hw_param(&ec_param);

	if (ec_param != NULL) {
		moduelid_chk = is_hw_param_valid_module_id(front_module_id);

		switch (moduelid_chk) {
		case HW_PARAMS_MI_VALID:
			rc = sprintf(buf, "\"CAMIF_ID\":\"%c%c%c%c%cXX%02X%02X%02X\",\"I2CF_AF\":\"%d\",\"I2CF_COM\":\"%d\",\"I2CF_OIS\":\"%d\","
					"\"I2CF_SEN\":\"%d\",\"MIPIF_COM\":\"%d\",\"MIPIF_SEN\":\"%d\"\n",
					front_module_id[0], front_module_id[1], front_module_id[2], front_module_id[3],
					front_module_id[4], front_module_id[7], front_module_id[8], front_module_id[9],
					ec_param->i2c_af_err_cnt, ec_param->i2c_comp_err_cnt, ec_param->i2c_ois_err_cnt,
					ec_param->i2c_sensor_err_cnt, ec_param->mipi_comp_err_cnt,
					ec_param->mipi_sensor_err_cnt);
			break;

		case HW_PARAMS_MIR_ERR_1:
			rc = sprintf(buf, "\"CAMIF_ID\":\"MIR_ERR\",\"I2CF_AF\":\"%d\",\"I2CF_COM\":\"%d\",\"I2CF_OIS\":\"%d\","
					"\"I2CF_SEN\":\"%d\",\"MIPIF_COM\":\"%d\",\"MIPIF_SEN\":\"%d\"\n",
					ec_param->i2c_af_err_cnt, ec_param->i2c_comp_err_cnt, ec_param->i2c_ois_err_cnt,
					ec_param->i2c_sensor_err_cnt, ec_param->mipi_comp_err_cnt,
					ec_param->mipi_sensor_err_cnt);
			break;

		default:
			rc = sprintf(buf, "\"CAMIF_ID\":\"MI_NO\",\"I2CF_AF\":\"%d\",\"I2CF_COM\":\"%d\",\"I2CF_OIS\":\"%d\","
					"\"I2CF_SEN\":\"%d\",\"MIPIF_COM\":\"%d\",\"MIPIF_SEN\":\"%d\"\n",
					ec_param->i2c_af_err_cnt, ec_param->i2c_comp_err_cnt, ec_param->i2c_ois_err_cnt,
					ec_param->i2c_sensor_err_cnt, ec_param->mipi_comp_err_cnt,
					ec_param->mipi_sensor_err_cnt);
			break;
		}
	}

	return rc;
}

static ssize_t front_camera_hw_param_store(struct device *dev,
		struct device_attribute *attr, const char *buf, size_t size)
{
	struct cam_hw_param *ec_param = NULL;

	CAM_DBG(CAM_HWB, "[F] buf : %s\n", buf);

	if (!strncmp(buf, "c", 1)) {
		msm_is_sec_get_front_hw_param(&ec_param);
		if (ec_param != NULL) {
			msm_is_sec_init_err_cnt_file(ec_param);
		}
	}

	return size;
}

#if defined(CONFIG_SAMSUNG_SECURE_CAMERA)
uint8_t iris_module_id[FROM_MODULE_ID_SIZE + 1] = "\0";
static ssize_t iris_camera_hw_param_show(struct device *dev,
		struct device_attribute *attr, char *buf)
{
	ssize_t rc = 0;
	int16_t moduelid_chk = 0;
	struct cam_hw_param *ec_param = NULL;

	msm_is_sec_get_iris_hw_param(&ec_param);

	if (ec_param != NULL) {
		moduelid_chk = is_hw_param_valid_module_id(iris_module_id);

		switch (moduelid_chk) {
		case HW_PARAMS_MI_VALID:
			rc = sprintf(buf, "\"CAMII_ID\":\"%c%c%c%c%cXX%02X%02X%02X\",\"I2CI_AF\":\"%d\",\"I2CI_COM\":\"%d\",\"I2CI_OIS\":\"%d\","
					"\"I2CI_SEN\":\"%d\",\"MIPII_COM\":\"%d\",\"MIPII_SEN\":\"%d\"\n",
					iris_module_id[0], iris_module_id[1], iris_module_id[2], iris_module_id[3],
					iris_module_id[4], iris_module_id[7], iris_module_id[8], iris_module_id[9],
					ec_param->i2c_af_err_cnt, ec_param->i2c_comp_err_cnt, ec_param->i2c_ois_err_cnt,
					ec_param->i2c_sensor_err_cnt, ec_param->mipi_comp_err_cnt,
					ec_param->mipi_sensor_err_cnt);
			break;

		case HW_PARAMS_MIR_ERR_1:
			rc = sprintf(buf, "\"CAMII_ID\":\"MIR_ERR\",\"I2CI_AF\":\"%d\",\"I2CI_COM\":\"%d\",\"I2CI_OIS\":\"%d\","
					"\"I2CI_SEN\":\"%d\",\"MIPII_COM\":\"%d\",\"MIPII_SEN\":\"%d\"\n",
					ec_param->i2c_af_err_cnt, ec_param->i2c_comp_err_cnt, ec_param->i2c_ois_err_cnt,
					ec_param->i2c_sensor_err_cnt, ec_param->mipi_comp_err_cnt,
					ec_param->mipi_sensor_err_cnt);
			break;

		default:
			rc = sprintf(buf, "\"CAMII_ID\":\"MI_NO\",\"I2CI_AF\":\"%d\",\"I2CI_COM\":\"%d\",\"I2CI_OIS\":\"%d\","
					"\"I2CI_SEN\":\"%d\",\"MIPII_COM\":\"%d\",\"MIPII_SEN\":\"%d\"\n",
					ec_param->i2c_af_err_cnt, ec_param->i2c_comp_err_cnt, ec_param->i2c_ois_err_cnt,
					ec_param->i2c_sensor_err_cnt, ec_param->mipi_comp_err_cnt,
					ec_param->mipi_sensor_err_cnt);
			break;
		}
	}

	return rc;
}

static ssize_t iris_camera_hw_param_store(struct device *dev,
		struct device_attribute *attr, const char *buf, size_t size)
{
	struct cam_hw_param *ec_param = NULL;

	CAM_DBG(CAM_HWB, "[I] buf : %s\n", buf);

	if (!strncmp(buf, "c", 1)) {
		msm_is_sec_get_iris_hw_param(&ec_param);
		if (ec_param != NULL) {
			msm_is_sec_init_err_cnt_file(ec_param);
		}
	}

	return size;
}
#endif

#if defined(CONFIG_SAMSUNG_MULTI_CAMERA)
static ssize_t rear2_camera_hw_param_show(struct device *dev,
		struct device_attribute *attr, char *buf)
{
	ssize_t rc = 0;
	int16_t moduelid_chk = 0;
	struct cam_hw_param *ec_param = NULL;

	msm_is_sec_get_rear2_hw_param(&ec_param);

	if (ec_param != NULL) {
		moduelid_chk = is_hw_param_valid_module_id(rear_module_id);

		switch (moduelid_chk) {
		case HW_PARAMS_MI_VALID:
			rc = sprintf(buf, "\"CAMIR2_ID\":\"%c%c%c%c%cXX%02X%02X%02X\",\"I2CR2_AF\":\"%d\",\"I2CR2_COM\":\"%d\",\"I2CR2_OIS\":\"%d\","
					"\"I2CR2_SEN\":\"%d\",\"MIPIR2_COM\":\"%d\",\"MIPIR2_SEN\":\"%d\"\n",
					rear_module_id[0], rear_module_id[1], rear_module_id[2], rear_module_id[3],
					rear_module_id[4], rear_module_id[7], rear_module_id[8], rear_module_id[9],
					ec_param->i2c_af_err_cnt, ec_param->i2c_comp_err_cnt, ec_param->i2c_ois_err_cnt,
					ec_param->i2c_sensor_err_cnt, ec_param->mipi_comp_err_cnt,
					ec_param->mipi_sensor_err_cnt);
			break;

		case HW_PARAMS_MIR_ERR_1:
			rc = sprintf(buf, "\"CAMIR2_ID\":\"MIR_ERR\",\"I2CR2_AF\":\"%d\",\"I2CR2_COM\":\"%d\",\"I2CR2_OIS\":\"%d\","
					"\"I2CR2_SEN\":\"%d\",\"MIPIR2_COM\":\"%d\",\"MIPIR2_SEN\":\"%d\"\n",
					ec_param->i2c_af_err_cnt, ec_param->i2c_comp_err_cnt, ec_param->i2c_ois_err_cnt,
					ec_param->i2c_sensor_err_cnt, ec_param->mipi_comp_err_cnt,
					ec_param->mipi_sensor_err_cnt);
			break;

		default:
			rc = sprintf(buf, "\"CAMIR2_ID\":\"MI_NO\",\"I2CR2_AF\":\"%d\",\"I2CR2_COM\":\"%d\",\"I2CR2_OIS\":\"%d\","
					"\"I2CR2_SEN\":\"%d\",\"MIPIR2_COM\":\"%d\",\"MIPIR2_SEN\":\"%d\"\n",
					ec_param->i2c_af_err_cnt, ec_param->i2c_comp_err_cnt, ec_param->i2c_ois_err_cnt,
					ec_param->i2c_sensor_err_cnt, ec_param->mipi_comp_err_cnt,
					ec_param->mipi_sensor_err_cnt);
			break;
		}
	}

	return rc;
}

static ssize_t rear2_camera_hw_param_store(struct device *dev,
		struct device_attribute *attr, const char *buf, size_t size)
{
	struct cam_hw_param *ec_param = NULL;

	CAM_DBG(CAM_HWB, "[R2] buf : %s\n", buf);

	if (!strncmp(buf, "c", 1)) {
		msm_is_sec_get_rear2_hw_param(&ec_param);
		if (ec_param != NULL) {
			msm_is_sec_init_err_cnt_file(ec_param);
		}
	}

	return size;
}
#endif
#endif

#if defined(CONFIG_CAMERA_SSM_I2C_ENV)
static DEVICE_ATTR(ssm_frame_id, S_IRUGO|S_IWUSR|S_IWGRP,
	back_camera_ssm_frame_id_show, back_camera_ssm_frame_id_store);
static DEVICE_ATTR(ssm_flicker_max_r, S_IRUGO|S_IWUSR|S_IWGRP,
	back_camera_ssm_flicker_max_r_show, back_camera_ssm_flicker_max_r_store);
static DEVICE_ATTR(ssm_flicker_max_g, S_IRUGO|S_IWUSR|S_IWGRP,
	back_camera_ssm_flicker_max_g_show, back_camera_ssm_flicker_max_g_store);
static DEVICE_ATTR(ssm_flicker_max_b, S_IRUGO|S_IWUSR|S_IWGRP,
	back_camera_ssm_flicker_max_b_show, back_camera_ssm_flicker_max_b_store);
static DEVICE_ATTR(ssm_flicker_coeff, S_IRUGO|S_IWUSR|S_IWGRP,
	back_camera_ssm_flicker_coeff_show, back_camera_ssm_flicker_coeff_store);
static DEVICE_ATTR(ssm_diff_max_g, S_IRUGO|S_IWUSR|S_IWGRP,
	back_camera_ssm_diff_max_g_show, back_camera_ssm_diff_max_g_store);
#endif

static DEVICE_ATTR(rear_camtype, S_IRUGO, back_camera_type_show, NULL);
static DEVICE_ATTR(rear_camfw, S_IRUGO|S_IWUSR|S_IWGRP,
	back_camera_firmware_show, back_camera_firmware_store);
static DEVICE_ATTR(rear_checkfw_user, S_IRUGO|S_IWUSR|S_IWGRP,
	back_camera_firmware_user_show, back_camera_firmware_user_store);
static DEVICE_ATTR(rear_checkfw_factory, S_IRUGO|S_IWUSR|S_IWGRP,
	back_camera_firmware_factory_show, back_camera_firmware_factory_store);
static DEVICE_ATTR(rear_camfw_full, S_IRUGO|S_IWUSR|S_IWGRP,
	back_camera_firmware_full_show, back_camera_firmware_full_store);
static DEVICE_ATTR(rear_camfw_load, S_IRUGO|S_IWUSR|S_IWGRP,
	back_camera_firmware_load_show, back_camera_firmware_load_store);
static DEVICE_ATTR(rear_calcheck, S_IRUGO|S_IWUSR|S_IWGRP,
	back_cal_data_check_show, back_cal_data_check_store);
static DEVICE_ATTR(rear_moduleinfo, S_IRUGO|S_IWUSR|S_IWGRP,
	back_module_info_show, back_module_info_store);
static DEVICE_ATTR(front_moduleinfo, S_IRUGO|S_IWUSR|S_IWGRP,
	front_module_info_show, front_module_info_store);
static DEVICE_ATTR(isp_core, S_IRUGO|S_IWUSR|S_IWGRP,
	back_isp_core_check_show, back_isp_core_check_store);
static DEVICE_ATTR(rear_afcal, S_IRUGO, back_camera_afcal_show, NULL);
static DEVICE_ATTR(rear_paf_offset_far, S_IRUGO,
		rear_paf_offset_far_show, NULL);
static DEVICE_ATTR(rear_paf_offset_mid, S_IRUGO,
		rear_paf_offset_mid_show, NULL);
static DEVICE_ATTR(rear_paf_cal_check, S_IRUGO,
		rear_paf_cal_check_show, NULL);
static DEVICE_ATTR(rear_f2_paf_offset_far, S_IRUGO,
		rear_f2_paf_offset_far_show, NULL);
static DEVICE_ATTR(rear_f2_paf_offset_mid, S_IRUGO,
		rear_f2_paf_offset_mid_show, NULL);
static DEVICE_ATTR(rear_f2_paf_cal_check, S_IRUGO,
		rear_f2_paf_cal_check_show, NULL);
static DEVICE_ATTR(front_afcal, S_IRUGO, front_camera_afcal_show, NULL);
static DEVICE_ATTR(front_camtype, S_IRUGO, front_camera_type_show, NULL);
static DEVICE_ATTR(front_camfw, S_IRUGO|S_IWUSR|S_IWGRP,
		front_camera_firmware_show, front_camera_firmware_store);
static DEVICE_ATTR(front_camfw_full, S_IRUGO | S_IWUSR | S_IWGRP,
		front_camera_firmware_full_show, front_camera_firmware_full_store);
static DEVICE_ATTR(front_checkfw_user, S_IRUGO|S_IWUSR|S_IWGRP,
	front_camera_firmware_user_show, front_camera_firmware_user_store);
static DEVICE_ATTR(front_checkfw_factory, S_IRUGO|S_IWUSR|S_IWGRP,
	front_camera_firmware_factory_show, front_camera_firmware_factory_store);
#if defined (CONFIG_CAMERA_SYSFS_V2)
static DEVICE_ATTR(rear_caminfo, S_IRUGO|S_IWUSR|S_IWGRP,
		rear_camera_info_show, rear_camera_info_store);
static DEVICE_ATTR(front_caminfo, S_IRUGO|S_IWUSR|S_IWGRP,
		front_camera_info_show, front_camera_info_store);
#endif
static DEVICE_ATTR(rear_sensorid_exif, S_IRUGO|S_IWUSR|S_IWGRP,
		rear_sensorid_exif_show, rear_sensorid_exif_store);
static DEVICE_ATTR(front_sensorid_exif, S_IRUGO|S_IWUSR|S_IWGRP,
		front_sensorid_exif_show, front_sensorid_exif_store);
static DEVICE_ATTR(rear_moduleid, S_IRUGO, back_camera_moduleid_show, NULL);
static DEVICE_ATTR(front_moduleid, S_IRUGO, front_camera_moduleid_show, NULL);
static DEVICE_ATTR(front_mtf_exif, S_IRUGO|S_IWUSR|S_IWGRP,
		front_mtf_exif_show, front_mtf_exif_store);
#if defined(CONFIG_CAMERA_DYNAMIC_MIPI)
static DEVICE_ATTR(front_mipi_clock, S_IRUGO, front_camera_mipi_clock_show, NULL);
#endif
static DEVICE_ATTR(rear_mtf_exif, S_IRUGO|S_IWUSR|S_IWGRP,
		rear_mtf_exif_show, rear_mtf_exif_store);
static DEVICE_ATTR(rear_mtf2_exif, S_IRUGO|S_IWUSR|S_IWGRP,
		rear_mtf2_exif_show, rear_mtf2_exif_store);
static DEVICE_ATTR(SVC_rear_module, S_IRUGO, back_camera_moduleid_show, NULL);
static DEVICE_ATTR(SVC_front_module, S_IRUGO, front_camera_moduleid_show, NULL);
static DEVICE_ATTR(ssrm_camera_info, S_IRUGO|S_IWUSR|S_IWGRP,
		ssrm_camera_info_show, ssrm_camera_info_store);

#if defined(CONFIG_SAMSUNG_SECURE_CAMERA)
static DEVICE_ATTR(iris_camfw, S_IRUGO|S_IWUSR|S_IWGRP,
		iris_camera_firmware_show, iris_camera_firmware_store);
static DEVICE_ATTR(iris_checkfw_user, S_IRUGO|S_IWUSR|S_IWGRP,
		iris_camera_firmware_user_show, iris_camera_firmware_user_store);
static DEVICE_ATTR(iris_checkfw_factory, S_IRUGO|S_IWUSR|S_IWGRP,
		iris_camera_firmware_factory_show, iris_camera_firmware_factory_store);
static DEVICE_ATTR(iris_camfw_full, S_IRUGO|S_IWUSR|S_IWGRP,
		iris_camera_firmware_full_show, iris_camera_firmware_full_store);
#if defined(CONFIG_CAMERA_SYSFS_V2)
static DEVICE_ATTR(iris_caminfo, S_IRUGO|S_IWUSR|S_IWGRP,
		iris_camera_info_show, iris_camera_info_store);
#endif
#endif

#if defined(CONFIG_SAMSUNG_MULTI_CAMERA)
static DEVICE_ATTR(rear2_camfw, S_IRUGO|S_IWUSR|S_IWGRP,
	back_camera2_firmware_show, back_camera2_firmware_store);
static DEVICE_ATTR(rear2_camfw_full, S_IRUGO|S_IWUSR|S_IWGRP,
	back_camera2_firmware_full_show, back_camera2_firmware_full_store);
static DEVICE_ATTR(rear2_afcal, S_IRUGO, back_camera2_afcal_show, NULL);
static DEVICE_ATTR(rear2_caminfo, S_IRUGO|S_IWUSR|S_IWGRP,
		rear2_camera_info_show, rear2_camera_info_store);
static DEVICE_ATTR(rear2_camtype, S_IRUGO, back_camera2_type_show, NULL);
static DEVICE_ATTR(rear2_mtf_exif, S_IRUGO|S_IWUSR|S_IWGRP,
		rear2_mtf_exif_show, rear2_mtf_exif_store);
static DEVICE_ATTR(rear2_sensorid_exif, S_IRUGO|S_IWUSR|S_IWGRP,
		rear2_sensorid_exif_show, rear2_sensorid_exif_store);
static DEVICE_ATTR(rear_dualcal, S_IRUGO, rear_dual_cal_show, NULL);
static DEVICE_ATTR(rear_dualcal_extra, S_IRUGO, rear_dual_cal_extra_show, NULL);
static DEVICE_ATTR(rear_dualcal_size, S_IRUGO, rear_dual_cal_size_show, NULL);
static DEVICE_ATTR(rear2_tilt, S_IRUGO, rear2_tilt_show, NULL);
#endif

#if defined(CONFIG_USE_CAMERA_HW_BIG_DATA)
static DEVICE_ATTR(rear_hwparam, S_IRUGO|S_IWUSR|S_IWGRP,
		rear_camera_hw_param_show, rear_camera_hw_param_store);
static DEVICE_ATTR(front_hwparam, S_IRUGO|S_IWUSR|S_IWGRP,
		front_camera_hw_param_show, front_camera_hw_param_store);
#if defined(CONFIG_SAMSUNG_SECURE_CAMERA)
static DEVICE_ATTR(iris_hwparam, S_IRUGO|S_IWUSR|S_IWGRP,
		iris_camera_hw_param_show, iris_camera_hw_param_store);
#endif
#if defined(CONFIG_SAMSUNG_MULTI_CAMERA)
static DEVICE_ATTR(rear2_hwparam, S_IRUGO|S_IWUSR|S_IWGRP,
		rear2_camera_hw_param_show, rear2_camera_hw_param_store);
#endif
#endif

static DEVICE_ATTR(ois_power, S_IWUSR, NULL, ois_power_store);
static DEVICE_ATTR(autotest, S_IRUGO|S_IWUSR|S_IWGRP, ois_autotest_show, ois_autotest_store);
#if defined(CONFIG_SAMSUNG_OIS_RUMBA_S6)
static DEVICE_ATTR(autotest_2nd, S_IRUGO|S_IWUSR|S_IWGRP, ois_autotest_2nd_show, ois_autotest_2nd_store);
#endif
static DEVICE_ATTR(selftest, S_IRUGO, gyro_selftest_show, NULL);
static DEVICE_ATTR(ois_rawdata, S_IRUGO, gyro_rawdata_test_show, NULL);
static DEVICE_ATTR(oisfw, S_IRUGO|S_IWUSR|S_IWGRP, ois_fw_full_show, ois_fw_full_store);
static DEVICE_ATTR(ois_exif, S_IRUGO|S_IWUSR|S_IWGRP, ois_exif_show, ois_exif_store);

#if defined(CONFIG_CAMERA_FRS_DRAM_TEST)
static DEVICE_ATTR(rear_frs_test, S_IRUGO|S_IWUSR|S_IWGRP,
		rear_camera_frs_test_show, rear_camera_frs_test_shore);
#endif
#if defined(CONFIG_CAMERA_FAC_LN_TEST)
static DEVICE_ATTR(cam_ln_test, S_IRUGO|S_IWUSR|S_IWGRP,
		cam_factory_ln_test_show, cam_factory_ln_test_store);
#endif


int svc_cheating_prevent_device_file_create(struct kobject **obj)
{
	struct kernfs_node *SVC_sd;
	struct kobject *data;
	struct kobject *Camera;

	/* To find SVC kobject */
	SVC_sd = sysfs_get_dirent(devices_kset->kobj.sd, "svc");
	if (IS_ERR_OR_NULL(SVC_sd)) {
		/* try to create SVC kobject */
		data = kobject_create_and_add("SVC", &devices_kset->kobj);
		if (IS_ERR_OR_NULL(data))
			pr_info("Failed to create sys/devices/svc already exist svc : 0x%p\n", data);
		else
			pr_info("Success to create sys/devices/svc svc : 0x%p\n", data);
	} else {
		data = (struct kobject *)SVC_sd->priv;
		pr_info("Success to find SVC_sd : 0x%p SVC : 0x%p\n", SVC_sd, data);
	}

	Camera = kobject_create_and_add("Camera", data);
	if (IS_ERR_OR_NULL(Camera))
		pr_info("Failed to create sys/devices/svc/Camera : 0x%p\n", Camera);
	else
		pr_info("Success to create sys/devices/svc/Camera : 0x%p\n", Camera);

	*obj = Camera;
	return 0;
}


#if defined(CONFIG_SAMSUNG_MULTI_CAMERA)
char af_position_value[128] = "0\n";
static ssize_t af_position_show(struct device *dev,
					 struct device_attribute *attr, char *buf)
{
	pr_info("[syscamera] af_position_show : %s\n", af_position_value);
	return snprintf(buf, sizeof(af_position_value), "%s", af_position_value);
}

static ssize_t af_position_store(struct device *dev,
					  struct device_attribute *attr, const char *buf, size_t size)
{
	pr_info("[FW_DBG] buf : %s\n", buf);
	snprintf(af_position_value, sizeof(af_position_value), "%s", buf);
	return size;
}
static DEVICE_ATTR(af_position, S_IRUGO|S_IWUSR|S_IWGRP,
		af_position_show, af_position_store);

char dual_fallback_value[SYSFS_FW_VER_SIZE] = "0\n";
static ssize_t dual_fallback_show(struct device *dev,
					 struct device_attribute *attr, char *buf)
{
	pr_info("[syscamera] dual_fallback_show : %s\n", dual_fallback_value);
	return snprintf(buf, sizeof(dual_fallback_value), "%s", dual_fallback_value);
}

static ssize_t dual_fallback_store(struct device *dev,
					  struct device_attribute *attr, const char *buf, size_t size)
{
	pr_info("[FW_DBG] buf : %s\n", buf);
	snprintf(dual_fallback_value, sizeof(dual_fallback_value), "%s", buf);
	return size;
}
static DEVICE_ATTR(fallback, S_IRUGO|S_IWUSR|S_IWGRP,
		dual_fallback_show, dual_fallback_store);
#endif

static int __init cam_sysfs_init(void)
{
	struct device		  *cam_dev_back;
	struct device		  *cam_dev_front;
#if defined(CONFIG_SAMSUNG_SECURE_CAMERA)
	struct device		  *cam_dev_iris;
#endif
#if defined(CONFIG_SAMSUNG_MULTI_CAMERA)
	struct device		  *cam_dev_af;
	struct device		  *cam_dev_dual;
#endif
	struct device		  *cam_dev_ois;
#if defined(CONFIG_CAMERA_SSM_I2C_ENV)
	struct device		  *cam_dev_ssm;
#endif
	struct kobject *SVC = 0;
	int ret = 0;

	svc_cheating_prevent_device_file_create(&SVC);

	camera_class = class_create(THIS_MODULE, "camera");
	if (IS_ERR(camera_class))
		pr_err("failed to create device cam_dev_rear!\n");

#if defined(CONFIG_CAMERA_SSM_I2C_ENV)
	cam_dev_ssm = device_create(camera_class, NULL,
		0, NULL, "ssm");

	if (device_create_file(cam_dev_ssm, &dev_attr_ssm_frame_id) < 0) {
		pr_err("Failed to create device file!(%s)!\n",
			dev_attr_ssm_frame_id.attr.name);
		ret = -ENODEV;
	}
	if (device_create_file(cam_dev_ssm, &dev_attr_ssm_flicker_max_r) < 0) {
		pr_err("Failed to create device file!(%s)!\n",
			dev_attr_ssm_flicker_max_r.attr.name);
		ret = -ENODEV;
	}
	if (device_create_file(cam_dev_ssm, &dev_attr_ssm_flicker_max_g) < 0) {
		pr_err("Failed to create device file!(%s)!\n",
			dev_attr_ssm_flicker_max_g.attr.name);
		ret = -ENODEV;
	}
	if (device_create_file(cam_dev_ssm, &dev_attr_ssm_flicker_max_b) < 0) {
		pr_err("Failed to create device file!(%s)!\n",
			dev_attr_ssm_flicker_max_b.attr.name);
		ret = -ENODEV;
	}
	if (device_create_file(cam_dev_ssm, &dev_attr_ssm_flicker_coeff) < 0) {
		pr_err("Failed to create device file!(%s)!\n",
			dev_attr_ssm_flicker_coeff.attr.name);
		ret = -ENODEV;
	}
	if (device_create_file(cam_dev_ssm, &dev_attr_ssm_diff_max_g) < 0) {
		pr_err("Failed to create device file!(%s)!\n",
			dev_attr_ssm_diff_max_g.attr.name);
		ret = -ENODEV;
	}
#endif

	cam_dev_back = device_create(camera_class, NULL,
		1, NULL, "rear");

	if (device_create_file(cam_dev_back, &dev_attr_rear_camtype) < 0) {
		pr_err("Failed to create device file!(%s)!\n",
			dev_attr_rear_camtype.attr.name);
		ret = -ENODEV;
	}
	if (device_create_file(cam_dev_back, &dev_attr_rear_camfw) < 0) {
		pr_err("Failed to create device file!(%s)!\n",
			dev_attr_rear_camfw.attr.name);
		ret = -ENODEV;
	}
	if (device_create_file(cam_dev_back, &dev_attr_rear_checkfw_user) < 0) {
		pr_err("Failed to create device file!(%s)!\n",
			dev_attr_rear_checkfw_user.attr.name);
		ret = -ENODEV;
	}
	if (device_create_file(cam_dev_back, &dev_attr_rear_checkfw_factory) < 0) {
		pr_err("Failed to create device file!(%s)!\n",
			dev_attr_rear_checkfw_factory.attr.name);
		ret = -ENODEV;
	}
	if (device_create_file(cam_dev_back, &dev_attr_rear_camfw_full) < 0) {
		pr_err("Failed to create device file!(%s)!\n",
			dev_attr_rear_camfw_full.attr.name);
		ret = -ENODEV;
	}
	if (device_create_file(cam_dev_back, &dev_attr_rear_camfw_load) < 0) {
		pr_err("Failed to create device file!(%s)!\n",
			dev_attr_rear_camfw_load.attr.name);
		ret = -ENODEV;
	}
	if (device_create_file(cam_dev_back, &dev_attr_rear_calcheck) < 0) {
		pr_err("Failed to create device file!(%s)!\n",
			dev_attr_rear_calcheck.attr.name);
		ret = -ENODEV;
	}
	if (device_create_file(cam_dev_back, &dev_attr_rear_moduleinfo) < 0) {
		pr_err("Failed to create device file!(%s)!\n",
			dev_attr_rear_moduleinfo.attr.name);
		ret = -ENODEV;
	}
	if (device_create_file(cam_dev_back, &dev_attr_isp_core) < 0) {
		pr_err("Failed to create device file!(%s)!\n",
			dev_attr_isp_core.attr.name);
		ret = -ENODEV;
	}
#if defined(CONFIG_CAMERA_SYSFS_V2)
	if (device_create_file(cam_dev_back, &dev_attr_rear_caminfo) < 0) {
		pr_err("Failed to create device file!(%s)!\n",
			dev_attr_rear_caminfo.attr.name);
	}
#endif
	if (device_create_file(cam_dev_back, &dev_attr_rear_afcal) < 0) {
		pr_err("Failed to create device file!(%s)!\n",
			dev_attr_rear_afcal.attr.name);
		ret = -ENODEV;
	}
	if (device_create_file(cam_dev_back, &dev_attr_rear_paf_offset_far) < 0) {
		pr_err("Failed to create device file!(%s)!\n",
			dev_attr_rear_paf_offset_far.attr.name);
		ret = -ENODEV;
	}
	if (device_create_file(cam_dev_back, &dev_attr_rear_paf_offset_mid) < 0) {
		pr_err("Failed to create device file!(%s)!\n",
			dev_attr_rear_paf_offset_mid.attr.name);
		ret = -ENODEV;
	}
	if (device_create_file(cam_dev_back, &dev_attr_rear_paf_cal_check) < 0) {
		pr_err("Failed to create device file!(%s)!\n",
			dev_attr_rear_paf_cal_check.attr.name);
		ret = -ENODEV;
	}
	if (device_create_file(cam_dev_back, &dev_attr_rear_f2_paf_offset_far) < 0) {
		pr_err("Failed to create device file!(%s)!\n",
			dev_attr_rear_f2_paf_offset_far.attr.name);
		ret = -ENODEV;
	}
	if (device_create_file(cam_dev_back, &dev_attr_rear_f2_paf_offset_mid) < 0) {
		pr_err("Failed to create device file!(%s)!\n",
			dev_attr_rear_f2_paf_offset_mid.attr.name);
		ret = -ENODEV;
	}
	if (device_create_file(cam_dev_back, &dev_attr_rear_f2_paf_cal_check) < 0) {
		pr_err("Failed to create device file!(%s)!\n",
			dev_attr_rear_f2_paf_cal_check.attr.name);
		ret = -ENODEV;
	}
	if (device_create_file(cam_dev_back, &dev_attr_rear_sensorid_exif) < 0) {
		pr_err("Failed to create device file!(%s)!\n",
			dev_attr_rear_sensorid_exif.attr.name);
		ret = -ENODEV;
	}
	if (device_create_file(cam_dev_back, &dev_attr_rear_moduleid) < 0) {
		pr_err("Failed to create device file!(%s)!\n",
			dev_attr_rear_moduleid.attr.name);
		ret = -ENODEV;
	}
	if (device_create_file(cam_dev_back, &dev_attr_rear_mtf_exif) < 0) {
		pr_err("Failed to create device file!(%s)!\n",
			dev_attr_rear_mtf_exif.attr.name);
		ret = -ENODEV;
	}
	if (device_create_file(cam_dev_back, &dev_attr_rear_mtf2_exif) < 0) {
		pr_err("Failed to create device file!(%s)!\n",
			dev_attr_rear_mtf2_exif.attr.name);
		ret = -ENODEV;
	}
	if (device_create_file(cam_dev_back, &dev_attr_ssrm_camera_info) < 0) {
		pr_err("Failed to create device file!(%s)!\n",
			dev_attr_ssrm_camera_info.attr.name);
		ret = -ENODEV;
	}
	if (sysfs_create_file(SVC, &dev_attr_SVC_rear_module.attr) < 0) {
		printk("Failed to create device file!(%s)!\n",
			dev_attr_SVC_rear_module.attr.name);
		ret = -ENODEV;
	}
	cam_dev_front = device_create(camera_class, NULL,
		2, NULL, "front");

	if (IS_ERR(cam_dev_front)) {
		pr_err("Failed to create cam_dev_front device!");
		ret = -ENODEV;
	}

	if (device_create_file(cam_dev_front, &dev_attr_front_camtype) < 0) {
		pr_err("Failed to create device file!(%s)!\n",
			dev_attr_front_camtype.attr.name);
		ret = -ENODEV;
	}
	if (device_create_file(cam_dev_front, &dev_attr_front_camfw) < 0) {
		pr_err("Failed to create device file!(%s)!\n",
			dev_attr_front_camfw.attr.name);
		ret = -ENODEV;
	}
	if (device_create_file(cam_dev_front, &dev_attr_front_camfw_full) < 0) {
		pr_err("Failed to create device file!(%s)!\n",
			dev_attr_front_camfw_full.attr.name);
		ret = -ENODEV;
	}
	if (device_create_file(cam_dev_front, &dev_attr_front_checkfw_user) < 0) {
		pr_err("Failed to create device file!(%s)!\n",
			dev_attr_front_checkfw_user.attr.name);
		ret = -ENODEV;
	}
	if (device_create_file(cam_dev_front, &dev_attr_front_checkfw_factory) < 0) {
		pr_err("Failed to create device file!(%s)!\n",
			dev_attr_front_checkfw_factory.attr.name);
		ret = -ENODEV;
	}
	if (device_create_file(cam_dev_front, &dev_attr_front_moduleinfo) < 0) {
		pr_err("Failed to create device file!(%s)!\n",
			dev_attr_front_moduleinfo.attr.name);
		ret = -ENODEV;
	}
#if defined(CONFIG_CAMERA_SYSFS_V2)
	if (device_create_file(cam_dev_front, &dev_attr_front_caminfo) < 0) {
		pr_err("Failed to create device file!(%s)!\n",
			dev_attr_front_caminfo.attr.name);
	}
#endif
	if (device_create_file(cam_dev_front, &dev_attr_front_afcal) < 0) {
		pr_err("Failed to create device file!(%s)!\n",
			dev_attr_front_afcal.attr.name);
		ret = -ENODEV;
	}
	if (device_create_file(cam_dev_front, &dev_attr_front_sensorid_exif) < 0) {
		pr_err("Failed to create device file!(%s)!\n",
			dev_attr_front_sensorid_exif.attr.name);
		ret = -ENODEV;
	}
	if (device_create_file(cam_dev_front, &dev_attr_front_moduleid) < 0) {
		pr_err("Failed to create device file!(%s)!\n",
			dev_attr_front_moduleid.attr.name);
		ret = -ENODEV;
	}
	if (device_create_file(cam_dev_front, &dev_attr_front_mtf_exif) < 0) {
		pr_err("Failed to create device file!(%s)!\n",
			dev_attr_front_mtf_exif.attr.name);
		ret = -ENODEV;
	}
	if (sysfs_create_file(SVC, &dev_attr_SVC_front_module.attr) < 0) {
		printk("Failed to create device file!(%s)!\n",
			dev_attr_SVC_front_module.attr.name);
		ret = -ENODEV;
	}
#if defined(CONFIG_SAMSUNG_SECURE_CAMERA)
	cam_dev_iris = device_create(camera_class, NULL,
		2, NULL, "secure");

	if (IS_ERR(cam_dev_iris)) {
		pr_err("Failed to create cam_dev_iris device!");
		ret = -ENODEV;
	}
	if (device_create_file(cam_dev_iris, &dev_attr_iris_camfw) < 0) {
		pr_err("Failed to create device file!(%s)!\n",
			dev_attr_iris_camfw.attr.name);
		ret = -ENODEV;
	}
	if (device_create_file(cam_dev_iris, &dev_attr_iris_camfw_full) < 0) {
		pr_err("Failed to create device file!(%s)!\n",
			dev_attr_iris_camfw_full.attr.name);
		ret = -ENODEV;
	}
	if (device_create_file(cam_dev_iris, &dev_attr_iris_checkfw_user) < 0) {
		pr_err("Failed to create device file!(%s)!\n",
			dev_attr_iris_checkfw_user.attr.name);
		ret = -ENODEV;
	}
	if (device_create_file(cam_dev_iris, &dev_attr_iris_checkfw_factory) < 0) {
		pr_err("Failed to create device file!(%s)!\n",
			dev_attr_iris_checkfw_factory.attr.name);
		ret = -ENODEV;
	}
#if defined(CONFIG_CAMERA_SYSFS_V2)
	if (device_create_file(cam_dev_iris, &dev_attr_iris_caminfo) < 0) {
		pr_err("Failed to create device file!(%s)!\n",
			dev_attr_iris_caminfo.attr.name);
	}
#endif
#endif
#if defined(CONFIG_SAMSUNG_MULTI_CAMERA)
	if (device_create_file(cam_dev_back, &dev_attr_rear2_camfw) < 0) {
		pr_err("Failed to create device file!(%s)!\n",
			dev_attr_rear2_camfw.attr.name);
		ret = -ENODEV;
	}
	if (device_create_file(cam_dev_back, &dev_attr_rear2_camfw_full) < 0) {
		pr_err("Failed to create device file!(%s)!\n",
			dev_attr_rear2_camfw_full.attr.name);
		ret = -ENODEV;
	}
	if (device_create_file(cam_dev_back, &dev_attr_rear2_afcal) < 0) {
		pr_err("Failed to create device file!(%s)!\n",
			dev_attr_rear2_afcal.attr.name);
		ret = -ENODEV;
	}
	if (device_create_file(cam_dev_back, &dev_attr_rear2_tilt) < 0) {
		pr_err("Failed to create device file!(%s)!\n",
			dev_attr_rear2_tilt.attr.name);
		ret = -ENODEV;
	}
	if (device_create_file(cam_dev_back, &dev_attr_rear2_caminfo) < 0) {
		pr_err("Failed to create device file!(%s)!\n",
			dev_attr_rear2_caminfo.attr.name);
	}
	if (device_create_file(cam_dev_back, &dev_attr_rear2_camtype) < 0) {
		pr_err("Failed to create device file!(%s)!\n",
			dev_attr_rear2_camtype.attr.name);
		ret = -ENODEV;
	}
	if (device_create_file(cam_dev_back, &dev_attr_rear2_mtf_exif) < 0) {
		pr_err("Failed to create device file!(%s)!\n",
			dev_attr_rear2_mtf_exif.attr.name);
		ret = -ENODEV;
	}
	if (device_create_file(cam_dev_back, &dev_attr_rear2_sensorid_exif) < 0) {
		pr_err("Failed to create device file!(%s)!\n",
			dev_attr_rear2_sensorid_exif.attr.name);
		ret = -ENODEV;
	}
	if (device_create_file(cam_dev_back, &dev_attr_rear_dualcal) < 0) {
		pr_err("Failed to create device file!(%s)!\n",
			dev_attr_rear_dualcal.attr.name);
		ret = -ENODEV;
	}
	if (device_create_file(cam_dev_back, &dev_attr_rear_dualcal_extra) < 0) {
		pr_err("Failed to create device file!(%s)!\n",
			dev_attr_rear_dualcal_extra.attr.name);
		ret = -ENODEV;
	}
	if (device_create_file(cam_dev_back, &dev_attr_rear_dualcal_size) < 0) {
		pr_err("Failed to create device file!(%s)!\n",
			dev_attr_rear_dualcal_size.attr.name);
		ret = -ENODEV;
	}
#endif
#if defined(CONFIG_USE_CAMERA_HW_BIG_DATA)
	if (device_create_file(cam_dev_back, &dev_attr_rear_hwparam) < 0) {
		pr_err("Failed to create device file!(%s)!\n",
				dev_attr_rear_hwparam.attr.name);
	}
	if (device_create_file(cam_dev_front, &dev_attr_front_hwparam) < 0) {
		pr_err("Failed to create device file!(%s)!\n",
				dev_attr_front_hwparam.attr.name);
	}
#if defined(CONFIG_SAMSUNG_SECURE_CAMERA)
	if (device_create_file(cam_dev_iris, &dev_attr_iris_hwparam) < 0) {
		pr_err("Failed to create device file!(%s)!\n",
				dev_attr_iris_hwparam.attr.name);
	}
#endif
#if defined(CONFIG_SAMSUNG_MULTI_CAMERA)
	if (device_create_file(cam_dev_back, &dev_attr_rear2_hwparam) < 0) {
		pr_err("Failed to create device file!(%s)!\n",
				dev_attr_rear2_hwparam.attr.name);
	}
#endif
#endif

#if defined(CONFIG_SAMSUNG_MULTI_CAMERA)
	cam_dev_af = device_create(camera_class, NULL,
		1, NULL, "af");

	if (IS_ERR(cam_dev_af)) {
		pr_err("Failed to create cam_dev_af device!\n");
		ret = -ENODEV;
	}
	if (device_create_file(cam_dev_af, &dev_attr_af_position) < 0) {
		pr_err("Failed to create device file!(%s)!\n",
			dev_attr_af_position.attr.name);
		ret = -ENODEV;
	}

	cam_dev_dual = device_create(camera_class, NULL,
		1, NULL, "dual");
	if (IS_ERR(cam_dev_dual)) {
		pr_err("Failed to create cam_dev_dual device!\n");
		ret = -ENODEV;
	}
	if (device_create_file(cam_dev_dual, &dev_attr_fallback) < 0) {
		pr_err("Failed to create device file!(%s)!\n",
			dev_attr_fallback.attr.name);
		ret = -ENODEV;
	}
#endif

	cam_dev_ois = device_create(camera_class, NULL,
		0, NULL, "ois");
	if (IS_ERR(cam_dev_ois)) {
		pr_err("Failed to create cam_dev_ois device!\n");
		ret = -ENOENT;
	}
	if (device_create_file(cam_dev_ois, &dev_attr_ois_power) < 0) {
		pr_err("failed to create device file, %s\n",
			dev_attr_ois_power.attr.name);
		ret = -ENOENT;
	}
	if (device_create_file(cam_dev_ois, &dev_attr_autotest) < 0) {
		pr_err("Failed to create device file, %s\n",
			dev_attr_autotest.attr.name);
		ret = -ENODEV;
	}
#if defined(CONFIG_SAMSUNG_OIS_RUMBA_S6)
	if (device_create_file(cam_dev_ois, &dev_attr_autotest_2nd) < 0) {
		pr_err("Failed to create device file, %s\n",
			dev_attr_autotest_2nd.attr.name);
		ret = -ENODEV;
	}
#endif
	if (device_create_file(cam_dev_ois, &dev_attr_selftest) < 0) {
		pr_err("failed to create device file, %s\n",
		dev_attr_selftest.attr.name);
		ret = -ENOENT;
	}
	if (device_create_file(cam_dev_ois, &dev_attr_ois_rawdata) < 0) {
		pr_err("failed to create device file, %s\n",
		dev_attr_ois_rawdata.attr.name);
		ret = -ENOENT;
	}
	if (device_create_file(cam_dev_ois, &dev_attr_oisfw) < 0) {
		pr_err("Failed to create device file, %s\n",
		dev_attr_oisfw.attr.name);
		ret = -ENODEV;
	}

	if (device_create_file(cam_dev_ois, &dev_attr_ois_exif) < 0) {
		pr_err("Failed to create device file,%s\n",
			dev_attr_ois_exif.attr.name);
		ret = -ENODEV;
	}
#if defined(CONFIG_CAMERA_DYNAMIC_MIPI)
	if (device_create_file(cam_dev_front, &dev_attr_front_mipi_clock) < 0) {
		pr_err("failed to create front device file, %s\n",
		dev_attr_front_mipi_clock.attr.name);
	}
	cam_mipi_register_ril_notifier();
#endif
#if defined(CONFIG_CAMERA_FRS_DRAM_TEST)
	if (device_create_file(cam_dev_back, &dev_attr_rear_frs_test) < 0) {
		pr_err("Failed to create device file!(%s)!\n",
			dev_attr_rear_frs_test.attr.name);
		ret = -ENODEV;
	}
#endif
#if defined(CONFIG_CAMERA_FAC_LN_TEST)
	if (device_create_file(cam_dev_back, &dev_attr_cam_ln_test) < 0) {
		pr_err("Failed to create device file!(%s)!\n",
			dev_attr_cam_ln_test.attr.name);
		ret = -ENODEV;
	}
#endif
	return ret;
}

module_init(cam_sysfs_init);
MODULE_DESCRIPTION("cam_sysfs_init");
