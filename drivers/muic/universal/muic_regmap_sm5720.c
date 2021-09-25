/*
 * muic_regmap_sm5720.c
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
#include <linux/string.h>

#include <linux/muic/muic.h>

#if defined(CONFIG_MUIC_NOTIFIER)
#include <linux/muic/muic_notifier.h>
#endif /* CONFIG_MUIC_NOTIFIER */

#if defined(CONFIG_OF)
#include <linux/of_device.h>
#include <linux/of_gpio.h>
#endif /* CONFIG_OF */

#include "muic_regmap_sm5720.h"
#include "muic-internal.h"
#include "muic_i2c.h"
#include "muic_regmap.h"
#include "muic_debug.h"

#if defined(CONFIG_MUIC_HV)
#include "muic_sm5720_afc.h"
#endif

extern void muic_afc_delay_check_state(int state);
static void sm5720_set_switching_mode(struct regmap_desc *pdesc, int mode);

/* ADC Scan Mode Values : b'1 */
static struct reg_value_set sm5720_adc_scanmode_tbl[] = {
	[ADC_SCANMODE_CONTINUOUS] = {0x01, "Periodic"},
	[ADC_SCANMODE_ONESHOT]    = {0x00, "Oneshot."},
	[ADC_SCANMODE_PULSE]      = {0x00, "Oneshot.."},
};

static int sm5720_com_value_tbl[] = {
	[COM_OPEN]      = _COM_OPEN,
	//  [COM_OPEN_WITH_V_BUS]   = _COM_OPEN_WITH_V_BUS,
	[COM_UART_AP]   = _COM_UART_AP,
	[COM_UART_CP]   = _COM_UART_CP,
	[COM_USB_AP]    = _COM_USB_AP,
	[COM_USB_CP]    = _COM_USB_CP,
	[COM_AUDIO]     = _COM_AUDIO,
};

static regmap_t sm5720_muic_regmap_table[] = {
	[REG_DEVID]       = {"DeviceID",      0x01, 0x00, INIT_NONE},
	[REG_CTRL]        = {"CONTROL",       0x1E, 0x00, REG_CTRL_INITIAL,},
	[REG_INT1]        = {"INT1",          0x00, 0x00, INIT_NONE,},
	[REG_INT2]        = {"INT2",          0x00, 0x00, INIT_NONE,},
	[REG_INT3]        = {"INT3_AFC",      0x00, 0x00, INIT_NONE,},
	[REG_INTMASK1]    = {"INTMASK1",      0x00, 0x00, REG_INTMASK1_VALUE,},
	[REG_INTMASK2]    = {"INTMASK2",      0x00, 0x00, REG_INTMASK2_VALUE,},
	[REG_INTMASK3]    = {"INTMASK3_AFC",  0x00, 0x00, REG_INTMASK3AFC_VALUE,},
	[REG_ADC]         = {"ADC",           0x1F, 0x00, INIT_NONE,},
	[REG_DEVT1]       = {"DEVICETYPE1",   0x00, 0x00, INIT_NONE,},
	[REG_DEVT2]       = {"DEVICETYPE2",   0x00, 0x00, INIT_NONE,},
	[REG_DEVT3]       = {"DEVICETYPE3",   0x00, 0x00, INIT_NONE,},
	[REG_TIMING1]     = {"TimingSet1",    0x00, 0x00, REG_TIMING1_VALUE,},
	[REG_TIMING2]     = {"TimingSet2",    0x00, 0x00, INIT_NONE,},
	/* 0x0F: Reserved */
	[REG_BTN1]        = {"BUTTON1",       0x00, 0x00, INIT_NONE,},
	[REG_BTN2]        = {"BUTTON2",       0x00, 0x00, INIT_NONE,},
	[REG_CarKit]      = {"CarKitStatus",  0x00, 0x00, INIT_NONE,},
	[REG_MANSW1]      = {"ManualSW1",     0x00, 0x00, INIT_NONE,},
	[REG_MANSW2]      = {"ManualSW2",     0x00, 0x00, INIT_NONE,},
	[REG_RSVDID1]     = {"Reserved_ID1",  0x00, 0x00, INIT_NONE,},
	[REG_RSVDID2]     = {"Reserved_ID2",  0x24, 0x00, REG_RSVDID2_VALUE,},
	[REG_CHGTYPE]     = {"REG_CHG_TYPE",  0x00, 0x00, INIT_NONE,},
	/* 0x18 ~ 0x23: AFC */
#if defined(CONFIG_SUPPORT_QC30)
	[REG_AFCCNTL]     = {"AFC_CNTL",      0x00, 0x00, REG_AFCCNTL_VALUE,},
#else
	[REG_AFCCNTL]     = {"AFC_CNTL",      0x00, 0x00, INIT_NONE,},
#endif
	[REG_AFCTXD]      = {"AFC_TXD",       0x46, 0x00, REG_AFC_TXD_VALUE,}, /* 0x46 : 9V / 1.65A , 0x79 : 12V,2.1A */
	[REG_AFCSTAT]     = {"AFC_STATUS",    0x00, 0x00, INIT_NONE,},
	[REG_VBUSVOL1]    = {"VBUS_STATUS",   0x00, 0x00, INIT_NONE,},
	[REG_VBUSVOL2]    = {"VBUS_STATUS",   0x00, 0x00, INIT_NONE,},
	[REG_AFCRXD1]     = {"AFC_RXD1",      0x00, 0x00, INIT_NONE,},
	[REG_AFCRXD2]     = {"AFC_RXD2",      0x00, 0x00, INIT_NONE,},
	[REG_AFCRXD3]     = {"AFC_RXD3",      0x00, 0x00, INIT_NONE,},
	[REG_AFCRXD4]     = {"AFC_RXD4",      0x00, 0x00, INIT_NONE,},
	[REG_AFCRXD5]     = {"AFC_RXD5",      0x00, 0x00, INIT_NONE,},
	[REG_AFCRXD6]     = {"AFC_RXD6",      0x00, 0x00, INIT_NONE,},
	[REG_AFCTASTATUS] = {"AFC_TA_STATUS", 0x00, 0x00, INIT_NONE,},
	[REG_RESET]       = {"RESET",         0x00, 0x00, INIT_NONE,},
	[REG_INT_OPTION]  = {"INT_OPTION",    0x00, 0x00, REG_INT_OPTION_VALUE,},
	[REG_END]         = {NULL, 0, 0, INIT_NONE},
};

static int sm5720_muic_ioctl(struct regmap_desc *pdesc,
		int arg1, int *arg2, int *arg3)
{
	int ret = 0;

	switch (arg1) {
		case GET_COM_VAL:
			*arg2 = sm5720_com_value_tbl[*arg2];
			*arg3 = REG_MANSW1;
			break;
		case GET_CTLREG:
			*arg3 = REG_CTRL;
			break;

		case GET_ADC:
			*arg3 = ADC_ADC_VALUE;
			break;

		case GET_SWITCHING_MODE:
			*arg3 = CTRL_ManualSW;
			break;

		case GET_INT_MASK:
			*arg3 = CTRL_MASK_INT;
			break;

		case GET_REVISION:
			*arg3 = DEVID_VendorID;
			break;

		case GET_OTG_STATUS:
			*arg3 = INTMASK2_VBUS_OFF_M;
			break;

		case GET_CHGTYPE:
			*arg3 = CHGTYPE_CHG_TYPE;
			break;

		case GET_RESID3:
			*arg3 = RSVDID2_BCD_RESCAN;
			break;

		default:
			ret = -1;
			break;
	}

	if (pdesc->trace) {
		pr_info("%s: ret:%d arg1:%x,", __func__, ret, arg1);

		if (arg2)
			pr_info(" arg2:%x,", *arg2);

		if (arg3)
			pr_info(" arg3:%x - %s", *arg3,
					regmap_to_name(pdesc, _ATTR_ADDR(*arg3)));
		pr_info("\n");
	}

	return ret;
}

static int sm5720_run_chgdet(struct regmap_desc *pdesc, bool started)
{
	int attr, value, ret;

	pr_info("%s: start. %s\n", __func__, started ? "enabled": "disabled");

	attr = MANSW1_DM_CON_SW;
	value = 0;
	ret = regmap_write_value(pdesc, attr, value);
	if (ret < 0)
		pr_err("%s Reset reg write fail.\n", __func__);
	else
		_REGMAP_TRACE(pdesc, 'w', ret, attr, value);

	pr_info("%s: reset [DP]\n", __func__);
	attr = MANSW1_DP_CON_SW;
	value = 0;
	ret = regmap_write_value(pdesc, attr, value);
	if (ret < 0)
		pr_err("%s Reset reg write fail.\n", __func__);
	else
		_REGMAP_TRACE(pdesc, 'w', ret, attr, value);

	pr_info("%s: reset [MannualSW]\n", __func__);
	attr = CTRL_ManualSW;
	value = 0;
	ret = regmap_write_value(pdesc, attr, value);
	if (ret < 0)
		pr_err("%s Reset reg write fail.\n", __func__);
	else
		_REGMAP_TRACE(pdesc, 'w', ret, attr, value);

	pr_info("%s: reset [RESET]\n", __func__);
	attr = RESET_RESET;
	value = started ? 1 : 0;
	ret = regmap_write_value_ex(pdesc, attr, value);
	if (ret < 0)
		pr_err("%s Reset reg write fail.\n", __func__);
	else
		_REGMAP_TRACE(pdesc, 'w', ret, attr, value);

	pr_info("%s: init all register\n", __func__);
	muic_reg_init(pdesc->muic);

	pr_info("%s: enable INT\n", __func__);
	set_int_mask(pdesc->muic, 0);

	return ret;
}

static int sm5720_attach_ta(struct regmap_desc *pdesc)
{
#if defined(CONFIG_SUPPORT_QC30)
	int attr = 0, value = 0, ret = 0;

	attr = MANSW1_DM_CON_USB_CP;
	value = 1;
	ret = regmap_write_value(pdesc, attr, value);
	if (ret < 0)
		pr_err("%s MANSW1_DM_CON_USB_CP reg write fail.\n", __func__);
	else
		_REGMAP_TRACE(pdesc, 'w', ret, attr, value);

	attr = MANSW1_DP_CON_USB_CP;
	value = 1;
	ret = regmap_write_value(pdesc, attr, value);
	if (ret < 0)
		pr_err("%s MANSW1_DP_CON_USB_CP reg write fail.\n", __func__);
	else
		_REGMAP_TRACE(pdesc, 'w', ret, attr, value);

	sm5720_set_switching_mode(pdesc, SWMODE_MANUAL);
#endif
	pr_info("%s\n", __func__);

	return 0;
}

static int sm5720_detach_ta(struct regmap_desc *pdesc)
{
	pr_info("%s\n", __func__);
	return 0;
}

static int sm5720_set_rustproof(struct regmap_desc *pdesc, int op)
{
	int attr = 0, value = 0, ret = 0;

	pr_info("%s\n", __func__);

	do {
		attr = MANSW2_JIG_ON;
		value = op ? 1 : 0;
		ret = regmap_write_value(pdesc, attr, value);
		if (ret < 0) {
			pr_err("%s MANSW2_JIG_ON write fail.\n", __func__);
			break;
		}

		_REGMAP_TRACE(pdesc, 'w', ret, attr, value);

		attr = CTRL_ManualSW;
		value = op ? 0 : 1;
		ret = regmap_write_value(pdesc, attr, value);
		if (ret < 0) {
			pr_err("%s CTRL_ManualSW write fail.\n", __func__);
			break;
		}

		_REGMAP_TRACE(pdesc, 'w', ret, attr, value);

	} while (0);

	return ret;
}

static int sm5720_get_vps_data(struct regmap_desc *pdesc, void *pbuf)
{
	muic_data_t *pmuic = pdesc->muic;
	vps_data_t *pvps = (vps_data_t *)pbuf;
	int attr;

	if (pdesc->trace)
		pr_info("%s\n", __func__);

	*(u8 *)&pvps->s.val1 = muic_i2c_read_byte(pmuic->i2c, REG_DEVT1);
	*(u8 *)&pvps->s.val2 = muic_i2c_read_byte(pmuic->i2c, REG_DEVT2);
	*(u8 *)&pvps->s.val3 = muic_i2c_read_byte(pmuic->i2c, REG_DEVT3);

	*(u8 *)&pvps->s.chgtyp = muic_i2c_read_byte(pmuic->i2c, REG_CHGTYPE);

	attr = RSVDID1_VBUS_VALID;
	*(u8 *)&pvps->s.vbvolt = regmap_read_value(pdesc, attr);

	attr = ADC_ADC_VALUE;
	*(u8 *)&pvps->s.adc = regmap_read_value(pdesc, attr);

	return 0;
}

static int sm5720_get_adc_scan_mode(struct regmap_desc *pdesc)
{
	struct reg_value_set *pvset;
	int attr, value, mode = 0;

	attr = MANSW2_SINGLE_MODE;
	value = regmap_read_value(pdesc, attr);

	for ( ; mode <ARRAY_SIZE(sm5720_adc_scanmode_tbl); mode++) {
		pvset = &sm5720_adc_scanmode_tbl[mode];
		if (pvset->value == value)
			break;
	}

	pr_info("%s: [%2d]=%02x,%02x\n", __func__, mode, value, pvset->value);
	pr_info("%s:       %s\n", __func__, pvset->alias);

	return mode;
}

static void sm5720_set_adc_scan_mode(struct regmap_desc *pdesc,
		const int mode)
{
	struct reg_value_set *pvset;
	int attr, ret, value;

	if (mode > ADC_SCANMODE_PULSE) {
		pr_err("%s Out of range(%d).\n", __func__, mode);
		return;
	}

	pvset = &sm5720_adc_scanmode_tbl[mode];
	pr_info("%s: [%2d] %s\n", __func__, mode, pvset->alias);

	do {
		value = pvset->value;
		attr = MANSW2_SINGLE_MODE;
		ret = regmap_write_value(pdesc, attr, value);
		if (ret < 0) {
			pr_err("%s MANSW2_SINGLE_MODE write fail.\n", __func__);
			break;
		}

		_REGMAP_TRACE(pdesc, 'w', ret, attr, value);

#define _ENABLE_PERIODIC_SCAN (0)
#define _DISABLE_PERIODIC_SCAN (1)

		attr = CTRL_RAWDATA;
		if (mode == ADC_SCANMODE_CONTINUOUS)
			value = _ENABLE_PERIODIC_SCAN;
		else
			value = _DISABLE_PERIODIC_SCAN;

		ret = regmap_write_value(pdesc, attr, value);
		if (ret < 0) {
			pr_err("%s CTRL_RAWDATA write fail.\n", __func__);
			break;
		}

		_REGMAP_TRACE(pdesc, 'w', ret, attr, value);

	} while (0);
}

enum switching_mode_val{
	_SWMODE_AUTO =1,
	_SWMODE_MANUAL =0,
};

static int sm5720_get_switching_mode(struct regmap_desc *pdesc)
{
	int attr, value, mode;

	pr_info("%s\n",__func__);

	attr = CTRL_ManualSW;
	value = regmap_read_value(pdesc, attr);

	mode = (value == _SWMODE_AUTO) ? SWMODE_AUTO : SWMODE_MANUAL;

	return mode;
}

static void sm5720_set_switching_mode(struct regmap_desc *pdesc, int mode)
{
	int attr, value;
	int ret = 0;

	pr_info("%s\n",__func__);

	value = (mode == SWMODE_AUTO) ? _SWMODE_AUTO : _SWMODE_MANUAL;
	attr = CTRL_ManualSW;
	ret = regmap_write_value(pdesc, attr, value);
	if (ret < 0)
		pr_err("%s REG_CTRL write fail.\n", __func__);
	else
		_REGMAP_TRACE(pdesc, 'w', ret, attr, value);
}

static int sm5720_enable_chgdet(struct regmap_desc *pdesc, int off)
{
	int ret, attr;

	off = (off == 1) ? 0 : 1;

	attr = RSVDID2_BC12OFF;
	ret = regmap_write_value(pdesc, attr, off);
	if (ret < 0)
		pr_err("%s RSVDID2 BC12OFF reg write fail.\n", __func__);
	else
		_REGMAP_TRACE(pdesc, 'w', ret, attr, off);

	return 0;
}

static void sm5720_set_manual_usb_path(struct regmap_desc *pdesc)
{
	int ret, value, attr;

	attr = MANSW1_DM_CON_SW;
	value = 1;
	ret = regmap_write_value(pdesc, attr, value);
	if (ret < 0)
		pr_err("%s MANSW1 DM_CON_SW reg write fail.\n", __func__);
	else
		_REGMAP_TRACE(pdesc, 'w', ret, attr, value);

	attr = MANSW1_DP_CON_SW;
	value = 1;
	ret = regmap_write_value(pdesc, attr, value);
	if (ret < 0)
		pr_err("%s MANSW1 DP_CON_SW reg write fail.\n", __func__);
	else
		_REGMAP_TRACE(pdesc, 'w', ret, attr, value);

	sm5720_set_switching_mode(pdesc, SWMODE_MANUAL);
}

static void sm5720_set_jig_on(struct regmap_desc *pdesc, int state)
{
	int attr, value;
	int ret = 0;

	pr_info("%s\n",__func__);

	attr = GEN_CNTL;
	value = 1;
	ret = regmap_write_value(pdesc, attr, value);
	if (ret < 0)
		pr_err("%s GEN_CNTL write fail.\n", __func__);
	else
		_REGMAP_TRACE(pdesc, 'w', ret, attr, value);

	attr = MANSW2_JIG_ON;
	ret = regmap_write_value(pdesc, attr, state);
	if (ret < 0)
		pr_err("%s REG_CTRL write fail.\n", __func__);
	else
		_REGMAP_TRACE(pdesc, 'w', ret, attr, state);
}

#define DCD_OUT_SDP	(1 << 2)

static bool sm5720_get_dcdtmr_irq(struct regmap_desc *pdesc)
{
	muic_data_t *muic = pdesc->muic;
	int ret;

	ret = muic_i2c_read_byte(muic->i2c, REG_DEVT3);
	if (ret < 0) {
		pr_err("%s: failed to read REG_DEVT3\n", __func__);
		return false;
	}

	pr_info("%s: REG_DEVT3: 0x%02x\n", __func__, ret);

	if (ret & DCD_OUT_SDP)
		return true;

	return false;
}

static void sm5720_get_fromatted_dump(struct regmap_desc *pdesc, char *mesg)
{
	muic_data_t *muic = pdesc->muic;
	int val;

	if (pdesc->trace)
		pr_info("%s\n", __func__);

	val = i2c_smbus_read_byte_data(muic->i2c, REG_CTRL);
	sprintf(mesg, "CT:%x ", val);
	val = i2c_smbus_read_byte_data(muic->i2c, REG_INTMASK1);
	sprintf(mesg+strlen(mesg), "IM1:%x ", val);
	val = i2c_smbus_read_byte_data(muic->i2c, REG_INTMASK2);
	sprintf(mesg+strlen(mesg), "IM2:%x ", val);
	val = i2c_smbus_read_byte_data(muic->i2c, REG_INTMASK3);
	sprintf(mesg+strlen(mesg), "IM3:%x ", val);
	val = i2c_smbus_read_byte_data(muic->i2c, REG_MANSW1);
	sprintf(mesg+strlen(mesg), "MS1:%x ", val);
	val = i2c_smbus_read_byte_data(muic->i2c, REG_MANSW2);
	sprintf(mesg+strlen(mesg), "MS2:%x ", val);
	val = i2c_smbus_read_byte_data(muic->i2c, REG_ADC);
	sprintf(mesg+strlen(mesg), "ADC:%x ", val);
	val = i2c_smbus_read_byte_data(muic->i2c, REG_DEVT1);
	sprintf(mesg+strlen(mesg), "DT1:%x ", val);
	val = i2c_smbus_read_byte_data(muic->i2c, REG_DEVT2);
	sprintf(mesg+strlen(mesg), "DT2:%x ", val);
	val = i2c_smbus_read_byte_data(muic->i2c, REG_DEVT3);
	sprintf(mesg+strlen(mesg), "DT3:%x ", val);
	val = i2c_smbus_read_byte_data(muic->i2c, REG_RSVDID1);
	sprintf(mesg+strlen(mesg), "RS1:%x ", val);
	val = i2c_smbus_read_byte_data(muic->i2c, REG_RSVDID2);
	sprintf(mesg+strlen(mesg), "RS2:%x ", val);
	val = i2c_smbus_read_byte_data(muic->i2c, REG_CHGTYPE);
	sprintf(mesg+strlen(mesg), "CTY:%x ", val);
	val = i2c_smbus_read_byte_data(muic->i2c, REG_AFCCNTL);
	sprintf(mesg+strlen(mesg), "AFC:%x ", val);
	val = i2c_smbus_read_byte_data(muic->i2c, REG_AFCTXD);
	sprintf(mesg+strlen(mesg), "TXD:%x ", val);
	val = i2c_smbus_read_byte_data(muic->i2c, REG_AFCSTAT);
	sprintf(mesg+strlen(mesg), "AST:%x ", val);
	val = i2c_smbus_read_byte_data(muic->i2c, REG_VBUSVOL1);
	sprintf(mesg+strlen(mesg), "VO1:%x ", val);
	val = i2c_smbus_read_byte_data(muic->i2c, REG_VBUSVOL2);
	sprintf(mesg+strlen(mesg), "VO2:%x ", val);
	val = i2c_smbus_read_byte_data(muic->i2c, REG_AFCTASTATUS);
	sprintf(mesg+strlen(mesg), "ATT:%x ", val);
	val = i2c_smbus_read_byte_data(muic->i2c, 0x3C);
	sprintf(mesg+strlen(mesg), "H3C:%x ", val);
	val = i2c_smbus_read_byte_data(muic->i2c, 0x3D);
	sprintf(mesg+strlen(mesg), "H3D:%x ", val);
}

static int sm5720_get_sizeof_regmap(void)
{
	pr_info("%s:%s\n", MUIC_DEV_NAME, __func__);
	return (int)ARRAY_SIZE(sm5720_muic_regmap_table);
}

static int sm5720_BCD_rescan(struct regmap_desc *pdesc)
{
	muic_data_t *muic = pdesc->muic;
	int value = 0, chgtype = 0, cnt = 0;

	pr_info("%s:%s\n", MUIC_DEV_NAME, __func__);
	value = muic_i2c_read_byte(muic->i2c, REG_RSVDID2);

	value |= 0x10;
	muic_i2c_write_byte(muic->i2c, REG_RSVDID2, value);

	value &= 0xEF;
	muic_i2c_write_byte(muic->i2c, REG_RSVDID2, value);

	for (cnt = 0; cnt < 15; cnt++) {
		msleep(100);
		chgtype = muic_i2c_read_byte(muic->i2c, REG_CHGTYPE);
		if (chgtype != 0) {
			pr_info("%s:%s count[%d]\n", MUIC_DEV_NAME, __func__, cnt);
			break;
		}
	}

	return chgtype;
}

#if defined(CONFIG_SEC_DEBUG) || defined(CONFIG_SEC_NAD)
static int sm5720_usb_to_ta(struct regmap_desc *pdesc, int mode)
{
	int ret = -1;
	muic_data_t *pmuic = pdesc->muic;
	vps_data_t *pmsr = &pmuic->vps;

	pr_info("%s\n",__func__);

	if (mode == 0) {
		pr_info("%s, Disable USB to TA\n",__func__);
		if (pmuic->attached_dev == ATTACHED_DEV_TA_MUIC && pmuic->usb_to_ta_state) {
			switch (pmsr->s.chgtyp) {
			case 2:
				pmuic->attached_dev = ATTACHED_DEV_CDP_MUIC;
				break;
			case 4:
				pmuic->attached_dev = ATTACHED_DEV_USB_MUIC;
				break;
			default:
				pmuic->attached_dev = ATTACHED_DEV_USB_MUIC;
				break;
			}
			muic_notifier_detach_attached_dev(ATTACHED_DEV_TA_MUIC);
			muic_notifier_attach_attached_dev(pmuic->attached_dev);
			pmuic->usb_to_ta_state = false;
		}
	} else if (mode == 1) {
		pr_info("%s, Enable USB to TA attached_dev %d\n",
				__func__, pdesc->muic->attached_dev);
		if ((pdesc->muic->attached_dev == ATTACHED_DEV_CDP_MUIC ||
				pdesc->muic->attached_dev == ATTACHED_DEV_USB_MUIC)
				&& !pmuic->usb_to_ta_state) {
			muic_notifier_detach_attached_dev(pdesc->muic->attached_dev);
			muic_notifier_attach_attached_dev(ATTACHED_DEV_TA_MUIC);
			pmuic->attached_dev = ATTACHED_DEV_TA_MUIC;
			pmuic->usb_to_ta_state = true;
		}
	} else if (mode == 2) {
		pr_info("%s, USB to TA %s\n",__func__,
				pmuic->usb_to_ta_state?"Enabled":"Disabled");
		ret = pmuic->usb_to_ta_state;
	} else {
		pr_info("%s, Unknown CMD\n",__func__);
	}
	return ret;
}
#endif

static int sm5720_get_vbus_value(struct regmap_desc *pdesc, int type)
{
	muic_data_t *muic = pdesc->muic;
	u8 val = 0;
	int ret, attr, value, result = 0;

	/* type 0 : afc charger , type 1 : PD charger */
	pr_info("%s, type %d\n", __func__, type);

	if(type == 1) {
		/* write '1' to INT3_VBUS_UPDATE_M: masking */
		attr = INT3_VBUS_UPDATE_M;
		value = 1;
		ret = regmap_write_value(pdesc, attr, value);
		if (ret < 0) {
			pr_err("%s: failed to set VBUS_UPDATE_M to 1\n", __func__);
			return -EINVAL;
		}


		/* write '1' -> '0' to VBUS_READ of AFC_CNTL */
		attr = AFCCNTL_VBUS_READ;
		value = 1;
		ret = regmap_write_value(pdesc, attr, value);
		if (ret < 0) {
			pr_err("%s: failed to set VBUS_READ to 1\n", __func__);
			return -EINVAL;
		}

		msleep(20);

		value = 0;
		ret = regmap_write_value(pdesc, attr, value);
		if (ret < 0) {
			pr_err("%s: failed to set VBUS_READ to 0\n", __func__);
			return -EINVAL;
		}

		mdelay(100);

		/* check VBUS_UPDATE */
		attr = INT3_VBUS_UPDATE;
		ret = regmap_read_value(pdesc, attr);
		if (ret == 0) {
			pr_err("%s: INT3_VBUS_UPDATE is not updated\n", __func__);
			return -EINVAL;
		} else {
			int voltage1, voltage2;

			voltage1 = muic_i2c_read_byte(muic->i2c, REG_VBUSVOL1);
			voltage2 = muic_i2c_read_byte(muic->i2c, REG_VBUSVOL2);
			val = (voltage1*1000 + (voltage2*3900)/1000)/1000;
			pr_info("%s: vbus:%d\n", __func__, val);
		}

		/* write '0' to INT3_VBUS_UPDATE_M: unmasking */
		attr = INT3_VBUS_UPDATE_M;
		value = 0;
		ret = regmap_write_value(pdesc, attr, value);
		if (ret < 0) {
			pr_err("%s: failed to set VBUS_UPDATE_M to 0\n", __func__);
			return -EINVAL;
		}
	} else {
		int voltage1, voltage2;

		voltage1 = muic_i2c_read_byte(muic->i2c, REG_VBUSVOL1);
		voltage2 = muic_i2c_read_byte(muic->i2c, REG_VBUSVOL2);
		val = (voltage1*1000 + (voltage2*3900)/1000)/1000;
		pr_info("%s: vbus:%d\n", __func__, val);
	}

	if(muic->vps.s.vbvolt == 1) {
		switch (val) {
		case (0)...(5):
			result = 5;
			break;
		case 8:
		case 9:
			result = 9;
			break;
		case (10)...(12):
			result = 12;
			break;
		default:
			break;
		}
	}

	return result;
}

static struct regmap_ops sm5720_muic_regmap_ops = {
	.get_size = sm5720_get_sizeof_regmap,
	.ioctl = sm5720_muic_ioctl,
	.get_formatted_dump = sm5720_get_fromatted_dump,
};

static struct vendor_ops sm5720_muic_vendor_ops = {
	.attach_ta = sm5720_attach_ta,
	.detach_ta = sm5720_detach_ta,
	.bcd_rescan = sm5720_BCD_rescan,
	.get_switch = sm5720_get_switching_mode,
	.set_switch = sm5720_set_switching_mode,
	.enable_chgdet = sm5720_enable_chgdet,
	.set_manual_usb_path = sm5720_set_manual_usb_path,
	.set_adc_scan_mode = sm5720_set_adc_scan_mode,
	.get_adc_scan_mode = sm5720_get_adc_scan_mode,
	.set_jig_on = sm5720_set_jig_on,
	.set_rustproof = sm5720_set_rustproof,
	.get_vps_data = sm5720_get_vps_data,
	.get_vbus_value = sm5720_get_vbus_value,
	.get_dcdtmr_irq = sm5720_get_dcdtmr_irq,
	.run_chgdet = sm5720_run_chgdet,
#if defined(CONFIG_SEC_DEBUG) || defined(CONFIG_SEC_NAD)
	.usb_to_ta = sm5720_usb_to_ta,
#endif
};

#if defined(CONFIG_MUIC_HV)
static struct afc_ops sm5720_muic_afc_ops = {
	.afc_ta_attach = sm5720_afc_ta_attach,
	.afc_ta_accept = sm5720_afc_ta_accept,
	.afc_vbus_update = sm5720_afc_vbus_update,
	.afc_multi_byte = sm5720_afc_multi_byte,
	.afc_error = sm5720_afc_error,
	.afc_ctrl_reg = sm5720_set_afc_ctrl_reg,
	.afc_init_check = sm5720_afc_init_check,
};
#endif

static struct regmap_desc sm5720_muic_regmap_desc = {
	.name = "sm5720-MUIC",
	.regmap = sm5720_muic_regmap_table,
	.size = REG_END,
	.regmapops = &sm5720_muic_regmap_ops,
	.vendorops = &sm5720_muic_vendor_ops,
#if defined(CONFIG_MUIC_HV)
	.afcops = &sm5720_muic_afc_ops,
#endif
};

void muic_register_sm5720_regmap_desc(struct regmap_desc **pdesc)
{
	*pdesc = &sm5720_muic_regmap_desc;
}

void sm5720_muic_check_reset(struct i2c_client *i2c, void *muic_data )
{
	muic_data_t *pmuic = muic_data;
	int value;

	pr_info("%s:%s \n",MUIC_DEV_NAME, __func__);

	value = muic_i2c_read_byte(pmuic->i2c, REG_INTMASK1);
	if ( value == 0x00 ) { // muic reset ...
		muic_print_reg_log();
		muic_print_reg_dump(pmuic);
		pr_err("%s: err muic could have been reseted. Initilize!!\n",
				__func__);
		muic_reg_init(pmuic);
		muic_print_reg_dump(pmuic);

		/* MUIC Interrupt On */
		set_int_mask(pmuic, false);
	}

	return;
}

