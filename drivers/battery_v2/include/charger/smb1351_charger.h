/*
 * smb1351-charger.h - Header for DA9155
 * Copyright (C) 2015 Samsung Electronics
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#ifndef __SMB1351_CHARGER_H__
#define __SMB1351_CHARGER_H__

#include "include/sec_charging_common.h"

static int fast_chg_current[] = {
	1000, 1200, 1400, 1600, 1800, 2000, 2200, 2400,
	2600, 2600, 2600, 2600, 2600, 2600, 2600, 2600,
};

static int ac_input_current[] = {
	500, 685, 1000, 1100, 1200, 1300, 1500, 1600,
	1700, 1800, 2000, 2200, 2500, 3000,
};

enum {
	MODE_5V = 0,
	MODE_QC20,
	MODE_QC30,
};

enum {
	MODE = 0,
	DATA,
};

#define SEC_INPUT_VOLTAGE_5V    5
#define SEC_INPUT_VOLTAGE_9V    9
#define SEC_INPUT_VOLTAGE_10V   10
#define SEC_INPUT_VOLTAGE_12V   12

ssize_t smb1351_chg_show_attrs(struct device *dev,
		struct device_attribute *attr, char *buf);

ssize_t smb1351_chg_store_attrs(struct device *dev,
		struct device_attribute *attr,
		const char *buf, size_t count);

#define SMB1351_CHARGER_ATTR(_name)                            \
{                                                       \
	.attr = {.name = #_name, .mode = 0664}, \
	.show = smb1351_chg_show_attrs,                        \
	.store = smb1351_chg_store_attrs,                      \
}

/* Mask/Bit helpers */
#define _SMB1351_MASK(BITS, POS) \
	((unsigned char)(((1 << (BITS)) - 1) << (POS)))
#define SMB1351_MASK(LEFT_BIT_POS, RIGHT_BIT_POS) \
	_SMB1351_MASK((LEFT_BIT_POS) - (RIGHT_BIT_POS) + 1, \
			(RIGHT_BIT_POS))

#define SMB1351_CHG_CURRENT_CTRL_REG            0x0
#define FAST_CHG_CURRENT_MASK                   SMB1351_MASK(7, 4)
#define AC_INPUT_CURRENT_LIMIT_MASK             SMB1351_MASK(3, 0)

#define SMB1351_OTHER_CHARGE_CURRENTS_REG		0x1
#define TERMINATION_CURRENT_SHIFT				2
#define TERMINATION_CURRENT_MASK				SMB1351_MASK(4, 2)

#define SMB1351_VARIOUS_FUNC_REG				0x2
#define SUSPEND_MODE_CTRL_BIT                   BIT(7)
#define SUSPEND_MODE_CTRL_BY_PIN                0
#define SUSPEND_MODE_CTRL_BY_I2C                0x80
#define BATT_TO_SYS_POWER_CTRL_BIT              BIT(6)
#define MAX_SYS_VOLTAGE                         BIT(5)
#define AICL_EN_BIT                             BIT(4)
#define AICL_DET_TH_BIT                         BIT(3)
#define APSD_EN_BIT                             BIT(2)
#define BATT_OV_BIT                             BIT(1)
#define VCHG_FUNC_BIT                           BIT(0)

#define SMB1351_FLOAT_VOLTAGE_REG				0x3
#define FLOAT_VOLTAGE_MASK						SMB1351_MASK(5, 0)

#define SMB1351_CHARGE_CONTROL_REG				0x4
#define TURBO_CHARGE_MASK						SMB1351_MASK(2, 0)

#define SMB1351_CHG_PIN_EN_CTRL_REG             0x6
#define LED_BLINK_FUNC_BIT                      BIT(7)
#define EN_PIN_CTRL_MASK                        SMB1351_MASK(6, 5)
#define EN_BY_I2C_0_DISABLE                     0
#define EN_BY_I2C_0_ENABLE                      0x20
#define EN_BY_PIN_HIGH_ENABLE                   0x40
#define EN_BY_PIN_LOW_ENABLE                    0x60
#define USBCS_CTRL_BIT                          BIT(4)
#define USBCS_CTRL_BY_I2C                       0
#define USBCS_CTRL_BY_PIN                       0x10
#define USBCS_INPUT_STATE_BIT                   BIT(3)
#define CHG_ERR_BIT                             BIT(2)
#define APSD_DONE_BIT                           BIT(1)
#define USB_FAIL_BIT                            BIT(0)

#define SMB1351_WATCHDOG_SAFETY_TIMER_REG		0x8
#define WATCHDOG_TIMER_MASK						SMB1351_MASK(6, 5)
#define SAFETY_TIMER_MASK						SMB1351_MASK(4, 3)

#define SMB1351_VARIOUS_FUNC2_REG               0xE
#define CHG_HOLD_OFF_TIMER_AFTER_PLUGIN_BIT     BIT(7)
#define CHG_INHIBIT_BIT                         BIT(6)
#define FAST_CHG_CC_IN_BATT_SOFT_LIMIT_MODE_BIT BIT(5)
#define FVCL_IN_BATT_SOFT_LIMIT_MODE_MASK       SMB1351_MASK(4, 3)
#define HARD_TEMP_LIMIT_BEHAVIOR_BIT            BIT(2)
#define PRECHG_TO_FASTCHG_BIT                   BIT(1)
#define STAT_PIN_CONFIG_BIT                     BIT(0)

#define SMB1351_FLEXCHARGE_REG					0x10
#define CHARGER_CONFIGURATION_MASK				SMB1351_MASK(6, 4)

#define SMB1351_VARIOUS_FUNC3_REG               0x11
#define QUICK3_AUTO_INCREMENT_BIT				BIT(2)
#define QUICK3_AUTO_AUTHENTICATION_BIT			BIT(1)

#define SMB1351_HVDCP_BATTMISSING_CTRL_REG      0x12
#define HVDCP_ADAPTER_SEL_MASK                  SMB1351_MASK(7, 6)
#define HVDCP_EN_BIT                            BIT(5)
#define HVDCP_AUTO_INCREMENT_LIMIT_BIT          BIT(4)
#define BATT_MISSING_ON_INPUT_PLUGIN_BIT        BIT(3)
#define BATT_MISSING_2P6S_POLLER_BIT            BIT(2)
#define BATT_MISSING_ALGO_BIT                   BIT(1)
#define BATT_MISSING_THERM_PIN_SOURCE_BIT       BIT(0)

#define SMB1351_OTG_MODE_POWER_OPTIONS_REG      0x14
#define ADAPTER_CONFIG_MASK                     SMB1351_MASK(7, 6)
#define MAP_HVDCP_BIT                           BIT(5)
#define SDP_LOW_BATT_FORCE_USB5_OVER_USB1_BIT   BIT(4)
#define OTG_HICCUP_MODE_BIT                     BIT(2)
#define INPUT_CURRENT_LIMIT_MASK                SMB1351_MASK(1, 0)

#define SMB1351_FACTORY_SETTING			0x15
#define DISABLE_AFVC_ENTERING_TAPER_BIT		BIT(3)

/* Command registers */
#define SMB1351_CMD_I2C_REG                     0x30
#define CMD_RELOAD_BIT                          BIT(7)
#define CMD_BQ_CFG_ACCESS_BIT                   BIT(6)

#define SMB1351_CMD_IL_REG			0x31
#define INPUT_CURRENT_MODE			BIT(3)
#define SUSPEND_MODE_BIT			BIT(6)
#define USB_CONTROL_BIT				SMB1351_MASK(2, 0)

#define SMB1351_CMD_CHG_REG                     0x32
#define CMD_DISABLE_THERM_MONITOR_BIT           BIT(4)
#define CMD_TURN_OFF_STAT_PIN_BIT               BIT(3)
#define CMD_PRE_TO_FAST_EN_BIT                  BIT(2)
#define CMD_CHG_EN_BIT                          BIT(1)
#define CMD_CHG_DISABLE                         0
#define CMD_CHG_ENABLE                          0x2
#define CMD_OTG_EN_BIT                          BIT(0)

#define SMB1351_CMD_HVDCP_REG                   0x34
#define CMD_APSD_RE_RUN_BIT                     BIT(7)
#define CMD_FORCE_HVDCP_2P0_BIT                 BIT(5)
#define CMD_QC_3P0_AUTO_INCREMENT_MODE_BIT		BIT(4)
#define CMD_DECREMENT_200MV_BIT					BIT(2)
#define CMD_INCREMENT_200MV_BIT					BIT(0)
#define CMD_HVDCP_MODE_MASK                     SMB1351_MASK(5, 0)

/* Status registers */
#define SMB1351_STATUS0_REG                     0x36
#define STATUS_AICL_BIT                         BIT(7)
#define STATUS_INPUT_CURRENT_LIMIT_MASK         SMB1351_MASK(6, 5)
#define STATUS_DCIN_INPUT_CURRENT_LIMIT_MASK    SMB1351_MASK(4, 0)

#define SMB1351_STATUS1_REG                     0x37
#define STATUS_INPUT_RANGE_MASK                 SMB1351_MASK(7, 4)
#define STATUS_INPUT_USB_BIT                    BIT(0)

#define SMB1351_STATUS2_REG                     0x38
#define STATUS_FAST_CHG_BIT                     BIT(7)
#define STATUS_HARD_LIMIT_BIT                   BIT(6)
#define STATUS_FLOAT_VOLTAGE_MASK               SMB1351_MASK(5, 0)

#define SMB1351_STATUS3_REG                     0x39
#define STATUS_CHG_BIT                          BIT(7)
#define STATUS_PRECHG_CURRENT_MASK              SMB1351_MASK(6, 4)
#define STATUS_FAST_CHG_CURRENT_MASK            SMB1351_MASK(3, 0)

#define SMB1351_STATUS4_REG                     0x3A
#define STATUS_OTG_BIT                          BIT(7)
#define STATUS_AFVC_BIT                         BIT(6)
#define STATUS_DONE_BIT                         BIT(5)
#define STATUS_BATT_LESS_THAN_2V_BIT            BIT(4)
#define STATUS_HOLD_OFF_BIT                     BIT(3)
#define STATUS_CHG_MASK                         SMB1351_MASK(2, 1)
#define STATUS_NO_CHARGING                      0
#define STATUS_FAST_CHARGING                    0x4
#define STATUS_PRE_CHARGING                     0x2
#define STATUS_TAPER_CHARGING                   0x6
#define STATUS_CHG_EN_STATUS_BIT                BIT(0)

#define SMB1351_STATUS5_REG                     0x3B
#define STATUS_SOURCE_DETECTED_MASK             SMB1351_MASK(7, 0)
#define STATUS_PORT_CDP                         0x80
#define STATUS_PORT_DCP                         0x40
#define STATUS_PORT_OTHER                       0x20
#define STATUS_PORT_SDP                         0x10
#define STATUS_PORT_ACA_A                       0x8
#define STATUS_PORT_ACA_B                       0x4
#define STATUS_PORT_ACA_C                       0x2
#define STATUS_PORT_ACA_DOCK                    0x1

#define SMB1351_STATUS6_REG                     0x3C
#define STATUS_DCD_TIMEOUT_BIT                  BIT(7)
#define STATUS_DCD_GOOD_DG_BIT                  BIT(6)
#define STATUS_OCD_GOOD_DG_BIT                  BIT(5)
#define STATUS_RID_ABD_DG_BIT                   BIT(4)
#define STATUS_RID_FLOAT_STATE_MACHINE_BIT      BIT(3)
#define STATUS_RID_A_STATE_MACHINE_BIT          BIT(2)
#define STATUS_RID_B_STATE_MACHINE_BIT          BIT(1)
#define STATUS_RID_C_STATE_MACHINE_BIT          BIT(0)

#define SMB1351_STATUS7_REG                     0x3D
#define STATUS_HVDCP_MASK                       SMB1351_MASK(7, 0)

#define SMB1351_STATUS8_REG                     0x3E
#define STATUS_USNIN_HV_INPUT_SEL_BIT           BIT(5)
#define STATUS_USBIN_LV_UNDER_INPUT_SEL_BIT     BIT(4)
#define STATUS_USBIN_LV_INPUT_SEL_BIT           BIT(3)

/* Revision register */
#define CHG_REVISION_REG                        0x3F
#define GUI_REVISION_MASK                       SMB1351_MASK(7, 4)
#define DEVICE_REVISION_MASK                    SMB1351_MASK(3, 0)

/* IRQ status registers */
#define IRQ_A_REG                               0x40
#define IRQ_HOT_HARD_BIT                        BIT(6)
#define IRQ_COLD_HARD_BIT                       BIT(4)
#define IRQ_HOT_SOFT_BIT                        BIT(2)
#define IRQ_COLD_SOFT_BIT                       BIT(0)

#define IRQ_B_REG                               0x41
#define IRQ_BATT_TERMINAL_REMOVED_BIT           BIT(6)
#define IRQ_BATT_MISSING_BIT                    BIT(4)
#define IRQ_LOW_BATT_VOLTAGE_BIT                BIT(2)
#define IRQ_INTERNAL_TEMP_LIMIT_BIT             BIT(0)

#define IRQ_C_REG                               0x42
#define IRQ_PRE_TO_FAST_VOLTAGE_BIT             BIT(6)
#define IRQ_RECHG_BIT                           BIT(4)
#define IRQ_TAPER_BIT                           BIT(2)
#define IRQ_TERM_BIT                            BIT(0)

#define IRQ_D_REG                               0x43
#define IRQ_BATT_OV_BIT                         BIT(6)
#define IRQ_CHG_ERROR_BIT                       BIT(4)
#define IRQ_CHG_TIMEOUT_BIT                     BIT(2)
#define IRQ_PRECHG_TIMEOUT_BIT                  BIT(0)

#define IRQ_E_REG                               0x44
#define IRQ_USBIN_OV_BIT                        BIT(6)
#define IRQ_USBIN_UV_BIT                        BIT(4)
#define IRQ_AFVC_BIT                            BIT(2)
#define IRQ_POWER_OK_BIT                        BIT(0)

#define IRQ_F_REG                               0x45
#define IRQ_OTG_OVER_CURRENT_BIT                BIT(6)
#define IRQ_OTG_FAIL_BIT                        BIT(4)
#define IRQ_RID_BIT                             BIT(2)
#define IRQ_OTG_OC_RETRY_BIT                    BIT(0)

#define IRQ_G_REG                               0x46
#define IRQ_SOURCE_DET_BIT                      BIT(6)
#define IRQ_AICL_DONE_BIT                       BIT(4)
#define IRQ_AICL_FAIL_BIT                       BIT(2)
#define IRQ_CHG_INHIBIT_BIT                     BIT(0)

#define IRQ_H_REG                               0x47
#define IRQ_IC_LIMIT_STATUS_BIT                 BIT(5)
#define IRQ_HVDCP_2P1_STATUS_BIT                BIT(4)
#define IRQ_HVDCP_AUTH_DONE_BIT                 BIT(2)
#define IRQ_WDOG_TIMEOUT_BIT                    BIT(0)

/* constants */
#define USB2_MIN_CURRENT_MA                     100
#define USB2_MAX_CURRENT_MA                     500
#define USB3_MIN_CURRENT_MA                     150
#define USB3_MAX_CURRENT_MA                     900
#define SMB1351_IRQ_REG_COUNT                   8
#define SMB1351_CHG_PRE_MIN_MA                  100
#define SMB1351_CHG_FAST_MIN_MA                 1000
#define SMB1351_CHG_FAST_MAX_MA                 4500
#define SMB1351_CHG_PRE_SHIFT                   5
#define SMB1351_CHG_FAST_SHIFT                  4
#define DEFAULT_BATT_CAPACITY                   50
#define DEFAULT_BATT_TEMP                       250
#define SUSPEND_CURRENT_MA                      2

#define CHG_ITERM_70MA                          0x1C
#define CHG_ITERM_100MA                         0x18
#define CHG_ITERM_200MA                         0x0
#define CHG_ITERM_300MA                         0x04
#define CHG_ITERM_400MA                         0x08
#define CHG_ITERM_500MA                         0x0C
#define CHG_ITERM_600MA                         0x10
#define CHG_ITERM_700MA                         0x14

#define ADC_TM_WARM_COOL_THR_ENABLE             ADC_TM_HIGH_LOW_THR_ENABLE

#define INPUT_CURRENT_MODE_AUTO         0
#define INPUT_CURRENT_MODE_MANUAL 	1

struct smb1351_charger_platform_data {
        int irq_gpio;
        int chg_en;
};

struct smb1351_charger_data {
	struct device           *dev;
	struct i2c_client       *i2c;
	struct mutex            io_lock;

	struct smb1351_charger_platform_data *pdata;

	struct power_supply     *psy_chg;
	struct workqueue_struct *wqueue;
	struct delayed_work     isr_work;
	struct delayed_work     qc_work;

	struct wake_lock qc_wake_lock;

	unsigned int siop_level;
	unsigned int chg_irq;
	unsigned int is_charging;
	unsigned int charging_type;
	unsigned int cable_type;
	unsigned int charging_current_max;
	unsigned int charging_current;
	unsigned int topoff_current;
	unsigned int float_voltage;

	u8 addr;
	int size;

	bool is_init;
	int apsd_en;
	int charge_mode;
};

#endif  /* __SMB1351_CHARGER_H__ */
