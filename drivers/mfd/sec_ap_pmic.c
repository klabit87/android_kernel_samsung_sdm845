/*
 * Copyright (c) 2015, The Linux Foundation. All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 and
 * only version 2 as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 */

#include <linux/init.h>
#include <linux/module.h>
#include <linux/sec_class.h>
#include <linux/device.h>
#include <linux/input/qpnp-power-on.h>
#include <linux/sec_crashkey.h>

/* for enable/disable manual reset, from retail group's request */
extern void do_keyboard_notifier(int onoff);

static struct device *sec_ap_pmic_dev;


static ssize_t manual_reset_show(struct device *in_dev,
				struct device_attribute *attr, char *buf)
{
	int ret = 0;

	ret = qpnp_get_s2_reset_onoff();

	pr_info("%s: ret = %d\n", __func__, ret);
	return sprintf(buf, "%d\n", ret);
}

static ssize_t manual_reset_store(struct device *in_dev,
		struct device_attribute *attr, const char *buf, size_t len)
{
	int onoff = 0;

	if (kstrtoint(buf, 10, &onoff))
		return -EINVAL;

	pr_info("%s: onoff(%d)\n", __func__, onoff);

	do_keyboard_notifier(onoff);
	qpnp_control_s2_reset_onoff(onoff);

	return len;
}
static DEVICE_ATTR_RW(manual_reset);


static ssize_t chg_det_show(struct device *in_dev,
				struct device_attribute *attr, char *buf)
{
	int ret = 0;

	ret = qpnp_pon_check_chg_det();

	pr_info("%s: ret = %d\n", __func__, ret);
	return sprintf(buf, "%d\n", ret);
}
static DEVICE_ATTR_RO(chg_det);

static struct attribute *sec_ap_pmic_attributes[] = {
	&dev_attr_chg_det.attr,
	&dev_attr_manual_reset.attr,
	NULL,
};

static struct attribute_group sec_ap_pmic_attr_group = {
	.attrs = sec_ap_pmic_attributes,
};

static int __init sec_ap_pmic_init(void)
{
	int err;

	sec_ap_pmic_dev = sec_device_create(0, NULL, "ap_pmic");

	if (unlikely(IS_ERR(sec_ap_pmic_dev))) {
		pr_err("%s: Failed to create ap_pmic device\n", __func__);
		err = PTR_ERR(sec_ap_pmic_dev);
		goto err_device_create;
	}

	err = sysfs_create_group(&sec_ap_pmic_dev->kobj,
				&sec_ap_pmic_attr_group);
	if (err < 0) {
		pr_err("%s: Failed to create sysfs group\n", __func__);
		goto err_device_create;
	}

	pr_info("%s: ap_pmic successfully inited.\n", __func__);

	return 0;

err_device_create:
	return err;
}
module_init(sec_ap_pmic_init);

MODULE_DESCRIPTION("sec_ap_pmic driver");
MODULE_AUTHOR("Jiman Cho <jiman85.cho@samsung.com");
MODULE_LICENSE("GPL");
