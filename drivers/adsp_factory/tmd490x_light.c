/*
 *  Copyright (C) 2012, Samsung Electronics Co. Ltd. All Rights Reserved.
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 */
#include <linux/init.h>
#include <linux/module.h>
#include <linux/dirent.h>
#include "adsp.h"
#define VENDOR "AMS"
#if defined(CONFIG_SUPPORT_TMD4904_FACTORY)
#define CHIP_ID "TMD4904"
#elif defined(CONFIG_SUPPORT_TMD4906_FACTORY)
#define CHIP_ID "TMD4906"
#else
#define CHIP_ID "TMD4903"
#endif

/*************************************************************************/
/* factory Sysfs							 */
/*************************************************************************/
static ssize_t light_vendor_show(struct device *dev,
	struct device_attribute *attr, char *buf)
{
	return snprintf(buf, PAGE_SIZE, "%s\n", VENDOR);
}

static ssize_t light_name_show(struct device *dev,
	struct device_attribute *attr, char *buf)
{
	return snprintf(buf, PAGE_SIZE, "%s\n", CHIP_ID);
}

static ssize_t light_raw_data_show(struct device *dev,
	struct device_attribute *attr, char *buf)
{
	struct adsp_data *data = dev_get_drvdata(dev);
	uint8_t cnt = 0;

	adsp_unicast(NULL, 0, MSG_LIGHT, 0, MSG_TYPE_GET_RAW_DATA);

	while (!(data->ready_flag[MSG_TYPE_GET_RAW_DATA] & 1 << MSG_LIGHT) &&
		cnt++ < TIMEOUT_CNT)
		usleep_range(500, 550);

	data->ready_flag[MSG_TYPE_GET_RAW_DATA] &= ~(1 << MSG_LIGHT);

	if (cnt >= TIMEOUT_CNT) {
		pr_err("[FACTORY] %s: Timeout!!!\n", __func__);
		return snprintf(buf, PAGE_SIZE, "0,0,0,0,0,0\n");
	}

	return snprintf(buf, PAGE_SIZE, "%d,%d,%d,%d,%d,%d\n",
		data->msg_buf[MSG_LIGHT][0], data->msg_buf[MSG_LIGHT][1],
		data->msg_buf[MSG_LIGHT][2], data->msg_buf[MSG_LIGHT][3],
		data->msg_buf[MSG_LIGHT][4], data->msg_buf[MSG_LIGHT][5]);
}

static DEVICE_ATTR(vendor, 0444, light_vendor_show, NULL);
static DEVICE_ATTR(name, 0444, light_name_show, NULL);
static DEVICE_ATTR(lux, 0444, light_raw_data_show, NULL);
static DEVICE_ATTR(raw_data, 0444, light_raw_data_show, NULL);

static struct device_attribute *light_attrs[] = {
	&dev_attr_vendor,
	&dev_attr_name,
	&dev_attr_lux,
	&dev_attr_raw_data,
	NULL,
};

static int __init tmd490x_light_factory_init(void)
{
	adsp_factory_register(MSG_LIGHT, light_attrs);
	pr_info("[FACTORY] %s\n", __func__);

	return 0;
}

static void __exit tmd490x_light_factory_exit(void)
{
	adsp_factory_unregister(MSG_LIGHT);
	pr_info("[FACTORY] %s\n", __func__);
}
module_init(tmd490x_light_factory_init);
module_exit(tmd490x_light_factory_exit);
