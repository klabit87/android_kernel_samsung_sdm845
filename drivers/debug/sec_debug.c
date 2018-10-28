/*
 * drivers/debug/sec_debug.c
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

#define pr_fmt(fmt)     KBUILD_MODNAME ":%s() " fmt, __func__

#include <linux/kernel.h>
#include <linux/delay.h>
#include <linux/fdtable.h>
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/input.h>
#include <linux/input/qpnp-power-on.h>
#include <linux/kernel_stat.h>
#include <linux/module.h>
#include <linux/nmi.h>
#include <linux/notifier.h>
#include <linux/of_address.h>
#include <linux/proc_fs.h>
#include <linux/reboot.h>
#include <linux/regulator/consumer.h>
#include <linux/sched.h>
#include <linux/seq_file.h>
#include <linux/slab.h>
#include <linux/spinlock.h>
#include <linux/sysrq.h>
#include <linux/uaccess.h>
#include <linux/vmalloc.h>

#include <soc/qcom/scm.h>
#include <soc/qcom/smem.h>

#include <asm/cacheflush.h>
#include <asm/compiler.h>
#include <asm/stacktrace.h>
#include <asm/system_misc.h>

#include <linux/sec_debug.h>
#include <linux/sec_debug_summary.h>
#include <linux/sec_debug_user_reset.h>
#include <linux/sec_param.h>

// [ SEC_SELINUX_PORTING_QUALCOMM
#include <linux/proc_avc.h>
// ] SEC_SELINUX_PORTING_QUALCOMM

#define CREATE_TRACE_POINTS
#include <trace/events/sec_debug.h>

#include "sec_debug_internal.h"

#ifdef CONFIG_ARM64
static inline void outer_flush_all(void) { }
#endif

/* FIXME: to avoid pr_fmt macro */
#define __printx	printk
#define __pr_info(fmt, ...) \
	__printx(KERN_INFO fmt, ##__VA_ARGS__)
#define __pr_err(fmt, ...) \
	__printx(KERN_ERR fmt, ##__VA_ARGS__)

/* enable sec_debug feature */
static unsigned int sec_dbg_level;
static int force_upload;

static unsigned int enable = 1;
module_param_named(enable, enable, uint, 0644);

static unsigned int enable_user = 1;
module_param_named(enable_user, enable_user, uint, 0644);

static unsigned int runtime_debug_val;
module_param_named(runtime_debug_val, runtime_debug_val, uint, 0644);

#ifdef CONFIG_SEC_SSR_DEBUG_LEVEL_CHK
static unsigned int enable_cp_debug = 1;
module_param_named(enable_cp_debug, enable_cp_debug, uint, 0644);
#endif

/* This is shared with msm-power off  module. */
void __iomem *restart_reason;
static void __iomem *upload_cause;

DEFINE_PER_CPU(struct sec_debug_core_t, sec_debug_core_reg);
DEFINE_PER_CPU(struct sec_debug_mmu_reg_t, sec_debug_mmu_reg);
DEFINE_PER_CPU(enum sec_debug_upload_cause_t, sec_debug_upload_cause);

static long *g_allocated_phys_mem;
static long *g_allocated_virt_mem;

static int sec_alloc_virtual_mem(const char *val, struct kernel_param *kp)
{
	long *mem;
	char *str = (char *) val;
	size_t size = (size_t)memparse(str, &str);

	if (size) {
		mem = vmalloc(size);
		if (!mem) {
			pr_err("Failed to allocate virtual memory of size: 0x%zx bytes\n",
					size);
		} else {
			pr_info("Allocated virtual memory of size: 0x%zx bytes\n",
					size);
			*mem = (long)g_allocated_virt_mem;
			g_allocated_virt_mem = mem;

			return 0;
		}
	}

	pr_info("Invalid size: %s bytes\n", val);

	return -EAGAIN;
}
module_param_call(alloc_virtual_mem, sec_alloc_virtual_mem, NULL, NULL, 0644);

static int sec_free_virtual_mem(const char *val, struct kernel_param *kp)
{
	long *mem;
	char *str = (char *) val;
	size_t free_count = (size_t)memparse(str, &str);

	if (!free_count) {
		if (strncmp(val, "all", 4)) {
			free_count = 10;
		} else {
			pr_err("Invalid free count: %s\n", val);
			return -EAGAIN;
		}
	}

	if (free_count > 10)
		free_count = 10;

	if (!g_allocated_virt_mem) {
		pr_err("No virtual memory chunk to free.\n");
		return 0;
	}

	while (g_allocated_virt_mem && free_count--) {
		mem = (long *) *g_allocated_virt_mem;
		vfree(g_allocated_virt_mem);
		g_allocated_virt_mem = mem;
	}

	pr_info("Freed previously allocated virtual memory chunks.\n");

	if (g_allocated_virt_mem)
		pr_info("Still, some virtual memory chunks are not freed. Try again.\n");

	return 0;
}
module_param_call(free_virtual_mem, sec_free_virtual_mem, NULL, NULL, 0644);

static int sec_alloc_physical_mem(const char *val, struct kernel_param *kp)
{
	long *mem;
	char *str = (char *) val;
	size_t size = (size_t)memparse(str, &str);

	if (size) {
		mem = kmalloc(size, GFP_KERNEL);
		if (!mem) {
			pr_err("Failed to allocate physical memory of size: 0x%zx bytes\n",
			       size);
		} else {
			pr_info("Allocated physical memory of size: 0x%zx bytes\n",
				size);
			*mem = (long) g_allocated_phys_mem;
			g_allocated_phys_mem = mem;

			return 0;
		}
	}

	pr_info("Invalid size: %s bytes\n", val);

	return -EAGAIN;
}
module_param_call(alloc_physical_mem, sec_alloc_physical_mem, NULL, NULL, 0644);

static int sec_free_physical_mem(const char *val, struct kernel_param *kp)
{
	long *mem;
	char *str = (char *) val;
	size_t free_count = (size_t)memparse(str, &str);

	if (!free_count) {
		if (strncmp(val, "all", 4)) {
			free_count = 10;
		} else {
			pr_info("Invalid free count: %s\n", val);
			return -EAGAIN;
		}
	}

	if (free_count > 10)
		free_count = 10;

	if (!g_allocated_phys_mem) {
		pr_info("No physical memory chunk to free.\n");
		return 0;
	}

	while (g_allocated_phys_mem && free_count--) {
		mem = (long *) *g_allocated_phys_mem;
		kfree(g_allocated_phys_mem);
		g_allocated_phys_mem = mem;
	}

	pr_info("Freed previously allocated physical memory chunks.\n");

	if (g_allocated_phys_mem)
		pr_info("Still, some physical memory chunks are not freed. Try again.\n");

	return 0;
}
module_param_call(free_physical_mem, sec_free_physical_mem, NULL, NULL, 0644);

static int dbg_set_cpu_affinity(const char *val, struct kernel_param *kp)
{
	char *endptr;
	pid_t pid;
	int cpu;
	struct cpumask mask;
	long ret;

	pid = (pid_t)memparse(val, &endptr);
	if (*endptr != '@') {
		pr_info("invalid input strin: %s\n", val);
		return -EINVAL;
	}

	cpu = (int)memparse(++endptr, &endptr);
	cpumask_clear(&mask);
	cpumask_set_cpu(cpu, &mask);
	pr_info("Setting %d cpu affinity to cpu%d\n", pid, cpu);

	ret = sched_setaffinity(pid, &mask);
	pr_info("sched_setaffinity returned %ld\n", ret);

	return 0;
}
module_param_call(setcpuaff, dbg_set_cpu_affinity, NULL, NULL, 0644);

static void sec_debug_set_qc_dload_magic(int on)
{
	pr_info("on=%d\n", on);
	set_dload_mode(on);
}

#define PON_RESTART_REASON_NOT_HANDLE	PON_RESTART_REASON_MAX
#define RESTART_REASON_NOT_HANDLE	RESTART_REASON_END

/* This is shared with 'msm-poweroff.c'  module. */
static enum sec_restart_reason_t __iomem *qcom_restart_reason;

static void sec_debug_set_upload_magic(unsigned int magic)
{
	__pr_err("(%s) %x\n", __func__, magic);

	if (magic)
		sec_debug_set_qc_dload_magic(1);
	__raw_writel(magic, qcom_restart_reason);

	flush_cache_all();
	outer_flush_all();
}

static int sec_debug_normal_reboot_handler(struct notifier_block *nb,
		unsigned long action, void *data)
{
	char recovery_cause[256];

	set_dload_mode(0);	/* set defalut (not upload mode) */

	sec_debug_set_upload_magic(RESTART_REASON_NORMAL);

	if (unlikely(!data))
		return 0;

	if ((action == SYS_RESTART) &&
		!strncmp((char *)data, "recovery", 8)) {
		sec_get_param(param_index_reboot_recovery_cause,
			      recovery_cause);
		if (!recovery_cause[0] || !strlen(recovery_cause)) {
			snprintf(recovery_cause, sizeof(recovery_cause),
				 "%s:%d ", current->comm, task_pid_nr(current));
			sec_set_param(param_index_reboot_recovery_cause,
				      recovery_cause);
		}
	}

	return 0;
}

void sec_debug_update_dload_mode(const int restart_mode, const int in_panic)
{
#ifdef CONFIG_SEC_DEBUG_LOW_LOG
	if (sec_debug_is_enabled() &&
	    ((restart_mode == RESTART_DLOAD) || in_panic))
		set_dload_mode(1);
	else
		set_dload_mode(0);
#else
	/* FIXME: dead code? */
	/* set_dload_mod((RESTART_DLOAD == restart_mode) || in_panic); */
#endif
}

static inline void __sec_debug_set_restart_reason(
				enum sec_restart_reason_t __r)
{
	__raw_writel((u32)__r, qcom_restart_reason);
}

static enum pon_restart_reason __pon_restart_pory_start(
				unsigned long opt_code)
{
	return (PON_RESTART_REASON_RORY_START | opt_code);
}

static enum pon_restart_reason __pon_restart_set_debug_level(
				unsigned long opt_code)
{
	switch (opt_code) {
	case ANDROID_DEBUG_LEVEL_LOW:
		return PON_RESTART_REASON_DBG_LOW;
	case ANDROID_DEBUG_LEVEL_MID:
		return PON_RESTART_REASON_DBG_MID;
	case ANDROID_DEBUG_LEVEL_HIGH:
		return PON_RESTART_REASON_DBG_HIGH;
	}

	return PON_RESTART_REASON_UNKNOWN;
}

static enum pon_restart_reason __pon_restart_set_cpdebug(
				unsigned long opt_code)
{
	if (opt_code == ANDROID_CP_DEBUG_ON)
		return PON_RESTART_REASON_CP_DBG_ON;
	else if (opt_code == ANDROID_CP_DEBUG_OFF)
		return PON_RESTART_REASON_CP_DBG_OFF;

	return PON_RESTART_REASON_UNKNOWN;
}

static enum pon_restart_reason __pon_restart_force_upload(
				unsigned long opt_code)
{
	return (opt_code) ?
		PON_RESTART_REASON_FORCE_UPLOAD_ON :
		PON_RESTART_REASON_FORCE_UPLOAD_OFF;
}

#ifdef CONFIG_MUIC_SUPPORT_RUSTPROOF
static enum pon_restart_reason __pon_restart_swsel(
				unsigned long opt_code)
{
	unsigned long value =
		 (((opt_code & 0x8) >> 1) | opt_code) & 0x7;

	return (PON_RESTART_REASON_SWITCHSEL | value);
}
#endif

void sec_debug_update_restart_reason(const char *cmd, const int in_panic)
{
	struct __magic {
		const char *cmd;
		enum pon_restart_reason pon_rr;
		enum sec_restart_reason_t sec_rr;
		enum pon_restart_reason (*func)(unsigned long opt_code);
	} magic[] = {
		{ "sec_debug_hw_reset",
			PON_RESTART_REASON_NOT_HANDLE,
			RESTART_REASON_SEC_DEBUG_MODE, NULL },
		{ "download",
			PON_RESTART_REASON_DOWNLOAD,
			RESTART_REASON_NOT_HANDLE, NULL },
		{ "nvbackup",
			PON_RESTART_REASON_NVBACKUP,
			RESTART_REASON_NOT_HANDLE, NULL },
		{ "nvrestore",
			PON_RESTART_REASON_NVRESTORE,
			RESTART_REASON_NOT_HANDLE, NULL },
		{ "nverase",
			PON_RESTART_REASON_NVERASE,
			RESTART_REASON_NOT_HANDLE, NULL },
		{ "nvrecovery",
			PON_RESTART_REASON_NVRECOVERY,
			RESTART_REASON_NOT_HANDLE, NULL },
		{ "cpmem_on",
			PON_RESTART_REASON_CP_MEM_RESERVE_ON,
			RESTART_REASON_NOT_HANDLE, NULL },
		{ "cpmem_off",
			PON_RESTART_REASON_CP_MEM_RESERVE_OFF,
			RESTART_REASON_NOT_HANDLE, NULL },
		{ "mbsmem_on",
			PON_RESTART_REASON_MBS_MEM_RESERVE_ON,
			RESTART_REASON_NOT_HANDLE, NULL },
		{ "mbsmem_off",
			PON_RESTART_REASON_MBS_MEM_RESERVE_OFF,
			RESTART_REASON_NOT_HANDLE, NULL },
		{ "GlobalActions restart",
			PON_RESTART_REASON_NORMALBOOT,
			RESTART_REASON_NOT_HANDLE, NULL },
		{ "userrequested",
			PON_RESTART_REASON_NORMALBOOT,
			RESTART_REASON_NOT_HANDLE, NULL },
		{ "oem-",
			PON_RESTART_REASON_UNKNOWN,
			RESTART_REASON_NORMAL, NULL },
		{ "sud",
			PON_RESTART_REASON_UNKNOWN,
			RESTART_REASON_NORMAL, __pon_restart_pory_start },
		{ "debug",
			PON_RESTART_REASON_UNKNOWN,
			RESTART_REASON_NORMAL, __pon_restart_set_debug_level },
		{ "cpdebug",
			PON_RESTART_REASON_UNKNOWN,
			RESTART_REASON_NORMAL, __pon_restart_set_cpdebug },
		{ "forceupload",
			PON_RESTART_REASON_UNKNOWN,
			RESTART_REASON_NORMAL, __pon_restart_force_upload },
#ifdef CONFIG_MUIC_SUPPORT_RUSTPROOF
		{ "swsel",
			PON_RESTART_REASON_UNKNOWN,
			RESTART_REASON_NORMAL, __pon_restart_swsel },
#endif
	};
	enum pon_restart_reason pon_rr = (!in_panic) ?
				PON_RESTART_REASON_NORMALBOOT :
				PON_RESTART_REASON_KERNEL_PANIC;
	enum sec_restart_reason_t sec_rr = RESTART_REASON_NORMAL;
	char cmd_buf[16];
	size_t i;

	if (!cmd || !strlen(cmd))
		goto __done;

	for (i = 0; i < ARRAY_SIZE(magic); i++) {
		size_t len = strlen(magic[i].cmd);

		if (strncmp(cmd, magic[i].cmd, len))
			continue;

		pon_rr = magic[i].pon_rr;
		sec_rr = magic[i].sec_rr;

		if (magic[i].func != NULL) {
			unsigned long opt_code;

			if (!kstrtoul(cmd + len, 0, &opt_code))
				pon_rr = magic[i].func(opt_code);
		}

		goto __done;
	}


	strlcpy(cmd_buf, cmd, ARRAY_SIZE(cmd_buf));
	__pr_err("(%s) unknown reboot command : %s\n",
	       __func__, cmd_buf);

	__sec_debug_set_restart_reason(RESTART_REASON_REBOOT);

	return;

__done:
	if (pon_rr != PON_RESTART_REASON_NOT_HANDLE)
		qpnp_pon_set_restart_reason(pon_rr);
	if (sec_rr != RESTART_REASON_NOT_HANDLE)
		__sec_debug_set_restart_reason(sec_rr);
	__pr_err("(%s) restart_reason = 0x%x(0x%x)\n",
	       __func__, sec_rr, pon_rr);
}

void sec_debug_set_upload_cause(enum sec_debug_upload_cause_t type)
{
	if (unlikely(!upload_cause)) {
		pr_err("upload cause address unmapped.\n");
	} else {
		per_cpu(sec_debug_upload_cause, smp_processor_id()) = type;
		__raw_writel(type, upload_cause);

		pr_emerg("%x\n", type);
	}
}
EXPORT_SYMBOL(sec_debug_set_upload_cause);

static inline void sec_debug_pm_restart(const char *cmd)
{
	__pr_err("(%s) %s %s\n", __func__,
		init_uts_ns.name.release, init_uts_ns.name.version);
	__pr_err("(%s) rebooting...\n", __func__);
	flush_cache_all();
	outer_flush_all();

	arm_pm_restart(REBOOT_COLD, cmd);

	/* while (1) ; */
	asm volatile ("b .");
}

void sec_debug_hw_reset(void)
{
	sec_debug_pm_restart("sec_debug_hw_reset");
}
EXPORT_SYMBOL(sec_debug_hw_reset);

#ifdef CONFIG_SEC_PERIPHERAL_SECURE_CHK
void sec_peripheral_secure_check_fail(void)
{
	if (!sec_debug_is_enabled()) {
		sec_debug_set_qc_dload_magic(0);
		sec_debug_pm_restart("peripheral_hw_reset");
		/* never reach here */
	}

	panic("subsys - modem secure check fail");
}
EXPORT_SYMBOL(sec_peripheral_secure_check_fail);
#endif

void sec_debug_set_thermal_upload(void)
{
	pr_emerg("set thermal upload cause\n");
	sec_debug_set_upload_magic(0x776655ee);
	sec_debug_set_upload_cause(UPLOAD_CAUSE_POWER_THERMAL_RESET);
}
EXPORT_SYMBOL(sec_debug_set_thermal_upload);

void sec_debug_print_model(struct seq_file *m, const char *cpu_name)
{
	u32 cpuid = read_cpuid_id();

	seq_printf(m, "model name\t: %s rev %d (%s)\n",
		   cpu_name, cpuid & 15, ELF_PLATFORM);
}

#ifdef CONFIG_USER_RESET_DEBUG
static inline void sec_debug_store_backtrace(void)
{
	unsigned long flags;

	local_irq_save(flags);
	sec_debug_backtrace();
	local_irq_restore(flags);
}
#endif /* CONFIG_USER_RESET_DEBUG */

enum sec_debug_strncmp_func {
	SEC_STRNCMP = 0,
	SEC_STRNSTR,
	SEC_STRNCASECMP,
};

static inline bool __sec_debug_strncmp(const char *s1, const char *s2,
		size_t len, enum sec_debug_strncmp_func func)
{
	switch (func) {
	case SEC_STRNCMP:
		return !strncmp(s1, s2, len);
	case SEC_STRNSTR:
		return !!strnstr(s1, s2, len);
	case SEC_STRNCASECMP:
		return !strncasecmp(s1, s2, len);
	}

	pr_warn("%d is not a valid strncmp function!\n", (int)func);

	return false;
}

static int sec_debug_panic_handler(struct notifier_block *nb,
		unsigned long l, void *buf)
{
#define MAX_STR_LEN 80
	size_t len, i;
	int timeout = 100; /* means timeout * 100ms */
	struct __upload_cause {
		const char *msg;
		enum sec_debug_upload_cause_t type;
		enum sec_debug_strncmp_func func;
	} upload_cause[] = {
		{ UPLOAD_MSG_USER_FAULT, UPLOAD_CAUSE_USER_FAULT, SEC_STRNCMP },
		{ UPLOAD_MSG_CRASH_KEY, UPLOAD_CAUSE_FORCED_UPLOAD,
			SEC_STRNCMP },
		{ UPLOAD_MSG_USER_CRASH_KEY, UPLOAD_CAUSE_USER_FORCED_UPLOAD,
			SEC_STRNCMP },
		{ UPLOAD_MSG_LONG_KEY_PRESS, UPLOAD_CAUSE_POWER_LONG_PRESS,
			SEC_STRNCMP },
		{ "CP Crash", UPLOAD_CAUSE_CP_ERROR_FATAL, SEC_STRNCMP },
		{ "MDM Crash", UPLOAD_CAUSE_MDM_ERROR_FATAL, SEC_STRNCMP },
		{ "external_modem", UPLOAD_CAUSE_MDM_ERROR_FATAL, SEC_STRNSTR },
		{ "esoc0 crashed", UPLOAD_CAUSE_MDM_ERROR_FATAL, SEC_STRNSTR },
		{ "modem", UPLOAD_CAUSE_MODEM_RST_ERR, SEC_STRNSTR },
		{ "riva", UPLOAD_CAUSE_RIVA_RST_ERR, SEC_STRNSTR },
		{ "lpass", UPLOAD_CAUSE_LPASS_RST_ERR, SEC_STRNSTR },
		{ "dsps", UPLOAD_CAUSE_DSPS_RST_ERR, SEC_STRNSTR },
		{ "subsys", UPLOAD_CAUSE_PERIPHERAL_ERR, SEC_STRNCASECMP },
#if defined(CONFIG_SEC_NAD)
		{ "crypto_test", UPLOAD_CAUSE_NAD_CRYPTO, SEC_STRNCMP },
		{ "icache_test", UPLOAD_CAUSE_NAD_ICACHE, SEC_STRNCMP },
		{ "cachecoherency_test", UPLOAD_CAUSE_NAD_CACHECOHERENCY,
			SEC_STRNCMP },
		{ "suspend_test", UPLOAD_CAUSE_NAD_SUSPEND, SEC_STRNCMP },
		{ "vddmin_test", UPLOAD_CAUSE_NAD_VDDMIN, SEC_STRNCMP },
		{ "qmesa_ddr_test", UPLOAD_CAUSE_NAD_QMESADDR, SEC_STRNCMP },
		{ "qmesa_ddr_test_cache_centric", UPLOAD_CAUSE_NAD_QMESACACHE,
			SEC_STRNCMP },
		{ "pmic_test", UPLOAD_CAUSE_NAD_PMIC, SEC_STRNCMP },
		{ "ufs_test", UPLOAD_CAUSE_NAD_UFS, SEC_STRNCMP },
		{ "sdcard_test", UPLOAD_CAUSE_NAD_SDCARD, SEC_STRNCMP },
		{ "sensor_test", UPLOAD_CAUSE_NAD_SENSOR, SEC_STRNCMP },
		{ "qdaf_fail", UPLOAD_CAUSE_NAD_QDAF_FAIL, SEC_STRNCMP },
		{ "nad_fail", UPLOAD_CAUSE_NAD_FAIL, SEC_STRNCMP },
#endif
	};

	emerg_pet_watchdog();	/* CTC-should be modify */
#ifdef CONFIG_USER_RESET_DEBUG
	sec_debug_store_backtrace();
#endif
	sec_debug_set_upload_magic(RESTART_REASON_SEC_DEBUG_MODE);

	__pr_err("%s :%s\n", __func__, (char *)buf);

	for (i = 0; i < ARRAY_SIZE(upload_cause); i++) {
		len = strnlen(buf, MAX_STR_LEN);
		if (__sec_debug_strncmp(buf, upload_cause[i].msg, len,
					upload_cause[i].func)) {
			sec_debug_set_upload_cause(upload_cause[i].type);
			break;
		}
	}

	if (i == ARRAY_SIZE(upload_cause))
		sec_debug_set_upload_cause(UPLOAD_CAUSE_KERNEL_PANIC);

	if (!sec_debug_is_enabled()) {
#ifdef CONFIG_SEC_DEBUG_LOW_LOG
		sec_debug_hw_reset();
#endif
		/* SEC will get reset_summary.html in debug low.
		 * reset_summary.html need more information about abnormal reset
		 * or kernel panic.
		 * So we skip as below
		 */
		/* return -EPERM; */
	}

	/* enable after SSR feature */
	/* ssr_panic_handler_for_sec_dbg(); */

	/* wait for all cpus to be deactivated */
	while (num_active_cpus() > 1 && timeout--) {
		touch_nmi_watchdog();
		mdelay(100);
	}

	/* save context here so that function call after this point doesn't
	 * corrupt stacks below the saved sp
	 */
	sec_debug_save_context();
	sec_debug_hw_reset();

	return 0;
}

void sec_debug_prepare_for_wdog_bark_reset(void)
{
	sec_debug_set_upload_magic(RESTART_REASON_SEC_DEBUG_MODE);
	sec_debug_set_upload_cause(UPLOAD_CAUSE_NON_SECURE_WDOG_BARK);
}

static struct notifier_block nb_reboot_block = {
	.notifier_call = sec_debug_normal_reboot_handler,
};

static struct notifier_block nb_panic_block = {
	.notifier_call = sec_debug_panic_handler,
	.priority = -1,
};

#ifdef CONFIG_OF
static int __init __sec_debug_dt_addr_init(void)
{
	struct device_node *np;

	/* Using bottom of sec_dbg DDR address range
	 * for writing restart reason
	 */
	np = of_find_compatible_node(NULL, NULL,
			"qcom,msm-imem-restart_reason");
	if (unlikely(!np)) {
		pr_err("unable to find DT imem restart reason node\n");
		return -ENODEV;
	}

	qcom_restart_reason = of_iomap(np, 0);
	if (unlikely(!qcom_restart_reason)) {
		pr_err("unable to map imem restart reason offset\n");
		return -ENODEV;
	}

	/* check restart_reason address here */
	pr_emerg("restart_reason addr : 0x%p(0x%llx)\n",
			qcom_restart_reason,
			(uint64_t)virt_to_phys(qcom_restart_reason));

	/* Using bottom of sec_dbg DDR address range for writing upload_cause */
	np = of_find_compatible_node(NULL, NULL, "qcom,msm-imem-upload_cause");
	if (unlikely(!np)) {
		pr_err("unable to find DT imem upload cause node\n");
		return -ENODEV;
	}

	upload_cause = of_iomap(np, 0);
	if (unlikely(!upload_cause)) {
		pr_err("unable to map imem upload_cause offset\n");
		return -ENODEV;
	}

	/* check upload_cause here */
	pr_emerg("upload_cause addr : 0x%p(0x%llx)\n", upload_cause,
			(uint64_t)virt_to_phys(upload_cause));

	return 0;
}
#else /* CONFIG_OF */
static int __init __sec_debug_dt_addr_init(void) { return 0; }
#endif /* CONFIG_OF */

#define SCM_WDOG_DEBUG_BOOT_PART 0x9

void sec_do_bypass_sdi_execution_in_low(void)
{
	int ret;
#if 1
	struct scm_desc desc = {
		.args[0] = 1,
		.args[1] = 0,
		.arginfo = SCM_ARGS(2),
	};

	/* Needed to bypass debug image on some chips */
	if (!is_scm_armv8())
		ret = scm_call_atomic2(SCM_SVC_BOOT,
			       SCM_WDOG_DEBUG_BOOT_PART, 1, 0);
	else
		ret = scm_call2_atomic(SCM_SIP_FNID(SCM_SVC_BOOT,
			  SCM_WDOG_DEBUG_BOOT_PART), &desc);
	if (ret)
		pr_err("Failed to disable wdog debug: %d\n", ret);
#else
	ret = set_reduced_sdi_mode();
	if (ret)
		pr_err("Failed to sdi setting: %d\n", ret);
#endif
}

static char ap_serial_from_cmdline[20];

static int __init ap_serial_setup(char *str)
{
	snprintf(ap_serial_from_cmdline, sizeof(ap_serial_from_cmdline), "%s", &str[2]);
	return 1;
}
__setup("androidboot.ap_serial=", ap_serial_setup);

static int __init force_upload_setup(char *en)
{
	get_option(&en, &force_upload);
	return 1;
}
__setup("androidboot.force_upload=", force_upload_setup);

/* for sec debug level */
static int __init sec_debug_level_setup(char *str)
{
	get_option(&str, &sec_dbg_level);
	return 1;
}
__setup("androidboot.debug_level=", sec_debug_level_setup);

extern struct kset *devices_kset;

struct bus_type chip_id_subsys = {
	.name = "chip-id",
	.dev_name = "chip-id",
};

static ssize_t ap_serial_show(struct kobject *kobj,
		struct kobj_attribute *attr, char *buf)
{
	pr_info("%s: ap_serial:[%s]\n", __func__, ap_serial_from_cmdline);
	return snprintf(buf, sizeof(ap_serial_from_cmdline), "%s\n", ap_serial_from_cmdline);
}

static struct kobj_attribute sysfs_SVC_AP_attr =
__ATTR(SVC_AP, 0444, ap_serial_show, NULL);

static struct kobj_attribute chipid_unique_id_attr =
__ATTR(unique_id, 0444, ap_serial_show, NULL);

static struct attribute *chip_id_attrs[] = {
	&chipid_unique_id_attr.attr,
	NULL,
};

static struct attribute_group chip_id_attr_group = {
	.attrs = chip_id_attrs,
};

static const struct attribute_group *chip_id_attr_groups[] = {
	&chip_id_attr_group,
	NULL,
};

void create_ap_serial_node(void)
{
	int ret;
	struct kernfs_node *svc_sd;
	struct kobject *svc;
	struct kobject *AP;

	/* create /sys/devices/system/chip-id/unique_id */
	if (subsys_system_register(&chip_id_subsys, chip_id_attr_groups))
		pr_err("%s:Failed to register subsystem-%s\n", __func__, chip_id_subsys.name);

	/* To find /sys/devices/svc/ */
	svc_sd = sysfs_get_dirent(devices_kset->kobj.sd, "svc");
	if (IS_ERR_OR_NULL(svc_sd)) {
		svc = kobject_create_and_add("svc", &devices_kset->kobj);
		if (IS_ERR_OR_NULL(svc)) {
			pr_err("%s:Failed to create sys/devices/svc\n", __func__);
			goto skip_ap_serial;
		}
	} else {
		svc = (struct kobject *)svc_sd->priv;
	}
	/* create /sys/devices/SVC/AP */
	AP = kobject_create_and_add("AP", svc);
	if (IS_ERR_OR_NULL(AP)) {
		pr_err("%s:Failed to create sys/devices/svc/AP\n", __func__);
		goto skip_ap_serial;
	} else {
		/* create /sys/devices/svc/AP/SVC_AP */
		ret = sysfs_create_file(AP, &sysfs_SVC_AP_attr.attr);
		if (ret < 0) {
			pr_err("%s:sysfs create fail-%s\n", __func__, sysfs_SVC_AP_attr.attr.name);
			goto skip_ap_serial;
		}
	}

	pr_info("%s: Completed\n",__func__);

skip_ap_serial:
	return;
}

int __init sec_debug_init(void)
{
	int ret;
// [ SEC_SELINUX_PORTING_QUALCOMM
#ifdef CONFIG_PROC_AVC
	sec_avc_log_init();
#endif
// ] SEC_SELINUX_PORTING_QUALCOMM

	ret = __sec_debug_dt_addr_init();
	if (unlikely(ret < 0))
		return ret;

	register_reboot_notifier(&nb_reboot_block);
	atomic_notifier_chain_register(&panic_notifier_list, &nb_panic_block);

	sec_debug_set_upload_magic(RESTART_REASON_SEC_DEBUG_MODE);
	sec_debug_set_upload_cause(UPLOAD_CAUSE_INIT);

	create_ap_serial_node();

	/* TODO: below code caused reboot fail when debug level LOW */
	switch (sec_dbg_level) {
	case ANDROID_DEBUG_LEVEL_LOW:
#ifdef CONFIG_SEC_FACTORY
	case ANDROID_DEBUG_LEVEL_MID:
#endif
		if (!force_upload)
			sec_do_bypass_sdi_execution_in_low();
		break;
	}

	return 0;
}
arch_initcall_sync(sec_debug_init);

bool sec_debug_is_enabled(void)
{
	switch (sec_dbg_level) {
	case ANDROID_DEBUG_LEVEL_LOW:
#ifdef CONFIG_SEC_FACTORY
	case ANDROID_DEBUG_LEVEL_MID:
#endif
		return !!(force_upload);
	}

	return !!(enable);
}
EXPORT_SYMBOL(sec_debug_is_enabled);

unsigned int sec_debug_level(void)
{
	return sec_dbg_level;
}
EXPORT_SYMBOL(sec_debug_level);

#ifdef CONFIG_SEC_SSR_DEBUG_LEVEL_CHK
int sec_debug_is_enabled_for_ssr(void)
{
	return enable_cp_debug;
}
#endif

#ifdef CONFIG_SEC_FILE_LEAK_DEBUG
int sec_debug_print_file_list(void)
{
	size_t i;
	unsigned int nCnt;
	struct file *file;
	struct files_struct *files = current->files;
	const char *pRootName;
	const char *pFileName;
	int ret = 0;

	nCnt = files->fdt->max_fds;

	pr_err(" [Opened file list of process %s(PID:%d, TGID:%d) :: %d]\n",
		current->group_leader->comm, current->pid, current->tgid, nCnt);

	for (i = 0; i < nCnt; i++) {
		rcu_read_lock();
		file = fcheck_files(files, i);

		pRootName = NULL;
		pFileName = NULL;

		if (file) {
			if (file->f_path.mnt &&
			    file->f_path.mnt->mnt_root &&
			    file->f_path.mnt->mnt_root->d_name.name)
				pRootName =
					file->f_path.mnt->mnt_root->d_name.name;

			if (file->f_path.dentry &&
			    file->f_path.dentry->d_name.name)
				pFileName = file->f_path.dentry->d_name.name;

			pr_err("[%04zd]%s%s\n", i,
					pRootName ? pRootName : "null",
					pFileName ? pFileName : "null");
			ret++;
		}
		rcu_read_unlock();
	}

	return (ret == nCnt) ? 1 : 0;
}

void sec_debug_EMFILE_error_proc(void)
{
	pr_err("Too many open files(%d:%s)\n",
		current->tgid, current->group_leader->comm);

	if (!sec_debug_is_enabled())
		return;

	/* We check EMFILE error in only "system_server",
	 * "mediaserver" and "surfaceflinger" process.
	 */
	if (!strcmp(current->group_leader->comm, "system_server") ||
	    !strcmp(current->group_leader->comm, "mediaserver") ||
	    !strcmp(current->group_leader->comm, "surfaceflinger")) {
		if (sec_debug_print_file_list() == 1)
			panic("Too many open files");
	}
}
#endif /* CONFIG_SEC_FILE_LEAK_DEBUG */

static void sec_user_fault_dump(void)
{
	if (sec_debug_is_enabled() && enable_user)
		panic(UPLOAD_MSG_USER_FAULT);
}

static ssize_t sec_user_fault_write(struct file *file,
		const char __user *buffer, size_t count, loff_t *offs)
{
	char buf[100];

	if (count > sizeof(buf) - 1)
		return -EINVAL;

	if (copy_from_user(buf, buffer, count))
		return -EFAULT;

	buf[count] = '\0';
	if (!strncmp(buf, "dump_user_fault", 15))
		sec_user_fault_dump();

	return count;
}

static const struct file_operations sec_user_fault_proc_fops = {
	.write = sec_user_fault_write,
};

static int __init sec_debug_user_fault_init(void)
{
	struct proc_dir_entry *entry;

	entry = proc_create("user_fault", 0220, NULL,
			&sec_user_fault_proc_fops);
	if (!entry)
		return -ENOMEM;

	return 0;
}
device_initcall(sec_debug_user_fault_init);

static inline void __dump_one_task_info(struct task_struct *tsk,
		bool is_process)
{
	char stat_array[] = { 'R', 'S', 'D' };
	char stat_ch;

	stat_ch = tsk->state <= TASK_UNINTERRUPTIBLE ?
		stat_array[tsk->state] : '?';
	__pr_info("%8d  %8llu  %8llu  %16llu  %c (%ld)  %p  %c %s\n",
		tsk->pid, (unsigned long long)tsk->utime,
		(unsigned long long)tsk->stime,
		tsk->se.exec_start, stat_ch, tsk->state,
		tsk, is_process ? '*' : ' ', tsk->comm);

	if (tsk->state == TASK_RUNNING || tsk->state == TASK_UNINTERRUPTIBLE)
		show_stack(tsk, NULL);
}

#define __HLINE_LEFT	" -------------------------------------------"
#define __HLINE_RIGHT	"--------------------------------------------\n"

void dump_all_task_info(void)
{
	struct task_struct *p;
	struct task_struct *t;

	__pr_info("\n");
	__pr_info(" current proc : %d %s\n",
			current->pid, current->comm);
	__pr_info(__HLINE_LEFT __HLINE_RIGHT);
	__pr_info("%8s  %8s  %8s  %16s  %5s  %s\n",
			"pid", "uTime", "sTime", "exec(ns)",
			"stat", "task_struct");
	__pr_info(__HLINE_LEFT __HLINE_RIGHT);

	for_each_process_thread (p, t) {
		__dump_one_task_info(t, p == t);
	}

	__pr_info(__HLINE_LEFT __HLINE_RIGHT);
}

/* TODO: this is a modified version of 'show_stat' in 'fs/proc/stat.c'
 * this function should be adapted for each kernel version
 */
#ifndef arch_irq_stat_cpu
#define arch_irq_stat_cpu(cpu) 0
#endif
#ifndef arch_irq_stat
#define arch_irq_stat() 0
#endif

void dump_cpu_stat(void)
{
	int i, j;
	u64 user, nice, system, idle, iowait, irq, softirq, steal;
	u64 guest, guest_nice;
	u64 sum = 0;
	u64 sum_softirq = 0;
	unsigned int per_softirq_sums[NR_SOFTIRQS] = {0};
	struct timespec64 boottime;

	user = nice = system = idle = iowait =
		irq = softirq = steal = 0;
	guest = guest_nice = 0;
	getboottime64(&boottime);

	for_each_possible_cpu(i) {
		user += kcpustat_cpu(i).cpustat[CPUTIME_USER];
		nice += kcpustat_cpu(i).cpustat[CPUTIME_NICE];
		system += kcpustat_cpu(i).cpustat[CPUTIME_SYSTEM];
		idle += kcpustat_cpu(i).cpustat[CPUTIME_IDLE];
		iowait += kcpustat_cpu(i).cpustat[CPUTIME_IOWAIT];
		irq += kcpustat_cpu(i).cpustat[CPUTIME_IRQ];
		softirq += kcpustat_cpu(i).cpustat[CPUTIME_SOFTIRQ];
		steal += kcpustat_cpu(i).cpustat[CPUTIME_STEAL];
		guest += kcpustat_cpu(i).cpustat[CPUTIME_GUEST];
		guest_nice += kcpustat_cpu(i).cpustat[CPUTIME_GUEST_NICE];
		sum += kstat_cpu_irqs_sum(i);
		sum += arch_irq_stat_cpu(i);

		for (j = 0; j < NR_SOFTIRQS; j++) {
			unsigned int softirq_stat = kstat_softirqs_cpu(j, i);

			per_softirq_sums[j] += softirq_stat;
			sum_softirq += softirq_stat;
		}
	}
	sum += arch_irq_stat();

	__pr_info("\n");
	__pr_info(__HLINE_LEFT __HLINE_RIGHT);
	__pr_info("      %8s %8s %8s %8s %8s %8s %8s %8s %8s %8s\n",
			"user", "nice", "system", "idle", "iowait", "irq",
			"softirq", "steal", "guest", "guest_nice");
	__pr_info("cpu   %8llu %8llu %8llu %8llu %8llu %8llu %8llu %8llu %8llu %8llu\n",
			cputime64_to_clock_t(user),
			cputime64_to_clock_t(nice),
			cputime64_to_clock_t(system),
			cputime64_to_clock_t(idle),
			cputime64_to_clock_t(iowait),
			cputime64_to_clock_t(irq),
			cputime64_to_clock_t(softirq),
			cputime64_to_clock_t(steal),
			cputime64_to_clock_t(guest),
			cputime64_to_clock_t(guest_nice));

	for_each_online_cpu(i) {
		/* Copy values here to work around gcc-2.95.3, gcc-2.96 */
		user = kcpustat_cpu(i).cpustat[CPUTIME_USER];
		nice = kcpustat_cpu(i).cpustat[CPUTIME_NICE];
		system = kcpustat_cpu(i).cpustat[CPUTIME_SYSTEM];
		idle = kcpustat_cpu(i).cpustat[CPUTIME_IDLE];
		iowait = kcpustat_cpu(i).cpustat[CPUTIME_IOWAIT];
		irq = kcpustat_cpu(i).cpustat[CPUTIME_IRQ];
		softirq = kcpustat_cpu(i).cpustat[CPUTIME_SOFTIRQ];
		steal = kcpustat_cpu(i).cpustat[CPUTIME_STEAL];
		guest = kcpustat_cpu(i).cpustat[CPUTIME_GUEST];
		guest_nice = kcpustat_cpu(i).cpustat[CPUTIME_GUEST_NICE];
		__pr_info("cpu%-2d %8llu %8llu %8llu %8llu %8llu %8llu %8llu %8llu %8llu %8llu\n",
				i,
				cputime64_to_clock_t(user),
				cputime64_to_clock_t(nice),
				cputime64_to_clock_t(system),
				cputime64_to_clock_t(idle),
				cputime64_to_clock_t(iowait),
				cputime64_to_clock_t(irq),
				cputime64_to_clock_t(softirq),
				cputime64_to_clock_t(steal),
				cputime64_to_clock_t(guest),
				cputime64_to_clock_t(guest_nice));
	}

	__pr_info(__HLINE_LEFT __HLINE_RIGHT);
	__pr_info("intr %llu\n", (unsigned long long)sum);

	/* sum again ? it could be updated? */
	for_each_irq_nr(j)
		if (kstat_irqs(j))
			__pr_info(" irq-%d : %u\n", j, kstat_irqs(j));

	__pr_info(__HLINE_LEFT __HLINE_RIGHT);
	__pr_info("\nctxt %llu\n"
		"btime %llu\n"
		"processes %lu\n"
		"procs_running %lu\n"
		"procs_blocked %lu\n",
		nr_context_switches(),
		(unsigned long long)boottime.tv_sec,
		total_forks,
		nr_running(),
		nr_iowait());

	__pr_info(__HLINE_LEFT __HLINE_RIGHT);
	__pr_info("softirq %llu\n", (unsigned long long)sum_softirq);

	for (i = 0; i < NR_SOFTIRQS; i++)
		__pr_info(" softirq-%d : %u\n", i, per_softirq_sums[i]);
	__pr_info("\n");

	__pr_info(__HLINE_LEFT __HLINE_RIGHT);
}
