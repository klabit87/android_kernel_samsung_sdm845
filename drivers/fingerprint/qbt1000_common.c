/* Copyright (c) 2016-2017, The Linux Foundation. All rights reserved.
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
#define DEBUG
#define pr_fmt(fmt) "qbt1000:%s: " fmt, __func__

#include <linux/ktime.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/platform_device.h>
#include <linux/types.h>
#include <linux/cdev.h>
#include <linux/slab.h>
#include <linux/interrupt.h>
#include <linux/workqueue.h>
#include <linux/pm.h>
#include <linux/mutex.h>
#include <linux/atomic.h>
#include <linux/of.h>
#include <linux/of_gpio.h>
#include <linux/kfifo.h>
#include <linux/poll.h>
#include "qbt1000_common.h"
#include "fingerprint.h"

#define QBT1000_DEV "qbt1000"
#define MAX_FW_EVENTS 128
#define FW_MAX_IPC_MSG_DATA_SIZE 0x500
#define IPC_MSG_ID_CBGE_REQUIRED 29
#define IPC_MSG_ID_FINGER_ON_SENSOR 55
#define IPC_MSG_ID_FINGER_OFF_SENSOR 56

#if 1  // SS modified
#define VENDOR						"QCOM"
#define CHIP_ID						"QBT1000"
#endif

struct fw_event_desc {
	enum qbt1000_fw_event ev;
};

struct fw_ipc_info {
	int gpio;
	int irq;
};

struct qbt1000_drvdata {
	struct class	*qbt1000_class;
	struct cdev	qbt1000_cdev;
	struct device	*dev;
	struct device *fp_device;  // SS modified
	char		*qbt1000_node;
	atomic_t	available;
	struct mutex	mutex;
	struct mutex	fw_events_mutex;
	struct fw_ipc_info	fw_ipc;
	DECLARE_KFIFO(fw_events, struct fw_event_desc, MAX_FW_EVENTS);
	wait_queue_head_t read_wait_queue;
};

/*
 * struct ipc_msg_type_to_fw_event -
 *      entry in mapping between an IPC message type to a firmware event
 * @msg_type - IPC message type, as reported by firmware
 * @fw_event - corresponding firmware event code to report to driver client
 */
struct ipc_msg_type_to_fw_event {
	uint32_t msg_type;
	enum qbt1000_fw_event fw_event;
};

/* mapping between firmware IPC message types to HLOS firmware events */
struct ipc_msg_type_to_fw_event g_msg_to_event[] = {
	{IPC_MSG_ID_CBGE_REQUIRED, FW_EVENT_CBGE_REQUIRED},
	{IPC_MSG_ID_FINGER_ON_SENSOR, FW_EVENT_FINGER_DOWN},
	{IPC_MSG_ID_FINGER_OFF_SENSOR, FW_EVENT_FINGER_UP},
};

#if 1  // SS modified
static ssize_t qbt_vendor_show(struct device *dev,
	struct device_attribute *attr, char *buf)
{
	pr_info("%s:%s\n", __func__, sensor_status[0]);
	return snprintf(buf, PAGE_SIZE, "%s\n", VENDOR);
}

static ssize_t qbt_name_show(struct device *dev,
	struct device_attribute *attr, char *buf)
{
	return snprintf(buf, PAGE_SIZE, "%s\n", CHIP_ID);
}

static DEVICE_ATTR(vendor, 0444, qbt_vendor_show, NULL);
static DEVICE_ATTR(name, 0444, qbt_name_show, NULL);

static struct device_attribute *fp_attrs[] = {
	&dev_attr_vendor,
	&dev_attr_name,
	NULL,
};
#endif

/**
 * qbt1000_open() - Function called when user space opens device.
 * Successful if driver not currently open.
 * @inode:	ptr to inode object
 * @file:	ptr to file object
 *
 * Return: 0 on success. Error code on failure.
 */
static int qbt1000_open(struct inode *inode, struct file *file)
{
	int rc = 0;

	struct qbt1000_drvdata *drvdata = container_of(inode->i_cdev,
						   struct qbt1000_drvdata,
						   qbt1000_cdev);
	file->private_data = drvdata;

	pr_debug("qbt1000_open begin\n");
	/* disallowing concurrent opens */
	if (!atomic_dec_and_test(&drvdata->available)) {
		atomic_inc(&drvdata->available);
		rc = -EBUSY;
	}

	pr_debug("qbt1000_open end : %d\n", rc);
	return rc;
}

/**
 * qbt1000_release() - Function called when user space closes device.

 * @inode:	ptr to inode object
 * @file:	ptr to file object
 *
 * Return: 0 on success. Error code on failure.
 */
static int qbt1000_release(struct inode *inode, struct file *file)
{
	struct qbt1000_drvdata *drvdata;

	if (!file || !file->private_data) {
		pr_err("qbt1000_release: NULL pointer passed");
		return -EINVAL;
	}
	drvdata = file->private_data;
	atomic_inc(&drvdata->available);
	return 0;
}

static int get_events_fifo_len_locked(struct qbt1000_drvdata *drvdata)
{
	int len;

	mutex_lock(&drvdata->fw_events_mutex);
	len = kfifo_len(&drvdata->fw_events);
	mutex_unlock(&drvdata->fw_events_mutex);

	return len;
}

static ssize_t qbt1000_read(struct file *filp, char __user *ubuf,
		size_t cnt, loff_t *ppos)
{
	struct fw_event_desc fw_event;
	struct qbt1000_drvdata *drvdata = filp->private_data;

	if (cnt < sizeof(fw_event.ev))
		return -EINVAL;

	mutex_lock(&drvdata->fw_events_mutex);

	while (kfifo_len(&drvdata->fw_events) == 0) {
		mutex_unlock(&drvdata->fw_events_mutex);

		if (filp->f_flags & O_NONBLOCK)
			return -EAGAIN;

		pr_debug("fw_events fifo: empty, waiting\n");

		if (wait_event_interruptible(drvdata->read_wait_queue,
			  (get_events_fifo_len_locked(drvdata) > 0)))
			return -ERESTARTSYS;

		mutex_lock(&drvdata->fw_events_mutex);
	}

	if (!kfifo_get(&drvdata->fw_events, &fw_event)) {
		pr_debug("fw_events fifo: unexpectedly empty\n");

		mutex_unlock(&drvdata->fw_events_mutex);
		return -EINVAL;
	}

	mutex_unlock(&drvdata->fw_events_mutex);

	pr_crit("Firmware event %d read at time %lu uS\n", (int)fw_event.ev, (unsigned long)ktime_to_us(ktime_get()));
	return copy_to_user(ubuf, &fw_event.ev, sizeof(fw_event.ev));
}

static unsigned int qbt1000_poll(struct file *filp,
	struct poll_table_struct *wait)
{
	struct qbt1000_drvdata *drvdata = filp->private_data;
	unsigned int mask = 0;

	poll_wait(filp, &drvdata->read_wait_queue, wait);

	if (kfifo_len(&drvdata->fw_events) > 0)
		mask |= (POLLIN | POLLRDNORM);

	return mask;
}

static const struct file_operations qbt1000_fops = {
	.owner = THIS_MODULE,
	.open = qbt1000_open,
	.release = qbt1000_release,
	.read = qbt1000_read,
	.poll = qbt1000_poll
};

static int qbt1000_dev_register(struct qbt1000_drvdata *drvdata)
{
	dev_t dev_no;
	int ret = 0;
	size_t node_size;
	char *node_name = QBT1000_DEV;
	struct device *dev = drvdata->dev;
	struct device *device;

	node_size = strlen(node_name) + 1;

	drvdata->qbt1000_node = devm_kzalloc(dev, node_size, GFP_KERNEL);
	if (!drvdata->qbt1000_node) {
		ret = -ENOMEM;
		goto err_alloc;
	}

	strlcpy(drvdata->qbt1000_node, node_name, node_size);

	ret = alloc_chrdev_region(&dev_no, 0, 1, drvdata->qbt1000_node);
	if (ret) {
		pr_err("alloc_chrdev_region failed %d\n", ret);
		goto err_alloc;
	}

	cdev_init(&drvdata->qbt1000_cdev, &qbt1000_fops);

	drvdata->qbt1000_cdev.owner = THIS_MODULE;
	ret = cdev_add(&drvdata->qbt1000_cdev, dev_no, 1);
	if (ret) {
		pr_err("cdev_add failed %d\n", ret);
		goto err_cdev_add;
	}

	drvdata->qbt1000_class = class_create(THIS_MODULE,
					   drvdata->qbt1000_node);
	if (IS_ERR(drvdata->qbt1000_class)) {
		ret = PTR_ERR(drvdata->qbt1000_class);
		pr_err("class_create failed %d\n", ret);
		goto err_class_create;
	}

	device = device_create(drvdata->qbt1000_class, NULL,
			       drvdata->qbt1000_cdev.dev, drvdata,
			       drvdata->qbt1000_node);
	if (IS_ERR(device)) {
		ret = PTR_ERR(device);
		pr_err("device_create failed %d\n", ret);
		goto err_dev_create;
	}

	return 0;
err_dev_create:
	class_destroy(drvdata->qbt1000_class);
err_class_create:
	cdev_del(&drvdata->qbt1000_cdev);
err_cdev_add:
	unregister_chrdev_region(drvdata->qbt1000_cdev.dev, 1);
err_alloc:
	return ret;
}

/**
 * qbt1000_ipc_irq_handler() - function processes IPC
 * interrupts on its own thread
 * @irq:	the interrupt that occurred
 * @dev_id: pointer to the qbt1000_drvdata
 *
 * Return: IRQ_HANDLED when complete
 */
static irqreturn_t qbt1000_ipc_irq_handler(int irq, void *dev_id)
{

	struct qbt1000_drvdata *drvdata = (struct qbt1000_drvdata *)dev_id;
	enum qbt1000_fw_event ev = FW_EVENT_CBGE_REQUIRED;
	struct fw_event_desc fw_ev_des;

	pm_stay_awake(drvdata->dev);
	mutex_lock(&drvdata->mutex);

	if (irq != drvdata->fw_ipc.irq) {
		pr_warn("invalid irq %d (expected %d)\n",
			irq, drvdata->fw_ipc.irq);
		goto end;
	}

	pr_debug("firmware interrupt received (irq %d)\n", irq);

	//if (!drvdata->fp_app_handle)
		//goto end;
	pr_warn("before changes qbt driveer irq handler");

	mutex_lock(&drvdata->fw_events_mutex);
	pr_debug("fw events: add %d\n", (int) ev);
	fw_ev_des.ev = ev;
	if (!kfifo_put(&drvdata->fw_events, fw_ev_des))
		pr_err("fw events: fifo full, drop event %d\n",
		(int) ev);

	mutex_unlock(&drvdata->fw_events_mutex);
	pr_warn("after changes qbt driveer irq handler");

	wake_up_interruptible(&drvdata->read_wait_queue);
end:

	mutex_unlock(&drvdata->mutex);
	pm_relax(drvdata->dev);
	return IRQ_HANDLED;
}

static int setup_ipc_irq(struct platform_device *pdev,
	struct qbt1000_drvdata *drvdata)
{
	int rc = 0;
	const char *desc = "qbt_ipc";

	drvdata->fw_ipc.irq = gpio_to_irq(drvdata->fw_ipc.gpio);
	pr_debug("\nirq %d gpio %d\n",
			drvdata->fw_ipc.irq, drvdata->fw_ipc.gpio);
	if (drvdata->fw_ipc.irq < 0) {
		rc = drvdata->fw_ipc.irq;
		pr_err("no irq for gpio %d, error=%d\n",
		  drvdata->fw_ipc.gpio, rc);
		goto end;
	}

	rc = devm_gpio_request_one(&pdev->dev, drvdata->fw_ipc.gpio,
			GPIOF_IN, desc);

	if (rc < 0) {
		pr_err("failed to request gpio %d, error %d\n",
			drvdata->fw_ipc.gpio, rc);
		goto end;
	}

	rc = devm_request_threaded_irq(&pdev->dev,
		drvdata->fw_ipc.irq,
		NULL,
		qbt1000_ipc_irq_handler,
		IRQF_ONESHOT | IRQF_TRIGGER_RISING,
		desc,
		drvdata);

	if (rc < 0) {
		pr_err("failed to register for ipc irq %d, rc = %d\n",
			drvdata->fw_ipc.irq, rc);
		goto end;
	}

end:
	return rc;
}

/**
 * qbt1000_read_device_tree() - Function reads device tree
 * properties into driver data
 * @pdev:	ptr to platform device object
 * @drvdata:	ptr to driver data
 *
 * Return: 0 on success. Error code on failure.
 */
static int qbt1000_read_device_tree(struct platform_device *pdev,
	struct qbt1000_drvdata *drvdata)
{
	int rc = 0;

	/* read IPC gpio */
	drvdata->fw_ipc.gpio = of_get_named_gpio(pdev->dev.of_node,
		"qcom,ipc-gpio", 0);
	if (drvdata->fw_ipc.gpio < 0) {
		rc = drvdata->fw_ipc.gpio;
		pr_err("ipc gpio not found, error=%d\n", rc);
		goto end;
	}

#if 1 //SS modified
	{
		int ldogpio = 0;
		int wintgpio = 0;

		ldogpio = of_get_named_gpio(pdev->dev.of_node, "qcom,ldo-gpio", 0);
		if (ldogpio < 0) {
			rc = ldogpio;
			pr_err("ldo gpio not found, error=%d\n", rc);
		} else {			
			rc = gpio_request(ldogpio, "qsd_ldo_en");
			gpio_direction_output(ldogpio, 1);
			gpio_set_value(ldogpio, 1);
		}

		wintgpio = of_get_named_gpio(pdev->dev.of_node, "qcom,wint-gpio", 0);
		if (wintgpio < 0) {
			pr_err("wuhb_int gpio not found, error=%d\n", wintgpio);
		} else {			
			gpio_request(wintgpio, "qsd_wuhb_int");
			gpio_direction_input(wintgpio);
		}
	}
#endif

end:
	return rc;
}

/**
 * qbt1000_probe() - Function loads hardware config from device tree
 * @pdev:	ptr to platform device object
 *
 * Return: 0 on success. Error code on failure.
 */
static int qbt1000_probe(struct platform_device *pdev)
{
	struct device *dev = &pdev->dev;
	struct qbt1000_drvdata *drvdata;
	int rc = 0;

	pr_debug("qbt1000_probe begin\n");
	drvdata = devm_kzalloc(dev, sizeof(*drvdata), GFP_KERNEL);
	if (!drvdata)
		return -ENOMEM;

	drvdata->dev = &pdev->dev;
	platform_set_drvdata(pdev, drvdata);

	rc = qbt1000_read_device_tree(pdev, drvdata);
	if (rc < 0)
		goto end;

	atomic_set(&drvdata->available, 1);

	mutex_init(&drvdata->mutex);
	mutex_init(&drvdata->fw_events_mutex);

	rc = qbt1000_dev_register(drvdata);
	if (rc < 0)
		goto end;

	INIT_KFIFO(drvdata->fw_events);
	init_waitqueue_head(&drvdata->read_wait_queue);

	rc = setup_ipc_irq(pdev, drvdata);
	if (rc < 0)
		goto end;

	rc = device_init_wakeup(&pdev->dev, 1);
	if (rc < 0)
		goto end;

	rc  = fingerprint_register(drvdata->fp_device,
		drvdata, fp_attrs, "fingerprint");
	if (rc ) {
		pr_err("%s sysfs register failed\n", __func__);
		goto end;
	}

end:
	pr_debug("qbt1000_probe end : %d\n", rc);
	return rc;
}

static int qbt1000_remove(struct platform_device *pdev)
{
	struct qbt1000_drvdata *drvdata = platform_get_drvdata(pdev);

	mutex_destroy(&drvdata->mutex);
	mutex_destroy(&drvdata->fw_events_mutex);

	device_destroy(drvdata->qbt1000_class, drvdata->qbt1000_cdev.dev);
	class_destroy(drvdata->qbt1000_class);
	cdev_del(&drvdata->qbt1000_cdev);
	unregister_chrdev_region(drvdata->qbt1000_cdev.dev, 1);
	fingerprint_unregister(drvdata->fp_device, fp_attrs);
	device_init_wakeup(&pdev->dev, 0);

	return 0;
}

static int qbt1000_suspend(struct platform_device *pdev, pm_message_t state)
{
	int rc = 0;
	struct qbt1000_drvdata *drvdata = platform_get_drvdata(pdev);

	/*
	 * Returning an error code if driver currently making a TZ call.
	 * Note: The purpose of this driver is to ensure that the clocks are on
	 * while making a TZ call. Hence the clock check to determine if the
	 * driver will allow suspend to occur.
	 */
	if (!mutex_trylock(&drvdata->mutex))
		return -EBUSY;
	else {
		enable_irq_wake(drvdata->fw_ipc.irq);
	}

	mutex_unlock(&drvdata->mutex);

	return rc;
}

static int qbt1000_resume(struct platform_device *pdev)
{
	struct qbt1000_drvdata *drvdata = platform_get_drvdata(pdev);

	disable_irq_wake(drvdata->fw_ipc.irq);

	return 0;
}

static const struct of_device_id qbt1000_match[] = {
	{ .compatible = "qcom,qbt1000" },
	{}
};

static struct platform_driver qbt1000_plat_driver = {
	.probe = qbt1000_probe,
	.remove = qbt1000_remove,
	.suspend = qbt1000_suspend,
	.resume = qbt1000_resume,
	.driver = {
		.name = "qbt1000",
		.owner = THIS_MODULE,
		.of_match_table = qbt1000_match,
	},
};

module_platform_driver(qbt1000_plat_driver);

MODULE_LICENSE("GPL v2");
MODULE_DESCRIPTION("Qualcomm Technologies, Inc. QBT1000 driver");
