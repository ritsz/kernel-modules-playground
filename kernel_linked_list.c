#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/sched.h>  // for task_struct
#include <linux/list.h>
#include <linux/slab.h>

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
struct kool_list mylist;

static int hello_init (void) 
{
	printk(KERN_INFO "Hello World!\n");

	struct kool_list *tmp;
	struct list_head *pos, *q;
	unsigned int i;

	INIT_LIST_HEAD(&mylist.list);	// Adding Head Node

	for(i=5; i!=0; --i) {
		// Data is kernel should always be dynamic. Stack size is very small.
		tmp= (struct kool_list *)kmalloc(sizeof(struct kool_list), GFP_KERNEL);
		tmp->to = 10 + i;
		tmp->from = 20*i;
		
		// Adding Additional nodes (at head)
		__list_add(&((*tmp).list), &(mylist.list), mylist.list.next);
		
	}

	// Iterate over list
	list_for_each(pos, &mylist.list) {
	
	// Extended form of the macro "container_of" and "List_Entry"
		tmp= ({ const typeof( ((struct kool_list *)0)->list ) *__mptr = (pos); (struct kool_list *)( (char *)__mptr - offsetof(struct kool_list,list) );});
		printk(KERN_INFO "to= %d from= %d\n", tmp->to, tmp->from);
	}

	return 0;
}

static void hello_exit(void)
{
	printk(KERN_INFO "Good bye!\n");
	list_del(&mylist.list);	
}

module_init(hello_init);
module_exit(hello_exit);
