/*
 *  sm5720_haptic.c
 *  Samsung SM5720 Haptic Driver
 *
 *  Copyright (C) 2016 Samsung Electronics
 *
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */


#include <linux/mfd/sm5720.h>
#include <linux/mfd/sm5720-private.h>

struct sm5720_haptic_data {
	struct sm5720_dev *sm5720;
	struct i2c_client *i2c;
	struct sm5720_haptic_platform_data *pdata;

	bool running;
};
static struct sm5720_haptic_data *sm5720_g_hap_data;

enum {
	HAPTIC_MOTOR_LRA    = 0x0,
	HAPTIC_MOTOR_ERM,
};

enum {
	HAPTIC_VDD_External = 0x0,
	HAPTIC_VDD_Internal,
};

/*
 *	SM5720 Haptic register control functions
 */

static int sm5720_set_haptic_enable(struct sm5720_haptic_data *haptic, bool enable)
{
	sm5720_update_reg(haptic->i2c, SM5720_CHG_REG_HAPTICCNTL, ((enable & 0x1) << 0), (0x1 << 0));

	return 0;
}

static int sm5720_set_haptic_motor_sel(struct sm5720_haptic_data *haptic, bool motor_type)
{
	sm5720_update_reg(haptic->i2c, SM5720_CHG_REG_HAPTICCNTL, ((motor_type & 0x1) << 1), (motor_type << 1));

	return 0;
}

static int sm5720_set_haptic_VDD(struct sm5720_haptic_data *haptic, bool select)
{
	sm5720_update_reg(haptic->i2c, SM5720_CHG_REG_HAPTICCNTL, ((select & 0x1) << 2), (0x1 << 2));

	return 0;
}

/*
 *	SM5720 Haptic External interface functions
 */

#if defined(CONFIG_SS_VIBRATOR)
void sm5720_vibtonz_en(bool en)
{
	if (sm5720_g_hap_data == NULL)
		return;

	if (en) {
		if (sm5720_g_hap_data->running)
			return;
		sm5720_set_haptic_enable(sm5720_g_hap_data, true);

		sm5720_g_hap_data->running = true;
		pr_info("[VIB] %s: running!\n", __func__);
	} else {
		if (!sm5720_g_hap_data->running)
			return;
		sm5720_set_haptic_enable(sm5720_g_hap_data, false);

		sm5720_g_hap_data->running = false;
		pr_info("[VIB] %s: stop!\n", __func__);
	}
}
EXPORT_SYMBOL(sm5720_vibtonz_en);
#endif

/**
 * SM5720 Haptic platform driver handling functions
 */

#if defined(CONFIG_OF)
static struct sm5720_haptic_platform_data *of_sm5720_haptic_dt(struct device *dev)
{
	struct device_node *np = dev->parent->of_node;
	struct sm5720_haptic_platform_data *pdata;

	pdata = kzalloc(sizeof(struct sm5720_haptic_platform_data), GFP_KERNEL);
	if (pdata == NULL)
		return NULL;

	if (!of_property_read_u32(np, "haptic,mode", &pdata->mode))
		pdata->mode = 1;
	if (!of_property_read_u32(np, "haptic,divisor", &pdata->divisor))
		pdata->divisor = 128;
	pr_info("[VIB] %s: mode: %d\n", __func__, pdata->mode);
	pr_info("[VIB] %s: divisor: %d\n", __func__, pdata->divisor);

	return pdata;
}
#endif

static int sm5720_haptic_probe(struct platform_device *pdev)
{
	int error = 0;
	struct sm5720_dev *sm5720 = dev_get_drvdata(pdev->dev.parent);
	struct sm5720_platform_data *sm5720_pdata = dev_get_platdata(sm5720->dev);
	struct sm5720_haptic_platform_data *pdata = sm5720_pdata->haptic_data;
	struct sm5720_haptic_data *hap_data;

#if defined(CONFIG_OF)
	if (pdata == NULL)
		pdata = of_sm5720_haptic_dt(&pdev->dev);
#endif

	pr_err("[VIB] ++ %s\n", __func__);
	if (pdata == NULL) {
		pr_err("[VIB] %s: no pdata\n", __func__);
		return -ENODEV;
	}

	hap_data = kzalloc(sizeof(struct sm5720_haptic_data), GFP_KERNEL);
	if (!hap_data) {
		kfree(pdata);
		return -ENOMEM;
	}

	sm5720_g_hap_data = hap_data;
	hap_data->sm5720 = sm5720;
	hap_data->i2c = sm5720->charger;
	hap_data->pdata = pdata;
	platform_set_drvdata(pdev, hap_data);

	sm5720_set_haptic_VDD(hap_data, HAPTIC_VDD_External);
	sm5720_set_haptic_motor_sel(hap_data, HAPTIC_MOTOR_LRA);

	pr_err("[VIB] -- %s\n", __func__);

	return error;
}

static int sm5720_haptic_remove(struct platform_device *pdev)
{
	struct sm5720_haptic_data *haptic = platform_get_drvdata(pdev);

	sm5720_set_haptic_enable(haptic, 0);
	kfree(haptic);
	sm5720_g_hap_data = NULL;

	return 0;
}

static void sm5720_haptic_shutdown(struct platform_device *pdev)
{
	struct sm5720_haptic_data *haptic = platform_get_drvdata(pdev);

	pr_info("[VIB] %s: Disable HAPTIC\n", __func__);
	sm5720_set_haptic_enable(haptic, 0);
}

static int sm5720_haptic_suspend(struct platform_device *pdev,
			pm_message_t state)
{
	return 0;
}
static int sm5720_haptic_resume(struct platform_device *pdev)
{
	return 0;
}


static struct platform_driver sm5720_haptic_driver = {
	.probe		= sm5720_haptic_probe,
	.remove		= sm5720_haptic_remove,
	.suspend	= sm5720_haptic_suspend,
	.resume		= sm5720_haptic_resume,
	.shutdown	= sm5720_haptic_shutdown,
	.driver = {
		.name	= "sm5720-haptic",
		.owner	= THIS_MODULE,
	},
};

static int __init sm5720_haptic_init(void)
{
	return platform_driver_register(&sm5720_haptic_driver);
}
module_init(sm5720_haptic_init);

static void __exit sm5720_haptic_exit(void)
{
	platform_driver_unregister(&sm5720_haptic_driver);
}
module_exit(sm5720_haptic_exit);

MODULE_DESCRIPTION("Samsung SM5720 Charger Driver");
MODULE_AUTHOR("Samsung Electronics");
MODULE_LICENSE("GPL");
