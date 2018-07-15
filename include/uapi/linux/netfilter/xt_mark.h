#ifndef _XT_MARK_H_target
#define _XT_MARK_H_target

#include <linux/netfilter/xt_mark.h>

struct xt_mark_tginfo2 {
	__u32 mark, mask;
};

struct xt_mark_mtinfo1 {
	__u32 mark, mask;
	__u8 invert;
};

#endif /*_XT_MARK_H_target */
