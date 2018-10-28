/*
 * drivers/regulator/cpr3-util-sec.c
 *
 * COPYRIGHT(C) 2017 Samsung Electronics Co., Ltd. All Right Reserved.
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

#define pr_fmt(fmt)     KBUILD_MODNAME ":%s() " fmt, __func__

#include <linux/slab.h>

#include <linux/sec_debug_partition.h>

#include "cpr3-regulator.h"

static struct {
	int fuse_corner_count;
	int speed_bin_fuse;
	int cpr_rev_fuse;
	int *fuse_volt;
} apps_cpr_saved_info[MAX_CLUSTER_NUM];

int cpr3_save_fused_open_loop_voltage(struct cpr3_regulator *vreg,
				int *fuse_volt)
{
	int id;

	if (unlikely(!vreg)) {
		cpr3_err(vreg, "failed to save fused open loop voltage.\n");
		return -EINVAL;
	}

	id = vreg->thread->ctrl->ctrl_id;
	if (!strcmp(vreg->name, "apc0_l3_corner")) {
		cpr3_err(vreg, "skip apc0_l3_corner (%d)\n", id);
		return -EINVAL;
	}

	if (unlikely(id >= MAX_CLUSTER_NUM)) {
		cpr3_err(vreg,
			 "failed to save fused open loop voltage. id(%d)\n",
			 id);
		return -EINVAL;
	}

	if (unlikely(apps_cpr_saved_info[id].fuse_volt)) {
		cpr3_err(vreg,
			 "failed to save fused open loop voltage. id(%d), fuse_volt(%p)\n",
			 id, apps_cpr_saved_info[id].fuse_volt);
		return -EINVAL;
	}

	apps_cpr_saved_info[id].fuse_corner_count = vreg->fuse_corner_count;
	apps_cpr_saved_info[id].speed_bin_fuse = vreg->speed_bin_fuse;
	apps_cpr_saved_info[id].cpr_rev_fuse = vreg->cpr_rev_fuse;
	apps_cpr_saved_info[id].fuse_volt = kcalloc(vreg->fuse_corner_count,
				sizeof(*fuse_volt), GFP_KERNEL);

	memcpy((void *)apps_cpr_saved_info[id].fuse_volt, (void *)fuse_volt,
		vreg->fuse_corner_count * sizeof(*fuse_volt));

	return 0;
}

int cpr3_get_fuse_open_loop_voltage(int id, int fuse_corner)
{
	if (unlikely(!apps_cpr_saved_info[id].fuse_volt)) {
		pr_err("cpr info isn't saved. id(%d)\n", id);
		return -EINVAL;
	}

	if (fuse_corner >= apps_cpr_saved_info[id].fuse_corner_count) {
		pr_err("cpr info isn't saved. id(%d), corner(%d)\n",
			id, fuse_corner);
		return -EINVAL;
	}

	return apps_cpr_saved_info[id].fuse_volt[fuse_corner];
}

int cpr3_get_fuse_corner_count(int id)
{
	if (unlikely(!apps_cpr_saved_info[id].fuse_volt)) {
		pr_err("cpr info isn't saved. id(%d)\n", id);
		return -EINVAL;
	}

	return apps_cpr_saved_info[id].fuse_corner_count;

}

int cpr3_get_fuse_cpr_rev(int id)
{
	if (unlikely(!apps_cpr_saved_info[id].fuse_volt)) {
		pr_err("cpr info isn't saved. id(%d)\n", id);
		return -EINVAL;
	}

	return apps_cpr_saved_info[id].cpr_rev_fuse;
}

int cpr3_get_fuse_speed_bin(int id)
{
	if (unlikely(!apps_cpr_saved_info[id].fuse_volt)) {
		pr_err("cpr info isn't saved. id(%d)\n", id);
		return -EINVAL;
	}

	return apps_cpr_saved_info[id].speed_bin_fuse;
}
