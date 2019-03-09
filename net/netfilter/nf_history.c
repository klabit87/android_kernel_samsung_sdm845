#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/proc_fs.h>
#include <linux/seq_file.h>
#include <linux/netdevice.h>

#define nf_history_buffer_size (1024 * 16)

static char *nf_history_buffer;
static int nf_history_index;

static int nf_history_proc_show(struct seq_file *m, void *v)
{
	if (!nf_history_buffer)
		return 0;

	seq_printf(m, "%s", nf_history_buffer);
	return 0;
}

static int nf_history_proc_open(struct inode *inode, struct file  *file)
{
	return single_open(file, nf_history_proc_show, PDE_DATA(inode));
}

static ssize_t nf_history_write(struct file *file,
					const char __user *buf,
					size_t len, loff_t *offset)
{
	if (!nf_history_buffer)
		return 0;

	if (nf_history_index < 0 || nf_history_index >= nf_history_buffer_size)
		nf_history_index  = 0;

	if (len >= nf_history_buffer_size)
		len = nf_history_buffer_size - 1;

	if (nf_history_index + len < nf_history_buffer_size) {
		if (copy_from_user(nf_history_buffer + nf_history_index, buf, len))
			return -EFAULT;
		nf_history_index += len;
	} else {
		if (copy_from_user(nf_history_buffer + nf_history_index,
							buf,
							nf_history_buffer_size - nf_history_index))
			return -EFAULT;
		if (copy_from_user(nf_history_buffer,
							buf + nf_history_buffer_size - nf_history_index,
							len + nf_history_index - nf_history_buffer_size))
			return -EFAULT;
		nf_history_index = len + nf_history_index - nf_history_buffer_size;
	}

	return len;
}

static const struct file_operations nfhistory_file_ops = {
	.owner		= THIS_MODULE,
	.open		= nf_history_proc_open,
	.read		= seq_read,
	.write		= nf_history_write,
};

static int __init nf_history_init(void)
{
	struct proc_dir_entry *entry;

	nf_history_buffer = kzalloc(nf_history_buffer_size, GFP_NOWAIT);
	if (!nf_history_buffer) {
		pr_err("%s: alloc failed\n", __func__);
		return 0;
	}

	entry = proc_create("nf_history", 0666,
			    init_net.proc_net, &nfhistory_file_ops);
	if (!entry) {
		pr_err("%s: failed to create proc entry\n", __func__);
		return 0;
	}
	proc_set_size(entry, nf_history_buffer_size);

	return 0;
}

static void __exit nf_history_exit(void)
{
	if (nf_history_buffer)
		kfree(nf_history_buffer);
}

module_init(nf_history_init);
module_exit(nf_history_exit);
