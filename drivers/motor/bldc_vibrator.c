
/* Copyright (c) 2016, The Linux Foundation. All rights reserved.
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

#include <linux/kernel.h>
#include <linux/platform_device.h>
#include <linux/err.h>
#include <linux/hrtimer.h>
#include <linux/module.h>
#include <linux/gpio.h>
#include <linux/regulator/consumer.h>
#include <linux/workqueue.h>
#include <linux/of_gpio.h>
#include <linux/pinctrl/consumer.h>
#include <linux/pm_qos.h>
#include <linux/wakelock.h>


#define MAX_INTENSITY		10000
#define HIGH_INTENSITY			10000	/*HIGH 6667 ~ 10000*/
#define HIGH_INTENSITY_VOLTAGE		3200000	/*HIGH Voltage 3.2V*/
#define MID_INTENSITY			6666	/*MID 3334 ~ 6666*/
#define MID_INTENSITY_VOLTAGE		2800000	/*MID Voltage 3.0V*/
#define LOW_INTENSITY			3333	/*LOW 1 ~ 3333*/
#define LOW_INTENSITY_VOLTAGE		2400000	/*LOW Voltage 2.7V*/
#define NO_INTENSITY			0		/*NO INTENSITY*/
#define NO_INTENSITY_VOLTAGE		0		/* Voltage 0V*/
#define PM_QOS_NONIDLE_VALUE			300

#if defined(CONFIG_SLPI_MOTOR)
#include <linux/adsp/slpi_motor.h>
#endif

/* default timeout */
#define VIB_DEFAULT_TIMEOUT 10000

struct pm_qos_request pm_qos_req;
static struct wake_lock vib_wake_lock;


static int vib_voltage = 0;


struct bldc_vib {
	struct class *to_class;
	struct device *to_dev;
	struct device *dev;
	struct hrtimer vib_timer;
	struct work_struct work;
	struct workqueue_struct *queue;
	struct mutex lock;

	int state;
	int timeout;
	int intensity;
	int timevalue;
	void (*power_onoff)(int onoff);
	const char *regulator_name;	
	struct regulator *regulator;
};


void vibe_set_intensity(int intensity)
{
	pr_info("vibrator intensity %d\n", intensity);
	if (intensity == NO_INTENSITY) {
		vib_voltage = NO_INTENSITY_VOLTAGE; 
	} else if (intensity < LOW_INTENSITY) {
		vib_voltage = LOW_INTENSITY_VOLTAGE;
	} else if (intensity < MID_INTENSITY) {
		vib_voltage = MID_INTENSITY_VOLTAGE;
	} else if (intensity <= HIGH_INTENSITY){
		vib_voltage = HIGH_INTENSITY_VOLTAGE;
	} else {
		vib_voltage = HIGH_INTENSITY_VOLTAGE;
	    pr_err("Intensity out of range !\n"); 
	}
}


static void set_vibrator(struct bldc_vib *vib)
{

	int ret=0;
	static int enable_status = 0;

	mutex_lock(&vib->lock);


	pr_info("[VIB]: %s, value[%d]\n", __func__, vib->state);
	if (vib->state) {
		wake_lock(&vib_wake_lock);
		pm_qos_update_request(&pm_qos_req, PM_QOS_NONIDLE_VALUE);
		
#if defined(CONFIG_SLPI_MOTOR)
		setSensorCallback(true, vib->timevalue);
#endif

		pr_info("[VIB]: %s, vib_voltage is [%d]\n", __func__,vib_voltage);

		if (vib_voltage == NO_INTENSITY_VOLTAGE) {
			if (regulator_disable(vib->regulator)) 
				pr_err("Fail to disable regulator_vib\n");
		} else if (enable_status != vib->state) {
		   	if (regulator_set_voltage(vib->regulator, vib_voltage, vib_voltage))
				pr_err("Fail to enable regulator_vib Voltage!\n");		
			if (regulator_is_enabled(vib->regulator)) {
					pr_err("vib->regulator is already enabled\n");
			} else {
				ret=regulator_enable(vib->regulator);
				if(ret){
					pr_err("Fail to enable regulator_vib\n");	
				}else{
					enable_status = vib->state;
				}
			} 
		}else {
			pr_info("[VIB]:  Vibration On status no change.\n");
		}
		
		hrtimer_start(&vib->vib_timer, ktime_set(vib->timevalue / 1000, (vib->timevalue % 1000) * 1000000),HRTIMER_MODE_REL);	
	} else {

		if (enable_status != vib->state) {
			if (!regulator_disable(vib->regulator)){
				enable_status = vib->state;
			}else{
				pr_err("Fail to disable regulator_vib\n");
			}
		} else {
			pr_info("[VIB]: Vibration OFF status no change.\n");
		}
		
#if defined(CONFIG_SLPI_MOTOR)
		setSensorCallback(false, vib->timevalue);
#endif
		//PM_QOS_DEFAULT_VALUE
		wake_unlock(&vib_wake_lock);
		pm_qos_update_request(&pm_qos_req, PM_QOS_DEFAULT_VALUE);
	}

	mutex_unlock(&vib->lock);

	pr_info("[VIB]: %s, vibrator control finish value[%d]\n", __func__, vib->state);
}

static void vibrator_enable(struct bldc_vib *vib, int value)
{

	mutex_lock(&vib->lock);
	hrtimer_cancel(&vib->vib_timer);

	if (value == 0) {
		pr_info("[VIB]: OFF\n");
		vib->state = 0;
		vib->timevalue = 0;
	} else {
		pr_info("[VIB]: ON, Duration : %d msec, intensity : %d\n", value, vib->intensity);
		vib->state = 1;
		vib->timevalue = value;
	}

	mutex_unlock(&vib->lock);
	queue_work(vib->queue, &vib->work);
}

static void bldc_vibrator_update(struct work_struct *work)
{
	struct bldc_vib *vib = container_of(work, struct bldc_vib, work);

	set_vibrator(vib);
}

static enum hrtimer_restart vibrator_timer_func(struct hrtimer *timer)
{
	struct bldc_vib *vib = container_of(timer, struct bldc_vib, vib_timer);

	vib->state = 0;
	queue_work(vib->queue, &vib->work);

	return HRTIMER_NORESTART;
}

#if defined(CONFIG_PM)
static int bldc_vibrator_suspend(struct device *dev)
{
	struct bldc_vib *vib = dev_get_drvdata(dev);

	pr_info("[VIB]: %s\n", __func__);

	hrtimer_cancel(&vib->vib_timer);
	cancel_work_sync(&vib->work);
	/* turn-off vibrator */
	vib->state = 0;
	set_vibrator(vib);
	return 0;
}
#endif

static SIMPLE_DEV_PM_OPS(vibrator_pm_ops, bldc_vibrator_suspend, NULL);

static int vibrator_parse_dt(struct bldc_vib *vib)
{
	struct device_node *np = vib->dev->of_node;
	int rc;

	rc = of_property_read_string(np, "bldc,vib-regulator", &vib->regulator_name);
	if (rc) {
		pr_info("vib-regulator not specified so using default address\n");	
		vib->regulator_name = NULL;
		rc = 0;
	}

	return rc;
}

static ssize_t enable_show(struct device *dev, struct device_attribute *attr,
		char *buf)
{
	struct bldc_vib *vib = dev_get_drvdata(dev);
	struct hrtimer *timer = &vib->vib_timer;
	int remaining = 0;

	if (hrtimer_active(timer)) {
		ktime_t remain = hrtimer_get_remaining(timer);
		struct timeval t = ktime_to_timeval(remain);

		remaining = t.tv_sec * 1000 + t.tv_usec / 1000;
	}

	return sprintf(buf, "%d\n", remaining);
}

static ssize_t enable_store(struct device *dev, struct device_attribute *attr,
		const char *buf, size_t size)
{
	struct bldc_vib *vib = dev_get_drvdata(dev);
	int value;
	int ret;

	ret = kstrtoint(buf, 0, &value);
	if (ret != 0)
		return -EINVAL;

	vibrator_enable(vib, value);

	return size;
}

static DEVICE_ATTR(enable, 0660, enable_show, enable_store);

static int bldc_vibrator_probe(struct platform_device *pdev)
{
	struct bldc_vib *vib;
	int rc = 0;

	pr_info("[VIB]: %s\n",__func__);

	vib = devm_kzalloc(&pdev->dev, sizeof(*vib), GFP_KERNEL);
	if (!vib) {
		pr_err("[VIB]: %s : Failed to allocate memory\n", __func__);
		return -ENOMEM;
	}

	if (!pdev->dev.of_node) {
		pr_err("[VIB]: %s failed, DT is NULL", __func__);
		return -ENODEV;
	}

	vib->dev = &pdev->dev;
	rc = vibrator_parse_dt(vib);
	if(rc)
		return rc;

	vib->power_onoff = NULL;
	vib->intensity = MAX_INTENSITY;

	vib->timeout = VIB_DEFAULT_TIMEOUT;

	vibe_set_intensity(vib->intensity);
	INIT_WORK(&vib->work, bldc_vibrator_update);
	mutex_init(&vib->lock);

	vib->queue = create_singlethread_workqueue("bldc_vibrator");
	if (!vib->queue) {
		pr_err("[VIB]: %s: can't create workqueue\n", __func__);
		return -ENOMEM;
	}

	hrtimer_init(&vib->vib_timer, CLOCK_MONOTONIC, HRTIMER_MODE_REL);
	vib->vib_timer.function = vibrator_timer_func;

	dev_set_drvdata(&pdev->dev, vib);

	vib->to_class = class_create(THIS_MODULE, "timed_output");
	if (IS_ERR(vib->to_class)) {
		pr_err("[VIB]: timed_output classs create fail (rc=%d)\n", rc);
		goto err_read_vib;
	}

	vib->to_dev = device_create(vib->to_class, NULL, 0, vib, "vibrator");
	if (IS_ERR(vib->to_dev))
		return PTR_ERR(vib->to_dev);

	rc = sysfs_create_file(&vib->to_dev->kobj, &dev_attr_enable.attr);
	if (rc < 0)
		pr_err("[VIB]: Failed to register sysfs enable: %d\n", rc);


	vib->regulator = devm_regulator_get(vib->dev, vib->regulator_name);
	if (IS_ERR(vib->regulator)) {
		pr_err("Fail to get regulator_vib\n");
		return PTR_ERR(vib->regulator);
	} else if (regulator_is_enabled(vib->regulator)){
		rc = regulator_disable(vib->regulator);
		if(rc){	
		pr_err("Fail to disable regulator_vib\n");
		}
	}

	wake_lock_init(&vib_wake_lock, WAKE_LOCK_SUSPEND, "vib_present");
	pm_qos_add_request(&pm_qos_req, PM_QOS_CPU_DMA_LATENCY,
                                                PM_QOS_DEFAULT_VALUE);

	return 0;
err_read_vib:
	destroy_workqueue(vib->queue);
	mutex_destroy(&vib->lock);
	return rc;
}

static int bldc_vibrator_remove(struct platform_device *pdev)
{
	struct bldc_vib *vib = dev_get_drvdata(&pdev->dev);

	if (!IS_ERR(vib->regulator)) {
		regulator_put(vib->regulator);
	}

	pm_qos_remove_request(&pm_qos_req);

	destroy_workqueue(vib->queue);
	mutex_destroy(&vib->lock);
	wake_lock_destroy(&vib_wake_lock);
	return 0;
}

static const struct of_device_id vib_motor_match[] = {
	{	.compatible = "bldc_vib",
	},
	{}
};

static struct platform_driver bldc_vibrator_platdrv =
{
	.driver =
	{
		.name = "bldc_vib",
		.owner = THIS_MODULE,
		.of_match_table = vib_motor_match,
		.pm	= &vibrator_pm_ops,
	},
	.probe = bldc_vibrator_probe,
	.remove = bldc_vibrator_remove,
};

static int __init bldc_timed_vibrator_init(void)
{
	return platform_driver_register(&bldc_vibrator_platdrv);
}

void __exit bldc_timed_vibrator_exit(void)
{
	platform_driver_unregister(&bldc_vibrator_platdrv);
}
module_init(bldc_timed_vibrator_init);
module_exit(bldc_timed_vibrator_exit);

MODULE_AUTHOR("Samsung Corporation");
MODULE_DESCRIPTION("timed output vibrator device");
MODULE_LICENSE("GPL v2");
