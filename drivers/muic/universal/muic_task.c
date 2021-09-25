/*
 * muic_task.c
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
#include <linux/ccic/s2mm005.h>

#if defined(CONFIG_MUIC_UNIVERSAL_SM5720)
#include <linux/mfd/sm5720.h>
#include <linux/mfd/sm5720-private.h>
#endif

#if defined(CONFIG_MUIC_UNIVERSAL_SM5703)
#include <linux/mfd/sm5703.h>
#include <linux/mfd/sm5703-private.h>
#endif

#include <linux/muic/muic.h>
#include <linux/mfd/muic_mfd.h>

#if defined(CONFIG_MUIC_NOTIFIER)
#include <linux/muic/muic_notifier.h>
#endif /* CONFIG_MUIC_NOTIFIER */

#if defined(CONFIG_VBUS_NOTIFIER)
#include <linux/vbus_notifier.h>
#endif /* CONFIG_VBUS_NOTIFIER */

#if defined (CONFIG_OF)
#include <linux/of_device.h>
#include <linux/of_gpio.h>
#endif /* CONFIG_OF */

#include "muic-internal.h"
#include "muic_apis.h"
#include "muic_state.h"
#include "muic_vps.h"
#include "muic_i2c.h"
#include "muic_sysfs.h"
#include "muic_debug.h"
#include "muic_dt.h"
#include "muic_regmap.h"
#include "muic_coagent.h"

#if defined(CONFIG_MUIC_HV)
#include "muic_hv.h"
#endif

#if defined(CONFIG_MUIC_SUPPORT_CCIC)
#include "muic_ccic.h"
#endif

#if defined(CONFIG_USB_EXTERNAL_NOTIFY)
#include "muic_usb.h"
#endif

#define MUIC_INT_DETACH_MASK	(0x1 << 1)
#define MUIC_INT_ATTACH_MASK	(0x1 << 0)
#define MUIC_INT_OVP_EN_MASK	(0x1 << 5)

#define MUIC_REG_CTRL_RESET_VALUE	(0x1F)

#define MUIC_REG_CTRL	0x02
#define MUIC_REG_INT1	0x03
#define MUIC_REG_INT2	0x04

/* Interrupt type */
enum {
	INT_REQ_ATTACH = 1<<0,
	INT_REQ_DETACH = 1<<1,
	INT_REQ_OVP = 1<<2,
	INT_REQ_RESET = 1<<3,
	INT_REQ_DONE = 1<<4,
	INT_REQ_DISCARD = 1<<5,
};

extern struct muic_platform_data muic_pdata;

extern void muic_send_dock_intent(int type);

static bool muic_online = false;

/*
 * 0 : normal, 1: abnormal
 * This flag is set by USB for an abnormal HMT and
 * cleared by MUIC on detachment.
 */
static int muic_hmt_status;

bool muic_is_online(void)
{
	return muic_online;
}

#if defined(CONFIG_MUIC_UNIVERSAL_SM5703)
extern void sm5703_muic_check_reset(struct i2c_client *i2c, void *muic_data );

static int sm5703_muic_irq_handler(muic_data_t *pmuic, int irq)
{
	struct i2c_client *i2c = pmuic->i2c;
	int intr1, intr2, ctrl;

	pr_info("%s:%s irq(%d)\n", pmuic->chip_name, __func__, irq);

	/* read and clear interrupt status bits */
	intr1 = muic_i2c_read_byte(i2c, MUIC_REG_INT1);
	intr2 = muic_i2c_read_byte(i2c, MUIC_REG_INT2);

	if ((intr1 < 0) || (intr2 < 0)) {
		pr_err("%s: err read interrupt status [1:0x%x, 2:0x%x]\n",
				__func__, intr1, intr2);
		return INT_REQ_DISCARD;
	}

	if (intr1 & MUIC_INT_ATTACH_MASK) {
		int intr_tmp;
		intr_tmp = muic_i2c_read_byte(i2c, MUIC_REG_INT1);
		if (intr_tmp & 0x2) {
			pr_info("%s:%s attach/detach interrupt occurred\n",
					pmuic->chip_name, __func__);
			intr1 &= 0xFE;
		}
		intr1 |= intr_tmp;
	}

	pr_info("%s:%s intr[1:0x%x, 2:0x%x]\n", pmuic->chip_name, __func__,
			intr1, intr2);

	ctrl = muic_i2c_read_byte(i2c, MUIC_REG_CTRL);

	/* check for muic reset and recover for every interrupt occurred */
	if ((intr1 & MUIC_INT_OVP_EN_MASK) ||
			((ctrl == MUIC_REG_CTRL_RESET_VALUE) && (irq != -1))) {
		if (ctrl == MUIC_REG_CTRL_RESET_VALUE) {
			/* CONTROL register is reset to 1F */
			muic_print_reg_log();
			muic_print_reg_dump(pmuic);
			pr_err("%s: err muic could have been reseted. Initilize!!\n",
					__func__);
			muic_reg_init(pmuic);
			muic_print_reg_dump(pmuic);

			/* MUIC Interrupt On */
			set_int_mask(pmuic, false);
		}

		if ((intr1 & MUIC_INT_ATTACH_MASK) == 0)
			return INT_REQ_DISCARD;
	}

	pmuic->intr.intr1 = intr1;
	pmuic->intr.intr2 = intr2;

	return INT_REQ_DONE;
}
#elif defined(CONFIG_MUIC_UNIVERSAL_SM5720)

#define MUIC_REG_AFCCNTL  0x18

static int sm5720_muic_irq_handler(muic_data_t *pmuic, int irq)
{
#if defined(CONFIG_MUIC_HV)
	struct afc_ops *afcops = pmuic->regmapdesc->afcops;

	struct i2c_client *i2c = pmuic->i2c;
	struct muic_platform_data *pdata = pmuic->pdata;
#endif
	int local_irq;

	pr_info("%s:%s irq(%d)\n", pmuic->chip_name, __func__, irq);

	if(irq < 0){
		return INT_REQ_DONE;
	}

	local_irq = irq - pmuic->pdata->irq_gpio;
	pr_info("%s: local_irq(%d)  irq_gpio(%d)\n", __func__, local_irq, pmuic->pdata->irq_gpio);

#if defined(CONFIG_MUIC_HV)
	if (local_irq == SM5720_MUIC_IRQ_INT1_DETACH) {

		cancel_delayed_work(&pmuic->afc_retry_work);
		pr_info("%s: afc_retry_work(INT1_DETACH) cancel \n", __func__);

		// ENAFC set '0'
		muic_i2c_write_byte(i2c, MUIC_REG_AFCCNTL , 0x00);
		pr_info("%s:%s INT1_DETACH - REG_AFCCNTL:0x00\n", pmuic->chip_name, __func__);

		pmuic->afc_retry_count = 0;
		pmuic->afc_vbus_retry_count = 0;
		pmuic->qc20_vbus = 0;
		pdata->afc_limit_voltage = false;

		hv_afc_delay_check_state(pmuic, 0);
	}

	if(pdata->afc_limit_voltage) {
		pr_err("%s: Ignore INTS, limit voltage \n", __func__);
		return INT_REQ_DISCARD;
	}
	else if(!pdata->afc_disable) {
		if ( local_irq == SM5720_MUIC_IRQ_INT3_QC20_ACCEPTED ) {  /*QC20_ACCEPTED*/
			pr_info("%s:%s INT3_QC20_ACCEPTED irq(%d)\n", pmuic->chip_name, __func__, local_irq);
			return INT_REQ_DISCARD;
		}

		if ( local_irq == SM5720_MUIC_IRQ_INT3_AFC_ERROR ) {     /*AFC_ERROR*/
			cancel_delayed_work(&pmuic->afc_retry_work);
			pr_info("%s: afc_retry_work(AFC ERROR) cancel \n", __func__);

			afcops->afc_error(pmuic->regmapdesc);
			return INT_REQ_DISCARD;
		}

		if ( local_irq == SM5720_MUIC_IRQ_INT3_AFC_STA_CHG ) {   /*AFC_STA_CHG*/
			pr_info("%s:%s IRQ_INT3_AFC_STA_CHG irq(%d)\n", pmuic->chip_name, __func__, local_irq);
			return INT_REQ_DISCARD;
		}

		if ( local_irq == SM5720_MUIC_IRQ_INT3_MULTI_BYTE ) {    /*AFC_MULTI_BYTE*/
			afcops->afc_multi_byte(pmuic->regmapdesc);
			return INT_REQ_DISCARD;
		}

		if ( local_irq == SM5720_MUIC_IRQ_INT3_VBUS_UPDATE ) {   /*AFC_VBUS_UPDATE*/
			afcops->afc_vbus_update(pmuic->regmapdesc);
			return INT_REQ_DISCARD;
		}

		if ( local_irq == SM5720_MUIC_IRQ_INT3_AFC_ACCEPTED ) {  /*AFC_ACCEPTED*/
			afcops->afc_ta_accept(pmuic->regmapdesc);
			return INT_REQ_DISCARD;
		}

		if ((local_irq == SM5720_MUIC_IRQ_INT3_AFC_TA_ATTACHED) && (pmuic->ccic_rp == Rp_56K)) {  /*AFC_TA_ATTACHED*/
#if !defined(CONFIG_SEC_FACTORY)
			/* To prevent damage by RP0 Cable, AFC should be progress after ccic_attach */
			if (pmuic->is_ccic_attach)
				afcops->afc_ta_attach(pmuic->regmapdesc);
			else {
				pmuic->retry_afc = true;
				pr_info("%s: Need AFC restart for late ccic_attach\n", __func__);
			}
#else
			afcops->afc_ta_attach(pmuic->regmapdesc);
#endif
			return INT_REQ_DISCARD;
		}
	}
	else
		pr_err("%s: Ignore AFC_INTS, AFC is disabled \n", __func__);
#endif

	return INT_REQ_DONE;
}

#else
static int muic_irq_handler(muic_data_t *pmuic, int irq)
{
	struct i2c_client *i2c = pmuic->i2c;
	int intr1, intr2;

	pr_info("%s:%s irq(%d)\n", pmuic->chip_name, __func__, irq);

	/* read and clear interrupt status bits */
	intr1 = muic_i2c_read_byte(i2c, MUIC_REG_INT1);
	intr2 = muic_i2c_read_byte(i2c, MUIC_REG_INT2);

	if ((intr1 < 0) || (intr2 < 0)) {
		pr_err("%s: err read interrupt status [1:0x%x, 2:0x%x]\n",
				__func__, intr1, intr2);
		return INT_REQ_DISCARD;
	}

	if (intr1 & MUIC_INT_ATTACH_MASK)
	{
		int intr_tmp;
		intr_tmp = muic_i2c_read_byte(i2c, MUIC_REG_INT1);
		if (intr_tmp & 0x2)
		{
			pr_info("%s:%s attach/detach interrupt occurred\n",
					pmuic->chip_name, __func__);
			intr1 &= 0xFE;
		}
		intr1 |= intr_tmp;
	}

	pr_info("%s:%s intr[1:0x%x, 2:0x%x]\n", pmuic->chip_name, __func__,
			intr1, intr2);

	/* check for muic reset and recover for every interrupt occurred */
	if ((intr1 & MUIC_INT_OVP_EN_MASK) ||
			((intr1 == 0) && (intr2 == 0) && (irq != -1)))
	{
		int ctrl;
		ctrl = muic_i2c_read_byte(i2c, MUIC_REG_CTRL);
		if (ctrl == 0x1F)
		{
			/* CONTROL register is reset to 1F */
			muic_print_reg_log();
			muic_print_reg_dump(pmuic);
			pr_err("%s: err muic could have been reseted. Initilize!!\n",
					__func__);
			muic_reg_init(pmuic);
			muic_print_reg_dump(pmuic);

			/* MUIC Interrupt On */
			set_int_mask(pmuic, false);
		}

		if ((intr1 & MUIC_INT_ATTACH_MASK) == 0)
			return INT_REQ_DISCARD;
	}

	pmuic->intr.intr1 = intr1;
	pmuic->intr.intr2 = intr2;

	return INT_REQ_DONE;
}
#endif

enum max_intr_bits{
	INTR1_ADC1K_MASK = (1<<3),
	INTR1_ADCERR_MASK = (1<<2),
	INTR1_ADC_MASK   = (1<<0),

	INTR2_VBVOLT_MASK    = (1<<4),
	INTR2_OVP_MASK       = (1<<3),
	INTR2_DCTTMR_MASK    = (1<<2),
	INTR2_CHGDETRUN_MASK = (1<<1),
	INTR2_CHGTYP_MASK    = (1<<0),
};

/* Fixme. */
enum irq_num {
	MAX77854_MUIC_IRQ_BASE = 10,
	/* MUIC INT1 */
	MAX77854_MUIC_IRQ_INT1_ADC,
	MAX77854_MUIC_IRQ_INT1_ADCERR,
	MAX77854_MUIC_IRQ_INT1_ADC1K,
	/* MUIC INT2 */
	MAX77854_MUIC_IRQ_INT2_CHGTYP,
	MAX77854_MUIC_IRQ_INT2_CHGDETREUN,
	MAX77854_MUIC_IRQ_INT2_DCDTMR,
	MAX77854_MUIC_IRQ_INT2_DXOVP,
	MAX77854_MUIC_IRQ_INT2_VBVOLT,
};

#define MAX77854_REG_INT1 (0x01)
#define MAX77854_REG_INT2 (0x02)

static inline bool muic_is_vbus_int(muic_data_t *pmuic, int irq)
{
	int local_irq;

	if (irq < 0)
		return false;

	local_irq = irq - pmuic->pdata->irq_gpio;

	if (pmuic->vps_table == VPS_TYPE_TABLE) {
		if (local_irq == MAX77854_MUIC_IRQ_INT2_VBVOLT)
			return true;
	} else {
#if defined(CONFIG_MUIC_UNIVERSAL_SM5720)
		if (local_irq == SM5720_MUIC_IRQ_INT2_VBUS_OFF ||
			local_irq == SM5720_MUIC_IRQ_INT2_VBUSDET_ON
			)
			return true;
#endif
	}

	return false;
}

void muic_set_hmt_status(int status)
{

	pr_info("%s:%s hmt_status=[%s]\n", MUIC_DEV_NAME, __func__,
			status ? "abnormal" : "normal");

	muic_hmt_status = status;

	if (status)
		muic_send_dock_intent(MUIC_DOCK_ABNORMAL);
}

static int muic_get_hmt_status(void)
{
	return muic_hmt_status;
}

#if defined(CONFIG_VBUS_NOTIFIER)
#if defined(CONFIG_VBUS_NOTIFIER_WORK)
static void muic_handle_vbus_workq(struct work_struct *work)
{
	muic_data_t *pmuic;
	vbus_status_t status;

	pmuic = container_of(work, muic_data_t, vbus_notify_work);

	if(pmuic->vps_table == VPS_TYPE_TABLE)
		status = pmuic->vps.t.vbvolt ? STATUS_VBUS_HIGH: STATUS_VBUS_LOW;
	else
		status = pmuic->vps.s.vbvolt ? STATUS_VBUS_HIGH: STATUS_VBUS_LOW;

	pr_info("%s:%s <%d>\n", MUIC_DEV_NAME, __func__, status);

	if (pmuic->attached_dev == ATTACHED_DEV_HMT_MUIC) {
		if (muic_get_hmt_status()) {
			pr_info("%s:%s Abnormal HMT -> VBUS_UNKNOWN Noti.\n",
					MUIC_DEV_NAME, __func__);
			status = STATUS_VBUS_UNKNOWN;
		}
	}

	vbus_notifier_handle(status);

	return;
}
#endif
static void muic_handle_vbus(muic_data_t *pmuic)
{
	vbus_status_t status;

	if(pmuic->vps_table == VPS_TYPE_TABLE)
		status = pmuic->vps.t.vbvolt ? STATUS_VBUS_HIGH: STATUS_VBUS_LOW;
	else
		status = pmuic->vps.s.vbvolt ? STATUS_VBUS_HIGH: STATUS_VBUS_LOW;

	pr_info("%s:%s <%d>\n", MUIC_DEV_NAME, __func__, status);

	if (pmuic->attached_dev == ATTACHED_DEV_HMT_MUIC) {
		if (muic_get_hmt_status()) {
			pr_info("%s:%s Abnormal HMT -> VBUS_UNKNOWN Noti.\n",
					MUIC_DEV_NAME, __func__);
			status = STATUS_VBUS_UNKNOWN;
		}
	}

	vbus_notifier_handle(status);

	return;
}
#else
static void muic_handle_vbus(muic_data_t *pmuic)
{
	pr_info("%s:%s <%d> Not implemented.\n", MUIC_DEV_NAME,
			__func__, pmuic->vps.t.vbvolt);
}
#endif

static irqreturn_t max77854_muic_irq_handler(muic_data_t *pmuic, int irq)
{
	int local_irq;

	pr_info("%s:%s irq:%d\n", MUIC_DEV_NAME, __func__, irq);

	if (irq < 0)
		return INT_REQ_DONE;

	local_irq = irq - pmuic->pdata->irq_gpio;

	switch (local_irq) {
		case MAX77854_MUIC_IRQ_INT2_DXOVP:
			pr_info("%s OVP interrupt occured\n",__func__);
			return INT_REQ_OVP;

		case MAX77854_MUIC_IRQ_INT2_DCDTMR:
			pr_info("%s DCTTMR interrupt occured\n",__func__);
			pmuic->is_dcdtmr_intr = true;
			break;
		default:
			break;
	}

	return INT_REQ_DONE;
}

static irqreturn_t muic_irq_thread(int irq, void *data)
{
	muic_data_t *pmuic = data;

	mutex_lock(&pmuic->muic_mutex);

	if(pmuic->vps_table == VPS_TYPE_TABLE) {
		if(max77854_muic_irq_handler(pmuic, irq) & INT_REQ_DONE) {
			muic_detect_dev(pmuic, irq);
			if (muic_is_vbus_int(pmuic, irq))
				muic_handle_vbus(pmuic);
		}
	} else{
#if defined(CONFIG_MUIC_UNIVERSAL_SM5703)
		if (sm5703_muic_irq_handler(pmuic, irq) & INT_REQ_DONE)
			muic_detect_dev(pmuic, irq);
#elif defined(CONFIG_MUIC_UNIVERSAL_SM5720)
		if (sm5720_muic_irq_handler(pmuic, irq) & INT_REQ_DONE) {
			muic_detect_dev(pmuic, irq);
			if (muic_is_vbus_int(pmuic, irq))
#if defined(CONFIG_VBUS_NOTIFIER_WORK)
				schedule_work(&pmuic->vbus_notify_work);
#else
				muic_handle_vbus(pmuic);
#endif
		}
#else
		if (muic_irq_handler(pmuic, irq) & INT_REQ_DONE)
			muic_detect_dev(pmuic, irq);
#endif
	}

	mutex_unlock(&pmuic->muic_mutex);

	return IRQ_HANDLED;
}

static void muic_init_detect(muic_data_t *pmuic)
{
	pr_info("%s:%s\n", pmuic->chip_name, __func__);

	pmuic->is_muic_ready = true;
#if defined(CONFIG_MUIC_UNIVERSAL_SM5703) || defined(CONFIG_MUIC_UNIVERSAL_SM5720)
	/* MUIC Interrupt On */
	set_int_mask(pmuic, false);
#endif
	muic_irq_thread(-1, pmuic);
}

static int muic_irq_init(muic_data_t *pmuic)
{
	struct muic_platform_data *pdata = pmuic->pdata;
	int ret = 0;

	pr_info("%s:%s\n", pmuic->chip_name, __func__);

	if (!pdata->irq_gpio) {
		pr_warn("%s:%s No interrupt specified\n", pmuic->chip_name,
				__func__);
		return -ENXIO;
	}

	if(!strcmp(pmuic->chip_name,"max,max77854")) {
		int irq_adc1k, irq_adcerr, irq_adc, irq_chgtyp, irq_vbvolt;
#if defined(CONFIG_MUIC_SUPPORT_CCIC)
		/* type C needs checking dcd tmr interrupt */
		int irq_dcdtmr;
#endif

		irq_adc1k = pdata->irq_gpio + MAX77854_MUIC_IRQ_INT1_ADC1K;
		printk(" %s requesting irq no = %d\n",__func__,irq_adc1k);
		ret = request_threaded_irq(irq_adc1k, NULL,
				muic_irq_thread, (IRQF_TRIGGER_FALLING | IRQF_ONESHOT),
				"muic-irq", pmuic);

		irq_adcerr = pdata->irq_gpio + MAX77854_MUIC_IRQ_INT1_ADCERR;
		printk(" %s requesting irq no = %d\n",__func__,irq_adcerr);
		ret = request_threaded_irq(irq_adcerr, NULL,
				muic_irq_thread, (IRQF_TRIGGER_FALLING | IRQF_ONESHOT),
				"muic-irq", pmuic);
		irq_adc = pdata->irq_gpio + MAX77854_MUIC_IRQ_INT1_ADC;
		printk(" %s requesting irq no = %d\n",__func__,irq_adc);
		ret = request_threaded_irq(irq_adc, NULL,
				muic_irq_thread, (IRQF_TRIGGER_FALLING | IRQF_ONESHOT),
				"muic-irq", pmuic);
		irq_chgtyp = pdata->irq_gpio + MAX77854_MUIC_IRQ_INT2_CHGTYP;
		printk(" %s requesting irq no = %d\n",__func__,irq_chgtyp);
		ret = request_threaded_irq(irq_chgtyp, NULL,
				muic_irq_thread, (IRQF_TRIGGER_FALLING | IRQF_ONESHOT),
				"muic-irq", pmuic);
		irq_vbvolt = pdata->irq_gpio + MAX77854_MUIC_IRQ_INT2_VBVOLT;
		printk(" %s requesting irq no = %d\n",__func__,irq_vbvolt);
		ret = request_threaded_irq(irq_vbvolt, NULL,
				muic_irq_thread, (IRQF_TRIGGER_FALLING | IRQF_ONESHOT),
				"muic-irq", pmuic);
#if defined(CONFIG_MUIC_SUPPORT_CCIC)
		irq_dcdtmr = pdata->irq_gpio + MAX77854_MUIC_IRQ_INT2_DCDTMR;
		printk(" %s requesting irq no = %d\n",__func__,irq_dcdtmr);
		ret = request_threaded_irq(irq_dcdtmr, NULL,
				muic_irq_thread, (IRQF_TRIGGER_FALLING | IRQF_ONESHOT),
				"muic-irq", pmuic);
#endif

		pmuic->irqs.irq_adc1k = irq_adc1k;
		pmuic->irqs.irq_adcerr = irq_adcerr;
		pmuic->irqs.irq_adc = irq_adc;
		pmuic->irqs.irq_chgtyp = irq_chgtyp;
		pmuic->irqs.irq_vbvolt = irq_vbvolt;
#if defined(CONFIG_MUIC_SUPPORT_CCIC)
		pmuic->irqs.irq_dcdtmr = irq_dcdtmr;
#endif

	}
#if defined(CONFIG_MUIC_UNIVERSAL_SM5703)
	else if (!strcmp(pmuic->chip_name,"sm,sm5703")) {
		int irq_int1_attach, irq_int1_detach;
		int irq_int2_vbusdet_on, irq_int2_rid_charger, irq_int2_mhl;
		int irq_int2_adc_chg, irq_int2_rev_acce, irq_int2_vbus_off;

		/* SM5703 MUIC INT1 */
		irq_int1_attach = pdata->irq_gpio + SM5703_MUIC_IRQ_INT1_ATTACH;
		printk(" %s requesting irq no(irq_int1_attach) = %d\n",__func__,irq_int1_attach);
		ret = request_threaded_irq(irq_int1_attach, NULL,
				muic_irq_thread, (IRQF_TRIGGER_FALLING | IRQF_ONESHOT),
				"muic-irq", pmuic);

		irq_int1_detach = pdata->irq_gpio + SM5703_MUIC_IRQ_INT1_DETACH;
		printk(" %s requesting irq no(irq_int1_detach) = %d\n",__func__,irq_int1_detach);
		ret = request_threaded_irq(irq_int1_detach, NULL,
				muic_irq_thread, (IRQF_TRIGGER_FALLING | IRQF_ONESHOT),
				"muic-irq", pmuic);

		/* SM5703 MUIC INT2 */
		irq_int2_vbusdet_on = pdata->irq_gpio + SM5703_MUIC_IRQ_INT2_VBUSDET_ON;
		printk(" %s requesting irq no(irq_int2_vbusdet_on) = %d\n",__func__,irq_int2_vbusdet_on);
		ret = request_threaded_irq(irq_int2_vbusdet_on, NULL,
				muic_irq_thread, (IRQF_TRIGGER_FALLING | IRQF_ONESHOT),
				"muic-irq", pmuic);

		irq_int2_rid_charger = pdata->irq_gpio + SM5703_MUIC_IRQ_INT2_RID_CHARGER;
		printk(" %s requesting irq no(irq_int2_rid_charger) = %d\n",__func__,irq_int2_rid_charger);
		ret = request_threaded_irq(irq_int2_rid_charger, NULL,
				muic_irq_thread, (IRQF_TRIGGER_FALLING | IRQF_ONESHOT),
				"muic-irq", pmuic);

		irq_int2_mhl = pdata->irq_gpio + SM5703_MUIC_IRQ_INT2_MHL;
		printk(" %s requesting irq no(irq_int2_mhl) = %d\n",__func__,irq_int2_mhl);
		ret = request_threaded_irq(irq_int2_mhl, NULL,
				muic_irq_thread, (IRQF_TRIGGER_FALLING | IRQF_ONESHOT),
				"muic-irq", pmuic);

		irq_int2_adc_chg = pdata->irq_gpio + SM5703_MUIC_IRQ_INT2_ADC_CHG;
		printk(" %s requesting irq no(irq_int2_adc_chg) = %d\n",__func__,irq_int2_adc_chg);
		ret = request_threaded_irq(irq_int2_adc_chg, NULL,
				muic_irq_thread, (IRQF_TRIGGER_FALLING | IRQF_ONESHOT),
				"muic-irq", pmuic);

		irq_int2_rev_acce = pdata->irq_gpio + SM5703_MUIC_IRQ_INT2_REV_ACCE;
		printk(" %s requesting irq no(irq_int2_rev_acce) = %d\n",__func__,irq_int2_rev_acce);
		ret = request_threaded_irq(irq_int2_rev_acce, NULL,
				muic_irq_thread, (IRQF_TRIGGER_FALLING | IRQF_ONESHOT),
				"muic-irq", pmuic);

		irq_int2_vbus_off = pdata->irq_gpio + SM5703_MUIC_IRQ_INT2_VBUS_OFF;
		printk(" %s requesting irq no(irq_int2_vbus_off) = %d\n",__func__,irq_int2_vbus_off);
		ret = request_threaded_irq(irq_int2_vbus_off, NULL,
				muic_irq_thread, (IRQF_TRIGGER_FALLING | IRQF_ONESHOT),
				"muic-irq", pmuic);

		pmuic->irqs.irq_int1_attach = irq_int1_attach;
		pmuic->irqs.irq_int1_detach = irq_int1_detach;

		pmuic->irqs.irq_int2_vbusdet_on  = irq_int2_vbusdet_on;
		pmuic->irqs.irq_int2_rid_charger = irq_int2_rid_charger;
		pmuic->irqs.irq_int2_mhl         = irq_int2_mhl;
		pmuic->irqs.irq_int2_adc_chg     = irq_int2_adc_chg;
		pmuic->irqs.irq_int2_rev_acce    = irq_int2_rev_acce;
		pmuic->irqs.irq_int2_vbus_off    = irq_int2_vbus_off;
	}
#elif defined(CONFIG_MUIC_UNIVERSAL_SM5720)
	else if (!strcmp(pmuic->chip_name,"sm,sm5720")) {
		int irq_int1_attach, irq_int1_detach;
		int irq_int2_vbusdet_on, irq_int2_vbus_off;
		int irq_int3_qc20_accepted, irq_int3_afc_error, irq_int3_afc_sta_chg, irq_int3_multi_byte;
		int irq_int3_vbus_update, irq_int3_afc_accepted, irq_int3_afc_ta_attached;

		/* SM5720 MUIC INT1 */
		irq_int1_attach = pdata->irq_gpio + SM5720_MUIC_IRQ_INT1_ATTACH;
		printk(" %s requesting irq no(irq_int1_attach) = %d\n",__func__,irq_int1_attach);
		ret = request_threaded_irq(irq_int1_attach, NULL,
				muic_irq_thread, (IRQF_TRIGGER_FALLING | IRQF_ONESHOT),
				"muic-irq", pmuic);

		irq_int1_detach = pdata->irq_gpio + SM5720_MUIC_IRQ_INT1_DETACH;
		printk(" %s requesting irq no(irq_int1_detach) = %d\n",__func__,irq_int1_detach);
		ret = request_threaded_irq(irq_int1_detach, NULL,
				muic_irq_thread, (IRQF_TRIGGER_FALLING | IRQF_ONESHOT),
				"muic-irq", pmuic);

		/* SM5720 MUIC INT2 */
		irq_int2_vbusdet_on = pdata->irq_gpio + SM5720_MUIC_IRQ_INT2_VBUSDET_ON;
		printk(" %s requesting irq no(irq_int2_vbusdet_on) = %d\n",__func__,irq_int2_vbusdet_on);
		ret = request_threaded_irq(irq_int2_vbusdet_on, NULL,
				muic_irq_thread, (IRQF_TRIGGER_FALLING | IRQF_ONESHOT),
				"muic-irq", pmuic);

		irq_int2_vbus_off = pdata->irq_gpio + SM5720_MUIC_IRQ_INT2_VBUS_OFF;
		printk(" %s requesting irq no(irq_int2_vbus_off) = %d\n",__func__,irq_int2_vbus_off);
		ret = request_threaded_irq(irq_int2_vbus_off, NULL,
				muic_irq_thread, (IRQF_TRIGGER_FALLING | IRQF_ONESHOT),
				"muic-irq", pmuic);

		/* SM5720 MUIC INT3 */
		irq_int3_qc20_accepted = pdata->irq_gpio + SM5720_MUIC_IRQ_INT3_QC20_ACCEPTED;
		printk(" %s requesting irq no(irq_int3_qc20_accepted) = %d\n",__func__,irq_int3_qc20_accepted);
		ret = request_threaded_irq(irq_int3_qc20_accepted, NULL,
				muic_irq_thread, (IRQF_TRIGGER_FALLING | IRQF_ONESHOT),
				"muic-irq", pmuic);

		irq_int3_afc_error = pdata->irq_gpio + SM5720_MUIC_IRQ_INT3_AFC_ERROR;
		printk(" %s requesting irq no(irq_int3_afc_error) = %d\n",__func__,irq_int3_afc_error);
		ret = request_threaded_irq(irq_int3_afc_error, NULL,
				muic_irq_thread, (IRQF_TRIGGER_FALLING | IRQF_ONESHOT),
				"muic-irq", pmuic);

		irq_int3_afc_sta_chg = pdata->irq_gpio + SM5720_MUIC_IRQ_INT3_AFC_STA_CHG;
		printk(" %s requesting irq no(irq_int3_afc_sta_chg) = %d\n",__func__,irq_int3_afc_sta_chg);
		ret = request_threaded_irq(irq_int3_afc_sta_chg, NULL,
				muic_irq_thread, (IRQF_TRIGGER_FALLING | IRQF_ONESHOT),
				"muic-irq", pmuic);

		irq_int3_multi_byte = pdata->irq_gpio + SM5720_MUIC_IRQ_INT3_MULTI_BYTE;
		printk(" %s requesting irq no(irq_int3_multi_byte) = %d\n",__func__,irq_int3_multi_byte);
		ret = request_threaded_irq(irq_int3_multi_byte, NULL,
				muic_irq_thread, (IRQF_TRIGGER_FALLING | IRQF_ONESHOT),
				"muic-irq", pmuic);

		irq_int3_vbus_update = pdata->irq_gpio + SM5720_MUIC_IRQ_INT3_VBUS_UPDATE;
		printk(" %s requesting irq no(irq_int3_vbus_update) = %d\n",__func__,irq_int3_vbus_update);
		ret = request_threaded_irq(irq_int3_vbus_update, NULL,
				muic_irq_thread, (IRQF_TRIGGER_FALLING | IRQF_ONESHOT),
				"muic-irq", pmuic);

		irq_int3_afc_accepted = pdata->irq_gpio + SM5720_MUIC_IRQ_INT3_AFC_ACCEPTED;
		printk(" %s requesting irq no(irq_int3_afc_accepted) = %d\n",__func__,irq_int3_afc_accepted);
		ret = request_threaded_irq(irq_int3_afc_accepted, NULL,
				muic_irq_thread, (IRQF_TRIGGER_FALLING | IRQF_ONESHOT),
				"muic-irq", pmuic);

		irq_int3_afc_ta_attached = pdata->irq_gpio + SM5720_MUIC_IRQ_INT3_AFC_TA_ATTACHED;
		printk(" %s requesting irq no(irq_int3_afc_ta_attached) = %d\n",__func__,irq_int3_afc_ta_attached);
		ret = request_threaded_irq(irq_int3_afc_ta_attached, NULL,
				muic_irq_thread, (IRQF_TRIGGER_FALLING | IRQF_ONESHOT),
				"muic-irq", pmuic);


		pmuic->irqs.irq_int1_attach = irq_int1_attach;
		pmuic->irqs.irq_int1_detach = irq_int1_detach;

		pmuic->irqs.irq_int2_vbusdet_on  = irq_int2_vbusdet_on;
		pmuic->irqs.irq_int2_vbus_off    = irq_int2_vbus_off;

		pmuic->irqs.irq_int3_qc20_accepted   = irq_int3_qc20_accepted;
		pmuic->irqs.irq_int3_afc_error       = irq_int3_afc_error;
		pmuic->irqs.irq_int3_afc_sta_chg     = irq_int3_afc_sta_chg;
		pmuic->irqs.irq_int3_multi_byte      = irq_int3_multi_byte;
		pmuic->irqs.irq_int3_vbus_update     = irq_int3_vbus_update;
		pmuic->irqs.irq_int3_afc_accepted    = irq_int3_afc_accepted;
		pmuic->irqs.irq_int3_afc_ta_attached = irq_int3_afc_ta_attached;
	}
#endif
	else
		ret = request_threaded_irq(pdata->irq_gpio, NULL,
				muic_irq_thread,
				(IRQF_TRIGGER_FALLING | IRQF_ONESHOT),
				"muic-irq", pmuic);
	if (ret < 0) {
		pr_err("%s:%s failed to reqeust IRQ(%d)\n",
				pmuic->chip_name, __func__, pdata->irq_gpio);
		return ret;
	}

	return ret;
}

#define FREE_IRQ(_irq, _dev_id, _name)					\
	do {									\
		if (_irq) {							\
			free_irq(_irq, _dev_id);				\
			pr_info("%s:%s IRQ(%d):%s free done\n", MUIC_DEV_NAME,	\
					__func__, _irq, _name);			\
		}								\
	} while (0)
static void muic_free_irqs(muic_data_t *pmuic)
{
	pr_info("%s:%s\n", MUIC_DEV_NAME, __func__);

	/* free MUIC IRQ */
	FREE_IRQ(pmuic->irqs.irq_vbvolt, pmuic, "muic-vbvolt");
	FREE_IRQ(pmuic->irqs.irq_chgtyp, pmuic, "muic-chgtyp");
	FREE_IRQ(pmuic->irqs.irq_adc, pmuic, "muic-adc");
	FREE_IRQ(pmuic->irqs.irq_adcerr, pmuic, "muic-adcerr");
	FREE_IRQ(pmuic->irqs.irq_adc1k, pmuic, "muic-adc1k");
#if defined(CONFIG_MUIC_SUPPORT_CCIC)
	FREE_IRQ(pmuic->irqs.irq_dcdtmr, pmuic, "muic-dcdtmr");
#endif
#if defined(CONFIG_MUIC_UNIVERSAL_SM5703)
	FREE_IRQ(pmuic->irqs.irq_int1_attach, pmuic, "muic-int1_attach");
	FREE_IRQ(pmuic->irqs.irq_int1_detach, pmuic, "muic-int1_detach");
	FREE_IRQ(pmuic->irqs.irq_int2_vbusdet_on, pmuic, "muic-int2_vbusdet_on");
	FREE_IRQ(pmuic->irqs.irq_int2_rid_charger, pmuic, "muic-int2_rid_charger");
	FREE_IRQ(pmuic->irqs.irq_int2_mhl, pmuic, "muic-int2_mhl");
	FREE_IRQ(pmuic->irqs.irq_int2_adc_chg, pmuic, "muic-int2_adc_chg");
	FREE_IRQ(pmuic->irqs.irq_int2_rev_acce, pmuic, "muic-int2_rev_acce");
	FREE_IRQ(pmuic->irqs.irq_int2_vbus_off, pmuic, "muic-int2_vbus_off");
#elif defined(CONFIG_MUIC_UNIVERSAL_SM5720)
	FREE_IRQ(pmuic->irqs.irq_int1_attach, pmuic, "muic-int1_attach");
	FREE_IRQ(pmuic->irqs.irq_int1_detach, pmuic, "muic-int1_detach");
	FREE_IRQ(pmuic->irqs.irq_int2_vbusdet_on, pmuic, "muic-int2_vbusdet_on");
	FREE_IRQ(pmuic->irqs.irq_int2_vbus_off, pmuic, "muic-int2_vbus_off");
	FREE_IRQ(pmuic->irqs.irq_int3_qc20_accepted, pmuic, "muic-int3_qc20_accepted");
	FREE_IRQ(pmuic->irqs.irq_int3_afc_error, pmuic, "muic-int3_afc_error");
	FREE_IRQ(pmuic->irqs.irq_int3_afc_sta_chg, pmuic, "muic-int3_afc_sta_chg");
	FREE_IRQ(pmuic->irqs.irq_int3_multi_byte, pmuic, "muic-int3_multi_byte");
	FREE_IRQ(pmuic->irqs.irq_int3_vbus_update, pmuic, "muic-int3_vbus_update");
	FREE_IRQ(pmuic->irqs.irq_int3_afc_accepted, pmuic, "muic-int3_afc_accepted");
	FREE_IRQ(pmuic->irqs.irq_int3_afc_ta_attached, pmuic, "muic-int3_afc_ta_attached");
#endif
}

#if defined(CONFIG_MUIC_SUPPORT_EARJACK)
static ssize_t mic_adc_show(struct device *dev,
		struct device_attribute *attr, char *buf)
{
	int value = 0;

	return snprintf(buf, 10, "%d\n", value);
}

static DEVICE_ATTR(mic_adc, 0444 , mic_adc_show, NULL);
#endif

static int muic_probe(struct platform_device *pdev)
{
#if defined(CONFIG_MUIC_UNIVERSAL_SM5703)
	struct sm5703_dev *pmfd = dev_get_drvdata(pdev->dev.parent);
	struct sm5703_platform_data *mfd_pdata = dev_get_platdata(pmfd->dev);
#else
	struct muic_mfd_dev *pmfd = dev_get_drvdata(pdev->dev.parent);
	struct muic_mfd_platform_data *mfd_pdata = dev_get_platdata(pmfd->dev);
#endif
	struct muic_platform_data *pdata = &muic_pdata;
	muic_data_t *pmuic;
	struct regmap_desc *pdesc = NULL;
	struct regmap_ops *pops = NULL;
	int ret = 0;

#if defined(CONFIG_MUIC_SUPPORT_EARJACK)
	struct input_dev *input;
	struct class *audio;
	struct device *earjack;
#endif
	pr_info("%s:%s\n", MUIC_DEV_NAME, __func__);

	pmuic = kzalloc(sizeof(muic_data_t), GFP_KERNEL);
	if (!pmuic) {
		pr_err("%s: failed to allocate driver data\n", __func__);
		ret = -ENOMEM;
		goto err_return;
	}

#if defined(CONFIG_MUIC_SUPPORT_EARJACK)
	input = input_allocate_device();
	if (!input) {
		pr_err("%s: failed to allocate input\n", __func__);
		ret = -ENOMEM;
		goto err_return;
	}
#endif

#if defined(CONFIG_OF)
#if defined(CONFIG_MUIC_UNIVERSAL_SM5703)
	ret = of_muic_dt(pmfd->muic, pdata, pmuic);
#else
	ret = of_muic_dt(pmfd->i2c, pdata, pmuic);
#endif
	if (ret < 0) {
		pr_err("%s:%s Failed to get device of_node \n",
				MUIC_DEV_NAME, __func__);
		goto err_io;
	}
#if defined(CONFIG_MUIC_UNIVERSAL_SM5703)
	of_update_supported_list(pmfd->muic, pdata);
#else
	of_update_supported_list(pmfd->i2c, pdata);
#endif
	vps_show_table();

#endif /* CONFIG_OF */
	if (!pdata) {
		pr_err("%s: failed to get i2c platform data\n", __func__);
		ret = -ENODEV;
		goto err_io;
	}

	mutex_init(&pmuic->muic_mutex);

	pmuic->pdata = pdata;

	pdata->irq_gpio = mfd_pdata->irq_base;
	pr_info("%s: muic irq_gpio = %d\n", __func__, pdata->irq_gpio);
#if defined(CONFIG_MUIC_UNIVERSAL_SM5703)
	pr_info("%s: muic i2c client %s at 0x%02x\n", __func__, pmfd->muic->name, pmfd->muic->addr);
#else
	pr_info("%s: muic i2c client %s at 0x%02x\n", __func__, pmfd->muic->name, pmfd->muic->addr);
	pr_info("%s: mfd i2c client %s at 0x%02x\n", __func__, pmfd->i2c->name, pmfd->i2c->addr);
	pr_info("%s: fuel i2c client %s at 0x%02x\n", __func__, pmfd->fuel->name, pmfd->fuel->addr);
	pr_info("%s: chg  i2c client %s at 0x%02x\n", __func__, pmfd->chg->name, pmfd->chg->addr);
#endif
	pmuic->i2c = pmfd->muic;

	pmuic->is_factory_start = false;
	pmuic->is_otg_test = false;
	pmuic->attached_dev = ATTACHED_DEV_UNKNOWN_MUIC;
	pmuic->is_gamepad = false;
	pmuic->is_dcdtmr_intr = false;
	pmuic->is_ccic_attach = false;
	pmuic->retry_afc = false;
#if defined(CONFIG_SEC_DEBUG)
	pmuic->usb_to_ta_state = false;
#endif

#if defined(CONFIG_MUIC_SUPPORT_EARJACK)
	pmuic->is_earkeypressed = false;
	pmuic->old_keycode = 0;

	/* input */
	pmuic->input = input;
	input->name = pdev->name;
	input->phys = "deskdock-key/input0";
	input->dev.parent = &pdev->dev;

	input->id.bustype = BUS_HOST;
	input->id.vendor = 0x0001;
	input->id.product = 0x0001;
	input->id.version = 0x0001;

	/* Enable auto repeat feature of Linux input subsystem */
	__set_bit(EV_REP, input->evbit);

	pr_info("%s, input->name:%s\n", __func__, input->name);

	input_set_capability(input, EV_KEY, KEY_MEDIA);
	input_set_capability(input, EV_KEY, KEY_VOLUMEUP);
	input_set_capability(input, EV_KEY, KEY_VOLUMEDOWN);

	ret = input_register_device(input);
	if (ret) {
		pr_err("%s: Unable to register input device, "
				"error: %d\n", __func__, ret);
	}
#endif
	if(!strcmp(pmuic->chip_name,"max,max77854"))
		pmuic->vps_table = VPS_TYPE_TABLE;
	else
		pmuic->vps_table = VPS_TYPE_SCATTERED;

	pr_info("%s: VPS_TYPE=%d\n", __func__, pmuic->vps_table);

	if (!pdata->set_gpio_uart_sel) {
		if (pmuic->gpio_uart_sel) {
			pr_info("%s: gpio_uart_sel registered.\n", __func__);
			pdata->set_gpio_uart_sel = muic_set_gpio_uart_sel;
		} else
			pr_info("%s: gpio_uart_sel is not supported.\n", __func__);
	}
#if defined(CONFIG_MUIC_UNIVERSAL_SM5703) || defined(CONFIG_MUIC_UNIVERSAL_SM5720)
	pmuic->pdata->switch_sel=0xB;
#endif
	if (pmuic->pdata->init_gpio_cb) {
		ret = pmuic->pdata->init_gpio_cb(get_switch_sel());
		if (ret) {
			pr_err("%s: failed to init gpio(%d)\n", __func__, ret);
			goto fail_init_gpio;
		}
	}

	if (pmuic->pdata->init_switch_dev_cb)
		pmuic->pdata->init_switch_dev_cb();

	pr_info("  switch_sel : 0x%04x\n", get_switch_sel());

	if (!(get_switch_sel() & SWITCH_SEL_RUSTPROOF_MASK)) {
		pr_info("  Enable rustproof mode\n");
		pmuic->is_rustproof = true;
	} else {
		pr_info("  Disable rustproof mode\n");
		pmuic->is_rustproof = false;
	}

	if (get_afc_mode() == CH_MODE_AFC_DISABLE_VAL) {
		pr_info("  AFC mode disabled\n");
		pmuic->pdata->afc_disable = true;
	} else {
		pr_info("  AFC mode enabled\n");
		pmuic->pdata->afc_disable = false;
	}

	pmuic->pdata->afc_limit_voltage = false;
	/* Register chipset register map. */
	muic_register_regmap(&pdesc, pmuic);
	pdesc->muic = pmuic;
	pops = pdesc->regmapops;
	pmuic->regmapdesc = pdesc;

#if defined(CONFIG_MUIC_SUPPORT_CCIC)
	pmuic->opmode = OPMODE_CCIC;
	pmuic->afc_water_disable = false;
#endif
#if defined(CONFIG_MUIC_SUPPORT_EARJACK)
	audio = class_create(THIS_MODULE, "audio");
	if (IS_ERR(audio)) {
		pr_err("Failed to create class(audio)!\n");
	}

	earjack = device_create(audio, NULL, 0, NULL, "earjack");
	if (IS_ERR(earjack)) {
		pr_err("Failed to create device(earjack)!\n");
	}

	/*add /sys/class/audio/earjack/mic_adc to pass SMD keyshort test*/
	ret = device_create_file(earjack, &dev_attr_mic_adc);
	if (ret)
		pr_err("Failed to create device file in sysfs entries (mic_adc)!\n");

	dev_set_drvdata(earjack, pmuic);
#endif
	/* set switch device's driver_data */
	dev_set_drvdata(switch_device, pmuic);
	dev_set_drvdata(&pdev->dev, pmuic);
	/* create sysfs group */
	ret = sysfs_create_group(&switch_device->kobj, &muic_sysfs_group);
	if (ret) {
		pr_err("%s: failed to create sm5703 muic attribute group\n",
				__func__);
		goto fail;
	}

	ret = pops->revision(pdesc);
	if (ret) {
		pr_err("%s: failed to init muic rev register(%d)\n", __func__,
				ret);
		goto fail;
	}

	ret = muic_reg_init(pmuic);
	if (ret) {
		pr_err("%s: failed to init muic register(%d)\n", __func__, ret);
		goto fail;
	}

	pops->update(pdesc);
	pops->show(pdesc);

#if defined(CONFIG_MUIC_HV)
	hv_afc_init(pmuic);
#endif

	coagent_update_ctx(pmuic);

	/* initial cable detection */
	muic_init_detect(pmuic);

#if defined(CONFIG_MUIC_HV)
	hv_afc_init_detect(pmuic);
#endif

#ifdef DEBUG_MUIC
	INIT_DELAYED_WORK(&pmuic->usb_work, muic_show_debug_info);
	schedule_delayed_work(&pmuic->usb_work, msecs_to_jiffies(10000));
#endif

#if defined(CONFIG_VBUS_NOTIFIER_WORK)
	INIT_WORK(&pmuic->vbus_notify_work, muic_handle_vbus_workq);
#endif

#if defined(CONFIG_MUIC_SUPPORT_CCIC)
	muic_is_ccic_supported_jig(pmuic, pmuic->attached_dev);

	if (pmuic->opmode & OPMODE_CCIC)
		muic_register_ccic_notifier(pmuic);
	else
		pr_info("%s: OPMODE_MUIC. CCIC is not used.\n", __func__);
#endif

#if defined(CONFIG_USB_EXTERNAL_NOTIFY)
	muic_register_usb_notifier(pmuic);
#endif
	muic_online =  true;

#ifdef CONFIG_SEC_BSP
	muic_dump = ipc_log_context_create(5, "muic_dump", 0);
#endif

	muic_irq_init(pmuic);
#if defined(CONFIG_MUIC_HV)
	hv_afc_irq_init(pmuic);
#endif
	return 0;

fail:
	if (pmuic->pdata->cleanup_switch_dev_cb)
		pmuic->pdata->cleanup_switch_dev_cb();
	sysfs_remove_group(&switch_device->kobj, &muic_sysfs_group);
	mutex_unlock(&pmuic->muic_mutex);
	mutex_destroy(&pmuic->muic_mutex);
fail_init_gpio:

err_io:
	kfree(pmuic);
err_return:
	return ret;
}

static int muic_remove(struct platform_device *pdev)
{
	muic_data_t *pmuic = platform_get_drvdata(pdev);
	sysfs_remove_group(&switch_device->kobj, &muic_sysfs_group);

	if (pmuic) {
		pr_info("%s:%s\n", pmuic->chip_name, __func__);

#if defined(CONFIG_MUIC_HV)
	hv_afc_remove(pmuic);
#endif
		cancel_delayed_work(&pmuic->init_work);
		cancel_delayed_work(&pmuic->usb_work);
		disable_irq_wake(pmuic->i2c->irq);
		free_irq(pmuic->i2c->irq, pmuic);

		if (pmuic->pdata->cleanup_switch_dev_cb)
			pmuic->pdata->cleanup_switch_dev_cb();

#if defined(CONFIG_USB_EXTERNAL_NOTIFY)
		muic_unregister_usb_notifier(pmuic);
#endif
		mutex_destroy(&pmuic->muic_mutex);
		i2c_set_clientdata(pmuic->i2c, NULL);
		kfree(pmuic);
	}
	muic_online = false;
	return 0;
}

#if defined(CONFIG_OF)
static struct of_device_id muic_i2c_dt_ids[] = {
	{ .compatible = "muic-universal" },
	{ },
};
MODULE_DEVICE_TABLE(of, muic_i2c_dt_ids);
#endif /* CONFIG_OF */

static void muic_shutdown(struct device *dev)
{
	muic_data_t *pmuic = dev_get_drvdata(dev);
	int ret;
	if(pmuic == NULL)
	{
		printk("sabin pmuic is NULL\n");
		return;
	}
	printk("sabin muic_shutdown start\n");
	pr_info("%s:%s\n", pmuic->chip_name, __func__);
	if (!pmuic->i2c) {
		pr_err("%s:%s no muic i2c client\n", pmuic->chip_name, __func__);
		return;
	}

	muic_free_irqs(pmuic);

#if defined(CONFIG_MUIC_HV)
	hv_afc_remove(pmuic);
#endif

	if (cancel_delayed_work(&pmuic->usb_work))
		pr_info("%s: usb_work canceled.\n", __func__);
	else
		pr_info("%s: usb_work is not pending.\n", __func__);


	if (cancel_delayed_work(&pmuic->init_work))
		pr_info("%s: init_work canceled.\n", __func__);
	else
		pr_info("%s: init_work is not pending.\n", __func__);
	pr_info("%s:%s open D+,D-\n", pmuic->chip_name, __func__);
	ret = com_to_open_with_vbus(pmuic);
	if (ret < 0)
		pr_err("%s:%s fail to open mansw1 reg\n", pmuic->chip_name,
				__func__);

	/* set auto sw mode before shutdown to make sure device goes into */
	/* LPM charging when TA or USB is connected during off state */
	pr_info("%s:%s muic auto detection enable\n", pmuic->chip_name, __func__);
	set_switch_mode(pmuic,SWMODE_AUTO);

	if (pmuic->pdata && pmuic->pdata->cleanup_switch_dev_cb)
		pmuic->pdata->cleanup_switch_dev_cb();

	muic_online =  false;
	pr_info("%s:%s -\n", MUIC_DEV_NAME, __func__);
}

#if defined(CONFIG_PM)
static int muic_suspend(struct device *dev)
{
	muic_data_t *pmuic = dev_get_drvdata(dev);

	cancel_delayed_work(&pmuic->usb_work);

	return 0;
}

static int muic_resume(struct device *dev)
{
	muic_data_t *pmuic = dev_get_drvdata(dev);

	schedule_delayed_work(&pmuic->usb_work, msecs_to_jiffies(1000));

	return 0;
}

static const struct dev_pm_ops muic_pm = {
	.suspend = muic_suspend,
	.resume = muic_resume,
};
#endif /* CONFIG_PM */

static struct platform_driver muic_driver = {
	.driver		= {
		.name	= MUIC_DEV_NAME,
		.owner	= THIS_MODULE,
		.shutdown = muic_shutdown,
#if defined(CONFIG_OF)
		.of_match_table	= muic_i2c_dt_ids,
#endif /* CONFIG_OF */
#if defined(CONFIG_PM)
		.pm = &muic_pm,
#endif
	},
	.probe		= muic_probe,
	.remove		= muic_remove,
};

static int __init muic_init(void)
{
	pr_info("%s:%s\n", MUIC_DEV_NAME, __func__);
	return platform_driver_register(&muic_driver);
}
module_init(muic_init);

static void muic_exit(void)
{
	pr_info("%s:%s\n", MUIC_DEV_NAME, __func__);
	platform_driver_unregister(&muic_driver);
}
module_exit(muic_exit);

MODULE_DESCRIPTION("MUIC driver");
MODULE_AUTHOR("<smilesr.ryu@samsung.com>");
MODULE_LICENSE("GPL");
