/*
 * @file mstdrv.c
 * @brief MST drv Support
 * Copyright (c) 2015, Samsung Electronics Corporation. All rights reserved.
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

#include <linux/cpu.h>
#include <linux/cpumask.h>
#include <linux/cpufreq.h>
#include <linux/device.h>
#include <linux/delay.h>
#include <linux/err.h>
#include <linux/fs.h>
#include <linux/gpio.h>
#include <linux/i2c.h>
#include <linux/kernel.h>
#include <linux/kern_levels.h>
#include <linux/kobject.h>
#include <linux/module.h>
#include <linux/of_gpio.h>
#include <linux/percpu-defs.h>
#include <linux/pinctrl/consumer.h>
#include <linux/platform_device.h>
#include <linux/pm_qos.h>
#include <linux/power_supply.h>
#include <linux/regulator/consumer.h>
#include <linux/sched.h>
#include <linux/slab.h>
#include <linux/spinlock.h>
#include <linux/syscalls.h>
#include <linux/threads.h>
#include <linux/wakelock.h>
#include <linux/workqueue.h>

#include "mstdrv.h"

/* defines */
#define	ON				1	// On state
#define	OFF				0	// Off state
#define	TRACK1				1	// Track1 data
#define	TRACK2				2	// Track2 data

#define CMD_MST_LDO_OFF			'0'	// MST LDO off
#define CMD_MST_LDO_ON			'1'	// MST LDO on
#define CMD_SEND_TRACK1_DATA		'2'	// Send track1 test data
#define CMD_SEND_TRACK2_DATA		'3'	// send track2 test data
#define CMD_HW_RELIABILITY_TEST_START	'4'	// start HW reliability test
#define CMD_HW_RELIABILITY_TEST_STOP	'5'	// stop HW reliability test
#define ERROR_VALUE			-1	// Error value

#if defined(CONFIG_MST_NONSECURE)
#define	BITS_PER_CHAR_TRACK1		7	// Number of bits per character for Track1
#define	BITS_PER_CHAR_TRACK2		5	// Number of bits per character Track2
#define LEADING_ZEROES			28	// Number of leading zeroes to data
#define TRAILING_ZEROES			28	// Number of trailing zeroes to data
#define LRC_CHAR_TRACK1			70	// LRC character ('&' in ISO/IEC 7811) for Track1
#define	PWR_EN_WAIT_TIME		11	// time (mS) to wait after MST+PER_EN signal sent
#define	MST_DATA_TIME_DURATION		300	// time (uS) duration for transferring MST bit
#endif

#if defined(CONFIG_MFC_CHARGER)
#if 0
static inline struct power_supply *get_power_supply_by_name(char *name)
{
	if (!name)
		return (struct power_supply *)NULL;
	else
		return power_supply_get_by_name(name);
}

#define psy_do_property(name, function, property, value) \
{    \
	struct power_supply *psy;    \
	int ret;    \
	psy = get_power_supply_by_name((name));    \
	if (!psy) {    \
		mst_err("%s: Fail to "#function" psy (%s)\n",    \
			__func__, (name));    \
		value.intval = 0;    \
	} else {    \
		if (psy->desc->function##_property != NULL) { \
			ret = psy->desc->function##_property(psy, (property), &(value)); \
			if (ret < 0) {    \
				mst_err("%s: Fail to %s "#function" (%d=>%d)\n", \
					__func__, name, (property), ret);    \
				value.intval = 0;    \
			}    \
		}    \
	}    \
}
#endif
#endif

/* global variables */
static struct class *mst_drv_class;
struct device *mst_drv_dev;
static int escape_loop = 1;
static int nfc_state;
static struct wake_lock mst_wakelock;
#if defined(CONFIG_MST_REGULATOR)
struct regulator *regulator3_0;
#else
#if !defined(CONFIG_MST_IF_PMIC)
static int mst_pwr_en;
#endif
#endif
#if defined(CONFIG_MST_SUPPORT_GPIO)
static int mst_support_check;
#endif
#if defined(CONFIG_MST_NONSECURE)
static int mst_en;
static int mst_data;
static spinlock_t event_lock;
#endif
#if defined(CONFIG_MFC_CHARGER)
static int wpc_det;
#endif
#if defined(CONFIG_MST_LPM_DISABLE)
static struct pm_qos_request mst_pm_qos_request;
uint32_t mst_lpm_tag;
#endif

/* function */
/**
 * mst_printk - print with mst tag
 */
int mst_log_level = MST_LOG_LEVEL;
void mst_printk(int level, const char *fmt, ...)
{
	struct va_format vaf;
	va_list args;

	if (mst_log_level < level)
		return;

	va_start(args, fmt);

	vaf.fmt = fmt;
	vaf.va = &args;

	printk("%s %pV", TAG, &vaf);

	va_end(args);
}

#if defined(CONFIG_ARCH_QCOM)
/**
 * boost_enable - set all CPUs freq upto Max
 * boost_disable - set all CPUs freq back to Normal
 */
static int boost_enable(void)
{
	mst_info("%s: bump up snoc clock request\n", __func__);
	if (0 == ss_mst_bus_hdl){
		ss_mst_bus_hdl = msm_bus_scale_register_client(&ss_mst_bus_client_pdata);
		if (ss_mst_bus_hdl) { 
			if(msm_bus_scale_client_update_request(ss_mst_bus_hdl, 1)) {
				mst_err("%s: fail to update request!\n", __func__);
				WARN_ON(1);
				msm_bus_scale_unregister_client(ss_mst_bus_hdl);
				ss_mst_bus_hdl = 0;
			}
		} else {
			mst_err("%s: fail to register client, ss_mst_bus_hdl = %d\n",
				__func__, ss_mst_bus_hdl);
		}
	}
	return ss_mst_bus_hdl;
}

static int boost_disable(void)
{
	mst_info("%s: bump up snoc clock remove\n", __func__);
	if (ss_mst_bus_hdl) {
		if(msm_bus_scale_client_update_request(ss_mst_bus_hdl, 0))
			WARN_ON(1);
		msm_bus_scale_unregister_client(ss_mst_bus_hdl);
		ss_mst_bus_hdl = 0;
	} else {
		mst_err("%s: fail to unregister client, ss_mst_bus_hdl = %d\n",
			__func__, ss_mst_bus_hdl);
	}
	return ss_mst_bus_hdl;
}
#endif

/**
 * mst_ctrl_of_mst_hw_onoff - function to disable/enable MST when NFC on
 * @on: on/off value
 */
extern void mst_ctrl_of_mst_hw_onoff(bool on)
{
#if defined(CONFIG_MFC_CHARGER)
	union power_supply_propval value;	/* power_supply prop */
#endif
#if defined(CONFIG_MST_SUPPORT_GPIO)
	uint8_t is_mst_support = 0;

	is_mst_support = gpio_get_value(mst_support_check);
	if (is_mst_support == 0) {
		mst_info("%s: MST not supported, no need nfc control, %d\n",
			 __func__, is_mst_support);
		return;
	}
#endif
	mst_info("%s: on = %d\n", on);

	if (on) {
		nfc_state = 0;
		mst_info("%s: nfc_state unlocked, %d\n", __func__, nfc_state);
	} else {
#if defined(CONFIG_MST_REGULATOR)
		if (regulator3_0) {
			if (regulator_is_enabled(regulator3_0)) {
				regulator_disable(regulator3_0);
				mst_info("%s: regulator 3.0 is disabled\n",
					 __func__);
			}
		}
#else
#if !defined(CONFIG_MST_IF_PMIC)
		gpio_set_value(mst_pwr_en, 0);
		mst_info("%s: mst_pwr_en LOW\n", __func__);
#endif
#endif
		usleep_range(800, 1000);

#if defined(CONFIG_MFC_CHARGER)
		value.intval = OFF;
		psy_do_property("mfc-charger", set,
				POWER_SUPPLY_PROP_TECHNOLOGY, value);
		mst_info("%s: MST_MODE notify : %d\n", __func__, value.intval);

                value.intval = 0;
                psy_do_property("mfc-charger", set, POWER_SUPPLY_EXT_PROP_WPC_EN_MST, value);
                mst_info("%s : MFC_IC Disable notify : %d\n", __func__, value.intval);
#endif
#if defined(CONFIG_ARCH_QCOM)
		/* Boost Disable */
		mst_info("%s: boost disable", __func__);
		mutex_lock(&mst_mutex);
		if (boost_disable())
			mst_err("%s: boost disable failed\n",__func__);

#endif
#if defined(CONFIG_MST_LPM_CONTROL)
		/* PCIe LPM Enable */
		mst_info("%s: l1ss enable\n", __func__);
#if defined(CONFIG_PCI_EXYNOS)
		exynos_pcie_l1ss_ctrl(1, PCIE_L1SS_CTRL_MST);
#elif defined(CONFIG_ARCH_QCOM)
		sec_pcie_l1ss_enable(L1SS_MST);
#endif
#endif
#if defined(CONFIG_MST_LPM_DISABLE)
		/* CPU LPM Enable*/
		if (mst_lpm_tag) {
			mst_info("%s: pm_qos remove\n", __func__);
			pm_qos_remove_request(&mst_pm_qos_request);
			mst_info("%s: core online lock disable\n", __func__);
			core_ctl_set_boost(false);
			mst_lpm_tag = 0;
		}
#endif
#if defined(CONFIG_ARCH_QCOM)
		mutex_unlock(&mst_mutex);
#endif
		nfc_state = 1;
		mst_info("%s: nfc_state locked, %d\n", __func__, nfc_state);
	}
}

/**
 * of_mst_hw_onoff - Enable/Disable MST LDO GPIO pin (or Regulator)
 * @on: on/off value
 */
static void of_mst_hw_onoff(bool on)
{
#if defined(CONFIG_MFC_CHARGER)
	union power_supply_propval value;	/* power_supply prop */
	int retry_cnt = 8;
#endif
	mst_info("%s: on = %d\n", __func__, on);

#if defined(CONFIG_MST_REGULATOR)
	if (regulator3_0 == NULL) {
		mst_err("%s: regulator_get retry\n", __func__);
		regulator3_0 = regulator_get(NULL, "pm6150l_l7");
		if(regulator3_0 == NULL)
			mst_err("%s: regulator get failed\n", regulator3_0);
	}
#endif
	if (nfc_state == 1) {
		mst_info("%s: nfc_state on!!!\n", __func__);
		return;
	}

	if (on) {
#if defined(CONFIG_MFC_CHARGER)
                mst_info("%s : MFC_IC Enable notify start\n", __func__);
                value.intval = 1;
                psy_do_property("mfc-charger", set, POWER_SUPPLY_EXT_PROP_WPC_EN_MST, value);
                mst_info("%s : MFC_IC Enable notified : %d\n", __func__, value.intval);

		value.intval = ON;
		psy_do_property("mfc-charger", set,
				POWER_SUPPLY_PROP_TECHNOLOGY, value);
		mst_info("%s: MST_MODE notify : %d\n", __func__, value.intval);
#endif

#if defined(CONFIG_MST_REGULATOR)
		if (regulator3_0) {
			regulator_enable(regulator3_0);
			mst_info("%s: regulator 3.0 is enabled\n", __func__);
		}
#else
#if !defined(CONFIG_MST_IF_PMIC)
		gpio_set_value(mst_pwr_en, 1);
		usleep_range(3600, 4000);
		gpio_set_value(mst_pwr_en, 0);
		mdelay(50);

		gpio_set_value(mst_pwr_en, 1);
		printk("%s : mst_pwr_en gets the HIGH\n", __func__);
#endif
#endif

		cancel_delayed_work_sync(&dwork);
		queue_delayed_work(cluster_freq_ctrl_wq, &dwork, 90 * HZ);

#if defined(CONFIG_ARCH_QCOM)
		/* Boost Enable */
		mst_info("%s: boost enable for performacne", __func__);
		mutex_lock(&mst_mutex);
		if (!boost_enable())
			mst_err("%s: boost enable is failed\n", __func__);
#endif

#if defined(CONFIG_MST_LPM_CONTROL)
		/* PCIe LPM Disable */
		mst_info("%s: l1ss disable\n", __func__);
#if defined(CONFIG_PCI_EXYNOS)
		exynos_pcie_l1ss_ctrl(0, PCIE_L1SS_CTRL_MST);
#elif defined(CONFIG_ARCH_QCOM)
		sec_pcie_l1ss_disable(L1SS_MST);
#endif
#endif

#if defined(CONFIG_MST_LPM_DISABLE)
		/* CPU LPM Disable */
		if (mst_lpm_tag == 0) {
			mst_info("%s: pm_qos add\n", __func__);
			pm_qos_add_request(&mst_pm_qos_request,
					   PM_QOS_CPU_DMA_LATENCY,
					   PM_QOS_DEFAULT_VALUE);
			pm_qos_update_request(&mst_pm_qos_request, 1);
			mst_info("%s: core online lock enable\n", __func__);
			core_ctl_set_boost(true);
			mst_lpm_tag = 1;
		}
#endif

#if defined(CONFIG_ARCH_QCOM)
		mutex_unlock(&mst_mutex);
#endif
		mdelay(40);

#if defined(CONFIG_MFC_CHARGER)
		while (--retry_cnt) {
			psy_do_property("mfc-charger", get,
					POWER_SUPPLY_PROP_TECHNOLOGY, value);
			if (value.intval == 0x02) {
				mst_info("%s: mst mode set!!! : %d\n", __func__,
					 value.intval);
				retry_cnt = 1;
				break;
			}
			usleep_range(3600, 4000);
		}

		if (!retry_cnt) {
			mst_info("%s: timeout !!! : %d\n", __func__,
				 value.intval);
		}
#endif
	} else {
#if defined(CONFIG_MST_REGULATOR)
		if (regulator3_0) {
			regulator_disable(regulator3_0);
			mst_info("%s: regulator 3.0 is disabled\n", __func__);
		}
#else
#if !defined(CONFIG_MST_IF_PMIC)
		gpio_set_value(mst_pwr_en, 0);
		mst_info("%s: mst_pwr_en LOW\n", __func__);
#endif
#endif
		usleep_range(800, 1000);

#if defined(CONFIG_MFC_CHARGER)
		value.intval = OFF;
		psy_do_property("mfc-charger", set,
				POWER_SUPPLY_PROP_TECHNOLOGY, value);
		mst_info("%s: MST_MODE notify : %d\n", __func__,
			 value.intval);

                value.intval = 0;
                psy_do_property("mfc-charger", set, POWER_SUPPLY_EXT_PROP_WPC_EN_MST, value);
                printk("%s : MFC_IC Disable notify : %d\n", __func__, value.intval);
#endif

#if defined(CONFIG_ARCH_QCOM)
		/* Boost Disable */
		mst_info("%s: boost disable", __func__);
		mutex_lock(&mst_mutex);
		if (boost_disable())
			mst_err("%s: boost disable is failed\n", __func__);
#endif

#if defined(CONFIG_MST_LPM_CONTROL)
		/* PCIe LPM Enable */
		mst_info("%s: l1ss enable\n", __func__);
#if defined(CONFIG_PCI_EXYNOS)
		exynos_pcie_l1ss_ctrl(1, PCIE_L1SS_CTRL_MST);
#elif defined(CONFIG_ARCH_QCOM)
		sec_pcie_l1ss_enable(L1SS_MST);
#endif
#endif

#if defined(CONFIG_MST_LPM_DISABLE)
		/* CPU LPM Enable*/
		if (mst_lpm_tag) {
			mst_info("%s: pm_qos remove\n", __func__);
			pm_qos_remove_request(&mst_pm_qos_request);
			mst_info("%s: core online lock disable\n", __func__);
			core_ctl_set_boost(false);
			mst_lpm_tag = 0;
		}
#endif

#if defined(CONFIG_ARCH_QCOM)
		mutex_unlock(&mst_mutex);
#endif
	}
}

#if defined(CONFIG_MST_TEEGRIS)
uint32_t transmit_mst_teegris(uint32_t cmd)
{
	TEEC_Context context;
	TEEC_Session session_ta;
	TEEC_Operation operation;
	TEEC_Result ret;

	uint32_t origin;

	origin = 0x0;
	operation.paramTypes = TEEC_PARAM_TYPES(TEEC_NONE, TEEC_NONE,
						TEEC_NONE, TEEC_NONE);

	mst_info("%s: TEEC_InitializeContext\n", __func__);
	ret = TEEC_InitializeContext(NULL, &context);
	if (ret != TEEC_SUCCESS) {
		mst_err("%s: InitializeContext failed, %d\n", __func__, ret);
		goto exit;
	}

	mst_info("%s: TEEC_OpenSession\n", __func__);
	ret = TEEC_OpenSession(&context, &session_ta, &uuid_ta, 0,
			       NULL, &operation, &origin);
	if (ret != TEEC_SUCCESS) {
		mst_err("%s: OpenSession(ta) failed, %d\n", __func__, ret);
		goto finalize_context;
	}

	mst_info("%s: TEEC_InvokeCommand (CMD_OPEN)\n", __func__);
	ret = TEEC_InvokeCommand(&session_ta, CMD_OPEN, &operation, &origin);
	if (ret != TEEC_SUCCESS) {
		mst_err("%s: InvokeCommand(OPEN) failed, %d\n", __func__, ret);
		goto ta_close_session;
	}

	/* MST IOCTL - transmit track data */
	trace_printk("tracing mark write: MST transmission Start\n");
	mst_info("%s: TEEC_InvokeCommand (TRACK1 or TRACK2)\n", __func__);
	ret = TEEC_InvokeCommand(&session_ta, cmd, &operation, &origin);
	if (ret != TEEC_SUCCESS) {
		mst_err("%s: InvokeCommand failed, %d\n", __func__, ret);
		goto ta_close_session;
	}
	trace_printk("tracing mark write: MST transmission End\n");

	if (ret) {
		mst_info("%s: Send track%d data --> failed\n", __func__, cmd-2);
	} else {
		mst_info("%s: Send track%d data --> success\n", __func__, cmd-2);
	}

	mst_info("%s: TEEC_InvokeCommand (CMD_CLOSE)\n", __func__);
	ret = TEEC_InvokeCommand(&session_ta, CMD_CLOSE, &operation, &origin);
	if (ret != TEEC_SUCCESS) {
		mst_err("%s: InvokeCommand(CLOSE) failed, %d\n", __func__, ret);
		goto ta_close_session;
	}

ta_close_session:
	TEEC_CloseSession(&session_ta);
finalize_context:
	TEEC_FinalizeContext(&context);
exit:
	return ret;
}
#endif

#if defined(CONFIG_ARCH_QCOM)
/**
 * transmit_mst_data - Transmit test track data
 * @track: 1:track1, 2:track2
 */
static int transmit_mst_data(int track)
{
	int ret = 0;
	int qsee_ret = 0;
	char app_name[MAX_APP_NAME_SIZE];
	mst_req_t *kreq = NULL;
	mst_rsp_t *krsp = NULL;
	int req_len = 0, rsp_len = 0;

	mutex_lock(&mst_mutex);
	snprintf(app_name, MAX_APP_NAME_SIZE, "%s", MST_TA);
	if (NULL == qhandle) {
		/* start the mst tzapp only when it is not loaded. */
		qsee_ret = qseecom_start_app(&qhandle, app_name, 1024);
		if (qsee_ret) {
			/* It seems we couldn't start mst tzapp. */
			mst_err("%s: failed to qseecom_start_app, %d\n",
				__func__, qsee_ret);
			ret = ERROR_VALUE;
			goto exit;	/* leave the function now. */
		}
	}

	kreq = (struct mst_req_s *)qhandle->sbuf;

	switch (track) {
	case TRACK1:
		kreq->cmd_id = MST_CMD_TRACK1_TEST;
		break;

	case TRACK2:
		kreq->cmd_id = MST_CMD_TRACK2_TEST;
		break;

	default:
		ret = ERROR_VALUE;
		goto exit;
		break;
	}

	req_len = sizeof(mst_req_t);
	krsp = (struct mst_rsp_s *)(qhandle->sbuf + req_len);
	rsp_len = sizeof(mst_rsp_t);

	mst_info("%s: cmd_id = %x, req_len = %d, rsp_len = %d\n", __func__,
		 kreq->cmd_id, req_len, rsp_len);

	trace_printk("tracing mark write: MST transmission Start\n");
	qsee_ret = qseecom_send_command(qhandle, kreq, req_len, krsp, rsp_len);
	trace_printk("tracing mark write: MST transmission End\n");
	if (qsee_ret) {
		ret = ERROR_VALUE;
		mst_err("%s: failed to send cmd, %d\n", __func__, qsee_ret);
	}

	if (krsp->status) {
		ret = ERROR_VALUE;
		mst_err("%s: generate track data from TZ failed, %d\n",
			__func__, krsp->status);
	}

	if (ret) {
		mst_info("%s: Send track%d data --> failed\n", __func__, track);
	} else {
		mst_info("%s: Send track%d data --> success\n", __func__, track);
	}

	mst_info("%s: shutting down the tzapp\n", __func__);
	qsee_ret = qseecom_shutdown_app(&qhandle);
	if (qsee_ret) {
		mst_err("%s: failed to shut down the tzapp\n", __func__);
	} else {
		qhandle = NULL;
	}
exit:
	mutex_unlock(&mst_mutex);
	return ret;
}
#endif

/**
 * show_mst_drv - device attribute show sysfs operation
 */
static ssize_t show_mst_drv(struct device *dev,
			    struct device_attribute *attr, char *buf)
{
	if (!dev)
		return -ENODEV;

	if (escape_loop == 0) {
		return sprintf(buf, "%s\n", "activating");
	} else {
		return sprintf(buf, "%s\n", "waiting");
	}
}

/**
 * store_mst_drv - device attribute store sysfs operation
 */
static ssize_t store_mst_drv(struct device *dev,
			     struct device_attribute *attr, const char *buf,
			     size_t count)
{
	char in = 0;
	int ret = 0;
#if defined(CONFIG_MFC_CHARGER)
	struct device_node *np;
	enum of_gpio_flags irq_gpio_flags;
#endif

	sscanf(buf, "%c\n", &in);
	mst_info("%s: in = %c\n", __func__, in);

#if defined(CONFIG_MFC_CHARGER)
	if (wpc_det < 0) {
		np = of_find_node_by_name(NULL, "battery");
		if (!np) {
			mst_err("%s: np NULL\n", __func__);
		} else {
			/* wpc_det */
			wpc_det = of_get_named_gpio_flags(np, "battery,wpc_det",
							  0, &irq_gpio_flags);
			if (wpc_det < 0) {
				mst_err("%s: can't get wpc_det = %d\n",
					__func__, wpc_det);
			}
		}
	}

	if (wpc_det && (gpio_get_value(wpc_det) == 1)) {
		mst_info("%s: Wireless charging, no proceed MST\n", __func__);
		return count;
	}
#endif

	switch (in) {
	case CMD_MST_LDO_OFF:
		of_mst_hw_onoff(OFF);
		break;

	case CMD_MST_LDO_ON:
		of_mst_hw_onoff(ON);
		break;

	case CMD_SEND_TRACK1_DATA:
#if !defined(CONFIG_MFC_CHARGER)
		of_mst_hw_onoff(ON);
#endif
		mst_info("%s: send track1 data\n", __func__);
#if defined(CONFIG_MST_NONSECURE) || defined(CONFIG_ARCH_QCOM)
		ret = transmit_mst_data(TRACK1);
#else
#if defined(CONFIG_MST_TEEGRIS)
		ret = transmit_mst_teegris(CMD_TRACK1);
#else
		ret = exynos_smc((0x8300000f), TRACK1, 0, 0);
#endif
#endif
#if !defined(CONFIG_MFC_CHARGER)
		of_mst_hw_onoff(0);
#endif
		break;

	case CMD_SEND_TRACK2_DATA:
#if !defined(CONFIG_MFC_CHARGER)
		of_mst_hw_onoff(1);
#endif
		mst_info("%s: send track2 data\n", __func__);
#if defined(CONFIG_MST_NONSECURE) || defined(CONFIG_ARCH_QCOM)
		ret = transmit_mst_data(TRACK2);
#else
#if defined(CONFIG_MST_TEEGRIS)
		ret = transmit_mst_teegris(CMD_TRACK2);
#else
		ret = exynos_smc((0x8300000f), TRACK2, 0, 0);
#endif
#endif
#if !defined(CONFIG_MFC_CHARGER)
		of_mst_hw_onoff(0);
#endif
		break;

	case CMD_HW_RELIABILITY_TEST_START:
		if (escape_loop) {
			wake_lock_init(&mst_wakelock, WAKE_LOCK_SUSPEND,
				       "mst_wakelock");
			wake_lock(&mst_wakelock);
		}
		escape_loop = 0;
		while (1) {
			if (escape_loop == 1)
				break;
#if !defined(CONFIG_MFC_CHARGER)
			of_mst_hw_onoff(1);
#endif
			mdelay(10);
			mst_info("%s: Continuous track2 data\n", __func__);
#if defined(CONFIG_MST_NONSECURE) || defined(CONFIG_ARCH_QCOM)
			ret = transmit_mst_data(TRACK2);
#else
#if defined(CONFIG_MST_TEEGRIS)
			ret = transmit_mst_teegris(CMD_TRACK2);
#else
			ret = exynos_smc((0x8300000f), TRACK2, 0, 0);
#endif
#endif
#if !defined(CONFIG_MFC_CHARGER)
			of_mst_hw_onoff(0);
#endif
			mdelay(1000);
		}
		break;

	case CMD_HW_RELIABILITY_TEST_STOP:
		if (!escape_loop)
			wake_lock_destroy(&mst_wakelock);
		escape_loop = 1;
		mst_info("%s: escape_loop value = %d\n", __func__, escape_loop);
		break;

	default:
		mst_err("%s: invalid value : %c\n", __func__, in);
		break;
	}
	return count;
}
static DEVICE_ATTR(transmit, 0770, show_mst_drv, store_mst_drv);

/* support node */
static ssize_t show_support(struct device *dev,
			    struct device_attribute *attr, char *buf)
{
#if defined(CONFIG_MST_SUPPORT_GPIO)
	uint8_t is_mst_support = 0;
#endif

	if (!dev)
		return -ENODEV;

#if defined(CONFIG_MST_SUPPORT_GPIO)
	is_mst_support = gpio_get_value(mst_support_check);
	if (is_mst_support == 1) {
		mst_info("%s: Support MST!, %d\n", __func__, is_mst_support);
		return sprintf(buf, "%d\n", 1);
	} else {
		mst_info("%s: Not support MST!, %d\n", __func__, is_mst_support);
		return sprintf(buf, "%d\n", 0);
	}
#else
	mst_info("%s: MST_LDO enabled, support MST\n", __func__);
	return sprintf(buf, "%d\n", 1);
#endif
}

static ssize_t store_support(struct device *dev,
			     struct device_attribute *attr, const char *buf,
			     size_t count)
{
	return count;
}
static DEVICE_ATTR(support, 0444, show_support, store_support);

/* mfc_charger node */
#if defined(CONFIG_MFC_CHARGER)
static ssize_t show_mfc(struct device *dev,
			struct device_attribute *attr, char *buf)
{
	if (!dev)
		return -ENODEV;

	return sprintf(buf, "%s\n", "mfc_charger");
}

static ssize_t store_mfc(struct device *dev,
			 struct device_attribute *attr, const char *buf,
			 size_t count)
{
	return count;
}
static DEVICE_ATTR(mfc, 0770, show_mfc, store_mfc);
#endif

/**
 * sec_mst_gpio_init - Initialize GPIO pins used by driver
 * @dev: driver handle
 */
static int sec_mst_gpio_init(struct device *dev)
{
#if defined(CONFIG_MST_NONSECURE) || (!defined(CONFIG_MST_IF_PMIC) && !defined(CONFIG_MST_REGULATOR))
	int ret = 0;
#endif
#if defined(CONFIG_MFC_CHARGER)
	struct device_node *np;
	enum of_gpio_flags irq_gpio_flags;

	/* get wireless chraging check gpio */
	np = of_find_node_by_name(NULL, "battery");
	if (!np) {
		mst_err("%s: np NULL\n", __func__);
	} else {
		/* wpc_det */
		wpc_det = of_get_named_gpio_flags(np, "battery,wpc_det",
						  0, &irq_gpio_flags);
		if (wpc_det < 0) {
			mst_err("%s: can't get wpc_det = %d\n",
				__func__, wpc_det);
		}
	}
#endif

#if defined(CONFIG_MST_REGULATOR)
	regulator3_0 = regulator_get(NULL, "pm6150l_l7");
	if (regulator3_0 == NULL) {
		mst_err("%s: regulator3_0 invalid(NULL)\n", __func__);
	}
#else
#if !defined(CONFIG_MST_IF_PMIC)
	mst_pwr_en = of_get_named_gpio(dev->of_node, "sec-mst,mst-pwr-gpio", 0);

	mst_info("%s: Data Value : %d\n", __func__, mst_pwr_en);

	/* check if gpio pin is inited */
	if (mst_pwr_en < 0) {
		mst_err("%s: fail to get pwr gpio, %d\n", __func__, mst_pwr_en);
		return 1;
	}
	mst_info("%s: gpio pwr inited\n", __func__);

	/* gpio request */
	ret = gpio_request(mst_pwr_en, "sec-mst,mst-pwr-gpio");
	if (ret) {
		mst_err("%s: failed to request pwr gpio, %d, %d\n",
			__func__, ret, mst_pwr_en);
	}

	/* set gpio direction */
	if (!(ret < 0) && (mst_pwr_en > 0)) {
		gpio_direction_output(mst_pwr_en, 0);
		mst_info("%s: mst_pwr_en output\n", __func__);
	}
#endif
#endif

#if defined(CONFIG_MST_NONSECURE)
	mst_en = of_get_named_gpio(dev->of_node, "sec-mst,mst-en-gpio", 0);
	mst_data = of_get_named_gpio(dev->of_node, "sec-mst,mst-data-gpio", 0);
	mst_info("%s: Data Value mst_en : %d, mst_data : %d\n",
		 __func__, mst_en, mst_data);

	/* check if gpio pin is available */
	if (mst_en < 0) {
		mst_err("%s: fail to get en gpio, %d\n", __func__, mst_en);
		return 1;
	}
	mst_info("%s: gpio en inited\n", __func__);

	if (mst_data < 0) {
		mst_err("%s: fail to get data gpio, %d\n", __func__, mst_data);
		return 1;
	}
	mst_info("%s: gpio data inited\n", __func__);

	/* gpio request */
	ret = gpio_request(mst_en, "sec-mst,mst-en-gpio");
	if (ret) {
		mst_err("%s: failed to request en gpio, %d, %d\n",
			__func__, ret, mst_en);
	}

	ret = gpio_request(mst_data, "sec-mst,mst-data-gpio");
	if (ret) {
		mst_err("%s: failed to request data gpio, %d, %d\n",
			__func__, ret, mst_data);
	}

	/* set gpio direction */
	if (!(ret < 0) && (mst_en > 0)) {
		gpio_direction_output(mst_en, 0);
		mst_info("%s: mst_en output\n", __func__);
	}
	if (!(ret < 0)  && (mst_data > 0)) {
		gpio_direction_output(mst_data, 0);
		mst_info("%s: mst_data output\n", __func__);
	}
#endif
	return 0;
}

static int mst_ldo_device_probe(struct platform_device *pdev)
{
	int retval = 0;
#if defined(CONFIG_MST_SUPPORT_GPIO)
	struct device *dev = &pdev->dev;
	uint8_t is_mst_support = 0;
#endif

	mst_info("%s: probe start\n", __func__);

#if defined(CONFIG_MST_SUPPORT_GPIO)
	/* MST support/non-support node check gpio */
	mst_support_check = of_get_named_gpio(dev->of_node, "sec-mst,mst-support-gpio", 0);
	mst_info("%s: mst_support_check, %d\n", __func__, mst_support_check);
	if (mst_support_check < 0) {
		mst_err("%s: fail to get support gpio, %d\n",
			__func__, mst_support_check);
		return -1;
	}
	mst_info("%s: gpio support_check inited\n", __func__);

	is_mst_support = gpio_get_value(mst_support_check);
	if (is_mst_support == 1) {
		mst_info("%s: Support MST!, %d\n", __func__, is_mst_support);
	} else {
		mst_info("%s: Not support MST!, %d\n", __func__, is_mst_support);

		mst_info("%s: create sysfs node\n", __func__);
		mst_drv_class = class_create(THIS_MODULE, "mstldo");
		if (IS_ERR(mst_drv_class)) {
			retval = PTR_ERR(mst_drv_class);
			goto error;
		}

		mst_drv_dev = device_create(mst_drv_class,
					    NULL /* parent */, 0 /* dev_t */,
					    NULL /* drvdata */,
					    MST_DRV_DEV);
		if (IS_ERR(mst_drv_dev)) {
			retval = PTR_ERR(mst_drv_dev);
			goto error_destroy;
		}

		retval = device_create_file(mst_drv_dev, &dev_attr_support);
		if (retval)
			goto error_destroy;

		return -1;
	}
#endif
#if defined(CONFIG_MST_NONSECURE)
	spin_lock_init(&event_lock);
#endif

	if (sec_mst_gpio_init(&pdev->dev))
		return -1;

	mst_info("%s: create sysfs node\n", __func__);
	mst_drv_class = class_create(THIS_MODULE, "mstldo");
	if (IS_ERR(mst_drv_class)) {
		retval = PTR_ERR(mst_drv_class);
		goto error;
	}

	mst_drv_dev = device_create(mst_drv_class,
				    NULL /* parent */, 0 /* dev_t */,
				    NULL /* drvdata */,
				    MST_DRV_DEV);
	if (IS_ERR(mst_drv_dev)) {
		retval = PTR_ERR(mst_drv_dev);
		goto error_destroy;
	}

	/* register this mst device with the driver core */
	retval = device_create_file(mst_drv_dev, &dev_attr_transmit);
	if (retval)
		goto error_destroy;

	retval = device_create_file(mst_drv_dev, &dev_attr_support);
	if (retval)
		goto error_destroy;

#if defined(CONFIG_MFC_CHARGER)
	retval = device_create_file(mst_drv_dev, &dev_attr_mfc);
	if (retval)
		goto error_destroy;
#endif

	mst_info("%s: MST driver(%s) initialized\n", __func__, MST_DRV_DEV);
	return 0;

error_destroy:
	kfree(mst_drv_dev);
	device_destroy(mst_drv_class, 0);
error:
	mst_info("%s: MST driver(%s) init failed\n", __func__, MST_DRV_DEV);
	return retval;
}
EXPORT_SYMBOL_GPL(mst_drv_dev);

/**
 * device suspend - function called device goes suspend
 */
static int mst_ldo_device_suspend(struct platform_device *dev,
				  pm_message_t state)
{
	uint8_t is_mst_pwr_on;
#if defined(CONFIG_MST_REGULATOR)
	if (regulator3_0) {
		is_mst_pwr_on = regulator_is_enabled(regulator3_0);
		if (is_mst_pwr_on > 0) {
			mst_info("%s: mst regulator is on, %d\n",
				 __func__, is_mst_pwr_on);
			of_mst_hw_onoff(OFF);
			mst_info("%s: mst regulator off\n", __func__);
		} else {
			mst_info("%s: mst regulator is off, %d\n",
				 __func__, is_mst_pwr_on);
		}
	}
#else
#if !defined(CONFIG_MST_IF_PMIC)
	is_mst_pwr_on = gpio_get_value(mst_pwr_en);
	if (is_mst_pwr_on == 1) {
		mst_info("%s: mst power is on, %d\n", __func__, is_mst_pwr_on);
		of_mst_hw_onoff(0);
		mst_info("%s: mst power off\n", __func__);
	} else {
		mst_info("%s: mst power is off, %d\n", __func__, is_mst_pwr_on);
	}
#endif
#endif
	return 0;
}

static struct of_device_id mst_match_ldo_table[] = {
	{.compatible = "sec-mst",},
	{},
};

static struct platform_driver sec_mst_ldo_driver = {
	.driver = {
		.owner = THIS_MODULE,
		.name = "mstldo",
		.of_match_table = mst_match_ldo_table,
	},
	.probe = mst_ldo_device_probe,
	.suspend = mst_ldo_device_suspend,
};

/**
 * mst delayed work
 */
static void mst_cluster_freq_ctrl_worker(struct work_struct *work)
{

	uint8_t is_mst_pwr_on;
#if defined(CONFIG_MST_REGULATOR)
	if (regulator3_0) {
		is_mst_pwr_on = regulator_is_enabled(regulator3_0);
		if (is_mst_pwr_on > 0) {
			mst_info("%s: mst regulator is on, %d\n",
				 __func__, is_mst_pwr_on);
			of_mst_hw_onoff(OFF);
			mst_info("%s: mst regulator off\n", __func__);
		} else {
			mst_info("%s: mst regulator is off, %d\n",
				 __func__, is_mst_pwr_on);
		}
	}
#else
#if !defined(CONFIG_MST_IF_PMIC)
	is_mst_pwr_on = gpio_get_value(mst_pwr_en);
	if (is_mst_pwr_on == 1) {
		mst_info("%s: mst power is on, %d\n", __func__, is_mst_pwr_on);
		of_mst_hw_onoff(0);
		mst_info("%s: mst power off\n", __func__);
	} else {
		mst_info("%s: mst power is off, %d\n", __func__, is_mst_pwr_on);
	}
#endif
#endif
	return;
}

/**
 * mst_drv_init - Driver init function
 */
static int __init mst_drv_init(void)
{
	int ret = 0;

	mst_info("%s\n", __func__);
	ret = platform_driver_register(&sec_mst_ldo_driver);
	mst_info("%s: init , ret : %d\n", __func__, ret);

	cluster_freq_ctrl_wq =
		create_singlethread_workqueue("mst_cluster_freq_ctrl_wq");
	INIT_DELAYED_WORK(&dwork, mst_cluster_freq_ctrl_worker);

	return ret;
}

/**
 * mst_drv_exit - Driver exit function
 */
static void __exit mst_drv_exit(void)
{
	class_destroy(mst_drv_class);
#if defined(CONFIG_MST_REGULATOR)
	if (regulator3_0)
		regulator_put(regulator3_0);
#endif
	mst_info("%s\n", __func__);
}

MODULE_AUTHOR("yurak.choe@samsung.com");
MODULE_DESCRIPTION("MST QC/LSI combined driver");
MODULE_VERSION("1.0");
late_initcall(mst_drv_init);
module_exit(mst_drv_exit);
