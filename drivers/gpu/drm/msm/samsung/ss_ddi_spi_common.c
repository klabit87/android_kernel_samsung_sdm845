/*
 * =================================================================
 *
 *
 *	Description:  samsung display common file
 *
 *	Author: samsung display driver team
 *	Company:  Samsung Electronics
 *
 * ================================================================
 */

#include "ss_ddi_spi_common.h"

static struct spi_device *ss_spi;
static DEFINE_MUTEX(ss_spi_lock);
static int ddi_spi_status = DDI_SPI_RESUME;

#define SPI_CTRL_RX 0x00

int ss_spi_read(struct spi_device *spi, u8 rxaddr, u8 *buf, int size)
{
	u64 i;
	u16 cmd_addr;
	u8 rx_buf[256];
#if defined(CONFIG_SEC_FACTORY)
	u8 dummy_buf;
#endif

	struct spi_message msg;
	int ret = 0;

	struct spi_transfer xfer[] = {
		{ .bits_per_word = 9,	.len = 2,		.tx_buf = &cmd_addr, },
		{ .bits_per_word = 8,	.len = size,	.rx_buf = &rx_buf, },
#if defined(CONFIG_SEC_FACTORY)
		{ .bits_per_word = 8,	.len = 1,		.rx_buf = &dummy_buf, },
#endif
	};

	mutex_lock(&ss_spi_lock);

	if (ddi_spi_status == DDI_SPI_SUSPEND) {
		LCD_DEBUG("ddi spi is suspend..\n");
		ret = -EINVAL;
		goto err;
	}

	LCD_DEBUG("++\n");

	if (!spi) {
		LCD_ERR("no spi device..\n");
		goto err;
	}

	cmd_addr = (0x0 << 8) | rxaddr;

	spi_message_init(&msg);

	for (i = 0; i < ARRAY_SIZE(xfer); i++)
		spi_message_add_tail(&xfer[i], &msg);

	ret = spi_sync(spi, &msg);
	if (ret) {
		pr_err("[mdss spi] %s : spi_sync fail..\n", __func__);
		goto err;
	}

	LCD_DEBUG("rx(0x%x) : ", rxaddr);
	for (i = 0; i < size; i++) {
		buf[i] = rx_buf[i];
		LCD_DEBUG("%02x ", buf[i]);
	}
	LCD_DEBUG("\n");

	LCD_DEBUG("--\n");

err:
	mutex_unlock(&ss_spi_lock);

	return ret;
}

static int ss_spi_probe(struct spi_device *client)
{
	int ret = 0;
	struct device_node *np;
	struct samsung_display_driver_data *vdd = samsung_get_vdd();

	if (client == NULL) {
		pr_err("[mdss] %s : ss_spi_probe spi is null\n", __func__);
		return -EINVAL;
	}

	LCD_INFO("chip select(%d), bus number(%d)\n",
			client->chip_select, client->master->bus_num);

	np = client->master->dev.of_node;
	if (!np) {
		LCD_ERR("of_node is null..\n");
		return -ENODEV;
	}

	LCD_ERR("np name : %s\n", np->full_name);

	client->max_speed_hz = 12000000;
	client->bits_per_word = 9;
	client->mode =  SPI_MODE_0;

	LCD_INFO("max speed (%d), bpw (%d), mode (%d) \n",
			client->max_speed_hz, client->bits_per_word, client->mode);

	ret = spi_setup(client);
	if (ret < 0) {
		pr_err("[mdss] %s : spi_setup error (%d)\n", __func__, ret);
		return ret;
	}

	ss_spi = client;
	vdd->spi_dev = ss_spi;

	pr_info("[mdss] %s : --\n", __func__);
	return ret;
}

static int ss_spi_remove(struct spi_device *spi)
{
	pr_err("[mdss] %s : remove \n", __func__);
	return 0;
}

static int ddi_spi_suspend(struct device *dev)
{
	mutex_lock(&ss_spi_lock);
	ddi_spi_status = DDI_SPI_SUSPEND;
	LCD_DEBUG(" %d\n", ddi_spi_status);
	mutex_unlock(&ss_spi_lock);

	return 0;
}

static int ddi_spi_resume(struct device *dev)
{
	mutex_lock(&ss_spi_lock);
	ddi_spi_status = DDI_SPI_RESUME;
	LCD_DEBUG(" %d\n", ddi_spi_status);
	mutex_unlock(&ss_spi_lock);

	return 0;
}

static int ddi_spi_reboot_cb(struct notifier_block *nb,
			unsigned long action, void *data)
{
	mutex_lock(&ss_spi_lock);
	ddi_spi_status = DDI_SPI_SUSPEND;
	LCD_ERR(" %d\n", ddi_spi_status);
	mutex_unlock(&ss_spi_lock);

	return NOTIFY_OK;
}

static const struct dev_pm_ops ddi_spi_pm_ops = {
	.suspend = ddi_spi_suspend,
	.resume = ddi_spi_resume,
};

static const struct of_device_id ddi_spi_match_table[] = {
	{   .compatible = "ss,ddi-spi",
	},
	{}
};

static struct spi_driver ddi_spi_driver = {
	.driver = {
		.name		= DRIVER_NAME,
		.owner		= THIS_MODULE,
		.of_match_table = ddi_spi_match_table,
		.pm	= &ddi_spi_pm_ops,
	},
	.probe		= ss_spi_probe,
	.remove		= ss_spi_remove,
};

static struct notifier_block ss_spi_reboot_notifier = {
	.notifier_call = ddi_spi_reboot_cb,
};

static int __init ss_spi_init(void)
{
	int ret;

	pr_info("[mdss] %s : ++\n", __func__);

	ret = spi_register_driver(&ddi_spi_driver);
	if (ret)
		pr_err("[mdss] %s : ddi spi register fail : %d\n", __func__, ret);

	register_reboot_notifier(&ss_spi_reboot_notifier);

	pr_info("[mdss] %s : --\n", __func__);
	return ret;
}
module_init(ss_spi_init);

static void __exit ss_spi_exit(void)
{
	spi_unregister_driver(&ddi_spi_driver);
}
module_exit(ss_spi_exit);

MODULE_DESCRIPTION("spi interface driver for ddi");
MODULE_LICENSE("GPL");
