/*
 * TIMA Uevent Support
 *
 */
#include <linux/uaccess.h>
#include <linux/list.h>
#include <linux/device.h>
#include <linux/kernel.h>
#include <linux/kobject.h>
#include <linux/module.h>
#include <linux/miscdevice.h>
#include <linux/slab.h>
#include <linux/syscalls.h>

#include "tima_uevent.h"

// Forward declaration
struct inode;

// Global declaration
static DEFINE_MUTEX(tima_uevent_mutex);

static struct class *tima_uevent_class;
struct device *tima_uevent_dev;
EXPORT_SYMBOL_GPL(tima_uevent_dev);

static LIST_HEAD(tima_uevent_list);
static DEFINE_SPINLOCK(tima_uevent_list_lock);

static ssize_t show_tima_uevent(struct device *dev,
		struct device_attribute *attr, char *buf)
{
	if (!dev)
		return -ENODEV;

	// TODO: extract TIMA uevent from the uevent list
	return sprintf(buf, "%s\n", "TIMA uevent data");
}

static DEVICE_ATTR(name, 0444, show_tima_uevent, NULL);

static int tima_uevent_release(struct inode *inode, struct file *file)
{
	pr_warn("%s: TIMA uevent driver is released.", __func__);
	module_put(THIS_MODULE);
	return 0;
}

static bool
tima_uevent_validate(void)
{
	pr_err("Validate caller (%s)", current->comm);
	return true;
}

static ssize_t tima_uevent_read(struct file *filp, char __user *buff,
		size_t len, loff_t *offset)
{
	char tima_uevent[80] = "TIMA uevent read";
	char *data;
	int size = strlen(tima_uevent);
	int retval = 0;

	if (!tima_uevent_validate()) {
		pr_err("%s: invalid request.\n", __func__);
		return -EPERM;
	}

	data = kzalloc(sizeof(tima_uevent), GFP_KERNEL);
	if (!data)
		return -ENOMEM;

	spin_lock(&tima_uevent_list_lock);
	snprintf(data, size, "%c%s", size - 2, tima_uevent);
	if (copy_to_user(buff, data, size)) {
		retval = -EFAULT;
		goto out;
	}

out:
	spin_unlock(&tima_uevent_list_lock);
	kfree(data);
	if (retval)
		return retval;
	else
		return size;
}

static ssize_t tima_uevent_write(struct file *filp, const char __user *buff,
		size_t len, loff_t *off)
{
	int retval = 0;
	char *req;

	if (!tima_uevent_validate()) {
		pr_err("%s: invalid request.\n", __func__);
		return -EPERM;
	}

	req = kzalloc(256, GFP_KERNEL);
	if (!req)
		return -ENOMEM;

	/* Copy from user with truncation. */
	if (copy_from_user(req, buff, min_t(size_t, 256, len))) {
		pr_err("%s: copy_from_user failed.\n", __func__);
		retval = -EFAULT;
		goto out;
	}

	/* Terminate the request string if it has been truncated. */
	if (len > 255)
		req[255] = '\0';

	pr_debug("%s: TIMA uevent request (%s).\n", __func__, req);

out:
	kfree(req);
	return retval;
}

const static struct file_operations tima_uevent_fops = {
	.owner = THIS_MODULE,
	.read = tima_uevent_read,
	.write = tima_uevent_write,
	.release = tima_uevent_release
};

struct miscdevice tima_uevent_mdev = {
	.minor = MISC_DYNAMIC_MINOR,
	.name = TIMA_UEVENT_DEV,
	.fops = &tima_uevent_fops,
};

static int __init tima_uevent_init(void)
{
	int retval = 0;

	retval = misc_register(&tima_uevent_mdev);
	if (retval)
		goto out;

	pr_alert("%s\n", __func__);
	tima_uevent_class = class_create(THIS_MODULE, "tima");
	if (IS_ERR(tima_uevent_class)) {
		retval = PTR_ERR(tima_uevent_class);
		goto error;
	}

	tima_uevent_dev = device_create(tima_uevent_class,
			NULL /* parent */, 0 /* dev_t */, NULL /* drvdata */,
			TIMA_UEVENT_DEV);
	if (IS_ERR(tima_uevent_dev)) {
		retval = PTR_ERR(tima_uevent_dev);
		goto error_destroy;
	}

	/* register this tima device with the driver core */
	retval = device_create_file(tima_uevent_dev, &dev_attr_name);
	if (retval)
		goto error_destroy;

	pr_debug("TIMA uevent driver (%s) is initialized.\n", TIMA_UEVENT_DEV);
	return 0;

error_destroy:
	kfree(tima_uevent_dev);
	device_destroy(tima_uevent_class, 0);
error:
	misc_deregister(&tima_uevent_mdev);
out:
	pr_err("%s: TIMA uevent driver initialization failed\n", __FILE__);
	return retval;
}

static void __exit tima_uevent_exit(void)
{
	class_destroy(tima_uevent_class);
	pr_alert("%s\n", __func__);
}

MODULE_AUTHOR("Ken Chen, geng.c@sta.samsung.com");
MODULE_DESCRIPTION("TIMA uevent driver");
MODULE_VERSION("0.1");

module_init(tima_uevent_init);
module_exit(tima_uevent_exit);

/* char *envp[2]; */
/* env[0] = data */
/* env[1] = NULL */
/* kobject_uevent_env(&tima_uevent_dev->kobj, KOBJ_CHANGE, envp); */
