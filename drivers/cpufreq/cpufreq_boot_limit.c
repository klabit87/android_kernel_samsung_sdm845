#include <linux/cpu.h>
#include <linux/cpufreq.h>
#include <linux/cpufreq_boot_limit.h>
#include <linux/device.h>
#include <linux/platform_device.h>
#include <linux/of_platform.h>
#include <linux/module.h>

#define GET_CLST(cpu)	(cpu >= 4 ? CLST_GOLD : CLST_SILVER)

static int batt_temp;
static int __init get_batt_temp(char *str)
{
	get_option(&str, &batt_temp);
	pr_info("Boot batt_temp is %d\n", batt_temp);

	return 1;
}
__setup("batt_temp=", get_batt_temp);

struct cpufreq_boot_limit cpufreq_boot_limit = {.cur_period = -1,};

static int cpufreq_boot_limit_verify(struct notifier_block *nb,
					unsigned long val, void *data)
{
	struct cpufreq_policy *policy = data;
	u32 max_freq_s, max_freq_g;
	int cluster, cur_period;

	if (val != CPUFREQ_ADJUST)
		return 0;

	if (unlikely(cpufreq_boot_limit.on)) {
		cur_period = cpufreq_boot_limit.cur_period;
		max_freq_s = cpufreq_boot_limit.freq[cur_period][CLST_SILVER];
		max_freq_g = cpufreq_boot_limit.freq[cur_period][CLST_GOLD];
		cluster = GET_CLST(policy->cpu);

		switch (cluster) {
		case CLST_SILVER:
			if (policy->max <= max_freq_s)
				return 0;
			cpufreq_boot_limit.stored_freq[CLST_SILVER] =
				policy->max;
			policy->max = max_freq_s;
			break;
		case CLST_GOLD:
			if (policy->max <= max_freq_g)
				return 0;
			cpufreq_boot_limit.stored_freq[CLST_GOLD] = policy->max;
			policy->max = max_freq_g;
			break;
		default:
			pr_err("%s: invalid cluster for cpu%d\n", __func__,
					policy->cpu);
			return -EINVAL;
		}

		pr_info("%s: changing max freq (cpu%d: %u -> %u)\n",
				__func__, policy->cpu,
				cpufreq_boot_limit.stored_freq[cluster],
				policy->max);
	}

	return 0;
}

static struct notifier_block cpufreq_boot_limit_verify_nb = {
	.notifier_call = cpufreq_boot_limit_verify,
	.priority = -1,
};

static void cpufreq_boot_limit_finish(void)
{
	struct cpufreq_policy *policy;
	u32 stored_freq;
	int cpu;

	cpufreq_unregister_notifier(&cpufreq_boot_limit_verify_nb,
			CPUFREQ_POLICY_NOTIFIER);

	del_timer_sync(&cpufreq_boot_limit.timer);
	cpufreq_boot_limit.on = false;

	get_online_cpus();

	for_each_online_cpu(cpu) {
		policy = cpufreq_cpu_get(cpu);
		if (!policy) {
			pr_debug("%s: No policy has been registered for cpu%d yet\n",
					__func__, cpu);
			continue;
		}

		stored_freq = cpufreq_boot_limit.stored_freq[GET_CLST(cpu)];

		if (stored_freq)
			policy->user_policy.max = stored_freq;
		else
			policy->user_policy.max = policy->cpuinfo.max_freq;

		cpufreq_update_policy(cpu);
		cpufreq_cpu_put(policy);
	}

	put_online_cpus();

	pr_info("%s: cpufreq_boot_limit is finished\n", __func__);
}

static void cpufreq_boot_limit_start_period(int period)
{
	struct cpufreq_policy *policy;
	int cpu;

	if (!cpufreq_boot_limit.on) {
		pr_err("%s: cpufreq_boot_limit is turned off\n", __func__);
		return;
	}
	if (period != cpufreq_boot_limit.cur_period + 1) {
		pr_err("%s: invalid period (cur_period %d, req_period %d)\n",
			__func__, cpufreq_boot_limit.cur_period, period);
		return;
	}
	if (period >= cpufreq_boot_limit.num_period)
		return cpufreq_boot_limit_finish();

	cpufreq_boot_limit.cur_period = period;

	mod_timer(&cpufreq_boot_limit.timer, jiffies +
			msecs_to_jiffies(cpufreq_boot_limit.timeout[period]));

	get_online_cpus();

	for_each_online_cpu(cpu) {
		policy = cpufreq_cpu_get(cpu);
		if (!policy) {
			pr_debug("%s: No policy has been registered for cpu%d yet\n",
					__func__, cpu);
			continue;
		}

		policy->user_policy.max =
				cpufreq_boot_limit.freq[period][GET_CLST(cpu)];
		cpufreq_update_policy(cpu);
		cpufreq_cpu_put(policy);
	}

	put_online_cpus();

	pr_info("%s: period %d is started (%dkhz %dkhz) for %d msec\n", __func__,
			period, cpufreq_boot_limit.freq[period][CLST_SILVER],
			cpufreq_boot_limit.freq[period][CLST_GOLD],
			cpufreq_boot_limit.timeout[period]);
}

static void cpufreq_boot_limit_start(void)
{
	int ret;

	ret = cpufreq_register_notifier(&cpufreq_boot_limit_verify_nb,
			CPUFREQ_POLICY_NOTIFIER);
	if (ret) {
		pr_err("%s: Failed to register cpufreq notifier(%d)\n",
				__func__, ret);
		return;
	}

	cpufreq_boot_limit.on = true;

	return cpufreq_boot_limit_start_period(0);
}

static void cpufreq_boot_limit_timeout_work(struct work_struct *work)
{
	cpufreq_boot_limit_start_period(cpufreq_boot_limit.cur_period + 1);
}

static void cpufreq_boot_limit_timeout_handler(unsigned long data)
{
	schedule_work(&cpufreq_boot_limit.timeout_work);
}

static int cpufreq_boot_limit_probe(struct platform_device *pdev)
{
	struct device *dev = &pdev->dev;
	u32 num_period, len_arr;
	const u32 *vec_arr;
	int i, ret = 0;
	static bool limit_done;
	int temperature;

#ifdef CONFIG_SEC_FACTORY
	if (of_property_read_bool(dev->of_node, "limit-for-user-bin")) {
		pr_info("%s: It is for user bin, skip it\n", __func__);
		goto ret;
	}
#else
	if (of_property_read_bool(dev->of_node, "limit-for-factory-bin")) {
		pr_info("%s: It is for factory bin, skip it\n", __func__);
		goto ret;
	}
#endif

	ret = of_property_read_u32(dev->of_node, "limit-temp",
					&temperature);
	if (ret)
		pr_err("%s: Failed to get limit-temp\n", __func__);
	else {
		if (limit_done || temperature < batt_temp) {
			pr_err("%s: skip thermal limit(%d)\n", __func__, temperature);
			goto ret;
		}
		limit_done = true;
	}

	ret = of_property_read_u32(dev->of_node, "num-period",
			&num_period);
	if (ret) {
		pr_err("%s: Failed to get num-period\n", __func__);
		goto invalid_dt;
	}

	vec_arr = of_get_property(dev->of_node, "limit-freq-table",
			&len_arr);
	if (vec_arr == NULL) {
		pr_err("%s: Failed to get limit freq table\n", __func__);
		goto invalid_dt;
	}

	if (len_arr != num_period * sizeof(u32) * 3) {
		pr_err("%s: the length of the limit-freq-table is invalid\n",
				__func__);
		goto invalid_dt;
	}

	if (num_period > MAX_NUM_PERIOD)
		num_period = MAX_NUM_PERIOD;

	cpufreq_boot_limit.num_period = num_period;

	for (i = 0; i < num_period; i++) {
		cpufreq_boot_limit.freq[i][CLST_SILVER] =
						be32_to_cpu(vec_arr[i * 3]);
		cpufreq_boot_limit.freq[i][CLST_GOLD] =
						be32_to_cpu(vec_arr[i * 3 + 1]);
		cpufreq_boot_limit.timeout[i] =	be32_to_cpu(vec_arr[i * 3 + 2]);
		pr_info("%s: period %d is set (%dhz %dhz) for %dmsec\n",
				__func__, i,
				cpufreq_boot_limit.freq[i][CLST_SILVER],
				cpufreq_boot_limit.freq[i][CLST_GOLD],
				cpufreq_boot_limit.timeout[i]);
	}

	INIT_WORK(&cpufreq_boot_limit.timeout_work,
			cpufreq_boot_limit_timeout_work);
	init_timer(&cpufreq_boot_limit.timer);
	cpufreq_boot_limit.timer.function = cpufreq_boot_limit_timeout_handler;

	cpufreq_boot_limit_start();

ret:
	return 0;

invalid_dt:
	return -EINVAL;
}

static const struct of_device_id match_table[] = {
	{ .compatible = "sec,cpufreq-boot-limit" },
	{}
};

static struct platform_driver cpufreq_boot_limit_driver = {
	.probe = cpufreq_boot_limit_probe,
	.driver = {
		.name = "cpufreq-boot-limit",
		.of_match_table = match_table,
		.owner = THIS_MODULE,
	},
};

static int __init cpufreq_boot_limit_init(void)
{
	return platform_driver_register(&cpufreq_boot_limit_driver);
}
arch_initcall(cpufreq_boot_limit_init);
