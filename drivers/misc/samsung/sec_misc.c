/*
 * drivers/soc/samsung/sec_misc.c
 *
 * COPYRIGHT(C) 2006-2011 Samsung Electronics Co., Ltd. All Right Reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

#define pr_fmt(fmt)     KBUILD_MODNAME ":%s() " fmt, __func__

#include <linux/module.h>
#include <linux/device.h>
#include <linux/miscdevice.h>
#include <linux/blkdev.h>
#include <linux/sec_class.h>

#define QFPROM_CORR_PTE_ROW0_LSB_ID		0x00784130
#define POWER_DOU_ADDR			0x00780320


static struct device *sec_misc_dev;

static ssize_t msm_feature_id_show(struct device *dev,
		struct device_attribute *attr, char *buf)
{
	uint32_t feature_id;
	void *feature_id_addr;

	feature_id_addr = ioremap_nocache(QFPROM_CORR_PTE_ROW0_LSB_ID, SZ_4K);
	if (!feature_id_addr) {
		pr_err("could not map FEATURE_ID address\n");
		return scnprintf(buf, PAGE_SIZE,
				"could not map FEATURE_ID address\n");
	}

	feature_id = readl_relaxed(feature_id_addr);
	iounmap(feature_id_addr);

	if (!feature_id)
		return scnprintf(buf, PAGE_SIZE,
				"Feature ID is not supported!!\n");


	feature_id = (feature_id >> 20) & 0xff;
	pr_debug("FEATURE_ID : 0x%08x\n", feature_id);

	return scnprintf(buf, PAGE_SIZE, "%02d\n", feature_id);
}
static DEVICE_ATTR(msm_feature_id, 0444, msm_feature_id_show, NULL);
/*  End of Feature ID */

#ifdef CONFIG_SEC_FACTORY
static char *cpr_iddq_info;
static int __init cpr_iddq_info_setup(char *str)
{
	if (str)
		cpr_iddq_info = str;
	pr_err("sec_misc: can't get cpr_iddq_info\n");
	return 0;
}
__setup("QCOM.CPR_IDDQ_INFO=", cpr_iddq_info_setup);

static ssize_t msm_apc_avs_info_show(struct device *dev,
		struct device_attribute *attr, char *buf)
{
	if (cpr_iddq_info) {
		pr_debug("msm cpr_iddq_info : %s\n", cpr_iddq_info);
		return scnprintf(buf, PAGE_SIZE, "%s\n", cpr_iddq_info);
	}

	return scnprintf(buf, PAGE_SIZE, "Not Support!\n");
}

static DEVICE_ATTR(msm_apc_avs_info, 0644, msm_apc_avs_info_show, NULL);

static ssize_t msm_dour_show(struct device *dev,
		struct device_attribute *attr, char *buf)
{
	uint32_t power_dou;
	void *power_dou_addr;

	power_dou_addr = ioremap_nocache(POWER_DOU_ADDR, SZ_4K);
	if (!power_dou_addr) {
		pr_err("could not map POWER_DOU address\n");
		return scnprintf(buf, PAGE_SIZE,
				"could not map POWER_DOU address\n");
	}

	power_dou = readl_relaxed(power_dou_addr);
	iounmap(power_dou_addr);

	if (!power_dou)
		return scnprintf(buf, PAGE_SIZE,
				"POWER DOU is not supported!!\n");


	power_dou = (power_dou & 0x200000) >> 0x15;

	pr_debug("POWER DOU REVISION : 0x%08x\n", power_dou);

	return scnprintf(buf, PAGE_SIZE, "%02d\n", power_dou);
}

static DEVICE_ATTR(msm_dour, 0444, msm_dour_show, NULL);

static ssize_t msm_doub_show(struct device *dev,
				struct device_attribute *attr, char *buf)
{
	uint32_t power_dou;
	void *power_dou_addr;

	power_dou_addr = ioremap_nocache(POWER_DOU_ADDR, SZ_4K);
	if (!power_dou_addr) {
		pr_err("could not map POWER_DOU address\n");
		return scnprintf(buf, PAGE_SIZE,
				"could not map POWER_DOU address\n");
	}

	power_dou = readl_relaxed(power_dou_addr);
	iounmap(power_dou_addr);

	if (!power_dou)
		return scnprintf(buf, PAGE_SIZE,
				"POWER DOU is not supported!!\n");


	power_dou = (power_dou & 0x180000) >> 0x13;

	pr_debug("POWER DOU REVISION : 0x%08x\n", power_dou);

	return scnprintf(buf, PAGE_SIZE, "%02d\n", power_dou);
}

static DEVICE_ATTR(msm_doub, 0444, msm_doub_show, NULL);
#endif

static struct attribute *sec_misc_attrs[] = {
	&dev_attr_msm_feature_id.attr,
#ifdef CONFIG_SEC_FACTORY
	&dev_attr_msm_apc_avs_info.attr,
	&dev_attr_msm_dour.attr,
	&dev_attr_msm_doub.attr,
#endif
	NULL,
};

static const struct attribute_group sec_misc_attr_group = {
	.attrs = sec_misc_attrs,
};

static const struct file_operations sec_misc_fops = {
	.owner = THIS_MODULE,
};

static struct miscdevice sec_misc_device = {
	.minor = MISC_DYNAMIC_MINOR,
	.name = "sec_misc",
	.fops = &sec_misc_fops,
};

static int __init sec_misc_init(void)
{
	int ret;

	ret = misc_register(&sec_misc_device);
	if (unlikely(ret < 0)) {
		pr_err("misc_register failed!\n");
		goto failed_register_misc;
	}

	sec_misc_dev = sec_device_create(0, NULL, "sec_misc");
	if (unlikely(IS_ERR(sec_misc_dev))) {
		pr_err("failed to create device!\n");
		ret = PTR_ERR(sec_misc_dev);
		goto failed_create_device;
	}

	ret = sysfs_create_group(&sec_misc_dev->kobj, &sec_misc_attr_group);
	if (unlikely(ret)) {
		pr_err("Failed to create device files!\n");
		goto failed_create_device_file;
	}

	return 0;

failed_create_device_file:
failed_create_device:
	misc_deregister(&sec_misc_device);
failed_register_misc:
	return ret;
}

module_init(sec_misc_init);

/* Module information */
MODULE_AUTHOR("Samsung");
MODULE_DESCRIPTION("Samsung misc. driver");
MODULE_LICENSE("GPL");
