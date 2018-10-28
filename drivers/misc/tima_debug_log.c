#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/types.h>
#include <linux/proc_fs.h>
#include <linux/seq_file.h>
#include <linux/mm.h>
#include <linux/highmem.h>
#include <linux/io.h>
#include <linux/types.h>
#include <linux/uaccess.h>
#include <linux/slab.h>

#define DEBUG_LOG_START (0x9f900000)
#define	DEBUG_LOG_SIZE	(1<<20)
#define	DEBUG_LOG_MAGIC	(0xaabbccdd)
#define	DEBUG_LOG_ENTRY_SIZE	128

struct debug_log_entry_s {
	uint32_t	timestamp;          /* timestamp at which log entry was made*/
	uint32_t	logger_id;          /* id is 1 for tima, 2 for lkmauth app  */
#define	DEBUG_LOG_MSG_SIZE	(DEBUG_LOG_ENTRY_SIZE - sizeof(uint32_t) - sizeof(uint32_t))
	char	log_msg[DEBUG_LOG_MSG_SIZE];      /* buffer for the entry */
} __packed;

struct debug_log_header_s {
	uint32_t	magic;              /* magic number                         */
	uint32_t	log_start_addr;     /* address at which log starts          */
	uint32_t	log_write_addr;     /* address at which next entry is written*/
	uint32_t	num_log_entries;    /* number of log entries                */
	char	padding[DEBUG_LOG_ENTRY_SIZE - 4 * sizeof(uint32_t)];
} __packed;

#define DRIVER_DESC   "A kernel module to read tima debug log"

unsigned long *tima_debug_log_addr = 0;

ssize_t	tima_read(struct file *filep, char __user *buf, size_t size, loff_t *offset)
{
    char *localbuf = NULL;
	mm_segment_t old_fs = get_fs();
	/* First check is to get rid of integer overflow exploits */
	if (size > DEBUG_LOG_SIZE || (*offset) + size > DEBUG_LOG_SIZE) {
		pr_err("Extra read\n");
		return -EINVAL;
	}

    localbuf = kzalloc(size, GFP_KERNEL);
    if(localbuf == NULL)
        return -ENOMEM;

	set_fs(KERNEL_DS);
	memcpy_fromio(localbuf, (const char *)tima_debug_log_addr + (*offset), size);
	if (copy_to_user(buf, localbuf, size)) {
		set_fs(old_fs);
		pr_err("Copy to user failed\n");
		kfree(localbuf);
		return -1;
	} else {
		set_fs(old_fs);
		*offset += size;
		kfree(localbuf);
		return size;
	}
}

static const struct file_operations tima_proc_fops = {
	.read		= tima_read,
};

/**
 *      tima_debug_log_read_init -  Initialization function for TIMA
 *
 *      It creates and initializes tima proc entry with initialized read handler
 */
static int __init tima_debug_log_read_init(void)
{
	if (proc_create("tima_debug_log", 0644, NULL, &tima_proc_fops) == NULL) {
		pr_err("%s: Error creating proc entry\n", __func__);
		goto error_return;
	}
	pr_info("%s: Registering /proc/tima_debug_log Interface\n", __func__);
	tima_debug_log_addr = (unsigned long *)ioremap(DEBUG_LOG_START, DEBUG_LOG_SIZE);

	if (tima_debug_log_addr == NULL) {
		pr_err("%s: ioremap Failed\n", __func__);
		goto ioremap_failed;
	}
	return 0;

ioremap_failed:
	remove_proc_entry("tima_debug_log", NULL);
error_return:
	return -1;
}

/**
 *      tima_debug_log_read_exit -  Cleanup Code for TIMA
 *
 *      It removes /proc/tima proc entry and does the required cleanup operations
 */
static void __exit tima_debug_log_read_exit(void)
{
	remove_proc_entry("tima_debug_log", NULL);
	pr_info("Deregistering /proc/tima_debug_log Interface\n");
	if (tima_debug_log_addr != NULL)
		iounmap(tima_debug_log_addr);
}


module_init(tima_debug_log_read_init);
module_exit(tima_debug_log_read_exit);

MODULE_DESCRIPTION(DRIVER_DESC);
