/*
 *  sm5720_charger.c
 *  Samsung SM5720 Charger Driver
 *
 *  Copyright (C) 2016 Samsung Electronics
 *
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#include <linux/debugfs.h>
#include <linux/seq_file.h>
#include <linux/power_supply.h>
#include "include/sec_charging_common.h"
#include "include/charger/sm5720_charger.h"

#ifdef CONFIG_USB_HOST_NOTIFY
#include <linux/usb_notify.h>
#endif

static enum power_supply_property sm5720_charger_props[] = {
};

static int sm5720_set_input_current(struct sm5720_charger_data *charger, unsigned short limit_mA);
static int sm5720_set_wireless_input_current(struct sm5720_charger_data *charger, unsigned short limit_mA);
static unsigned short sm5720_get_input_current(struct sm5720_charger_data *charger);
static int sm5720_set_charge_current(struct sm5720_charger_data *charger, unsigned short chg_mA);
static unsigned short sm5720_get_charge_current(struct sm5720_charger_data *charger);
static void sm5720_charger_enable_aicl_irq(struct sm5720_charger_data *charger);
static void sm5720_charger_initialize(struct sm5720_charger_data *charger);

static inline u8 _calc_limit_current_offset_to_mA(unsigned short mA)
{
	unsigned char offset;

	if (mA < 100) {
		offset = 0x00;
	} else {
		offset = ((mA - 100) / 25) & 0x7F;
	}

	return offset;
}

static inline unsigned short _calc_limit_current_mA_to_offset(u8 offset)
{
	return (offset * 25) + 100;
}

static inline u8 _calc_chg_current_offset_to_mA(unsigned short mA)
{
	unsigned char offset;

	if (mA < 100) {
		offset = 0x00;
	} else {
		if (mA > 3250)
			mA = 3250;
		offset = ((mA - 100) / 50) & 0x3F;
	}

	return offset;
}

static inline unsigned short _calc_chg_current_mA_to_offset(u8 offset)
{
	return (offset * 50) + 100;
}

static inline u8 _calc_bat_float_voltage_offset_to_mV(unsigned short mV)
{
	unsigned char offset;

	if (mV < 40000) {
		offset = 0x0;     /* BATREG = 3.8V */
	} else if (mV < 40100) {
		offset = 0x1;     /* BATREG = 4.0V */
	} else if (mV < 46300) {
		offset = (((mV - 40100) / 100) + 2);    /* BATREG = 4.01 ~ 4.62 in 0.01V steps */
	} else {
		pr_err("sm5720-charger: %s: can't support BATREG at over voltage 4.62V (mV=%d)\n", __func__, mV);
		offset = 0x15;    /* default Offset : 4.2V */
	}

	return offset;
}

static inline unsigned char _calc_topoff_current_offset_to_mA(unsigned short mA)
{
	unsigned char offset;

	if (mA < 100) {
		offset = 0x0;               /* Topoff = 100mA */
	} else if (mA < 600) {
		offset = (mA - 100) / 25;   /* Topoff = 125mA ~ 575mA in 25mA steps */
	} else {
		offset = 0x1F;              /* Topoff = 600mA */
	}

	return offset;
}

static inline unsigned short _calc_topoff_mA_to_offset(unsigned char offset)
{
    unsigned short mA;

    if (offset < 0x15) {
        mA = (offset * 25) + 100;
    } else {
        mA = 600;
    }

    return mA;
}

static inline unsigned short _calc_bat_float_voltage_mV_to_offset(u8 offset)
{
	return (((offset - 2) * 10) + 4010) * 10;
}

static int sm5720_CHG_set_ENQ4FET(struct sm5720_charger_data *charger, bool enable)
{
    bool bit_value;

	if(charger->pmic_ver < SM5720_PASS7) {
        bit_value = enable & 0x1;           /* if reversion < PASS7, used ENQ4FET */
    } else {
        bit_value = enable ? 0 : 1;         /* if reversion >= PASS7, used nENQ4FET */
    }

	sm5720_update_reg(charger->i2c, SM5720_CHG_REG_CNTL1, (bit_value << 3), (0x1 << 3));

	dev_dbg(charger->dev, "%s: Q4FET = %s\n", __func__, enable ? "enable" : "disable");

	return 0;
}

static int sm5720_CHG_set_AUTOSET(struct sm5720_charger_data *charger, bool enable)
{
	sm5720_update_reg(charger->i2c, SM5720_CHG_REG_CNTL1, ((enable & 0x1) << 4), (0x1 << 4));

	return 0;
}

static int sm5720_CHG_get_AUTOSET(struct sm5720_charger_data *charger)
{
	u8 reg = 0x0;

	sm5720_read_reg(charger->i2c, SM5720_CHG_REG_CNTL1, &reg);

	reg = (reg >> 4) & 0x1;

	return reg;
}

static int sm5720_CHG_set_AICLEN_WPC(struct sm5720_charger_data *charger, bool enable)
{
	sm5720_update_reg(charger->i2c, SM5720_CHG_REG_CNTL1, ((enable & 0x1) << 5), (0x1 << 5));

	return 0;
}

static int sm5720_CHG_set_AICLEN_VBUS(struct sm5720_charger_data *charger, bool enable)
{
	sm5720_update_reg(charger->i2c, SM5720_CHG_REG_CNTL1, ((enable & 0x1) << 6), (0x1 << 6));

	return 0;
}

static int sm5720_set_input_current(struct sm5720_charger_data *charger, unsigned short limit_mA)
{
	u8 offset = _calc_limit_current_offset_to_mA(limit_mA);

	dev_info(charger->dev, "set Input LIMIT src=0x%02X, current=%dmA, offset=0x%02x\n",
		is_wireless_type(charger->cable_type) ? SM5720_CHG_REG_WPCINCNTL:SM5720_CHG_REG_VBUSCNTL,
		limit_mA, offset);

	mutex_lock(&charger->charger_mutex);

	if (is_wireless_type(charger->cable_type))
		sm5720_update_reg(charger->i2c, SM5720_CHG_REG_WPCINCNTL, ((offset & 0x7F) << 0), (0x7F << 0));
	else
		sm5720_update_reg(charger->i2c, SM5720_CHG_REG_VBUSCNTL, ((offset & 0x7F) << 0), (0x7F << 0));

	mutex_unlock(&charger->charger_mutex);

	return 0;
}

static int sm5720_set_wireless_input_current(struct sm5720_charger_data *charger, unsigned short limit_mA)
{
	union power_supply_propval value;

	/* Wcurr-A) In cases of wireless input current change,
		configure the Vrect adj room to 270mV for safe wireless charging. */
	if (is_wireless_type(charger->cable_type)) {
		wake_lock(&charger->wpc_current_wake_lock);

		value.intval = WIRELESS_VRECT_ADJ_ROOM_1; /* 270mV */
		psy_do_property(charger->wireless_charger_name, set,
			POWER_SUPPLY_PROP_INPUT_VOLTAGE_REGULATION, value);
		msleep(500); /* delay 0.5sec */
		charger->wc_pre_current = sm5720_get_input_current(charger);
		charger->wc_current = limit_mA;
		if (charger->wc_current > charger->wc_pre_current) {
			sm5720_set_charge_current(charger, charger->charging_current);
		}
	}
	queue_delayed_work(charger->wqueue, &charger->wc_current_work, 0);

	return 0;
}

static unsigned short sm5720_get_input_current(struct sm5720_charger_data *charger)
{
	u8 offset = 0x0;
	unsigned short LIMIT_mA = 0;

	if (is_wireless_type(charger->cable_type))
		sm5720_read_reg(charger->i2c, SM5720_CHG_REG_WPCINCNTL, &offset);
	else
		sm5720_read_reg(charger->i2c, SM5720_CHG_REG_VBUSCNTL, &offset);

	LIMIT_mA = _calc_limit_current_mA_to_offset(offset & 0x7F);

	dev_info(charger->dev, "get INPUT LIMIT src=0x%02X, offset=0x%02x, current=%dmA\n",
		is_wireless_type(charger->cable_type) ? SM5720_CHG_REG_WPCINCNTL:SM5720_CHG_REG_VBUSCNTL,
		offset, LIMIT_mA);

	return LIMIT_mA;
}

static int sm5720_set_charge_current(struct sm5720_charger_data *charger, unsigned short chg_mA)
{
	u8 offset = _calc_chg_current_offset_to_mA(chg_mA);

	dev_info(charger->dev, "set FASTCHG src=0x%02X, current=%dmA, offset=0x%02x\n",
		is_wireless_type(charger->cable_type) ? SM5720_CHG_REG_CHGCNTL3:SM5720_CHG_REG_CHGCNTL2,
		chg_mA, offset);

	if (is_wireless_type(charger->cable_type))
		sm5720_update_reg(charger->i2c, SM5720_CHG_REG_CHGCNTL3, ((offset & 0x3F) << 0), (0x3F << 0));
	else
		sm5720_update_reg(charger->i2c, SM5720_CHG_REG_CHGCNTL2, ((offset & 0x3F) << 0), (0x3F << 0));

	return 0;
}

static unsigned short sm5720_get_charge_current(struct sm5720_charger_data *charger)
{
	u8 offset = 0x0;
	unsigned short FASTCHG_mA = 0;

	if (is_wireless_type(charger->cable_type))
		sm5720_read_reg(charger->i2c, SM5720_CHG_REG_CHGCNTL3, &offset);
	else
		sm5720_read_reg(charger->i2c, SM5720_CHG_REG_CHGCNTL2, &offset);

	FASTCHG_mA = _calc_chg_current_mA_to_offset(offset & 0x3F);

	dev_info(charger->dev, "get FASTCHG src=0x%02X, offset=0x%02x, current=%dmA\n",
		is_wireless_type(charger->cable_type) ? SM5720_CHG_REG_CHGCNTL3:SM5720_CHG_REG_CHGCNTL2,
		offset, FASTCHG_mA);

	return FASTCHG_mA;
}

static int sm5720_CHG_set_VSYS_MIN(struct sm5720_charger_data *charger, u8 val)
{
	sm5720_update_reg(charger->i2c, SM5720_CHG_REG_CHGCNTL1, ((val & 0x7) << 2), (0x7 << 2));

	return 0;
}

static int sm5720_CHG_set_AUTOSTOP(struct sm5720_charger_data *charger, bool enable)
{
	sm5720_update_reg(charger->i2c, SM5720_CHG_REG_CHGCNTL4, ((enable & 0x1) << 6), (0x1 << 6));

	return 0;
}

static int sm5720_CHG_set_BATREG(struct sm5720_charger_data *charger, unsigned short float_mV)
{
	u8 offset = _calc_bat_float_voltage_offset_to_mV(float_mV);

	sm5720_update_reg(charger->i2c, SM5720_CHG_REG_CHGCNTL4, ((offset & 0x3F) << 0), (0x3F << 0));

	return 0;
}

static unsigned short sm5720_CHG_get_BATREG(struct sm5720_charger_data *charger)
{
	u8 offset = 0x0;

	sm5720_read_reg(charger->i2c, SM5720_CHG_REG_CHGCNTL4, &offset);

	return _calc_bat_float_voltage_mV_to_offset(offset & 0x3F);
}

static u8 sm5720_CHG_get_CVMODE(struct sm5720_charger_data *charger)
{
	u8 cv_mode = 0x0;

	sm5720_read_reg(charger->i2c, SM5720_CHG_REG_MODESTATUS, &cv_mode);
	cv_mode = cv_mode & 0x01;

	return cv_mode;
}

static int sm5720_CHG_set_TOPOFF(struct sm5720_charger_data *charger, unsigned short topoff_mA)
{
	u8 offset = _calc_topoff_current_offset_to_mA(topoff_mA);

	sm5720_update_reg(charger->i2c, SM5720_CHG_REG_CHGCNTL5, ((offset & 0x1F) << 0), (0x1F << 0));

	return 0;
}

static unsigned short sm5720_CHG_get_TOPOFF(struct sm5720_charger_data *charger)
{
    u8 offset = 0x0;

    sm5720_read_reg(charger->i2c, SM5720_CHG_REG_CHGCNTL5, &offset);

    return _calc_topoff_mA_to_offset(offset & 0x1F);
}

static int sm5720_CHG_set_SS_LIM_VBUS(struct sm5720_charger_data *charger, u8 val)
{
	sm5720_update_reg(charger->i2c, SM5720_CHG_REG_VBUSCNTL, ((val & 0x1) << 7), (0x1 << 7));

	return 0;
}

static int sm5720_CHG_set_SS_FAST_VBUS(struct sm5720_charger_data *charger, u8 val)
{
	sm5720_update_reg(charger->i2c, SM5720_CHG_REG_CHGCNTL2, ((val & 0x1) << 6), (0x1 << 6));

	return 0;
}

static int sm5720_CHG_set_AICLTH(struct sm5720_charger_data *charger, u8 val)
{
	sm5720_update_reg(charger->i2c, SM5720_CHG_REG_CHGCNTL6, ((val & 0x3) << 6), (0x3 << 6));

	return 0;
}

static int sm5720_CHG_set_DISCHARGE(struct sm5720_charger_data *charger, u8 val)
{
	sm5720_update_reg(charger->i2c, SM5720_CHG_REG_CHGCNTL6, ((val & 0x3) << 2), (0x3 << 2));

	return 0;
}

static int sm5720_CHG_set_TOPOFFTIMER(struct sm5720_charger_data *charger, u8 val)
{
	sm5720_update_reg(charger->i2c, SM5720_CHG_REG_CHGCNTL7, ((val & 0x3) << 3), (0x3 << 3));

	return 0;
}

#if defined(SM5720_WATCHDOG_RESET_ACTIVATE)
static int sm5720_CHG_set_WATCHDOG_TMR(struct sm5720_charger_data *charger, u8 val)
{
	sm5720_update_reg(charger->i2c, SM5720_CHG_REG_WDTCNTL, ((val & 0x3) << 1), (0x3 << 1));

	return 0;
}

static int sm5720_CHG_set_ENWATCHDOG(struct sm5720_charger_data *charger, bool enable)
{
	pr_info("%s: WDT en = %d \n", __func__, enable);

	sm5720_CHG_set_WATCHDOG_TMR(charger, WATCHDOG_TIMER_120s);
	sm5720_update_reg(charger->i2c, SM5720_CHG_REG_WDTCNTL, ((enable & 0x1) << 0), (0x1 << 0));

	return 0;
}

static int sm5720_CHG_set_WDTMR_KICK(struct sm5720_charger_data *charger)
{
	pr_info("%s: WDT Kick \n", __func__);

	sm5720_update_reg(charger->i2c, SM5720_CHG_REG_WDTCNTL, (0x1 << 3), (0x1 << 3));

	return 0;
}

static bool sm5720_CHG_get_WDTMR_STATUS(struct sm5720_charger_data *charger)
{
	u8 reg = 0;
	int ret = 0;

	ret = sm5720_read_reg(charger->i2c, SM5720_CHG_REG_STATUS2, &reg);

	if ( !ret && (reg & (0x1 << 7)) ) {
		dev_info(charger->dev, "WDT expired 0x%x !! \n", reg);
		return true;
	} else
		return false;
}
#endif

static int sm5720_CHG_set_OTGCURRENT(struct sm5720_charger_data *charger, u8 val)
{
	sm5720_update_reg(charger->i2c, SM5720_CHG_REG_BSTCNTL1, ((val & 0x3) << 6), (0x3 << 6));

	return 0;
}

static int sm5720_CHG_set_BST_IQ3LIMIT(struct sm5720_charger_data *charger, u8 val)
{
	sm5720_update_reg(charger->i2c, SM5720_CHG_REG_BSTCNTL1, ((val & 0x3) << 4), (0x3 << 4));

	return 0;
}

static void sm5720_chg_print_reg(struct sm5720_charger_data *charger)
{
	u8 regs[SM5720_CHG_REG_END] = {0x0, };
	int i;

	sm5720_bulk_read(charger->i2c, SM5720_CHG_REG_STATUS1, (SM5720_CHG_REG_RGBWCNTL1 - SM5720_CHG_REG_STATUS1), regs);

	pr_info("sm5720-charger: ");

	for (i = 0; i < (SM5720_CHG_REG_RGBWCNTL1 - SM5720_CHG_REG_STATUS1); ++i) {
		pr_info("0x%x:0x%x ", SM5720_CHG_REG_STATUS1 + i, regs[i]);
	}
}

static int psy_chg_set_cable_online(struct sm5720_charger_data *charger, int cable_type)
{
	int prev_cable_type = charger->cable_type;

	charger->slow_late_chg_mode = false;
	charger->cable_type = cable_type;


	charger->input_current = sm5720_get_input_current(charger);

	dev_info(charger->dev, "[start] prev_cable_type(%d), cable_type(%d), op_mode(%d), op_status(0x%x)",
			prev_cable_type, cable_type, sm5720_charger_oper_get_current_op_mode(), sm5720_charger_oper_get_current_status());

	if (charger->cable_type == SEC_BATTERY_CABLE_NONE) {
		charger->wc_pre_current = WC_CURRENT_START;
		wake_unlock(&charger->wpc_wake_lock);

		/* set default value */
		sm5720_charger_oper_push_event(SM5720_CHARGER_OP_EVENT_VBUSIN, 0);
		sm5720_charger_oper_push_event(SM5720_CHARGER_OP_EVENT_WPCIN, 0);
		sm5720_charger_oper_push_event(SM5720_CHARGER_OP_EVENT_PWR_SHAR, 0);

		/* set default input current */
		sm5720_set_input_current(charger, 500);
		sm5720_set_wireless_input_current(charger, 400);
		if (charger->irq_aicl_enabled == 0) {
			u8 reg_data;
			charger->irq_aicl_enabled = 1;
			enable_irq(charger->irq_aicl);
			sm5720_read_reg(charger->i2c, SM5720_CHG_REG_INTMSK2, &reg_data);
			pr_info("%s: enable aicl : 0x%x\n", __func__, reg_data);
		}
	} else {
		if (sm5720_CHG_get_AUTOSET(charger) == true) {
			pr_info("Charger IC Reset!!! - do re-initialize process\n");
			sm5720_charger_initialize(charger);
		}

		if (is_wireless_type(charger->cable_type)) {
			sm5720_charger_oper_push_event(SM5720_CHARGER_OP_EVENT_VBUSIN, 0);
			sm5720_charger_oper_push_event(SM5720_CHARGER_OP_EVENT_WPCIN, 1);
		} else if (charger->cable_type != SEC_BATTERY_CABLE_OTG) {
			sm5720_charger_oper_push_event(SM5720_CHARGER_OP_EVENT_WPCIN, 0);
			sm5720_charger_oper_push_event(SM5720_CHARGER_OP_EVENT_VBUSIN, 1);
		}
		if (is_hv_wire_type(charger->cable_type) ||
			(charger->cable_type == SEC_BATTERY_CABLE_HV_TA_CHG_LIMIT)) {
			if (charger->irq_aicl_enabled == 1) {
				u8 reg_data;

				charger->irq_aicl_enabled = 0;
				disable_irq_nosync(charger->irq_aicl);
				cancel_delayed_work_sync(&charger->aicl_work);
				sm5720_read_reg(charger->i2c, SM5720_CHG_REG_INTMSK2, &reg_data);
				pr_info("%s: disable aicl : 0x%x\n", __func__, reg_data);
				charger->slow_late_chg_mode = false;
			}
		}
	}
	dev_info(charger->dev, "[end] is_charging=%d, fc = %d, il = %d, cable = %d,"
			"op_mode(%d), op_status(0x%x)\n",
			charger->is_charging,
			charger->charging_current,
			charger->input_current,
			charger->cable_type,
			sm5720_charger_oper_get_current_op_mode(),
			sm5720_charger_oper_get_current_status());

	return 0;
}

static void psy_chg_set_charging_enabled(struct sm5720_charger_data *charger, int charge_mode)
{
	int buck_off = false;

	dev_info(charger->dev, "charger_mode changed [%d] -> [%d]\n", charger->charge_mode, charge_mode);

	charger->charge_mode = charge_mode;

	switch (charger->charge_mode) {
		case SEC_BAT_CHG_MODE_BUCK_OFF:
			buck_off = true;
			break;
		case SEC_BAT_CHG_MODE_CHARGING_OFF:
			charger->is_charging = false;
			break;
		case SEC_BAT_CHG_MODE_CHARGING:
			charger->is_charging = true;
			break;
	}

#if defined(SM5720_WATCHDOG_RESET_ACTIVATE)
	/* Watchdog-Timer Kick
		Watchdog-Timer is able to work from PASS5 */
	if(charger->pmic_ver >= SM5720_PASS5) {
	    if (charger->charge_mode != SEC_BAT_CHG_MODE_CHARGING)
	        sm5720_CHG_set_ENWATCHDOG(charger, false);
	    else
	        sm5720_CHG_set_ENWATCHDOG(charger, charger->is_charging);
	}
#endif
	sm5720_CHG_set_ENQ4FET(charger, charger->is_charging);
	sm5720_charger_oper_push_event(SM5720_CHARGER_OP_EVENT_SUSPEND, buck_off);
}

static int psy_chg_get_charger_status(struct sm5720_charger_data *charger)
{
	int status = POWER_SUPPLY_STATUS_UNKNOWN;
	u8 reg;

	sm5720_read_reg(charger->i2c, SM5720_CHG_REG_STATUS2, &reg);
	dev_info(charger->dev, "%s: INT_STATUS2=0x%x\n", __func__, reg);

	if (reg & (0x1 << 5)) { /* check: Top-off */
		status = POWER_SUPPLY_STATUS_FULL;
	} else if (reg & (0x1 << 3)) {  /* check: Charging ON */
		status = POWER_SUPPLY_STATUS_CHARGING;
	} else {
		sm5720_read_reg(charger->i2c, SM5720_CHG_REG_STATUS1, &reg);
		dev_info(charger->dev, "%s: INT_STATUS1=0x%x\n", __func__, reg);

		if ((reg & (0x1 << 0)) || (reg & (0x1 << 4))) { /* check: VBUS_POK, WPC_POK */
			status = POWER_SUPPLY_STATUS_NOT_CHARGING;
		} else {
			status = POWER_SUPPLY_STATUS_DISCHARGING;
		}
	}

	return status;
}

static int psy_chg_get_online(struct sm5720_charger_data *charger)
{
	int type = SEC_BATTERY_CABLE_NONE;
	u8 reg;

	sm5720_read_reg(charger->i2c, SM5720_CHG_REG_STATUS1, &reg);
	dev_info(charger->dev, "%s: INT_STATUS1=0x%x\n", __func__, reg);

	if (reg & (0x1 << 0)) { /* check: VBUS_POK */
		type = SEC_BATTERY_CABLE_TA;
	} else if (reg & (0x1 << 4)) {  /* check: WPC_POK */
		type = SEC_BATTERY_CABLE_WIRELESS;
	}

	return type;
}

static int psy_chg_get_battery_present(struct sm5720_charger_data *charger)
{
	u8 reg;

	sm5720_read_reg(charger->i2c, SM5720_CHG_REG_STATUS2, &reg);
	dev_info(charger->dev, "%s: INT_STATUS2=0x%x\n", __func__, reg);

	if (reg & (0x1 << 2)) { /* check: NOBAT */
		return 0;
	}

	return 1;
}

static int psy_chg_get_charge_type(struct sm5720_charger_data *charger)
{
	int charge_type;

	if (charger->is_charging) {
		if (charger->slow_late_chg_mode) {
			dev_info(charger->dev, "%s: slow late charge mode\n", __func__);
			charge_type = POWER_SUPPLY_CHARGE_TYPE_SLOW;
		} else {
			charge_type = POWER_SUPPLY_CHARGE_TYPE_FAST;
		}
	} else {
		charge_type = POWER_SUPPLY_CHARGE_TYPE_NONE;
	}

	return charge_type;
}

static int psy_chg_get_charging_health(struct sm5720_charger_data *charger)
{
	u8 reg = 0;
	int offset = 0, health = POWER_SUPPLY_HEALTH_GOOD;
	union power_supply_propval value;

	sm5720_read_reg(charger->i2c, SM5720_CHG_REG_STATUS1, &reg);
	dev_info(charger->dev, "pmic_ver=%d, is_charging=%d, cable_type=%d, status1_reg=0x%x\n",
			charger->pmic_ver, charger->is_charging, charger->cable_type, reg);

	if (is_wireless_type(charger->cable_type)) {
		offset = 4;
	} else {
		offset = 0;
	}

	/* this function has to return only GOOD, UNDERVOLTAGE, OVERVOLTAGE */
	if (reg & (0x1 << (0 + offset))) {
		health = POWER_SUPPLY_HEALTH_GOOD;
	} else if (reg & (0x1 << (1 + offset))) {
		health = POWER_SUPPLY_HEALTH_UNDERVOLTAGE;
	} else if (reg & (0x1 << (2 + offset))) {
		health = POWER_SUPPLY_HEALTH_OVERVOLTAGE;
	}

	value.intval = health;
	psy_do_property("sm5720-fuelgauge", set, POWER_SUPPLY_PROP_HEALTH, value);

	/* Watchdog-Timer Kick
		Watchdog-Timer is able to work from PASS5 */
#if defined(SM5720_WATCHDOG_RESET_ACTIVATE)
	if(charger->pmic_ver >= SM5720_PASS5)
		sm5720_CHG_set_WDTMR_KICK(charger);
#endif

	return health;
}

static int sm5720_chg_get_property(struct power_supply *psy,
		enum power_supply_property psp,
		union power_supply_propval *val)
{
	struct sm5720_charger_data *charger = power_supply_get_drvdata(psy);
	int status;
	enum power_supply_ext_property ext_psp = psp;

	switch (psp) {
		case POWER_SUPPLY_PROP_STATUS:
			val->intval = psy_chg_get_charger_status(charger);
			break;
		case POWER_SUPPLY_PROP_CHARGING_ENABLED:
			break;
		case POWER_SUPPLY_PROP_PRESENT:
			val->intval = psy_chg_get_battery_present(charger);
			break;
		case POWER_SUPPLY_PROP_CHARGE_TYPE:
			val->intval = psy_chg_get_charge_type(charger);
			break;
		case POWER_SUPPLY_PROP_HEALTH:
			val->intval = psy_chg_get_charging_health(charger);
			sm5720_chg_print_reg(charger);
			break;
		case POWER_SUPPLY_PROP_ONLINE:
			val->intval = psy_chg_get_online(charger);
			break;
		/* get input current which was set */
		case POWER_SUPPLY_PROP_CURRENT_MAX:
			val->intval = charger->input_current;
			break;
		/* get input current which was read */
		case POWER_SUPPLY_PROP_CURRENT_AVG:
			val->intval = sm5720_get_input_current(charger);
			break;
		/* get charge current which was set */
		case POWER_SUPPLY_PROP_CURRENT_NOW:
			val->intval = charger->charging_current;
			break;
		/* get charge current which was read */
		case POWER_SUPPLY_PROP_CONSTANT_CHARGE_CURRENT:
			val->intval = sm5720_get_charge_current(charger);
			break;
		/* get cv mode */
		case POWER_SUPPLY_PROP_CHARGE_NOW:
			val->intval = sm5720_CHG_get_CVMODE(charger);
			break;
		case POWER_SUPPLY_PROP_CHARGE_OTG_CONTROL:
			status = sm5720_charger_oper_get_current_status();
			val->intval = status & (0x1 << SM5720_CHARGER_OP_EVENT_USB_OTG) ? 1 : 0;
			break;
		case POWER_SUPPLY_PROP_CHARGE_UNO_CONTROL:
			status = sm5720_charger_oper_get_current_status();
			val->intval = status & (0x1 << SM5720_CHARGER_OP_EVENT_5V_TX) ? 1 : 0;
			break;
		case POWER_SUPPLY_PROP_USB_HC:
			return -ENODATA;
		case POWER_SUPPLY_PROP_VOLTAGE_MAX:
			val->intval = sm5720_CHG_get_BATREG(charger);
			break;
		case POWER_SUPPLY_PROP_CURRENT_FULL:
        	val ->intval = sm5720_CHG_get_TOPOFF(charger);
			break;
		case POWER_SUPPLY_PROP_MAX ... POWER_SUPPLY_EXT_PROP_MAX:
			switch (ext_psp) {
			case POWER_SUPPLY_EXT_PROP_CHIP_ID:
				{
					u8 reg_data = 0;
					if (sm5720_read_reg(charger->i2c, SM5720_CHG_REG_DEVICEID, &reg_data) == 0){
						val->intval = ((reg_data == 0x1) ? 1 : 0);
						pr_info("%s : device id = 0x%x \n", __func__, reg_data);
					} else {
						val->intval = 0;
						pr_info("%s : IF PMIC I2C fail \n", __func__);
					}
				}
				break;
			case POWER_SUPPLY_EXT_PROP_WDT_STATUS:
				if(sm5720_CHG_get_WDTMR_STATUS(charger)) {
					dev_info(charger->dev, "charger WDT is expired!!\n");
					sm5720_chg_print_reg(charger);
					panic("charger WDT is expired!!");
				}
				break;
			default:
				return -EINVAL;
			}
			break;
		default:
			return -EINVAL;
	}

	return 0;
}

#if defined(CONFIG_AFC_CHARGER_MODE)
extern void sm5720_muic_charger_init(void);
#endif

static int sm5720_chg_set_property(struct power_supply *psy,
		enum power_supply_property psp,
		const union power_supply_propval *val)
{
	struct sm5720_charger_data *charger = power_supply_get_drvdata(psy);
	u8 reg;
	switch (psp) {
		case POWER_SUPPLY_PROP_STATUS:
			charger->status = val->intval;
			break;
		case POWER_SUPPLY_PROP_CHARGING_ENABLED:
			dev_info(charger->dev, "CHG:POWER_SUPPLY_PROP_CHARGING_ENABLED (val=%d)\n", val->intval);
			psy_chg_set_charging_enabled(charger, val->intval);
			sm5720_chg_print_reg(charger);
			break;
		case POWER_SUPPLY_PROP_ONLINE:
			psy_chg_set_cable_online(charger, val->intval);
			break;
		/* set input current */
		case POWER_SUPPLY_PROP_CURRENT_MAX:
			dev_info(charger->dev, "CHG:POWER_SUPPLY_PROP_CURRENT_MAX (val=%d)\n", val->intval);

			charger->input_current = val->intval;
			if (is_wireless_type(charger->cable_type))
				sm5720_set_wireless_input_current(charger, val->intval);
			else
				sm5720_set_input_current(charger, val->intval);

			if (charger->cable_type == SEC_BATTERY_CABLE_NONE)
				sm5720_set_wireless_input_current(charger, val->intval);
			break;
		/* set charge current */
		case POWER_SUPPLY_PROP_CURRENT_AVG:
			dev_info(charger->dev, "CHG:POWER_SUPPLY_PROP_CURRENT_AVG (val=%d)\n", val->intval);
			charger->charging_current = val->intval;
			sm5720_set_charge_current(charger, charger->charging_current);
			break;
		/* set charge current but not for wireless */
		case POWER_SUPPLY_PROP_CURRENT_NOW:
			dev_info(charger->dev, "CHG:POWER_SUPPLY_PROP_CURRENT_NOW (val=%d)\n", val->intval);
			charger->charging_current = val->intval;
			if (!is_wireless_type(charger->cable_type)) {
				charger->charging_current = val->intval;
				sm5720_set_charge_current(charger, charger->charging_current);
			}
			break;
		case POWER_SUPPLY_PROP_CONSTANT_CHARGE_CURRENT:
			break;
		/* set top-off current */
		case POWER_SUPPLY_PROP_CURRENT_FULL:
			dev_info(charger->dev, "CHG:POWER_SUPPLY_PROP_CURRENT_FULL (val=%d)\n", val->intval);
			sm5720_CHG_set_TOPOFF(charger, val->intval);
			break;
#if defined(CONFIG_BATTERY_SWELLING)
		case POWER_SUPPLY_PROP_VOLTAGE_MAX:
			dev_info(charger->dev, "CHG:POWER_SUPPLY_PROP_VOLTAGE_MAX (val=%d)\n", val->intval);

			charger->float_voltage = val->intval;
			sm5720_CHG_set_BATREG(charger, charger->float_voltage);
			break;
#endif
		case POWER_SUPPLY_PROP_CHARGE_OTG_CONTROL:
			dev_info(charger->dev, "CHG:POWER_SUPPLY_PROP_CHARGE_OTG_CONTROL - %s\n", val->intval > 0 ? "ON" : "OFF");

			if (val->intval) {
				sm5720_charger_oper_push_event(SM5720_CHARGER_OP_EVENT_USB_OTG, 1);
			} else {
				sm5720_charger_oper_push_event(SM5720_CHARGER_OP_EVENT_USB_OTG, 0);
			}
			break;
		case POWER_SUPPLY_PROP_CHARGE_UNO_CONTROL:
			dev_info(charger->dev, "CHG:POWER_SUPPLY_PROP_CHARGE_UNO_CONTROL - %s\n", val->intval > 0 ? "ON" : "OFF");
			sm5720_read_reg(charger->i2c, SM5720_CHG_REG_STATUS1, &reg);
			if (reg & (0x1 << 0)) { /* check: VBUS_POK */
				sm5720_charger_oper_push_event(SM5720_CHARGER_OP_EVENT_VBUSIN, 1);
			} else {
				sm5720_charger_oper_push_event(SM5720_CHARGER_OP_EVENT_VBUSIN, 0);
			}

			if (val->intval) {
				sm5720_charger_oper_push_event(SM5720_CHARGER_OP_EVENT_5V_TX, 1);
			} else {
				sm5720_charger_oper_push_event(SM5720_CHARGER_OP_EVENT_5V_TX, 0);
			}
			break;
		case POWER_SUPPLY_PROP_USB_HC:
			return -ENODATA;
		case POWER_SUPPLY_PROP_ENERGY_NOW:
			/* if jig attached, */
			break;
		case POWER_SUPPLY_PROP_CHARGE_CONTROL_LIMIT_MAX:
		{
			u8 reg;
			sm5720_charger_enable_aicl_irq(charger);
			sm5720_read_reg(charger->i2c, SM5720_CHG_REG_STATUS2, &reg);
			if (reg & (0x1 << 0))
				queue_delayed_work(charger->wqueue, &charger->aicl_work, msecs_to_jiffies(50));
		}
			break;
#if defined(CONFIG_AFC_CHARGER_MODE) && defined(CONFIG_MUIC_HV)
		case POWER_SUPPLY_PROP_AFC_CHARGER_MODE:
			sm5720_muic_charger_init();
			break;
#endif
		case POWER_SUPPLY_PROP_CHARGE_COUNTER_SHADOW:
			break;
		default:
			return -EINVAL;
	}

	return 0;
}

/**
 * Power Supply - OTG operation functions.
 */

static enum power_supply_property sm5720_otg_props[] = {
	POWER_SUPPLY_PROP_ONLINE,
};

static bool sm5720_charger_check_oper_otg_mode_on(void)
{
	unsigned char current_status = sm5720_charger_oper_get_current_status();
	bool ret;

	if (current_status & (1 << SM5720_CHARGER_OP_EVENT_USB_OTG)) {
		ret = true;
	} else {
		ret = false;
	}

	return ret;
}

static int sm5720_otg_get_property(struct power_supply *psy,
		enum power_supply_property psp,
		union power_supply_propval *val)
{
	switch (psp) {
		case POWER_SUPPLY_PROP_ONLINE:
			val->intval = sm5720_charger_check_oper_otg_mode_on();
			break;
		default:
			return -EINVAL;
	}

	return 0;
}

static int sm5720_otg_set_property(struct power_supply *psy,
		enum power_supply_property psp,
		const union power_supply_propval *val)
{
	struct sm5720_charger_data *charger = power_supply_get_drvdata(psy);
	union power_supply_propval value;
	int ret = 0;
	u8 reg;

	wake_lock(&charger->otg_wake_lock);

	switch (psp) {
	case POWER_SUPPLY_PROP_ONLINE:
		dev_info(charger->dev, "OTG:POWER_SUPPLY_PROP_ONLINE - %s\n", val->intval > 0 ? "ON" : "OFF");
		if (sm5720_charger_check_oper_otg_mode_on() == val->intval || lpcharge) {
			goto err_otg;
		}
		value.intval = val->intval;
		if (val->intval) {
			psy_do_property("wireless", set,
				POWER_SUPPLY_PROP_CHARGE_OTG_CONTROL, value);
			if (!is_hv_wireless_type(charger->cable_type)) {
				sm5720_read_reg(charger->i2c, SM5720_CHG_REG_STATUS1, &reg);
				pr_info("%s: CHECK VBUS 0x%x", __func__, reg);
				if (reg & (0x1 << 0)) /* check: VBUS_POK */
					sm5720_charger_oper_push_event(SM5720_CHARGER_OP_EVENT_VBUSIN, 1);
				else
					sm5720_charger_oper_push_event(SM5720_CHARGER_OP_EVENT_VBUSIN, 0);
			}
			sm5720_charger_oper_push_event(SM5720_CHARGER_OP_EVENT_USB_OTG, 1);
		} else {
			sm5720_charger_oper_push_event(SM5720_CHARGER_OP_EVENT_USB_OTG, 0);
			psy_do_property("wireless", set,
				POWER_SUPPLY_PROP_CHARGE_OTG_CONTROL, value);
		}
		power_supply_changed(charger->psy_otg);
		break;
	default:
		ret = -EINVAL;
		goto err_otg;
	}

err_otg:
	wake_unlock(&charger->otg_wake_lock);
	return ret;
}

/**
 * SM5720 Charger Platform driver operation functions.
 */

static struct device_attribute sm5720_charger_attrs[] = {
	SM5720_CHARGER_ATTR(chip_id),
	SM5720_CHARGER_ATTR(data),
};

static int sm5720_chg_create_attrs(struct device *dev)
{
	unsigned long i;
	int rc;

	for (i = 0; i < ARRAY_SIZE(sm5720_charger_attrs); i++) {
		rc = device_create_file(dev, &sm5720_charger_attrs[i]);
		if (rc)
			goto create_attrs_failed;
	}
	return rc;

create_attrs_failed:
	dev_err(dev, "%s: failed (%d)\n", __func__, rc);
	while (i--)
		device_remove_file(dev, &sm5720_charger_attrs[i]);
	return rc;
}

ssize_t sm5720_chg_show_attrs(struct device *dev,
		struct device_attribute *attr, char *buf)
{
	struct power_supply *psy = dev_get_drvdata(dev);
	struct sm5720_charger_data *charger = power_supply_get_drvdata(psy);
	const ptrdiff_t offset = attr - sm5720_charger_attrs;
	int i = 0;
	u8 addr, data;

	switch(offset) {
		case CHIP_ID:
			i += scnprintf(buf + i, PAGE_SIZE - i, "%s\n", "SM5720");
			break;
		case DATA:
			for (addr = SM5720_CHG_REG_STATUS1; addr <= SM5720_CHG_REG_BSTCNTL2; addr++) {
				sm5720_read_reg(charger->i2c, addr, &data);
				i += scnprintf(buf + i, PAGE_SIZE - i, "0x%02x : 0x%02x\n", addr, data);
			}
			break;
		default:
			return -EINVAL;
	}
	return i;
}

ssize_t sm5720_chg_store_attrs(struct device *dev,
		struct device_attribute *attr,
		const char *buf, size_t count)
{
	struct power_supply *psy = dev_get_drvdata(dev);
	struct sm5720_charger_data *charger = power_supply_get_drvdata(psy);
	const ptrdiff_t offset = attr - sm5720_charger_attrs;
	int ret = 0;
	int x,y;

	switch(offset) {
		case CHIP_ID:
			ret = count;
			break;
		case DATA:
			if (sscanf(buf, "0x%8x 0x%8x", &x, &y) == 2) {
				if (x >= SM5720_CHG_REG_STATUS1 && x <= SM5720_CHG_REG_BSTCNTL2) {
					u8 addr = x;
					u8 data = y;
					if (sm5720_write_reg(charger->i2c, addr, data) < 0)
					{
						dev_info(charger->dev,
								"%s: addr: 0x%x write fail\n", __func__, addr);
					}
				} else {
					dev_info(charger->dev,
							"%s: addr: 0x%x is wrong\n", __func__, x);
				}
			}
			ret = count;
			break;
		default:
			ret = -EINVAL;
	}
	return ret;
}

static irqreturn_t chg_vbus_in_isr(int irq, void *data)
{
	struct sm5720_charger_data *charger = data;

	dev_info(charger->dev, "%s - irq=%d\n", __func__, irq);

	return IRQ_HANDLED;
}

static irqreturn_t chg_wpcin_pok_isr(int irq, void *data)
{
	struct sm5720_charger_data *charger = data;

	dev_info(charger->dev, "%s - irq=%d\n", __func__, irq);

	return IRQ_HANDLED;
}

static irqreturn_t chg_aicl_isr(int irq, void *data)
{
	struct sm5720_charger_data *charger = data;

	dev_info(charger->dev, "%s - irq=%d\n", __func__, irq);
	wake_unlock(&charger->wpc_current_wake_lock);
	cancel_delayed_work(&charger->wc_current_work);

	wake_lock(&charger->aicl_wake_lock);
	queue_delayed_work(charger->wqueue, &charger->aicl_work, msecs_to_jiffies(50));

	return IRQ_HANDLED;
}

static void sm5720_charger_enable_aicl_irq(struct sm5720_charger_data *charger)
{
	int ret;

	pr_info("%s \n", __func__);
	ret = request_threaded_irq(charger->irq_aicl, NULL,
			chg_aicl_isr, 0, "aicl-irq", charger);
	if (ret < 0) {
		charger->irq_aicl_enabled = -1;
		pr_err("fail to request aicl IRQ: %d: %d\n", charger->irq_aicl, ret);
	} else {
		charger->irq_aicl_enabled = 1;
	}
}

static irqreturn_t chg_topoff_isr(int irq, void *data)
{
	struct sm5720_charger_data *charger = data;

	dev_info(charger->dev, "%s - irq=%d\n", __func__, irq);

	return IRQ_HANDLED;
}

static irqreturn_t chg_done_isr(int irq, void *data)
{
	struct sm5720_charger_data *charger = (struct sm5720_charger_data *)data;

	/* Toggle Q4FAT for Reset Topoff Timer condition */

	dev_info(charger->dev, "%s - irq=%d\n", __func__, irq);

	sm5720_CHG_set_ENQ4FET(charger, 0);
	msleep(10);
	sm5720_CHG_set_ENQ4FET(charger, 1);

	return IRQ_HANDLED;
}

static irqreturn_t sm5720_vsys_ovlo_isr(int irq, void *data)
{
	struct sm5720_charger_data *charger = (struct sm5720_charger_data *)data;
	union power_supply_propval value = {0, };

	/* Toggle Q4FAT for Reset Topoff Timer condition */

	dev_info(charger->dev, "%s - irq=%d\n", __func__, irq);

	wake_lock_timeout(&charger->sysovlo_wake_lock, HZ * 5);

	psy_do_property("battery", set,
		POWER_SUPPLY_EXT_PROP_SYSOVLO, value);

	return IRQ_HANDLED;
}

static irqreturn_t sm5720_otg_fail_isr(int irq, void *data)
{
	struct sm5720_charger_data *charger = (struct sm5720_charger_data *)data;
	union power_supply_propval value = {0, };
#ifdef CONFIG_USB_HOST_NOTIFY
	struct otg_notify *o_notify = get_otg_notify();
#endif

	/* Because of Auto reboosting by OTGFAIL irq, just disable boosting manually
	   without checking OTGFAIL status bit */
	dev_info(charger->dev, "%s - irq=%d\n", __func__, irq);

#ifdef CONFIG_USB_HOST_NOTIFY
	send_otg_notify(o_notify, NOTIFY_EVENT_OVERCURRENT, 0);
#endif

	value.intval = 0;
	psy_do_property("otg", set,
			POWER_SUPPLY_PROP_ONLINE, value);

	return IRQ_HANDLED;
}

static irqreturn_t sm5720_wdtmroff_isr(int irq, void *data)
{
	struct sm5720_charger_data *charger = (struct sm5720_charger_data *)data;

	dev_info(charger->dev, "%s - irq=%d\n", __func__, irq);

	if(sm5720_CHG_get_WDTMR_STATUS(charger)) {
		dev_info(charger->dev, "charger WDT is expired!!\n");
		sm5720_chg_print_reg(charger);
		panic("charger WDT is expired!!");
	}

	return IRQ_HANDLED;
}

static inline int _reduce_input_limit_current(struct sm5720_charger_data *charger)
{
	int input_limit = sm5720_get_input_current(charger);
	u8 offset = _calc_limit_current_offset_to_mA(input_limit - REDUCE_CURRENT_STEP);

	if (is_wireless_type(charger->cable_type))
		sm5720_update_reg(charger->i2c, SM5720_CHG_REG_WPCINCNTL, ((offset & 0x7F) << 0), (0x7F << 0));
	else
		sm5720_update_reg(charger->i2c, SM5720_CHG_REG_VBUSCNTL, ((offset & 0x7F) << 0), (0x7F << 0));

	charger->input_current = sm5720_get_input_current(charger);
	dev_info(charger->dev, "reduce input-limit: [%dmA] to [%dmA]\n",
			input_limit, charger->input_current);

	return charger->input_current;
}

static inline void _check_slow_rate_charging(struct sm5720_charger_data *charger)
{
	union power_supply_propval value;
	/* under 400mA considered as slow charging concept for VZW */
	if (charger->input_current <= SLOW_CHARGING_CURRENT_STANDARD &&
			charger->cable_type != SEC_BATTERY_CABLE_NONE) {

		dev_info(charger->dev, "slow-rate charging on : input current(%dmA), cable-type(%d)\n",
				charger->input_current, charger->cable_type);

		charger->slow_late_chg_mode = 1;
		value.intval = POWER_SUPPLY_CHARGE_TYPE_SLOW;
		psy_do_property("battery", set, POWER_SUPPLY_PROP_CHARGE_TYPE, value);
	}
}

static void aicl_work(struct work_struct *work)
{
	struct sm5720_charger_data *charger =
		container_of(work, struct sm5720_charger_data, aicl_work.work);

	int aicl_cnt = 0, input_limit;
	int aicl_on = false;
	u8 reg;

	dev_info(charger->dev, "%s - start\n", __func__);

	mutex_lock(&charger->charger_mutex);
	sm5720_read_reg(charger->i2c, SM5720_CHG_REG_STATUS2, &reg);
	while ((reg & (0x1 << 0)) && charger->cable_type != SEC_BATTERY_CABLE_NONE) {
		if (++aicl_cnt >= 2) {
			input_limit = _reduce_input_limit_current(charger);
			aicl_on = true;
			if (input_limit <= MINIMUM_INPUT_CURRENT)
				break;
			aicl_cnt = 0;
		}
		mdelay(50);
		sm5720_read_reg(charger->i2c, SM5720_CHG_REG_STATUS2, &reg);
	}

	if (aicl_on) {
		union power_supply_propval value;

		value.intval = input_limit;
		psy_do_property("battery", set,
		POWER_SUPPLY_EXT_PROP_AICL_CURRENT, value);

		if (!is_wireless_type(charger->cable_type))
			_check_slow_rate_charging(charger);
	}

	mutex_unlock(&charger->charger_mutex);
	wake_unlock(&charger->aicl_wake_lock);

	dev_info(charger->dev, "%s - done\n", __func__);
}

static void wc_current_work(struct work_struct *work)
{
	struct sm5720_charger_data *charger =
			container_of(work, struct sm5720_charger_data, wc_current_work.work);

	int diff_current = 0;

	if (is_not_wireless_type(charger->cable_type)) {
		charger->wc_pre_current = WC_CURRENT_START;
		/* set input defalut current 400mA */
		sm5720_update_reg(charger->i2c, SM5720_CHG_REG_WPCINCNTL, ((0x0C & 0x7F) << 0), (0x7F << 0));
		wake_unlock(&charger->wpc_current_wake_lock);
		return;
	}

	if (charger->wc_pre_current == charger->wc_current) {
		union power_supply_propval value;
		sm5720_set_charge_current(charger, charger->charging_current);
		/* Wcurr-B) Restore Vrect adj room to previous value
			after finishing wireless input current setting. Refer to Wcurr-A) step */
		msleep(500);
		if (is_nv_wireless_type(charger->cable_type)) {
			psy_do_property("battery", get, POWER_SUPPLY_PROP_CAPACITY, value);
			if (value.intval < charger->wireless_cc_cv)
				value.intval = WIRELESS_VRECT_ADJ_ROOM_4; /* WPC 4.5W, Vrect Room 30mV */
			else
				value.intval = WIRELESS_VRECT_ADJ_ROOM_5; /* WPC 4.5W, Vrect Room 80mV */
		} else if (is_hv_wireless_type(charger->cable_type)) {
			value.intval = WIRELESS_VRECT_ADJ_ROOM_5; /* WPC 9W, Vrect Room 80mV */
		} else
			value.intval = WIRELESS_VRECT_ADJ_OFF; /* PMA 4.5W, Vrect Room 0mV */
		psy_do_property(charger->wireless_charger_name, set,
			POWER_SUPPLY_PROP_INPUT_VOLTAGE_REGULATION, value);
		wake_unlock(&charger->wpc_current_wake_lock);
	} else {
		if (charger->wc_pre_current > charger->wc_current) {
			diff_current = charger->wc_pre_current - charger->wc_current;
			if (diff_current < WC_CURRENT_STEP)
				charger->wc_pre_current -= diff_current;
			else
				charger->wc_pre_current -= WC_CURRENT_STEP;
		} else {
			diff_current = charger->wc_current - charger->wc_pre_current;
			if (diff_current < WC_CURRENT_STEP)
				charger->wc_pre_current += diff_current;
			else
				charger->wc_pre_current += WC_CURRENT_STEP;
		}
		sm5720_set_input_current(charger, charger->wc_pre_current);
		queue_delayed_work(charger->wqueue, &charger->wc_current_work,
			msecs_to_jiffies(WC_CURRENT_WORK_STEP));
	}
	pr_info("%s: wc_current(%d), wc_pre_current(%d), diff(%d)\n",
		__func__, charger->wc_current, charger->wc_pre_current, diff_current);
}

#ifdef CONFIG_OF
static int sm5720_charger_parse_dt(struct sm5720_charger_data *charger)
{
	struct device_node *np;
	int ret = 0;

	np = of_find_node_by_name(NULL, "battery");
	if (!np) {
		pr_err("%s np NULL\n", __func__);
	} else {
		charger->enable_sysovlo_irq = of_property_read_bool(np, "battery,enable_sysovlo_irq");

		ret = of_property_read_u32(np, "battery,chg_float_voltage", &charger->float_voltage);
		if (ret) {
			pr_info("%s: battery,chg_float_voltage is Empty\n", __func__);
			charger->float_voltage = 4200;
		}
		pr_info("%s: battery,chg_float_voltage is %d\n", __func__, charger->float_voltage);

		ret = of_property_read_u32(np, "battery,wireless_cc_cv", &charger->wireless_cc_cv);
		if (ret)
			pr_info("%s : wireless_cc_cv is Empty\n", __func__);

		ret = of_property_read_string(np,
			"battery,wireless_charger_name", (char const **)&charger->wireless_charger_name);
		if (ret)
			pr_info("%s: Wireless charger name is Empty\n", __func__);
	}
	return ret;
}
#endif

static void sm5720_charger_initialize(struct sm5720_charger_data *charger)
{
	dev_info(charger->dev, "charger initial hardware condition process start. (float_voltage=%d)\n",
			charger->float_voltage);

	sm5720_CHG_set_SS_LIM_VBUS(charger, SS_LIM_VBUS_40mA_per_ms);
	sm5720_CHG_set_SS_FAST_VBUS(charger, SS_FAST_VBUS_10mA_per_ms);
	sm5720_CHG_set_AUTOSET(charger, 0);
	sm5720_CHG_set_AICLTH(charger, AICL_THRES_VOLTAGE_4_5V);
	sm5720_CHG_set_AICLEN_VBUS(charger, 1);
	sm5720_CHG_set_AICLEN_WPC(charger, 1);
	sm5720_CHG_set_DISCHARGE(charger, DISCHARGE_CUR_LIMIT_4_5A);
	sm5720_CHG_set_BATREG(charger, charger->float_voltage);

	sm5720_CHG_set_BST_IQ3LIMIT(charger, BST_IQ3LIMIT_4000mA);
	sm5720_CHG_set_OTGCURRENT(charger, OTGCURRENT_1500mA);
	sm5720_CHG_set_VSYS_MIN(charger, VSYS_MIN_REG_V_3600mV);

	/* Topoff-Timer configuration for Battery Swelling protection */
	sm5720_CHG_set_TOPOFFTIMER(charger, TOPOFF_TIMER_30m);
	sm5720_CHG_set_AUTOSTOP(charger, 1);

#if defined(SM5720_SBPS_ACTIVATE)
	/* SBPS configuration */
	sm5720_CHG_set_THEM_H(charger, SBPS_THEM_HOT_60C);
	sm5720_CHG_set_THEM_C(charger, SBPS_THEM_COLD_0C);
	sm5720_CHG_set_I_SBPS(charger, SBPS_DISCHARGE_I_50mA);
	sm5720_CHG_set_VBAT_SBPS(charger, SBPS_EN_THRES_V_4_2V);
	sm5720_CHG_set_ENSBPS(charger, 1);
#endif

	dev_info(charger->dev, "charger initial hardware condition process done.\n");
}

static inline int _init_sm5720_charger_drv_info(struct sm5720_charger_data *charger)
{
	struct sm5720_platform_data *pdata = charger->sm5720_pdata;

	if (pdata == NULL) {
		dev_err(charger->dev, "%s: can't get sm5720_platform_data info\n", __func__);
		return -EINVAL;
	}

	mutex_init(&charger->charger_mutex);

	charger->wqueue = create_singlethread_workqueue(dev_name(charger->dev));
	if (!charger->wqueue) {
		dev_err(charger->dev, "%s: fail to create workqueue\n", __func__);
		return -ENOMEM;
	}

	charger->slow_late_chg_mode = false;

	/* setup Work-queue control handlers */
	INIT_DELAYED_WORK(&charger->aicl_work, aicl_work);
	INIT_DELAYED_WORK(&charger->wc_current_work, wc_current_work);

	wake_lock_init(&charger->aicl_wake_lock, WAKE_LOCK_SUSPEND, "charger-aicl");
	wake_lock_init(&charger->wpc_wake_lock, WAKE_LOCK_SUSPEND, "charger-wpc");
	wake_lock_init(&charger->wpc_current_wake_lock, WAKE_LOCK_SUSPEND, "charger-wpc-current");
	wake_lock_init(&charger->sysovlo_wake_lock, WAKE_LOCK_SUSPEND, "sysovlo");
	wake_lock_init(&charger->otg_wake_lock, WAKE_LOCK_SUSPEND, "otg-path");

	/* Get IRQ-service numbers */
	charger->irq_wpcin_pok = pdata->irq_base + SM5720_CHG_IRQ_INT1_WPCINPOK;
	charger->irq_vbus_pok = pdata->irq_base + SM5720_CHG_IRQ_INT1_VBUSPOK;
	charger->irq_aicl = pdata->irq_base + SM5720_CHG_IRQ_INT2_AICL;
	charger->irq_topoff = pdata->irq_base + SM5720_CHG_IRQ_INT2_TOPOFF;
	charger->irq_done = pdata->irq_base + SM5720_CHG_IRQ_INT2_DONE;
	charger->irq_sysovlo = pdata->irq_base + SM5720_CHG_IRQ_INT3_VSYSOVP;
	charger->irq_otgfail = pdata->irq_base + SM5720_CHG_IRQ_INT3_OTGFAIL;
	charger->irq_wdtmroff = pdata->irq_base + SM5720_CHG_IRQ_INT2_WDTMROFF;

	return 0;
}

static void sm5720_check_reset_ic(struct i2c_client *i2c, void *data)
{

}

static const struct power_supply_desc sm5720_charger_power_supply_desc = {
	.name = "sm5720-charger",
	.type = POWER_SUPPLY_TYPE_UNKNOWN,
	.properties = sm5720_charger_props,
	.num_properties = ARRAY_SIZE(sm5720_charger_props),
	.get_property = sm5720_chg_get_property,
	.set_property = sm5720_chg_set_property,
};

static const struct power_supply_desc otg_power_supply_desc = {
	.name = "otg",
	.type = POWER_SUPPLY_TYPE_OTG,
	.properties = sm5720_otg_props,
	.num_properties = ARRAY_SIZE(sm5720_otg_props),
	.get_property = sm5720_otg_get_property,
	.set_property = sm5720_otg_set_property,
};

static int sm5720_charger_probe(struct platform_device *pdev)
{
	struct sm5720_dev *sm5720 = dev_get_drvdata(pdev->dev.parent);
	struct sm5720_platform_data *pdata = dev_get_platdata(sm5720->dev);
	struct sm5720_charger_data *charger;
	struct power_supply_config charger_cfg = {};
	int ret = 0;

	dev_info(&pdev->dev, "%s: SM5720 Charger Driver Probe Start\n", __func__);

	charger = kzalloc(sizeof(*charger), GFP_KERNEL);
	if (!charger)
		return -ENOMEM;

	charger->dev = &pdev->dev;
	charger->i2c = sm5720->charger;
	charger->sm5720_pdata = pdata;
	charger->wc_pre_current = WC_CURRENT_START;

#if defined(CONFIG_OF)
	ret = sm5720_charger_parse_dt(charger);
	if (ret < 0) {
		dev_err(&pdev->dev, "%s not found charger dt! ret[%d]\n", __func__, ret);
	}
#endif
	platform_set_drvdata(pdev, charger);

	sm5720->chg_data = (void *)charger;
	sm5720->check_chg_reset = sm5720_check_reset_ic;

	charger->pmic_ver = sm5720_get_device_ID();
	pr_info("%s : pmic rev 0x%x\n", __func__, charger->pmic_ver);
	charger->irq_aicl_enabled = -1;

	sm5720_charger_initialize(charger);
#if defined(SM5720_WATCHDOG_RESET_ACTIVATE)
	/* Watchdog-Timer configuration for Battery Swelling protection
		Watchdog-Timer is able to work from PASS5 */
	if(charger->pmic_ver >= SM5720_PASS5) {
		sm5720_CHG_set_ENWATCHDOG(charger, false);
	}
#endif
	sm5720_chg_print_reg(charger);
	sm5720_charger_oper_table_init(charger->i2c);

	ret = _init_sm5720_charger_drv_info(charger);
	if (ret) {
		goto err_pdata_free;
	}

	charger_cfg.drv_data = charger;

	charger->psy_chg = power_supply_register(&pdev->dev, &sm5720_charger_power_supply_desc, &charger_cfg);
	if (!charger->psy_chg) {
		dev_err(&pdev->dev, "%s: fail to Register psy_chg\n", __func__);
		goto err_power_supply_register_chg;
	}

	charger->psy_otg = power_supply_register(&pdev->dev, &otg_power_supply_desc, &charger_cfg);
	if (!charger->psy_otg) {
		dev_err(&pdev->dev, "%s: fail to Register psy_otg\n", __func__);
		goto err_power_supply_register_otg;
	}

	ret = sm5720_chg_create_attrs(&charger->psy_chg->dev);
	if (ret) {
		dev_err(&pdev->dev, "%s : fail to create_attrs\n", __func__);
		goto err_irq;
	}

	/* Request IRQ */
	ret = request_threaded_irq(charger->irq_vbus_pok, NULL,
			chg_vbus_in_isr, 0, "chgin-irq", charger);
	if (ret < 0) {
		pr_err("fail to request chgin IRQ: %d: %d\n", charger->irq_vbus_pok, ret);
		goto err_irq;
	}
	ret = request_threaded_irq(charger->irq_wpcin_pok, NULL,
			chg_wpcin_pok_isr, 0, "wpc-int", charger);
	if (ret) {
		pr_err("fail to request wpcin IRQ: %d: %d\n", charger->irq_wpcin_pok, ret);
		goto err_irq;
	}
	ret = request_threaded_irq(charger->irq_topoff, NULL,
			chg_topoff_isr, 0, "topoff-irq", charger);
	if (ret < 0) {
		pr_err("fail to request topoff IRQ: %d: %d\n", charger->irq_topoff, ret);
		goto err_power_supply_register_otg;
	}

	ret = request_threaded_irq(charger->irq_done, NULL,
			chg_done_isr, 0, "done-irq", charger);
	if (ret < 0) {
		pr_err("fail to request chgin IRQ: %d: %d\n", charger->irq_done, ret);
		goto err_power_supply_register_otg;
	}

	ret = request_threaded_irq(charger->irq_sysovlo, NULL,
			sm5720_vsys_ovlo_isr, 0, "sysovlo-irq", charger);
	if (ret < 0) {
		pr_err("fail to request sysovlo IRQ: %d: %d\n", charger->irq_sysovlo, ret);
		goto err_irq;
	}

	ret = request_threaded_irq(charger->irq_otgfail, NULL,
			sm5720_otg_fail_isr, 0, "otgfail-irq", charger);
	if (ret < 0) {
		pr_err("fail to request sysovlo IRQ: %d: %d\n", charger->irq_otgfail, ret);
		goto err_irq;
	}

	ret = request_threaded_irq(charger->irq_wdtmroff, NULL,
			sm5720_wdtmroff_isr, 0, "wdtmroff-irq", charger);
	if (ret < 0) {
		pr_err("fail to request wdtmroff IRQ: %d: %d\n", charger->irq_wdtmroff, ret);
		goto err_irq;
	}

	dev_info(&pdev->dev, "SM5720 Charger Driver Loaded\n");

	return 0;

err_irq:
	power_supply_unregister(charger->psy_otg);
err_power_supply_register_otg:
	power_supply_unregister(charger->psy_chg);
err_power_supply_register_chg:
	destroy_workqueue(charger->wqueue);
err_pdata_free:
	mutex_destroy(&charger->charger_mutex);
	kfree(charger);

	return ret;
}

static int sm5720_charger_remove(struct platform_device *pdev)
{
	struct sm5720_charger_data *charger = platform_get_drvdata(pdev);

	power_supply_unregister(charger->psy_otg);
	power_supply_unregister(charger->psy_chg);

	if (charger->enable_sysovlo_irq) {
		free_irq(charger->irq_sysovlo, NULL);
	}

	destroy_workqueue(charger->wqueue);

	mutex_destroy(&charger->charger_mutex);

	kfree(charger);

	return 0;
}

#if defined CONFIG_PM
static int sm5720_charger_suspend(struct device *dev)
{
	return 0;
}

static int sm5720_charger_resume(struct device *dev)
{
	return 0;
}
#else
#define sm5720_charger_suspend NULL
#define sm5720_charger_resume NULL
#endif

static void sm5720_charger_shutdown(struct platform_device *pdev)
{
	struct sm5720_charger_data *charger = platform_get_drvdata(pdev);

	pr_info("%s: SM5720 Charger driver shutdown\n", __func__);
	if (!charger->i2c) {
		pr_err("%s: no sm5720 i2c client\n", __func__);
		return;
	}

	/* set default input current */
	sm5720_update_reg(charger->i2c, SM5720_CHG_REG_VBUSCNTL, ((0x10 & 0x7F) << 0), (0x7F << 0));
	sm5720_update_reg(charger->i2c, SM5720_CHG_REG_WPCINCNTL, ((0x0C & 0x7F) << 0), (0x7F << 0));

#if !defined(CONFIG_SEC_FACTORY)
	/* set default value */
	sm5720_charger_oper_table_init(charger->i2c);
#endif

	/* Watchdog-Timer configuration for Battery Swelling protection
		Watchdog-Timer is able to work from PASS5 */
#if defined(SM5720_WATCHDOG_RESET_ACTIVATE)
	if(charger->pmic_ver >= SM5720_PASS5) {
		sm5720_CHG_set_ENWATCHDOG(charger, false);
	}
#endif
}

static SIMPLE_DEV_PM_OPS(sm5720_charger_pm_ops, sm5720_charger_suspend,
		sm5720_charger_resume);

static struct platform_driver sm5720_charger_driver = {
	.driver = {
		.name = "sm5720-charger",
		.owner = THIS_MODULE,
#ifdef CONFIG_PM
		.pm = &sm5720_charger_pm_ops,
#endif
	},
	.probe = sm5720_charger_probe,
	.remove = sm5720_charger_remove,
	.shutdown = sm5720_charger_shutdown,
};

static int __init sm5720_charger_init(void)
{
	pr_info("%s : \n", __func__);
	return platform_driver_register(&sm5720_charger_driver);
}

static void __exit sm5720_charger_exit(void)
{
	platform_driver_unregister(&sm5720_charger_driver);
}

module_init(sm5720_charger_init);
module_exit(sm5720_charger_exit);

MODULE_DESCRIPTION("Samsung SM5720 Charger Driver");
MODULE_AUTHOR("Samsung Electronics");
MODULE_LICENSE("GPL");
