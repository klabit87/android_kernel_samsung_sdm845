#ifndef _LINUX_CPUFREQ_BOOT_LIMIT_H
#define _LINUX_CPUFREQ_BOOT_LIMIT_H

#include <linux/types.h>
#include <linux/timer.h>
#include <linux/workqueue.h>

#define MAX_NUM_PERIOD 10

struct cpufreq_boot_limit {
	bool on;
	u32 freq[MAX_NUM_PERIOD][2];
	int timeout[MAX_NUM_PERIOD];
	u32 num_period;
	u32 cur_period;
	u32 stored_freq[2];
	struct timer_list timer;
	struct work_struct timeout_work;
};

enum {
	CLST_SILVER,
	CLST_GOLD,
	CLST_MAX
};

#endif /* _LINUX_CPUFREQ_BOOT_LIMIT_H */
