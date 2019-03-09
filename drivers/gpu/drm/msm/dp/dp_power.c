/*
 * Copyright (c) 2012-2017, The Linux Foundation. All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 and
 * only version 2 as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 */

#define pr_fmt(fmt)	"[drm-dp] %s: " fmt, __func__

#include <linux/clk.h>
#include "dp_power.h"
#ifdef CONFIG_SEC_DISPLAYPORT
#include <linux/regulator/consumer.h>
#include <linux/delay.h>
#include "secdp.h"
#endif

#define DP_CLIENT_NAME_SIZE	20

struct dp_power_private {
	struct dp_parser *parser;
	struct platform_device *pdev;
	struct clk *pixel_clk_rcg;
	struct clk *pixel_parent;

	struct dp_power dp_power;
	struct sde_power_client *dp_core_client;
	struct sde_power_handle *phandle;

	bool core_clks_on;
	bool link_clks_on;
#ifdef CONFIG_SEC_DISPLAYPORT
	bool aux_pullup_on;
	bool usb1_ss_core_on;
#endif
};

#ifdef CONFIG_SEC_DISPLAYPORT
struct dp_power_private *g_secdp_power;
#endif

static int dp_power_regulator_init(struct dp_power_private *power)
{
	int rc = 0, i = 0, j = 0;
	struct platform_device *pdev;
	struct dp_parser *parser;

	parser = power->parser;
	pdev = power->pdev;

	for (i = DP_CORE_PM; !rc && (i < DP_MAX_PM); i++) {
		rc = msm_dss_config_vreg(&pdev->dev,
			parser->mp[i].vreg_config,
			parser->mp[i].num_vreg, 1);
		if (rc) {
			pr_err("failed to init vregs for %s\n",
				dp_parser_pm_name(i));
			for (j = i - 1; j >= DP_CORE_PM; j--) {
				msm_dss_config_vreg(&pdev->dev,
				parser->mp[j].vreg_config,
				parser->mp[j].num_vreg, 0);
			}

			goto error;
		}
	}
error:
	return rc;
}

static void dp_power_regulator_deinit(struct dp_power_private *power)
{
	int rc = 0, i = 0;
	struct platform_device *pdev;
	struct dp_parser *parser;

	parser = power->parser;
	pdev = power->pdev;

	for (i = DP_CORE_PM; (i < DP_MAX_PM); i++) {
		rc = msm_dss_config_vreg(&pdev->dev,
			parser->mp[i].vreg_config,
			parser->mp[i].num_vreg, 0);
		if (rc)
			pr_err("failed to deinit vregs for %s\n",
				dp_parser_pm_name(i));
	}
}

#ifdef CONFIG_SEC_DISPLAYPORT
extern struct regulator *usb1_ss_core_vreg;
extern struct regulator *aux_pullup_vreg;

/* factory use only
 * ref: qusb_phy_enable_power()
 */
static int secdp_aux_pullup_vreg_enable(bool on)
{
	int rc = 0;
	struct dp_power_private *power = g_secdp_power;
	struct regulator *aux_pu_vreg = aux_pullup_vreg;

	pr_debug("+++, on(%d)\n", on);

	if (!aux_pu_vreg) {
		pr_err("vdda33 is null!\n");
		goto exit;
	}

#define QUSB2PHY_3P3_VOL_MIN		3075000 /* uV */
#define QUSB2PHY_3P3_VOL_MAX		3200000 /* uV */
#define QUSB2PHY_3P3_HPM_LOAD		30000	/* uA */

	if (on) {
		if (power->aux_pullup_on) {
			pr_info("already on\n");
			goto exit;
		}

		rc = regulator_set_load(aux_pu_vreg, QUSB2PHY_3P3_HPM_LOAD);
		if (rc < 0) {
			pr_err("Unable to set HPM of vdda33: %d\n", rc);
			goto exit;
		}

		rc = regulator_set_voltage(aux_pu_vreg, QUSB2PHY_3P3_VOL_MIN,
					QUSB2PHY_3P3_VOL_MAX);
		if (rc) {
			pr_err("Unable to set voltage for vdda33: %d\n", rc);
			goto put_vdda33_lpm;
		}

		rc = regulator_enable(aux_pu_vreg);
		if (rc) {
			pr_err("Unable to enable vdda33: %d\n", rc);
			goto unset_vdd33;
		}

		pr_info("on success\n");
		power->aux_pullup_on = true;
	} else {

		rc = regulator_disable(aux_pu_vreg);
		if (rc)
			pr_err("Unable to disable vdda33: %d\n", rc);

unset_vdd33:
		rc = regulator_set_voltage(aux_pu_vreg, 0, QUSB2PHY_3P3_VOL_MAX);
		if (rc)
			pr_err("Unable to set (0) voltage for vdda33: %d\n", rc);

put_vdda33_lpm:
		rc = regulator_set_load(aux_pu_vreg, 0);
		if (rc < 0)
			pr_err("Unable to set (0) HPM of vdda33: %d\n", rc);

		if (!rc)
			pr_info("off success\n");

		power->aux_pullup_on = false;
	}

exit:
	return rc;
}

static int secdp_usb1_ss_core_vreg_enable(bool on)
{
	int rc = 0;
	struct dp_power_private *power = g_secdp_power;
	struct regulator *ss_core_vreg = usb1_ss_core_vreg;

	pr_debug("+++, on(%d)\n", on);

	if (!ss_core_vreg) {
		pr_err("ss_core_vreg is null!\n");
		goto exit;
	}

	if (on) {
		if (power->usb1_ss_core_on) {
			pr_info("already on\n");
			goto exit;
		}

		rc = regulator_enable(ss_core_vreg);
		if (rc) {
			pr_err("Unable to enable ss_core_vreg: %d\n", rc);
			goto exit;
		}

		pr_info("on success\n");
		power->usb1_ss_core_on = true;
	} else {
		if (!power->usb1_ss_core_on) {
			pr_info("already off\n");
			goto exit;
		}

		rc = regulator_disable(ss_core_vreg);
		if (rc) {
			pr_err("Unable to disable ss_core_vreg: %d\n", rc);
			goto exit;
		}

		pr_info("off success\n");
		power->usb1_ss_core_on = false;
	}

exit:
	return rc;
}
#endif

static int dp_power_regulator_ctrl(struct dp_power_private *power, bool enable)
{
	int rc = 0, i = 0, j = 0;
	struct dp_parser *parser;

	parser = power->parser;

	for (i = DP_CORE_PM; i < DP_MAX_PM; i++) {
		rc = msm_dss_enable_vreg(
			parser->mp[i].vreg_config,
			parser->mp[i].num_vreg, enable);
		if (rc) {
			pr_err("failed to '%s' vregs for %s\n",
					enable ? "enable" : "disable",
					dp_parser_pm_name(i));
			if (enable) {
				for (j = i-1; j >= DP_CORE_PM; j--) {
					msm_dss_enable_vreg(
					parser->mp[j].vreg_config,
					parser->mp[j].num_vreg, 0);
				}
			}
			goto error;
		}
	}

#ifdef CONFIG_SEC_DISPLAYPORT
	secdp_aux_pullup_vreg_enable(enable);
	secdp_usb1_ss_core_vreg_enable(enable);
#endif

error:
	return rc;
}

static int dp_power_pinctrl_set(struct dp_power_private *power, bool active)
{
	int rc = -EFAULT;
	struct pinctrl_state *pin_state;
	struct dp_parser *parser;

	parser = power->parser;

	if (IS_ERR_OR_NULL(parser->pinctrl.pin))
		return PTR_ERR(parser->pinctrl.pin);

	pin_state = active ? parser->pinctrl.state_active
				: parser->pinctrl.state_suspend;
	if (!IS_ERR_OR_NULL(pin_state)) {
		rc = pinctrl_select_state(parser->pinctrl.pin,
				pin_state);
		if (rc)
			pr_err("can not set %s pins\n",
			       active ? "dp_active"
			       : "dp_sleep");
	} else {
		pr_err("invalid '%s' pinstate\n",
		       active ? "dp_active"
		       : "dp_sleep");
	}

	return rc;
}

static int dp_power_clk_init(struct dp_power_private *power, bool enable)
{
	int rc = 0;
	struct dss_module_power *core, *ctrl;
	struct device *dev;

	core = &power->parser->mp[DP_CORE_PM];
	ctrl = &power->parser->mp[DP_CTRL_PM];

	dev = &power->pdev->dev;

	if (!core || !ctrl) {
		pr_err("invalid power_data\n");
		rc = -EINVAL;
		goto exit;
	}

	if (enable) {
		rc = msm_dss_get_clk(dev, core->clk_config, core->num_clk);
		if (rc) {
			pr_err("failed to get %s clk. err=%d\n",
				dp_parser_pm_name(DP_CORE_PM), rc);
			goto exit;
		}

		rc = msm_dss_get_clk(dev, ctrl->clk_config, ctrl->num_clk);
		if (rc) {
			pr_err("failed to get %s clk. err=%d\n",
				dp_parser_pm_name(DP_CTRL_PM), rc);
			goto ctrl_get_error;
		}

		power->pixel_clk_rcg = devm_clk_get(dev, "pixel_clk_rcg");
		if (IS_ERR(power->pixel_clk_rcg)) {
			pr_debug("Unable to get DP pixel clk RCG\n");
			power->pixel_clk_rcg = NULL;
		}

		power->pixel_parent = devm_clk_get(dev, "pixel_parent");
		if (IS_ERR(power->pixel_parent)) {
			pr_debug("Unable to get DP pixel RCG parent\n");
			power->pixel_parent = NULL;
		}
	} else {
		if (power->pixel_parent)
			devm_clk_put(dev, power->pixel_parent);

		if (power->pixel_clk_rcg)
			devm_clk_put(dev, power->pixel_clk_rcg);

		msm_dss_put_clk(ctrl->clk_config, ctrl->num_clk);
		msm_dss_put_clk(core->clk_config, core->num_clk);
	}

	return rc;

ctrl_get_error:
	msm_dss_put_clk(core->clk_config, core->num_clk);
exit:
	return rc;
}

static int dp_power_clk_set_rate(struct dp_power_private *power,
		enum dp_pm_type module, bool enable)
{
	int rc = 0;
	struct dss_module_power *mp;

	if (!power) {
		pr_err("invalid power data\n");
		rc = -EINVAL;
		goto exit;
	}

	mp = &power->parser->mp[module];

	if (enable) {
		rc = msm_dss_clk_set_rate(mp->clk_config, mp->num_clk);
		if (rc) {
			pr_err("failed to set clks rate.\n");
			goto exit;
		}

		rc = msm_dss_enable_clk(mp->clk_config, mp->num_clk, 1);
		if (rc) {
			pr_err("failed to enable clks\n");
			goto exit;
		}
	} else {
		rc = msm_dss_enable_clk(mp->clk_config, mp->num_clk, 0);
		if (rc) {
			pr_err("failed to disable clks\n");
				goto exit;
		}
	}
exit:
	return rc;
}

static int dp_power_clk_enable(struct dp_power *dp_power,
		enum dp_pm_type pm_type, bool enable)
{
	int rc = 0;
	struct dss_module_power *mp;
	struct dp_power_private *power;

	if (!dp_power) {
		pr_err("invalid power data\n");
		rc = -EINVAL;
		goto error;
	}

	power = container_of(dp_power, struct dp_power_private, dp_power);

	mp = &power->parser->mp[pm_type];

	if ((pm_type != DP_CORE_PM) && (pm_type != DP_CTRL_PM)) {
		pr_err("unsupported power module: %s\n",
				dp_parser_pm_name(pm_type));
		return -EINVAL;
	}

	if (enable) {
		if ((pm_type == DP_CORE_PM)
			&& (power->core_clks_on)) {
			pr_debug("core clks already enabled\n");
			return 0;
		}

		if ((pm_type == DP_CTRL_PM)
			&& (power->link_clks_on)) {
			pr_debug("links clks already enabled\n");
			return 0;
		}

		if ((pm_type == DP_CTRL_PM) && (!power->core_clks_on)) {
			pr_debug("Need to enable core clks before link clks\n");

			rc = dp_power_clk_set_rate(power, pm_type, enable);
			if (rc) {
				pr_err("failed to enable clks: %s. err=%d\n",
					dp_parser_pm_name(DP_CORE_PM), rc);
				goto error;
			} else {
				power->core_clks_on = true;
			}
		}
	}

#ifdef CONFIG_SEC_DISPLAYPORT
	if (!enable) {
		/* consider below abnormal sequence :
		 * CCIC_NOTIFY_ATTACH
		 * -> no CCIC_NOTIFY_ID_DP_LINK_CONF, no CCIC_NOTIFY_ID_DP_HPD
		 * -> CCIC_NOTIFY_DETACH
		 */
		if ((pm_type == DP_CORE_PM)
			&& (!power->core_clks_on)) {
			pr_debug("core clks already disabled\n");
			return 0;
		}

		if ((pm_type == DP_CTRL_PM)
			&& (!power->link_clks_on)) {
			pr_debug("links clks already disabled\n");
			return 0;
		}
	}
#endif

	rc = dp_power_clk_set_rate(power, pm_type, enable);
	if (rc) {
		pr_err("failed to '%s' clks for: %s. err=%d\n",
			enable ? "enable" : "disable",
			dp_parser_pm_name(pm_type), rc);
			goto error;
	}

	if (pm_type == DP_CORE_PM)
		power->core_clks_on = enable;
	else
		power->link_clks_on = enable;

	pr_debug("%s clocks for %s\n",
			enable ? "enable" : "disable",
			dp_parser_pm_name(pm_type));
	pr_debug("link_clks:%s core_clks:%s\n",
		power->link_clks_on ? "on" : "off",
		power->core_clks_on ? "on" : "off");
error:
	return rc;
}

static int dp_power_request_gpios(struct dp_power_private *power)
{
	int rc = 0, i;
	struct device *dev;
	struct dss_module_power *mp;
	static const char * const gpio_names[] = {
		"aux_enable", "aux_sel", "usbplug_cc",
	};

	if (!power) {
		pr_err("invalid power data\n");
		return -EINVAL;
	}

	dev = &power->pdev->dev;
	mp = &power->parser->mp[DP_CORE_PM];

	for (i = 0; i < ARRAY_SIZE(gpio_names); i++) {
		unsigned int gpio = mp->gpio_config[i].gpio;

		if (gpio_is_valid(gpio)) {
			rc = devm_gpio_request(dev, gpio, gpio_names[i]);
			if (rc) {
				pr_err("request %s gpio failed, rc=%d\n",
					       gpio_names[i], rc);
				goto error;
			}
		}
	}
	return 0;
error:
	for (i = 0; i < ARRAY_SIZE(gpio_names); i++) {
		unsigned int gpio = mp->gpio_config[i].gpio;

		if (gpio_is_valid(gpio))
			gpio_free(gpio);
	}
	return rc;
}

static bool dp_power_find_gpio(const char *gpio1, const char *gpio2)
{
	return !!strnstr(gpio1, gpio2, strlen(gpio1));
}

#ifndef CONFIG_SEC_DISPLAYPORT
static void dp_power_set_gpio(struct dp_power_private *power, bool flip)
{
	int i;
	struct dss_module_power *mp = &power->parser->mp[DP_CORE_PM];
	struct dss_gpio *config = mp->gpio_config;

	for (i = 0; i < mp->num_gpio; i++) {
		if (dp_power_find_gpio(config->gpio_name, "aux-sel"))
			config->value = flip;

		if (gpio_is_valid(config->gpio)) {
			pr_debug("gpio %s, value %d\n", config->gpio_name,
				config->value);

			if (dp_power_find_gpio(config->gpio_name, "aux-en") ||
			    dp_power_find_gpio(config->gpio_name, "aux-sel"))
				gpio_direction_output(config->gpio,
					config->value);
			else
				gpio_set_value(config->gpio, config->value);

		}
		config++;
	}
}
#else
int secdp_power_request_gpios(struct dp_power *dp_power)
{
	int rc;
	struct dp_power_private *power;

	if (!dp_power) {
		pr_err("invalid power data\n");
		rc = -EINVAL;
		goto exit;
	}

	power = container_of(dp_power, struct dp_power_private, dp_power);
	rc = dp_power_request_gpios(power);

exit:
	return rc;
}

/* turn on EDP_AUX switch
 * ===================================================
 * | usbplug-cc(dir) | orientation | flip  | aux-sel |
 * ===================================================
 * |        0        |     CC1     | false |    0    |
 * |        1        |     CC2     | true  |    1    |
 * ===================================================
 */
static void secdp_power_set_gpio(bool flip)
{
	int i;
	/*int dir = (flip == false) ? 0 : 1;*/
	struct dp_power_private *power = g_secdp_power;
	struct dss_module_power *mp = &power->parser->mp[DP_CORE_PM];
	struct dss_gpio *config;

	pr_debug("+++, flip(%d)\n", flip);

#if 0 /*use flip instead*/
	config = mp->gpio_config;
	for (i = 0; i < mp->num_gpio; i++) {
		if (gpio_is_valid(config->gpio)) {
			if (dp_power_find_gpio(config->gpio_name, "usbplug-cc")) {
				dir = gpio_get_value(config->gpio);
				pr_info("%s -> dir: %d\n", config->gpio_name, dir);
				break;
			}
		}
		config++;
	}

	usleep_range(100, 120);
#endif

	config = mp->gpio_config;
	for (i = 0; i < mp->num_gpio; i++) {
		if (gpio_is_valid(config->gpio)) {
			if (dp_power_find_gpio(config->gpio_name, "aux-sel")) {
				gpio_direction_output(config->gpio, (flip == false)/*(dir == 0)*/ ? 0 : 1);
				usleep_range(100, 120);
				pr_info("%s -> set %d\n", config->gpio_name, gpio_get_value(config->gpio));
				break;
			}
		}
		config++;
	}

	usleep_range(100, 120);
	config = mp->gpio_config;
	for (i = 0; i < mp->num_gpio; i++) {
		if (gpio_is_valid(config->gpio)) {
			if (dp_power_find_gpio(config->gpio_name, "aux-en")) {
				gpio_direction_output(config->gpio, 0);
				pr_info("%s -> %d\n", config->gpio_name, gpio_get_value(config->gpio));
				break;
			}
		}
		config++;
	}
}

/* turn off EDP_AUX switch */
static void secdp_power_unset_gpio(void)
{
	int i;
	struct dp_power_private *power = g_secdp_power;
	struct dss_module_power *mp = &power->parser->mp[DP_CORE_PM];
	struct dss_gpio *config;

	pr_debug("+++\n");

	config = mp->gpio_config;
	for (i = 0; i < mp->num_gpio; i++) {
		if (gpio_is_valid(config->gpio)) {
			if (dp_power_find_gpio(config->gpio_name, "aux-en")) {
				gpio_direction_output(config->gpio, 1);
				pr_info("%s -> 1\n", config->gpio_name);
				break;
			}
		}
		config++;
	}

	config = mp->gpio_config;
	for (i = 0; i < mp->num_gpio; i++) {
		if (gpio_is_valid(config->gpio)) {
			if (dp_power_find_gpio(config->gpio_name, "aux-sel")) {
				gpio_direction_output(config->gpio, 0);
				pr_info("%s -> 0\n", config->gpio_name);
				break;
			}
		}
		config++;
	}
}

/*
 * @aux_sel : 1 or 0
 */
void secdp_config_gpios_factory(int aux_sel, bool on)
{
	pr_debug("%s (%d,%d)\n", __func__, aux_sel, on);

	if (on) {
		/* power on ldo24 */
		secdp_aux_pullup_vreg_enable(true);

		/* set aux_sel, aux_en */
		secdp_power_set_gpio(aux_sel);
	} else {
		/* unset aux_sel, aux_en */
		secdp_power_unset_gpio();

		/* power off ldo24 */
		secdp_aux_pullup_vreg_enable(false);
	}
}

enum plug_orientation secdp_get_plug_orientation(void)
{
	int i, dir;
	struct dp_power_private *power = g_secdp_power;
	struct dss_module_power *mp = &power->parser->mp[DP_CORE_PM];
	struct dss_gpio *config = mp->gpio_config;

	for (i = 0; i < mp->num_gpio; i++) {
		if (gpio_is_valid(config->gpio)) {
			if (dp_power_find_gpio(config->gpio_name, "usbplug-cc")) {
				dir = gpio_get_value(config->gpio);
				pr_info("orientation: %s\n", (dir == 0) ? "CC1" : "CC2");
				if (dir == 0)
					return ORIENTATION_CC1;
				else /* if (dir == 1) */
					return ORIENTATION_CC2;
			}
		}
		config++;
	}

	/*cannot be here*/
	return ORIENTATION_NONE;
}
#endif

static int dp_power_config_gpios(struct dp_power_private *power, bool flip,
					bool enable)
{
#ifndef CONFIG_SEC_DISPLAYPORT
	int rc = 0, i;
#endif
	struct dss_module_power *mp;
	struct dss_gpio *config;

	mp = &power->parser->mp[DP_CORE_PM];
	config = mp->gpio_config;

	if (enable) {
#ifndef CONFIG_SEC_DISPLAYPORT
		rc = dp_power_request_gpios(power);
		if (rc) {
			pr_err("gpio request failed\n");
			return rc;
		}

		dp_power_set_gpio(power, flip);
#else
		secdp_power_set_gpio(flip);
#endif
	} else {
#ifndef CONFIG_SEC_DISPLAYPORT
		for (i = 0; i < mp->num_gpio; i++) {
			gpio_set_value(config[i].gpio, 0);
			gpio_free(config[i].gpio);
		}
#else
		secdp_power_unset_gpio();
#endif
	}

	return 0;
}

static int dp_power_client_init(struct dp_power *dp_power,
		struct sde_power_handle *phandle)
{
	int rc = 0;
	struct dp_power_private *power;
	char dp_client_name[DP_CLIENT_NAME_SIZE];

	if (!dp_power) {
		pr_err("invalid power data\n");
		return -EINVAL;
	}

	power = container_of(dp_power, struct dp_power_private, dp_power);

	rc = dp_power_regulator_init(power);
	if (rc) {
		pr_err("failed to init regulators\n");
		goto error_power;
	}

	rc = dp_power_clk_init(power, true);
	if (rc) {
		pr_err("failed to init clocks\n");
		goto error_clk;
	}

	power->phandle = phandle;
	snprintf(dp_client_name, DP_CLIENT_NAME_SIZE, "dp_core_client");
	power->dp_core_client = sde_power_client_create(phandle,
			dp_client_name);
	if (IS_ERR_OR_NULL(power->dp_core_client)) {
		pr_err("[%s] client creation failed for DP", dp_client_name);
		rc = -EINVAL;
		goto error_client;
	}

#ifdef CONFIG_SEC_DISPLAYPORT
	rc = dp_power_pinctrl_set(power, false);
	if (rc) {
		pr_err("failed to set pinctrl state\n");
		goto error_client;
	}
#endif
	return 0;

error_client:
	dp_power_clk_init(power, false);
error_clk:
	dp_power_regulator_deinit(power);
error_power:
	return rc;
}

static void dp_power_client_deinit(struct dp_power *dp_power)
{
	struct dp_power_private *power;

	if (!dp_power) {
		pr_err("invalid power data\n");
		return;
	}

	power = container_of(dp_power, struct dp_power_private, dp_power);

	sde_power_client_destroy(power->phandle, power->dp_core_client);
	dp_power_clk_init(power, false);
	dp_power_regulator_deinit(power);
}

static int dp_power_set_pixel_clk_parent(struct dp_power *dp_power)
{
	int rc = 0;
	struct dp_power_private *power;

	if (!dp_power) {
		pr_err("invalid power data\n");
		rc = -EINVAL;
		goto exit;
	}

	power = container_of(dp_power, struct dp_power_private, dp_power);

	if (power->pixel_clk_rcg && power->pixel_parent)
		clk_set_parent(power->pixel_clk_rcg, power->pixel_parent);
exit:
	return rc;
}

static int dp_power_init(struct dp_power *dp_power, bool flip)
{
	int rc = 0;
	struct dp_power_private *power;

	pr_debug("+++\n");

	if (!dp_power) {
		pr_err("invalid power data\n");
		rc = -EINVAL;
		goto exit;
	}

	power = container_of(dp_power, struct dp_power_private, dp_power);

	rc = dp_power_regulator_ctrl(power, true);
	if (rc) {
		pr_err("failed to enable regulators\n");
		goto exit;
	}

	rc = dp_power_pinctrl_set(power, true);
	if (rc) {
		pr_err("failed to set pinctrl state\n");
		goto err_pinctrl;
	}

	rc = dp_power_config_gpios(power, flip, true);
	if (rc) {
		pr_err("failed to enable gpios\n");
		goto err_gpio;
	}

	rc = sde_power_resource_enable(power->phandle,
		power->dp_core_client, true);
	if (rc) {
		pr_err("Power resource enable failed\n");
		goto err_sde_power;
	}

	rc = dp_power_clk_enable(dp_power, DP_CORE_PM, true);
	if (rc) {
		pr_err("failed to enable DP core clocks\n");
		goto err_clk;
	}

	return 0;

err_clk:
	sde_power_resource_enable(power->phandle, power->dp_core_client, false);
err_sde_power:
	dp_power_config_gpios(power, flip, false);
err_gpio:
	dp_power_pinctrl_set(power, false);
err_pinctrl:
	dp_power_regulator_ctrl(power, false);
exit:
	return rc;
}

static int dp_power_deinit(struct dp_power *dp_power)
{
	int rc = 0;
	struct dp_power_private *power;

	pr_debug("+++\n");

	if (!dp_power) {
		pr_err("invalid power data\n");
		rc = -EINVAL;
		goto exit;
	}

	power = container_of(dp_power, struct dp_power_private, dp_power);

	dp_power_clk_enable(dp_power, DP_CORE_PM, false);
	/*
	 * If the display power on event was not successful, for example if
	 * there was a link training failure, then the link clocks could
	 * possibly still be on. In this scenario, we need to turn off the
	 * link clocks as soon as the cable is disconnected so that the clock
	 * state is cleaned up before subsequent connection events.
	 */
	if (power->link_clks_on)
		dp_power_clk_enable(dp_power, DP_CTRL_PM, false);
	rc = sde_power_resource_enable(power->phandle,
			power->dp_core_client, false);
	if (rc) {
		pr_err("Power resource enable failed, rc=%d\n", rc);
		goto exit;
	}
	dp_power_config_gpios(power, false, false);
	dp_power_pinctrl_set(power, false);
	dp_power_regulator_ctrl(power, false);
exit:
	return rc;
}

struct dp_power *dp_power_get(struct dp_parser *parser)
{
	int rc = 0;
	struct dp_power_private *power;
	struct dp_power *dp_power;

	if (!parser) {
		pr_err("invalid input\n");
		rc = -EINVAL;
		goto error;
	}

	power = devm_kzalloc(&parser->pdev->dev, sizeof(*power), GFP_KERNEL);
	if (!power) {
		rc = -ENOMEM;
		goto error;
	}

	power->parser = parser;
	power->pdev = parser->pdev;

	dp_power = &power->dp_power;

	dp_power->init = dp_power_init;
	dp_power->deinit = dp_power_deinit;
	dp_power->clk_enable = dp_power_clk_enable;
	dp_power->set_pixel_clk_parent = dp_power_set_pixel_clk_parent;
	dp_power->power_client_init = dp_power_client_init;
	dp_power->power_client_deinit = dp_power_client_deinit;

#ifdef CONFIG_SEC_DISPLAYPORT
	g_secdp_power = power;
#endif

	return dp_power;
error:
	return ERR_PTR(rc);
}

void dp_power_put(struct dp_power *dp_power)
{
	struct dp_power_private *power = NULL;

	if (!dp_power)
		return;

	power = container_of(dp_power, struct dp_power_private, dp_power);

	devm_kfree(&power->pdev->dev, power);

#ifdef CONFIG_SEC_DISPLAYPORT
	g_secdp_power = NULL;
#endif
}
