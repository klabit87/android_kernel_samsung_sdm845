/*
 *  smb1351_charger.c
 *  Samsung smb1351 Charger Driver
 *
 *  Copyright (C) 2015 Samsung Electronics
 *
 * This software is licensed under the terms of the GNU General Public
 * License version 2, as published by the Free Software Foundation, and
 * may be copied, distributed, and modified under those terms.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 */

#include <linux/of.h>
#include <linux/of_gpio.h>
#include "include/charger/smb1351_charger.h"

#define DEBUG

#define ENABLE 1
#define DISABLE 0

static struct smb1351_charger_data *g_charger;

static struct device_attribute smb1351_charger_attrs[] = {
	SMB1351_CHARGER_ATTR(mode),
	SMB1351_CHARGER_ATTR(data),
};

static enum power_supply_property smb1351_charger_props[] = {
};

static int smb1351_read_reg(struct i2c_client *client, u8 reg, u8 *dest)
{
	struct smb1351_charger_data *charger = i2c_get_clientdata(client);
	int ret = 0;

	mutex_lock(&charger->io_lock);
	ret = i2c_smbus_read_byte_data(client, reg);
	mutex_unlock(&charger->io_lock);

	if (ret < 0) {
		pr_err("%s: can't read reg(0x%x), ret(%d)\n", __func__, reg, ret);
		return ret;
	}

	reg &= 0xFF;
	*dest = ret;

	return 0;
}

#if 0
static int smb1351_write_reg(struct i2c_client *client, u8 reg, u8 data)
{
	struct smb1351_charger_data *charger = i2c_get_clientdata(client);
	int ret = 0;

	mutex_lock(&charger->io_lock);
	ret = i2c_smbus_write_byte_data(client, reg, data);
	mutex_unlock(&charger->io_lock);

	if (ret < 0)
		pr_err("%s: can't write reg(0x%x), ret(%d)\n", __func__, reg, ret);

	return ret;
}
#endif

static int smb1351_update_reg(struct i2c_client *client, u8 reg, u8 val, u8 mask)
{
	struct smb1351_charger_data *charger = i2c_get_clientdata(client);
	int ret = 0;

	mutex_lock(&charger->io_lock);
	ret = i2c_smbus_read_byte_data(client, reg);

	if (ret < 0)
		pr_err("%s: can't update reg(0x%x), ret(%d)\n", __func__, reg, ret);
	else {
		u8 old_val = ret & 0xFF;
		u8 new_val = (val & mask) | (old_val & (~mask));
		ret = i2c_smbus_write_byte_data(client, reg, new_val);
	}
	mutex_unlock(&charger->io_lock);

	return ret;
}

static void smb1351_charger_test_read(struct smb1351_charger_data *charger)
{
	u8 data = 0;
	u32 addr = 0;
	char str[1024]={0,};
	for (addr = 0x00; addr <= 0x15; addr++) {
		smb1351_read_reg(charger->i2c, addr, &data);
		sprintf(str + strlen(str), "[0x%02x]0x%02x, ", addr, data);
	}
	smb1351_read_reg(charger->i2c, 0x1C, &data);
	sprintf(str + strlen(str), "[0x%02x]0x%02x, ", 0x1C, data);

	for (addr = 0x25; addr <= 0x47; addr++) {
		smb1351_read_reg(charger->i2c, addr, &data);
		sprintf(str + strlen(str), "[0x%02x]0x%02x, ", addr, data);
	}
	pr_info("SMB1351 : %s\n", str);
}

static int smb1351_get_charger_state(struct smb1351_charger_data *charger)
{
	int rc;
	u8 reg = 0;

	rc = smb1351_read_reg(charger->i2c, SMB1351_STATUS4_REG, &reg);
	if (rc) {
		pr_err("Couldn't read STATUS4 reg = %d\n", rc);
		return POWER_SUPPLY_STATUS_UNKNOWN;
	}

	pr_debug("STATUS4_REG(0x3A)=%x\n", reg);
	if (reg & STATUS_HOLD_OFF_BIT)
		return POWER_SUPPLY_STATUS_NOT_CHARGING;

	if (reg & STATUS_CHG_MASK)
		return POWER_SUPPLY_STATUS_CHARGING;

	if (reg & STATUS_DONE_BIT)
		return POWER_SUPPLY_STATUS_FULL;

	return POWER_SUPPLY_STATUS_DISCHARGING;
}

static int smb1351_get_charger_health(struct smb1351_charger_data *charger)
{
	u8 reg = 0;
	smb1351_read_reg(charger->i2c, SMB1351_STATUS7_REG, &reg);
	pr_info("%s: HVDCP Reg : 0x%x\n", __func__, reg);

	return POWER_SUPPLY_HEALTH_GOOD;
}

static int smb1351_check_input_current(struct smb1351_charger_data *charger, int input_current)
{
	int i;

	for (i = ARRAY_SIZE(ac_input_current) - 1; i >= 0; i--) {
		if (ac_input_current[i] <= input_current)
			return ac_input_current[i];
	}

	/* return minimun ac input current */
	return ac_input_current[0];
}

static int smb1351_set_input_current(struct smb1351_charger_data *charger, int current_ma)
{
	int i, rc;

	/* HC mode  - if none of the above */
	for (i = ARRAY_SIZE(ac_input_current) - 1; i >= 0; i--) {
		if (ac_input_current[i] <= current_ma)
			break;
	}
	if (i < 0)
		i = 0;
	rc = smb1351_update_reg(charger->i2c, SMB1351_CHG_CURRENT_CTRL_REG,
			i, AC_INPUT_CURRENT_LIMIT_MASK);
	if (rc) {
		pr_err("Couldn't set input mA rc=%d\n", rc);
		return rc;
	}

	return 0;
}

static int smb1351_get_charge_current(struct smb1351_charger_data *charger)
{
	u8 reg_data;
	int charge_current;

	smb1351_read_reg(charger->i2c, SMB1351_CHG_CURRENT_CTRL_REG, &reg_data);
	reg_data = reg_data >> SMB1351_CHG_FAST_SHIFT;
	charge_current = fast_chg_current[reg_data];

	return charge_current;
}

static void smb1351_set_charge_current(struct smb1351_charger_data *charger,
	int charge_current)
{
	int i, rc;

	/* set fastchg current */
	for (i = ARRAY_SIZE(fast_chg_current) - 1; i >= 0; i--) {
		if (fast_chg_current[i] <= charge_current)
			break;
	}
	if (i < 0)
		i = 0;
	i = i << SMB1351_CHG_FAST_SHIFT;

	/* make sure pre chg mode is disabled */
	rc = smb1351_update_reg(charger->i2c, SMB1351_VARIOUS_FUNC2_REG,
			PRECHG_TO_FASTCHG_BIT, 0);
	if (rc)
		pr_err("Couldn't write VARIOUS_FUNC_2_REG rc=%d\n", rc);

	smb1351_update_reg(charger->i2c, SMB1351_CHG_CURRENT_CTRL_REG,
			i, FAST_CHG_CURRENT_MASK);

	pr_info("%s: charge_current(%d)\n", __func__, charge_current);
}

static void smb1351_set_charger_state(struct smb1351_charger_data *charger,
	int enable)
{
	pr_info("%s: SMB1351 CHARGE : %s\n", enable > 0 ? "ENABLE" : "DISABLE", __func__);

	if (enable)
		smb1351_update_reg(charger->i2c, SMB1351_CMD_CHG_REG,
			CMD_CHG_ENABLE, CMD_CHG_EN_BIT);
	else
		smb1351_update_reg(charger->i2c, SMB1351_CMD_CHG_REG,
			CMD_CHG_DISABLE, CMD_CHG_EN_BIT);
}

static void smb1351_set_topoff_current(struct smb1351_charger_data *charger,
	int topoff_current)
{
	int i, rc;

	topoff_current = (topoff_current < 200) ? 200 :
		((topoff_current > 700) ? 700 : topoff_current);

	i = (topoff_current - 200) / 100;
	rc = smb1351_update_reg(charger->i2c, SMB1351_OTHER_CHARGE_CURRENTS_REG,
			(i << TERMINATION_CURRENT_SHIFT), TERMINATION_CURRENT_MASK);
	if (rc)
		pr_err("Couldn't write SMB1351_OTHER_CHARGE_CURRENTS_REG rc=%d\n", rc);

	charger->topoff_current = topoff_current;
	pr_info("%s: topoff_current(%d)\n", __func__, topoff_current);
}

static void smb1351_set_charger_volatile(struct smb1351_charger_data *charger,
	int enable)
{
	pr_info("%s: SMB1351 CHARGE : %s\n", enable > 0 ? "ENABLE" : "DISABLE", __func__);

	if (enable)
		smb1351_update_reg(charger->i2c, SMB1351_CMD_I2C_REG,
			CMD_BQ_CFG_ACCESS_BIT, CMD_BQ_CFG_ACCESS_BIT);
	else
		smb1351_update_reg(charger->i2c, SMB1351_CMD_CHG_REG,
			0x00, CMD_BQ_CFG_ACCESS_BIT);
}

static void smb1351_set_float_voltage(struct smb1351_charger_data *charger, int float_voltage)
{
	int i, rc;

	if (float_voltage / 10000)
		float_voltage = float_voltage / 10;

	i = (float_voltage < 3500) ? 3500 :
		((float_voltage > 4500) ? 4500 : float_voltage);

	i = (i - 3500) / 20;
	rc = smb1351_update_reg(charger->i2c, SMB1351_FLOAT_VOLTAGE_REG,
			i, FLOAT_VOLTAGE_MASK);

	if (rc)
		pr_err("Couldn't write FLOAT_VOLTAGE_REG rc=%d\n", rc);

	charger->float_voltage = float_voltage;
	pr_info("%s: update float voltage(%d)\n", __func__, float_voltage);
}

static void smb1351_check_charging_configure(struct smb1351_charger_data *charger)
{
	u8 reg_data;
	int i;

	/* 1. float voltage */
	i = (charger->float_voltage - 3500) / 20;
	smb1351_read_reg(charger->i2c, SMB1351_CHG_CURRENT_CTRL_REG, &reg_data);
	reg_data = reg_data & FLOAT_VOLTAGE_MASK;
	if (i != reg_data)
		smb1351_set_float_voltage(charger, charger->float_voltage);

	/* 2. topoff current */
	i = (charger->topoff_current - 200) / 100;
	smb1351_read_reg(charger->i2c, SMB1351_OTHER_CHARGE_CURRENTS_REG, &reg_data);
	reg_data = (reg_data & TERMINATION_CURRENT_MASK) >> TERMINATION_CURRENT_SHIFT;
	if (i != reg_data)
		smb1351_set_topoff_current(charger, charger->topoff_current);
}

static void smb1351_mode_change(struct smb1351_charger_data *charger, int mode)
{
	union power_supply_propval value;
	if (mode == MODE_QC20) {
		pr_info("%s: Mode change to QC2.0\n", __func__);
		/* 1. select HVDCP adapter select as 5V. */
		smb1351_update_reg(charger->i2c, SMB1351_HVDCP_BATTMISSING_CTRL_REG,
				0x0, HVDCP_ADAPTER_SEL_MASK);

		msleep(1000);
		/* 2. Force HVDCP 2.0 mode by commend bit */
		smb1351_update_reg(charger->i2c, SMB1351_CMD_HVDCP_REG,
				0x20, CMD_FORCE_HVDCP_2P0_BIT);
		msleep(1000);
		/* 3. select HVDCP adapter select as 9V. */
		smb1351_update_reg(charger->i2c, SMB1351_HVDCP_BATTMISSING_CTRL_REG,
				0x40, HVDCP_ADAPTER_SEL_MASK);

		value.intval = SEC_BATTERY_CABLE_QC20;
		psy_do_property("battery", set,
				POWER_SUPPLY_PROP_ONLINE, value);
		charger->charge_mode = mode;
	} else if (mode == MODE_5V) {
		pr_info("%s: Mode change to 5V Normal\n", __func__);
		/* 1. select HVDCP adapter select as 5V. */
		smb1351_update_reg(charger->i2c, SMB1351_HVDCP_BATTMISSING_CTRL_REG,
				0x0, HVDCP_ADAPTER_SEL_MASK);

		value.intval = SEC_BATTERY_CABLE_TA;
		psy_do_property("battery", set,
				POWER_SUPPLY_PROP_ONLINE, value);
		charger->charge_mode = mode;
	}

	smb1351_charger_test_read(charger);
}

void muic_afc_set_voltage(int vol)
{
	union power_supply_propval value;

	if (vol == SEC_INPUT_VOLTAGE_9V) {
		pr_info("%s: Change to 9V(QC2.0)\n", __func__);
		/* 1. select HVDCP adapter select as 5V. */
		smb1351_update_reg(g_charger->i2c, SMB1351_HVDCP_BATTMISSING_CTRL_REG,
				0x0, HVDCP_ADAPTER_SEL_MASK);

		msleep(1000);
		/* 2. Force HVDCP 2.0 mode by commend bit */
		smb1351_update_reg(g_charger->i2c, SMB1351_CMD_HVDCP_REG,
				0x20, CMD_FORCE_HVDCP_2P0_BIT);
		msleep(1000);
		/* 3. select HVDCP adapter select as 9V. */
		smb1351_update_reg(g_charger->i2c, SMB1351_HVDCP_BATTMISSING_CTRL_REG,
				0x40, HVDCP_ADAPTER_SEL_MASK);

		value.intval = SEC_BATTERY_CABLE_QC20;
		psy_do_property("battery", set,
				POWER_SUPPLY_PROP_ONLINE, value);
	} else if (vol == SEC_INPUT_VOLTAGE_5V) {
		pr_info("%s: Change to 5V(Normal)\n", __func__);
		/* 1. select HVDCP adapter select as 5V. */
		{
			/* u8 reg_data; */
                        smb1351_update_reg(g_charger->i2c, SMB1351_HVDCP_BATTMISSING_CTRL_REG,
                                        0x0, HVDCP_ADAPTER_SEL_MASK);
                        msleep(1000);

                        /* 2. Force HVDCP 2.0 mode by commend bit */
                        smb1351_update_reg(g_charger->i2c, SMB1351_CMD_HVDCP_REG,
                                        0x20, CMD_FORCE_HVDCP_2P0_BIT);
			msleep(1000);

			smb1351_update_reg(g_charger->i2c, SMB1351_HVDCP_BATTMISSING_CTRL_REG,
					0x0, HVDCP_ADAPTER_SEL_MASK);
		}
		msleep(1000);

		value.intval = SEC_BATTERY_CABLE_HV_TA_CHG_LIMIT;
		psy_do_property("battery", set,
				POWER_SUPPLY_PROP_ONLINE, value);
	}
}

static void smb1351_charger_initialize(struct smb1351_charger_data *charger)
{
	int rc;
	u8 data;

	pr_info("%s: \n", __func__);

	/* For updateing configuration register */
	smb1351_update_reg(charger->i2c, SMB1351_CMD_I2C_REG,
		CMD_BQ_CFG_ACCESS_BIT, CMD_BQ_CFG_ACCESS_BIT);

	/* For controlling suspend mode */
	smb1351_update_reg(charger->i2c, SMB1351_VARIOUS_FUNC_REG,
		SUSPEND_MODE_CTRL_BY_I2C, SUSPEND_MODE_CTRL_BIT);

	/* For changing charging enable */
	smb1351_update_reg(charger->i2c, SMB1351_CHG_PIN_EN_CTRL_REG,
		EN_BY_I2C_0_DISABLE, EN_PIN_CTRL_MASK);

	/* Enable APSD & AICL*/
	smb1351_update_reg(charger->i2c, SMB1351_VARIOUS_FUNC_REG,
		(AICL_EN_BIT | APSD_EN_BIT), (APSD_EN_BIT | AICL_EN_BIT));

	/* Set Input Current Mode (Normal : manual, QC3.0 : auto) */
	pr_info("%s: IS APSD : %d\n", __func__, charger->apsd_en);
	smb1351_update_reg(charger->i2c, SMB1351_CMD_IL_REG, 0x9, 0x9);

	if (charger->apsd_en == INPUT_CURRENT_MODE_AUTO) {
		/* For enable QC3.0 detection */
		smb1351_update_reg(charger->i2c, SMB1351_VARIOUS_FUNC3_REG,
			QUICK3_AUTO_AUTHENTICATION_BIT, QUICK3_AUTO_AUTHENTICATION_BIT);

		/* disable QC3.0 auto increment  */
		smb1351_update_reg(charger->i2c, SMB1351_VARIOUS_FUNC3_REG,
			0x00, QUICK3_AUTO_INCREMENT_BIT);

		/* For changing HVDCP adaptor setting - 5V */
		smb1351_update_reg(charger->i2c, SMB1351_HVDCP_BATTMISSING_CTRL_REG,
			0x00, HVDCP_ADAPTER_SEL_MASK);

		/* For changing input allowance(9V only) */
		smb1351_update_reg(charger->i2c, SMB1351_FLEXCHARGE_REG,
			0x20, CHARGER_CONFIGURATION_MASK);

		/* For changing input allowance(9V only) */
		smb1351_update_reg(charger->i2c, SMB1351_OTG_MODE_POWER_OPTIONS_REG,
			0x00, ADAPTER_CONFIG_MASK);

		/* For checking USBIN UV status */
		rc = smb1351_read_reg(charger->i2c, IRQ_E_REG, &data);
		if (!rc) {
			pr_info("%s: read USBIN status(0x44 = 0x%x)\n", __func__, data);
		} else {
			pr_info("%s: failed to read USBIN status\n", __func__);
		}

		/* For return back to initial HVDCP adaptor setting */
		smb1351_update_reg(charger->i2c, SMB1351_HVDCP_BATTMISSING_CTRL_REG,
			0x40, HVDCP_ADAPTER_SEL_MASK);

		/* last setting for APSD rerun */
		smb1351_update_reg(charger->i2c, SMB1351_FLEXCHARGE_REG,
			0x00, CHARGER_CONFIGURATION_MASK);

		/* Set TurboCharge+ (Max Value:175mV) */
		smb1351_update_reg(charger->i2c, SMB1351_CHARGE_CONTROL_REG,
			0x07, TURBO_CHARGE_MASK);

		/* APSD re-run */
		smb1351_update_reg(charger->i2c, SMB1351_CMD_HVDCP_REG,
			CMD_APSD_RE_RUN_BIT, CMD_APSD_RE_RUN_BIT);
	}

	/* temp code for APSD */
	/* smb1351_update_reg(charger->i2c, SMB1351_CHG_CURRENT_CTRL_REG, 0x52, 0xFF); */
}

static irqreturn_t smb1351_irq_handler(int irq, void *data)
{
	struct smb1351_charger_data *charger = data;
	u8 irq_h, status7;
	int rc;
	union power_supply_propval value;

	if (charger->apsd_en == INPUT_CURRENT_MODE_MANUAL)
		return IRQ_HANDLED;

	pr_info("%s: SMB1351 Interrupt occured\n", __func__);

	rc = smb1351_read_reg(charger->i2c, IRQ_H_REG, &irq_h);
	if (rc) {
		pr_info("%s: failed to read IRQ_G reg\n", __func__);
	} else if (irq_h & 0x0C) {
		if (charger->cable_type != SEC_BATTERY_CABLE_NONE && !charger->is_init) {
			smb1351_read_reg(charger->i2c, IRQ_H_REG, &irq_h);
			smb1351_read_reg(charger->i2c, SMB1351_STATUS7_REG, &status7);
			pr_info("%s: 0x47 :  0x%x, status7 : 0x%x\n",
					__func__, irq_h, status7);

			if (irq_h & 0x10) {
				pr_info("%s: QC3.0 Detected.\n", __func__);
				value.intval = SEC_BATTERY_CABLE_QC30;
				charger->charge_mode = MODE_QC30;
				charger->is_init = true;
			} else if (status7) {
				pr_info("%s: QC2.0 Detected.\n", __func__);
				value.intval = SEC_BATTERY_CABLE_QC20;
				charger->charge_mode = MODE_QC20;
				charger->is_init = true;
			} else {
				pr_info("%s: Normal charger Detected.\n", __func__);
				value.intval = SEC_BATTERY_CABLE_TA;
				charger->charge_mode = MODE_5V;
			}

			if (charger->is_init) {
				psy_do_property("battery", set,
					POWER_SUPPLY_PROP_ONLINE, value);

				queue_delayed_work(charger->wqueue, &charger->qc_work, 0);
			}
		}
	}

	smb1351_charger_test_read(charger);

	return IRQ_HANDLED;
}

static int smb1351_chg_get_property(struct power_supply *psy,
	enum power_supply_property psp, union power_supply_propval *val)
{
	struct smb1351_charger_data *charger = power_supply_get_drvdata(psy);
	enum power_supply_ext_property ext_psp = psp;

	switch (psp) {
	case POWER_SUPPLY_PROP_HEALTH:
		val->intval = smb1351_get_charger_health(charger);
		smb1351_charger_test_read(charger);
		break;
	case POWER_SUPPLY_PROP_STATUS:
	case POWER_SUPPLY_PROP_ONLINE:
		val->intval = smb1351_get_charger_state(charger);
		break;
	case POWER_SUPPLY_PROP_CURRENT_NOW:
		val->intval = smb1351_get_charge_current(charger);
		break;
	case POWER_SUPPLY_PROP_CURRENT_MAX:
		val->intval = smb1351_check_input_current(charger, val->intval);
		break;
	case POWER_SUPPLY_PROP_MAX ... POWER_SUPPLY_EXT_PROP_MAX:
		switch (ext_psp) {
		case POWER_SUPPLY_EXT_PROP_CHECK_SLAVE_I2C:
			{
				u8 reg_data;
				val->intval = (smb1351_read_reg(charger->i2c, SMB1351_STATUS4_REG, &reg_data) == 0);
			}
			break;
		case POWER_SUPPLY_EXT_PROP_CHECK_MULTI_CHARGE:
			val->intval = smb1351_get_charger_state(charger);
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

static int smb1351_chg_set_property(struct power_supply *psy,
	enum power_supply_property psp, const union power_supply_propval *val)
{
	struct smb1351_charger_data *charger = power_supply_get_drvdata(psy);

	switch (psp) {
	case POWER_SUPPLY_PROP_CHARGING_ENABLED:
		charger->is_charging =
			(val->intval == SEC_BAT_CHG_MODE_CHARGING) ? ENABLE : DISABLE;
		if (charger->is_init) {
			if (charger->is_charging)
				smb1351_check_charging_configure(charger);
			smb1351_set_charger_state(charger, charger->is_charging);
		}
		break;
	case POWER_SUPPLY_PROP_CURRENT_NOW:
		charger->charging_current = val->intval;
		smb1351_set_charge_current(charger, charger->charging_current);
		break;
	case POWER_SUPPLY_PROP_CHARGE_FULL_DESIGN:
		charger->siop_level = val->intval;
		break;
	case POWER_SUPPLY_PROP_ONLINE:
		charger->cable_type = val->intval;

		if (charger->cable_type == SEC_BATTERY_CABLE_NONE) {
			charger->is_init = false;
		} else if (!is_hv_qc_wire_type(charger->cable_type) && !charger->is_init) {
			smb1351_set_charger_volatile(charger, ENABLE);
			/* Enable QC3.0 Authentication IRQ */
			smb1351_update_reg(charger->i2c, 0x13, 0x1, 0x1);

			if (charger->cable_type != SEC_BATTERY_CABLE_NONE) {
				if (charger->cable_type == SEC_BATTERY_CABLE_TA)
					charger->apsd_en = INPUT_CURRENT_MODE_AUTO;
				else if (charger->cable_type == SEC_BATTERY_CABLE_PDIC) {
					charger->apsd_en = INPUT_CURRENT_MODE_MANUAL;
					charger->is_init = true;
				}

				smb1351_charger_initialize(charger);
			} else {
				charger->apsd_en = INPUT_CURRENT_MODE_MANUAL;
			}

			smb1351_charger_test_read(charger);
		}
		break;
	case POWER_SUPPLY_PROP_CURRENT_FULL:
		smb1351_set_topoff_current(charger, val->intval);
		break;
	case POWER_SUPPLY_PROP_CURRENT_MAX:
		pr_info("%s: Sub Charger Input current set : %d\n",
			__func__, val->intval);
		smb1351_set_input_current(charger, val->intval);
		break;
	case POWER_SUPPLY_PROP_STATUS:
	case POWER_SUPPLY_PROP_HEALTH:
	case POWER_SUPPLY_PROP_VOLTAGE_MAX:
		return -ENODATA;
	default:
		return -EINVAL;
	}

	return 0;
}

static void smb1351_charger_qc_detection_work(struct work_struct *work)
{
	struct smb1351_charger_data *charger =
		container_of(work, struct smb1351_charger_data, qc_work.work);

	wake_lock(&charger->qc_wake_lock);
	switch (charger->charge_mode) {
	case MODE_QC30:
		{
			int vbus_start = 5000, vbus_end = 7400, vbus_step = 200;
			u8 data = 0;

			smb1351_read_reg(charger->i2c, SMB1351_STATUS1_REG, &data);
			pr_info("%s: start increment algorithm(data:0x%x, vbus:%d)\n",
				__func__, data, vbus_start);

			do {
				smb1351_update_reg(charger->i2c, SMB1351_CMD_HVDCP_REG,
					0x1, CMD_INCREMENT_200MV_BIT);
				vbus_start += vbus_step;
				msleep(200);
			} while (vbus_start < vbus_end);

			pr_info("%s: finished increment vbus to %dmV\n",	__func__, vbus_start);
		}
		break;
	case MODE_QC20:
		smb1351_update_reg(charger->i2c, SMB1351_CMD_HVDCP_REG,
			0x20, CMD_FORCE_HVDCP_2P0_BIT);
		break;
	default:
		break;
	}

	wake_unlock(&charger->qc_wake_lock);
}

static int smb1351_charger_parse_dt(struct smb1351_charger_data *charger,
	struct smb1351_charger_platform_data *pdata)
{
	struct device_node *np = of_find_node_by_name(NULL, "smb1351-charger");
	int ret = 0;

	if (!np) {
		pr_err("%s: np is NULL\n", __func__);
		return -1;
	} else {
		ret = of_get_named_gpio_flags(np, "smb1351-charger,irq_gpio",
			0, NULL);
		if (ret < 0) {
			pr_err("%s: smb1351-charger,irq_gpio is empty\n", __func__);
			pdata->irq_gpio = 0;
		} else {
			pdata->irq_gpio = ret;
			pr_info("%s: irq_gpio = %d\n", __func__, pdata->irq_gpio);
		}
	}
#if 0
	np = of_find_node_by_name(NULL, "battery");
	if (!np) {
		pr_err("%s np NULL\n", __func__);
	} else {
		ret = of_property_read_u32(np, "battery,chg_float_voltage",
					   &charger->float_voltage);
		if (ret) {
			pr_info("%s: battery,chg_float_voltage is Empty\n", __func__);
			charger->float_voltage = 42000;
		}
	}
#endif
	return ret;
}

static int smb1351_chg_create_attrs(struct device *dev)
{
	unsigned long i;
	int rc;

	for (i = 0; i < ARRAY_SIZE(smb1351_charger_attrs); i++) {
		rc = device_create_file(dev, &smb1351_charger_attrs[i]);
		if (rc)
			goto create_attrs_failed;
	}
	return rc;

create_attrs_failed:
	dev_err(dev, "%s: failed (%d)\n", __func__, rc);
	while (i--)
		device_remove_file(dev, &smb1351_charger_attrs[i]);
	return rc;
}

ssize_t smb1351_chg_show_attrs(struct device *dev,
		struct device_attribute *attr, char *buf)
{
	struct power_supply *psy = dev_get_drvdata(dev);
	struct smb1351_charger_data *charger = power_supply_get_drvdata(psy);
	const ptrdiff_t offset = attr - smb1351_charger_attrs;
	int i = 0;

	switch(offset) {
		case MODE:
			i += scnprintf(buf + i, PAGE_SIZE - i, "%d\n", charger->charge_mode);
			break;
		case DATA:
			break;
		default:
			return -EINVAL;
	}
	return i;
}

ssize_t smb1351_chg_store_attrs(struct device *dev,
		struct device_attribute *attr,
		const char *buf, size_t count)
{
	struct power_supply *psy = dev_get_drvdata(dev);
	struct smb1351_charger_data *charger = power_supply_get_drvdata(psy);
	const ptrdiff_t offset = attr - smb1351_charger_attrs;
	int ret = 0;
	int x;

	switch(offset) {
		case MODE:
			if (sscanf(buf, "%d", &x) == 1) {
				if (x >= 0 && x <= 2) {
					pr_info("%s: SMB1351 Charge Mode Change to %s\n", __func__,
						x == 0 ? "5V" : x == 1 ? "QC2.0" : "QC3.0");
					smb1351_mode_change(charger, x);
				} else {
					pr_err("%s: SMB1351 Charge Mode : Invalid Mode\n", __func__);
				}
			}
			ret = count;
			break;
		case DATA:
			ret = count;
			break;
		default:
			ret = -EINVAL;
	}
	return ret;
}

static const struct power_supply_desc smb1351_charger_power_supply_desc = {
	.name = "smb1351-charger",
	.type = SEC_BATTERY_CABLE_UNKNOWN,
	.properties = smb1351_charger_props,
	.num_properties = ARRAY_SIZE(smb1351_charger_props),
	.get_property = smb1351_chg_get_property,
	.set_property = smb1351_chg_set_property,
};

static int smb1351_charger_probe(struct i2c_client *client,
	const struct i2c_device_id *id)
{
	struct device_node *of_node = client->dev.of_node;
	struct smb1351_charger_data *charger;
	struct smb1351_charger_platform_data *pdata = client->dev.platform_data;
	struct power_supply_config charger_cfg = {};
	int ret = 0;

	pr_info("%s: SMB1351 Charger Driver Loading\n", __func__);

	charger = kzalloc(sizeof(*charger), GFP_KERNEL);
	if (!charger) {
		pr_err("%s: Failed to allocate memory\n", __func__);
		return -ENOMEM;
	}

	mutex_init(&charger->io_lock);
	charger->dev = &client->dev;
	charger->i2c = client;
	if (of_node) {
		pdata = devm_kzalloc(&client->dev, sizeof(*pdata), GFP_KERNEL);
		if (!pdata) {
			pr_err("%s: Failed to allocate memory\n", __func__);
			ret = -ENOMEM;
			goto err_nomem;
		}
		ret = smb1351_charger_parse_dt(charger, pdata);
		if (ret < 0)
			goto err_parse_dt;
	} else {
		pdata = client->dev.platform_data;
	}

	charger->pdata = pdata;
	i2c_set_clientdata(client, charger);

	charger_cfg.drv_data = charger;

	/* smb1351_charger_initialize(charger); */
	charger->cable_type = SEC_BATTERY_CABLE_NONE;
	charger->apsd_en = INPUT_CURRENT_MODE_MANUAL;
	charger->charge_mode = MODE_5V;
	/* set max float voltage */
	charger->float_voltage = 45000;

	g_charger = charger;

	charger->psy_chg = power_supply_register(charger->dev, &smb1351_charger_power_supply_desc, &charger_cfg);
	if (!charger->psy_chg) {
		pr_err("%s: Failed to Register psy_chg\n", __func__);
		ret = -1;
		goto err_power_supply_register;
	}

	ret = smb1351_chg_create_attrs(&charger->psy_chg->dev);
	if (ret) {
		dev_err(charger->dev,
				"%s : Failed to create_attrs\n", __func__);
		goto err_req_irq;
	}

	charger->wqueue =
		create_singlethread_workqueue(dev_name(charger->dev));
	if (!charger->wqueue) {
		pr_err("%s: Fail to Create Workqueue\n", __func__);
		ret = -1;
		goto err_create_wqueue;
	}
	INIT_DELAYED_WORK(&charger->qc_work, smb1351_charger_qc_detection_work);
	wake_lock_init(&charger->qc_wake_lock, WAKE_LOCK_SUSPEND,
       "smb1351-qc-detection");

	if (pdata->irq_gpio) {
		charger->chg_irq = gpio_to_irq(pdata->irq_gpio);

		ret = request_threaded_irq(charger->chg_irq, NULL,
			smb1351_irq_handler,
			IRQF_TRIGGER_FALLING | IRQF_ONESHOT,
			"smb1351-irq", charger);
		if (ret < 0) {
			pr_err("%s: Failed to Request IRQ(%d)\n", __func__, ret);
			goto err_req_irq;
		}

		smb1351_update_reg(charger->i2c, SMB1351_CHG_PIN_EN_CTRL_REG, APSD_DONE_BIT, APSD_DONE_BIT);
	}
	device_init_wakeup(charger->dev, 1);


	pr_info("%s: SMB1351 Charger Driver Loaded\n", __func__);

	/* For controlling suspend mode */
	smb1351_update_reg(charger->i2c, SMB1351_VARIOUS_FUNC_REG,
			SUSPEND_MODE_CTRL_BY_I2C, SUSPEND_MODE_CTRL_BIT);

	return 0;

err_req_irq:
	wake_lock_destroy(&charger->qc_wake_lock);
err_create_wqueue:
	power_supply_unregister(charger->psy_chg);
err_power_supply_register:
	mutex_destroy(&charger->io_lock);
err_parse_dt:
	kfree(pdata);
err_nomem:
	kfree(charger);

	return ret;
}

static int smb1351_charger_remove(struct i2c_client *client)
{
	struct smb1351_charger_data *charger = i2c_get_clientdata(client);

	free_irq(charger->chg_irq, charger);
	wake_lock_destroy(&charger->qc_wake_lock);
	destroy_workqueue(charger->wqueue);
	power_supply_unregister(charger->psy_chg);
	mutex_destroy(&charger->io_lock);
	kfree(charger->pdata);
	kfree(charger);

	return 0;
}

static void smb1351_charger_shutdown(struct i2c_client *client)
{
	struct smb1351_charger_data *charger = i2c_get_clientdata(client);

	if (charger->chg_irq)
		free_irq(charger->chg_irq, charger);
	smb1351_set_charger_state(charger, 0);
}

static const struct i2c_device_id smb1351_charger_id_table[] = {
	{"smb1351-charger", 0},
	{},
};
MODULE_DEVICE_TABLE(i2c, smb1351_id_table);

#ifdef CONFIG_OF
static struct of_device_id smb1351_charger_match_table[] = {
	{.compatible = "smb,smb1351-charger"},
	{},
};
#else
#define smb1351_charger_match_table NULL
#endif

#if defined(CONFIG_PM)
static int smb1351_charger_suspend(struct device *dev)
{
	struct i2c_client *i2c = container_of(dev, struct i2c_client, dev);
	struct smb1351_charger_data *charger = i2c_get_clientdata(i2c);

	if (device_may_wakeup(dev))
		enable_irq_wake(charger->chg_irq);

	disable_irq(charger->chg_irq);

	return 0;
}

static int smb1351_charger_resume(struct device *dev)
{
	struct i2c_client *i2c = container_of(dev, struct i2c_client, dev);
	struct smb1351_charger_data *charger = i2c_get_clientdata(i2c);

	if (charger->chg_irq) {
		if (device_may_wakeup(dev))
			disable_irq_wake(charger->chg_irq);
		enable_irq(charger->chg_irq);
	}

	return 0;
}
#else
#define smb1351_charger_suspend		NULL
#define smb1351_charger_resume		NULL
#endif /* CONFIG_PM */

const struct dev_pm_ops smb1351_pm = {
	.suspend = smb1351_charger_suspend,
	.resume = smb1351_charger_resume,
};

static struct i2c_driver smb1351_charger_driver = {
	.driver = {
		.name	= "smb1351-charger",
		.owner	= THIS_MODULE,
#if defined(CONFIG_PM)
		.pm	= &smb1351_pm,
#endif /* CONFIG_PM */
		.of_match_table = smb1351_charger_match_table,
	},
	.probe		= smb1351_charger_probe,
	.remove		= smb1351_charger_remove,
	.shutdown	= smb1351_charger_shutdown,
	.id_table	= smb1351_charger_id_table,
};

static int __init smb1351_charger_init(void)
{
	pr_info("%s: \n", __func__);
	return i2c_add_driver(&smb1351_charger_driver);
}

static void __exit smb1351_charger_exit(void)
{
	pr_info("%s: \n", __func__);
	i2c_del_driver(&smb1351_charger_driver);
}

module_init(smb1351_charger_init);
module_exit(smb1351_charger_exit);

MODULE_DESCRIPTION("Samsung SMB1351 Charger Driver");
MODULE_AUTHOR("Samsung Electronics");
MODULE_LICENSE("GPL");
