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
#ifndef _CAM_EEPROM_DEV_H_
#define _CAM_EEPROM_DEV_H_

#include <linux/i2c.h>
#include <linux/kernel.h>
#include <linux/list.h>
#include <linux/gpio.h>
#include <media/v4l2-event.h>
#include <media/v4l2-subdev.h>
#include <media/v4l2-ioctl.h>
#include <media/cam_sensor.h>
#include <cam_sensor_i2c.h>
#include <cam_sensor_spi.h>
#include <cam_sensor_io.h>
#include <cam_cci_dev.h>
#include <cam_req_mgr_util.h>
#include <cam_req_mgr_interface.h>
#include <cam_mem_mgr.h>
#include <cam_subdev.h>
#include <media/cam_sensor.h>
#include "cam_soc_util.h"

#define DEFINE_MSM_MUTEX(mutexname) \
	static struct mutex mutexname = __MUTEX_INITIALIZER(mutexname)
#define OK 1
#define CRASH 0
#define PROPERTY_MAXSIZE 32

#define MSM_EEPROM_MEMORY_MAP_MAX_SIZE          80
#define MSM_EEPROM_MAX_MEM_MAP_CNT              50
#define MSM_EEPROM_MEM_MAP_PROPERTIES_CNT       6

#define REAR_MODULE_FW_VERSION                  0x0050
#if defined(CONFIG_SAMSUNG_MULTI_CAMERA)
#define REAR2_MODULE_FW_VERSION                 0x0040
#endif
#define FRONT_MODULE_FW_VERSION                 0x0030
#define FROM_MODULE_FW_INFO_SIZE                11
#define FROM_REAR_HEADER_SIZE                   0x1000

#define REAR_CAL_VERSION_ADDR                   0x5FE0
#define FRONT_CAL_VERSION_ADDR                  0x0150

#define FRONT_CAM_MAP_VERSION_ADDR              0x0043
#define REAR_CAM_MAP_VERSION_ADDR               0x0073
#define REAR_DLL_VERSION_ADDR                   0x007D
#if defined(CONFIG_SAMSUNG_MULTI_CAMERA)
#define REAR2_DLL_VERSION_ADDR                  0x0089
#endif
#define FRONT_DLL_VERSION_ADDR                  0x004B

#define FROM_REAR_AF_CAL_D10_ADDR               0x4690
/*#define FROM_REAR_AF_CAL_D40_ADDR             0x468C*/
#define FROM_REAR_AF_CAL_D50_ADDR               0x4688
/*#define FROM_REAR_AF_CAL_D80_ADDR             0x4680*/
#define FROM_REAR_AF_CAL_PAN_ADDR               0x4684
#define FROM_REAR_AF_CAL_MACRO_ADDR             0x4690
#define FROM_FRONT_AF_CAL_PAN_ADDR              0x0104
#define FROM_FRONT_AF_CAL_MACRO_ADDR            0x0110
#define FROM_FRONT_SENSOR_ID_ADDR               0x0054

#define FROM_REAR_SENSOR_ID_ADDR                0x00B8

#define FROM_PAF_CAL_DATA_START_ADDR            0x1E20 // ~2E1F
#define FROM_F2_PAF_CAL_DATA_START_ADDR         0x3250 // ~424F
/* REAR PAF OFFSET MID (30CM, WIDE) */
#define FROM_PAF_OFFSET_MID_ADDR                (FROM_PAF_CAL_DATA_START_ADDR + 0x0730)
#define FROM_PAF_OFFSET_MID_SIZE                936
/* REAR PAF OFFSET FAR (1M, WIDE) */
#define FROM_PAF_OFFSET_FAR_ADDR                (FROM_PAF_CAL_DATA_START_ADDR + 0x0CD0)
#define FROM_PAF_OFFSET_FAR_SIZE                234

/* REAR F2 PAF OFFSET MID (30CM, WIDE) */
#define FROM_F2_PAF_OFFSET_MID_ADDR             (FROM_F2_PAF_CAL_DATA_START_ADDR + 0x0730)
#define FROM_F2_PAF_OFFSET_MID_SIZE             936
/* REAR F2 PAF OFFSET FAR (1M, WIDE) */
#define FROM_F2_PAF_OFFSET_FAR_ADDR             (FROM_F2_PAF_CAL_DATA_START_ADDR + 0x0CD0)
#define FROM_F2_PAF_OFFSET_FAR_SIZE             234
#define FROM_PAF_CAL_ERR_CHECK_OFFSET		0x14

#if defined(CONFIG_SAMSUNG_MULTI_CAMERA)
#define FROM_REAR2_PAF_CAL_DATA_START_ADDR      0x6710 // ~6F0F

/*#define FROM_REAR2_AF_CAL_D10_ADDR*/
/*#define FROM_REAR2_AF_CAL_D20_ADDR*/
/*#define FROM_REAR2_AF_CAL_D30_ADDR*/
/*#define FROM_REAR2_AF_CAL_D40_ADDR            0x754C*/
#define FROM_REAR2_AF_CAL_D50_ADDR              0x7548
/*#define FROM_REAR2_AF_CAL_D60_ADDR*/
/*#define FROM_REAR2_AF_CAL_D70_ADDR*/
/*#define FROM_REAR2_AF_CAL_D80_ADDR            0x7540*/
/*#define FROM_REAR2_AF_CAL_MACRO_ADDR*/
#define FROM_REAR2_AF_CAL_PAN_ADDR              0x7544
#define FROM_REAR2_SENSOR_ID_ADDR               0x00C8
#define FROM_REAR2_DUAL_TILT_X                  0x7680
#define FROM_REAR2_DUAL_TILT_Y                  0x7684
#define FROM_REAR2_DUAL_TILT_Z                  0x7688
#define FROM_REAR2_DUAL_TILT_SX                 0x76E0
#define FROM_REAR2_DUAL_TILT_SY                 0x76E4
#define FROM_REAR2_DUAL_TILT_RANGE              0x8610
#define FROM_REAR2_DUAL_TILT_MAX_ERR            0x8614
#define FROM_REAR2_DUAL_TILT_AVG_ERR            0x8618
#define FROM_REAR2_DUAL_TILT_DLL_VERSION        0x7620
#define FROM_REAR_DUAL_CAL_ADDR                 0x7620
#define FROM_REAR_DUAL_CAL_SIZE                 4112
#endif

#define FROM_REAR_AF_CAL_SIZE                   10
#define FROM_SENSOR_ID_SIZE                     16

/* Module ID : 0x00A8~0x00B7(16Byte) for FROM, EEPROM (Don't support OTP)*/
#define FROM_MODULE_ID_ADDR                     0x00AE
#define FROM_MODULE_ID_SIZE                     10

/* MTF exif : 0x0064~0x0099(54Byte) for FROM, EEPROM */
#define FROM_REAR_MTF_ADDR                      0x0160
#define FROM_REAR_MTF2_ADDR                     0x0196
#define FROM_FRONT_MTF_ADDR                     0x0064
#if defined(CONFIG_SAMSUNG_MULTI_CAMERA)
#define FROM_REAR2_MTF_ADDR                     0x01CC
#endif
#define FROM_MTF_SIZE                           54
#define SYSFS_FW_VER_SIZE                       40
#define SYSFS_MODULE_INFO_SIZE                  96
#define FROM_CAL_MAP_VERSION                    0x38
#define FRONT_FROM_CAL_MAP_VERSION              0x33
#define MODULE_VER_ON_PVR                       0x42
#define MODULE_VER_ON_SRA                       0x4D
#define FRONT_MODULE_VER_ON_PVR                 0x42
#define FRONT_MODULE_VER_ON_SRA                 0x4D
#if defined(CONFIG_SAMSUNG_MULTI_CAMERA)
#define CAMERA_CAL_CRC                          0x3FFFFF
#else
#define CAMERA_CAL_CRC                          0x3FFF
#endif
#define CAMERA_CAL_CRC_WIDE                     0x3FFF
#define CAMERA_CAL_CRC_FRONT                    0x1F
#define REAR_PAF_CAL_INFO_SIZE                  1024

extern uint32_t front_af_cal_pan;
extern uint32_t front_af_cal_macro;
#if defined(CONFIG_SAMSUNG_MULTI_CAMERA)
extern uint8_t rear_dual_cal[FROM_REAR_DUAL_CAL_SIZE + 1];
extern int rear2_af_cal[FROM_REAR_AF_CAL_SIZE + 1];
#endif
extern int rear_af_cal[FROM_REAR_AF_CAL_SIZE + 1];
extern char rear_sensor_id[FROM_SENSOR_ID_SIZE + 1];
extern char front_sensor_id[FROM_SENSOR_ID_SIZE + 1];
#if defined(CONFIG_SAMSUNG_MULTI_CAMERA)
extern char rear2_sensor_id[FROM_SENSOR_ID_SIZE + 1];
extern int rear2_dual_tilt_x;
extern int rear2_dual_tilt_y;
extern int rear2_dual_tilt_z;
extern int rear2_dual_tilt_sx;
extern int rear2_dual_tilt_sy;
extern int rear2_dual_tilt_range;
extern int rear2_dual_tilt_max_err;
extern int rear2_dual_tilt_avg_err;
extern int rear2_dual_tilt_dll_ver;
#endif
extern char rear_paf_cal_data_far[REAR_PAF_CAL_INFO_SIZE];
extern char rear_paf_cal_data_mid[REAR_PAF_CAL_INFO_SIZE];
extern uint32_t paf_err_data_result;
extern char rear_f2_paf_cal_data_far[REAR_PAF_CAL_INFO_SIZE];
extern char rear_f2_paf_cal_data_mid[REAR_PAF_CAL_INFO_SIZE];
extern uint32_t f2_paf_err_data_result;

extern uint8_t rear_module_id[FROM_MODULE_ID_SIZE + 1];
extern uint8_t front_module_id[FROM_MODULE_ID_SIZE + 1];

extern char front_mtf_exif[FROM_MTF_SIZE + 1];
extern char rear_mtf_exif[FROM_MTF_SIZE + 1];
extern char rear_mtf2_exif[FROM_MTF_SIZE + 1];
#if defined(CONFIG_SAMSUNG_MULTI_CAMERA)
extern char rear2_mtf_exif[FROM_MTF_SIZE + 1];
extern char cam2_fw_ver[SYSFS_FW_VER_SIZE];
extern char cam2_fw_full_ver[SYSFS_FW_VER_SIZE];
#endif

extern char cam_fw_ver[SYSFS_FW_VER_SIZE];
extern char cam_fw_full_ver[SYSFS_FW_VER_SIZE];
extern char front_cam_fw_ver[SYSFS_FW_VER_SIZE];
extern char front_cam_fw_full_ver[SYSFS_FW_VER_SIZE];
extern char cam_fw_factory_ver[SYSFS_FW_VER_SIZE];
extern char cam_fw_user_ver[SYSFS_FW_VER_SIZE];
extern char front_cam_fw_user_ver[SYSFS_FW_VER_SIZE];
extern char front_cam_fw_factory_ver[SYSFS_FW_VER_SIZE];
extern char cal_crc[SYSFS_FW_VER_SIZE];

extern char front_module_info[SYSFS_MODULE_INFO_SIZE];
extern char module_info[SYSFS_MODULE_INFO_SIZE];


/* phone fw info */
#define HW_INFO_MAX_SIZE 6
#define SW_INFO_MAX_SIZE 5
#define VENDOR_INFO_MAX_SIZE 2
#define PROCESS_INFO_MAX_SIZE 2

#if defined(CONFIG_SAMSUNG_MULTI_CAMERA)
#if defined(CONFIG_SEC_CROWNQLTE_PROJECT)
#define HW_INFO                                 ("J12QS")
#define SW_INFO                                 ("LD01")
#define VENDOR_INFO                             ("V")
#define PROCESS_INFO                            ("A")
#define CRITERION_REV                           (10)
#else
#define HW_INFO                                 ("I12QS")
#define SW_INFO                                 ("KK01")
#define VENDOR_INFO                             ("V")
#define PROCESS_INFO                            ("A")
#define CRITERION_REV                           (14)
#endif
#else
#define HW_INFO                                 ("H12QS")
#define SW_INFO                                 ("KK01")
#define VENDOR_INFO                             ("V")
#define PROCESS_INFO                            ("A")
#define CRITERION_REV                           (14)
#endif

#define FRONT_HW_INFO                           ("W08QS")
#define FRONT_SW_INFO                           ("JA01")
#define FRONT_VENDOR_INFO                       ("A")
#define FRONT_PROCESS_INFO                      ("A")
/**
 * struct cam_eeprom_map_t - eeprom map
 * @data_type       :   Data type
 * @addr_type       :   Address type
 * @addr            :   Address
 * @data            :   data
 * @delay           :   Delay
 *
 */
struct cam_eeprom_map_t {
	uint32_t valid_size;
	uint32_t addr;
	uint32_t addr_type;
	uint32_t data;
	uint32_t data_type;
	uint32_t delay;
};

enum cam_eeprom_state {
	CAM_EEPROM_INIT,
	CAM_EEPROM_ACQUIRE,
	CAM_EEPROM_CONFIG,
};

/**
 * struct cam_eeprom_memory_map_t - eeprom memory map types
 * @page            :   page memory
 * @pageen          :   pageen memory
 * @poll            :   poll memory
 * @mem             :   mem
 * @saddr           :   slave addr
 *
 */
struct cam_eeprom_memory_map_t {
	struct cam_eeprom_map_t page;
	struct cam_eeprom_map_t pageen;
	struct cam_eeprom_map_t poll;
	struct cam_eeprom_map_t mem;
	uint32_t saddr;
};

/**
 * struct cam_eeprom_memory_block_t - eeprom mem block info
 * @map             :   eeprom memory map
 * @num_map         :   number of map blocks
 * @mapdata         :   map data
 * @cmd_type        :   size of total mapdata
 *
 */
struct cam_eeprom_memory_block_t {
	struct cam_eeprom_memory_map_t *map;
	uint32_t num_map;
	uint8_t *mapdata;
	uint32_t num_data;
	uint16_t is_supported;
};

/**
 * struct cam_eeprom_cmm_t - camera multimodule
 * @cmm_support     :   cmm support flag
 * @cmm_compression :   cmm compression flag
 * @cmm_offset      :   cmm data start offset
 * @cmm_size        :   cmm data size
 *
 */
struct cam_eeprom_cmm_t {
	uint32_t cmm_support;
	uint32_t cmm_compression;
	uint32_t cmm_offset;
	uint32_t cmm_size;
};

/**
 * struct cam_eeprom_i2c_info_t - I2C info
 * @slave_addr      :   slave address
 * @i2c_freq_mode   :   i2c frequency mode
 *
 */
struct cam_eeprom_i2c_info_t {
	uint16_t slave_addr;
	uint8_t i2c_freq_mode;
};

/**
 * struct cam_eeprom_soc_private - eeprom soc private data structure
 * @eeprom_name     :   eeprom name
 * @i2c_info        :   i2c info structure
 * @power_info      :   eeprom power info
 * @cmm_data        :   cmm data
 *
 */
struct cam_eeprom_soc_private {
	const char *eeprom_name;
	struct cam_eeprom_i2c_info_t i2c_info;
	struct cam_sensor_power_ctrl_t power_info;
	struct cam_eeprom_cmm_t cmm_data;
};

/**
 * struct cam_eeprom_intf_params - bridge interface params
 * @device_hdl   : Device Handle
 * @session_hdl  : Session Handle
 * @ops          : KMD operations
 * @crm_cb       : Callback API pointers
 */
struct cam_eeprom_intf_params {
	int32_t device_hdl;
	int32_t session_hdl;
	int32_t link_hdl;
	struct cam_req_mgr_kmd_ops ops;
	struct cam_req_mgr_crm_cb *crm_cb;
};

struct eebin_info {
	uint32_t start_address;
	uint32_t size;
	uint32_t is_valid;
};

/**
 * struct cam_cmd_conditional_wait - Conditional wait command
 * @pdev            :   platform device
 * @spi             :   spi device
 * @eeprom_mutex    :   eeprom mutex
 * @soc_info        :   eeprom soc related info
 * @io_master_info  :   Information about the communication master
 * @gpio_num_info   :   gpio info
 * @cci_i2c_master  :   I2C structure
 * @v4l2_dev_str    :   V4L2 device structure
 * @bridge_intf     :   bridge interface params
 * @cam_eeprom_state:   eeprom_device_state
 * @userspace_probe :   flag indicates userspace or kernel probe
 * @cal_data        :   Calibration data
 * @device_name     :   Device name
 *
 */
struct cam_eeprom_ctrl_t {
	struct platform_device *pdev;
	struct spi_device *spi;
	struct mutex eeprom_mutex;
	struct cam_hw_soc_info soc_info;
	struct camera_io_master io_master_info;
	struct msm_camera_gpio_num_info *gpio_num_info;
	enum cci_i2c_master_t cci_i2c_master;
	struct cam_subdev v4l2_dev_str;
	struct cam_eeprom_intf_params bridge_intf;
	enum msm_camera_device_type_t eeprom_device_type;
	enum cam_eeprom_state cam_eeprom_state;
	bool userspace_probe;
	struct cam_eeprom_memory_block_t cal_data;
	char device_name[20];
	uint32_t is_supported;
	uint16_t is_multimodule_node;
	struct i2c_settings_array wr_settings;
	struct eebin_info eebin_info;
};

typedef enum{
	EEPROM_FW_VER = 1,
	PHONE_FW_VER,
	LOAD_FW_VER
} cam_eeprom_fw_version_idx;

typedef enum{
	CAM_EEPROM_IDX_BACK,
	CAM_EEPROM_IDX_FRONT,
	CAM_EEPROM_IDX_MAX
} cam_eeprom_idx_type;


int32_t cam_eeprom_update_i2c_info(struct cam_eeprom_ctrl_t *e_ctrl,
	struct cam_eeprom_i2c_info_t *i2c_info);

#endif /*_CAM_EEPROM_DEV_H_ */
