/* Copyright (c) 2017, The Linux Foundation. All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 and
 * only version 2 as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	 See the
 * GNU General Public License for more details.
 */

#include <linux/module.h>
#include <linux/regulator/consumer.h>
#include <linux/clk.h>
#include <cam_sensor_cmn_header.h>
#include "cam_sensor_core.h"
#include "cam_sensor_util.h"
#include "cam_soc_util.h"
#include "cam_trace.h"
#if defined(CONFIG_SENSOR_RETENTION)
#include "cam_sensor_retention.h"
#endif

struct cam_sensor_ctrl_t *g_s_ctrl;
#if defined(CONFIG_CAMERA_FRS_DRAM_TEST)
extern uint8_t frs_frame_cnt;
uint8_t rear_frs_test_mode;
uint8_t frs_output_done_flag;
#endif

#if defined(CONFIG_CAMERA_FAC_LN_TEST)
uint8_t factory_ln_test;
#define AE_GROUP_START_ADDR 0x0104 // AE groupHold setting
#endif

#if defined(CONFIG_CAMERA_DYNAMIC_MIPI)
#include "cam_sensor_mipi.h"
#define STREAM_ON_ADDR   0x100
#define FRONT_SENSOR_ID_IMX320 0x320
#define INVALID_MIPI_INDEX -1
#endif

#if defined(CONFIG_SENSOR_RETENTION)
uint8_t sensor_retention_mode = RETENTION_INIT;
uint32_t sensor_retention_checksum_value;
uint32_t sensor_version;
uint32_t sensor_nvm;
#endif

#if defined(CONFIG_USE_CAMERA_HW_BIG_DATA)
//#define HWB_FILE_OPERATION 1
uint32_t sec_sensor_position;
uint32_t sec_sensor_clk_size;

static struct cam_hw_param_collector cam_hwparam_collector;
#endif

#if defined(CONFIG_SAMSUNG_SECURE_CAMERA)
#define FW_VER_SIZE 40
#define IRIS_CMD_CHECK_RESOLUTION_1 8
#define IRIS_CMD_CHECK_RESOLUTION_2 9
extern char iris_cam_fw_ver[FW_VER_SIZE];
extern char iris_cam_fw_full_ver[FW_VER_SIZE];
extern char iris_cam_fw_user_ver[FW_VER_SIZE];
extern char iris_cam_fw_factory_ver[FW_VER_SIZE];

int cam_sensor_check_resolution(struct cam_sensor_ctrl_t *s_ctrl)
{
	struct cam_sensor_i2c_reg_setting reg_setting;
	struct cam_sensor_i2c_reg_array reg_arr;
	int rc = 0;
	uint32_t page_info = 0;
	uint32_t page_select = 0;
	uint32_t sensor_rev = 0;
	uint32_t sensor_test[3] = { 0, };
	uint32_t iris_cam_year = 0;
	uint32_t iris_cam_month = 0;
	uint32_t iris_cam_company = 0;
	uint8_t year_month_company[3] = {'0', '0', '0'};
	bool is_final_module = false;
	bool is_mtf_check = false;
	bool is_read_ver = false;

	/* Stream On */
	memset(&reg_setting, 0, sizeof(reg_setting));
	memset(&reg_arr, 0, sizeof(reg_arr));
	reg_setting.size = 1;
	reg_setting.addr_type = CAMERA_SENSOR_I2C_TYPE_WORD;
	reg_setting.data_type = CAMERA_SENSOR_I2C_TYPE_WORD;
	reg_setting.reg_setting = &reg_arr;
	reg_arr.reg_addr = 0x0100;
	reg_arr.reg_data = 0x0100;
	rc = camera_io_dev_write(&s_ctrl->io_master_info, &reg_setting);
	if (rc < 0) {
		CAM_ERR(CAM_SENSOR, "stream on failed");
		goto exit;
	}

	/* page select */
	/* set the PAGE 62 for reading page_info number */
	reg_arr.reg_addr = 0x0A02;
	reg_arr.reg_data = 0x3E00;
	rc = camera_io_dev_write(&s_ctrl->io_master_info, &reg_setting);
	if (rc < 0) {
		CAM_ERR(CAM_SENSOR, "stream on failed");
		goto exit;
	}

	/* read start */
	reg_arr.reg_addr = 0x0A00;
	reg_arr.reg_data = 0x0100;
	rc = camera_io_dev_write(&s_ctrl->io_master_info, &reg_setting);
	if (rc < 0) {
		CAM_ERR(CAM_SENSOR, "stream on failed");
		goto exit;
	}

	usleep_range(350, 360);

	/* read page_info number 0x01(p.62), 0x03(p.63) */
	rc = camera_io_dev_read(&s_ctrl->io_master_info, 0x0A12,
		&page_info, CAMERA_SENSOR_I2C_TYPE_WORD, CAMERA_SENSOR_I2C_TYPE_BYTE);
	if (rc < 0) {
		CAM_ERR(CAM_SENSOR, "stream on failed");
		goto stream_off;
	}

	/* page select */
	CAM_INFO(CAM_SENSOR, "page select : 0x%x", page_info);
	if (page_info == 0x00 || page_info == 0x01)
		page_select = 0x3E00;
	else if (page_info == 0x03)
		page_select = 0x3F00;
	else {
		is_mtf_check = false;
		CAM_ERR(CAM_SENSOR, "page read fail read data %d", page_select);
		goto stream_off;
	}

	/* set the PAGE of OTP */
	reg_arr.reg_addr = 0x0A02;
	reg_arr.reg_data = page_select;
	rc = camera_io_dev_write(&s_ctrl->io_master_info, &reg_setting);
	if (rc < 0) {
		CAM_ERR(CAM_SENSOR, "set page failed");
		goto stream_off;
	}

	/* read start */
	reg_arr.reg_addr = 0x0A00;
	reg_arr.reg_data = 0x0100;
	rc = camera_io_dev_write(&s_ctrl->io_master_info, &reg_setting);
	if (rc < 0) {
		CAM_ERR(CAM_SENSOR, "start read page fail");
		goto stream_off;
	}

	usleep_range(350, 360);

	/* read pass or fail /year/month/company information */
	/* read test result */

	/* focusing */
	rc = camera_io_dev_read(&s_ctrl->io_master_info, 0x0A04,
		&sensor_test[0], CAMERA_SENSOR_I2C_TYPE_WORD, CAMERA_SENSOR_I2C_TYPE_BYTE);
	if (rc < 0)
		CAM_ERR(CAM_SENSOR, "read focusing test result failed");
	CAM_INFO(CAM_SENSOR, "read focusing test result : 0x%x", sensor_test[0]);

	/* macro */
	rc = camera_io_dev_read(&s_ctrl->io_master_info, 0x0A05,
		&sensor_test[1], CAMERA_SENSOR_I2C_TYPE_WORD, CAMERA_SENSOR_I2C_TYPE_BYTE);
	if (rc < 0)
		CAM_ERR(CAM_SENSOR, "read macro test result failed");
	CAM_INFO(CAM_SENSOR, "read macro test result : 0x%x", sensor_test[1]);

	/* pan */
	rc = camera_io_dev_read(&s_ctrl->io_master_info, 0x0A06,
		&sensor_test[2], CAMERA_SENSOR_I2C_TYPE_WORD, CAMERA_SENSOR_I2C_TYPE_BYTE);
	if (rc < 0)
		CAM_ERR(CAM_SENSOR, "read pan test result failed");
	CAM_INFO(CAM_SENSOR, "read pan test result : 0x%x", sensor_test[2]);

	if ((sensor_test[0] == 0x01) &&
		(sensor_test[1] == 0x01) &&
		(sensor_test[2] == 0x01))
		is_mtf_check = true;
	else
		is_mtf_check = false;

	/* read year */
	rc = camera_io_dev_read(&s_ctrl->io_master_info, 0x0A07,
		&iris_cam_year, CAMERA_SENSOR_I2C_TYPE_WORD, CAMERA_SENSOR_I2C_TYPE_BYTE);
	if (rc < 0)
		CAM_ERR(CAM_SENSOR, "read year failed");
	CAM_INFO(CAM_SENSOR, "read year : %c", iris_cam_year);

	/* read month */
	rc = camera_io_dev_read(&s_ctrl->io_master_info, 0x0A08,
		&iris_cam_month, CAMERA_SENSOR_I2C_TYPE_WORD, CAMERA_SENSOR_I2C_TYPE_BYTE);
	if (rc < 0)
		CAM_ERR(CAM_SENSOR, "read month failed");
	CAM_INFO(CAM_SENSOR, "read month : %c", iris_cam_month);

	/* read company */
	rc = camera_io_dev_read(&s_ctrl->io_master_info, 0x0A09,
		&iris_cam_company, CAMERA_SENSOR_I2C_TYPE_WORD, CAMERA_SENSOR_I2C_TYPE_BYTE);
	if (rc < 0)
		CAM_ERR(CAM_SENSOR, "read company failed");
	CAM_INFO(CAM_SENSOR, "read company : %c", iris_cam_company);

	/* read sensor version and write sysfs */
	rc = camera_io_dev_read(&s_ctrl->io_master_info, 0x0002,
		&sensor_rev, CAMERA_SENSOR_I2C_TYPE_WORD, CAMERA_SENSOR_I2C_TYPE_BYTE);
	if (rc < 0) {
		is_read_ver = false;
		CAM_ERR(CAM_SENSOR, "read sensor_rev failed");
	} else {
		is_read_ver = true;
		if (sensor_rev == 0xA1)
			is_final_module = true;
		else
			is_final_module = false;
		CAM_INFO(CAM_SENSOR, "read sensor_rev : 0x%x, is_final_module %d", sensor_rev, (is_final_module ? 1 : 0));
	}

	/* write sysfs for resolution/year/month/company */
	if (iris_cam_year != 0x00 && iris_cam_year >= 'A' && iris_cam_year <= 'Z')
		year_month_company[0] = (uint8_t)iris_cam_year;

	if (iris_cam_month != 0x00 && iris_cam_month >= 'A' && iris_cam_month <= 'Z')
		year_month_company[1] = (uint8_t)iris_cam_month;

	if (iris_cam_company != 0x00 && iris_cam_company >= 'A' && iris_cam_company <= 'Z')
		year_month_company[2] = (uint8_t)iris_cam_company;

stream_off:
	/* Stream Off */
	memset(&reg_setting, 0, sizeof(reg_setting));
	reg_setting.size = 1;
	reg_setting.addr_type = CAMERA_SENSOR_I2C_TYPE_WORD;
	reg_setting.data_type = CAMERA_SENSOR_I2C_TYPE_WORD;
	reg_setting.reg_setting = &reg_arr;
	reg_arr.reg_addr = 0x0100;
	reg_arr.reg_data = 0x0000;
	rc = camera_io_dev_write(&s_ctrl->io_master_info, &reg_setting);
	if (rc < 0) {
		CAM_ERR(CAM_SENSOR, "stream off failed");
		goto exit;
	}

	usleep_range(1000, 1050);

exit:
	if (is_mtf_check == false)
		snprintf(iris_cam_fw_factory_ver, FW_VER_SIZE, "NG_RES %c %c %c\n", year_month_company[0], year_month_company[1], year_month_company[2]); //resolution check fail with 0x00

	if (is_read_ver) {
		if (is_final_module) {
			snprintf(iris_cam_fw_user_ver, FW_VER_SIZE, "OK\n");
			snprintf(iris_cam_fw_factory_ver, FW_VER_SIZE, "OK %c %c %c\n", year_month_company[0], year_month_company[1], year_month_company[2]); // resolution check pass with 0x01
		} else {
			snprintf(iris_cam_fw_user_ver, FW_VER_SIZE, "NG\n");
			snprintf(iris_cam_fw_factory_ver, FW_VER_SIZE, "NG_VER %c %c %c\n", year_month_company[0], year_month_company[1], year_month_company[2]); // resolution check pass but dev module ver
		}
		snprintf(iris_cam_fw_ver, FW_VER_SIZE, "S5K5F1 N\n");
		snprintf(iris_cam_fw_full_ver, FW_VER_SIZE, "S5K5F1 N N\n");
	} else {
		snprintf(iris_cam_fw_user_ver, FW_VER_SIZE, "NG\n");
		snprintf(iris_cam_fw_factory_ver, FW_VER_SIZE, "NG_VER %c %c %c\n", year_month_company[0], year_month_company[1], year_month_company[2]); // resolution check pass but dev module ver
		snprintf(iris_cam_fw_ver, FW_VER_SIZE, "UNKNOWN N\n");
		snprintf(iris_cam_fw_full_ver, FW_VER_SIZE, "UNKNOWN N N\n");
	}
	return rc;
}
#endif

#if defined(CONFIG_SENSOR_RETENTION)
uint32_t cam_sensor_retention_calc_checksum(struct camera_io_master *io_master_info)
{
	int32_t rc = 0;
	uint32_t read_value = 0xBEEF;
	uint8_t read_cnt = 0;
	struct cam_sensor_i2c_reg_setting  i2c_reg_settings;
	struct cam_sensor_i2c_reg_array    i2c_reg_array;

	// 1. Start to calc checksum - write addr: 0x302B, data: 0x01
	i2c_reg_settings.addr_type = CAMERA_SENSOR_I2C_TYPE_WORD;
	i2c_reg_settings.data_type = CAMERA_SENSOR_I2C_TYPE_BYTE;
	i2c_reg_settings.size = 1;
	i2c_reg_settings.delay = 0;
	i2c_reg_array.reg_addr = 0x302B;
	i2c_reg_array.reg_data = 0x1;
	i2c_reg_array.delay = 0;
	i2c_reg_array.data_mask = 0x0;
	i2c_reg_settings.reg_setting = &i2c_reg_array;

	for (read_cnt = 0; read_cnt < SENSOR_RETENTION_READ_RETRY_CNT; read_cnt++) {
		rc = camera_io_dev_write(io_master_info,
			&i2c_reg_settings);
		if (rc < 0) {
			CAM_ERR(CAM_SENSOR, "[RET_DBG] Fail to write to calc checksum rc %d. retry: %d", rc, read_cnt);
			continue;
		}

		// 2. Wait - 3ms delay
		usleep_range(5000, 6000);

		// 3. Check checksum calc - read addr: 0x302B
		camera_io_dev_read(io_master_info, 0x302B, &read_value,
			CAMERA_SENSOR_I2C_TYPE_WORD, CAMERA_SENSOR_I2C_TYPE_BYTE);

		if (read_value == 0) {
			CAM_DBG(CAM_SENSOR, "[RET_DBG] Checksum calc is done");
			break;
		} else {
			CAM_DBG(CAM_SENSOR, "[RET_DBG] Wait until checksum calc is done - 5ms");
			usleep_range(5000, 6000);
		}
	}

	if ((read_cnt == SENSOR_RETENTION_READ_RETRY_CNT) && (read_value != 0)) {
		CAM_ERR(CAM_SENSOR, "[RET_DBG] Fail to calc checksum!");
		return 0;
	}

	read_value = 0xBEEF;

	// 4. Read checksum result - read addr: 0x302C
	camera_io_dev_read(io_master_info, 0x302C, &read_value,
		CAMERA_SENSOR_I2C_TYPE_WORD, CAMERA_SENSOR_I2C_TYPE_DWORD);

	CAM_INFO(CAM_SENSOR, "[RET_DBG] checksum read_value: 0x%x", read_value);
	return read_value;
}

bool cam_sensor_retention_compare_checksum(uint32_t read_value)
{
	CAM_INFO(CAM_SENSOR, "[RET_DBG] Compare saved checksum value: 0x%x and read_value: 0x%x",
		sensor_retention_checksum_value, read_value);

	if (sensor_retention_checksum_value == read_value) {
		CAM_INFO(CAM_SENSOR, "[RET_DBG] Same checksum!");
		return TRUE;
	} else {
		CAM_INFO(CAM_SENSOR, "[RET_DBG] Different checksum!");
		return FALSE;
	}
	return 0;
}

void cam_sensor_retention_write_global(struct camera_io_master *io_master_info)
{
	int32_t rc = 0;
	struct cam_sensor_i2c_reg_setting  i2c_reg_settings;

	i2c_reg_settings.addr_type = CAMERA_SENSOR_I2C_TYPE_WORD;
	i2c_reg_settings.data_type = CAMERA_SENSOR_I2C_TYPE_BYTE;
	i2c_reg_settings.delay = 0;
	if (sensor_version == 0x14 && sensor_nvm == 0x0) {
		CAM_INFO(CAM_SENSOR, "[RET_DBG] Sensor version is 14");
		i2c_reg_settings.size = sizeof(ver14_global_reg_array)/sizeof(struct cam_sensor_i2c_reg_array);
		i2c_reg_settings.reg_setting = ver14_global_reg_array;
	} else if (sensor_version == 0x14 && sensor_nvm == 0x1) {
		CAM_INFO(CAM_SENSOR, "[RET_DBG] Sensor version is 1401");
		i2c_reg_settings.size = sizeof(ver1401_global_reg_array)/sizeof(struct cam_sensor_i2c_reg_array);
		i2c_reg_settings.reg_setting = ver1401_global_reg_array;
	} else if (sensor_version == 0xBEEF || sensor_nvm == 0xBEEF) {
		CAM_ERR(CAM_SENSOR, "[RET_DBG] Error to read sensor info");
		i2c_reg_settings.size = sizeof(ver0_global_reg_array)/sizeof(struct cam_sensor_i2c_reg_array);
		i2c_reg_settings.reg_setting = ver0_global_reg_array;
	} else {
		CAM_INFO(CAM_SENSOR, "[RET_DBG] Sensor version is 0");
		i2c_reg_settings.size = sizeof(ver0_global_reg_array)/sizeof(struct cam_sensor_i2c_reg_array);
		i2c_reg_settings.reg_setting = ver0_global_reg_array;
	}

	rc = camera_io_dev_write(io_master_info,
		&i2c_reg_settings);
}


#if defined(CONFIG_CAMERA_FAC_LN_TEST)
void cam_sensor_ln_test_retention_write_global(struct camera_io_master *io_master_info)
{
	int32_t rc = 0;
	struct cam_sensor_i2c_reg_setting  i2c_reg_settings;

	i2c_reg_settings.addr_type = CAMERA_SENSOR_I2C_TYPE_WORD;
	i2c_reg_settings.data_type = CAMERA_SENSOR_I2C_TYPE_BYTE;
	i2c_reg_settings.delay = 0;
	if (sensor_version == 0x14 && sensor_nvm == 0x0) {
		CAM_INFO(CAM_SENSOR, "[LN_TEST] Sensor version is 14");
		i2c_reg_settings.size = sizeof(ver14_ln4_test_global_reg_array)/sizeof(struct cam_sensor_i2c_reg_array);
		i2c_reg_settings.reg_setting = ver14_ln4_test_global_reg_array;
	} else if (sensor_version == 0x14 && sensor_nvm == 0x1) {
		CAM_INFO(CAM_SENSOR, "[LN_TEST] Sensor version is 1401");
		i2c_reg_settings.size = sizeof(ver1401_ln4_test_global_reg_array)/sizeof(struct cam_sensor_i2c_reg_array);
		i2c_reg_settings.reg_setting = ver1401_ln4_test_global_reg_array;
	} else {
		CAM_ERR(CAM_SENSOR, "[LN_TEST] Invalid sensor info");
		i2c_reg_settings.size = sizeof(ver0_global_reg_array)/sizeof(struct cam_sensor_i2c_reg_array);
		i2c_reg_settings.reg_setting = ver0_global_reg_array;
	}

	rc = camera_io_dev_write(io_master_info,
		&i2c_reg_settings);
}

void cam_sensor_ln_test_write_grouphold(struct camera_io_master *io_master_info)
{
	int32_t rc = 0;
	struct cam_sensor_i2c_reg_setting  i2c_reg_settings;

	i2c_reg_settings.addr_type = CAMERA_SENSOR_I2C_TYPE_WORD;
	i2c_reg_settings.data_type = CAMERA_SENSOR_I2C_TYPE_BYTE;
	i2c_reg_settings.delay = 0;

	CAM_INFO(CAM_SENSOR, "[LN_TEST] Set LN4, 20fps, exposure 1ms");
	i2c_reg_settings.size = sizeof(ln4_test_group_reg_array)/sizeof(struct cam_sensor_i2c_reg_array);
	i2c_reg_settings.reg_setting = ln4_test_group_reg_array;

	rc = camera_io_dev_write(io_master_info,
		&i2c_reg_settings);
}
#endif

uint32_t cam_sensor_read_revision(struct camera_io_master *io_master_info)
{
	int32_t rc = 0;
	uint32_t read_value = 0xBEEF;
	uint8_t read_cnt = 0;
	struct cam_sensor_i2c_reg_setting  i2c_reg_settings;
	struct cam_sensor_i2c_reg_array    i2c_reg_array;

	i2c_reg_settings.addr_type = CAMERA_SENSOR_I2C_TYPE_WORD;
	i2c_reg_settings.data_type = CAMERA_SENSOR_I2C_TYPE_BYTE;
	i2c_reg_settings.size = 1;
	i2c_reg_settings.delay = 0;

	i2c_reg_array.reg_addr = 0x0a02;
	i2c_reg_array.reg_data = 0x5F;
	i2c_reg_array.delay = 0;
	i2c_reg_array.data_mask = 0x0;
	i2c_reg_settings.reg_setting = &i2c_reg_array;

	rc = camera_io_dev_write(io_master_info,
		&i2c_reg_settings);

	i2c_reg_array.reg_addr = 0x0a00;
	i2c_reg_array.reg_data = 0x01;

	rc = camera_io_dev_write(io_master_info,
		&i2c_reg_settings);

	usleep_range(3000, 4000);

	for (read_cnt = 0; read_cnt < SENSOR_RETENTION_READ_RETRY_CNT; read_cnt++) {
		camera_io_dev_read(io_master_info, 0x0a01, &read_value,
			CAMERA_SENSOR_I2C_TYPE_WORD, CAMERA_SENSOR_I2C_TYPE_BYTE);

		if (read_value == 0x01) {
			CAM_DBG(CAM_SENSOR, "[RET_DBG] Ready to read sensor version");
			break;
		} else {
			CAM_DBG(CAM_SENSOR, "[RET_DBG] Wait for reading sensor version - 3ms");
			usleep_range(3000, 4000);
		}
	}

	if ((read_cnt == SENSOR_RETENTION_READ_RETRY_CNT) && (read_value != 0x01)) {
		CAM_ERR(CAM_SENSOR, "[RET_DBG] Fail to read sensor version");
		return 0xBEEF;
	}

	read_value = 0xBEEF;
	camera_io_dev_read(io_master_info, 0x0a21, &read_value,
		CAMERA_SENSOR_I2C_TYPE_WORD, CAMERA_SENSOR_I2C_TYPE_BYTE);

	CAM_INFO(CAM_SENSOR, "[RET_DBG] IMX345 sensor version: 0x%x", read_value);
	return read_value;
}

uint32_t cam_sensor_read_nvm(struct camera_io_master *io_master_info)
{
	int32_t rc = 0;
	uint32_t read_value = 0xBEEF;
	uint8_t read_cnt = 0;
	struct cam_sensor_i2c_reg_setting  i2c_reg_settings;
	struct cam_sensor_i2c_reg_array    i2c_reg_array;

	i2c_reg_settings.addr_type = CAMERA_SENSOR_I2C_TYPE_WORD;
	i2c_reg_settings.data_type = CAMERA_SENSOR_I2C_TYPE_BYTE;
	i2c_reg_settings.size = 1;
	i2c_reg_settings.delay = 0;

	i2c_reg_array.reg_addr = 0x0a02;
	i2c_reg_array.reg_data = 0x0;
	i2c_reg_array.delay = 0;
	i2c_reg_array.data_mask = 0x0;
	i2c_reg_settings.reg_setting = &i2c_reg_array;

	rc = camera_io_dev_write(io_master_info,
		&i2c_reg_settings);

	i2c_reg_array.reg_addr = 0x0a00;
	i2c_reg_array.reg_data = 0x01;

	rc = camera_io_dev_write(io_master_info,
		&i2c_reg_settings);

	usleep_range(3000, 4000);

	for (read_cnt = 0; read_cnt < SENSOR_RETENTION_READ_RETRY_CNT; read_cnt++) {
		camera_io_dev_read(io_master_info, 0x0a01, &read_value,
			CAMERA_SENSOR_I2C_TYPE_WORD, CAMERA_SENSOR_I2C_TYPE_BYTE);

		if (read_value == 0x01) {
			CAM_DBG(CAM_SENSOR, "[RET_DBG] Ready to read sensor nvm");
			break;
		} else {
			CAM_DBG(CAM_SENSOR, "[RET_DBG] Wait for reading sensor nvm - 3ms");
			usleep_range(3000, 4000);
		}
	}

	if ((read_cnt == SENSOR_RETENTION_READ_RETRY_CNT) && (read_value != 0x01)) {
		CAM_ERR(CAM_SENSOR, "[RET_DBG] Fail to read sensor nvm");
		return 0xBEEF;
	}

	read_value = 0xBEEF;
	camera_io_dev_read(io_master_info, 0x0a04, &read_value,
		CAMERA_SENSOR_I2C_TYPE_WORD, CAMERA_SENSOR_I2C_TYPE_BYTE);

	CAM_INFO(CAM_SENSOR, "[RET_DBG] IMX345 sensor nvm: 0x%x", read_value);
	return read_value;
}

#endif

#if defined(CONFIG_CAMERA_FRS_DRAM_TEST)
/* Write FRS test setting again in order to avoid sof freeze */
uint32_t cam_sensor_frs_test_setting_retry(void)
{
	struct cam_sensor_i2c_reg_setting reg_setting;
	struct cam_sensor_i2c_reg_array reg_arr;
	uint16_t sensor_id = g_s_ctrl->sensordata->slave_info.sensor_id;
	int32_t rc = 0;

	CAM_INFO(CAM_SENSOR, "[FRS_DBG] START - FRS stream on again. sensor_id: 0x%x", sensor_id);

	if (sensor_id != 0x345) { // support for IMX345
		CAM_ERR(CAM_SENSOR, "[FRS_DBG} Failed. sensor_id: 0x%x", sensor_id);
		return -1;
	}

	memset(&reg_setting, 0, sizeof(reg_setting));
	memset(&reg_arr, 0, sizeof(reg_arr));
	reg_setting.size = 1;
	reg_setting.addr_type = CAMERA_SENSOR_I2C_TYPE_WORD;
	reg_setting.data_type = CAMERA_SENSOR_I2C_TYPE_BYTE;
	reg_setting.reg_setting = &reg_arr;
	reg_arr.reg_addr = 0x0E00;
	reg_arr.reg_data = 0x08;
	rc = camera_io_dev_write(&(g_s_ctrl->io_master_info), &reg_setting);
	if (rc < 0) {
		CAM_ERR(CAM_SENSOR, "[FRS_DBG} Failed to write 0x0E00, 0x08");
		return rc;
	}

	reg_arr.reg_addr = 0x0100;
	reg_arr.reg_data = 0x01;
	rc = camera_io_dev_write(&(g_s_ctrl->io_master_info), &reg_setting);
	if (rc < 0) {
		CAM_ERR(CAM_SENSOR, "[FRS_DBG} Failed to write 0x0100, 0x01");
		return rc;
	}

	CAM_INFO(CAM_SENSOR, "[FRS_DBG] END - FRS stream on again");
	return rc;
}

uint32_t cam_sensor_frs_test_check_reg(void)
{
		int rc = 0;
		uint32_t frame_cnt = 0;
		uint32_t total_frame_num = 0;

		// read frame count
		rc = camera_io_dev_read(&g_s_ctrl->io_master_info, 0x0005,
			&frame_cnt, CAMERA_SENSOR_I2C_TYPE_WORD, CAMERA_SENSOR_I2C_TYPE_BYTE);
		if (rc < 0)
			CAM_ERR(CAM_SENSOR, "[FRS_DBG} Failed to read 0x0005");

		// read bracketing over n frame
		rc = camera_io_dev_read(&g_s_ctrl->io_master_info, 0x0E00,
			&total_frame_num, CAMERA_SENSOR_I2C_TYPE_WORD, CAMERA_SENSOR_I2C_TYPE_BYTE);
		if (rc < 0)
			CAM_ERR(CAM_SENSOR, "[FRS_DBG} Failed to read 0x0E00");

		CAM_INFO(CAM_SENSOR, "[FRS_DBG] read_addr: 0x0005, 0x%x, read_addr: 0x0E00, 0x%x",
			frame_cnt, total_frame_num);

		// check register and write FRS test setting again
		if ((frame_cnt == 0xFF) && (total_frame_num == 0x0)) {
			CAM_INFO(CAM_SENSOR, "[FRS_DBG] 8 test patterns output is done.");
			cam_sensor_frs_test_setting_retry();
			frs_frame_cnt = 0;
		}

		return rc;
}
#endif

#if defined(CONFIG_CAMERA_SSM_I2C_ENV)
void cam_sensor_ssm_i2c_read(uint32_t addr, uint32_t *data,
	enum camera_sensor_i2c_type addr_type,
	enum camera_sensor_i2c_type data_type)
{
	int rc = 0;

	if (g_s_ctrl)
	{
		rc = camera_io_dev_read(&g_s_ctrl->io_master_info, addr,
			data, addr_type, data_type);
		if (rc < 0)
			CAM_ERR(CAM_SENSOR, "Failed to read 0x%x", addr);
	}
	else
	{
		CAM_ERR(CAM_SENSOR, "ssm i2c is not ready!");
	}
}
#endif

static void cam_sensor_update_req_mgr(
	struct cam_sensor_ctrl_t *s_ctrl,
	struct cam_packet *csl_packet)
{
	struct cam_req_mgr_add_request add_req;

	add_req.link_hdl = s_ctrl->bridge_intf.link_hdl;
	add_req.req_id = csl_packet->header.request_id;
	CAM_DBG(CAM_SENSOR, " Rxed Req Id: %lld",
		csl_packet->header.request_id);

#if defined(CONFIG_CAMERA_FRS_DRAM_TEST)
	if (rear_frs_test_mode == 1) {
		CAM_INFO(CAM_SENSOR, "[FRS_DBG] Rxed Req Id: %lld, frs_output_done_flag: %d",
			add_req.req_id, frs_output_done_flag);

		/* Check reg and write FRS test setting again in order to avoid sof freeze */
		cam_sensor_frs_test_check_reg();
	}
#endif
	add_req.dev_hdl = s_ctrl->bridge_intf.device_hdl;
	add_req.skip_before_applying = 0;
	if (s_ctrl->bridge_intf.crm_cb &&
		s_ctrl->bridge_intf.crm_cb->add_req)
		s_ctrl->bridge_intf.crm_cb->add_req(&add_req);

	CAM_DBG(CAM_SENSOR, " add req to req mgr: %lld",
			add_req.req_id);
}

static void cam_sensor_release_stream_rsc(
	struct cam_sensor_ctrl_t *s_ctrl)
{
	struct i2c_settings_array *i2c_set = NULL;
	int rc;

	i2c_set = &(s_ctrl->i2c_data.streamoff_settings);
	if (i2c_set->is_settings_valid == 1) {
		i2c_set->is_settings_valid = -1;
		rc = delete_request(i2c_set);
		if (rc < 0)
			CAM_ERR(CAM_SENSOR,
				"failed while deleting Streamoff settings");
	}

	i2c_set = &(s_ctrl->i2c_data.streamon_settings);
	if (i2c_set->is_settings_valid == 1) {
		i2c_set->is_settings_valid = -1;
		rc = delete_request(i2c_set);
		if (rc < 0)
			CAM_ERR(CAM_SENSOR,
				"failed while deleting Streamon settings");
	}
}

static void cam_sensor_release_resource(
	struct cam_sensor_ctrl_t *s_ctrl)
{
	struct i2c_settings_array *i2c_set = NULL;
	int i, rc;

	i2c_set = &(s_ctrl->i2c_data.init_settings);
	if (i2c_set->is_settings_valid == 1) {
		i2c_set->is_settings_valid = -1;
		rc = delete_request(i2c_set);
		if (rc < 0)
			CAM_ERR(CAM_SENSOR,
				"failed while deleting Init settings");
	}

	i2c_set = &(s_ctrl->i2c_data.config_settings);
	if (i2c_set->is_settings_valid == 1) {
		i2c_set->is_settings_valid = -1;
		rc = delete_request(i2c_set);
		if (rc < 0)
			CAM_ERR(CAM_SENSOR,
				"failed while deleting Res settings");
	}

	if (s_ctrl->i2c_data.per_frame != NULL) {
		for (i = 0; i < MAX_PER_FRAME_ARRAY; i++) {
			i2c_set = &(s_ctrl->i2c_data.per_frame[i]);

			if (i2c_set->is_settings_valid == 1) {
				i2c_set->is_settings_valid = -1;
				rc = delete_request(i2c_set);
				if (rc < 0)
					CAM_ERR(CAM_SENSOR,
						"delete request: %lld rc: %d",
						i2c_set->request_id, rc);
			}
		}
	}
}

static int32_t cam_sensor_i2c_pkt_parse(struct cam_sensor_ctrl_t *s_ctrl,
	void *arg)
{
	int32_t rc = 0;
	uint64_t generic_ptr;
	struct cam_control *ioctl_ctrl = NULL;
	struct cam_packet *csl_packet = NULL;
	struct cam_cmd_buf_desc *cmd_desc = NULL;
	struct i2c_settings_array *i2c_reg_settings = NULL;
	size_t len_of_buff = 0;
	uint32_t *offset = NULL;
	struct cam_config_dev_cmd config;
	struct i2c_data_settings *i2c_data = NULL;

	ioctl_ctrl = (struct cam_control *)arg;

	if (ioctl_ctrl->handle_type != CAM_HANDLE_USER_POINTER) {
		CAM_ERR(CAM_SENSOR, "Invalid Handle Type");
		return -EINVAL;
	}

	if (copy_from_user(&config, (void __user *) ioctl_ctrl->handle,
		sizeof(config)))
		return -EFAULT;

	rc = cam_mem_get_cpu_buf(
		config.packet_handle,
		(uint64_t *)&generic_ptr,
		&len_of_buff);
	if (rc < 0) {
		CAM_ERR(CAM_SENSOR, "Failed in getting the buffer: %d", rc);
		return rc;
	}

	csl_packet = (struct cam_packet *)(generic_ptr +
		config.offset);
	if (config.offset > len_of_buff) {
		CAM_ERR(CAM_SENSOR,
			"offset is out of bounds: off: %lld len: %zu",
			 config.offset, len_of_buff);
		return -EINVAL;
	}

	i2c_data = &(s_ctrl->i2c_data);
	CAM_DBG(CAM_SENSOR, "Header OpCode: %d", csl_packet->header.op_code);
	switch (csl_packet->header.op_code & 0xFFFFFF) {
	case CAM_SENSOR_PACKET_OPCODE_SENSOR_INITIAL_CONFIG: {
		i2c_reg_settings = &i2c_data->init_settings;
		i2c_reg_settings->request_id = 0;
		i2c_reg_settings->is_settings_valid = 1;
		break;
	}
	case CAM_SENSOR_PACKET_OPCODE_SENSOR_CONFIG: {
		i2c_reg_settings = &i2c_data->config_settings;
		i2c_reg_settings->request_id = 0;
		i2c_reg_settings->is_settings_valid = 1;
		break;
	}
	case CAM_SENSOR_PACKET_OPCODE_SENSOR_STREAMON: {
		if (s_ctrl->streamon_count > 0)
			return 0;

		s_ctrl->streamon_count = s_ctrl->streamon_count + 1;
		i2c_reg_settings = &i2c_data->streamon_settings;
		i2c_reg_settings->request_id = 0;
		i2c_reg_settings->is_settings_valid = 1;
		break;
	}
	case CAM_SENSOR_PACKET_OPCODE_SENSOR_STREAMOFF: {
		if (s_ctrl->streamoff_count > 0)
			return 0;

		s_ctrl->streamoff_count = s_ctrl->streamoff_count + 1;
		i2c_reg_settings = &i2c_data->streamoff_settings;
		i2c_reg_settings->request_id = 0;
		i2c_reg_settings->is_settings_valid = 1;
		break;
	}

	case CAM_SENSOR_PACKET_OPCODE_SENSOR_UPDATE: {
		if (s_ctrl->sensor_state < CAM_SENSOR_CONFIG) {
			CAM_WARN(CAM_SENSOR,
				"Rxed Update packets without linking");
			return 0;
		}
		i2c_reg_settings =
			&i2c_data->
			per_frame[csl_packet->header.request_id %
			MAX_PER_FRAME_ARRAY];
		CAM_DBG(CAM_SENSOR, "Received Packet: %lld req: %lld",
			csl_packet->header.request_id % MAX_PER_FRAME_ARRAY, csl_packet->header.request_id);
		if (i2c_reg_settings->is_settings_valid == 1) {
			CAM_ERR(CAM_SENSOR,
				"Already some pkt in offset req : %lld",
				csl_packet->header.request_id);
			/* Update req mgr even in case of Failure
			* This will help not to wait indefinitely
			* and freeze. If this log is triggered then
			* fix it.*/
			cam_sensor_update_req_mgr(s_ctrl, csl_packet);
			return 0;
		}
		break;
	}
	case CAM_SENSOR_PACKET_OPCODE_SENSOR_NOP: {
		if (s_ctrl->sensor_state < CAM_SENSOR_CONFIG) {
			CAM_WARN(CAM_SENSOR,
				"Rxed Update packets without linking");
			return 0;
		}

		cam_sensor_update_req_mgr(s_ctrl, csl_packet);
		return rc;
	}
	default:
		CAM_ERR(CAM_SENSOR, "Invalid Packet Header");
		return -EINVAL;
	}

	offset = (uint32_t *)&csl_packet->payload;
	offset += csl_packet->cmd_buf_offset / 4;
	cmd_desc = (struct cam_cmd_buf_desc *)(offset);

	rc = cam_sensor_i2c_command_parser(i2c_reg_settings, cmd_desc, 1);
	if (rc < 0) {
		CAM_ERR(CAM_SENSOR, "Fail parsing I2C Pkt: %d", rc);
		return rc;
	}

	if ((csl_packet->header.op_code & 0xFFFFFF) ==
		CAM_SENSOR_PACKET_OPCODE_SENSOR_UPDATE) {
		i2c_reg_settings->request_id =
			csl_packet->header.request_id;
		cam_sensor_update_req_mgr(s_ctrl, csl_packet);
	}

	return rc;
}

static int32_t cam_sensor_i2c_modes_util(
#if defined(CONFIG_CAMERA_DYNAMIC_MIPI)
	struct cam_sensor_ctrl_t *s_ctrl,
#endif
	struct camera_io_master *io_master_info,
	struct i2c_settings_list *i2c_list)
{
	int32_t rc = 0;
	uint32_t i, size;
#if defined(CONFIG_CAMERA_DYNAMIC_MIPI)
	const struct cam_mipi_sensor_mode *cur_mipi_sensor_mode;
	struct i2c_settings_list mipi_i2c_list;
#endif
#if defined(CONFIG_SENSOR_RETENTION)
	uint32_t read_value = 0xBEEF;

	if (s_ctrl->sensordata->slave_info.sensor_id == RETENTION_SENSOR_ID) {
		if (i2c_list->i2c_settings.reg_setting[0].reg_addr == RETENTION_SETTING_START_ADDR) {
			if (sensor_retention_mode == RETENTION_ON) {
				read_value = cam_sensor_retention_calc_checksum(io_master_info);
				if (cam_sensor_retention_compare_checksum(read_value) == TRUE) {
					CAM_INFO(CAM_SENSOR, "[RET_DBG] Retention wake! Skip to write initSetting(retention setting)");
					sensor_retention_mode = RETENTION_READY_TO_ON;
#if defined(CONFIG_CAMERA_FAC_LN_TEST)
					if (factory_ln_test == 1) {
						cam_sensor_ln_test_retention_write_global(io_master_info);
						return 0;
					}
#endif
					cam_sensor_retention_write_global(io_master_info);
					return 0;
				} else {
					CAM_INFO(CAM_SENSOR, "[RET_DBG] Fail to retention wake! Write initSetting(retention setting)");
					sensor_retention_mode = RETENTION_INIT;
				}
			}
			sensor_version = cam_sensor_read_revision(io_master_info);
			sensor_nvm = cam_sensor_read_nvm(io_master_info);
		}
	}
#endif

	if (i2c_list->op_code == CAM_SENSOR_I2C_WRITE_RANDOM) {
#if defined(CONFIG_CAMERA_DYNAMIC_MIPI)
		if (i2c_list->i2c_settings.reg_setting[0].reg_addr == STREAM_ON_ADDR
			&& s_ctrl->sensordata->slave_info.sensor_id == FRONT_SENSOR_ID_IMX320
			&& s_ctrl->mipi_clock_index_new != INVALID_MIPI_INDEX
			&& s_ctrl->i2c_data.streamon_settings.is_settings_valid) {
			pr_err("[dynamic_mipi] Write MIPI setting before Stream On setting. mipi_index : %d",
				s_ctrl->mipi_clock_index_new);

			cur_mipi_sensor_mode = &(s_ctrl->mipi_info[0]);
			memset(&mipi_i2c_list, 0, sizeof(mipi_i2c_list));

			mipi_i2c_list.i2c_settings.reg_setting =
				cur_mipi_sensor_mode->mipi_setting[s_ctrl->mipi_clock_index_new].clk_setting->reg_setting;
			mipi_i2c_list.i2c_settings.addr_type =
				cur_mipi_sensor_mode->mipi_setting[s_ctrl->mipi_clock_index_new].clk_setting->addr_type;
			mipi_i2c_list.i2c_settings.data_type =
				cur_mipi_sensor_mode->mipi_setting[s_ctrl->mipi_clock_index_new].clk_setting->data_type;
			mipi_i2c_list.i2c_settings.size =
				cur_mipi_sensor_mode->mipi_setting[s_ctrl->mipi_clock_index_new].clk_setting->size;
			mipi_i2c_list.i2c_settings.delay =
				cur_mipi_sensor_mode->mipi_setting[s_ctrl->mipi_clock_index_new].clk_setting->delay;

			pr_err("[dynamic_mipi] Picked MIPI clock : %s", cur_mipi_sensor_mode->mipi_setting[s_ctrl->mipi_clock_index_new].str_mipi_clk);

			rc = camera_io_dev_write(io_master_info,
				&(mipi_i2c_list.i2c_settings));
		}
#endif

		rc = camera_io_dev_write(io_master_info,
			&(i2c_list->i2c_settings));
		if (rc < 0) {
			CAM_ERR(CAM_SENSOR,
				"Failed to random write I2C settings: %d",
				rc);
			return rc;
		}

		if (i2c_list->i2c_settings.reg_setting[0].reg_addr == STREAM_ON_ADDR
			&& i2c_list->i2c_settings.reg_setting[0].reg_data == 0x0
			&& s_ctrl->sensordata->slave_info.sensor_id == RETENTION_SENSOR_ID) {
			uint32_t frame_cnt = 0;
			int retry_cnt = 10;
			rc = camera_io_dev_read(&s_ctrl->io_master_info, 0x0005,
				&frame_cnt, CAMERA_SENSOR_I2C_TYPE_WORD, CAMERA_SENSOR_I2C_TYPE_BYTE);
			CAM_INFO(CAM_SENSOR, "[SENSOR_DBG] Stream off, frame_cnt : %x", frame_cnt);

			while (frame_cnt != 0xFF && retry_cnt > 0) {
				usleep_range(2000, 3000);
				rc = camera_io_dev_read(&s_ctrl->io_master_info, 0x0005,
					&frame_cnt, CAMERA_SENSOR_I2C_TYPE_WORD, CAMERA_SENSOR_I2C_TYPE_BYTE);
				CAM_INFO(CAM_SENSOR, "[SENSOR_DBG] retry cnt : %d, Stream off, frame_cnt : %x", retry_cnt, frame_cnt);
				retry_cnt--;
			}
		} else if (i2c_list->i2c_settings.reg_setting[0].reg_addr == STREAM_ON_ADDR
			&& i2c_list->i2c_settings.reg_setting[0].reg_data == 0x1
			&& s_ctrl->sensordata->slave_info.sensor_id == RETENTION_SENSOR_ID) {
			CAM_INFO(CAM_SENSOR, "[SENSOR_DBG] Stream on");
		}
	} else if (i2c_list->op_code == CAM_SENSOR_I2C_WRITE_SEQ) {
		rc = camera_io_dev_write_continuous(
			io_master_info,
			&(i2c_list->i2c_settings),
			0);
		if (rc < 0) {
			CAM_ERR(CAM_SENSOR,
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
			CAM_ERR(CAM_SENSOR,
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
			i2c_list->i2c_settings.addr_type,
				i2c_list->i2c_settings.data_type,
			i2c_list->i2c_settings.
				reg_setting[i].delay);
			if (rc < 0) {
				CAM_ERR(CAM_SENSOR,
					"i2c poll apply setting Fail: %d", rc);
				return rc;
			}
		}
	}

#if defined(CONFIG_SENSOR_RETENTION)
	if (s_ctrl->sensordata->slave_info.sensor_id == RETENTION_SENSOR_ID) {
		if (i2c_list->i2c_settings.reg_setting[0].reg_addr == RETENTION_SETTING_START_ADDR) {
			if (sensor_retention_mode == RETENTION_INIT) {
				sensor_retention_checksum_value = cam_sensor_retention_calc_checksum(io_master_info);
				if (sensor_retention_checksum_value != 0) {
					sensor_retention_mode = RETENTION_READY_TO_ON;
					CAM_INFO(CAM_SENSOR, "[RET_DBG] Save checksum value: 0x%x",
						sensor_retention_checksum_value);
				} else {
					sensor_retention_mode = RETENTION_INIT;
					CAM_INFO(CAM_SENSOR, "[RET_DBG] Fail to calc checksum");
				}
#if defined(CONFIG_CAMERA_FAC_LN_TEST)
				if (factory_ln_test == 1) {
					CAM_WARN(CAM_SENSOR, "[LN_TEST] Not in retention mode");
				}
#endif
			}
		}
#if defined(CONFIG_CAMERA_FAC_LN_TEST)
		else if (i2c_list->i2c_settings.reg_setting[0].reg_addr == AE_GROUP_START_ADDR) {
			if (factory_ln_test == 1) {
				cam_sensor_ln_test_write_grouphold(io_master_info);
			}
		}
#endif
	}
#endif

	return rc;
}

int32_t cam_sensor_update_i2c_info(struct cam_cmd_i2c_info *i2c_info,
	struct cam_sensor_ctrl_t *s_ctrl)
{
	int32_t rc = 0;
	struct cam_sensor_cci_client   *cci_client = NULL;

	if (s_ctrl->io_master_info.master_type == CCI_MASTER) {
		cci_client = s_ctrl->io_master_info.cci_client;
		if (!cci_client) {
			CAM_ERR(CAM_SENSOR, "failed: cci_client %pK",
				cci_client);
			return -EINVAL;
		}
		cci_client->cci_i2c_master = s_ctrl->cci_i2c_master;
		cci_client->sid = i2c_info->slave_addr >> 1;
		cci_client->retries = 3;
		cci_client->id_map = 0;
		cci_client->i2c_freq_mode = i2c_info->i2c_freq_mode;
		CAM_DBG(CAM_SENSOR, " Master: %d sid: %d freq_mode: %d",
			cci_client->cci_i2c_master, i2c_info->slave_addr,
			i2c_info->i2c_freq_mode);
	}

	s_ctrl->sensordata->slave_info.sensor_slave_addr =
		i2c_info->slave_addr;
	return rc;
}

int32_t cam_sensor_update_slave_info(struct cam_cmd_probe *probe_info,
	struct cam_sensor_ctrl_t *s_ctrl)
{
	int32_t rc = 0;

	s_ctrl->sensordata->slave_info.sensor_id_reg_addr =
		probe_info->reg_addr;
	s_ctrl->sensordata->slave_info.sensor_id =
		probe_info->expected_data;
	s_ctrl->sensordata->slave_info.sensor_id_mask =
		probe_info->data_mask;

	s_ctrl->sensor_probe_addr_type =  probe_info->addr_type;
	s_ctrl->sensor_probe_data_type =  probe_info->data_type;
	CAM_DBG(CAM_SENSOR,
		"Sensor Addr: 0x%x sensor_id: 0x%x sensor_mask: 0x%x",
		s_ctrl->sensordata->slave_info.sensor_id_reg_addr,
		s_ctrl->sensordata->slave_info.sensor_id,
		s_ctrl->sensordata->slave_info.sensor_id_mask);
	return rc;
}

int32_t cam_handle_cmd_buffers_for_probe(void *cmd_buf,
	struct cam_sensor_ctrl_t *s_ctrl,
	int32_t cmd_buf_num, int cmd_buf_length)
{
	int32_t rc = 0;

	switch (cmd_buf_num) {
	case 0: {
		struct cam_cmd_i2c_info *i2c_info = NULL;
		struct cam_cmd_probe *probe_info;

		i2c_info = (struct cam_cmd_i2c_info *)cmd_buf;
		rc = cam_sensor_update_i2c_info(i2c_info, s_ctrl);
		if (rc < 0) {
			CAM_ERR(CAM_SENSOR, "Failed in Updating the i2c Info");
			return rc;
		}
		probe_info = (struct cam_cmd_probe *)
			(cmd_buf + sizeof(struct cam_cmd_i2c_info));
		rc = cam_sensor_update_slave_info(probe_info, s_ctrl);
		if (rc < 0) {
			CAM_ERR(CAM_SENSOR, "Updating the slave Info");
			return rc;
		}
		cmd_buf = probe_info;
	}
		break;
	case 1: {
		rc = cam_sensor_update_power_settings(cmd_buf,
			cmd_buf_length, &s_ctrl->sensordata->power_info);
		if (rc < 0) {
			CAM_ERR(CAM_SENSOR,
				"Failed in updating power settings");
			return rc;
		}
	}
		break;
	default:
		CAM_ERR(CAM_SENSOR, "Invalid command buffer");
		break;
	}
	return rc;
}

int32_t cam_handle_mem_ptr(uint64_t handle, struct cam_sensor_ctrl_t *s_ctrl)
{
	int rc = 0, i;
	void *packet = NULL, *cmd_buf1 = NULL;
	uint32_t *cmd_buf;
	void *ptr;
	size_t len;
	struct cam_packet *pkt;
	struct cam_cmd_buf_desc *cmd_desc;

	rc = cam_mem_get_cpu_buf(handle,
		(uint64_t *)&packet, &len);
	if (rc < 0) {
		CAM_ERR(CAM_SENSOR, "Failed to get the command Buffer");
		return -EINVAL;
	}
	pkt = (struct cam_packet *)packet;
	cmd_desc = (struct cam_cmd_buf_desc *)
		((uint32_t *)&pkt->payload + pkt->cmd_buf_offset/4);
	if (cmd_desc == NULL) {
		CAM_ERR(CAM_SENSOR, "command descriptor pos is invalid");
		return -EINVAL;
	}
	if (pkt->num_cmd_buf != 2) {
		CAM_ERR(CAM_SENSOR, "Expected More Command Buffers : %d",
			 pkt->num_cmd_buf);
		return -EINVAL;
	}
	for (i = 0; i < pkt->num_cmd_buf; i++) {
		if (!(cmd_desc[i].length))
			continue;
		rc = cam_mem_get_cpu_buf(cmd_desc[i].mem_handle,
			(uint64_t *)&cmd_buf1, &len);
		if (rc < 0) {
			CAM_ERR(CAM_SENSOR,
				"Failed to parse the command Buffer Header");
			return -EINVAL;
		}
		cmd_buf = (uint32_t *)cmd_buf1;
		cmd_buf += cmd_desc[i].offset/4;
		ptr = (void *) cmd_buf;

		rc = cam_handle_cmd_buffers_for_probe(ptr, s_ctrl,
			i, cmd_desc[i].length);
		if (rc < 0) {
			CAM_ERR(CAM_SENSOR,
				"Failed to parse the command Buffer Header");
			return -EINVAL;
		}
	}
	return rc;
}

void cam_sensor_query_cap(struct cam_sensor_ctrl_t *s_ctrl,
	struct  cam_sensor_query_cap *query_cap)
{
	query_cap->pos_roll = s_ctrl->sensordata->pos_roll;
	query_cap->pos_pitch = s_ctrl->sensordata->pos_pitch;
	query_cap->pos_yaw = s_ctrl->sensordata->pos_yaw;
	query_cap->secure_camera = 0;
	query_cap->actuator_slot_id =
		s_ctrl->sensordata->subdev_id[SUB_MODULE_ACTUATOR];
	query_cap->csiphy_slot_id =
		s_ctrl->sensordata->subdev_id[SUB_MODULE_CSIPHY];
	query_cap->eeprom_slot_id =
		s_ctrl->sensordata->subdev_id[SUB_MODULE_EEPROM];
	query_cap->flash_slot_id =
		s_ctrl->sensordata->subdev_id[SUB_MODULE_LED_FLASH];
	query_cap->ois_slot_id =
		s_ctrl->sensordata->subdev_id[SUB_MODULE_OIS];
	query_cap->aperture_slot_id =
		s_ctrl->sensordata->subdev_id[SUB_MODULE_APERTURE];
	query_cap->slot_info =
		s_ctrl->soc_info.index;
}

static uint16_t cam_sensor_id_by_mask(struct cam_sensor_ctrl_t *s_ctrl,
	uint32_t chipid)
{
	uint16_t sensor_id = (uint16_t)(chipid & 0xFFFF);
	int16_t sensor_id_mask = s_ctrl->sensordata->slave_info.sensor_id_mask;

	if (!sensor_id_mask)
		sensor_id_mask = ~sensor_id_mask;

	sensor_id &= sensor_id_mask;
	sensor_id_mask &= -sensor_id_mask;
	sensor_id_mask -= 1;
	while (sensor_id_mask) {
		sensor_id_mask >>= 1;
		sensor_id >>= 1;
	}
	return sensor_id;
}

void cam_sensor_shutdown(struct cam_sensor_ctrl_t *s_ctrl)
{
	struct cam_sensor_power_ctrl_t *power_info =
		&s_ctrl->sensordata->power_info;
	int rc = 0;

	s_ctrl->is_probe_succeed = 0;
	if (s_ctrl->sensor_state == CAM_SENSOR_INIT)
		return;

	cam_sensor_release_resource(s_ctrl);
	cam_sensor_release_stream_rsc(s_ctrl);

	if (s_ctrl->sensor_state >= CAM_SENSOR_ACQUIRE)
		cam_sensor_power_down(s_ctrl);

	rc = cam_destroy_device_hdl(s_ctrl->bridge_intf.device_hdl);
	if (rc < 0)
		CAM_ERR(CAM_SENSOR, " failed destroying dhdl");
	s_ctrl->bridge_intf.device_hdl = -1;
	s_ctrl->bridge_intf.link_hdl = -1;
	s_ctrl->bridge_intf.session_hdl = -1;

	kfree(power_info->power_setting);
	kfree(power_info->power_down_setting);
	s_ctrl->streamon_count = 0;
	s_ctrl->streamoff_count = 0;

	s_ctrl->sensor_state = CAM_SENSOR_INIT;
}

int cam_sensor_match_id(struct cam_sensor_ctrl_t *s_ctrl)
{
	int rc = 0;
	uint32_t chipid = 0;
	struct cam_camera_slave_info *slave_info;

	slave_info = &(s_ctrl->sensordata->slave_info);

	if (!slave_info) {
		CAM_ERR(CAM_SENSOR, "failed: %pK",
			 slave_info);
		return -EINVAL;
	}

	rc = camera_io_dev_read(
		&(s_ctrl->io_master_info),
		slave_info->sensor_id_reg_addr,
		&chipid, CAMERA_SENSOR_I2C_TYPE_WORD,
		CAMERA_SENSOR_I2C_TYPE_WORD);
	if (rc < 0) {
		CAM_ERR(CAM_SENSOR, "dev_read failed rc %d\n", rc);
	}

	CAM_DBG(CAM_SENSOR, "read id: 0x%x expected id 0x%x:",
			 chipid, slave_info->sensor_id);
	if (cam_sensor_id_by_mask(s_ctrl, chipid) != slave_info->sensor_id) {
		CAM_ERR(CAM_SENSOR, "chip id %x does not match %x",
				chipid, slave_info->sensor_id);
		return -ENODEV;
	}
	return rc;
}

int32_t cam_sensor_driver_cmd(struct cam_sensor_ctrl_t *s_ctrl,
	void *arg)
{
	int rc = 0;
	struct cam_control *cmd = (struct cam_control *)arg;
	struct cam_sensor_power_setting *pu = NULL;
	struct cam_sensor_power_setting *pd = NULL;
	struct cam_sensor_power_ctrl_t *power_info =
		&s_ctrl->sensordata->power_info;
#if defined(CONFIG_USE_CAMERA_HW_BIG_DATA)
	struct cam_hw_param *hw_param = NULL;
#endif

	if (!s_ctrl || !arg) {
		CAM_ERR(CAM_SENSOR, "s_ctrl is NULL");
		return -EINVAL;
	}

	mutex_lock(&(s_ctrl->cam_sensor_mutex));
	switch (cmd->op_code) {
	case CAM_SENSOR_PROBE_CMD: {
		int i, j;
		struct cam_hw_soc_info *soc_info;

		if (s_ctrl->is_probe_succeed == 1) {
			CAM_ERR(CAM_SENSOR,
				"Already Sensor Probed in the slot");
			break;
		}
		/* Allocate memory for power up setting */
		pu = kzalloc(sizeof(struct cam_sensor_power_setting) *
			MAX_POWER_CONFIG, GFP_KERNEL);
		if (!pu) {
			rc = -ENOMEM;
			goto release_mutex;
		}

		pd = kzalloc(sizeof(struct cam_sensor_power_setting) *
			MAX_POWER_CONFIG, GFP_KERNEL);
		if (!pd) {
			kfree(pu);
			rc = -ENOMEM;
			goto release_mutex;
		}

		power_info->power_setting = pu;
		power_info->power_down_setting = pd;

#if defined(CONFIG_USE_CAMERA_HW_BIG_DATA)
		sec_sensor_position = s_ctrl->id;
#endif

#if defined(CONFIG_CAMERA_DYNAMIC_MIPI)
		cam_mipi_init_setting(s_ctrl);
#endif
		if (cmd->handle_type ==
			CAM_HANDLE_MEM_HANDLE) {
			rc = cam_handle_mem_ptr(cmd->handle, s_ctrl);
			if (rc < 0) {
				CAM_ERR(CAM_SENSOR, "Get Buffer Handle Failed");
				kfree(pu);
				kfree(pd);
				goto release_mutex;
			}
		} else {
			CAM_ERR(CAM_SENSOR, "Invalid Command Type: %d",
				 cmd->handle_type);
		}

		/* Parse and fill vreg params for powerup settings */
		rc = msm_camera_fill_vreg_params(
			&s_ctrl->soc_info,
			s_ctrl->sensordata->power_info.power_setting,
			s_ctrl->sensordata->power_info.power_setting_size);
		if (rc < 0) {
			CAM_ERR(CAM_SENSOR,
				"Fail in filling vreg params for PUP rc %d",
				 rc);
			kfree(pu);
			kfree(pd);
			goto release_mutex;
		}

		/* Parse and fill vreg params for powerdown settings*/
		rc = msm_camera_fill_vreg_params(
			&s_ctrl->soc_info,
			s_ctrl->sensordata->power_info.power_down_setting,
			s_ctrl->sensordata->power_info.power_down_setting_size);
		if (rc < 0) {
			CAM_ERR(CAM_SENSOR,
				"Fail in filling vreg params for PDOWN rc %d",
				 rc);
			kfree(pu);
			kfree(pd);
			goto release_mutex;
		}

		/* Power up and probe sensor */
		rc = cam_sensor_power_up(s_ctrl);
		if (rc < 0) {
			CAM_ERR(CAM_SENSOR, "power up failed");
			cam_sensor_power_down(s_ctrl);
			kfree(pu);
			kfree(pd);
			goto release_mutex;
		}

		/* Match sensor ID */
		for (i = 0; i < 3; i++) {
			rc = cam_sensor_match_id(s_ctrl);
			if (rc == -ENODEV) {
				CAM_ERR(CAM_SENSOR, "Retrying again for sensor: 0x%x retry cnt: %d",
					s_ctrl->sensordata->slave_info.sensor_id, i);
				soc_info = &s_ctrl->soc_info;
				for (j = 0 ; j < soc_info->num_rgltr; j++) {
					if (soc_info->rgltr[j]) {
						CAM_INFO(CAM_SENSOR, "Regulator Name: %s Is_Enabled: %d get_voltage: %d",
							soc_info->rgltr_name[j], regulator_is_enabled(soc_info->rgltr[j]),
							regulator_get_voltage(soc_info->rgltr[j]));
					}
				}
				for (j = 0 ; j < soc_info->num_clk; j++) {
					CAM_INFO(CAM_SENSOR, "Clock Name: %s get_clk: %ld",
						soc_info->clk_name[j],
						clk_get_rate(soc_info->clk[j]));
				}
			}
			else
				break;
		}
#if 1 //For factory module test
		if (rc < 0) {
			CAM_ERR(CAM_SENSOR, "need to check sensor module : 0x%x",
				s_ctrl->sensordata->slave_info.sensor_id);
#if defined(CONFIG_USE_CAMERA_HW_BIG_DATA)
			if (rc < 0) {
				CAM_ERR(CAM_HWB, "failed rc %d\n", rc);
				if (s_ctrl != NULL) {
					switch (s_ctrl->id) {
					case CAMERA_0:
						if (!msm_is_sec_get_rear_hw_param(&hw_param)) {
							if (hw_param != NULL) {
								CAM_ERR(CAM_HWB, "[R][I2C] Err\n");
								hw_param->i2c_sensor_err_cnt++;
								hw_param->need_update_to_file = TRUE;
							}
						}
						break;

					case CAMERA_1:
						if (!msm_is_sec_get_front_hw_param(&hw_param)) {
							if (hw_param != NULL) {
								CAM_ERR(CAM_HWB, "[F][I2C] Err\n");
								hw_param->i2c_sensor_err_cnt++;
								hw_param->need_update_to_file = TRUE;
							}
						}
						break;

#if defined(CONFIG_SAMSUNG_MULTI_CAMERA)
					case CAMERA_2:
						if (!msm_is_sec_get_rear2_hw_param(&hw_param)) {
							if (hw_param != NULL) {
								CAM_ERR(CAM_HWB, "[R2][I2C] Err\n");
								hw_param->i2c_sensor_err_cnt++;
								hw_param->need_update_to_file = TRUE;
							}
						}
						break;
#endif

#if defined(CONFIG_SAMSUNG_SECURE_CAMERA)
					case CAMERA_3:
						if (!msm_is_sec_get_iris_hw_param(&hw_param)) {
							if (hw_param != NULL) {
								CAM_ERR(CAM_HWB, "[I][I2C] Err\n");
								hw_param->i2c_sensor_err_cnt++;
								hw_param->need_update_to_file = TRUE;
							}
						}
						break;
#endif

					default:
						CAM_ERR(CAM_HWB, "[NON][I2C] Unsupport\n");
						break;
					}
				}
			}
#endif
		}
#else
		if (rc < 0) {
			cam_sensor_power_down(s_ctrl);
			msleep(20);
			kfree(pu);
			kfree(pd);
			goto release_mutex;
		}
#endif

#if defined(CONFIG_SAMSUNG_SECURE_CAMERA)
		if (s_ctrl->id == 3)
			cam_sensor_check_resolution(s_ctrl);
#endif
		CAM_INFO(CAM_SENSOR,
			"Probe Succees,slot:%d,slave_addr:0x%x,sensor_id:0x%x",
			s_ctrl->soc_info.index,
			s_ctrl->sensordata->slave_info.sensor_slave_addr,
			s_ctrl->sensordata->slave_info.sensor_id);

		rc = cam_sensor_power_down(s_ctrl);
		if (rc < 0) {
			CAM_ERR(CAM_SENSOR, "fail in Sensor Power Down");
			kfree(pu);
			kfree(pd);
			goto release_mutex;
		}
		/*
		 * Set probe succeeded flag to 1 so that no other camera shall
		 * probed on this slot
		 */
		s_ctrl->is_probe_succeed = 1;
		s_ctrl->sensor_state = CAM_SENSOR_INIT;

#if defined(CONFIG_CAMERA_FAC_LN_TEST)
		factory_ln_test = 0;
#endif
	}
		break;
	case CAM_ACQUIRE_DEV: {
		struct cam_sensor_acquire_dev sensor_acq_dev;
		struct cam_create_dev_hdl bridge_params;
		int i, j;
		struct cam_hw_soc_info *soc_info;

		if (s_ctrl->bridge_intf.device_hdl != -1) {
			CAM_ERR(CAM_SENSOR, "Device is already acquired");
			rc = -EINVAL;
			goto release_mutex;
		}
		rc = copy_from_user(&sensor_acq_dev,
			(void __user *) cmd->handle, sizeof(sensor_acq_dev));
		if (rc < 0) {
			CAM_ERR(CAM_SENSOR, "Failed Copying from user");
			goto release_mutex;
		}
		CAM_INFO(CAM_SENSOR, "CAM_ACQUIRE_DEV : sensor_id : 0x%x",
			s_ctrl->sensordata->slave_info.sensor_id);

		bridge_params.session_hdl = sensor_acq_dev.session_handle;
		bridge_params.ops = &s_ctrl->bridge_intf.ops;
		bridge_params.v4l2_sub_dev_flag = 0;
		bridge_params.media_entity_flag = 0;
		bridge_params.priv = s_ctrl;

		sensor_acq_dev.device_handle =
			cam_create_device_hdl(&bridge_params);
		s_ctrl->bridge_intf.device_hdl = sensor_acq_dev.device_handle;
		s_ctrl->bridge_intf.session_hdl = sensor_acq_dev.session_handle;

		g_s_ctrl = s_ctrl;

#if defined(CONFIG_USE_CAMERA_HW_BIG_DATA)
		if (sec_sensor_position < g_s_ctrl->id) {
			sec_sensor_position = g_s_ctrl->id;
			CAM_ERR(CAM_SENSOR, "sensor_position: %d", sec_sensor_position);
		}
#endif

#if defined(CONFIG_CAMERA_FRS_DRAM_TEST)
		rear_frs_test_mode = 0;
		frs_output_done_flag = 0;
#endif

		CAM_DBG(CAM_SENSOR, "Device Handle: %d",
			sensor_acq_dev.device_handle);
		if (copy_to_user((void __user *) cmd->handle, &sensor_acq_dev,
			sizeof(struct cam_sensor_acquire_dev))) {
			CAM_ERR(CAM_SENSOR, "Failed Copy to User");
			rc = -EFAULT;
			goto release_mutex;
		}

		rc = cam_sensor_power_up(s_ctrl);
		if (rc < 0) {
			CAM_ERR(CAM_SENSOR, "Sensor Power up failed");
			goto release_mutex;
		}
#if 1 //For factory module test
		for (i = 0; i < 3; i++) {
			rc = cam_sensor_match_id(s_ctrl);
			if (rc == -ENODEV) {
				CAM_ERR(CAM_SENSOR, "Retrying again for sensor: 0x%x retry cnt: %d",
					s_ctrl->sensordata->slave_info.sensor_id, i);
				soc_info = &s_ctrl->soc_info;
				for (j = 0 ; j < soc_info->num_rgltr; j++) {
					if (soc_info->rgltr[j]) {
						CAM_INFO(CAM_SENSOR, "Regulator Name: %s Is_Enabled: %d get_voltage: %d",
							soc_info->rgltr_name[j], regulator_is_enabled(soc_info->rgltr[j]),
							regulator_get_voltage(soc_info->rgltr[j]));
					}
				}
				for (j = 0 ; j < soc_info->num_clk; j++) {
					CAM_INFO(CAM_SENSOR, "Clock Name: %s get_clk: %ld",
						soc_info->clk_name[j],
						clk_get_rate(soc_info->clk[j]));
				}
				rc = 0;
				continue;
			} else if (rc < 0) {
				CAM_ERR(CAM_SENSOR, "need to check sensor module : 0x%x",
					s_ctrl->sensordata->slave_info.sensor_id);
				cam_sensor_power_down(s_ctrl);
				goto release_mutex;
			}
		}
#endif
		s_ctrl->sensor_state = CAM_SENSOR_ACQUIRE;
	}
		break;
	case CAM_RELEASE_DEV: {

		if ((s_ctrl->sensor_state < CAM_SENSOR_ACQUIRE) ||
         (s_ctrl->sensor_state > CAM_SENSOR_CONFIG)) {
			rc = -EINVAL;
			CAM_WARN(CAM_SENSOR,
			"Not in right state to release : %d",
			s_ctrl->sensor_state);
			goto release_mutex;
		}
		CAM_INFO(CAM_SENSOR, "CAM_RELEASE_DEV : sensor_id : 0x%x",
			s_ctrl->sensordata->slave_info.sensor_id);
		s_ctrl->streamon_count = 0;
		s_ctrl->streamoff_count = 0;

		rc = cam_sensor_power_down(s_ctrl);
		if (rc < 0) {
			CAM_ERR(CAM_SENSOR, "Sensor Power Down failed");
			goto release_mutex;
		}

		cam_sensor_release_resource(s_ctrl);
		cam_sensor_release_stream_rsc(s_ctrl);
		if (s_ctrl->bridge_intf.device_hdl == -1) {
			CAM_ERR(CAM_SENSOR,
				"Invalid Handles: link hdl: %d device hdl: %d",
				s_ctrl->bridge_intf.device_hdl,
				s_ctrl->bridge_intf.link_hdl);
			rc = -EINVAL;
			goto release_mutex;
		}
		rc = cam_destroy_device_hdl(s_ctrl->bridge_intf.device_hdl);
		if (rc < 0)
			CAM_ERR(CAM_SENSOR,
				"failed in destroying the device hdl");
		s_ctrl->bridge_intf.device_hdl = -1;
		s_ctrl->bridge_intf.link_hdl = -1;
		s_ctrl->bridge_intf.session_hdl = -1;

		s_ctrl->sensor_state = CAM_SENSOR_INIT;

		g_s_ctrl = NULL;

#if defined(CONFIG_CAMERA_FAC_LN_TEST)
		factory_ln_test = 0;
#endif
	}
		break;
	case CAM_QUERY_CAP: {
		struct  cam_sensor_query_cap sensor_cap;

		cam_sensor_query_cap(s_ctrl, &sensor_cap);
		if (copy_to_user((void __user *) cmd->handle, &sensor_cap,
			sizeof(struct  cam_sensor_query_cap))) {
			CAM_ERR(CAM_SENSOR, "Failed Copy to User");
			rc = -EFAULT;
			goto release_mutex;
		}
	}
		break;
	case CAM_START_DEV: {
		int i, j;
		struct cam_hw_soc_info *soc_info;

		if ((s_ctrl->sensor_state < CAM_SENSOR_ACQUIRE) ||
         (s_ctrl->sensor_state > CAM_SENSOR_CONFIG)) {
			rc = -EINVAL;
			CAM_WARN(CAM_SENSOR,
			"Not in right state to start : %d",
			s_ctrl->sensor_state);
			goto release_mutex;
		}

		if (s_ctrl->i2c_data.streamon_settings.is_settings_valid &&
			(s_ctrl->i2c_data.streamon_settings.request_id == 0)) {
			CAM_INFO(CAM_SENSOR, "CAM_START_DEV : sensor_id : 0x%x",
				s_ctrl->sensordata->slave_info.sensor_id);
#if 1 //For factory module test
			for (i = 0; i < 3; i++) {
				rc = cam_sensor_match_id(s_ctrl);
				if (rc == -ENODEV) {
					CAM_ERR(CAM_SENSOR, "Retrying again for sensor: 0x%x retry cnt: %d",
						s_ctrl->sensordata->slave_info.sensor_id, i);
									soc_info = &s_ctrl->soc_info;
					for (j = 0 ; j < soc_info->num_rgltr; j++) {
						if (soc_info->rgltr[j]) {
							CAM_ERR(CAM_SENSOR, "Regulator Name: %s Is_Enabled: %d get_voltage: %d",
								soc_info->rgltr_name[j], regulator_is_enabled(soc_info->rgltr[j]),
								regulator_get_voltage(soc_info->rgltr[j]));
						}
					}
					for (j = 0 ; j < soc_info->num_clk; j++) {
						CAM_INFO(CAM_SENSOR, "Clock Name: %s get_clk: %ld",
							soc_info->clk_name[j],
							clk_get_rate(soc_info->clk[j]));
					}
				} else
					break;
			}
			if (rc < 0) {
				CAM_ERR(CAM_SENSOR, "need to check sensor module : 0x%x",
					s_ctrl->sensordata->slave_info.sensor_id);
#if defined(CONFIG_USE_CAMERA_HW_BIG_DATA)
				if (rc < 0) {
					CAM_ERR(CAM_HWB, "failed rc %d\n", rc);
					if (s_ctrl != NULL) {
						switch (s_ctrl->id) {
						case CAMERA_0:
							if (!msm_is_sec_get_rear_hw_param(&hw_param)) {
								if (hw_param != NULL) {
									CAM_ERR(CAM_HWB, "[R][I2C] Err\n");
									hw_param->i2c_sensor_err_cnt++;
									hw_param->need_update_to_file = TRUE;
								}
							}
							break;

						case CAMERA_1:
							if (!msm_is_sec_get_front_hw_param(&hw_param)) {
								if (hw_param != NULL) {
									CAM_ERR(CAM_HWB, "[F][I2C] Err\n");
									hw_param->i2c_sensor_err_cnt++;
									hw_param->need_update_to_file = TRUE;
								}
							}
							break;

#if defined(CONFIG_SAMSUNG_MULTI_CAMERA)
						case CAMERA_2:
							if (!msm_is_sec_get_rear2_hw_param(&hw_param)) {
								if (hw_param != NULL) {
									CAM_ERR(CAM_HWB, "[R2][I2C] Err\n");
									hw_param->i2c_sensor_err_cnt++;
									hw_param->need_update_to_file = TRUE;
								}
							}
							break;
#endif

#if defined(CONFIG_SAMSUNG_SECURE_CAMERA)
						case CAMERA_3:
							if (!msm_is_sec_get_iris_hw_param(&hw_param)) {
								if (hw_param != NULL) {
									CAM_ERR(CAM_HWB, "[I][I2C] Err\n");
									hw_param->i2c_sensor_err_cnt++;
									hw_param->need_update_to_file = TRUE;
								}
							}
							break;
#endif

						default:
							CAM_ERR(CAM_HWB, "[NON][I2C] Unsupport\n");
							break;
						}
					}
				}
#endif
				goto release_mutex;
			}
#endif
#if defined(CONFIG_CAMERA_FRS_DRAM_TEST)
			CAM_INFO(CAM_SENSOR, "[FRS_DBG] FRS DRAM test mode: %d",
				rear_frs_test_mode);
#endif
#if defined(CONFIG_CAMERA_DYNAMIC_MIPI)
			cam_mipi_update_info(s_ctrl);
			cam_mipi_get_clock_string(s_ctrl);
#endif
			rc = cam_sensor_apply_settings(s_ctrl, 0,
				CAM_SENSOR_PACKET_OPCODE_SENSOR_STREAMON);
			if (rc < 0) {
				CAM_ERR(CAM_SENSOR,
					"cannot apply streamon settings");
				goto release_mutex;
			}


		}
		s_ctrl->sensor_state = CAM_SENSOR_START;
	}
		break;
	case CAM_STOP_DEV: {
		if (s_ctrl->sensor_state != CAM_SENSOR_START) {
			rc = -EINVAL;
			CAM_WARN(CAM_SENSOR,
			"Not in right state to stop : %d",
			s_ctrl->sensor_state);
			goto release_mutex;
		}

		if (s_ctrl->i2c_data.streamoff_settings.is_settings_valid &&
			(s_ctrl->i2c_data.streamoff_settings.request_id == 0)) {
			CAM_INFO(CAM_SENSOR, "CAM_STOP_DEV : sensor_id : 0x%x",
				s_ctrl->sensordata->slave_info.sensor_id);

			rc = cam_sensor_apply_settings(s_ctrl, 0,
				CAM_SENSOR_PACKET_OPCODE_SENSOR_STREAMOFF);
			if (rc < 0) {
				CAM_ERR(CAM_SENSOR,
                    "cannot apply streamoff settings");
			}
		}

		cam_sensor_release_resource(s_ctrl);
		s_ctrl->sensor_state = CAM_SENSOR_ACQUIRE;
	}
		break;
	case CAM_CONFIG_DEV: {
		rc = cam_sensor_i2c_pkt_parse(s_ctrl, arg);
		if (rc < 0) {
			CAM_ERR(CAM_SENSOR, "Failed CCI Config: %d", rc);
			goto release_mutex;
		}
		if (s_ctrl->i2c_data.init_settings.is_settings_valid &&
			(s_ctrl->i2c_data.init_settings.request_id == 0)) {

			rc = cam_sensor_apply_settings(s_ctrl, 0,
				CAM_SENSOR_PACKET_OPCODE_SENSOR_INITIAL_CONFIG);
			if (rc < 0) {
				CAM_ERR(CAM_SENSOR,
					"cannot apply init settings");
				goto release_mutex;
			}
			rc = delete_request(&s_ctrl->i2c_data.init_settings);
			if (rc < 0) {
				CAM_ERR(CAM_SENSOR,
					"Fail in deleting the Init settings");
				goto release_mutex;
			}
			s_ctrl->i2c_data.init_settings.request_id = -1;
		}

		if (s_ctrl->i2c_data.config_settings.is_settings_valid &&
			(s_ctrl->i2c_data.config_settings.request_id == 0)) {
			rc = cam_sensor_apply_settings(s_ctrl, 0,
				CAM_SENSOR_PACKET_OPCODE_SENSOR_CONFIG);
			if (rc < 0) {
				CAM_ERR(CAM_SENSOR,
					"cannot apply config settings");
				goto release_mutex;
			}

			rc = delete_request(&s_ctrl->i2c_data.config_settings);
			if (rc < 0) {
				CAM_ERR(CAM_SENSOR,
					"Fail in deleting the config settings");
				goto release_mutex;
			}
      s_ctrl->sensor_state = CAM_SENSOR_CONFIG;
      s_ctrl->i2c_data.config_settings.request_id = -1;
		}
	}
		break;
	default:
		CAM_ERR(CAM_SENSOR, "Invalid Opcode: %d", cmd->op_code);
		rc = -EINVAL;
		goto release_mutex;
	}

release_mutex:
	mutex_unlock(&(s_ctrl->cam_sensor_mutex));
	return rc;
}

int cam_sensor_publish_dev_info(struct cam_req_mgr_device_info *info)
{
	int rc = 0;

	if (!info)
		return -EINVAL;

	info->dev_id = CAM_REQ_MGR_DEVICE_SENSOR;
	strlcpy(info->name, CAM_SENSOR_NAME, sizeof(info->name));
	info->p_delay = 2;
	info->trigger = CAM_TRIGGER_POINT_SOF;

	return rc;
}

int cam_sensor_establish_link(struct cam_req_mgr_core_dev_link_setup *link)
{
	struct cam_sensor_ctrl_t *s_ctrl = NULL;

	if (!link)
		return -EINVAL;

	s_ctrl = (struct cam_sensor_ctrl_t *)
		cam_get_device_priv(link->dev_hdl);
	if (!s_ctrl) {
		CAM_ERR(CAM_SENSOR, "Device data is NULL");
		return -EINVAL;
	}
	if (link->link_enable) {
		s_ctrl->bridge_intf.link_hdl = link->link_hdl;
		s_ctrl->bridge_intf.crm_cb = link->crm_cb;
	} else {
		s_ctrl->bridge_intf.link_hdl = -1;
		s_ctrl->bridge_intf.crm_cb = NULL;
	}

	return 0;
}

int cam_sensor_power(struct v4l2_subdev *sd, int on)
{
	struct cam_sensor_ctrl_t *s_ctrl = v4l2_get_subdevdata(sd);

	mutex_lock(&(s_ctrl->cam_sensor_mutex));
	if (!on && s_ctrl->sensor_state == CAM_SENSOR_START) {
		cam_sensor_power_down(s_ctrl);
		s_ctrl->sensor_state = CAM_SENSOR_ACQUIRE;
	}
	mutex_unlock(&(s_ctrl->cam_sensor_mutex));

	return 0;
}

int cam_sensor_power_up(struct cam_sensor_ctrl_t *s_ctrl)
{
	int rc;
	struct cam_sensor_power_ctrl_t *power_info;
	struct cam_camera_slave_info *slave_info;
	struct cam_hw_soc_info *soc_info =
		&s_ctrl->soc_info;
#if defined(CONFIG_USE_CAMERA_HW_BIG_DATA)
	struct cam_hw_param *hw_param = NULL;
#endif

	if (!s_ctrl) {
		CAM_ERR(CAM_SENSOR, "failed: %pK", s_ctrl);
		return -EINVAL;
	}
	CAM_INFO(CAM_SENSOR, "sensor_id : 0x%x", s_ctrl->sensordata->slave_info.sensor_id);

#if defined(CONFIG_USE_CAMERA_HW_BIG_DATA)
	if (s_ctrl != NULL) {
		switch (s_ctrl->id) {
		case CAMERA_0:
			if (!msm_is_sec_get_rear_hw_param(&hw_param)) {
				if (hw_param != NULL) {
					CAM_DBG(CAM_HWB, "[R][INIT] Init\n");
					hw_param->i2c_chk = FALSE;
					hw_param->mipi_chk = FALSE;
					hw_param->need_update_to_file = FALSE;
					hw_param->comp_chk = FALSE;
				}
			}
			break;

		case CAMERA_1:
			if (!msm_is_sec_get_front_hw_param(&hw_param)) {
				if (hw_param != NULL) {
					CAM_DBG(CAM_HWB, "[F][INIT] Init\n");
					hw_param->i2c_chk = FALSE;
					hw_param->mipi_chk = FALSE;
					hw_param->need_update_to_file = FALSE;
					hw_param->comp_chk = FALSE;
				}
			}
			break;

#if defined(CONFIG_SAMSUNG_MULTI_CAMERA)
		case CAMERA_2:
			if (!msm_is_sec_get_rear2_hw_param(&hw_param)) {
				if (hw_param != NULL) {
					CAM_DBG(CAM_HWB, "[R2][INIT] Init\n");
					hw_param->i2c_chk = FALSE;
					hw_param->mipi_chk = FALSE;
					hw_param->need_update_to_file = FALSE;
					hw_param->comp_chk = FALSE;
				}
			}
			break;
#endif

#if defined(CONFIG_SAMSUNG_SECURE_CAMERA)
		case CAMERA_3:
			if (!msm_is_sec_get_iris_hw_param(&hw_param)) {
				if (hw_param != NULL) {
					CAM_DBG(CAM_HWB, "[I][INIT] Init\n");
					hw_param->i2c_chk = FALSE;
					hw_param->mipi_chk = FALSE;
					hw_param->need_update_to_file = FALSE;
					hw_param->comp_chk = FALSE;
				}
			}
			break;
#endif

		default:
			CAM_ERR(CAM_HWB, "[NON][INIT] Unsupport\n");
			break;
		}
	}
#endif

	power_info = &s_ctrl->sensordata->power_info;
	slave_info = &(s_ctrl->sensordata->slave_info);

	if (!power_info || !slave_info) {
		CAM_ERR(CAM_SENSOR, "failed: %pK %pK", power_info, slave_info);
		return -EINVAL;
	}

	rc = cam_sensor_core_power_up(power_info, soc_info);
	if (rc < 0) {
		CAM_ERR(CAM_SENSOR, "power up the core is failed:%d", rc);
		return rc;
	}

	rc = camera_io_init(&(s_ctrl->io_master_info));
	if (rc < 0)
		CAM_ERR(CAM_SENSOR, "cci_init failed: rc: %d", rc);

	return rc;
}

int cam_sensor_power_down(struct cam_sensor_ctrl_t *s_ctrl)
{
	struct cam_sensor_power_ctrl_t *power_info;
	struct cam_hw_soc_info *soc_info;
	int rc = 0;
#if defined(CONFIG_USE_CAMERA_HW_BIG_DATA)
	struct cam_hw_param *hw_param = NULL;
#endif

	if (!s_ctrl) {
		CAM_ERR(CAM_SENSOR, "failed: s_ctrl %pK", s_ctrl);
		return -EINVAL;
	}

	CAM_INFO(CAM_SENSOR, "sensor_id : 0x%x", s_ctrl->sensordata->slave_info.sensor_id);

#if defined(CONFIG_USE_CAMERA_HW_BIG_DATA)
	if (s_ctrl != NULL) {
		switch (s_ctrl->id) {
		case CAMERA_0:
			if (!msm_is_sec_get_rear_hw_param(&hw_param)) {
				if (hw_param != NULL) {
					hw_param->i2c_chk = FALSE;
					hw_param->mipi_chk = FALSE;
					hw_param->comp_chk = FALSE;

					if (hw_param->need_update_to_file) {
						CAM_DBG(CAM_HWB, "[R][DEINIT] Update\n");
						msm_is_sec_copy_err_cnt_to_file();
					}
					hw_param->need_update_to_file = FALSE;
				}
			}
			break;

		case CAMERA_1:
			if (!msm_is_sec_get_front_hw_param(&hw_param)) {
				if (hw_param != NULL) {
					hw_param->i2c_chk = FALSE;
					hw_param->mipi_chk = FALSE;
					hw_param->comp_chk = FALSE;

					if (hw_param->need_update_to_file) {
						CAM_DBG(CAM_HWB, "[F][DEINIT] Update\n");
						msm_is_sec_copy_err_cnt_to_file();
					}
					hw_param->need_update_to_file = FALSE;
				}
			}
			break;

#if defined(CONFIG_SAMSUNG_MULTI_CAMERA)
		case CAMERA_2:
			if (!msm_is_sec_get_rear2_hw_param(&hw_param)) {
				if (hw_param != NULL) {
					hw_param->i2c_chk = FALSE;
					hw_param->mipi_chk = FALSE;
					hw_param->comp_chk = FALSE;

					if (hw_param->need_update_to_file) {
						CAM_DBG(CAM_HWB, "[R2][DEINIT] Update\n");
						msm_is_sec_copy_err_cnt_to_file();
					}
					hw_param->need_update_to_file = FALSE;
				}
			}
			break;
#endif

#if defined(CONFIG_SAMSUNG_SECURE_CAMERA)
		case CAMERA_3:
			if (!msm_is_sec_get_iris_hw_param(&hw_param)) {
				if (hw_param != NULL) {
					hw_param->i2c_chk = FALSE;
					hw_param->mipi_chk = FALSE;
					hw_param->comp_chk = FALSE;

					if (hw_param->need_update_to_file) {
						CAM_DBG(CAM_HWB, "[I][DEINIT] Update\n");
						msm_is_sec_copy_err_cnt_to_file();
					}
					hw_param->need_update_to_file = FALSE;
				}
			}
			break;
#endif

		default:
			CAM_ERR(CAM_HWB, "[NON][DEINIT] Unsupport\n");
			break;
		}
	}
#endif

	power_info = &s_ctrl->sensordata->power_info;
	soc_info = &s_ctrl->soc_info;

	if (!power_info) {
		CAM_ERR(CAM_SENSOR, "failed: power_info %pK", power_info);
		return -EINVAL;
	}

#if defined(CONFIG_SENSOR_RETENTION)
	if (s_ctrl->sensordata->slave_info.sensor_id == RETENTION_SENSOR_ID) {
		rc = msm_camera_power_down(power_info, soc_info, 1);
	} else {
		rc = msm_camera_power_down(power_info, soc_info, 0);
	}
#else
	rc = msm_camera_power_down(power_info, soc_info, 0);
#endif
	if (rc < 0) {
		CAM_ERR(CAM_SENSOR, "power down the core is failed:%d", rc);
		return rc;
	}

	camera_io_release(&(s_ctrl->io_master_info));

	return rc;
}

int cam_sensor_apply_settings(struct cam_sensor_ctrl_t *s_ctrl,
	int64_t req_id, enum cam_sensor_packet_opcodes opcode)
{
	int rc = 0, offset, i;
	uint64_t top = 0, del_req_id = 0;
	struct i2c_settings_array *i2c_set = NULL;
	struct i2c_settings_list *i2c_list;

	if (req_id == 0) {
		switch (opcode) {
		case CAM_SENSOR_PACKET_OPCODE_SENSOR_STREAMON: {
			i2c_set = &s_ctrl->i2c_data.streamon_settings;
			break;
		}
		case CAM_SENSOR_PACKET_OPCODE_SENSOR_INITIAL_CONFIG: {
			i2c_set = &s_ctrl->i2c_data.init_settings;
			break;
		}
		case CAM_SENSOR_PACKET_OPCODE_SENSOR_CONFIG: {
			i2c_set = &s_ctrl->i2c_data.config_settings;
			break;
		}
		case CAM_SENSOR_PACKET_OPCODE_SENSOR_STREAMOFF: {
			i2c_set = &s_ctrl->i2c_data.streamoff_settings;
			break;
		}
		case CAM_SENSOR_PACKET_OPCODE_SENSOR_UPDATE:
		case CAM_SENSOR_PACKET_OPCODE_SENSOR_PROBE:
		default:
			return 0;
		}
		if (i2c_set->is_settings_valid == 1) {
			list_for_each_entry(i2c_list,
				&(i2c_set->list_head), list) {
#if defined(CONFIG_CAMERA_DYNAMIC_MIPI)
				rc = cam_sensor_i2c_modes_util(s_ctrl,
					&(s_ctrl->io_master_info),
					i2c_list);
#else
				rc = cam_sensor_i2c_modes_util(
					&(s_ctrl->io_master_info),
					i2c_list);
#endif
				if (rc < 0) {
					CAM_ERR(CAM_SENSOR,
						"Failed to apply settings: %d",
						rc);
					return rc;
				}
			}
			//i2c_set->is_settings_valid = 0;
		}
	} else {
		offset = req_id % MAX_PER_FRAME_ARRAY;
		CAM_DBG(CAM_SENSOR, "Rxed Request ID %lld with offset %d",
			req_id, offset);
		i2c_set = &(s_ctrl->i2c_data.per_frame[offset]);
		if (i2c_set->is_settings_valid == 1 &&
			i2c_set->request_id == req_id) {
			list_for_each_entry(i2c_list,
				&(i2c_set->list_head), list) {
#if defined(CONFIG_CAMERA_DYNAMIC_MIPI)
				rc = cam_sensor_i2c_modes_util(s_ctrl,
					&(s_ctrl->io_master_info),
					i2c_list);
#else
				rc = cam_sensor_i2c_modes_util(
					&(s_ctrl->io_master_info),
					i2c_list);
#endif
				if (rc < 0) {
					CAM_ERR(CAM_SENSOR,
						"Failed to apply settings: %d",
						rc);
					return rc;
				}
			}
		} else {
			CAM_DBG(CAM_SENSOR,
				"Invalid/NOP request to apply: %lld with offset %d",
				req_id, offset);
		}

		/* Change the logic dynamically */
		for (i = 0; i < MAX_PER_FRAME_ARRAY; i++) {
			if (req_id >= s_ctrl->i2c_data.per_frame[i].request_id &&
				s_ctrl->i2c_data.per_frame[i].is_settings_valid
				== 1) {
				if (top <
					s_ctrl->i2c_data.per_frame[i].request_id) {
					del_req_id = top;
					top = s_ctrl->i2c_data.per_frame[i].request_id;
				} else if (top >
					s_ctrl->i2c_data.per_frame[i].request_id &&
					del_req_id <
					s_ctrl->i2c_data.per_frame[i].request_id) {
					del_req_id =
						s_ctrl->i2c_data.per_frame[i].request_id;
				}

			}
		}

		if (top < req_id) {
			if (abs((top % MAX_PER_FRAME_ARRAY) - (req_id %
				MAX_PER_FRAME_ARRAY)) >= 10) //Max batch mode
				del_req_id = req_id; // VR:: Just to delete the same request too
		}

		if (!del_req_id)
			return rc;

		CAM_DBG(CAM_SENSOR, "top: %llu, del_req_id:%llu",
			top, del_req_id);

		for (i = 0; i < MAX_PER_FRAME_ARRAY; i++) {
			if ((del_req_id >
				 s_ctrl->i2c_data.per_frame[i].request_id) &&
				(s_ctrl->i2c_data.per_frame[i].
					is_settings_valid == 1)) {
				CAM_DBG(CAM_SENSOR, "Deleting Request[%d] %llu",
					i, s_ctrl->i2c_data.per_frame[i].request_id);
				s_ctrl->i2c_data.per_frame[i].request_id = 0;
				rc = delete_request(
					&(s_ctrl->i2c_data.per_frame[i]));
				if (rc < 0)
					CAM_ERR(CAM_SENSOR,
						"Delete request Fail:%lld rc:%d",
						del_req_id, rc);
			}
		}
	}

	return rc;
}

int32_t cam_sensor_apply_request(struct cam_req_mgr_apply_request *apply)
{
	int32_t rc = 0;
	struct cam_sensor_ctrl_t *s_ctrl = NULL;

	if (!apply)
		return -EINVAL;

	s_ctrl = (struct cam_sensor_ctrl_t *)
		cam_get_device_priv(apply->dev_hdl);
	if (!s_ctrl) {
		CAM_ERR(CAM_SENSOR, "Device data is NULL");
		return -EINVAL;
	}
	CAM_DBG(CAM_SENSOR, " Req Id: %lld", apply->request_id);
	trace_cam_apply_req("Sensor", apply->request_id);
	mutex_lock(&(s_ctrl->cam_sensor_mutex));
	rc = cam_sensor_apply_settings(s_ctrl, apply->request_id,
		CAM_SENSOR_PACKET_OPCODE_SENSOR_UPDATE);
	mutex_unlock(&(s_ctrl->cam_sensor_mutex));
	return rc;
}

int32_t cam_sensor_flush_request(struct cam_req_mgr_flush_request *flush_req)
{
	int32_t rc = 0, i;
	uint32_t cancel_req_id_found = 0;
	struct cam_sensor_ctrl_t *s_ctrl = NULL;
	struct i2c_settings_array *i2c_set = NULL;

	if (!flush_req)
		return -EINVAL;

	s_ctrl = (struct cam_sensor_ctrl_t *)
		cam_get_device_priv(flush_req->dev_hdl);
	if (!s_ctrl) {
		CAM_ERR(CAM_SENSOR, "Device data is NULL");
		return -EINVAL;
	}

	for (i = 0; i < MAX_PER_FRAME_ARRAY; i++) {
		i2c_set = &(s_ctrl->i2c_data.per_frame[i]);

		if ((flush_req->type == CAM_REQ_MGR_FLUSH_TYPE_CANCEL_REQ)
				&& (i2c_set->request_id != flush_req->req_id))
			continue;

		if (i2c_set->is_settings_valid == 1) {
			rc = delete_request(i2c_set);
			if (rc < 0)
				CAM_ERR(CAM_SENSOR,
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
		CAM_DBG(CAM_SENSOR,
			"Flush request id:%lld not found in the pending list",
			flush_req->req_id);
	return rc;
}

#if defined(CONFIG_USE_CAMERA_HW_BIG_DATA)
void msm_is_sec_init_all_cnt(void)
{
	CAM_INFO(CAM_HWB, "All_Init_Cnt\n");
	memset(&cam_hwparam_collector, 0, sizeof(struct cam_hw_param_collector));
}

void msm_is_sec_init_err_cnt_file(struct cam_hw_param *hw_param)
{
	if (hw_param != NULL) {
		CAM_INFO(CAM_HWB, "Init_Cnt\n");

		memset(hw_param, 0, sizeof(struct cam_hw_param));
		msm_is_sec_copy_err_cnt_to_file();
	} else {
		CAM_INFO(CAM_HWB, "NULL\n");
	}
}

void msm_is_sec_dbg_check(void)
{
	CAM_INFO(CAM_HWB, "Dbg E\n");
	CAM_INFO(CAM_HWB, "Dbg X\n");
}

void msm_is_sec_copy_err_cnt_to_file(void)
{
#if defined(HWB_FILE_OPERATION)
	struct file *fp = NULL;
	mm_segment_t old_fs;
	long nwrite = 0;
	int old_mask = 0;

	CAM_INFO(CAM_HWB, "To_F\n");

	old_fs = get_fs();
	set_fs(KERNEL_DS);
	old_mask = sys_umask(0);

	fp = filp_open(CAM_HW_ERR_CNT_FILE_PATH, O_WRONLY | O_CREAT | O_TRUNC | O_SYNC, 0660);
	if (IS_ERR_OR_NULL(fp)) {
		CAM_ERR(CAM_HWB, "[To_F] Err\n");
		sys_umask(old_mask);
		set_fs(old_fs);
		return;
	}

	nwrite = vfs_write(fp, (char *)&cam_hwparam_collector, sizeof(struct cam_hw_param_collector), &fp->f_pos);

	filp_close(fp, NULL);
	fp = NULL;
	sys_umask(old_mask);
	set_fs(old_fs);
#endif
}

void msm_is_sec_copy_err_cnt_from_file(void)
{
#if defined(HWB_FILE_OPERATION)
	struct file *fp = NULL;
	mm_segment_t old_fs;
	long nread = 0;
	int ret = 0;

	ret = msm_is_sec_file_exist(CAM_HW_ERR_CNT_FILE_PATH, HW_PARAMS_NOT_CREATED);
	if (ret == 1) {
		CAM_INFO(CAM_HWB, "From_F\n");
		old_fs = get_fs();
		set_fs(KERNEL_DS);

		fp = filp_open(CAM_HW_ERR_CNT_FILE_PATH, O_RDONLY, 0660);
		if (IS_ERR_OR_NULL(fp)) {
			CAM_ERR(CAM_HWB, "[From_F] Err\n");
			set_fs(old_fs);
			return;
		}

		nread = vfs_read(fp, (char *)&cam_hwparam_collector, sizeof(struct cam_hw_param_collector), &fp->f_pos);

		filp_close(fp, NULL);
		fp = NULL;
		set_fs(old_fs);
	} else {
		CAM_INFO(CAM_HWB, "NoEx_F\n");
	}
#endif
}

int msm_is_sec_file_exist(char *filename, hw_params_check_type chktype)
{
	int ret = 0;
#if defined(HWB_FILE_OPERATION)
	struct file *fp = NULL;
	mm_segment_t old_fs;
	long nwrite = 0;
	int old_mask = 0;

	old_fs = get_fs();
	set_fs(KERNEL_DS);

	if (sys_access(filename, 0) == 0) {
		CAM_INFO(CAM_HWB, "Ex_F\n");
		ret = 1;
	} else {
		switch (chktype) {
		case HW_PARAMS_CREATED:
			CAM_INFO(CAM_HWB, "Ex_Cr\n");
			msm_is_sec_init_all_cnt();

			old_mask = sys_umask(0);

			fp = filp_open(CAM_HW_ERR_CNT_FILE_PATH, O_WRONLY | O_CREAT | O_TRUNC | O_SYNC, 0660);
			if (IS_ERR_OR_NULL(fp)) {
				CAM_ERR(CAM_HWB, "[Ex_F] ERROR\n");
				ret = 0;
			} else {
				nwrite = vfs_write(fp, (char *)&cam_hwparam_collector, sizeof(struct cam_hw_param_collector), &fp->f_pos);

				filp_close(fp, current->files);
				fp = NULL;
				ret = 2;
			}
			sys_umask(old_mask);
			break;

		case HW_PARAMS_NOT_CREATED:
			CAM_INFO(CAM_HWB, "Ex_NoCr\n");
			ret = 0;
			break;

		default:
			CAM_INFO(CAM_HWB, "Ex_Err\n");
			ret = 0;
			break;
		}
	}

	set_fs(old_fs);
#endif

	return ret;
}

int msm_is_sec_get_sensor_position(uint32_t **cam_position)
{
	*cam_position = &sec_sensor_position;
	return 0;
}

int msm_is_sec_get_sensor_comp_mode(uint32_t **sensor_clk_size)
{
	*sensor_clk_size = &sec_sensor_clk_size;
	return 0;
}

int msm_is_sec_get_rear_hw_param(struct cam_hw_param **hw_param)
{
	*hw_param = &cam_hwparam_collector.rear_hwparam;
	return 0;
}

int msm_is_sec_get_front_hw_param(struct cam_hw_param **hw_param)
{
	*hw_param = &cam_hwparam_collector.front_hwparam;
	return 0;
}

int msm_is_sec_get_iris_hw_param(struct cam_hw_param **hw_param)
{
	*hw_param = &cam_hwparam_collector.iris_hwparam;
	return 0;
}

int msm_is_sec_get_rear2_hw_param(struct cam_hw_param **hw_param)
{
	*hw_param = &cam_hwparam_collector.rear2_hwparam;
	return 0;
}
#endif

