#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/stat.h>			//Permissions for sysfs

MODULE_LICENSE("GPL");

static char *name = "ritsz";
module_param(name, charp, S_IRUSR|S_IWUSR);	//name can be changed by echoing into /sys/modules/<>/parameters/name

static void load_clean(void)
{
	printk("2. Name is : %s\n", name);
	printk("Unloading %s module\n", __this_module.name);
}


static __init load_init(void) 
{
	printk("1. Name is : %s\n", name);
	return 0;
}

module_init(load_init);
module_exit(load_clean);
