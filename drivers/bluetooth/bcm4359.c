/*
 * Bluetooth Broadcom GPIO and Low Power Mode control
 *
 *  Copyright (C) 2011 Samsung Electronics Co., Ltd.
 *  Copyright (C) 2011 Google, Inc.
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
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */

#define DEBUG

#include <linux/kernel.h>
#include <linux/delay.h>
#include <linux/gpio.h>
#include <linux/hrtimer.h>
#include <linux/init.h>
#include <linux/interrupt.h>
#include <linux/irq.h>
#include <linux/module.h>
#include <linux/platform_device.h>
#include <linux/rfkill.h>
#include <linux/wakelock.h>
#include <linux/of_gpio.h>

#include <linux/gpio.h>
#include <linux/of_gpio.h>
#include <linux/of_irq.h>

#include <linux/slab.h>
#include <linux/pinctrl/consumer.h>


static struct rfkill *bt_rfkill;
static int cnt = 0;

struct bluetooth_bcm_platform_data {
   struct pinctrl *bt_pinctrl;
   //struct pinctrl_state *bt_state_default;

   /* Bluetooth reset gpio */
   int bt_gpio_bt_en;
};

static struct of_device_id bt_dt_match_table[] = {
	{	.compatible = "bcm,btdriver" },
	{}
};

static struct bluetooth_bcm_platform_data *bt_power_pdata = NULL;


static int bcm4359_bt_rfkill_set_power(void *data, bool blocked)
{
    if (!blocked) {
		pr_err("[BT] Bluetooth Power On (%d)\n", bt_power_pdata->bt_gpio_bt_en);

		gpio_set_value(bt_power_pdata->bt_gpio_bt_en, 1);

    } else {
        pr_err("[BT] Bluetooth Power Off.\n");

		gpio_set_value(bt_power_pdata->bt_gpio_bt_en, 0);
    }

    return 0;
}

static const struct rfkill_ops bcm4359_bt_rfkill_ops = {
    .set_block = bcm4359_bt_rfkill_set_power,
};

static int bt_populate_dt_pinfo(struct platform_device *pdev)
{
	pr_err("[BT] bcm4359 dt_info \n");

	if (!bt_power_pdata)
		return -ENOMEM;

	if (pdev->dev.of_node) {
		bt_power_pdata->bt_gpio_bt_en =
			of_get_named_gpio(pdev->dev.of_node,
						"bcm,bt-reset-gpio", 0);

		if (bt_power_pdata->bt_gpio_bt_en < 0) {
			pr_err("bt-reset-gpio not provided in device tree");
			return bt_power_pdata->bt_gpio_bt_en;
		}
		pr_err("[BT] bt_en pin is %d\n", bt_power_pdata->bt_gpio_bt_en);
	}
	return 0;
}


static int bcm4359_bluetooth_probe(struct platform_device *pdev)
{
    int rc = 0;
	int ret;

	pr_err("[BT] bcm4359_bluetooth_probe\n");

	if (!bt_power_pdata)
		bt_power_pdata =
				kzalloc(sizeof(struct bluetooth_bcm_platform_data),	GFP_KERNEL);

	if (!bt_power_pdata) {
		pr_err("Failed to allocate memory");
		return -ENOMEM;
	}

	if (pdev->dev.of_node) {
		ret = bt_populate_dt_pinfo(pdev);
		if (ret < 0) {
			pr_err("[BT] Failed to populate device tree info\n");
			return ret;
		}
		pdev->dev.platform_data = bt_power_pdata;

		bt_power_pdata->bt_pinctrl = devm_pinctrl_get(&pdev->dev);
		if (IS_ERR(bt_power_pdata->bt_pinctrl)) {
			pr_err("[BT] Failed to get pinctrl\n");
		} else {
			pr_err("[BT] bcm4359 Succ pinctrl\n");
		}
	}

	rc = gpio_request(bt_power_pdata->bt_gpio_bt_en, "bcm4359_bten_gpio");
    if (rc)
    {
        pr_err("[BT] %s: gpio_request for GPIO_BT_EN is failed", __func__);
        gpio_free(bt_power_pdata->bt_gpio_bt_en);
    }

    bt_rfkill = rfkill_alloc("bcm4359 Bluetooth", &pdev->dev,
                RFKILL_TYPE_BLUETOOTH, &bcm4359_bt_rfkill_ops,
                NULL);

    if (unlikely(!bt_rfkill)) {
        pr_err("[BT] bt_rfkill alloc failed.\n");
        return -ENOMEM;
    }

    rfkill_init_sw_state(bt_rfkill, 0);

    rc = rfkill_register(bt_rfkill);

    if (unlikely(rc)) {
        pr_err("[BT] bt_rfkill register failed.\n");
        rfkill_destroy(bt_rfkill);
        return rc;
    }

    rfkill_set_sw_state(bt_rfkill, true);

    return rc;
}

static int bcm4359_bluetooth_remove(struct platform_device *pdev)
{
    rfkill_unregister(bt_rfkill);
    rfkill_destroy(bt_rfkill);

    gpio_free(bt_power_pdata->bt_gpio_bt_en);

	cnt = 0;
    return 0;
}

static struct platform_driver bcm4359_bluetooth_platform_driver = {
    .probe = bcm4359_bluetooth_probe,
    .remove = bcm4359_bluetooth_remove,
    .driver = {
        .name = "bcm4359_bluetooth",
        .owner = THIS_MODULE,
        .of_match_table = bt_dt_match_table,
    },
};

static int __init bcm4359_bluetooth_init(void)
{
	int ret;

	pr_err("[BT] bcm4359_bluetooth_init\n");

    ret = platform_driver_register(&bcm4359_bluetooth_platform_driver);
	if (ret) {
		pr_err("[BT] bcm4359_bluetooth_init failed\n");
	}
	return ret;
}

static void __exit bcm4359_bluetooth_exit(void)
{
    platform_driver_unregister(&bcm4359_bluetooth_platform_driver);
}

module_init(bcm4359_bluetooth_init);
module_exit(bcm4359_bluetooth_exit);

MODULE_ALIAS("platform:bcm4359");
MODULE_DESCRIPTION("bcm4359_bluetooth");
MODULE_LICENSE("GPL");
