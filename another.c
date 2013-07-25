#include <linux/init.h>
//#include <linux/autoconf.h>
#include <linux/module.h>
#include <linux/kernel.h> /* printk() */
#include <linux/slab.h> /* kmalloc() */
#include <linux/fs.h> /* everything... */
#include <linux/errno.h> /* error codes */
#include <linux/types.h> /* size_t */
#include <linux/proc_fs.h>
#include <linux/fcntl.h> /* O_ACCMODE */
#include <asm/system.h> /* cli(), *_flags */
#include <asm/uaccess.h> /* copy_from/to_user */
#include <linux/miscdevice.h>
#include <linux/highmem.h>
#include <asm/unistd.h>

MODULE_LICENSE("Dual BSD/GPL");

#define IOCTL_PATCH_TABLE 0x00000001
#define IOCTL_FIX_TABLE   0x00000004

unsigned long *sys_call_table = (unsigned long*)0xffffffff81801320;
 int is_set=0;


int mem_open(struct inode *inode, struct file *file);
int mem_release(struct inode *inode, struct file *file);
static int our_ioctl(struct file *file, unsigned int cmd, unsigned long arg);
ssize_t memory_read(struct file *filp, char *buf, size_t count, loff_t *f_pos);
ssize_t memory_write(struct file *filp, char *buf, size_t count, loff_t *f_pos);
int mod_init(void);
void mod_exit(void);

char *memory_buffer;
 
struct file_operations our_fops={
      .open  =  &mem_open,ioctl
      .release  = &mem_release,
      .unlocked_ioctl = (void*)&our_ioctl,
      .compat_ioctl = (void*)&our_ioctl,
      .read =  memory_read,
      .write =  memory_write,
   };

   static struct miscdevice our_device={
      MISC_DYNAMIC_MINOR,
      "interceptor",
      &our_fops
   };


   module_init(mod_init);
   module_exit(mod_exit);


int make_rw(unsigned long address)
   {
      unsigned int level;
      pte_t *pte = lookup_address(address, &level);
      if(pte->pte &~ _PAGE_RW)
         pte->pte |= _PAGE_RW;
      return 0;
   }

   /* Make the page write protected */
   int make_ro(unsigned long address)
   {
      unsigned int level;
      pte_t *pte = lookup_address(address, &level);
      pte->pte = pte->pte &~ _PAGE_RW;
      return 0;
   }

   /* Pointer to the original sys_open */
   asmlinkage int (*real_open)(const char* __user, int, int);
   
   /* Our replacement */
   asmlinkage int custom_open(const char* __user file_name, int flags, int mode)
   {
      printk("interceptor: open(\"%s\", %X, %X)\n", file_name,
                                                    flags, 
                                                    mode);
      return real_open(file_name, flags, mode);
   }



int in_use = 0;

    int mod_init(void) 
   {
      
      printk(KERN_INFO "We are in kernel space \n");
      int retval = misc_register(&our_device);
      return retval;
   }


    void  mod_exit(void) 
   {
      printk(KERN_INFO "Elvis has left the building \n");
      if(is_set)
      {
      make_rw((unsigned long)sys_call_table);
      *(sys_call_table + __NR_open) = (unsigned long)real_open;
      make_ro((unsigned long)sys_call_table);
      }
      misc_deregister(&our_device);
      //return;
   }



    int mem_open(struct inode *inode, struct file *file)
   {
      /* We would not like to allow multiple processes to open this device */
      if(in_use)
         return -EBUSY;
      in_use++;
      printk("device has been opened\n");
      return 0;
   }

   /* This function, in turn, will be called when a process closes our device */
    int mem_release(struct inode *inode, struct file *file)
   {
      in_use--;
      printk("device has been closed\n");
      return 0;
   }

	
   




   /* This function will handle ioctl calls performed on our device */
   static int our_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
   {
      int retval = 0;
      printk("<1>device has been edited\n");
      switch(cmd)
   {
      case IOCTL_PATCH_TABLE:
         make_rw((unsigned long)sys_call_table);
         real_open = (void*)*(sys_call_table + __NR_open);
         *(sys_call_table + __NR_open) = (unsigned long)custom_open;
         make_ro((unsigned long)sys_call_table);
         is_set=1;
         break;
      case IOCTL_FIX_TABLE:
         make_rw((unsigned long)sys_call_table);
         *(sys_call_table + __NR_open) = (unsigned long)real_open;
         make_ro((unsigned long)sys_call_table);
         is_set=0;
         break;
      default:
         printk("Ooops....\n");
         break;
   }
      return retval;
   }


ssize_t memory_read(struct file *filp, char *buf,
size_t count, loff_t *f_pos) {
printk("<1>Reading Data\n");
/* Transfering data to user space */
copy_to_user(buf,memory_buffer,1);
/* Changing reading position as best suits */
if (*f_pos == 0) {
*f_pos+=1;
return 1;
} else {
return 0;
}
}


ssize_t memory_write( struct file *filp, char *buf,
size_t count, loff_t *f_pos) {
printk("<1>Writing Data\n");  
char *tmp;
tmp=buf+count-1;
copy_from_user(memory_buffer,tmp,1);
return 1;
}
   
