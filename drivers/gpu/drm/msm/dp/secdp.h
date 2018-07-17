/*
 * Copyright (c) 2017, The Linux Foundation. All rights reserved.
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
#ifndef __SECDP_H
#define __SECDP_H

#include <linux/usb/usbpd.h>
#include <linux/usb/manager/usb_typec_manager_notifier.h>
#include <linux/secdp_logger.h>
#include "dp_power.h"
#include "dp_panel.h"

/*defined at: include/linux/ccic/ccic_alternate.h*/
#define SAMSUNG_VENDOR_ID		0x04E8
#define DEXDOCK_PRODUCT_ID		0xA020

#define SECDP_ENUM_STR(x)		#x

/*#define SECDP_HDCP_DISABLE*/

#define LEN_BRANCH_REVISION		3
#define DPCD_BRANCH_HW_REVISION			0x509
#define DPCD_BRANCH_SW_REVISION_MAJOR	0x50A
#define DPCD_BRANCH_SW_REVISION_MINOR	0x50B

enum DEX_STATUS {
	DEX_DISABLED,
	DEX_ENABLED,
	DEX_DURING_MODE_CHANGE,
};

struct secdp_dex {
	int prev;			/* previously known as "dex_now" */
	int curr;			/* previously known as "dex_en" */
	int setting_ui;	/* "dex_set", true if setting has Dex mode */

	bool support;		/* previously known as "is_dex_supported" */
	char fw_ver[10];	/* firmware ver, 0:h/w, 1:s/w major, 2:s/w minor */
	int reconnecting; /* it's 1 during dex reconnecting */
};

struct secdp_misc {
	bool                  ccic_noti_registered;
	struct delayed_work   ccic_noti_reg_work;
	struct notifier_block ccic_noti_block;

	struct delayed_work hdcp_start_work;
	struct delayed_work link_status_work;

	struct secdp_dex dex;

	bool cable_connected; /* previously known as "cable_connected_phy" */
	bool link_conf;       /* previously known as "sec_link_conf" */
	bool hpd;             /* previously known as "sec_hpd" */
};

struct secdp_display_timing {
	uint32_t index; /* Resolution priority */
	int active_h;
	int active_v;
	int refresh_rate;
	bool interlaced;
	bool dex_support; /* true if it's dex supported resolution */
	int supported; /* for unit test */
};

#define EV_USBPD_ATTENTION		BIT(13)

static inline char *secdp_ev_event_to_string(int event)
{
	switch (event) {
	case EV_USBPD_ATTENTION:
		return DP_ENUM_STR(EV_USBPD_ATTENTION);
	default:
		return "unknown";
	}
}

struct secdp_attention_node {
	CC_NOTI_TYPEDEF noti;
	struct list_head list;
};

int secdp_ccic_noti_register_ex(struct secdp_misc *sec, bool retry);
int secdp_init(struct platform_device *pdev);
void secdp_deinit(struct platform_device *pdev);
bool secdp_get_power_status(void);
bool secdp_get_cable_status(void);
bool secdp_get_hpd_irq_status(void);
int secdp_get_hpd_status(void);
struct dp_panel *secdp_get_panel_info(void);
struct drm_connector *secdp_get_connector(void);

int secdp_power_request_gpios(struct dp_power *dp_power);
void secdp_config_gpios_factory(int aux_sel, bool out_en);
enum plug_orientation secdp_get_plug_orientation(void);

void secdp_dex_res_init(void);
void secdp_dex_do_reconnecting(void);
bool secdp_check_dex_reconnect(void);
bool secdp_check_dex_mode(void);

void secdp_clear_link_status_update_cnt(struct dp_link *dp_link);
bool secdp_check_link_stable(struct dp_link *dp_link);
#endif/*__SECDP_H*/
