/*
 *  sec_adc.c
 *  Samsung Mobile Battery Driver
 *
 *  Copyright (C) 2012 Samsung Electronics
 *
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#include "include/sec_adc.h"

#define DEBUG

static struct qpnp_vadc_chip *adc_client;

#define WPC_TEMP_70_DEGREE_VOL 224000
#define WPC_TEMP_85_DEGREE_VOL 152000
/* Need 1 degree margin adc variatoin */
#define WPC_HIGH_THR_TEMP 850
#define WPC_LOW_THR_TEMP 710
#define WPC_HIGH_THR_TEMP_VOL WPC_TEMP_85_DEGREE_VOL
#define WPC_LOW_THR_TEMP_VOL WPC_TEMP_70_DEGREE_VOL
#define WPC_TEMP_HIGH_STATE ADC_TM_LOW_STATE
#define WPC_TEMP_LOW_STATE ADC_TM_HIGH_STATE
#define WPC_HIGH_TEMP_INT_ENABLE ADC_TM_LOW_THR_ENABLE
#define WPC_LOW_TEMP_INT_ENABLE ADC_TM_HIGH_THR_ENABLE
#define WPC_HIGH_LOW_TEMP_INT_ENABLE ADC_TM_HIGH_LOW_THR_ENABLE

static struct qpnp_adc_tm_btm_param	adc_param;

struct adc_list {
	const char *name;
	int channel;
	bool is_used;
	int prev_value;
};

static struct adc_list batt_adc_list[SEC_BAT_ADC_CHANNEL_NUM] = {
	{.name = "adc-cable"},
	{.name = "adc-bat"},
	{.name = "adc-temp"},
	{.name = "adc-temp"},
	{.name = "adc-full"},
	{.name = "adc-volt"},
	{.name = "adc-chg-temp"},
	{.name = "adc-in-bat"},
	{.name = "adc-dischg"},
	{.name = "adc-dischg-ntc"},
	{.name = "adc-wpc-temp"},
	{.name = "adc-slave-chg-temp"},
	{.name = "adc-usb-temp"},
};

static void btm_notification(enum qpnp_tm_state state, void *ctx)
{
	int rc;
	struct sec_battery_info *battery = ctx;

	if (state >= ADC_TM_STATE_NUM) {
		pr_err("invalid state parameter %d\n", state);
		return;
	}

	/* read temperature */
	wake_lock(&battery->monitor_wake_lock);
	queue_delayed_work(battery->monitor_wqueue, &battery->monitor_work, 0);
	pr_info("%s: wpc temp = %d, chg temp = %d\n",
			__func__, battery->wpc_temp, battery->chg_temp);

	/* check thermal state */
	switch (state) {
	case WPC_TEMP_HIGH_STATE:
		if (battery->wpc_temp >= WPC_HIGH_THR_TEMP)
			adc_param.state_request = WPC_LOW_TEMP_INT_ENABLE;

		pr_err("%s: wpc temperature meet high threshold\n", __func__);
		break;
	case WPC_TEMP_LOW_STATE:
		adc_param.state_request = WPC_HIGH_TEMP_INT_ENABLE;
		pr_err("%s: wpc temperature meet low threshold\n", __func__);
		break;
	default:
		pr_err("%s: wpc temperature is unknown state(=%d)\n",
							__func__, state);
		break;
	}

	rc = qpnp_adc_tm_channel_measure(battery->adc_tm_dev, &adc_param);
	if (rc)
		pr_err("requesting BTM failed(=%d)\n", rc);
	else
		pr_err("%s: high temp = %d, low_temp = %d, state_request = %d\n",
			__func__, adc_param.low_thr, adc_param.high_thr,
			adc_param.state_request);
}

void btm_init(struct sec_battery_info *battery)
{
	int rc;

	adc_param.low_thr = WPC_HIGH_THR_TEMP_VOL;
	adc_param.high_thr = WPC_LOW_THR_TEMP_VOL;
	adc_param.state_request = WPC_HIGH_TEMP_INT_ENABLE;
	adc_param.timer_interval = ADC_MEAS1_INTERVAL_1S;
	adc_param.btm_ctx = battery;
	adc_param.threshold_notification = btm_notification;
	adc_param.channel = VADC_AMUX3_GPIO_PU2;

	rc = qpnp_adc_tm_channel_measure(battery->adc_tm_dev, &adc_param);
	if (rc)
		pr_err("requesting BTM failed(=%d)\n", rc);
	else
		pr_err("%s: high temp = %d, low_temp = %d, state_request = %d\n",
			__func__, adc_param.low_thr, adc_param.high_thr,
			adc_param.state_request);
}

static void sec_bat_adc_ap_init(struct platform_device *pdev,
		struct sec_battery_info *battery)
{
	struct device_node *np;
	const char *channel_str[SEC_BAT_ADC_CHANNEL_NUM];
	int channel_num[SEC_BAT_ADC_CHANNEL_NUM];
	int channel_cells = 0;
	unsigned int i , j = 0;
	int ret = 0;
	u32 temp = 0;	
	
	adc_client = qpnp_get_vadc(battery->dev, "sec-battery");

	if (IS_ERR(adc_client)) {
		int rc;
		rc = PTR_ERR(adc_client);
		if (rc != -EPROBE_DEFER)
			pr_err("%s: Fail to get vadc %d\n", __func__, rc);
	}
	
	np = of_find_node_by_name(NULL, "battery");
	if (!np) {
		pr_err("%s: np NULL\n", __func__);
		return;
	}

	ret = of_property_read_u32(np, "battery,channel_cells", &channel_cells);
	if (ret) {
		pr_err("%s : battery,channel_cells is Empty\n", __func__);
		return;
	}

	ret = of_property_read_string_array(np,
			"battery,channel_names",
			channel_str, SEC_BAT_ADC_CHANNEL_NUM);
	if (ret < 0) {
		pr_err("%s: Looking up %s property in node battery failed",
			__func__, "battery,channel_names");
	}

	for (i = 0; i < channel_cells; i++) {
		ret = of_property_read_u32_index(np,
				"battery,adc_channels", i, &temp);
		if (ret)		
			pr_err("%s : battery,adc_channels is Empty\n", __func__);		
		channel_num[i] = (int)temp;
	}

	for (i = 0; i < channel_cells; i++) {
		for (j = 0; j < SEC_BAT_ADC_CHANNEL_NUM; j++) {
			if(!strcmp(batt_adc_list[j].name, channel_str[i])) {
				batt_adc_list[j].channel = channel_num[i];
				batt_adc_list[j].is_used = true;
			}
		}
	}

	for (j = 0; j < SEC_BAT_ADC_CHANNEL_NUM; j++) {
		pr_info("%s %s[0x%x] - %s \n",
			__func__, batt_adc_list[j].name, batt_adc_list[j].channel,
			batt_adc_list[j].is_used ? "used" : "not used");
	}	
}

static int sec_bat_adc_ap_read(struct sec_battery_info *battery, int channel)
{
	int data = -1;
	int ret = 0;
	int retry_cnt = RETRY_CNT;
	struct qpnp_vadc_result result;

	if (batt_adc_list[channel].is_used) {
		do {
			ret = (batt_adc_list[channel].is_used) ?
			qpnp_vadc_read(adc_client, batt_adc_list[channel].channel, &result) : 0;
			data = result.adc_code;
			retry_cnt--;
		} while ((retry_cnt > 0) && (data < 0));
	}

	if (retry_cnt <= 0) {
		pr_err("%s: Error in ADC\n", __func__);
		data = batt_adc_list[channel].prev_value;
	} else
		batt_adc_list[channel].prev_value = data;

	return data;
}

static void sec_bat_adc_ap_exit(void)
{
	return;
}

static void sec_bat_adc_none_init(struct platform_device *pdev)
{
}

static int sec_bat_adc_none_read(int channel)
{
	return 0;
}

static void sec_bat_adc_none_exit(void)
{
}

static void sec_bat_adc_ic_init(struct platform_device *pdev)
{
}

static int sec_bat_adc_ic_read(int channel)
{
	return 0;
}

static void sec_bat_adc_ic_exit(void)
{
}
static int adc_read_type(struct sec_battery_info *battery, int channel)
{
	int adc = 0;

	switch (battery->pdata->temp_adc_type) {
	case SEC_BATTERY_ADC_TYPE_NONE:
		adc = sec_bat_adc_none_read(channel);
		break;
	case SEC_BATTERY_ADC_TYPE_AP:
		adc = sec_bat_adc_ap_read(battery, channel);
		break;
	case SEC_BATTERY_ADC_TYPE_IC:
		adc = sec_bat_adc_ic_read(channel);
		break;
	case SEC_BATTERY_ADC_TYPE_NUM:
		break;
	default:
		break;
	}
	pr_debug("[%s] [%d] ADC = %d\n", __func__, channel, adc);

	return adc;
}

static void adc_init_type(struct platform_device *pdev,
			  struct sec_battery_info *battery)
{
	switch (battery->pdata->temp_adc_type) {
	case SEC_BATTERY_ADC_TYPE_NONE:
		sec_bat_adc_none_init(pdev);
		break;
	case SEC_BATTERY_ADC_TYPE_AP:
		sec_bat_adc_ap_init(pdev, battery);
		break;
	case SEC_BATTERY_ADC_TYPE_IC:
		sec_bat_adc_ic_init(pdev);
		break;
	case SEC_BATTERY_ADC_TYPE_NUM:
		break;
	default:
		break;
	}
}

static void adc_exit_type(struct sec_battery_info *battery)
{
	switch (battery->pdata->temp_adc_type) {
	case SEC_BATTERY_ADC_TYPE_NONE:
		sec_bat_adc_none_exit();
		break;
	case SEC_BATTERY_ADC_TYPE_AP:
		sec_bat_adc_ap_exit();
		break;
	case SEC_BATTERY_ADC_TYPE_IC:
		sec_bat_adc_ic_exit();
		break;
	case SEC_BATTERY_ADC_TYPE_NUM:
		break;
	default:
		break;
	}
}

int sec_bat_get_adc_data(struct sec_battery_info *battery,
			int adc_ch, int count)
{
	int adc_data = 0;
	int adc_max = 0;
	int adc_min = 0xFFFF;
	int adc_total = 0;
	int i = 0;

	if (count < 3)
		count = 3;

	for (i = 0; i < count; i++) {
		mutex_lock(&battery->adclock);
#ifdef CONFIG_OF
		adc_data = adc_read_type(battery, adc_ch);
#else
		adc_data = adc_read_type(battery->pdata, adc_ch);
#endif
		mutex_unlock(&battery->adclock);

		if (i != 0) {
			if (adc_data > adc_max)
				adc_max = adc_data;
			else if (adc_data < adc_min)
				adc_min = adc_data;
		} else {
			adc_max = adc_data;
			adc_min = adc_data;
		}
		adc_total += adc_data;
	}

	return (adc_total - adc_max - adc_min) / (count - 2);
}

int sec_bat_get_charger_type_adc
				(struct sec_battery_info *battery)
{
	/* It is true something valid is
	connected to the device for charging.
	By default this something is considered to be USB.*/
	int result = SEC_BATTERY_CABLE_USB;

	int adc = 0;
	int i = 0;

	/* Do NOT check cable type when cable_switch_check() returns false
	 * and keep current cable type
	 */
	if (battery->pdata->cable_switch_check &&
	    !battery->pdata->cable_switch_check())
		return battery->cable_type;

	adc = sec_bat_get_adc_data(battery,
		SEC_BAT_ADC_CHANNEL_CABLE_CHECK,
		battery->pdata->adc_check_count);

	/* Do NOT check cable type when cable_switch_normal() returns false
	 * and keep current cable type
	 */
	if (battery->pdata->cable_switch_normal &&
	    !battery->pdata->cable_switch_normal())
		return battery->cable_type;

	for (i = 0; i < SEC_BATTERY_CABLE_MAX; i++)
		if ((adc > battery->pdata->cable_adc_value[i].min) &&
			(adc < battery->pdata->cable_adc_value[i].max))
			break;
	if (i >= SEC_BATTERY_CABLE_MAX)
		dev_err(battery->dev,
			"%s : default USB\n", __func__);
	else
		result = i;

	dev_dbg(battery->dev, "%s : result(%d), adc(%d)\n",
		__func__, result, adc);

	return result;
}

bool sec_bat_get_value_by_adc(
				struct sec_battery_info *battery,
				enum sec_battery_adc_channel channel,
				union power_supply_propval *value)
{
	int temp = 0;
	int temp_adc;
	int low = 0;
	int high = 0;
	int mid = 0;
	const sec_bat_adc_table_data_t *temp_adc_table = {0 , };
	unsigned int temp_adc_table_size = 0;

	temp_adc = sec_bat_get_adc_data(battery, channel, battery->pdata->adc_check_count);
	if (temp_adc < 0)
		return false;

	switch (channel) {
	case SEC_BAT_ADC_CHANNEL_TEMP:
		temp_adc_table = battery->pdata->temp_adc_table;
		temp_adc_table_size =
			battery->pdata->temp_adc_table_size;
		battery->temp_adc = temp_adc;
		break;
	case SEC_BAT_ADC_CHANNEL_TEMP_AMBIENT:
		temp_adc_table = battery->pdata->temp_amb_adc_table;
		temp_adc_table_size =
			battery->pdata->temp_amb_adc_table_size;
		battery->temp_ambient_adc = temp_adc;
		break;
	case SEC_BAT_ADC_CHANNEL_USB_TEMP:
		temp_adc_table = battery->pdata->usb_temp_adc_table;
		temp_adc_table_size =
			battery->pdata->usb_temp_adc_table_size;
		battery->usb_temp_adc = temp_adc;
		break;
	case SEC_BAT_ADC_CHANNEL_CHG_TEMP:
		temp_adc_table = battery->pdata->chg_temp_adc_table;
		temp_adc_table_size =
		battery->pdata->chg_temp_adc_table_size;
		battery->chg_temp_adc = temp_adc;
		break;
	case SEC_BAT_ADC_CHANNEL_WPC_TEMP: /* Coil Therm */
		temp_adc_table = battery->pdata->wpc_temp_adc_table;
		temp_adc_table_size =
			battery->pdata->wpc_temp_adc_table_size;
		battery->wpc_temp_adc = temp_adc;
		battery->coil_temp_adc = temp_adc;
		break;
	case SEC_BAT_ADC_CHANNEL_SLAVE_CHG_TEMP:
		temp_adc_table = battery->pdata->slave_chg_temp_adc_table;
		temp_adc_table_size =
			battery->pdata->slave_chg_temp_adc_table_size;
		battery->slave_chg_temp_adc = temp_adc;
		break;
	default:
		dev_err(battery->dev,
			"%s: Invalid Property\n", __func__);
		return false;
	}

	if (temp_adc_table[0].adc >= temp_adc) {
		temp = temp_adc_table[0].data;
		goto temp_by_adc_goto;
	} else if (temp_adc_table[temp_adc_table_size-1].adc <= temp_adc) {
		temp = temp_adc_table[temp_adc_table_size-1].data;
		goto temp_by_adc_goto;
	}

	high = temp_adc_table_size - 1;

	while (low <= high) {
		mid = (low + high) / 2;
		if (temp_adc_table[mid].adc > temp_adc)
			high = mid - 1;
		else if (temp_adc_table[mid].adc < temp_adc)
			low = mid + 1;
		else {
			temp = temp_adc_table[mid].data;
			goto temp_by_adc_goto;
		}
	}

	temp = temp_adc_table[high].data;
	temp += ((temp_adc_table[low].data - temp_adc_table[high].data) *
		 (temp_adc - temp_adc_table[high].adc)) /
		(temp_adc_table[low].adc - temp_adc_table[high].adc);

temp_by_adc_goto:
	value->intval = temp;

	dev_info(battery->dev,
		"%s:[%d] Temp(%d), Temp-ADC(%d)\n",
		__func__,channel, temp, temp_adc);

	return true;
}

bool sec_bat_check_vf_adc(struct sec_battery_info *battery)
{
	int adc = 0;

	adc = sec_bat_get_adc_data(battery,
		SEC_BAT_ADC_CHANNEL_BAT_CHECK,
		battery->pdata->adc_check_count);

	if (adc < 0) {
		dev_err(battery->dev, "%s: VF ADC error\n", __func__);
		adc = battery->check_adc_value;
	} else
		battery->check_adc_value = adc;

	if ((battery->check_adc_value <= battery->pdata->check_adc_max) &&
		(battery->check_adc_value >= battery->pdata->check_adc_min)) {
		return true;
	} else {
		dev_info(battery->dev, "%s: adc (%d)\n", __func__, battery->check_adc_value);
		return false;
	}
}

void adc_init(struct platform_device *pdev, struct sec_battery_info *battery)
{
	adc_init_type(pdev, battery);
	btm_init(battery);
}

void adc_exit(struct sec_battery_info *battery)
{
	adc_exit_type(battery);
}
