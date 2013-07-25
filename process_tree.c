#include <linux/module.h>
#include <linux/kernel.h>

#include <linux/kthread.h>  // for threads
#include <linux/sched.h>  // for task_struct
#include <linux/time.h>   // for using jiffies  
#include <linux/timer.h>
#include <linux/fs.h>
#include <linux/list.h>
#include <linux/slab.h>
#include <linux/proc_fs.h>
#include <asm/current.h>

MODULE_LICENSE("Dual BSD/GPL");

static int rday_1 = 1;
int rday_2 = 2;
int rday_3 = 3;

EXPORT_SYMBOL(rday_3);

struct kool_list {
	int to;
	struct list_head list;
	struct list_head list2;
	int from;
};

int thread_fn() 
{
	printk(KERN_INFO "In thread 1.0");
	return 0;
}

static int hello_init(void)
{
static struct task_struct *thread1;
char our_thread[8]="thread1";
//do_fork(CLONE_VM|CLONE_UNTRACED, (unsigned long)fn, 0, , 0,0);
printk("Hello World!\n");

thread1 = kthread_create(thread_fn,NULL,our_thread);
set_task_state(thread1, __TASK_STOPPED);

//  for_each_process(task) {
// printk("%s %d\n", task->comm, task->pid);
// printk("<1>%s has %d thread(s)\n", task->comm, get_nr_threads(task));
// }
//  printk("\n\n\n");
//  for(task = &init_task; ((task=next_task(task)) != &init_task) ; )
// printk("%s %d\n", task->comm, task->pid);
//  printk("\n\n\n");
// struct inode ino;
// struct file fl;

	struct kool_list *tmp;
	struct list_head *pos, *q;
	unsigned int i;

	struct kool_list mylist;
	INIT_LIST_HEAD(&mylist.list);

	for(i=5; i!=0; --i) {
		tmp= (struct kool_list *)kmalloc(sizeof(struct kool_list), GFP_KERNEL);
		tmp->to = 10 + i;
		tmp->from = 20*i;
		////////////////////////////////////////////
		__list_add(&((*tmp).list), &(mylist.list), mylist.list.next);
		//////////////////////////////////////////
	}

	/////////////////////////////////////////////
	list_for_each(pos, &mylist.list){
	////////////////////////////////////////////
	
	///////////////////////////////////////////	
		tmp= ({ const typeof( ((struct kool_list *)0)->list ) *__mptr = (pos); (struct kool_list *)( (char *)__mptr - offsetof(struct kool_list,list) );});
	//////////////////////////////////////////	
		printk("to= %d from= %d\n", tmp->to, tmp->from);
	}

	struct proc_dir_entry *file;

	const char* name = "noname";

	file = create_proc_entry(name, 0666, NULL);

	printk("Current %d : %s \n", current->pid, current->comm);

	if((thread1))
        {
        printk(KERN_INFO "in if");
        wake_up_process(thread1);
        }

 return 0;
}

static void hello_exit(void)
{
	remove_proc_entry("noname", NULL);
	printk("Good bye!\n");
}

module_init(hello_init);
module_exit(hello_exit);