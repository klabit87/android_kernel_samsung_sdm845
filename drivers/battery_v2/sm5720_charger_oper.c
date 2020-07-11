/*
 *  sm5720_charger_oper.c
 *  Samsung SM5720 Charger Operation Mode control module
 *
 *  Copyright (C) 2016 Samsung Electronics
 *
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#include <linux/debugfs.h>
#include <linux/power_supply.h>
#include <linux/wakelock.h>
#include "include/charger/sm5720_charger.h"
#include "include/sec_charging_common.h"

enum {
    BSTOUT_5000mV       = 0x0,
    BSTOUT_5100mV       = 0x1,
    BSTOUT_5200mV       = 0x2,
    BSTOUT_5500mV       = 0x3,
    BSTOUT_6000mV       = 0x4,
    BSTOUT_6500mV       = 0x5,
    BSTOUT_7000mV       = 0x6,
    BSTOUT_7500mV       = 0x7,
    BSTOUT_8000mV       = 0x8,
    BSTOUT_8500mV       = 0x9,
    BSTOUT_9000mV       = 0xA,
    BSTOUT_9100mV       = 0xB,
};

enum {
	OTG_CURRENT_500mA   = 0x0,
	OTG_CURRENT_900mA   = 0x1,
	OTG_CURRENT_1200mA  = 0x2,
	OTG_CURRENT_1500mA  = 0x3,
};

enum {
    TX_CURRENT_100mA    = 0x0,
    TX_CURRENT_200mA    = 0x1,
    TX_CURRENT_300mA    = 0x2,
    TX_CURRENT_400mA    = 0x3,
    TX_CURRENT_500mA    = 0x4,
    TX_CURRENT_600mA    = 0x5,
    TX_CURRENT_700mA    = 0x6,
    TX_CURRENT_800mA    = 0x7,
};

#define SM5720_OPERATION_MODE_MASK  0x07
#define SM5720_BSTOUT_MASK          0x0F
#define SM5720_OTGCURRENT_MASK      0xC0
#define SM5720_TXCURRENT_MASK       0x07

#define make_OP_STATUS(vbus, wpc, otg, pwr_shar, tx_5v, tx_9v, suspend) (((vbus & 0x1)      << SM5720_CHARGER_OP_EVENT_VBUSIN)      | \
                                                                         ((wpc & 0x1)       << SM5720_CHARGER_OP_EVENT_WPCIN)       | \
                                                                         ((otg & 0x1)       << SM5720_CHARGER_OP_EVENT_USB_OTG)     | \
                                                                         ((pwr_shar & 0x1)  << SM5720_CHARGER_OP_EVENT_PWR_SHAR)    | \
                                                                         ((tx_5v & 0x1)     << SM5720_CHARGER_OP_EVENT_5V_TX)       | \
                                                                         ((tx_9v & 0x1)     << SM5720_CHARGER_OP_EVENT_9V_TX)       | \
                                                                         ((suspend & 0x1)   << SM5720_CHARGER_OP_EVENT_SUSPEND))

struct sm5720_charger_oper_table_info {
	unsigned short status;
	unsigned char oper_mode;
	unsigned char BST_OUT;
	unsigned char OTG_CURRENT;
    unsigned char TX_CURRENT;
};

struct sm5720_charger_oper_info {
	struct i2c_client *i2c;

	int max_table_num;
	struct sm5720_charger_oper_table_info current_table;
};
static struct sm5720_charger_oper_info oper_info;

/**
 *  (VBUS in/out) (WPC in/out) (USB-OTG in/out) (Power Sharing
 *  cable in/out) (5V TX-mode on/off) (9V TX-mode on/off)
 *  (Suspend mode on/off)
 **/
static struct sm5720_charger_oper_table_info sm5720_charger_operation_mode_table[] = {
	/* Charger=ON Mode in a valid Input */
	{ make_OP_STATUS(0, 0, 0, 0, 0, 0, 0), SM5720_CHARGER_OP_MODE_CHG_ON_VBUS,        BSTOUT_5200mV, OTG_CURRENT_500mA, TX_CURRENT_500mA},
    { make_OP_STATUS(1, 0, 0, 0, 0, 0, 0), SM5720_CHARGER_OP_MODE_CHG_ON_VBUS,        BSTOUT_5200mV, OTG_CURRENT_500mA, TX_CURRENT_500mA},
    { make_OP_STATUS(1, 0, 1, 0, 0, 0, 0), SM5720_CHARGER_OP_MODE_CHG_ON_VBUS,        BSTOUT_5200mV, OTG_CURRENT_1500mA, TX_CURRENT_500mA},
    /* Charger=ON Mode over WPCIN */
    { make_OP_STATUS(0, 1, 0, 0, 0, 0, 0), SM5720_CHARGER_OP_MODE_CHG_ON_WPCIN,       BSTOUT_5200mV, OTG_CURRENT_500mA, TX_CURRENT_500mA},
    /* TX Mode in a valid VBUS */
    { make_OP_STATUS(1, 0, 0, 0, 1, 0, 0), SM5720_CHARGER_OP_MODE_TX_MODE_VBUS,       BSTOUT_5200mV, OTG_CURRENT_500mA, TX_CURRENT_500mA},
    { make_OP_STATUS(1, 0, 0, 0, 0, 1, 0), SM5720_CHARGER_OP_MODE_TX_MODE_VBUS,       BSTOUT_9000mV, OTG_CURRENT_500mA, TX_CURRENT_500mA},
    /* Wireless OTG Mode and Charger=ON */
    { make_OP_STATUS(0, 1, 1, 0, 0, 0, 0), SM5720_CHARGER_OP_MODE_WPC_OTG_CHG_ON,     BSTOUT_5200mV, OTG_CURRENT_1500mA, TX_CURRENT_500mA},
    { make_OP_STATUS(0, 1, 0, 1, 0, 0, 0), SM5720_CHARGER_OP_MODE_WPC_OTG_CHG_ON,     BSTOUT_5200mV, OTG_CURRENT_500mA, TX_CURRENT_500mA},
    /* USB OTG Mode */
    { make_OP_STATUS(0, 0, 1, 0, 0, 0, 0), SM5720_CHARGER_OP_MODE_USB_OTG,            BSTOUT_5200mV, OTG_CURRENT_1500mA, TX_CURRENT_500mA},
    { make_OP_STATUS(0, 0, 0, 1, 0, 0, 0), SM5720_CHARGER_OP_MODE_USB_OTG,            BSTOUT_5200mV, OTG_CURRENT_500mA, TX_CURRENT_500mA},
    /* USB OTG and TX Mode */
    { make_OP_STATUS(0, 0, 1, 0, 1, 0, 0), SM5720_CHARGER_OP_MODE_USB_OTG_TX_MODE,    BSTOUT_5200mV, OTG_CURRENT_1500mA, TX_CURRENT_500mA},
    { make_OP_STATUS(0, 0, 0, 1, 1, 0, 0), SM5720_CHARGER_OP_MODE_USB_OTG_TX_MODE,    BSTOUT_5200mV, OTG_CURRENT_500mA, TX_CURRENT_500mA},
    /* TX Mode in NO valid VBUS */
    { make_OP_STATUS(0, 0, 0, 0, 1, 0, 0), SM5720_CHARGER_OP_MODE_TX_MODE_NOVBUS,     BSTOUT_5200mV, OTG_CURRENT_500mA, TX_CURRENT_500mA},
    { make_OP_STATUS(0, 0, 0, 0, 0, 1, 0), SM5720_CHARGER_OP_MODE_TX_MODE_NOVBUS,     BSTOUT_9000mV, OTG_CURRENT_500mA, TX_CURRENT_500mA},
    /* Suspend Mode */
    { make_OP_STATUS(0, 0, 0, 0, 0, 0, 1), SM5720_CHARGER_OP_MODE_SUSPEND,            BSTOUT_5200mV, OTG_CURRENT_500mA, TX_CURRENT_500mA}, /* Reserved */
};



/**
 * SM5720 Charger operation mode controller relative I2C setup
 */

static int sm5720_charger_oper_set_mode(struct i2c_client *i2c, unsigned char mode)
{
	return sm5720_update_reg(i2c, SM5720_CHG_REG_CNTL2, mode, SM5720_OPERATION_MODE_MASK);
}

static int sm5720_charger_oper_set_BSTOUT(struct i2c_client *i2c, unsigned char BSTOUT)
{
	return sm5720_update_reg(i2c, SM5720_CHG_REG_BSTCNTL1, BSTOUT, SM5720_BSTOUT_MASK);
}

static int sm5720_charger_oper_set_OTG_CURRENT(struct i2c_client *i2c, unsigned char OTG_CURRENT)
{
	return sm5720_update_reg(i2c, SM5720_CHG_REG_BSTCNTL1, OTG_CURRENT << 6, SM5720_OTGCURRENT_MASK);
}

static int sm5720_charger_oper_set_TX_CURRENT(struct i2c_client *i2c, unsigned char TX_CURRENT)
{
	return sm5720_update_reg(i2c, SM5720_CHG_REG_BSTCNTL2, TX_CURRENT, SM5720_TXCURRENT_MASK);
}

/**
 * SM5720 Charger operation mode controller API functions.
 */

static inline int sm5720_charger_oper_change_state(unsigned char new_status)
{
	int i = 0;

    /* Check actvated Suspend Mode */
    if (new_status & (0x1 << SM5720_CHARGER_OP_EVENT_SUSPEND)) {
        i = oper_info.max_table_num - 1;    /* Reserved SUSPEND Mode Table index */
    } else {
        /* Search matched Table */
    	for (i=0; i < oper_info.max_table_num; ++i) {
    		if (new_status == sm5720_charger_operation_mode_table[i].status) {
    			break;
    		}
    	}
    }
    if (i == oper_info.max_table_num) {
		pr_err("sm5720-charger: %s: can't find matched Charger Operation Mode Table (status = 0x%x)\n", __func__, new_status);
		return -EINVAL;
	}

    /* Update current table info */
	if (sm5720_charger_operation_mode_table[i].BST_OUT != oper_info.current_table.BST_OUT) {
		sm5720_charger_oper_set_BSTOUT(oper_info.i2c, sm5720_charger_operation_mode_table[i].BST_OUT);
		oper_info.current_table.BST_OUT = sm5720_charger_operation_mode_table[i].BST_OUT;
	}
	if (sm5720_charger_operation_mode_table[i].OTG_CURRENT != oper_info.current_table.OTG_CURRENT) {
		sm5720_charger_oper_set_OTG_CURRENT(oper_info.i2c, sm5720_charger_operation_mode_table[i].OTG_CURRENT);
		oper_info.current_table.OTG_CURRENT = sm5720_charger_operation_mode_table[i].OTG_CURRENT;
	}
	if (sm5720_charger_operation_mode_table[i].TX_CURRENT != oper_info.current_table.TX_CURRENT) {
		sm5720_charger_oper_set_TX_CURRENT(oper_info.i2c, sm5720_charger_operation_mode_table[i].TX_CURRENT);
		oper_info.current_table.TX_CURRENT = sm5720_charger_operation_mode_table[i].TX_CURRENT;
	}
	if (sm5720_charger_operation_mode_table[i].oper_mode != oper_info.current_table.oper_mode) {
		/* for RESTART VL Regulator */
		if (sm5720_charger_operation_mode_table[i].oper_mode == SM5720_CHARGER_OP_MODE_CHG_ON_WPCIN && \
			oper_info.current_table.oper_mode == SM5720_CHARGER_OP_MODE_CHG_ON_VBUS) {
			sm5720_charger_oper_set_mode(oper_info.i2c, SM5720_CHARGER_OP_MODE_SUSPEND);
			msleep(3);
		}
		sm5720_charger_oper_set_mode(oper_info.i2c, sm5720_charger_operation_mode_table[i].oper_mode);
		oper_info.current_table.oper_mode = sm5720_charger_operation_mode_table[i].oper_mode;
	}
	oper_info.current_table.status = new_status;

	pr_info("sm5720-charger: %s: New table[%d] info (STATUS: 0x%x, MODE: %d, BST_OUT: 0x%x, OTG_CURRENT: 0x%x TX_CURRENT: 0x%x\n", \
			__func__, i, oper_info.current_table.status, oper_info.current_table.oper_mode, oper_info.current_table.BST_OUT, \
            oper_info.current_table.OTG_CURRENT, oper_info.current_table.TX_CURRENT);

    return 0;
}

static inline unsigned char _update_status(int event_type, bool enable)
{
	if (event_type > SM5720_CHARGER_OP_EVENT_VBUSIN) {
        pr_debug("sm5720-charger: %s: invalid event type (type=0x%x)\n", __func__, event_type);
		return oper_info.current_table.status;
	}

	if (enable) {
		return (oper_info.current_table.status | (1 << event_type));
	} else {
		return (oper_info.current_table.status & ~(1 << event_type));
	}
}

int sm5720_charger_oper_push_event(int event_type, bool enable)
{
	unsigned char new_status;
    int ret;

	if (oper_info.i2c == NULL) {
		pr_err("sm5720-charger: %s: required sm5720 charger operation table initialize\n", __func__);
		return -ENOENT;
	}

	pr_info("sm5720-charger: %s: event_type=%d, enable=%d\n", __func__, event_type, enable);

	new_status = _update_status(event_type, enable);
    if (new_status == oper_info.current_table.status) {
		goto out;
	}

    ret = sm5720_charger_oper_change_state(new_status);
    if (ret < 0) {
        return ret;
    }

out:
	return 0;
}
EXPORT_SYMBOL(sm5720_charger_oper_push_event);

static inline int _detect_initial_table_index(struct i2c_client *i2c)
{
    unsigned short status = 0;
    u8 i, op_mode, status1;

    sm5720_read_reg(i2c, SM5720_CHG_REG_CNTL2, &op_mode);
    if (op_mode == SM5720_CHARGER_OP_MODE_CHG_ON_WPCIN)
    {
        sm5720_read_reg(i2c, SM5720_CHG_REG_STATUS1, &status1);
        if ((status1 >> 4) & 0x1) /* WPCINPOK */
        {
            status += (1 << SM5720_CHARGER_OP_EVENT_WPCIN);
        }
    } else if (op_mode == SM5720_CHARGER_OP_MODE_SUSPEND)
    {
        status += (1 << SM5720_CHARGER_OP_EVENT_SUSPEND);
    }

    for (i = 0; i < oper_info.max_table_num; ++i) {
		if (status == sm5720_charger_operation_mode_table[i].status)
			break;
    }

    return i;
}

int sm5720_charger_oper_table_init(struct i2c_client *i2c)
{
    int index;

	if (i2c == NULL) {
		pr_err("sm5720-charger: %s: invalid i2c client handler=n", __func__);
		return -EINVAL;
	}
	oper_info.i2c = i2c;

	/* set default operation mode condition */
	oper_info.max_table_num = ARRAY_SIZE(sm5720_charger_operation_mode_table);
    index = _detect_initial_table_index(oper_info.i2c);
	oper_info.current_table.status = sm5720_charger_operation_mode_table[index].status;
	oper_info.current_table.oper_mode = sm5720_charger_operation_mode_table[index].oper_mode;
	oper_info.current_table.BST_OUT = sm5720_charger_operation_mode_table[index].BST_OUT;
	oper_info.current_table.OTG_CURRENT = sm5720_charger_operation_mode_table[index].OTG_CURRENT;
    oper_info.current_table.TX_CURRENT = sm5720_charger_operation_mode_table[index].TX_CURRENT;

	sm5720_charger_oper_set_mode(oper_info.i2c, oper_info.current_table.oper_mode);
	sm5720_charger_oper_set_BSTOUT(oper_info.i2c, oper_info.current_table.BST_OUT);
	sm5720_charger_oper_set_OTG_CURRENT(oper_info.i2c, oper_info.current_table.OTG_CURRENT);
    sm5720_charger_oper_set_TX_CURRENT(oper_info.i2c, oper_info.current_table.TX_CURRENT);

	pr_info("sm5720-charger: %s: current table info (STATUS: 0x%x, MODE: %d, BST_OUT: 0x%x, OTG_CURRENT: 0x%x TX_CURRENT: 0x%x\n", \
			__func__, oper_info.current_table.status, oper_info.current_table.oper_mode, oper_info.current_table.BST_OUT, \
            oper_info.current_table.OTG_CURRENT, oper_info.current_table.TX_CURRENT);

	return 0;
}
EXPORT_SYMBOL(sm5720_charger_oper_table_init);

int sm5720_charger_oper_get_current_status(void)
{
	return oper_info.current_table.status;
}
EXPORT_SYMBOL(sm5720_charger_oper_get_current_status);

int sm5720_charger_oper_get_current_op_mode(void)
{
	return oper_info.current_table.oper_mode;
}
EXPORT_SYMBOL(sm5720_charger_oper_get_current_op_mode);
