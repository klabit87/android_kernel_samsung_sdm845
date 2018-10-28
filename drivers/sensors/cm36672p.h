#ifndef __LINUX_CM36672P_H
#define __CM36672P_H__

#include <linux/types.h>

#ifdef __KERNEL__
struct cm36672p_platform_data {
	int irq;		/* proximity-sensor irq gpio */

	int default_hi_thd;
	int default_low_thd;
	int cancel_hi_thd;
	int cancel_low_thd;
	int offset_range_hi;
	int offset_range_low;
	int default_trim;

	int vdd_always_on; /* 1: vdd is always on, 0: enable only when proximity is on */
	int vled_ldo; /*0: vled(anode) source regulator, other: get power by LDO control */
	int regulator_divided; /* 1: vdd & vled uses divided circuit, 0: vdd & vled uses seperate circuit */
	int vio_ldo;
};

#define SENSOR_ERR(fmt, ...) \
	pr_err("[SENSOR] %s: "fmt, __func__, ##__VA_ARGS__)

#define SENSOR_INFO(fmt, ...) \
	pr_info("[SENSOR] %s: "fmt, __func__, ##__VA_ARGS__)

#define SENSOR_WARN(fmt, ...) \
	pr_warn("[SENSOR] %s: "fmt, __func__, ##__VA_ARGS__)

extern int sensors_create_symlink(struct input_dev *inputdev);
extern void sensors_remove_symlink(struct input_dev *inputdev);
extern int sensors_register(struct device **dev, void *drvdata,
	struct device_attribute *attributes[], char *name);
extern void sensors_unregister(struct device *dev,
	struct device_attribute *attributes[]);
#endif
#endif
