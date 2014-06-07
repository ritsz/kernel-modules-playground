#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/proc_fs.h>
#include <asm/uaccess.h>
#include <linux/seq_file.h>
#include <linux/slab.h>

/* proc_mkdir -> to create directory
 * proc_create_data -> make a proc file, add file_operations callbacks
 * proc_remove_entry -> remove directory/file
 */

static void *my_start(struct seq_file *s, loff_t *pos)
{
	pr_info("START\n");
	loff_t *spos = kmalloc(sizeof(loff_t), GFP_KERNEL);
	if (!spos)
		return NULL;
	*spos = *pos;
	return spos;
}

static void *my_next(struct seq_file *s, void *v, loff_t *pos)
{
	pr_info("NEXT\n");
	loff_t *spos = (loff_t *)v;
	*pos = ++(*spos);
	return spos;
}

static void my_stop(struct seq_file *s, void *v)
{
	kfree(v);
}

static int my_show(struct seq_file *s, void *v)
{
	loff_t *spos = (loff_t *) v;
	seq_printf(s, "%Ld\n", *spos);
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
	/* Effectively, everything is similar to the misc_char_device file that
	 * we created, only this time, file gets created in /proc instead of
	 * /dev. To create the files in the procfs filesystem we use the procfs
	 * API such as proc_mkdir, proc_create_data and proc_remove_entry. The
	 * legacy procfs API such as create_proc_entry have been depricated
	 * since 3.10
	 */
	example_dir = proc_mkdir("example", NULL);
        if(example_dir == NULL)
                return -ENOMEM;

	first_file = proc_create_data("first", 0644, example_dir, &fops, NULL);
        if(first_file == NULL) {
		remove_proc_entry("example", NULL);
		return -ENOMEM;
	}
        return 0;
}

static void __exit procexample_module_exit(void)
{
	remove_proc_entry("first", example_dir);
	remove_proc_entry("example", NULL);
}

module_init(procexample_module_init);
module_exit(procexample_module_exit);
MODULE_LICENSE("GPL");
