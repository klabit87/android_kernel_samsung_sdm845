/*
 * Copyright (C) 2010 Samsung Electronics
 * Hyoyoung Kim <hyway.kim@samsung.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
 *
 */

#ifndef __SM5720_MUIC_AFC_H__
#define __SM5720_MUIC_AFC_H__
#include <../drivers/muic/universal/muic-internal.h>

// struct muic_data_t;

/* SM5720 AFC CTRL register */
#define AFCCTRL_DIS_AFC		 5
#define AFCCTRL_VBUS_READ    3
#define AFCCTRL_DM_RESET     2
#define AFCCTRL_DP_RESET     1
#define AFCCTRL_ENAFC        0

enum {
	SUPPORT_QC_5V	= 5,
	SUPPORT_QC_9V	= 9,
	SUPPORT_QC_12V	= 12,
	SUPPORT_QC_20V	= 20,
};

int muic_check_afc_state(int state);
int muic_torch_prepare(int state);
void sm5720_afc_delay_check_state(int state);
void sm5720_afc_init_state(muic_data_t *pmuic);
int sm5720_afc_set_voltage(int vol);
int sm5720_afc_restart(void);
int muic_sm5720_set_afc(bool enable);

int sm5720_set_afc_ctrl_reg(struct regmap_desc *pdesc, int shift, bool on);
int sm5720_afc_ta_attach(struct regmap_desc *pdesc);
int sm5720_afc_ta_accept(struct regmap_desc *pdesc);
int sm5720_afc_vbus_update(struct regmap_desc *pdesc);
int sm5720_afc_multi_byte(struct regmap_desc *pdesc);
int sm5720_afc_error(struct regmap_desc *pdesc);
int sm5720_afc_init_check(struct regmap_desc *pdesc);

struct afc_ops {
	int (*afc_init)(struct regmap_desc  *);
	int (*afc_ta_attach)(struct regmap_desc  *);
	int (*afc_ta_accept)(struct regmap_desc  *);
	int (*afc_vbus_update)(struct regmap_desc  *);
	int (*afc_multi_byte)(struct regmap_desc  *);
	int (*afc_error)(struct regmap_desc  *);
	int (*afc_ctrl_reg)(struct regmap_desc  *, int, bool);
	int (*afc_init_check)(struct regmap_desc  *);
};

#endif /* __SM5720_MUIC_AFC_H__ */
