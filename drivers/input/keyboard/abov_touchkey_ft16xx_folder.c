/* abov_touchkey.c -- Linux driver for abov chip as touchkey
 *
 * Copyright (C) 2013 Samsung Electronics Co.Ltd
 * Author: Junkyeong Kim <jk0430.kim@samsung.com>
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation; either version 2, or (at your option) any
 * later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 */

#include <linux/delay.h>
#include <linux/firmware.h>
#include <linux/gpio.h>
#include <linux/i2c.h>
#include <linux/init.h>
#include <linux/input.h>
#include <linux/input/mt.h>
#include <linux/interrupt.h>
#include <linux/irq.h>
#include <linux/module.h>
#include <linux/mutex.h>
#include <linux/slab.h>
#include <linux/uaccess.h>
#include <linux/io.h>
#include <asm/unaligned.h>
#include <linux/regulator/consumer.h>
#include <linux/wakelock.h>
#include <linux/pinctrl/consumer.h>
#include <linux/hall.h>

#ifdef CONFIG_OF
#include <linux/of_gpio.h>
#endif
#ifdef CONFIG_INPUT_BOOSTER
#include <linux/input/input_booster.h>
#endif

#if defined(CONFIG_DRV_SAMSUNG)
#include <linux/input/sec_cmd.h>
#endif

/* registers */
#define ABOV_BTNSTATUS		0x00
#define ABOV_FW_VER		0x01
#define ABOV_PCB_VER		0x02
#define ABOV_COMMAND		0x03
#define ABOV_THRESHOLD		0x04
#define ABOV_SETIDAC		0x06
#define ABOV_DIFFDATA		0x0A
#define ABOV_KEY_VALUE		0x10
#define ABOV_GLOVE		0x12
#define ABOV_VENDORID		0x13
#define ABOV_MODELNO		0x14
#define ABOV_RAWDATA		0x15

/* command */
#define CMD_LED_ON		0x10
#define CMD_LED_OFF		0x20
#define CMD_DATA_UPDATE		0x40
#define CMD_LED_CTRL_ON		0x60
#define CMD_LED_CTRL_OFF	0x70
#define CMD_STOP_MODE		0x80
#define CMD_GLOVE_ON		0x20
#define CMD_GLOVE_OFF		0x10
#define CMD_CRC_CHECK_START	0xAA

struct device *sec_touchkey;
#define ABOV_TK_NAME		"abov-touchkey"
#define FW_VERSION		0x04
#define FW_CHECKSUM_H		0x4B
#define FW_CHECKSUM_L		0xC4
#define TK_FW_PATH_BIN		"abov/abov_tk_kelly.fw"
#define TK_FW_PATH_SDCARD	"/sdcard/Firmware/TOUCHKEY/abov_fw.bin"

#define CRC_CHECK_WITHBOOTING
#define CRC_CHECK_FW_VER	0x10

#define ABOV_BOOT_DELAY		45
#ifdef CRC_CHECK_WITHBOOTING
#define ABOV_RESET_DELAY	100
#else
#define ABOV_RESET_DELAY	150
#endif

#define I2C_M_WR 0		/* for i2c */

enum {
	BUILT_IN = 0,
	SDCARD,
};

enum flip_status{
	FLIP_OPEN = 0,
	FLIP_CLOSE,
};

enum fw_update_status{
	FW_UP_SUCCESS = 0,
	FW_DOWNLOADING,
	FW_UP_FAILED,
};

#define LIGHT_VERSION_PATH		"/efs/FactoryApp/tkey_light_version"
#define LIGHT_TABLE_PATH		"/efs/FactoryApp/tkey_light"
#define LIGHT_CRC_PATH			"/efs/FactoryApp/tkey_light_crc"
#define LIGHT_TABLE_PATH_LEN		50
#define LIGHT_VERSION_LEN		25
#define LIGHT_CRC_SIZE			10
#define LIGHT_DATA_SIZE			5
#define LIGHT_VOLTAGE_MIN_VAL		3000
#define LIGHT_TABLE_MAX			2

struct light_info {
	int octa_id;
	int vol_mv;
};

enum WINDOW_COLOR {
	WINDOW_COLOR_BLACK_UTYPE = 0,
	WINDOW_COLOR_BLACK,
	WINDOW_COLOR_WHITE,
	WINDOW_COLOR_GOLD,
	WINDOW_COLOR_SILVER,
	WINDOW_COLOR_GREEN,
	WINDOW_COLOR_BLUE,
	WINDOW_COLOR_PINKGOLD,
};

#define WINDOW_COLOR_DEFAULT		WINDOW_COLOR_GOLD

/* should be pair table item numbers and table size */
#define LIGHT_VERSION			160822

struct light_info tkey_light_voltage_table[LIGHT_TABLE_MAX] =
{
	/* octa id, voltage */
	{ WINDOW_COLOR_BLACK,		3300},
	{ WINDOW_COLOR_GOLD,		3300},
};
/**************************************************/

#ifdef CONFIG_SAMSUNG_LPM_MODE
extern int lpcharge;
#endif
extern unsigned int system_rev;
#if defined(CONFIG_DRV_SAMSUNG)
#else
extern struct class *sec_class;
#endif
extern int get_lcd_attached(char *);
extern int get_lcd_attached_secondary(char *);
static int touchkey_keycode[] = { 0,
	KEY_RECENT, KEY_BACK, KEY_HOMEPAGE, //KEY_TKEY_WAKEUP,
};

struct abov_tk_info {
	struct i2c_client *client;
	struct input_dev *input_dev;
	struct abov_touchkey_dt_data *dtdata;
	struct mutex lock;
	struct pinctrl *pinctrl;
	const struct firmware *firm_data_bin;
	const u8 *firm_data_ums;
	char phys[32];
	long firm_size;
	int irq;
	u16 menu_s;
	u16 back_s;
	u16 home_s;
	u16 menu_raw;
	u16 back_raw;
	u16 home_raw;
	int (*power) (bool on);
	int touchkey_count;
	u8 fw_update_state;
	u8 fw_ver;
	u8 checksum_h;
	u8 checksum_l;
	bool enabled;
	bool fw_update_possible;
	bool glovemode;
	bool wakeup_mode;
	bool wakeup_state;
	struct wake_lock report_wake_lock;
#ifdef CONFIG_DUAL_TSP
	struct delayed_work switching_work;
	struct notifier_block hall_ic_nb;
	int flip_status;
	int flip_status_current;
#endif
	bool probe_done;
#ifdef CONFIG_INPUT_BOOSTER
	struct input_booster *tkey_booster;
#endif

	struct delayed_work efs_open_work;
	int light_version_efs;
	char light_version_full_efs[LIGHT_VERSION_LEN];
	char light_version_full_bin[LIGHT_VERSION_LEN];
	int light_table_crc;
};

struct abov_touchkey_dt_data {
	unsigned long irq_flag;
	int gpio_int;
	int gpio_sda;
	int gpio_scl;
	int sub_det;
	int gpio_hall;
	int gpio_tkey_led_en;
	bool reg_boot_on;
	bool forced_update;
	struct regulator *vdd_io_vreg;
	struct regulator *avdd_vreg;
	struct regulator *led_avdd_vreg;
	int (*power) (struct abov_touchkey_dt_data *dtdata, bool on);
	const char *fw_name;
	int light_version;
};

static int abov_touchkey_led_status;
static int abov_touchled_cmd_reserved;

static int abov_tk_suspend(struct device *dev);
static int abov_tk_input_open(struct input_dev *dev);
static void abov_tk_input_close(struct input_dev *dev);
int abov_power(struct abov_touchkey_dt_data *dtdata, bool on);
static int efs_read_light_table_version(struct abov_tk_info *info);

#ifdef CONFIG_DUAL_TSP
static void abov_switching_tkey_work(struct work_struct *work);
static int abov_hall_ic_notify(struct notifier_block *nb,
				unsigned long flip_cover, void *v);

static struct abov_tk_info *tkey_driver = NULL;
void abov_set_tkey_info(struct abov_tk_info *info)
{
	if(info != NULL)
		tkey_driver = info;
	else
		printk("[sec_input] %s : tkey info is null\n", __func__);
}

static struct abov_tk_info *abov_get_tkey_info(void)
{
	return tkey_driver;
}
#endif

static int abov_glove_mode_enable(struct i2c_client *client, u8 cmd)
{
	return i2c_smbus_write_byte_data(client, ABOV_GLOVE, cmd);
}

static void change_touch_key_led_voltage(struct device *dev, int vol_mv)
{
	struct abov_tk_info *info = dev_get_drvdata(dev);	
	int ret;

	ret = regulator_set_voltage(info->dtdata->led_avdd_vreg, vol_mv * 1000, vol_mv * 1000);
	if (ret)
		input_err(true, dev, "%s: failed to set key led %d mv, %d\n",
				__func__, vol_mv, ret);
	
	input_info(true, dev, "%s: %dmV\n", __func__, vol_mv);
}

static ssize_t brightness_control(struct device *dev,
		struct device_attribute *attr,
		const char *buf, size_t size)
{
	struct abov_tk_info *info = dev_get_drvdata(dev);
	int data;

	if (sscanf(buf, "%d\n", &data) == 1)
		change_touch_key_led_voltage(&info->client->dev, data);
	else
		input_err(true, &info->client->dev, "%s Error\n", __func__);

	return size;
}

static int read_window_type(void)
{
	struct file *type_filp = NULL;
	mm_segment_t old_fs;
	int ret = 0;
	char window_type[10] = {0, };

	old_fs = get_fs();
	set_fs(KERNEL_DS);

	type_filp = filp_open("/sys/class/lcd/panel/window_type", O_RDONLY, 0440);
	if (IS_ERR(type_filp)) {
		set_fs(old_fs);
		ret = PTR_ERR(type_filp);
		return ret;
	}

	ret = type_filp->f_op->read(type_filp, window_type,
			sizeof(window_type), &type_filp->f_pos);
	if (ret != 9 * sizeof(char)) {
		pr_err("%s touchkey %s: fd read fail\n", SECLOG, __func__);
		set_fs(old_fs);
		ret = -EIO;
		return ret;
	}

	filp_close(type_filp, current->files);
	set_fs(old_fs);

	if (window_type[1] < '0' || window_type[1] >= 'f')
		return -EAGAIN;

	ret = (window_type[1] - '0') & 0x0f;
	pr_info("%s touchkey %s: %d\n", SECLOG, __func__, ret);
	return ret;
}

static int efs_calculate_crc (struct abov_tk_info *info)
{
	struct file *temp_file = NULL;
	int crc = info->light_version_efs;
	mm_segment_t old_fs;
	char predefine_value_path[LIGHT_TABLE_PATH_LEN];
	int ret = 0, i;
	char temp_vol[LIGHT_CRC_SIZE] = {0, };
	int table_size;

	efs_read_light_table_version(info);
	table_size = (int)strlen(info->light_version_full_efs) - 8;

	for (i = 0; i < table_size; i++) {
		char octa_temp = info->light_version_full_efs[8 + i];
		int octa_temp_i;

		if (octa_temp >= 'A')
			octa_temp_i = octa_temp - 'A' + 0x0A;
		else
			octa_temp_i = octa_temp - '0';
		
		input_info(true, &info->client->dev, "%s: octa %d\n", __func__, octa_temp_i);

		snprintf(predefine_value_path, LIGHT_TABLE_PATH_LEN, "%s%d",
				LIGHT_TABLE_PATH, octa_temp_i);
		old_fs = get_fs();
		set_fs(KERNEL_DS);
		temp_file = filp_open(predefine_value_path, O_RDONLY, 0440);
		if (!IS_ERR(temp_file)) {
			temp_file->f_op->read(temp_file, temp_vol,
					sizeof(temp_vol), &temp_file->f_pos);
			filp_close(temp_file, current->files);
			if (kstrtoint(temp_vol, 0, &ret) < 0) {
				ret = -EIO;
			} else {
				crc += octa_temp_i;
				crc += ret;
				ret = 0;
			}
		}
		set_fs(old_fs);
	}

	if (!ret)
		ret = crc;

	return ret;
}

static int efs_read_crc(struct abov_tk_info *info)
{
	struct file *temp_file = NULL;
	char crc[LIGHT_CRC_SIZE] = {0, };
	mm_segment_t old_fs;
	int ret = 0;
	
	old_fs = get_fs();
	set_fs(KERNEL_DS);
	temp_file = filp_open(LIGHT_CRC_PATH, O_RDONLY, 0440);
	if (IS_ERR(temp_file)) {
		ret = PTR_ERR(temp_file);
		input_info(true, &info->client->dev,
				"%s: failed to open efs file %d\n", __func__, ret);
	} else {
		temp_file->f_op->read(temp_file, crc, sizeof(crc), &temp_file->f_pos);
		filp_close(temp_file, current->files);
		if (kstrtoint(crc, 0, &ret) < 0)
			ret = -EIO;
	}
	set_fs(old_fs);

	return ret;
}


static bool check_light_table_crc(struct abov_tk_info *info)
{
	int crc_efs = efs_read_crc(info);

	if (info->light_version_efs == LIGHT_VERSION) {
		/* compare efs crc file with binary crc*/
		input_info(true, &info->client->dev,
				"%s: efs:%d, bin:%d\n",
				__func__, crc_efs, info->light_table_crc);
		if (crc_efs != info->light_table_crc)
			return false;
	}

	return true;
}

static int efs_write_light_table_crc(struct abov_tk_info *info, int crc_cal)
{
	struct file *temp_file = NULL;
	char crc[LIGHT_CRC_SIZE] = {0, };
	mm_segment_t old_fs;
	int ret = 0;

	old_fs = get_fs();
	set_fs(KERNEL_DS);
	temp_file = filp_open(LIGHT_CRC_PATH, O_TRUNC | O_RDWR | O_CREAT, 0660);
	if (IS_ERR(temp_file)) {
		ret = PTR_ERR(temp_file);
		input_info(true, &info->client->dev,
				"%s: failed to open efs file %d\n", __func__, ret);
	} else {
		snprintf(crc, sizeof(crc), "%d", crc_cal);
		temp_file->f_op->write(temp_file, crc, sizeof(crc), &temp_file->f_pos);
		filp_close(temp_file, current->files);
		input_info(true, &info->client->dev, "%s: %s\n", __func__, crc);
	}
	set_fs(old_fs);
	return ret;
}

static int efs_write_light_table_version(struct abov_tk_info *info, char *full_version)
{
	struct file *temp_file = NULL;
	mm_segment_t old_fs;
	int ret = 0;

	old_fs = get_fs();
	set_fs(KERNEL_DS);
	temp_file = filp_open(LIGHT_VERSION_PATH, O_TRUNC | O_RDWR | O_CREAT, 0660);
	if (IS_ERR(temp_file)) {
		ret = -ENOENT;
	} else {
		temp_file->f_op->write(temp_file, full_version,
				LIGHT_VERSION_LEN, &temp_file->f_pos);
		filp_close(temp_file, current->files);
		input_info(true, &info->client->dev, "%s: version = %s\n",
				__func__, full_version);
	}
	set_fs(old_fs);
	return ret;
}

static int efs_write_light_table(struct abov_tk_info *info, struct light_info table)
{
	struct file *type_filp = NULL;
	mm_segment_t old_fs;
	int ret = 0;
	char predefine_value_path[LIGHT_TABLE_PATH_LEN];
	char vol_mv[LIGHT_DATA_SIZE] = {0, };

	snprintf(predefine_value_path, LIGHT_TABLE_PATH_LEN,
			"%s%d", LIGHT_TABLE_PATH, table.octa_id);
	snprintf(vol_mv, sizeof(vol_mv), "%d", table.vol_mv);

	input_info(true, &info->client->dev, "%s: make %s\n", __func__, predefine_value_path);

	old_fs = get_fs();
	set_fs(KERNEL_DS);

	type_filp = filp_open(predefine_value_path, O_TRUNC | O_RDWR | O_CREAT, 0660);
	if (IS_ERR(type_filp)) {
		set_fs(old_fs);
		ret = PTR_ERR(type_filp);
		input_err(true, &info->client->dev, "%s: open fail :%d\n",
			__func__, ret);
		return ret;
	}

	type_filp->f_op->write(type_filp, vol_mv, sizeof(vol_mv), &type_filp->f_pos);
	filp_close(type_filp, current->files);
	set_fs(old_fs);

	return ret;
}

static int efs_write(struct abov_tk_info *info)
{
	int ret = 0;
	int i, crc_cal;

	ret = efs_write_light_table_version(info, info->light_version_full_bin);
	if (ret < 0)
		return ret;
	info->light_version_efs = LIGHT_VERSION;

	for (i = 0; i < LIGHT_TABLE_MAX; i++) {
		ret = efs_write_light_table(info, tkey_light_voltage_table[i]);
		if (ret < 0)
			break;
	}
	if (ret < 0)
		return ret;

	crc_cal = efs_calculate_crc(info);
	if (crc_cal < 0)
		return crc_cal;

	ret = efs_write_light_table_crc(info, crc_cal);
	if (ret < 0)
		return ret;

	if (!check_light_table_crc(info))
		ret = -EIO;

	return ret;
}

static int pick_light_table_version(char* str)
{
	static char* str_addr;
	char* token = NULL;
	int ret = 0;
	
	if (str != NULL)
		str_addr = str;
	else if (str_addr == NULL)
		return 0;

	token = str_addr;
	while (true) {
		if (!(*str_addr)) {
			break;
 		} else if (*str_addr == 'T') {
			*str_addr = '0';
		} else if (*str_addr == '.') {
			*str_addr = '\0';
			str_addr = str_addr + 1;
			break;
		}
		str_addr++;
	}

	if (kstrtoint(token + 1, 0, &ret) < 0)
		return 0;

	return ret;
}


static int efs_read_light_table_version(struct abov_tk_info *info)
{
	struct file *temp_file = NULL;
	char version[LIGHT_VERSION_LEN] = {0, };
	mm_segment_t old_fs;
	int ret = 0;
	
	old_fs = get_fs();
	set_fs(KERNEL_DS);
	temp_file = filp_open(LIGHT_VERSION_PATH, O_RDONLY, 0440);
	if (IS_ERR(temp_file)) {
		ret = PTR_ERR(temp_file);
	} else {
		temp_file->f_op->read(temp_file, version, sizeof(version), &temp_file->f_pos);
		filp_close(temp_file, current->files);
		input_info(true, &info->client->dev,
				"%s: table full version = %s\n", __func__, version);
		snprintf(info->light_version_full_efs,
				sizeof(info->light_version_full_efs), version);
		info->light_version_efs = pick_light_table_version(version);
		input_dbg(true, &info->client->dev,
				"%s: table version = %d\n", __func__, info->light_version_efs);
	}
	set_fs(old_fs);

	return ret;
}

static int efs_read_light_table(struct abov_tk_info *info, int octa_id)
{
	struct file *type_filp = NULL;
	mm_segment_t old_fs;
	char predefine_value_path[LIGHT_TABLE_PATH_LEN];
	char voltage[LIGHT_DATA_SIZE] = {0, };
	int ret;

	snprintf(predefine_value_path, LIGHT_TABLE_PATH_LEN,
		"%s%d", LIGHT_TABLE_PATH, octa_id);

	input_info(true, &info->client->dev, "%s: %s\n", __func__, predefine_value_path);

	old_fs = get_fs();
	set_fs(KERNEL_DS);

	type_filp = filp_open(predefine_value_path, O_RDONLY, 0440);
	if (IS_ERR(type_filp)) {
		set_fs(old_fs);
		ret = PTR_ERR(type_filp);
		input_err(true, &info->client->dev,
				"%s: fail to open light data %d\n", __func__, ret);
		return ret;
	}

	type_filp->f_op->read(type_filp, voltage, sizeof(voltage), &type_filp->f_pos);
	filp_close(type_filp, current->files);
	set_fs(old_fs);

	if (kstrtoint(voltage, 0, &ret) < 0)
		return -EIO;

	return ret;
}

static int efs_read_light_table_with_default(struct abov_tk_info *info, int octa_id)
{
	bool set_default = false;
	int ret;

retry:
	if (set_default)
		octa_id = WINDOW_COLOR_DEFAULT;

	ret = efs_read_light_table(info, octa_id);
	if (ret < 0) {
		if (!set_default) {
			set_default = true;
			goto retry;
		}
	}

	return ret;
}


static bool need_update_light_table(struct abov_tk_info *info)
{
	/* Check version file exist*/
	if (efs_read_light_table_version(info) < 0) {
		return true;
	}

	/* Compare version */
	input_info(true, &info->client->dev,
			"%s: efs:%d, bin:%d\n", __func__,
			info->light_version_efs, LIGHT_VERSION);
	if (info->light_version_efs < LIGHT_VERSION)
		return true;

	/* Check CRC */
	if (!check_light_table_crc(info)) {
		input_info(true, &info->client->dev,
				"%s: crc is diffrent\n", __func__);
		return true;
	}

	return false;
}

static void touchkey_efs_open_work(struct work_struct *work)
{
	struct abov_tk_info *info =
			container_of(work, struct abov_tk_info, efs_open_work.work);
	int window_type;
	static int count = 0;
	int vol_mv;

	if (need_update_light_table(info)) {
		if (efs_write(info) < 0)
			goto out;
	}

	window_type = read_window_type();
	if (window_type < 0)
		goto out;

	vol_mv = efs_read_light_table_with_default(info, window_type);
	if (vol_mv >= LIGHT_VOLTAGE_MIN_VAL) {
		change_touch_key_led_voltage(&info->client->dev, vol_mv);
		input_info(true, &info->client->dev,
				"%s: read done for %d\n", __func__, window_type);
	} else {
		input_err(true, &info->client->dev,
				"%s: fail. voltage is %d\n", __func__, vol_mv);
	}
	return;

out:
	if (count < 50) {
		schedule_delayed_work(&info->efs_open_work, msecs_to_jiffies(2000));
		count++;
 	} else {
		input_err(true, &info->client->dev,
				"%s: retry %d times but can't check efs\n", __func__, count);
 	}
}


static int abov_tk_i2c_read(struct i2c_client *client,
		u8 reg, u8 *val, unsigned int len)
{
	struct abov_tk_info *info = i2c_get_clientdata(client);
	struct i2c_msg msg;
	int ret;
	int retry = 3;

	mutex_lock(&info->lock);
	msg.addr = client->addr;
	msg.flags = I2C_M_WR;
	msg.len = 1;
	msg.buf = &reg;
	while (retry--) {
		ret = i2c_transfer(client->adapter, &msg, 1);
		if (ret >= 0)
			break;

		input_err(true, &client->dev, "%s fail(address set)(%d)\n",
			__func__, retry);
		msleep(10);
	}
	if (ret < 0) {
		mutex_unlock(&info->lock);
		return ret;
	}
	retry = 3;
	msg.flags = 1;/*I2C_M_RD*/
	msg.len = len;
	msg.buf = val;
	while (retry--) {
		ret = i2c_transfer(client->adapter, &msg, 1);
		if (ret >= 0) {
			mutex_unlock(&info->lock);
			return 0;
		}
		input_err(true, &client->dev, "%s fail(data read)(%d)\n",
			__func__, retry);
		msleep(10);
	}
	mutex_unlock(&info->lock);
	return ret;
}

static int abov_tk_i2c_write(struct i2c_client *client,
		u8 reg, u8 *val, unsigned int len)
{
	struct abov_tk_info *info = i2c_get_clientdata(client);
	struct i2c_msg msg[1];
	unsigned char data[2];
	int ret;
	int retry = 3;

	mutex_lock(&info->lock);
	data[0] = reg;
	data[1] = *val;
	msg->addr = client->addr;
	msg->flags = I2C_M_WR;
	msg->len = 2;
	msg->buf = data;

	while (retry--) {
		ret = i2c_transfer(client->adapter, msg, 1);
		if (ret >= 0) {
			mutex_unlock(&info->lock);
			return 0;
		}
		input_err(true, &client->dev, "%s fail(%d)\n",
			__func__, retry);
		msleep(10);
	}
	mutex_unlock(&info->lock);
	return ret;
}

static void release_all_fingers(struct abov_tk_info *info)
{
	struct i2c_client *client = info->client;
	int i;

	input_dbg(true, &client->dev, "[TK] %s\n", __func__);

	for (i = 1; i < info->touchkey_count; i++) {
		input_report_key(info->input_dev,
			touchkey_keycode[i], 0);
	}
	input_sync(info->input_dev);
#ifdef CONFIG_INPUT_BOOSTER
	if (info->tkey_booster && info->tkey_booster->dvfs_set)
		info->tkey_booster->dvfs_set(info->tkey_booster, 2);
#endif
}

static void abov_tk_reset_for_bootmode(struct abov_tk_info *info)
{
	info->dtdata->power(info->dtdata, false);
	msleep(50);
	info->dtdata->power(info->dtdata, true);
}

static void abov_tk_reset(struct abov_tk_info *info)
{
	struct i2c_client *client = info->client;

	if (info->enabled == false)
		return;

	input_info(true, &client->dev, "%s++\n", __func__);
	disable_irq_nosync(info->irq);

	info->enabled = false;

	release_all_fingers(info);

	abov_tk_reset_for_bootmode(info);
	msleep(ABOV_RESET_DELAY);

	if (info->glovemode)
		abov_glove_mode_enable(client, CMD_GLOVE_ON);

	info->enabled = true;

	enable_irq(info->irq);
	input_info(true, &client->dev, "%s--\n", __func__);
}

static irqreturn_t abov_tk_interrupt(int irq, void *dev_id)
{
	struct abov_tk_info *info = dev_id;
	struct i2c_client *client = info->client;
	int ret, retry;
	u8 buf;
	bool press = 0;
	u8 key_data[info->touchkey_count];
	u8 key_press[info->touchkey_count];
	int i;

	ret = abov_tk_i2c_read(client, ABOV_KEY_VALUE, &buf, 1);
	if (ret < 0) {
		retry = 3;
		while (retry--) {
			input_err(true, &client->dev, "%s read fail(%d)\n",
				__func__, retry);
			ret = abov_tk_i2c_read(client, ABOV_KEY_VALUE, &buf, 1);
			if (ret == 0)
				break;
			else
				msleep(10);
		}
		if (retry == 0) {
			abov_tk_reset(info);
			return IRQ_HANDLED;
		}
	}

	/* concept for L OS screen pinning (multi key)
	 *
	 * bit	7	6	5	4	3	2	1	0
	 * event	-	double	home	home	back	back	menu	menu
	 *	-	tab	press	release	press	release	press	release
	 */

	for (i = 1; i < info->touchkey_count; i++) {
		if (i == 4) {
			/* double tab event has only 1 bit */
			key_data[i] = (buf >> ((i - 1) * 2)) & 0x01;
			key_press[i] = key_data[i];
		} else {
			key_data[i] = (buf >> ((i - 1) * 2)) & 0x03;
			key_press[i] = !(key_data[i] % 2);
		}

		if (key_data[i]) {
			press |= key_press[i];
			input_report_key(info->input_dev,
					touchkey_keycode[i], key_press[i]);
#ifdef CONFIG_SAMSUNG_PRODUCT_SHIP
			input_info(true, &client->dev, "key %s ver:0x%x\n",
					key_press[i] ? "P" : "R",
					info->fw_ver);
#else
			input_info(true, &client->dev, "key %d %s ver:0x%x\n",
					touchkey_keycode[i],
					key_press[i] ? "P" : "R",
					info->fw_ver);
#endif
		}
	}

	input_sync(info->input_dev);

	if (key_press[4]){
		input_info(true, &client->dev, "Double tab wakeup\n");
		wake_lock_timeout(&info->report_wake_lock, 3 * HZ);
	}

#ifdef CONFIG_INPUT_BOOSTER
	if (info->tkey_booster && info->tkey_booster->dvfs_set)
		info->tkey_booster->dvfs_set(info->tkey_booster, press);
#endif

	return IRQ_HANDLED;
}

#ifdef CONFIG_INPUT_BOOSTER
static ssize_t boost_level_store(struct device *dev,
				   struct device_attribute *attr,
				   const char *buf, size_t count)
{
	struct abov_tk_info *info = dev_get_drvdata(dev);
	int val, stage;
	struct dvfs value;

	if (!info->tkey_booster) {
		input_err(true, &info->client->dev,
			"%s: booster is NULL\n", __func__);
		return count;
	}

	sscanf(buf, "%d", &val);

	stage = 1 << val;

	if (!(info->tkey_booster->dvfs_stage & stage)) {
		input_info(true, &info->client->dev,
			"%s: wrong cmd %d\n", __func__, val);
		return count;
	}

	info->tkey_booster->dvfs_boost_mode = stage;
	input_info(true, &info->client->dev,
			"%s: dvfs_boost_mode = 0x%X\n",
			__func__, info->tkey_booster->dvfs_boost_mode);

	if (info->tkey_booster->dvfs_boost_mode == DVFS_STAGE_NONE) {
		if (info->tkey_booster->dvfs_set)
			info->tkey_booster->dvfs_set(info->tkey_booster, 2);
	} else if (info->tkey_booster->dvfs_boost_mode == DVFS_STAGE_SINGLE) {
		input_booster_get_default_setting("head", &value);
		info->tkey_booster->dvfs_freq = value.cpu_freq;
		input_info(true, &info->client->dev,
			"%s: boost_mode SINGLE, dvfs_freq = %d\n",
			__func__, info->tkey_booster->dvfs_freq);
	} else if (info->tkey_booster->dvfs_boost_mode == DVFS_STAGE_DUAL) {
		input_booster_get_default_setting("tail", &value);
		info->tkey_booster->dvfs_freq = value.cpu_freq;
		input_info(true, &info->client->dev,
			"%s: boost_mode DUAL, dvfs_freq = %d\n",
			__func__, info->tkey_booster->dvfs_freq);
	}

	return count;
}
#endif

static ssize_t touchkey_led_control(struct device *dev,
		 struct device_attribute *attr, const char *buf,
		 size_t count)
{
	struct abov_tk_info *info = dev_get_drvdata(dev);
	struct i2c_client *client = info->client;
	int data;
	int ret;
	u8 cmd;

	ret = sscanf(buf, "%d", &data);
	if (ret != 1) {
		input_err(true, &client->dev, "%s: cmd read err\n", __func__);
		return count;
	}

	if (!(data == 0 || data == 1)) {
		input_err(true, &client->dev, "%s: wrong command(%d)\n",
			__func__, data);
		return count;
	}

	if (data == 1)
		cmd = CMD_LED_ON;
	else
		cmd = CMD_LED_OFF;

	if (!info->enabled){
		abov_touchled_cmd_reserved = 1;
		goto out;
	}

	ret = abov_tk_i2c_write(client, ABOV_BTNSTATUS, &cmd, 1);
	if (ret < 0){
		input_err(true, &client->dev, "%s fail(%d)\n", __func__, ret);
		abov_touchled_cmd_reserved = 1;
		goto out;
	}
	abov_touchled_cmd_reserved = 0;
	input_info(true, &client->dev, "%s %s\n", __func__, data ? "ON" : "OFF");

out:
	abov_touchkey_led_status = cmd;
	return count;
}

static ssize_t touchkey_threshold_show(struct device *dev,
				struct device_attribute *attr, char *buf)
{
	struct abov_tk_info *info = dev_get_drvdata(dev);
	struct i2c_client *client = info->client;
	u8 r_buf;
	int ret;

	ret = abov_tk_i2c_read(client, ABOV_THRESHOLD, &r_buf, 1);
	if (ret < 0) {
		input_err(true, &client->dev, "%s fail(%d)\n", __func__, ret);
		r_buf = 0;
	}
	return sprintf(buf, "%d\n", r_buf);
}

static void get_diff_data(struct abov_tk_info *info)
{
	struct i2c_client *client = info->client;
	u8 r_buf[6];
	int ret;

	ret = abov_tk_i2c_read(client, ABOV_DIFFDATA, r_buf, 6);
	if (ret < 0) {
		input_err(true, &client->dev, "%s fail(%d)\n", __func__, ret);
		info->menu_s = 0;
		info->back_s = 0;
		info->home_s = 0;
		return;
	}

	info->menu_s = (r_buf[0] << 8) | r_buf[1];
	info->back_s = (r_buf[2] << 8) | r_buf[3];
	info->home_s = (r_buf[4] << 8) | r_buf[5];
}

static ssize_t touchkey_menu_show(struct device *dev,
				struct device_attribute *attr, char *buf)
{
	struct abov_tk_info *info = dev_get_drvdata(dev);

	get_diff_data(info);

	return sprintf(buf, "%d\n", info->menu_s);
}

static ssize_t touchkey_back_show(struct device *dev,
				struct device_attribute *attr, char *buf)
{
	struct abov_tk_info *info = dev_get_drvdata(dev);

	get_diff_data(info);

	return sprintf(buf, "%d\n", info->back_s);
}

static ssize_t touchkey_home_show(struct device *dev,
				struct device_attribute *attr, char *buf)
{
	struct abov_tk_info *info = dev_get_drvdata(dev);

	get_diff_data(info);

	return sprintf(buf, "%d\n", info->home_s);
}


static void get_raw_data(struct abov_tk_info *info)
{
	struct i2c_client *client = info->client;
	u8 r_buf[6];
	int ret;

	ret = abov_tk_i2c_read(client, ABOV_RAWDATA, r_buf, 6);
	if (ret < 0) {
		input_err(true, &client->dev, "%s fail(%d)\n", __func__, ret);
		info->menu_raw = 0;
		info->back_raw = 0;
		info->home_raw = 0;
		return;
	}

	info->menu_raw = (r_buf[0] << 8) | r_buf[1];
	info->back_raw = (r_buf[2] << 8) | r_buf[3];
	info->home_raw = (r_buf[4] << 8) | r_buf[5];
}

static ssize_t touchkey_menu_raw_show(struct device *dev,
				struct device_attribute *attr, char *buf)
{
	struct abov_tk_info *info = dev_get_drvdata(dev);

	get_raw_data(info);

	return sprintf(buf, "%d\n", info->menu_raw);
}

static ssize_t touchkey_back_raw_show(struct device *dev,
				struct device_attribute *attr, char *buf)
{
	struct abov_tk_info *info = dev_get_drvdata(dev);

	get_raw_data(info);

	return sprintf(buf, "%d\n", info->back_raw);
}

static ssize_t touchkey_home_raw_show(struct device *dev,
				struct device_attribute *attr, char *buf)
{
	struct abov_tk_info *info = dev_get_drvdata(dev);

	get_raw_data(info);

	return sprintf(buf, "%d\n", info->home_raw);
}

static ssize_t bin_fw_ver(struct device *dev,
				struct device_attribute *attr, char *buf)
{
	struct abov_tk_info *info = dev_get_drvdata(dev);
	struct i2c_client *client = info->client;

	input_dbg(true, &client->dev, "fw version bin : 0x%x\n", FW_VERSION);

	return sprintf(buf, "0x%02x\n", FW_VERSION);
}

int get_tk_fw_version(struct abov_tk_info *info, bool bootmode)
{
	struct i2c_client *client = info->client;
	u8 buf;
	int ret;
	int retry = 3;

	ret = abov_tk_i2c_read(client, ABOV_FW_VER, &buf, 1);
	if (ret < 0) {
		while (retry--) {
			input_err(true, &client->dev, "%s read fail(%d)\n",
				__func__, retry);
			if (!bootmode)
				abov_tk_reset(info);
			else
				return -1;
			ret = abov_tk_i2c_read(client, ABOV_FW_VER, &buf, 1);
			if (ret == 0)
				break;
		}
		if (retry == 0)
			return -1;
	}

	info->fw_ver = buf;
	input_info(true, &client->dev, "%s : 0x%x\n", __func__, buf);
	return 0;
}

static ssize_t read_fw_ver(struct device *dev,
				struct device_attribute *attr, char *buf)
{
	struct abov_tk_info *info = dev_get_drvdata(dev);
	struct i2c_client *client = info->client;
	int ret;

	if(info->flip_status == FLIP_CLOSE || !info->fw_ver){
		ret = get_tk_fw_version(info, false);
		if (ret < 0) {
			input_err(true, &client->dev, "%s read fail\n", __func__);
			info->fw_ver = 0;
		}
	}

	return sprintf(buf, "0x%02x\n", info->fw_ver);
}

static int abov_load_fw(struct abov_tk_info *info, u8 cmd)
{
	struct i2c_client *client = info->client;
	struct file *fp;
	mm_segment_t old_fs;
	long fsize, nread;
	int ret = 0;

	switch(cmd) {
	case BUILT_IN:
		ret = request_firmware(&info->firm_data_bin,
			TK_FW_PATH_BIN, &client->dev);
		if (ret) {
			input_err(true, &client->dev,
				"%s request_firmware fail(%d)\n", __func__, cmd);
			return ret;
		}
		info->firm_size = info->firm_data_bin->size;
		break;

	case SDCARD:
		old_fs = get_fs();
		set_fs(get_ds());
		fp = filp_open(TK_FW_PATH_SDCARD, O_RDONLY, S_IRUSR);
		if (IS_ERR(fp)) {
			input_err(true, &client->dev,
				"%s %s open error\n", __func__, TK_FW_PATH_SDCARD);
			ret = -ENOENT;
			goto fail_sdcard_open;
		}

		fsize = fp->f_path.dentry->d_inode->i_size;
		info->firm_data_ums = kzalloc((size_t)fsize, GFP_KERNEL);
		if (!info->firm_data_ums) {
			input_err(true, &client->dev,
				"%s fail to kzalloc for fw\n", __func__);
			ret = -ENOMEM;
			goto fail_sdcard_kzalloc;
		}

		nread = vfs_read(fp,
			(char __user *)info->firm_data_ums, fsize, &fp->f_pos);
		if (nread != fsize) {
			input_err(true, &client->dev,
				"%s fail to vfs_read file\n", __func__);
			ret = -EINVAL;
			goto fail_sdcard_size;
		}
		filp_close(fp, current->files);
		set_fs(old_fs);
		info->firm_size = nread;
		break;

	default:
		ret = -1;
		break;
	}
	input_info(true, &client->dev, "fw_size : %lu\n", info->firm_size);
	input_info(true, &client->dev, "%s success\n", __func__);
	return ret;

fail_sdcard_size:
	kfree(&info->firm_data_ums);
fail_sdcard_kzalloc:
	filp_close(fp, current->files);
fail_sdcard_open:
	set_fs(old_fs);
	return ret;
}

static void abov_release_fw(struct abov_tk_info *info, u8 cmd)
{
	switch(cmd) {
	case BUILT_IN:
		release_firmware(info->firm_data_bin);
		break;

	case SDCARD:
		kfree(info->firm_data_ums);
		break;

	default:
		break;
	}
}

static int abov_tk_check_busy(struct abov_tk_info *info)
{
	int ret, count = 0;
	unsigned char val = 0x00;

	do {
		ret = i2c_master_recv(info->client, &val, sizeof(val));

		if (val) {
			count++;
		} else {
			break;
		}

	} while(1);

	if (count > 1000)
		input_err(true, &info->client->dev, "%s: busy %d\n", __func__, count);
	return ret;
}

static int abov_tk_i2c_read_checksum(struct abov_tk_info *info)
{
	unsigned char data[6] = {0xAC, 0x9E, 0x10, 0x00, 0x3F, 0xFF};
	unsigned char checksum[5] = {0, };
	int ret;
	unsigned char reg = 0x00;

	i2c_master_send(info->client, data, 6);

	usleep_range(5 * 1000, 5 * 1000);

	abov_tk_check_busy(info);

	ret = abov_tk_i2c_read(info->client, reg, checksum, 5);

	input_info(true, &info->client->dev, "%s: ret:%d [%X][%X][%X][%X][%X]\n",
			__func__, ret, checksum[0], checksum[1], checksum[2]
			, checksum[3], checksum[4]);
	info->checksum_h = checksum[3];
	info->checksum_l = checksum[4];
	return 0;
}

static int abov_tk_fw_write(struct abov_tk_info *info, unsigned char *addrH,
						unsigned char *addrL, unsigned char *val)
{
	int length = 36, ret = 0;
	unsigned char data[36];

	data[0] = 0xAC;
	data[1] = 0x7A;
	memcpy(&data[2], addrH, 1);
	memcpy(&data[3], addrL, 1);
	memcpy(&data[4], val, 32);

	ret = i2c_master_send(info->client, data, length);
	if (ret != length) {
		input_err(true, &info->client->dev, "%s: write fail[%x%x], %d\n",
		__func__, *addrH, *addrL, ret);
		return ret;
	}

	usleep_range(3 * 1000, 3 * 1000);

	abov_tk_check_busy(info);

	return 0;
}

static int abov_tk_fw_mode_enter(struct abov_tk_info *info)
{
	unsigned char data[3] = {0xAC, 0x5B, 0x2D};
	int ret = 0;

	ret = i2c_master_send(info->client, data, 3);
	if (ret != 3) {
		input_err(true, &info->client->dev,
			"%s: write fail %d\n", __func__, ret);
		return ret;
	}

	return 0;

}

static int abov_tk_fw_update(struct abov_tk_info *info, u8 cmd)
{
	int ret, ii = 0;
	int count;
	unsigned short address;
	unsigned char addrH, addrL;
	unsigned char data[32] = {0, };

	count = info->firm_size / 32;
	address = 0x1000;

	abov_power(info->dtdata, 0);
	msleep(30);
	abov_power(info->dtdata, 1);
	msleep(ABOV_BOOT_DELAY);

	ret = abov_tk_fw_mode_enter(info);

	input_info(true, &info->client->dev,
		"%s: enter BL mode, ret = %d\n", __func__, ret);

	msleep(1100);

	for (ii = 0; ii < count; ii++) {

		addrH = (unsigned char)((address >> 8) & 0xFF);
		addrL = (unsigned char)(address & 0xFF);
		if (cmd == BUILT_IN)
			memcpy(data, &info->firm_data_bin->data[ii * 32], 32);
		else if(cmd == SDCARD)
			memcpy(data, &info->firm_data_ums[ii * 32], 32);

		ret = abov_tk_fw_write(info, &addrH, &addrL, data);
		if (ret < 0) {
			input_err(true, &info->client->dev,
			"%s: err, no device : %d\n", __func__, ret);
			return ret;
		}
		usleep_range(3 * 1000, 3 * 1000);

		abov_tk_check_busy(info);

		address += 0x20;

		memset(data, 0, 32);
	}

	ret = abov_tk_i2c_read_checksum(info);

	return ret;
}

static int abov_flash_fw(struct abov_tk_info *info, bool probe, u8 cmd)
{
	struct i2c_client *client = info->client;
	int retry = 3;
	int ret;

	ret = get_tk_fw_version(info, probe);
	if (ret)
		info->fw_ver = 0;

	ret = abov_load_fw(info, cmd);
	if (ret) {
		input_err(true, &client->dev,
			"%s fw load fail\n", __func__);
		return ret;
	}

	while (retry--) {
		ret = abov_tk_fw_update(info, cmd);
		
		if (ret < 0)
			break;
		if (cmd == BUILT_IN) {
			if ((info->checksum_h != FW_CHECKSUM_H) ||
				(info->checksum_l != FW_CHECKSUM_L)) {
				input_err(true, &client->dev,
					"%s checksum fail. IC(0x%x,0x%x), BN(0x%x,0x%x) retry:%d\n",
					__func__, info->checksum_h, info->checksum_l,
					FW_CHECKSUM_H, FW_CHECKSUM_L, retry);
				ret = -1;
				continue;
			}
		}
		abov_tk_reset_for_bootmode(info);
		msleep(ABOV_RESET_DELAY);
		ret = get_tk_fw_version(info, true);
		if (ret) {
			input_err(true, &client->dev, "%s fw version read fail\n", __func__);
			ret = -1;
			continue;
		}

		if (info->fw_ver == 0) {
			input_err(true, &client->dev, "%s fw version fail (0x%x)\n",
				__func__, info->fw_ver);
			ret = -1;
			continue;
		}

		if ((cmd == BUILT_IN) && (info->fw_ver != FW_VERSION)) {
			input_err(true, &client->dev, "%s fw version fail 0x%x, 0x%x\n",
				__func__, info->fw_ver, FW_VERSION);
			ret = -1;
			continue;
		}
		ret = 0;
		break;
	}

	abov_release_fw(info, cmd);

	return ret;
}

static ssize_t touchkey_fw_update(struct device *dev,
			struct device_attribute *attr, const char *buf, size_t count)
{
	struct abov_tk_info *info = dev_get_drvdata(dev);
	struct i2c_client *client = info->client;
	int ret;
	u8 cmd;

	if (info->fw_update_possible == false) {
		input_err(true, &client->dev, "%s fail (no reset pin)\n", __func__);
		info->fw_update_state = FW_UP_FAILED;
		goto touchkey_fw_update_out;
	}

	switch(*buf) {
	case 's':
	case 'S':
		cmd = BUILT_IN;
		break;
	case 'i':
	case 'I':
		cmd = SDCARD;
		break;
	default:
		info->fw_update_state = FW_UP_FAILED;
		goto touchkey_fw_update_out;
	}

	info->fw_update_state = FW_DOWNLOADING;
	disable_irq(info->irq);
	info->enabled = false;
	ret = abov_flash_fw(info, false, cmd);
	if (info->glovemode)
		abov_glove_mode_enable(client, CMD_GLOVE_ON);
	info->enabled = true;
	enable_irq(info->irq);
	if (ret) {
		input_err(true, &client->dev, "%s fail\n", __func__);
		info->fw_update_state = FW_UP_FAILED;
	} else {
		input_info(true, &client->dev, "%s success\n", __func__);
		info->fw_update_state = FW_UP_SUCCESS;
	}

touchkey_fw_update_out:
	input_dbg(true, &client->dev, "%s : %d\n", __func__, info->fw_update_state);

	return count;
}

static ssize_t touchkey_fw_update_status(struct device *dev,
				struct device_attribute *attr, char *buf)
{
	struct abov_tk_info *info = dev_get_drvdata(dev);
	struct i2c_client *client = info->client;
	int count = 0;

	input_info(true, &client->dev, "%s : %d\n", __func__, info->fw_update_state);

	if (info->fw_update_state == FW_UP_SUCCESS)
		count = sprintf(buf, "PASS\n");
	else if (info->fw_update_state == FW_DOWNLOADING)
		count = sprintf(buf, "Downloading\n");
	else if (info->fw_update_state == FW_UP_FAILED)
		count = sprintf(buf, "Fail\n");

	return count;
}

static ssize_t abov_glove_mode(struct device *dev,
	 struct device_attribute *attr, const char *buf, size_t count)
{
	struct abov_tk_info *info = dev_get_drvdata(dev);
	struct i2c_client *client = info->client;
	int scan_buffer;
	int ret;
	u8 cmd;

	ret = sscanf(buf, "%d", &scan_buffer);
	if (ret != 1) {
		input_err(true, &client->dev, "%s: cmd read err\n", __func__);
		return count;
	}

	if (!(scan_buffer == 0 || scan_buffer == 1)) {
		input_err(true, &client->dev, "%s: wrong command(%d)\n",
			__func__, scan_buffer);
		return count;
	}

	if (!info->enabled)
		return count;

	if (info->glovemode == scan_buffer) {
		input_info(true, &client->dev, "%s same command(%d)\n",
			__func__, scan_buffer);
		return count;
	}

	if (scan_buffer == 1) {
		input_info(true, &client->dev, "%s glove mode\n", __func__);
		cmd = CMD_GLOVE_ON;
	} else {
		input_info(true, &client->dev, "%s normal mode\n", __func__);
		cmd = CMD_GLOVE_OFF;
	}

	ret = abov_glove_mode_enable(client, cmd);
	if (ret < 0) {
		input_err(true, &client->dev, "%s fail(%d)\n", __func__, ret);
		return count;
	}

	info->glovemode = scan_buffer;

	return count;
}

static ssize_t abov_glove_mode_show(struct device *dev,
		struct device_attribute *attr, char *buf)
{
	struct abov_tk_info *info = dev_get_drvdata(dev);

	return sprintf(buf, "%d\n", info->glovemode);
}

static ssize_t abov_wakeup_mode(struct device *dev,
	 struct device_attribute *attr, const char *buf, size_t count)
{
	struct abov_tk_info *info = dev_get_drvdata(dev);
	int scan_buffer;
	int ret;

	ret = sscanf(buf, "%d", &scan_buffer);
	if (ret != 1)
		goto err;

	if (!(scan_buffer == 0 || scan_buffer == 1))
		goto err;

	if(scan_buffer == 1)
		info->wakeup_mode = true;
	else
		info->wakeup_mode = false;

	input_info(true, dev, "%s : Set to %s mode\n",
		__func__, info->wakeup_mode ? "wakeup" : "normal");
err:
	return count;
}

static ssize_t abov_wakeup_mode_show(struct device *dev,
		struct device_attribute *attr, char *buf)
{
	struct abov_tk_info *info = dev_get_drvdata(dev);

	return sprintf(buf, "%d\n", info->wakeup_mode);
}

static ssize_t touchkey_light_version_read(struct device *dev,
		struct device_attribute *attr, char *buf)
{
	struct abov_tk_info *info = dev_get_drvdata(dev);
	int count;
	int crc_cal, crc_efs;

	if (efs_read_light_table_version(info) < 0) {
		count = sprintf(buf, "NG");
		goto out;
	} else {
		if (info->light_version_efs == LIGHT_VERSION) {
			if (!check_light_table_crc(info)) {
				count = sprintf(buf, "NG_CS");
				goto out;
			}
		} else {
			crc_cal = efs_calculate_crc(info);
			crc_efs = efs_read_crc(info);
			input_info(true, &info->client->dev,
					"CRC compare: efs[%d], bin[%d]\n",
					crc_cal, crc_efs);
			if (crc_cal != crc_efs) {
				count = sprintf(buf, "NG_CS");
				goto out;
			}
		}
	}

	count = sprintf(buf, "%s,%s",
			info->light_version_full_efs,
			info->light_version_full_bin);
out:
	input_info(true, &info->client->dev, "%s: %s\n", __func__, buf);
	return count;
}

static ssize_t touchkey_light_update(struct device *dev,
		struct device_attribute *attr,
		const char *buf, size_t size)
{
	struct abov_tk_info *info = dev_get_drvdata(dev);
	int ret;
	int vol_mv;
	int window_type = read_window_type();

	ret = efs_write(info);
	if (ret < 0) {
		input_err(true, &info->client->dev, "%s: fail %d\n", __func__, ret);
		return -EIO;
	}

	vol_mv = efs_read_light_table_with_default(info, window_type);
	if (vol_mv >= LIGHT_VOLTAGE_MIN_VAL) {
		change_touch_key_led_voltage(&info->client->dev, vol_mv);
		input_info(true, &info->client->dev,
				"%s: read done for %d\n", __func__, window_type);
	} else {
		input_err(true, &info->client->dev,
				"%s: fail. voltage is %d\n", __func__, vol_mv);
	}

	return size;
}

static ssize_t touchkey_light_id_compare(struct device *dev,
		struct device_attribute *attr, char *buf)
{
	struct abov_tk_info *info = dev_get_drvdata(dev);
	int count, ret;
	int window_type = read_window_type();
	int crc_cal, crc_efs;

	if (window_type < 0) {
		input_err(true, &info->client->dev,
				"%s: window_type:%d, NG\n", __func__, window_type);
		return sprintf(buf, "NG");
	}

	ret = efs_read_light_table(info, window_type);
	if (ret < 0) {
		count = sprintf(buf, "NG");
	} else {
		crc_cal = efs_calculate_crc(info);
		crc_efs = efs_read_crc(info);
		input_info(true, &info->client->dev,
				"EFS CRC compare: efs[%d], bin[%d]\n",
				crc_cal, crc_efs);
		if (crc_cal != crc_efs) {
			count = sprintf(buf, "NG_CS");
		}else{
			count = sprintf(buf, "OK");
		}	
	}

	input_info(true, &info->client->dev,
			"%s: window_type:%d, %s\n", __func__, window_type, buf);
	return count;
}

static char* tokenizer(char* str, char delim)
{
	static char* str_addr;
	char* token = NULL;

	if (str != NULL)
		str_addr = str;
	else if (str_addr == NULL)
		return 0;

	token = str_addr;
	while (true) {
		if (!(*str_addr)) {
			break;
		} else if (*str_addr == delim) {
			*str_addr = '\0';
			str_addr = str_addr + 1;
			break;
		}
		str_addr++;
	}

	return token;
}

static ssize_t touchkey_light_table_write(struct device *dev,
		struct device_attribute *attr,
		const char *buf, size_t size)
{
	struct abov_tk_info *info = dev_get_drvdata(dev);
	struct light_info table[16];
	int ret;
	int vol_mv;
	int window_type;
	char *full_version;
	char data[150] = {0, };
	int i, crc, crc_cal;
	char *octa_id;
	int table_size = 0;

	snprintf(data, sizeof(data), buf);

	input_info(true, &info->client->dev, "%s: %s\n",
			__func__, data);

	full_version = tokenizer(data, ',');
	if (!full_version)
		return -EIO;

	table_size = (int)strlen(full_version) - 8;
	input_info(true, &info->client->dev, "%s: version:%s size:%d\n",
			__func__, full_version, table_size);
	if (table_size < 0 || table_size > 16) {
		input_err(true, &info->client->dev, "%s: table_size is unavailable\n", __func__);
		return -EIO;
	}

	if (kstrtoint(tokenizer(NULL, ','), 0, &crc))
		return -EIO;

	input_info(true, &info->client->dev, "%s: crc:%d\n",
			__func__, crc);
	if (!crc)
		return -EIO;

	for (i = 0; i < table_size; i++) {
		octa_id = tokenizer(NULL, '_');
		if (!octa_id)
			break;

		if (octa_id[0] >= 'A')
			table[i].octa_id = octa_id[0] - 'A' + 0x0A;
		else
			table[i].octa_id = octa_id[0] - '0';
		if (table[i].octa_id < 0 || table[i].octa_id > 0x0F)
			break;
		if (kstrtoint(tokenizer(NULL, ','), 0, &table[i].vol_mv))
			break;
	}

	if (!table_size) {
		input_err(true, &info->client->dev, "%s: no data in table\n", __func__);
		return -ENODATA;
	}

	for (i = 0; i < table_size; i++) {
		input_info(true, &info->client->dev, "%s: [%d] %X - %dmv\n",
				__func__, i, table[i].octa_id, table[i].vol_mv);
	}

	/* write efs */
	ret = efs_write_light_table_version(info, full_version);
	if (ret < 0) {
		input_err(true, &info->client->dev,
				"%s: failed to write table ver %s. %d\n",
				__func__, full_version, ret);
		return ret;
	}

	info->light_version_efs = pick_light_table_version(full_version);

	for (i = 0; i < table_size; i++) {
		ret = efs_write_light_table(info, table[i]);
		if (ret < 0)
			break;
	}
	if (ret < 0) {
		input_err(true, &info->client->dev,
				"%s: failed to write table%d. %d\n",
				__func__, i, ret);
		return ret;
	}

	ret = efs_write_light_table_crc(info, crc);
	if (ret < 0) {
		input_err(true, &info->client->dev,
				"%s: failed to write table crc. %d\n",
				__func__, ret);
		return ret;
	}

	crc_cal = efs_calculate_crc(info);
	input_info(true, &info->client->dev,
			"%s: efs crc:%d, caldulated crc:%d\n",
			__func__, crc, crc_cal);
	if (crc_cal != crc)
		return -EIO;

	window_type = read_window_type();
	vol_mv = efs_read_light_table_with_default(info, window_type);
	if (vol_mv >= LIGHT_VOLTAGE_MIN_VAL) {
		change_touch_key_led_voltage(&info->client->dev, vol_mv);
		input_info(true, &info->client->dev,
				"%s: read done for %d\n", __func__, window_type);
	} else {
		input_err(true, &info->client->dev,
				"%s: fail. voltage is %d\n", __func__, vol_mv);
	}

	return size;
}


static DEVICE_ATTR(touchkey_threshold, S_IRUGO, touchkey_threshold_show, NULL);
static DEVICE_ATTR(brightness, S_IRUGO | S_IWUSR | S_IWGRP, NULL,
			touchkey_led_control);
static DEVICE_ATTR(touchkey_recent, S_IRUGO, touchkey_menu_show, NULL);
static DEVICE_ATTR(touchkey_back, S_IRUGO, touchkey_back_show, NULL);
static DEVICE_ATTR(touchkey_home, S_IRUGO, touchkey_home_show, NULL);
static DEVICE_ATTR(touchkey_recent_raw, S_IRUGO, touchkey_menu_raw_show, NULL);
static DEVICE_ATTR(touchkey_back_raw, S_IRUGO, touchkey_back_raw_show, NULL);
static DEVICE_ATTR(touchkey_home_raw, S_IRUGO, touchkey_home_raw_show, NULL);
static DEVICE_ATTR(touchkey_firm_version_phone, S_IRUGO, bin_fw_ver, NULL);
static DEVICE_ATTR(touchkey_firm_version_panel, S_IRUGO, read_fw_ver, NULL);
static DEVICE_ATTR(touchkey_firm_update, S_IRUGO | S_IWUSR | S_IWGRP, NULL,
			touchkey_fw_update);
static DEVICE_ATTR(touchkey_firm_update_status, S_IRUGO | S_IWUSR | S_IWGRP,
			touchkey_fw_update_status, NULL);
static DEVICE_ATTR(glove_mode, S_IRUGO | S_IWUSR | S_IWGRP,
			abov_glove_mode_show, abov_glove_mode);
static DEVICE_ATTR(two_touch_wakeup_mode, S_IRUGO | S_IWUSR | S_IWGRP,
			abov_wakeup_mode_show, abov_wakeup_mode);
#ifdef CONFIG_INPUT_BOOSTER
static DEVICE_ATTR(boost_level,
		   S_IWUSR | S_IWGRP, NULL, boost_level_store);
#endif
static DEVICE_ATTR(touchkey_brightness, S_IRUGO | S_IWUSR | S_IWGRP, NULL, brightness_control);
static DEVICE_ATTR(touchkey_light_version, S_IRUGO, touchkey_light_version_read, NULL);
static DEVICE_ATTR(touchkey_light_update, S_IWUSR | S_IWGRP, NULL, touchkey_light_update);
static DEVICE_ATTR(touchkey_light_id_compare, S_IRUGO, touchkey_light_id_compare, NULL);
static DEVICE_ATTR(touchkey_light_table_write, S_IWUSR | S_IWGRP, NULL, touchkey_light_table_write);

static struct attribute *sec_touchkey_attributes[] = {
	&dev_attr_touchkey_threshold.attr,
	&dev_attr_brightness.attr,
	&dev_attr_touchkey_recent.attr,
	&dev_attr_touchkey_back.attr,
	&dev_attr_touchkey_home.attr,
	&dev_attr_touchkey_recent_raw.attr,
	&dev_attr_touchkey_back_raw.attr,
	&dev_attr_touchkey_home_raw.attr,
	&dev_attr_touchkey_firm_version_phone.attr,
	&dev_attr_touchkey_firm_version_panel.attr,
	&dev_attr_touchkey_firm_update.attr,
	&dev_attr_touchkey_firm_update_status.attr,
	&dev_attr_glove_mode.attr,
	&dev_attr_two_touch_wakeup_mode.attr,
#ifdef CONFIG_INPUT_BOOSTER
	&dev_attr_boost_level.attr,
#endif
	&dev_attr_touchkey_brightness.attr,
	&dev_attr_touchkey_light_version.attr,
	&dev_attr_touchkey_light_update.attr,
	&dev_attr_touchkey_light_id_compare.attr,
	&dev_attr_touchkey_light_table_write.attr,
	NULL,
};

static struct attribute_group sec_touchkey_attr_group = {
	.attrs = sec_touchkey_attributes,
};
static int abov_tk_fw_check(struct abov_tk_info *info)
{
	struct i2c_client *client = info->client;
	int ret;

#ifdef CRC_CHECK_WITHBOOTING
	u8 checksum_cmd;
	int i;

	ret = abov_tk_i2c_read(client, ABOV_FW_VER, &checksum_cmd, 1);
	if (ret < 0) {
		input_err(true, &client->dev, "%s fail(%d)\n", __func__, ret);
		checksum_cmd = 0;
	}

	input_info(true, &client->dev, "%s FW ver = 0x%x\n", __func__, checksum_cmd);

	/* CRC Check */
	if(checksum_cmd >= CRC_CHECK_FW_VER) {
		checksum_cmd = CMD_CRC_CHECK_START;
		ret = abov_tk_i2c_write(client, ABOV_FW_VER, &checksum_cmd, 1);
		if (ret < 0) {
			input_err(true, &client->dev,
				"%s %d fail(%d)\n", __func__, __LINE__, ret);
			checksum_cmd = 0;
		}

		msleep(40);

		for (i = 0; i < 30; i++) {
			/* Read 0x01 to check pass */
			ret = abov_tk_i2c_read(client, ABOV_FW_VER, &checksum_cmd, 1);
			if (ret < 0) {
				input_err(true, &client->dev,
					"%s %d fail(%d)\n", __func__, __LINE__, ret);
				checksum_cmd = 0;
			}

			if (checksum_cmd != CMD_CRC_CHECK_START) {
				input_info(true, &client->dev,
					"%s checksum result = 0x%x, %d\n",
					__func__, checksum_cmd, i);
				break;
			}
			usleep_range(5 * 1000, 5 * 1000);
		}
	} else {
		msleep(80);
	}
#endif

	ret = get_tk_fw_version(info, true);
	if (ret) {
		input_err(true, &client->dev,
			"%s: i2c fail...[%d], addr[%d]\n",
			__func__, ret, info->client->addr);
		}

	if (!info->fw_update_possible)
		return ret;

	if (ret || info->fw_ver < FW_VERSION || info->fw_ver > 0xf0) {
		input_err(true, &client->dev, "excute tk firmware update (0x%x -> 0x%x\n",
			info->fw_ver, FW_VERSION);
		ret = abov_flash_fw(info, true, BUILT_IN);
		if (ret) {
			input_err(true, &client->dev,
				"failed to abov_flash_fw (%d)\n", ret);
		} else {
			input_info(true, &client->dev,
				"fw update success\n");
		}
	}

	return ret;
}
int abov_power(struct abov_touchkey_dt_data *dtdata, bool on)
{
	static bool reg_boot_on = true;
	int ret = 0;

	if (reg_boot_on && !dtdata->reg_boot_on)
		reg_boot_on = false;

	/* SUB LED 3P0 off before IC VDD or it will blink one time when LCD off.*/
	if (!IS_ERR_OR_NULL(dtdata->led_avdd_vreg)) {
		if (!on) {
			if (regulator_is_enabled((dtdata->led_avdd_vreg))) {
				ret = regulator_disable(dtdata->led_avdd_vreg);
				} else
					pr_err("%s: LED 3p3 is already disabled\n", __func__);
			}

		if (ret)
			pr_err("%s %s: led_avdd_vreg reg %s fail ret=%d\n",
				SECLOG, __func__, on ? "enable" : "disable", ret);
	}

	/* 3.3V on,off control. */
	if (!IS_ERR_OR_NULL(dtdata->avdd_vreg)) {
		if (on) {
			if (!reg_boot_on && regulator_is_enabled(dtdata->avdd_vreg)) {
				pr_err("%s %s: 3p3 is already enabled\n", SECLOG, __func__);
			} else {
				ret = regulator_enable(dtdata->avdd_vreg);
				if (ret) {
					pr_err("%s %s: 3p3 enable fail ret=%d\n",SECLOG, __func__, ret);
				}
			}
			reg_boot_on = false;
		} else {
			if (regulator_is_enabled(dtdata->avdd_vreg)) {
				ret = regulator_disable(dtdata->avdd_vreg);
				if (ret) {
					pr_err("%s %s: 3p3 disable fail ret=%d\n",
						SECLOG,__func__, ret);
				}
			} else {
				pr_err("%s %s: 3p3 is already disabled\n",
					SECLOG, __func__);
			}
		}
	}

	/* 1.8V on,off control. */
	if (!IS_ERR_OR_NULL(dtdata->vdd_io_vreg)) {
		if (on)
			ret = regulator_enable(dtdata->vdd_io_vreg);
		else
			ret = regulator_disable(dtdata->vdd_io_vreg);

		if (ret)
			pr_err("%s %s: iovdd reg %s fail ret=%d\n", SECLOG,
				__func__, on ? "enable" : "disable", ret);
	}

	pr_info("%s %s %s", SECLOG, __func__, on ? "ON" : "OFF");
	if (!IS_ERR_OR_NULL(dtdata->avdd_vreg))
		pr_cont(", avdd is %s",
			regulator_is_enabled(dtdata->avdd_vreg) ? "ON" : "OFF");
	if (!IS_ERR_OR_NULL(dtdata->vdd_io_vreg))
		pr_cont(", vdd_io is %s", 
			regulator_is_enabled(dtdata->vdd_io_vreg) ? "ON" : "OFF");
	pr_cont("\n");

/*
	if (gpio_is_valid(dtdata->gpio_tkey_led_en)) {
		gpio_direction_output(dtdata->gpio_tkey_led_en, on);
		pr_info("%s %s: %s: %d\n", SECLOG, __func__, on ? "on" : "off",
			gpio_get_value(dtdata->gpio_tkey_led_en));
	}
*/	
	/* SUB LED 3P0 on,off control.*/ 
	if (!IS_ERR_OR_NULL(dtdata->led_avdd_vreg)) {
		if (on){
			if (regulator_is_enabled((dtdata->led_avdd_vreg))) {
				pr_err("%s: 3p3 is already enabled\n", __func__);
			} else
				ret = regulator_enable(dtdata->led_avdd_vreg);
		}

		if (ret)
			pr_err("%s %s: led_avdd_vreg reg %s fail ret=%d\n",
				SECLOG, __func__, on ? "enable" : "disable", ret);
	}

	return ret;
}

static int abov_pinctrl_configure(struct abov_tk_info *info,
							bool active)
{
	struct pinctrl_state *set_state;
	int retval;

	if (active) {
		set_state =
			pinctrl_lookup_state(info->pinctrl,
						"touchkey_active");
		if (IS_ERR(set_state)) {
			input_err(true, &info->client->dev,
				"%s: cannot get ts pinctrl active state\n", __func__);
			return PTR_ERR(set_state);
		}
	} else {
		set_state =
			pinctrl_lookup_state(info->pinctrl,
						"touchkey_suspend");
		if (IS_ERR(set_state)) {
			input_err(true, &info->client->dev,
				"%s: cannot get gpiokey pinctrl sleep state\n", __func__);
			return PTR_ERR(set_state);
		}
	}
	retval = pinctrl_select_state(info->pinctrl, set_state);
	if (retval) {
		input_err(true, &info->client->dev,
			"%s: cannot set ts pinctrl active state\n", __func__);
		return retval;
	}

	return 0;
}

int abov_gpio_reg_init(struct device *dev,
			struct abov_touchkey_dt_data *dtdata)
{
	int ret = 0;

	if (gpio_is_valid(dtdata->gpio_int)) {
		ret = gpio_request(dtdata->gpio_int, "tkey_gpio_int");
		if (ret < 0) {
			input_err(true, dev, "unable to request gpio_int\n");
			return ret;
		}
	}

	if (gpio_is_valid(dtdata->gpio_tkey_led_en)) {
		ret = gpio_request(dtdata->gpio_tkey_led_en, "gpio_tkey_led_en");
		if (ret < 0) {
			input_err(true, dev, "unable to request gpio_tkey_led_en\n");
			ret = 0;
		}
	}

	if (gpio_is_valid(dtdata->sub_det)) {
		ret = gpio_request(dtdata->sub_det, "gpio_tkey_sub_det");
		if (ret < 0) {
			input_err(true, dev, "unable to request gpio_tkey_sub_det\n");
			ret = 0;
		}
	}

	dtdata->vdd_io_vreg = devm_regulator_get(dev, "abov,vddo");
	if (IS_ERR(dtdata->vdd_io_vreg)) {
		dtdata->vdd_io_vreg = NULL;
		input_err(true, dev, "dtdata->vdd_io_vreg get error, ignoring\n");
	} else {
		regulator_set_voltage(dtdata->vdd_io_vreg, 1800000, 1800000);
	}

	dtdata->avdd_vreg = devm_regulator_get(dev, "abov,avdd");
	if (IS_ERR(dtdata->avdd_vreg)) {
		input_err(true, dev, "dtdata->avdd_vreg get error\n");
		ret = -ENODEV;
	} else {
		regulator_set_voltage(dtdata->avdd_vreg, 3300000, 3300000);
	}
	
	dtdata->led_avdd_vreg = devm_regulator_get(dev, "abov,ledavdd");
	if (IS_ERR(dtdata->led_avdd_vreg)) {
		input_err(true, dev, "dtdata->ledavdd get error\n");
		ret = -ENODEV;
	} else {
		regulator_set_voltage(dtdata->led_avdd_vreg, 3300000, 3300000);
	}

	dtdata->power = abov_power;

	return ret;
}

#ifdef CONFIG_OF
static int abov_parse_dt(struct device *dev,
			struct abov_touchkey_dt_data *dtdata)
{
	struct device_node *np = dev->of_node;
	int rc;

	dtdata->gpio_int = of_get_named_gpio(np, "abov,irq-gpio", 0);
	if (!gpio_is_valid(dtdata->gpio_int)) {
		input_err(true, dev, "unable to get gpio_int\n");
		return dtdata->gpio_int;
	}

	dtdata->gpio_scl = of_get_named_gpio(np, "abov,scl-gpio", 0);
	if (!gpio_is_valid(dtdata->gpio_scl)) {
		input_err(true, dev, "unable to get gpio_scl\n");
		return dtdata->gpio_scl;
	}

	dtdata->gpio_sda = of_get_named_gpio(np, "abov,sda-gpio", 0);
	if (!gpio_is_valid(dtdata->gpio_sda)) {
		input_err(true, dev, "unable to get gpio_sda\n");
		return dtdata->gpio_sda;
	}

	dtdata->sub_det = of_get_named_gpio(np, "abov,sub-det", 0);
	if (!gpio_is_valid(dtdata->sub_det)) {
		input_info(true, dev, "unable to get sub_det\n");
	} else {
		input_info(true, dev, "%s: sub_det:%d\n",__func__, dtdata->sub_det);
	}
	
	dtdata->gpio_hall = of_get_named_gpio(np, "abov,hall_flip-gpio", 0);
	if(dtdata->gpio_hall < 0){
		input_err(true, dev, "unable to get gpio_hall\n");
	}
/*
	dtdata->gpio_tkey_led_en = of_get_named_gpio(np, "abov,tkey_led_en-gpio", 0);
	if (!gpio_is_valid(dtdata->gpio_tkey_led_en)) {
		input_err(true, dev, "unable to get gpio_tkey_led_en...ignoring\n");
	}
*/	

	dtdata->reg_boot_on = of_property_read_bool(np, "abov,reg-boot-on");

	rc = of_property_read_string(np, "abov,fw-name", &dtdata->fw_name);
	if (rc < 0) {
		input_info(true, dev, "%s: Unable to read abov,fw_name\n", __func__);
		return rc;
	}
	
	dtdata->forced_update = of_property_read_bool(np, "abov,forced-update");
	input_info(true, dev, "%s: gpio_int:%d, gpio_scl:%d,"
		" gpio_sda:%d, gpio_led_en:%d, gpio_hall:%d, reg_boot_on:%d, fw_name:%s, %s\n",
			__func__, dtdata->gpio_int, dtdata->gpio_scl,
			dtdata->gpio_sda, dtdata->gpio_tkey_led_en, dtdata->gpio_hall,
			dtdata->reg_boot_on, dtdata->fw_name,
			dtdata->forced_update ? "F" : "");

	return 0;
}
#else
static int abov_parse_dt(struct device *dev,
			struct abov_touchkey_dt_data *dtdata)
{
	return -ENODEV;
}
#endif

static int abov_tk_probe(struct i2c_client *client,
				  const struct i2c_device_id *id)
{
	struct abov_tk_info *info;
	struct input_dev *input_dev;
	int ret = 0;
	int i;
	char tmp[2] = {0, };

	input_info(true, &client->dev, "%s start\n",__func__);

	/* SUB LCD broken case -> Unloading TKEY driver because Main OCTA doesn't have TKEY module */
	if (get_lcd_attached_secondary("GET") == 0) {
			input_err(true, &client->dev, "%s : get_lcd_attached()=0 \n", __func__);
			return -EIO;
	}

	if (!i2c_check_functionality(client->adapter, I2C_FUNC_I2C)) {
		input_err(true, &client->dev,
			"i2c_check_functionality fail\n");
		return -EIO;
	}

	info = kzalloc(sizeof(struct abov_tk_info), GFP_KERNEL);
	if (!info) {
		input_err(true, &client->dev, "Failed to allocate memory\n");
		ret = -ENOMEM;
		goto err_alloc;
	}

	input_dev = input_allocate_device();
	if (!input_dev) {
		input_err(true, &client->dev,
			"Failed to allocate memory for input device\n");
		ret = -ENOMEM;
		goto err_input_alloc;
	}

	info->probe_done = false;
#ifdef CONFIG_DUAL_TSP
	abov_set_tkey_info(info);
	info->flip_status = -1;
	info->flip_status_current = -1;
#endif
	info->client = client;
	info->input_dev = input_dev;
	info->wakeup_mode = false;

	if (client->dev.of_node) {
		struct abov_touchkey_dt_data *dtdata;
		dtdata = devm_kzalloc(&client->dev,
			sizeof(struct abov_touchkey_dt_data), GFP_KERNEL);
		if (!dtdata) {
			input_err(true, &client->dev, "Failed to allocate memory\n");
			return -ENOMEM;
		}

		ret = abov_parse_dt(&client->dev, dtdata);
		if (ret)
			return ret;

		info->dtdata = dtdata;
	} else
		info->dtdata = client->dev.platform_data;

	if (info->dtdata == NULL) {
		input_err(true, &client->dev, "failed to get platform data\n");
		goto err_config;
	}

	/* Get pinctrl if target uses pinctrl */
	info->pinctrl = devm_pinctrl_get(&client->dev);
	if (IS_ERR(info->pinctrl)) {
		if (PTR_ERR(info->pinctrl) == -EPROBE_DEFER)
			return -EPROBE_DEFER;

		input_err(true, &client->dev,
			"%s: Target does not use pinctrl\n", __func__);
		info->pinctrl = NULL;
	}

	if (info->pinctrl) {
		ret = abov_pinctrl_configure(info, true);
		if (ret)
			input_err(true, &client->dev,
				"%s: cannot set ts pinctrl active state\n", __func__);
	}

	ret = abov_gpio_reg_init(&client->dev, info->dtdata);
	if(ret){
		input_err(true, &client->dev, "failed to init reg\n");
		goto pwr_config;
	}

	if (ret) {
		input_err(true, &client->dev, "failed to init reg\n");
		goto pwr_config;
	}
	if (info->dtdata->power)
		info->dtdata->power(info->dtdata, true);

	if (!info->dtdata->reg_boot_on)
		msleep(ABOV_RESET_DELAY);

	client->irq = gpio_to_irq(info->dtdata->gpio_int);

	info->irq = -1;
	mutex_init(&info->lock);
	wake_lock_init(&info->report_wake_lock, WAKE_LOCK_SUSPEND, "report_wake_lock");

	info->fw_update_possible = true;


	info->touchkey_count = sizeof(touchkey_keycode) / sizeof(int);
	i2c_set_clientdata(client, info);
	ret = abov_tk_fw_check(info);
	if (ret) {
		input_err(true, &client->dev,
			"failed to firmware check (%d)\n", ret);
		goto err_reg_input_dev;
	}
	snprintf(info->phys, sizeof(info->phys),
		 "%s/input0", dev_name(&client->dev));
	input_dev->name = "sec_touchkey";
	input_dev->phys = info->phys;
	input_dev->id.bustype = BUS_HOST;
	input_dev->dev.parent = &client->dev;
	input_dev->open = abov_tk_input_open;
	input_dev->close = abov_tk_input_close;
	set_bit(EV_KEY, input_dev->evbit);
	set_bit(KEY_RECENT, input_dev->keybit);
	set_bit(KEY_BACK, input_dev->keybit);
	set_bit(KEY_HOMEPAGE, input_dev->keybit);
	//set_bit(KEY_TKEY_WAKEUP, input_dev->keybit);
	set_bit(EV_LED, input_dev->evbit);
	set_bit(LED_MISC, input_dev->ledbit);
	input_set_drvdata(input_dev, info);

	ret = input_register_device(input_dev);
	if (ret) {
		input_err(true, &client->dev, "failed to register input dev (%d)\n",
			ret);
		goto err_reg_input_dev;
	}

	info->enabled = true;

	if (!info->dtdata->irq_flag) {
		ret = request_threaded_irq(client->irq, NULL, abov_tk_interrupt,
			IRQF_TRIGGER_LOW | IRQF_ONESHOT, ABOV_TK_NAME, info);
	} else {
		ret = request_threaded_irq(client->irq, NULL, abov_tk_interrupt,
			info->dtdata->irq_flag, ABOV_TK_NAME, info);
	}
	if (ret < 0) {
		input_err(true, &client->dev, "Failed to register interrupt %d\n", ret);
		goto err_req_irq;
	}
	info->irq = client->irq;

#if defined(CONFIG_DRV_SAMSUNG)
	sec_touchkey = sec_device_create(SEC_CLASS_DEVT_TKEY, info, "sec_touchkey");
#else
	sec_touchkey = device_create(sec_class,
		NULL, 0, info, "sec_touchkey");
#endif
	if (IS_ERR(sec_touchkey))
		input_err(true, &client->dev,
		"Failed to create device for the touchkey sysfs\n");

	ret = sysfs_create_group(&sec_touchkey->kobj,
		&sec_touchkey_attr_group);
	if (ret)
		input_err(true, &client->dev, "Failed to create sysfs group\n");

	ret = sysfs_create_link(&sec_touchkey->kobj,
		&info->input_dev->dev.kobj, "input");
	if (ret < 0) {
		input_err(true, &info->client->dev,
			"%s: Failed to create input symbolic link\n",
			__func__);
	}

#ifdef CONFIG_INPUT_BOOSTER
	info->tkey_booster = input_booster_allocate(INPUT_BOOSTER_ID_TKEY);
	if (!info->tkey_booster) {
		input_err(true, &client->dev,
			"%s: Failed to alloc mem for tsp_booster\n", __func__);
		goto err_get_tkey_booster;
	}
#endif
#ifdef CONFIG_DUAL_TSP
	if(info->dtdata->gpio_hall < 0)
		/* default set : tkey enable */
		info->flip_status = FLIP_CLOSE;
	else
		info->flip_status = !(gpio_get_value(info->dtdata->gpio_hall));
	input_info(true, &client->dev, "%s: Folder is %sed now.\n",
		__func__, info->flip_status ? "clos":"open");


	/* MAIN LCD broken case -> Always Enable TKEY */
	if (get_lcd_attached("GET") == 0) {
		info->flip_status = FLIP_CLOSE;
		input_info(true, &client->dev, "%s: Main LCD is broken, Always Enable TKEY\n", __func__);
	}

	if (!info->flip_status)
		abov_tk_suspend(&client->dev);
#endif

	info->probe_done = true;

#ifdef CONFIG_DUAL_TSP
	INIT_DELAYED_WORK(&info->switching_work, abov_switching_tkey_work);
	/* Hall IC notify priority -> ftn -> register */
	info->hall_ic_nb.priority = 9;
	info->hall_ic_nb.notifier_call = abov_hall_ic_notify;
	hall_ic_register_notify(&info->hall_ic_nb);
#endif

	INIT_DELAYED_WORK(&info->efs_open_work, touchkey_efs_open_work);

	info->light_table_crc = LIGHT_VERSION;
	sprintf(info->light_version_full_bin, "T%d.", LIGHT_VERSION);
	for (i = 0; i < LIGHT_TABLE_MAX; i++) {
		info->light_table_crc += tkey_light_voltage_table[i].octa_id;
		info->light_table_crc += tkey_light_voltage_table[i].vol_mv;
		snprintf(tmp, 2, "%X", tkey_light_voltage_table[i].octa_id);
		strncat(info->light_version_full_bin, tmp, 1);
	}
	input_info(true, &client->dev, "%s: light version of kernel : %s\n",
			__func__, info->light_version_full_bin);

	//schedule_delayed_work(&info->efs_open_work, msecs_to_jiffies(2000));

	input_info(true, &info->client->dev, "%s done\n", __func__);
	return 0;

#ifdef CONFIG_INPUT_BOOSTER
/* Delete annotation when use this code later */
	//input_booster_free(info->tkey_booster);
	//info->tkey_booster = NULL;
err_get_tkey_booster:
#endif
err_req_irq:
	input_unregister_device(input_dev);
err_reg_input_dev:
	wake_lock_destroy(&info->report_wake_lock);
	mutex_destroy(&info->lock);
pwr_config:
err_config:
#ifdef CONFIG_DUAL_TSP
	tkey_driver = NULL;
#endif
	input_free_device(input_dev);
err_input_alloc:
	kfree(info);
err_alloc:
	return ret;

}

static int abov_tk_remove(struct i2c_client *client)
{
	struct abov_tk_info *info = i2c_get_clientdata(client);

/*	if (info->enabled)
		info->dtdata->power(0);
*/
	info->enabled = false;
	if (info->irq >= 0)
		free_irq(info->irq, info);
	input_unregister_device(info->input_dev);
	input_free_device(info->input_dev);
#ifdef CONFIG_INPUT_BOOSTER
	input_booster_free(info->tkey_booster);
	info->tkey_booster = NULL;
#endif
	kfree(info);

	return 0;
}

static void abov_tk_shutdown(struct i2c_client *client)
{
	struct abov_tk_info *info = i2c_get_clientdata(client);
	u8 cmd = CMD_LED_OFF;

	info->enabled = false;

#ifdef CONFIG_DUAL_TSP
	hall_ic_unregister_notify(&info->hall_ic_nb);
#endif
	cancel_delayed_work(&info->efs_open_work);
	abov_tk_i2c_write(client, ABOV_BTNSTATUS, &cmd, 1);
	input_err(true, &info->client->dev, "%s: done \n", __func__);
}

static int abov_tk_suspend(struct device *dev)
{
	struct i2c_client *client = to_i2c_client(dev);
	struct abov_tk_info *info = i2c_get_clientdata(client);
	u8 cmd;
	int ret;

	if (!info->wakeup_state)
		if (!info->enabled)
			return 0;

	printk("Inside abov_tk_suspend \n");
	input_dbg(true, &info->client->dev, "%s: users=%d\n", __func__,
		   info->input_dev->users);

	if (info->wakeup_mode && info->flip_status == FLIP_CLOSE){
		/*Enter WAKEUP mode*/
		cmd = CMD_STOP_MODE;
		ret = abov_tk_i2c_write(client, ABOV_BTNSTATUS, &cmd, 1);
		if (ret < 0){
			input_err(true, dev,
				"%s : failed to write wakeup mode(%d)\n",
				__func__, ret);
			return 0;
		}
		input_info(true, dev,
			"%s : success to enter tkey wakeup state\n",
			__func__);
		release_all_fingers(info);
		enable_irq_wake(info->irq);
		info->enabled = false;
		info->wakeup_state = true;
	}
	else {
		disable_irq(info->irq);
		info->enabled = false;
		info->wakeup_state = false;
		release_all_fingers(info);

		info->dtdata->power(info->dtdata, false);
	}
	return 0;
}

static int abov_tk_resume(struct device *dev)
{
	struct i2c_client *client = to_i2c_client(dev);
	struct abov_tk_info *info = i2c_get_clientdata(client);
	u8 led_data;

	if (info->enabled)
		return 0;
#ifdef CONFIG_DUAL_TSP

	/* Don't follow below condition when Main LCD broken case */
	if ( (info->flip_status == FLIP_OPEN) && (get_lcd_attached("GET") != 0) ){
		input_err(true, &info->client->dev,
			"%s: flip is opened, so resume is ignored.\n", __func__);
		return 0;
	}
#endif
	printk("Inside abov_tk_resume \n");

	input_dbg(true, &info->client->dev, "%s: users=%d\n", __func__,
		   info->input_dev->users);

	if (info->wakeup_state){
		input_info(true, dev, "%s: tkey wakeup\n", __func__);
		disable_irq_wake(info->irq);
		info->wakeup_state = false;
		disable_irq(info->irq);
		release_all_fingers(info);

		info->dtdata->power(info->dtdata, false);
		usleep_range(5 * 1000, 5 * 1000);
	}

	info->dtdata->power(info->dtdata, true);
	msleep(ABOV_RESET_DELAY);
	info->enabled = true;

	if (info->glovemode)
		abov_glove_mode_enable(client, CMD_GLOVE_ON);

	if (abov_touchled_cmd_reserved && \
		abov_touchkey_led_status == CMD_LED_ON) {
		abov_touchled_cmd_reserved = 0;
		led_data = abov_touchkey_led_status;

		abov_tk_i2c_write(client, ABOV_BTNSTATUS, &led_data, 1);

		input_info(true, &info->client->dev, "%s: LED reserved on\n", __func__);
	}

	enable_irq(info->irq);

	return 0;
}

#ifdef CONFIG_DUAL_TSP
static void abov_switching_tkey_work(struct work_struct *work)
{
	struct abov_tk_info *info =
			container_of(work, struct abov_tk_info,
			switching_work.work);

	/* No switching when MAIN LCD broken case */
	if (get_lcd_attached("GET") == 0) {
		input_err(true, &info->client->dev,
			"%s : LCD is broken so don't try to switching \n",
			__func__);
		return;
	}

	if (info == NULL){
		input_err(true, &info->client->dev, "%s: tkey info is null\n", __func__);
		return;
	}

	if (!info->probe_done){
		input_err(true, &info->client->dev,
			"%s: touchkey probe is not done yet\n",
			__func__);
		return;
	}

	if(info->fw_update_state == FW_DOWNLOADING){
		input_err(true, &info->client->dev,
			"%s: tk fw update is running. switching is ignored.\n",
			__func__);
		return;
	}

	input_info(true, &info->client->dev,
		"%s : flip: %d(now) change to %d, tk_enabled : %d\n",
		__func__,info->flip_status, info->flip_status_current, info->enabled);

	if (info->flip_status != info->flip_status_current)
	{
		info->flip_status = info->flip_status_current;
		if (info->flip_status_current == FLIP_CLOSE) {
			input_info(true, &info->client->dev,
				"%s : flip closed. tkey must be resumed.\n",
				__func__);
			abov_tk_resume(&info->client->dev);
		}
		else {
			input_info(true, &info->client->dev,
				"%s : flip opened. tkey must be suspended\n",
				__func__);
			abov_tk_suspend(&info->client->dev);
		}
	}
}

static int abov_hall_ic_notify(struct notifier_block *nb,
				unsigned long flip_cover, void *v)
{
	struct abov_tk_info *info = abov_get_tkey_info();

	input_info(true, &info->client->dev, "%s %s\n", __func__, flip_cover ? "close" : "open");

	info->flip_status_current = flip_cover;
	schedule_delayed_work(&info->switching_work,
				msecs_to_jiffies(1));

	return 0;
}
#endif

static int abov_tk_input_open(struct input_dev *dev)
{
	struct abov_tk_info *info = input_get_drvdata(dev);
	input_info(true, &info->client->dev, "%s: users=%d\n", __func__,
		   info->input_dev->users);

	abov_tk_resume(&info->client->dev);

	if (info->pinctrl) {
		int retval;
		retval = abov_pinctrl_configure(info, true);
		input_info(true, &info->client->dev, "failed to put the pin in active state\n");
	}

	return 0;
}

static void abov_tk_input_close(struct input_dev *dev)
{
	struct abov_tk_info *info = input_get_drvdata(dev);
	input_info(true, &info->client->dev, "%s: users=%d\n", __func__,
		   info->input_dev->users);

	abov_tk_suspend(&info->client->dev);

	if (info->pinctrl) {
		int retval;
		retval = abov_pinctrl_configure(info, false);
		input_info(true, &info->client->dev, "failed to put the pin in suspend state\n");
	}

}

static const struct i2c_device_id abov_tk_id[] = {
	{ABOV_TK_NAME, 0},
	{}
};

MODULE_DEVICE_TABLE(i2c, abov_tk_id);

#ifdef CONFIG_OF
static struct of_device_id abov_match_table[] = {
	{ .compatible = "abov,mc96ft16xx",},
	{ },
};
#else
#define abov_match_table NULL
#endif

static struct i2c_driver abov_tk_driver = {
	.probe = abov_tk_probe,
	.remove = abov_tk_remove,
	.shutdown = abov_tk_shutdown,
	.driver = {
		   .name = ABOV_TK_NAME,
		   .of_match_table = abov_match_table,
	},
	.id_table = abov_tk_id,
};

static int __init touchkey_init(void)
{
#ifdef CONFIG_SAMSUNG_LPM_MODE
	if (lpcharge) {
		pr_info("%s %s : LPM Charging Mode!!\n", SECLOG, __func__);
		return 0;
	}
#endif
	
	return i2c_add_driver(&abov_tk_driver);
}

static void __exit touchkey_exit(void)
{
	i2c_del_driver(&abov_tk_driver);
}

module_init(touchkey_init);
module_exit(touchkey_exit);

/* Module information */
MODULE_AUTHOR("Samsung Electronics");
MODULE_DESCRIPTION("Touchkey driver for Abov MF16xx chip");
MODULE_LICENSE("GPL");
