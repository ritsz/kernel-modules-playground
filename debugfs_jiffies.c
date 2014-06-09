#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/debugfs.h>
#include <linux/jiffies.h>

static struct dentry *dir;

int jiffies_init(void)
{
	/* Creating a directory called my_time under the root directory of debugfs
	 */
	dir = debugfs_create_dir("my_time", NULL);
	if (!dir)
		return -ENODEV;
	
	/*creating a u64 single integer file that will export the value of the jiffies
	  timer with the user space.
	 */

	if (!debugfs_create_u64("time", 0444, dir, &jiffies))
		return -1; 

	return 0;
}

void jiffies_exit(void)
{
	debugfs_remove_recursive(dir);
}

module_init(jiffies_init);
module_exit(jiffies_exit);
MODULE_LICENSE("GPL");
MODULE_AUTHOR("ritsz");
