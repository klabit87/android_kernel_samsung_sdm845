/*
 * =================================================================
 *
 *
 *	Description:  samsung display common file
 *
 *	Author: jb09.kim
 *	Company:  Samsung Electronics
 *
 * ================================================================
 */
/*
<one line to give the program's name and a brief idea of what it does.>
Copyright (C) 2012, Samsung Electronics. All rights reserved.

 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 and
 * only version 2 as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 */

#ifndef SS_DSI_PANEL_COMMON_H
#define SS_DSI_PANEL_COMMON_H

#include <linux/module.h>
#include <linux/of.h>
#include <linux/of_gpio.h>
#include <linux/gpio.h>
#include <linux/delay.h>
#include <linux/slab.h>
#include <linux/leds.h>
#include <linux/err.h>
#include <linux/err.h>
#include <linux/lcd.h>
#include <linux/syscalls.h>
#include <asm/uaccess.h>
#include <linux/kernel.h>
#include <linux/errno.h>
#include <linux/mutex.h>
#include <linux/wait.h>
#include <linux/ctype.h>
#include <asm/div64.h>
#include <linux/interrupt.h>
#include <linux/msm-bus.h>
#include <linux/sched.h>
#include <linux/dma-buf.h>
#include <linux/debugfs.h>
#include <linux/wakelock.h>
#include <linux/miscdevice.h>
#include <video/mipi_display.h>
#include <linux/dev_ril_bridge.h>
#include <linux/regulator/consumer.h>

#include "dsi_display.h"
#include "dsi_panel.h"
#include "sde_kms.h"
#include "sde_connector.h"
#include "sde_encoder.h"
#include "sde_encoder_phys.h"

#include "ss_ddi_spi_common.h"
#include "ss_dpui_common.h"

#include "ss_dsi_panel_sysfs.h"
#include "ss_dsi_panel_debug.h"

#include "ss_self_display_common.h"
#include "ss_ddi_poc_common.h"
#include "ss_copr_common.h"

#include "ss_interpolation_common.h"
#include "ss_flash_table_data_common.h"

#if defined(CONFIG_SEC_DEBUG)
#include <linux/sec_debug.h>
#endif

extern bool enable_pr_debug;

#define LOG_KEYWORD "[SDE]"

#define LCD_DEBUG(X, ...)	\
		do {	\
			if (enable_pr_debug)	\
				pr_info("%s %s : "X, LOG_KEYWORD, __func__, ## __VA_ARGS__);\
			else	\
				pr_debug("%s %s : "X, LOG_KEYWORD, __func__, ## __VA_ARGS__);\
		} while (0)	\

#define LCD_INFO(X, ...) pr_info("%s %s : "X, LOG_KEYWORD, __func__, ## __VA_ARGS__)
#define LCD_INFO_ONCE(X, ...) pr_info_once("%s %s : "X, LOG_KEYWORD, __func__, ## __VA_ARGS__)
#define LCD_ERR(X, ...) pr_err("%s %s : "X, LOG_KEYWORD, __func__, ## __VA_ARGS__)

#define MAX_PANEL_NAME_SIZE 100

#define PARSE_STRING 64
#define MAX_EXTRA_POWER_GPIO 4
#define MAX_BACKLIGHT_TFT_GPIO 4

/* OSC TE FITTING */
#define OSC_TE_FITTING_LUT_MAX 2

/* Register dump info */
#define MAX_INTF_NUM 2

/* Panel Unique Cell ID Byte count */
#define MAX_CELL_ID 11

/* Panel Unique OCTA ID Byte count */
#define MAX_OCTA_ID 20

/* PBA booting ID */
#define PBA_ID 0xFFFFFF

/* Default elvss_interpolation_lowest_temperature */
#define ELVSS_INTERPOLATION_TEMPERATURE -20

/* Default lux value for entering mdnie HBM */
#define ENTER_HBM_LUX 40000

/* MAX ESD Recovery gpio */
#define MAX_ESD_GPIO 2

#define BASIC_FB_PANLE_TYPE 0x01
#define NEW_FB_PANLE_TYPE 0x00
#define OTHERLINE_WORKQ_DEALY 900 /*ms*/
#define OTHERLINE_WORKQ_CNT 70

//#define DYNAMIC_DSI_CLK

extern int poweroff_charging;

enum PANEL_LEVEL_KEY {
	LEVEL_KEY_NONE = 0,
	LEVEL0_KEY = BIT(0),
	LEVEL1_KEY = BIT(1),
	LEVEL2_KEY = BIT(2),
};

enum mipi_samsung_cmd_map_list {
	PANEL_CMD_MAP_NULL,
	PANEL_CMD_MAP_MAX,
};

enum {
	MIPI_RESUME_STATE,
	MIPI_SUSPEND_STATE,
};

enum {
	ACL_OFF,
	ACL_30,
	ACL_15,
	ACL_50
};

enum {
	TE_FITTING_DONE = BIT(0),
	TE_CHECK_ENABLE = BIT(1),
	TE_FITTING_REQUEST_IRQ = BIT(3),
	TE_FITTING_STEP1 = BIT(4),
	TE_FITTING_STEP2 = BIT(5),
};

enum ACL_GRADUAL_MODE {
	GRADUAL_ACL_OFF,
	GRADUAL_ACL_ON,
	GRADUAL_ACL_UNSTABLE,
};

#define SAMSUNG_DISPLAY_PINCTRL0_STATE_DEFAULT "samsung_display_gpio_control0_default"
#define SAMSUNG_DISPLAY_PINCTRL0_STATE_SLEEP  "samsung_display_gpio_control0_sleep"
#define SAMSUNG_DISPLAY_PINCTRL1_STATE_DEFAULT "samsung_display_gpio_control0_default"
#define SAMSUNG_DISPLAY_PINCTRL1_STATE_SLEEP  "samsung_display_gpio_control0_sleep"

enum {
	SAMSUNG_GPIO_CONTROL0,
	SAMSUNG_GPIO_CONTROL1,
};

/* foder open : 0, close : 1 */
enum {
	HALL_IC_OPEN,
	HALL_IC_CLOSE,
	HALL_IC_UNDEFINED,
};

#define LCD_FLIP_NOT_REFRESH	BIT(8)

enum IRC_MODE {
	IRC_MODERATO_MODE = 0, /* fixed value */
	IRC_FLAT_GAMMA_MODE = 1,  /* fixed value */
	IRC_MAX_MODE,
};

enum {
	VDDM_ORG = 0,
	VDDM_LV,
	VDDM_HV,
	MAX_VDDM,
};

enum {
	MIPI_TX_TYPE_GRAM,
	MIPI_TX_TYPE_SIDERAM,
};

struct te_fitting_lut {
	int te_duration;
	int value;
};

struct osc_te_fitting_info {
	unsigned int status;
	long long te_duration;
	long long *te_time;
	int sampling_rate;
	struct completion te_check_comp;
	struct work_struct work;
	struct te_fitting_lut *lut[OSC_TE_FITTING_LUT_MAX];
};

/* DDI CMD log buffer max size is 512 bytes.
 * But, qct display driver limit max read size to 255 bytes (0xFF)
 * due to panel dtsi. panel dtsi determines length with one byte.
 * samsung,ldi_debug_logbuf_rx_cmds_revA   = [06 01 00 00 00 00 01 9C FF 00];
 */

#define DDI_CMD_LOGBUF_SIZE	255
struct te_period_check {
	struct work_struct te_work;
	int te_irq;
	int te_cnt;
	char cmd_log[DDI_CMD_LOGBUF_SIZE];
	bool is_working; /* block queueing work while working */
};

struct panel_lpm_info {
	u8 origin_mode;
	u8 ver;
	u8 mode;
	u8 hz;
	int lpm_bl_level;
	bool esd_recovery;
};

struct samsung_display_driver_data *samsung_get_vdd(void);

struct clk_timing_table {
	int tab_size;
	int *fps;
	int *hfp;
	int *hbp;
	int *hsw;
	int *hss;
	int *vfp;
	int *vbp;
	int *vsw;
};

struct clk_sel_table {
	int tab_size;
	int *rat;
	int *band;
	int *from;
	int *end;
	int *target_clk_idx;
};

struct dyn_mipi_clk {
	int is_support;
	struct clk_sel_table clk_sel_table;
	struct clk_timing_table clk_timing_table;
};

struct cmd_map {
	int *bl_level;
	int *cmd_idx;
	int size;
};

struct candela_map_table {
	int tab_size;
	int *scaled_idx;
	int *idx;
	int *from;
	int *end;

	/*
		cd :
		This is cacultated brightness to standardize brightness formula.
	*/
	int *cd;

	/* 	interpolation_cd :
		cd value is only calcuated brightness by formula.
		This is real measured brightness on panel.
	*/
	int *interpolation_cd;

	int min_lv;
	int max_lv;
};

struct hbm_candela_map_table {
	int tab_size;
	int *cd;
	int *idx;
	int *from;
	int *end;
	int *auto_level;
	int hbm_min_lv;
	int hbm_max_lv;
};

struct samsung_display_dtsi_data {
	bool samsung_lp11_init;
	bool samsung_tcon_clk_on_support;
	bool samsung_esc_clk_128M;
	bool samsung_osc_te_fitting;
	bool samsung_support_factory_panel_swap;
	u32  samsung_power_on_reset_delay;
	u32  samsung_dsi_off_reset_delay;
	u32 samsung_lpm_init_delay;
	u8	samsung_delayed_display_on;

	/*
	 * index[0] : array index for te fitting command from "ctrl->on_cmd"
	 * index[1] : array index for te fitting command from "osc_te_fitting_tx_cmds"
	 */
	int samsung_osc_te_fitting_cmd_index[2];
	int backlight_tft_gpio[MAX_BACKLIGHT_TFT_GPIO];

	/* PANEL TX / RX CMDS LIST */
	struct dsi_panel_cmd_set cmd_sets[SS_DSI_CMD_SET_MAX];

	struct candela_map_table candela_map_table[SUPPORT_PANEL_REVISION];
	struct candela_map_table pac_candela_map_table[SUPPORT_PANEL_REVISION];
	struct hbm_candela_map_table hbm_candela_map_table[SUPPORT_PANEL_REVISION];
	struct hbm_candela_map_table pac_hbm_candela_map_table[SUPPORT_PANEL_REVISION];
	struct candela_map_table aod_candela_map_table[SUPPORT_PANEL_REVISION];

	struct cmd_map aid_map_table[SUPPORT_PANEL_REVISION];
	struct cmd_map vint_map_table[SUPPORT_PANEL_REVISION];
	struct cmd_map acl_map_table[SUPPORT_PANEL_REVISION];
	struct cmd_map elvss_map_table[SUPPORT_PANEL_REVISION];
	struct cmd_map smart_acl_elvss_map_table[SUPPORT_PANEL_REVISION];
	struct cmd_map caps_map_table[SUPPORT_PANEL_REVISION];

	struct candela_map_table scaled_level_map_table[SUPPORT_PANEL_REVISION];

	struct cmd_map hmt_reverse_aid_map_table[SUPPORT_PANEL_REVISION];
	struct candela_map_table hmt_candela_map_table[SUPPORT_PANEL_REVISION];

	bool panel_lpm_enable;
	bool hmt_enabled;

	/* TFT LCD Features*/
	int tft_common_support;
	int backlight_gpio_config;
	int pwm_ap_support;
	const char *tft_module_name;
	const char *panel_vendor;
	const char *disp_model;

	/* MDINE HBM_CE_TEXT_MDNIE mode used */
	int hbm_ce_text_mode_support;

	/* Backlight IC discharge delay */
	int blic_discharging_delay_tft;
	int cabc_delay;

	/*
	*	INTERPOLATION(AOR & IRC)
	*/
	int hbm_brightness_step;
	int normal_brightness_step;
	int hmd_brightness_step;

	int gamma_size;
	int aor_size;
	int vint_size;
	int elvss_size;
	int irc_size;

	int flash_table_hbm_aor_offset;
	int flash_table_hbm_vint_offset;
	int flash_table_hbm_elvss_offset;
	int flash_table_hbm_irc_offset;

	int flash_table_normal_gamma_offset;
	int flash_table_normal_aor_offset;
	int flash_table_normal_vint_offset;
	int flash_table_normal_elvss_offset;
	int flash_table_normal_irc_offset;

	int flash_table_hmd_gamma_offset;
	int flash_table_hmd_aor_offset;

	/*
	 *	Flash gamma feature
	*/
	bool flash_gamma_support;

	/* Below things are emmc read address */
	int flash_gamma_write_check_address;

	/* Here is bank base address */
	int flash_gamma_bank_start_len;
	int *flash_gamma_bank_start;
	int flash_gamma_bank_end_len;
	int *flash_gamma_bank_end;

	/* Below is offset address of base bank */
	int flash_gamma_check_sum_start_offset;
	int flash_gamma_check_sum_end_offset;

	/* For support 0xC8 register integrity */
	int flash_gamma_0xc8_start_offset;
	int flash_gamma_0xc8_end_offset;
	int flash_gamma_0xc8_size;
	int flash_gamma_0xc8_check_sum_start_offset;
	int flash_gamma_0xc8_check_sum_end_offset;

	/* For MCD flash data */
	int flash_MCD1_R_address;
	int flash_MCD2_R_address;
	int flash_MCD1_L_address;
	int flash_MCD2_L_address;

	/*
	 *	Flash gamma feature end
	*/
};

struct display_status {
	bool wait_disp_on;
	int wait_actual_disp_on;
	int aod_delay;

	int hbm_mode;

	int elvss_value1;
	int elvss_value2;
	u8 *irc_otp;
	int disp_on_pre;
};

struct hmt_status {
	unsigned int hmt_on;
	unsigned int hmt_reverse;
	unsigned int hmt_is_first;

	int hmt_bl_level;
	int candela_level_hmt;
	int cmd_idx_hmt;

	int (*hmt_enable)(struct samsung_display_driver_data *vdd);
	int (*hmt_reverse_update)(struct samsung_display_driver_data *vdd,
			int enable);
	int (*hmt_bright_update)(struct samsung_display_driver_data *vdd);
};

struct esd_recovery {
	spinlock_t irq_lock;
	bool esd_recovery_init;
	bool is_enabled_esd_recovery;
	bool is_wakeup_source;
	int esd_gpio[MAX_ESD_GPIO];
	u8 num_of_gpio;
	unsigned long irqflags[MAX_ESD_GPIO];
	void (*esd_irq_enable)(bool enable, bool nosync, void *data);
};

/* Panel LPM(ALPM/HLPM) status flag */
// TODO: how about to use vdd->panel_state instaed of  below..???
enum {
	LPM_VER0 = 0,
	LPM_VER1,
};

enum {
	LPM_MODE_OFF = 0,			/* Panel LPM Mode OFF */
	ALPM_MODE_ON,			/* ALPM Mode On */
	HLPM_MODE_ON,			/* HLPM Mode On */
	MAX_LPM_MODE,			/* Panel LPM Mode MAX */
};

enum {
	ALPM_MODE_ON_2NIT = 1,		/* ALPM Mode On 2nit*/
	HLPM_MODE_ON_2NIT,			/* HLPM Mode On 60nit*/
	ALPM_MODE_ON_60NIT,			/* ALPM Mode On 2nit*/
	HLPM_MODE_ON_60NIT,			/* HLPM Mode On 60nit*/
};

enum {
	LPM_2NIT_IDX = 0,
	LPM_10NIT_IDX,
	LPM_30NIT_IDX,
	LPM_40NIT_IDX,
	LPM_60NIT_IDX,
	LPM_BRIGHTNESS_MAX_IDX,
};

enum {
	LPM_2NIT = 2,
	LPM_10NIT = 10,
	LPM_30NIT = 30,
	LPM_40NIT = 40,
	LPM_60NIT = 60,
	LPM_BRIGHTNESS_MAX,
};

struct panel_func {
	/* ON/OFF */
	int (*samsung_panel_on_pre)(struct samsung_display_driver_data *vdd);
	int (*samsung_panel_on_post)(struct samsung_display_driver_data *vdd);
	int (*samsung_panel_off_pre)(struct samsung_display_driver_data *vdd);
	int (*samsung_panel_off_post)(struct samsung_display_driver_data *vdd);
	void (*samsung_backlight_late_on)(struct samsung_display_driver_data *vdd);
	void (*samsung_panel_init)(struct samsung_display_driver_data *vdd);

	/* DDI RX */
	char (*samsung_panel_revision)(struct samsung_display_driver_data *vdd);
	int (*samsung_manufacture_date_read)(struct samsung_display_driver_data *vdd);
	int (*samsung_ddi_id_read)(struct samsung_display_driver_data *vdd);

	int (*samsung_cell_id_read)(struct samsung_display_driver_data *vdd);
	int (*samsung_octa_id_read)(struct samsung_display_driver_data *vdd);
	int (*samsung_hbm_read)(struct samsung_display_driver_data *vdd);
	int (*samsung_elvss_read)(struct samsung_display_driver_data *vdd);
	int (*samsung_irc_read)(struct samsung_display_driver_data *vdd);
	int (*samsung_mdnie_read)(struct samsung_display_driver_data *vdd);
	int (*samsung_smart_dimming_init)(struct samsung_display_driver_data *vdd);

	int (*samsung_flash_gamma_support)(struct samsung_display_driver_data *vdd);
	int (*samsung_interpolation_init)(struct samsung_display_driver_data *vdd, enum INTERPOLATION_MODE mode);

	struct smartdim_conf *(*samsung_smart_get_conf)(void);

	/* Brightness */
	struct dsi_panel_cmd_set * (*samsung_brightness_hbm_off)(struct samsung_display_driver_data *vdd, int *level_key);
	struct dsi_panel_cmd_set * (*samsung_brightness_aid)(struct samsung_display_driver_data *vdd, int *level_key);
	struct dsi_panel_cmd_set * (*samsung_brightness_acl_on)(struct samsung_display_driver_data *vdd, int *level_key);
	struct dsi_panel_cmd_set * (*samsung_brightness_pre_acl_percent)(struct samsung_display_driver_data *vdd, int *level_key);
	struct dsi_panel_cmd_set * (*samsung_brightness_acl_percent)(struct samsung_display_driver_data *vdd, int *level_key);
	struct dsi_panel_cmd_set * (*samsung_brightness_acl_off)(struct samsung_display_driver_data *vdd, int *level_key);
	struct dsi_panel_cmd_set * (*samsung_brightness_pre_elvss)(struct samsung_display_driver_data *vdd, int *level_key);
	struct dsi_panel_cmd_set * (*samsung_brightness_elvss)(struct samsung_display_driver_data *vdd, int *level_key);
	struct dsi_panel_cmd_set *(*samsung_brightness_pre_caps)(struct samsung_display_driver_data *vdd, int *level_key);
	struct dsi_panel_cmd_set *(*samsung_brightness_caps)(struct samsung_display_driver_data *vdd, int *level_key);
	struct dsi_panel_cmd_set * (*samsung_brightness_elvss_temperature1)(struct samsung_display_driver_data *vdd, int *level_key);
	struct dsi_panel_cmd_set * (*samsung_brightness_elvss_temperature2)(struct samsung_display_driver_data *vdd, int *level_key);
	struct dsi_panel_cmd_set * (*samsung_brightness_vint)(struct samsung_display_driver_data *vdd, int *level_key);
	struct dsi_panel_cmd_set * (*samsung_brightness_irc)(struct samsung_display_driver_data *vdd, int *level_key);
	struct dsi_panel_cmd_set * (*samsung_brightness_gamma)(struct samsung_display_driver_data *vdd, int *level_key);

	/* HBM */
	struct dsi_panel_cmd_set * (*samsung_hbm_gamma)(struct samsung_display_driver_data *vdd, int *level_key);
	struct dsi_panel_cmd_set * (*samsung_hbm_etc)(struct samsung_display_driver_data *vdd, int *level_key);
	struct dsi_panel_cmd_set * (*samsung_hbm_irc)(struct samsung_display_driver_data *vdd, int *level_key);
	int (*get_hbm_candela_value)(int level);

	/* Event */
	void (*ss_event_frame_update)(struct samsung_display_driver_data *vdd, int event, void *arg);
	void (*ss_event_fb_event_callback)(struct samsung_display_driver_data *vdd, int event, void *arg);
	void (*ss_event_osc_te_fitting)(struct samsung_display_driver_data *vdd, int event, void *arg);
	void (*ss_event_esd_recovery_init)(struct samsung_display_driver_data *vdd, int event, void *arg);

	/* OSC Tuning */
	int (*samsung_osc_te_fitting)(struct samsung_display_driver_data *vdd);
	int (*samsung_change_ldi_fps)(struct samsung_display_driver_data *vdd,
			unsigned int input_fps);

	/* HMT */
	struct dsi_panel_cmd_set * (*samsung_brightness_gamma_hmt)(struct samsung_display_driver_data *vdd, int *level_key);
	struct dsi_panel_cmd_set * (*samsung_brightness_aid_hmt)(struct samsung_display_driver_data *vdd, int *level_key);
	struct dsi_panel_cmd_set * (*samsung_brightness_elvss_hmt)(struct samsung_display_driver_data *vdd, int *level_key);
	struct dsi_panel_cmd_set * (*samsung_brightness_vint_hmt)(struct samsung_display_driver_data *vdd, int *level_key);

	int (*samsung_smart_dimming_hmt_init)(struct samsung_display_driver_data *vdd);
	struct smartdim_conf *(*samsung_smart_get_conf_hmt)(void);

	/* TFT */
	void (*samsung_tft_blic_init)(struct samsung_display_driver_data *vdd);
	void (*samsung_brightness_tft_pwm)(struct samsung_display_driver_data *vdd, int level);
	struct dsi_panel_cmd_set * (*samsung_brightness_tft_pwm_ldi)(struct samsung_display_driver_data *vdd, int *level_key);

	void (*samsung_bl_ic_pwm_en)(int enable);
	void (*samsung_bl_ic_i2c_ctrl)(int scaled_level);
	void (*samsung_bl_ic_outdoor)(int enable);

	/*LVDS*/
	void (*samsung_ql_lvds_register_set)(struct samsung_display_driver_data *vdd);
	int (*samsung_lvds_write_reg)(u16 addr, u32 data);

	/* Panel LPM(ALPM/HLPM) */
	void (*samsung_update_lpm_ctrl_cmd)(struct samsung_display_driver_data *vdd);
	void (*samsung_set_lpm_brightness)(struct samsung_display_driver_data *vdd);

	/* A3 line panel data parsing fn */
	int (*parsing_otherline_pdata)(struct file *f, struct samsung_display_driver_data *vdd,
		char *src, int len);
	void (*set_panel_fab_type)(int type);
	int (*get_panel_fab_type)(void);

	/* color weakness */
	void (*color_weakness_ccb_on_off)(struct samsung_display_driver_data *vdd, int mode);

	/* DDI H/W Cursor */
	int (*ddi_hw_cursor)(struct samsung_display_driver_data *vdd, int *input);

	/* COVER CONTROL */
	void (*samsung_cover_control)(struct samsung_display_driver_data *vdd);

	/* POC */
	int (*samsung_poc_ctrl)(struct samsung_display_driver_data *vdd, u32 cmd);

	/* Gram Checksum Test */
	int (*samsung_gct_read)(struct samsung_display_driver_data *vdd);
	int (*samsung_gct_write)(struct samsung_display_driver_data *vdd);

	/* Self display */
	int (*samsung_self_display_init)(struct samsung_display_driver_data *vdd);

	/* PBA */
	void (*samsung_pba_config)(struct samsung_display_driver_data *vdd, void *arg);
};

struct samsung_register_info {
	size_t virtual_addr;
};

struct samsung_register_dump_info {
	/* DSI PLL */
	struct samsung_register_info dsi_pll;

	/* DSI CTRL */
	struct samsung_register_info dsi_ctrl;

	/* DSI PHY */
	struct samsung_register_info dsi_phy;
};

struct samsung_display_debug_data {
	struct dentry *root;
	struct dentry *dump;
	struct dentry *hw_info;
	struct dentry *display_status;
	struct dentry *display_ltp;

	bool print_cmds;
	bool *is_factory_mode;
	bool panic_on_pptimeout;
};

struct rf_noti_info {
	u16 len;
	u8 msg_seq;
	u8 ack_seq;
	u8 main_cmd;
	u8 sub_cmd;
	u8 cmd_type;
	u8 rat;
	u32 band;
	u32 arfcn;
};

struct rf_info {
	int rat;
	int band;
	int arfcn;
	struct notifier_block notifier;
};

enum {
	POC_OP_NONE = 0,
	POC_OP_ERASE,
	POC_OP_WRITE,
	POC_OP_READ,
	POC_OP_ERASE_WRITE_IMG,
	POC_OP_ERASE_WRITE_TEST,
	POC_OP_BACKUP,
	POC_OP_CHECKSUM,
	POC_OP_CHECK_FLASH,
	POC_OP_SET_FLASH_WRITE,
	POC_OP_SET_FLASH_EMPTY,
	MAX_POC_OP,
};

enum poc_state {
	POC_STATE_NONE,
	POC_STATE_FLASH_EMPTY,
	POC_STATE_FLASH_FILLED,
	POC_STATE_ER_START,
	POC_STATE_ER_PROGRESS,
	POC_STATE_ER_COMPLETE,
	POC_STATE_ER_FAILED,
	POC_STATE_WR_START,
	POC_STATE_WR_PROGRESS,
	POC_STATE_WR_COMPLETE,
	POC_STATE_WR_FAILED,
	MAX_POC_STATE,
};

enum mdss_cpufreq_cluster {
	CPUFREQ_CLUSTER_BIG,
	CPUFREQ_CLUSTER_LITTLE,
	CPUFREQ_CLUSTER_ALL,
};

enum ss_panel_pwr_state {
	PANEL_PWR_OFF,
	PANEL_PWR_ON_READY,
	PANEL_PWR_ON,
	PANEL_PWR_LPM,
	MAX_PANEL_PWR,
};

enum ss_display_ndx {
	PRIMARY_DISPLAY_NDX = 0,
	SECONDARY_DISPLAY_NDX,
	MAX_DISPLAY_NDX,
};

#define IOC_GET_POC_STATUS	_IOR('A', 100, __u32)		/* 0:NONE, 1:ERASED, 2:WROTE, 3:READ */
#define IOC_GET_POC_CHKSUM	_IOR('A', 101, __u32)		/* 0:CHKSUM ERROR, 1:CHKSUM SUCCESS */
#define IOC_GET_POC_CSDATA	_IOR('A', 102, __u32)		/* CHKSUM DATA 4 Bytes */
#define IOC_GET_POC_ERASED	_IOR('A', 103, __u32)		/* 0:NONE, 1:NOT ERASED, 2:ERASED */
#define IOC_GET_POC_FLASHED	_IOR('A', 104, __u32)		/* 0:NOT POC FLASHED(0x53), 1:POC FLAHSED(0x52) */

#define IOC_SET_POC_ERASE	_IOR('A', 110, __u32)		/* ERASE POC FLASH */
#define IOC_SET_POC_TEST	_IOR('A', 112, __u32)		/* POC FLASH TEST - ERASE/WRITE/READ/COMPARE */

/* POC */
struct POC {
	bool is_support;
	u32 file_opend;
	struct miscdevice dev;
	bool erased;
	atomic_t cancel;
#ifdef CONFIG_DISPLAY_USE_INFO
	struct notifier_block dpui_notif;
#endif

	u8 chksum_data[4];
	u8 chksum_res;

	u8 *wbuf;
	u32 wpos;
	u32 wsize;

	u8 *rbuf;
	u32 rpos;
	u32 rsize;

	u32 erase_delay_ms; /* msleep */
	u32 erase_delay_us; /* usleep */
	u32 write_delay_us; /* usleep */
};

#define GCT_RES_CHECKSUM_PASS	(1)
#define GCT_RES_CHECKSUM_NG	(0)
#define GCT_RES_CHECKSUM_OFF	(-2)
#define GCT_RES_CHECKSUM_NOT_SUPPORT	(-3)

struct gram_checksum_test {
	bool is_support;
	int on;
	u8 checksum[4];
};

/* ss_exclusive_mipi_tx: block dcs tx and
 * "frame update", in ss_event_frame_update_pre().
 * Allow only dcs packets that has pass token.
 *
 * How to use exclusive_mipi_tx
 * 1) lock ex_tx_lock.
 * 2) set enable to true. This means that exclusvie mode is enabled,
 *    and the only packet that has token (exclusive_pass==true) can be sent.
 *    Other packets, which doesn't have token, should wait
 *    until exclusive mode disabled.
 * 3) calls ss_set_exclusive_tx_packet() to set the packet's
 *    exclusive_pass to true.
 * 4) In the end, calls wake_up_all(&vdd->exclusive_tx.ex_tx_waitq)
 *    to start to tx other mipi cmds that has no token and is waiting
 *    to be sent.
 */
struct ss_exclusive_mipi_tx {
	struct mutex ex_tx_lock;
	int enable; /* This shuold be set in ex_tx_lock lock */
	wait_queue_head_t ex_tx_waitq;

	/*
		To allow frame update under exclusive mode.
		Please be careful & Check exclusive cmds allow 2C&3C or othere value at frame header.
	*/
	int permit_frame_update;
};

/*
 * Manage vdd per dsi_panel, respectivley, like below table.
 * Each dsi_display has one dsi_panel and one vdd, respectively.
 * ------------------------------------------------------------------------------------------------------------------------------------------
 * case		physical conneciton	panel dtsi	dsi_dsiplay	dsi_panel	vdd	dsi_ctrl	lcd sysfs	bl sysfs
 * ------------------------------------------------------------------------------------------------------------------------------------------
 * single dsi	1 panel, 1 dsi port	1		1		1		1	1		1		1
 * split mode	1 panel, 2 dsi port	1		1		1		1	2		1		1
 * dual panel	2 panel, 2 dsi port	2		2		2		2	2		2		1
 * hall ic	2 panel, 1 dsi port	2		2		2		2	1		1		1
 * ------------------------------------------------------------------------------------------------------------------------------------------
 */
struct samsung_display_driver_data {

	void *msm_private;

	/*
	*	PANEL COMMON DATA
	*/

	// TODO: vdd_lock is for protection of mpi cmd tx and rx..
	// but it is already protected by dsi_ctrl->ctrl_lock and panel->panel_lock...
//	struct mutex vdd_lock;
	struct mutex vdd_panel_lpm_lock;
	struct mutex vdd_dyn_mipi_lock;
	struct mutex vdd_lock;
	struct mutex cmd_lock;
	struct mutex bl_lock;
	// TODO: vdd_ss_direct_cmdlist_lock is necessary..???
	// if not necessary, remove vdd_ss_direct_cmdlist_lock...
//	struct mutex vdd_ss_direct_cmdlist_lock;

	struct samsung_display_debug_data *debug_data;
	struct ss_exclusive_mipi_tx exclusive_tx;
	struct list_head vdd_list;

	int support_mdnie_lite;

	int support_mdnie_trans_dimming;

	bool is_factory_mode;

	bool panel_attach_status; // 1: attached, 0: detached

	int panel_revision;

	char *panel_vendor;

	int recovery_boot_mode;

	int elvss_interpolation_temperature;
	int temperature;
	char temperature_value;

	int lux;
	int enter_hbm_lux;

	int auto_brightness;
	int bl_level;
	int pac_bl_level; // scaled idx
	int candela_level;
	int interpolation_cd;
	int cd_idx;		// original idx
	int pac_cd_idx; // scaled idx

	int acl_status;
	int siop_status;
	bool mdnie_tuning_enable_tft;
	int mdnie_tune_size1;
	int mdnie_tune_size2;
	int mdnie_tune_size3;
	int mdnie_tune_size4;
	int mdnie_tune_size5;
	int mdnie_tune_size6;
	int mdnie_lcd_on_notifiy;
	int mdnie_disable_trans_dimming;

	int samsung_support_irc;

	/* To enhance color accuracy, change IRC mode for each screen mode.
	 * - Adaptive display mode: Moderato mode
	 * - Other screen modes: Flat Gamma mode
	 */
	enum IRC_MODE irc_mode;

	struct panel_func panel_func;

	struct samsung_display_dtsi_data dtsi_data;

	struct display_status display_status_dsi;

	/* register dump info */
	struct samsung_register_dump_info dump_info[MAX_INTF_NUM];

	/*
	*	PANEL OPERATION DATA
	*/
	int manufacture_id_dsi;

	int manufacture_date_loaded_dsi;
	int manufacture_date_dsi;
	int manufacture_time_dsi;

	// TODO: For mdnie
	// 1) make mdnie struct like struct ss_mdnie_st mdnie, and manage it
	// 2) If multi vdds share same mdnie setting, like hall ic project, share same mdnie pointer... (use some flag like bool share_mdnie_setitng...
	int mdnie_loaded_dsi;
	struct mdnie_lite_tun_type *mdnie_tune_state_dsi;
	int mdnie_x;
	int mdnie_y;
	struct mdnie_lite_tune_data *mdnie_data;

	int ddi_id_loaded_dsi;
	int ddi_id_dsi[5];

	/* Panel Unique Cell ID */
	int cell_id_loaded_dsi;
	int cell_id_dsi[MAX_CELL_ID];

	/* Panel Unique OCTA ID */
	int octa_id_loaded_dsi;
	u8 octa_id_dsi[MAX_OCTA_ID];

	int hbm_loaded_dsi;
	int elvss_loaded_dsi;
	int irc_loaded_dsi;
	int smart_dimming_loaded_dsi;
	struct smartdim_conf *smart_dimming_dsi;

	/*
	*	Brightness control packet
	*/

	/*
	 * OSC TE fitting info
	 */
	struct osc_te_fitting_info te_fitting_info;
	struct te_period_check te_check;

	/*
	 *  HMT
	 */
	struct hmt_status hmt_stat;
	int smart_dimming_hmt_loaded_dsi;
	struct smartdim_conf *smart_dimming_dsi_hmt;

	/* CABC feature */
	int support_cabc;

	/* TFT BL DCS Scaled level*/
	int scaled_level;

	/* TFT LCD Features*/
	int (*backlight_tft_config)(struct samsung_display_driver_data *vdd, int enable);
	void (*backlight_tft_pwm_control)(struct samsung_display_driver_data *vdd, int bl_lvl);
	void (*ss_panel_tft_outdoormode_update)(struct samsung_display_driver_data *vdd);
	/* ESD */
	struct esd_recovery esd_recovery;

	/* Panel Recovery Count */
	int panel_recovery_cnt;

	/*Image dump*/
	struct workqueue_struct *image_dump_workqueue;
	struct work_struct image_dump_work;

	/* Other line panel support */
	struct workqueue_struct *other_line_panel_support_workq;
	struct delayed_work other_line_panel_support_work;
	int other_line_panel_work_cnt;

	/* auto brightness level */
	int auto_brightness_level;

	/* weakness hbm comp */
	int weakness_hbm_comp;

	/* Panel LPM info */
	struct panel_lpm_info panel_lpm;

	/* send recovery pck before sending image date (for ESD recovery) */
	int send_esd_recovery;

	/* DOZE */
	struct wake_lock doze_wakelock;

	/* graudal acl: config_adaptiveControlValues in config_display.xml
	 * has step values, like 0, 1, 2, 3, 4, and 5.
	 * In case of gallery app, PMS sets <5 ~ 0> via adaptive_control sysfs.
	 * gradual_acl_val has the value, 0 ~ 5, means acl percentage step,
	 * then ss_acl_on() will set appropriate acl cmd value.
	 */
	int gradual_acl_val;

	int gradual_pre_acl_on;
	int gradual_acl_update;
	enum ACL_GRADUAL_MODE gradual_acl_status;

	/* ldu correction */
	int ldu_correction_state;
	int ldu_bl_backup;

	/* color weakness */
	int color_weakness_mode;
	int color_weakness_level;
	int color_weakness_value; /*mode+level*/
	int ccb_bl_backup;
	int ccb_cd;
	int ccb_stepping;

	/* Cover Control Status */
	int cover_control;

	int select_panel_gpio;
	bool select_panel_use_expander_gpio;

	/* Power Control for LPM */
	bool lpm_power_control;
	char lpm_power_control_supply_name[32];
	int lpm_power_control_supply_min_voltage;
	int lpm_power_control_supply_max_voltage;

	char lpm_power_control_elvss_name[32];
	int lpm_power_control_elvss_lpm_voltage;
	int lpm_power_control_elvss_normal_voltage;

#ifdef CONFIG_DISPLAY_USE_INFO
	struct notifier_block dpui_notif;
	struct notifier_block dpci_notif;
	u64 dsi_errors; /* report dpci bigdata */
#endif

	/* COPR */
	struct COPR copr;
	int copr_load_init_cmd;

	/* SPI device */
	struct spi_device *spi_dev;

	/* POC */
	struct POC poc_driver;

	/* PAC */
	int pac;

	/* X-Talk */
	int xtalk_mode;

	/* Dynamic MIPI Clock */
	struct rf_info rf_info;
	struct dyn_mipi_clk dyn_mipi_clk;

	int poc_operation;

	struct gram_checksum_test gct;

	/* hall ic */
	bool support_hall_ic;
	int hall_ic_status;
	int hall_ic_mode_change_trigger;
	bool hall_ic_status_pending;
	bool hall_ic_status_unhandled;
	struct notifier_block hall_ic_notifier_display;
	bool lcd_flip_not_refresh;
	u32 lcd_flip_delay_ms;
	struct delayed_work delay_disp_on_work;

	enum ss_panel_pwr_state panel_state;

	/* ndx means display ID
	 * ndx = 0: primary display
	 * ndx = 1: secondary display
	 */
	enum ss_display_ndx ndx;

	/* smmu debug(sde & rotator) */
	struct ss_smmu_debug ss_debug_smmu[SMMU_MAX_DEBUG];
	struct kmem_cache *ss_debug_smmu_cache;

	/* SELF DISPLAY opration */
	struct self_display self_disp;
	int self_display_loaded_dsi;

	/* mipi cmd type */
	int cmd_type;

	bool panel_dead;

	int read_panel_status_from_lk;

	/* AOR & IRC Interpolation feature */
	struct workqueue_struct *flash_br_workqueue;
	struct delayed_work flash_br_work;
	struct brightness_data_info panel_br_info;
	struct ss_interpolation flash_itp;
	struct ss_interpolation table_itp;
	int table_interpolation_loaded;
};

extern struct list_head vdds_list;

/* COMMON FUNCTION */
void ss_panel_init(struct dsi_panel *panel);
//void ss_dsi_panel_registered(struct dsi_panel *pdata);
void ss_set_max_cpufreq(struct samsung_display_driver_data *vdd,
		int enable, enum mdss_cpufreq_cluster cluster);
void ss_set_exclusive_tx_packet(
		struct samsung_display_driver_data *vdd,
		enum dsi_cmd_set_type cmd, int pass);
void ss_set_exclusive_tx_lock_from_qct(struct samsung_display_driver_data *vdd, bool lock);
int ss_send_cmd(struct samsung_display_driver_data *vdd,
		enum dsi_cmd_set_type cmd);
int ss_write_ddi_ram(struct samsung_display_driver_data *vdd,
				int target, u8 *buffer, int len);
int ss_panel_on_pre(struct samsung_display_driver_data *vdd);
int ss_panel_on_post(struct samsung_display_driver_data *vdd);
int ss_panel_off_pre(struct samsung_display_driver_data *vdd);
int ss_panel_off_post(struct samsung_display_driver_data *vdd);
//int ss_panel_extra_power(struct dsi_panel *pdata, int enable);
int ss_backlight_tft_gpio_config(struct samsung_display_driver_data *vdd, int enable);
int ss_backlight_tft_request_gpios(struct samsung_display_driver_data *vdd);
int ss_panel_data_read(struct samsung_display_driver_data *vdd,
		enum dsi_cmd_set_type type, u8 *buffer, int level_key);
void ss_panel_low_power_config(void *vdd_data, int enable);


/*
 * Check lcd attached status for DISPLAY_1 or DISPLAY_2
 * if the lcd was not attached, the function will return 0
 */
int ss_panel_attached(int ndx);
int get_lcd_attached(char *mode);
int get_lcd_attached_secondary(char *mode);
int ss_panel_attached(int ndx);

struct samsung_display_driver_data *check_valid_ctrl(struct dsi_panel *panel);

char ss_panel_id0_get(struct samsung_display_driver_data *vdd);
char ss_panel_id1_get(struct samsung_display_driver_data *vdd);
char ss_panel_id2_get(struct samsung_display_driver_data *vdd);
char ss_panel_rev_get(struct samsung_display_driver_data *vdd);

int ss_panel_attach_get(struct samsung_display_driver_data *vdd);
int ss_panel_attach_set(struct samsung_display_driver_data *vdd, bool attach);

int ss_read_loading_detection(void);

/* EXTERN FUNCTION */
#ifdef CONFIG_FOLDER_HALL
extern void hall_ic_register_notify(struct notifier_block *nb);
#endif

/* EXTERN VARIABLE */
extern struct dsi_status_data *pstatus_data;

/* HMT FUNCTION */
int hmt_enable(struct samsung_display_driver_data *vdd);
int hmt_reverse_update(struct samsung_display_driver_data *vdd, int enable);

/* HALL IC FUNCTION */
int samsung_display_hall_ic_status(struct notifier_block *nb,
		unsigned long hall_ic, void *data);

/* CORP CALC */
void ss_copr_calc_work(struct work_struct *work);
void ss_copr_calc_delayed_work(struct delayed_work *work);

/* PANEL LPM FUNCTION */
int ss_find_reg_offset(int (*reg_list)[2],
		struct dsi_panel_cmd_set *cmd_list[], int list_size);
void ss_panel_lpm_ctrl(struct samsung_display_driver_data *vdd, int enable);
int ss_panel_lpm_power_ctrl(struct samsung_display_driver_data *vdd, int enable);

/* Dynamic MIPI Clock FUNCTION */
int ss_change_dyn_mipi_clk_timing(struct samsung_display_driver_data *vdd);



/* Other line panel support */
#define MAX_READ_LINE_SIZE 256
int read_line(char *src, char *buf, int *pos, int len);

/***************************************************************************************************
*		BRIGHTNESS RELATED.
****************************************************************************************************/

#define DEFAULT_BRIGHTNESS 255
#define DEFAULT_BRIGHTNESS_PAC3 25500

#define BRIGHTNESS_MAX_PACKET 50
#define HBM_MODE 6
#define HBM_CE_MODE 9

/* BRIGHTNESS RELATED FUNCTION */
int ss_brightness_dcs(struct samsung_display_driver_data *vdd, int level);
void ss_brightness_tft_pwm(struct samsung_display_driver_data *vdd, int level);
void update_packet_level_key_enable(struct samsung_display_driver_data *vdd,
		struct dsi_cmd_desc *packet, int *cmd_cnt, int level_key);
void update_packet_level_key_disable(struct samsung_display_driver_data *vdd,
		struct dsi_cmd_desc *packet, int *cmd_cnt, int level_key);
int ss_single_transmission_packet(struct dsi_panel_cmd_set *cmds);

int ss_set_backlight(struct samsung_display_driver_data *vdd, u32 bl_lvl);

/* HMT BRIGHTNESS */
int ss_brightness_dcs_hmt(struct samsung_display_driver_data *vdd, int level);
int hmt_bright_update(struct samsung_display_driver_data *vdd);

/* TFT BL DCS RELATED FUNCTION */
int get_scaled_level(struct samsung_display_driver_data *vdd, int ndx);
void ss_tft_autobrightness_cabc_update(struct samsung_display_driver_data *vdd);

/***************************************************************************************************
*		BRIGHTNESS RELATED END.
****************************************************************************************************/


/* SAMSUNG COMMON HEADER*/
#include "ss_dsi_smart_dimming_common.h"
/* MDNIE_LITE_COMMON_HEADER */
#include "ss_dsi_mdnie_lite_common.h"
/* SAMSUNG MODEL HEADER */




/* Interface betwenn samsung and msm display driver
 */

extern struct dsi_display_boot_param boot_displays[MAX_DSI_ACTIVE_DISPLAY];
extern char dsi_display_primary[MAX_CMDLINE_PARAM_LEN];
extern char dsi_display_secondary[MAX_CMDLINE_PARAM_LEN];

static inline void ss_get_primary_panel_name_cmdline(char *panel_name)
{
	char *pos = NULL;
	size_t len;

	pos = strnstr(dsi_display_primary, ":", sizeof(dsi_display_primary));
	if (!pos)
		return;

	len = (size_t) (pos - dsi_display_primary) + 1;

	strlcpy(panel_name, dsi_display_primary, len);
}

static inline void ss_get_secondary_panel_name_cmdline(char *panel_name)
{
	char *pos = NULL;
	size_t len;

	pos = strnstr(dsi_display_secondary, ":", sizeof(dsi_display_secondary));
	if (!pos)
		return;

	len = (size_t) (pos - dsi_display_secondary);

	strlcpy(panel_name, dsi_display_secondary, len);
}


#define GET_DSI_PANEL(vdd)	((struct dsi_panel *) (vdd)->msm_private)
static inline struct dsi_display *GET_DSI_DISPLAY(
		struct samsung_display_driver_data *vdd)
{
	struct dsi_panel *panel = GET_DSI_PANEL(vdd);
	struct dsi_display *display = dev_get_drvdata(panel->parent);
	return display;
}

static inline struct drm_device *GET_DRM_DEV(
		struct samsung_display_driver_data *vdd)
{
	struct dsi_display *display = GET_DSI_DISPLAY(vdd);
	struct drm_device *ddev = display->drm_dev;

	return ddev;
}

// refer to msm_drm_init()
static inline struct msm_kms *GET_MSM_KMS(
		struct samsung_display_driver_data *vdd)
{
	struct dsi_display *display = GET_DSI_DISPLAY(vdd);
	struct drm_device *ddev = display->drm_dev;
	struct msm_drm_private *priv = ddev->dev_private;

	return priv->kms;
}
#define GET_SDE_KMS(vdd)	to_sde_kms(GET_MSM_KMS(vdd))

static inline struct drm_crtc *GET_DRM_CRTC(
		struct samsung_display_driver_data *vdd)
{
#if 1
	struct dsi_display *display = GET_DSI_DISPLAY(vdd);
	struct drm_device *ddev = display->drm_dev;
	struct msm_drm_private *priv = ddev->dev_private;
	int id;

	// TODO: get correct crtc_id ...
	id = 0; // crtc id: 0 = primary display, 1 = wb...

	return priv->crtcs[id];
#else
	struct dsi_display *display = GET_DSI_DISPLAY(vdd);
	struct drm_device *ddev = display->drm_dev;
	struct list_head *crtc_list;
	struct drm_crtc *crtc = NULL, *crtc_iter;
	struct sde_crtc *sde_crtc;

	// refer to _sde_kms_setup_displays()
	crtc_list = &ddev->mode_config.crtc_list;
	list_for_each_entry(crtc_iter, crtc_list, head) {
		sde_crtc = to_sde_crtc(crtc_iter);
		if (sde_crtc->display == display) {
			crtc = crtc_iter;
			break;
		}
	}

	if (!crtc)
		LCD_ERR("failed to find drm_connector\n");

	return crtc;

#endif

}

static inline struct drm_encoder *GET_DRM_ENCODER(
		struct samsung_display_driver_data *vdd)
{
#if 1
	struct dsi_display *display = GET_DSI_DISPLAY(vdd);
	struct drm_device *ddev = display->drm_dev;
	struct msm_drm_private *priv = ddev->dev_private;
	int id;

	// TODO: get correct encoder id ...
	id = 0; // crtc id: 0 = primary display, 1 = wb...

	return priv->encoders[id];
#else
	struct dsi_display *display = GET_DSI_DISPLAY(vdd);
	struct drm_device *ddev = display->drm_dev;
	struct list_head *connector_list;
	struct drm_connector *conn = NULL, *conn_iter;
	struct sde_connector *c_conn;

	// refer to _sde_kms_setup_displays()
	connector_list = &ddev->mode_config.connector_list;
	list_for_each_entry(conn_iter, connector_list, head) {
		c_conn = to_sde_connector(conn_iter);
		if (c_conn->display == display) {
			conn = conn_iter;
			break;
		}
	}

	if (!conn)
		LCD_ERR("failed to find drm_connector\n");

	return conn;

#endif
}

static inline struct drm_connector *GET_DRM_CONNECTORS(
		struct samsung_display_driver_data *vdd)
{
#if 0
	struct dsi_display *display = GET_DSI_DISPLAY(vdd);
	struct drm_device *ddev = display->drm_dev;
	struct msm_drm_private *priv = ddev->dev_private;
	int id;

	// TODO: get connector id ...
	id = 0;

	return priv->connectors[id];
#else
	struct dsi_display *display = GET_DSI_DISPLAY(vdd);
	struct drm_device *ddev = display->drm_dev;
	struct list_head *connector_list;
	struct drm_connector *conn = NULL, *conn_iter;
	struct sde_connector *c_conn;

	// refer to _sde_kms_setup_displays()
	connector_list = &ddev->mode_config.connector_list;
	list_for_each_entry(conn_iter, connector_list, head) {
		c_conn = to_sde_connector(conn_iter);
		if (c_conn->display == display) {
			conn = conn_iter;
			break;
		}
	}

	if (!conn)
		LCD_ERR("failed to find drm_connector\n");

	return conn;
#endif
}

#define GET_SDE_CONNECTOR(vdd)	to_sde_connector(GET_DRM_CONNECTORS(vdd))

static inline struct backlight_device *GET_SDE_BACKLIGHT_DEVICE(
		struct samsung_display_driver_data *vdd)
{
	struct backlight_device *bd = NULL;
	struct sde_connector *conn = NULL;

	if (IS_ERR_OR_NULL(vdd))
		goto end;

	conn = GET_SDE_CONNECTOR(vdd);
	if (IS_ERR_OR_NULL(conn))
		goto end;

	bd = conn->bl_device;
	if (IS_ERR_OR_NULL(bd))
		goto end;

end:
	return bd;
}

/* In dual panel, it has two panel, and
 * ndx = 0: primary panel, ndx = 1: secondary panel
 * In one panel with dual dsi port, like split mode,
 * it has only one panel, and ndx = 0.
 * In one panel with single dsi port,
 * it has only one panel, and ndx = 0.
 */
static inline enum ss_display_ndx ss_get_display_ndx(
		struct samsung_display_driver_data *vdd)
{
	return vdd->ndx;
}

static inline void ss_set_display_ndx(struct samsung_display_driver_data *vdd,
		int ndx)
{
	vdd->ndx = ndx;
}

extern struct samsung_display_driver_data vdd_data[MAX_DISPLAY_NDX];

static inline struct samsung_display_driver_data *ss_check_hall_ic_get_vdd(
		struct samsung_display_driver_data *vdd)
{
	enum ss_display_ndx ndx;

	if (!vdd->support_hall_ic)
		return vdd;

/*	if (vdd->hall_ic_status == HALL_IC_OPEN)
		ndx = PRIMARY_DISPLAY_NDX;
	else
		ndx = SECONDARY_DISPLAY_NDX;
	TBD */
	ndx = PRIMARY_DISPLAY_NDX;

	return &vdd_data[ndx];
}

static inline const char *ss_get_panel_name(struct samsung_display_driver_data *vdd)
{
	struct dsi_panel *panel = GET_DSI_PANEL(vdd);
	return panel->name;
}

/* return panel x resolution */
static inline u32 ss_get_xres(struct samsung_display_driver_data *vdd)
{
	struct dsi_panel *panel = GET_DSI_PANEL(vdd);
	/* FC2 change: set panle mode in SurfaceFlinger initialization, instead of kenrel booting... */
	if (!panel->cur_mode) {
		LCD_ERR("err: no panel mode yet...\n");
		return 0;
	}

	return panel->cur_mode->timing.h_active;
}

/* return panel y resolution */
static inline u32 ss_get_yres(struct samsung_display_driver_data *vdd)
{
	struct dsi_panel *panel = GET_DSI_PANEL(vdd);
	/* FC2 change: set panle mode in SurfaceFlinger initialization, instead of kenrel booting... */
	if (!panel->cur_mode) {
		LCD_ERR("err: no panel mode yet...\n");
		return 0;
	}
	return panel->cur_mode->timing.v_active;
}

/* check if it is dual dsi port */
static inline bool ss_is_dual_dsi(struct samsung_display_driver_data *vdd)
{
	struct dsi_panel *panel = GET_DSI_PANEL(vdd);

	/* FC2 change: set panle mode in SurfaceFlinger initialization, instead of kenrel booting... */
	if (!panel->cur_mode) {
		LCD_ERR("err: no panel mode yet...\n");
		return false;
	}

	if (!panel->cur_mode->priv_info)
		return false;

	if (panel->cur_mode->priv_info->topology.num_intf == 2)
		return true;
	else
		return false;
}

/* check if it is dual dsi port */
static inline bool ss_is_single_dsi(struct samsung_display_driver_data *vdd)
{
	struct dsi_panel *panel = GET_DSI_PANEL(vdd);

	/* FC2 change: set panle mode in SurfaceFlinger initialization, instead of kenrel booting... */
	if (!panel->cur_mode) {
		LCD_ERR("err: no panel mode yet...\n");
		return true;
	}

	if (panel->cur_mode->priv_info->topology.num_intf == 1)
		return true;
	else
		return false;
}

/* check if it is command mode panel */
static inline bool ss_is_cmd_mode(struct samsung_display_driver_data *vdd)
{
	struct dsi_panel *panel = GET_DSI_PANEL(vdd);

	if (panel->panel_mode == DSI_OP_CMD_MODE)
		return true;
	else
		return false;
}

/* check if it is video mode panel */
static inline bool ss_is_video_mode(struct samsung_display_driver_data *vdd)
{
	struct dsi_panel *panel = GET_DSI_PANEL(vdd);

	if (panel->panel_mode == DSI_OP_VIDEO_MODE)
		return true;
	else
		return false;
}
/* check if it controls backlight via MIPI DCS */
static inline bool ss_is_bl_dcs(struct samsung_display_driver_data *vdd)
{
	struct dsi_panel *panel = GET_DSI_PANEL(vdd);

	if (panel->bl_config.type == DSI_BACKLIGHT_DCS)
		return true;
	else
		return false;
}

/* check if panel is on */
// blank_state = MDSS_PANEL_BLANK_UNBLANK
static inline bool ss_is_panel_on(struct samsung_display_driver_data *vdd)
{
//	struct dsi_panel *panel = GET_DSI_PANEL(vdd);

	// panel_initialized is set to true when panel is on,
	// and is set to fase when panel is off
//	return dsi_panel_initialized(panel);
	return (vdd->panel_state == PANEL_PWR_ON);
}

static inline bool ss_is_panel_on_ready(struct samsung_display_driver_data *vdd)
{
	return (vdd->panel_state == PANEL_PWR_ON_READY);
}

static inline bool ss_is_ready_to_send_cmd(struct samsung_display_driver_data *vdd)
{
	return ((vdd->panel_state == PANEL_PWR_ON) || (vdd->panel_state == PANEL_PWR_LPM));
}

static inline bool ss_is_panel_off(struct samsung_display_driver_data *vdd)
{
//	struct dsi_panel *panel = GET_DSI_PANEL(vdd);

	// panel_initialized is set to true when panel is on,
	// and is set to fase when panel is off
//	return dsi_panel_initialized(panel);

	return (vdd->panel_state == PANEL_PWR_OFF);
}

/* check if panel is low power mode */
// blank_state = MDSS_PANEL_BLANK_LOW_POWER
static inline bool ss_is_panel_lpm(
		struct samsung_display_driver_data *vdd)
{

	// TODO: get lpm value from somewhere..
	return (vdd->panel_state == PANEL_PWR_LPM);
}

static inline int ss_is_read_cmd(enum dsi_cmd_set_type type)
{
	if ((type > RX_CMD_START && type < RX_CMD_END) ||
			type == RX_SELF_DISP_DEBUG) {
		return 1;
	}

	return 0;
}

/* check if it is seamless mode (continuous splash mode) */
//  cont_splash_enabled is disabled after kernel booting...
// but seamless flag.. need to check...
//	if (pdata->panel_info.cont_splash_enabled) {
static inline bool ss_is_seamless_mode(struct samsung_display_driver_data *vdd)
{
	struct dsi_panel *panel = GET_DSI_PANEL(vdd);

	/* FC2 change: set panle mode in SurfaceFlinger initialization, instead of kenrel booting... */
	if (!panel->cur_mode) {
		LCD_ERR("err: no panel mode yet...\n");
		// setting ture prevents tx dcs cmd...
		return false;
	}

	if (panel->cur_mode->dsi_mode_flags & DSI_MODE_FLAG_SEAMLESS)
		return true;
	else
		return false;
}

// TODO: 1) check if it has no problem to set like below..
//       2) it need to set drm seamless mode also...
//           mode->flags & DRM_MODE_FLAG_SEAMLESS
static inline void ss_set_seamless_mode(struct samsung_display_driver_data *vdd)
{
	struct dsi_panel *panel = GET_DSI_PANEL(vdd);

	/* FC2 change: set panle mode in SurfaceFlinger initialization, instead of kenrel booting... */
	if (!panel->cur_mode) {
		LCD_ERR("err: no panel mode yet...\n");
		return;
	}

	panel->cur_mode->dsi_mode_flags |= DSI_MODE_FLAG_SEAMLESS;
}

// TODO: so far, qcomm bsp has no code to parse dtsi and save te gpio...
// but it would be applied... maybe.. like panel->reset_config.reset_gpio, disp_en_gpio...
static inline unsigned int ss_get_te_gpio(struct samsung_display_driver_data *vdd)
{
	return 10;
}

// TODO: find the code or symbols the get vblank enabled status...
// refer to drm_vblank_enable()
static inline bool ss_is_vsync_enabled(struct samsung_display_driver_data *vdd)
{
	struct dsi_display *display = GET_DSI_DISPLAY(vdd);
	struct drm_device *dev;
	struct drm_vblank_crtc *vblank;
	struct drm_crtc *crtc = GET_DRM_CRTC(vdd);
	int pipe;

	pipe = drm_crtc_index(crtc);

	dev = display->drm_dev;
	vblank = &dev->vblank[pipe];

	return vblank->enabled;
}

// TOOD: get esd check flag.. not yet implemented in qcomm bsp...
static inline unsigned int ss_is_esd_check_enabled(struct samsung_display_driver_data *vdd)
{
	return true;
}

extern char *cmd_set_prop_map[SS_DSI_CMD_SET_MAX];
static inline char *ss_get_cmd_name(enum dsi_cmd_set_type type)
{
	return cmd_set_prop_map[type];
}

static inline struct dsi_panel_cmd_set *ss_get_cmds(
		struct samsung_display_driver_data *vdd,
		enum dsi_cmd_set_type type)
{
	struct dsi_panel *panel = GET_DSI_PANEL(vdd);
	struct dsi_display_mode_priv_info *priv_info;
	struct dsi_panel_cmd_set *set;
	int rev = vdd->panel_revision;

	/* save the mipi cmd type */
	vdd->cmd_type = type;

	/* QCT cmds */
	if (type < SS_DSI_CMD_SET_START) {
		if (!panel->cur_mode || !panel->cur_mode->priv_info) {
			LCD_ERR("err: (%d) panel has no valid priv\n", type);
			return NULL;
		}

		priv_info = panel->cur_mode->priv_info;
		set = &priv_info->cmd_sets[type];
		return set;
	}

	/* SS cmds */
	if (type == TX_AID_SUBDIVISION && vdd->pac)
		type = TX_PAC_AID_SUBDIVISION;

	if (type == TX_IRC_SUBDIVISION && vdd->pac)
		type = TX_PAC_IRC_SUBDIVISION;

	set = &vdd->dtsi_data.cmd_sets[type];

	if (set->cmd_set_rev[rev])
		set = (struct dsi_panel_cmd_set *) set->cmd_set_rev[rev];

	return set;
}

static inline struct device_node *ss_get_panel_of(
		struct samsung_display_driver_data *vdd)
{
	struct dsi_panel *panel = GET_DSI_PANEL(vdd);

	return panel->panel_of_node;
}

#endif /* SAMSUNG_DSI_PANEL_COMMON_H */
