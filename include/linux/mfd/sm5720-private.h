/*
 *  sm5720-private.h
 *  Samsung IF-PMIC device header file for SM5720
 *
 *  Copyright (C) 2016 Samsung Electronics
 *
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#ifndef __SM5720_PRIV_H__
#define __SM5720_PRIV_H__

#include <linux/i2c.h>

#define SM5720_IRQSRC_MUIC	    (1 << 0)
#define SM5720_IRQSRC_CHG	    (1 << 1)
#define SM5720_IRQSRC_FG        (1 << 2)

/* Slave addr = 0x4A : MUIC */
enum sm5720_muic_reg {
    SM5720_MUIC_REG_DEVICE_ID       = 0x01,
    SM5720_MUIC_REG_CONTROL         = 0x02,
    SM5720_MUIC_REG_INT1            = 0x03,
    SM5720_MUIC_REG_INT2            = 0x04,
    SM5720_MUIC_REG_INT3_AFC        = 0x05,
    SM5720_MUIC_REG_INTMASK1        = 0x06,
    SM5720_MUIC_REG_INTMASK2        = 0x07,
    SM5720_MUIC_REG_INTMASK3_AFC    = 0x08,

    SM5720_MUIC_REG_END,
};

/* Slave addr = 0x92 : SW Charger, RGBW, Haptic */
enum sm5720_charger_reg {
    SM5720_CHG_REG_INT_SOURCE       = 0x00,
    SM5720_CHG_REG_INT1             = 0x01,
    SM5720_CHG_REG_INT2             = 0x02,
    SM5720_CHG_REG_INT3             = 0x03,
    SM5720_CHG_REG_INT4             = 0x04,
    SM5720_CHG_REG_INTMSK1          = 0x05,
    SM5720_CHG_REG_INTMSK2          = 0x06,
    SM5720_CHG_REG_INTMSK3          = 0x07,
    SM5720_CHG_REG_INTMSK4          = 0x08,
    SM5720_CHG_REG_STATUS1          = 0x09,
    SM5720_CHG_REG_STATUS2          = 0x0A,
    SM5720_CHG_REG_STATUS3          = 0x0B,
    SM5720_CHG_REG_STATUS4          = 0x0C,
    SM5720_CHG_REG_CNTL1            = 0x0D,
    SM5720_CHG_REG_CNTL2            = 0x0E,
    SM5720_CHG_REG_VBUSCNTL         = 0x0F,
    SM5720_CHG_REG_WPCINCNTL        = 0x10,
    SM5720_CHG_REG_CHGCNTL1         = 0x11,
    SM5720_CHG_REG_CHGCNTL2         = 0x12,
    SM5720_CHG_REG_CHGCNTL3         = 0x13,
    SM5720_CHG_REG_CHGCNTL4         = 0x14,
    SM5720_CHG_REG_CHGCNTL5         = 0x15,
    SM5720_CHG_REG_CHGCNTL6         = 0x16,
    SM5720_CHG_REG_CHGCNTL7         = 0x17,
    SM5720_CHG_REG_CHGCNTL8         = 0x18,
    SM5720_CHG_REG_WDTCNTL          = 0x19,
    SM5720_CHG_REG_BSTCNTL1         = 0x1A,
    SM5720_CHG_REG_BSTCNTL2         = 0x1B,
    SM5720_CHG_REG_SBPSCNTL         = 0x1C,
    SM5720_CHG_REG_RGBWCNTL1        = 0x1D,
    SM5720_CHG_REG_RGBWCNTL2        = 0x1E,
    SM5720_CHG_REG_RLEDCURRENT      = 0x1F,
    SM5720_CHG_REG_GLEDCURRENT      = 0x20,
    SM5720_CHG_REG_BLEDCURRENT      = 0x21,
    SM5720_CHG_REG_WLEDCURRENT      = 0x22,
    SM5720_CHG_REG_DIMSLPRLEDCNTL   = 0x23,
    SM5720_CHG_REG_DIMSLPGLEDCNTL   = 0x24,
    SM5720_CHG_REG_DIMSLPBLEDCNTL   = 0x25,
    SM5720_CHG_REG_DIMSLPWLEDCNTL   = 0x26,
    SM5720_CHG_REG_RLEDCNTL1        = 0x27,
    SM5720_CHG_REG_RLEDCNTL2        = 0x28,
    SM5720_CHG_REG_RLEDCNTL3        = 0x29,
    SM5720_CHG_REG_RLEDCNTL4        = 0x2A,
    SM5720_CHG_REG_GLEDCNTL1        = 0x2B,
    SM5720_CHG_REG_GLEDCNTL2        = 0x2C,
    SM5720_CHG_REG_GLEDCNTL3        = 0x2D,
    SM5720_CHG_REG_GLEDCNTL4        = 0x2E,
    SM5720_CHG_REG_BLEDCNTL1        = 0x2F,
    SM5720_CHG_REG_BLEDCNTL2        = 0x30,
    SM5720_CHG_REG_BLEDCNTL3        = 0x31,
    SM5720_CHG_REG_BLEDCNTL4        = 0x32,
    SM5720_CHG_REG_WLEDCNTL1        = 0x33,
    SM5720_CHG_REG_WLEDCNTL2        = 0x34,
    SM5720_CHG_REG_WLEDCNTL3        = 0x35,
    SM5720_CHG_REG_WLEDCNTL4        = 0x36,
    SM5720_CHG_REG_HAPTICCNTL       = 0x37,
    SM5720_CHG_REG_DEVICEID         = 0x38,
    SM5720_CHG_REG_WPCCNTL          = 0x39,
    SM5720_CHG_REG_WPCIN_VOLTAGE    = 0x3A,
    SM5720_CHG_REG_MODESTATUS	    = 0x3E,

    SM5720_CHG_REG_END,
};

/* Slave addr = 0xE2 : FUEL GAUGE */
enum sm5720_fuelgauge_reg {
    SM5720_FG_REG_DEVICE_ID         = 0x00,
    SM5720_FG_REG_CNTL              = 0x01,
    SM5720_FG_REG_INTFG             = 0x02,
    SM5720_FG_REG_INTFG_MASK        = 0x03,
    SM5720_FG_REG_STATUS            = 0x04,
    SM5720_FG_REG_SOC               = 0x05,
    SM5720_FG_REG_OCV               = 0x06,
    SM5720_FG_REG_VOLTAGE           = 0x07,
    SM5720_FG_REG_CURRENT           = 0x08,
    SM5720_FG_REG_TEMPERATURE       = 0x09,
    SM5720_FG_REG_CYCLE             = 0x0A,

    SM5720_FG_REG_V_ALARM           = 0x0C,
    SM5720_FG_REG_T_ALARM           = 0x0D,
    SM5720_FG_REG_SOC_ALARM         = 0x0E,
    SM5720_FG_REG_FG_OP_STATUS      = 0x10,
    SM5720_FG_REG_TOPOFFSOC         = 0x12,
    SM5720_FG_REG_PARAM_CTRL        = 0x13,
    SM5720_FG_REG_PARAM_RUN_UPDATE  = 0x14,
    SM5720_FG_REG_CYCLE_CFG         = 0x15,
    SM5720_FG_REG_VIT_PERIOD        = 0x1A,
    SM5720_FG_REG_MIX_RATE          = 0x1B,
    SM5720_FG_REG_MIX_INIT_BLANK    = 0x1C,
    SM5720_FG_REG_RESERVED          = 0x1F,

    SM5720_FG_REG_RCE0              = 0x20,
    SM5720_FG_REG_RCE1              = 0x21,
    SM5720_FG_REG_RCE2              = 0x22,
    SM5720_FG_REG_DTCD              = 0x23,
    SM5720_FG_REG_AUTO_RS_MAN       = 0x24,
    SM5720_FG_REG_RS_MIX_FACTOR     = 0x25,
    SM5720_FG_REG_RS_MAX            = 0x26,
    SM5720_FG_REG_RS_MIN            = 0x27,
    SM5720_FG_REG_RS_TUNE           = 0x28,
    SM5720_FG_REG_RS_MAN            = 0x29,

    SM5720_FG_REG_IOFF_MODE         = 0x2B,
    SM5720_FG_REG_IOCV_MAN          = 0x2E,
    SM5720_FG_REG_END_V_IDX         = 0x2F,

    SM5720_FG_REG_START_LB_V        = 0x30,
    SM5720_FG_REG_START_CB_V        = 0x36,
    SM5720_FG_REG_START_LB_I        = 0x40,
    SM5720_FG_REG_START_CB_I        = 0x46,

    SM5720_FG_REG_VOLT_CAL          = 0x50,

    SM5720_FG_REG_ECV_I_OFF         = 0x51,
    SM5720_FG_REG_ECV_I_SLO         = 0x52,
    SM5720_FG_REG_CSP_I_OFF         = 0x53,
    SM5720_FG_REG_CSP_I_SLO         = 0x54,
    SM5720_FG_REG_CSN_I_OFF         = 0x55,
    SM5720_FG_REG_CSN_I_SLO         = 0x56,
    SM5720_FG_REG_START_MQ          = 0x57,
    SM5720_FG_REG_FULL_MQ_HIGH      = 0x5B,
    SM5720_FG_REG_FULL_MQ_LOW	    = 0x5C,

    SM5720_FG_REG_SOC_CHG_INFO      = 0x60,
    SM5720_FG_REG_SOC_DISCHG_INFO   = 0x61,

    SM5720_FG_REG_DP_ECV_I_OFF      = 0x62,
    SM5720_FG_REG_DP_ECV_I_SLO      = 0x63,
    SM5720_FG_REG_DP_CSP_I_OFF      = 0x64,
    SM5720_FG_REG_DP_CSP_I_SLO      = 0x65,
    SM5720_FG_REG_DP_CSN_I_OFF      = 0x66,
    SM5720_FG_REG_DP_CSN_I_SLO      = 0x67,

        //for debug
    SM5720_FG_REG_OCV_STATE         = 0x80,
    SM5720_FG_REG_MQ                = 0x83,
    SM5720_FG_REG_CURRENT_EST       = 0x85,
    SM5720_FG_REG_CURRENT_ERR       = 0x86,
    SM5720_FG_REG_Q_EST             = 0x87,
    SM5720_FG_REG_CURR_ALG          = 0x88,

        //etc
    SM5720_FG_REG_MISC              = 0x90,
    SM5720_FG_REG_RESET             = 0x91,
    SM5720_FG_REG_AUX_1             = 0x92,
    SM5720_FG_REG_AUX_2             = 0x93,
    SM5720_FG_REG_AUX_STAT          = 0x94,

    SM5720_FG_REG_TABLE_START       = 0xA0,
    SM5720_FG_REG_MIN_CAP           = 0xC0,
    SM5720_FG_REG_CAP               = 0xC1,

    SM5720_FG_REG_END,
};

enum sm5720_irq_source {
    MUIC_INT1 = 0,
    MUIC_INT2,
    MUIC_INT3_AFC,
    CHG_INT1,
    CHG_INT2,
    CHG_INT3,
    CHG_INT4,
    FG_INT,

    SM5720_IRQ_GROUP_NR,
};
#define SM5720_NUM_IRQ_MUIC_REGS    3
#define SM5720_NUM_IRQ_CHG_REGS     4


enum sm5720_irq {
    /* MUIC INT1 */
    SM5720_MUIC_IRQ_INT1_DETACH,
    SM5720_MUIC_IRQ_INT1_ATTACH,

    /* MUIC INT2 */
    SM5720_MUIC_IRQ_INT2_VBUSDET_ON,
    SM5720_MUIC_IRQ_INT2_VBUS_OFF,

    /* MUIC INT3-AFC */
    SM5720_MUIC_IRQ_INT3_QC20_ACCEPTED,
    SM5720_MUIC_IRQ_INT3_AFC_ERROR,
    SM5720_MUIC_IRQ_INT3_AFC_STA_CHG,
    SM5720_MUIC_IRQ_INT3_MULTI_BYTE,
    SM5720_MUIC_IRQ_INT3_VBUS_UPDATE,
    SM5720_MUIC_IRQ_INT3_AFC_ACCEPTED,
    SM5720_MUIC_IRQ_INT3_AFC_TA_ATTACHED,

    /* CHG INT1 */
    SM5720_CHG_IRQ_INT1_VBUSPOK,
    SM5720_CHG_IRQ_INT1_VBUSUVLO,
    SM5720_CHG_IRQ_INT1_VBUSOVP,
    SM5720_CHG_IRQ_INT1_VBUSLIMIT,
    SM5720_CHG_IRQ_INT1_WPCINPOK,
    SM5720_CHG_IRQ_INT1_WPCINUVLO,
    SM5720_CHG_IRQ_INT1_WPCINOVP,
    SM5720_CHG_IRQ_INT1_WPCINLIMIT,

    /* CHG INT2 */
    SM5720_CHG_IRQ_INT2_AICL,
    SM5720_CHG_IRQ_INT2_BATOVP,
    SM5720_CHG_IRQ_INT2_NOBAT,
    SM5720_CHG_IRQ_INT2_CHGON,
    SM5720_CHG_IRQ_INT2_Q4FULLON,
    SM5720_CHG_IRQ_INT2_TOPOFF,
    SM5720_CHG_IRQ_INT2_DONE,
    SM5720_CHG_IRQ_INT2_WDTMROFF,

    /* CHG INT3 */
	SM5720_CHG_IRQ_INT3_OTGFAIL,
    SM5720_CHG_IRQ_INT3_DISLIMIT,
    SM5720_CHG_IRQ_INT3_PRETMROFF,
    SM5720_CHG_IRQ_INT3_FASTTMROFF,
    SM5720_CHG_IRQ_INT3_nENQ4,
    SM5720_CHG_IRQ_INT3_VSYSOVP,

    /* CHG INT4 */
    SM5720_CHG_IRQ_INT4_nVBUSOK,
    SM5720_CHG_IRQ_INT4_nWPCOK,

    /* FG INT */
    SM5720_FG_IRQ_INT_LOW_VOLTAGE,
    SM5720_FG_IRQ_INT_LOW_SOC,

    SM5720_IRQ_NR,
};

struct sm5720_dev {
	struct device *dev;
	struct i2c_client *i2c; 	/* 0x92; Haptic, PMIC */
	struct i2c_client *charger;     /* 0x92; Charger */
	struct i2c_client *fuelgauge;   /* 0xE2; Fuelgauge */
	struct i2c_client *muic;        /* 0x4A; MUIC */
	struct mutex i2c_lock;

	int type;

	int irq;
	int irq_base;
	int irq_gpio;
	bool wakeup;
	struct mutex irqlock;
	int irq_masks_cur[SM5720_IRQ_GROUP_NR];
	int irq_masks_cache[SM5720_IRQ_GROUP_NR];

	struct pinctrl *i2c_pinctrl;
	struct pinctrl_state *i2c_gpio_state_active;
	struct pinctrl_state *i2c_gpio_state_suspend;

#ifdef CONFIG_HIBERNATION
	/* For hibernation */
	u8 reg_muic_dump[SM5720_MUIC_REG_END];
    u8 reg_chg_dump[SM5720_CHG_REG_END];
	u8 reg_fg_dump[SM5720_FG_REG_END];
#endif

    /* For IC-Reset protection */
    void (*check_muic_reset)(struct i2c_client *, void *);
    void (*check_chg_reset)(struct i2c_client *, void *);
    void (*check_fg_reset)(struct i2c_client *, void *);
    void *muic_data;
    void *chg_data;
    void *fg_data;

	u8 device_id;

	struct sm5720_platform_data *pdata;
};

enum sm5720_types {
	TYPE_SM5720,
};

extern int sm5720_irq_init(struct sm5720_dev *sm5720);
extern void sm5720_irq_exit(struct sm5720_dev *sm5720);

/* SM5720 shared i2c API function */
extern int sm5720_read_reg(struct i2c_client *i2c, u8 reg, u8 *dest);
extern int sm5720_bulk_read(struct i2c_client *i2c, u8 reg, int count, u8 *buf);
extern int sm5720_read_word(struct i2c_client *i2c, u8 reg);
extern int sm5720_write_reg(struct i2c_client *i2c, u8 reg, u8 value);
extern int sm5720_bulk_write(struct i2c_client *i2c, u8 reg, int count, u8 *buf);
extern int sm5720_write_word(struct i2c_client *i2c, u8 reg, u16 value);

extern int sm5720_update_reg(struct i2c_client *i2c, u8 reg, u8 val, u8 mask);

extern int sm5720_get_device_ID(void);

#endif /* __SM5720_PRIV_H__ */

