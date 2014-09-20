#include <linux/kobject.h>
#include <linux/string.h>
#include <linux/sysfs.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/jiffies.h>
#include <linux/spinlock.h>
#include <linux/slab.h>

static char kern_buff[128];     /*Kernel Buffer to hold user data*/
static char my_eud_id[13] = "HELLO  WORLD\n"; /*My my_sysfs ID*/
static char *my_page_buffer;	/* Buffer to hold foo's value */

DEFINE_RWLOCK(my_rwlock);

static ssize_t id_show(struct kobject *kobj, struct kobj_attribute *attr,
                        char *buf)
{
        return sprintf(buf, "%s\n", my_eud_id);
}

static ssize_t id_store(struct kobject *kobj, struct kobj_attribute *attr,
                         const char *buf, size_t count)
{
        sscanf(buf, "%s", kern_buff);
        pr_info("%s string from userspace\n", kern_buff);

        if (strcmp(kern_buff, "ritsz") != 0)
                return -EINVAL;         /* Return Invalid Error for String Mismatch*/
        else
                return count;
}

static struct kobj_attribute id_attribute =
        __ATTR(id, 0666, id_show, id_store);

static ssize_t jiffies_show(struct kobject *kobj, struct kobj_attribute *attr,
                        char *buf)
{
	return sprintf(buf, "%lu\n", jiffies);
}

static struct kobj_attribute jiffies_attribute =
        __ATTR(jiffies, 0444, jiffies_show, NULL);

static ssize_t foo_show(struct kobject *kobj, struct kobj_attribute *attr,
                        char *buf)
{
	int retval;
	read_lock(&my_rwlock);
	retval = sprintf(buf, "%s\n", my_page_buffer);
	read_unlock(&my_rwlock);
	return retval;
}

static ssize_t foo_store(struct kobject *kobj, struct kobj_attribute *attr,
                         const char *buf, size_t count)
{
	write_lock(&my_rwlock);
	strncpy(my_page_buffer, buf, count);
	pr_info("GOT %s from USER\n", my_page_buffer);
	write_unlock(&my_rwlock);
	return count;
}

static struct kobj_attribute foo_attribute =
        __ATTR(foo, 0644, foo_show, foo_store);

static struct attribute *attrs[] = {
       	&id_attribute.attr,
	&jiffies_attribute.attr,
	&foo_attribute.attr,
        NULL,
};

static struct attribute_group attr_group = {
        .attrs = attrs,
};

static struct kobject *my_sysfs_kobj;

static int __init my_sysfs_init (void)
{
	int retval;
	
	my_page_buffer = (char *)kmalloc(PAGE_SIZE, GFP_KERNEL);
	if (!my_page_buffer)
		return -ENOMEM;

	my_sysfs_kobj = kobject_create_and_add("my_sysfs", kernel_kobj);
        if (!my_sysfs_kobj)
                return -ENOMEM;

        /* Create the files associated with this kobject */
        retval = sysfs_create_group(my_sysfs_kobj, &attr_group);
        if (retval)
                kobject_put(my_sysfs_kobj);

        return retval;

}

static void __exit my_sysfs_exit (void)
{
	kfree(my_page_buffer);
	kobject_put(my_sysfs_kobj);
	
}

module_init(my_sysfs_init);
module_exit(my_sysfs_exit);
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Ritesh Ranjan");

