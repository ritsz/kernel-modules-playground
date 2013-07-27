#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/sched.h>  // for task_struct

MODULE_LICENSE("Dual BSD/GPL");

int __init hello_init(void)
{
	struct task_struct *task, *g, *t;
	printk("ENTERING THREAD TREE MODULE\n");	
	

	// 	Printing process data using for_each_process requires printing init_task separately

	#if 0
	//	Information about the swapper(IDLE)
	printk("Process %d : %s has %d threads\n", init_task.pid, init_task.comm, get_nr_threads(&init_task));
	printk("%lu time, %llu wait, %llu last run and %llu last queued\n\n", init_task.sched_info.pcount, init_task.sched_info.run_delay,
			init_task.sched_info.last_arrival, init_task.sched_info.last_queued);
	
	//	Shouldn't be used too often, takes too long.
	//	Prints the name, no. of threads and scheduling info of all process
	//	using the list_head list.

	for_each_process(task) {
		printk("Process %d : %s has %d threads\n", task->pid, task->comm, get_nr_threads(task));
		printk("%lu time, %llu wait, %llu last run and %llu last queued\n\n", task->sched_info.pcount, task->sched_info.run_delay,
				task->sched_info.last_arrival, task->sched_info.last_queued);			
	}

	// 	Yet another way to traverse the processes.
	
	do_each_thread(g, t) {
		printk("%s and %s\n", g->comm, t->comm);
	} while(0);

	#endif


	//	This method prints data about init_task as well, since it's a do-while loop :P

	task = &init_task;
	do {
		printk("Process %d : %s has %d threads\n", task->pid, task->comm, get_nr_threads(task));
		printk("%lu time, %llu wait, %llu last run and %llu last queued\n\n", task->sched_info.pcount, task->sched_info.run_delay,
	 			task->sched_info.last_arrival, task->sched_info.last_queued);
		task = next_task(task);
	} while (task != &init_task);

	return 0;
}

static __exit void hello_exit(void)
{
	printk("EXITING THREAD TREE MODULE\n");
}

module_init(hello_init);
module_exit(hello_exit);
