#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/proc_fs.h>
#include <asm/uaccess.h>

/* proc_mkdir -> to create directory
 * proc_create_data -> make a proc file, add file_operations callbacks
 * proc_remove_entry -> remove directory/file
 */

#define MAXBUFF 128
static char kern_buff[MAXBUFF];         /*Kernel Buffer to hold user data*/
static char my_eud_id[12] = "HELLO WORLD\0"; /*string*/
static ssize_t my_read(struct file *f, char __user *buf,
                size_t len, loff_t *off)
{
        dump_stack();
        return simple_read_from_buffer(buf, len, off, my_eud_id, strlen(my_eud_id));
}

static ssize_t my_write(struct file *f, const char __user *buf,
                    size_t len, loff_t *off)
{
        dump_stack();
        int ret = simple_write_to_buffer(kern_buff, MAXBUFF, off, buf, len);
        kern_buff[ret] = '\0';

        if (strcmp(kern_buff, "linux") != 0)
                return -EINVAL;         /* Return Invalid Error for String*/
        else
                return ret;             /* Return Data length upon Success*/
}

static struct file_operations fops = {
	.owner = THIS_MODULE,
	.read = my_read,
	.write = my_write,
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

	/* Another method of doing this would be to use create_proc_entry as:
	 * 	first_file = proc_create_entry("first", 0644, example_dir);
	 * 	first_file->proc_fops = &fops;
	 */

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
