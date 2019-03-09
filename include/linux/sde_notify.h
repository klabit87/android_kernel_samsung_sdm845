/* Copyright (c) 2017, The Linux Foundation. All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 and
 * only version 2 as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 */

#ifndef _SDE_NOTIFY_H_
#define _SDE_NOTIFY_H_

#include <linux/kernel.h>
#include <linux/notifier.h>

int msm_drm_register_notifier_client(struct notifier_block *nb);
int msm_drm_unregister_notifier_client(struct notifier_block *nb);

#endif /*_SDE_NOTIFY_H_*/
