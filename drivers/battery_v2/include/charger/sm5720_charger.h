/*
 *  sm5720_charger.h
 *  Samsung SM5720 Charger Driver header file
 *
 *  Copyright (C) 2016 Samsung Electronics
 *
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#ifndef __SM5720_CHARGER_H
#define __SM5720_CHARGER_H __FILE__

#include <linux/mfd/core.h>
#include <linux/mfd/sm5720.h>
#include <linux/mfd/sm5720-private.h>
#include <linux/regulator/machine.h>

#define SM5720_WATCHDOG_RESET_ACTIVATE
//#define SM5720_SBPS_ACTIVATE

enum {
	CHIP_ID = 0,
	DATA,
};

enum {
	SM5720_PASS4 = 6,
	SM5720_PASS5 = 9,
	SM5720_PASS6 = 11,
	SM5720_PASS7 = 14,
};

ssize_t sm5720_chg_show_attrs(struct device *dev,
				struct device_attribute *attr, char *buf);

ssize_t sm5720_chg_store_attrs(struct device *dev,
				struct device_attribute *attr,
				const char *buf, size_t count);

#define SM5720_CHARGER_ATTR(_name)				\
{							                    \
	.attr = {.name = #_name, .mode = 0664},	    \
	.show = sm5720_chg_show_attrs,			    \
	.store = sm5720_chg_store_attrs,			\
}

#define INPUT_CURRENT_TA		                1000

#define REDUCE_CURRENT_STEP						100
#define MINIMUM_INPUT_CURRENT					300
#define SLOW_CHARGING_CURRENT_STANDARD          400

#define WC_CURRENT_STEP		100
#define WC_CURRENT_START	400

#if defined(CONFIG_SEC_FACTORY)
#define WC_CURRENT_WORK_STEP	250
#else
#define WC_CURRENT_WORK_STEP	1000
#endif

extern int lpcharge;

enum {
	SS_LIM_VBUS_10mA_per_ms = 0x0,
	SS_LIM_VBUS_40mA_per_ms = 0x1,
};

enum {
	SS_LIM_WPC_5mA_per_ms   = 0x0,
	SS_LIM_WPC_10mA_per_ms  = 0x1,
};

enum {
	SS_FAST_VBUS_10mA_per_ms = 0x0,
	SS_FAST_VBUS_5mA_per_ms = 0x1,
};

enum {
	PRECHG_CURRENT_450mA    = 0x0,
	PRECHG_CURRENT_650mA    = 0x1,
};

enum {
	VSYS_MIN_REG_V_3000mV   = 0x0,
	VSYS_MIN_REG_V_3100mV   = 0x1,
	VSYS_MIN_REG_V_3200mV   = 0x2,
	VSYS_MIN_REG_V_3300mV   = 0x3,
	VSYS_MIN_REG_V_3400mV   = 0x4,
	VSYS_MIN_REG_V_3500mV   = 0x5,
	VSYS_MIN_REG_V_3600mV   = 0x7,
};

enum {
	RECHARGE_THRES_D_50mV   = 0x0,
	RECHARGE_THRES_D_100mV  = 0x1,
	RECHARGE_THRES_D_150mV  = 0x2,
	RECHARGE_THRES_D_200mV  = 0x3,
};

enum {
	AICL_THRES_VOLTAGE_4_3V = 0x0,
	AICL_THRES_VOLTAGE_4_4V = 0x1,
	AICL_THRES_VOLTAGE_4_5V = 0x2,
	AICL_THRES_VOLTAGE_4_6V = 0x3,
};

enum {
	DISCHARGE_CUR_LIMIT_3_5A = 0x0,
	DISCHARGE_CUR_LIMIT_4_5A = 0x1,
	DISCHARGE_CUR_LIMIT_5_0A = 0x2,
	DISCHARGE_CUR_LIMIT_6_0A = 0x3,
};

enum {
	TOPOFF_TIMER_10m        = 0x0,
	TOPOFF_TIMER_20m        = 0x1,
	TOPOFF_TIMER_30m        = 0x2,
	TOPOFF_TIMER_45m        = 0x3,
};

enum {
	OTGCURRENT_500mA	= 0x0,
	OTGCURRENT_900mA	= 0x1,
	OTGCURRENT_1200mA	= 0x2,
	OTGCURRENT_1500mA	= 0x3,
};

enum {
	BST_IQ3LIMIT_2000mA     = 0x0,
	BST_IQ3LIMIT_2800mA     = 0x1,
	BST_IQ3LIMIT_3500mA     = 0x2,
	BST_IQ3LIMIT_4000mA     = 0x3,
};

enum {
	SBPS_THEM_HOT_45C       = 0x0,
	SBPS_THEM_HOT_50C       = 0x1,
	SBPS_THEM_HOT_55C       = 0x2,
	SBPS_THEM_HOT_60C       = 0x3,
};

enum {
	SBPS_THEM_COLD_5C       = 0x0,
	SBPS_THEM_COLD_0C       = 0x1,
};

enum {
	SBPS_DISCHARGE_I_50mA   = 0x0,
	SBPS_DISCHARGE_I_100mA  = 0x1,
	SBPS_DISCHARGE_I_150mA  = 0x2,
	SBPS_DISCHARGE_I_200mA  = 0x3,
};

enum {
	SBPS_EN_THRES_V_4_2V    = 0x0,
	SBPS_EN_THRES_V_4_25V   = 0x1,
};

enum {
	WATCHDOG_TIMER_30s      = 0x0,
	WATCHDOG_TIMER_60s      = 0x1,
	WATCHDOG_TIMER_90s      = 0x2,
	WATCHDOG_TIMER_120s     = 0x3,
};

/* For Charger Operation mode control module */
enum {
    SM5720_CHARGER_OP_MODE_SUSPEND              = 0x0,
    SM5720_CHARGER_OP_MODE_TX_MODE_NOVBUS       = 0x1,
    SM5720_CHARGER_OP_MODE_TX_MODE_VBUS         = 0x2,
    SM5720_CHARGER_OP_MODE_WPC_OTG_CHG_ON       = 0x3,
    SM5720_CHARGER_OP_MODE_CHG_ON_WPCIN         = 0x4,
    SM5720_CHARGER_OP_MODE_CHG_ON_VBUS          = 0x5,
    SM5720_CHARGER_OP_MODE_USB_OTG_TX_MODE      = 0x6,
    SM5720_CHARGER_OP_MODE_USB_OTG              = 0x7,
};

enum {
    SM5720_CHARGER_OP_EVENT_VBUSIN              = 0x6,
    SM5720_CHARGER_OP_EVENT_WPCIN               = 0x5,
    SM5720_CHARGER_OP_EVENT_USB_OTG             = 0x4,
    SM5720_CHARGER_OP_EVENT_PWR_SHAR            = 0x3,
    SM5720_CHARGER_OP_EVENT_5V_TX               = 0x2,
    SM5720_CHARGER_OP_EVENT_9V_TX               = 0x1,
    SM5720_CHARGER_OP_EVENT_SUSPEND             = 0x0,
};

struct sm5720_charger_data {
	struct device *dev;
	struct i2c_client *i2c;
	struct mutex charger_mutex;

	struct sm5720_platform_data *sm5720_pdata;

	struct power_supply	*psy_chg;
	struct power_supply	*psy_otg;
	int status;

	/* for IRQ-service handling */
	int irq_aicl;
	int irq_aicl_enabled;
	int irq_vbus_pok;
	int irq_wpcin_pok;
	int irq_topoff;
	int irq_done;
	int irq_sysovlo;
	int irq_otgfail;
	int irq_wdtmroff;

	/* for Workqueue & wake-lock, mutex process */
	struct workqueue_struct *wqueue;
	struct delayed_work afc_work;
	struct delayed_work aicl_work;
	struct delayed_work wc_current_work;
	struct delayed_work topoff_work;

	struct wake_lock wpc_wake_lock;
	struct wake_lock wpc_current_wake_lock;
	struct wake_lock afc_wake_lock;
	struct wake_lock aicl_wake_lock;
	struct wake_lock sysovlo_wake_lock;
	struct wake_lock otg_wake_lock;

	/* for charging operation handling */
	int charge_mode;
	int is_charging;
	int cable_type;
	int input_current;
	int charging_current;

	int irq_wpcin_state;
	int siop_level;
	int float_voltage;

	int pmic_ver;

	bool slow_late_chg_mode;
	bool enable_sysovlo_irq;

	/* wireless charger */
	char *wireless_charger_name;
	int wireless_cc_cv;
	int wc_current;
	int	wc_pre_current;
};

int sm5720_charger_oper_push_event(int event_type, bool enable);
int sm5720_charger_oper_table_init(struct i2c_client *i2c);

int sm5720_charger_oper_get_current_status(void);
int sm5720_charger_oper_get_current_op_mode(void);

#endif /* __SM5720_CHARGER_H */
