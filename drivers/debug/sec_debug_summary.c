/*
 * drivers/debug/sec_debug_summary.c
 *
 * driver supporting debug functions for Samsung device
 *
 * COPYRIGHT(C) 2006-2017 Samsung Electronics Co., Ltd. All Right Reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

#define pr_fmt(fmt)     KBUILD_MODNAME ":%s: " fmt, __func__

#include <linux/module.h>
#include <linux/errno.h>
#include <linux/ctype.h>
#include <soc/qcom/smem.h>
#include <linux/ptrace.h>
#include <asm/processor.h>
#include <asm/irq.h>
#include <asm/memory.h>

#include <linux/sec_bsp.h>
#include <linux/sec_debug.h>
#include <linux/sec_debug_user_reset.h>
#include <linux/sec_debug_summary.h>

#include "sec_debug_internal.h"

struct sec_debug_summary *secdbg_summary;
struct sec_debug_summary_data_apss *secdbg_apss;
static char build_root[] = __FILE__;

static uint32_t tzapps_start_addr;
static uint32_t tzapps_size;

char *sec_debug_arch_desc;

static unsigned long cpu_buf_vaddr;
static unsigned long cpu_buf_paddr;
static unsigned long cpu_data_vaddr;
static unsigned long cpu_data_paddr;
static unsigned long max_cpu_ctx_size;

#ifdef CONFIG_SEC_DEBUG_VERBOSE_SUMMARY_HTML
unsigned int cpu_frequency[CONFIG_NR_CPUS];
unsigned int cpu_volt[CONFIG_NR_CPUS];
char cpu_state[CONFIG_NR_CPUS][VAR_NAME_MAX];
EXPORT_SYMBOL(cpu_frequency);
EXPORT_SYMBOL(cpu_volt);
EXPORT_SYMBOL(cpu_state);
#endif

#ifdef CONFIG_ARM64
#define ARM_PT_REG_PC pc
#define ARM_PT_REG_LR regs[30]
#else
#define ARM_PT_REG_PC ARM_pc
#define ARM_PT_REG_LR ARM_lr
#endif

int sec_debug_save_die_info(const char *str, struct pt_regs *regs)
{
#ifdef CONFIG_USER_RESET_DEBUG
	_kern_ex_info_t *p_ex_info;
#endif
	if (!secdbg_apss)
		return -ENOMEM;
	snprintf(secdbg_apss->excp.pc_sym, sizeof(secdbg_apss->excp.pc_sym),
		"%pS", (void *)regs->ARM_PT_REG_PC);
	snprintf(secdbg_apss->excp.lr_sym, sizeof(secdbg_apss->excp.lr_sym),
		"%pS", (void *)regs->ARM_PT_REG_LR);

#ifdef CONFIG_USER_RESET_DEBUG
	sec_debug_store_extc_idx(false);

	if (sec_debug_reset_ex_info) {
		p_ex_info = &sec_debug_reset_ex_info->kern_ex_info.info;
		if (p_ex_info->cpu == -1) {
			int slen;
			char *msg;

			p_ex_info->cpu = smp_processor_id();
			snprintf(p_ex_info->task_name,
				sizeof(p_ex_info->task_name), "%s", current->comm);
			p_ex_info->ktime = local_clock();
			snprintf(p_ex_info->pc,
				sizeof(p_ex_info->pc), "%pS", (void *)regs->ARM_PT_REG_PC);
			snprintf(p_ex_info->lr,
				sizeof(p_ex_info->lr), "%pS", (void *)regs->ARM_PT_REG_LR);
			slen = snprintf(p_ex_info->panic_buf,
				sizeof(p_ex_info->panic_buf), "%s", str);

			msg = p_ex_info->panic_buf;

			if ((slen >= 1) && (msg[slen-1] == '\n'))
				msg[slen-1] = 0;
		}
	}
#endif

	return 0;
}

int sec_debug_save_panic_info(const char *str, unsigned long caller)
{
#ifdef CONFIG_USER_RESET_DEBUG
	_kern_ex_info_t *p_ex_info;
#endif
	if (!secdbg_apss)
		return -ENOMEM;
	snprintf(secdbg_apss->excp.panic_caller,
		sizeof(secdbg_apss->excp.panic_caller), "%pS", (void *)caller);
	snprintf(secdbg_apss->excp.panic_msg,
		sizeof(secdbg_apss->excp.panic_msg), "%s", str);
	snprintf(secdbg_apss->excp.thread,
		sizeof(secdbg_apss->excp.thread), "%s:%d", current->comm,
		task_pid_nr(current));

#ifdef CONFIG_USER_RESET_DEBUG
	sec_debug_store_extc_idx(false);

	if (sec_debug_reset_ex_info) {
		p_ex_info = &sec_debug_reset_ex_info->kern_ex_info.info;
		if (p_ex_info->cpu == -1) {
			int slen;
			char *msg;

			p_ex_info->cpu = smp_processor_id();
			snprintf(p_ex_info->task_name,
				sizeof(p_ex_info->task_name), "%s", current->comm);
			p_ex_info->ktime = local_clock();
			snprintf(p_ex_info->pc,
				sizeof(p_ex_info->pc), "%pS", (void *)(caller-0x4));
			snprintf(p_ex_info->lr,
				sizeof(p_ex_info->lr), "%pS", (void *)caller);
			slen = snprintf(p_ex_info->panic_buf,
				sizeof(p_ex_info->panic_buf), "%s", str);

			msg = p_ex_info->panic_buf;

			if ((slen >= 1) && (msg[slen-1] == '\n'))
				msg[slen-1] = 0;
		}
	}
#endif

	return 0;
}

int sec_debug_summary_add_infomon(char *name, unsigned int size, phys_addr_t pa)
{
	if (!secdbg_apss)
		return -ENOMEM;

	if (secdbg_apss->info_mon.idx >= ARRAY_SIZE(secdbg_apss->info_mon.var))
		return -ENOMEM;

	strlcpy(secdbg_apss->info_mon.var[secdbg_apss->info_mon.idx].name,
		name, sizeof(secdbg_apss->info_mon.var[0].name));
	secdbg_apss->info_mon.var[secdbg_apss->info_mon.idx].sizeof_type
		= size;
	secdbg_apss->info_mon.var[secdbg_apss->info_mon.idx].var_paddr = pa;

	secdbg_apss->info_mon.idx++;

	return 0;
}

int sec_debug_summary_add_varmon(char *name, unsigned int size, phys_addr_t pa)
{
	if (!secdbg_apss)
		return -ENOMEM;

	if (secdbg_apss->var_mon.idx >= ARRAY_SIZE(secdbg_apss->var_mon.var))
		return -ENOMEM;

	strlcpy(secdbg_apss->var_mon.var[secdbg_apss->var_mon.idx].name, name,
		sizeof(secdbg_apss->var_mon.var[0].name));
	secdbg_apss->var_mon.var[secdbg_apss->var_mon.idx].sizeof_type = size;
	secdbg_apss->var_mon.var[secdbg_apss->var_mon.idx].var_paddr = pa;

	secdbg_apss->var_mon.idx++;

	return 0;
}

#ifdef CONFIG_SEC_DEBUG_MDM_FILE_INFO
void sec_set_mdm_summary_info(char *str_buf)
{
	snprintf(secdbg_apss->mdmerr_info,
		sizeof(secdbg_apss->mdmerr_info), "%s", str_buf);
}
#endif

static int ___build_root_init(char *str)
{
	char *st, *ed;
	int len;
	ed = strstr(str, "/android/kernel");
	if (!ed || ed == str)
		return -1;
	*ed = '\0';
	st = strrchr(str, '/');
	if (!st)
		return -1;
	st++;
	len = (unsigned long)ed - (unsigned long)st + 1;
	memmove(str, st, len);
	return 0;
}

#ifdef CONFIG_SEC_DEBUG_VERBOSE_SUMMARY_HTML
void sec_debug_save_cpu_freq_voltage(int cpu, int flag, unsigned long value)
{
	if (SAVE_FREQ == flag)
		cpu_frequency[cpu] = value;
	else if (SAVE_VOLT == flag)
		cpu_volt[cpu] = (unsigned int)value;
}
#else
void sec_debug_save_cpu_freq_voltage(int cpu, int flag, unsigned long value)
{
}
#endif

void sec_debug_secure_app_addr_size(uint32_t addr, uint32_t size)
{
	tzapps_start_addr = addr;
	tzapps_size = size;
}

static int __init _set_kconst(struct sec_debug_summary_data_apss *p)
{
	p->kconst.nr_cpus = NR_CPUS;
	p->kconst.per_cpu_offset.pa = virt_to_phys(__per_cpu_offset);
	p->kconst.per_cpu_offset.size = sizeof(__per_cpu_offset[0]);
	p->kconst.per_cpu_offset.count = sizeof(__per_cpu_offset) /
					sizeof(__per_cpu_offset[0]);
	p->kconst.phys_offset = PHYS_OFFSET;
	p->kconst.page_offset = PAGE_OFFSET;
	p->kconst.va_bits = VA_BITS;
	p->kconst.kimage_vaddr = kimage_vaddr;
	p->kconst.kimage_voffset = kimage_voffset;

	return 0;
}

static int __init summary_init_infomon(void)
{
	if (___build_root_init(build_root) == 0)
		ADD_STR_TO_INFOMON(build_root);
	ADD_STR_TO_INFOMON(linux_banner);
	sec_debug_summary_add_infomon("Kernel cmdline", -1,
				      __pa(saved_command_line));
	sec_debug_summary_add_infomon("Hardware name", -1,
				      __pa(sec_debug_arch_desc));

	return 0;
}

static int __init summary_init_varmon(void)
{
	uint64_t last_pet_paddr = 0;
	uint64_t last_ns_paddr = 0;

	/* save paddrs of last_pet und last_ns */
	if (secdbg_paddr && secdbg_log) {
		last_pet_paddr = secdbg_paddr +
			offsetof(struct sec_debug_log, last_pet);
		last_ns_paddr = secdbg_paddr +
			offsetof(struct sec_debug_log, last_ns);
		sec_debug_summary_add_varmon("last_pet",
			sizeof((secdbg_log->last_pet)), last_pet_paddr);
		sec_debug_summary_add_varmon("last_ns",
				sizeof((secdbg_log->last_ns.counter)),
				last_ns_paddr);
	} else
		pr_emerg("**** secdbg_log or secdbg_paddr is not initialized ****\n");

#if defined(CONFIG_ARM) || defined(CONFIG_ARM64)
	ADD_VAR_TO_VARMON(boot_reason);
	ADD_VAR_TO_VARMON(cold_boot);
#endif

#ifdef CONFIG_SEC_DEBUG_VERBOSE_SUMMARY_HTML
	for (i = 0; i < CONFIG_NR_CPUS; i++) {
		ADD_STR_ARRAY_TO_VARMON(cpu_state[i], i, CPU_STAT_CORE);
		ADD_ARRAY_TO_VARMON(cpu_frequency[i], i, CPU_FREQ_CORE);
		ADD_ARRAY_TO_VARMON(cpu_volt[i], i, CPU_VOLT_CORE);
	}
#endif

	return 0;
}

static int __init summary_init_sched_log(struct sec_debug_summary_data_apss *p)
{
	if (!secdbg_paddr)
		return -ENOMEM;

	p->sched_log.sched_idx_paddr = secdbg_paddr +
		offsetof(struct sec_debug_log, idx_sched);
	p->sched_log.sched_buf_paddr = secdbg_paddr +
		offsetof(struct sec_debug_log, sched);
	p->sched_log.sched_struct_sz =
		sizeof(struct sched_log);
	p->sched_log.sched_array_cnt = SCHED_LOG_MAX;

	p->sched_log.irq_idx_paddr = secdbg_paddr +
		offsetof(struct sec_debug_log, idx_irq);
	p->sched_log.irq_buf_paddr = secdbg_paddr +
		offsetof(struct sec_debug_log, irq);
	p->sched_log.irq_struct_sz =
		sizeof(struct irq_log);
	p->sched_log.irq_array_cnt = SCHED_LOG_MAX;

	p->sched_log.secure_idx_paddr = secdbg_paddr +
		offsetof(struct sec_debug_log, idx_secure);
	p->sched_log.secure_buf_paddr = secdbg_paddr +
		offsetof(struct sec_debug_log, secure);
	p->sched_log.secure_struct_sz =
		sizeof(struct secure_log);
	p->sched_log.secure_array_cnt = TZ_LOG_MAX;

	p->sched_log.irq_exit_idx_paddr = secdbg_paddr +
		offsetof(struct sec_debug_log, idx_irq_exit);
	p->sched_log.irq_exit_buf_paddr = secdbg_paddr +
		offsetof(struct sec_debug_log, irq_exit);
	p->sched_log.irq_exit_struct_sz =
		sizeof(struct irq_exit_log);
	p->sched_log.irq_exit_array_cnt = SCHED_LOG_MAX;

#ifdef CONFIG_SEC_DEBUG_MSG_LOG
	p->sched_log.msglog_idx_paddr = secdbg_paddr +
		offsetof(struct sec_debug_log, idx_secmsg);
	p->sched_log.msglog_buf_paddr = secdbg_paddr +
		offsetof(struct sec_debug_log, secmsg);
	p->sched_log.msglog_struct_sz =
		sizeof(struct secmsg_log);
	p->sched_log.msglog_array_cnt = MSG_LOG_MAX;
#else
	p->sched_log.msglog_idx_paddr = 0;
	p->sched_log.msglog_buf_paddr = 0;
	p->sched_log.msglog_struct_sz = 0;
	p->sched_log.msglog_array_cnt = 0;
#endif

	return 0;
}

static unsigned long get_wdog_regsave_paddr(void)
{
	return __pa(&cpu_buf_paddr);
}
unsigned int get_last_pet_paddr(void)
{
#if 0 // MUST BE CHECK
	return virt_to_phys(&wdog_dd->last_pet);
#else
	return 0;
#endif
}

void sec_debug_summary_bark_dump(unsigned long cpu_data,
		unsigned long pcpu_data, unsigned long cpu_buf,
		unsigned long pcpu_buf, unsigned long cpu_ctx_size)
{
	cpu_data_vaddr = cpu_data;
	cpu_data_paddr = pcpu_data;
	cpu_buf_vaddr = cpu_buf;
	cpu_buf_paddr = pcpu_buf;
	max_cpu_ctx_size = cpu_ctx_size;
}

static int sec_debug_summary_set_msm_dump_info(
		struct sec_debug_summary_data_apss *apss)
{
	apss->cpu_reg.msm_dump_info.cpu_data_paddr = cpu_data_paddr;
	apss->cpu_reg.msm_dump_info.cpu_buf_paddr = cpu_buf_paddr;
	apss->cpu_reg.msm_dump_info.cpu_ctx_size = max_cpu_ctx_size;
	apss->cpu_reg.msm_dump_info.offset = 0x10;

	pr_info("cpu_data_paddr = 0x%llx\n",
			apss->cpu_reg.msm_dump_info.cpu_data_paddr);
	pr_info("cpu_buf_paddr = 0x%llx\n",
			apss->cpu_reg.msm_dump_info.cpu_buf_paddr);

	return 0;
}

static int __init summary_init_core_reg(struct sec_debug_summary_data_apss *p)
{
	/* setup sec debug core reg info */
	p->cpu_reg.sec_debug_core_reg_paddr = virt_to_phys(&sec_debug_core_reg);

	pr_info("sec_debug_core_reg_paddr = 0x%llx\n",
				p->cpu_reg.sec_debug_core_reg_paddr);

#ifdef CONFIG_QCOM_MEMORY_DUMP_V2
	/* setup qc core reg info */
	sec_debug_summary_set_msm_dump_info(p);
#endif

	return 0;
}

static int __init summary_init_avc_log(struct sec_debug_summary_data_apss *p)
{
	if (!secdbg_paddr)
		return -EINVAL;

#ifdef CONFIG_SEC_DEBUG_AVC_LOG
	p->avc_log.secavc_idx_paddr = secdbg_paddr +
		offsetof(struct sec_debug_log, idx_secavc);
	p->avc_log.secavc_buf_paddr = secdbg_paddr +
		offsetof(struct sec_debug_log, secavc);
	p->avc_log.secavc_struct_sz =
		sizeof(struct secavc_log);
	p->avc_log.secavc_array_cnt = AVC_LOG_MAX;
#else
	p->avc_log.secavc_idx_paddr = 0;
	p->avc_log.secavc_buf_paddr = 0;
	p->avc_log.secavc_struct_sz = 0;
	p->avc_log.secavc_array_cnt = 0;
#endif

	return 0;
}

int sec_debug_is_modem_separate_debug_ssr(void)
{
	return secdbg_summary->priv.modem.seperate_debug;
}

#define SET_MEMBER_TYPE_INFO(PTR, TYPE, MEMBER) \
	{ \
		(PTR)->size = sizeof(((TYPE *)0)->MEMBER); \
		(PTR)->offset = offsetof(TYPE, MEMBER); \
	}

int summary_set_task_info(struct sec_debug_summary_data_apss *apss)
{
	extern struct task_struct init_task;

	apss->task.stack_size = THREAD_SIZE;
	apss->task.start_sp = THREAD_START_SP;
	apss->task.irq_stack.pcpu_stack = (uint64_t)&irq_stack;
	apss->task.irq_stack.size = IRQ_STACK_SIZE;
	apss->task.irq_stack.start_sp = IRQ_STACK_START_SP;

	apss->task.ti.struct_size = sizeof(struct thread_info);
	SET_MEMBER_TYPE_INFO(&apss->task.ti.flags, struct thread_info, flags);
	SET_MEMBER_TYPE_INFO(&apss->task.ts.cpu, struct task_struct, cpu);
#ifdef CONFIG_RKP_CFP_ROPP
	SET_MEMBER_TYPE_INFO(&apss->task.ti.rrk, struct thread_info, rrk);
#endif

	apss->task.ts.struct_size = sizeof(struct task_struct);
	SET_MEMBER_TYPE_INFO(&apss->task.ts.state, struct task_struct, state);
	SET_MEMBER_TYPE_INFO(&apss->task.ts.exit_state, struct task_struct,
					exit_state);
	SET_MEMBER_TYPE_INFO(&apss->task.ts.stack, struct task_struct, stack);
	SET_MEMBER_TYPE_INFO(&apss->task.ts.flags, struct task_struct, flags);
	SET_MEMBER_TYPE_INFO(&apss->task.ts.on_cpu, struct task_struct, on_cpu);
	SET_MEMBER_TYPE_INFO(&apss->task.ts.pid, struct task_struct, pid);
	SET_MEMBER_TYPE_INFO(&apss->task.ts.comm, struct task_struct, comm);
	SET_MEMBER_TYPE_INFO(&apss->task.ts.tasks_next, struct task_struct,
					tasks.next);
	SET_MEMBER_TYPE_INFO(&apss->task.ts.thread_group_next,
					struct task_struct, thread_group.next);
	SET_MEMBER_TYPE_INFO(&apss->task.ts.fp, struct task_struct,
					thread.cpu_context.fp);
	SET_MEMBER_TYPE_INFO(&apss->task.ts.sp, struct task_struct,
					thread.cpu_context.sp);
	SET_MEMBER_TYPE_INFO(&apss->task.ts.pc, struct task_struct,
					thread.cpu_context.pc);

#ifdef CONFIG_SCHED_INFO
	/* sched_info */
	SET_MEMBER_TYPE_INFO(&apss->task.ts.sched_info__pcount,
					struct task_struct, sched_info.pcount);
	SET_MEMBER_TYPE_INFO(&apss->task.ts.sched_info__run_delay,
					struct task_struct,
					sched_info.run_delay);
	SET_MEMBER_TYPE_INFO(&apss->task.ts.sched_info__last_arrival,
					struct task_struct,
					sched_info.last_arrival);
	SET_MEMBER_TYPE_INFO(&apss->task.ts.sched_info__last_queued,
					struct task_struct,
					sched_info.last_queued);
#endif

	apss->task.init_task = (uint64_t)&init_task;
#ifdef CONFIG_RKP_CFP_ROPP
	apss->task.ropp.magic = 0x50504F52;
#else
	apss->task.ropp.magic = 0x0;
#endif

	return 0;
}

#ifdef CONFIG_MSM_PM
void summary_set_lpm_info_cci(uint64_t paddr)
{
	if (secdbg_apss) {
		pr_info("%s : 0x%llx\n", __func__, paddr);
		secdbg_apss->aplpm.p_cci = paddr;
	}
}
#else
void summary_set_lpm_info_cci(uint64_t phy_addr)
{
}
#endif

void * sec_debug_summary_get_modem(void)
{
	if (secdbg_summary) {
		return (void *)&secdbg_summary->priv.modem;
	} else {
		pr_info("%s : secdbg_summary is null.\n", __func__);
		return NULL;
	}
}

int __init sec_debug_summary_init(void)
{
#ifdef CONFIG_SEC_DEBUG_VERBOSE_SUMMARY_HTML
	short i;
#endif

	pr_info("SMEM_ID_VENDOR2=0x%x size=0x%lx\n",
		(unsigned int)SMEM_ID_VENDOR2,
		sizeof(struct sec_debug_summary));

	/* set summary address in smem for other subsystems to see */
	secdbg_summary = (struct sec_debug_summary *)smem_alloc(
		SMEM_ID_VENDOR2,
		sizeof(struct sec_debug_summary),
		0,
		SMEM_ANY_HOST_FLAG);

	if (secdbg_summary == NULL) {
		pr_info("smem alloc failed!\n");
		return -ENOMEM;
	}

	memset(secdbg_summary, 0, (unsigned long)sizeof(secdbg_summary));

	secdbg_summary->secure_app_start_addr = tzapps_start_addr;
	secdbg_summary->secure_app_size = tzapps_size;

	secdbg_apss = &secdbg_summary->priv.apss;

	secdbg_summary->apss = (struct sec_debug_summary_data_apss *)
	    (smem_virt_to_phys(&secdbg_summary->priv.apss)&0xFFFFFFFF);
	secdbg_summary->rpm = (struct sec_debug_summary_data *)
		(smem_virt_to_phys(&secdbg_summary->priv.rpm)&0xFFFFFFFF);
	secdbg_summary->modem = (struct sec_debug_summary_data_modem *)
		(smem_virt_to_phys(&secdbg_summary->priv.modem)&0xFFFFFFFF);
	secdbg_summary->dsps = (struct sec_debug_summary_data *)
		(smem_virt_to_phys(&secdbg_summary->priv.dsps)&0xFFFFFFFF);

	pr_info("apss(%lx) rpm(%lx) modem(%lx) dsps(%lx)\n",
		(unsigned long)secdbg_summary->apss,
		(unsigned long)secdbg_summary->rpm,
		(unsigned long)secdbg_summary->modem,
		(unsigned long)secdbg_summary->dsps);


	strlcpy(secdbg_apss->name, "APSS", sizeof(secdbg_apss->name) + 1);
	strlcpy(secdbg_apss->state, "Init", sizeof(secdbg_apss->state) + 1);
	secdbg_apss->nr_cpus = CONFIG_NR_CPUS;

	sec_debug_summary_set_kloginfo(&secdbg_apss->log.first_idx_paddr,
		&secdbg_apss->log.next_idx_paddr,
		&secdbg_apss->log.log_paddr, &secdbg_apss->log.size_paddr);

	secdbg_apss->tz_core_dump =
		(struct msm_dump_data **)get_wdog_regsave_paddr();

	summary_init_infomon();

	summary_init_varmon();

	summary_init_sched_log(secdbg_apss);

	summary_init_core_reg(secdbg_apss);

	summary_init_avc_log(secdbg_apss);

	sec_debug_summary_set_kallsyms_info(secdbg_apss);

	_set_kconst(secdbg_apss);

#ifdef CONFIG_QCOM_RTB
	sec_debug_summary_set_rtb_info(secdbg_apss);
#endif

	summary_set_task_info(secdbg_apss);

	summary_set_lpm_info_cluster(secdbg_apss);
	summary_set_lpm_info_runqueues(secdbg_apss);

	summary_set_msm_memdump_info(secdbg_apss);

	/* fill magic nubmer last to ensure data integrity when the magic
	 * numbers are written
	 */
	secdbg_summary->magic[0] = SEC_DEBUG_SUMMARY_MAGIC0;
	secdbg_summary->magic[1] = SEC_DEBUG_SUMMARY_MAGIC1;
	secdbg_summary->magic[2] = SEC_DEBUG_SUMMARY_MAGIC2;
	secdbg_summary->magic[3] = SEC_DEBUG_SUMMARY_MAGIC3;

	return 0;
}

subsys_initcall_sync(sec_debug_summary_init);
