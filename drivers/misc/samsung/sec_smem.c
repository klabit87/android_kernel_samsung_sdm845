/*
 * drivers/misc/samsung/sec_smem.c
 *
 * COPYRIGHT(C) 2015-2016 Samsung Electronics Co., Ltd. All Right Reserved.
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

#define pr_fmt(fmt)     KBUILD_MODNAME ":%s() " fmt, __func__

#include <linux/module.h>
#include <linux/platform_device.h>
#include <linux/pm.h>
#include <soc/qcom/smem.h>
#include <linux/sec_smem.h>
#include <linux/notifier.h>
#include <linux/sec_debug_partition.h>

#ifdef CONFIG_SEC_DEBUG_APPS_CLK_LOGGING
extern void *clk_osm_get_log_addr(void);
#endif

#define SUSPEND	0x1
#define RESUME	0x0

static ap_health_t *p_health;

static char *lpddr4_manufacture_name[] = {
	"NA",
	"SEC"/* Samsung */,
	"NA",
	"NA",
	"NA",
	"NAN" /* Nanya */,
	"HYN" /* SK hynix */,
	"NA",
	"WIN" /* Winbond */,
	"ESM" /* ESMT */,
	"NA",
	"NA",
	"NA",
	"NA",
	"NA",
	"MIC" /* Micron */,
};

static void *__get_ddr_smem_entry(unsigned int id)
{
	unsigned int size;
	void *entry;

	entry = smem_get_entry(id, &size, SMEM_APPS, SMEM_ANY_HOST_FLAG);
	if (!size) {
		pr_err("entry size can not be zero\n");
		return NULL;
	}

	return entry;
}

uint8_t get_ddr_info(uint8_t type) {
	sec_smem_id_vendor0_v2_t *vendor0 = __get_ddr_smem_entry(SMEM_ID_VENDOR0);
	
	if (!vendor0) {
		pr_err("SMEM_ID_VENDOR0 get entry error\n");
		return 0;
	}

	return (vendor0->ddr_vendor >> type) & 0xFF;
}

char *get_ddr_vendor_name(void)
{
	sec_smem_id_vendor0_v2_t *vendor0;
	size_t lpddr4_manufacture;

	vendor0 = __get_ddr_smem_entry(SMEM_ID_VENDOR0);
	if (!vendor0) {
		pr_err("SMEM_ID_VENDOR0 get entry error\n");
		return 0;
	}

	lpddr4_manufacture =
		vendor0->ddr_vendor % ARRAY_SIZE(lpddr4_manufacture_name);
	return lpddr4_manufacture_name[lpddr4_manufacture];
}
EXPORT_SYMBOL(get_ddr_vendor_name);

uint32_t get_ddr_DSF_version(void)
{
	sec_smem_id_vendor1_v4_t *vendor1;

	vendor1 = __get_ddr_smem_entry(SMEM_ID_VENDOR1);
	if (!vendor1) {
		pr_err("SMEM_ID_VENDOR1 get entry error\n");
		return 0;
	}

	return vendor1->ddr_training.version;
}
EXPORT_SYMBOL(get_ddr_DSF_version);

uint8_t get_ddr_revision_id_1(void)
{
	return get_ddr_info(DDR_IFNO_REVISION_ID1);
}
EXPORT_SYMBOL(get_ddr_revision_id_1);

uint8_t get_ddr_revision_id_2(void)
{
	return get_ddr_info(DDR_IFNO_REVISION_ID2);
}
EXPORT_SYMBOL(get_ddr_revision_id_2);

uint8_t get_ddr_total_density(void)
{
	return get_ddr_info(DDR_IFNO_TOTAL_DENSITY);
}
EXPORT_SYMBOL(get_ddr_total_density);

uint8_t get_ddr_rcw_tDQSCK(uint32_t ch, uint32_t cs, uint32_t dq)
{
	sec_smem_id_vendor1_v4_t *vendor1;

	vendor1 = __get_ddr_smem_entry(SMEM_ID_VENDOR1);
	if (!vendor1) {
		pr_err("SMEM_ID_VENDOR1 get entry error\n");
		return 0;
	}

	return vendor1->ddr_training.rcw.tDQSCK[ch][cs][dq];
}
EXPORT_SYMBOL(get_ddr_rcw_tDQSCK);

uint8_t get_ddr_wr_coarseCDC(uint32_t ch, uint32_t cs, uint32_t dq)
{
	sec_smem_id_vendor1_v4_t *vendor1;

	vendor1 = __get_ddr_smem_entry(SMEM_ID_VENDOR1);
	if (!vendor1) {
		pr_err("SMEM_ID_VENDOR1 get entry error\n");
		return 0;
	}

	return vendor1->ddr_training.wr_dqdqs.coarse_cdc[ch][cs][dq];
}
EXPORT_SYMBOL(get_ddr_wr_coarseCDC);

uint8_t get_ddr_wr_fineCDC(uint32_t ch, uint32_t cs, uint32_t dq)
{
	sec_smem_id_vendor1_v4_t *vendor1;

	vendor1 = __get_ddr_smem_entry(SMEM_ID_VENDOR1);
	if (!vendor1) {
		pr_err("SMEM_ID_VENDOR1 get entry error\n");
		return 0;
	}

	return vendor1->ddr_training.wr_dqdqs.fine_cdc[ch][cs][dq];
}
EXPORT_SYMBOL(get_ddr_wr_fineCDC);

// SED
uint8_t ddr_get_wr_pr_width(uint32_t ch, uint32_t cs, uint32_t dq)
{
	sec_smem_id_vendor1_v4_t *vendor1;

	vendor1 = __get_ddr_smem_entry(SMEM_ID_VENDOR1);
	if (!vendor1) {
		pr_err("SMEM_ID_VENDOR1 get entry error\n");
		return 0;
	}

	return vendor1->ddr_training.dqdqs_eye.wr_pr_width[ch][cs][dq];
}
EXPORT_SYMBOL(ddr_get_wr_pr_width);

uint8_t ddr_get_wr_min_eye_height(uint32_t ch, uint32_t cs)
{
	sec_smem_id_vendor1_v4_t *vendor1;

	vendor1 = __get_ddr_smem_entry(SMEM_ID_VENDOR1);
	if (!vendor1) {
		pr_err("SMEM_ID_VENDOR1 get entry error\n");
		return 0;
	}

	return vendor1->ddr_training.dqdqs_eye.wr_min_eye_height[ch][cs];
}
EXPORT_SYMBOL(ddr_get_wr_min_eye_height);

uint8_t ddr_get_wr_best_vref(uint32_t ch, uint32_t cs)
{
	sec_smem_id_vendor1_v4_t *vendor1;

	vendor1 = __get_ddr_smem_entry(SMEM_ID_VENDOR1);
	if (!vendor1) {
		pr_err("SMEM_ID_VENDOR1 get entry error\n");
		return 0;
	}

	return vendor1->ddr_training.dqdqs_eye.wr_best_vref[ch][cs];
}
EXPORT_SYMBOL(ddr_get_wr_best_vref);

uint8_t ddr_get_wr_vmax_to_vmid(uint32_t ch, uint32_t cs, uint32_t dq)
{
	sec_smem_id_vendor1_v4_t *vendor1;

	vendor1 = __get_ddr_smem_entry(SMEM_ID_VENDOR1);
	if (!vendor1) {
		pr_err("SMEM_ID_VENDOR1 get entry error\n");
		return 0;
	}

	return vendor1->ddr_training.dqdqs_eye.wr_vmax_to_vmid[ch][cs][dq];
}
EXPORT_SYMBOL(ddr_get_wr_vmax_to_vmid);

uint8_t ddr_get_wr_vmid_to_vmin(uint32_t ch, uint32_t cs, uint32_t dq)
{
	sec_smem_id_vendor1_v4_t *vendor1;

	vendor1 = __get_ddr_smem_entry(SMEM_ID_VENDOR1);
	if (!vendor1) {
		pr_err("SMEM_ID_VENDOR1 get entry error\n");
		return 0;
	}

	return vendor1->ddr_training.dqdqs_eye.wr_vmid_to_vmin[ch][cs][dq];
}
EXPORT_SYMBOL(ddr_get_wr_vmid_to_vmin);

uint8_t ddr_get_dqs_dcc_adj(uint32_t ch, uint32_t dq)
{
	sec_smem_id_vendor1_v4_t *vendor1;

	vendor1 = __get_ddr_smem_entry(SMEM_ID_VENDOR1);
	if (!vendor1) {
		pr_err("SMEM_ID_VENDOR1 get entry error\n");
		return 0;
	}

	return vendor1->ddr_training.dqdqs_eye.dqs_dcc_adj[ch][dq];
}
EXPORT_SYMBOL(ddr_get_dqs_dcc_adj);

uint8_t ddr_get_rd_pr_width(uint32_t ch, uint32_t cs, uint32_t dq)
{
	sec_smem_id_vendor1_v4_t *vendor1;

	vendor1 = __get_ddr_smem_entry(SMEM_ID_VENDOR1);
	if (!vendor1) {
		pr_err("SMEM_ID_VENDOR1 get entry error\n");
		return 0;
	}

	return vendor1->ddr_training.dqdqs_eye.rd_pr_width[ch][cs][dq];
}
EXPORT_SYMBOL(ddr_get_rd_pr_width);

uint8_t ddr_get_rd_min_eye_height(uint32_t ch, uint32_t cs)
{
	sec_smem_id_vendor1_v4_t *vendor1;

	vendor1 = __get_ddr_smem_entry(SMEM_ID_VENDOR1);
	if (!vendor1) {
		pr_err("SMEM_ID_VENDOR1 get entry error\n");
		return 0;
	}

	return vendor1->ddr_training.dqdqs_eye.rd_min_eye_height[ch][cs];
}
EXPORT_SYMBOL(ddr_get_rd_min_eye_height);

uint8_t ddr_get_rd_best_vref(uint32_t ch, uint32_t cs, uint32_t dq)
{
	sec_smem_id_vendor1_v4_t *vendor1;

	vendor1 = __get_ddr_smem_entry(SMEM_ID_VENDOR1);
	if (!vendor1) {
		pr_err("SMEM_ID_VENDOR1 get entry error\n");
		return 0;
	}

	return vendor1->ddr_training.dqdqs_eye.rd_best_vref[ch][cs][dq];
}
EXPORT_SYMBOL(ddr_get_rd_best_vref);

uint8_t ddr_get_dq_dcc_abs(uint32_t ch, uint32_t cs, uint32_t dq)
{
	sec_smem_id_vendor1_v4_t *vendor1;

	vendor1 = __get_ddr_smem_entry(SMEM_ID_VENDOR1);
	if (!vendor1) {
		pr_err("SMEM_ID_VENDOR1 get entry error\n");
		return 0;
	}

	return vendor1->ddr_training.dqdqs_eye.dq_dcc_abs[ch][cs][dq];
}
EXPORT_SYMBOL(ddr_get_dq_dcc_abs);

uint16_t ddr_get_small_eye_detected(void)
{
	sec_smem_id_vendor1_v4_t *vendor1;

	vendor1 = __get_ddr_smem_entry(SMEM_ID_VENDOR1);
	if (!vendor1) {
		pr_err("SMEM_ID_VENDOR1 get entry error\n");
		return 0;
	}

	return vendor1->ddr_training.dqdqs_eye.small_eye_detected;
}
EXPORT_SYMBOL(ddr_get_small_eye_detected);

// ssuk end
static int sec_smem_suspend(struct device *dev)
{
	struct platform_device *pdev = to_platform_device(dev);
	sec_smem_id_vendor1_v4_t *vendor1 = platform_get_drvdata(pdev);

	vendor1->ven1_v2.ap_suspended = SUSPEND;

	pr_debug("smem_vendor1 ap_suspended(%llu)\n",
			(unsigned long long)vendor1->ven1_v2.ap_suspended);
	return 0;
}

static int sec_smem_resume(struct device *dev)
{
	struct platform_device *pdev = to_platform_device(dev);
	sec_smem_id_vendor1_v4_t *vendor1 = platform_get_drvdata(pdev);

	vendor1->ven1_v2.ap_suspended = RESUME;

	pr_debug("smem_vendor1 ap_suspended(%llu)\n",
			(unsigned long long)vendor1->ven1_v2.ap_suspended);
	return 0;
}

static int sec_smem_probe(struct platform_device *pdev)
{
	sec_smem_id_vendor1_v4_t *vendor1;

	vendor1 = __get_ddr_smem_entry(SMEM_ID_VENDOR1);
	if (!vendor1) {
		pr_err("SMEM_ID_VENDOR1 get entry error\n");
		panic("sec_smem_probe fail");

		return -EINVAL;
	}

#ifdef CONFIG_SEC_DEBUG_APPS_CLK_LOGGING
	vendor1->apps_stat.clk = (void *)virt_to_phys(clk_osm_get_log_addr());
	pr_err("%s vendor1->apps_stat.clk = %p\n", __func__, vendor1->apps_stat.clk);
#endif

	platform_set_drvdata(pdev, vendor1);

	return 0;
}

#ifdef CONFIG_OF
static const struct of_device_id sec_smem_dt_ids[] = {
	{ .compatible = "samsung,sec-smem" },
	{ }
};
MODULE_DEVICE_TABLE(of, sec_smem_dt_ids);
#endif /* CONFIG_OF */

static const struct dev_pm_ops sec_smem_pm_ops = {
	SET_SYSTEM_SLEEP_PM_OPS(sec_smem_suspend, sec_smem_resume)
};

struct platform_driver sec_smem_driver = {
	.probe		= sec_smem_probe,
	.driver		= {
		.name = "sec_smem",
		.owner	= THIS_MODULE,
		.pm = &sec_smem_pm_ops,
#ifdef CONFIG_OF
		.of_match_table = sec_smem_dt_ids,
#endif
	},
};

static int sec_smem_dbg_part_notifier_callback(struct notifier_block *nfb,
		unsigned long action, void *data)
{
	sec_smem_id_vendor1_v4_t *vendor1;

	switch (action) {
	case DBG_PART_DRV_INIT_DONE:
		p_health = ap_health_data_read();

		vendor1 = __get_ddr_smem_entry(SMEM_ID_VENDOR1);
		if (!vendor1)
			return NOTIFY_DONE;

		vendor1->ap_health = (void *)virt_to_phys(p_health);
		break;
	default:
		return NOTIFY_DONE;
	}

	return NOTIFY_OK;
}


static struct notifier_block sec_smem_dbg_part_notifier = {
	.notifier_call = sec_smem_dbg_part_notifier_callback,
};

static int __init sec_smem_init(void)
{
	int err;

	err = platform_driver_register(&sec_smem_driver);
	if (err)
		pr_err("Failed to register platform driver: %d\n", err);

	dbg_partition_notifier_register(&sec_smem_dbg_part_notifier);

	return 0;
}
device_initcall(sec_smem_init);

static void __exit sec_smem_exit(void)
{
	platform_driver_unregister(&sec_smem_driver);
}
module_exit(sec_smem_exit);

MODULE_DESCRIPTION("SEC SMEM");
MODULE_LICENSE("GPL v2");
