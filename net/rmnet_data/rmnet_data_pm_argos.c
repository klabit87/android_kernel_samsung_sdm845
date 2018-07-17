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

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/cpumask.h>
#include <linux/sec_argos.h>
#include <linux/netdevice.h>

#include "rmnet_data_private.h"
#include "rmnet_data_vnd.h"

#define ARGOS_IPA_LABEL "IPA"
/* tuning value for rps map */
#define ARGOS_RMNET_RPS_MASK "60" /* big core 2, 3 */

/* load default rps map for restore */
static char rmnet_default_rps_map[4];
static int rmnet_default_rps_map_len;

/* rps boosting trigger value in Mbps */
#define ARGOS_RMNET_RPS_BOOST_MBPS 100
static unsigned int rmnet_rps_boost_mbps = ARGOS_RMNET_RPS_BOOST_MBPS;
module_param(rmnet_rps_boost_mbps, uint, 0644);
MODULE_PARM_DESC(rmnet_rps_boost_mbps, "Rps Boost Threshold");

#ifdef CONFIG_RPS
/*
 * reference from net/core/net-sysfs.c show_rps_map()
 */
static int rmnet_data_pm_get_default_rps_map(void)
{
	struct net_device *vndev;
	struct netdev_rx_queue *queue;
	struct rps_map *map;
	cpumask_var_t mask;
	int i, len;

	/* get rx_queueu from net devices pointer */
	for (i = 0; i < RMNET_DATA_MAX_VND; i++) {
		vndev = rmnet_vnd_get_by_id(i);
		if (vndev)
			break;
	}

	if (!vndev)
		return -ENODEV;

	queue = vndev->_rx;

	if (!zalloc_cpumask_var(&mask, GFP_KERNEL))
		return -ENOMEM;

	rcu_read_lock();
	map = rcu_dereference(queue->rps_map);
	if (map)
		for (i = 0; i < map->len; i++)
			cpumask_set_cpu(map->cpus[i], mask);

	len = snprintf(rmnet_default_rps_map,
		       sizeof(rmnet_default_rps_map),
		       "%*pb\n", cpumask_pr_args(mask));

	rmnet_default_rps_map_len = len;

	rcu_read_unlock();
	free_cpumask_var(mask);

	return len;
}


/*
 * reference from net/core/net-sysfs.c store_rps_map()
 */
static int rmnet_data_pm_change_rps_map(struct netdev_rx_queue *queue,
		      const char *buf, int len)
{
	struct rps_map *old_map, *map;
	cpumask_var_t mask;
	int err, cpu, i;
	static DEFINE_MUTEX(rps_map_mutex);

	if (!alloc_cpumask_var(&mask, GFP_KERNEL))
		return -ENOMEM;

	err = bitmap_parse(buf, len, cpumask_bits(mask), nr_cpumask_bits);
	if (err) {
		free_cpumask_var(mask);
		return err;
	}

	map = kzalloc(max_t(unsigned int,
	    RPS_MAP_SIZE(cpumask_weight(mask)), L1_CACHE_BYTES),
	    GFP_KERNEL);
	if (!map) {
		free_cpumask_var(mask);
		return -ENOMEM;
	}

	i = 0;
	for_each_cpu_and(cpu, mask, cpu_online_mask)
		map->cpus[i++] = cpu;

	if (i)
		map->len = i;
	else {
		kfree(map);
		map = NULL;
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
	return len;
}
#else
#define rmnet_data_pm_get_default_rps_map(queue) do { } while (0)
#define rmnet_data_pm_change_rps_map(queue, buf, len) do { } while (0)
#endif /* CONFIG_RPS */

static int rmnet_data_pm_set_rps(char *buf, int len)
{
	struct net_device *vndev;
	int i, ret = 0;
	/* get rx_queue from net devices pointer */
	for (i = 0; i < RMNET_DATA_MAX_VND; i++) {
		vndev = rmnet_vnd_get_by_id(i);
		if (!vndev)
			continue;
		ret = rmnet_data_pm_change_rps_map(vndev->_rx, buf, len);
		if (ret < 0)
			pr_err("set rps %s : %s, err %d\n",
							vndev->name, buf, ret);
	};
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

		/* reset to default value */
		rmnet_data_pm_set_rps(rmnet_default_rps_map,
						rmnet_default_rps_map_len);
		rmnet_data_pm_in_boost = false;
	} else {
		if (speed < rmnet_rps_boost_mbps)
			return NOTIFY_DONE;

		if (rmnet_data_pm_get_default_rps_map() < 0)
			return NOTIFY_DONE;

		rmnet_data_pm_in_boost = true;

		rmnet_data_pm_set_rps(ARGOS_RMNET_RPS_MASK,
						sizeof(ARGOS_RMNET_RPS_MASK));
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
