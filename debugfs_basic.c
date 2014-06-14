#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/debugfs.h>

static struct dentry *dir = 0;
static u32 sum = 0;

static int add_write(void *data, u64 value)
{
    sum += value;
	return 0;
}

DEFINE_SIMPLE_ATTRIBUTE(add_fops, NULL, add_write, "%llu\n");
/* DEFINE_SIMPLE_ATTRIBUTE is a macro that can be used to populate the add_fops
 * structure. Source code for this function is available in
 * include/linux/fs.h:DEFINE_SIMPLE_ATTRIBUTE.

static struct file_operations add_fops = {
	.owner = THIS_MODULE,
	.read = NULL,
	.write = add_write,
};
*/

int __init my_debugfs_init(void)
{
	struct dentry *holder;
	/* create a directory under the debugfs root directory. debugfs is
	 * loaded at /sys/kernel/debug in most distros
	 */
	dir = debugfs_create_dir("basics", NULL);
	if (!dir) {
		pr_err("debugfs failed to create directory\n");
		return -1;
	}

	holder = debugfs_create_file("add", 0222, dir, NULL, &add_fops);
	if (!holder) {
		pr_err("debugfs failed to create file 'add'\n");
		return -1;
	}

	/* This function creates a file called sum that will share the value of
	 * sum variable to user space. Thus everytime
	 * /sys/kernel/debug/basics/sum is read from, it will output the value
	 * of sum variable 
	 */
	holder = debugfs_create_u32("sum", 0644, dir, &sum);
	if (!holder) {
		pr_err("debugfs failed to create file 'sum'\n");
		return -1;	
	}
	return 0;
}

void module_cleanup(void)
{
	debugfs_remove_recursive(dir);
}

module_init(my_debugfs_init);
module_exit(module_cleanup);
MODULE_LICENSE("GPL");
MODULE_AUTHOR("ritsz");
