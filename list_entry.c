#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/sched.h>  // for task_struct
#include <asm/current.h>

MODULE_LICENSE("Dual BSD/GPL");

int __init hello_init(void)
{
	struct task_struct *task = current;
	struct task_struct *next = list_entry(task->tasks.next, struct task_struct, tasks);
	printk("Process %d : %s has %d threads\n", next->pid, next->comm, get_nr_threads(next));

}

static __exit void hello_exit(void)
{
	printk("EXITING THREAD TREE MODULE\n");
}

module_init(hello_init);
module_exit(hello_exit);