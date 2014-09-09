#include <linux/kernel.h>
#include <linux/module.h>
#include <asm/current.h>
#include <linux/sched.h>
#include <linux/cgroup.h>
#include <linux/rcupdate.h>

static struct css_set *my_css_set;

static int my_cgroup_init (void)
{
	struct task_struct *task = current;
	struct task_struct *iterator;
	struct cgroup *my_cgroup;
	struct cgroup_subsys *my_cs;
	int curr_pid = task->pid;
	int refcnt, count = 0;


	pr_info("We are working for [PID %d] Process : %s\n", curr_pid, task->comm); 

	/* The task_struct structure has a struct css_set pointer called cgroups
	 * which is a structure containing a set of cgroup_subsys_state objects,
	 * one for each subsystem. The css_set is managed by an rcu lock and
	 * thus should be derefenced accordingly.
	 */
	rcu_read_lock();
	my_css_set = rcu_dereference(task->cgroups);
	rcu_read_unlock();
	
	pr_info("The number of tasks in this css_set is %d\n", atomic_read(&(my_css_set->refcount)));
	refcnt = atomic_read(&(my_css_set->refcount));

	list_for_each_entry(iterator, &(my_css_set->tasks), cg_list) {
		pr_info("\t[PID %d] Process : %s part of the cgroup\n", iterator->pid, iterator->comm);
	}

	/* The cgroup_subsys_state contains a pointer to the associated
	 * cgroup and the cgroup_subsys structure associated with it. There is
	 * one css pointer for each subsystem like cpuset, cpuacct, memory,
	 * devices etc. 
	 */
	pr_info("We'll go thorough all %d subsystems in this cgroup\n", CGROUP_SUBSYS_COUNT);
	while(count != CGROUP_SUBSYS_COUNT) {
		my_cgroup = my_css_set->subsys[count]->cgroup;
		my_cs = my_css_set->subsys[count]->ss;
		if (!my_cgroup) {
			pr_err("No cgroup found\n");
			return -ENOMEM;
		}
	
		pr_info("\t\tThe reference count of [%s] cgroup is %d\n", my_cs->name, atomic_read(&(my_cgroup->refcnt)));
		count++;
	}
	

	return 0;
}

static void my_cgroup_exit (void)
{
	return;
}

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Ritesh Ranjan");
MODULE_DESCRIPTION("A simple module to print the cgroups the task is attached to.");
module_init(my_cgroup_init);
module_exit(my_cgroup_exit);
