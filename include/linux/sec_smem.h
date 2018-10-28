/*
 * include/linux/sec_smem.h
 *
 * COPYRIGHT(C) 2016-2017 Samsung Electronics Co., Ltd. All Right Reserved.
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

#ifndef _SEC_SMEM_H_
#define _SEC_SMEM_H_

typedef struct {
	uint64_t magic;
	uint64_t version;
} sec_smem_header_t;

/* For SMEM_ID_VENDOR0 */
#define SMEM_VEN0_MAGIC 0x304E4556304E4556
#define SMEM_VEN0_VER 2

#define DDR_IFNO_REVISION_ID1		8
#define DDR_IFNO_REVISION_ID2		16
#define DDR_IFNO_TOTAL_DENSITY		24

typedef struct{
    uint64_t afe_rx_good;
    uint64_t afe_rx_mute;
    uint64_t afe_tx_good;
    uint64_t afe_tx_mute;
} smem_afe_log_t;

typedef struct{
    uint64_t reserved[5];
} smem_afe_ext_t;

typedef struct {
	uint32_t ddr_vendor;
	uint32_t reserved;
	smem_afe_log_t afe_log;
} sec_smem_id_vendor0_v1_t;

typedef struct {
	sec_smem_header_t header;
	uint32_t ddr_vendor;
	uint32_t reserved;
	smem_afe_log_t afe_log;
	smem_afe_ext_t afe_ext;
} sec_smem_id_vendor0_v2_t;

/* For SMEM_ID_VENDOR1 */
#define SMEM_VEN1_MAGIC 0x314E4556314E4556
#define SMEM_VEN1_VER 4

typedef struct {
	uint64_t hw_rev;
} sec_smem_id_vendor1_v1_t;

typedef struct {
	uint64_t hw_rev;
	uint64_t ap_suspended;
} sec_smem_id_vendor1_v2_t;

#define NUM_CH 4
#define NUM_CS 2
#define NUM_DQ_PCH 2

typedef struct {
	uint8_t tDQSCK[NUM_CH][NUM_CS][NUM_DQ_PCH];
} ddr_rcw_t;

typedef struct {
	uint8_t coarse_cdc[NUM_CH][NUM_CS][NUM_DQ_PCH];
	uint8_t fine_cdc[NUM_CH][NUM_CS][NUM_DQ_PCH];
} ddr_wr_dqdqs_t;

typedef struct {
	/* WR */
	uint8_t wr_pr_width[NUM_CH][NUM_CS][NUM_DQ_PCH];
	uint8_t wr_min_eye_height[NUM_CH][NUM_CS];
	uint8_t wr_best_vref[NUM_CH][NUM_CS];
	uint8_t wr_vmax_to_vmid[NUM_CH][NUM_CS][NUM_DQ_PCH];
	uint8_t wr_vmid_to_vmin[NUM_CH][NUM_CS][NUM_DQ_PCH];
	/* RD */
	uint8_t rd_pr_width[NUM_CH][NUM_CS][NUM_DQ_PCH];
	uint8_t rd_min_eye_height[NUM_CH][NUM_CS];
	uint8_t rd_best_vref[NUM_CH][NUM_CS][NUM_DQ_PCH];
	/* DCC */
	uint8_t dq_dcc_abs[NUM_CH][NUM_CS][NUM_DQ_PCH];
	uint8_t dqs_dcc_adj[NUM_CH][NUM_DQ_PCH];	

	uint16_t small_eye_detected;
} ddr_dqdqs_eye_t;

typedef struct {
	uint32_t version;
	ddr_rcw_t rcw;
	ddr_wr_dqdqs_t wr_dqdqs;
	ddr_dqdqs_eye_t dqdqs_eye;
} ddr_train_t;

typedef struct {
	uint64_t num;
	void * nIndex __attribute__((aligned(8)));
	void * DDRLogs __attribute__((aligned(8)));
	void * DDR_STRUCT __attribute__((aligned(8)));
} smem_ddr_stat_t;

typedef struct {
	sec_smem_header_t header;
	sec_smem_id_vendor1_v2_t ven1_v2;
	smem_ddr_stat_t ddr_stat;
} sec_smem_id_vendor1_v3_t;

typedef struct {
	void *clk __attribute__((aligned(8)));
	void *apc_cpr __attribute__((aligned(8)));
} smem_apps_stat_t;

typedef struct {
	void *vreg __attribute__((aligned(8)));
} smem_vreg_stat_t;

typedef struct {
	sec_smem_header_t header;
	sec_smem_id_vendor1_v2_t ven1_v2;
	smem_ddr_stat_t ddr_stat;
	void * ap_health __attribute__((aligned(8)));
	smem_apps_stat_t apps_stat;
	smem_vreg_stat_t vreg_stat;
	ddr_train_t ddr_training;
} sec_smem_id_vendor1_v4_t;

extern uint8_t ddr_get_wr_pr_width(uint32_t ch, uint32_t cs, uint32_t dq);
extern uint8_t ddr_get_wr_min_eye_height(uint32_t ch, uint32_t cs);
extern uint8_t ddr_get_wr_best_vref(uint32_t ch, uint32_t cs);
extern uint8_t ddr_get_wr_vmax_to_vmid(uint32_t ch, uint32_t cs, uint32_t dq);
extern uint8_t ddr_get_wr_vmid_to_vmin(uint32_t ch, uint32_t cs, uint32_t dq);

extern uint8_t ddr_get_rd_pr_width(uint32_t ch, uint32_t cs, uint32_t dq);
extern uint8_t ddr_get_rd_min_eye_height(uint32_t ch, uint32_t cs);
extern uint8_t ddr_get_rd_best_vref(uint32_t ch, uint32_t cs, uint32_t dq);

extern uint8_t ddr_get_dq_dcc_abs(uint32_t ch, uint32_t cs, uint32_t dq);
extern uint8_t ddr_get_dqs_dcc_adj(uint32_t ch, uint32_t dq);

extern uint16_t ddr_get_small_eye_detected(void);

#ifdef CONFIG_SEC_BSP
extern char* get_ddr_vendor_name(void);
extern uint32_t get_ddr_DSF_version(void);
extern uint8_t get_ddr_revision_id_1(void);
extern uint8_t get_ddr_revision_id_2(void);
extern uint8_t get_ddr_total_density(void);
extern uint8_t get_ddr_rcw_tDQSCK(uint32_t ch, uint32_t cs, uint32_t dq);
extern uint8_t get_ddr_wr_coarseCDC(uint32_t ch, uint32_t cs, uint32_t dq);
extern uint8_t get_ddr_wr_fineCDC(uint32_t ch, uint32_t cs, uint32_t dq);
#endif

#endif // _SEC_SMEM_H_
