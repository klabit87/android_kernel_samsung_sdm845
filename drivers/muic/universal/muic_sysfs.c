/*
 * muic_sysfs.c
 *
 * Copyright (C) 2014 Samsung Electronics
 * Thomas Ryu <smilesr.ryu@samsung.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
 *
 */

#include <linux/gpio.h>
#include <linux/i2c.h>
#include <linux/interrupt.h>
#include <linux/slab.h>
#include <linux/platform_device.h>
#include <linux/module.h>
#include <linux/delay.h>
#include <linux/host_notify.h>
#include <linux/string.h>

#include <linux/muic/muic.h>
#include <linux/sec_param.h>

#if defined(CONFIG_MUIC_NOTIFIER)
#include <linux/muic/muic_notifier.h>
#endif /* CONFIG_MUIC_NOTIFIER */

#if defined (CONFIG_OF)
#include <linux/of_device.h>
#include <linux/of_gpio.h>
#endif /* CONFIG_OF */

#include "muic-internal.h"
#include "muic_state.h"
#include "muic_i2c.h"
#include "muic_debug.h"
#include "muic_apis.h"
#include "muic_regmap.h"
#if defined(CONFIG_MUIC_HV)
#include "muic_hv.h"
#endif

#if defined(CONFIG_MUIC_SUPPORT_CCIC)
#include "muic_ccic.h"
#endif

#if defined(CONFIG_MUIC_HV) || defined(CONFIG_SUPPORT_QC30)
#include "../../battery_v2/include/sec_charging_common.h"
#endif

static int muic_resolve_attached_dev(muic_data_t *pmuic)
{
#if defined(CONFIG_MUIC_SUPPORT_CCIC)
	if (pmuic->opmode & OPMODE_CCIC)
		return muic_get_current_legacy_dev(pmuic);
#endif
	return pmuic->attached_dev;
}

static ssize_t muic_show_uart_en(struct device *dev,
						struct device_attribute *attr,
						char *buf)
{
	muic_data_t *pmuic = dev_get_drvdata(dev);

	if (!pmuic->is_rustproof) {
		pr_info("%s:%s UART ENABLE\n", MUIC_DEV_NAME, __func__);
		return sprintf(buf, "1\n");
	}
	pr_info("%s:%s UART DISABLE\n", MUIC_DEV_NAME, __func__);
	return sprintf(buf, "0\n");
}

static ssize_t muic_set_uart_en(struct device *dev,
						struct device_attribute *attr,
						const char *buf, size_t count)
{
	muic_data_t *pmuic = dev_get_drvdata(dev);

	if (!strncmp(buf, "1", 1)) {
		pmuic->is_rustproof = false;
	} else if (!strncmp(buf, "0", 1)) {
		pmuic->is_rustproof = true;
	} else {
		pr_warn("%s:%s invalid value\n", MUIC_DEV_NAME, __func__);
	}

	pr_info("%s:%s uart_en(%d)\n", MUIC_DEV_NAME, __func__,
			!pmuic->is_rustproof);

	return count;
}

static ssize_t muic_show_uart_sel(struct device *dev,
					   struct device_attribute *attr,
					   char *buf)
{
	muic_data_t *pmuic = dev_get_drvdata(dev);
	struct muic_platform_data *pdata = pmuic->pdata;

	switch (pdata->uart_path) {
	case MUIC_PATH_UART_AP:
		pr_info("%s:%s AP\n", MUIC_DEV_NAME, __func__);
		return sprintf(buf, "AP\n");
	case MUIC_PATH_UART_CP:
		pr_info("%s:%s CP\n", MUIC_DEV_NAME, __func__);
		return sprintf(buf, "CP\n");
	default:
		break;
	}

	pr_info("%s:%s UNKNOWN\n", MUIC_DEV_NAME, __func__);
	return sprintf(buf, "UNKNOWN\n");
}

static ssize_t muic_set_uart_sel(struct device *dev,
					  struct device_attribute *attr,
					  const char *buf, size_t count)
{
	muic_data_t *pmuic = dev_get_drvdata(dev);
	struct muic_platform_data *pdata = pmuic->pdata;

	if (!strncasecmp(buf, "AP", 2)) {
		pdata->uart_path = MUIC_PATH_UART_AP;
		switch_to_ap_uart(pmuic);
	} else if (!strncasecmp(buf, "CP", 2)) {
		pdata->uart_path = MUIC_PATH_UART_CP;
		switch_to_cp_uart(pmuic);
	} else {
		pr_warn("%s:%s invalid value\n", MUIC_DEV_NAME, __func__);
	}

	pr_info("%s:%s uart_path(%d)\n", MUIC_DEV_NAME, __func__,
			pdata->uart_path);

	return count;
}

static ssize_t muic_show_usb_sel(struct device *dev,
					   struct device_attribute *attr,
					   char *buf)
{
	muic_data_t *pmuic = dev_get_drvdata(dev);
	struct muic_platform_data *pdata = pmuic->pdata;

	switch (pdata->usb_path) {
	case MUIC_PATH_USB_AP:
		return sprintf(buf, "PDA\n");
	case MUIC_PATH_USB_CP:
		return sprintf(buf, "MODEM\n");
	default:
		break;
	}

	pr_info("%s:%s UNKNOWN\n", MUIC_DEV_NAME, __func__);
	return sprintf(buf, "UNKNOWN\n");
}

static ssize_t muic_set_usb_sel(struct device *dev,
					  struct device_attribute *attr,
					  const char *buf, size_t count)
{
	muic_data_t *pmuic = dev_get_drvdata(dev);
	struct muic_platform_data *pdata = pmuic->pdata;

	if (!strncasecmp(buf, "PDA", 3)) {
		pdata->usb_path = MUIC_PATH_USB_AP;
	} else if (!strncasecmp(buf, "MODEM", 5)) {
		pdata->usb_path = MUIC_PATH_USB_CP;
	} else {
		pr_warn("%s:%s invalid value\n", MUIC_DEV_NAME, __func__);
	}

	pr_info("%s:%s usb_path(%d)\n", MUIC_DEV_NAME, __func__,
			pdata->usb_path);

	return count;
}

static ssize_t muic_show_adc(struct device *dev,
				      struct device_attribute *attr, char *buf)
{
	muic_data_t *pmuic = dev_get_drvdata(dev);
	int ret;

	mutex_lock(&pmuic->muic_mutex);
	ret = get_adc(pmuic);
	mutex_unlock(&pmuic->muic_mutex);
	if (ret < 0) {
		pr_err("%s:%s err read adc reg(%d)\n", MUIC_DEV_NAME, __func__,
				ret);
		return sprintf(buf, "UNKNOWN\n");
	}

	return sprintf(buf, "%x\n", ret);
}

static ssize_t muic_show_usb_state(struct device *dev,
					    struct device_attribute *attr,
					    char *buf)
{
	muic_data_t *pmuic = dev_get_drvdata(dev);
	int mdev = muic_resolve_attached_dev(pmuic);

	switch (mdev) {
	case ATTACHED_DEV_USB_MUIC:
	case ATTACHED_DEV_CDP_MUIC:
	case ATTACHED_DEV_JIG_USB_OFF_MUIC:
	case ATTACHED_DEV_JIG_USB_ON_MUIC:
		return sprintf(buf, "USB_STATE_CONFIGURED\n");
	default:
		break;
	}

	return 0;
}

#if defined(CONFIG_USB_HOST_NOTIFY)
static ssize_t muic_show_otg_test(struct device *dev,
					   struct device_attribute *pattr,
					   char *buf)
{
	muic_data_t *pmuic = dev_get_drvdata(dev);
	struct regmap_ops *pops = pmuic->regmapdesc->regmapops;
	int uattr;
	u8 val = 0;

	mutex_lock(&pmuic->muic_mutex);
	pops->ioctl(pmuic->regmapdesc, GET_OTG_STATUS, NULL, &uattr);
	val = regmap_read_value(pmuic->regmapdesc, uattr);
	mutex_unlock(&pmuic->muic_mutex);

	pr_info("%s val:%x buf%s\n", __func__, val, buf);

	if (val < 0) {
		pr_err("%s: fail to read muic reg\n", __func__);
		return sprintf(buf, "UNKNOWN\n");
	}

	return sprintf(buf, "%x\n", val);
}

static ssize_t muic_set_otg_test(struct device *dev,
		struct device_attribute *pattr,
		const char *buf, size_t count)
{
	muic_data_t *pmuic = dev_get_drvdata(dev);
	struct regmap_ops *pops = pmuic->regmapdesc->regmapops;
	struct vendor_ops *pvendor = pmuic->regmapdesc->vendorops;
	struct reg_attr attr;
	int uattr;
	u8 val;

	pr_info("%s buf:%s\n", __func__, buf);
	if (!strncmp(buf, "0", 1)) {
		val = 0;
		pmuic->is_otg_test = true;
	} else if (!strncmp(buf, "1", 1)) {
		val = 1;
		pmuic->is_otg_test = false;
	} else {
		pr_warn("%s:%s Wrong command\n", MUIC_DEV_NAME, __func__);
		return count;
	}

	if (pvendor && pvendor->start_otg_test) {
		mutex_lock(&pmuic->muic_mutex);
		pvendor->start_otg_test(pmuic->regmapdesc, pmuic->is_otg_test);
		mutex_unlock(&pmuic->muic_mutex);

	} else {
		mutex_lock(&pmuic->muic_mutex);
		pops->ioctl(pmuic->regmapdesc, GET_OTG_STATUS, NULL, &uattr);
		_REG_ATTR(&attr, uattr);

		val = muic_i2c_read_byte(pmuic->i2c, attr.addr);
		val |= attr.mask << attr.bitn;
		val |= _ATTR_OVERWRITE_M;
		val = regmap_write_value(pmuic->regmapdesc, uattr, val);
		mutex_unlock(&pmuic->muic_mutex);

		if (val < 0) {
			pr_err("%s err writing %s reg(%d)\n", __func__,
				regmap_to_name(pmuic->regmapdesc, attr.addr), val);
		}

		val = muic_i2c_read_byte(pmuic->i2c, attr.addr);
		val &= (attr.mask << attr.bitn);
		pr_info("%s: %s(0x%02x)\n", __func__,
			regmap_to_name(pmuic->regmapdesc, attr.addr), val);
	}

	return count;
}
#endif

#if defined(CONFIG_SEC_DEBUG) || defined(CONFIG_SEC_NAD)
static ssize_t muic_show_usb_to_ta(struct device *dev,
					   struct device_attribute *pattr,
					   char *buf)
{
	muic_data_t *pmuic = dev_get_drvdata(dev);
	struct vendor_ops *pvendor = pmuic->regmapdesc->vendorops;
	int val;

	if (pvendor->usb_to_ta)
		val = pvendor->usb_to_ta(pmuic->regmapdesc, 2);
	else {
		pr_err("%s: No Vendor API ready.\n", __func__);
		val = -EINVAL;
	}

	pr_info("%s:USB TO TA status %s\n", __func__, val?"Enable":"Disable");
	return sprintf(buf,"USB TO TA status %s\n", val?"Enable":"Disable");
}

static ssize_t muic_set_usb_to_ta(struct device *dev,
		struct device_attribute *pattr,
		const char *buf, size_t count)
{
	muic_data_t *pmuic = dev_get_drvdata(dev);
	struct vendor_ops *pvendor = pmuic->regmapdesc->vendorops;
	u8 val;

	pr_info("%s buf:%s\n", __func__, buf);
	if (!strncmp(buf, "0", 1)) {
		if (pvendor->usb_to_ta)
			val = pvendor->usb_to_ta(pmuic->regmapdesc, 0);
		else {
			pr_err("%s: No Vendor API ready.\n", __func__);
			val = -EINVAL;
		}
	} else if (!strncmp(buf, "1", 1)) {
		if (pvendor->usb_to_ta)
			val = pvendor->usb_to_ta(pmuic->regmapdesc, 1);
		else {
			pr_err("%s: No Vendor API ready.\n", __func__);
			val = -EINVAL;
		}
	} else {
		pr_warn("%s:%s Wrong command\n", MUIC_DEV_NAME, __func__);
		return count;
	}

	return count;
}
#endif

static ssize_t muic_show_attached_dev(struct device *dev,
					 struct device_attribute *attr,
					 char *buf)
{
	muic_data_t *pmuic = dev_get_drvdata(dev);
	int mdev = muic_resolve_attached_dev(pmuic);

	pr_info("%s:%s attached_dev:%d\n", MUIC_DEV_NAME, __func__,
			mdev);

	switch(mdev) {
	case ATTACHED_DEV_NONE_MUIC:
		return sprintf(buf, "No VPS\n");
	case ATTACHED_DEV_USB_MUIC:
		return sprintf(buf, "USB\n");
	case ATTACHED_DEV_CDP_MUIC:
		return sprintf(buf, "CDP\n");
	case ATTACHED_DEV_OTG_MUIC:
		return sprintf(buf, "OTG\n");
	case ATTACHED_DEV_TA_MUIC:
		return sprintf(buf, "TA\n");
	case ATTACHED_DEV_JIG_UART_OFF_MUIC:
		return sprintf(buf, "JIG UART OFF\n");
	case ATTACHED_DEV_JIG_UART_OFF_VB_MUIC:
		return sprintf(buf, "JIG UART OFF/VB\n");
	case ATTACHED_DEV_JIG_UART_ON_MUIC:
		return sprintf(buf, "JIG UART ON\n");
	case ATTACHED_DEV_JIG_UART_ON_VB_MUIC:
		return sprintf(buf, "JIG UART ON/VB\n");
	case ATTACHED_DEV_JIG_USB_OFF_MUIC:
		return sprintf(buf, "JIG USB OFF\n");
	case ATTACHED_DEV_JIG_USB_ON_MUIC:
		return sprintf(buf, "JIG USB ON\n");
	case ATTACHED_DEV_DESKDOCK_MUIC:
		return sprintf(buf, "DESKDOCK\n");
	case ATTACHED_DEV_AUDIODOCK_MUIC:
		return sprintf(buf, "AUDIODOCK\n");
	case ATTACHED_DEV_CHARGING_CABLE_MUIC:
		return sprintf(buf, "PS CABLE\n");
	default:
		break;
	}

	return sprintf(buf, "UNKNOWN\n");
}

static ssize_t muic_show_audio_path(struct device *dev,
					     struct device_attribute *attr,
					     char *buf)
{
	return 0;
}

static ssize_t muic_set_audio_path(struct device *dev,
					    struct device_attribute *attr,
					    const char *buf, size_t count)
{
	return 0;
}

static ssize_t muic_show_apo_factory(struct device *dev,
					     struct device_attribute *attr,
					     char *buf)
{
	muic_data_t *pmuic = dev_get_drvdata(dev);
	const char *mode;

	/* true: Factory mode, false: not Factory mode */
	if (pmuic->is_factory_start)
		mode = "FACTORY_MODE";
	else
		mode = "NOT_FACTORY_MODE";

	pr_info("%s:%s apo factory=%s\n", MUIC_DEV_NAME, __func__, mode);

	return sprintf(buf, "%s\n", mode);
}

static ssize_t muic_set_apo_factory(struct device *dev,
				struct device_attribute *attr,
				const char *buf, size_t count)
{
	muic_data_t *pmuic = dev_get_drvdata(dev);
	const char *mode;

	pr_info("%s:%s buf:%s\n", MUIC_DEV_NAME, __func__, buf);

	/* "FACTORY_START": factory mode */
	if (!strncmp(buf, "FACTORY_START", 13)) {
		pmuic->is_factory_start = true;
		mode = "FACTORY_MODE";
	} else {
		pr_warn("%s:%s Wrong command\n", MUIC_DEV_NAME, __func__);
		return count;
	}

	pr_info("%s:%s apo factory=%s\n", MUIC_DEV_NAME, __func__, mode);

	return count;
}
#if defined(CONFIG_SEC_FACTORY)
static ssize_t muic_set_jig_on(struct device *dev,
				struct device_attribute *attr,
				const char *buf, size_t count)
{
	muic_data_t *pmuic = dev_get_drvdata(dev);
	struct vendor_ops *pvendor = pmuic->regmapdesc->vendorops;

	pr_info("%s:%s buf:%s\n", MUIC_DEV_NAME, __func__, buf);

	if (!strncmp(buf, "1", 1)) {
		set_switch_mode(pmuic,SWMODE_MANUAL);
		if (pvendor->set_jig_on)
			pvendor->set_jig_on(pmuic->regmapdesc, 1);
	} else if (!strncmp(buf, "0", 1)) {
		set_switch_mode(pmuic,SWMODE_AUTO);
		if (pvendor->set_jig_on)
			pvendor->set_jig_on(pmuic->regmapdesc, 0);
	} else {
		pr_warn("%s:%s Wrong command\n", MUIC_DEV_NAME, __func__);
		return count;
	}

	return count;
}
#endif
static ssize_t muic_show_vbus_value(struct device *dev,
				struct device_attribute *attr,
				char *buf)
{
	muic_data_t *pmuic = dev_get_drvdata(dev);
	struct vendor_ops *pvendor = pmuic->regmapdesc->vendorops;
	int val;

	if (pvendor->get_vbus_value) {
		mutex_lock(&pmuic->muic_mutex);
		val = pvendor->get_vbus_value(pmuic->regmapdesc, 0);
		mutex_unlock(&pmuic->muic_mutex);
	} else {
		pr_err("%s: No Vendor API ready.\n", __func__);
		val = -EINVAL;
	}

	pr_info("%s:%s VBUS:%d\n", MUIC_DEV_NAME, __func__, val);

	if (val > 0)
		return sprintf(buf, "%dV\n", val);

	return sprintf(buf, "UNKNOWN\n");
}

static ssize_t muic_show_vbus_value_pd(struct device *dev,
				struct device_attribute *attr,
				char *buf)
{
	muic_data_t *pmuic = dev_get_drvdata(dev);
	struct vendor_ops *pvendor = pmuic->regmapdesc->vendorops;
	int val;

	if (pvendor->get_vbus_value)
		val = pvendor->get_vbus_value(pmuic->regmapdesc, 1);
	else {
		pr_err("%s: No Vendor API ready.\n", __func__);
		val = -EINVAL;
	}

	pr_info("%s:%s VBUS:%d\n", MUIC_DEV_NAME, __func__, val);

	if (val > 0)
		return sprintf(buf, "%dV\n", val);

	return sprintf(buf, "UNKNOWN\n");
}

static ssize_t muic_show_vbus_rawdata(struct device *dev,
				struct device_attribute *attr,
				char *buf)
{
	muic_data_t *pmuic = dev_get_drvdata(dev);
	struct vendor_ops *pvendor = pmuic->regmapdesc->vendorops;
	int val;

	if (pvendor->get_vbus_rawdata)
		val = pvendor->get_vbus_rawdata(pmuic->regmapdesc);
	else {
		pr_err("%s: No Vendor API ready.\n", __func__);
		val = -EINVAL;
	}

	pr_info("%s:%s VBUS:%d\n", MUIC_DEV_NAME, __func__, val);

	if (val > 0)
		return sprintf(buf, "%d\n", val);

	return sprintf(buf, "UNKNOWN\n");
}

#if defined(CONFIG_MUIC_HV) || defined(CONFIG_SUPPORT_QC30)
static ssize_t muic_show_afc_disable(struct device *dev,
		struct device_attribute *attr, char *buf)
{
	muic_data_t *pmuic = dev_get_drvdata(dev);
	struct muic_platform_data *pdata = pmuic->pdata;

	if (pdata->afc_disable) {
		pr_info("%s:%s AFC DISABLE\n", MUIC_DEV_NAME, __func__);
		return sprintf(buf, "1\n");
	}

	pr_info("%s:%s AFC ENABLE", MUIC_DEV_NAME, __func__);
	return sprintf(buf, "0\n");
}

extern int muic_set_afc(bool enable);
static ssize_t muic_set_afc_disable(struct device *dev,
				    struct device_attribute *attr,
				    const char *buf, size_t count)
{
	muic_data_t *pmuic = dev_get_drvdata(dev);
	struct muic_platform_data *pdata = pmuic->pdata;
	bool curr_val = pdata->afc_disable;
	unsigned int param_val;
#if defined(CONFIG_MUIC_HV) || defined(CONFIG_SUPPORT_QC30)
	union power_supply_propval psy_val;
#endif
	int ret = 0;

	/* Disable AFC */
	if (!strncasecmp(buf, "1", 1)) {
		pdata->afc_disable = true;
	} else if (!strncasecmp(buf, "0", 1)) {
	/* Enable AFC */
		pdata->afc_disable = false;
	} else {
		pr_warn("%s:%s invalid value\n", MUIC_DEV_NAME, __func__);
		return count;
	}

	param_val = pdata->afc_disable ? '1' : '0';
	pr_info("%s: param_val:%d\n",__func__,param_val);
	ret = sec_set_param(param_index_afc_disable, &param_val);

	if (ret == false) {
		pr_err("%s:set_param failed - %02x:%02x(%d)\n", __func__,
			param_val, curr_val, ret);
		pdata->afc_disable = curr_val;
		return ret;
	}else{
		pr_info("%s:%s afc_disable:%d (AFC %s)\n", MUIC_DEV_NAME, __func__,
			pdata->afc_disable, pdata->afc_disable ? "Disabled": "Enabled");
	}

#if defined(CONFIG_MUIC_HV)
	psy_val.intval = param_val;
	psy_do_property("battery", set,
		POWER_SUPPLY_EXT_PROP_HV_DISABLE, psy_val);
#endif

#if defined(CONFIG_SUPPORT_QC30)
	psy_val.intval = param_val;
	psy_do_property("smb1351-charger", set,
			POWER_SUPPLY_PROP_VOLTAGE_MAX_DESIGN, psy_val);

#else
	/* for factory self charging test (AFC-> NORMAL TA) */
	if (pmuic->is_factory_start) {
		muic_set_afc(pdata->afc_disable ? false: true);

		run_chgdet(pmuic, true);
		run_chgdet(pmuic, false);
	}
#endif
	return count;
}
#if defined(CONFIG_MUIC_HV)
extern int muic_afc_set_voltage(int vol);
static ssize_t muic_store_afc_set_voltage(struct device *dev,
				    struct device_attribute *attr,
				    const char *buf, size_t count)
{
	if (!strncasecmp(buf, "5V", 2)) {
		muic_afc_set_voltage(5);
	} else if (!strncasecmp(buf, "9V", 2)) {
		muic_afc_set_voltage(9);
	} else if (!strncasecmp(buf, "12V", 3)) {
		muic_afc_set_voltage(12);
	} else {
		pr_warn("%s:%s invalid value\n", MUIC_DEV_NAME, __func__);
		return count;
	}

	return count;
}
#endif
#if defined(CONFIG_MUIC_HV_FORCE_LIMIT)
static ssize_t muic_show_hv_sel(struct device *dev,
		struct device_attribute *attr, char *buf)
{
	muic_data_t *pmuic = dev_get_drvdata(dev);
	struct muic_platform_data *pdata = pmuic->pdata;

	if (pdata->hv_sel) {
		pr_info("%s:%s AFC OFF\n", MUIC_DEV_NAME, __func__);
		return sprintf(buf, "1\n");
	}

	pr_info("%s:%s AFC OFF", MUIC_DEV_NAME, __func__);
	return sprintf(buf, "0\n");
}

static ssize_t muic_set_hv_sel(struct device *dev,
				    struct device_attribute *attr,
				    const char *buf, size_t count)
{
	muic_data_t *pmuic = dev_get_drvdata(dev);
	struct muic_platform_data *pdata = pmuic->pdata;

	/* Disable AFC */
	if (!strncasecmp(buf, "5V", 2)) {
		if(pdata->hv_sel == 1) {
			pr_info("%s:%s off 9V called twice [%d]\n",	MUIC_DEV_NAME, __func__, pdata->hv_sel);
			return count;
		}
		pdata->hv_sel = true;
	} else if (!strncasecmp(buf, "9V", 2)) {
		if(pdata->hv_sel == 0) {
			pr_info("%s:%s off 5V called twice [%d]\n",	MUIC_DEV_NAME, __func__, pdata->hv_sel);
			return count;
		}
	/* Enable AFC */
		pdata->hv_sel = false;
	} else {
		pr_warn("%s:%s invalid value\n", MUIC_DEV_NAME, __func__);
		return count;
	}

	pr_info("%s:%s hv_sel:%d (AFC %s)\n", MUIC_DEV_NAME, __func__,
			pdata->hv_sel, pdata->hv_sel ? "Disabled": "Enabled");

	hv_muic_change_afc_voltage(pmuic, pdata->hv_sel ? MUIC_HV_5V : MUIC_HV_9V);

	return count;
}
#endif
#endif /* CONFIG_MUIC_HV */

#if defined(CONFIG_MUIC_SUPPORT_CCIC)
static ssize_t cc_xx_show(struct device *dev,
					   struct device_attribute *attr,
					   char *buf)
{
	const char *mode = "UNKNOWN\n";

	pr_info("%s:%s %s", MUIC_DEV_NAME, __func__, mode);

	return sprintf(buf, mode);
}

static ssize_t cc_xx_set(struct device *dev,
					  struct device_attribute *attr,
					  const char *buf, size_t count)
{
	muic_data_t *pmuic = dev_get_drvdata(dev);
	int mid = 0, rid = 0;
	int ret = 0;

	pr_info("%s:%s buf=%s\n", MUIC_DEV_NAME, __func__, buf);

	if (buf[1] != ':') {
		pr_err("A wrong Input Fomat!\n");
		return count;
	}

	if ((buf[0]=='A') || (buf[0]=='a'))
		mid = 1; /* ATTACH */

	else if ((buf[0]=='R') || (buf[0]=='r'))
		mid = 2; /* RID */

	else if ((buf[0]=='M') || (buf[0]=='m'))
		mid = 100; /* MODE */
	else {
		pr_err("Undefined Command!\n");
		return count;
	}

	ret = kstrtoint(buf + 2, 0, &rid);
	if (ret) {
		pr_err("%s: Undefined rid\n", __func__);
		goto err;
	}

	if (mid == 100) {
		pmuic->opmode  = rid ? OPMODE_CCIC : OPMODE_MUIC;
		pr_info("%s:%s OP_MODE=%d\n", MUIC_DEV_NAME, __func__, pmuic->opmode);
		if (pmuic->opmode & OPMODE_CCIC) {
			muic_notifier_set_new_noti(true);
			muic_register_ccic_notifier(pmuic);
		}

		return count;
	}

	muic_ccic_pseudo_noti(mid, rid);

err:
	return count;
}

static DEVICE_ATTR(cc_xx, 0664, cc_xx_show,
		cc_xx_set);
#endif

static DEVICE_ATTR(uart_en, 0664, muic_show_uart_en, muic_set_uart_en);
static DEVICE_ATTR(uart_sel, 0664, muic_show_uart_sel,
		muic_set_uart_sel);
static DEVICE_ATTR(usb_sel, 0664,
		muic_show_usb_sel, muic_set_usb_sel);
static DEVICE_ATTR(adc, 0664, muic_show_adc, NULL);
static DEVICE_ATTR(usb_state, 0664, muic_show_usb_state, NULL);
#if defined(CONFIG_USB_HOST_NOTIFY)
static DEVICE_ATTR(otg_test, 0664,
		muic_show_otg_test, muic_set_otg_test);
#endif
#if defined(CONFIG_SEC_DEBUG) || defined(CONFIG_SEC_NAD)
static DEVICE_ATTR(usb_to_ta, 0664,
		muic_show_usb_to_ta, muic_set_usb_to_ta);
#endif
static DEVICE_ATTR(attached_dev, 0664, muic_show_attached_dev, NULL);
static DEVICE_ATTR(audio_path, 0664,
		muic_show_audio_path, muic_set_audio_path);
static DEVICE_ATTR(apo_factory, 0664,
		muic_show_apo_factory,
		muic_set_apo_factory);
#if defined(CONFIG_SEC_FACTORY)
static DEVICE_ATTR(jig_on, 0664, NULL, muic_set_jig_on);
#endif
static DEVICE_ATTR(vbus_value, 0444, muic_show_vbus_value, NULL);
static DEVICE_ATTR(vbus_value_pd, 0444, muic_show_vbus_value_pd, NULL);
static DEVICE_ATTR(vbus_rawdata, 0444, muic_show_vbus_rawdata, NULL);
#if defined(CONFIG_MUIC_HV) || defined(CONFIG_SUPPORT_QC30)
static DEVICE_ATTR(afc_disable, 0664,
		muic_show_afc_disable, muic_set_afc_disable);
#if defined(CONFIG_MUIC_HV_FORCE_LIMIT)
static DEVICE_ATTR(hv_sel, 0664,
		muic_show_hv_sel, muic_set_hv_sel);
#endif
#if defined(CONFIG_MUIC_HV)
static DEVICE_ATTR(afc_set_voltage, 0664,
		NULL, muic_store_afc_set_voltage);
#endif
#endif

static struct attribute *muic_attributes[] = {
#if defined(CONFIG_MUIC_SUPPORT_CCIC)
	&dev_attr_cc_xx.attr,
#endif
	&dev_attr_uart_en.attr,
	&dev_attr_uart_sel.attr,
	&dev_attr_usb_sel.attr,
	&dev_attr_adc.attr,
	&dev_attr_usb_state.attr,
#if defined(CONFIG_USB_HOST_NOTIFY)
	&dev_attr_otg_test.attr,
#endif
#if defined(CONFIG_SEC_DEBUG) || defined(CONFIG_SEC_NAD)
	&dev_attr_usb_to_ta.attr,
#endif
	&dev_attr_attached_dev.attr,
	&dev_attr_audio_path.attr,
	&dev_attr_apo_factory.attr,
#if defined(CONFIG_SEC_FACTORY)
	&dev_attr_jig_on.attr,
#endif
	&dev_attr_vbus_value.attr,
	&dev_attr_vbus_value_pd.attr,
	&dev_attr_vbus_rawdata.attr,
#if defined(CONFIG_MUIC_HV) || defined(CONFIG_SUPPORT_QC30)
	&dev_attr_afc_disable.attr,
#if defined(CONFIG_MUIC_HV)
	&dev_attr_afc_set_voltage.attr,
#endif
#if defined(CONFIG_MUIC_HV_FORCE_LIMIT)
	&dev_attr_hv_sel.attr,
#endif
#endif
	NULL
};

const struct attribute_group muic_sysfs_group = {
	.attrs = muic_attributes,
};
