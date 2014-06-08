#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/device.h>
#include <linux/uaccess.h>
#include <linux/fs.h>
#include <linux/miscdevice.h>
#include <linux/spinlock.h>

#define MAXBUFF 128
DEFINE_RWLOCK(my_rwlock);
static char kern_buff[MAXBUFF];		/*Kernel Buffer to hold user data*/
static char my_eud_id[12] = "HELLO WORLD\0"; /*string*/
static ssize_t my_read(struct file *f, char __user *buf,
		size_t len, loff_t *off)
{
	int ret;
	dump_stack();
	read_lock(&my_rwlock);
	ret = simple_read_from_buffer(buf, len, off, my_eud_id, strlen(my_eud_id));
	read_unlock(&my_rwlock);
	return ret;
}

static ssize_t my_write(struct file *f, const char __user *buf,
		    size_t len, loff_t *off)
{
	dump_stack();
	write_lock(&my_rwlock);
	int ret = simple_write_to_buffer(kern_buff, MAXBUFF, off, buf, len);
	kern_buff[ret] = '\0';
	write_unlock(&my_rwlock);

	if (strcmp(kern_buff, "linux") != 0)
		return -EINVAL;		/* Return Invalid Error for String*/
	else
		return ret;		/* Return Data length upon Success*/
}

static const struct file_operations misc_fops = {
	.owner = THIS_MODULE,
	.read = my_read,
	.write = my_write
};

static struct miscdevice misc_dev = {
	.minor = MISC_DYNAMIC_MINOR,
	.name = "ritsz",
	.fops = &misc_fops
};

static int __init task1_init(void)
{
	return misc_register(&misc_dev);
}

static void __exit task1_exit(void)
{
	misc_deregister(&misc_dev);
}

module_init(task1_init);
module_exit(task1_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Ritesh Ranjan");
MODULE_DESCRIPTION("A simple misc char device driver");
