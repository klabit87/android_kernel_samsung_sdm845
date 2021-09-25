/*
 * linux/regulator/s2dos03-regulator.h
 *
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#ifndef __LINUX_S2DOS03_REGULATOR_H
#define __LINUX_S2DOS03_REGULATOR_H

/*******************************************************************************
 * Useful Macros
 ******************************************************************************/
#undef  __CONST_FFS
#define __CONST_FFS(_x) \
        ((_x) & 0x0F ? ((_x) & 0x03 ? ((_x) & 0x01 ? 0 : 1) :\
                                      ((_x) & 0x04 ? 2 : 3)) :\
                       ((_x) & 0x30 ? ((_x) & 0x10 ? 4 : 5) :\
                                      ((_x) & 0x40 ? 6 : 7)))

#undef  BIT_RSVD
#define BIT_RSVD  0

#undef  BITS
#define BITS(_end, _start) \
        ((BIT(_end) - BIT(_start)) + BIT(_end))

#undef  __BITS_GET
#define __BITS_GET(_word, _mask, _shift) \
        (((_word) & (_mask)) >> (_shift))

#undef  BITS_GET
#define BITS_GET(_word, _bit) \
        __BITS_GET(_word, _bit, FFS(_bit))

#undef  __BITS_SET
#define __BITS_SET(_word, _mask, _shift, _val) \
        (((_word) & ~(_mask)) | (((_val) << (_shift)) & (_mask)))

#undef  BITS_SET
#define BITS_SET(_word, _bit, _val) \
        __BITS_SET(_word, _bit, FFS(_bit), _val)

#undef  BITS_MATCH
#define BITS_MATCH(_word, _bit) \
        (((_word) & (_bit)) == (_bit))


/*******************************************************************************
 * Register
 ******************************************************************************/
/* Slave Address */
#define S2DOS03_I2C_ADDR	(0xC0>>1)

/* Register */
#define REG_DEVICE_ID		0x00

#define REG_TOPSYS_STAT		0x01
#define BIT_TJCT_140C		BIT (1)
#define BIT_TJCT_120C		BIT (0)

#define REG_REG_STAT		0x02
#define BIT_ELVDD_POK		BIT (7)
#define BIT_ELVSS_POK		BIT (6)
#define BIT_AVDD_POK		BIT (5)
#define BIT_BUCK_POK		BIT (4)
#define BIT_LDO4_POK		BIT (3)
#define BIT_LDO3_POK		BIT (2)
#define BIT_LDO2_POK		BIT (1)
#define BIT_LDO1_POK		BIT (0)

#define REG_REG_EN			0x03
#define BIT_B_EN			BIT (4)
#define BIT_L4_EN			BIT (3)
#define BIT_L3_EN			BIT (2)
#define BIT_L2_EN			BIT (1)
#define BIT_L1_EN			BIT (0)

#define REG_GPIO_PD_CTRL	0x04
#define BIT_EN_B_PD_DIS		BIT (0)

#define REG_UVLO_CFG1		0x05
#define BIT_UVLO_F			BITS(1,0)

#define REG_LDO1_CFG		0x10
#define REG_LDO2_CFG		0x11
#define REG_LDO3_CFG		0x12
#define REG_LDO4_CFG		0x13
#define REG_LDOX_CFG(X)		(REG_LDO1_CFG + X - 1)
#define BIT_LX_AD			BIT (7)
#define BIT_LX_VOUT			BITS(6,0)

#define REG_BUCK_CFG		0x20
#define BIT_B_RU_SR			BITS(7,6)
#define BIT_B_RD_SR			BITS(5,4)
#define BIT_B_AD			BIT (3)
#define BIT_B_FPWM			BIT (2)
#define BIT_B_FSRAD			BIT (0)

#define REG_BUCK_VOUT		0x21
#define BIT_B_VOUT			BITS(7,0)


/* voltage range and step */
#define LDO_MINUV			600000		/* 0.6V */
#define LDO_MAXUV			3775000		/* 3.775V */
#define LDO_STEP			25000		/* 25mV */

#define BUCK_MINUV			500000		/* 0.5V */
#define BUCK_MAXUV			2093750		/* 2.09375V */
#define BUCK_STEP			6250		/* 6.25mV */


/* S2DOS03 regulator ids */
enum s2dos03_regulator_id {
	S2DOS03_LDO1 = 0,
	S2DOS03_LDO2,
	S2DOS03_LDO3,
	S2DOS03_LDO4,

	S2DOS03_BUCK,

	S2DOS03_REGULATORS = S2DOS03_BUCK,
};

struct s2dos03_regulator_data {
	int active_discharge_enable;

	struct regulator_init_data *initdata;
	struct device_node *of_node;
};

struct s2dos03_regulator_platform_data
{
	int num_regulators;
	struct s2dos03_regulator_data *regulators;

	int buck_ramp_up;
	int buck_ramp_down;
	int buck_fpwm;
	int buck_fsrad;

	int uvlo_fall_threshold;
};

#endif
