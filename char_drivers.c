#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/types.h>
#include <linux/kdev_t.h>
#include <linux/fs.h>
#include <linux/device.h>
#include <linux/errno.h>
#include <linux/cdev.h>

MODULE_LICENSE("GPL");

static dev_t first_dev;
static struct class *cl;
static struct device* dev;
static struct cdev c_dev;

void chrdev_exit(void)
{
	if (dev) {
		printk(KERN_INFO "Destroying Device\n");
		device_destroy(cl, first_dev);
	}
	if (cl) {
		printk(KERN_INFO "Destroying Class\n");
		class_destroy(cl);
	}
	unregister_chrdev_region(first_dev, 1);
	printk(KERN_INFO "Unregistering char device\n");
}

static int my_open(struct inode *i, struct file *f)
{
  	printk(KERN_INFO "Driver: open()\n");
  	return 0;
}
  
static int my_close(struct inode *i, struct file *f)
{
	printk(KERN_INFO "Driver: close()\n");
  	return 0;
}
  
static ssize_t my_read(struct file *f, char __user *buf, size_t len, loff_t *off)
{
  	printk(KERN_INFO "Driver: read()\n");
  	return 0;
}

static ssize_t my_write(struct file *f, const char __user *buf, size_t len, loff_t *off)
{
  	printk(KERN_INFO "Driver: write()\n");
  	return len;
}
  

static struct file_operations fops =
{
  	.owner = THIS_MODULE,
  	.open = my_open,
  	.release = my_close,
  	.read = my_read,
  	.write = my_write
};


static __init int chrdev_init(void)
{
	// Register the major:minor number
	printk(KERN_INFO "Registering char device\n");
	if (alloc_chrdev_region(&first_dev, 0, 1, "ritsz") < 0)
		return -ENOMEM;

	printk(KERN_INFO "<Major, Minor>: <%d, %d>\n", MAJOR(first_dev), MINOR(first_dev));

	// Create a device class in the /sys directory
	if ((cl = class_create(THIS_MODULE, "chardrv")) == NULL)
  		goto fail;
  	else printk(KERN_INFO "Class Created\n");

  	// Create a device in /sys/<class>/<device> and /dev/<device>
  	if ((dev = device_create(cl, NULL, first_dev, NULL, "mynull")) == NULL)
	  	goto fail;
	else  printk(KERN_INFO "Device Created\n");

	// Add file operations to the Character Device File
	cdev_init(&c_dev, &fops);
	if (cdev_add(&c_dev, first_dev, 1) == -1)
		goto fail;

	return 0;

	fail:
		chrdev_exit();
		return -1;
}


module_init(chrdev_init);
module_exit(chrdev_exit);
