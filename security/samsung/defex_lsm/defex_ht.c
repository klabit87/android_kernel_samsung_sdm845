/*
 * Copyright (c) 2018 Samsung Electronics Co., Ltd. All Rights Reserved
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License version 2
 * as published by the Free Software Foundation.
 */

#include <linux/kernel.h>
#include <linux/list.h>
#include <linux/slab.h>
#include <linux/string.h>
#include <linux/spinlock.h>
#include "include/defex_catch_list.h"
#include "include/defex_internal.h"

#define MAX_PID_32 32768

#ifdef DEFEX_PED_ENABLE
DECLARE_HASHTABLE(creds_hash, 15);
#endif /* DEFEX_PED_ENABLE */

#ifdef DEFEX_PED_BASED_ON_TGID_ENABLE
struct id_set {
	unsigned int uid, fsuid, egid;
};
#endif /* DEFEX_PED_BASED_ON_TGID_ENABLE */

struct proc_cred_data {
	unsigned int uid, fsuid, egid;
	unsigned short cred_flags;
#ifdef DEFEX_PED_BASED_ON_TGID_ENABLE
	unsigned short tcnt;
	struct id_set upd_ids[];
#endif /* DEFEX_PED_BASED_ON_TGID_ENABLE */
};

struct proc_cred_struct {
	struct hlist_node node;
	struct proc_cred_data cred_data;
};

static DEFINE_SPINLOCK(creds_hash_update_lock);
static struct proc_cred_data *creds_fast_hash[MAX_PID_32 + 1];
static int creds_fast_hash_ready;

void creds_fast_hash_init(void)
{
	unsigned int i;

	for (i = 0; i <= MAX_PID_32; i++)
		creds_fast_hash[i] = NULL;
	creds_fast_hash_ready = 1;
}

int is_task_creds_ready(void)
{
	return creds_fast_hash_ready;
}

#ifdef DEFEX_PED_ENABLE
#ifdef DEFEX_PED_BASED_ON_TGID_ENABLE
void get_task_creds(struct task_struct *p, unsigned int *uid_ptr, unsigned int *fsuid_ptr, unsigned int *egid_ptr, unsigned short *cred_flags_ptr)
#else
void get_task_creds(int pid, unsigned int *uid_ptr, unsigned int *fsuid_ptr, unsigned int *egid_ptr, unsigned short *cred_flags_ptr)
#endif
{
	struct proc_cred_struct *obj;
	struct proc_cred_data *cred_data;
	unsigned int uid = 0, fsuid = 0, egid = 0;
	unsigned short cred_flags = CRED_FLAGS_PROOT;
	unsigned long flags;
#ifdef DEFEX_PED_BASED_ON_TGID_ENABLE
	int tgid = p->tgid, pid = p->pid;
#endif /* DEFEX_PED_BASED_ON_TGID_ENABLE */

	if (pid <= MAX_PID_32) {
		spin_lock_irqsave(&creds_hash_update_lock, flags);
#ifdef DEFEX_PED_BASED_ON_TGID_ENABLE
		cred_data = creds_fast_hash[tgid];
		if (cred_data) {
			if (tgid == pid) {
				if (cred_data->cred_flags & CRED_FLAGS_MAIN_UPDATED) {
					GET_CREDS(upd_ids->uid, upd_ids->fsuid, upd_ids->egid, cred_flags);
				} else {
					GET_CREDS(uid, fsuid, egid, cred_flags);
				}
			} else {
				if ((cred_data->cred_flags & CRED_FLAGS_SUB_UPDATED) && creds_fast_hash[pid])
					cred_data = creds_fast_hash[pid];
				GET_CREDS(uid, fsuid, egid, cred_flags);
			}
		}
#else
		cred_data = creds_fast_hash[pid];
		if (cred_data) {
			GET_CREDS(uid, fsuid, egid, cred_flags);
		}
#endif /* DEFEX_PED_BASED_ON_TGID_ENABLE */
		spin_unlock_irqrestore(&creds_hash_update_lock, flags);
	} else {
		spin_lock_irqsave(&creds_hash_update_lock, flags);
#ifdef DEFEX_PED_BASED_ON_TGID_ENABLE
		if (tgid == pid) {
			hash_for_each_possible(creds_hash, obj, node, tgid) {
				if (obj->cred_data.cred_flags & CRED_FLAGS_MAIN_UPDATED) {
					GET_CREDS_OBJ(upd_ids->uid, upd_ids->fsuid, upd_ids->egid, cred_flags);
				} else {
					GET_CREDS_OBJ(uid, fsuid, egid, cred_flags);
				}
				break;
			}
		} else {
			hash_for_each_possible(creds_hash, obj, node, tgid) {
				GET_CREDS_OBJ(uid, fsuid, egid, cred_flags);
				break;
			}
			if (cred_flags & CRED_FLAGS_SUB_UPDATED) {
#endif /* DEFEX_PED_BASED_ON_TGID_ENABLE */
				hash_for_each_possible(creds_hash, obj, node, pid) {
					GET_CREDS_OBJ(uid, fsuid, egid, cred_flags);
					break;
				}
#ifdef DEFEX_PED_BASED_ON_TGID_ENABLE
			}
		}
#endif /* DEFEX_PED_BASED_ON_TGID_ENABLE */
		spin_unlock_irqrestore(&creds_hash_update_lock, flags);
	}
	*uid_ptr = uid;
	*fsuid_ptr = fsuid;
	*egid_ptr = egid;
	*cred_flags_ptr = cred_flags;
}

#ifdef DEFEX_PED_BASED_ON_TGID_ENABLE
int set_task_creds(struct task_struct *p, unsigned int uid, unsigned int fsuid, unsigned int egid, unsigned short cred_flags)
#else
int set_task_creds(int pid, unsigned int uid, unsigned int fsuid, unsigned int egid, unsigned short cred_flags)
#endif /* DEFEX_PED_BASED_ON_TGID_ENABLE */
{
	struct proc_cred_struct *obj;
	struct proc_cred_data *cred_data = NULL, *tmp_data = NULL;
	unsigned long flags;
#ifdef DEFEX_PED_BASED_ON_TGID_ENABLE
	struct proc_cred_struct *tmp_obj = NULL;
	int tgid = p->tgid, pid = p->pid;
	unsigned short tmp_cred_flags = 0x80;
#endif /* DEFEX_PED_BASED_ON_TGID_ENABLE */

alloc_obj:;
	if (pid <= MAX_PID_32) {
#ifdef DEFEX_PED_BASED_ON_TGID_ENABLE
		if (!creds_fast_hash[tgid]) {
#else
		if (!creds_fast_hash[pid]) {
#endif /* DEFEX_PED_BASED_ON_TGID_ENABLE */
			tmp_data = kmalloc(sizeof(struct proc_cred_data), GFP_ATOMIC);
			if (!tmp_data)
				return -1;
		}
		spin_lock_irqsave(&creds_hash_update_lock, flags);
#ifdef DEFEX_PED_BASED_ON_TGID_ENABLE
		cred_data = creds_fast_hash[tgid];
#else
		cred_data = creds_fast_hash[pid];
#endif /* DEFEX_PED_BASED_ON_TGID_ENABLE */
		if (!cred_data) {
			if (!tmp_data) {
				spin_unlock_irqrestore(&creds_hash_update_lock, flags);
				goto alloc_obj;
			}
			cred_data = tmp_data;
#ifdef DEFEX_PED_BASED_ON_TGID_ENABLE
			cred_data->cred_flags = 0;
			cred_data->tcnt = 1;
			creds_fast_hash[tgid] = cred_data;
#else
			creds_fast_hash[pid] = cred_data;
#endif /* DEFEX_PED_BASED_ON_TGID_ENABLE */
			tmp_data = NULL;
		}
#ifdef DEFEX_PED_BASED_ON_TGID_ENABLE
		if (cred_data && cred_data->tcnt >= 2) {
			if (tgid == pid) {
				if (!(cred_data->cred_flags & CRED_FLAGS_MAIN_UPDATED)) {
					cred_data->cred_flags |= CRED_FLAGS_MAIN_UPDATED;
					spin_unlock_irqrestore(&creds_hash_update_lock, flags);
					tmp_data = kmalloc(sizeof(struct proc_cred_data) + sizeof(struct id_set), GFP_ATOMIC);
					if (!tmp_data) {
						return -1;
					}
					spin_lock_irqsave(&creds_hash_update_lock, flags);
					*tmp_data = *cred_data;
					kfree(cred_data);
					cred_data = tmp_data;
					creds_fast_hash[tgid] = cred_data;
					tmp_data = NULL;
				}
				SET_CREDS(upd_ids->uid, upd_ids->fsuid, upd_ids->egid, cred_flags);
			} else {
				cred_data->cred_flags |= CRED_FLAGS_SUB_UPDATED;
				cred_data = creds_fast_hash[pid];
				if (!cred_data) {
					spin_unlock_irqrestore(&creds_hash_update_lock, flags);
					tmp_data = kmalloc(sizeof(struct proc_cred_data), GFP_ATOMIC);
					if (!tmp_data) {
						return -1;
					}
					spin_lock_irqsave(&creds_hash_update_lock, flags);
					cred_data = tmp_data;
					creds_fast_hash[pid] = cred_data;
					tmp_data = NULL;
				}
				cred_data->cred_flags = 0;
				SET_CREDS(uid, fsuid, egid, cred_flags);
			}
		} else {
#endif /* DEFEX_PED_BASED_ON_TGID_ENABLE */
			SET_CREDS(uid, fsuid, egid, cred_flags);
#ifdef DEFEX_PED_BASED_ON_TGID_ENABLE
		}
#endif /* DEFEX_PED_BASED_ON_TGID_ENABLE */
		spin_unlock_irqrestore(&creds_hash_update_lock, flags);
		if (tmp_data)
			kfree(tmp_data);
		return 0;
	}

	spin_lock_irqsave(&creds_hash_update_lock, flags);
#ifdef DEFEX_PED_BASED_ON_TGID_ENABLE
	hash_for_each_possible(creds_hash, obj, node, tgid) {
		if (obj->cred_data.tcnt >= 2) {
			tmp_cred_flags = obj->cred_data.cred_flags;
			obj->cred_data.cred_flags |= ((tgid == pid) ? CRED_FLAGS_MAIN_UPDATED : CRED_FLAGS_SUB_UPDATED);
			break;
		}
#else
	hash_for_each_possible(creds_hash, obj, node, pid) {
#endif /* DEFEX_PED_BASED_ON_TGID_ENABLE */
		SET_CREDS_OBJ(uid, fsuid, egid, cred_flags);
		spin_unlock_irqrestore(&creds_hash_update_lock, flags);
		return 0;
	}
	spin_unlock_irqrestore(&creds_hash_update_lock, flags);
#ifdef DEFEX_PED_BASED_ON_TGID_ENABLE
	if (tmp_cred_flags == 0x80) {
#endif /* DEFEX_PED_BASED_ON_TGID_ENABLE */
		obj = kmalloc(sizeof(struct proc_cred_struct), GFP_ATOMIC);
		if (!obj)
			return -1;
		obj->cred_data.cred_flags = 0;
		SET_CREDS_OBJ(uid, fsuid, egid, cred_flags);
#ifdef DEFEX_PED_BASED_ON_TGID_ENABLE
		obj->cred_data.tcnt = 1;
#endif /* DEFEX_PED_BASED_ON_TGID_ENABLE */
		spin_lock_irqsave(&creds_hash_update_lock, flags);
#ifdef DEFEX_PED_BASED_ON_TGID_ENABLE
		hash_add(creds_hash, &obj->node, tgid);
#else
		hash_add(creds_hash, &obj->node, pid);
#endif /* DEFEX_PED_BASED_ON_TGID_ENABLE */
		spin_unlock_irqrestore(&creds_hash_update_lock, flags);
		return 0;
#ifdef DEFEX_PED_BASED_ON_TGID_ENABLE
	}
#endif /* DEFEX_PED_BASED_ON_TGID_ENABLE */

#ifdef DEFEX_PED_BASED_ON_TGID_ENABLE
	if (tgid == pid) {
		if (!(tmp_cred_flags & CRED_FLAGS_MAIN_UPDATED)) {
			obj = kmalloc(sizeof(struct proc_cred_struct) + sizeof(struct id_set), GFP_ATOMIC);
			if (!obj)
				return -1;
			spin_lock_irqsave(&creds_hash_update_lock, flags);
			hash_for_each_possible(creds_hash, tmp_obj, node, tgid) {
				*obj = *tmp_obj;
				hash_del(&tmp_obj->node);
				kfree(tmp_obj);
				break;
			}
			SET_CREDS_OBJ(upd_ids->uid, upd_ids->fsuid, upd_ids->egid, cred_flags);
			hash_add(creds_hash, &obj->node, tgid);
			spin_unlock_irqrestore(&creds_hash_update_lock, flags);
			return 0;
		} else {
			spin_lock_irqsave(&creds_hash_update_lock, flags);
			hash_for_each_possible(creds_hash, obj, node, tgid) {
				SET_CREDS_OBJ(upd_ids->uid, upd_ids->fsuid, upd_ids->egid, cred_flags);
				spin_unlock_irqrestore(&creds_hash_update_lock, flags);
				return 0;
			}
			spin_unlock_irqrestore(&creds_hash_update_lock, flags);
			return -1;
		}
	} else {
		spin_lock_irqsave(&creds_hash_update_lock, flags);
		hash_for_each_possible(creds_hash, obj, node, pid) {
			SET_CREDS_OBJ(uid, fsuid, egid, cred_flags);
			spin_unlock_irqrestore(&creds_hash_update_lock, flags);
			return 0;
		}
		spin_unlock_irqrestore(&creds_hash_update_lock, flags);
		obj = kmalloc(sizeof(struct proc_cred_struct), GFP_ATOMIC);
		if (!obj)
			return -1;
		obj->cred_data.cred_flags = 0;
		SET_CREDS_OBJ(uid, fsuid, egid, cred_flags);
		spin_lock_irqsave(&creds_hash_update_lock, flags);
		hash_add(creds_hash, &obj->node, pid);
		spin_unlock_irqrestore(&creds_hash_update_lock, flags);
		return 0;
	}
#endif /* DEFEX_PED_BASED_ON_TGID_ENABLE */
}
#endif /* DEFEX_PED_ENABLE */

#ifdef DEFEX_PED_BASED_ON_TGID_ENABLE
void set_task_creds_tcnt(struct task_struct *p, int addition)
{
	struct proc_cred_struct *tgid_obj, *pid_obj;
	struct proc_cred_data *tgid_cred_data = NULL, *pid_cred_data = NULL;
	unsigned long flags;
	int tgid = p->tgid, pid = p->pid;

	if (pid <= MAX_PID_32) {
		spin_lock_irqsave(&creds_hash_update_lock, flags);
		if (tgid != pid && addition == -1) {
			pid_cred_data = creds_fast_hash[pid];
			if (pid_cred_data) {
				creds_fast_hash[pid] = NULL;
				kfree(pid_cred_data);
			}
		}
		tgid_cred_data = creds_fast_hash[tgid];
		if (tgid_cred_data) {
			tgid_cred_data->tcnt += addition;
			if (!tgid_cred_data->tcnt) {
				creds_fast_hash[tgid] = NULL;
				spin_unlock_irqrestore(&creds_hash_update_lock, flags);
				kfree(tgid_cred_data);
				return;
			}
		}
		spin_unlock_irqrestore(&creds_hash_update_lock, flags);
		return;
	}

	spin_lock_irqsave(&creds_hash_update_lock, flags);
	if (tgid != pid && addition == -1) {
		hash_for_each_possible(creds_hash, pid_obj, node, pid) {
			hash_del(&pid_obj->node);
			kfree(pid_obj);
			break;
		}
	}
	hash_for_each_possible(creds_hash, tgid_obj, node, tgid) {
		tgid_obj->cred_data.tcnt += addition;
		if (!tgid_obj->cred_data.tcnt) {
			hash_del(&tgid_obj->node);
			spin_unlock_irqrestore(&creds_hash_update_lock, flags);
			kfree(tgid_obj);
			return;
		}
		break;
	}
	spin_unlock_irqrestore(&creds_hash_update_lock, flags);
	return;
}
#else
void delete_task_creds(int pid)
{
	struct proc_cred_struct *obj;
	struct proc_cred_data *cred_data;
	unsigned long flags;

	if (pid <= MAX_PID_32) {
		spin_lock_irqsave(&creds_hash_update_lock, flags);
		cred_data = creds_fast_hash[pid];
		creds_fast_hash[pid] = NULL;
		spin_unlock_irqrestore(&creds_hash_update_lock, flags);
		kfree(cred_data);
		return;
	}

	spin_lock_irqsave(&creds_hash_update_lock, flags);
	hash_for_each_possible(creds_hash, obj, node, pid) {
		hash_del(&obj->node);
		spin_unlock_irqrestore(&creds_hash_update_lock, flags);
		kfree(obj);
		return;
	}
	spin_unlock_irqrestore(&creds_hash_update_lock, flags);
}
#endif /* DEFEX_PED_BASED_ON_TGID_ENABLE */

