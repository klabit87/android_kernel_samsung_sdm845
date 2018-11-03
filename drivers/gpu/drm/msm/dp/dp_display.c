/*
 * Copyright (c) 2017-2018, The Linux Foundation. All rights reserved.
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

#define pr_fmt(fmt)	"[drm-dp] %s: " fmt, __func__

#include <linux/module.h>
#include <linux/slab.h>
#include <linux/uaccess.h>
#include <linux/debugfs.h>
#include <linux/component.h>
#include <linux/of_irq.h>
#include <linux/hdcp_qseecom.h>

#include "msm_drv.h"
#include "dp_usbpd.h"
#include "dp_parser.h"
#include "dp_power.h"
#include "dp_catalog.h"
#include "dp_aux.h"
#include "dp_link.h"
#include "dp_panel.h"
#include "dp_ctrl.h"
#include "dp_audio.h"
#include "dp_display.h"
#include "sde_hdcp.h"
#include "dp_debug.h"

#ifdef CONFIG_SEC_DISPLAYPORT
#include <linux/switch.h>
#ifdef CONFIG_SEC_DISPLAYPORT_BIGDATA
#include <linux/displayport_bigdata.h>
#endif
#include "secdp.h"
#include "secdp_sysfs.h"
#define CCIC_DP_NOTI_REG_DELAY		60000

struct secdp_event {
	struct dp_display_private *dp;
	u32 id;
};

#define SECDP_EVENT_Q_MAX 4

struct secdp_event_data {
	wait_queue_head_t event_q;
	u32 pndx;
	u32 gndx;
	struct secdp_event event_list[SECDP_EVENT_Q_MAX];
	spinlock_t event_lock;
};

static void secdp_handle_attention(struct dp_display_private *dp);
#endif

static struct dp_display *g_dp_display;
#define HPD_STRING_SIZE 30

#ifdef CONFIG_SEC_DISPLAYPORT
static struct switch_dev switch_secdp = {
	.name = "secdp",
};

static struct switch_dev switch_secdp_msg = {
	.name = "secdp_msg",
};
#endif

struct dp_hdcp {
	void *data;
	struct sde_hdcp_ops *ops;

	void *hdcp1;
	void *hdcp2;

	int enc_lvl;

	bool auth_state;
	bool hdcp1_present;
	bool hdcp2_present;
	bool feature_enabled;
};

struct dp_display_private {
	char *name;
	int irq;

	/* state variables */
	bool core_initialized;
	bool power_on;
	bool audio_supported;

	atomic_t aborted;

	struct platform_device *pdev;
	struct dentry *root;
	struct completion notification_comp;

	struct dp_usbpd   *usbpd;
	struct dp_parser  *parser;
	struct dp_power   *power;
	struct dp_catalog *catalog;
	struct dp_aux     *aux;
	struct dp_link    *link;
	struct dp_panel   *panel;
	struct dp_ctrl    *ctrl;
	struct dp_audio   *audio;
	struct dp_debug   *debug;

	struct dp_hdcp hdcp;

#ifdef CONFIG_SEC_DISPLAYPORT
	struct completion dp_off_comp;

	struct secdp_sysfs	*sysfs;
	struct secdp_misc	 sec;

	/* for ccic event handler */
	struct secdp_event_data dp_event;
	struct task_struct *ev_thread;
	struct list_head attention_head;
	struct mutex attention_lock;
	struct workqueue_struct *workq;
	atomic_t notification_status;
#endif
	struct dp_usbpd_cb usbpd_cb;

	struct dp_display_mode mode;
	struct dp_display dp_display;
	struct msm_drm_private *priv;

	struct workqueue_struct *wq;
	struct delayed_work hdcp_cb_work;
	struct delayed_work connect_work;
	struct work_struct attention_work;
	struct mutex hdcp_mutex;
	struct mutex session_lock;
	int hdcp_status;
	unsigned long audio_status;
};

static const struct of_device_id dp_dt_match[] = {
	{.compatible = "qcom,dp-display"},
	{}
};

#ifdef CONFIG_SEC_DISPLAYPORT
struct dp_display_private *g_secdp_priv;

void secdp_send_poor_connection_event(void)
{
	switch_set_state(&switch_secdp_msg, 1);
	switch_set_state(&switch_secdp_msg, 0);
}

bool secdp_get_power_status(void)
{
	if (!g_secdp_priv)
		return false;

	return g_secdp_priv->power_on;
}

/* check if dp cable has connected or not */
bool secdp_get_cable_status(void)
{
	if (!g_secdp_priv)
		return false;

	return g_secdp_priv->sec.cable_connected;
}

/* check if hpd high has come or not */
int secdp_get_hpd_status(void)
{
	if (!g_secdp_priv)
		return 0;

	return g_secdp_priv->sec.hpd;
}

int secdp_read_branch_revision(struct dp_display_private *dp)
{
	int rlen = 0;
	struct drm_dp_aux *drm_aux;
	char *fw_ver;

	if (!dp || !dp->aux || !dp->aux->drm_aux) {
		pr_err("invalid param\n");
		goto end;
	}

	drm_aux = dp->aux->drm_aux;
	fw_ver = dp->sec.dex.fw_ver;

	rlen = drm_dp_dpcd_read(drm_aux, DPCD_BRANCH_HW_REVISION, fw_ver,
						LEN_BRANCH_REVISION);
	if (rlen < 3) {
		pr_err("read fail, rlen(%d)\n", rlen);
		goto end;
	}

	pr_info("branch revision: HW(0x%X), SW(0x%X, 0x%X)\n",
		fw_ver[0], fw_ver[1], fw_ver[2]);

#ifdef CONFIG_SEC_DISPLAYPORT_BIGDATA
	secdp_bigdata_save_item(BD_ADAPTER_HWID, fw_ver[0]);
	secdp_bigdata_save_item(BD_ADAPTER_FWVER, (fw_ver[1] << 8) | fw_ver[2]);
#endif

end:
	return rlen;
}

void secdp_clear_branch_info(struct dp_display_private *dp)
{
	int i;
	char *fw_ver;

	if (!dp)
		goto end;

	fw_ver = dp->sec.dex.fw_ver;

	for (i = 0; i < LEN_BRANCH_REVISION; i++)
		fw_ver[i] = 0;

end:
	return;
}

static enum dex_support_type secdp_check_adapter_type(uint64_t ven_id, uint64_t prod_id)
{
	enum dex_support_type type = DEX_FHD_SUPPORT;	/* unknown */

	pr_info("ven_id(0x%04x), prod_id(0x%04x)\n", (uint)ven_id, (uint)prod_id);

#ifdef NOT_SUPPORT_DEX_RES_CHANGE
	return DEX_NOT_SUPPORT;
#endif

	if (ven_id == SAMSUNG_VENDOR_ID) {
		switch (prod_id) {
			case 0xa029: /* PAD */
			case 0xa020: /* Station */
			case 0xa02a:
			case 0xa02b:
			case 0xa02c:
			case 0xa02d:
			case 0xa02e:
			case 0xa02f:
			case 0xa030:
			case 0xa031:
			case 0xa032:
			case 0xa033:
				type = DEX_WQHD_SUPPORT;
				break;
		default:
			pr_info("it's SS dongle but unknown\n");
			break;
		}
	} else {
		pr_info("it's not SS dongle\n");
	}

	return type;
}
#endif

static bool dp_display_framework_ready(struct dp_display_private *dp)
{
	return dp->dp_display.post_open ? false : true;
}

static inline bool dp_display_is_hdcp_enabled(struct dp_display_private *dp)
{
	return dp->hdcp.feature_enabled &&
		(dp->hdcp.hdcp1_present || dp->hdcp.hdcp2_present) &&
		dp->hdcp.ops;
}

static irqreturn_t dp_display_irq(int irq, void *dev_id)
{
	struct dp_display_private *dp = dev_id;

	if (!dp) {
		pr_err("invalid data\n");
		return IRQ_NONE;
	}

	/* DP controller isr */
	dp->ctrl->isr(dp->ctrl);

	/* DP aux isr */
	dp->aux->isr(dp->aux);

	/* HDCP isr */
	if (dp_display_is_hdcp_enabled(dp) && dp->hdcp.ops->isr) {
		if (dp->hdcp.ops->isr(dp->hdcp.data))
			pr_err("dp_hdcp_isr failed\n");
	}

	return IRQ_HANDLED;
}

#ifdef CONFIG_SEC_DISPLAYPORT
static void dp_display_handle_maintenance_req(struct dp_display_private *dp);
#endif

static void dp_display_hdcp_cb_work(struct work_struct *work)
{
	struct dp_display_private *dp;
	struct delayed_work *dw = to_delayed_work(work);
	struct sde_hdcp_ops *ops;
	int rc = 0;
	u32 hdcp_auth_state;

	pr_debug("+++\n");

	dp = container_of(dw, struct dp_display_private, hdcp_cb_work);

	rc = dp->catalog->ctrl.read_hdcp_status(&dp->catalog->ctrl);
	if (rc >= 0) {
		hdcp_auth_state = (rc >> 20) & 0x3;
		pr_debug("hdcp auth state %d\n", hdcp_auth_state);
	}

	ops = dp->hdcp.ops;

	switch (dp->hdcp_status) {
	case HDCP_STATE_AUTHENTICATING:
		pr_info("start authenticaton\n");

		if (dp->hdcp.ops && dp->hdcp.ops->authenticate)
			rc = dp->hdcp.ops->authenticate(dp->hdcp.data);

		break;
	case HDCP_STATE_AUTHENTICATED:
		pr_info("hdcp authenticated\n");
		dp->hdcp.auth_state = true;
		break;
	case HDCP_STATE_AUTH_FAIL:
		dp->hdcp.auth_state = false;

#ifdef CONFIG_SEC_DISPLAYPORT_BIGDATA
		secdp_bigdata_inc_error_cnt(ERR_HDCP_AUTH);
#endif

		if (dp->power_on) {
			pr_info("Reauthenticating\n");
			if (ops && ops->reauthenticate) {
				rc = ops->reauthenticate(dp->hdcp.data);
				if (rc)
					pr_err("reauth failed rc=%d\n", rc);
			}
		} else {
			pr_info("not reauthenticating, cable disconnected\n");
		}

		break;
	default:
		break;
	}
}

static void dp_display_notify_hdcp_status_cb(void *ptr,
		enum sde_hdcp_states status)
{
	struct dp_display_private *dp = ptr;

	if (!dp) {
		pr_err("invalid input\n");
		return;
	}

	pr_debug("+++\n");

	dp->hdcp_status = status;

	if (dp->dp_display.is_connected)
		queue_delayed_work(dp->wq, &dp->hdcp_cb_work, HZ/4);
}

static void dp_display_destroy_hdcp_workqueue(struct dp_display_private *dp)
{
	if (dp->wq)
		destroy_workqueue(dp->wq);
}

static void dp_display_update_hdcp_info(struct dp_display_private *dp)
{
	void *fd = NULL;
	struct sde_hdcp_ops *ops = NULL;

	if (!dp) {
		pr_err("invalid input\n");
		return;
	}

	if (!dp->hdcp.feature_enabled) {
		pr_debug("feature not enabled\n");
		return;
	}

	pr_debug("+++\n");

	fd = dp->hdcp.hdcp2;
	if (fd)
		ops = sde_dp_hdcp2p2_start(fd);

	if (ops && ops->feature_supported)
		dp->hdcp.hdcp2_present = ops->feature_supported(fd);
	else
		dp->hdcp.hdcp2_present = false;

	pr_info("hdcp2p2: %s\n",
			dp->hdcp.hdcp2_present ? "supported" : "not supported");

	if (!dp->hdcp.hdcp2_present) {
		dp->hdcp.hdcp1_present = hdcp1_check_if_supported_load_app();

		if (dp->hdcp.hdcp1_present) {
			fd = dp->hdcp.hdcp1;
			ops = sde_hdcp_1x_start(fd);
		}
	}

	pr_info("hdcp1x: %s\n",
			dp->hdcp.hdcp1_present ? "supported" : "not supported");

	if (dp->hdcp.hdcp2_present || dp->hdcp.hdcp1_present) {
		dp->hdcp.data = fd;
		dp->hdcp.ops = ops;

#ifdef CONFIG_SEC_DISPLAYPORT_BIGDATA
		secdp_bigdata_save_item(BD_HDCP_VER,
				dp->hdcp.hdcp2_present ? "hdcp2" : "hdcp1");
#endif
	} else {
		dp->hdcp.data = NULL;
		dp->hdcp.ops = NULL;
	}
}

static void dp_display_deinitialize_hdcp(struct dp_display_private *dp)
{
	if (!dp) {
		pr_err("invalid input\n");
		return;
	}

	sde_dp_hdcp2p2_deinit(dp->hdcp.data);
	dp_display_destroy_hdcp_workqueue(dp);
	if (&dp->hdcp_mutex)
		mutex_destroy(&dp->hdcp_mutex);
}

static int dp_display_initialize_hdcp(struct dp_display_private *dp)
{
	struct sde_hdcp_init_data hdcp_init_data;
	int rc = 0;

	if (!dp) {
		pr_err("invalid input\n");
		return -EINVAL;
	}

	mutex_init(&dp->hdcp_mutex);

	hdcp_init_data.client_id     = HDCP_CLIENT_DP;
	hdcp_init_data.drm_aux       = dp->aux->drm_aux;
	hdcp_init_data.cb_data       = (void *)dp;
	hdcp_init_data.workq         = dp->wq;
	hdcp_init_data.mutex         = &dp->hdcp_mutex;
	hdcp_init_data.sec_access    = true;
	hdcp_init_data.notify_status = dp_display_notify_hdcp_status_cb;
	hdcp_init_data.core_io       = &dp->parser->io.ctrl_io;
	hdcp_init_data.dp_ahb        = &dp->parser->io.dp_ahb;
	hdcp_init_data.dp_aux        = &dp->parser->io.dp_aux;
	hdcp_init_data.dp_link       = &dp->parser->io.dp_link;
	hdcp_init_data.dp_p0         = &dp->parser->io.dp_p0;
	hdcp_init_data.qfprom_io     = &dp->parser->io.qfprom_io;
	hdcp_init_data.hdcp_io       = &dp->parser->io.hdcp_io;
	hdcp_init_data.revision      = &dp->panel->link_info.revision;

	dp->hdcp.hdcp1 = sde_hdcp_1x_init(&hdcp_init_data);
	if (IS_ERR_OR_NULL(dp->hdcp.hdcp1)) {
		pr_err("Error initializing HDCP 1.x\n");
		rc = -EINVAL;
		goto error;
	}

	pr_debug("HDCP 1.3 initialized\n");

	dp->hdcp.hdcp2 = sde_dp_hdcp2p2_init(&hdcp_init_data);
	if (IS_ERR_OR_NULL(dp->hdcp.hdcp2)) {
		pr_err("Error initializing HDCP 2.x\n");
		rc = -EINVAL;
		goto error;
	}

	pr_debug("HDCP 2.2 initialized\n");

	dp->hdcp.feature_enabled = true;

	return 0;
error:
	dp_display_deinitialize_hdcp(dp);
	return rc;
}

static int dp_display_bind(struct device *dev, struct device *master,
		void *data)
{
	int rc = 0;
	struct dp_display_private *dp;
	struct drm_device *drm;
	struct platform_device *pdev = to_platform_device(dev);

	if (!dev || !pdev || !master) {
		pr_err("invalid param(s), dev %pK, pdev %pK, master %pK\n",
				dev, pdev, master);
		rc = -EINVAL;
		goto end;
	}

	drm = dev_get_drvdata(master);
	dp = platform_get_drvdata(pdev);
	if (!drm || !dp) {
		pr_err("invalid param(s), drm %pK, dp %pK\n",
				drm, dp);
		rc = -EINVAL;
		goto end;
	}

	dp->dp_display.drm_dev = drm;
	dp->priv = drm->dev_private;

#ifdef CONFIG_SEC_DISPLAYPORT
	rc = secdp_init(pdev);
	if (rc)
		pr_err("secdp_init failed\n");
#endif

end:
	return rc;
}

static void dp_display_unbind(struct device *dev, struct device *master,
		void *data)
{
	struct dp_display_private *dp;
	struct platform_device *pdev = to_platform_device(dev);

	pr_debug("+++\n");

	if (!dev || !pdev) {
		pr_err("invalid param(s)\n");
		return;
	}

	dp = platform_get_drvdata(pdev);
	if (!dp) {
		pr_err("Invalid params\n");
		return;
	}

#ifdef CONFIG_SEC_DISPLAYPORT
	secdp_deinit(pdev);
#endif

	(void)dp->power->power_client_deinit(dp->power);
	(void)dp->aux->drm_aux_deregister(dp->aux);
	dp_display_deinitialize_hdcp(dp);
}

static const struct component_ops dp_display_comp_ops = {
	.bind = dp_display_bind,
	.unbind = dp_display_unbind,
};

static bool dp_display_is_ds_bridge(struct dp_panel *panel)
{
	return (panel->dpcd[DP_DOWNSTREAMPORT_PRESENT] &
		DP_DWN_STRM_PORT_PRESENT);
}

static bool dp_display_is_sink_count_zero(struct dp_display_private *dp)
{
	return dp_display_is_ds_bridge(dp->panel) &&
		(dp->link->sink_count.count == 0);
}

static void dp_display_send_hpd_event(struct dp_display_private *dp)
{
	struct drm_device *dev = NULL;
	struct drm_connector *connector;
	char name[HPD_STRING_SIZE], status[HPD_STRING_SIZE],
		bpp[HPD_STRING_SIZE], pattern[HPD_STRING_SIZE];
	char *envp[5];

	connector = dp->dp_display.connector;

	if (!connector) {
		pr_err("connector not set\n");
		return;
	}

#ifdef CONFIG_SEC_DISPLAYPORT
	switch_set_state(&switch_secdp, (int)dp->dp_display.is_connected);
	pr_info("secdp displayport uevent: %d\n", dp->dp_display.is_connected);
	msleep(100);
#endif
	connector->status = connector->funcs->detect(connector, false);

	dev = dp->dp_display.connector->dev;

	snprintf(name, HPD_STRING_SIZE, "name=%s", connector->name);
	snprintf(status, HPD_STRING_SIZE, "status=%s",
		drm_get_connector_status_name(connector->status));
	snprintf(bpp, HPD_STRING_SIZE, "bpp=%d",
		dp_link_bit_depth_to_bpp(
		dp->link->test_video.test_bit_depth));
	snprintf(pattern, HPD_STRING_SIZE, "pattern=%d",
		dp->link->test_video.test_video_pattern);

	pr_debug("[%s]:[%s] [%s] [%s]\n", name, status, bpp, pattern);
	envp[0] = name;
	envp[1] = status;
	envp[2] = bpp;
	envp[3] = pattern;
	envp[4] = NULL;
	kobject_uevent_env(&dev->primary->kdev->kobj, KOBJ_CHANGE,
			envp);
}

static void dp_display_post_open(struct dp_display *dp_display)
{
	struct drm_connector *connector;
	struct dp_display_private *dp;

	if (!dp_display) {
		pr_err("invalid input\n");
		return;
	}

	dp = container_of(dp_display, struct dp_display_private, dp_display);
	if (IS_ERR_OR_NULL(dp)) {
		pr_err("invalid params\n");
		return;
	}

	connector = dp->dp_display.connector;

	if (!connector) {
		pr_err("connector not set\n");
		return;
	}

	/* if cable is already connected, send notification */
	if (dp->usbpd->hpd_high)
		queue_delayed_work(dp->wq, &dp->connect_work, HZ * 10);
	else
		dp_display->post_open = NULL;
}

static int dp_display_send_hpd_notification(struct dp_display_private *dp,
		bool hpd)
{
	u32 timeout_sec;

#ifdef CONFIG_SEC_DISPLAYPORT
	if (hpd && !secdp_get_cable_status()) {
		pr_info("cable is out\n");
		return -EIO;
	}
#endif

	pr_debug("+++\n");

	dp->dp_display.is_connected = hpd;

#ifndef CONFIG_SEC_DISPLAYPORT
	if  (dp_display_framework_ready(dp))
		timeout_sec = 5;
	else
		timeout_sec = 10;
#else
	timeout_sec = 15;
#endif

	reinit_completion(&dp->notification_comp);
	dp_display_send_hpd_event(dp);

#ifdef CONFIG_SEC_DISPLAYPORT
	if (!hpd && !dp->power_on) {
		pr_info("DP is already off, no wait\n");
		return 0;
	}

	atomic_set(&dp->notification_status, 1);
#endif

	if (!wait_for_completion_timeout(&dp->notification_comp,
						HZ * timeout_sec)) {
		pr_warn("%s timeout\n", hpd ? "connect" : "disconnect");
		return -EINVAL;
	}

	pr_debug("---\n");
	return 0;
}

static int dp_display_process_hpd_high(struct dp_display_private *dp)
{
	int rc = 0;
	struct edid *edid;

#ifdef CONFIG_SEC_DISPLAYPORT
	if (dp->dp_display.is_connected) {
		pr_debug("it's already handled\n");
		return rc;
	}

	pr_debug("+++\n");
#endif

	dp->aux->init(dp->aux, dp->parser->aux_cfg);

	if (dp->debug->psm_enabled) {
		dp->link->psm_config(dp->link, &dp->panel->link_info, false);
		dp->debug->psm_enabled = false;
	}

	if (!dp->dp_display.connector)
		return 0;

	rc = dp->panel->read_sink_caps(dp->panel, dp->dp_display.connector);

#ifndef CONFIG_SEC_DISPLAYPORT
	if (rc) {
		if (rc == -ETIMEDOUT) {
			pr_err("Sink cap read failed, skip notification\n");
			goto end;
		} else {
			goto notify;
		}
	}
#else
	if (rc) {
		if (!secdp_get_hpd_status() || !secdp_get_cable_status() || rc == -EIO) {
			pr_info("hpd_low or cable_lost or AUX failure\n");
#ifndef CONFIG_SEC_FACTORY
			dp->link->poor_connection = true;
#endif
			goto end;
		}

		dp->link->process_request(dp->link);
		if (dp_display_is_sink_count_zero(dp)) {
			pr_debug("no downstream devices connected.\n");
			rc = -EINVAL;
			goto end;
		}

		pr_info("fall through failsafe\n");
		goto notify;
	}
	dp->sec.dex.prev = secdp_check_dex_mode();
	pr_info("dex.setting_ui: %d, dex.curr: %d\n", dp->sec.dex.setting_ui, dp->sec.dex.curr);

#ifdef CONFIG_SEC_DISPLAYPORT_BIGDATA
	if (dp->sec.dex.prev)
		secdp_bigdata_save_item(BD_DP_MODE, "DEX");
	else
		secdp_bigdata_save_item(BD_DP_MODE, "MIRROR");
#endif

	secdp_read_branch_revision(dp);
#endif

	dp->link->process_request(dp->link);

	if (dp_display_is_sink_count_zero(dp)) {
		pr_debug("no downstream devices connected\n");
		rc = -EINVAL;
		goto end;
	}

	edid = dp->panel->edid_ctrl->edid;

	dp->audio_supported = drm_detect_monitor_audio(edid);

	mutex_lock(&dp->audio->ops_lock);
	dp->panel->handle_sink_request(dp->panel);
	mutex_unlock(&dp->audio->ops_lock);

	dp->dp_display.max_pclk_khz = dp->parser->max_pclk_khz;
notify:
	dp_display_send_hpd_notification(dp, true);

end:
#ifndef CONFIG_SEC_FACTORY
	if (dp->link->poor_connection) {
		secdp_send_poor_connection_event();
		pr_info("poor connection! send secdp_msg uevent\n");
		dp->link->poor_connection = 0;
	}
#endif
	return rc;
}

static void dp_display_host_init(struct dp_display_private *dp)
{
	bool flip = false;

	pr_debug("+++\n");

	if (dp->core_initialized) {
		pr_debug("DP core already initialized\n");
		return;
	}

	if (dp->usbpd->orientation == ORIENTATION_CC2)
		flip = true;

	dp->power->init(dp->power, flip);
	dp->ctrl->init(dp->ctrl, flip, dp->usbpd->multi_func);
	enable_irq(dp->irq);
	dp->core_initialized = true;
}

static void dp_display_host_deinit(struct dp_display_private *dp)
{
	pr_debug("+++\n");

	if (!dp->core_initialized) {
		pr_debug("DP core already off\n");
		return;
	}

	dp->ctrl->deinit(dp->ctrl);
	dp->power->deinit(dp->power);
	disable_irq(dp->irq);
	dp->core_initialized = false;
}

static int dp_display_process_hpd_low(struct dp_display_private *dp)
{
	int rc = 0;

	if (!dp->dp_display.is_connected) {
		pr_debug("HPD already off\n");
		return 0;
	}

	pr_debug("+++\n");

#ifdef CONFIG_SEC_DISPLAYPORT
	cancel_delayed_work_sync(&dp->sec.hdcp_start_work);
	cancel_delayed_work(&dp->sec.link_status_work);
#endif

	if (dp_display_is_hdcp_enabled(dp) && dp->hdcp.ops->off)
		dp->hdcp.ops->off(dp->hdcp.data);

	if (dp->audio_supported)
		dp->audio->off(dp->audio);

	dp->audio_status = -ENODEV;

#ifdef CONFIG_SEC_DISPLAYPORT
	if (!dp->power_on) {
		pr_info("DP is already off, skip\n");
		if (dp->dp_display.is_connected) {
			pr_info("platform reset? clear!\n");
			dp->dp_display.is_connected = false;
		}
		return rc;
	}
#endif
	rc = dp_display_send_hpd_notification(dp, false);

	dp->aux->deinit(dp->aux);

	dp->panel->video_test = false;

	return rc;
}

#ifdef CONFIG_SEC_DISPLAYPORT
void secdp_dex_do_reconnecting(void)
{
	struct dp_display_private *dp = g_secdp_priv;

	if (dp->link->poor_connection) {
		pr_info("poor connection, return!\n");
		return;
	}

	mutex_lock(&dp->attention_lock);
	pr_info("dex_reconnect hpd low++\n");
	dp->sec.dex.reconnecting = 1;

	if (dp->sec.dex.curr == DEX_ENABLED)
		dp->sec.dex.curr = DEX_DURING_MODE_CHANGE;

	dp->usbpd->hpd_high = false;
	dp_display_host_init(dp);
	dp_display_process_hpd_low(dp);

	pr_info("dex_reconnect hpd low--\n");
	mutex_unlock(&dp->attention_lock);

	msleep(400);

	mutex_lock(&dp->attention_lock);
	if (!dp->power_on && dp->sec.dex.reconnecting) {
		pr_info("dex_reconnect hpd high++\n");

		dp->usbpd->hpd_high = true;
		dp_display_host_init(dp);
		dp_display_process_hpd_high(dp);

		pr_info("dex_reconnect hpd high--\n");
	}
	dp->sec.dex.reconnecting = 0;
	mutex_unlock(&dp->attention_lock);
}

bool secdp_check_dex_mode(void)
{
	struct dp_display_private *dp = g_secdp_priv;

	if (!dp->sec.dex.support ||
			(dp->sec.dex.setting_ui == DEX_DISABLED && dp->sec.dex.curr == DEX_DISABLED))
		return false;

	return true;
}

enum dex_support_type secdp_get_adapter_type(void)
{
	struct dp_display_private *dp = g_secdp_priv;

	return dp->sec.dex.support;
}
#endif

#ifndef CONFIG_SEC_DISPLAYPORT
static int dp_display_usbpd_configure_cb(struct device *dev)
{
	int rc = 0;
	struct dp_display_private *dp;

	if (!dev) {
		pr_err("invalid dev\n");
		rc = -EINVAL;
		goto end;
	}

	dp = dev_get_drvdata(dev);
	if (!dp) {
		pr_err("no driver data found\n");
		rc = -ENODEV;
		goto end;
	}

	atomic_set(&dp->aborted, 0);

	dp_display_host_init(dp);

	/* check for hpd high and framework ready */
	if  (dp->usbpd->hpd_high && dp_display_framework_ready(dp))
		queue_delayed_work(dp->wq, &dp->connect_work, 0);
end:
	return rc;
}
#else
static int secdp_display_usbpd_configure_cb(void)
{
	int rc = 0;
	struct dp_display_private *dp = g_secdp_priv;

	pr_debug("+++\n");

	dp_display_host_init(dp);

	/* check for hpd high and framework ready */
	if  (dp->usbpd->hpd_high && dp_display_framework_ready(dp))
		queue_delayed_work(dp->wq, &dp->connect_work, 0);

	return rc;
}
#endif

static void dp_display_clean(struct dp_display_private *dp)
{
	pr_debug("+++\n");

#ifdef CONFIG_SEC_DISPLAYPORT
	cancel_delayed_work_sync(&dp->sec.hdcp_start_work);
	cancel_delayed_work(&dp->sec.link_status_work);
#endif
	if (dp_display_is_hdcp_enabled(dp)) {
		dp->hdcp_status = HDCP_STATE_INACTIVE;

		cancel_delayed_work_sync(&dp->hdcp_cb_work);
		if (dp->hdcp.ops->off)
			dp->hdcp.ops->off(dp->hdcp.data);
	}

	dp->ctrl->push_idle(dp->ctrl);
	dp->ctrl->off(dp->ctrl);
	dp->power_on = false;
}

static int dp_display_handle_disconnect(struct dp_display_private *dp)
{
	int rc;

	dp->usbpd->hpd_high = false;

	pr_debug("+++\n");

	rc = dp_display_process_hpd_low(dp);
	if (rc) {
		/* cancel any pending request */
		dp->ctrl->abort(dp->ctrl);
		dp->aux->abort(dp->aux);
	}

	mutex_lock(&dp->session_lock);
	if (rc && dp->power_on)
		dp_display_clean(dp);

	if (!dp->usbpd->alt_mode_cfg_done)
		dp_display_host_deinit(dp);

	mutex_unlock(&dp->session_lock);

	return rc;
}
#ifndef CONFIG_SEC_DISPLAYPORT
static int dp_display_usbpd_disconnect_cb(struct device *dev)
{
	int rc = 0;
	struct dp_display_private *dp;

	if (!dev) {
		pr_err("invalid dev\n");
		rc = -EINVAL;
		goto end;
	}

	dp = dev_get_drvdata(dev);
	if (!dp) {
		pr_err("no driver data found\n");
		rc = -ENODEV;
		goto end;
	}

	if (dp->debug->psm_enabled)
		dp->link->psm_config(dp->link, &dp->panel->link_info, true);

	/* cancel any pending request */
	atomic_set(&dp->aborted, 1);
	dp->ctrl->abort(dp->ctrl);
	dp->aux->abort(dp->aux);

	/* wait for idle state */
	cancel_delayed_work(&dp->connect_work);
	cancel_work(&dp->attention_work);
	flush_workqueue(dp->wq);

	dp_display_handle_disconnect(dp);
end:
	return rc;
}
#else
static int secdp_display_usbpd_disconnect_cb(void)
{
	int rc = 0;
	struct dp_display_private *dp = g_secdp_priv;

	if (!dp) {
		pr_err("no driver data found\n");
		rc = -ENODEV;
		goto end;
	}

	pr_debug("+++, psm(%d)\n", dp->debug->psm_enabled);

	if (atomic_read(&dp->notification_status)) {
		reinit_completion(&dp->notification_comp);
		pr_info("wait for connection logic++\n");
		if (atomic_read(&dp->notification_status) &&
			!wait_for_completion_timeout(&dp->notification_comp, HZ * 5)) {

			pr_warn("notification_comp timeout\n");
		}
		pr_info("wait for connection logic--\n");
	}

	atomic_set(&dp->notification_status, 0);

	if (dp->debug->psm_enabled)
		dp->link->psm_config(dp->link, &dp->panel->link_info, true);

	/* cancel any pending request */
	atomic_set(&dp->aborted, 1);
	dp->ctrl->abort(dp->ctrl);
	dp->aux->abort(dp->aux);

	/* wait for idle state */
	cancel_delayed_work(&dp->connect_work);
	flush_workqueue(dp->wq);

	dp_display_handle_disconnect(dp);
	atomic_set(&dp->aborted, 0);

end:
	pr_debug("---\n");
	return rc;
}
#endif

static void dp_display_handle_maintenance_req(struct dp_display_private *dp)
{
#ifdef CONFIG_SEC_DISPLAYPORT
	int ret;

	if (!secdp_get_cable_status()) {
		pr_info("cable is out\n");
		return;
	}
#endif
	mutex_lock(&dp->audio->ops_lock);

	if (dp->audio_supported && !IS_ERR_VALUE(dp->audio_status))
		dp->audio->off(dp->audio);

#ifndef CONFIG_SEC_DISPLAYPORT
	dp->ctrl->link_maintenance(dp->ctrl);

	if (dp->audio_supported && !IS_ERR_VALUE(dp->audio_status))
		dp->audio_status = dp->audio->on(dp->audio);
#else
	ret = dp->ctrl->link_maintenance(dp->ctrl);
	if (!ret && dp->audio_supported) {
/* In case of poor connection, hpd irq is coming continuously in a short time.
 * and audio off/on fucntions are called rapidly.
 * It can causes dp audio problem. so, added delay.
 */
		if (dp->link->sink_request & DP_LINK_STATUS_UPDATED)
			msleep(30);
		dp->audio->on(dp->audio);
	}
#endif
	mutex_unlock(&dp->audio->ops_lock);
}

static void dp_display_attention_work(struct work_struct *work)
{
	struct dp_display_private *dp = container_of(work,
			struct dp_display_private, attention_work);

#ifdef CONFIG_SEC_DISPLAYPORT
	if (!secdp_get_hpd_status() || !secdp_get_cable_status()) {
		pr_info("hpd_low or cable_lost\n");
		return;
	}

	pr_debug("+++, sink_request: 0x%08x\n", dp->link->sink_request);
#endif

	if (dp->link->sink_request & DS_PORT_STATUS_CHANGED) {
		dp_display_handle_disconnect(dp);

		if (dp_display_is_sink_count_zero(dp)) {
			pr_debug("sink count is zero, nothing to do\n");
			return;
		}

#ifdef CONFIG_SEC_DISPLAYPORT
		/* add some delay to guarantee hpd event handling in framework side */
		msleep(60);
#endif

		queue_delayed_work(dp->wq, &dp->connect_work, 0);
		return;
	}

	if (dp->link->sink_request & DP_TEST_LINK_VIDEO_PATTERN) {
		dp_display_handle_disconnect(dp);

		dp->panel->video_test = true;
		dp_display_send_hpd_notification(dp, true);
		dp->link->send_test_response(dp->link);

		return;
	}

	if (dp->link->sink_request & DP_TEST_LINK_PHY_TEST_PATTERN) {
		dp->ctrl->process_phy_test_request(dp->ctrl);
		return;
	}

	if (dp->link->sink_request & DP_TEST_LINK_TRAINING) {
		dp->link->send_test_response(dp->link);
		dp_display_handle_maintenance_req(dp);
		return;
	}

	if (dp->link->sink_request & DP_LINK_STATUS_UPDATED)
		dp_display_handle_maintenance_req(dp);

	if (dp_display_is_hdcp_enabled(dp) && dp->hdcp.ops->cp_irq)
		dp->hdcp.ops->cp_irq(dp->hdcp.data);
}

#ifndef CONFIG_SEC_DISPLAYPORT
static int dp_display_usbpd_attention_cb(struct device *dev)
{
	struct dp_display_private *dp;

	if (!dev) {
		pr_err("invalid dev\n");
		return -EINVAL;
	}

	dp = dev_get_drvdata(dev);
	if (!dp) {
		pr_err("no driver data found\n");
		return -ENODEV;
	}

	if (dp->usbpd->hpd_irq && dp->usbpd->hpd_high) {
		dp->link->process_request(dp->link);
		queue_work(dp->wq, &dp->attention_work);
	} else if (dp->usbpd->hpd_high) {
		queue_delayed_work(dp->wq, &dp->connect_work, 0);
	} else {
		/* cancel any pending request */
		atomic_set(&dp->aborted, 1);
		dp->ctrl->abort(dp->ctrl);
		dp->aux->abort(dp->aux);

		/* wait for idle state */
		cancel_delayed_work(&dp->connect_work);
		cancel_work(&dp->attention_work);
		flush_workqueue(dp->wq);

		dp_display_handle_disconnect(dp);
		atomic_set(&dp->aborted, 0);
	}

	return 0;
}

#else
static int secdp_event_thread(void *data)
{
	unsigned long flag;
	u32 todo = 0;

	struct secdp_event_data *ev_data;
	struct secdp_event *ev;
	struct dp_display_private *dp = NULL;

	if (!data)
		return -EINVAL;

	ev_data = (struct secdp_event_data *)data;
	init_waitqueue_head(&ev_data->event_q);
	spin_lock_init(&ev_data->event_lock);

	while (!kthread_should_stop()) {
		wait_event(ev_data->event_q,
			(ev_data->pndx != ev_data->gndx) ||
			kthread_should_stop());
		spin_lock_irqsave(&ev_data->event_lock, flag);
		ev = &(ev_data->event_list[ev_data->gndx++]);
		todo = ev->id;
		dp = ev->dp;
		ev->id = 0;
		ev_data->gndx %= SECDP_EVENT_Q_MAX;
		spin_unlock_irqrestore(&ev_data->event_lock, flag);

		pr_debug("todo=%s\n", secdp_ev_event_to_string(todo));

		switch (todo) {
		case EV_USBPD_ATTENTION:
			secdp_handle_attention(dp);
			break;
		default:
			pr_err("Unknown event:%d\n", todo);
		}
	}

	return 0;
}

static int secdp_event_setup(struct dp_display_private *dp)
{

	dp->ev_thread = kthread_run(secdp_event_thread,
		(void *)&dp->dp_event, "secdp_event");
	if (IS_ERR(dp->ev_thread)) {
		pr_err("unable to start event thread\n");
		return PTR_ERR(dp->ev_thread);
	}

	dp->workq = create_workqueue("secdp_hpd");
	if (!dp->workq) {
		pr_err("error creating workqueue\n");
		return -EPERM;
	}

	INIT_LIST_HEAD(&dp->attention_head);
	return 0;
}

static void secdp_event_cleanup(struct dp_display_private *dp)
{
	destroy_workqueue(dp->workq);

	if (dp->ev_thread == current)
		return;

	kthread_stop(dp->ev_thread);
}

static void secdp_send_events(struct dp_display_private *dp, u32 event)
{
	struct secdp_event *ev;
	struct secdp_event_data *ev_data = &dp->dp_event;

	pr_debug("event=%s\n", secdp_ev_event_to_string(event));

	spin_lock(&ev_data->event_lock);
	ev = &ev_data->event_list[ev_data->pndx++];
	ev->id = event;
	ev->dp = dp;
	ev_data->pndx %= SECDP_EVENT_Q_MAX;
	wake_up(&ev_data->event_q);
	spin_unlock(&ev_data->event_lock);
}

static void secdp_process_attention(struct dp_display_private *dp,
		CC_NOTI_TYPEDEF *noti)
{
	int rc = 0;

	if (!noti || !dp)
		goto end;

	pr_debug("sub1(%d), sub2(%d), sub3(%d)\n", noti->sub1, noti->sub2, noti->sub3);

	dp->sec.dex.reconnecting = 0;

	if (noti->id == CCIC_NOTIFY_ID_DP_CONNECT && noti->sub1 == CCIC_NOTIFY_DETACH) {
		dp->usbpd->hpd_high = false;

		rc = secdp_display_usbpd_disconnect_cb();
		goto end;
	}

	if (noti->id == CCIC_NOTIFY_ID_DP_HPD &&
		noti->sub1 == CCIC_NOTIFY_HIGH && noti->sub2 == CCIC_NOTIFY_IRQ) {

		if (!secdp_get_cable_status()) {
			pr_info("cable is out\n");
			goto end;
		}

		if (dp->link->poor_connection) {
			pr_info("poor connection\n");
			goto end;
		}

		if (!dp->power_on) {
			flush_workqueue(dp->wq);
			if (!dp->power_on) {
				pr_debug("handle it as hpd high\n");

				if (dp->dp_display.is_connected) {
					pr_info("platform reset!\n");
					secdp_display_usbpd_disconnect_cb();
					msleep(100);
				}
				goto handle_hpd_high;
			}
		}

		/* do the same with: dp_display_usbpd_attention_cb */
		dp->link->process_request(dp->link);
		queue_work(dp->wq, &dp->attention_work);

		goto end;
	}

	if (noti->id == CCIC_NOTIFY_ID_DP_HPD && noti->sub1 == CCIC_NOTIFY_LOW) {
		dp->sec.dex.prev = dp->sec.dex.curr = DEX_DISABLED;
		secdp_clear_link_status_update_cnt(dp->link);

/* Following functions were copied from dp_display_usbpd_disconnect_cb() fucntion.
 * Sometimes they make connection status abnormal and it's causing CTS failure
 * when running CTS continuously. so, we commented it out.
 */
#if 0
		/* cancel any pending request */
		atomic_set(&dp->aborted, 1);
		dp->ctrl->abort(dp->ctrl);
		dp->aux->abort(dp->aux);
#endif

		/* wait for idle state */
		cancel_delayed_work(&dp->connect_work);
		flush_workqueue(dp->wq);

		dp_display_handle_disconnect(dp);
		atomic_set(&dp->aborted, 0);
		goto end;
	}

handle_hpd_high:
	/* handle it as hpd high */
	pr_info("is_connected <%d>\n", dp->dp_display.is_connected);
	if (!dp->dp_display.is_connected) {
		secdp_clear_link_status_update_cnt(dp->link);
		queue_delayed_work(dp->wq, &dp->connect_work, 0);
	}

end:
	return;
}

static void secdp_handle_attention(struct dp_display_private *dp)
{
	int i = 0;

	while (!list_empty_careful(&dp->attention_head)) {
		struct secdp_attention_node *node;

		pr_debug("+++ processing item %d in the list +++\n", ++i);

		mutex_lock(&dp->attention_lock);
		node = list_first_entry(&dp->attention_head,
				struct secdp_attention_node, list);

		secdp_process_attention(dp, &node->noti);

#ifdef CONFIG_SEC_DISPLAYPORT
		/* add some delay to guarantee hpd event handling in framework side */
		msleep(60);
#endif

		list_del(&node->list);
		mutex_unlock(&dp->attention_lock);

		kzfree(node);

		pr_debug("--- processing item %d in the list ---\n", i);
	};

}

static int secdp_ccic_noti_cb(struct notifier_block *nb, unsigned long action,
		void *data)
{
	struct dp_display_private *dp = g_secdp_priv;
	CC_NOTI_TYPEDEF noti = *(CC_NOTI_TYPEDEF *)data;
	struct secdp_attention_node *node;
	int rc = 0;

	if (noti.dest != CCIC_NOTIFY_DEV_DP) {
		/*pr_debug("not DP, skip\n");*/
		goto end;
	}

	switch (noti.id) {
	case CCIC_NOTIFY_ID_ATTACH:
		pr_info("CCIC_NOTIFY_ID_ATTACH\n");
		break;

	case CCIC_NOTIFY_ID_DP_CONNECT:
		pr_info("CCIC_NOTIFY_ID_DP_CONNECT, <%d>\n", noti.sub1);

		switch (noti.sub1) {
		case CCIC_NOTIFY_ATTACH:
			dp->sec.cable_connected = true;
			dp->usbpd->alt_mode_cfg_done = true;
			secdp_clear_link_status_update_cnt(dp->link);
			dp->usbpd->orientation = secdp_get_plug_orientation();
			dp->sec.dex.support =
				secdp_check_adapter_type(noti.sub2, noti.sub3);
#ifdef CONFIG_SEC_DISPLAYPORT_BIGDATA
			secdp_bigdata_connection();
			secdp_bigdata_save_item(BD_ORIENTATION,
				dp->usbpd->orientation == ORIENTATION_CC1 ? "CC1" : "CC2");
			secdp_bigdata_save_item(BD_ADT_VID, noti.sub2);
			secdp_bigdata_save_item(BD_ADT_PID, noti.sub3);
#endif
			secdp_logger_set_max_count(150);

			/* see dp_display_usbpd_configure_cb() */
			dp_display_host_init(dp);

			break;

		case CCIC_NOTIFY_DETACH:
			if (!secdp_get_cable_status()) {
				pr_info("already disconnected\n");
				goto end;
			}

			/* set flags here as soon as disconnected
			 * resource clear will be made later at "secdp_process_attention"
			 */
			dp->sec.dex.prev = dp->sec.dex.curr = DEX_DISABLED;
			dp->sec.dex.support = DEX_NOT_SUPPORT;
			dp->sec.dex.reconnecting = 0;
			dp->sec.cable_connected = false;
			dp->sec.link_conf = false;
			dp->sec.hpd = false;
			dp->usbpd->orientation = ORIENTATION_NONE;
			secdp_clear_link_status_update_cnt(dp->link);
			dp->usbpd->alt_mode_cfg_done = false;

			secdp_clear_branch_info(dp);
#ifdef CONFIG_SEC_DISPLAYPORT_BIGDATA
			secdp_bigdata_disconnection();
#endif
			break;

		default:
			break;
		}
		break;

	case CCIC_NOTIFY_ID_DP_LINK_CONF:
		pr_info("CCIC_NOTIFY_ID_DP_LINK_CONF, <%c>\n",
			noti.sub1 + 'A' - 1);
		if (!secdp_get_cable_status()) {
			pr_info("cable is out\n");
			goto end;
		}

#ifdef CONFIG_SEC_DISPLAYPORT_BIGDATA
		secdp_bigdata_save_item(BD_LINK_CONFIGURE, noti.sub1 + 'A' - 1);
#endif
		dp->sec.link_conf = true;
		break;

	case CCIC_NOTIFY_ID_DP_HPD:
		pr_info("CCIC_NOTIFY_ID_DP_HPD, sub1 <%s>, sub2<%s>\n",
			(noti.sub1 == CCIC_NOTIFY_HIGH) ? "high" :
				((noti.sub1 == CCIC_NOTIFY_LOW) ? "low" : "??"),
			(noti.sub2 == CCIC_NOTIFY_IRQ) ? "irq" : "??");
		if (!secdp_get_cable_status()) {
			pr_info("cable is out\n");
			goto end;
		}

		if (noti.sub1 == CCIC_NOTIFY_HIGH) {
			dp->sec.hpd = true;
		} else /* if (noti.sub1 == CCIC_NOTIFY_LOW) */ {
			dp->sec.hpd = false;
		}

		break;

	default:
		break;
	}

	pr_debug("sec.link_conf(%d), sec.hpd(%d)\n", dp->sec.link_conf, dp->sec.hpd);
	if ((dp->sec.link_conf && dp->sec.hpd) || /*hpd high? or hpd_irq?*/
		(noti.id == CCIC_NOTIFY_ID_DP_HPD && noti.sub1 == CCIC_NOTIFY_LOW) || /*hpd low?*/
		(noti.id == CCIC_NOTIFY_ID_DP_CONNECT && noti.sub1 == CCIC_NOTIFY_DETACH)) { /*cable disconnect?*/

		node = kzalloc(sizeof(*node), GFP_KERNEL);

		node->noti.src  = noti.src;
		node->noti.dest = noti.dest;
		node->noti.id   = noti.id;
		node->noti.sub1 = noti.sub1;
		node->noti.sub2 = noti.sub2;
		node->noti.sub3 = noti.sub3;

		mutex_lock(&dp->attention_lock);
		list_add_tail(&node->list, &dp->attention_head);
		mutex_unlock(&dp->attention_lock);

		secdp_send_events(dp, EV_USBPD_ATTENTION);

		if ((noti.id == CCIC_NOTIFY_ID_DP_CONNECT && noti.sub1 == CCIC_NOTIFY_DETACH) &&
				(dp->power_on == true || atomic_read(&dp->notification_status))) {
			int ret;

			init_completion(&dp->dp_off_comp);
			ret = wait_for_completion_timeout(&dp->dp_off_comp, msecs_to_jiffies(10000));
			if (ret <= 0)
				pr_err("dp_off_comp timedout\n");
			else
				pr_debug("detach complete!\n");

			atomic_set(&dp->notification_status, 0);
		}
	}

end:
	return rc;
}

int secdp_ccic_noti_register_ex(struct secdp_misc *sec, bool retry)
{
	int rc;

	sec->ccic_noti_registered = true;
	rc = manager_notifier_register(&sec->ccic_noti_block,
				secdp_ccic_noti_cb, MANAGER_NOTIFY_CCIC_DP);
	if (!rc) {
		pr_debug("success\n");
		goto exit;
	}

	sec->ccic_noti_registered = false;
	if (!retry) {
		pr_debug("no more try\n");
		goto exit;
	}

	pr_err("manager_dev is not ready yet, rc(%d)\n", rc);
	pr_err("try again in %d[ms]\n", CCIC_DP_NOTI_REG_DELAY);

	schedule_delayed_work(&sec->ccic_noti_reg_work,
			msecs_to_jiffies(CCIC_DP_NOTI_REG_DELAY));

exit:
	return rc;
}

static void secdp_ccic_noti_register(struct work_struct *work)
{
	int rc;
	struct dp_display_private *dp = g_secdp_priv;
	struct secdp_misc *sec = &dp->sec;

	mutex_lock(&sec->notifier_lock);
	if (sec->ccic_noti_registered) {
		pr_info("already registered\n");
		goto exit;
	}

	rc = secdp_ccic_noti_register_ex(sec, true);
	if (rc) {
		pr_err("fail, rc(%d)\n", rc);
		goto exit;
	}

	pr_debug("success\n");
	sec->ccic_noti_registered = true;

	/* cancel immediately */
	rc = cancel_delayed_work(&sec->ccic_noti_reg_work);
	pr_debug("cancel_work, rc(%d)\n", rc);
	destroy_delayed_work_on_stack(&sec->ccic_noti_reg_work);

exit:
	mutex_unlock(&sec->notifier_lock);
	return;
}

static void secdp_hdcp_start_work(struct work_struct *work)
{
	struct dp_display_private *dp = g_secdp_priv;

	pr_debug("+++\n");
	if (secdp_get_cable_status() && dp->power_on) {
		dp_display_update_hdcp_info(dp);

		if (dp_display_is_hdcp_enabled(dp)) {
			cancel_delayed_work_sync(&dp->hdcp_cb_work);

			dp->hdcp_status = HDCP_STATE_AUTHENTICATING;
			queue_delayed_work(dp->wq, &dp->hdcp_cb_work, HZ / 2);
		}
	}
}

/* This logic is to check poor DP connection. if link train is failed or
 * irq hpd is coming more than 4th times in 13 sec, regard it as a poor connection
 * and do disconnection of displayport
 */
static void secdp_link_status_work(struct work_struct *work)
{
	struct dp_display_private *dp = g_secdp_priv;

	pr_info("+++ status_update_cnt %d\n", dp->link->status_update_cnt);

	if (secdp_get_cable_status() && dp->power_on) {

		if (!dp->ctrl->get_link_train_status(dp->ctrl) ||
			dp->link->status_update_cnt > 4) {

			dp->link->poor_connection = true;
			dp->link->status_update_cnt = 0;

			/* cancel any pending request */
			dp->ctrl->abort(dp->ctrl);
			dp->aux->abort(dp->aux);

			/* wait for idle state */
			flush_workqueue(dp->wq);

			dp_display_handle_disconnect(dp);

			secdp_send_poor_connection_event();
			pr_info("poor connection! send secdp_msg uevent\n");
		} else {
			if (!secdp_check_link_stable(dp->link)) {
				pr_info("Check poor connection, again\n");
				schedule_delayed_work(&dp->sec.link_status_work,
							msecs_to_jiffies(3000));
			}
		}
	}
}

int secdp_init(struct platform_device *pdev)
{
	int rc = -1;
	struct dp_display_private *dp;

	dp = platform_get_drvdata(pdev);

	INIT_DELAYED_WORK(&dp->sec.hdcp_start_work, secdp_hdcp_start_work);
	INIT_DELAYED_WORK(&dp->sec.link_status_work, secdp_link_status_work);

	INIT_DELAYED_WORK(&dp->sec.ccic_noti_reg_work, secdp_ccic_noti_register);
	schedule_delayed_work(&dp->sec.ccic_noti_reg_work,
			msecs_to_jiffies(CCIC_DP_NOTI_REG_DELAY));

	rc = secdp_power_request_gpios(dp->power);
	if (rc)
		pr_err("DRM DP gpio request failed\n");

	rc = secdp_sysfs_init();
	if (rc)
		pr_err("secdp_sysfs_init failed\n");

	mutex_init(&dp->attention_lock);
	mutex_init(&dp->sec.notifier_lock);

	rc = secdp_event_setup(dp);
	if (rc)
		pr_err("secdp_event_setup failed\n");

	pr_info("exit, rc(%d)\n", rc);
	return rc;
}

void secdp_deinit(struct platform_device *pdev)
{
	struct dp_display_private *dp;

	dp = platform_get_drvdata(pdev);

	secdp_event_cleanup(dp);
}

struct dp_panel *secdp_get_panel_info(void)
{
	struct dp_display_private *dp = g_secdp_priv;
	struct dp_panel *panel = NULL;

	if (dp)
		panel = dp->panel;

	pr_debug("panel\n");
	return panel;
}

struct drm_connector *secdp_get_connector(void)
{
	struct dp_display *dp_disp = g_dp_display;
	struct drm_connector *connector = NULL;

	if (dp_disp)
		connector = dp_disp->connector;

	pr_debug("connector\n");
	return connector;
}
#endif

static void dp_display_connect_work(struct work_struct *work)
{
	struct delayed_work *dw = to_delayed_work(work);
	struct dp_display_private *dp = container_of(dw,
			struct dp_display_private, connect_work);

	dp->usbpd->hpd_high = true;

	if (dp->dp_display.is_connected) {
		pr_debug("HPD already on\n");
		return;
	}

	if (atomic_read(&dp->aborted)) {
		pr_err("aborted\n");
		return;
	}

#ifdef CONFIG_SEC_DISPLAYPORT
	dp_display_host_init(dp);
#endif
	dp_display_process_hpd_high(dp);
}

static void dp_display_deinit_sub_modules(struct dp_display_private *dp)
{
	dp_audio_put(dp->audio);
	dp_ctrl_put(dp->ctrl);
#ifdef CONFIG_SEC_DISPLAYPORT
	secdp_sysfs_put(dp->sysfs);
#endif
	dp_link_put(dp->link);
	dp_panel_put(dp->panel);
	dp_aux_put(dp->aux);
	dp_power_put(dp->power);
	dp_catalog_put(dp->catalog);
	dp_parser_put(dp->parser);
	dp_usbpd_put(dp->usbpd);
	mutex_destroy(&dp->session_lock);
	dp_debug_put(dp->debug);
}

static int dp_init_sub_modules(struct dp_display_private *dp)
{
	int rc = 0;
	struct device *dev = &dp->pdev->dev;
	struct dp_usbpd_cb *cb = &dp->usbpd_cb;
	struct dp_ctrl_in ctrl_in = {
		.dev = dev,
	};
	struct dp_panel_in panel_in = {
		.dev = dev,
	};

	mutex_init(&dp->session_lock);

	dp->parser = dp_parser_get(dp->pdev);
	if (IS_ERR(dp->parser)) {
		rc = PTR_ERR(dp->parser);
		pr_err("failed to initialize parser, rc = %d\n", rc);
		dp->parser = NULL;
		goto error;
	}

	rc = dp->parser->parse(dp->parser);
	if (rc) {
		pr_err("device tree parsing failed\n");
		goto error_catalog;
	}

	dp->catalog = dp_catalog_get(dev, &dp->parser->io);
	if (IS_ERR(dp->catalog)) {
		rc = PTR_ERR(dp->catalog);
		pr_err("failed to initialize catalog, rc = %d\n", rc);
		dp->catalog = NULL;
		goto error_catalog;
	}

	dp->power = dp_power_get(dp->parser);
	if (IS_ERR(dp->power)) {
		rc = PTR_ERR(dp->power);
		pr_err("failed to initialize power, rc = %d\n", rc);
		dp->power = NULL;
		goto error_power;
	}

	rc = dp->power->power_client_init(dp->power, &dp->priv->phandle);
	if (rc) {
		pr_err("Power client create failed\n");
		goto error_aux;
	}

	dp->aux = dp_aux_get(dev, &dp->catalog->aux, dp->parser->aux_cfg);
	if (IS_ERR(dp->aux)) {
		rc = PTR_ERR(dp->aux);
		pr_err("failed to initialize aux, rc = %d\n", rc);
		dp->aux = NULL;
		goto error_aux;
	}

	rc = dp->aux->drm_aux_register(dp->aux);
	if (rc) {
		pr_err("DRM DP AUX register failed\n");
		goto error_link;
	}

	dp->link = dp_link_get(dev, dp->aux);
	if (IS_ERR(dp->link)) {
		rc = PTR_ERR(dp->link);
		pr_err("failed to initialize link, rc = %d\n", rc);
		dp->link = NULL;
		goto error_link;
	}

#ifdef CONFIG_SEC_DISPLAYPORT
	dp->sysfs = secdp_sysfs_get(dev, &dp->sec);
	if (IS_ERR(dp->sysfs)) {
		rc = PTR_ERR(dp->sysfs);
		pr_err("failed to initialize sysfs, rc = %d\n", rc);
		dp->sysfs = NULL;
		goto error_sysfs;
	}
#endif

	panel_in.aux = dp->aux;
	panel_in.catalog = &dp->catalog->panel;
	panel_in.link = dp->link;

	dp->panel = dp_panel_get(&panel_in);
	if (IS_ERR(dp->panel)) {
		rc = PTR_ERR(dp->panel);
		pr_err("failed to initialize panel, rc = %d\n", rc);
		dp->panel = NULL;
		goto error_panel;
	}

	ctrl_in.link = dp->link;
	ctrl_in.panel = dp->panel;
	ctrl_in.aux = dp->aux;
	ctrl_in.power = dp->power;
	ctrl_in.catalog = &dp->catalog->ctrl;
	ctrl_in.parser = dp->parser;

	dp->ctrl = dp_ctrl_get(&ctrl_in);
	if (IS_ERR(dp->ctrl)) {
		rc = PTR_ERR(dp->ctrl);
		pr_err("failed to initialize ctrl, rc = %d\n", rc);
		dp->ctrl = NULL;
		goto error_ctrl;
	}

	dp->audio = dp_audio_get(dp->pdev, dp->panel, &dp->catalog->audio);
	if (IS_ERR(dp->audio)) {
		rc = PTR_ERR(dp->audio);
		pr_err("failed to initialize audio, rc = %d\n", rc);
		dp->audio = NULL;
		goto error_audio;
	}

#ifndef CONFIG_SEC_DISPLAYPORT
	cb->configure  = dp_display_usbpd_configure_cb;
	cb->disconnect = dp_display_usbpd_disconnect_cb;
	cb->attention  = dp_display_usbpd_attention_cb;

	dp->usbpd = dp_usbpd_get(dev, cb);
#else
	cb->configure  = secdp_display_usbpd_configure_cb;
	cb->disconnect = secdp_display_usbpd_disconnect_cb;

	dp->usbpd = secdp_usbpd_get(dev, cb);
#endif

	if (IS_ERR(dp->usbpd)) {
		rc = PTR_ERR(dp->usbpd);
		pr_err("failed to initialize usbpd, rc = %d\n", rc);
		dp->usbpd = NULL;
		goto error_usbpd;
	}

	dp->debug = dp_debug_get(dev, dp->panel, dp->usbpd,
				dp->link, &dp->dp_display.connector);
	if (IS_ERR(dp->debug)) {
		rc = PTR_ERR(dp->debug);
		pr_err("failed to initialize debug, rc = %d\n", rc);
		dp->debug = NULL;
		goto error_debug;
	}

	return rc;
error_debug:
	dp_usbpd_put(dp->usbpd);
error_usbpd:
	dp_audio_put(dp->audio);
error_audio:
	dp_ctrl_put(dp->ctrl);
error_ctrl:
	dp_panel_put(dp->panel);
error_panel:
	dp_link_put(dp->link);
#ifdef CONFIG_SEC_DISPLAYPORT
error_sysfs:
	secdp_sysfs_put(dp->sysfs);
#endif
error_link:
	dp_aux_put(dp->aux);
error_aux:
	dp_power_put(dp->power);
error_power:
	dp_catalog_put(dp->catalog);
error_catalog:
	dp_parser_put(dp->parser);
error:
	mutex_destroy(&dp->session_lock);
	return rc;
}

static void dp_display_post_init(struct dp_display *dp_display)
{
	int rc = 0;
	struct dp_display_private *dp;

	if (!dp_display) {
		pr_err("invalid input\n");
		rc = -EINVAL;
		goto end;
	}

	dp = container_of(dp_display, struct dp_display_private, dp_display);
	if (IS_ERR_OR_NULL(dp)) {
		pr_err("invalid params\n");
		rc = -EINVAL;
		goto end;
	}

	rc = dp_init_sub_modules(dp);
	if (rc)
		goto end;

	dp_display_initialize_hdcp(dp);

	dp_display->post_init = NULL;
end:
	pr_debug("%s\n", rc ? "failed" : "success");
}

static int dp_display_set_mode(struct dp_display *dp_display,
		struct dp_display_mode *mode)
{
	const u32 num_components = 3, default_bpp = 24;
	struct dp_display_private *dp;

	if (!dp_display) {
		pr_err("invalid input\n");
		return -EINVAL;
	}
	dp = container_of(dp_display, struct dp_display_private, dp_display);

	mutex_lock(&dp->session_lock);
	mode->timing.bpp =
		dp_display->connector->display_info.bpc * num_components;
	if (!mode->timing.bpp)
		mode->timing.bpp = default_bpp;

	mode->timing.bpp = dp->panel->get_mode_bpp(dp->panel,
			mode->timing.bpp, mode->timing.pixel_clk_khz);

	dp->panel->pinfo = mode->timing;
	dp->panel->init(dp->panel);
	mutex_unlock(&dp->session_lock);

	return 0;
}

static int dp_display_prepare(struct dp_display *dp)
{
	return 0;
}

static int dp_display_enable(struct dp_display *dp_display)
{
	int rc = 0;
	struct dp_display_private *dp;

	if (!dp_display) {
		pr_err("invalid input\n");
		return -EINVAL;
	}

	pr_debug("+++\n");

	dp = container_of(dp_display, struct dp_display_private, dp_display);

	mutex_lock(&dp->session_lock);

	if (dp->power_on) {
		pr_debug("Link already setup, return\n");
		goto end;
	}

#ifndef CONFIG_SEC_DISPLAYPORT
	if (atomic_read(&dp->aborted)) {
		pr_err("aborted\n");
		goto end;
	}
#endif

	dp->aux->init(dp->aux, dp->parser->aux_cfg);

	rc = dp->ctrl->on(dp->ctrl);

	if (dp->debug->tpg_state)
		dp->panel->tpg_config(dp->panel, true);

	if (!rc)
		dp->power_on = true;
end:
	mutex_unlock(&dp->session_lock);
	return rc;
}

static int dp_display_post_enable(struct dp_display *dp_display)
{
	struct dp_display_private *dp;

	if (!dp_display) {
		pr_err("invalid input\n");
		return -EINVAL;
	}

	pr_debug("+++\n");

	dp = container_of(dp_display, struct dp_display_private, dp_display);

	mutex_lock(&dp->session_lock);

	if (!dp->power_on) {
		pr_debug("Link not setup, return\n");
		goto end;
	}

#ifndef CONFIG_SEC_DISPLAYPORT
	if (atomic_read(&dp->aborted)) {
		pr_err("aborted\n");
		goto end;
	}
#endif

	dp->panel->spd_config(dp->panel);

	if (dp->audio_supported) {
		dp->audio->bw_code = dp->link->link_params.bw_code;
		dp->audio->lane_count = dp->link->link_params.lane_count;
		dp->audio_status = dp->audio->on(dp->audio);
	}

#ifndef CONFIG_SEC_DISPLAYPORT
	dp_display_update_hdcp_info(dp);

	if (dp_display_is_hdcp_enabled(dp)) {
		cancel_delayed_work_sync(&dp->hdcp_cb_work);

		dp->hdcp_status = HDCP_STATE_AUTHENTICATING;
		queue_delayed_work(dp->wq, &dp->hdcp_cb_work, HZ / 2);
	}
#else
#ifdef SECDP_HDCP_DISABLE
	pr_info("skip hdcp\n");
#else
	schedule_delayed_work(&dp->sec.hdcp_start_work,
					msecs_to_jiffies(3500));
#endif
#ifndef CONFIG_SEC_FACTORY
	schedule_delayed_work(&dp->sec.link_status_work,
					msecs_to_jiffies(13000));
#else
	pr_info("skip chekcing poor connection\n");
#endif
#endif
end:
	/* clear framework event notifier */
	dp_display->post_open = NULL;
#ifdef CONFIG_SEC_DISPLAYPORT
	atomic_set(&dp->notification_status, 0);
#endif
	complete_all(&dp->notification_comp);
	mutex_unlock(&dp->session_lock);
	return 0;
}

static int dp_display_pre_disable(struct dp_display *dp_display)
{
	struct dp_display_private *dp;

	if (!dp_display) {
		pr_err("invalid input\n");
		return -EINVAL;
	}

	pr_debug("+++\n");

	dp = container_of(dp_display, struct dp_display_private, dp_display);

	mutex_lock(&dp->session_lock);

	if (!dp->power_on) {
		pr_debug("Link already powered off, return\n");
		goto end;
	}

#ifdef CONFIG_SEC_DISPLAYPORT
	if (dp->dp_display.is_connected) {
		/* send audio disconnect event to audio framework if DP is still connected.
		 * it can happen in case of platform reset with DP connection.
		 */
		pr_info("audio is still on DP side -> send audio disconnect event!\n");
		if (dp->audio_supported)
			dp->audio->off(dp->audio);
	}

	cancel_delayed_work_sync(&dp->sec.hdcp_start_work);
	cancel_delayed_work(&dp->sec.link_status_work);
#endif
	if (dp_display_is_hdcp_enabled(dp)) {
		dp->hdcp_status = HDCP_STATE_INACTIVE;

#ifndef CONFIG_SEC_DISPLAYPORT
		cancel_delayed_work_sync(&dp->hdcp_cb_work);
#else
		pr_info("cable <%d>\n", secdp_get_cable_status());
		cancel_delayed_work(&dp->hdcp_cb_work);
		usleep_range(3000, 5000);
#endif
		if (dp->hdcp.ops->off)
			dp->hdcp.ops->off(dp->hdcp.data);
	}

	dp->ctrl->push_idle(dp->ctrl);
end:
	mutex_unlock(&dp->session_lock);
	return 0;
}

static int dp_display_disable(struct dp_display *dp_display)
{
	struct dp_display_private *dp;

	if (!dp_display) {
		pr_err("invalid input\n");
		return -EINVAL;
	}

	pr_debug("+++\n");

	dp = container_of(dp_display, struct dp_display_private, dp_display);

	mutex_lock(&dp->session_lock);

	if (!dp->power_on || !dp->core_initialized) {
		pr_debug("Link already powered off, return\n");
		goto end;
	}

	dp->ctrl->off(dp->ctrl);
	dp->panel->deinit(dp->panel);

	dp->power_on = false;

end:
	complete_all(&dp->notification_comp);
#ifdef CONFIG_SEC_DISPLAYPORT
	atomic_set(&dp->notification_status, 0);
	complete(&dp->dp_off_comp);
#endif
	mutex_unlock(&dp->session_lock);

	pr_debug("---\n");
	return 0;
}

static int dp_request_irq(struct dp_display *dp_display)
{
	int rc = 0;
	struct dp_display_private *dp;

	if (!dp_display) {
		pr_err("invalid input\n");
		return -EINVAL;
	}

	dp = container_of(dp_display, struct dp_display_private, dp_display);

	dp->irq = irq_of_parse_and_map(dp->pdev->dev.of_node, 0);
	if (dp->irq < 0) {
		rc = dp->irq;
		pr_err("failed to get irq: %d\n", rc);
		return rc;
	}

	rc = devm_request_irq(&dp->pdev->dev, dp->irq, dp_display_irq,
		IRQF_TRIGGER_HIGH, "dp_display_isr", dp);
	if (rc < 0) {
		pr_err("failed to request IRQ%u: %d\n",
				dp->irq, rc);
		return rc;
	}
	disable_irq(dp->irq);

	return 0;
}

static struct dp_debug *dp_get_debug(struct dp_display *dp_display)
{
	struct dp_display_private *dp;

	if (!dp_display) {
		pr_err("invalid input\n");
		return ERR_PTR(-EINVAL);
	}

	dp = container_of(dp_display, struct dp_display_private, dp_display);

	return dp->debug;
}

static int dp_display_unprepare(struct dp_display *dp)
{
	return 0;
}

static int dp_display_validate_mode(struct dp_display *dp, u32 mode_pclk_khz)
{
	const u32 num_components = 3, default_bpp = 24;
	struct dp_display_private *dp_display;
	struct drm_dp_link *link_info;
	u32 mode_rate_khz = 0, supported_rate_khz = 0, mode_bpp = 0;

	if (!dp || !mode_pclk_khz || !dp->connector) {
		pr_err("invalid params\n");
		return -EINVAL;
	}

	dp_display = container_of(dp, struct dp_display_private, dp_display);
	link_info = &dp_display->panel->link_info;

	mode_bpp = dp->connector->display_info.bpc * num_components;
	if (!mode_bpp)
		mode_bpp = default_bpp;

	mode_bpp = dp_display->panel->get_mode_bpp(dp_display->panel,
			mode_bpp, mode_pclk_khz);

	mode_rate_khz = mode_pclk_khz * mode_bpp;
	supported_rate_khz = link_info->num_lanes * link_info->rate * 8;

	if (mode_rate_khz > supported_rate_khz)
		return MODE_BAD;

	return MODE_OK;
}

static int dp_display_get_modes(struct dp_display *dp,
	struct dp_display_mode *dp_mode)
{
	struct dp_display_private *dp_display;
	int ret = 0;

	if (!dp || !dp->connector) {
		pr_err("invalid params\n");
		return 0;
	}

	dp_display = container_of(dp, struct dp_display_private, dp_display);

	ret = dp_display->panel->get_modes(dp_display->panel,
		dp->connector, dp_mode);
	if (dp_mode->timing.pixel_clk_khz)
		dp->max_pclk_khz = dp_mode->timing.pixel_clk_khz;
	return ret;
}

static int dp_display_pre_kickoff(struct dp_display *dp_display,
			struct drm_msm_ext_hdr_metadata *hdr)
{
	int rc = 0;
	struct dp_display_private *dp;

	if (!dp_display) {
		pr_err("invalid input\n");
		return -EINVAL;
	}

	dp = container_of(dp_display, struct dp_display_private, dp_display);

	if (hdr->hdr_supported && dp->panel->hdr_supported(dp->panel))
		rc = dp->panel->setup_hdr(dp->panel, hdr);

	return rc;
}

static int dp_display_create_workqueue(struct dp_display_private *dp)
{
	dp->wq = create_singlethread_workqueue("drm_dp");
	if (IS_ERR_OR_NULL(dp->wq)) {
		pr_err("Error creating wq\n");
		return -EPERM;
	}

	INIT_DELAYED_WORK(&dp->hdcp_cb_work, dp_display_hdcp_cb_work);
	INIT_DELAYED_WORK(&dp->connect_work, dp_display_connect_work);
	INIT_WORK(&dp->attention_work, dp_display_attention_work);

	return 0;
}

static int dp_display_probe(struct platform_device *pdev)
{
	int rc = 0;
	struct dp_display_private *dp;

	if (!pdev || !pdev->dev.of_node) {
		pr_err("pdev not found\n");
		rc = -ENODEV;
		goto bail;
	}

	dp = devm_kzalloc(&pdev->dev, sizeof(*dp), GFP_KERNEL);
	if (!dp) {
		rc = -ENOMEM;
		goto bail;
	}

	init_completion(&dp->notification_comp);
#ifdef CONFIG_SEC_DISPLAYPORT
	init_completion(&dp->dp_off_comp);
	rc = switch_dev_register(&switch_secdp);
	if (rc)
		pr_info("Failed to register secdp switch(%d)\n", rc);

	rc = switch_dev_register(&switch_secdp_msg);
	if (rc)
		pr_info("Failed to register secdp_msg switch(%d)\n", rc);
#endif

	dp->pdev = pdev;
	dp->name = "drm_dp";
	dp->audio_status = -ENODEV;
	atomic_set(&dp->aborted, 0);

	rc = dp_display_create_workqueue(dp);
	if (rc) {
		pr_err("Failed to create workqueue\n");
		goto error;
	}

#ifdef CONFIG_SEC_DISPLAYPORT
	g_secdp_priv = dp;
	secdp_logger_init();
	atomic_set(&dp->notification_status, 0);
#endif

	platform_set_drvdata(pdev, dp);

	g_dp_display = &dp->dp_display;

	g_dp_display->enable        = dp_display_enable;
	g_dp_display->post_enable   = dp_display_post_enable;
	g_dp_display->pre_disable   = dp_display_pre_disable;
	g_dp_display->disable       = dp_display_disable;
	g_dp_display->set_mode      = dp_display_set_mode;
	g_dp_display->validate_mode = dp_display_validate_mode;
	g_dp_display->get_modes     = dp_display_get_modes;
	g_dp_display->prepare       = dp_display_prepare;
	g_dp_display->unprepare     = dp_display_unprepare;
	g_dp_display->request_irq   = dp_request_irq;
	g_dp_display->get_debug     = dp_get_debug;
	g_dp_display->post_open     = dp_display_post_open;
	g_dp_display->post_init     = dp_display_post_init;
	g_dp_display->pre_kickoff   = dp_display_pre_kickoff;

	rc = component_add(&pdev->dev, &dp_display_comp_ops);
	if (rc) {
		pr_err("component add failed, rc=%d\n", rc);
		goto error;
	}

	pr_info("exit, rc(%d)\n", rc);

	return 0;
error:
	devm_kfree(&pdev->dev, dp);
bail:
	return rc;
}

int dp_display_get_displays(void **displays, int count)
{
	if (!displays) {
		pr_err("invalid data\n");
		return -EINVAL;
	}

	if (count != 1) {
		pr_err("invalid number of displays\n");
		return -EINVAL;
	}

	displays[0] = g_dp_display;
	return count;
}

int dp_display_get_num_of_displays(void)
{
	return 1;
}

static int dp_display_remove(struct platform_device *pdev)
{
	struct dp_display_private *dp;

	if (!pdev)
		return -EINVAL;

	dp = platform_get_drvdata(pdev);

	dp_display_deinit_sub_modules(dp);

	platform_set_drvdata(pdev, NULL);
	devm_kfree(&pdev->dev, dp);

#ifdef CONFIG_SEC_DISPLAYPORT 
	switch_dev_unregister(&switch_secdp); 
	switch_dev_unregister(&switch_secdp_msg); 
#endif
	return 0;
}

static struct platform_driver dp_display_driver = {
	.probe  = dp_display_probe,
	.remove = dp_display_remove,
	.driver = {
		.name = "msm-dp-display",
		.of_match_table = dp_dt_match,
	},
};

static int __init dp_display_init(void)
{
	int ret;

	ret = platform_driver_register(&dp_display_driver);
	if (ret) {
		pr_err("driver register failed");
		return ret;
	}

	return ret;
}
late_initcall(dp_display_init);

static void __exit dp_display_cleanup(void)
{
	platform_driver_unregister(&dp_display_driver);
}
module_exit(dp_display_cleanup);

