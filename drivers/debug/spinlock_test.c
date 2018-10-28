/*
 * File is for internal use only, and not for distribution
 * If distribution is needed, OS review is required.
 * See OSRQCT-5220 for further information.
 */

#define pr_fmt(fmt) "spinlock-test: " fmt

#include <linux/delay.h>
#include <linux/err.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/sched.h>
#include <linux/workqueue.h>
#include <linux/cpu.h>
#include <linux/debugfs.h>
#include <linux/platform_device.h>
#include <linux/slab.h>
#include <linux/ktime.h>
#include <linux/dma-mapping.h>
#include <linux/version.h>

#define MODULE_NAME "spinlock_test"
#define CPU_NUM_MAX	NR_CPUS
#define TEST_ITERS_MAX	100
#define INNER_TEST_TIME_SECS_DEFAULT 5
#define INNER_TEST_TIME_SECS_MAX 100
#define SPINLOCK_TEST_DELAY_MS 1000
#define IRQ_DIALBE_TIME_MS (MSEC_PER_SEC/HZ)

static void spinwork_fn(struct work_struct *work);
static struct delayed_work spinworks[CPU_NUM_MAX];
static struct dentry *dent;
static DEFINE_PER_CPU(int, cpu_test_idx);

static u32 start;
static u32 num_test_iters;
static u32 canceltest;
static u32 cpu_start[CPU_NUM_MAX];
static u32 inner_test_time_secs = INNER_TEST_TIME_SECS_DEFAULT;

static struct locks {
	spinlock_t lock1;
	spinlock_t lock2;
} testlocks __cacheline_aligned;

/*
 * global_lock_static will be in the kernel static (BSS?) region.
 * global_inner_lock will be kmalloc'ed. The idea here is to use
 * different cache-lines for the two locks.
 */
static DEFINE_SPINLOCK(global_lock_static);
static spinlock_t *global_lock_heap;
static spinlock_t *uncached_lock;
static phys_addr_t phys;

enum {
	IRQS_DISABLED_TEST = 1,
	STACK_LOCK_TEST,
	TWOLOCKS_TEST,
	SIMPLE_TEST,
	UNCACHED_LOCK_TEST,
	NUM_OF_SPINLOCK_TEST = 5,
};

static int set_inner_test_time(void *data, u64 val)
{
	if (val != 0) {
		inner_test_time_secs = val < INNER_TEST_TIME_SECS_MAX ? \
			val : INNER_TEST_TIME_SECS_MAX;
		pr_debug("set_inner_test_time: %u\n", inner_test_time_secs);
	}
	return 0;
}

static int get_inner_test_time(void *data, u64 *val)
{
	*val = inner_test_time_secs;
	return 0;
}

static int spinlock_test_start(void);
static int spinlock_test_stop(void);
static int set_spinlock_start(void *data, u64 val)
{
	int ret = 0;
	if (val > 0) {
		if (start) {
			pr_err("The last test is not done yet. Exitting...\n");
			return 0;
		}
		num_test_iters = val < TEST_ITERS_MAX ? val : TEST_ITERS_MAX;
		ret = spinlock_test_start();
	} else if (val == 0) {
		if (!start) {
			pr_err("No spinlock test is running. Exitting...\n");
			return 0;
		}
		ret = spinlock_test_stop();
	}
	return ret;
}

static int get_spinlock_start(void *data, u64 *val)
{
	*val = start;
	return 0;
}

DEFINE_SIMPLE_ATTRIBUTE(spinlock_start_ops,
	get_spinlock_start, set_spinlock_start, "%llu\n");
DEFINE_SIMPLE_ATTRIBUTE(inner_test_time_ops,
	get_inner_test_time, set_inner_test_time, "%llu\n");

static int creat_spinlock_debugfs(void)
{
	/*
	* Create a simple debugfs with the name of "spinlock-test",
	*
	* As this is a simple directory, no uevent will be sent to
	* userspace.
	*/
	struct dentry *phandle;

	dent = debugfs_create_dir("spinlock-test", 0);
	if (IS_ERR(dent))
		return -ENOMEM;

	phandle = debugfs_create_file("start", 0666, dent, 0, &spinlock_start_ops);
	if (!phandle || IS_ERR(phandle))
		goto err;

	phandle = debugfs_create_file("inner_test_time", 0666, dent, 0, &inner_test_time_ops);
	if (!phandle || IS_ERR(phandle))
		goto err;

	return 0;
err:
	debugfs_remove_recursive(dent);
	return -ENOMEM;
}

static void sync_cpus(int test_idx, int cpu)
{
	int ocpu, flag;
	per_cpu(cpu_test_idx, cpu) = test_idx;
	/*
		Wait until all other cpus finished the same test case
		within the same iteration.
	*/
	do {
		flag = 0;
		for_each_online_cpu(ocpu) {
			if (per_cpu(cpu_test_idx, ocpu) != test_idx ||
				cpu_start[ocpu] != cpu_start[cpu]) {
				flag = 1;
				break;
			}
		}
	} while (flag);
}

static void irqs_disabled_spintest(int cpu)
{
	unsigned long flags, irqflags;
	int incritical = 0;
	ktime_t test_time = ktime_add_us(ktime_get(),
				(inner_test_time_secs * USEC_PER_SEC));
	pr_info("(irq_disabled): Running test on CPU %d for %d seconds.\n", cpu,
		inner_test_time_secs);

	while (ktime_compare(ktime_get(), test_time) < 0)
	{
		ktime_t irq_disable_test_time = ktime_add_us(ktime_get(),
				(IRQ_DIALBE_TIME_MS * USEC_PER_MSEC));
		local_irq_save(irqflags);
		while (ktime_compare(ktime_get(), irq_disable_test_time) < 0) {
			spin_lock_irqsave(&global_lock_static, flags);
			spin_lock(global_lock_heap);
			incritical++;
			spin_unlock(global_lock_heap);
			spin_unlock_irqrestore(&global_lock_static, flags);
		}
		local_irq_restore(irqflags);
	}

	pr_info("(irqs disabled) CPU%d entered critical section %d times.\n",
		cpu, incritical);
	sync_cpus(IRQS_DISABLED_TEST, cpu);
}

static void spintest_two_lock_single_cacheline(int cpu)
{
	unsigned long flags;
	int incritical = 0;
	ktime_t test_time = ktime_add_us(ktime_get(),
				(inner_test_time_secs * USEC_PER_SEC));
	pr_info("(two-locks): Running test on CPU %d for %d seconds.\n", cpu,
		inner_test_time_secs);

	while (ktime_compare(ktime_get(), test_time) < 0)
	{
		spin_lock_irqsave(&testlocks.lock1, flags);
		spin_lock(&testlocks.lock2);
		incritical++;
		spin_unlock(&testlocks.lock2);
		spin_unlock_irqrestore(&testlocks.lock1, flags);
	}

	pr_info("(two-locks): CPU%d entered critical section %d times.\n",
		cpu, incritical);
	sync_cpus(TWOLOCKS_TEST, cpu);
}

static void spintest_simple(int cpu)
{
	unsigned long flags;
	int incritical = 0;
	ktime_t test_time = ktime_add_us(ktime_get(),
				(inner_test_time_secs * USEC_PER_SEC));
	pr_info("(simple): Running test on CPU %d for %d seconds.\n", cpu,
		inner_test_time_secs);

	while (ktime_compare(ktime_get(), test_time) < 0)
	{
		spin_lock_irqsave(&global_lock_static, flags);
		incritical++;
		spin_unlock_irqrestore(&global_lock_static, flags);
	}

	pr_info("(simple): CPU%d entered critical section %d times.\n",
		cpu, incritical);
	sync_cpus(SIMPLE_TEST, cpu);
}

static void spintest_stack_lock(int cpu)
{
	unsigned long flags;
	int incritical = 0;
	ktime_t test_time = ktime_add_us(ktime_get(),
				(inner_test_time_secs * USEC_PER_SEC));
	spinlock_t stack_lock;
	spin_lock_init(&stack_lock);

	pr_info("(stack-lock): Running test on CPU %d for %d seconds.\n", cpu,
		inner_test_time_secs);

	while (ktime_compare(ktime_get(), test_time) < 0)
	{
		spin_lock_irqsave(&global_lock_static, flags);
		spin_lock(&stack_lock);
		incritical++;
		spin_unlock(&stack_lock);
		spin_unlock_irqrestore(&global_lock_static, flags);
	}

	pr_info("(stack-lock): CPU%d entered critical section %d times.\n",
		cpu, incritical);
	sync_cpus(STACK_LOCK_TEST, cpu);
}

static void spintest_uncached_lock_test(int cpu)
{
	unsigned long flags;
	int incritical = 0;
	ktime_t test_time = ktime_add_us(ktime_get(),
				(inner_test_time_secs * USEC_PER_SEC));
	pr_info("(uncached-lock): Running test on CPU %d for %d seconds.\n", cpu,
		inner_test_time_secs);

	while (ktime_compare(ktime_get(), test_time) < 0)
	{
		spin_lock_irqsave(&global_lock_static, flags);
		spin_lock(uncached_lock);
		incritical++;
		spin_unlock(uncached_lock);
		spin_unlock_irqrestore(&global_lock_static, flags);
	}

	pr_info("(uncached-lock): CPU%d entered critical section %d times.\n",
		cpu, incritical);

	sync_cpus(UNCACHED_LOCK_TEST, cpu);
}

static void spinwork_fn(struct work_struct *work)
{
	struct delayed_work *dwork = (struct delayed_work *)work;
	int cpu = smp_processor_id();

	if (canceltest) {
		pr_err("test cancelled!\n");
		return;
	}

	/*
	 * Start interrupt disabled test. This should provide maximum
	 * contention.
	 */
	irqs_disabled_spintest(cpu);
	pr_info("irqs_disabled_spintest is done (CPU:%d iteration:%d)\n",
		cpu, cpu_start[cpu]);
	msleep(SPINLOCK_TEST_DELAY_MS);

	spintest_simple(cpu);
	pr_info("spintest_simple is done (CPU:%d iteration:%d)\n",
		cpu, cpu_start[cpu]);
	msleep(SPINLOCK_TEST_DELAY_MS);

	/* Use a spinlock allocated on the stack */
	spintest_stack_lock(cpu);
	pr_info("spintest_stack_lock is done (CPU:%d iteration:%d)\n",
		cpu, cpu_start[cpu]);
	msleep(SPINLOCK_TEST_DELAY_MS);

	spintest_two_lock_single_cacheline(cpu);
	pr_info("spintest_two_lock is done (CPU:%d iteration:%d)\n",
		cpu, cpu_start[cpu]);
	msleep(SPINLOCK_TEST_DELAY_MS);

	spintest_uncached_lock_test(cpu);
	pr_info("spintest_uncached_lock_test is done (CPU:%d iteration:%d)\n",
		cpu, cpu_start[cpu]);
	msleep(SPINLOCK_TEST_DELAY_MS);

	if (++cpu_start[cpu] < num_test_iters) {
		schedule_work_on(cpu, &dwork->work);
	}
}

static int spinlock_test_start(void)
{
	int i, flag;
	ktime_t test_time;
	unsigned long total_wait_time;

	start = 1;
	canceltest = 0;
	pr_info("Starting spinlock test\n");

#ifdef CONFIG_HOTPLUG_CPU
	/* Online all the cores */
	for_each_present_cpu(i) {
		if (!cpu_online(i)) {
			cpu_up(i);
		}
	}
#endif

	get_online_cpus();
	for_each_online_cpu(i) {
		cpu_start[i] = 0;
		INIT_DEFERRABLE_WORK(&spinworks[i], spinwork_fn);
		schedule_delayed_work_on(i, &spinworks[i], msecs_to_jiffies(150));
		pr_info("Scheduling spinlock test on cpu %d\n", i);
	}

	/*
	* Estimate and calculate total wait time according to
	*  - number of toal test cases. (e.g irq_disable, stack spinlock, etc..)
	*  - inner test time for each test case
	*  - number of iterations for each test case
	*  - sleep time during two test cases
	*/
	total_wait_time = (NUM_OF_SPINLOCK_TEST + 1) * (inner_test_time_secs * \
		 num_test_iters + SPINLOCK_TEST_DELAY_MS / MSEC_PER_SEC);
	pr_info("Total test time might be ~%lu seconds\n", total_wait_time);

	test_time = ktime_add_us(ktime_get(),
				(total_wait_time * USEC_PER_SEC));
	while (ktime_compare(ktime_get(), test_time) < 0) {
		flag = 0;
		for_each_online_cpu(i) {
			if (cpu_start[i] != num_test_iters) {
				flag = 1;
				break;
			}
		}
		if (!flag) {
			start = 0;
			break;
		}
		msleep(SPINLOCK_TEST_DELAY_MS);
	}

	if (!start) {
		pr_info("spinlock test is done successfully!\n");
		put_online_cpus();
		return 0;
	} else {
		pr_err("spinlock test Failed!\n");
		spinlock_test_stop();
		return -1;
	}
}

static int spinlock_test_stop(void)
{
	int i;
	canceltest = 1;
	if (!start) {
		pr_err("No spinlock test is running. Exitting...\n");
		return -1;
	}
	for(i = 0; i < CPU_NUM_MAX; i++){
		if (spinworks[i].wq != NULL) {
			pr_info("canceling work oncpu %d\n", i);
			cancel_delayed_work_sync(&spinworks[i]);
		}
	}
	/* "start" might be updated by the test work thread */
	if (start)
		put_online_cpus();
	start = 0;
	return 0;
}

static int spinlock_test_remove(struct platform_device *pdev)
{
	/* Cancel the test if it's not done yet */
	if (start)
		spinlock_test_stop();

	if (dent) {
		debugfs_remove_recursive(dent);
		dent = NULL;
	}
	if (global_lock_heap) {
		kfree(global_lock_heap);
		global_lock_heap = NULL;
	}

	if (uncached_lock) {
		dma_free_coherent(&pdev->dev, sizeof(spinlock_t), uncached_lock, phys);
		uncached_lock = NULL;
	}

	return 0;
}

static int spinlock_test_probe(struct platform_device *pdev)
{
	int ret = 0;
	if (num_possible_cpus() > CPU_NUM_MAX) {
		dev_err(&pdev->dev, "Number of possible cpus on this target \
			exceeds the limit %d\n", CPU_NUM_MAX);
		return -EINVAL;
	}

	global_lock_heap = kzalloc(sizeof(spinlock_t), GFP_KERNEL);
	if (!global_lock_heap) {
		dev_err(&pdev->dev, "unable to alloc memory for global lock\n");
		return -ENOMEM;
	}

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(3,18,0))
	arch_setup_dma_ops(&pdev->dev, 0, U64_MAX, NULL, 0);
#endif

	dma_set_coherent_mask(&pdev->dev, DMA_BIT_MASK(64));
	uncached_lock = dma_alloc_coherent(&pdev->dev, sizeof(spinlock_t), &phys, GFP_KERNEL);
	if (!uncached_lock) {
		dev_err(&pdev->dev ,"Failed to allocate Uncached memory for lock\n");
		kfree(global_lock_heap);
		return -ENOMEM;
	}

	spin_lock_init(&testlocks.lock1);
	spin_lock_init(&testlocks.lock2);
	spin_lock_init(global_lock_heap);
	spin_lock_init(uncached_lock);

	ret = creat_spinlock_debugfs();
	if (ret) {
		dev_err(&pdev->dev, "create spinlock debugfs failed!\n");
		kfree(global_lock_heap);
		dma_free_coherent(&pdev->dev, sizeof(spinlock_t), uncached_lock, phys);
		uncached_lock = NULL;
		return ret;
	}
	return 0;
}

static struct platform_driver spinlock_test_driver = {
	.probe = spinlock_test_probe,
	.remove = spinlock_test_remove,
	.driver = {
		.name = MODULE_NAME,
		.owner = THIS_MODULE,
	},
};

static void platform_spinlock_release(struct device* dev)
{
	return;
}

static struct platform_device spinlock_test_device = {
	.name = MODULE_NAME,
	.id   = -1,
	.dev  = {
		.release = platform_spinlock_release,
	}
};

static int __init  spinlock_test_init(void)
{
	platform_device_register(&spinlock_test_device);
	return platform_driver_register(&spinlock_test_driver);
}

static void __exit spinlock_test_exit(void)
{
	platform_driver_unregister(&spinlock_test_driver);
	platform_device_unregister(&spinlock_test_device);
}

MODULE_DESCRIPTION("SPINLOCK TEST");
MODULE_LICENSE("GPL v2");
module_init(spinlock_test_init);
module_exit(spinlock_test_exit);
