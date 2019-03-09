/*
 *  sm5720.c
 *  Samsung mfd core driver for the SM5720.
 *
 *  Copyright (C) 2016 Samsung Electronics
 *
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#include <linux/module.h>
#include <linux/slab.h>
#include <linux/i2c.h>
#include <linux/irq.h>
#include <linux/interrupt.h>
#include <linux/mutex.h>
#include <linux/mfd/core.h>
#include <linux/mfd/sm5720.h>
#include <linux/mfd/sm5720-private.h>
#include <linux/regulator/machine.h>

#if defined (CONFIG_OF)
#include <linux/of_device.h>
#include <linux/of.h>
#include <linux/of_gpio.h>
#include <linux/of_irq.h>
#include <linux/pinctrl/consumer.h>
#endif /* CONFIG_OF */

#define I2C_ADDR_MUIC	(0x4A >> 1)
#define I2C_ADDR_CHG    (0x92 >> 1)
#define I2C_ADDR_FG     (0xE2 >> 1)

static struct mfd_cell sm5720_devs[] = {
#if defined(CONFIG_MUIC_UNIVERSAL)
	{ .name = "muic-universal", },
#endif
#if defined(CONFIG_MUIC_SM5720)
	{ .name = MUIC_DEV_NAME, },
#endif
#if defined(CONFIG_FUELGAUGE_SM5720)
	{ .name = "sm5720-fuelgauge", },
#endif
#if defined(CONFIG_CHARGER_SM5720)
	{ .name = "sm5720-charger", },
#endif
#if defined(CONFIG_MOTOR_DRV_SM5720)
	{ .name = "sm5720-haptic", },
#endif
#if defined(CONFIG_LEDS_SM5720_RGB)
	{ .name = "leds-sm5720-rgb", },
#endif
};

static struct sm5720_dev *g_sm5720_dev = NULL;

int sm5720_read_reg(struct i2c_client *i2c, u8 reg, u8 *dest)
{
	struct sm5720_dev *sm5720 = i2c_get_clientdata(i2c);
	int ret;

	mutex_lock(&sm5720->i2c_lock);
	ret = i2c_smbus_read_byte_data(i2c, reg);
	mutex_unlock(&sm5720->i2c_lock);
	if (ret < 0) {
		pr_info("%s:%s reg(0x%x), ret(%d)\n", MFD_DEV_NAME, __func__, reg, ret);
		return ret;
	}
	*dest = (ret & 0xff);

	return 0;
}
EXPORT_SYMBOL(sm5720_read_reg);

int sm5720_bulk_read(struct i2c_client *i2c, u8 reg, int count, u8 *buf)
{
	struct sm5720_dev *sm5720 = i2c_get_clientdata(i2c);
	int ret;

	mutex_lock(&sm5720->i2c_lock);
	ret = i2c_smbus_read_i2c_block_data(i2c, reg, count, buf);
	mutex_unlock(&sm5720->i2c_lock);
	if (ret < 0)
		return ret;

	return 0;
}
EXPORT_SYMBOL(sm5720_bulk_read);

int sm5720_read_word(struct i2c_client *i2c, u8 reg)
{
	struct sm5720_dev *sm5720 = i2c_get_clientdata(i2c);
	int ret;

	mutex_lock(&sm5720->i2c_lock);
	ret = i2c_smbus_read_word_data(i2c, reg);
	mutex_unlock(&sm5720->i2c_lock);
	if (ret < 0)
		return ret;

	return ret;
}
EXPORT_SYMBOL(sm5720_read_word);

int sm5720_write_reg(struct i2c_client *i2c, u8 reg, u8 value)
{
	struct sm5720_dev *sm5720 = i2c_get_clientdata(i2c);
	int ret;

	mutex_lock(&sm5720->i2c_lock);
	ret = i2c_smbus_write_byte_data(i2c, reg, value);
	mutex_unlock(&sm5720->i2c_lock);
	if (ret < 0)
		pr_info("%s:%s reg(0x%x), ret(%d)\n", MFD_DEV_NAME, __func__, reg, ret);

	return ret;
}
EXPORT_SYMBOL(sm5720_write_reg);

int sm5720_bulk_write(struct i2c_client *i2c, u8 reg, int count, u8 *buf)
{
	struct sm5720_dev *sm5720 = i2c_get_clientdata(i2c);
	int ret;

	mutex_lock(&sm5720->i2c_lock);
	ret = i2c_smbus_write_i2c_block_data(i2c, reg, count, buf);
	mutex_unlock(&sm5720->i2c_lock);
	if (ret < 0)
		return ret;

	return 0;
}
EXPORT_SYMBOL(sm5720_bulk_write);

int sm5720_write_word(struct i2c_client *i2c, u8 reg, u16 value)
{
	struct sm5720_dev *sm5720 = i2c_get_clientdata(i2c);
	int ret;

	mutex_lock(&sm5720->i2c_lock);
	ret = i2c_smbus_write_word_data(i2c, reg, value);
	mutex_unlock(&sm5720->i2c_lock);
	if (ret < 0)
		return ret;
	return 0;
}
EXPORT_SYMBOL(sm5720_write_word);

int sm5720_update_reg(struct i2c_client *i2c, u8 reg, u8 val, u8 mask)
{
	struct sm5720_dev *sm5720 = i2c_get_clientdata(i2c);
	int ret;

	mutex_lock(&sm5720->i2c_lock);
	ret = i2c_smbus_read_byte_data(i2c, reg);
	if (ret >= 0) {
		u8 old_val = ret & 0xff;
		u8 new_val = (val & mask) | (old_val & (~mask));
		ret = i2c_smbus_write_byte_data(i2c, reg, new_val);
	}
	mutex_unlock(&sm5720->i2c_lock);
	return ret;
}
EXPORT_SYMBOL(sm5720_update_reg);

int sm5720_get_device_ID(void)
{
	int revision;

	if (g_sm5720_dev == NULL) {
		return -EBUSY;
	}

	revision = sm5720_read_word(g_sm5720_dev->fuelgauge, SM5720_FG_REG_DEVICE_ID) & 0xFF;

	pr_info("%s: revision=%d\n", __func__, revision);

	return revision;
}
EXPORT_SYMBOL(sm5720_get_device_ID);

#if defined(CONFIG_OF)
static int of_sm5720_dt(struct device *dev, struct sm5720_platform_data *pdata)
{
	struct device_node *np_sm5720 = dev->of_node;
	struct platform_device *pdev = to_platform_device(dev);

	if(!np_sm5720)
		return -EINVAL;

	pdata->irq_gpio = platform_get_irq_byname(pdev, "sm5720_irq");
	pr_info("%s: irq: %u \n", __func__, pdata->irq_gpio);

	pdata->wakeup = of_property_read_bool(np_sm5720, "sm5720,wakeup");

	return 0;
}

static int sm5720_pinctrl_select(struct sm5720_dev *sm5720, bool on) {
	struct pinctrl_state *pins_i2c_state;
	int ret;

	pins_i2c_state = on ? sm5720->i2c_gpio_state_active : sm5720->i2c_gpio_state_suspend;

	if (!IS_ERR_OR_NULL(pins_i2c_state)) {
		ret = pinctrl_select_state(sm5720->i2c_pinctrl, pins_i2c_state);
		if(ret) {
			dev_err(sm5720->dev, "%s: fail to set sm5720 i2c pin state.\n", __func__);
			return ret;
		}
	}

	return 0;
}

static int sm5720_i2c_pinctrl_init(struct sm5720_dev *sm5720) {
	int ret = 0;
	struct pinctrl *i2c_pinctrl;

	sm5720->i2c_pinctrl = devm_pinctrl_get(&sm5720->i2c->dev);
	if (IS_ERR_OR_NULL(sm5720->i2c_pinctrl)) {
		dev_err(sm5720->dev, "%s: fail to alloc mem for sm5720 i2c pinctrl.\n", __func__);
		ret = PTR_ERR(sm5720->i2c_pinctrl);
		return -ENOMEM;
	}
	i2c_pinctrl = sm5720->i2c_pinctrl;

	sm5720->i2c_gpio_state_active = pinctrl_lookup_state(i2c_pinctrl, "muic_active");
	if (IS_ERR_OR_NULL(sm5720->i2c_gpio_state_active)) {
		dev_err(sm5720->dev, "%s: fail to set active state for sm5720 i2c\n", __func__);
		ret = PTR_ERR(sm5720->i2c_gpio_state_active);
		goto err_i2c_active_state;
	}

	sm5720->i2c_gpio_state_suspend = pinctrl_lookup_state(i2c_pinctrl, "muic_suspend");
	if (IS_ERR_OR_NULL(sm5720->i2c_gpio_state_suspend)) {
		dev_err(sm5720->dev, "%s: fail to set suspend state for sm5720 i2c\n", __func__);
		ret = PTR_ERR(sm5720->i2c_gpio_state_suspend);
		goto err_i2c_suspend_state;
	}

#if defined(CONFIG_OF)
	sm5720_pinctrl_select(sm5720, true);
#endif
	return ret;

err_i2c_suspend_state:
	sm5720->i2c_gpio_state_suspend = 0;
err_i2c_active_state:
	sm5720->i2c_gpio_state_active = 0;
	return ret;
}
#else
static int of_sm5720_dt(struct device *dev, struct sm5720_platform_data *pdata)
{
	return 0;
}
#endif /* CONFIG_OF */

static int sm5720_i2c_probe(struct i2c_client *i2c, const struct i2c_device_id *dev_id)
{
	struct sm5720_dev *sm5720;
	struct sm5720_platform_data *pdata = i2c->dev.platform_data;

	u8 reg_data;
	int ret = 0;

	pr_info("%s:%s\n", MFD_DEV_NAME, __func__);

	sm5720 = kzalloc(sizeof(struct sm5720_dev), GFP_KERNEL);
	if (!sm5720) {
		dev_err(&i2c->dev, "%s: fail to alloc mem for sm5720\n", __func__);
		return -ENOMEM;
	}

	if (i2c->dev.of_node) {
		pdata = devm_kzalloc(&i2c->dev, sizeof(struct sm5720_platform_data), GFP_KERNEL);
		if (!pdata) {
			dev_err(&i2c->dev, "fail to allocate memory \n");
			ret = -ENOMEM;
			goto err;
		}

		ret = of_sm5720_dt(&i2c->dev, pdata);
		if (ret < 0){
			dev_err(&i2c->dev, "fail to get device of_node \n");
			goto err;
		}

		i2c->dev.platform_data = pdata;
	}

	sm5720->dev = &i2c->dev;
	sm5720->i2c = sm5720->charger = i2c;
	sm5720->irq = i2c->irq;

	if (pdata) {
		sm5720->pdata = pdata;

		pdata->irq_base = irq_alloc_descs(-1, 0, SM5720_IRQ_NR, -1);
		if (pdata->irq_base < 0) {
			pr_err("%s:%s irq_alloc_descs Fail! ret(%d)\n", MFD_DEV_NAME, __func__, pdata->irq_base);
			ret = -EINVAL;
			goto err;
		} else {
			sm5720->irq_base = pdata->irq_base;
		}

		sm5720->irq = pdata->irq_gpio;
		sm5720->wakeup = pdata->wakeup;
	} else {
		ret = -EINVAL;
		goto err;
	}

	mutex_init(&sm5720->i2c_lock);

	i2c_set_clientdata(i2c, sm5720);

    /* Check vender ID & Charger I2C transmission */
    ret = sm5720_read_reg(i2c, SM5720_CHG_REG_DEVICEID, &reg_data);
	if (ret < 0 || reg_data != 0x1) {
		pr_err("%s:%s device not found on this channel (reg_data=0x%x)\n", MFD_DEV_NAME, __func__, reg_data);
		ret = -ENODEV;
		goto err_w_lock;
	}

#if defined(CONFIG_OF)
	/* Initialize pinctrl for i2c & irq */
	ret = sm5720_i2c_pinctrl_init(sm5720);
	if(ret) {
        pr_err("%s:%s sm5720 pinctrl is failed.\n", MFD_DEV_NAME, __func__);
		/* if pnictrl is not supported, -EINVAL is returned*/
		if(ret == -EINVAL)
			ret = 0;
	} else {
		pr_info("%s:%s sm5720 pinctrl is succeed.\n",	MFD_DEV_NAME, __func__);
	}
#endif

	sm5720->fuelgauge = i2c_new_dummy(i2c->adapter, I2C_ADDR_FG);
	i2c_set_clientdata(sm5720->fuelgauge, sm5720);
    /* Check FG I2C transmission */
    ret = sm5720_read_word(sm5720->fuelgauge, SM5720_FG_REG_DEVICE_ID);
    if ((unsigned int)ret > 0xFF) {
		pr_err("%s:%s fail to setup FG I2C transmission (ret=0x%x)\n", MFD_DEV_NAME, __func__, ret);
		ret = -ENODEV;
		goto err_w_lock;
    }

	sm5720->muic = i2c_new_dummy(i2c->adapter, I2C_ADDR_MUIC);
	i2c_set_clientdata(sm5720->muic, sm5720);
    /* Check MUIC I2C transmission */
    ret = sm5720_read_reg(sm5720->muic, SM5720_MUIC_REG_DEVICE_ID, &reg_data);
	if (ret < 0 || reg_data != 0x1) {
		pr_err("%s:%s fail to setup MUIC I2C transmission (reg_data=0x%x)\n", MFD_DEV_NAME, __func__, reg_data);
		ret = -ENODEV;
		goto err_w_lock;
	}

	ret = sm5720_irq_init(sm5720);

	if (ret < 0)
		goto err_irq_init;

	ret = mfd_add_devices(sm5720->dev, -1, sm5720_devs, ARRAY_SIZE(sm5720_devs), NULL, 0, NULL);
	if (ret < 0)
		goto err_mfd;

	device_init_wakeup(sm5720->dev, pdata->wakeup);

	g_sm5720_dev = sm5720;

	pr_info("%s: %s done (rev:%d)\n", MFD_DEV_NAME, __func__, sm5720_get_device_ID());

	return ret;

err_mfd:
	mfd_remove_devices(sm5720->dev);
	sm5720_irq_exit(sm5720);
err_irq_init:
	i2c_unregister_device(sm5720->muic);
	i2c_unregister_device(sm5720->fuelgauge);
err_w_lock:
	mutex_destroy(&sm5720->i2c_lock);
err:
	kfree(sm5720);

	return ret;
}

static int sm5720_i2c_remove(struct i2c_client *i2c)
{
	struct sm5720_dev *sm5720 = i2c_get_clientdata(i2c);

	mfd_remove_devices(sm5720->dev);
	sm5720_irq_exit(sm5720);

	i2c_unregister_device(sm5720->muic);
	i2c_unregister_device(sm5720->fuelgauge);

	mutex_destroy(&sm5720->i2c_lock);

	kfree(sm5720);

	return 0;
}

static const struct i2c_device_id sm5720_i2c_id[] = {
	{ MFD_DEV_NAME, TYPE_SM5720 },
	{ }
};
MODULE_DEVICE_TABLE(i2c, sm5720_i2c_id);

#if defined(CONFIG_OF)
static struct of_device_id sm5720_i2c_dt_ids[] = {
	{ .compatible = "siliconmitus,sm5720" },
	{ },
};
MODULE_DEVICE_TABLE(of, sm5720_i2c_dt_ids);
#endif /* CONFIG_OF */

#if defined(CONFIG_PM)
static int sm5720_suspend(struct device *dev)
{
	struct i2c_client *i2c = container_of(dev, struct i2c_client, dev);
	struct sm5720_dev *sm5720 = i2c_get_clientdata(i2c);

	if (device_may_wakeup(dev))
		enable_irq_wake(sm5720->irq);

	disable_irq(sm5720->irq);

	return 0;
}

static int sm5720_resume(struct device *dev)
{
	struct i2c_client *i2c = container_of(dev, struct i2c_client, dev);
	struct sm5720_dev *sm5720 = i2c_get_clientdata(i2c);

#if !defined(CONFIG_SAMSUNG_PRODUCT_SHIP)
	pr_info("%s:%s\n", MFD_DEV_NAME, __func__);
#endif /* CONFIG_SAMSUNG_PRODUCT_SHIP */

	if (device_may_wakeup(dev))
		disable_irq_wake(sm5720->irq);

	enable_irq(sm5720->irq);

	return 0;
}
#else
#define sm5720_suspend	    NULL
#define sm5720_resume		NULL
#endif /* CONFIG_PM */

const struct dev_pm_ops sm5720_pm = {
	.suspend    = sm5720_suspend,
	.resume     = sm5720_resume,
};

static struct i2c_driver sm5720_i2c_driver = {
	.driver		= {
		.name	= MFD_DEV_NAME,
		.owner	= THIS_MODULE,
#if defined(CONFIG_PM)
		.pm	    = &sm5720_pm,
#endif /* CONFIG_PM */
#if defined(CONFIG_OF)
		.of_match_table	= sm5720_i2c_dt_ids,
#endif /* CONFIG_OF */
	},
	.probe		= sm5720_i2c_probe,
	.remove		= sm5720_i2c_remove,
	.id_table	= sm5720_i2c_id,
};

static int __init sm5720_i2c_init(void)
{
	pr_info("%s:%s\n", MFD_DEV_NAME, __func__);
	return i2c_add_driver(&sm5720_i2c_driver);
}
/* init early so consumer devices can complete system boot */
subsys_initcall(sm5720_i2c_init);

static void __exit sm5720_i2c_exit(void)
{
	i2c_del_driver(&sm5720_i2c_driver);
}
module_exit(sm5720_i2c_exit);

MODULE_DESCRIPTION("SM5720 multi-function core driver");
MODULE_AUTHOR("Samsung Electronics");
MODULE_LICENSE("GPL");
