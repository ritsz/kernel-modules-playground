#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/sched.h>  // for task_struct

MODULE_LICENSE("Dual BSD/GPL");

int hello_init(void)
{
	struct task_struct *task, *g, *t;
	printk("ENTERING THREAD TREE MODULE\n");	
	
	// 	Shouldn't be used too often, takes too long.
	//	Prints the name, no. of threads and scheduling info of all process
	// 	using the list_head list.
	for_each_process(task) {
		printk("Process : %s has %d threads\n", task->comm, get_nr_threads(task));
		printk("%lu time, %llu wait, %llu last run and %llu last queued\n\n", task->sched_info.pcount, task->sched_info.run_delay,
				task->sched_info.last_arrival, task->sched_info.last_queued);
	}

	// 	Yet another way to traverse the processes.
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
