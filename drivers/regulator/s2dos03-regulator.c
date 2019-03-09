/*
 * Copyright (c) 2016, The Linux Foundation. All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 and
 * only version 2 as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#define pr_fmt(fmt) "%s: " fmt, __func__

#include <linux/err.h>
#include <linux/slab.h>
#include <linux/platform_device.h>
#include <linux/i2c.h>
#include <linux/regulator/driver.h>
#include <linux/regmap.h>
#include <linux/module.h>
#include <linux/regulator/of_regulator.h>
#include <linux/of.h>
#include <linux/regulator/s2dos03-regulator.h>

#define M2SH	__CONST_FFS


static const struct regmap_config s2dos03_regmap_config = {
    .reg_bits   = 8,
    .val_bits   = 8,
    .cache_type = REGCACHE_NONE,
};

struct s2dos03_reg {
	struct device *dev;
	struct regulator_dev **rdev;
	int num_regulators;
	struct regmap *regmap;
};

static struct regulator_ops s2dos03_reg_ops = {
	.list_voltage		= regulator_list_voltage_linear,
	.map_voltage		= regulator_map_voltage_linear,
	.is_enabled		= regulator_is_enabled_regmap,
	.enable			= regulator_enable_regmap,
	.disable		= regulator_disable_regmap,
	.get_voltage_sel	= regulator_get_voltage_sel_regmap,
	.set_voltage_sel	= regulator_set_voltage_sel_regmap,
};

#define REGULATOR_DESC_PLDO(num) {				\
	.name		= "s2dos03-ldo"#num,			\
	.id			= S2DOS03_LDO##num,		\
	.ops		= &s2dos03_reg_ops,			\
	.type		= REGULATOR_VOLTAGE,			\
	.owner		= THIS_MODULE,				\
	.n_voltages	= BIT_LX_VOUT + 1,			\
	.min_uV 	= LDO_MINUV,				\
	.uV_step 	= LDO_STEP,				\
	.vsel_reg	= REG_LDO##num##_CFG,			\
	.vsel_mask	= BIT_LX_VOUT,				\
	.enable_reg	= REG_REG_EN,				\
	.enable_mask	= BIT_L##num##_EN,			\
}

#define REGULATOR_DESC_BUCK() {					\
	.name		= "s2dos03-buck",			\
	.id			= S2DOS03_BUCK,			\
	.ops		= &s2dos03_reg_ops,			\
	.type		= REGULATOR_VOLTAGE,			\
	.owner		= THIS_MODULE,				\
	.n_voltages	= BIT_B_VOUT + 1,			\
	.min_uV 	= BUCK_MINUV,				\
	.uV_step 	= BUCK_STEP,				\
	.vsel_reg	= REG_BUCK_VOUT,			\
	.vsel_mask	= BIT_B_VOUT,				\
	.enable_reg	= REG_REG_EN,				\
	.enable_mask	= BIT_B_EN,				\
}

const static struct regulator_desc s2dos03_reg_desc[] =
{
	REGULATOR_DESC_PLDO(1),
	REGULATOR_DESC_PLDO(2),
	REGULATOR_DESC_PLDO(3),
	REGULATOR_DESC_PLDO(4),
	REGULATOR_DESC_BUCK(),
};

static int s2dos03_reg_set_active_discharge(struct s2dos03_reg *s2dos03_reg,
		struct s2dos03_regulator_platform_data *pdata, int reg_id)
{
	int rc, reg, val;

	switch (reg_id) {
		case S2DOS03_LDO1 ... S2DOS03_LDO4:
			val = pdata->regulators[reg_id - 1].active_discharge_enable ? BIT_LX_AD : 0;
			reg = REG_LDOX_CFG(reg_id);
			rc = regmap_update_bits(s2dos03_reg->regmap, reg, BIT_LX_AD, val);
			break;
		case S2DOS03_BUCK:
			val = pdata->regulators[reg_id - 1].active_discharge_enable ? BIT_B_AD : 0;
			reg = REG_BUCK_CFG;
			rc = regmap_update_bits(s2dos03_reg->regmap, reg, BIT_B_AD, val);
			break;
		default:
			rc = -EINVAL;
			break;
	}

	return rc;
}

static int s2dos03_reg_hw_init(struct s2dos03_reg *s2dos03_reg,
				struct s2dos03_regulator_platform_data *pdata)
{
	int rc, reg, val;

	/* buck configuration */
	reg = REG_BUCK_CFG;
	val = pdata->buck_ramp_up<<M2SH(BIT_B_RU_SR) |
		pdata->buck_ramp_down<<M2SH(BIT_B_RD_SR) |
		pdata->buck_fpwm<<M2SH(BIT_B_FPWM) |
		pdata->buck_fsrad<<M2SH(BIT_B_FSRAD);
	rc = regmap_update_bits(s2dos03_reg->regmap, reg,
		BIT_B_RU_SR | BIT_B_RD_SR | BIT_B_FPWM | BIT_B_FSRAD, val);
	if (rc != 0)
		return rc;

	/* uvlo falling threshold */
	reg = REG_UVLO_CFG1;
	val = pdata->uvlo_fall_threshold<<M2SH(BIT_UVLO_F);
	rc = regmap_write(s2dos03_reg->regmap, reg, val);

	return rc;
}


#ifdef CONFIG_OF
static struct s2dos03_regulator_platform_data *s2dos03_reg_parse_dt(struct device *dev)
{
	struct device_node *nproot = dev->of_node;
	struct device_node *regulators_np, *reg_np;
	struct s2dos03_regulator_platform_data *pdata;
	struct s2dos03_regulator_data *rdata;
	size_t i;
	int ret;

	if (unlikely(nproot == NULL))
		return ERR_PTR(-EINVAL);

	regulators_np = of_find_node_by_name(nproot, "regulators");
	if (unlikely(regulators_np == NULL))
	{
		dev_err(dev, "regulators node not found\n");
		return ERR_PTR(-EINVAL);
	}

	pdata = devm_kzalloc(dev, sizeof(*pdata), GFP_KERNEL);

	/* count the number of regulators to be supported in pmic */
	pdata->num_regulators = of_get_child_count(regulators_np);

	rdata = devm_kzalloc(dev, sizeof(*rdata) *
				pdata->num_regulators, GFP_KERNEL);
	if (!rdata) {
		of_node_put(regulators_np);
		dev_err(dev, "could not allocate memory for regulator data\n");
		return ERR_PTR(-ENOMEM);
	}

	pdata->regulators = rdata;
	for_each_child_of_node(regulators_np, reg_np) {
		for (i = 0; i < ARRAY_SIZE(s2dos03_reg_desc); i++)
			if (!of_node_cmp(reg_np->name, s2dos03_reg_desc[i].name))
				break;

		if (i == ARRAY_SIZE(s2dos03_reg_desc)) {
			dev_warn(dev, "don't know how to configure regulator %s\n",
				 reg_np->name);
			continue;
		}

		rdata->initdata = of_get_regulator_init_data(dev, reg_np, s2dos03_reg_desc);
		rdata->of_node = reg_np;
		ret = of_property_read_u32(reg_np, "active-discharge-enable",
					&rdata->active_discharge_enable);
		if (ret != 0)
			rdata->active_discharge_enable = 1;
		rdata++;
	}
	of_node_put(regulators_np);

	ret = of_property_read_u32(nproot, "buck-ramp-up",
						&pdata->buck_ramp_up);
	if (ret != 0)
		pdata->buck_ramp_up = 1;	/* 12.5mV/us */

	ret = of_property_read_u32(nproot, "buck-ramp-down",
						&pdata->buck_ramp_down);
	if (ret != 0)
		pdata->buck_ramp_down = 1;	/* 6.25mV/us */

	ret = of_property_read_u32(nproot, "buck-fpwm",
						&pdata->buck_fpwm);
	if (ret != 0)
		pdata->buck_fpwm = 0;		/* turn off FPWM */

	ret = of_property_read_u32(nproot, "buck-fsrad",
						&pdata->buck_fsrad);
	if (ret != 0)
		pdata->buck_fsrad = 1;		/* enable Active Discharge */

	ret = of_property_read_u32(nproot, "uvlo-fall-threshold",
						&pdata->uvlo_fall_threshold);
	if (ret != 0)
		pdata->uvlo_fall_threshold = 3;	/* 2.45V */

	return pdata;
}
#endif

static void s2dos03_destroy (struct s2dos03_reg *me)
{
	struct device *dev = me->dev;

	if (likely(me->regmap))
		regmap_exit(me->regmap);

	devm_kfree(dev, me);
}

static int s2dos03_regulator_probe(struct i2c_client *client,
						const struct i2c_device_id *id)
{
	struct device *dev = &client->dev;
	struct s2dos03_regulator_platform_data *pdata;
	struct s2dos03_reg *s2dos03_reg;
	struct regulator_config config = { };
	int unsigned long rc;
	int i, size;

	pr_info("<%s> probe start\n", client->name);

	s2dos03_reg = devm_kzalloc(dev, sizeof(struct s2dos03_reg),
								GFP_KERNEL);
	if (unlikely(!s2dos03_reg))
		return -ENOMEM;

	i2c_set_clientdata(client, s2dos03_reg);

#ifdef CONFIG_OF
	pdata = s2dos03_reg_parse_dt(dev);
	if (IS_ERR(pdata))
		return PTR_ERR(pdata);
#else
	pdata = dev_get_platdata(dev);
#endif

	size = sizeof(struct regulator_dev *) * pdata->num_regulators;
	s2dos03_reg->rdev = devm_kzalloc(dev, size, GFP_KERNEL);
	if (!s2dos03_reg->rdev)
		return -ENOMEM;
	s2dos03_reg->dev = &client->dev;


	s2dos03_reg->regmap = devm_regmap_init_i2c(client,
						&s2dos03_regmap_config);

	if (unlikely(IS_ERR(s2dos03_reg->regmap))) {
		rc = PTR_ERR(s2dos03_reg->regmap);
		s2dos03_reg->regmap = NULL;
		pr_err("<%s> failed to initialize i2c regmap pmic [%lu]\n",
							client->name, rc);
		goto abort;
	}
	s2dos03_reg->num_regulators = pdata->num_regulators;

	for (i = 0; i < pdata->num_regulators; i++) {
		config.dev = &client->dev;
		config.init_data = pdata->regulators[i].initdata;
		config.of_node = pdata->regulators[i].of_node;
		config.regmap = s2dos03_reg->regmap;
		config.driver_data = s2dos03_reg;

		s2dos03_reg->rdev[i] =
			regulator_register(&s2dos03_reg_desc[i], &config);

		if (IS_ERR(s2dos03_reg->rdev[i])) {
			rc = PTR_ERR(s2dos03_reg->rdev[i]);
			pr_err("<%s> regulator init failed for %d\n",
							client->name, i);
 			s2dos03_reg->rdev[i] = NULL;
			goto abort;
		}

		rc= s2dos03_reg_set_active_discharge(s2dos03_reg, pdata,
						s2dos03_reg_desc[i].id);
		if (IS_ERR_VALUE(rc))
			goto abort;
	}

	/* initialize platform data */
	rc= s2dos03_reg_hw_init(s2dos03_reg, pdata);
	if (IS_ERR_VALUE(rc))
		goto abort;


	pr_info("<%s> probe end\n", client->name);

	return 0;

abort:
	i2c_set_clientdata(client, NULL);
	s2dos03_destroy(s2dos03_reg);
	return rc;
}


static int s2dos03_regulator_remove(struct i2c_client *client)
{
	struct s2dos03_reg *s2dos03_reg = i2c_get_clientdata(client);
	int i;

	for (i = 0; i < s2dos03_reg->num_regulators; i++)
		regulator_unregister(s2dos03_reg->rdev[i]);

	i2c_set_clientdata(client, NULL);
	s2dos03_destroy(s2dos03_reg);

	return 0;
}

#ifdef CONFIG_OF
static struct of_device_id s2dos03_of_id[] = {
    { .compatible = "slsi,s2dos03"      },
    { },
};
MODULE_DEVICE_TABLE(of, s2dos03_of_id);
#endif /* CONFIG_OF */

static const struct i2c_device_id s2dos03_i2c_id[] = {
    { "s2dos03", 0 },
    { },
};
MODULE_DEVICE_TABLE(i2c, s2dos03_i2c_id);

static struct i2c_driver s2dos03_i2c_driver = {
    .driver.name            = "s2dos03",
    .driver.owner           = THIS_MODULE,
#ifdef CONFIG_OF
    .driver.of_match_table  = s2dos03_of_id,
#endif /* CONFIG_OF */
    .driver.suppress_bind_attrs = true,
    .id_table               = s2dos03_i2c_id,
    .probe                  = s2dos03_regulator_probe,
    .remove                 = s2dos03_regulator_remove,
};

static __init int s2dos03_init (void)
{
	int rc = -ENODEV;

	rc = i2c_add_driver(&s2dos03_i2c_driver);
	if (rc != 0)
		pr_err("Failed to register I2C driver: %d\n", rc);

	return rc;
}
subsys_initcall(s2dos03_init);

static __exit void s2dos03_exit (void)
{
	i2c_del_driver(&s2dos03_i2c_driver);
}
module_exit(s2dos03_exit);

MODULE_ALIAS("platform:s2dos03-regulator");
MODULE_DESCRIPTION("Regulator driver for S2DOS03");
MODULE_LICENSE("GPL v2");
MODULE_VERSION("1.0");

