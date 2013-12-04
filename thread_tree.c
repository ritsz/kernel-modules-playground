#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/sched.h>  // for task_struct
#include <linux/kthread.h>  // for threads
#include <asm/current.h>
//#include <asm/page.h>

MODULE_LICENSE("Dual BSD/GPL");

void my_kernel_fn() 
{
	// When the kernel thread runs, current refers to kritsz
	printk("WE ARE RUNNING A KERNEL THREAD %s\n\n", current->comm);
}

int __init hello_init(void)
{
	struct task_struct *task, *t;
	printk("ENTERING THREAD TREE MODULE\n");	
	
	//Lets create a new kernel thread called kritsz
	t = kthread_create(my_kernel_fn, NULL, "kritsz");


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


	//	This method prints data about init_task as well, since it's a do-while loop

	task = &init_task;
	do {
		// A check to see whether void *stack points to the thread_info structure or not.
		struct thread_info * ti = (struct thread_info*)(task->stack);
		struct task_struct * _task = ti->task;
		// A check to see if each task has a separate stack area.
		printk("First Check Address %p\n", ti); 
		printk("Check :: %d, %s\n", _task->pid, _task->comm);

		printk("Process %d : %s has %d threads\n", task->pid, task->comm, get_nr_threads(task));
		printk("%uKB Kernel Stack size, %lu time, %llu wait, %llu last run and %llu last queued\n", THREAD_SIZE/1024, task->sched_info.pcount, task->sched_info.run_delay,
	 			task->sched_info.last_arrival, task->sched_info.last_queued);
		printk("%lu\n",task->rt.timeout);


		// A kernel thread has no address space.
		if (task->mm == NULL) {
			printk("KERNEL THREAD\n\n");
		}
		else {
			printk("USER THREAD\n\n");
		}
		

		// If task is kthreadadd, traverse all children to find out all the kernel threads.

		if (task->pid == 2) {
			struct task_struct *child_task;
			// Iterate task->children list of 'kthreadadd', where elements are related as siblings
			list_for_each_entry(child_task, &(task->children), sibling) {
				printk("\t\t %d 	:	%s\n",child_task->pid, child_task->comm);
			}
		}

		task = next_task(task);
	} while (task != &init_task);


	// Lets wake up the kritsz kernel thread
	wake_up_process(t);

	return 0;
}

static __exit void hello_exit(void)
{
	printk("EXITING THREAD TREE MODULE\n");
}

module_init(hello_init);
module_exit(hello_exit);
