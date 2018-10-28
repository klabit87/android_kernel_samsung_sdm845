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

#include <linux/device.h>
#include <linux/kernel.h>
#include <linux/kobject.h>
#include <linux/module.h>
#include <linux/slab.h>
#include <linux/syscalls.h>
#include <linux/gpio.h>
#include <linux/of_gpio.h>
#include <linux/platform_device.h>
#include <linux/wakelock.h>
#include <linux/delay.h>
#include <linux/qseecom.h>
#include <linux/regulator/machine.h>
#include <linux/pinctrl/consumer.h>
#include <linux/err.h>
#include "mstdrv.h"
#include <linux/power_supply.h>
#include <linux/msm_pcie.h>
#include <linux/threads.h>
#include <linux/cpumask.h>
#include <linux/percpu-defs.h>
#include <linux/cpu.h>
#include <linux/cpumask.h>
#include <linux/cpufreq.h>
#include <../../kernel/sched/sched.h>
#include <linux/workqueue.h>
#include <linux/msm-bus.h> 

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
#define TEST_RESULT_LEN			256	// result array length
#define MST_LDO_3P0				"VDD_MST"	// MST LDO regulator
#define MST_MODE_ON			1	// ON Message to MFC ic
#define MST_MODE_OFF		0	// OFF Message to MFC ic
#define SVC_MST_ID			0x000A0000	// need to check ID
#define MST_CREATE_CMD(x)		(SVC_MST_ID | x)	// Create MST commands
#define MST_TA				"mst"
#define MAX_CLUSTERS 2

#define psy_do_property(name, function, property, value) \
{    \
	struct power_supply *psy;    \
	int ret;    \
	psy = get_power_supply_by_name((name));    \
	if (!psy) {    \
		pr_err("%s: Fail to "#function" psy (%s)\n",    \
				__func__, (name));    \
		value.intval = 0;    \
	} else {    \
		if (psy->desc->function##_property != NULL) { \
			ret = psy->desc->function##_property(psy, (property), &(value)); \
			if (ret < 0) {    \
				pr_err("%s: Fail to %s "#function" (%d=>%d)\n", \
						__func__, name, (property), ret);    \
				value.intval = 0;    \
			}    \
		}    \
	}    \
}

/* enum definitions */
typedef enum {
	MST_CMD_TRACK1_TEST = MST_CREATE_CMD(0x00000000),
	MST_CMD_TRACK2_TEST = MST_CREATE_CMD(0x00000001),
	MST_CMD_UNKNOWN = MST_CREATE_CMD(0x7FFFFFFF)
} mst_cmd_type;

/* struct definitions */
struct qseecom_handle {
	void *dev;		/* in/out */
	unsigned char *sbuf;	/* in/out */
	uint32_t sbuf_len;	/* in/out */
};

typedef struct mst_req_s {
	mst_cmd_type cmd_id;
	uint32_t data;
} __attribute__ ((packed)) mst_req_t;

typedef struct mst_rsp_s {
	uint32_t data;
	uint32_t status;
} __attribute__ ((packed)) mst_rsp_t;

static struct msm_bus_paths ss_mst_usecases[] = { 
    { 
	.vectors = (struct msm_bus_vectors[]){ 
	    { 
		.src = 1, 
		.dst = 512, 
		.ab = 0, 
		.ib = 0, 
	    }, 
	}, 
	.num_paths = 1, 
    }, 
    { 
	.vectors = (struct msm_bus_vectors[]){ 
	    { 
		.src = 1, 
		.dst = 512, 
		.ab = 4068000000, 
		.ib = 4068000000, 
	    }, 
	}, 
	.num_paths = 1, 
    }, 
}; 

static struct msm_bus_scale_pdata ss_mst_bus_client_pdata = { 
    .usecase = ss_mst_usecases, 
    .num_usecases = ARRAY_SIZE(ss_mst_usecases), 
    .name = "ss_mst", 
}; 

uint32_t ss_mst_bus_hdl;

/* extern function declarations */
extern int qseecom_start_app(struct qseecom_handle **handle, char *app_name,
			     uint32_t size);
extern int qseecom_shutdown_app(struct qseecom_handle **handle);
extern int qseecom_send_command(struct qseecom_handle *handle, void *send_buf,
				uint32_t sbuf_len, void *resp_buf,
				uint32_t rbuf_len);

/* function declarations */
static ssize_t show_mst_drv(struct device *dev,
			    struct device_attribute *attr, char *buf);
static ssize_t store_mst_drv(struct device *dev,
			     struct device_attribute *attr, const char *buf,
			     size_t count);
static int mst_ldo_device_probe(struct platform_device *pdev);
static inline struct power_supply *get_power_supply_by_name(char *name)
{
	if (!name)
		return (struct power_supply *)NULL;
	else
		return power_supply_get_by_name(name);
}

static void of_mst_hw_onoff(bool on);
static int boost_enable(void);
static int boost_disable(void);

/* global variables */
static int mst_pwr_en;	// MST_PWR_EN pin
static int escape_loop = 1;	// for HW reliability test
static struct class *mst_drv_class;	// mst_drv class
struct device *mst_drv_dev;	// mst_drv driver handle
static struct wake_lock mst_wakelock;	// wake_lock used for HW reliability test
static DEVICE_ATTR(transmit, 0770, show_mst_drv, store_mst_drv);	// define device attribute
static bool mst_power_on;	// to track current level of mst signal
static struct qseecom_handle *qhandle;
static int nfc_state;
static int wpc_det;

/* cpu freq control variables */
extern int num_clusters;
extern struct sched_cluster *sched_cluster[NR_CPUS];
unsigned int cluster_freq[MAX_CLUSTERS];
struct workqueue_struct *cluster_freq_ctrl_wq;
struct delayed_work dwork;

DEFINE_MUTEX(mst_mutex);

/* device driver structures */
static struct of_device_id mst_match_ldo_table[] = {
	{.compatible = "sec-mst",},
	{},
};

static int mst_ldo_device_suspend(struct platform_device *dev, pm_message_t state)
{
	int ret = 0;
	/* Boost Disable */
	printk("%s : boost disable to back to Normal", __func__);
	ret = boost_disable();
	if (ret)
	    printk("%s : boost disable is failed, ret = %d\n"
		, __func__, ret);
	/* PCIe LPM Enable */
	sec_pcie_l1ss_enable(L1SS_MST);
	
	return 0;
}

static struct platform_driver sec_mst_ldo_driver = {
	.driver = {
		   .owner = THIS_MODULE,
		   .name = "mstldo",
		   .of_match_table = mst_match_ldo_table,
		   },
	.probe = mst_ldo_device_probe,
	.suspend = mst_ldo_device_suspend,
};

EXPORT_SYMBOL_GPL(mst_drv_dev);

/**
 * boost_enable - set all CPUs freq upto Max
 * boost_disable - set all CPUs freq back to Normal
 */
static void mst_cluster_freq_ctrl_worker(struct work_struct *work)
{
	//struct delayed_work *dwork = to_delayed_work(work);
	of_mst_hw_onoff(OFF);
	return;
}

static int boost_enable(void)
{
	pr_info("boost_enable: bump up snoc clock request\n");
	if (0 == ss_mst_bus_hdl){
	    ss_mst_bus_hdl = msm_bus_scale_register_client(&ss_mst_bus_client_pdata);
	    if (ss_mst_bus_hdl) { 
		if(msm_bus_scale_client_update_request(ss_mst_bus_hdl, 1)) {
		    pr_err("[SS_MST] fail to update request!\n");
		    WARN_ON(1); 
		    msm_bus_scale_unregister_client(ss_mst_bus_hdl);
		    ss_mst_bus_hdl = 0;
		}
	    } else {
		pr_err("[SS_MST] fail to register snoc clock config! ss_mst_bus_hdl = %d\n",
			ss_mst_bus_hdl);
	    }
	}

	return ss_mst_bus_hdl;
}

static int boost_disable(void)
{
	pr_info("boost_disable: bump up snoc clock remove\n");
	if (ss_mst_bus_hdl) {
	    if(msm_bus_scale_client_update_request(ss_mst_bus_hdl, 0))
		WARN_ON(1);

	    msm_bus_scale_unregister_client(ss_mst_bus_hdl);
	    ss_mst_bus_hdl = 0;
	} else {
	    pr_err("[SS_MST] fail to unregister snoc clock config! ss_mst_bus_hdl = %d\n",
			ss_mst_bus_hdl);
	}

	return ss_mst_bus_hdl;
}

/**
 * mst_ctrl_of_mst_hw_onoff - function to disable/enable MST whenever samsung pay app wants
 * @on: on/off value
 */
extern void mst_ctrl_of_mst_hw_onoff(bool on)
{
	union power_supply_propval value;	/* power_supply prop */
	struct regulator *regulator3_0;
	int ret = 0;

	regulator3_0 = regulator_get(NULL, MST_LDO_3P0);
	if (IS_ERR(regulator3_0)) {
		printk("[MST] %s : regulator 3.0 is not available\n", __func__);
		return;
	}

	if (regulator3_0 == NULL) {
		printk(KERN_ERR "[MST] %s: regulator3_0 is invalid(NULL)\n",
		       __func__);
		return;
	}

	if (on) {
		printk("[MST] %s : nfc_status gets back to 0(unlock)\n",
		       __func__);
		nfc_state = 0;
	} else {
		gpio_set_value(mst_pwr_en, 0);
		printk("%s : mst_pwr_en gets the LOW\n", __func__);

		usleep_range(800, 1000);
		printk("%s : msleep(1)\n", __func__);

		value.intval = MST_MODE_OFF;
		psy_do_property("mfc-charger", set,
				POWER_SUPPLY_PROP_TECHNOLOGY, value);
		printk("%s : MST_MODE_OFF notify : %d\n", __func__,
		       value.intval);

		/* Boost Disable */
		printk("%s : boost disable to back to Normal", __func__);
		mutex_lock(&mst_mutex);
		ret = boost_disable();
		if (ret)
		    printk("%s : boost disable is failed, ret = %d\n"
			, __func__, ret);
		/* PCIe LPM Enable */
		sec_pcie_l1ss_enable(L1SS_MST);
		mutex_unlock(&mst_mutex);

		ret = regulator_disable(regulator3_0);
		if (ret < 0) {
			printk("%s : regulator 3.0 is not disabled\n",
			       __func__);
		}
		printk("%s : regulator 3.0 is disabled\n", __func__);
		printk("%s : nfc_status gets 1(lock)\n", __func__);
		nfc_state = 1;
		mst_power_on = on;
	}
	regulator_put(regulator3_0);
}

/**
 * of_mst_hw_onoff - Enable/Disable MST LDO GPIO pin (or Regulator)
 * @on: on/off value
 */
static void of_mst_hw_onoff(bool on)
{
	union power_supply_propval value;	/* power_supply prop */
	struct regulator *regulator3_0;
	int ret;
	int retry_cnt = 8;

	if (nfc_state == 1) {
		printk("[MST] %s : nfc_state is on!!!\n", __func__);
		return;
	}

	regulator3_0 = regulator_get(NULL, MST_LDO_3P0);
	if (IS_ERR(regulator3_0)) {
		printk("[MST] %s : regulator 3.0 is not available\n", __func__);
		return;
	}
	if (mst_power_on == on) {
		printk("[MST] mst-drv : mst_power_onoff : already %d\n", on);
		regulator_put(regulator3_0);
		return;
	}
	mst_power_on = on;
	if (regulator3_0 == NULL) {
		printk(KERN_ERR "[MST] %s: regulator3_0 is invalid(NULL)\n",
		       __func__);
		return;
	}
	if (on) {
		ret = regulator_enable(regulator3_0);
		if (ret < 0) {
			printk("[MST] %s : regulator 3.0 is not enable\n",
			       __func__);
		}
		printk("%s : regulator 3.0 is enabled\n", __func__);

		printk("%s : MST_MODE_ON notify start\n", __func__);
		value.intval = MST_MODE_ON;

		psy_do_property("mfc-charger", set,
				POWER_SUPPLY_PROP_TECHNOLOGY, value);
		printk("%s : MST_MODE_ON notified : %d\n", __func__,
		       value.intval);

		gpio_set_value(mst_pwr_en, 1);
		usleep_range(3600, 4000);
		gpio_set_value(mst_pwr_en, 0);
		mdelay(50);

		gpio_set_value(mst_pwr_en, 1);
		printk("%s : mst_pwr_en gets the HIGH\n", __func__);

		/* Boost Enable */
		cancel_delayed_work_sync(&dwork);
		printk("%s : boost enable for performacne", __func__);
		mutex_lock(&mst_mutex);
		ret = boost_enable();
		if (!ret)
		    printk("%s : boost enable is failed, ret = %d\n",
			    __func__, ret);
		queue_delayed_work(cluster_freq_ctrl_wq, &dwork, 90 * HZ);

		/* PCIe LPM Disable */
		sec_pcie_l1ss_disable(L1SS_MST);
		mutex_unlock(&mst_mutex);

		mdelay(40);

		while (--retry_cnt) {
			psy_do_property("mfc-charger", get,
					POWER_SUPPLY_PROP_TECHNOLOGY, value);
			//printk("%s : check mst mode status : %d\n", __func__, value.intval);
			if (value.intval == 0x02) {
				printk("%s : mst mode set!!! : %d\n", __func__,
				       value.intval);
				retry_cnt = 1;
				break;
			}
			usleep_range(3600, 4000);
		}

		if (!retry_cnt) {
			printk("%s : timeout !!! : %d\n", __func__,
			       value.intval);
		}

	} else {
		gpio_set_value(mst_pwr_en, 0);
		printk("%s : mst_pwr_en gets the LOW\n", __func__);

		usleep_range(800, 1000);
		printk("%s : msleep(1)\n", __func__);

		value.intval = MST_MODE_OFF;
		psy_do_property("mfc-charger", set,
				POWER_SUPPLY_PROP_TECHNOLOGY, value);
		printk("%s : MST_MODE_OFF notify : %d\n", __func__,
		       value.intval);

		/* Boost Disable */
		printk("%s : boost disable to back to Normal", __func__);
		mutex_lock(&mst_mutex);
		ret = boost_disable();
		if (ret)
		    printk("%s : boost disable is failed, ret = %d\n"
			, __func__, ret);
		/* PCIe LPM Enable */
		sec_pcie_l1ss_enable(L1SS_MST);
		mutex_unlock(&mst_mutex);

		ret = regulator_disable(regulator3_0);
		if (ret < 0) {
			printk("%s : regulator 3.0 is not disabled\n",
			       __func__);
		}
		printk("%s : regulator 3.0 is disabled\n", __func__);
	}
	regulator_put(regulator3_0);
}

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
	}

	if (NULL == qhandle) {
		/* qhandle is still NULL. It seems we couldn't start mst tzapp. */
		printk("[MST] cannot get tzapp handle from kernel.\n");
		ret = ERROR_VALUE;
		goto exit;	/* leave the function now. */
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

	printk("[MST] cmd_id = %x, req_len = %d, rsp_len = %d\n", kreq->cmd_id,
	       req_len, rsp_len);

	//of_mst_hw_onoff(ON);
	qsee_ret = qseecom_send_command(qhandle, kreq, req_len, krsp, rsp_len);
	//of_mst_hw_onoff(OFF);

	if (qsee_ret) {
		ret = ERROR_VALUE;
		printk("[MST] failed to send cmd to qseecom; qsee_ret = %d.\n",
		       qsee_ret);
	}

	if (krsp->status) {
		ret = ERROR_VALUE;
		printk
		    ("[MST] generate sample track data from TZ -- failed. %d\n",
		     krsp->status);
	}

	printk("[MST] shutting down the tzapp.\n");
	qsee_ret = qseecom_shutdown_app(&qhandle);
	if (qsee_ret) {
		printk("[MST] failed to shut down the tzapp.\n");
	} else {
		qhandle = NULL;
	}
exit:
	mutex_unlock(&mst_mutex);
	return ret;
}

/**
 * show_mst_drv - device attribute show sysfs operation
 */
static ssize_t show_mst_drv(struct device *dev,
			    struct device_attribute *attr, char *buf)
{
	if (!dev)
		return -ENODEV;

	// todo
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
	char test_result[TEST_RESULT_LEN] = { 0, };

	sscanf(buf, "%20s\n", test_result);

	if (wpc_det && (gpio_get_value(wpc_det) == 1)) {
		printk("[MST] Wireless charging is ongoing, do not proceed MST work\n");
		return count;
	}

	switch (test_result[0]) {

	case CMD_MST_LDO_OFF:
		of_mst_hw_onoff(OFF);
		break;

	case CMD_MST_LDO_ON:
		of_mst_hw_onoff(ON);
		break;

	case CMD_SEND_TRACK1_DATA:
		if (transmit_mst_data(TRACK1))
			printk("[MST] Send track1 data --> failed\n");
		else
			printk("[MST] Send track1 data --> successful\n");
		break;

	case CMD_SEND_TRACK2_DATA:
		if (transmit_mst_data(TRACK2))
			printk("[MST] Send track2 data --> failed\n");
		else
			printk("[MST] Send track2 data --> successful\n");
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
			mdelay(10);
			if (transmit_mst_data(TRACK2)) {
				printk("[MST] Send track2 data --> failed\n");
				break;
			}
			printk("[MST] Send track2 data --> successful\n");
			mdelay(1000);
		}
		break;

	case CMD_HW_RELIABILITY_TEST_STOP:
		if (!escape_loop)
			wake_lock_destroy(&mst_wakelock);
		escape_loop = 1;
		printk("[MST] MST escape_loop value = 1\n");
		break;

	default:
		printk(KERN_ERR "[MST] MST invalid value : %s\n", test_result);
		break;
	}
	return count;
}

/**
 * sec_mst_gpio_init - Initialize GPIO pins used by driver
 * @dev: driver handle
 */
static int sec_mst_gpio_init(struct device *dev)
{
	int ret = 0;
	struct device_node *np;
	enum of_gpio_flags irq_gpio_flags;

	/* get wireless chraging check gpio */
	np = of_find_node_by_name(NULL, "battery");
	if (!np) {
		pr_err("%s np NULL\n", __func__);
	} else {
		/* wpc_det */
		wpc_det = of_get_named_gpio_flags(np, "battery,wpc_det",
			0, &irq_gpio_flags);
		if (ret < 0) {
			dev_err(dev, "%s : can't get wpc_det\r\n", __FUNCTION__);
		}
	}

	mst_pwr_en =
	    of_get_named_gpio(dev->of_node, "sec-mst,mst-pwr-gpio", OFF);

	/* check if gpio pin is inited */
	if (mst_pwr_en < 0) {
		printk(KERN_ERR "[MST] %s : Cannot create the gpio\n",
		       __func__);
		//return 1;
	}

	/* gpio request */
	ret = gpio_request(mst_pwr_en, "sec-mst,mst-pwr-gpio");
	if (ret) {
		printk(KERN_ERR "[MST] failed to get en gpio : %d, %d\n", ret,
		       mst_pwr_en);
	}

	/* set gpio direction */
	if (!(ret < 0) && (mst_pwr_en > 0)) {
		gpio_direction_output(mst_pwr_en, OFF);
		printk(KERN_ERR "[MST] %s : Send Output\n", __func__);
	}

	return 0;
}

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
 * mst_ldo_device_probe - Driver probe function
 */
static int mst_ldo_device_probe(struct platform_device *pdev)
{
	int retval = 0;

	printk("[MST] %s init start\n", __func__);

	if (sec_mst_gpio_init(&pdev->dev)) {
		retval = ERROR_VALUE;
		printk(KERN_ERR
		       "[MST] %s: driver initialization failed, retval : %d\n",
		       __FILE__, retval);
		goto done;
	}

	mst_drv_class = class_create(THIS_MODULE, "mstldo");
	if (IS_ERR(mst_drv_class)) {
		retval = PTR_ERR(mst_drv_class);
		printk(KERN_ERR
		       "[MST] %s: driver initialization failed, retval : %d\n",
		       __FILE__, retval);
		goto done;
	}

	mst_drv_dev = device_create(mst_drv_class,
				    NULL /* parent */, 0 /* dev_t */,
				    NULL /* drvdata */,
				    MST_DRV_DEV);
	if (IS_ERR(mst_drv_dev)) {
		retval = PTR_ERR(mst_drv_dev);
		kfree(mst_drv_dev);
		device_destroy(mst_drv_class, 0);
		printk(KERN_ERR
		       "[MST] %s: driver initialization failed, retval : %d\n",
		       __FILE__, retval);
		goto done;
	}

	/* register this mst device with the driver core */
	retval = device_create_file(mst_drv_dev, &dev_attr_transmit);
	if (retval) {
		kfree(mst_drv_dev);
		device_destroy(mst_drv_class, 0);
		printk(KERN_ERR
		       "[MST] %s: (transmit)driver initialization failed, retval : %d\n",
		       __FILE__, retval);
		goto done;
	}
#if defined(CONFIG_MFC_CHARGER)
	retval = device_create_file(mst_drv_dev, &dev_attr_mfc);
	if (retval) {
		kfree(mst_drv_dev);
		device_destroy(mst_drv_class, 0);
		printk(KERN_ERR
		       "[MST] %s: (mfc)driver initialization failed, retval : %d\n",
		       __FILE__, retval);
		goto done;
	}
#endif

	printk(KERN_DEBUG "[MST] MST drv driver (%s) is initialized.\n",
	       MST_DRV_DEV);

done:
	return retval;
}

/**
 * mst_drv_init - Driver init function
 */
static int __init mst_drv_init(void)
{
	int ret = 0;
	printk(KERN_ERR "[MST] %s\n", __func__);
	ret = platform_driver_register(&sec_mst_ldo_driver);
	printk(KERN_ERR "[MST] MST_LDO_DRV]]] init , ret val : %d\n", ret);

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
	printk(KERN_ALERT "[MST] %s\n", __func__);
}

MODULE_AUTHOR("JASON KANG, j_seok.kang@samsung.com");
MODULE_DESCRIPTION("MST drv driver");
MODULE_VERSION("0.1");

module_init(mst_drv_init);
module_exit(mst_drv_exit);
