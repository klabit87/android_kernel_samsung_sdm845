/*
 * s2abb01.h
 *
 * Copyright (c) 2017 Samsung Electronics Co., Ltd
 *              http://www.samsung.com
 *
 *  This program is free software; you can redistribute  it and/or modify it
 *  under  the terms of  the GNU General  Public License as published by the
 *  Free Software Foundation;  either version 2 of the  License, or (at your
 *  option) any later version.
 *
 */

#ifndef __LINUX_MFD_S2ABB01_H
#define __LINUX_MFD_S2ABB01_H
#include <linux/platform_device.h>
#include <linux/regmap.h>

#define MFD_DEV_NAME "s2abb01"

/**
 * sec_regulator_data - regulator data
 * @id: regulator id
 * @initdata: regulator init data (constraints, supplies, ...)
 */

struct s2abb01_dev {
	struct device *dev;
	struct i2c_client *i2c;
	struct mutex i2c_lock;

	int type;
	u8 rev_num; /* pmic Rev */
	bool wakeup;

	struct s2abb01_platform_data *pdata;
};

struct s2abb01_regulator_data {
	int id;
	struct regulator_init_data *initdata;
	struct device_node *reg_node;
};

struct s2abb01_platform_data {
	bool wakeup;
	int num_regulators;
	struct	s2abb01_regulator_data *regulators;
	int	device_type;
};

struct s2abb01 {
	struct regmap *regmap;
};

/* S2ABB01 registers */
/* Slave Addr : 0xAA */
enum S2ABB01_reg {
	S2ABB01_REG_PMIC_ID,
	S2ABB01_REG_BUCK_CFG,
	S2ABB01_REG_BUCK_OUT,
	S2ABB01_REG_LDO_CFG,
	S2ABB01_REG_LDO_DSCH,
};

/* S2ABB01 regulator ids */
enum S2ABB01_regulators {
	S2ABB01_LDO,
	S2ABB01_BUCK,

	S2ABB01_REG_MAX,
};

#define S2ABB01_LDO_MIN		1500000
#define S2ABB01_LDO_STEP	50000
#define S2ABB01_LDO_VSEL_MASK	0x3F

#define S2ABB01_BUCK_MIN	2400000	/* real support voltage is 2600000 ~ */
#define S2ABB01_BUCK_STEP	12500
#define S2ABB01_BUCK_VSEL_MASK		0x7F
#define S2ABB01_BUCK_ENABLE_MASK	0x80

#define S2ABB01_RAMP_DELAY	18000
#define S2ABB01_ENABLE_MASK	0x80

#define S2ABB01_ENABLE_TIME_LDO		50
#define S2ABB01_ENABLE_TIME_BUCK	350

#define S2ABB01_LDO_ENABLE_SHIFT	0x07
#define S2ABB01_LDO_N_VOLTAGES	(S2ABB01_LDO_VSEL_MASK + 1)

#define S2ABB01_BUCK_ENABLE_SHIFT	0x07
#define S2ABB01_BUCK_N_VOLTAGES		(0x60 + 1)

#define S2ABB01_REGULATOR_MAX (S2ABB01_REG_MAX)

#endif /*  __LINUX_MFD_S2ABB01_H */
