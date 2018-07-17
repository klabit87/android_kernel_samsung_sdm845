#ifndef _RKP_H
#define _RKP_H
#include <asm/memory.h>
#include <asm/page.h>


#ifndef __ASSEMBLY__
typedef unsigned long long u64;
typedef unsigned int       u32;
typedef unsigned short     u16;
typedef unsigned char      u8;
typedef signed long long   s64;
typedef signed int         s32;
typedef signed short       s16;
typedef signed char        s8;


/* uH_RKP Command ID */
enum {
	RKP_START = 1,
	RKP_DEFERRED_START,
	RKP_WRITE_PGT1,
	RKP_WRITE_PGT2,
	RKP_WRITE_PGT3,
	RKP_EMULT_TTBR0,
	RKP_EMULT_TTBR1,
	RKP_EMULT_DORESUME,
	RKP_FREE_PGD,
	RKP_NEW_PGD,
	RKP_KASLR_MEM,
	RKP_FIMC_VERIFY, /* CFP cmds */
	RKP_JOPP_INIT,
	RKP_ROPP_INIT,
	RKP_ROPP_SAVE,
	RKP_ROPP_RELOAD,
	/* and KDT cmds */
	RKP_NOSHIP,
	RKP_MAX
};

#define RKP_PREFIX  UL(0xc300c000)
#define RKP_CMDID(CMD_ID)  ((UL(CMD_ID) & UL(0xff)) | RKP_PREFIX)

#define RKP_EMULT_TTBR0	RKP_CMDID(0x10)
#define RKP_EMULT_SCTLR	RKP_CMDID(0x15)
#define RKP_PGD_SET	RKP_CMDID(0x21)
#define RKP_PMD_SET	RKP_CMDID(0x22)
#define RKP_PTE_SET	RKP_CMDID(0x23)
#define RKP_PGD_FREE	RKP_CMDID(0x24)
#define RKP_PGD_NEW	RKP_CMDID(0x25)
#define RKP_INIT	RKP_CMDID(0x0)
#define RKP_DEF_INIT	RKP_CMDID(0x1)


#define CFP_ROPP_INIT		RKP_CMDID(0x90)
#define CFP_ROPP_SAVE		RKP_CMDID(0x91)
#define CFP_ROPP_RELOAD		RKP_CMDID(0x92)
#define CFP_JOPP_INIT		RKP_CMDID(0x98)

#define RKP_INIT_MAGIC 0x5afe0001
#define RKP_RO_BUFFER  UL(0x800000)

#define CRED_JAR_RO "cred_jar_ro"
#define TSEC_JAR	"tsec_jar"
#define VFSMNT_JAR	"vfsmnt_cache"

#define   TIMA_DEBUG_LOG_START  UL(0xA0600000)
#define   TIMA_DEBUG_LOG_SIZE   0x40000

#define   TIMA_SEC_LOG          UL(0x9FA00000)
#define   TIMA_SEC_LOG_SIZE     0x6000

extern u8 rkp_pgt_bitmap[];
extern u8 rkp_map_bitmap[];

//typedef struct rkp_init rkp_init_t;
extern u8 rkp_started;
extern u8 rkp_def_init_done;
extern void *rkp_ro_alloc(void);
extern void rkp_ro_free(void *free_addr);
extern unsigned int is_rkp_ro_page(u64 addr);

extern unsigned long max_pfn;

struct rkp_init { //copy from uh (app/rkp/rkp.h)
	u32 magic;
	u64 vmalloc_start;
	u64 vmalloc_end;
	u64 init_mm_pgd;
	u64 id_map_pgd;
	u64 zero_pg_addr;
	u64 rkp_pgt_bitmap;
	u64 rkp_dbl_bitmap;
	u32 rkp_bitmap_size;
	u32 no_fimc_verify;
	u64 _text;
	u64 _etext;
	u64 extra_memory_addr;
	u32 extra_memory_size;
	u64 physmap_addr; //not used. what is this for?
	u64 _srodata;
	u64 _erodata;
	u32 large_memory;
};

#ifdef CONFIG_RKP_KDP
typedef struct __attribute__((__packed__)) kdp_init_struct {
	u32 credSize;
	u32 sp_size;
	u32 pgd_mm;
	u32 uid_cred;
	u32 euid_cred;
	u32 gid_cred;
	u32 egid_cred;
	u32 bp_pgd_cred;
	u32 bp_task_cred;
	u32 type_cred;
	u32 security_cred;
	u32 usage_cred;
	u32 cred_task;
	u32 mm_task;
	u32 pid_task;
	u32 rp_task;
	u32 comm_task;
	u32 bp_cred_secptr;
} kdp_init_t;
#endif  /* CONFIG_RKP_KDP */

#define	RKP_PHYS_OFFSET_MAX		(({ max_pfn; }) << PAGE_SHIFT)
#define RKP_PHYS_ADDR_MASK		((1ULL << 40)-1)

#define	RKP_PGT_BITMAP_LEN	0x30000
#define	TIMA_ROBUF_START	0x9FA08000
#define	TIMA_ROBUF_SIZE		0x5f8000 /* 6MB - RKP_SEC_LOG_SIZE - RKP_DASHBOARD_SIZE - KASLR_OFFSET)*/

#define RKP_RBUF_VA      (phys_to_virt(TIMA_ROBUF_START))
#define RO_PAGES  (TIMA_ROBUF_SIZE >> PAGE_SHIFT) // (TIMA_ROBUF_SIZE/PAGE_SIZE)



static inline void rkp_bm_flush_cache(u64 addr)
{
	asm volatile(
			"mov x1, %0\n"
			"dc civac, x1\n"
			:
			: "r" (addr)
			: "x1", "memory");
}

#define rkp_is_pte_protected(va)	rkp_is_protected(__pa(va), (u64 *)rkp_pgt_bitmap, 0)
#define rkp_is_pg_protected(va)	rkp_is_protected(__pa(va), (u64 *)rkp_pgt_bitmap, 1)
#define rkp_is_pg_dbl_mapped(pa) rkp_is_protected(pa, (u64 *)rkp_map_bitmap, 0)
static inline u8 rkp_is_protected(u64 pa, u64 *base_addr, int type)
{
	u64 phys_addr = (pa & (RKP_PHYS_ADDR_MASK));
	u64 index = ((phys_addr-PHYS_OFFSET)>>PAGE_SHIFT);
	u64 *p = base_addr;
	u64 tmp = (index>>6);
	u64 rindex;
	u8 val;

	/* We are going to ignore if input address does NOT belong to DRAM area */
	if (!rkp_started || (phys_addr < PHYS_OFFSET) || (phys_addr  > RKP_PHYS_OFFSET_MAX))
		return 0;
	/* We don't have to check for RO buffer area, This is optimization */
	if (type && ((phys_addr >= TIMA_ROBUF_START) && (phys_addr  < (TIMA_ROBUF_START + TIMA_ROBUF_SIZE))))
		return 1;
	p += (tmp);
	rindex = index % 64;
	if (type)
		rkp_bm_flush_cache((u64)p);
	val = (((*p) & (1ULL<<rindex))?1:0);
	return val;
}
#endif //__ASSEMBLY__

#endif //_RKP_H

