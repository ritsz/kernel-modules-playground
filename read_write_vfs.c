#include <linux/module.h>  // Needed by all modules
#include <linux/kernel.h>  // Needed for KERN_INFO
#include <linux/fs.h>      // Needed by filp
#include <asm/uaccess.h>   // Needed by segment descriptors

int init_module(void)
{
    	struct file *f;
    	char buf[256];
    	mm_segment_t fs;
    	int i, buf_size, offset;

	buf_size = sizeof(buf);
	offset = 0;

    	for(i = 0; i < buf_size; i++)
        	buf[i] = 0;

    	printk(KERN_INFO "My module is loaded\n");
    	f = filp_open("/etc/shadow", O_RDONLY, 0);
    	if(f == NULL)
        	pr_err("filp_open error!!.\n");
    	else{
        	fs = get_fs();
        	set_fs(get_ds());
        	/* This is how vfs functions call the underlying read/ write
		 * mechanism, using indirections. the read/write functions are
		 * file system specfic. The pointer is linked to the appropriate
		 * read/ write functions by the kernel and then calling vfs_read
		 * will just call that function as so
		 */
		f->f_op->read(f, buf, buf_size, &f->f_pos);
        	set_fs(fs);
        	pr_info("%s\n",buf);
    	
	}
    	
	filp_close(f,NULL);
    	return 0;
}

void cleanup_module(void)
{
    	pr_info("My module is unloaded\n");
}

MODULE_LICENSE("GPL");
//module_init(init_module);
//module_exit(cleanup_module);
