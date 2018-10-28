/*
 * include/linux/sec_debug.h
 *
 * COPYRIGHT(C) 2006-2018 Samsung Electronics Co., Ltd. All Right Reserved.
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

#ifndef SEC_DEBUG_H
#define SEC_DEBUG_H

#include <linux/ftrace.h>
#include <linux/of_address.h>
#include <linux/reboot.h>
#include <linux/sched.h>
#include <linux/semaphore.h>

#include <asm/sec_debug.h>
#include <asm/cacheflush.h>
#include <asm/io.h>

#define __SEC_DEBUG_SCHED_LOG_INDIRECT
#include <linux/sec_debug_sched_log.h>
#undef __SEC_DEBUG_SCHED_LOG_INDIRECT

#define IRQ_ENTRY	0x4945
#define IRQ_EXIT	0x4958

#define SOFTIRQ_ENTRY	0x5345
#define SOFTIRQ_EXIT	0x5358

#define SCM_ENTRY	0x5555
#define SCM_EXIT	0x6666

#define SEC_DEBUG_MODEM_SEPARATE_EN  0xEAEAEAEA
#define SEC_DEBUG_MODEM_SEPARATE_DIS 0xDEADDEAD

#define RESET_EXTRA_INFO_SIZE	1024

enum sec_debug_upload_cause_t {
	UPLOAD_CAUSE_INIT = 0xCAFEBABE,
	UPLOAD_CAUSE_KERNEL_PANIC = 0x000000C8,
	UPLOAD_CAUSE_POWER_LONG_PRESS = 0x00000085,
	UPLOAD_CAUSE_FORCED_UPLOAD = 0x00000022,
	UPLOAD_CAUSE_USER_FORCED_UPLOAD = 0x00009890,
	UPLOAD_CAUSE_CP_ERROR_FATAL = 0x000000CC,
	UPLOAD_CAUSE_MDM_ERROR_FATAL = 0x000000EE,
	UPLOAD_CAUSE_USER_FAULT = 0x0000002F,
	UPLOAD_CAUSE_HSIC_DISCONNECTED = 0x000000DD,
	UPLOAD_CAUSE_MODEM_RST_ERR = 0x000000FC,
	UPLOAD_CAUSE_RIVA_RST_ERR = 0x000000FB,
	UPLOAD_CAUSE_LPASS_RST_ERR = 0x000000FA,
	UPLOAD_CAUSE_DSPS_RST_ERR = 0x000000FD,
	UPLOAD_CAUSE_PERIPHERAL_ERR = 0x000000FF,
	UPLOAD_CAUSE_NON_SECURE_WDOG_BARK = 0x00000DBA,
	UPLOAD_CAUSE_NON_SECURE_WDOG_BITE = 0x00000DBE,
	UPLOAD_CAUSE_POWER_THERMAL_RESET = 0x00000075,
	UPLOAD_CAUSE_SECURE_WDOG_BITE = 0x00005DBE,
	UPLOAD_CAUSE_BUS_HANG = 0x000000B5,
#if defined(CONFIG_SEC_NAD)
	UPLOAD_CAUSE_NAD_CRYPTO = 0x00000ACF,
	UPLOAD_CAUSE_NAD_ICACHE = 0x00000ACA,
	UPLOAD_CAUSE_NAD_CACHECOHERENCY = 0x00000ACC,
	UPLOAD_CAUSE_NAD_SUSPEND = 0x00000A3E,
	UPLOAD_CAUSE_NAD_VDDMIN = 0x00000ADD,
	UPLOAD_CAUSE_NAD_QMESADDR = 0x00000A29,
	UPLOAD_CAUSE_NAD_QMESACACHE = 0x00000AED,
	UPLOAD_CAUSE_NAD_PMIC = 0x00000AB8,
	UPLOAD_CAUSE_NAD_UFS = 0x00000AF5,
	UPLOAD_CAUSE_NAD_SDCARD = 0x00000A7C,
	UPLOAD_CAUSE_NAD_SENSOR = 0x00000A9C,
	UPLOAD_CAUSE_NAD_QDAF_FAIL = 0x00000A9D,
	UPLOAD_CAUSE_NAD_FAIL = 0x00000A65,
	UPLOAD_CAUSE_DDR_TEST =	0x00000A30,
	UPLOAD_CAUSE_DDR_TEST_FOR_MAIN = 0x00000A35,
	UPLOAD_CAUSE_DDR_TEST_FOR_SSLT = 0x00000A3A,
	UPLOAD_CAUSE_DDR_TEST_FOR_SMD = 0x00000A3F,
#endif
};

enum sec_restart_reason_t {
	RESTART_REASON_NORMAL = 0x0,
	RESTART_REASON_BOOTLOADER = 0x77665500,
	RESTART_REASON_REBOOT = 0x77665501,
	RESTART_REASON_RECOVERY = 0x77665502,
	RESTART_REASON_RTC = 0x77665503,
	RESTART_REASON_DMVERITY_CORRUPTED = 0x77665508,
	RESTART_REASON_DMVERITY_ENFORCE = 0x77665509,
	RESTART_REASON_KEYS_CLEAR = 0x7766550a,
	RESTART_REASON_SEC_DEBUG_MODE = 0x776655ee,
	RESTART_REASON_END = 0xffffffff,
};

#define UPLOAD_MSG_USER_FAULT			"User Fault"
#define UPLOAD_MSG_CRASH_KEY			"Crash Key"
#define UPLOAD_MSG_USER_CRASH_KEY		"User Crash Key"
#define UPLOAD_MSG_LONG_KEY_PRESS		"Long Key Press"

#ifdef CONFIG_SEC_DEBUG
DECLARE_PER_CPU(struct sec_debug_core_t, sec_debug_core_reg);
DECLARE_PER_CPU(struct sec_debug_mmu_reg_t, sec_debug_mmu_reg);
DECLARE_PER_CPU(enum sec_debug_upload_cause_t, sec_debug_upload_cause);

static inline void sec_debug_save_context(void)
{
	unsigned long flags;
	unsigned int cpu = smp_processor_id();

	local_irq_save(flags);
	sec_debug_save_mmu_reg(&per_cpu(sec_debug_mmu_reg, cpu));
	sec_debug_save_core_reg(&per_cpu(sec_debug_core_reg, cpu));
	pr_emerg("(%s) context saved(CPU:%d)\n", __func__, cpu);
	local_irq_restore(flags);
	flush_cache_all();
}

void simulate_msm_thermal_bite(void);
extern void sec_debug_set_upload_cause(enum sec_debug_upload_cause_t type);
extern void sec_debug_prepare_for_wdog_bark_reset(void);
extern int sec_debug_init(void);
extern int sec_debug_dump_stack(void);
extern void sec_debug_hw_reset(void);
extern void sec_getlog_supply_fbinfo(void *p_fb, u32 res_x, u32 res_y, u32 bpp,
		u32 frames);
extern void sec_getlog_supply_meminfo(u32 size0, u32 addr0, u32 size1,
		u32 addr1);
extern void sec_getlog_supply_loggerinfo(void *p_main, void *p_radio,
		void *p_events, void *p_system);
extern void sec_getlog_supply_kloginfo(void *klog_buf);

extern void sec_gaf_supply_rqinfo(unsigned short curr_offset,
				  unsigned short rq_offset);
extern bool sec_debug_is_enabled(void);
extern unsigned int sec_debug_level(void);
extern int sec_debug_is_modem_separate_debug_ssr(void);
extern int silent_log_panic_handler(void);
extern void sec_debug_print_model(struct seq_file *m, const char *cpu_name);

extern void sec_debug_update_dload_mode(const int restart_mode,
		const int in_panic);
extern void sec_debug_update_restart_reason(const char *cmd,
		const int in_panic);
extern void sec_debug_set_thermal_upload(void);

#ifdef CONFIG_POWER_RESET_QCOM
/* from 'msm-poweroff.c' */
extern void set_dload_mode(int on);
#endif

#ifdef CONFIG_SEC_PERIPHERAL_SECURE_CHK
extern void sec_peripheral_secure_check_fail(void);
#else
static inline void sec_peripheral_secure_check_fail(void) {}
#endif

#else /* CONFIG_SEC_DEBUG */
static inline void sec_debug_save_context(void) {}
static inline void sec_debug_set_upload_cause(
			enum sec_debug_upload_cause_t type) {}
static inline void sec_debug_prepare_for_wdog_bark_reset(void) {}
static inline int sec_debug_init(void) { return 0; }
static inline int sec_debug_dump_stack(void) { return 0; }
static inline void sec_peripheral_secure_check_fail(void) {}
static inline void sec_getlog_supply_fbinfo(void *p_fb, u32 res_x, u32 res_y,
					    u32 bpp, u32 frames) {}

static inline void sec_getlog_supply_meminfo(u32 size0, u32 addr0, u32 size1,
					     u32 addr1) {}

#define sec_getlog_supply_loggerinfo(p_main, p_radio, p_events, p_system)

static inline void sec_getlog_supply_kloginfo(void *klog_buf) {}

static inline void sec_gaf_supply_rqinfo(unsigned short curr_offset,
					unsigned short rq_offset) {}

static inline bool sec_debug_is_enabled(void) { return false; }
static inline unsigned int sec_debug_level(void) {return 0; }
static inline  int sec_debug_is_modem_separate_debug_ssr(void)
			{ return SEC_DEBUG_MODEM_SEPARATE_DIS; }
static inline void sec_debug_hw_reset(void) {}
static inline void emerg_pet_watchdog(void) {}
static inline void sec_debug_set_rr(u32 reason) {}
static inline u32 sec_debug_get_rr(void) { return 0; }
static inline void sec_debug_print_model(
		struct seq_file *m, const char *cpu_name) {}

static inline void sec_debug_update_dload_mode(const int restart_mode,
		const int in_panic) {}
static inline void sec_debug_update_restart_reason(const char *cmd,
		const int in_panic) {}
static inline void sec_debug_set_thermal_upload(void) {}
static inline void sec_peripheral_secure_check_fail(void) {}
#endif /* CONFIG_SEC_DEBUG */

#ifdef CONFIG_SEC_LOG_LAST_KMSG
extern void sec_set_reset_extra_info(char *last_kmsg_buffer,
				unsigned last_kmsg_size);
#else
static inline void sec_set_reset_extra_info(char *last_kmsg_buffer,
				unsigned last_kmsg_size) {}
#endif /* CONFIG_SEC_LOG_LAST_KMSG */

#ifdef CONFIG_SEC_FILE_LEAK_DEBUG
extern void sec_debug_EMFILE_error_proc(void);
#else
static inline void sec_debug_EMFILE_error_proc(void) {}
#endif /* CONFIG_SEC_FILE_LEAK_DEBUG */

#ifdef CONFIG_SEC_SSR_DEBUG_LEVEL_CHK
extern int sec_debug_is_enabled_for_ssr(void);
#else
static inline int sec_debug_is_enabled_for_ssr(void) { return 0; }
#endif /* CONFIG_SEC_SSR_DEBUG_LEVEL_CHK */

/* for sec debug level */
#define KERNEL_SEC_DEBUG_LEVEL_LOW	(0x574F4C44)
#define KERNEL_SEC_DEBUG_LEVEL_MID	(0x44494D44)
#define KERNEL_SEC_DEBUG_LEVEL_HIGH	(0x47494844)

#define ANDROID_DEBUG_LEVEL_LOW		0x4f4c
#define ANDROID_DEBUG_LEVEL_MID		0x494d
#define ANDROID_DEBUG_LEVEL_HIGH	0x4948

#define ANDROID_CP_DEBUG_ON		0x5500
#define ANDROID_CP_DEBUG_OFF		0x55ff

extern int ssr_panic_handler_for_sec_dbg(void);
extern void emerg_pet_watchdog(void);
#define LOCAL_CONFIG_PRINT_EXTRA_INFO

#ifdef CONFIG_SEC_DEBUG_DOUBLE_FREE
extern void *kfree_hook(void *p, void *caller);
#endif

typedef enum {
	USER_UPLOAD_CAUSE_MIN = 1,
	USER_UPLOAD_CAUSE_SMPL = USER_UPLOAD_CAUSE_MIN,	/* RESET_REASON_SMPL */
	USER_UPLOAD_CAUSE_WTSR,			/* RESET_REASON_WTSR */
	USER_UPLOAD_CAUSE_WATCHDOG,		/* RESET_REASON_WATCHDOG */
	USER_UPLOAD_CAUSE_PANIC,		/* RESET_REASON_PANIC */
	USER_UPLOAD_CAUSE_MANUAL_RESET,	/* RESET_REASON_MANUAL_RESET */
	USER_UPLOAD_CAUSE_POWER_RESET,	/* RESET_REASON_POWER_RESET */
	USER_UPLOAD_CAUSE_REBOOT,		/* RESET_REASON_REBOOT */
	USER_UPLOAD_CAUSE_BOOTLOADER_REBOOT,/* RESET_REASON_BOOTLOADER_REBOOT */
	USER_UPLOAD_CAUSE_POWER_ON,		/* RESET_REASON_POWER_ON */
	USER_UPLOAD_CAUSE_THERMAL,		/* RESET_REASON_THERMAL_RESET */
	USER_UPLOAD_CAUSE_UNKNOWN,		/* RESET_REASON_UNKNOWN */
	USER_UPLOAD_CAUSE_MAX = USER_UPLOAD_CAUSE_UNKNOWN,
} user_upload_cause_t;

#ifdef CONFIG_TOUCHSCREEN_DUMP_MODE
struct tsp_dump_callbacks {
	void (*inform_dump)(void);
};
#endif

extern int set_reduced_sdi_mode(void);

#endif	/* SEC_DEBUG_H */
