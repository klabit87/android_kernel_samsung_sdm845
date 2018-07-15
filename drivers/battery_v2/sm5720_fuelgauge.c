/*
 *  sm5720_fuelgauge.c
 *  Samsung SM5720 Fuel Gauge Driver
 *
 *  Copyright (C) 2015 Samsung Electronics
 *
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

/* #define BATTERY_LOG_MESSAGE */

#include <linux/mfd/sm5720-private.h>
#include "include/fuelgauge/sm5720_fuelgauge.h"
#include <linux/of_gpio.h>
#include <linux/debugfs.h>
#include <linux/seq_file.h>
#include "include/sec_charging_common.h"

static enum power_supply_property sm5720_fuelgauge_props[] = {
};

#define MINVAL(a, b) ((a <= b) ? a : b)
#define MAXVAL(a, b) ((a > b) ? a : b)

#define LIMIT_N_CURR_MIXFACTOR -2000
#define TABLE_READ_COUNT 2
#define FG_ABNORMAL_RESET -1
#define IGNORE_N_I_OFFSET 1

#define CAPACITY_MAX_CONTROL_THRESHOLD 300
#define CAPACITY_MAX_THRESHOLD 1000

//#define SM5720_FG_FULL_DEBUG 1

static int sm5720_get_full_chg_mq (struct sm5720_fuelgauge_data *fuelgauge);
static void sm5720_set_full_chg_mq (struct sm5720_fuelgauge_data *fuelgauge, int mq);
static void sm5720_meas_mq_suspend (struct sm5720_fuelgauge_data *fuelgauge);
static void sm5720_meas_mq_resume (struct sm5720_fuelgauge_data *fuelgauge);
static void sm5720_meas_mq_start (struct sm5720_fuelgauge_data *fuelgauge);
static int sm5720_meas_eq_dump (struct sm5720_fuelgauge_data *fuelgauge);
static int sm5720_meas_mq_dump (struct sm5720_fuelgauge_data *fuelgauge);
static bool sm5720_fg_init(struct sm5720_fuelgauge_data *fuelgauge, bool is_surge);
static void sm5720_meas_mq_off (struct sm5720_fuelgauge_data *fuelgauge);

static int sm5720_device_id = -1;

extern unsigned int lpcharge;

enum sm5720_battery_table_type {
	DISCHARGE_TABLE = 0,
	Q_TABLE,
	TABLE_MAX,
};

void sm5720_adabt_full_offset(struct sm5720_fuelgauge_data *fuelgauge);

bool sm5720_fg_fuelalert_init(struct sm5720_fuelgauge_data *fuelgauge,
                int soc);

#if !defined(CONFIG_SEC_FACTORY)
static void sm5720_fg_periodic_read(struct sm5720_fuelgauge_data *fuelgauge)
{
    u8 reg;
    int i;
    int data[0x10];
    char *str = NULL;

    str = kzalloc(sizeof(char)*2560, GFP_KERNEL);
    if (!str)
        return;

    for (i = 0; i <= 0xC; i++) {
        if (i == 7)
            i = 8;
        for (reg = 0; reg < 0x10; reg++) {
            if((i == 0xC) && (reg > 2))
            {
                data[reg] = 0;
                continue;
            }
	    	if((i == 0) && ((reg == 0) || (reg == 2)))
            {
                data[reg] = 0;
                reg++;
            }
            data[reg] = sm5720_read_word(fuelgauge->i2c, reg + (i * 0x10));
            if (data[reg] < 0) {
                kfree(str);
                return;
            }
        }

        if(i == 0xC)
        {
            sprintf(str+strlen(str),
            "%02x:%04x,%04x,%04x,%04x,",
            i, data[0x00], data[0x01], data[0x02], data[0x03]);
            break;
        }
        sprintf(str+strlen(str),
            "%02x:%04x,%04x,%04x,%04x,%04x,%04x,%04x,%04x,",
            i, data[0x00], data[0x01], data[0x02], data[0x03],
            data[0x04], data[0x05], data[0x06], data[0x07]);
        sprintf(str+strlen(str),
            "%04x,%04x,%04x,%04x,%04x,%04x,%04x,%04x,",
            data[0x08], data[0x09], data[0x0a], data[0x0b],
            data[0x0c], data[0x0d], data[0x0e], data[0x0f]);
		if (!fuelgauge->sleep_initial_update_of_soc) {
			msleep(1); // it has to call msleep
		}
    }

    pr_info("[FG_ALL] %s\n", str);

    kfree(str);
}
#endif

static bool sm5720_fg_check_reg_init_need(struct sm5720_fuelgauge_data *fuelgauge)
{
	int ret;

	ret = sm5720_read_word(fuelgauge->i2c, SM5720_FG_REG_FG_OP_STATUS);

	if((ret & INIT_CHECK_MASK) == DISABLE_RE_INIT)
	{
		pr_info("%s: SM5720_REG_FG_OP_STATUS : 0x%x , return FALSE NO init need\n", __func__, ret);
		return 0;
	}
	else
	{
		pr_info("%s: SM5720_REG_FG_OP_STATUS : 0x%x , return TRUE init need!!!!\n", __func__, ret);
		return 1;
	}
}

void sm5720_cal_avg_vbat(struct sm5720_fuelgauge_data *fuelgauge)
{
	if (fuelgauge->info.batt_avgvoltage == 0)
		fuelgauge->info.batt_avgvoltage = fuelgauge->info.batt_voltage;

	else if (fuelgauge->info.batt_voltage == 0 && fuelgauge->info.p_batt_voltage == 0)
		fuelgauge->info.batt_avgvoltage = 3400;

	else if(fuelgauge->info.batt_voltage == 0)
		fuelgauge->info.batt_avgvoltage =
				((fuelgauge->info.batt_avgvoltage) + (fuelgauge->info.p_batt_voltage))/2;

	else if(fuelgauge->info.p_batt_voltage == 0)
		fuelgauge->info.batt_avgvoltage =
				((fuelgauge->info.batt_avgvoltage) + (fuelgauge->info.batt_voltage))/2;

	else
		fuelgauge->info.batt_avgvoltage =
				((fuelgauge->info.batt_avgvoltage*2) +
				 (fuelgauge->info.p_batt_voltage+fuelgauge->info.batt_voltage))/4;

#if defined(SM5720_FG_FULL_DEBUG)
	pr_info("%s: batt_avgvoltage = %d\n", __func__, fuelgauge->info.batt_avgvoltage);
#endif

	return;
}

void sm5720_voffset_cancel(struct sm5720_fuelgauge_data *fuelgauge)
{
	int volt_slope, mohm_volt_cal;
	int fg_temp_gap=0, volt_cal=0, fg_delta_volcal=0, pn_volt_slope=0, volt_offset=0;

	if(sm5720_read_word(fuelgauge->i2c, SM5720_FG_REG_AUX_STAT) & 0x02)
	{
		sm5720_write_word(fuelgauge->i2c, SM5720_FG_REG_VOLT_CAL, fuelgauge->info.volt_cal);
	}
	else
	{
		//set vbat offset cancel start
		volt_slope = sm5720_read_word(fuelgauge->i2c, SM5720_FG_REG_VOLT_CAL);
		volt_slope = volt_slope & 0xFF00;
		mohm_volt_cal = fuelgauge->info.volt_cal & 0x00FF;
		if(fuelgauge->info.enable_v_offset_cancel_p)
		{
			if(fuelgauge->is_charging && (fuelgauge->info.batt_current > fuelgauge->info.v_offset_cancel_level))
			{
				if(mohm_volt_cal & 0x0080)
				{
					mohm_volt_cal = -(mohm_volt_cal & 0x007F);
				}
				mohm_volt_cal = mohm_volt_cal - (fuelgauge->info.batt_current/(fuelgauge->info.v_offset_cancel_mohm * 13)); // ((curr*0.001)*0.006)*2048 -> 6mohm
				if(mohm_volt_cal < 0)
				{
					mohm_volt_cal = -mohm_volt_cal;
					mohm_volt_cal = mohm_volt_cal|0x0080;
				}
			}
		}
		if(fuelgauge->info.enable_v_offset_cancel_n)
		{
			if(!(fuelgauge->is_charging) && (fuelgauge->info.batt_current < -(fuelgauge->info.v_offset_cancel_level)))
			{
				if(fuelgauge->info.volt_cal & 0x0080)
				{
					mohm_volt_cal = -(mohm_volt_cal & 0x007F);
				}
				mohm_volt_cal = mohm_volt_cal - (fuelgauge->info.batt_current/(fuelgauge->info.v_offset_cancel_mohm * 13)); // ((curr*0.001)*0.006)*2048 -> 6mohm
				if(mohm_volt_cal < 0)
				{
					mohm_volt_cal = -mohm_volt_cal;
					mohm_volt_cal = mohm_volt_cal|0x0080;
				}
			}
		}
		sm5720_write_word(fuelgauge->i2c, SM5720_FG_REG_VOLT_CAL, (mohm_volt_cal | volt_slope));
		pr_info("%s: <%d %d %d %d> volt_cal = 0x%x, volt_slope = 0x%x, mohm_volt_cal = 0x%x\n",
				__func__, fuelgauge->info.enable_v_offset_cancel_p, fuelgauge->info.enable_v_offset_cancel_n
				, fuelgauge->info.v_offset_cancel_level, fuelgauge->info.v_offset_cancel_mohm
				, fuelgauge->info.volt_cal, volt_slope, mohm_volt_cal);
		//set vbat offset cancel end

		fg_temp_gap = (fuelgauge->info.temp_fg/10) - fuelgauge->info.temp_std;

		volt_cal = sm5720_read_word(fuelgauge->i2c, SM5720_FG_REG_VOLT_CAL);
		volt_offset = volt_cal & 0x00FF;
		pn_volt_slope = fuelgauge->info.volt_cal & 0xFF00;

		if (fuelgauge->info.en_fg_temp_volcal)
		{
			fg_delta_volcal = (fg_temp_gap / fuelgauge->info.fg_temp_volcal_denom)*fuelgauge->info.fg_temp_volcal_fact;
			pn_volt_slope = pn_volt_slope + (fg_delta_volcal<<8);
			volt_cal = pn_volt_slope | volt_offset;
			sm5720_write_word(fuelgauge->i2c, SM5720_FG_REG_VOLT_CAL, volt_cal);
		}
	}

	return;
}

static unsigned int sm5720_get_vbat(struct sm5720_fuelgauge_data *fuelgauge)
{
	int ret = 0;
	unsigned int vbat = 0;/* = 3500; 3500 means 3500mV*/

    sm5720_voffset_cancel(fuelgauge);

	ret = sm5720_read_word(fuelgauge->i2c, SM5720_FG_REG_VOLTAGE);
	if (ret<0) {
		pr_err("%s: read vbat reg fail", __func__);
		vbat = 4000;
	} else {
		vbat = ((ret&0x3800)>>11) * 1000; //integer;
		vbat = vbat + (((ret&0x07ff)*1000)/2048); // integer + fractional
	}
	fuelgauge->info.batt_voltage = vbat;

#if defined(SM5720_FG_FULL_DEBUG)
	pr_info("%s: read = 0x%x, vbat = %d\n", __func__, ret, vbat);
#endif
	sm5720_cal_avg_vbat(fuelgauge);

	if ((fuelgauge->vempty_mode == VEMPTY_MODE_SW_VALERT) &&
		(vbat >= fuelgauge->battery_data->sw_v_empty_recover_vol)) {
		fuelgauge->vempty_mode = VEMPTY_MODE_SW_RECOVERY;

        sm5720_fg_fuelalert_init(fuelgauge,
                       fuelgauge->pdata->fuel_alert_soc);
		pr_info("%s : Recoverd from SW V EMPTY Activation\n", __func__);

    }

	return vbat;
}

static unsigned int sm5720_get_ocv(struct sm5720_fuelgauge_data *fuelgauge)
{
	int ret;
	unsigned int ocv;// = 3500; /*3500 means 3500mV*/

	ret = sm5720_read_word(fuelgauge->i2c, SM5720_FG_REG_OCV);
	if (ret<0) {
		pr_err("%s: read ocv reg fail\n", __func__);
		ocv = 4000;
	} else {
		ocv = ((ret&0x7800)>>11) * 1000; //integer;
		ocv = ocv + (((ret&0x07ff)*1000)/2048); // integer + fractional
	}

	fuelgauge->info.batt_ocv = ocv;
#if defined(SM5720_FG_FULL_DEBUG)
	pr_info("%s: read = 0x%x, ocv = %d\n", __func__, ret, ocv);
#endif

	return ocv;
}

void sm5720_cal_avg_current(struct sm5720_fuelgauge_data *fuelgauge)
{
	if (fuelgauge->info.batt_avgcurrent == 0)
		fuelgauge->info.batt_avgcurrent = fuelgauge->info.batt_current;

	else if (fuelgauge->info.batt_avgcurrent == 0 && fuelgauge->info.p_batt_current == 0)
		fuelgauge->info.batt_avgcurrent = fuelgauge->info.batt_current;

	else if(fuelgauge->info.batt_current == 0)
		fuelgauge->info.batt_avgcurrent =
				((fuelgauge->info.batt_avgcurrent) + (fuelgauge->info.p_batt_current))/2;

	else if(fuelgauge->info.p_batt_current == 0)
		fuelgauge->info.batt_avgcurrent =
				((fuelgauge->info.batt_avgcurrent) + (fuelgauge->info.batt_current))/2;

	else
		fuelgauge->info.batt_avgcurrent =
				((fuelgauge->info.batt_avgcurrent*2) +
				 (fuelgauge->info.p_batt_current+fuelgauge->info.batt_current))/4;

#if defined(SM5720_FG_FULL_DEBUG)
	pr_info("%s: batt_avgcurrent = %d\n", __func__, fuelgauge->info.batt_avgcurrent);
#endif

	return;
}

static int sm5720_get_curr(struct sm5720_fuelgauge_data *fuelgauge)
{
	int ret;
	int curr = 0, curr_check = 0, aux_stat = 0, er = 0; /* = 1000; 1000 means 1000mA*/

#if defined(ENABLE_FULL_OFFSET)
	sm5720_adabt_full_offset(fuelgauge);
#endif

	ret = sm5720_read_word(fuelgauge->i2c, SM5720_FG_REG_CURRENT);
	curr_check = sm5720_read_word(fuelgauge->i2c, SM5720_FG_REG_CURRENT_EST);
	er = sm5720_read_word(fuelgauge->i2c, SM5720_FG_REG_CURRENT_ERR);

	if (ret<0) {
		pr_err("%s: read curr reg fail", __func__);
		curr = 0;
	} else {
		if(((ret&0x3FFF)>0x1A66) && (((ret&0x3FFF)-(curr_check&0x3FFF))>0x1000) && ((er&0x3FFF)<0x800))
		{
			if(sm5720_read_word(fuelgauge->i2c, SM5720_FG_REG_FG_OP_STATUS) & 0x10)
			{
				if(fuelgauge->info.p_batt_current < 0)
					ret = ((abs(fuelgauge->info.p_batt_current)<<11)/1000)|0x8000;
				else
					ret = (fuelgauge->info.p_batt_current<<11)/1000;
				pr_info("%s: ret = 0x%x, curr_check = 0x%x, er = 0x%x\n", __func__, ret, curr_check, er);
			}
		}

		curr = ((ret&0x3800)>>11) * 1000; //integer;
		curr = curr + (((ret&0x07ff)*1000)/2048); // integer + fractional
		aux_stat = sm5720_read_word(fuelgauge->i2c, SM5720_FG_REG_AUX_STAT);
		if(((aux_stat & 0xD0) == 0x90) || (aux_stat & 0x02) || (!(ret&0x8000) && !(aux_stat & 0x40)))
		{
			curr = curr-(((ret&0x3fff)*1000)>>11);
		}
		if(ret&0x8000) {
			curr *= -1;
		}
	}
	fuelgauge->info.batt_current = curr;

#if defined(SM5720_FG_FULL_DEBUG)
		pr_info("%s: read = 0x%x, curr = %d\n", __func__, ret, curr);
#endif

	sm5720_cal_avg_current(fuelgauge);

	return curr;
}

static int sm5720_get_temperature(struct sm5720_fuelgauge_data *fuelgauge)
{
	int ret;
	int temp;/* = 250; 250 means 25.0oC*/

	ret = sm5720_read_word(fuelgauge->i2c, SM5720_FG_REG_TEMPERATURE);
	if (ret<0) {
		pr_err("%s: read temp reg fail", __func__);
		temp = 0;
	} else {
		temp = ((ret&0x7F00)>>8) * 10; //integer bit
		temp = temp + (((ret&0x00f0)*10)/256); // integer + fractional bit
		if(ret&0x8000) {
			temp *= -1;
		}
	}
	fuelgauge->info.temp_fg = temp;

#if defined(SM5720_FG_FULL_DEBUG)
	pr_info("%s: read = 0x%x, temp_fg = %d\n", __func__, ret, temp);
#endif

	return temp;
}

static int sm5720_get_cycle(struct sm5720_fuelgauge_data *fuelgauge)
{
	int ret;
	int cycle;

	ret = sm5720_read_word(fuelgauge->i2c, SM5720_FG_REG_CYCLE);
	if (ret<0) {
		pr_err("%s: read cycle reg fail", __func__);
		cycle = 0;
	} else {
		cycle = ret&0x03FF;
	}
	fuelgauge->info.batt_soc_cycle = cycle;

#if defined(SM5720_FG_FULL_DEBUG)
	pr_info("%s: read = 0x%x, soc_cycle = %d\n", __func__, ret, cycle);
#endif

	return cycle;
}

static void sm5720_vbatocv_check(struct sm5720_fuelgauge_data *fuelgauge)
{
	int ret;

	if((abs(fuelgauge->info.batt_current)<50) ||
	   ((fuelgauge->is_charging) && (fuelgauge->info.batt_current<(fuelgauge->info.top_off)) &&
	   (fuelgauge->info.batt_current>(fuelgauge->info.top_off/3)) && (fuelgauge->info.batt_soc>=900)))
	{
		if(abs(fuelgauge->info.batt_ocv-fuelgauge->info.batt_voltage)>30) // 30mV over
		{
			fuelgauge->info.iocv_error_count ++;
		}

		pr_info("%s: sm5720 FG iocv_error_count (%d)\n", __func__, fuelgauge->info.iocv_error_count);

		if(fuelgauge->info.iocv_error_count > 5) // prevent to overflow
			fuelgauge->info.iocv_error_count = 6;
	}
	else
	{
		fuelgauge->info.iocv_error_count = 0;
	}

	if(fuelgauge->info.iocv_error_count > 5)
	{
		pr_info("%s: p_v - v = (%d)\n", __func__, fuelgauge->info.p_batt_voltage - fuelgauge->info.batt_voltage);
		if(abs(fuelgauge->info.p_batt_voltage - fuelgauge->info.batt_voltage)>15) // 15mV over
		{
			fuelgauge->info.iocv_error_count = 0;
		}
		else
		{
			// mode change to mix RS manual mode
			pr_info("%s: mode change to mix RS manual mode\n", __func__);
			// run update set
			sm5720_write_word(fuelgauge->i2c, SM5720_FG_REG_PARAM_RUN_UPDATE, 1);
			// RS manual value write
			sm5720_write_word(fuelgauge->i2c, SM5720_FG_REG_RS_MAN, fuelgauge->info.rs_value[0]);
			// run update set
			sm5720_write_word(fuelgauge->i2c, SM5720_FG_REG_PARAM_RUN_UPDATE, 0);
			// mode change
			ret = sm5720_read_word(fuelgauge->i2c, SM5720_FG_REG_CNTL);
			ret = (ret | ENABLE_MIX_MODE) | ENABLE_RS_MAN_MODE; // +RS_MAN_MODE
			sm5720_write_word(fuelgauge->i2c, SM5720_FG_REG_CNTL, ret);
		}
	}
	else
	{
		/* voltage mode with 3.4V */
		if (fuelgauge->vempty_mode != VEMPTY_MODE_HW)
		{
			if((fuelgauge->info.p_batt_voltage < fuelgauge->info.n_tem_poff) &&
				(fuelgauge->info.batt_voltage < fuelgauge->info.n_tem_poff) && (!fuelgauge->is_charging))
			{
				pr_info("%s: mode change to normal tem mix RS manual mode\n", __func__);
				// mode change to mix RS manual mode
				// run update init
				sm5720_write_word(fuelgauge->i2c, SM5720_FG_REG_PARAM_RUN_UPDATE, 1);
				// RS manual value write
				if((fuelgauge->info.p_batt_voltage <
					(fuelgauge->info.n_tem_poff - fuelgauge->info.n_tem_poff_offset)) &&
					(fuelgauge->info.batt_voltage <
					(fuelgauge->info.n_tem_poff - fuelgauge->info.n_tem_poff_offset)))
				{
					sm5720_write_word(fuelgauge->i2c, SM5720_FG_REG_RS_MAN, fuelgauge->info.rs_value[0]>>1);
				}
				else
				{
					sm5720_write_word(fuelgauge->i2c, SM5720_FG_REG_RS_MAN, fuelgauge->info.rs_value[0]);
				}
				// run update set
				sm5720_write_word(fuelgauge->i2c, SM5720_FG_REG_PARAM_RUN_UPDATE, 0);

				// mode change
				ret = sm5720_read_word(fuelgauge->i2c, SM5720_FG_REG_CNTL);
				ret = (ret | ENABLE_MIX_MODE) | ENABLE_RS_MAN_MODE; // +RS_MAN_MODE
				sm5720_write_word(fuelgauge->i2c, SM5720_FG_REG_CNTL, ret);
			}
			else
			{
				pr_info("%s: mode change to mix RS auto mode\n", __func__);

				// mode change to mix RS auto mode
				ret = sm5720_read_word(fuelgauge->i2c, SM5720_FG_REG_CNTL);
				ret = (ret | ENABLE_MIX_MODE) & ~ENABLE_RS_MAN_MODE; // -RS_MAN_MODE
				sm5720_write_word(fuelgauge->i2c, SM5720_FG_REG_CNTL, ret);
			}
		}
		else /* voltage mode with 3.25V, VEMPTY_MODE_HW mode */
		{
			if((fuelgauge->info.p_batt_voltage < fuelgauge->info.l_tem_poff) &&
				(fuelgauge->info.batt_voltage < fuelgauge->info.l_tem_poff) && (!fuelgauge->is_charging))
			{
				pr_info("%s: mode change to normal tem mix RS manual mode\n", __func__);
				// mode change to mix RS manual mode
				// run update init
				sm5720_write_word(fuelgauge->i2c, SM5720_FG_REG_PARAM_RUN_UPDATE, 1);
				// RS manual value write
				if((fuelgauge->info.p_batt_voltage <
					(fuelgauge->info.l_tem_poff - fuelgauge->info.l_tem_poff_offset)) &&
					(fuelgauge->info.batt_voltage <
					(fuelgauge->info.l_tem_poff - fuelgauge->info.l_tem_poff_offset)))
				{
					sm5720_write_word(fuelgauge->i2c, SM5720_FG_REG_RS_MAN, fuelgauge->info.rs_value[0]>>1);
				}
				else
				{
					sm5720_write_word(fuelgauge->i2c, SM5720_FG_REG_RS_MAN, fuelgauge->info.rs_value[0]);
				}
				// run update set
				sm5720_write_word(fuelgauge->i2c, SM5720_FG_REG_PARAM_RUN_UPDATE, 0);

				// mode change
				ret = sm5720_read_word(fuelgauge->i2c, SM5720_FG_REG_CNTL);
				ret = (ret | ENABLE_MIX_MODE) | ENABLE_RS_MAN_MODE; // +RS_MAN_MODE
				sm5720_write_word(fuelgauge->i2c, SM5720_FG_REG_CNTL, ret);
			}
			else
			{
				pr_info("%s: mode change to mix RS auto mode\n", __func__);

				// mode change to mix RS auto mode
				ret = sm5720_read_word(fuelgauge->i2c, SM5720_FG_REG_CNTL);
				ret = (ret | ENABLE_MIX_MODE) & ~ENABLE_RS_MAN_MODE; // -RS_MAN_MODE
				sm5720_write_word(fuelgauge->i2c, SM5720_FG_REG_CNTL, ret);
			}
		}
	}
	fuelgauge->info.p_batt_voltage = fuelgauge->info.batt_voltage;
	fuelgauge->info.p_batt_current = fuelgauge->info.batt_current;
	// iocv error case cover end
}

#if defined(ENABLE_FULL_OFFSET)
void sm5720_adabt_full_offset(struct sm5720_fuelgauge_data *fuelgauge)
{
	int full_offset, i_offset, sign_offset;
	int curr_off, sign_origin, i_origin;
	int curr, sign_curr, i_curr;
	int aux_stat;

#if defined(SM5720_FG_FULL_DEBUG)
	pr_info("%s: flag_charge_health=%d, flag_full_charge=%d\n", __func__, fuelgauge->info.flag_charge_health, fuelgauge->info.flag_full_charge);
#endif
	curr = sm5720_read_word(fuelgauge->i2c, SM5720_FG_REG_CURRENT);
	aux_stat = sm5720_read_word(fuelgauge->i2c, SM5720_FG_REG_AUX_STAT);
	curr_off = sm5720_read_word(fuelgauge->i2c, SM5720_FG_REG_DP_CSP_I_OFF);
	pr_info("%s: curr=%x, curr_off=%x, aux_stat=%x, \n", __func__, curr, curr_off, aux_stat);

	if(((aux_stat & 0xD0) == 0x90) || ((!(aux_stat & 0x40)) && (!(curr & 0x8000))))
	{
		sign_curr = curr & 0x8000;
		i_curr = (curr & 0x7FFF)>>1;
		if(sign_curr)
			i_curr = -i_curr;

		sign_origin = curr_off & 0x0080;
		i_origin = curr_off & 0x007F;
		if(sign_origin)
			i_origin = -i_origin;

		full_offset = i_origin - i_curr;
		if(full_offset < 0)
		{
			i_offset = -full_offset;
			sign_offset = 1;
		}
		else
		{
			i_offset = full_offset;
			sign_offset = 0;
		}

		if(i_offset > 0x10)
			full_offset = curr_off;
		else if(sign_offset == 1)
			full_offset = i_offset|0x0080;

		sm5720_write_word(fuelgauge->i2c, SM5720_FG_REG_DP_ECV_I_OFF, full_offset);
		sm5720_write_word(fuelgauge->i2c, SM5720_FG_REG_DP_CSP_I_OFF, full_offset);
		sm5720_write_word(fuelgauge->i2c, SM5720_FG_REG_DP_CSN_I_OFF, full_offset);
		pr_info("%s: full_offset=%x, i_offset=%x, sign_offset=%d, full_offset_margin=%x, full_extra_offset=%x\n", __func__, full_offset, i_offset, sign_offset, fuelgauge->info.full_offset_margin, fuelgauge->info.full_extra_offset);
	}
	else
	{
		sm5720_write_word(fuelgauge->i2c, SM5720_FG_REG_DP_ECV_I_OFF, fuelgauge->info.dp_ecv_i_off);
	}

	return;
}
#endif

static int sm5720_get_full_chg_mq (struct sm5720_fuelgauge_data *fuelgauge)
{
    int mq_high, mq_low, mq_raw, mq;

    mq_high = 0x00FF & sm5720_read_word(fuelgauge->i2c, SM5720_FG_REG_FULL_MQ_HIGH);
    mq_low = 0x00FF & sm5720_read_word(fuelgauge->i2c, SM5720_FG_REG_FULL_MQ_LOW);

    mq_raw = (mq_high<<8) + mq_low;

    mq = ((mq_raw&0x07FF) * 1000) >> 7;
    if(mq_raw&0x8000) {
        mq = -mq;
    }

    pr_info("%s: high = 0x%x, low = 0x%x, raw = 0x%x, mq = %d\n", __func__, mq_high, mq_low, mq_raw, mq);

    return mq;
}

static void sm5720_set_full_chg_mq (struct sm5720_fuelgauge_data *fuelgauge, int mq)
{
    int mq_sign=0, mq_high, mq_low, mq_abs;

    mq_abs = mq;
    if(mq_abs < 0)
    {
        mq_sign = 1;
        mq_abs = -mq_abs;
    }

    mq_abs = (mq_abs << 7)/1000;
    mq_high = mq_abs & 0x0700;
    mq_low = mq_abs & 0x00FF;
    if(mq_sign == 1)
    {
        mq_high = mq_high | 0x8000;
    }
    mq_high = mq_high >> 8;

    sm5720_write_word(fuelgauge->i2c, SM5720_FG_REG_FULL_MQ_HIGH, mq_high);
    sm5720_write_word(fuelgauge->i2c, SM5720_FG_REG_FULL_MQ_LOW, mq_low);

    pr_info("%s: mq = %d, sign = %d, abs = 0x%x, high = 0x%x, low = 0x%x\n", __func__, mq, mq_sign, mq_abs, mq_high, mq_low);
}

static void sm5720_meas_mq_suspend (struct sm5720_fuelgauge_data *fuelgauge)
{
    int ret, suspend_mq;

    ret = sm5720_read_word(fuelgauge->i2c, SM5720_FG_REG_AUX_2);

    if((ret & START_MQ) == START_MQ)
    {
        suspend_mq = sm5720_meas_mq_dump(fuelgauge);

        sm5720_write_word(fuelgauge->i2c, SM5720_FG_REG_START_MQ, ((suspend_mq<<11)/1000));
        pr_info("%s: suspend mode mq is <0x%x>, AUX_2 : <0x%x> \n", __func__, suspend_mq, ret);

        sm5720_write_word(fuelgauge->i2c, SM5720_FG_REG_AUX_2, TH_SOC_INV);
    }
    else
    {
        pr_info("%s: AUX_2 : <0x%x> \n", __func__, ret);
    }
}

static void sm5720_meas_mq_resume (struct sm5720_fuelgauge_data *fuelgauge)
{
    int ret;
    ret = sm5720_read_word(fuelgauge->i2c, SM5720_FG_REG_AUX_2);
    pr_info("%s: SM5720_FG_REG_AUX_2 : <0x%x> \n", __func__, ret);

    sm5720_write_word(fuelgauge->i2c, SM5720_FG_REG_AUX_2, (START_MQ|TH_SOC_INV));
}

static void sm5720_meas_mq_start (struct sm5720_fuelgauge_data *fuelgauge)
{
    int ret, mq;
    ret = sm5720_read_word(fuelgauge->i2c, SM5720_FG_REG_AUX_2);
    pr_info("%s: SM5720_FG_REG_AUX_2 : <0x%x> \n", __func__, ret);

    if((ret & START_MQ) != START_MQ)
    {
        ret = sm5720_read_word(fuelgauge->i2c, SM5720_FG_REG_START_MQ);
        if(ret == 0)
        {
            mq = sm5720_read_word(fuelgauge->i2c, SM5720_FG_REG_Q_EST);
            sm5720_write_word(fuelgauge->i2c, SM5720_FG_REG_START_MQ, mq);
            pr_info("%s: starting mq is <0x%x> \n", __func__, mq);

            msleep(50);
            sm5720_write_word(fuelgauge->i2c, SM5720_FG_REG_AUX_2, (START_MQ|TH_SOC_INV));

            // full mq set
			sm5720_set_full_chg_mq(fuelgauge, ((fuelgauge->info.cap * 1000) >> 11));
        }
        else
        {
            pr_info("%s: read start mq is <0x%x> and resumed \n", __func__, ret);
            sm5720_meas_mq_resume(fuelgauge);
        }
    }
    else
    {
        pr_info("%s: sm5720_meas_mq_start already started : mq is <%d> \n", __func__, sm5720_meas_mq_dump(fuelgauge));
    }
}

static int sm5720_meas_eq_dump (struct sm5720_fuelgauge_data *fuelgauge)
{
    int ret, eq;
    ret = sm5720_read_word(fuelgauge->i2c, SM5720_FG_REG_Q_EST);
    pr_info("%s: SM5720_FG_REG_Q_EST : <0x%x> \n", __func__, ret);

    eq = ((ret&0x7FFF) * 1000) >> 11;
    if(ret&0x8000) {
        eq *= -1;
    }

    if(eq == 0) {
        eq = (fuelgauge->info.cap * 1000) >> 11;
        pr_info("%s: eq is 0, It's abnormal, return cap : %d\n", __func__, eq);
    }

    return eq;
}

static int sm5720_meas_mq_dump (struct sm5720_fuelgauge_data *fuelgauge)
{
    int ret, mq=0, count=0;

    ret = sm5720_read_word(fuelgauge->i2c, SM5720_FG_REG_AUX_2);

    if((ret & START_MQ) == START_MQ)
    {
        pr_info("%s: AUX_2 : <0x%x> \n", __func__, ret);

        ret = START_MQ | DUMP_MQ;
        sm5720_write_word(fuelgauge->i2c, SM5720_FG_REG_AUX_2, (ret|TH_SOC_INV));
        msleep(50);
        ret = START_MQ;
        sm5720_write_word(fuelgauge->i2c, SM5720_FG_REG_AUX_2, (ret|TH_SOC_INV));

        for(count = 0; count < 5; count++)
        {
            ret = sm5720_read_word(fuelgauge->i2c, SM5720_FG_REG_MQ);
            if (ret == 0)
            {
                pr_info("%s: SM5720_FG_REG_MQ : <0x%x> retry %d\n", __func__, ret, count);
                msleep(50);
            }
            else
            {
                pr_info("%s: SM5720_FG_REG_MQ : <0x%x> OK count = %d\n", __func__, ret, count);
                break;
            }
        }

        if (ret<0) {
            mq = ((fuelgauge->info.cap * 1000) >> 11);
            pr_err("%s: sm5720_meas_mq_dump read fail!!!! return battery default value : %d\n", __func__, mq);
        } else {
            mq = ((ret&0x07FF) * 1000) >> 7;
            if(ret&0x8000) {
                mq *= -1;
            }
        }

        if(mq == 0) {
            mq = sm5720_meas_eq_dump(fuelgauge);
            pr_info("%s: mq is 0, It's abnormal, return eq : %d\n", __func__, mq);
        }
    }
    else
    {
        mq = ((fuelgauge->info.cap * 1000) >> 11);
        pr_info("%s: sm5720_meas_mq not started : return battery default value : %d, AUX_2 : <0x%x>\n", __func__, mq, ret);
    }

    return mq;
}

static void sm5720_meas_mq_off (struct sm5720_fuelgauge_data *fuelgauge)
{
    int last_mq;
    last_mq = sm5720_meas_mq_dump(fuelgauge);

    sm5720_write_word(fuelgauge->i2c, SM5720_FG_REG_START_MQ, 0);
    sm5720_write_word(fuelgauge->i2c, SM5720_FG_REG_AUX_2, TH_SOC_INV);

    pr_info("%s: mq off completed, last_mq : %d\n", __func__, last_mq);
}

static void sm5720_dp_setup (struct sm5720_fuelgauge_data *fuelgauge)
{
    sm5720_write_word(fuelgauge->i2c, SM5720_FG_REG_DP_ECV_I_OFF, fuelgauge->info.dp_ecv_i_off);
    sm5720_write_word(fuelgauge->i2c, SM5720_FG_REG_DP_CSP_I_OFF, fuelgauge->info.dp_csp_i_off);
    sm5720_write_word(fuelgauge->i2c, SM5720_FG_REG_DP_CSN_I_OFF, fuelgauge->info.dp_csn_i_off);

    sm5720_write_word(fuelgauge->i2c, SM5720_FG_REG_DP_ECV_I_SLO, fuelgauge->info.dp_ecv_i_slo);
    sm5720_write_word(fuelgauge->i2c, SM5720_FG_REG_DP_CSP_I_SLO, fuelgauge->info.dp_csp_i_slo);
    sm5720_write_word(fuelgauge->i2c, SM5720_FG_REG_DP_CSN_I_SLO, fuelgauge->info.dp_csn_i_slo);

	pr_info("%s: dp_off : <0x%x 0x%x 0x%x> dp_slo : <0x%x 0x%x 0x%x>\n",
		__func__,
		fuelgauge->info.dp_ecv_i_off, fuelgauge->info.dp_csp_i_off, fuelgauge->info.dp_csn_i_off,
		fuelgauge->info.dp_ecv_i_slo, fuelgauge->info.dp_csp_i_slo, fuelgauge->info.dp_csn_i_slo);
}

static void sm5720_alg_setup (struct sm5720_fuelgauge_data *fuelgauge)
{
    sm5720_write_word(fuelgauge->i2c, SM5720_FG_REG_ECV_I_OFF, fuelgauge->info.ecv_i_off);
    sm5720_write_word(fuelgauge->i2c, SM5720_FG_REG_CSP_I_OFF, fuelgauge->info.csp_i_off);
    sm5720_write_word(fuelgauge->i2c, SM5720_FG_REG_CSN_I_OFF, fuelgauge->info.csn_i_off);

    sm5720_write_word(fuelgauge->i2c, SM5720_FG_REG_ECV_I_SLO, fuelgauge->info.ecv_i_slo);
    sm5720_write_word(fuelgauge->i2c, SM5720_FG_REG_CSP_I_SLO, fuelgauge->info.csp_i_slo);
    sm5720_write_word(fuelgauge->i2c, SM5720_FG_REG_CSN_I_SLO, fuelgauge->info.csn_i_slo);

	pr_info("%s: alg_off : <0x%x 0x%x 0x%x> alg_slo : <0x%x 0x%x 0x%x>\n",
		__func__,
		fuelgauge->info.ecv_i_off, fuelgauge->info.csp_i_off, fuelgauge->info.csn_i_off,
		fuelgauge->info.ecv_i_slo, fuelgauge->info.csp_i_slo, fuelgauge->info.csn_i_slo);
}

static void sm5720_cal_carc (struct sm5720_fuelgauge_data *fuelgauge)
{
	int curr_cal = 0, p_curr_cal=0, n_curr_cal=0, p_delta_cal=0, n_delta_cal=0, p_fg_delta_cal=0, n_fg_delta_cal=0, temp_curr_offset=0;
	int temp_gap, fg_temp_gap, mix_factor=0;

	sm5720_vbatocv_check(fuelgauge);

	if(fuelgauge->is_charging || (fuelgauge->info.batt_current < LIMIT_N_CURR_MIXFACTOR))
	{
		mix_factor = fuelgauge->info.rs_value[1];
	}
	else
	{
		mix_factor = fuelgauge->info.rs_value[2];
	}
	sm5720_write_word(fuelgauge->i2c, SM5720_FG_REG_RS_MIX_FACTOR, mix_factor);

	fg_temp_gap = (fuelgauge->info.temp_fg/10) - fuelgauge->info.temp_std;


	temp_curr_offset = fuelgauge->info.ecv_i_off;
	if(fuelgauge->info.en_high_fg_temp_offset && (fg_temp_gap > 0))
	{
		if(temp_curr_offset & 0x0080)
		{
			temp_curr_offset = -(temp_curr_offset & 0x007F);
		}
		temp_curr_offset = temp_curr_offset + (fg_temp_gap / fuelgauge->info.high_fg_temp_offset_denom)*fuelgauge->info.high_fg_temp_offset_fact;
		if(temp_curr_offset < 0)
		{
			temp_curr_offset = -temp_curr_offset;
			temp_curr_offset = temp_curr_offset|0x0080;
		}
	}
	else if (fuelgauge->info.en_low_fg_temp_offset && (fg_temp_gap < 0))
	{
		if(temp_curr_offset & 0x0080)
		{
			temp_curr_offset = -(temp_curr_offset & 0x007F);
		}
		temp_curr_offset = temp_curr_offset + ((-fg_temp_gap) / fuelgauge->info.low_fg_temp_offset_denom)*fuelgauge->info.low_fg_temp_offset_fact;
		if(temp_curr_offset < 0)
		{
			temp_curr_offset = -temp_curr_offset;
			temp_curr_offset = temp_curr_offset|0x0080;
		}
	}
    temp_curr_offset = temp_curr_offset | (temp_curr_offset<<8);
	sm5720_write_word(fuelgauge->i2c, SM5720_FG_REG_ECV_I_OFF, temp_curr_offset);
	sm5720_write_word(fuelgauge->i2c, SM5720_FG_REG_CSP_I_OFF, temp_curr_offset);
	sm5720_write_word(fuelgauge->i2c, SM5720_FG_REG_CSN_I_OFF, temp_curr_offset);

	n_curr_cal = (fuelgauge->info.ecv_i_slo & 0xFF00)>>8;
	p_curr_cal = (fuelgauge->info.ecv_i_slo & 0x00FF);

	if (fuelgauge->info.en_high_fg_temp_cal && (fg_temp_gap > 0))
	{
		p_fg_delta_cal = (fg_temp_gap / fuelgauge->info.high_fg_temp_p_cal_denom)*fuelgauge->info.high_fg_temp_p_cal_fact;
		n_fg_delta_cal = (fg_temp_gap / fuelgauge->info.high_fg_temp_n_cal_denom)*fuelgauge->info.high_fg_temp_n_cal_fact;
	}
	else if (fuelgauge->info.en_low_fg_temp_cal && (fg_temp_gap < 0))
	{
		fg_temp_gap = -fg_temp_gap;
		p_fg_delta_cal = (fg_temp_gap / fuelgauge->info.low_fg_temp_p_cal_denom)*fuelgauge->info.low_fg_temp_p_cal_fact;
		n_fg_delta_cal = (fg_temp_gap / fuelgauge->info.low_fg_temp_n_cal_denom)*fuelgauge->info.low_fg_temp_n_cal_fact;
	}
	p_curr_cal = p_curr_cal + (p_fg_delta_cal);
	n_curr_cal = n_curr_cal + (n_fg_delta_cal);

	pr_info("%s: <%d %d %d %d %d %d %d %d %d %d>, temp_fg = %d ,p_curr_cal = 0x%x, n_curr_cal = 0x%x, "
		"batt_temp = %d\n",
		__func__,
		fuelgauge->info.en_high_fg_temp_cal,
		fuelgauge->info.high_fg_temp_p_cal_denom, fuelgauge->info.high_fg_temp_p_cal_fact,
		fuelgauge->info.high_fg_temp_n_cal_denom, fuelgauge->info.high_fg_temp_n_cal_fact,
		fuelgauge->info.en_low_fg_temp_cal,
		fuelgauge->info.low_fg_temp_p_cal_denom, fuelgauge->info.low_fg_temp_p_cal_fact,
		fuelgauge->info.low_fg_temp_n_cal_denom, fuelgauge->info.low_fg_temp_n_cal_fact,
		fuelgauge->info.temp_fg, p_curr_cal, n_curr_cal, fuelgauge->info.temperature);

	temp_gap = (fuelgauge->info.temperature/10) - fuelgauge->info.temp_std;
	if (fuelgauge->info.en_high_temp_cal && (temp_gap > 0))
	{
		p_delta_cal = (temp_gap / fuelgauge->info.high_temp_p_cal_denom)*fuelgauge->info.high_temp_p_cal_fact;
		n_delta_cal = (temp_gap / fuelgauge->info.high_temp_n_cal_denom)*fuelgauge->info.high_temp_n_cal_fact;
	}
	else if (fuelgauge->info.en_low_temp_cal && (temp_gap < 0))
	{
		temp_gap = -temp_gap;
		p_delta_cal = (temp_gap / fuelgauge->info.low_temp_p_cal_denom)*fuelgauge->info.low_temp_p_cal_fact;
		n_delta_cal = (temp_gap / fuelgauge->info.low_temp_n_cal_denom)*fuelgauge->info.low_temp_n_cal_fact;
	}
	p_curr_cal = p_curr_cal + (p_delta_cal);
	n_curr_cal = n_curr_cal + (n_delta_cal);

    curr_cal = (n_curr_cal << 8) | p_curr_cal;

	sm5720_write_word(fuelgauge->i2c, SM5720_FG_REG_ECV_I_SLO, curr_cal);
	sm5720_write_word(fuelgauge->i2c, SM5720_FG_REG_CSP_I_SLO, curr_cal);
	sm5720_write_word(fuelgauge->i2c, SM5720_FG_REG_CSN_I_SLO, curr_cal);

	pr_info("%s: <%d %d %d %d %d %d %d %d %d %d>, "
		"p_curr_cal = 0x%x, n_curr_cal = 0x%x, mix_factor=0x%x ,curr_cal = 0x%x\n",
		__func__,
		fuelgauge->info.en_high_temp_cal,
		fuelgauge->info.high_temp_p_cal_denom, fuelgauge->info.high_temp_p_cal_fact,
		fuelgauge->info.high_temp_n_cal_denom, fuelgauge->info.high_temp_n_cal_fact,
		fuelgauge->info.en_low_temp_cal,
		fuelgauge->info.low_temp_p_cal_denom, fuelgauge->info.low_temp_p_cal_fact,
		fuelgauge->info.low_temp_n_cal_denom, fuelgauge->info.low_temp_n_cal_fact,
		p_curr_cal, n_curr_cal, mix_factor, curr_cal);

	return;
}

static int sm5720_fg_verified_write_word(struct i2c_client *client,
		u8 reg_addr, u16 data)
{
	int ret;

	ret = sm5720_write_word(client, reg_addr, data);
	if(ret<0)
	{
		msleep(50);
		pr_info("1st fail i2c write %s: ret = %d, addr = 0x%x, data = 0x%x\n",
				__func__, ret, reg_addr, data);
		ret = sm5720_write_word(client, reg_addr, data);
		if(ret<0)
		{
			msleep(50);
			pr_info("2nd fail i2c write %s: ret = %d, addr = 0x%x, data = 0x%x\n",
					__func__, ret, reg_addr, data);
			ret = sm5720_write_word(client, reg_addr, data);
			if(ret<0)
			{
				pr_info("3rd fail i2c write %s: ret = %d, addr = 0x%x, data = 0x%x\n",
						__func__, ret, reg_addr, data);
			}
		}
	}

	return ret;
}

static int sm5720_fg_fs_read_word_table(struct i2c_client *client,
		u8 reg_addr, u8 count)
{
	int ret, i;

    for(i=0; i<count; i++)
    {
        ret = sm5720_read_word(client, reg_addr);
        if(ret<0)
        {
            pr_err("%s: 1st fail i2c write ret = %d, addr = 0x%x\n, count = %d",
                __func__, ret, reg_addr, count);
        }
        else
        {
            if(ret == 0xffff)
                ret = sm5720_read_word(client, reg_addr);
            else
                break;
        }
    }
	return ret;
}



int sm5720_fg_calculate_iocv(struct sm5720_fuelgauge_data *fuelgauge)
{
	bool only_lb = false, sign_i_offset = 0; //valid_cb=false,
	int roop_start = 0, roop_max = 0, i = 0, cb_last_index = 0, cb_pre_last_index = 0;
	int lb_v_buffer[6] = {0, 0, 0, 0, 0, 0};
	int lb_i_buffer[6] = {0, 0, 0, 0, 0, 0};
	int cb_v_buffer[6] = {0, 0, 0, 0, 0, 0};
	int cb_i_buffer[6] = {0, 0, 0, 0, 0, 0};
	int i_offset_margin = 0x14, i_vset_margin = 0x67;
	int v_max = 0, v_min = 0, v_sum = 0,
		lb_v_avg = 0, cb_v_avg = 0, lb_v_set = 0, lb_i_set = 0, i_offset = 0; //lb_v_minmax_offset=0,
	int i_max = 0, i_min = 0, i_sum = 0,
		lb_i_avg = 0, cb_i_avg = 0, cb_v_set = 0, cb_i_set = 0; //lb_i_minmax_offset=0,
	int lb_i_p_v_min = 0, lb_i_n_v_max = 0, cb_i_p_v_min = 0, cb_i_n_v_max = 0;

	int v_ret = 0, i_ret = 0, ret = 0;

	ret = sm5720_read_word(fuelgauge->i2c, SM5720_FG_REG_END_V_IDX);
	pr_info("%s: iocv_status_read = addr : 0x%x , data : 0x%x\n", __func__, SM5720_FG_REG_END_V_IDX, ret);

	// init start
	if((ret & 0x0010) == 0x0000)
	{
		only_lb = true;
	}

    /*
	if((ret & 0x0300) == 0x0300)
	{
		valid_cb = true;
	}
	*/
	// init end

	// lb get start
	roop_max = (ret & 0x000F);
	if(roop_max > 6)
		roop_max = 6;

	roop_start = SM5720_FG_REG_START_LB_V;
	for (i = roop_start; i < roop_start + roop_max; i++)
	{
		v_ret = sm5720_read_word(fuelgauge->i2c, i);
		i_ret = sm5720_read_word(fuelgauge->i2c, i+0x10);

		if((i_ret&0x4000) == 0x4000)
		{
			i_ret = -(i_ret&0x3FFF);
		}

		lb_v_buffer[i-roop_start] = v_ret;
		lb_i_buffer[i-roop_start] = i_ret;

		if (i == roop_start)
		{
			v_max = v_ret;
			v_min = v_ret;
			v_sum = v_ret;
			i_max = i_ret;
			i_min = i_ret;
			i_sum = i_ret;
		}
		else
		{
			if(v_ret > v_max)
				v_max = v_ret;
			else if(v_ret < v_min)
				v_min = v_ret;
			v_sum = v_sum + v_ret;

			if(i_ret > i_max)
				i_max = i_ret;
			else if(i_ret < i_min)
				i_min = i_ret;
			i_sum = i_sum + i_ret;
		}

		if(abs(i_ret) > i_vset_margin)
		{
			if(i_ret > 0)
			{
				if(lb_i_p_v_min == 0)
				{
					lb_i_p_v_min = v_ret;
				}
				else
				{
					if(v_ret < lb_i_p_v_min)
						lb_i_p_v_min = v_ret;
				}
			}
			else
			{
				if(lb_i_n_v_max == 0)
				{
					lb_i_n_v_max = v_ret;
				}
				else
				{
					if(v_ret > lb_i_n_v_max)
						lb_i_n_v_max = v_ret;
				}
			}
		}
	}
	v_sum = v_sum - v_max - v_min;
	i_sum = i_sum - i_max - i_min;

    /*
	lb_v_minmax_offset = v_max - v_min;
	lb_i_minmax_offset = i_max - i_min;
	*/

	lb_v_avg = v_sum / (roop_max-2);
	lb_i_avg = i_sum / (roop_max-2);
	// lb get end

	// lb_vset start
	if(abs(lb_i_buffer[roop_max-1]) < i_vset_margin)
	{
		if(abs(lb_i_buffer[roop_max-2]) < i_vset_margin)
		{
			lb_v_set = MAXVAL(lb_v_buffer[roop_max-2], lb_v_buffer[roop_max-1]);
			if(abs(lb_i_buffer[roop_max-3]) < i_vset_margin)
			{
				lb_v_set = MAXVAL(lb_v_buffer[roop_max-3], lb_v_set);
			}
		}
		else
		{
			lb_v_set = lb_v_buffer[roop_max-1];
		}
	}
	else
	{
		lb_v_set = lb_v_avg;
	}

	if(lb_i_n_v_max > 0)
	{
		lb_v_set = MAXVAL(lb_i_n_v_max, lb_v_set);
	}
	//else if(lb_i_p_v_min > 0)
	//{
	//	lb_v_set = MINVAL(lb_i_p_v_min, lb_v_set);
	//}
	// lb_vset end

	// lb offset make start
	if(roop_max > 3)
	{
		lb_i_set = (lb_i_buffer[2] + lb_i_buffer[3]) / 2;
	}

	if((abs(lb_i_buffer[roop_max-1]) < i_offset_margin) && (abs(lb_i_set) < i_offset_margin))
	{
		lb_i_set = MAXVAL(lb_i_buffer[roop_max-1], lb_i_set);
	}
	else if(abs(lb_i_buffer[roop_max-1]) < i_offset_margin)
	{
		lb_i_set = lb_i_buffer[roop_max-1];
	}
	else if(abs(lb_i_set) >= i_offset_margin)
	{
		lb_i_set = 0;
	}

	i_offset = lb_i_set;

	i_offset = i_offset + 4;	// add extra offset

	if(i_offset <= 0)
	{
		sign_i_offset = 1;
#if defined(IGNORE_N_I_OFFSET)
		i_offset = 0;
#else
		i_offset = -i_offset;
#endif
	}

	i_offset = i_offset>>1;

	if(sign_i_offset == 0)
	{
		i_offset = i_offset|0x0080;
	}
    i_offset = i_offset | i_offset<<8;

	//do not write in kernel point.
	//sm5720_write_word(client, SM5720_FG_REG_DP_ECV_I_OFF, i_offset);
	// lb offset make end

	pr_info("%s: iocv_l_max=0x%x, iocv_l_min=0x%x, iocv_l_avg=0x%x, lb_v_set=0x%x, roop_max=%d \n",
			__func__, v_max, v_min, lb_v_avg, lb_v_set, roop_max);
	pr_info("%s: ioci_l_max=0x%x, ioci_l_min=0x%x, ioci_l_avg=0x%x, lb_i_set=0x%x, i_offset=0x%x, sign_i_offset=%d\n",
			__func__, i_max, i_min, lb_i_avg, lb_i_set, i_offset, sign_i_offset);

	if(!only_lb)
	{
		// cb get start
		roop_start = SM5720_FG_REG_START_CB_V;
		roop_max = 6;
		for (i = roop_start; i < roop_start + roop_max; i++)
		{
			v_ret = sm5720_read_word(fuelgauge->i2c, i);
			i_ret = sm5720_read_word(fuelgauge->i2c, i+0x10);
			if((i_ret&0x4000) == 0x4000)
			{
				i_ret = -(i_ret&0x3FFF);
			}

			cb_v_buffer[i-roop_start] = v_ret;
			cb_i_buffer[i-roop_start] = i_ret;

			if (i == roop_start)
			{
				v_max = v_ret;
				v_min = v_ret;
				v_sum = v_ret;
				i_max = i_ret;
				i_min = i_ret;
				i_sum = i_ret;
			}
			else
			{
				if(v_ret > v_max)
					v_max = v_ret;
				else if(v_ret < v_min)
					v_min = v_ret;
				v_sum = v_sum + v_ret;

				if(i_ret > i_max)
					i_max = i_ret;
				else if(i_ret < i_min)
					i_min = i_ret;
				i_sum = i_sum + i_ret;
			}

			if(abs(i_ret) > i_vset_margin)
			{
				if(i_ret > 0)
				{
					if(cb_i_p_v_min == 0)
					{
						cb_i_p_v_min = v_ret;
					}
					else
					{
						if(v_ret < cb_i_p_v_min)
							cb_i_p_v_min = v_ret;
					}
				}
				else
				{
					if(cb_i_n_v_max == 0)
					{
						cb_i_n_v_max = v_ret;
					}
					else
					{
						if(v_ret > cb_i_n_v_max)
							cb_i_n_v_max = v_ret;
					}
				}
			}
		}
		v_sum = v_sum - v_max - v_min;
		i_sum = i_sum - i_max - i_min;

		cb_v_avg = v_sum / (roop_max-2);
		cb_i_avg = i_sum / (roop_max-2);
		// cb get end

		// cb_vset start
		cb_last_index = (ret & 0x000F)-7; //-6-1
		if(cb_last_index < 0)
		{
			cb_last_index = 5;
		}

		for (i = roop_max; i > 0; i--)
		{
			if(abs(cb_i_buffer[cb_last_index]) < i_vset_margin)
			{
				cb_v_set = cb_v_buffer[cb_last_index];
				if(abs(cb_i_buffer[cb_last_index]) < i_offset_margin)
				{
					cb_i_set = cb_i_buffer[cb_last_index];
				}

				cb_pre_last_index = cb_last_index - 1;
				if(cb_pre_last_index < 0)
				{
					cb_pre_last_index = 5;
				}

				if(abs(cb_i_buffer[cb_pre_last_index]) < i_vset_margin)
				{
					cb_v_set = MAXVAL(cb_v_buffer[cb_pre_last_index], cb_v_set);
					if(abs(cb_i_buffer[cb_pre_last_index]) < i_offset_margin)
					{
						cb_i_set = MAXVAL(cb_i_buffer[cb_pre_last_index], cb_i_set);
					}
				}
			}
			else
			{
				cb_last_index--;
				if(cb_last_index < 0)
				{
					cb_last_index = 5;
				}
			}
		}

		if(cb_v_set == 0)
		{
			cb_v_set = cb_v_avg;
			if(cb_i_set == 0)
			{
				cb_i_set = cb_i_avg;
			}
		}

		if(cb_i_n_v_max > 0)
		{
			cb_v_set = MAXVAL(cb_i_n_v_max, cb_v_set);
		}
		//else if(cb_i_p_v_min > 0)
		//{
		//	cb_v_set = MINVAL(cb_i_p_v_min, cb_v_set);
		//}
		// cb_vset end

		// cb offset make start
		if(abs(cb_i_set) < i_offset_margin)
		{
			if(cb_i_set > lb_i_set)
			{
				i_offset = cb_i_set;
				i_offset = i_offset + 4;	// add extra offset

				if(i_offset <= 0)
				{
					sign_i_offset = 1;
#if defined(IGNORE_N_I_OFFSET)
					i_offset = 0;
#else
					i_offset = -i_offset;
#endif
				}

				i_offset = i_offset>>1;

				if(sign_i_offset == 0)
				{
					i_offset = i_offset|0x0080;
				}
                i_offset = i_offset | i_offset<<8;

				//do not write in kernel point.
				//sm5720_write_word(fuelgauge->i2c, SM5720_FG_REG_DP_ECV_I_OFF, i_offset);
			}
		}
		// cb offset make end

		pr_info("%s: iocv_c_max=0x%x, iocv_c_min=0x%x, iocv_c_avg=0x%x, cb_v_set=0x%x, cb_last_index=%d \n",
				__func__, v_max, v_min, cb_v_avg, cb_v_set, cb_last_index);
		pr_info("%s: ioci_c_max=0x%x, ioci_c_min=0x%x, ioci_c_avg=0x%x, cb_i_set=0x%x, i_offset=0x%x, sign_i_offset=%d\n",
				__func__, i_max, i_min, cb_i_avg, cb_i_set, i_offset, sign_i_offset);

	}

	// final set
	if((abs(cb_i_set) > i_vset_margin) || only_lb)
	{
		ret = MAXVAL(lb_v_set, cb_i_n_v_max);
	}
	else
	{
		ret = cb_v_set;
	}

	if(ret > fuelgauge->info.battery_table[DISCHARGE_TABLE][FG_TABLE_LEN-1])
	{
		pr_info("%s: iocv ret change 0x%x -> 0x%x \n", __func__, ret, fuelgauge->info.battery_table[DISCHARGE_TABLE][FG_TABLE_LEN-1]);
		ret = fuelgauge->info.battery_table[DISCHARGE_TABLE][FG_TABLE_LEN-1];
	}
	else if(ret < fuelgauge->info.battery_table[DISCHARGE_TABLE][0])
	{
		pr_info("%s: iocv ret change 0x%x -> 0x%x \n", __func__, ret, (fuelgauge->info.battery_table[DISCHARGE_TABLE][0] + 0x10));
		ret = fuelgauge->info.battery_table[DISCHARGE_TABLE][0] + 0x10;
	}

	return ret;
}

void sm5720_set_cycle_cfg(struct sm5720_fuelgauge_data *fuelgauge)
{
	int value;

	value = fuelgauge->info.cycle_limit_cntl|(fuelgauge->info.cycle_high_limit<<12)|(fuelgauge->info.cycle_low_limit<<8);

	sm5720_write_word(fuelgauge->i2c, SM5720_FG_REG_CYCLE_CFG, value);

	pr_info("%s: cycle cfg value = 0x%x\n", __func__, value);

}

void sm5720_set_arsm_cfg(struct sm5720_fuelgauge_data *fuelgauge)
{
	int value;

    value = fuelgauge->info.arsm[0]<<15 | fuelgauge->info.arsm[1]<<6 | fuelgauge->info.arsm[2]<<4 | fuelgauge->info.arsm[3];

	sm5720_write_word(fuelgauge->i2c, SM5720_FG_REG_AUTO_RS_MAN, value);

    pr_info("%s: arsm cfg value = 0x%x\n", __func__, value);
}

#if defined(CONFIG_BATTERY_AGE_FORECAST)
int get_v_max_index_by_cycle(struct sm5720_fuelgauge_data *fuelgauge)
{
	int cycle_index=0, len;

	for (len = fuelgauge->pdata->num_age_step-1; len >= 0; --len) {
		if(fuelgauge->chg_full_soc == fuelgauge->pdata->age_data[len].full_condition_soc) {
			cycle_index=len;
			break;
		}
	}
	pr_info("%s: chg_full_soc = %d, index = %d \n", __func__, fuelgauge->chg_full_soc, cycle_index);

	return cycle_index;
}
#endif

static bool sm5720_fg_reg_init(struct sm5720_fuelgauge_data *fuelgauge, bool is_surge)
{
	int i, j, value, ret;
	uint8_t table_reg;

	pr_info("%s: sm5720_fg_reg_init START!!\n", __func__);

	// init mark
	sm5720_write_word(fuelgauge->i2c, SM5720_FG_REG_RESET, FG_INIT_MARK);

	// start first param_ctrl unlock
	sm5720_write_word(fuelgauge->i2c, SM5720_FG_REG_PARAM_CTRL, FG_PARAM_UNLOCK_CODE);

	// RCE write
	for (i = 0; i < 3; i++)
	{
		sm5720_write_word(fuelgauge->i2c, SM5720_FG_REG_RCE0+i, fuelgauge->info.rce_value[i]);
		pr_info("%s: RCE write RCE%d = 0x%x : 0x%x\n",
				__func__,  i, SM5720_FG_REG_RCE0+i, fuelgauge->info.rce_value[i]);
	}

	// DTCD write
	sm5720_write_word(fuelgauge->i2c, SM5720_FG_REG_DTCD, fuelgauge->info.dtcd_value);
	pr_info("%s: DTCD write DTCD = 0x%x : 0x%x\n",
			__func__, SM5720_FG_REG_DTCD, fuelgauge->info.dtcd_value);

	// RS write
	sm5720_write_word(fuelgauge->i2c, SM5720_FG_REG_RS_MAN, fuelgauge->info.rs_value[0]);
	pr_info("%s: RS write RS = 0x%x : 0x%x\n",
			__func__, SM5720_FG_REG_AUTO_RS_MAN, fuelgauge->info.rs_value[0]);

	// VIT_PERIOD write
	sm5720_write_word(fuelgauge->i2c, SM5720_FG_REG_VIT_PERIOD, fuelgauge->info.vit_period);
	pr_info("%s: VIT_PERIOD write VIT_PERIOD = 0x%x : 0x%x\n",
			__func__, SM5720_FG_REG_VIT_PERIOD, fuelgauge->info.vit_period);

	// TABLE_LEN write & pram unlock
	sm5720_write_word(fuelgauge->i2c, SM5720_FG_REG_PARAM_CTRL,
					FG_PARAM_UNLOCK_CODE | FG_TABLE_LEN);

	for (i=0; i < TABLE_MAX; i++)
	{
		table_reg = SM5720_FG_REG_TABLE_START + (i<<4);
		for(j=0; j <= FG_TABLE_LEN; j++)
		{
			sm5720_write_word(fuelgauge->i2c, (table_reg + j), fuelgauge->info.battery_table[i][j]);
			msleep(10);
			if(fuelgauge->info.battery_table[i][j] != sm5720_fg_fs_read_word_table(fuelgauge->i2c, (table_reg + j), TABLE_READ_COUNT))
			{
				pr_info("%s: TABLE write FAIL retry[%d][%d] = 0x%x : 0x%x\n",
					__func__, i, j, (table_reg + j), fuelgauge->info.battery_table[i][j]);
				sm5720_write_word(fuelgauge->i2c, (table_reg + j), fuelgauge->info.battery_table[i][j]);
			}
			pr_info("%s: TABLE write OK [%d][%d] = 0x%x : 0x%x\n",
				__func__, i, j, (table_reg + j), fuelgauge->info.battery_table[i][j]);
		}
	}

	// MIX_MODE write
	sm5720_write_word(fuelgauge->i2c, SM5720_FG_REG_RS_MIX_FACTOR, fuelgauge->info.rs_value[2]);
	sm5720_write_word(fuelgauge->i2c, SM5720_FG_REG_RS_MAX, fuelgauge->info.rs_value[3]);
	sm5720_write_word(fuelgauge->i2c, SM5720_FG_REG_RS_MIN, fuelgauge->info.rs_value[4]);
	sm5720_write_word(fuelgauge->i2c, SM5720_FG_REG_MIX_RATE, fuelgauge->info.mix_value[0]);
	sm5720_write_word(fuelgauge->i2c, SM5720_FG_REG_MIX_INIT_BLANK, fuelgauge->info.mix_value[1]);

	pr_info("%s: RS_MIX_FACTOR = 0x%x, RS_MAX = 0x%x, RS_MIN = 0x%x, MIX_RATE = 0x%x, MIX_INIT_BLANK = 0x%x\n",
		__func__,
		fuelgauge->info.rs_value[2], fuelgauge->info.rs_value[3], fuelgauge->info.rs_value[4],
		fuelgauge->info.mix_value[0], fuelgauge->info.mix_value[1]);

    /*
	// v cal write
	sm5720_write_word(fuelgauge->i2c, SM5720_FG_REG_VOLT_CAL, fuelgauge->info.volt_cal);

	// i ecv write
	sm5720_write_word(fuelgauge->i2c, SM5720_FG_REG_ECV_I_OFF, fuelgauge->info.ecv_i_off);
	sm5720_write_word(fuelgauge->i2c, SM5720_FG_REG_ECV_I_SLO, fuelgauge->info.ecv_i_slo);
	sm5720_write_word(fuelgauge->i2c, SM5720_FG_REG_DP_ECV_I_OFF, fuelgauge->info.dp_ecv_i_off);
	sm5720_write_word(fuelgauge->i2c, SM5720_FG_REG_DP_ECV_I_SLO, fuelgauge->info.dp_ecv_i_slo);

	// i csp write
	sm5720_write_word(fuelgauge->i2c, SM5720_FG_REG_CSP_I_OFF, fuelgauge->info.csp_i_off);
	sm5720_write_word(fuelgauge->i2c, SM5720_FG_REG_CSP_I_SLO, fuelgauge->info.csp_i_slo);
	sm5720_write_word(fuelgauge->i2c, SM5720_FG_REG_DP_CSP_I_OFF, fuelgauge->info.dp_csp_i_off);
	sm5720_write_word(fuelgauge->i2c, SM5720_FG_REG_DP_CSP_I_SLO, fuelgauge->info.dp_csp_i_slo);

	// i csn write
	sm5720_write_word(fuelgauge->i2c, SM5720_FG_REG_CSN_I_OFF, fuelgauge->info.csn_i_off);
	sm5720_write_word(fuelgauge->i2c, SM5720_FG_REG_CSN_I_SLO, fuelgauge->info.csn_i_slo);
	sm5720_write_word(fuelgauge->i2c, SM5720_FG_REG_DP_CSN_I_OFF, fuelgauge->info.dp_csn_i_off);
	sm5720_write_word(fuelgauge->i2c, SM5720_FG_REG_DP_CSN_I_SLO, fuelgauge->info.dp_csn_i_slo);
	*/

    // need writing value print for debug

    // CAP write
	sm5720_write_word(fuelgauge->i2c, SM5720_FG_REG_MIN_CAP, fuelgauge->info.min_cap);
#if defined(CONFIG_BATTERY_AGE_FORECAST)
	i = get_v_max_index_by_cycle(fuelgauge);
	pr_info("%s: v_max_now is change %x -> %x \n", __func__, fuelgauge->info.v_max_now, fuelgauge->info.v_max_table[i]);
	pr_info("%s: q_max_now is change %x -> %x \n", __func__, fuelgauge->info.q_max_now, fuelgauge->info.q_max_table[i]);
	fuelgauge->info.v_max_now = fuelgauge->info.v_max_table[i];
	fuelgauge->info.q_max_now = fuelgauge->info.q_max_table[i];
	fuelgauge->info.cap = fuelgauge->info.q_max_now;
#endif

	sm5720_write_word(fuelgauge->i2c, SM5720_FG_REG_CAP, fuelgauge->info.cap);
	pr_info("%s: SM5720_REG_CAP 0x%x, 0x%x\n",
		__func__, fuelgauge->info.min_cap, fuelgauge->info.cap);

	// MISC write
	sm5720_write_word(fuelgauge->i2c, SM5720_FG_REG_MISC, fuelgauge->info.misc);
	pr_info("%s: SM5720_REG_MISC 0x%x : 0x%x\n",
		__func__, SM5720_FG_REG_MISC, fuelgauge->info.misc);

	// TOPOFF SOC
	sm5720_write_word(fuelgauge->i2c, SM5720_FG_REG_TOPOFFSOC, fuelgauge->info.topoff_soc);
	pr_info("%s: SM5720_REG_TOPOFFSOC 0x%x : 0x%x\n", __func__,
		SM5720_FG_REG_TOPOFFSOC, fuelgauge->info.topoff_soc);

	// INIT_last -  control register set
	value = sm5720_read_word(fuelgauge->i2c, SM5720_FG_REG_CNTL);
	if(value == CNTL_REG_DEFAULT_VALUE)
	{
		value = fuelgauge->info.cntl_value;
	}
	value = ENABLE_MIX_MODE | ENABLE_TEMP_MEASURE | ENABLE_MANUAL_OCV | (fuelgauge->info.enable_topoff_soc << 13);
	pr_info("%s: SM5720_REG_CNTL reg : 0x%x\n", __func__, value);

	ret = sm5720_write_word(fuelgauge->i2c, SM5720_FG_REG_CNTL, value);
	if (ret < 0)
		pr_info("%s: fail control register set(%d)\n", __func__, ret);

	pr_info("%s: LAST SM5720_REG_CNTL = 0x%x : 0x%x\n", __func__, SM5720_FG_REG_CNTL, value);

	// LOCK
	value = FG_PARAM_LOCK_CODE | FG_TABLE_LEN;
	sm5720_write_word(fuelgauge->i2c, SM5720_FG_REG_PARAM_CTRL, value);
	pr_info("%s: LAST PARAM CTRL VALUE = 0x%x : 0x%x\n", __func__, SM5720_FG_REG_PARAM_CTRL, value);

	// surge reset defence
	if(is_surge)
	{
		value = ((fuelgauge->info.batt_ocv<<8)/125);
	}
	else
	{
		value = sm5720_fg_calculate_iocv(fuelgauge);

		if((fuelgauge->info.volt_cal & 0x0080) == 0x0080)
		{
			value = value - (fuelgauge->info.volt_cal & 0x007F);
		}
		else
		{
			value = value + (fuelgauge->info.volt_cal & 0x007F);
		}
	}

	msleep(10);
	sm5720_write_word(fuelgauge->i2c, SM5720_FG_REG_IOCV_MAN, value);
	pr_info("%s: IOCV_MAN_WRITE = %d : 0x%x\n", __func__, SM5720_FG_REG_IOCV_MAN, value);

	// init delay
	msleep(20);

    // write cycle cfg
    sm5720_set_cycle_cfg(fuelgauge);
    // write auto_rs_man cfg
    sm5720_set_arsm_cfg(fuelgauge);

	// write batt data version
	value = (fuelgauge->info.data_ver << 4) & DATA_VERSION;
	sm5720_write_word(fuelgauge->i2c, SM5720_FG_REG_RESERVED, value);
	pr_info("%s: RESERVED = %d : 0x%x\n", __func__, SM5720_FG_REG_RESERVED, value);

    sm5720_dp_setup(fuelgauge);
    sm5720_alg_setup(fuelgauge);

	return 1;
}

static int sm5720_abnormal_reset_check(struct sm5720_fuelgauge_data *fuelgauge)
{
	int cntl_read, reset_read;

	reset_read = sm5720_read_word(fuelgauge->i2c, SM5720_FG_REG_RESET) & 0xF000;
	// abnormal case process
	if(sm5720_fg_check_reg_init_need(fuelgauge) || (reset_read == 0))
	{
		cntl_read = sm5720_read_word(fuelgauge->i2c, SM5720_FG_REG_CNTL);
		pr_info("%s: SM5720 FG abnormal case!!!! SM5720_REG_CNTL : 0x%x, is_FG_initialised : %d, reset_read : 0x%x\n", __func__, cntl_read, fuelgauge->info.is_FG_initialised, reset_read);

        if(fuelgauge->info.is_FG_initialised == 1)
        {
            // SW reset code
            if(sm5720_fg_verified_write_word(fuelgauge->i2c, SM5720_FG_REG_RESET, SW_RESET_OTP_CODE) < 0)
            {
                pr_info("%s: Warning!!!! SM5720 FG abnormal case.... SW reset FAIL \n", __func__);
            }
            else
            {
                pr_info("%s: SM5720 FG abnormal case.... SW reset OK\n", __func__);
            }
            // delay 100ms
            msleep(100);

            sm5720_meas_mq_off(fuelgauge);

            // init code
            sm5720_fg_init(fuelgauge, true);
        }
        return FG_ABNORMAL_RESET;
	}
	return 0;
}

static int sm5720_get_device_id(struct sm5720_fuelgauge_data *fuelgauge)
{
	int ret;
	ret = sm5720_read_word(fuelgauge->i2c, SM5720_FG_REG_DEVICE_ID);
	sm5720_device_id = ret;
	pr_info("%s: SM5720 device_id = 0x%x\n", __func__, ret);

	return ret;
}

int sm5720_call_fg_device_id(void)
{
	pr_info("%s: extern call SM5720 fg_device_id = 0x%x\n", __func__, sm5720_device_id);

	return sm5720_device_id;
}

unsigned int sm5720_get_soc(struct sm5720_fuelgauge_data *fuelgauge)
{
	int ret;
	unsigned int soc;

	ret = sm5720_read_word(fuelgauge->i2c, SM5720_FG_REG_SOC);
	if (ret<0) {
		pr_err("%s: Warning!!!! read soc reg fail\n", __func__);
		soc = 500;
	} else {
		soc = ((ret&0x7f00)>>8) * 10; //integer bit;
		soc = soc + (((ret&0x00ff)*10)/256); // integer + fractional bit
	}

    if(ret&0x8000)
    {
        soc = 0;
    }

	if (sm5720_abnormal_reset_check(fuelgauge) < 0)
	{
		pr_info("%s: FG init ERROR!! pre_SOC returned!!, read_SOC = %d, pre_SOC = %d\n", __func__, soc, fuelgauge->info.batt_soc);
		return fuelgauge->info.batt_soc;
	}

#if defined(SM5720_FG_FULL_DEBUG)
	pr_info("%s: read = 0x%x, soc = %d\n", __func__, ret, soc);
#endif

	// for low temp power off test
	if(fuelgauge->info.volt_alert_flag && (fuelgauge->info.temperature < -100))
	{
		pr_info("%s: volt_alert_flag is TRUE!!!! SOC make force ZERO!!!!\n", __func__);
		fuelgauge->info.batt_soc = 0;
		return 0;
	}
	else
	{
		fuelgauge->info.batt_soc = soc;
	}

	return soc;
}

static void sm5720_fg_test_read(struct sm5720_fuelgauge_data *fuelgauge)
{
	int ret0, ret1, ret2, ret3, ret4, ret5, ret6, ret7, ret8, ret9;

	ret0 = sm5720_read_word(fuelgauge->i2c, 0xA0);
	ret1 = sm5720_read_word(fuelgauge->i2c, 0xAC);
	ret2 = sm5720_read_word(fuelgauge->i2c, 0xAD);
	ret3 = sm5720_read_word(fuelgauge->i2c, 0xAE);
	ret4 = sm5720_read_word(fuelgauge->i2c, 0xAF);
	ret5 = sm5720_read_word(fuelgauge->i2c, 0x28);
	ret6 = sm5720_read_word(fuelgauge->i2c, 0x2F);
	ret7 = sm5720_read_word(fuelgauge->i2c, 0x01);
	pr_info("%s: 0xA0=0x%04x, 0xAC=0x%04x, 0xAD=0x%04x, 0xAE=0x%04x, 0xAF=0x%04x, 0x28=0x%04x, 0x2F=0x%04x, 0x01=0x%04x, SM5720_ID=0x%04x\n",
		__func__, ret0, ret1, ret2, ret3, ret4, ret5, ret6, ret7, sm5720_device_id);

	ret0 = sm5720_read_word(fuelgauge->i2c, 0xB0);
	ret1 = sm5720_read_word(fuelgauge->i2c, 0xBC);
	ret2 = sm5720_read_word(fuelgauge->i2c, 0xBD);
	ret3 = sm5720_read_word(fuelgauge->i2c, 0xBE);
	ret4 = sm5720_read_word(fuelgauge->i2c, 0xBF);
	ret5 = sm5720_read_word(fuelgauge->i2c, 0x85);
	ret6 = sm5720_read_word(fuelgauge->i2c, 0x86);
	ret7 = sm5720_read_word(fuelgauge->i2c, 0x87);
	ret8 = sm5720_read_word(fuelgauge->i2c, 0x1F);
	ret9 = sm5720_read_word(fuelgauge->i2c, 0x94);
	pr_info("%s: 0xB0=0x%04x, 0xBC=0x%04x, 0xBD=0x%04x, 0xBE=0x%04x, 0xBF=0x%04x, 0x85=0x%04x, 0x86=0x%04x, 0x87=0x%04x, 0x1F=0x%04x, 0x94=0x%04x\n",
		__func__, ret0, ret1, ret2, ret3, ret4, ret5, ret6, ret7, ret8, ret9);

#if !defined(CONFIG_SEC_FACTORY)
	sm5720_fg_periodic_read(fuelgauge);
#endif

	return;
}

static void sm5720_update_all_value(struct sm5720_fuelgauge_data *fuelgauge)
{
	union power_supply_propval value;
	int current_mq = 0;

	fuelgauge->is_charging = (fuelgauge->info.flag_charge_health |
		fuelgauge->ta_exist) && (fuelgauge->info.batt_current>=30);

	// check charger status
	psy_do_property("sm5720-charger", get,
			POWER_SUPPLY_PROP_STATUS, value);
	fuelgauge->info.flag_full_charge =
		(value.intval == POWER_SUPPLY_STATUS_FULL) ? 1 : 0;
	fuelgauge->info.flag_chg_status =
		(value.intval == POWER_SUPPLY_STATUS_CHARGING) ? 1 : 0;

	//vbat
	sm5720_get_vbat(fuelgauge);
	//current
	sm5720_get_curr(fuelgauge);
	//ocv
	sm5720_get_ocv(fuelgauge);
	//temperature
	sm5720_get_temperature(fuelgauge);
	//cycle
	sm5720_get_cycle(fuelgauge);
	//carc
	sm5720_cal_carc(fuelgauge);
	//soc
	sm5720_get_soc(fuelgauge);

	pr_info("%s: chg_h=%d, chg_f=%d, chg_s=%d, is_chg=%d, ta_exist=%d, "
		"v=%d, v_avg=%d, i=%d, i_avg=%d, ocv=%d, fg_t=%d, b_t=%d, cycle=%d, soc=%d, state=0x%x, capacity_max=%d\n",
		__func__, fuelgauge->info.flag_charge_health, fuelgauge->info.flag_full_charge,
		fuelgauge->info.flag_chg_status, fuelgauge->is_charging, fuelgauge->ta_exist,
		fuelgauge->info.batt_voltage, fuelgauge->info.batt_avgvoltage,
		fuelgauge->info.batt_current, fuelgauge->info.batt_avgcurrent, fuelgauge->info.batt_ocv,
		fuelgauge->info.temp_fg, fuelgauge->info.temperature, fuelgauge->info.batt_soc_cycle,
		fuelgauge->info.batt_soc, sm5720_read_word(fuelgauge->i2c, SM5720_FG_REG_OCV_STATE), fuelgauge->capacity_max);

	current_mq = sm5720_meas_mq_dump(fuelgauge);

	if (current_mq > fuelgauge->info.full_mq_dump) {
		sm5720_set_full_chg_mq(fuelgauge, current_mq);
		fuelgauge->info.full_mq_dump = current_mq;
	}

    return;
}

int sm5720_fg_get_jig_mode_real_vbat(struct sm5720_fuelgauge_data *fuelgauge, int meas_mode)
{
    int cntl, ret;

    if(sm5720_fg_check_reg_init_need(fuelgauge))
    {
        pr_info("%s: FG init fail!! \n", __func__);
        return -1;
    }

    //** meas_mode = 0 is inbat mode with jig **//
    if(meas_mode == 0)
    {
        cntl = sm5720_read_word(fuelgauge->i2c, SM5720_FG_REG_CNTL);
        pr_info("%s: start, CNTL=0x%x\n", __func__, cntl);

        cntl = cntl | ENABLE_MODE_nENQ4;
        sm5720_write_word(fuelgauge->i2c, SM5720_FG_REG_CNTL, cntl);
        msleep(300);
    }
    else
    {
        cntl = sm5720_read_word(fuelgauge->i2c, SM5720_FG_REG_CNTL);
        cntl = cntl & (~ENABLE_MODE_nENQ4);
        sm5720_write_word(fuelgauge->i2c, SM5720_FG_REG_CNTL, cntl);
        pr_info("%s: end_1, CNTL=0x%x\n", __func__, cntl);

        msleep(300);

        cntl = sm5720_read_word(fuelgauge->i2c, SM5720_FG_REG_CNTL);
        cntl = cntl & (~ENABLE_MODE_nENQ4);
        sm5720_write_word(fuelgauge->i2c, SM5720_FG_REG_CNTL, cntl);
        pr_info("%s: end_2, CNTL=0x%x\n", __func__, cntl);
    }

    ret = sm5720_get_vbat(fuelgauge);
    pr_info("%s: jig mode real batt V = %d, CNTL=0x%x, meas_mode=%d \n", __func__, ret, cntl, meas_mode);

    return ret;
}

static int sm5720_fg_check_battery_present(struct sm5720_fuelgauge_data *fuelgauge)
{
	// SM5720 is not suport batt present
	pr_info("%s: sm5720_fg_get_batt_present\n", __func__);

	return true;

}

static bool sm5720_check_jig_status(struct sm5720_fuelgauge_data *fuelgauge)
{
    bool ret = true;

    if(fuelgauge->pdata->jig_gpio) {
		if (fuelgauge->pdata->jig_low_active)
        	ret = !gpio_get_value(fuelgauge->pdata->jig_gpio);
		else
			ret = gpio_get_value(fuelgauge->pdata->jig_gpio);
    }

    return ret;
}

void sm5720_fg_reset_capacity_by_jig_connection(struct sm5720_fuelgauge_data *fuelgauge)
{
    union power_supply_propval value;
	int ret;

    pr_info("%s: voltage = 1.079v (Jig Connection)\n", __func__);

	ret = sm5720_read_word(fuelgauge->i2c, SM5720_FG_REG_RESERVED);
	ret |= JIG_CONNECTED;
	sm5720_write_word(fuelgauge->i2c, SM5720_FG_REG_RESERVED, ret);
	/* If JIG is attached, the voltage is set as 1079 */
	value.intval = 1079;
	psy_do_property("battery", set,
			POWER_SUPPLY_PROP_VOLTAGE_NOW, value);

    return;
}

int sm5720_fg_alert_init(struct sm5720_fuelgauge_data *fuelgauge, int soc)
{
    int ret;
    int value_soc_alarm;

    fuelgauge->is_fuel_alerted = false;

    // remove interrupt
    // ret = sm5720_read_word(fuelgauge->i2c, SM5720_FG_REG_INTFG);

    // check status
    ret = sm5720_read_word(fuelgauge->i2c, SM5720_FG_REG_STATUS);
    if(ret < 0)
    {
        pr_err("%s: Failed to read SM5720_FG_REG_STATUS\n", __func__);
        return -1;
    }

    // remove all mask
    // sm5720_write_word(fuelgauge->i2c,SM5720_FG_REG_INTFG_MASK, 0x0000);

    /* enable volt alert only, other alert mask*/
    //ret = MASK_L_SOC_INT|MASK_H_TEM_INT|MASK_L_TEM_INT;
    //sm5720_write_word(fuelgauge->i2c,SM5720_FG_REG_INTFG_MASK,ret);
    //fuelgauge->info.irq_ctrl = ~(ret);

    value_soc_alarm = (soc<<8);//0x0100 = 1.00%
    if (sm5720_write_word(fuelgauge->i2c, SM5720_FG_REG_SOC_ALARM, value_soc_alarm)<0)
    {
        pr_err("%s: Failed to write SM5720_FG_REG_SOC_ALARM\n", __func__);
        return -1;
    }

    // enabel volt alert control, other alert disable
    ret = sm5720_read_word(fuelgauge->i2c, SM5720_FG_REG_CNTL);
    if(ret < 0)
    {
        pr_err("%s: Failed to read SM5720_FG_REG_CNTL\n", __func__);
        return -1;
    }
    ret = ret | ENABLE_V_ALARM;
    ret = ret & (~ENABLE_SOC_ALARM & ~ENABLE_T_H_ALARM & ~ENABLE_T_L_ALARM);
    if (sm5720_write_word(fuelgauge->i2c, SM5720_FG_REG_CNTL, ret)<0)
    {
        pr_err("%s: Failed to write SM5720_REG_CNTL\n", __func__);
        return -1;
    }

    pr_info("%s: fg_irq= 0x%x, REG_CNTL=0x%x, SOC_ALARM=0x%x \n",
        __func__, fuelgauge->fg_irq, ret, value_soc_alarm);

    return 1;
}

static irqreturn_t sm5720_jig_irq_thread(int irq, void *irq_data)
{
    struct sm5720_fuelgauge_data *fuelgauge = irq_data;

    if (sm5720_check_jig_status(fuelgauge))
        sm5720_fg_reset_capacity_by_jig_connection(fuelgauge);
    else
        pr_info("%s: jig removed\n", __func__);
    return IRQ_HANDLED;
}

static void sm5720_fg_buffer_read(struct sm5720_fuelgauge_data *fuelgauge)
{
	int ret0, ret1, ret2, ret3, ret4, ret5;

	ret0 = sm5720_read_word(fuelgauge->i2c, SM5720_FG_REG_START_LB_V);
	ret1 = sm5720_read_word(fuelgauge->i2c, SM5720_FG_REG_START_LB_V+1);
	ret2 = sm5720_read_word(fuelgauge->i2c, SM5720_FG_REG_START_LB_V+2);
	ret3 = sm5720_read_word(fuelgauge->i2c, SM5720_FG_REG_START_LB_V+3);
	ret4 = sm5720_read_word(fuelgauge->i2c, SM5720_FG_REG_START_LB_V+4);
	ret5 = sm5720_read_word(fuelgauge->i2c, SM5720_FG_REG_START_LB_V+5);
	pr_info("%s: sm5720 FG buffer 0x30_0x35 lb_V = 0x%x, 0x%x, 0x%x, 0x%x, 0x%x, 0x%x \n",
		__func__, ret0, ret1, ret2, ret3, ret4, ret5);

	ret0 = sm5720_read_word(fuelgauge->i2c, SM5720_FG_REG_START_CB_V);
	ret1 = sm5720_read_word(fuelgauge->i2c, SM5720_FG_REG_START_CB_V+1);
	ret2 = sm5720_read_word(fuelgauge->i2c, SM5720_FG_REG_START_CB_V+2);
	ret3 = sm5720_read_word(fuelgauge->i2c, SM5720_FG_REG_START_CB_V+3);
	ret4 = sm5720_read_word(fuelgauge->i2c, SM5720_FG_REG_START_CB_V+4);
	ret5 = sm5720_read_word(fuelgauge->i2c, SM5720_FG_REG_START_CB_V+5);
	pr_info("%s: sm5720 FG buffer 0x36_0x3B cb_V = 0x%x, 0x%x, 0x%x, 0x%x, 0x%x, 0x%x \n",
		__func__, ret0, ret1, ret2, ret3, ret4, ret5);


	ret0 = sm5720_read_word(fuelgauge->i2c, SM5720_FG_REG_START_LB_I);
	ret1 = sm5720_read_word(fuelgauge->i2c, SM5720_FG_REG_START_LB_I+1);
	ret2 = sm5720_read_word(fuelgauge->i2c, SM5720_FG_REG_START_LB_I+2);
	ret3 = sm5720_read_word(fuelgauge->i2c, SM5720_FG_REG_START_LB_I+3);
	ret4 = sm5720_read_word(fuelgauge->i2c, SM5720_FG_REG_START_LB_I+4);
	ret5 = sm5720_read_word(fuelgauge->i2c, SM5720_FG_REG_START_LB_I+5);
	pr_info("%s: sm5720 FG buffer 0x40_0x45 lb_I = 0x%x, 0x%x, 0x%x, 0x%x, 0x%x, 0x%x \n",
		__func__, ret0, ret1, ret2, ret3, ret4, ret5);


	ret0 = sm5720_read_word(fuelgauge->i2c, SM5720_FG_REG_START_CB_I);
	ret1 = sm5720_read_word(fuelgauge->i2c, SM5720_FG_REG_START_CB_I+1);
	ret2 = sm5720_read_word(fuelgauge->i2c, SM5720_FG_REG_START_CB_I+2);
	ret3 = sm5720_read_word(fuelgauge->i2c, SM5720_FG_REG_START_CB_I+3);
	ret4 = sm5720_read_word(fuelgauge->i2c, SM5720_FG_REG_START_CB_I+4);
	ret5 = sm5720_read_word(fuelgauge->i2c, SM5720_FG_REG_START_CB_I+5);
	pr_info("%s: sm5720 FG buffer 0x46_0x4B cb_I = 0x%x, 0x%x, 0x%x, 0x%x, 0x%x, 0x%x \n",
		__func__, ret0, ret1, ret2, ret3, ret4, ret5);

	return;
}

static bool sm5720_fg_init(struct sm5720_fuelgauge_data *fuelgauge, bool is_surge)
{
	fuelgauge->info.is_FG_initialised = 0;

    if(sm5720_get_device_id(fuelgauge)<0)
    {
        return false;
    }
    sm5720_fg_check_battery_present(fuelgauge);

    if (fuelgauge->pdata->jig_gpio) {
        int ret;
		if (fuelgauge->pdata->jig_low_active) {
	        ret = request_threaded_irq(fuelgauge->pdata->jig_irq,
	                NULL, sm5720_jig_irq_thread,
	                IRQF_TRIGGER_FALLING | IRQF_ONESHOT,
	                "jig-irq", fuelgauge);
		} else {
	        ret = request_threaded_irq(fuelgauge->pdata->jig_irq,
					NULL, sm5720_jig_irq_thread,
					IRQF_TRIGGER_RISING | IRQF_ONESHOT,
					"jig-irq", fuelgauge);
		}
        if (ret) {
            pr_info("%s: Failed to Request IRQ\n",
                    __func__);
        }
        pr_info("%s: jig_result : %d\n", __func__, sm5720_check_jig_status(fuelgauge));

        /* initial check for the JIG */
        if (sm5720_check_jig_status(fuelgauge))
            sm5720_fg_reset_capacity_by_jig_connection(fuelgauge);
    }

#if defined(CONFIG_BATTERY_AGE_FORECAST)
	fuelgauge->info.v_max_now = sm5720_read_word(fuelgauge->i2c, 0xAE);
	pr_info("%s: v_max_now = 0x%x\n", __func__, fuelgauge->info.v_max_now);
	fuelgauge->info.v_max_now = sm5720_read_word(fuelgauge->i2c, 0xAE);
	pr_info("%s: v_max_now = 0x%x\n", __func__, fuelgauge->info.v_max_now);
	fuelgauge->info.q_max_now = sm5720_read_word(fuelgauge->i2c, 0xBE);
	pr_info("%s: q_max_now = 0x%x\n", __func__, fuelgauge->info.q_max_now);
	fuelgauge->info.q_max_now = sm5720_read_word(fuelgauge->i2c, 0xBE);
	pr_info("%s: q_max_now = 0x%x\n", __func__, fuelgauge->info.q_max_now);
	fuelgauge->info.q_max_now = sm5720_read_word(fuelgauge->i2c, SM5720_FG_REG_CAP);
	pr_info("%s: q_max_now = 0x%x\n", __func__, fuelgauge->info.q_max_now);
#endif

	if(sm5720_fg_check_reg_init_need(fuelgauge))
	{
		if(sm5720_fg_reg_init(fuelgauge, is_surge))
			pr_info("%s: boot time kernel init DONE!\n", __func__);
		else
			pr_info("%s: ERROR!! boot time kernel init ERROR!!\n", __func__);
	}
	else
	{
		sm5720_write_word(fuelgauge->i2c, SM5720_FG_REG_MIX_RATE, fuelgauge->info.mix_value[0]);
	}
    //sm5720_dp_setup(fuelgauge);
    sm5720_alg_setup(fuelgauge);

	// for debug
	sm5720_fg_buffer_read(fuelgauge);

    // for start mq
    if(is_surge)
    {
        sm5720_write_word(fuelgauge->i2c, SM5720_FG_REG_START_MQ, ((fuelgauge->info.full_mq_dump<<11)/1000));
    }
    sm5720_meas_mq_start(fuelgauge);

	fuelgauge->info.is_FG_initialised = 1;

    return true;
}

bool sm5720_fg_fuelalert_init(struct sm5720_fuelgauge_data *fuelgauge,
                int soc)
{
    /* 1. Set sm5720 alert configuration. */
    if (sm5720_fg_alert_init(fuelgauge, soc) > 0)
        return true;
    else
        return false;
}

void sm5720_fg_fuelalert_set(struct sm5720_fuelgauge_data *fuelgauge,
                   int enable)
{
    u16 ret;

    ret = sm5720_read_word(fuelgauge->i2c, SM5720_FG_REG_STATUS);
    pr_info("%s: SM5720_FG_REG_STATUS(0x%x)\n",
        __func__, ret);

    /* not use SOC alarm
    if(ret & fuelgauge->info.irq_ctrl & ENABLE_SOC_ALARM) {
        fuelgauge->info.soc_alert_flag = true;
        // todo more action
    }
    */

    if (ret & ENABLE_V_ALARM && !lpcharge && !fuelgauge->is_charging) {
		pr_info("%s : Battery Voltage is Very Low!! SW V EMPTY ENABLE\n", __func__);

		if (fuelgauge->vempty_mode == VEMPTY_MODE_SW ||
					fuelgauge->vempty_mode == VEMPTY_MODE_SW_VALERT) {
			fuelgauge->vempty_mode = VEMPTY_MODE_SW_VALERT;
		}
#if defined(CONFIG_BATTERY_CISD)
		else {
			union power_supply_propval value;
			value.intval = fuelgauge->vempty_mode;
			psy_do_property("battery", set,
					POWER_SUPPLY_PROP_VOLTAGE_MIN, value);
		}
#endif
    }
}

bool sm5720_fg_fuelalert_process(void *irq_data)
{
    struct sm5720_fuelgauge_data *fuelgauge =
        (struct sm5720_fuelgauge_data *)irq_data;

    sm5720_fg_fuelalert_set(fuelgauge, 0);

    return true;
}

bool sm5720_fg_reset(struct sm5720_fuelgauge_data *fuelgauge, bool is_quickstart)
{
	pr_info("%s: Start fg reset\n", __func__);
	// SW reset code
	sm5720_fg_verified_write_word(fuelgauge->i2c, SM5720_FG_REG_RESET, SW_RESET_CODE);
	// delay 1000ms
	msleep(1000);

    sm5720_meas_mq_off(fuelgauge);

    if(is_quickstart)
    {
        if(sm5720_fg_init(fuelgauge, false))
        {
            pr_info("%s: Quick Start - mq STOP!!\n", __func__);
#ifdef ENABLE_SM5720_MQ_FUNCTION
            sm5720_meas_mq_off(fuelgauge);
#endif
        }
        else
        {
            pr_info("%s: sm5720_fg_init ERROR!!!!\n", __func__);
            return false;
        }
}
#ifdef ENABLE_BATT_LONG_LIFE
    else
    {
        if(sm5720_fg_init(fuelgauge, true))
        {
            pr_info("%s: BATT_LONG_LIFE reset - mq CONTINUE!!\n", __func__);
        }
        else
        {
            pr_info("%s: sm5720_fg_init ERROR!!!!\n", __func__);
            return false;
        }
    }
#endif

    pr_info("%s: End fg reset\n", __func__);

    return true;
}

static int sm5720_fg_check_capacity_max(
	struct sm5720_fuelgauge_data *fuelgauge, int capacity_max)
{
	int cap_max, cap_min;

	cap_max = fuelgauge->pdata->capacity_max;
	cap_min = (fuelgauge->pdata->capacity_max -
			fuelgauge->pdata->capacity_max_margin);

	return (capacity_max < cap_min) ? cap_min :
		((capacity_max >= cap_max) ? cap_max : capacity_max);
}

static void sm5720_fg_get_scaled_capacity(
    struct sm5720_fuelgauge_data *fuelgauge,
    union power_supply_propval *val)
{
	union power_supply_propval cable_val, chg_val, chg_val2;
#if defined(CONFIG_BATTERY_SWELLING)
	union power_supply_propval swelling_val;
#endif
	int current_standard, raw_capacity = val->intval;
	struct power_supply *psy = get_power_supply_by_name("battery");

	if(!psy) {
		pr_info("%s : battery is not initailized\n", __func__);
		return;
	}

	psy_do_property("battery", get, POWER_SUPPLY_PROP_ONLINE, cable_val);
#if defined(CONFIG_BATTERY_SWELLING)
	/* Check whether DUT is in the swelling mode or not */
	psy_do_property("battery", get, POWER_SUPPLY_PROP_CHARGE_CONTROL_LIMIT, swelling_val);
#endif

	chg_val.intval = cable_val.intval;
	psy_do_property("sm5720-charger", get, POWER_SUPPLY_PROP_CURRENT_AVG,
			chg_val);
	psy_do_property("sm5720-charger", get, POWER_SUPPLY_PROP_CHARGE_NOW,
			chg_val2);

	if (is_hv_wireless_type(fuelgauge->cable_type) || is_hv_wire_type(fuelgauge->cable_type))
		current_standard = CAPACITY_SCALE_HV_CURRENT;
	else
		current_standard = CAPACITY_SCALE_DEFAULT_CURRENT;

	pr_info("%s : current_standard(%d)\n", __func__, current_standard);

	if ((cable_val.intval != SEC_BATTERY_CABLE_NONE) &&
#if defined(CONFIG_BATTERY_SWELLING)
		(!swelling_val.intval) &&
#endif
		chg_val2.intval &&
		(chg_val.intval >= current_standard)) {
		int max_temp;
		int temp, sample;
		int curr;
		int topoff;
		int capacity_threshold;
		static int cnt;

		max_temp = fuelgauge->capacity_max;
		curr = fuelgauge->info.batt_avgcurrent;
		topoff = fuelgauge->pdata->full_check_current_1st;
		capacity_threshold = topoff + CAPACITY_MAX_CONTROL_THRESHOLD;

		pr_info("%s : curr(%d) topoff(%d) capacity_max(%d)\n", __func__, curr, topoff, max_temp);

		if ((curr < capacity_threshold) && (curr > topoff)) {
			if (!cnt) {
				cnt = 1;
				fuelgauge->standard_capacity = (val->intval < fuelgauge->pdata->capacity_min) ?
					0 : ((val->intval - fuelgauge->pdata->capacity_min) * 999 /
					(fuelgauge->capacity_max - fuelgauge->pdata->capacity_min));
			} else if (fuelgauge->standard_capacity < 999) {
				temp = (val->intval < fuelgauge->pdata->capacity_min) ?
					0 : ((val->intval - fuelgauge->pdata->capacity_min) * 999 /
					(fuelgauge->capacity_max - fuelgauge->pdata->capacity_min));

				sample = ((capacity_threshold - curr) * (999 - fuelgauge->standard_capacity)) /
					(capacity_threshold - topoff);

				pr_info("%s : %d = ((%d - %d) * (999 - %d)) / (%d - %d)\n",
					__func__,
					sample, capacity_threshold, curr, fuelgauge->standard_capacity,
					capacity_threshold, topoff);

				if ((temp - fuelgauge->standard_capacity) >= sample) {
					pr_info("%s : TEMP > SAMPLE\n", __func__);
				} else if ((sample - (temp - fuelgauge->standard_capacity)) < 5) {
					pr_info("%s : TEMP < SAMPLE && GAP UNDER 5\n", __func__);
					max_temp -= (sample - (temp - fuelgauge->standard_capacity));
				} else {
					pr_info("%s : TEMP > SAMPLE && GAP OVER 5\n", __func__);
					max_temp -= 5;
				}
				pr_info("%s : TEMP(%d) SAMPLE(%d) CAPACITY_MAX(%d)\n",
					__func__, temp, sample, fuelgauge->capacity_max);

				fuelgauge->capacity_max =
					sm5720_fg_check_capacity_max(fuelgauge, max_temp);
			}
		} else
			cnt = 0;
	}

    val->intval = (val->intval < fuelgauge->pdata->capacity_min) ?
        0 : ((val->intval - fuelgauge->pdata->capacity_min) * 1000 /
        (fuelgauge->capacity_max - fuelgauge->pdata->capacity_min));

	pr_info("%s : CABLE TYPE(%d) INPUT CURRENT(%d) CV MODE(%d)" \
		"capacity_max (%d) scaled capacity(%d.%d), raw_soc(%d.%d)\n",
		__func__, cable_val.intval, chg_val.intval, chg_val2.intval,
		fuelgauge->capacity_max, val->intval/10, val->intval%10, raw_capacity/10, raw_capacity%10);
}

/* capacity is integer */
static void sm5720_fg_get_atomic_capacity(
    struct sm5720_fuelgauge_data *fuelgauge,
    union power_supply_propval *val)
{
    pr_debug("%s : NOW(%d), OLD(%d)\n",
        __func__, val->intval, fuelgauge->capacity_old);

    if (fuelgauge->pdata->capacity_calculation_type &
        SEC_FUELGAUGE_CAPACITY_TYPE_ATOMIC) {
    if (fuelgauge->capacity_old < val->intval)
        val->intval = fuelgauge->capacity_old + 1;
    else if (fuelgauge->capacity_old > val->intval)
        val->intval = fuelgauge->capacity_old - 1;
    }

    /* keep SOC stable in abnormal status */
    if (fuelgauge->pdata->capacity_calculation_type &
        SEC_FUELGAUGE_CAPACITY_TYPE_SKIP_ABNORMAL) {
        if (!fuelgauge->is_charging &&
            fuelgauge->capacity_old < val->intval) {
            pr_err("%s: capacity (old %d : new %d)\n",
                __func__, fuelgauge->capacity_old, val->intval);
            val->intval = fuelgauge->capacity_old;
        }
    }

    /* updated old capacity */
    fuelgauge->capacity_old = val->intval;
}

static int sm5720_fg_calculate_dynamic_scale(
    struct sm5720_fuelgauge_data *fuelgauge, int capacity)
{
	union power_supply_propval raw_soc_val;
	raw_soc_val.intval = sm5720_get_soc(fuelgauge);

	if (raw_soc_val.intval <
		fuelgauge->pdata->capacity_max -
		fuelgauge->pdata->capacity_max_margin) {
		pr_info("%s: raw soc(%d) is very low, skip routine\n",
			__func__, raw_soc_val.intval);
	} else {
		fuelgauge->capacity_max =
			(raw_soc_val.intval * 100 / (capacity + 1));
		fuelgauge->capacity_old = capacity;

		fuelgauge->capacity_max =
			sm5720_fg_check_capacity_max(fuelgauge,
			fuelgauge->capacity_max);

		pr_info("%s: %d is used for capacity_max, capacity(%d)\n",
			__func__, fuelgauge->capacity_max, capacity);
	}

	return fuelgauge->capacity_max;
}

static int calc_ttf(struct sm5720_fuelgauge_data *fuelgauge, union power_supply_propval *val)
{
        int i;
        int cc_time = 0, cv_time = 0;

        int soc = fuelgauge->raw_capacity;
        int charge_current = val->intval;
        struct cv_slope *cv_data = fuelgauge->cv_data;
        int design_cap = fuelgauge->battery_data->Capacity * fuelgauge->fg_resistor / 2;

        if(!cv_data || (val->intval <= 0)) {
                pr_info("%s: no cv_data or val: %d\n", __func__, val->intval);
                return -1;
        }
        for (i = 0; i < fuelgauge->cv_data_lenth ;i++) {
                if (charge_current >= cv_data[i].fg_current)
                        break;
        }
        i = i >= fuelgauge->cv_data_lenth ? fuelgauge->cv_data_lenth - 1 : i;
        if (cv_data[i].soc < soc) {
                for (i = 0; i < fuelgauge->cv_data_lenth; i++) {
                        if (soc <= cv_data[i].soc)
                                break;
                }
                cv_time = ((cv_data[i-1].time - cv_data[i].time) * (cv_data[i].soc - soc)\
                                / (cv_data[i].soc - cv_data[i-1].soc)) + cv_data[i].time;
        } else { //CC mode || NONE
                cv_time = cv_data[i].time;
                cc_time = design_cap * (cv_data[i].soc - soc)\
                                / val->intval * 3600 / 1000;
                pr_debug("%s: cc_time: %d\n", __func__, cc_time);
                if (cc_time < 0) {

                        cc_time = 0;
                }
        }

        pr_debug("%s: cap: %d, soc: %4d, T: %6d, avg: %4d, cv soc: %4d, i: %4d, val: %d\n",
         __func__, design_cap, soc, cv_time + cc_time, fuelgauge->current_avg, cv_data[i].soc, i, val->intval);

        if (cv_time + cc_time >= 0)
                return cv_time + cc_time + 60;
        else
                return 60; //minimum 1minutes
}

static void sm5720_fg_set_vempty(struct sm5720_fuelgauge_data *fuelgauge, int vempty_mode)
{
	u16 data = 0;
	u32 value_v_alarm = 0;

	if (!fuelgauge->using_temp_compensation) {
		pr_info("%s: does not use temp compensation, default hw vempty\n", __func__);
		vempty_mode = VEMPTY_MODE_HW;
	}

	fuelgauge->vempty_mode = vempty_mode;

	switch (vempty_mode) {
		case VEMPTY_MODE_SW:
			/* HW Vempty Disable */

			/* set volt alert threshold */
			value_v_alarm = (fuelgauge->battery_data->sw_v_empty_vol << 8)/1000;

			if (sm5720_write_word(fuelgauge->i2c, SM5720_FG_REG_V_ALARM, value_v_alarm)<0) {
				pr_info("%s: Failed to write VALRT_THRESHOLD_REG\n", __func__);
				return;
			}
			data = sm5720_read_word(fuelgauge->i2c, SM5720_FG_REG_V_ALARM);
			pr_info("%s: HW V EMPTY Disable, SW V EMPTY Enable with %d mV (%d) \n",
				__func__, fuelgauge->battery_data->sw_v_empty_vol, data);
			break;
		default:
			/* HW Vempty works together with CISD v_alarm */
			value_v_alarm = (fuelgauge->battery_data->sw_v_empty_vol_cisd << 8)/1000;

			if (sm5720_write_word(fuelgauge->i2c, SM5720_FG_REG_V_ALARM, value_v_alarm)<0) {
				pr_info("%s: Failed to write VALRT_THRESHOLD_REG\n", __func__);
				return;
			}
			data = sm5720_read_word(fuelgauge->i2c, SM5720_FG_REG_V_ALARM);
			pr_info("%s: HW V EMPTY Enable, SW V EMPTY Disable %d mV (%d) \n",
				__func__, fuelgauge->battery_data->sw_v_empty_vol_cisd, data);
			break;
	}
}

static int sm5720_fg_get_property(struct power_supply *psy,
                 enum power_supply_property psp,
                 union power_supply_propval *val)
{
    struct sm5720_fuelgauge_data *fuelgauge = power_supply_get_drvdata(psy);

    switch (psp) {
        /* Cell voltage (VCELL, mV) */
    case POWER_SUPPLY_PROP_VOLTAGE_NOW:
        sm5720_get_vbat(fuelgauge);
        val->intval = fuelgauge->info.batt_voltage;
        break;
    case POWER_SUPPLY_PROP_VOLTAGE_AVG:
        switch (val->intval) {
        case SEC_BATTERY_VOLTAGE_OCV:
			val->intval = fuelgauge->info.batt_ocv;
			sm5720_fg_test_read(fuelgauge);
            break;
        case SEC_BATTERY_VOLTAGE_AVERAGE:
            val->intval = fuelgauge->info.batt_avgvoltage;
            break;
        }
        break;
        /* Current */
    case POWER_SUPPLY_PROP_CURRENT_NOW:
		sm5720_get_curr(fuelgauge);
		if (val->intval == SEC_BATTERY_CURRENT_UA)
			val->intval = fuelgauge->info.batt_current * 1000;
		else
			val->intval = fuelgauge->info.batt_current;
		break;
        /* Average Current */
    case POWER_SUPPLY_PROP_CURRENT_AVG:
		if (val->intval == SEC_BATTERY_CURRENT_UA)
			val->intval = fuelgauge->info.batt_avgcurrent * 1000;
		else
			val->intval = fuelgauge->info.batt_avgcurrent;
		break;
        /* Full Capacity */
    case POWER_SUPPLY_PROP_ENERGY_NOW:
        switch (val->intval) {
        case SEC_BATTERY_CAPACITY_DESIGNED:
            break;
        case SEC_BATTERY_CAPACITY_ABSOLUTE:
            break;
        case SEC_BATTERY_CAPACITY_TEMPERARY:
            break;
        case SEC_BATTERY_CAPACITY_CURRENT:
		case SEC_BATTERY_CAPACITY_QH:
			val->intval = (fuelgauge->info.cap * 1000) >> 11;
            break;
        case SEC_BATTERY_CAPACITY_CYCLE:
            sm5720_get_cycle(fuelgauge);
            val->intval = fuelgauge->info.batt_soc_cycle;
            break;
		case SEC_BATTERY_CAPACITY_FULL:
		case SEC_BATTERY_CAPACITY_AGEDCELL:
			val->intval = (fuelgauge->info.cap * 1000) >> 11;
			break;
		case SEC_BATTERY_CAPACITY_VFSOC:
			val->intval = fuelgauge->info.batt_soc * 100;
			break;
        }
        break;
        /* SOC (%) */
    case POWER_SUPPLY_PROP_CAPACITY:
		sm5720_update_all_value(fuelgauge);
		/* SM5720 F/G unit is 0.1%, raw ==> convert the unit to 0.01% */
		if (val->intval == SEC_FUELGAUGE_CAPACITY_TYPE_RAW) {
			val->intval = fuelgauge->info.batt_soc * 10;
			break;
		} else
			val->intval = fuelgauge->info.batt_soc;

		if (fuelgauge->pdata->capacity_calculation_type &
			(SEC_FUELGAUGE_CAPACITY_TYPE_SCALE |
			SEC_FUELGAUGE_CAPACITY_TYPE_DYNAMIC_SCALE)){
			sm5720_fg_get_scaled_capacity(fuelgauge, val);

			if (val->intval > 1010) {
				pr_info("%s : scaled capacity (%d) \n", __func__, val->intval);
				sm5720_fg_calculate_dynamic_scale(fuelgauge, 100);
			}
		}
            /* capacity should be between 0% and 100%
             * (0.1% degree)
             */
            if (val->intval > 1000)
                val->intval = 1000;
            if (val->intval < 0)
                val->intval = 0;

            fuelgauge->raw_capacity = val->intval;

            /* get only integer part */
            val->intval /= 10;

			/* SW/HW V Empty setting */
			if (fuelgauge->using_hw_vempty) {
				if (fuelgauge->info.temperature <= (int)fuelgauge->low_temp_limit) {
					if (fuelgauge->raw_capacity <= 50) {
						if (fuelgauge->vempty_mode != VEMPTY_MODE_HW) {
							sm5720_fg_set_vempty(fuelgauge, VEMPTY_MODE_HW);
						}
					} else if (fuelgauge->vempty_mode == VEMPTY_MODE_HW) {
						sm5720_fg_set_vempty(fuelgauge, VEMPTY_MODE_SW);
					}
				} else if (fuelgauge->vempty_mode != VEMPTY_MODE_HW) {
					sm5720_fg_set_vempty(fuelgauge, VEMPTY_MODE_HW);
				}
			}

			if (!fuelgauge->is_charging &&
			    fuelgauge->vempty_mode == VEMPTY_MODE_SW_VALERT && !lpcharge) {
				pr_info("%s : SW V EMPTY. Decrease SOC\n", __func__);
				val->intval = 0;
			} else if ((fuelgauge->vempty_mode == VEMPTY_MODE_SW_RECOVERY) &&
				   (val->intval == fuelgauge->capacity_old)) {
				fuelgauge->vempty_mode = VEMPTY_MODE_SW;
			}


            /* check whether doing the wake_unlock */
            if ((val->intval > fuelgauge->pdata->fuel_alert_soc) &&
                 fuelgauge->is_fuel_alerted) {
                sm5720_fg_fuelalert_init(fuelgauge,
                      fuelgauge->pdata->fuel_alert_soc);
            }

            /* (Only for atomic capacity)
             * In initial time, capacity_old is 0.
             * and in resume from sleep,
             * capacity_old is too different from actual soc.
             * should update capacity_old
             * by val->intval in booting or resume.
             */
            if ((fuelgauge->initial_update_of_soc) &&
                (fuelgauge->vempty_mode != VEMPTY_MODE_SW_VALERT)) {
                /* updated old capacity */
                fuelgauge->capacity_old = val->intval;
                fuelgauge->initial_update_of_soc = false;
                break;
            }

			if (fuelgauge->sleep_initial_update_of_soc) {
				/* updated old capacity in case of resume */
				if(fuelgauge->is_charging ||
					((!fuelgauge->is_charging) && (fuelgauge->capacity_old >= val->intval))) {
					fuelgauge->capacity_old = val->intval;
					fuelgauge->sleep_initial_update_of_soc = false;
					break;
				}
			}

            if (fuelgauge->pdata->capacity_calculation_type &
                (SEC_FUELGAUGE_CAPACITY_TYPE_ATOMIC |
                 SEC_FUELGAUGE_CAPACITY_TYPE_SKIP_ABNORMAL)){
                sm5720_fg_get_atomic_capacity(fuelgauge, val);
        }
        break;
        /* Battery Temperature */
    case POWER_SUPPLY_PROP_TEMP:
        /* Target Temperature */
    case POWER_SUPPLY_PROP_TEMP_AMBIENT:
        sm5720_get_temperature(fuelgauge);
        val->intval = fuelgauge->info.temp_fg;
        break;
    case POWER_SUPPLY_PROP_ENERGY_FULL:
		val->intval = -1;
		break;
    case POWER_SUPPLY_PROP_ENERGY_FULL_DESIGN:
        val->intval = fuelgauge->capacity_max;
        break;
    case POWER_SUPPLY_PROP_TIME_TO_FULL_NOW:
        val->intval = calc_ttf(fuelgauge, val);
        break;
    case POWER_SUPPLY_PROP_CHARGE_ENABLED:
        break;
#if defined(CONFIG_BATTERY_AGE_FORECAST)
    case POWER_SUPPLY_PROP_CAPACITY_LEVEL:
        return -ENODATA;
#endif
    default:
        return -EINVAL;
    }
    return 0;
}

#if defined(CONFIG_UPDATE_BATTERY_DATA)
static int sm5720_fuelgauge_parse_dt(struct sm5720_fuelgauge_data *fuelgauge);
#endif
static int sm5720_fg_set_property(struct power_supply *psy,
                 enum power_supply_property psp,
                 const union power_supply_propval *val)
{
    struct sm5720_fuelgauge_data *fuelgauge = power_supply_get_drvdata(psy);

    switch (psp) {
    case POWER_SUPPLY_PROP_STATUS:
        break;
    case POWER_SUPPLY_PROP_CHARGE_FULL:
        if (fuelgauge->pdata->capacity_calculation_type &
            SEC_FUELGAUGE_CAPACITY_TYPE_DYNAMIC_SCALE)
            sm5720_fg_calculate_dynamic_scale(fuelgauge, val->intval);

            sm5720_set_full_chg_mq(fuelgauge, sm5720_meas_mq_dump(fuelgauge));
            fuelgauge->info.full_mq_dump = sm5720_meas_mq_dump(fuelgauge);
#if defined(CONFIG_BATTERY_AGE_FORECAST)
        pr_info("%s: POWER_SUPPLY_PROP_CHARGE_FULL : q_max_now = 0x%x \n", __func__, fuelgauge->info.q_max_now);
		if (fuelgauge->info.q_max_now !=
            fuelgauge->info.q_max_table[get_v_max_index_by_cycle(fuelgauge)]){
            if (!sm5720_fg_reset(fuelgauge, false))
                return -EINVAL;
        }
#endif

        break;
    case POWER_SUPPLY_PROP_ONLINE:
        fuelgauge->cable_type = val->intval;
        if (val->intval == SEC_BATTERY_CABLE_NONE) {
			fuelgauge->ta_exist = false;
            fuelgauge->is_charging = false;
        } else {
            fuelgauge->ta_exist = true;
            fuelgauge->is_charging = true;

			/* enable alert */
			if (fuelgauge->vempty_mode >= VEMPTY_MODE_SW_VALERT) {
				sm5720_fg_set_vempty(fuelgauge, VEMPTY_MODE_HW);
                fuelgauge->initial_update_of_soc = true;
                sm5720_fg_fuelalert_init(fuelgauge,
                               fuelgauge->pdata->fuel_alert_soc);
            }
        }
        break;
        /* Battery Temperature */
    case POWER_SUPPLY_PROP_CAPACITY:
        if (val->intval == SEC_FUELGAUGE_CAPACITY_TYPE_RESET) {
			bool error = sm5720_fg_reset(fuelgauge, true);

            fuelgauge->initial_update_of_soc = true;
			if (!error)
                return -EINVAL;
            else
                break;
        }
    case POWER_SUPPLY_PROP_TEMP:
		fuelgauge->info.temperature = val->intval;
        if(val->intval < 0) {
                pr_info("%s: set the low temp reset! temp : %d\n",
                        __func__, val->intval);
            }
        break;
    case POWER_SUPPLY_PROP_TEMP_AMBIENT:
        break;
    case POWER_SUPPLY_PROP_HEALTH:
	fuelgauge->info.flag_charge_health =
	(val->intval == POWER_SUPPLY_HEALTH_GOOD) ? 1 : 0;
	pr_info("%s: charge health from charger = 0x%x\n", __func__, val->intval);
	break;
    case POWER_SUPPLY_PROP_ENERGY_NOW:
        sm5720_fg_get_jig_mode_real_vbat(fuelgauge, val->intval);
		sm5720_fg_reset_capacity_by_jig_connection(fuelgauge);
        break;
    case POWER_SUPPLY_PROP_ENERGY_FULL_DESIGN:
        pr_info("%s: capacity_max changed, %d -> %d\n",
            __func__, fuelgauge->capacity_max, val->intval);
		fuelgauge->capacity_max = sm5720_fg_check_capacity_max(fuelgauge, val->intval);
        fuelgauge->initial_update_of_soc = true;
        break;
    case POWER_SUPPLY_PROP_CHARGE_ENABLED:
        break;
#if defined(CONFIG_BATTERY_AGE_FORECAST)
    case POWER_SUPPLY_PROP_CAPACITY_LEVEL:
        pr_info("%s: full condition soc changed, %d -> %d\n",
            __func__, fuelgauge->chg_full_soc, val->intval);
        fuelgauge->chg_full_soc = val->intval;
        break;
#endif
#if defined(CONFIG_UPDATE_BATTERY_DATA)
    case POWER_SUPPLY_PROP_POWER_DESIGN:
        sm5720_fuelgauge_parse_dt(fuelgauge);
        break;
#endif
    default:
        return -EINVAL;
    }
    return 0;
}

static void sm5720_fg_isr_work(struct work_struct *work)
{
    struct sm5720_fuelgauge_data *fuelgauge =
        container_of(work, struct sm5720_fuelgauge_data, isr_work.work);

    /* process for fuel gauge chip */
    sm5720_fg_fuelalert_process(fuelgauge);

    wake_unlock(&fuelgauge->fuel_alert_wake_lock);
}

static irqreturn_t sm5720_fg_irq_thread(int irq, void *irq_data)
{
    struct sm5720_fuelgauge_data *fuelgauge = irq_data;

    pr_info("%s\n", __func__);

    if (fuelgauge->is_fuel_alerted) {
        return IRQ_HANDLED;
    } else {
        wake_lock(&fuelgauge->fuel_alert_wake_lock);
        fuelgauge->is_fuel_alerted = true;
        schedule_delayed_work(&fuelgauge->isr_work, 0);
    }

    return IRQ_HANDLED;
}

static int sm5720_fuelgauge_debugfs_show(struct seq_file *s, void *data)
{
    struct sm5720_fuelgauge_data *fuelgauge = s->private;
    int i;
    u8 reg;
    u8 reg_data;

    seq_printf(s, "SM5720 FUELGAUGE IC :\n");
    seq_printf(s, "===================\n");
    for (i = 0; i < 16; i++) {
        if (i == 12)
            continue;
        for (reg = 0; reg < 0x10; reg++) {
            reg_data = sm5720_read_word(fuelgauge->i2c, reg + i * 0x10);
            seq_printf(s, "0x%02x:\t0x%04x\n", reg + i * 0x10, reg_data);
        }
        if (i == 4)
            i = 10;
    }
    seq_printf(s, "\n");
    return 0;
}

static int sm5720_fuelgauge_debugfs_open(struct inode *inode, struct file *file)
{
    return single_open(file, sm5720_fuelgauge_debugfs_show, inode->i_private);
}

static const struct file_operations sm5720_fuelgauge_debugfs_fops = {
    .open           = sm5720_fuelgauge_debugfs_open,
    .read           = seq_read,
    .llseek         = seq_lseek,
    .release        = single_release,
};

#if defined(CONFIG_OF)
#define PROPERTY_NAME_SIZE 128

#define PINFO(format, args...) \
	printk(KERN_INFO "%s() line-%d: " format, \
		__func__, __LINE__, ## args)

#if defined(CONFIG_BATTERY_AGE_FORECAST)
static int sm5720_fuelgauge_parse_dt_for_age(struct sm5720_fuelgauge_data *fuelgauge)
{
	struct device_node *np = of_find_node_by_name(NULL, "battery");
	int len=0, ret;
	const u32 *p;

	if (np == NULL) {
		pr_err("%s np NULL\n", __func__);
	} else {
		p = of_get_property(np, "battery,age_data", &len);
		if (p) {
			fuelgauge->pdata->num_age_step = len / sizeof(sec_age_data_t);
			fuelgauge->pdata->age_data = kzalloc(len, GFP_KERNEL);
			ret = of_property_read_u32_array(np, "battery,age_data",
					 (u32 *)fuelgauge->pdata->age_data, len/sizeof(u32));
			if (ret) {
				pr_err("%s failed to read battery->pdata->age_data: %d\n",
						__func__, ret);
				kfree(fuelgauge->pdata->age_data);
				fuelgauge->pdata->age_data = NULL;
				fuelgauge->pdata->num_age_step = 0;
			}
			pr_info("%s num_age_step : %d\n", __func__, fuelgauge->pdata->num_age_step);
			for (len = 0; len < fuelgauge->pdata->num_age_step; ++len) {
				pr_info("[%d/%d]cycle:%d, float:%d, full_v:%d, recharge_v:%d, soc:%d\n",
					len, fuelgauge->pdata->num_age_step-1,
					fuelgauge->pdata->age_data[len].cycle,
					fuelgauge->pdata->age_data[len].float_voltage,
					fuelgauge->pdata->age_data[len].full_condition_vcell,
					fuelgauge->pdata->age_data[len].recharge_condition_vcell,
					fuelgauge->pdata->age_data[len].full_condition_soc);
			}
		} else {
			fuelgauge->pdata->num_age_step = 0;
			pr_err("%s there is not age_data\n", __func__);
		}
	}
	return 0;
}
#endif

static int sm5720_fuelgauge_parse_dt_for_cable_info(struct sm5720_fuelgauge_data *fuelgauge)
{
	struct device_node *np = of_find_node_by_name(NULL, "cable-info");
	int ret;

	if (np == NULL) {
		pr_err("%s np NULL\n", __func__);
	} else {
		ret = of_property_read_u32(np, "full_check_current_1st",
			&fuelgauge->pdata->full_check_current_1st);
		if (ret < 0)
			pr_err("%s error reading full_check_current_1st %d\n", __func__, ret);
		else
			pr_info("%s : full_check_current_1st=%d\n",
				__func__, fuelgauge->pdata->full_check_current_1st);
	}
	return 0;
}
static int sm5720_fuelgauge_parse_dt(struct sm5720_fuelgauge_data *fuelgauge)
{
    char prop_name[PROPERTY_NAME_SIZE];
    int battery_id = -1;
    int table[16];
#if defined(CONFIG_BATTERY_AGE_FORECAST)
    int v_max_table[5];
    int q_max_table[5];
#endif
    int rce_value[3];
    int rs_value[5];
    int mix_value[2];
    int battery_type[3];
    int topoff_soc[3];
    int cycle_cfg[3];
    int v_offset_cancel[4];
    int temp_volcal[3];
    int temp_offset[6];
    int temp_cal[10];
    int ext_temp_cal[10];
    int set_temp_poff[4];
    int curr_offset[7];
    int curr_cal[6];
    int arsm[4];
#if defined(ENABLE_FULL_OFFSET)
    int full_offset[2];
#endif

    int ret;
    int i, j;
    const u32 *p;
    int len;

    struct device_node *np = of_find_node_by_name(NULL, "sm5720-fuelgauge");

    /* reset, irq gpio info */
    if (np == NULL) {
        pr_err("%s np NULL\n", __func__);
    } else {
		ret = of_property_read_u32(np, "fuelgauge,capacity_max",
				&fuelgauge->pdata->capacity_max);
		if (ret < 0)
			pr_err("%s error reading capacity_max %d\n", __func__, ret);

		ret = of_property_read_u32(np, "fuelgauge,capacity_max_margin",
				&fuelgauge->pdata->capacity_max_margin);
		if (ret < 0) {
			pr_err("%s error reading capacity_max_margin %d\n", __func__, ret);
			fuelgauge->pdata->capacity_max_margin = 300;
		}

		ret = of_property_read_u32(np, "fuelgauge,capacity_min",
				&fuelgauge->pdata->capacity_min);
		if (ret < 0)
			pr_err("%s error reading capacity_min %d\n", __func__, ret);

		pr_info("%s: capacity_max: %d, capacity_max_margin: %d, capacity_min: %d\n",
			__func__, fuelgauge->pdata->capacity_max,
			fuelgauge->pdata->capacity_max_margin, fuelgauge->pdata->capacity_min);

		ret = of_property_read_u32(np, "fuelgauge,capacity_calculation_type",
				&fuelgauge->pdata->capacity_calculation_type);
		if (ret < 0)
			pr_err("%s error reading capacity_calculation_type %d\n",
					__func__, ret);
		ret = of_property_read_u32(np, "fuelgauge,fuel_alert_soc",
				&fuelgauge->pdata->fuel_alert_soc);
		if (ret < 0)
			pr_err("%s error reading pdata->fuel_alert_soc %d\n",
					__func__, ret);
		fuelgauge->pdata->repeated_fuelalert = of_property_read_bool(np,
				"fuelgaguge,repeated_fuelalert");

		pr_info("%s: "
				"calculation_type: 0x%x, fuel_alert_soc: %d,\n"
				"repeated_fuelalert: %d\n", __func__,
				fuelgauge->pdata->capacity_calculation_type,
				fuelgauge->pdata->fuel_alert_soc, fuelgauge->pdata->repeated_fuelalert);


        fuelgauge->using_temp_compensation = of_property_read_bool(np,
                           "fuelgauge,using_temp_compensation");
		if (fuelgauge->using_temp_compensation) {
			ret = of_property_read_u32(np, "fuelgauge,low_temp_limit",
						   &fuelgauge->low_temp_limit);
			if (ret < 0)
				pr_err("%s error reading low temp limit %d\n", __func__, ret);

			pr_info("%s : LOW TEMP LIMIT(%d)\n",
				__func__, fuelgauge->low_temp_limit);
		}

        fuelgauge->using_hw_vempty = of_property_read_bool(np,
                                   "fuelgauge,using_hw_vempty");
        if (fuelgauge->using_hw_vempty) {
            ret = of_property_read_u32(np, "fuelgauge,v_empty",
                           &fuelgauge->battery_data->V_empty);
            if (ret < 0)
                pr_err("%s error reading v_empty %d\n",
                       __func__, ret);

            ret = of_property_read_u32(np, "fuelgauge,v_empty_origin",
                           &fuelgauge->battery_data->V_empty_origin);
            if(ret < 0)
                pr_err("%s error reading v_empty_origin %d\n",
                       __func__, ret);

			ret = of_property_read_u32(np, "fuelgauge,sw_v_empty_voltage_cisd",
					&fuelgauge->battery_data->sw_v_empty_vol_cisd);
			if(ret < 0) {
				pr_err("%s error reading sw_v_empty_default_vol_cise %d\n",
						__func__, ret);
				fuelgauge->battery_data->sw_v_empty_vol_cisd = 3100;
			}

			ret = of_property_read_u32(np, "fuelgauge,sw_v_empty_voltage",
						   &fuelgauge->battery_data->sw_v_empty_vol);
			if(ret < 0) {
				pr_err("%s error reading sw_v_empty_default_vol %d\n",
					   __func__, ret);
				fuelgauge->battery_data->sw_v_empty_vol	= 3200;
			}

			ret = of_property_read_u32(np, "fuelgauge,sw_v_empty_recover_voltage",
						   &fuelgauge->battery_data->sw_v_empty_recover_vol);
			if(ret < 0) {
				pr_err("%s error reading sw_v_empty_recover_vol %d\n",
					   __func__, ret);
				fuelgauge->battery_data->sw_v_empty_recover_vol = 3480;
			}

			pr_info("%s : SW V Empty (%d)mV,  SW V Empty recover (%d)mV, CISD V_Alarm (%d)mV\n",
				__func__,
				fuelgauge->battery_data->sw_v_empty_vol,
				fuelgauge->battery_data->sw_v_empty_recover_vol,
				fuelgauge->battery_data->sw_v_empty_vol_cisd);
        }

        fuelgauge->pdata->jig_gpio = of_get_named_gpio(np, "fuelgauge,jig_gpio", 0);
        if (fuelgauge->pdata->jig_gpio < 0) {
            pr_err("%s error reading jig_gpio = %d\n",
                    __func__,fuelgauge->pdata->jig_gpio);
            fuelgauge->pdata->jig_gpio = 0;
        } else {
            fuelgauge->pdata->jig_irq = gpio_to_irq(fuelgauge->pdata->jig_gpio);
        }

		if (fuelgauge->pdata->jig_gpio) {
			ret = of_property_read_u32(np, "fuelgauge,jig_low_active",
						&fuelgauge->pdata->jig_low_active);

			if (ret < 0) {
				pr_err("%s error reading jig_low_active %d\n", __func__, ret);
				fuelgauge->pdata->jig_low_active = 0;
			}
		}

        ret = of_property_read_u32(np, "fuelgauge,fg_resistor",
                &fuelgauge->fg_resistor);
        if (ret < 0) {
            pr_err("%s error reading fg_resistor %d\n",
                    __func__, ret);
            fuelgauge->fg_resistor = 1;
        }

        ret = of_property_read_u32(np, "fuelgauge,capacity",
                       &fuelgauge->battery_data->Capacity);
        if (ret < 0)
            pr_err("%s error reading Capacity %d\n",
                    __func__, ret);

        p = of_get_property(np, "fuelgauge,cv_data", &len);
        if (p) {
                fuelgauge->cv_data = kzalloc(len,
                                          GFP_KERNEL);
                fuelgauge->cv_data_lenth = len / sizeof(struct cv_slope);
                pr_err("%s len: %ld, lenth: %d, %d\n",
                                __func__, sizeof(int) * len, len, fuelgauge->cv_data_lenth);
                ret = of_property_read_u32_array(np, "fuelgauge,cv_data",
                                 (u32 *)fuelgauge->cv_data, len/sizeof(u32));

                if (ret) {
                        pr_err("%s failed to read fuelgauge->cv_data: %d\n",
                                        __func__, ret);
                        kfree(fuelgauge->cv_data);
                        fuelgauge->cv_data = NULL;
                }
        } else {
                pr_err("%s there is not cv_data\n", __func__);
        }


#if defined(CONFIG_BATTERY_AGE_FORECAST)
        ret = of_property_read_u32(np, "battery,full_condition_soc",
            &fuelgauge->pdata->full_condition_soc);
        if (ret) {
            fuelgauge->pdata->full_condition_soc = 93;
            pr_info("%s : Full condition soc is Empty\n", __func__);
        }
#endif
    }

    // get battery_params node for reg init
    np = of_find_node_by_name(of_node_get(np), "battery_params");
    if (np == NULL) {
        PINFO("Cannot find child node \"battery_params\"\n");
        return -EINVAL;
    }

    // get battery_id
    if (of_property_read_u32(np, "battery,id", &battery_id) < 0)
        PINFO("not battery,id property\n");
    PINFO("battery id = %d\n", battery_id);

    // get battery_table
    for (i = DISCHARGE_TABLE; i < TABLE_MAX; i++) {
        snprintf(prop_name, PROPERTY_NAME_SIZE,
             "battery%d,%s%d", battery_id, "battery_table", i);

        ret = of_property_read_u32_array(np, prop_name, table, 16);
        if (ret < 0) {
            PINFO("Can get prop %s (%d)\n", prop_name, ret);
        }
        for (j = 0; j <= FG_TABLE_LEN; j++)
        {
            fuelgauge->info.battery_table[i][j] = table[j];
            PINFO("%s = <table[%d][%d] 0x%x>\n", prop_name, i, j, table[j]);
        }
    }

    // get rce
    for (i = 0; i < 3; i++) {
        snprintf(prop_name, PROPERTY_NAME_SIZE, "battery%d,%s", battery_id, "rce_value");
        ret = of_property_read_u32_array(np, prop_name, rce_value, 3);
        if (ret < 0) {
            PINFO("Can get prop %s (%d)\n", prop_name, ret);
        }
        fuelgauge->info.rce_value[i] = rce_value[i];
    }
    PINFO("%s = <0x%x 0x%x 0x%x>\n", prop_name, rce_value[0], rce_value[1], rce_value[2]);

    // get dtcd_value
    snprintf(prop_name, PROPERTY_NAME_SIZE, "battery%d,%s", battery_id, "dtcd_value");
    ret = of_property_read_u32_array(np, prop_name, &fuelgauge->info.dtcd_value, 1);
    if (ret < 0)
        PINFO("Can get prop %s (%d)\n", prop_name, ret);
    PINFO("%s = <0x%x>\n",prop_name, fuelgauge->info.dtcd_value);

    // get rs_value
    for (i = 0; i < 5; i++) {
        snprintf(prop_name, PROPERTY_NAME_SIZE, "battery%d,%s", battery_id, "rs_value");
        ret = of_property_read_u32_array(np, prop_name, rs_value, 5);
        if (ret < 0) {
            PINFO("Can get prop %s (%d)\n", prop_name, ret);
        }
        fuelgauge->info.rs_value[i] = rs_value[i];
    }
    PINFO("%s = <0x%x 0x%x 0x%x 0x%x 0x%x>\n", prop_name, rs_value[0], rs_value[1], rs_value[2], rs_value[3], rs_value[4]);

    // get vit_period
    snprintf(prop_name, PROPERTY_NAME_SIZE, "battery%d,%s", battery_id, "vit_period");
    ret = of_property_read_u32_array(np, prop_name, &fuelgauge->info.vit_period, 1);
    if (ret < 0)
        PINFO("Can get prop %s (%d)\n", prop_name, ret);
    PINFO("%s = <0x%x>\n",prop_name, fuelgauge->info.vit_period);

    // get mix_value
    for (i = 0; i < 2; i++) {
        snprintf(prop_name, PROPERTY_NAME_SIZE, "battery%d,%s", battery_id, "mix_value");
        ret = of_property_read_u32_array(np, prop_name, mix_value, 2);
        if (ret < 0) {
            PINFO("Can get prop %s (%d)\n", prop_name, ret);
        }
        fuelgauge->info.mix_value[i] = mix_value[i];
    }
    PINFO("%s = <0x%x 0x%x>\n", prop_name, mix_value[0], mix_value[1]);

    // battery_type
    snprintf(prop_name, PROPERTY_NAME_SIZE, "battery%d,%s", battery_id, "battery_type");
    ret = of_property_read_u32_array(np, prop_name, battery_type, 3);
    if (ret < 0)
        PINFO("Can get prop %s (%d)\n", prop_name, ret);
    fuelgauge->info.batt_v_max = battery_type[0];
    fuelgauge->info.min_cap = battery_type[1];
    fuelgauge->info.cap = battery_type[2];

    PINFO("%s = <%d 0x%x 0x%x>\n", prop_name,
        fuelgauge->info.batt_v_max, fuelgauge->info.min_cap, fuelgauge->info.cap);

#if defined(CONFIG_BATTERY_AGE_FORECAST)
	snprintf(prop_name, PROPERTY_NAME_SIZE, "battery%d,%s", battery_id, "v_max_table");
	ret = of_property_read_u32_array(np, prop_name, v_max_table, fuelgauge->pdata->num_age_step);

	if(ret < 0){
		PINFO("Can get prop %s (%d)\n", prop_name, ret);

		for (i = 0; i < fuelgauge->pdata->num_age_step; i++){
			fuelgauge->info.v_max_table[i] = fuelgauge->info.battery_table[DISCHARGE_TABLE][FG_TABLE_LEN-1];
			PINFO("%s = <v_max_table[%d] 0x%x>\n", prop_name, i, fuelgauge->info.v_max_table[i]);
		}
	}else{
		for (i = 0; i < fuelgauge->pdata->num_age_step; i++){
			fuelgauge->info.v_max_table[i] = v_max_table[i];
			PINFO("%s = <v_max_table[%d] 0x%x>\n", prop_name, i, fuelgauge->info.v_max_table[i]);
		}
	}

	snprintf(prop_name, PROPERTY_NAME_SIZE, "battery%d,%s", battery_id, "q_max_table");
	ret = of_property_read_u32_array(np, prop_name, q_max_table, fuelgauge->pdata->num_age_step);

	if(ret < 0){
		PINFO("Can get prop %s (%d)\n", prop_name, ret);

		for (i = 0; i < fuelgauge->pdata->num_age_step; i++){
			fuelgauge->info.q_max_table[i] = fuelgauge->info.cap;
			PINFO("%s = <q_max_table[%d] 0x%x>\n", prop_name, i, fuelgauge->info.q_max_table[i]);
		}
	}else{
		for (i = 0; i < fuelgauge->pdata->num_age_step; i++){
			fuelgauge->info.q_max_table[i] = q_max_table[i];
			PINFO("%s = <q_max_table[%d] 0x%x>\n", prop_name, i, fuelgauge->info.q_max_table[i]);
		}
	}
	fuelgauge->chg_full_soc = fuelgauge->pdata->age_data[0].full_condition_soc;
	fuelgauge->info.v_max_now = fuelgauge->info.v_max_table[0];
	fuelgauge->info.q_max_now = fuelgauge->info.q_max_table[0];
	PINFO("%s = <v_max_now = 0x%x>, <q_max_now = 0x%x>, <chg_full_soc = %d>\n", prop_name, fuelgauge->info.v_max_now, fuelgauge->info.q_max_now, fuelgauge->chg_full_soc);
#endif

    // MISC
    snprintf(prop_name, PROPERTY_NAME_SIZE, "battery%d,%s", battery_id, "misc");
    ret = of_property_read_u32_array(np, prop_name, &fuelgauge->info.misc, 1);
    if (ret < 0)
        PINFO("Can get prop %s (%d)\n", prop_name, ret);
    PINFO("%s = <0x%x>\n", prop_name, fuelgauge->info.misc);

    // V_ALARM
    snprintf(prop_name, PROPERTY_NAME_SIZE, "battery%d,%s", battery_id, "v_alarm");
    ret = of_property_read_u32_array(np, prop_name, &fuelgauge->info.value_v_alarm, 1);
    if (ret < 0)
        PINFO("Can get prop %s (%d)\n", prop_name, ret);
    PINFO("%s = <%d>\n", prop_name, fuelgauge->info.value_v_alarm);

    // TOP OFF SOC
    snprintf(prop_name, PROPERTY_NAME_SIZE, "battery%d,%s", battery_id, "topoff_soc");
    ret = of_property_read_u32_array(np, prop_name, topoff_soc, 3);
    if (ret < 0)
        PINFO("Can get prop %s (%d)\n", prop_name, ret);
    fuelgauge->info.enable_topoff_soc = topoff_soc[0];
    fuelgauge->info.topoff_soc = topoff_soc[1];
    fuelgauge->info.top_off = topoff_soc[2];

    PINFO("%s = <%d %d %d>\n", prop_name,
        fuelgauge->info.enable_topoff_soc, fuelgauge->info.topoff_soc, fuelgauge->info.top_off);

    // SOC cycle cfg
    snprintf(prop_name, PROPERTY_NAME_SIZE, "battery%d,%s", battery_id, "cycle_cfg");
    ret = of_property_read_u32_array(np, prop_name, cycle_cfg, 3);
    if (ret < 0)
        PINFO("Can get prop %s (%d)\n", prop_name, ret);
    fuelgauge->info.cycle_high_limit = cycle_cfg[0];
    fuelgauge->info.cycle_low_limit = cycle_cfg[1];
    fuelgauge->info.cycle_limit_cntl = cycle_cfg[2];

    PINFO("%s = <%d %d %d>\n", prop_name,
        fuelgauge->info.cycle_high_limit, fuelgauge->info.cycle_low_limit, fuelgauge->info.cycle_limit_cntl);

    // v_offset_cancel
    snprintf(prop_name, PROPERTY_NAME_SIZE, "battery%d,%s", battery_id, "v_offset_cancel");
    ret = of_property_read_u32_array(np, prop_name, v_offset_cancel, 4);
    if (ret < 0)
        PINFO("Can get prop %s (%d)\n", prop_name, ret);
    fuelgauge->info.enable_v_offset_cancel_p = v_offset_cancel[0];
    fuelgauge->info.enable_v_offset_cancel_n = v_offset_cancel[1];
    fuelgauge->info.v_offset_cancel_level = v_offset_cancel[2];
    fuelgauge->info.v_offset_cancel_mohm = v_offset_cancel[3];

    PINFO("%s = <%d %d %d %d>\n", prop_name,
		fuelgauge->info.enable_v_offset_cancel_p, fuelgauge->info.enable_v_offset_cancel_n, fuelgauge->info.v_offset_cancel_level, fuelgauge->info.v_offset_cancel_mohm);

    // VOL & CURR CAL
    snprintf(prop_name, PROPERTY_NAME_SIZE, "battery%d,%s", battery_id, "volt_cal");
    ret = of_property_read_u32_array(np, prop_name, &fuelgauge->info.volt_cal, 1);
    if (ret < 0)
        PINFO("Can get prop %s (%d)\n", prop_name, ret);
    PINFO("%s = <0x%x>\n", prop_name, fuelgauge->info.volt_cal);

    snprintf(prop_name, PROPERTY_NAME_SIZE, "battery%d,%s", battery_id, "curr_offset");
    ret = of_property_read_u32_array(np, prop_name, curr_offset, 7);
    if (ret < 0)
        PINFO("Can get prop %s (%d)\n", prop_name, ret);
    fuelgauge->info.en_auto_i_offset = curr_offset[0];
    fuelgauge->info.ecv_i_off = curr_offset[1];
    fuelgauge->info.csp_i_off = curr_offset[2];
    fuelgauge->info.csn_i_off = curr_offset[3];
    fuelgauge->info.dp_ecv_i_off = curr_offset[4];
    fuelgauge->info.dp_csp_i_off = curr_offset[5];
    fuelgauge->info.dp_csn_i_off = curr_offset[6];

    PINFO("%s = <%d 0x%x 0x%x 0x%x>\n", prop_name, fuelgauge->info.en_auto_i_offset, fuelgauge->info.ecv_i_off, fuelgauge->info.csp_i_off, fuelgauge->info.csn_i_off);

    snprintf(prop_name, PROPERTY_NAME_SIZE, "battery%d,%s", battery_id, "curr_cal");
    ret = of_property_read_u32_array(np, prop_name, curr_cal, 6);
    if (ret < 0)
        PINFO("Can get prop %s (%d)\n", prop_name, ret);
    fuelgauge->info.ecv_i_slo = curr_cal[0];
    fuelgauge->info.csp_i_slo = curr_cal[1];
    fuelgauge->info.csn_i_slo = curr_cal[2];
    fuelgauge->info.dp_ecv_i_slo = curr_cal[3];
    fuelgauge->info.dp_csp_i_slo = curr_cal[4];
    fuelgauge->info.dp_csn_i_slo = curr_cal[5];

    PINFO("%s = <0x%x 0x%x 0x%x 0x%x 0x%x 0x%x>\n", prop_name, fuelgauge->info.ecv_i_slo, fuelgauge->info.csp_i_slo, fuelgauge->info.csn_i_slo
        ,fuelgauge->info.dp_ecv_i_slo, fuelgauge->info.dp_csp_i_slo, fuelgauge->info.dp_csn_i_slo);


#if defined(ENABLE_FULL_OFFSET)
    snprintf(prop_name, PROPERTY_NAME_SIZE, "battery%d,%s", battery_id, "full_offset");
    ret = of_property_read_u32_array(np, prop_name, full_offset, 2);
    if (ret < 0)
        PINFO("Can get prop %s (%d)\n", prop_name, ret);
    fuelgauge->info.full_offset_margin = full_offset[0];
    fuelgauge->info.full_extra_offset = full_offset[1];

    PINFO("%s = <%d %d>\n", prop_name, fuelgauge->info.full_offset_margin, fuelgauge->info.full_extra_offset);
#endif

    // temp_std
    snprintf(prop_name, PROPERTY_NAME_SIZE, "battery%d,%s", battery_id, "temp_std");
    ret = of_property_read_u32_array(np, prop_name, &fuelgauge->info.temp_std, 1);
    if (ret < 0)
        PINFO("Can get prop %s (%d)\n", prop_name, ret);
    PINFO("%s = <%d>\n", prop_name, fuelgauge->info.temp_std);

    // temp_volcal
    snprintf(prop_name, PROPERTY_NAME_SIZE, "battery%d,%s", battery_id, "temp_volcal");
    ret = of_property_read_u32_array(np, prop_name, temp_volcal, 3);
    if (ret < 0)
        PINFO("Can get prop %s (%d)\n", prop_name, ret);
    fuelgauge->info.en_fg_temp_volcal = temp_volcal[0];
    fuelgauge->info.fg_temp_volcal_denom = temp_volcal[1];
    fuelgauge->info.fg_temp_volcal_fact = temp_volcal[2];
    PINFO("%s = <%d, %d, %d>\n", prop_name,
        fuelgauge->info.en_fg_temp_volcal, fuelgauge->info.fg_temp_volcal_denom, fuelgauge->info.fg_temp_volcal_fact);

    // temp_offset
    snprintf(prop_name, PROPERTY_NAME_SIZE, "battery%d,%s", battery_id, "temp_offset");
    ret = of_property_read_u32_array(np, prop_name, temp_offset, 6);
    if (ret < 0)
        PINFO("Can get prop %s (%d)\n", prop_name, ret);
    fuelgauge->info.en_high_fg_temp_offset = temp_offset[0];
    fuelgauge->info.high_fg_temp_offset_denom = temp_offset[1];
    fuelgauge->info.high_fg_temp_offset_fact = temp_offset[2];
    fuelgauge->info.en_low_fg_temp_offset = temp_offset[3];
    fuelgauge->info.low_fg_temp_offset_denom = temp_offset[4];
    fuelgauge->info.low_fg_temp_offset_fact = temp_offset[5];
    PINFO("%s = <%d, %d, %d, %d, %d, %d>\n", prop_name,
		fuelgauge->info.en_high_fg_temp_offset,
        fuelgauge->info.high_fg_temp_offset_denom, fuelgauge->info.high_fg_temp_offset_fact,
        fuelgauge->info.en_low_fg_temp_offset,
        fuelgauge->info.low_fg_temp_offset_denom, fuelgauge->info.low_fg_temp_offset_fact);

    // temp_calc
    snprintf(prop_name, PROPERTY_NAME_SIZE, "battery%d,%s", battery_id, "temp_cal");
    ret = of_property_read_u32_array(np, prop_name, temp_cal, 10);
    if (ret < 0)
        PINFO("Can get prop %s (%d)\n", prop_name, ret);
    fuelgauge->info.en_high_fg_temp_cal = temp_cal[0];
    fuelgauge->info.high_fg_temp_p_cal_denom = temp_cal[1];
    fuelgauge->info.high_fg_temp_p_cal_fact = temp_cal[2];
    fuelgauge->info.high_fg_temp_n_cal_denom = temp_cal[3];
    fuelgauge->info.high_fg_temp_n_cal_fact = temp_cal[4];
    fuelgauge->info.en_low_fg_temp_cal = temp_cal[5];
    fuelgauge->info.low_fg_temp_p_cal_denom = temp_cal[6];
    fuelgauge->info.low_fg_temp_p_cal_fact = temp_cal[7];
    fuelgauge->info.low_fg_temp_n_cal_denom = temp_cal[8];
    fuelgauge->info.low_fg_temp_n_cal_fact = temp_cal[9];
    PINFO("%s = <%d, %d, %d, %d, %d, %d, %d, %d, %d, %d>\n", prop_name,
		fuelgauge->info.en_high_fg_temp_cal,
        fuelgauge->info.high_fg_temp_p_cal_denom, fuelgauge->info.high_fg_temp_p_cal_fact,
        fuelgauge->info.high_fg_temp_n_cal_denom, fuelgauge->info.high_fg_temp_n_cal_fact,
        fuelgauge->info.en_low_fg_temp_cal,
        fuelgauge->info.low_fg_temp_p_cal_denom, fuelgauge->info.low_fg_temp_p_cal_fact,
        fuelgauge->info.low_fg_temp_n_cal_denom, fuelgauge->info.low_fg_temp_n_cal_fact);

    // ext_temp_calc
    snprintf(prop_name, PROPERTY_NAME_SIZE, "battery%d,%s", battery_id, "ext_temp_cal");
    ret = of_property_read_u32_array(np, prop_name, ext_temp_cal, 10);
    if (ret < 0)
        PINFO("Can get prop %s (%d)\n", prop_name, ret);
    fuelgauge->info.en_high_temp_cal = ext_temp_cal[0];
    fuelgauge->info.high_temp_p_cal_denom = ext_temp_cal[1];
    fuelgauge->info.high_temp_p_cal_fact = ext_temp_cal[2];
    fuelgauge->info.high_temp_n_cal_denom = ext_temp_cal[3];
    fuelgauge->info.high_temp_n_cal_fact = ext_temp_cal[4];
    fuelgauge->info.en_low_temp_cal = ext_temp_cal[5];
    fuelgauge->info.low_temp_p_cal_denom = ext_temp_cal[6];
    fuelgauge->info.low_temp_p_cal_fact = ext_temp_cal[7];
    fuelgauge->info.low_temp_n_cal_denom = ext_temp_cal[8];
    fuelgauge->info.low_temp_n_cal_fact = ext_temp_cal[9];
    PINFO("%s = <%d, %d, %d, %d, %d, %d, %d, %d, %d, %d>\n", prop_name,
		fuelgauge->info.en_high_temp_cal,
        fuelgauge->info.high_temp_p_cal_denom, fuelgauge->info.high_temp_p_cal_fact,
        fuelgauge->info.high_temp_n_cal_denom, fuelgauge->info.high_temp_n_cal_fact,
        fuelgauge->info.en_low_temp_cal,
        fuelgauge->info.low_temp_p_cal_denom, fuelgauge->info.low_temp_p_cal_fact,
        fuelgauge->info.low_temp_n_cal_denom, fuelgauge->info.low_temp_n_cal_fact);

    // tem poff level
    snprintf(prop_name, PROPERTY_NAME_SIZE, "battery%d,%s", battery_id, "tem_poff");
    ret = of_property_read_u32_array(np, prop_name, set_temp_poff, 4);
    if (ret < 0)
        PINFO("Can get prop %s (%d)\n", prop_name, ret);
    fuelgauge->info.n_tem_poff = set_temp_poff[0];
    fuelgauge->info.n_tem_poff_offset = set_temp_poff[1];
    fuelgauge->info.l_tem_poff = set_temp_poff[2];
    fuelgauge->info.l_tem_poff_offset = set_temp_poff[3];

    PINFO("%s = <%d, %d, %d, %d>\n",
        prop_name,
        fuelgauge->info.n_tem_poff, fuelgauge->info.n_tem_poff_offset,
        fuelgauge->info.l_tem_poff, fuelgauge->info.l_tem_poff_offset);

    // arsm setting
    snprintf(prop_name, PROPERTY_NAME_SIZE, "battery%d,%s", battery_id, "arsm");
    ret = of_property_read_u32_array(np, prop_name, arsm, 4);
    if (ret < 0)
        PINFO("Can get prop %s (%d)\n", prop_name, ret);
    fuelgauge->info.arsm[0] = arsm[0];
    fuelgauge->info.arsm[1] = arsm[1];
    fuelgauge->info.arsm[2] = arsm[2];
    fuelgauge->info.arsm[3] = arsm[3];

    PINFO("%s = <%d, %d, %d, %d>\n",
        prop_name,
        fuelgauge->info.arsm[0], fuelgauge->info.arsm[1],
        fuelgauge->info.arsm[2], fuelgauge->info.arsm[3]);

    // batt data version
    snprintf(prop_name, PROPERTY_NAME_SIZE, "battery%d,%s", battery_id, "data_ver");
    ret = of_property_read_u32_array(np, prop_name, &fuelgauge->info.data_ver, 1);
    if (ret < 0)
        PINFO("Can get prop %s (%d)\n", prop_name, ret);
    PINFO("%s = <%d>\n", prop_name, fuelgauge->info.data_ver);




    return 0;
}
#endif

static const struct power_supply_desc sm5720_fuelgauge_power_supply_desc = {
	.name = "sm5720-fuelgauge",
	.type = POWER_SUPPLY_TYPE_UNKNOWN,
	.properties = sm5720_fuelgauge_props,
	.num_properties = ARRAY_SIZE(sm5720_fuelgauge_props),
	.get_property = sm5720_fg_get_property,
	.set_property = sm5720_fg_set_property,
	.no_thermal = true,
};

static int sm5720_fuelgauge_probe(struct platform_device *pdev)
{
    struct sm5720_dev *sm5720 = dev_get_drvdata(pdev->dev.parent);
    struct sm5720_platform_data *pdata = dev_get_platdata(sm5720->dev);
    struct sm5720_fuelgauge_data *fuelgauge;
    sec_fuelgauge_platform_data_t *fuelgauge_data;
	struct power_supply_config fuelgauge_cfg = {};
    int ret = 0;
    union power_supply_propval raw_soc_val;

    pr_info("%s: SM5720 Fuelgauge Driver Loading\n", __func__);

    fuelgauge = kzalloc(sizeof(*fuelgauge), GFP_KERNEL);
    if (!fuelgauge)
        return -ENOMEM;

    fuelgauge_data = kzalloc(sizeof(sec_fuelgauge_platform_data_t), GFP_KERNEL);
    if (!fuelgauge_data) {
        ret = -ENOMEM;
        goto err_free;
    }

    mutex_init(&fuelgauge->fg_lock);

    fuelgauge->dev = &pdev->dev;
    fuelgauge->pdata = fuelgauge_data;
    fuelgauge->i2c = sm5720->fuelgauge;
    //fuelgauge->pmic = sm5720->i2c;
    fuelgauge->sm5720_pdata = pdata;

#if defined(CONFIG_OF)
    fuelgauge->battery_data = kzalloc(sizeof(struct battery_data_t),
                      GFP_KERNEL);
    if(!fuelgauge->battery_data) {
        pr_err("Failed to allocate memory\n");
        ret = -ENOMEM;
        goto err_pdata_free;
    }
#if defined(CONFIG_BATTERY_AGE_FORECAST)
    sm5720_fuelgauge_parse_dt_for_age(fuelgauge);
#endif
    ret = sm5720_fuelgauge_parse_dt(fuelgauge);
    if (ret < 0) {
        pr_err("%s not found charger dt! ret[%d]\n",
               __func__, ret);
    }
    sm5720_fuelgauge_parse_dt_for_cable_info(fuelgauge);
#endif

    platform_set_drvdata(pdev, fuelgauge);

    (void) debugfs_create_file("sm5720-fuelgauge-regs",
        S_IRUGO, NULL, (void *)fuelgauge, &sm5720_fuelgauge_debugfs_fops);

    if (!sm5720_fg_init(fuelgauge, false)) {
        pr_err("%s: Failed to Initialize Fuelgauge\n", __func__);
        goto err_data_free;
    }

    fuelgauge->capacity_max = fuelgauge->pdata->capacity_max;
    raw_soc_val.intval = sm5720_get_soc(fuelgauge);

    if (raw_soc_val.intval > fuelgauge->capacity_max)
        sm5720_fg_calculate_dynamic_scale(fuelgauge, 100);

	/* SW/HW init code. SW/HW V Empty mode must be opposite ! */
	fuelgauge->info.temperature = 300; /* default value */
	pr_info("%s: SW/HW V empty init \n", __func__);
	sm5720_fg_set_vempty(fuelgauge, VEMPTY_MODE_HW);

	fuelgauge_cfg.drv_data = fuelgauge;
	fuelgauge->info.full_mq_dump = sm5720_get_full_chg_mq(fuelgauge);

	fuelgauge->psy_fg = power_supply_register(&pdev->dev, &sm5720_fuelgauge_power_supply_desc, &fuelgauge_cfg);
	if (!fuelgauge->psy_fg) {
        pr_err("%s: Failed to Register psy_fg\n", __func__);
        goto err_data_free;
    }

    fuelgauge->fg_irq = pdata->irq_base + SM5720_FG_IRQ_INT_LOW_VOLTAGE;
    pr_info("[%s]IRQ_BASE(%d) FG_IRQ(%d)\n",
        __func__, pdata->irq_base, fuelgauge->fg_irq);

    fuelgauge->is_fuel_alerted = false;
    if (fuelgauge->pdata->fuel_alert_soc >= 0) {
        if (sm5720_fg_fuelalert_init(fuelgauge,
                       fuelgauge->pdata->fuel_alert_soc)) {
            wake_lock_init(&fuelgauge->fuel_alert_wake_lock,
                       WAKE_LOCK_SUSPEND, "fuel_alerted");
            if (fuelgauge->fg_irq) {
                INIT_DELAYED_WORK(&fuelgauge->isr_work, sm5720_fg_isr_work);

                ret = request_threaded_irq(fuelgauge->fg_irq,
                       NULL, sm5720_fg_irq_thread,
                       IRQF_TRIGGER_FALLING | IRQF_ONESHOT,
                       "fuelgauge-irq", fuelgauge);
                if (ret) {
                    pr_err("%s: Failed to Request IRQ\n", __func__);
                    goto err_supply_unreg;
                }
            }
        } else {
            pr_err("%s: Failed to Initialize Fuel-alert\n",
                   __func__);
            goto err_supply_unreg;
        }
    }

	fuelgauge->sleep_initial_update_of_soc = false;
    fuelgauge->initial_update_of_soc = true;

    pr_info("%s: SM5720 Fuelgauge Driver Loaded\n", __func__);
    return 0;

err_supply_unreg:
    power_supply_unregister(fuelgauge->psy_fg);
err_data_free:
#if defined(CONFIG_OF)
    kfree(fuelgauge->battery_data);
#endif
err_pdata_free:
    kfree(fuelgauge_data);
    mutex_destroy(&fuelgauge->fg_lock);
err_free:
    kfree(fuelgauge);

    return ret;
}

static int sm5720_fuelgauge_remove(struct platform_device *pdev)
{
    struct sm5720_fuelgauge_data *fuelgauge =
        platform_get_drvdata(pdev);

    if (fuelgauge->pdata->fuel_alert_soc >= 0)
        wake_lock_destroy(&fuelgauge->fuel_alert_wake_lock);

    return 0;
}

static int sm5720_fuelgauge_suspend(struct device *dev)
{
    return 0;
}

static int sm5720_fuelgauge_resume(struct device *dev)
{
    struct sm5720_fuelgauge_data *fuelgauge = dev_get_drvdata(dev);

    fuelgauge->sleep_initial_update_of_soc = true;

    return 0;
}

static void sm5720_fuelgauge_shutdown(struct platform_device *pdev)
{
    struct sm5720_fuelgauge_data *fuelgauge = platform_get_drvdata(pdev);

    if (fuelgauge->using_hw_vempty)
        sm5720_fg_set_vempty(fuelgauge, false);

	sm5720_meas_mq_suspend(fuelgauge);
	/* To reduce current leakage during power off state by SM */
	sm5720_write_word(fuelgauge->i2c, SM5720_FG_REG_MIX_RATE, 0x0F03);
}

static SIMPLE_DEV_PM_OPS(sm5720_fuelgauge_pm_ops, sm5720_fuelgauge_suspend,
             sm5720_fuelgauge_resume);

static struct platform_driver sm5720_fuelgauge_driver = {
    .driver = {
           .name = "sm5720-fuelgauge",
           .owner = THIS_MODULE,
#if defined(CONFIG_PM)
           .pm = &sm5720_fuelgauge_pm_ops,
#endif
    },
    .probe  = sm5720_fuelgauge_probe,
    .remove = sm5720_fuelgauge_remove,
    .shutdown = sm5720_fuelgauge_shutdown,
};

static int __init sm5720_fuelgauge_init(void)
{
    pr_info("%s: \n", __func__);
    return platform_driver_register(&sm5720_fuelgauge_driver);
}

static void __exit sm5720_fuelgauge_exit(void)
{
    platform_driver_unregister(&sm5720_fuelgauge_driver);
}
module_init(sm5720_fuelgauge_init);
module_exit(sm5720_fuelgauge_exit);

MODULE_DESCRIPTION("Samsung SM5720 Fuel Gauge Driver");
MODULE_AUTHOR("Samsung Electronics");
MODULE_LICENSE("GPL");
