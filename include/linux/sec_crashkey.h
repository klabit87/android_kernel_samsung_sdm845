/*
 * include/linux/sec_crashkey.h
 *
 * COPYRIGHT(C) 2017 Samsung Electronics Co., Ltd. All Right Reserved.
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

#ifndef SEC_CRASHKEY_H
#define SEC_CRASHKEY_H

#define MAX_NAME_SIZE		64

#define SEC_CRASHKEY_SHORTKEY	0
#define SEC_CRASHKEY_LONGKEY	1

struct sec_crash_key {
	char name[MAX_NAME_SIZE];		/* name */
	unsigned int *keycode;			/* keycode array */
	unsigned int size;			/* number of used keycode */
	unsigned int timeout;			/* msec timeout */
	unsigned long long_keypress;		/* trigger by pressing key combination longger */
	unsigned int unlock;			/* unlocking mask value */
	unsigned int trigger;			/* trigger key code */
	unsigned int knock;			/* number of triggered */
	void (*callback)(unsigned long);	/* callback function when the key triggered */
	struct list_head node;
};

#ifdef CONFIG_SEC_DEBUG
extern int sec_debug_register_crash_key(struct sec_crash_key *crash_key);

#ifdef CONFIG_INPUT_QPNP_POWER_ON
extern int qpnp_control_s2_reset_onoff(int on);
#else
static int qpnp_control_s2_reset_onoff(int on) { return 0; };
#endif

#else
static inline int sec_debug_register_crash_key(
				struct sec_crash_key *crash_key) { }
#endif /* CONFIG_SEC_DEBUG */

#endif /* SEC_CRASHKEY_H */
