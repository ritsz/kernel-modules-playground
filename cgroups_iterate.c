#include <linux/kernel.h>
#include <linux/module.h>
#include <asm/current.h>
#include <linux/sched.h>
#include <linux/cgroup.h>

static struct css_set *my_css_set;

static int my_cgroup_init (void)
{
	struct task_struct *task = current;
	struct task_struct *iterator;
	int count = 0;
	int curr_pid = task->pid;


	pr_info("We are working for [PID %d] Process : %s\n", curr_pid, task->comm); 

	my_css_set = task->cgroups;
	

	list_for_each_entry(iterator, &(my_css_set->tasks), cg_list) {
		pr_info("\t[PID %d] Process : %s part of the cgroup\n", iterator->pid, iterator->comm);
		count++;
		if (count > 10)
			break;
	}

	return 0;
}

static void my_cgroup_exit (void)
{
	return;
}

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Ritesh Ranjan");
module_init(my_cgroup_init);
module_exit(my_cgroup_exit);
