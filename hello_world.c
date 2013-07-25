#include <linux/init.h>
#include <linux/module.h>
#include <linux/sched.h>
#include <asm/current.h>

MODULE_LICENSE("Dual BSD/GPL");

static int hello_init()
{
	printk("<1>HELLO WORLD!! THIS IS RITSZ %s ---- %i \n",current->comm,current->pid);
	return 0;
}

static void hello_exit()
{
	printk("<1>GOODBYE!! CRUEL WORLD %s ---- %i \n", current->comm,current->pid);
}

module_init(hello_init);
module_exit(hello_exit);
