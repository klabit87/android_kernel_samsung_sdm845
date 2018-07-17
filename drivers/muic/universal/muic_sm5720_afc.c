/*
 * muic_sm5720_afc.c
 *
 * Copyright (C) 2014 Samsung Electronics
 * Jeongrae Kim <jryu.kim@samsung.com>
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
#include <linux/platform_device.h>
#include <linux/delay.h>
#include <linux/module.h>

#include <linux/muic/muic.h>
#include <linux/ccic/s2mm005.h>

#include "muic-internal.h"
#include "muic_regmap.h"
#include "muic_i2c.h"
#if defined(CONFIG_MUIC_NOTIFIER)
#include <linux/muic/muic_notifier.h>
#endif /* CONFIG_MUIC_NOTIFIER */
#include "muic_regmap_sm5720.h"
#include "muic_sm5720_afc.h"

/* Bit 0 : VBUS_VAILD, Bit 1~7 : Reserved */
#define REG_RSVDID1     0x15
#define	REG_RSVDID2	0x16


#define REG_AFCTXD       0x19
#define REG_AFCSTAT      0x1a
#define REG_VBUSVOL1     0x1b
#define REG_VBUSVOL2     0x1c
#define REG_AFCTASTATUS  0x23
#define	REG_DEVT1	0x0a


muic_data_t *gpmuic;

int sm5720_afc_is_voltage(void);//not used
int sm5720_afc_dpreset(void);

/* To make AFC work properly on boot */
static int is_charger_ready;
static struct work_struct muic_afc_init_work;


int sm5720_set_afc_ctrl_reg(struct regmap_desc *pdesc, int shift, bool on)
{
	muic_data_t *pmuic = pdesc->muic;
	struct i2c_client *i2c = pmuic->i2c;
	u8 reg_val;
	int ret = 0;

	pr_info("%s: Register[%d], set [%d]\n", __func__, shift, on);
	ret = muic_i2c_read_byte(i2c, REG_AFCCNTL);
	if (ret < 0)
		printk(KERN_ERR "[muic] %s(%d)\n", __func__, ret);
	if (on)
		reg_val = ret | (0x1 << shift);
	else
		reg_val = ret & ~(0x1 << shift);

	if (reg_val ^ ret) {
		printk(KERN_DEBUG "[muic] %s reg_val(0x%x)!=AFC_CTRL reg(0x%x), update reg\n",
				__func__, reg_val, ret);

		ret = muic_i2c_write_byte(i2c, REG_AFCCNTL,
				reg_val);
		if (ret < 0)
			printk(KERN_ERR "[muic] %s err write AFC_CTRL(%d)\n",
					__func__, ret);
	} else {
		printk(KERN_DEBUG "[muic] %s (0x%x), just return\n",
				__func__, ret);
		return 0;
	}

	ret = muic_i2c_read_byte(i2c, REG_AFCCNTL);
	if (ret < 0)
		printk(KERN_ERR "[muic] %s err read AFC_CTRL(%d)\n",
				__func__, ret);
	else
		printk(KERN_DEBUG "[muic] %s AFC_CTRL reg after change(0x%x)\n",
				__func__, ret);

	return ret;
}

int sm5720_afc_ta_attach(struct regmap_desc *pdesc)
{
	muic_data_t *pmuic = pdesc->muic;
	struct i2c_client *i2c = pmuic->i2c;
	int ret, value;

	pr_info("%s:%s AFC_TA_ATTACHED \n",MUIC_DEV_NAME, __func__);

	// read clear : AFC_STATUS
	value = muic_i2c_read_byte(i2c, REG_AFCSTAT);
	if (value < 0)
		printk(KERN_ERR "%s: err read AFC_STATUS %d\n", __func__, value);
	pr_info("%s:%s AFC_STATUS [0x%02x]\n",MUIC_DEV_NAME, __func__, value);

	sm5720_afc_delay_check_state(0);

	pmuic->attached_dev = ATTACHED_DEV_AFC_CHARGER_PREPARE_MUIC;
	muic_notifier_attach_attached_dev(pmuic->attached_dev);

	cancel_delayed_work(&pmuic->afc_retry_work);
	schedule_delayed_work(&pmuic->afc_retry_work, msecs_to_jiffies(5000)); // 5sec
	pr_info("%s: afc_retry_work(ATTACH) start \n", __func__);

	/* Some devices have failed the AFC communication in case there is no delay after D- to low by recognized the DCP */
	msleep(120); // 120ms delay

	// voltage(9.0V)  + current(1.65A) setting : 0x46
	// voltage(12.0V) + current(2.1A) setting : 0x79
#if defined(CONFIG_MUIC_HV_12V)
	value = 0x79;
#else
	value = 0x46;
#endif
	ret = muic_i2c_write_byte(i2c, REG_AFCTXD, value);
	if (ret < 0)
		printk(KERN_ERR "[muic] %s: err write AFC_TXD(%d)\n", __func__, ret);
	pr_info("%s:%s AFC_TXD [0x%02x]\n",MUIC_DEV_NAME, __func__, value);

	// ENAFC set '1'
	sm5720_set_afc_ctrl_reg(pdesc, AFCCTRL_ENAFC, 1);
	pr_info("%s:%s AFCCTRL_ENAFC 1 \n",MUIC_DEV_NAME, __func__);
	pmuic->afc_retry_count = 0;
	pmuic->afc_vbus_retry_count = 0;
	pmuic->qc20_vbus = ENQC20_NONE;
	return 0;
}

int sm5720_afc_ta_accept(struct regmap_desc *pdesc)
{
	muic_data_t *pmuic = pdesc->muic;
	struct i2c_client *i2c = pmuic->i2c;
	int device_type3;

	// ENAFC set '0'
	sm5720_set_afc_ctrl_reg(pdesc, AFCCTRL_ENAFC, 0);
	pr_info("%s:%s AFCCTRL_ENAFC 0 \n",MUIC_DEV_NAME, __func__);

	pr_info("%s:%s AFC_ACCEPTED \n",MUIC_DEV_NAME, __func__);

	cancel_delayed_work(&pmuic->afc_retry_work);
	pr_info("%s: afc_retry_work(ACCEPTED) cancel \n", __func__);

	device_type3 = muic_i2c_read_byte(i2c, REG_DEVT3);
	pr_info("%s: dev3 [0x%02x]\n",MUIC_DEV_NAME, device_type3);
	if (device_type3 & DEV_TYPE3_AFC_TA) {
		// VBUS_READ
		sm5720_set_afc_ctrl_reg(pdesc, AFCCTRL_VBUS_READ, 1);
		pr_info("%s:%s VBUS READ start(AFC)\n",MUIC_DEV_NAME, __func__);
		if (pmuic->attached_dev != ATTACHED_DEV_AFC_CHARGER_PREPARE_MUIC) {
			pmuic->attached_dev = ATTACHED_DEV_AFC_CHARGER_PREPARE_MUIC;
			muic_notifier_attach_attached_dev(pmuic->attached_dev);
		}
		pmuic->afc_vbus_retry_count++;
	} else {
		pmuic->attached_dev = ATTACHED_DEV_AFC_CHARGER_5V_MUIC;
		muic_notifier_attach_attached_dev(pmuic->attached_dev);
		pr_info("%s:%s  attached_dev(%d) \n",MUIC_DEV_NAME, __func__, pmuic->attached_dev);
	}

	return 0;

}

int sm5720_afc_vbus_update(struct regmap_desc *pdesc)
{
	muic_data_t *pmuic = pdesc->muic;
	struct i2c_client *i2c = pmuic->i2c;
	int vbus_txd_voltage;
	int vbus_voltage, voltage1, voltage2;
	int voltage_min = 0, voltage_max = 0;
	int device_type3;
	int ret,reg_val;

	pr_info("%s:%s AFC_VBUS_UPDATE \n",MUIC_DEV_NAME, __func__);

	if (pmuic->attached_dev == ATTACHED_DEV_NONE_MUIC) {
		pr_info("%s:%s Device type is None\n",MUIC_DEV_NAME, __func__);
		return 0;
	}

	voltage1 = muic_i2c_read_byte(i2c, REG_VBUSVOL1);
	voltage2 = muic_i2c_read_byte(i2c, REG_VBUSVOL2);
	vbus_voltage = voltage1*1000 + (voltage2*3900)/1000;

	pr_info("%s: voltage1=[0x%02x], voltage2=[0x%02x], vbus_voltage=%d mV \n",
			MUIC_DEV_NAME, voltage1,voltage2,vbus_voltage);

	device_type3 = muic_i2c_read_byte(i2c, REG_DEVT3);
	pr_info("%s:%s DEVICE_TYPE3 [0x%02x]\n",MUIC_DEV_NAME, __func__, device_type3);
	if (device_type3 & DEV_TYPE3_QC20_TA){ // QC20_TA vbus update
		if (pmuic->qc20_vbus == ENQC20_12V){
			voltage_min  = 10000; // - 10000mV
			voltage_max  = 13000; // + 13000mV
		} else if (pmuic->qc20_vbus == ENQC20_9V){
			voltage_min  = 7000;  // - 7000mV
			voltage_max  = 10000; // + 10000mV
		}

		pr_info("%s:%s QC20 vbus_voltage:%d mV (%d)\n"
				,MUIC_DEV_NAME, __func__,vbus_voltage, pmuic->qc20_vbus );

		if ( (voltage_min <= vbus_voltage) && (vbus_voltage <= voltage_max) ) { /* AFC DONE */
			/*
			   if (pmuic->qc20_vbus == ENQC20_12V){
			   pmuic->attached_dev = ATTACHED_DEV_QC_CHARGER_12V_MUIC;
			   muic_notifier_attach_attached_dev(pmuic->attached_dev);
			   pr_info("%s:%s QC20 12V (%d)\n",MUIC_DEV_NAME, __func__,pmuic->attached_dev);
			   } else */
			if(pmuic->qc20_vbus == ENQC20_9V){
				pmuic->attached_dev = ATTACHED_DEV_QC_CHARGER_9V_MUIC;
				muic_notifier_attach_attached_dev(pmuic->attached_dev);
				pr_info("%s:%s QC20 9V (%d)\n",MUIC_DEV_NAME, __func__,pmuic->attached_dev);
			}
		} else { // vbus fail
			if (pmuic->qc20_vbus == ENQC20_12V){ // 9V retry
				if ( pmuic->afc_vbus_retry_count < 3 ) {
					msleep(100);
					pmuic->afc_vbus_retry_count++;

					sm5720_set_afc_ctrl_reg(pdesc, AFCCTRL_VBUS_READ, 1);
					pr_info("%s:%s [QC20-12V] VBUS READ retry = %d \n",MUIC_DEV_NAME, __func__,
							pmuic->afc_vbus_retry_count);
				} else {
					msleep(100);
					pmuic->qc20_vbus = ENQC20_9V;
					ret = muic_i2c_read_byte(i2c, REG_AFCCNTL);
					reg_val = (ret & 0x3F) | (ENQC20_9V<<6);    // QC20 9V
					muic_i2c_write_byte(i2c, REG_AFCCNTL, reg_val);
					pr_info("%s:%s read REG_AFCCNTL=0x%x ,  write REG_AFCCNTL=0x%x , qc20_vbus=%d \n",
							MUIC_DEV_NAME, __func__, ret, reg_val, pmuic->qc20_vbus );

					// VBUS_READ
					sm5720_set_afc_ctrl_reg(pdesc, AFCCTRL_VBUS_READ, 1);
					pr_info("%s:%s VBUS READ start(QC20-9V)\n",MUIC_DEV_NAME, __func__);
					pmuic->afc_vbus_retry_count = 0;
					return 0;
				}
			} else {
				if ( pmuic->afc_vbus_retry_count < 3 ) {
					msleep(100);
					pmuic->afc_vbus_retry_count++;

					sm5720_set_afc_ctrl_reg(pdesc, AFCCTRL_VBUS_READ, 1);
					pr_info("%s:%s [QC20-9V] VBUS READ retry = %d \n",MUIC_DEV_NAME, __func__,
							pmuic->afc_vbus_retry_count);
				} else {
					pmuic->qc20_vbus = ENQC20_NONE;
					ret = muic_i2c_read_byte(i2c, REG_AFCCNTL);
					reg_val = (ret & 0x3F) | (ENQC20_NONE<<6);    // QC20 noce
					muic_i2c_write_byte(i2c, REG_AFCCNTL, reg_val);
					pr_info("%s:%s read REG_AFCCNTL=0x%x ,  write REG_AFCCNTL=0x%x , qc20_vbus=%d \n",
							MUIC_DEV_NAME, __func__, ret, reg_val, pmuic->qc20_vbus );

					pmuic->attached_dev = ATTACHED_DEV_QC_CHARGER_5V_MUIC;
					muic_notifier_attach_attached_dev(pmuic->attached_dev);
				}
			}
		}

	} else { // AFC vbus update
		vbus_txd_voltage = muic_i2c_read_byte(i2c, REG_AFCTXD);
		pr_info("%s: AFC_TXD [0x%02x]\n",MUIC_DEV_NAME, vbus_txd_voltage);
		vbus_txd_voltage = ((vbus_txd_voltage&0xF0)>>4)*1000 + 5000;

		pr_info("%s:%s vbus_voltage:%d mV , AFC_TXD_VOLTAGE:%d mV \n"
				,MUIC_DEV_NAME, __func__,vbus_voltage, vbus_txd_voltage);

		voltage_min = vbus_txd_voltage - 2000; // - 2000mV
		voltage_max = vbus_txd_voltage + 1000; // + 1000mV

		if ( (voltage_min <= vbus_voltage) && (vbus_voltage <= voltage_max) ) { /* AFC DONE */
			pmuic->afc_vbus_retry_count = 0;

			pr_info("%s:%s AFC done \n",MUIC_DEV_NAME, __func__);
			if (vbus_txd_voltage == 12000) { // 12V
				pmuic->attached_dev = ATTACHED_DEV_AFC_CHARGER_12V_MUIC;
				muic_notifier_attach_attached_dev(pmuic->attached_dev);
				pr_info("%s:%s AFC 12V (%d)\n",MUIC_DEV_NAME, __func__,pmuic->attached_dev);
			} else if (vbus_txd_voltage == 9000){ // 9V
				pmuic->attached_dev = ATTACHED_DEV_AFC_CHARGER_9V_MUIC;
				muic_notifier_attach_attached_dev(pmuic->attached_dev);
				pr_info("%s:%s AFC 9V (%d)\n",MUIC_DEV_NAME, __func__,pmuic->attached_dev);
			} else {  // 5V
				pmuic->attached_dev = ATTACHED_DEV_AFC_CHARGER_5V_MUIC;
				muic_notifier_attach_attached_dev(pmuic->attached_dev);
				pr_info("%s:%s AFC 5V (%d)\n",MUIC_DEV_NAME, __func__,pmuic->attached_dev);
			}
		} else {
			// VBUS_READ
			if ( pmuic->afc_vbus_retry_count < 3 ) {
				msleep(100);
				pmuic->afc_vbus_retry_count++;

				sm5720_set_afc_ctrl_reg(pdesc, AFCCTRL_VBUS_READ, 1);
				pr_info("%s:%s VBUS READ retry = %d \n",MUIC_DEV_NAME, __func__,
						pmuic->afc_vbus_retry_count);
			} else {
				pmuic->attached_dev = ATTACHED_DEV_AFC_CHARGER_5V_MUIC;
				muic_notifier_attach_attached_dev(pmuic->attached_dev);
				pmuic->afc_vbus_retry_count = 0;
			}
		}
	} // if (device_type3 & DEV_TYPE3_QC20_TA) end

	return 0;
}

int sm5720_afc_multi_byte(struct regmap_desc *pdesc)
{
	muic_data_t *pmuic = pdesc->muic;
	struct i2c_client *i2c = pmuic->i2c;
	int multi_byte[6] = {0,0,0,0,0,0};
	int i;
	int ret;
	int voltage_find;

	// ENAFC set '0'
	sm5720_set_afc_ctrl_reg(pdesc, AFCCTRL_ENAFC, 0);
	pr_info("%s:%s AFCCTRL_ENAFC 0 \n",MUIC_DEV_NAME, __func__);

	pr_info("%s:%s AFC_MULTI_BYTE \n",MUIC_DEV_NAME, __func__);

	// read AFC_RXD1 ~ RXD6
	voltage_find = 0;
	for(i = 0 ; i < 6 ; i++ ){
		multi_byte[i] = muic_i2c_read_byte(i2c, REG_AFCRXD1 + i);
		if (multi_byte[i] < 0)
			printk(KERN_ERR "%s: err read AFC_RXD%d %d\n", __func__,i+1, multi_byte[i]);
		pr_info("%s:%s AFC_RXD%d [0x%02x]\n",MUIC_DEV_NAME, __func__,i+1, multi_byte[i]);

		if( i >= 1 ){   // voltate find
			if ( ((multi_byte[i]&0xF0)>>4) >= ((multi_byte[voltage_find]&0xF0)>>4) ) {
				voltage_find = i;
			}
		}
	}

	pr_info("%s:%s AFC_TXD multi_byte[%d]=0x%02x \n", MUIC_DEV_NAME, __func__,
			voltage_find+1, multi_byte[voltage_find] );

	ret = muic_i2c_write_byte(i2c, REG_AFCTXD, multi_byte[voltage_find]);
	if (ret < 0)
		printk(KERN_ERR "[muic] %s: err write AFC_TXD(%d)\n", __func__, ret);

	// ENAFC set '1'
	sm5720_set_afc_ctrl_reg(pdesc, AFCCTRL_ENAFC, 1);
	pr_info("%s:%s AFCCTRL_ENAFC 1 \n",MUIC_DEV_NAME, __func__);

	return 0;

}

int sm5720_afc_error(struct regmap_desc *pdesc)
{
	muic_data_t *pmuic = pdesc->muic;
	struct i2c_client *i2c = pmuic->i2c;
	int value;
	int device_type3;
	int ret,reg_val;

	// ENAFC set '0'
	sm5720_set_afc_ctrl_reg(pdesc, AFCCTRL_ENAFC, 0);
	pr_info("%s:%s AFCCTRL_ENAFC 0 \n",MUIC_DEV_NAME, __func__);

	pr_info("%s:%s AFC_ERROR (%d) \n",MUIC_DEV_NAME, __func__, pmuic->afc_retry_count);

	// read AFC_STATUS
	value = muic_i2c_read_byte(i2c, REG_AFCSTAT);
	if (value < 0)
		printk(KERN_ERR "%s: err read AFC_STATUS %d\n", __func__, value);
	pr_info("%s:%s AFC_STATUS [0x%02x]\n",MUIC_DEV_NAME, __func__, value);

	if (pmuic->afc_retry_count < 5) {
		device_type3 = muic_i2c_read_byte(i2c, REG_DEVT3);
		pr_info("%s:%s DEVICE_TYPE3 [0x%02x]\n",MUIC_DEV_NAME, __func__, device_type3);
		if (device_type3 & DEV_TYPE3_QC20_TA){ // QC20_TA

			ret = muic_i2c_read_byte(i2c, REG_AFCCNTL);
			switch (pmuic->qc_support) {
				case SUPPORT_QC_9V:
					reg_val = (ret & 0x3F) | (ENQC20_9V<<6);    // QC20 9V
					pmuic->qc20_vbus = ENQC20_9V;
					break;
				case SUPPORT_QC_12V:
					reg_val = (ret & 0x3F) | (ENQC20_12V<<6);    // QC20 12V
					pmuic->qc20_vbus = ENQC20_12V;
					break;
				case SUPPORT_QC_20V:
					reg_val = (ret & 0x3F) | (ENQC20_20V<<6);    // QC20 20V
					pmuic->qc20_vbus = ENQC20_20V;
					break;
				default:
					reg_val = (ret & 0x3F) | (ENQC20_NONE<<6);    // QC20 5V
					pmuic->qc20_vbus = ENQC20_NONE;
					pr_err("%s:%s not support QC Charger\n", MUIC_DEV_NAME, __func__);
					break;
			}
			muic_i2c_write_byte(i2c, REG_AFCCNTL, reg_val);
			pr_info("%s:%s read REG_AFCCNTL=0x%x ,  write REG_AFCCNTL=0x%x , qc20_vbus=%d \n",
					MUIC_DEV_NAME, __func__, ret, reg_val, pmuic->qc20_vbus );

			msleep(40); // 40ms delay

			// VBUS_READ
			sm5720_set_afc_ctrl_reg(pdesc, AFCCTRL_VBUS_READ, 1);
			pr_info("%s:%s VBUS READ start(QC20-12V)\n",MUIC_DEV_NAME, __func__);
		} else {
			msleep(10); // 10ms delay
			// ENAFC set '1'
			sm5720_set_afc_ctrl_reg(pdesc, AFCCTRL_ENAFC, 1);
			pmuic->afc_retry_count++;
			pr_info("%s:%s re-start AFC (afc_retry_count=%d)\n",MUIC_DEV_NAME, __func__, pmuic->afc_retry_count);
		}
	} else {
		pr_info("%s:%s  ENAFC end = %d \n",MUIC_DEV_NAME, __func__, pmuic->afc_retry_count);
		pmuic->attached_dev = ATTACHED_DEV_AFC_CHARGER_5V_MUIC;
		muic_notifier_attach_attached_dev(pmuic->attached_dev);
	}
	return 0;
}

int sm5720_afc_init_check(struct regmap_desc *pdesc)
{
	muic_data_t *pmuic = pdesc->muic;
	struct afc_ops *afcops = pmuic->regmapdesc->afcops;
	int afc_ta_attached;

	pr_info("%s:%s AFC_INIT_CHECK\n",MUIC_DEV_NAME, __func__);

	if(pmuic->pdata->afc_disable == true) {
		pr_info("%s:%s AFC_mode is disable\n",MUIC_DEV_NAME, __func__);
		return 0;
	}

	pr_info("%s:%s pmuic->vps.s.val1[0x%02x] , pmuic->vps.s.val3[0x%02x]\n",
			MUIC_DEV_NAME, __func__, pmuic->vps.s.val1, pmuic->vps.s.val3);

	/* check afc interrupt state */
	afc_ta_attached = muic_i2c_read_byte(pmuic->i2c, REG_AFCTASTATUS);
	pr_info("%s:%s REG_AFCTASTATUS:[0x%02x]\n", MUIC_DEV_NAME, __func__, afc_ta_attached);

	if ((afc_ta_attached & 0x01) && (pmuic->ccic_rp == Rp_56K)){ // AFC_TA_ATTACHED
#if !defined(CONFIG_SEC_FACTORY)
		if(pmuic->is_ccic_attach)
			afcops->afc_ta_attach(pmuic->regmapdesc);
		else {
			pmuic->retry_afc = true;
			pr_info("%s: Need AFC restart for late ccic_attach\n", __func__);
		}
#else
		afcops->afc_ta_attach(pmuic->regmapdesc);
#endif
	}

	return 0;
}

int sm5720_afc_is_voltage(void)
{
	struct i2c_client *i2c = gpmuic->i2c;
	int vbus_voltage, voltage1, voltage2;

	if (gpmuic->attached_dev == ATTACHED_DEV_NONE_MUIC) {
		pr_info("%s attached_dev None \n", __func__);
		return 0;
	}
	voltage1 = muic_i2c_read_byte(i2c, REG_VBUSVOL1);
	voltage2 = muic_i2c_read_byte(i2c, REG_VBUSVOL2);

	vbus_voltage = voltage1*1000 + (voltage2*3900)/1000;

	return vbus_voltage;
}

int sm5720_afc_dpreset(void)
{
	struct afc_ops *afcops = gpmuic->regmapdesc->afcops;

	pr_info("%s: gpmuic->attached_dev = %d\n", __func__, gpmuic->attached_dev);
	if ( (gpmuic->attached_dev == ATTACHED_DEV_AFC_CHARGER_9V_MUIC) ||
			(gpmuic->attached_dev == ATTACHED_DEV_AFC_CHARGER_12V_MUIC) ||
			(gpmuic->attached_dev == ATTACHED_DEV_QC_CHARGER_9V_MUIC) ||
			(gpmuic->attached_dev == ATTACHED_DEV_AFC_CHARGER_PREPARE_MUIC) ) {
		// ENAFC set '0'
		afcops->afc_ctrl_reg(gpmuic->regmapdesc, AFCCTRL_ENAFC, 0);
		msleep(50); // 50ms delay

		sm5720_afc_delay_check_state(0);
		// DP_RESET
		pr_info("%s:AFC Disable \n", __func__);
		afcops->afc_ctrl_reg(gpmuic->regmapdesc, AFCCTRL_DP_RESET, 1);

		if (gpmuic->attached_dev != ATTACHED_DEV_NONE_MUIC) {
			gpmuic->attached_dev = ATTACHED_DEV_AFC_CHARGER_5V_MUIC;
			muic_notifier_attach_attached_dev(ATTACHED_DEV_AFC_CHARGER_5V_MUIC);
		} else {
			pr_info("%s:Seems TA is removed.\n", __func__);
		}
	}

	return 0;
}

int sm5720_afc_restart(void)
{
	struct i2c_client *i2c = gpmuic->i2c;
	int ret, value;
	struct afc_ops *afcops = gpmuic->regmapdesc->afcops;
	int afc_ta_attached;

	pr_info("%s:AFC Restart attached_dev = 0x%x\n", __func__, gpmuic->attached_dev);

	if (gpmuic->attached_dev == ATTACHED_DEV_NONE_MUIC) {
		pr_info("%s:%s Device type is None\n",MUIC_DEV_NAME, __func__);
		return 0;
	}

	msleep(120); // 120ms delay
	afc_ta_attached = muic_i2c_read_byte(i2c, REG_AFCTASTATUS);
	pr_info("%s: afc_ta_attached = 0x%x\n", __func__, afc_ta_attached );
	if (afc_ta_attached == 0x00){
		return 0;
	}

	gpmuic->attached_dev = ATTACHED_DEV_AFC_CHARGER_PREPARE_MUIC;
	muic_notifier_attach_attached_dev(gpmuic->attached_dev);
	cancel_delayed_work(&gpmuic->afc_retry_work);
	schedule_delayed_work(&gpmuic->afc_retry_work, msecs_to_jiffies(5000)); // 5sec
	pr_info("%s: afc_retry_work(RESTART) start \n", __func__);

	// voltage(9.0V)  + current(1.65A) setting : 0x46
	// voltage(12.0V) + current(2.1A) setting : 0x79
#if defined(CONFIG_MUIC_HV_12V)
	value = 0x79;
#else
	value = 0x46;
#endif
	ret = muic_i2c_write_byte(i2c, REG_AFCTXD, value);
	if (ret < 0)
		printk(KERN_ERR "[muic] %s: err write AFC_TXD(%d)\n", __func__, ret);
	pr_info("%s:AFC_TXD [0x%02x]\n", __func__, value);

	// ENAFC set '1'
	afcops->afc_ctrl_reg(gpmuic->regmapdesc, AFCCTRL_ENAFC, 1);
	gpmuic->afc_retry_count = 0;
	gpmuic->afc_vbus_retry_count = 0;

	return 0;
}

static void sm5720_afc_retry_work(struct work_struct *work)
{
	struct afc_ops *afcops = gpmuic->regmapdesc->afcops;
	struct i2c_client *i2c = gpmuic->i2c;
	int ret, vbus;

	pr_info("%s:AFC retry work\n", __func__);

	ret = muic_i2c_read_byte(i2c, REG_AFCSTAT);
	pr_info("%s : Read REG_AFCSTAT = [0x%02x]\n", __func__, ret);

	ret = muic_i2c_read_byte(i2c, 0x3C);
	pr_info("%s : Read 0x3C = [0x%02x]\n", __func__, ret);

	ret = muic_i2c_read_byte(i2c, 0x3D);
	pr_info("%s : Read 0x3D = [0x%02x]\n", __func__, ret);

	pr_info("%s:attached_dev = %d \n", __func__, gpmuic->attached_dev);
	if (gpmuic->attached_dev == ATTACHED_DEV_AFC_CHARGER_PREPARE_MUIC) {
		vbus = muic_i2c_read_byte(i2c, REG_RSVDID1);
		if (!(vbus & 0x01)) {
			pr_info("%s:%s VBUS is nothing\n",MUIC_DEV_NAME, __func__);
			gpmuic->attached_dev = ATTACHED_DEV_NONE_MUIC;
			muic_notifier_attach_attached_dev(gpmuic->attached_dev);
			return;
		}
		sm5720_afc_delay_check_state(0);

		pr_info("%s: [MUIC] device type is afc prepare - Disable AFC\n", __func__);
		afcops->afc_ctrl_reg(gpmuic->regmapdesc, AFCCTRL_DP_RESET, 1);
	}
}

void sm5720_afc_delay_check_state(int state)
{
	struct i2c_client *i2c = gpmuic->i2c;
    int ret;

	pr_info("%s : state = %d \n", __func__, state);

    if ( state == 1 ) {
        ret = muic_i2c_read_byte(i2c, REG_RSVDID2);
        ret = ret | 0x08;
        muic_i2c_write_byte(i2c, REG_RSVDID2, ret); // VDP_SRC_EN '0' -> '1'

        gpmuic->delay_check_count = 0;
    	cancel_delayed_work(&gpmuic->afc_delay_check_work);
    	schedule_delayed_work(&gpmuic->afc_delay_check_work, msecs_to_jiffies(1700)); // 1.7 sec
    	pr_info("%s: afc_delay_check_work start\n", __func__);
    } else {
        ret = muic_i2c_read_byte(i2c, REG_RSVDID2);
        ret = ret & 0xF7;
        muic_i2c_write_byte(i2c, REG_RSVDID2, ret); // VDP_SRC_EN '1' -> '0'
        cancel_delayed_work(&gpmuic->afc_delay_check_work);
        pr_info("%s: afc_delay_check_work cancel (%d)\n", __func__ ,gpmuic->delay_check_count);
        gpmuic->delay_check_count = 0;
    }

}


static void sm5720_afc_delay_check_work(struct work_struct *work)
{
	struct afc_ops *afcops = gpmuic->regmapdesc->afcops;
	struct i2c_client *i2c = gpmuic->i2c;
    int val1;

	pr_info("%s: attached_dev = %d\n", __func__, gpmuic->attached_dev);

    pr_info("%s: delay_check_count = %d \n", __func__, gpmuic->delay_check_count);
    if (gpmuic->delay_check_count > 40) {
        sm5720_afc_delay_check_state(0);
        return;
    }

    val1 = muic_i2c_read_byte(i2c, REG_DEVT1);
    pr_info("%s:val1 = 0x%x \n", __func__, val1);
    if ( (gpmuic->attached_dev == ATTACHED_DEV_TA_MUIC) && (val1 == 0x40) ){
        pr_info("%s: DP_RESET\n", __func__);
        afcops->afc_ctrl_reg(gpmuic->regmapdesc, AFCCTRL_DIS_AFC, 1);
        msleep(60);
        afcops->afc_ctrl_reg(gpmuic->regmapdesc, AFCCTRL_DIS_AFC, 0);

    	cancel_delayed_work(&gpmuic->afc_delay_check_work);
    	schedule_delayed_work(&gpmuic->afc_delay_check_work, msecs_to_jiffies(1700)); // 1.7 sec
    	gpmuic->delay_check_count++;
        pr_info("%s: afc_delay_check_work retry : %d \n", __func__, gpmuic->delay_check_count);
    }
}


static void sm5720_afc_focrced_detection_by_charger(struct work_struct *work)
{
	struct afc_ops *afcops = gpmuic->regmapdesc->afcops;

	pr_info("%s\n", __func__);

	mutex_lock(&gpmuic->muic_mutex);

	afcops->afc_init_check(gpmuic->regmapdesc);

	mutex_unlock(&gpmuic->muic_mutex);
}

void sm5720_muic_charger_init(void)
{
	pr_info("%s\n", __func__);

	if (!gpmuic) {
		pr_info("%s: MUIC AFC is not ready.\n", __func__);
		return;
	}

	if (is_charger_ready) {
		pr_info("%s: charger is already ready.\n", __func__);
		return;
	}

	is_charger_ready = true;

	if (gpmuic->attached_dev == ATTACHED_DEV_TA_MUIC)
		schedule_work(&muic_afc_init_work);
}

int sm5720_afc_set_voltage(int vol)
{
	struct muic_platform_data *pdata = gpmuic->pdata;
	if(!pdata) {
		pr_err("%s:%s cannot read pmuic!\n", MUIC_DEV_NAME, __func__);
		return 0;
	}

	if (vol == 5) {
		pdata->afc_limit_voltage = true;
		sm5720_afc_dpreset();
	} else if (vol == 9 || vol == 12) {
		pdata->afc_limit_voltage = false;
		sm5720_afc_restart();
	} else {
		pr_warn("%s:%s invalid value\n", MUIC_DEV_NAME, __func__);
		return 0;
	}

	return 1;
}

int muic_sm5720_set_afc(bool enable)
{
	struct muic_platform_data *pdata = gpmuic->pdata;
	if(!pdata) {
		pr_err("%s:%s cannot read pmuic!\n", MUIC_DEV_NAME, __func__);
		return 0;
	}

	if (enable) {
		pdata->afc_limit_voltage = false;
		sm5720_afc_dpreset();
	} else {
		pdata->afc_limit_voltage = true;
		sm5720_afc_dpreset();
	}

	return 1;
}

void sm5720_afc_init_state(muic_data_t *pmuic)
{
	gpmuic = pmuic;

	/* To make AFC work properly on boot */
	INIT_WORK(&muic_afc_init_work, sm5720_afc_focrced_detection_by_charger);
	INIT_DELAYED_WORK(&gpmuic->afc_retry_work, sm5720_afc_retry_work);

	INIT_DELAYED_WORK(&gpmuic->afc_delay_check_work, sm5720_afc_delay_check_work);

	pr_info("%s:attached_dev = %d\n", __func__, gpmuic->attached_dev);
}

MODULE_DESCRIPTION("MUIC driver");
MODULE_AUTHOR("<jryu.kim@samsung.com>");
MODULE_LICENSE("GPL");
