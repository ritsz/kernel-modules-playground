#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/sched.h>
#include <linux/seq_file.h>
#include <linux/slab.h>
#include <linux/proc_fs.h>
#include <asm/uaccess.h>
#include <asm/current.h>

/* proc_mkdir -> to create directory
 * proc_create_data -> make a proc file, add file_operations callbacks
 * proc_remove_entry -> remove directory/file
 * To the file operations structure we add generic seq opetaions that are
 * reusable and only open() fops needs to change. The open function calls
 * seq_open, which adds a seq_operations structure to the file. The
 * seq_operations structure needs to be given the start, next, stop and show
 * functions that will be used to iterate over the list that has to be printed.
 */

/* We would like to iterate over the task_struct list and print name and pids */
static void *my_start(struct seq_file *s, loff_t *pos)
{
	/* We just return the pointer to the list_head tasks structure for current
	 * process.infinite loop. The structure is available to other functions 
	 * via the argument void *v
	 */
	pr_info("START\n");
	return &(current->tasks);
}

static void *my_next(struct seq_file *s, void *v, loff_t *pos)
{
	/* void *v contains the pointer from start() or previous call of next.
	 * We need to type cast it to the correct data structure and return the
	 * next pointer as well
	 */
	pr_info("NEXT\n");
	struct list_head *ptr = ((struct list_head *) v)->next;
	return ptr;
}

static void my_stop(struct seq_file *s, void *v)
{
}

static int my_show(struct seq_file *s, void *v)
{
	/* typecast void *v into list_head pointer. Find container of that
	 * pointer, which will be a pointer to the task_struct structure of that
	 * particular task. Derefference and print the task's comm and pid.
	 */
	struct list_head *ptr = ((struct list_head *) v)->next;
	struct task_struct *task = container_of(ptr, struct task_struct, tasks);
	seq_printf(s, "%u\t:\t%s\n", task->pid, task->comm);
	return 0;
}

static struct seq_operations swaps_op = {
	.start =        my_start,
	.next =         my_next,
	.stop =        	my_stop,
	.show =         my_show
};

static int my_open(struct inode *inode, struct file *file)
{
	return seq_open(file, &swaps_op);
}

/* The file_operations structure needs only the open function, all other
 * functions like read, seek and release are provided by seq_file.h, and
 * seq_file cannot be written to.
 */

static struct file_operations fops = {
	.owner = THIS_MODULE,
	.open = my_open,
	.read = seq_read,
	.llseek = seq_lseek,
	.release = seq_release,
};

static struct proc_dir_entry *example_dir, *first_file;

static int __init procexample_module_init(void)
{
	/* Instead of procfs API, we are using seq_file API to read data
	 * from proc file. This allows data more than PAGE_SIZE to be read.
	 * Seq_file can't be written to by user spcae.
	 */
	example_dir = proc_mkdir("example", NULL);
        if(example_dir == NULL)
                return -ENOMEM;

	first_file = proc_create_data("seq_proc", 0644, example_dir, &fops, NULL);
        if(first_file == NULL) {
		remove_proc_entry("example", NULL);
		return -ENOMEM;
	}
        return 0;
}

static void __exit procexample_module_exit(void)
{
	remove_proc_entry("seq_proc", example_dir);
	remove_proc_entry("example", NULL);
}

module_init(procexample_module_init);
module_exit(procexample_module_exit);
MODULE_LICENSE("GPL");
