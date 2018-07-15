/* Copyright (c) 2013-2014, The Linux Foundation. All rights reserved.
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

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/list.h>
#include <linux/spinlock.h>
#include <linux/device.h>
#include <linux/err.h>
#include <linux/ctype.h>
#include <linux/rwsem.h>
#include <linux/sensors.h>
#include <linux/string.h>
#include <linux/input.h>

#define APPLY_MASK	0x00000001

#define CMD_W_L_MASK 0x00
#define CMD_W_H_MASK 0x10
#define CMD_W_H_L	0x10
#define CMD_MASK	0xF
#define DATA_MASK	0xFFFF0000
#define DATA_AXIS_SHIFT	17
#define DATA_APPLY_SHIFT	16
/*
 * CMD_GET_PARAMS(BIT, PARA, DATA) combine high 16 bit and low 16 bit
 * as one params
 */

#define CMD_GET_PARAMS(BIT, PARA, DATA)	\
	((BIT) ?	\
		((DATA) & DATA_MASK)	\
		: ((PARA) \
		| (((DATA) & DATA_MASK) >> 16)))


/*
 * CMD_DO_CAL sensor do calibrate command, when do sensor calibrate must use
 * this.
 * AXIS_X,AXIS_Y,AXIS_Z write axis params to driver like accelerometer
 * magnetometer,gyroscope etc.
 * CMD_W_THRESHOLD_H,CMD_W_THRESHOLD_L,CMD_W_BIAS write theshold and bias
 * params to proximity driver.
 * CMD_W_FACTOR,CMD_W_OFFSET write factor and offset params to light
 * sensor driver.
 * CMD_COMPLETE when one sensor receive calibrate parameters complete, it
 * must use this command to end receive the parameters and send the
 * parameters to sensor.
 */

enum {
	CMD_DO_CAL = 0x0,
	CMD_W_OFFSET_X,
	CMD_W_OFFSET_Y,
	CMD_W_OFFSET_Z,
	CMD_W_THRESHOLD_H,
	CMD_W_THRESHOLD_L,
	CMD_W_BIAS,
	CMD_W_OFFSET,
	CMD_W_FACTOR,
	CMD_W_RANGE,
	CMD_COMPLETE,
	CMD_COUNT
};

int cal_map[] = {
	0,
	offsetof(struct cal_result_t, offset_x),
	offsetof(struct cal_result_t, offset_y),
	offsetof(struct cal_result_t, offset_z),
	offsetof(struct cal_result_t, threshold_h),
	offsetof(struct cal_result_t, threshold_l),
	offsetof(struct cal_result_t, bias),
	offsetof(struct cal_result_t, offset[0]),
	offsetof(struct cal_result_t, offset[1]),
	offsetof(struct cal_result_t, offset[2]),
	offsetof(struct cal_result_t, factor),
	offsetof(struct cal_result_t, range),
};

static struct class *sensors_class;
static struct device *symlink_dev;
static struct device *sensor_dev;
static struct input_dev *meta_input_dev;
static atomic_t sensor_count;

struct class *sensors_event_class;
EXPORT_SYMBOL_GPL(sensors_event_class);

DECLARE_RWSEM(sensors_list_lock);
LIST_HEAD(sensors_list);

static ssize_t name_show(struct device *dev,
		struct device_attribute *attr, char *buf)
{
	struct sensors_classdev *sensors_cdev = dev_get_drvdata(dev);

	return snprintf(buf, PAGE_SIZE, "%s\n", sensors_cdev->name);
}

static ssize_t vendor_show(struct device *dev,
		struct device_attribute *attr, char *buf)
{
	struct sensors_classdev *sensors_cdev = dev_get_drvdata(dev);

	return snprintf(buf, PAGE_SIZE, "%s\n", sensors_cdev->vendor);
}

static ssize_t version_show(struct device *dev,
	struct device_attribute *attr, char *buf)
{
	struct sensors_classdev *sensors_cdev = dev_get_drvdata(dev);

	return snprintf(buf, PAGE_SIZE, "%d\n", sensors_cdev->version);
}

static ssize_t handle_show(struct device *dev,
		struct device_attribute *attr, char *buf)
{
	struct sensors_classdev *sensors_cdev = dev_get_drvdata(dev);

	return snprintf(buf, PAGE_SIZE, "%d\n", sensors_cdev->handle);
}

static ssize_t type_show(struct device *dev,
		struct device_attribute *attr, char *buf)
{
	struct sensors_classdev *sensors_cdev = dev_get_drvdata(dev);

	return snprintf(buf, PAGE_SIZE, "%d\n", sensors_cdev->type);
}

static ssize_t max_delay_show(struct device *dev,
		struct device_attribute *attr, char *buf)
{
	struct sensors_classdev *sensors_cdev = dev_get_drvdata(dev);

	return snprintf(buf, PAGE_SIZE, "%d\n", sensors_cdev->max_delay);
}

static ssize_t flags_show(struct device *dev,
		struct device_attribute *attr, char *buf)
{
	struct sensors_classdev *sensors_cdev = dev_get_drvdata(dev);

	return snprintf(buf, PAGE_SIZE, "%d\n", sensors_cdev->flags);
}

static ssize_t max_range_show(struct device *dev,
		struct device_attribute *attr, char *buf)
{
	struct sensors_classdev *sensors_cdev = dev_get_drvdata(dev);

	return snprintf(buf, PAGE_SIZE, "%s\n", sensors_cdev->max_range);
}

static ssize_t resolution_show(struct device *dev,
		struct device_attribute *attr, char *buf)
{
	struct sensors_classdev *sensors_cdev = dev_get_drvdata(dev);

	return snprintf(buf, PAGE_SIZE, "%s\n", sensors_cdev->resolution);
}

static ssize_t sensor_power_show(struct device *dev,
		struct device_attribute *attr, char *buf)
{
	struct sensors_classdev *sensors_cdev = dev_get_drvdata(dev);

	return snprintf(buf, PAGE_SIZE, "%s\n", sensors_cdev->sensor_power);
}

static ssize_t min_delay_show(struct device *dev,
		struct device_attribute *attr, char *buf)
{
	struct sensors_classdev *sensors_cdev = dev_get_drvdata(dev);

	return snprintf(buf, PAGE_SIZE, "%d\n", sensors_cdev->min_delay);
}

static ssize_t fifo_reserved_event_count_show(struct device *dev,
		struct device_attribute *attr, char *buf)
{
	struct sensors_classdev *sensors_cdev = dev_get_drvdata(dev);

	return snprintf(buf, PAGE_SIZE, "%d\n",
		sensors_cdev->fifo_reserved_event_count);
}

static ssize_t fifo_max_event_count_show(struct device *dev,
		struct device_attribute *attr, char *buf)
{
	struct sensors_classdev *sensors_cdev = dev_get_drvdata(dev);

	return snprintf(buf, PAGE_SIZE, "%d\n",
		sensors_cdev->fifo_max_event_count);
}

static ssize_t enable_store(struct device *dev,
		struct device_attribute *attr, const char *buf, size_t size)
{
	struct sensors_classdev *sensors_cdev = dev_get_drvdata(dev);
	ssize_t ret = -EINVAL;
	unsigned int data = 0;

	ret = kstrtouint(buf, 10, &data);
	if (ret)
		return ret;
	if (data > 1) {
		dev_err(dev, "Invalid value of input, input=%u\n", data);
		return -EINVAL;
	}

	if (sensors_cdev->sensors_enable == NULL) {
		dev_err(dev, "Invalid sensor class enable handle\n");
		return -EINVAL;
	}
	ret = sensors_cdev->sensors_enable(sensors_cdev, data);
	if (ret)
		return ret;

	sensors_cdev->enabled = data;
	return size;
}


static ssize_t enable_show(struct device *dev,
		struct device_attribute *attr, char *buf)
{
	struct sensors_classdev *sensors_cdev = dev_get_drvdata(dev);

	return snprintf(buf, PAGE_SIZE, "%u\n", sensors_cdev->enabled);
}

static ssize_t poll_delay_store(struct device *dev,
		struct device_attribute *attr, const char *buf, size_t size)
{
	struct sensors_classdev *sensors_cdev = dev_get_drvdata(dev);
	ssize_t ret = -EINVAL;
	unsigned int data = 0;

	ret = kstrtouint(buf, 10, &data);
	if (ret)
		return ret;
	/* The data unit is millisecond, the min_delay unit is microseconds. */
	if ((data * 1000) < sensors_cdev->min_delay) {
		dev_err(dev, "Invalid value of delay, delay=%u\n", data);
		return -EINVAL;
	}
	if (sensors_cdev->sensors_poll_delay == NULL) {
		dev_err(dev, "Invalid sensor class delay handle\n");
		return -EINVAL;
	}
	ret = sensors_cdev->sensors_poll_delay(sensors_cdev, data);
	if (ret)
		return ret;

	sensors_cdev->delay_msec = data;
	return size;
}

static ssize_t poll_delay_show(struct device *dev,
		struct device_attribute *attr, char *buf)
{
	struct sensors_classdev *sensors_cdev = dev_get_drvdata(dev);

	return snprintf(buf, PAGE_SIZE, "%u\n", sensors_cdev->delay_msec);
}

static ssize_t self_test_show(struct device *dev,
		struct device_attribute *attr, char *buf)
{
	struct sensors_classdev *sensors_cdev = dev_get_drvdata(dev);
	int ret;

	if (sensors_cdev->sensors_self_test == NULL) {
		dev_err(dev, "Invalid sensor class self test handle\n");
		return -EINVAL;
	}

	ret = sensors_cdev->sensors_self_test(sensors_cdev);
	if (ret)
		dev_warn(dev, "self test failed.(%d)\n", ret);

	return snprintf(buf, PAGE_SIZE, "%s\n",
			ret ? "fail" : "pass");
}

static ssize_t max_latency_store(struct device *dev,
		struct device_attribute *attr, const char *buf, size_t size)
{
	struct sensors_classdev *sensors_cdev = dev_get_drvdata(dev);
	unsigned int latency;
	int ret = -EINVAL;

	ret = kstrtouint(buf, 10, &latency);
	if (ret)
		return ret;

	if (latency > (unsigned int)sensors_cdev->max_delay) {
		dev_err(dev, "max_latency(%u) is greater than max_delay(%u)\n",
				latency, sensors_cdev->max_delay);
		return -EINVAL;
	}

	if (sensors_cdev->sensors_set_latency == NULL) {
		dev_err(dev, "Invalid sensor calss set latency handle\n");
		return -EINVAL;
	}

	/* Disable batching for this sensor */
	if (latency < sensors_cdev->delay_msec) {
		dev_err(dev, "max_latency is less than delay_msec\n");
		return -EINVAL;
	}

	ret = sensors_cdev->sensors_set_latency(sensors_cdev, latency);
	if (ret)
		return ret;

	sensors_cdev->max_latency = latency;

	return size;
}

static ssize_t max_latency_show(struct device *dev,
		struct device_attribute *attr, char *buf)
{
	struct sensors_classdev *sensors_cdev = dev_get_drvdata(dev);

	return snprintf(buf, PAGE_SIZE, "%u\n", sensors_cdev->max_latency);
}

static ssize_t flush_store(struct device *dev,
		struct device_attribute *attr, const char *buf, size_t size)
{
	struct sensors_classdev *sensors_cdev = dev_get_drvdata(dev);
	ssize_t ret = -EINVAL;
	unsigned long data = 0;

	ret = kstrtoul(buf, 10, &data);
	if (ret)
		return ret;
	if (data != 1) {
		dev_err(dev, "Flush: Invalid value of input, input=%lu\n",
				data);
		return -EINVAL;
	}

	if (sensors_cdev->sensors_flush == NULL) {
		dev_err(dev, "Invalid sensor class flush handle\n");
		return -EINVAL;
	}
	ret = sensors_cdev->sensors_flush(sensors_cdev);
	if (ret)
		return ret;

	return size;
}

static ssize_t flush_show(struct device *dev,
		struct device_attribute *attr, char *buf)
{
	struct sensors_classdev *sensors_cdev = dev_get_drvdata(dev);

	return snprintf(buf, PAGE_SIZE,
		"Flush handler %s\n",
			(sensors_cdev->sensors_flush == NULL)
				? "not exist" : "exist");
}

static ssize_t enable_wakeup_store(struct device *dev,
		struct device_attribute *attr, const char *buf, size_t size)
{
	struct sensors_classdev *sensors_cdev = dev_get_drvdata(dev);
	ssize_t ret;
	unsigned int enable;

	if (sensors_cdev->sensors_enable_wakeup == NULL) {
		dev_err(dev, "Invalid sensor class enable_wakeup handle\n");
		return -EINVAL;
	}

	ret = kstrtouint(buf, 10, &enable);
	if (ret)
		return ret;

	enable = enable ? 1 : 0;
	ret = sensors_cdev->sensors_enable_wakeup(sensors_cdev, enable);
	if (ret)
		return ret;

	sensors_cdev->wakeup = enable;

	return size;
}

static ssize_t enable_wakeup_show(struct device *dev,
		struct device_attribute *attr, char *buf)
{
	struct sensors_classdev *sensors_cdev = dev_get_drvdata(dev);

	return snprintf(buf, PAGE_SIZE, "%d\n", sensors_cdev->wakeup);
}

static ssize_t calibrate_show(struct device *dev,
		struct device_attribute *atte, char *buf)
{
	struct sensors_classdev *sensors_cdev = dev_get_drvdata(dev);

	if (sensors_cdev->params == NULL) {
		dev_err(dev, "Invalid sensor params\n");
		return -EINVAL;
	}
	return snprintf(buf, PAGE_SIZE, "%s\n", sensors_cdev->params);
}

static ssize_t calibrate_store(struct device *dev,
		struct device_attribute *attr, const char *buf, size_t size)
{
	struct sensors_classdev *sensors_cdev = dev_get_drvdata(dev);
	ssize_t ret = -EINVAL;
	long data;
	int axis, apply_now;
	int cmd, bit_h;

	ret = kstrtol(buf, 0, &data);
	if (ret)
		return ret;
	dev_dbg(dev, "data = %lx\n", data);
	cmd = data & CMD_MASK;
	if (cmd == CMD_DO_CAL) {
		if (sensors_cdev->sensors_calibrate == NULL) {
			dev_err(dev, "Invalid calibrate handle\n");
			return -EINVAL;
		}
		/* parse the data to get the axis and apply_now value*/
		apply_now = (int)(data >> DATA_APPLY_SHIFT) & APPLY_MASK;
		axis = (int)data >> DATA_AXIS_SHIFT;
		dev_dbg(dev, "apply_now = %d, axis = %d\n", apply_now, axis);
		ret = sensors_cdev->sensors_calibrate(sensors_cdev,
				axis, apply_now);
		if (ret)
			return ret;
	} else {
		if (sensors_cdev->sensors_write_cal_params == NULL) {
			dev_err(dev,
					"Invalid write_cal_params handle\n");
			return -EINVAL;
		}
		bit_h = (data & CMD_W_H_L) >> 4;
		if (cmd > CMD_DO_CAL && cmd < CMD_COMPLETE) {
			char *p = (char *)(&sensors_cdev->cal_result)
					+ cal_map[cmd];
			*(int *)p = CMD_GET_PARAMS(bit_h, *(int *)p, data);
		} else if (cmd == CMD_COMPLETE) {
			ret = sensors_cdev->sensors_write_cal_params
				(sensors_cdev, &sensors_cdev->cal_result);
		} else {
			dev_err(dev, "Invalid command\n");
			return -EINVAL;
		}
	}
	return size;
}

static DEVICE_ATTR(name, 0444, name_show, NULL);
static DEVICE_ATTR(vendor, 0444, vendor_show, NULL);
static DEVICE_ATTR(version, 0444, version_show, NULL);
static DEVICE_ATTR(handle, 0444, handle_show, NULL);
static DEVICE_ATTR(type, 0444, type_show, NULL);
static DEVICE_ATTR(max_range, 0444, max_range_show, NULL);
static DEVICE_ATTR(max_delay, 0444, max_delay_show, NULL);
static DEVICE_ATTR(resolution, 0444, resolution_show, NULL);
static DEVICE_ATTR(sensor_power, 0444, sensor_power_show, NULL);
static DEVICE_ATTR(min_delay, 0444, min_delay_show, NULL);
static DEVICE_ATTR(fifo_reserved_event_count, 0444,
	fifo_reserved_event_count_show, NULL);
static DEVICE_ATTR(fifo_max_event_count, 0444, fifo_max_event_count_show, NULL);
static DEVICE_ATTR(enable, 0664, enable_show, enable_store);
static DEVICE_ATTR(poll_delay, 0664, poll_delay_show, poll_delay_store);
static DEVICE_ATTR_RO(self_test);
static DEVICE_ATTR_RW(max_latency);
static DEVICE_ATTR_RW(flush);
static DEVICE_ATTR(calibrate, 0664, calibrate_show, calibrate_store);
static DEVICE_ATTR(enable_wakeup, 0664, enable_wakeup_show,
		enable_wakeup_store);
static DEVICE_ATTR(flags, 0444, flags_show, NULL);

static struct attribute *sensors_attrs[] = {
	&dev_attr_name.attr,
	&dev_attr_vendor.attr,
	&dev_attr_version.attr,
	&dev_attr_handle.attr,
	&dev_attr_type.attr,
	&dev_attr_max_range.attr,
	&dev_attr_max_delay.attr,
	&dev_attr_resolution.attr,
	&dev_attr_sensor_power.attr,
	&dev_attr_min_delay.attr,
	&dev_attr_fifo_reserved_event_count.attr,
	&dev_attr_fifo_max_event_count.attr,
	&dev_attr_enable.attr,
	&dev_attr_poll_delay.attr,
	&dev_attr_self_test.attr,
	&dev_attr_flush.attr,
	&dev_attr_calibrate.attr,
	&dev_attr_enable_wakeup.attr,
	&dev_attr_max_latency.attr,
	&dev_attr_flags.attr,
	NULL,
};

ATTRIBUTE_GROUPS(sensors);

static ssize_t set_flush(struct device *dev, struct device_attribute *attr,
	const char *buf, size_t size)
{
	int64_t dTemp;
	u8 sensor_type = 0;

	if (kstrtoll(buf, 10, &dTemp) < 0)
		return -EINVAL;

	sensor_type = (u8)dTemp;

	input_report_rel(meta_input_dev, REL_DIAL,
		1);	/*META_DATA_FLUSH_COMPLETE*/
	input_report_rel(meta_input_dev, REL_HWHEEL, sensor_type + 1);
	input_sync(meta_input_dev);

	pr_info("[SENSOR] flush %d", sensor_type);
	return size;
}

static struct device_attribute sensor_flush_attr[] = {
	__ATTR(flush, 0220, NULL, set_flush),
};

/**
 * sensors_classdev_register - register a new object of sensors_classdev class.
 * @parent: The device to register.
 * @sensors_cdev: the sensors_classdev structure for this device.
 */
int sensors_classdev_register(struct device *parent,
				struct sensors_classdev *sensors_cdev)
{
	sensors_cdev->dev = device_create(sensors_class, parent, 0,
				      sensors_cdev, "%s", sensors_cdev->name);
	if (IS_ERR(sensors_cdev->dev))
		return PTR_ERR(sensors_cdev->dev);

	down_write(&sensors_list_lock);
	list_add_tail(&sensors_cdev->node, &sensors_list);
	up_write(&sensors_list_lock);

	pr_debug("Registered sensors device: %s\n",
			sensors_cdev->name);
	return 0;
}
EXPORT_SYMBOL(sensors_classdev_register);

/**
 * sensors_classdev_unregister - unregister a object of sensors class.
 * @sensors_cdev: the sensor device to unregister
 * Unregister a previously registered via sensors_classdev_register object.
 */
void sensors_classdev_unregister(struct sensors_classdev *sensors_cdev)
{
	device_unregister(sensors_cdev->dev);
	down_write(&sensors_list_lock);
	list_del(&sensors_cdev->node);
	up_write(&sensors_list_lock);
}
EXPORT_SYMBOL(sensors_classdev_unregister);

int sensors_create_symlink(struct input_dev *inputdev)
{
	int err = 0;

	if (symlink_dev == NULL) {
		pr_err("%s, symlink_dev is NULL!!!\n", __func__);
		return err;
	}

	err = sysfs_create_link(&symlink_dev->kobj,
		&inputdev->dev.kobj, inputdev->name);

	if (err < 0) {
		pr_err("%s, %s failed!(%d)\n", __func__, inputdev->name, err);
		return err;
	}

	return err;
}
EXPORT_SYMBOL_GPL(sensors_create_symlink);

void sensors_remove_symlink(struct input_dev *inputdev)
{

	if (symlink_dev == NULL) {
		pr_err("%s, symlink_dev is NULL!!!\n", __func__);
		return;
	}

	sysfs_delete_link(&symlink_dev->kobj,
		&inputdev->dev.kobj, inputdev->name);
}
EXPORT_SYMBOL_GPL(sensors_remove_symlink);


/*
 * Create sysfs interface
 */
int sensors_register(struct device **dev, void *drvdata,
	struct device_attribute *attributes[], char *name)
{
	int i;
	int ret;

	sensors_class->dev_groups = NULL;
	*dev = device_create(sensors_class, NULL, 0, drvdata, "%s", name);
	sensors_class->dev_groups = sensors_groups;

	if (IS_ERR(*dev)) {
		ret = PTR_ERR(*dev);
		pr_err("%s device_create failed! [%d]\n", __func__, ret);
		return ret;
	}

	for (i = 0; attributes[i] != NULL; i++)
		if ((device_create_file(*dev, attributes[i])) < 0)
			pr_err("%s fail device_create_file %d\n", __func__, i);
	atomic_inc(&sensor_count);

	return 0;
}
EXPORT_SYMBOL_GPL(sensors_register);

void sensors_unregister(struct device *dev,
	struct device_attribute *attributes[])
{
	int i;

	for (i = 0; attributes[i] != NULL; i++)
		device_remove_file(dev, attributes[i]);

	device_unregister(dev);
}
EXPORT_SYMBOL_GPL(sensors_unregister);

int sensors_input_init(void)
{
	int ret;

	/* Meta Input Event Initialization */
	meta_input_dev = input_allocate_device();
	if (!meta_input_dev) {
		pr_err("[SENSOR CORE] failed alloc meta dev\n");
		return -ENOMEM;
	}

	meta_input_dev->name = "meta_event";
	input_set_capability(meta_input_dev, EV_REL, REL_HWHEEL);
	input_set_capability(meta_input_dev, EV_REL, REL_DIAL);

	ret = input_register_device(meta_input_dev);
	if (ret < 0) {
		pr_err("[SENSOR CORE] failed register meta dev\n");
		input_free_device(meta_input_dev);
		return ret;
	}

	ret = sensors_create_symlink(meta_input_dev);
	if (ret < 0) {
		pr_err("[SENSOR CORE] failed create meta symlink\n");
		input_unregister_device(meta_input_dev);
	}

	return ret;
}
static int __init sensors_init(void)
{
	unsigned int i;

	pr_info("[SENSORS CLASS] sensors_class_init\n");

	sensors_class = class_create(THIS_MODULE, "sensors");
	if (IS_ERR(sensors_class)) {
		pr_err("%s, create sensors_class is failed.(err=%d)\n",
			__func__, IS_ERR(sensors_class));
		return PTR_ERR(sensors_class);
	}

	/* For flush sysfs */
	sensor_dev = device_create(sensors_class, NULL, 0, NULL,
		"%s", "sensor_dev");
	if (IS_ERR(sensor_dev)) {
		pr_err("[SENSORS CORE] sensor_dev create failed![%d]\n",
			IS_ERR(sensor_dev));

		class_destroy(sensors_class);
		return PTR_ERR(sensor_dev);
	}

	for (i = 0; i < ARRAY_SIZE(sensor_flush_attr); i++)
		if ((device_create_file(sensor_dev, sensor_flush_attr + i)) < 0)
			pr_err("[SENSOR CORE] failed flush device_file\n");

	/* For symbolic link */
	sensors_event_class = class_create(THIS_MODULE, "sensor_event");
	if (IS_ERR(sensors_event_class)) {
		pr_err("%s, create sensors_class is failed.(err=%d)\n",
			__func__, IS_ERR(sensors_event_class));
		class_destroy(sensors_class);
		return PTR_ERR(sensors_event_class);
	}

	symlink_dev = device_create(sensors_event_class, NULL, 0, NULL,
		"%s", "symlink");
	if (IS_ERR(symlink_dev)) {
		pr_err("[SENSORS CLASS] symlink_dev create failed! [%d]\n",
			IS_ERR(symlink_dev));
		class_destroy(sensors_class);
		class_destroy(sensors_event_class);
		return PTR_ERR(symlink_dev);
	}

	sensors_class->dev_groups = sensors_groups;
	sensors_class->dev_uevent = NULL;
	atomic_set(&sensor_count, 0);

	sensors_input_init();

	pr_info("[SENSORS CLASS] sensors_class_init succcess\n");

	return 0;
}

static void __exit sensors_exit(void)
{
	if (sensors_class || sensors_event_class) {
		class_destroy(sensors_class);
		sensors_class = NULL;
		class_destroy(sensors_event_class);
		sensors_event_class = NULL;
	}
}

subsys_initcall(sensors_init);
module_exit(sensors_exit);
