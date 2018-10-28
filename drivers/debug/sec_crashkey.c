/*
 * drivers/debug/sec_crashkey.c
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

#include <linux/init.h>
#include <linux/input.h>
#include <linux/slab.h>
#include <linux/notifier.h>
#include <linux/spinlock.h>
#include <linux/list_sort.h>
#include <linux/delay.h>

#include <linux/sec_debug.h>
#include <linux/sec_crashkey.h>

#include "sec_debug_internal.h"
#include "sec_key_notifier.h"

#define MIN_SIZE_KEY   2
#define MAX_SIZE_KEY   0xf

static unsigned int __available_keys[] = {
	KEY_VOLUMEDOWN, KEY_VOLUMEUP, KEY_POWER, KEY_HOMEPAGE, KEY_WINK
};

static unsigned int default_crash_key_code[] = {
	KEY_VOLUMEDOWN, KEY_POWER, KEY_POWER
};

static struct sec_crash_key dflt_crash_key = {
	.keycode	= default_crash_key_code,
	.size		= ARRAY_SIZE(default_crash_key_code),
	.long_keypress	= SEC_CRASHKEY_SHORTKEY,
	.timeout	= 1000,	/* 1 sec */
};

static unsigned int hardreset_key_code[] = {
	KEY_VOLUMEDOWN, KEY_POWER
};

static inline void __cb_longkeycrash(unsigned long arg);
static struct sec_crash_key hardreset_key = {
	.name		= "long_keycrash",
	.keycode	= hardreset_key_code,
	.size		= ARRAY_SIZE(hardreset_key_code),
	.long_keypress	= SEC_CRASHKEY_LONGKEY,
	.timeout	= 6000,	/* 6 sec */
	.callback	= &__cb_longkeycrash,
};


static size_t max_size_key = MAX_SIZE_KEY;
static unsigned int *__key_store;
static struct sec_crash_key *__sec_crash_key = &dflt_crash_key;
static struct timer_list __crash_timer;
static struct timer_list __longkey_timer;

static LIST_HEAD(key_crash_list);
static spinlock_t key_crash_lock;

static void __init_crash_timer(void)
{
	init_timer(&__crash_timer);
	init_timer(&__longkey_timer);
}

static inline void __sec_clear_crash_timer(struct timer_list *tl)
{
	del_timer(tl);
}

static inline void __sec_crash_timer_set(struct timer_list *tl,
					 struct sec_crash_key *crash_key)
{
	__sec_clear_crash_timer(tl);

	tl->data = crash_key->long_keypress;
	tl->function = crash_key->callback;
	tl->expires = jiffies + msecs_to_jiffies(crash_key->timeout);
	add_timer(tl);
}

static unsigned int __sec_debug_unlock_crash_key(unsigned int value,
		unsigned int *unlock)
{
	unsigned int ret = 0;
	unsigned int i = __sec_crash_key->size -
		__sec_crash_key->knock - 1;	/* except triggers */

	do {
		if (value == __sec_crash_key->keycode[i]) {
			ret = 1;
			*unlock |= 1 << i;
		}
	} while (i-- != 0);

	return ret;
}

static struct sec_crash_key *__sec_debug_get_first_key_entry(void)
{
	if (list_empty(&key_crash_list)) {
		pr_err("Key crash list is empty.\n");
		return NULL;
	}

	return list_first_entry(&key_crash_list, struct sec_crash_key, node);
}

static void __sec_debug_check_hard_reset_key(unsigned int value, int down)
{
	size_t i;
	static unsigned int key_bitmap;

	if (unlikely(!sec_debug_is_enabled()))
		return;

	for (i = 0; i < hardreset_key.size; i++) {
		if (hardreset_key.keycode[i] == value) {
			key_bitmap &= (~(1 << i));
			break;
		}
	}

	if (i == hardreset_key.size)
		return;

	if (down) {
		key_bitmap |= (1 << i);
		if (key_bitmap == ((1 << hardreset_key.size) - 1))
			__sec_crash_timer_set(&__longkey_timer, &hardreset_key);
	} else
		__sec_clear_crash_timer(&__longkey_timer);
}

static void __sec_debug_check_crash_key(unsigned int value, int down)
{
	static unsigned long unlock_jiffies;
	static unsigned long trigger_jiffies;
	static bool other_key_pressed;
	static unsigned int unlock;
	static unsigned int knock;
	static size_t k_idx;
	unsigned int timeout;

	__sec_clear_crash_timer(&__crash_timer);

	if (!down) {
		sec_debug_set_upload_cause(UPLOAD_CAUSE_INIT);

		if (unlock != __sec_crash_key->unlock ||
		    value != __sec_crash_key->trigger)
			goto __clear_all;
		else
			return;
	}

	if (KEY_POWER == value)
		sec_debug_set_upload_cause(UPLOAD_CAUSE_POWER_LONG_PRESS);

	if (unlikely(!sec_debug_is_enabled()))
		return;

	__key_store[k_idx++] = value;

	if (__sec_debug_unlock_crash_key(value, &unlock)) {
		if (unlock == __sec_crash_key->unlock && !unlock_jiffies)
			unlock_jiffies = jiffies;
	} else if ((value == __sec_crash_key->trigger)
		   && (k_idx <= __sec_crash_key->size)) {
		trigger_jiffies = jiffies;
		knock++;
	} else {
		size_t i;
		struct list_head *head = &__sec_crash_key->node;

		if (list_empty(&__sec_crash_key->node))
			goto __clear_all;

		list_for_each_entry(__sec_crash_key, head, node) {
			if (&__sec_crash_key->node == &key_crash_list)
				break;

			if (__sec_crash_key->size < k_idx)
				continue;

			for (i = 0; i < k_idx; i++) {
				if (__sec_crash_key->keycode[i] !=
						__key_store[i])
					break;
			}

			if (i == k_idx) {
				if (__sec_debug_unlock_crash_key(value,
								&unlock)) {
					if (unlock == __sec_crash_key->unlock
						&& !unlock_jiffies)
						unlock_jiffies = jiffies;
				} else {
					trigger_jiffies = jiffies;
					knock++;
				}
				goto __next;
			}
		}
		other_key_pressed = true;
		goto __clear_timeout;
	}

__next:
	if (unlock_jiffies && trigger_jiffies && !other_key_pressed &&
	    time_after(trigger_jiffies, unlock_jiffies)) {
		timeout = jiffies_to_msecs(trigger_jiffies - unlock_jiffies);
		if (timeout < __sec_crash_key->timeout) {
			if (knock == __sec_crash_key->knock) {
				if (__sec_crash_key->long_keypress)
					__sec_crash_timer_set(&__crash_timer,
							      __sec_crash_key);
				else
					__sec_crash_key->callback(0);
			}
		} else
			goto __clear_all;
	}

	if (k_idx < max_size_key)
		return;

__clear_all:
	other_key_pressed = false;
__clear_timeout:
	k_idx = 0;
	__sec_crash_key = __sec_debug_get_first_key_entry();

	unlock_jiffies = 0;
	trigger_jiffies = 0;
	unlock = 0;
	knock = 0;
}

static int sec_debug_keyboard_call(struct notifier_block *this,
				unsigned long type, void *data)
{
	unsigned long flags = 0;
	struct sec_key_notifier_param *param = data;

	spin_lock_irqsave(&key_crash_lock, flags);
	__sec_debug_check_hard_reset_key(param->keycode, param->down);
	__sec_debug_check_crash_key(param->keycode, param->down);
	spin_unlock_irqrestore(&key_crash_lock, flags);

	return NOTIFY_DONE;

}

static struct notifier_block sec_debug_keyboard_notifier = {
	.notifier_call = sec_debug_keyboard_call,
};

static int __check_crash_key(struct sec_crash_key *sck)
{
	if (!list_empty(&key_crash_list)) {
		size_t i;
		struct sec_crash_key *crash_key;

		list_for_each_entry(crash_key, &key_crash_list, node) {
			if (sck->size != crash_key->size)
				continue;

			for (i = 0; i < sck->size; i++)
				if (crash_key->keycode[i] !=
						sck->keycode[i])
					break;

			if (i == sck->size)
				return -ECANCELED;
		}
	}

	return 0;
}

static int __cmp_size_smaller_first(void *priv,
			    struct list_head *a, struct list_head *b)
{
	struct sec_crash_key *la = list_entry(a, struct sec_crash_key, node);
	struct sec_crash_key *lb = list_entry(b, struct sec_crash_key, node);

	if (la->long_keypress < lb->long_keypress)
		return 1;

	return (int)la->size - (int)lb->size;
}

int sec_debug_register_crash_key(struct sec_crash_key *crash_key)
{
	static unsigned int max_key_size;
	unsigned int i, j;
	unsigned long flags = 0;
	unsigned int size_keys = crash_key->size;

	if (unlikely(size_keys < MIN_SIZE_KEY)) {
		pr_err("Size of '%s' should be greater than %d\n",
		       crash_key->name, MIN_SIZE_KEY - 1);
		return -ECANCELED;
	} else if (unlikely(size_keys > MAX_SIZE_KEY)) {
		pr_err("Size of '%s' should be less than %d\n",
		       crash_key->name, MAX_SIZE_KEY + 1);
		return -ECANCELED;
	}

	for (i = 0; i < size_keys; i++) {
		int key = crash_key->keycode[i];

		for (j = 0; j < ARRAY_SIZE(__available_keys); j++)
			if (key == __available_keys[j])
				break;

		if (j == ARRAY_SIZE(__available_keys)) {
			pr_err("Unknown key found in %s\n", crash_key->name);
			return -ENOKEY;
		}
	}

	if (!crash_key->timeout)
		crash_key->timeout = dflt_crash_key.timeout;

	crash_key->trigger = crash_key->keycode[size_keys - 1];
	crash_key->knock = 1;

	i = size_keys - 2;
	do {
		if (crash_key->keycode[i] != crash_key->trigger)
			break;
		crash_key->knock++;
	} while (i--);

	if (crash_key->long_keypress)
		crash_key->knock = size_keys - 1;

	crash_key->unlock = 0;
	for (i = 0; i < size_keys - crash_key->knock; i++)
		crash_key->unlock |= 1 << i;

	spin_lock_irqsave(&key_crash_lock, flags);
	if (__check_crash_key(crash_key)) {
		spin_unlock_irqrestore(&key_crash_lock, flags);
		pr_err("Duplicated key combination found in %s\n",
		       crash_key->name);

		return -ECANCELED;
	}

	list_add_tail(&crash_key->node, &key_crash_list);
	list_sort(NULL, &key_crash_list, __cmp_size_smaller_first);

	__sec_crash_key = __sec_debug_get_first_key_entry();

	if (max_key_size < size_keys) {
		kvfree(__key_store);
		__key_store = kmalloc_array(size_keys,
					    sizeof(unsigned int),
					    GFP_ATOMIC);
		if (unlikely(!__key_store)) {
			spin_unlock_irqrestore(&key_crash_lock, flags);
			pr_err("Unable to allocate memory for crash key store.\n");
			return -ENOMEM;
		}
		max_key_size = size_keys;
	}

	if (max_size_key < size_keys)
		max_size_key = size_keys;

	spin_unlock_irqrestore(&key_crash_lock, flags);

	pr_info("%s registered.\n", crash_key->name);

	return 0;
}
EXPORT_SYMBOL(sec_debug_register_crash_key);

#if defined(CONFIG_TOUCHSCREEN_DUMP_MODE)
struct tsp_dump_callbacks dump_callbacks;
static inline void __cb_tsp_dump(unsigned long arg)
{
	pr_err("[TSP] dump_tsp_log : %d\n", __LINE__);
	if (dump_callbacks.inform_dump)
		dump_callbacks.inform_dump();
}

static unsigned int tsp_dump_code[] = {
	KEY_VOLUMEUP, KEY_WINK, KEY_WINK
};

static struct sec_crash_key tsp_dump = {
	.name		= "tsp_dump",
	.keycode	= tsp_dump_code,
	.size		= ARRAY_SIZE(tsp_dump_code),
	.long_keypress	= SEC_CRASHKEY_SHORTKEY,
	.timeout	= 1000,	/* 1 sec */
	.callback	= &__cb_tsp_dump,
};
#endif

static unsigned int keycrash_code[] = {
	KEY_VOLUMEDOWN, KEY_POWER, KEY_POWER
};

static inline void __reset_pon_s2_ctrl_reset(void) {
	qpnp_control_s2_reset_onoff(0);
	udelay(1000);
	qpnp_control_s2_reset_onoff(1);
}

static inline void __cb_keycrash(unsigned long arg)
{
	const unsigned char *msg[2] = {
			UPLOAD_MSG_CRASH_KEY, UPLOAD_MSG_LONG_KEY_PRESS};

	__reset_pon_s2_ctrl_reset();
	emerg_pet_watchdog();
	dump_stack();
	dump_all_task_info();
	dump_cpu_stat();
	panic(msg[arg]);
}

static struct sec_crash_key keycrash = {
	.name		= "keycrash",
	.keycode	= keycrash_code,
	.size		= ARRAY_SIZE(keycrash_code),
	.long_keypress	= SEC_CRASHKEY_SHORTKEY,
	.timeout	= 1000,	/* 1 sec */
	.callback	= &__cb_keycrash,
};

static inline void __cb_longkeycrash(unsigned long arg)
{
	printk(KERN_ERR "*** Force trigger kernel panic "
	       "before triggering hard reset ***\n");
	sec_debug_set_upload_cause(UPLOAD_CAUSE_POWER_LONG_PRESS);
	__cb_keycrash(SEC_CRASHKEY_LONGKEY);
}

static inline void __init __register_default_crash_key(void)
{
	sec_debug_register_crash_key(&keycrash);
#if defined(CONFIG_TOUCHSCREEN_DUMP_MODE)
	sec_debug_register_crash_key(&tsp_dump);
#endif
}

/* for retail group's request */
#if defined(CONFIG_SEC_PM)
void do_keyboard_notifier(int onoff)
{
	pr_info("%s: onoff(%d)\n", __func__, onoff);

	if (onoff)
		sec_kn_register_notifier(&sec_debug_keyboard_notifier);
	else
		sec_kn_unregister_notifier(&sec_debug_keyboard_notifier);
}
EXPORT_SYMBOL(do_keyboard_notifier);
#endif

static int __init sec_debug_init_crash_key(void)
{
	spin_lock_init(&key_crash_lock);

	__init_crash_timer();
	__register_default_crash_key();

	sec_kn_register_notifier(&sec_debug_keyboard_notifier);

	return 0;
}
arch_initcall_sync(sec_debug_init_crash_key);

