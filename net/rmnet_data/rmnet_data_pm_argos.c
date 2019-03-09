/*
 * rmnet_data_pm_argos.c
 *
 * Copyright (c) 2012 Samsung Electronics Co., Ltd
 *              http://www.samsung.com
 *
 *  This program is free software; you can redistribute  it and/or modify it
 *  under  the terms of  the GNU General  Public License as published by the
 *  Free Software Foundation;  either version 2 of the  License, or (at your
 *  option) any later version.
 *
 */

#include <linux/string.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/cpumask.h>
#include <linux/sec_argos.h>
#include <linux/netdevice.h>
#include <linux/pm_qos.h>

static struct pm_qos_request rmnet_qos_req;
#define PM_QOS_RMNET_LATENCY_VALUE 44

#define RMNET_DATA_MAX_VND	8
const char *ndev_prefix = "rmnet_data";

#define ARGOS_IPA_LABEL "IPA"

/* tuning value for rps map */
#define ARGOS_RMNET_RPS_BIG_MASK "c0" /* big core 2, 3 */
#define ARGOS_RMNET_RPS_DEFAULT_MASK "0d" /* default */

/* rps boosting trigger value in Mbps */
#define ARGOS_RMNET_RPS_BOOST_MBPS 300
static unsigned int rmnet_rps_boost_mbps = ARGOS_RMNET_RPS_BOOST_MBPS;
module_param(rmnet_rps_boost_mbps, uint, 0644);
MODULE_PARM_DESC(rmnet_rps_boost_mbps, "Rps Boost Threshold");

#ifdef CONFIG_RPS

/*
 * reference from net/core/net-sysfs.c store_rps_map()
 */
static int rmnet_data_pm_change_rps_map(struct netdev_rx_queue *queue,
		      const char *buf, size_t len)
{
	struct rps_map *old_map, *map;
	cpumask_var_t mask;
	int err, cpu, i;
	static DEFINE_MUTEX(rps_map_mutex);

	if (!alloc_cpumask_var(&mask, GFP_KERNEL)){
		pr_err("rmnet_data_pm_change_rps_map alloc_cpumask_var fail\n");
		return -ENOMEM;
	}
	err = bitmap_parse(buf, len, cpumask_bits(mask), nr_cpumask_bits);
	if (err) {
		free_cpumask_var(mask);
		pr_err("rmnet_data_pm_change_rps_map bitmap_parse err %d\n", err);
		return err;
	}

	map = kzalloc(max_t(unsigned long,
	    RPS_MAP_SIZE(cpumask_weight(mask)), L1_CACHE_BYTES),
	    GFP_KERNEL);
	if (!map) {
		free_cpumask_var(mask);
		pr_err("rmnet_data_pm_change_rps_map kzalloc fail\n");
		return -ENOMEM;
	}

	i = 0;
	for_each_cpu_and(cpu, mask, cpu_online_mask) {
		map->cpus[i++] = cpu;
	}

	if (i) {
		map->len = i;
	} else {
		kfree(map);
		map = NULL;

		free_cpumask_var(mask);
		pr_err("failed to map rps_cpu\n");
		return -EINVAL;
	}

	mutex_lock(&rps_map_mutex);
	old_map = rcu_dereference_protected(queue->rps_map,
					    mutex_is_locked(&rps_map_mutex));
	rcu_assign_pointer(queue->rps_map, map);

	if (map)
		static_key_slow_inc(&rps_needed);
	if (old_map)
		static_key_slow_dec(&rps_needed);

	mutex_unlock(&rps_map_mutex);

	if (old_map)
		kfree_rcu(old_map, rcu);

	free_cpumask_var(mask);
	return map->len;
}
#else
#define rmnet_data_pm_change_rps_map(queue, buf, len) do { } while (0)
#endif /* CONFIG_RPS */

static int rmnet_data_pm_set_rps(char *buf, int len)
{
	char ndev_name[IFNAMSIZ];
	struct net_device *ndev;
	int i, ret = 0;

	/* get rx_queue from net devices pointer */
	for (i = 0; i < RMNET_DATA_MAX_VND; i++) {
		memset(ndev_name, 0, IFNAMSIZ);
		snprintf(ndev_name, IFNAMSIZ, "%s%d", ndev_prefix, i);

		ndev = dev_get_by_name(&init_net, ndev_name);
		if (!ndev) {
			pr_info("Cannot find %s from init_net\n", ndev_name);
			continue;
		}

		ret = rmnet_data_pm_change_rps_map(ndev->_rx, buf, len);
		if (ret < 0)
			pr_err("set rps %s : %s, err %d\n", ndev->name, buf, ret);

		dev_put(ndev);
	}

	return ret;
}

/* event value notifies speed in matching argos table */
static bool rmnet_data_pm_in_boost;
static int rmnet_data_pm_argos_cb(struct notifier_block *nb,
				  unsigned long speed, void *data)
{
	pr_info("%s in speed %lu Mbps\n", __func__, speed);
	if (rmnet_data_pm_in_boost) {
		if (speed >= rmnet_rps_boost_mbps)
			return NOTIFY_DONE;

		pr_info("Speed: %luMbps, %s -> %s\n",
			speed, ARGOS_RMNET_RPS_BIG_MASK, ARGOS_RMNET_RPS_DEFAULT_MASK);

		/* reset to default value */
		rmnet_data_pm_set_rps(ARGOS_RMNET_RPS_DEFAULT_MASK,
			strlen(ARGOS_RMNET_RPS_DEFAULT_MASK));

		pm_qos_remove_request(&rmnet_qos_req);

		rmnet_data_pm_in_boost = false;
	} else {
		if (speed < rmnet_rps_boost_mbps)
			return NOTIFY_DONE;
		pr_info("%s in speed %lu Mbps\n", __func__, speed);

		pr_info("Speed: %luMbps, %s -> %s\n",
			speed, ARGOS_RMNET_RPS_DEFAULT_MASK, ARGOS_RMNET_RPS_BIG_MASK);

		pm_qos_add_request(&rmnet_qos_req, PM_QOS_CPU_DMA_LATENCY,
				   PM_QOS_RMNET_LATENCY_VALUE);

		rmnet_data_pm_set_rps(ARGOS_RMNET_RPS_BIG_MASK,
			strlen(ARGOS_RMNET_RPS_BIG_MASK));

		rmnet_data_pm_in_boost = true;
	}

	return NOTIFY_DONE;
}

static struct notifier_block rmnet_data_nb = {
	.notifier_call = rmnet_data_pm_argos_cb,
};

static int __init rmnet_data_pm_argos_init(void)
{
	int ret = sec_argos_register_notifier(&rmnet_data_nb, ARGOS_IPA_LABEL);
	if (ret) {
		pr_err("Fail to register rmnet_data pm argos notifier block\n");
	}
	return ret;
}

static void __exit rmnet_data_pm_argos_exit(void)
{
	sec_argos_unregister_notifier(&rmnet_data_nb, ARGOS_IPA_LABEL);
}

module_init(rmnet_data_pm_argos_init);
module_exit(rmnet_data_pm_argos_exit);
MODULE_LICENSE("GPL");
