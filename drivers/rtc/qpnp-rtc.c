/* Copyright (c) 2012-2015, 2017, The Linux Foundation. All rights reserved.
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

#include <linux/module.h>
#include <linux/regmap.h>
#include <linux/init.h>
#include <linux/rtc.h>
#include <linux/pm.h>
#include <linux/slab.h>
#include <linux/idr.h>
#include <linux/of_device.h>
#include <linux/of_irq.h>
#include <linux/spmi.h>
#include <linux/platform_device.h>
#include <linux/spinlock.h>
#include <linux/alarmtimer.h>
#ifdef CONFIG_RTC_AUTO_PWRON
#include <linux/reboot.h>
#include <linux/wakelock.h>
#include <linux/alarmtimer.h>
#include <linux/time.h>
#ifdef CONFIG_RTC_AUTO_PWRON_PARAM
#include <linux/sec_param.h>

#define SAPA_KPARAM_MAGIC	0x41504153
extern unsigned int sapa_param_time;
#endif

#define SAPA_START_POLL_TIME	(10LL * NSEC_PER_SEC)	/* 10 sec */
#define SAPA_BOOTING_TIME		(5*60)		/* 3 min */
#define SAPA_POLL_TIME			(15*60)		/* 15 min */

enum {
	SAPA_DISTANT = 0,
	SAPA_NEAR,
	SAPA_EXPIRED,
	SAPA_OVER
};
extern int lpcharge;
#endif

/* RTC/ALARM Register offsets */
#define REG_OFFSET_ALARM_RW	0x40
#define REG_OFFSET_ALARM_CTRL1	0x46
#define REG_OFFSET_ALARM_CTRL2	0x48
#define REG_OFFSET_RTC_WRITE	0x40
#define REG_OFFSET_RTC_CTRL	0x46
#define REG_OFFSET_RTC_READ	0x48
#define REG_OFFSET_PERP_SUBTYPE	0x05

/* RTC_CTRL register bit fields */
#define BIT_RTC_ENABLE		BIT(7)
#define BIT_RTC_ALARM_ENABLE	BIT(7)
#define BIT_RTC_ABORT_ENABLE	BIT(0)
#define BIT_RTC_ALARM_CLEAR	BIT(0)

/* RTC/ALARM peripheral subtype values */
#define RTC_PERPH_SUBTYPE       0x1
#define ALARM_PERPH_SUBTYPE     0x3

#define NUM_8_BIT_RTC_REGS	0x4

#define TO_SECS(arr)		(arr[0] | (arr[1] << 8) | (arr[2] << 16) | \
							(arr[3] << 24))

/* Module parameter to control power-on-alarm */
bool poweron_alarm = 0;	/* FALSE */
/* we don't use QC's poweron alarm feature
 * don't need to get the sysfs permission
EXPORT_SYMBOL(poweron_alarm);
module_param(poweron_alarm, bool, 0644);
MODULE_PARM_DESC(poweron_alarm, "Enable/Disable power-on alarm");
*/

/* rtc driver internal structure */
struct qpnp_rtc {
	u8			rtc_ctrl_reg;
	u8			alarm_ctrl_reg1;
	u16			rtc_base;
	u16			alarm_base;
	u32			rtc_write_enable;
	u32			rtc_alarm_powerup;
	int			rtc_alarm_irq;
	struct device		*rtc_dev;
	struct rtc_device	*rtc;
	struct platform_device	*pdev;
	struct regmap		*regmap;
	spinlock_t		alarm_ctrl_lock;
#ifdef CONFIG_RTC_AUTO_PWRON
	struct rtc_wkalrm	sapa;
	struct alarm		check_poll;
	struct work_struct	check_func;
	struct wake_lock	wakelock;
	int					lpm_mode;
	unsigned char		triggered;
#endif
};

static int qpnp_read_wrapper(struct qpnp_rtc *rtc_dd, u8 *rtc_val,
			u16 base, int count)
{
	int rc;

	rc = regmap_bulk_read(rtc_dd->regmap, base, rtc_val, count);
	if (rc) {
		dev_err(rtc_dd->rtc_dev, "SPMI read failed\n");
		return rc;
	}
	return 0;
}

static int qpnp_write_wrapper(struct qpnp_rtc *rtc_dd, u8 *rtc_val,
			u16 base, int count)
{
	int rc;

	rc = regmap_bulk_write(rtc_dd->regmap, base, rtc_val, count);
	if (rc) {
		dev_err(rtc_dd->rtc_dev, "SPMI write failed\n");
		return rc;
	}

	return 0;
}

static int
qpnp_rtc_set_time(struct device *dev, struct rtc_time *tm)
{
	int rc;
	unsigned long secs, irq_flags;
	u8 value[4], reg = 0, alarm_enabled = 0, ctrl_reg;
	u8 rtc_disabled = 0, rtc_ctrl_reg;
	struct qpnp_rtc *rtc_dd = dev_get_drvdata(dev);

	rtc_tm_to_time(tm, &secs);

	value[0] = secs & 0xFF;
	value[1] = (secs >> 8) & 0xFF;
	value[2] = (secs >> 16) & 0xFF;
	value[3] = (secs >> 24) & 0xFF;

	dev_dbg(dev, "Seconds value to be written to RTC = %lu\n", secs);

	spin_lock_irqsave(&rtc_dd->alarm_ctrl_lock, irq_flags);
	ctrl_reg = rtc_dd->alarm_ctrl_reg1;

	if (ctrl_reg & BIT_RTC_ALARM_ENABLE) {
		alarm_enabled = 1;
		ctrl_reg &= ~BIT_RTC_ALARM_ENABLE;
		rc = qpnp_write_wrapper(rtc_dd, &ctrl_reg,
			rtc_dd->alarm_base + REG_OFFSET_ALARM_CTRL1, 1);
		if (rc) {
			dev_err(dev, "Write to ALARM ctrl reg failed\n");
			goto rtc_rw_fail;
		}
	} else
		spin_unlock_irqrestore(&rtc_dd->alarm_ctrl_lock, irq_flags);

	/*
	 * 32 bit seconds value is coverted to four 8 bit values
	 *	|<------  32 bit time value in seconds  ------>|
	 *      <- 8 bit ->|<- 8 bit ->|<- 8 bit ->|<- 8 bit ->|
	 *       ----------------------------------------------
	 *      | BYTE[3]  |  BYTE[2]  |  BYTE[1]  |  BYTE[0]  |
	 *       ----------------------------------------------
	 *
	 * RTC has four 8 bit registers for writing time in seconds:
	 *             WDATA[3], WDATA[2], WDATA[1], WDATA[0]
	 *
	 * Write to the RTC registers should be done in following order
	 * Clear WDATA[0] register
	 *
	 * Write BYTE[1], BYTE[2] and BYTE[3] of time to
	 * RTC WDATA[3], WDATA[2], WDATA[1] registers
	 *
	 * Write BYTE[0] of time to RTC WDATA[0] register
	 *
	 * Clearing BYTE[0] and writing in the end will prevent any
	 * unintentional overflow from WDATA[0] to higher bytes during the
	 * write operation
	 */

	/* Disable RTC H/w before writing on RTC register*/
	rtc_ctrl_reg = rtc_dd->rtc_ctrl_reg;
	if (rtc_ctrl_reg & BIT_RTC_ENABLE) {
		rtc_disabled = 1;
		rtc_ctrl_reg &= ~BIT_RTC_ENABLE;
		rc = qpnp_write_wrapper(rtc_dd, &rtc_ctrl_reg,
				rtc_dd->rtc_base + REG_OFFSET_RTC_CTRL, 1);
		if (rc) {
			dev_err(dev, "Disabling of RTC control reg failed with error:%d\n",
				rc);
			goto rtc_rw_fail;
		}
		rtc_dd->rtc_ctrl_reg = rtc_ctrl_reg;
	}

	/* Clear WDATA[0] */
	reg = 0x0;
	rc = qpnp_write_wrapper(rtc_dd, &reg,
				rtc_dd->rtc_base + REG_OFFSET_RTC_WRITE, 1);
	if (rc) {
		dev_err(dev, "Write to RTC reg failed\n");
		goto rtc_rw_fail;
	}

	/* Write to WDATA[3], WDATA[2] and WDATA[1] */
	rc = qpnp_write_wrapper(rtc_dd, &value[1],
			rtc_dd->rtc_base + REG_OFFSET_RTC_WRITE + 1, 3);
	if (rc) {
		dev_err(dev, "Write to RTC reg failed\n");
		goto rtc_rw_fail;
	}

	/* Write to WDATA[0] */
	rc = qpnp_write_wrapper(rtc_dd, value,
				rtc_dd->rtc_base + REG_OFFSET_RTC_WRITE, 1);
	if (rc) {
		dev_err(dev, "Write to RTC reg failed\n");
		goto rtc_rw_fail;
	}

	/* Enable RTC H/w after writing on RTC register*/
	if (rtc_disabled) {
		rtc_ctrl_reg |= BIT_RTC_ENABLE;
		rc = qpnp_write_wrapper(rtc_dd, &rtc_ctrl_reg,
				rtc_dd->rtc_base + REG_OFFSET_RTC_CTRL, 1);
		if (rc) {
			dev_err(dev, "Enabling of RTC control reg failed with error:%d\n",
				rc);
			goto rtc_rw_fail;
		}
		rtc_dd->rtc_ctrl_reg = rtc_ctrl_reg;
	}

	if (alarm_enabled) {
		ctrl_reg |= BIT_RTC_ALARM_ENABLE;
		rc = qpnp_write_wrapper(rtc_dd, &ctrl_reg,
			rtc_dd->alarm_base + REG_OFFSET_ALARM_CTRL1, 1);
		if (rc) {
			dev_err(dev, "Write to ALARM ctrl reg failed\n");
			goto rtc_rw_fail;
		}
	}

	rtc_dd->alarm_ctrl_reg1 = ctrl_reg;
#ifdef CONFIG_RTC_AUTO_PWRON
	pr_info("%s : secs = %lu, h:m:s == %d:%d:%d, d/m/y = %d/%d/%d\n",
			__func__, secs, tm->tm_hour, tm->tm_min, tm->tm_sec,
			tm->tm_mday, tm->tm_mon, tm->tm_year);
#endif

rtc_rw_fail:
	if (alarm_enabled)
		spin_unlock_irqrestore(&rtc_dd->alarm_ctrl_lock, irq_flags);

	return rc;
}

static int
qpnp_rtc_read_time(struct device *dev, struct rtc_time *tm)
{
	int rc;
	u8 value[4], reg;
	unsigned long secs;
	struct qpnp_rtc *rtc_dd = dev_get_drvdata(dev);

	rc = qpnp_read_wrapper(rtc_dd, value,
				rtc_dd->rtc_base + REG_OFFSET_RTC_READ,
				NUM_8_BIT_RTC_REGS);
	if (rc) {
		dev_err(dev, "Read from RTC reg failed\n");
		return rc;
	}

	/*
	 * Read the LSB again and check if there has been a carry over
	 * If there is, redo the read operation
	 */
	rc = qpnp_read_wrapper(rtc_dd, &reg,
				rtc_dd->rtc_base + REG_OFFSET_RTC_READ, 1);
	if (rc) {
		dev_err(dev, "Read from RTC reg failed\n");
		return rc;
	}

	if (reg < value[0]) {
		rc = qpnp_read_wrapper(rtc_dd, value,
				rtc_dd->rtc_base + REG_OFFSET_RTC_READ,
				NUM_8_BIT_RTC_REGS);
		if (rc) {
			dev_err(dev, "Read from RTC reg failed\n");
			return rc;
		}
	}

	secs = TO_SECS(value);

	rtc_time_to_tm(secs, tm);

	rc = rtc_valid_tm(tm);
	if (rc) {
		dev_err(dev, "Invalid time read from RTC\n");
		return rc;
	}

	dev_dbg(dev, "secs = %lu, h:m:s == %d:%d:%d, d/m/y = %d/%d/%d\n",
			secs, tm->tm_hour, tm->tm_min, tm->tm_sec,
			tm->tm_mday, tm->tm_mon, tm->tm_year);

	return 0;
}

static int
qpnp_rtc_set_alarm(struct device *dev, struct rtc_wkalrm *alarm)
{
	int rc;
	u8 value[4], ctrl_reg;
	unsigned long secs, secs_rtc, irq_flags;
	struct qpnp_rtc *rtc_dd = dev_get_drvdata(dev);
	struct rtc_time rtc_tm;

	rtc_tm_to_time(&alarm->time, &secs);

	/*
	 * Read the current RTC time and verify if the alarm time is in the
	 * past. If yes, return invalid
	 */
	rc = qpnp_rtc_read_time(dev, &rtc_tm);
	if (rc) {
		dev_err(dev, "Unable to read RTC time\n");
		return -EINVAL;
	}

	rtc_tm_to_time(&rtc_tm, &secs_rtc);
	if (secs < secs_rtc) {
		dev_err(dev, "Trying to set alarm in the past\n");
		return -EINVAL;
	}

	value[0] = secs & 0xFF;
	value[1] = (secs >> 8) & 0xFF;
	value[2] = (secs >> 16) & 0xFF;
	value[3] = (secs >> 24) & 0xFF;

	spin_lock_irqsave(&rtc_dd->alarm_ctrl_lock, irq_flags);

	rc = qpnp_write_wrapper(rtc_dd, value,
				rtc_dd->alarm_base + REG_OFFSET_ALARM_RW,
				NUM_8_BIT_RTC_REGS);
	if (rc) {
		dev_err(dev, "Write to ALARM reg failed\n");
		goto rtc_rw_fail;
	}

	ctrl_reg = (alarm->enabled) ?
			(rtc_dd->alarm_ctrl_reg1 | BIT_RTC_ALARM_ENABLE) :
			(rtc_dd->alarm_ctrl_reg1 & ~BIT_RTC_ALARM_ENABLE);

	rc = qpnp_write_wrapper(rtc_dd, &ctrl_reg,
			rtc_dd->alarm_base + REG_OFFSET_ALARM_CTRL1, 1);
	if (rc) {
		dev_err(dev, "Write to ALARM cntrol reg failed\n");
		goto rtc_rw_fail;
	}

	rtc_dd->alarm_ctrl_reg1 = ctrl_reg;

	dev_dbg(dev, "Alarm Set for h:r:s=%d:%d:%d, d/m/y=%d/%d/%d\n",
			alarm->time.tm_hour, alarm->time.tm_min,
			alarm->time.tm_sec, alarm->time.tm_mday,
			alarm->time.tm_mon, alarm->time.tm_year);
rtc_rw_fail:
	spin_unlock_irqrestore(&rtc_dd->alarm_ctrl_lock, irq_flags);
	return rc;
}

static int
qpnp_rtc_read_alarm(struct device *dev, struct rtc_wkalrm *alarm)
{
	int rc;
	u8 value[4];
	unsigned long secs;
	struct qpnp_rtc *rtc_dd = dev_get_drvdata(dev);

	rc = qpnp_read_wrapper(rtc_dd, value,
				rtc_dd->alarm_base + REG_OFFSET_ALARM_RW,
				NUM_8_BIT_RTC_REGS);
	if (rc) {
		dev_err(dev, "Read from ALARM reg failed\n");
		return rc;
	}

	secs = TO_SECS(value);
	rtc_time_to_tm(secs, &alarm->time);

	rc = rtc_valid_tm(&alarm->time);
	if (rc) {
		dev_err(dev, "Invalid time read from RTC\n");
		return rc;
	}

	dev_dbg(dev, "Alarm set for - h:r:s=%d:%d:%d, d/m/y=%d/%d/%d\n",
		alarm->time.tm_hour, alarm->time.tm_min,
				alarm->time.tm_sec, alarm->time.tm_mday,
				alarm->time.tm_mon, alarm->time.tm_year);

	return 0;
}


static int
qpnp_rtc_alarm_irq_enable(struct device *dev, unsigned int enabled)
{
	int rc;
	unsigned long irq_flags;
	struct qpnp_rtc *rtc_dd = dev_get_drvdata(dev);
	u8 ctrl_reg;
	u8 value[4] = {0};
#ifdef CONFIG_RTC_AUTO_PWRON
	pr_info("sapa irq=%d\n", enabled);
#endif /* CONFIG_RTC_AUTO_PWRON */
	spin_lock_irqsave(&rtc_dd->alarm_ctrl_lock, irq_flags);
	ctrl_reg = rtc_dd->alarm_ctrl_reg1;
	ctrl_reg = enabled ? (ctrl_reg | BIT_RTC_ALARM_ENABLE) :
				(ctrl_reg & ~BIT_RTC_ALARM_ENABLE);

	rc = qpnp_write_wrapper(rtc_dd, &ctrl_reg,
			rtc_dd->alarm_base + REG_OFFSET_ALARM_CTRL1, 1);
	if (rc) {
		dev_err(dev, "Write to ALARM control reg failed\n");
		goto rtc_rw_fail;
	}

	rtc_dd->alarm_ctrl_reg1 = ctrl_reg;

	/* Clear Alarm register */
	if (!enabled) {
		rc = qpnp_write_wrapper(rtc_dd, value,
			rtc_dd->alarm_base + REG_OFFSET_ALARM_RW,
			NUM_8_BIT_RTC_REGS);
		if (rc)
			dev_err(dev, "Clear ALARM value reg failed\n");
	}

rtc_rw_fail:
	spin_unlock_irqrestore(&rtc_dd->alarm_ctrl_lock, irq_flags);
	return rc;
}

#ifdef CONFIG_RTC_AUTO_PWRON
static void
sapa_normalize_alarm(struct rtc_wkalrm *alarm)
{
	if (!alarm->enabled) {
	/* 50 years after RTC reset */
		alarm->time.tm_year = 70 + 50;
		alarm->time.tm_mon = 1;
		alarm->time.tm_mday = 1;
		alarm->time.tm_hour = 1;
		alarm->time.tm_min = 1;
		alarm->time.tm_sec = 1;
	}
}

#ifdef CONFIG_RTC_AUTO_PWRON_PARAM
static void
sapa_save_kparam(struct qpnp_rtc *rtc_dd)
{
	unsigned long secs_pwron;
	unsigned int sapa[3];
	int rc;

	sapa_normalize_alarm(&rtc_dd->sapa);
	rtc_tm_to_time(&rtc_dd->sapa.time, &secs_pwron);
	sapa[0] = SAPA_KPARAM_MAGIC;
	sapa[1] = (unsigned int)rtc_dd->sapa.enabled;
	sapa[2] = (unsigned int)secs_pwron;

	rc = sec_set_param(param_index_sapa, sapa);
	pr_info("sapa: %s rc=%d, enabled=%d, alarm=%u\n",
			__func__, rc, sapa[1], sapa[2]);
}

#endif
static int
sapa_is_testalarm(struct rtc_wkalrm *alarm)
{
	unsigned long alm_sec;

	rtc_tm_to_time(&alarm->time, &alm_sec);
	return (alm_sec % 2);
}

static int
sapa_rtc_getalarm(struct device *dev, struct rtc_wkalrm *alarm)
{
	struct qpnp_rtc *rtc_dd = dev_get_drvdata(dev);

	alarm->enabled = rtc_dd->triggered;
	return 1;
}

static int
sapa_rtc_setalarm(struct device *dev, struct rtc_wkalrm *alarm)
{
	struct qpnp_rtc *rtc_dd = dev_get_drvdata(dev);

	memcpy(&rtc_dd->sapa, alarm, sizeof(struct rtc_wkalrm));
#ifdef CONFIG_RTC_AUTO_PWRON_PARAM
	sapa_save_kparam(rtc_dd);
#endif

	return 0;
}

static int
sapa_check_state(struct qpnp_rtc *rtc_dd, unsigned long *data)
{
	unsigned long rtc_secs;
	unsigned long secs_pwron;
	u8 value[4];
	int rc;
	int res = SAPA_NEAR;

	rc = qpnp_read_wrapper(rtc_dd, value,
			rtc_dd->rtc_base + REG_OFFSET_RTC_READ,
			NUM_8_BIT_RTC_REGS);
	if (rc)
		pr_err("%s: rtc read failed.\n", __func__);
	rtc_secs = TO_SECS(value);

	rtc_tm_to_time(&rtc_dd->sapa.time, &secs_pwron);

	if (rtc_secs < secs_pwron) {
		if (secs_pwron - rtc_secs > SAPA_POLL_TIME)
			res = SAPA_DISTANT;
		if (data)
			*data = secs_pwron - rtc_secs;
	} else if (rtc_secs <= secs_pwron + SAPA_BOOTING_TIME) {
		res = SAPA_EXPIRED;
		if (data)
			*data = rtc_secs + 10;
	} else
		res = SAPA_OVER;

	pr_info("%s: rtc:%lu, alrm:%lu[%d]\n",
				__func__, rtc_secs, secs_pwron, res);
	return res;
}

static void sapa_check_func(struct work_struct *work)
{
	struct qpnp_rtc *rtc_dd =
			container_of(work, struct qpnp_rtc, check_func);
	int res;
	unsigned long remain;

	res = sapa_check_state(rtc_dd, &remain);
	if (res <= SAPA_NEAR) {
		ktime_t kt;

		if (res == SAPA_DISTANT)
			remain = SAPA_POLL_TIME;
		kt = ns_to_ktime((u64)remain * NSEC_PER_SEC);
		alarm_start_relative(&rtc_dd->check_poll, kt);
		pr_info("%s: next %lu s\n", __func__, remain);
	} else if (res == SAPA_EXPIRED) {
		wake_lock(&rtc_dd->wakelock);
		rtc_dd->triggered = 1;
	}
}

static enum alarmtimer_restart
sapa_check_callback(struct alarm *alarm, ktime_t now)
{
	struct qpnp_rtc *rtc_dd =
			container_of(alarm, struct qpnp_rtc, check_poll);

	schedule_work(&rtc_dd->check_func);
	return ALARMTIMER_NORESTART;
}

static void
sapa_load_alarm(struct qpnp_rtc *rtc_dd, u8 ctrl_reg)
{
	unsigned long alarm_secs;
	u8 value[4];
	int rc;

	rc = qpnp_read_wrapper(rtc_dd, value,
				rtc_dd->alarm_base + REG_OFFSET_ALARM_RW,
				NUM_8_BIT_RTC_REGS);
	if (rc) {
		pr_err("%s: alarm read failed\n", __func__);
		return;
	}
	alarm_secs = TO_SECS(value);

#ifdef CONFIG_RTC_AUTO_PWRON_PARAM
	pr_info("%s: param=%u\n", __func__, sapa_param_time);
	rtc_time_to_tm(sapa_param_time, &rtc_dd->sapa.time);
	rtc_dd->sapa.enabled = (sapa_param_time) ? 1 : 0;
#else
	rtc_time_to_tm(alarm_secs, &rtc_dd->sapa.time);
	rtc_dd->sapa.enabled = (ctrl_reg & BIT_RTC_ALARM_ENABLE) ? 1 : 0;
#endif /* CONFIG_RTC_AUTO_PWRON_PARAM  */

	pr_info("%s: alarm_reg=%02x, pmic=%lu\n",
				__func__, ctrl_reg, alarm_secs);
}

static void sapa_init(struct qpnp_rtc *rtc_dd)
{
	ktime_t kt;

	rtc_dd->lpm_mode = lpcharge;
	rtc_dd->triggered = 0;

	if (rtc_dd->lpm_mode && rtc_dd->sapa.enabled) {
		wake_lock_init(&rtc_dd->wakelock, WAKE_LOCK_SUSPEND, "SAPA");

		alarm_init(&rtc_dd->check_poll,
					ALARM_REALTIME, sapa_check_callback);
		INIT_WORK(&rtc_dd->check_func, sapa_check_func);

		kt = ns_to_ktime(SAPA_START_POLL_TIME);
		alarm_start_relative(&rtc_dd->check_poll, kt);
	}
}

static void sapa_exit(struct qpnp_rtc *rtc_dd)
{
	struct rtc_wkalrm *alarm;
	int rc;

	pr_info("%s\n", __func__);

	if (rtc_dd->lpm_mode && rtc_dd->sapa.enabled) {
		cancel_work_sync(&rtc_dd->check_func);
		alarm_cancel(&rtc_dd->check_poll);
		wake_lock_destroy(&rtc_dd->wakelock);
	}

	if (!rtc_dd->triggered) {
		if (rtc_dd->sapa.enabled) {
			unsigned long next_power_on;
			int res = sapa_check_state(rtc_dd, &next_power_on);

			if (res >= SAPA_EXPIRED &&
					!sapa_is_testalarm(&rtc_dd->sapa)) {
				rtc_time_to_tm(next_power_on,
						&rtc_dd->sapa.time);
				pr_info("%s: adjust %lu\n", __func__,
						next_power_on);
			} else if (res >= SAPA_EXPIRED) {
				rtc_dd->sapa.enabled = 0;
				pr_info("%s: over - clear\n", __func__);
			}
		}
	} else {
		rtc_dd->sapa.enabled = 0;
	}

	alarm = &rtc_dd->sapa;
	sapa_normalize_alarm(alarm);

	rc = qpnp_rtc_set_alarm(rtc_dd->rtc_dev, alarm);
	if (rc < 0)
		pr_err("%s: err=%d\n", __func__, rc);

	rc = qpnp_rtc_read_alarm(rtc_dd->rtc_dev, alarm);
	if (!rc) {
		pr_info("%s: %d-%02d-%02d %02d:%02d:%02d\n",
			__func__, alarm->time.tm_year, alarm->time.tm_mon,
			alarm->time.tm_mday, alarm->time.tm_hour,
			alarm->time.tm_min, alarm->time.tm_sec);
	}

}
#endif /* CONFIG_RTC_AUTO_PWRON */

static const struct rtc_class_ops qpnp_rtc_ro_ops = {
	.read_time = qpnp_rtc_read_time,
	.set_alarm = qpnp_rtc_set_alarm,
	.read_alarm = qpnp_rtc_read_alarm,
#ifdef CONFIG_RTC_AUTO_PWRON
	.read_bootalarm = sapa_rtc_getalarm,
	.set_bootalarm  = sapa_rtc_setalarm,
#endif /* CONFIG_RTC_AUTO_PWRON */
	.alarm_irq_enable = qpnp_rtc_alarm_irq_enable,
};

static const struct rtc_class_ops qpnp_rtc_rw_ops = {
	.read_time = qpnp_rtc_read_time,
	.set_alarm = qpnp_rtc_set_alarm,
	.read_alarm = qpnp_rtc_read_alarm,
#ifdef CONFIG_RTC_AUTO_PWRON
	.read_bootalarm = sapa_rtc_getalarm,
	.set_bootalarm  = sapa_rtc_setalarm,
#endif /*CONFIG_RTC_AUTO_PWRON*/
	.alarm_irq_enable = qpnp_rtc_alarm_irq_enable,
	.set_time = qpnp_rtc_set_time,
};

static irqreturn_t qpnp_alarm_trigger(int irq, void *dev_id)
{
	struct qpnp_rtc *rtc_dd = dev_id;
	u8 ctrl_reg;
	int rc;
	unsigned long irq_flags;

	rtc_update_irq(rtc_dd->rtc, 1, RTC_IRQF | RTC_AF);

	spin_lock_irqsave(&rtc_dd->alarm_ctrl_lock, irq_flags);

	/* Clear the alarm enable bit */
	ctrl_reg = rtc_dd->alarm_ctrl_reg1;
	ctrl_reg &= ~BIT_RTC_ALARM_ENABLE;

	rc = qpnp_write_wrapper(rtc_dd, &ctrl_reg,
			rtc_dd->alarm_base + REG_OFFSET_ALARM_CTRL1, 1);
	if (rc) {
		spin_unlock_irqrestore(&rtc_dd->alarm_ctrl_lock, irq_flags);
		dev_err(rtc_dd->rtc_dev,
				"Write to ALARM control reg failed\n");
		goto rtc_alarm_handled;
	}

	rtc_dd->alarm_ctrl_reg1 = ctrl_reg;
	spin_unlock_irqrestore(&rtc_dd->alarm_ctrl_lock, irq_flags);

	/* Set ALARM_CLR bit */
	ctrl_reg = 0x1;
	rc = qpnp_write_wrapper(rtc_dd, &ctrl_reg,
			rtc_dd->alarm_base + REG_OFFSET_ALARM_CTRL2, 1);
	if (rc)
		dev_err(rtc_dd->rtc_dev,
				"Write to ALARM control reg failed\n");

rtc_alarm_handled:
	return IRQ_HANDLED;
}

static int qpnp_rtc_probe(struct platform_device *pdev)
{
	const struct rtc_class_ops *rtc_ops = &qpnp_rtc_ro_ops;
	int rc;
	u8 subtype;
	struct qpnp_rtc *rtc_dd;
	unsigned int base;
	struct device_node *child;

	rtc_dd = devm_kzalloc(&pdev->dev, sizeof(*rtc_dd), GFP_KERNEL);
	if (rtc_dd == NULL)
		return -ENOMEM;

	rtc_dd->regmap = dev_get_regmap(pdev->dev.parent, NULL);
	if (!rtc_dd->regmap) {
		dev_err(&pdev->dev, "Couldn't get parent's regmap\n");
		return -EINVAL;
	}

	/* Get the rtc write property */
	rc = of_property_read_u32(pdev->dev.of_node, "qcom,qpnp-rtc-write",
						&rtc_dd->rtc_write_enable);
	if (rc && rc != -EINVAL) {
		dev_err(&pdev->dev,
			"Error reading rtc_write_enable property %d\n", rc);
		return rc;
	}

	rc = of_property_read_u32(pdev->dev.of_node,
						"qcom,qpnp-rtc-alarm-pwrup",
						&rtc_dd->rtc_alarm_powerup);
	if (rc && rc != -EINVAL) {
		dev_err(&pdev->dev,
			"Error reading rtc_alarm_powerup property %d\n", rc);
		return rc;
	}

	/* Initialise spinlock to protect RTC control register */
	spin_lock_init(&rtc_dd->alarm_ctrl_lock);

	rtc_dd->rtc_dev = &(pdev->dev);
	rtc_dd->pdev = pdev;


	if (of_get_available_child_count(pdev->dev.of_node) == 0) {
		pr_err("no child nodes\n");
		rc = -ENXIO;
		goto fail_rtc_enable;
	}

	/* Get RTC/ALARM resources */
	for_each_available_child_of_node(pdev->dev.of_node, child) {
		rc = of_property_read_u32(child, "reg", &base);
		if (rc < 0) {
			dev_err(&pdev->dev,
				"Couldn't find reg in node = %s rc = %d\n",
				child->full_name, rc);
			goto fail_rtc_enable;
		}

		rc = qpnp_read_wrapper(rtc_dd, &subtype,
				base + REG_OFFSET_PERP_SUBTYPE, 1);
		if (rc) {
			dev_err(&pdev->dev,
				"Peripheral subtype read failed\n");
			goto fail_rtc_enable;
		}

		switch (subtype) {
		case RTC_PERPH_SUBTYPE:
			rtc_dd->rtc_base = base;
			break;
		case ALARM_PERPH_SUBTYPE:
			rtc_dd->alarm_base = base;
			rtc_dd->rtc_alarm_irq = of_irq_get(child, 0);
			if (rtc_dd->rtc_alarm_irq < 0) {
				dev_err(&pdev->dev, "ALARM IRQ absent\n");
				rc = -ENXIO;
				goto fail_rtc_enable;
			}
			break;
		default:
			dev_err(&pdev->dev, "Invalid peripheral subtype\n");
			rc = -EINVAL;
			goto fail_rtc_enable;
		}
	}

	rc = qpnp_read_wrapper(rtc_dd, &rtc_dd->rtc_ctrl_reg,
				rtc_dd->rtc_base + REG_OFFSET_RTC_CTRL, 1);
	if (rc) {
		dev_err(&pdev->dev, "Read from RTC control reg failed\n");
		goto fail_rtc_enable;
	}

	if (!(rtc_dd->rtc_ctrl_reg & BIT_RTC_ENABLE)) {
		dev_err(&pdev->dev, "RTC h/w disabled, rtc not registered\n");
		goto fail_rtc_enable;
	}

	rc = qpnp_read_wrapper(rtc_dd, &rtc_dd->alarm_ctrl_reg1,
				rtc_dd->alarm_base + REG_OFFSET_ALARM_CTRL1, 1);
	if (rc) {
		dev_err(&pdev->dev, "Read from  Alarm control reg failed\n");
		goto fail_rtc_enable;
	}
#ifdef CONFIG_RTC_AUTO_PWRON
	sapa_load_alarm(rtc_dd, rtc_dd->alarm_ctrl_reg1);
#endif

	/* Enable abort enable feature */
	rtc_dd->alarm_ctrl_reg1 |= BIT_RTC_ABORT_ENABLE;
	rc = qpnp_write_wrapper(rtc_dd, &rtc_dd->alarm_ctrl_reg1,
			rtc_dd->alarm_base + REG_OFFSET_ALARM_CTRL1, 1);
	if (rc) {
		dev_err(&pdev->dev, "SPMI write failed!\n");
		goto fail_rtc_enable;
	}

	if (rtc_dd->rtc_write_enable == true)
		rtc_ops = &qpnp_rtc_rw_ops;

	dev_set_drvdata(&pdev->dev, rtc_dd);

	/* Register the RTC device */
	rtc_dd->rtc = rtc_device_register("qpnp_rtc", &pdev->dev,
					  rtc_ops, THIS_MODULE);
	if (IS_ERR(rtc_dd->rtc)) {
		dev_err(&pdev->dev, "%s: RTC registration failed (%ld)\n",
					__func__, PTR_ERR(rtc_dd->rtc));
		rc = PTR_ERR(rtc_dd->rtc);
		goto fail_rtc_enable;
	}

	/* Request the alarm IRQ */
	rc = request_any_context_irq(rtc_dd->rtc_alarm_irq,
				 qpnp_alarm_trigger, IRQF_TRIGGER_RISING,
				 "qpnp_rtc_alarm", rtc_dd);
	if (rc) {
		dev_err(&pdev->dev, "Request IRQ failed (%d)\n", rc);
		goto fail_req_irq;
	}

#ifdef CONFIG_RTC_AUTO_PWRON
	sapa_init(rtc_dd);
#endif

	device_init_wakeup(&pdev->dev, 1);
	enable_irq_wake(rtc_dd->rtc_alarm_irq);

	dev_dbg(&pdev->dev, "Probe success !!\n");

	return 0;

fail_req_irq:
	rtc_device_unregister(rtc_dd->rtc);
fail_rtc_enable:
	dev_set_drvdata(&pdev->dev, NULL);

	return rc;
}

static int qpnp_rtc_remove(struct platform_device *pdev)
{
	struct qpnp_rtc *rtc_dd = dev_get_drvdata(&pdev->dev);

	device_init_wakeup(&pdev->dev, 0);
	free_irq(rtc_dd->rtc_alarm_irq, rtc_dd);
	rtc_device_unregister(rtc_dd->rtc);
	dev_set_drvdata(&pdev->dev, NULL);

	return 0;
}

static void qpnp_rtc_shutdown(struct platform_device *pdev)
{
#ifdef CONFIG_RTC_AUTO_PWRON
	struct qpnp_rtc *rtc_dd;
#else
	u8 value[4] = {0};
	u8 reg;
	int rc;
	unsigned long irq_flags;
	struct qpnp_rtc *rtc_dd;
	bool rtc_alarm_powerup;
#endif /* CONFIG_RTC_AUTO_PWRON */

	if (!pdev) {
		pr_err("qpnp-rtc: spmi device not found\n");
		return;
	}
	rtc_dd = dev_get_drvdata(&pdev->dev);
	if (!rtc_dd) {
		pr_err("qpnp-rtc: rtc driver data not found\n");
		return;
	}
#ifdef CONFIG_RTC_AUTO_PWRON
	sapa_exit(rtc_dd);
#else
	rtc_alarm_powerup = rtc_dd->rtc_alarm_powerup;
	if (!rtc_alarm_powerup && !poweron_alarm) {
		spin_lock_irqsave(&rtc_dd->alarm_ctrl_lock, irq_flags);
		dev_dbg(&pdev->dev, "Disabling alarm interrupts\n");

		/* Disable RTC alarms */
		reg = rtc_dd->alarm_ctrl_reg1;
		reg &= ~BIT_RTC_ALARM_ENABLE;
		rc = qpnp_write_wrapper(rtc_dd, &reg,
			rtc_dd->alarm_base + REG_OFFSET_ALARM_CTRL1, 1);
		if (rc) {
			dev_err(rtc_dd->rtc_dev, "SPMI write failed\n");
			goto fail_alarm_disable;
		}

		/* Clear Alarm register */
		rc = qpnp_write_wrapper(rtc_dd, value,
				rtc_dd->alarm_base + REG_OFFSET_ALARM_RW,
				NUM_8_BIT_RTC_REGS);
		if (rc)
			dev_err(rtc_dd->rtc_dev, "SPMI write failed\n");

fail_alarm_disable:
		spin_unlock_irqrestore(&rtc_dd->alarm_ctrl_lock, irq_flags);
	}
#endif /* CONFIG_RTC_AUTO_PWRON */
}

static const struct of_device_id spmi_match_table[] = {
	{
		.compatible = "qcom,qpnp-rtc",
	},
	{}
};

static struct platform_driver qpnp_rtc_driver = {
	.probe		= qpnp_rtc_probe,
	.remove		= qpnp_rtc_remove,
	.shutdown	= qpnp_rtc_shutdown,
	.driver		= {
		.name		= "qcom,qpnp-rtc",
		.owner		= THIS_MODULE,
		.of_match_table	= spmi_match_table,
	},
};

static int __init qpnp_rtc_init(void)
{
	return platform_driver_register(&qpnp_rtc_driver);
}
module_init(qpnp_rtc_init);

static void __exit qpnp_rtc_exit(void)
{
	platform_driver_unregister(&qpnp_rtc_driver);
}
module_exit(qpnp_rtc_exit);

MODULE_DESCRIPTION("SMPI PMIC RTC driver");
MODULE_LICENSE("GPL v2");
