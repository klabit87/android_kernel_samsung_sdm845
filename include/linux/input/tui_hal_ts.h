
#ifndef __LINUX_TUI_HAL_TS_H_
#define __LINUX_TUI_HAL_TS_H_

int register_tui_hal_ts(struct device *dev,
			atomic_t *st_enabled,
			struct completion *st_irq_received,
			int (*secure_get_irq)(struct device *dev),
			ssize_t (*secure_touch_enable_store)(struct device *dev,
						struct device_attribute *attr,
						const char *buf, size_t count)
);

#endif /* __LINUX_TUI_HAL_TS_H_ */

