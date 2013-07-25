#include <linux/module.h>
#include <linux/kernel.h>

#include <linux/kthread.h>  // for threads
#include <linux/sched.h>  // for task_struct
#include <linux/fs.h>
#include <linux/list.h>

MODULE_LICENSE("Dual BSD/GPL");

int hello_init(void)
{
	struct task_struct *task, *g, *t;
	printk("ENTERING THREAD TREE MODULE\n");	
	// for_each_process(task) {
	// 	printk("Process : %s has %d threads\n", task->comm, get_nr_threads(task));
	// }
	do_each_thread(g, t) {
		printk("%s and %s\n", g->comm, t->comm);
	} while(0);
	return 0;
}

static void hello_exit(void)
{
	printk("EXITING THREAD TREE MODULE\n");
}

module_init(hello_init);
module_exit(hello_exit);