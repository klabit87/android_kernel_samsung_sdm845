/*
 *  sm5720-irq.c
 *  Samsung Interrupt controller support for SM5720
 *
 *  Copyright (C) 2016 Samsung Electronics
 *
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#include <linux/err.h>
#include <linux/irq.h>
#include <linux/interrupt.h>
#include <linux/gpio.h>
#include <linux/mfd/sm5720.h>
#include <linux/mfd/sm5720-private.h>
#include <linux/wakelock.h>

static struct wake_lock sm5720_irq_wakelock;

static const u8 sm5720_mask_reg[] = {
	[MUIC_INT1]     = SM5720_MUIC_REG_INTMASK1,
	[MUIC_INT2]     = SM5720_MUIC_REG_INTMASK2,
	[MUIC_INT3_AFC] = SM5720_MUIC_REG_INTMASK3_AFC,
	[CHG_INT1]      = SM5720_CHG_REG_INTMSK1,
	[CHG_INT2]      = SM5720_CHG_REG_INTMSK2,
	[CHG_INT3]      = SM5720_CHG_REG_INTMSK3,
	[CHG_INT4]      = SM5720_CHG_REG_INTMSK4,
	[FG_INT]        = SM5720_FG_REG_INTFG_MASK,
};

static struct i2c_client *get_i2c(struct sm5720_dev *sm5720, int src)
{
	switch (src) {
		case MUIC_INT1:
		case MUIC_INT2:
		case MUIC_INT3_AFC:
			return sm5720->muic;
		case CHG_INT1:
		case CHG_INT2:
		case CHG_INT3:
		case CHG_INT4:
			return sm5720->charger;
		case FG_INT:
			return sm5720->fuelgauge;
	}

	return ERR_PTR(-EINVAL);
}

struct sm5720_irq_data {
	int mask;
	int group;
};

#define DECLARE_IRQ(idx, _group, _mask)     [(idx)] = { .group = (_group), .mask = (_mask) }

static const struct sm5720_irq_data sm5720_irqs[] = {
	/* MUIC-irqs */
	DECLARE_IRQ(SM5720_MUIC_IRQ_INT1_DETACH,	        MUIC_INT1,      1 << 1),
	DECLARE_IRQ(SM5720_MUIC_IRQ_INT1_ATTACH,	        MUIC_INT1,      1 << 0),

	DECLARE_IRQ(SM5720_MUIC_IRQ_INT2_VBUSDET_ON,        MUIC_INT2,      1 << 7),
	DECLARE_IRQ(SM5720_MUIC_IRQ_INT2_VBUS_OFF,	        MUIC_INT2,      1 << 0),

	DECLARE_IRQ(SM5720_MUIC_IRQ_INT3_QC20_ACCEPTED,     MUIC_INT3_AFC,  1 << 6),
	DECLARE_IRQ(SM5720_MUIC_IRQ_INT3_AFC_ERROR,         MUIC_INT3_AFC,  1 << 5),
	DECLARE_IRQ(SM5720_MUIC_IRQ_INT3_AFC_STA_CHG,       MUIC_INT3_AFC,  1 << 4),
	DECLARE_IRQ(SM5720_MUIC_IRQ_INT3_MULTI_BYTE,	    MUIC_INT3_AFC,  1 << 3),
	DECLARE_IRQ(SM5720_MUIC_IRQ_INT3_VBUS_UPDATE,       MUIC_INT3_AFC,  1 << 2),
	DECLARE_IRQ(SM5720_MUIC_IRQ_INT3_AFC_ACCEPTED,      MUIC_INT3_AFC,  1 << 1),
	DECLARE_IRQ(SM5720_MUIC_IRQ_INT3_AFC_TA_ATTACHED,	MUIC_INT3_AFC,  1 << 0),

	/* Charger-irqs */
	DECLARE_IRQ(SM5720_CHG_IRQ_INT1_VBUSPOK,	        CHG_INT1,       1 << 0),
	DECLARE_IRQ(SM5720_CHG_IRQ_INT1_VBUSUVLO,	        CHG_INT1,       1 << 1),
	DECLARE_IRQ(SM5720_CHG_IRQ_INT1_VBUSOVP,	        CHG_INT1,       1 << 2),
	DECLARE_IRQ(SM5720_CHG_IRQ_INT1_VBUSLIMIT,	        CHG_INT1,       1 << 3),
	DECLARE_IRQ(SM5720_CHG_IRQ_INT1_WPCINPOK,	        CHG_INT1,       1 << 4),
	DECLARE_IRQ(SM5720_CHG_IRQ_INT1_WPCINUVLO,	        CHG_INT1,       1 << 5),
	DECLARE_IRQ(SM5720_CHG_IRQ_INT1_WPCINOVP,           CHG_INT1,       1 << 6),
	DECLARE_IRQ(SM5720_CHG_IRQ_INT1_WPCINLIMIT,         CHG_INT1,       1 << 7),

	DECLARE_IRQ(SM5720_CHG_IRQ_INT2_AICL,	            CHG_INT2,       1 << 0),
	DECLARE_IRQ(SM5720_CHG_IRQ_INT2_BATOVP,	            CHG_INT2,       1 << 1),
	DECLARE_IRQ(SM5720_CHG_IRQ_INT2_NOBAT,	            CHG_INT2,       1 << 2),
	DECLARE_IRQ(SM5720_CHG_IRQ_INT2_CHGON,	            CHG_INT2,       1 << 3),
	DECLARE_IRQ(SM5720_CHG_IRQ_INT2_Q4FULLON,	        CHG_INT2,       1 << 4),
	DECLARE_IRQ(SM5720_CHG_IRQ_INT2_TOPOFF,	            CHG_INT2,       1 << 5),
	DECLARE_IRQ(SM5720_CHG_IRQ_INT2_DONE,               CHG_INT2,       1 << 6),
	DECLARE_IRQ(SM5720_CHG_IRQ_INT2_WDTMROFF,           CHG_INT2,       1 << 7),

	DECLARE_IRQ(SM5720_CHG_IRQ_INT3_OTGFAIL,	        CHG_INT3,       1 << 2),
	DECLARE_IRQ(SM5720_CHG_IRQ_INT3_DISLIMIT,	        CHG_INT3,       1 << 3),
	DECLARE_IRQ(SM5720_CHG_IRQ_INT3_PRETMROFF,	        CHG_INT3,       1 << 4),
	DECLARE_IRQ(SM5720_CHG_IRQ_INT3_FASTTMROFF,	        CHG_INT3,       1 << 5),
	DECLARE_IRQ(SM5720_CHG_IRQ_INT3_nENQ4,              CHG_INT3,       1 << 6),
	DECLARE_IRQ(SM5720_CHG_IRQ_INT3_VSYSOVP,            CHG_INT3,       1 << 7),

	DECLARE_IRQ(SM5720_CHG_IRQ_INT4_nVBUSOK,	        CHG_INT4,       1 << 4),
	DECLARE_IRQ(SM5720_CHG_IRQ_INT4_nWPCOK,	            CHG_INT4,       1 << 5),

	/* FuelGauge-irqs */
	DECLARE_IRQ(SM5720_FG_IRQ_INT_LOW_VOLTAGE,          FG_INT,         1 << 0),
	DECLARE_IRQ(SM5720_FG_IRQ_INT_LOW_SOC,	            FG_INT,         1 << 3),
};

static void sm5720_irq_lock(struct irq_data *data)
{
	struct sm5720_dev *sm5720 = irq_get_chip_data(data->irq);

	mutex_lock(&sm5720->irqlock);
}

static void sm5720_irq_sync_unlock(struct irq_data *data)
{
	struct sm5720_dev *sm5720 = irq_get_chip_data(data->irq);
	int i;

	for (i = 0; i < SM5720_IRQ_GROUP_NR; i++) {
		struct i2c_client *i2c = get_i2c(sm5720, i);

		if (IS_ERR_OR_NULL(i2c))
			continue;

		sm5720->irq_masks_cache[i] = sm5720->irq_masks_cur[i];

		if (i == FG_INT) {
			sm5720_write_word(i2c, sm5720_mask_reg[i], sm5720->irq_masks_cur[i]);
		} else {
			sm5720_write_reg(i2c, sm5720_mask_reg[i], sm5720->irq_masks_cur[i]);
		}

	}

	mutex_unlock(&sm5720->irqlock);
}

	static const inline struct sm5720_irq_data *
irq_to_sm5720_irq(struct sm5720_dev *sm5720, int irq)
{
	return &sm5720_irqs[irq - sm5720->irq_base];
}

static void sm5720_irq_mask(struct irq_data *data)
{
	struct sm5720_dev *sm5720 = irq_get_chip_data(data->irq);
	const struct sm5720_irq_data *irq_data = irq_to_sm5720_irq(sm5720, data->irq);

	if (irq_data->group >= SM5720_IRQ_GROUP_NR)
		return;

	sm5720->irq_masks_cur[irq_data->group] |= irq_data->mask;
}

static void sm5720_irq_unmask(struct irq_data *data)
{
	struct sm5720_dev *sm5720 = irq_get_chip_data(data->irq);
	const struct sm5720_irq_data *irq_data = irq_to_sm5720_irq(sm5720, data->irq);

	if (irq_data->group >= SM5720_IRQ_GROUP_NR)
		return;

	sm5720->irq_masks_cur[irq_data->group] &= ~irq_data->mask;
}

static struct irq_chip sm5720_irq_chip = {
	.name			        = MFD_DEV_NAME,
	.irq_bus_lock		    = sm5720_irq_lock,
	.irq_bus_sync_unlock	= sm5720_irq_sync_unlock,
	.irq_mask		        = sm5720_irq_mask,
	.irq_unmask		        = sm5720_irq_unmask,
	.irq_disable		        = sm5720_irq_mask,
};

static irqreturn_t sm5720_irq_thread(int irq, void *data)
{
	struct sm5720_dev *sm5720 = data;
	u8 irq_reg[SM5720_IRQ_GROUP_NR] = {0};
	u8 irq_src;
	int i, ret;
	struct i2c_client *i2c;

	pr_debug("%s: irq gpio pre-state\n", __func__);

	wake_lock(&sm5720_irq_wakelock);

	ret = sm5720_read_reg(sm5720->charger, SM5720_CHG_REG_INT_SOURCE, &irq_src);
	if (ret) {
		pr_err("%s:%s fail to read interrupt source: %d\n", MFD_DEV_NAME, __func__, ret);
		wake_unlock(&sm5720_irq_wakelock);
		return IRQ_NONE;
	}
	pr_debug("%s: INT_SOURCE=0x%02x)\n", __func__, irq_src);

	/* Irregular case: check IC status */
	if (irq_src == 0) {
		if (sm5720->check_muic_reset) {sm5720->check_muic_reset(sm5720->muic, sm5720->muic_data); }
		if (sm5720->check_chg_reset) { sm5720->check_chg_reset(sm5720->charger, sm5720->chg_data); }
		if (sm5720->check_fg_reset) { sm5720->check_fg_reset(sm5720->fuelgauge, sm5720->fg_data); }
	}

	/* Charger INT 1 ~ 4 */
	if (irq_src & SM5720_IRQSRC_CHG) {
		ret = sm5720_bulk_read(sm5720->charger, SM5720_CHG_REG_INT1, SM5720_NUM_IRQ_CHG_REGS, &irq_reg[CHG_INT1]);
		if (ret) {
			pr_err("%s:%s fail to read CHG_INT source: %d\n", MFD_DEV_NAME, __func__, ret);
			wake_unlock(&sm5720_irq_wakelock);
			return IRQ_NONE;
		}
		for (i = CHG_INT1; i <= CHG_INT4; i++) {
			pr_debug("%s:%s CHG_INT%d = 0x%x\n", MFD_DEV_NAME, __func__, (i - 2), irq_reg[i]);
		}
	}

	/* MUIC INT 1 ~ 3 */
	if (irq_src & SM5720_IRQSRC_MUIC) {
		ret = sm5720_bulk_read(sm5720->muic, SM5720_MUIC_REG_INT1, SM5720_NUM_IRQ_MUIC_REGS, &irq_reg[MUIC_INT1]);
		if (ret) {
			pr_err("%s:%s fail to read MUIC_INT source: %d\n", MFD_DEV_NAME, __func__, ret);
			wake_unlock(&sm5720_irq_wakelock);
			return IRQ_NONE;
		}
		for (i = MUIC_INT1; i <= MUIC_INT3_AFC; i++) {
			pr_debug("%s:%s MUIC_INT%d = 0x%x\n", MFD_DEV_NAME, __func__, (i + 1), irq_reg[i]);
		}
	}

	/* Fuel Gauge INT */
	if (irq_src & SM5720_IRQSRC_FG) {
		i2c = get_i2c(sm5720, FG_INT);
		if (IS_ERR_OR_NULL(i2c)) {
			pr_err("%s:%s fail to get i2c\n", MFD_DEV_NAME, __func__);
			wake_unlock(&sm5720_irq_wakelock);
			return IRQ_NONE;
		}
        	/* FG_INT Lock */
        	sm5720->irq_masks_cur[FG_INT] |= (1 << 7);
        	sm5720_write_word(i2c, sm5720_mask_reg[FG_INT], sm5720->irq_masks_cur[FG_INT]);

		irq_reg[FG_INT] = (u8)(sm5720_read_word(sm5720->fuelgauge, SM5720_FG_REG_INTFG) & 0x00FF);

        	/* FG_INT Un-Lock */
        	sm5720->irq_masks_cur[FG_INT] &= ~(1 << 7);
        	sm5720_write_word(i2c, sm5720_mask_reg[FG_INT], sm5720->irq_masks_cur[FG_INT]);

		pr_debug("%s:%s FG_INT = 0x%x\n", MFD_DEV_NAME, __func__, irq_reg[FG_INT]);
	}

	if (sm5720_get_device_ID() == 0x0) {    // Check Rev.0
		sm5720_read_reg(sm5720->charger, SM5720_CHG_REG_INT_SOURCE, &irq_src);
	}

	/* Apply masking */
	for (i = 0; i < SM5720_IRQ_GROUP_NR; i++) {
		irq_reg[i] &= ~sm5720->irq_masks_cur[i];
	}

	/* Report */
	for (i = 0; i < SM5720_IRQ_NR; i++) {
		if (irq_reg[sm5720_irqs[i].group] & sm5720_irqs[i].mask) {
			handle_nested_irq(sm5720->irq_base + i);
		}
	}

	wake_unlock(&sm5720_irq_wakelock);

	return IRQ_HANDLED;
}

int sm5720_irq_init(struct sm5720_dev *sm5720)
{
	int i;
	int ret;

	if (!sm5720->irq) {
		dev_warn(sm5720->dev, "No interrupt specified.\n");
		sm5720->irq_base = 0;
		return 0;
	}

	if (!sm5720->irq_base) {
		dev_err(sm5720->dev, "No interrupt base specified.\n");
		return 0;
	}

	mutex_init(&sm5720->irqlock);

	wake_lock_init(&sm5720_irq_wakelock, WAKE_LOCK_SUSPEND, "sm5720-irq");

	/* Mask individual interrupt sources */
	for (i = 0; i < SM5720_IRQ_GROUP_NR; i++) {
		struct i2c_client *i2c;

		i2c = get_i2c(sm5720, i);

		if (IS_ERR_OR_NULL(i2c))
			continue;

		if (i == FG_INT) {
			sm5720->irq_masks_cur[i] = 0x000f;
			sm5720->irq_masks_cache[i] = 0x000f;
			sm5720_write_word(i2c, sm5720_mask_reg[i], sm5720->irq_masks_cur[i]);
		} else {
			sm5720->irq_masks_cur[i] = 0xff;
			sm5720->irq_masks_cache[i] = 0xff;
			sm5720_write_reg(i2c, sm5720_mask_reg[i], sm5720->irq_masks_cur[i]);
		}
	}

	/* Register with genirq */
	for (i = 0; i < SM5720_IRQ_NR; i++) {
		int cur_irq;
		cur_irq = i + sm5720->irq_base;
		irq_set_chip_data(cur_irq, sm5720);
		irq_set_chip_and_handler(cur_irq, &sm5720_irq_chip, handle_level_irq);
		irq_set_nested_thread(cur_irq, 1);
#ifdef CONFIG_ARM
		set_irq_flags(cur_irq, IRQF_VALID);
#else
		irq_set_noprobe(cur_irq);
#endif
	}

	irq_set_status_flags(sm5720->irq, IRQ_NOAUTOEN);

	ret = request_threaded_irq(sm5720->irq, NULL, sm5720_irq_thread, IRQF_TRIGGER_LOW | IRQF_ONESHOT,
			"sm5720-irq", sm5720);
	if (ret) {
		dev_err(sm5720->dev, "Fail to request IRQ %d: %d\n", sm5720->irq, ret);
		return ret;
	}

	enable_irq(sm5720->irq);

	return 0;
}
EXPORT_SYMBOL_GPL(sm5720_irq_init);

void sm5720_irq_exit(struct sm5720_dev *sm5720)
{
	if (sm5720->irq)
		free_irq(sm5720->irq, sm5720);
}
EXPORT_SYMBOL_GPL(sm5720_irq_exit);



