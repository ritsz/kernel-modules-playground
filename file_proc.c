#include <linux/module.h>
#include <linux/kernel.h>

#include <linux/kthread.h>  // for threads
#include <linux/sched.h>  // for task_struct
#include <linux/time.h>   // for using jiffies  
#include <linux/timer.h>
#include <linux/fs.h>
#include <linux/list.h>
#include <linux/slab.h>
#include <asm/uaccess.h>    // copy_from_user, copy_to_user
#include <linux/proc_fs.h> // struct proc
#include <linux/kallsyms.h>
#include <linux/string.h>
#include <asm/current.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Stephen Zhang");
MODULE_DESCRIPTION("Simple accumulator via procfs");

typedef int (*function)(void);

#define MAX_INPUT_SIZE 1024
char input_buf[MAX_INPUT_SIZE];
char result[16];
static struct proc_dir_entry *simacc_file;

static int simacc_read(char *page, char **start, off_t off,
    int count, int *eof, void *data)
{
    int len;
    len = sprintf(page, "%s\n", result);
    return len;
}

static int simacc_write(struct file *file, const char *buffer,
    unsigned long count, void *data)
{
    int len;
    unsigned long num = 0, sum = 0;
    int i;
    char c;
    if (count > MAX_INPUT_SIZE)
    len = MAX_INPUT_SIZE;
    else
    len = count;

    if(copy_from_user(input_buf, buffer, len))
    return -EFAULT;
    input_buf[len] = '&#92;&#48;';

    i = 0;
    do {
    c = input_buf[i++];
    if (c >= '0' && c <= '9') {
        num = num * 10 + c - '0';
    } else {
        sum += num;
        num = 0;
    }
    } while (c != '&#92;&#48;' && i < MAX_INPUT_SIZE);
    sprintf(result, "%lu", sum);
    return len;
} 

static int __init init_simacc(void) {
    simacc_file = create_proc_entry("simacc", 0666, NULL);
    if (simacc_file == NULL) {
    return -ENOMEM;
    }

    printk(KERN_ALERT,"Current pid is %d : %s\n", current->pid, current->comm);

    input_buf[MAX_INPUT_SIZE - 1] = '&#92;&#48;';
    result[0] = '0'; result[1] = '&#92;&#48;';
    simacc_file->data = input_buf;
    simacc_file->read_proc = simacc_read;
    simacc_file->write_proc = simacc_write;
    printk(KERN_ALERT"%s: Module Loaded.\n", __this_module.name);
    char *sym_name = "thread_fn";
    unsigned long sym_addr = kallsyms_lookup_name(sym_name);
    // int* rday_3 = sym_addr;
    // printk("%d, %lu\n", *rday_3, sym_addr);
    if (sym_addr) {
        function ca = (function)sym_addr;
        ca();
    } else {
        printk(KERN_ERR"Unresolved Symbol %s", sym_name);
    }

    return 0;
}

static void __exit cleanup_simacc(void)
{
    remove_proc_entry("simacc", NULL);
    printk(KERN_INFO "%s: Module Unloaded.\n", THIS_MODULE->name);
}

module_init(init_simacc);
module_exit(cleanup_simacc);