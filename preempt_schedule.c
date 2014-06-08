#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/sched.h>
#include <linux/spinlock.h>

DEFINE_SPINLOCK(my_lock);

/* Just trying to see if scheduling out during preempt disable or while holding
 * a lock produces an oops or warning
 */

static int my_func(void)
{
	pr_info("PREEMPT DISABLE\n");
	preempt_disable();
	pr_info("SCHEDULING OUT\n");
	schedule();
	pr_info("SCHEDULING IN\n");
	preempt_enable();
	pr_info("PREEMPT ENABLE\n");

	pr_info("SPINLOCK\n");
	spin_lock(&my_lock);
	pr_info("SCHEDULING OUT 2\n");
	schedule();
	pr_info("SCHEDULING IN 2\n");
	pr_info("UNLOCK\n");
	spin_unlock(&my_lock);

	return 0;
}

int init_func(void)
{
	my_func();
	return 0;
}

void exit_func(void)
{
	return;
}

module_init(init_func);
module_exit(exit_func);
MODULE_LICENSE("GPL");
MODULE_AUTHOR("ritsz");
