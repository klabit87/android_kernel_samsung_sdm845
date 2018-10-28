/*
 * =================================================================
 *
 *       Filename:  ss_dsi_flash_dimming_S6E3HA8_AMB638RD01.c
 *
 *    Description:  Smart dimming algorithm implementation
 *
 *        Company:  Samsung Electronics
 *
 * ================================================================
 */
/*
<one line to give the program's name and a brief idea of what it does.>
Copyright (C) 2012, Samsung Electronics. All rights reserved.

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
#ifndef _SS_DSI_INTERPOLATION_S6E3HA8_AMB638RD01_H_
#define _SS_DSI_INTERPOLATION_S6E3HA8_AMB638RD01_H_

#include "ss_dsi_panel_common.h"

int table_parsing_data_S6E3HA8_AMB638RD01(struct samsung_display_driver_data *vdd);
int table_gamma_update_S6E3HA8_AMB638RD01(struct samsung_display_driver_data *vdd);

int init_interpolation_S6E3HA8_AMB638RD01(struct samsung_display_driver_data *vdd, enum INTERPOLATION_MODE mode);
int flash_gamma_support_S6E3HA8_AMB638RD01(struct samsung_display_driver_data *vdd);

#endif
