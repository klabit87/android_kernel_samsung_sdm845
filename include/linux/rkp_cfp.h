/*
 * Copyright (c) 2015 Samsung Electronics Co., Ltd.
 * Authors:	James Gleeson <jagleeso@gmail.com>
 *		Wenbo Shen <wenbo.s@samsung.com>
 *
 * This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef __RKP_CFP_H__
#define __RKP_CFP_H__

/********************************************
 * For debugging SYSREGKEY, if defined,
 * thread key is fixed to 0x3333333333333333,
 * master key is fixed to 0x55555555
 * should be disabled for product release
 ********************************************/
/*#define SYSREG_DEBUG*/

#if (defined SYSREG_DEBUG) && (!defined CONFIG_RKP_CFP_ROPP_SYSREGKEY)
	#error "SYSREG_DEBUG depends on ROPP_SYSREGKEY"
#endif

#if (defined CONFIG_RKP_CFP_ROPP_SYSREGKEY) && (defined CONFIG_PERF_EVENTS_USERMODE)
	#error "SYSREGKEY can not co-exist with CONFIG_PERF_EVENTS_USERMODE"
#endif

/*
 * We've reserved x16, x17 using GCC options:
 * KCFLAGS="-ffixed-x16 -ffixed-17"
 * (RR = reserved register).
 *
 * Choose RRK to be x17, since x17 is used the least
 * frequently in low-level assembly code already.
 */
#define RRX x16
#define RRX_32 w16
#define RRX_NUM 16

#define RRK x17
#define RRK_NUM 17

#define RRMK dbgbvr5_el1

#define ROPP_ADDR	0x9FA07020
#define ROPP_MAGIC	0x4a4c4955

#ifndef __ASSEMBLY__

//two wrappers for replace and stringify
#define _STR(x) #x
#define STR(x) _STR(x)

#ifdef CONFIG_RKP_CFP_ROPP
struct task_struct;
unsigned long ropp_enable_backtrace(unsigned long addr, struct task_struct *tsk);

#ifdef CONFIG_RKP_CFP_ROPP_SYSREGKEY
extern unsigned long ropp_master_key;
#endif

#if (defined SYSREG_DEBUG) || defined(CONFIG_RKP_CFP_ROPP_FIXKEY)
extern unsigned long ropp_fixed_key;
#endif

#endif // CONFIG_RKP_CFP_ROPP
#endif //__ASSEMBLY__
#endif //__RKP_CFP_H__
