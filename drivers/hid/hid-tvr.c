/*
 * TVR driver for Linux. Based on hidraw
 */

/*
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the Free
 * Software Foundation; either version 2 of the License, or (at your option)
 * any later version.
 */

#include <linux/cdev.h>
#include <linux/poll.h>
#include <linux/sched.h>
#include <linux/module.h>
#include <linux/usb.h>
#include <linux/hidraw.h>
#include <linux/netdevice.h>
#include <linux/interrupt.h>
#include "hid-ids.h"

#define TVR_INTERFACE_PROTOCOL	0

/* number of reports to buffer */
#define TVR_HIDRAW_BUFFER_SIZE 64
#define TVR_HIDRAW_MAX_DEVICES 64

static struct class *tvr_class;
static struct hidraw *tvr_hidraw_table[TVR_HIDRAW_MAX_DEVICES];
static DEFINE_MUTEX(minors_lock);
static DEFINE_SPINLOCK(list_lock);

static int tvr_major;
static struct cdev tvr_cdev;
static int tvr_data_on = 0;
static struct kobject *virtual_dir = NULL;
extern struct kobject *virtual_device_parent(struct device *dev);

static int tvr_report_event(struct hid_device *hid, u8 *data, int len);
static int tvr_connect(struct hid_device *hid);
static void tvr_disconnect(struct hid_device *hid);

static ssize_t tvr_hidraw_read(struct file *file, char __user *buffer, size_t count, loff_t *ppos)
{
	struct hidraw_list *list = file->private_data;
	int ret = 0, len;
	DECLARE_WAITQUEUE(wait, current);

	mutex_lock(&list->read_mutex);

	while (ret == 0) {
		if (list->head == list->tail) {
			add_wait_queue(&list->hidraw->wait, &wait);
			set_current_state(TASK_INTERRUPTIBLE);

			while (list->head == list->tail) {
				if (signal_pending(current)) {
					ret = -ERESTARTSYS;
					break;
				}
				if (!list->hidraw->exist) {
					ret = -EIO;
					break;
				}
				if (file->f_flags & O_NONBLOCK) {
					ret = -EAGAIN;
					break;
				}

				/* allow O_NONBLOCK to work well from other threads */
				mutex_unlock(&list->read_mutex);
				schedule();
				mutex_lock(&list->read_mutex);
				set_current_state(TASK_INTERRUPTIBLE);
			}

			set_current_state(TASK_RUNNING);
			remove_wait_queue(&list->hidraw->wait, &wait);
		}

		if (ret)
			goto out;

		len = list->buffer[list->tail].len > count ?
			count : list->buffer[list->tail].len;

		if (list->buffer[list->tail].value) {
			if (copy_to_user(buffer, list->buffer[list->tail].value, len)) {
				ret = -EFAULT;
				goto out;
			}
			ret = len;
		}

		kfree(list->buffer[list->tail].value);
		list->buffer[list->tail].value = NULL;
		list->tail = (list->tail + 1) & (TVR_HIDRAW_BUFFER_SIZE - 1);
	}
out:
	mutex_unlock(&list->read_mutex);

	return ret;
}

/* The first byte is expected to be a report number.
 * This function is to be called with the minors_lock mutex held */
static ssize_t tvr_hidraw_send_report(struct file *file, const char __user *buffer, size_t count, unsigned char report_type)
{
	unsigned int minor = iminor(file_inode(file));
	struct hid_device *dev;
	__u8 *buf;
	int ret = 0;

	if (!tvr_hidraw_table[minor] || !tvr_hidraw_table[minor]->exist) {
		ret = -ENODEV;
		goto out;
	}

	dev = tvr_hidraw_table[minor]->hid;
	if (!dev) {
		ret = -ENODEV;
		goto out;
	}

	if (count > HID_MAX_BUFFER_SIZE) {
		hid_warn(dev, "tvr - pid %d passed too large report\n",
			 task_pid_nr(current));
		ret = -EINVAL;
		goto out;
	}

	if (count < 2) {
		hid_warn(dev, "tvr - pid %d passed too short report\n",
			 task_pid_nr(current));
		ret = -EINVAL;
		goto out;
	}

	buf = kmalloc(count * sizeof(__u8), GFP_KERNEL);
	if (!buf) {
		ret = -ENOMEM;
		goto out;
	}

	if (copy_from_user(buf, buffer, count)) {
		ret = -EFAULT;
		goto out_free;
	}

	if ((report_type == HID_OUTPUT_REPORT) &&
	    !(dev->quirks & HID_QUIRK_NO_OUTPUT_REPORTS_ON_INTR_EP)) {
		ret = hid_hw_output_report(dev, buf, count);
		/*
		 * compatibility with old implementation of USB-HID and I2C-HID:
		 * if the device does not support receiving output reports,
		 * on an interrupt endpoint, fallback to SET_REPORT HID command.
		 */
		if (ret != -ENOSYS)
			goto out_free;
	}

	ret = hid_hw_raw_request(dev, buf[0], buf, count, report_type,
				HID_REQ_SET_REPORT);

out_free:
	kfree(buf);
out:
	return ret;
}

/* the first byte is expected to be a report number */
static ssize_t tvr_hidraw_write(struct file *file, const char __user *buffer, size_t count, loff_t *ppos)
{
	ssize_t ret;
	mutex_lock(&minors_lock);
	ret = tvr_hidraw_send_report(file, buffer, count, HID_OUTPUT_REPORT);
	mutex_unlock(&minors_lock);	
	return ret;
}

/* This function performs a Get_Report transfer over the control endpoint
 * per section 7.2.1 of the HID specification, version 1.1.  The first byte
 * of buffer is the report number to request, or 0x0 if the defice does not
 * use numbered reports. The report_type parameter can be HID_FEATURE_REPORT
 * or HID_INPUT_REPORT.  This function is to be called with the minors_lock
 *  mutex held. */
static ssize_t tvr_hidraw_get_report(struct file *file, char __user *buffer, size_t count, unsigned char report_type)
{
	unsigned int minor = iminor(file_inode(file));
	struct hid_device *dev;
	__u8 *buf;
	int ret = 0, len;
	unsigned char report_number;

	dev = tvr_hidraw_table[minor]->hid;
	if (!dev) {
		ret = -ENODEV;
		goto out;		
	}

	if (!dev->ll_driver->raw_request) {
		ret = -ENODEV;
		goto out;
	}

	if (count > HID_MAX_BUFFER_SIZE) {
		printk(KERN_WARNING "tvr - hidraw: pid %d passed too large report\n",
				task_pid_nr(current));
		ret = -EINVAL;
		goto out;
	}

	if (count < 2) {
		printk(KERN_WARNING "tvr - hidraw: pid %d passed too short report\n",
				task_pid_nr(current));
		ret = -EINVAL;
		goto out;
	}

	buf = kmalloc(count * sizeof(__u8), GFP_KERNEL);
	if (!buf) {
		ret = -ENOMEM;
		goto out;
	}

	/*
	 * Read the first byte from the user. This is the report number,
	 * which is passed to tvr_hid_hw_raw_request().
	 */
	if (copy_from_user(&report_number, buffer, 1)) {
		ret = -EFAULT;
		goto out_free;
	}

	ret = hid_hw_raw_request(dev, report_number, buf, count, report_type,
				 HID_REQ_GET_REPORT);

	if (ret < 0)
		goto out_free;

	len = (ret < count) ? ret : count;

	if (copy_to_user(buffer, buf, len)) {
		ret = -EFAULT;
		goto out_free;
	}

	ret = len;

out_free:
	kfree(buf);
out:
	return ret;
}

static unsigned int tvr_hidraw_poll(struct file *file, poll_table *wait)
{
	struct hidraw_list *list = file->private_data;

	poll_wait(file, &list->hidraw->wait, wait);
	if (list->head != list->tail)
		return POLLIN | POLLRDNORM;
	if (!list->hidraw->exist)
		return POLLERR | POLLHUP;
	return 0;
}

static int tvr_hidraw_open(struct inode *inode, struct file *file)
{
	unsigned int minor = iminor(inode);
	struct hidraw *dev;
	struct hidraw_list *list;
	int err = 0;

	if (!(list = kzalloc(sizeof(struct hidraw_list), GFP_KERNEL))) {
		err = -ENOMEM;
		goto out;
	}

	mutex_lock(&minors_lock);
	if (!tvr_hidraw_table[minor]) {
		err = -ENODEV;
		goto out_unlock;
	}

	printk("TVR: open %d (%d:%s) >>>\n", minor, current->pid, current->comm);

	list->hidraw = tvr_hidraw_table[minor];
	mutex_init(&list->read_mutex);

	spin_lock_irq(&list_lock);
	list_add_tail(&list->node, &tvr_hidraw_table[minor]->list);
	spin_unlock_irq(&list_lock);

	file->private_data = list;

	dev = tvr_hidraw_table[minor];
	dev->open++;

	printk("TVR: open(%d) err %d <<<\n", dev->open, err);

out_unlock:
	mutex_unlock(&minors_lock);
out:
	if (err < 0)
		kfree(list);
	return err;
}

static int tvr_hidraw_fasync(int fd, struct file *file, int on)
{
	struct hidraw_list *list = file->private_data;

	return fasync_helper(fd, file, on, &list->fasync);
}

static int tvr_hidraw_release(struct inode * inode, struct file * file)
{
	unsigned int minor = iminor(inode);
	struct hidraw *dev;
	struct hidraw_list *list = file->private_data;
	int ret;
	int i;
	unsigned long flags;

	mutex_lock(&minors_lock);
	if (!tvr_hidraw_table[minor]) {
		ret = -ENODEV;
		goto unlock;
	}

	printk("TVR: release %d (%d:%s) >>>\n", minor, current->pid, current->comm);

	spin_lock_irqsave(&list_lock, flags);
	list_del(&list->node);
	spin_unlock_irqrestore(&list_lock, flags);

	dev = tvr_hidraw_table[minor];
	--dev->open;

	if (!dev->open) {
		if (!list->hidraw->exist) {
			printk("TVR: freed tvr_hidraw_table %d\n", minor);
			kfree(list->hidraw);
			tvr_hidraw_table[minor] = NULL;
		}
	}

	for (i = 0; i < TVR_HIDRAW_BUFFER_SIZE; ++i)
		kfree(list->buffer[i].value);
	kfree(list);
	ret = 0;

	printk("TVR: release <<<\n");

unlock:
	mutex_unlock(&minors_lock);

	return ret;
}

static int tvr_report_event(struct hid_device *hid, u8 *data, int len)
{
	struct hidraw *dev;
	struct hidraw_list *list;
	int ret = 0;
	unsigned long flags;

	dev = tvr_hidraw_table[hid->minor];

	spin_lock_irqsave(&list_lock, flags);
	list_for_each_entry(list, &dev->list, node) {
		int new_head = (list->head + 1) & (TVR_HIDRAW_BUFFER_SIZE - 1);

		if (new_head == list->tail)
			continue;

		if (!(list->buffer[list->head].value = kmemdup(data, len, GFP_ATOMIC))) {
			ret = -ENOMEM;
			spin_unlock_irqrestore(&list_lock, flags);
			break;
		}

		list->buffer[list->head].len = len;
		list->head = new_head;
		kill_fasync(&list->fasync, SIGIO, POLL_IN);
	}
	spin_unlock_irqrestore(&list_lock, flags);

	wake_up_interruptible(&dev->wait);

	return ret;
}

static int tvr_connect(struct hid_device *hid)
{
	int minor, result;
	struct hidraw *dev;

	/* we accept any HID device, no matter the applications */
	dev = kzalloc(sizeof(struct hidraw), GFP_KERNEL);
	if (!dev)
		return -ENOMEM;

	result = -EINVAL;

	mutex_lock(&minors_lock);

	for (minor = 0; minor < TVR_HIDRAW_MAX_DEVICES; minor++)
	{
		if (tvr_hidraw_table[minor]) {
			printk("TVR: old tvr_hidraw_table %d\n", minor);
			continue;
		}

		tvr_hidraw_table[minor] = dev;
		result = 0;
		break;
	}

	printk("TVR: connect %d %d (%d:%s) >>>\n", minor, result, current->pid, current->comm);

	if (result) {
		mutex_unlock(&minors_lock);
		kfree(dev);
		goto out;
	}

	dev->dev = device_create(tvr_class, &hid->dev, MKDEV(tvr_major, minor),
					 NULL, "%s%d", "tvr", minor);

	if (IS_ERR(dev->dev)) {
		tvr_hidraw_table[minor] = NULL;
		mutex_unlock(&minors_lock);
		result = PTR_ERR(dev->dev);
		kfree(dev);
		goto out;
	}

	printk("TVR: connect <<<\n");

	mutex_unlock(&minors_lock);
	init_waitqueue_head(&dev->wait);
	INIT_LIST_HEAD(&dev->list);

	dev->hid = hid;
	dev->minor = minor;

	dev->exist = 1;

out:
	return result;
}

static void tvr_disconnect(struct hid_device *hid)
{
	struct hidraw *hidraw;

	hidraw = tvr_hidraw_table[hid->minor];

	mutex_lock(&minors_lock);

	printk("TVR: disconnect %d %d (%d:%s) >>>\n", hidraw->minor, hidraw->open, current->pid, current->comm);

	hidraw->exist = 0;

	device_destroy(tvr_class, MKDEV(tvr_major, hidraw->minor));

	if (hidraw->open) {
		wake_up_interruptible(&hidraw->wait);
	} else {
		printk("TVR: freed tvr_hidraw_table %d\n", hidraw->minor);
		tvr_hidraw_table[hidraw->minor] = NULL;
		kfree(hidraw);
	}

	printk("TVR: disconnect <<<\n");

	mutex_unlock(&minors_lock);
}

static long tvr_hidraw_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{
	struct inode *inode = file->f_path.dentry->d_inode;
	unsigned int minor = iminor(inode);
	long ret = 0;
	struct hidraw *dev;
	void __user *user_arg = (void __user*) arg;

	mutex_lock(&minors_lock);
	dev = tvr_hidraw_table[minor];
	if (!dev || (dev && !dev->exist)) {
		ret = -ENODEV;
		goto out;
	}

	switch (cmd) {
		case HIDIOCGRDESCSIZE:
			if (dev->hid) {
				if (put_user(dev->hid->rsize, (int __user *)arg))
					ret = -EFAULT;
			} else {
				ret = -EFAULT;
			}
			break;

		case HIDIOCGRDESC:
			{
				if (dev->hid) {
					__u32 len;

					if (get_user(len, (int __user *)arg))
						ret = -EFAULT;
					else if (len > HID_MAX_DESCRIPTOR_SIZE - 1)
						ret = -EINVAL;
					else if (copy_to_user(user_arg + offsetof(
						struct hidraw_report_descriptor,
						value[0]),
						dev->hid->rdesc,
						min(dev->hid->rsize, len)))
						ret = -EFAULT;
				} else {
					ret = -EFAULT;
				}
				break;
			}
		case HIDIOCGRAWINFO:
			{
				struct hidraw_devinfo dinfo;

				dinfo.bustype = dev->hid->bus;
				dinfo.vendor = dev->hid->vendor;
				dinfo.product = dev->hid->product;

				if (copy_to_user(user_arg, &dinfo, sizeof(dinfo)))
					ret = -EFAULT;
				break;
			}
		default:
			{
				struct hid_device *hid = dev->hid;
				if (_IOC_TYPE(cmd) != 'H') {
					ret = -EINVAL;
					break;
				}

				if (_IOC_NR(cmd) == _IOC_NR(HIDIOCSFEATURE(0))) {
					int len = _IOC_SIZE(cmd);

					ret = tvr_hidraw_send_report(file, user_arg, len, HID_FEATURE_REPORT);

					break;
				}
				if (_IOC_NR(cmd) == _IOC_NR(HIDIOCGFEATURE(0))) {
					int len = _IOC_SIZE(cmd);

					ret = tvr_hidraw_get_report(file, user_arg, len, HID_FEATURE_REPORT);

					break;
				}

				/* Begin Read-only ioctls. */
				if (_IOC_DIR(cmd) != _IOC_READ) {
					ret = -EINVAL;
					break;
				}

				if (_IOC_NR(cmd) == _IOC_NR(HIDIOCGRAWNAME(0))) {

					int len = strlen(hid->name) + 1;
					if (len > _IOC_SIZE(cmd))
						len = _IOC_SIZE(cmd);
					ret = copy_to_user(user_arg, hid->name, len) ?
						-EFAULT : len;

					break;
				}

				if (_IOC_NR(cmd) == _IOC_NR(HIDIOCGRAWPHYS(0))) {

					int len = strlen(hid->phys) + 1;
					if (len > _IOC_SIZE(cmd))
						len = _IOC_SIZE(cmd);
					ret = copy_to_user(user_arg, hid->phys, len) ?
						-EFAULT : len;
					break;
				}
			}

		ret = -ENOTTY;
	}
out:
	mutex_unlock(&minors_lock);
	return ret;
}

static const struct file_operations tvr_ops = {
	.owner = THIS_MODULE,
	.read = tvr_hidraw_read,
	.write = tvr_hidraw_write,
	.poll = tvr_hidraw_poll,
	.open = tvr_hidraw_open,
	.release = tvr_hidraw_release,
	.unlocked_ioctl = tvr_hidraw_ioctl,
#ifdef CONFIG_COMPAT
	.compat_ioctl   = tvr_hidraw_ioctl,
#endif
	.fasync = tvr_hidraw_fasync,
	.llseek = noop_llseek,
};

static int tvr_probe(struct hid_device *hdev, const struct hid_device_id *id)
{
	int retval;

	struct usb_interface *intf = to_usb_interface(hdev->dev.parent);

	retval = hid_parse(hdev);
	if (retval) {
		hid_err(hdev, "tvr - parse failed\n");
		goto exit;
	}

	retval = hid_hw_start(hdev, HID_CONNECT_DEFAULT);
	if (retval) {
		hid_err(hdev, "tvr - hw start failed\n");
		goto exit;
	}

	if (!intf || intf->cur_altsetting->desc.bInterfaceProtocol
			!= TVR_INTERFACE_PROTOCOL) {
		return 0;
	}
	retval = tvr_connect(hdev);

	if (retval) {
		hid_err(hdev, "tvr - Couldn't connect\n");
		goto exit_stop;
	}

	retval = hid_hw_power(hdev, PM_HINT_FULLON);
	if (retval < 0) {
		hid_err(hdev, "tvr - Couldn't feed power\n");
		tvr_disconnect(hdev);
		goto exit_stop;
	}

	retval = hid_hw_open(hdev);
	if (retval < 0) {
		hid_err(hdev, "tvr - Couldn't open hid\n");
		hid_hw_power(hdev, PM_HINT_NORMAL);
		tvr_disconnect(hdev);
		goto exit_stop;
	}

	return 0;

exit_stop:
	hid_hw_stop(hdev);
exit:
	return retval;
}

static void tvr_remove(struct hid_device *hdev)
{
	struct usb_interface *intf = to_usb_interface(hdev->dev.parent);
	if (intf->cur_altsetting->desc.bInterfaceProtocol
			!= TVR_INTERFACE_PROTOCOL) {
		hid_hw_stop(hdev);
		return;
	}

	hid_hw_close(hdev);

	hid_hw_power(hdev, PM_HINT_NORMAL);

	tvr_disconnect(hdev);

	hid_hw_stop(hdev);
}

static int tvr_raw_event(struct hid_device *hdev, struct hid_report *report, u8 *data, int size)
{
	int retval = 0;

	struct usb_interface *intf = to_usb_interface(hdev->dev.parent);
	if (intf->cur_altsetting->desc.bInterfaceProtocol
			!= TVR_INTERFACE_PROTOCOL) {
		return 0;
	}

	if (!tvr_data_on) {
		if (size >= 1 && data[0] == 1)
			return 0;
	}

	retval = tvr_report_event(hdev, data, size);
	if (retval < 0)
		printk("TVR: raw event err %d\n", retval);

	return retval;
}

static ssize_t data_on_show(struct device *dev, struct device_attribute *attr, char *buf)
{
	return sprintf(buf, "%d\n", tvr_data_on);
}
static ssize_t data_on_store(struct device *dev, struct device_attribute *attr, const char *buf, size_t count)
{
	unsigned long val;

	if (kstrtoul(buf, 0, &val))
		return -EINVAL;

	tvr_data_on = val;

	return count;
}

static DEVICE_ATTR(data_on, 0664, data_on_show, data_on_store);
static struct device_attribute *tvr_attrs[] = {
	&dev_attr_data_on,
	NULL,
};


static const struct hid_device_id tvr_devices[] = {
	{ HID_USB_DEVICE(USB_VENDOR_ID_SAMSUNG_ELECTRONICS, USB_DEVICE_ID_SAMSUNG_TVR_1) },
	{ }
};

MODULE_DEVICE_TABLE(hid, tvr_devices);

static struct hid_driver tvr_driver = {
		.name = "tvr",
		.id_table = tvr_devices,
		.probe = tvr_probe,
		.remove = tvr_remove,
		.raw_event = tvr_raw_event
};

static int __init tvr_init(void)
{
	int retval = 0;
	dev_t dev_id;

	tvr_class = class_create(THIS_MODULE, "tvr");
	if (IS_ERR(tvr_class)) {
		return PTR_ERR(tvr_class);
	}

	retval = hid_register_driver(&tvr_driver);
	if (retval < 0) {
		pr_warn("tvr_init - Can't register drive.\n");
		goto out_class;
	}

	retval = alloc_chrdev_region(&dev_id, 0,
			TVR_HIDRAW_MAX_DEVICES, "tvr");
	if (retval < 0) {
		pr_warn("tvr_init - Can't allocate chrdev region.\n");
		goto out_register;
	}

	tvr_major = MAJOR(dev_id);
	cdev_init(&tvr_cdev, &tvr_ops);
	cdev_add(&tvr_cdev, dev_id, TVR_HIDRAW_MAX_DEVICES);

	virtual_dir = virtual_device_parent(NULL);
	if (virtual_dir) {
    	retval = sysfs_create_file(virtual_dir, &tvr_attrs[0]->attr);
    	if (retval) {
    		pr_warn("tvr_init - failed sysfs_create_file\n");
    		kobject_put(virtual_dir);
    		virtual_dir = NULL;
    	}
	} else {
		pr_warn("tvr_init - failed virtual_device_parent\n");
	}

	return 0;

out_register:
	hid_unregister_driver(&tvr_driver);

out_class:
	class_destroy(tvr_class);

	return retval;
}

static void __exit tvr_exit(void)
{
	dev_t dev_id = MKDEV(tvr_major, 0);

	cdev_del(&tvr_cdev);

	unregister_chrdev_region(dev_id, TVR_HIDRAW_MAX_DEVICES);

	hid_unregister_driver(&tvr_driver);

	class_destroy(tvr_class);

	sysfs_remove_file(virtual_dir, &tvr_attrs[0]->attr);
	kobject_put(virtual_dir);
	virtual_dir = NULL;    
}

module_init(tvr_init);
module_exit(tvr_exit);

MODULE_DESCRIPTION("USB TVR device driver.");
MODULE_LICENSE("GPL v2");
