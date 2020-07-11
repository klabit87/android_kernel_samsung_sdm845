/*
 * muic_hv.h
 *
 * Copyright (C) 2011 Samsung Electrnoics
 * Thomas Ryu <smilesr.ryu@samsung.com>
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
 *
 */

#ifndef __MUIC_HV_H__
#define __MUIC_HV_H__

#include "muic-internal.h"
#include "muic_hv_max77854.h"
#include "muic_sm5720_afc.h"

#include <linux/muic/muic.h>

#define MUIC_HV_DEV_NAME	"muic-hv"

#define MUIC_HV_5V	0x08
#define MUIC_HV_9V	0x46
#define MUIC_HV_12V	0x79

int muic_afc_set_voltage(int vol);
int muic_set_afc(bool enable);
void hv_afc_delay_check_state(muic_data_t *pmuic, int state);
void hv_afc_init(muic_data_t *pmuic);
void hv_afc_init_detect(muic_data_t *pmuic);
void hv_afc_irq_init(muic_data_t *pmuic);
void hv_afc_clear_hvcontrol(muic_data_t *pmuic);
void hv_afc_prepare_charger(muic_data_t *pmuic);
bool hv_afc_do_predetach(muic_data_t *pmuic, int mdev);
void hv_afc_do_detach(muic_data_t *pmuic);
void hv_afc_remove(muic_data_t *pmuic);
muic_attached_dev_t hv_afc_check_id_err(muic_data_t *pmuic, muic_attached_dev_t new_dev);
extern void hv_update_status(struct hv_data *phv, int mdev);

#endif /* __MUIC_HV_H__ */

