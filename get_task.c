#include <linux/init.h>
#include <linux/module.h>
#include <linux/sched.h>
#include <asm/current.h>

MODULE_LICENSE("Dual BSD/GPL");

static int hello_init()
{
	struct sched_param sp;
	sp.sched_priority = 1;
	sched_setscheduler (0, SCHED_RR, &sp);
	printk("<1>HELLO WORLD!! THIS IS RITSZ %s ---- %i, State : %ld \n", current->comm, current->tgid, current->state);
	printk("<1>SWITCH COUNT : %c", current->fpu_counter);
	printk("<1> LAST ARRIVAL : %Lu and LAST QUEUED : %Lu. Pushable : %d\n", current->sched_info.last_arrival, current->sched_info.last_queued, current->pushable_tasks.prio);
	printk("<1>SWITCH COUNT : %c", current->fpu_counter); 
    printk("<1> LAST ARRIVAL : %Lu and LAST QUEUED : %Lu. Pushable : %d\n", current->sched_info.last_arrival, current->sched_info.last_queued, current->pushable_tasks.prio);	printk("<1>SWITCH COUNT : %c", current->fpu_counter);
	printk("<1>SWITCH COUNT : %c", current->fpu_counter); 
	printk("<1> LAST ARRIVAL : %Lu and LAST QUEUED : %Lu. Pushable : %d\n", current->sched_info.last_arrival, current->sched_info.last_queued, current->pushable_tasks.prio);
	return 0;
}

static void hello_exit()
{
	printk("<1>GOODBYE!! CRUEL WORLD %s ---- %i, Run_delay : %Lu \n", current->comm,current->pid, current->sched_info.run_delay);
	printk("<1> LAST ARRIVAL : %Lu and LAST QUEUED : %Lu. Pushable : %d\n", current->sched_info.last_arrival, current->sched_info.last_queued, current->pushable_tasks.prio);
	printk("<1>SWITCH COUNT : %c", current->fpu_counter);	
}

module_init(hello_init);
module_exit(hello_exit);