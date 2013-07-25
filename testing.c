#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/sched.h>
#include <linux/unistd.h>
#include <asm/current.h>
#include <linux/pid.h>

MODULE_LICENSE("Dual BSD/GPL");

static int hello_init(void)
{
	printk("Hello World!\n");
	getpid();
	return 0;
}

static void hello_exit(void)
{
	printk("Good bye!\n");
}

module_init(hello_init);
module_exit(hello_exit);