#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <asm/segment.h>
#include <asm/uaccess.h>
#include <linux/buffer_head.h>

struct file* open_file;

struct file* file_open(const char* path, int flags, int rights) 
{
    	struct file* filp = NULL;
    	mm_segment_t oldfs;
    	int err = 0;

    	oldfs = get_fs();
    	set_fs(get_ds());
    	filp = filp_open(path, flags, rights);
    	set_fs(oldfs);
    	if(IS_ERR(filp)) {
    		err = PTR_ERR(filp);
        	return NULL;
    	}
    	return filp;
}

void file_close(struct file* file)
{
    	filp_close(file, NULL);
}

int file_read(struct file* file, unsigned long long offset, unsigned char* data, unsigned int size) 
{
    	mm_segment_t oldfs;
    	int ret;

    	oldfs = get_fs();
    	set_fs(get_ds());

    	ret = vfs_read(file, data, size, &offset);

    	set_fs(oldfs);
    	return ret;
}

int file_write(struct file* file, unsigned long long offset, unsigned char* data, unsigned int size) 
{
    	mm_segment_t oldfs;
    	int ret;

    	oldfs = get_fs();
    	set_fs(get_ds());

    	ret = vfs_write(file, data, size, &offset);

    	set_fs(oldfs);
    	return ret;
}

int file_sync(struct file* file)
{
    	vfs_fsync(file, 0);
    	return 0;
}

static int __init file_init(void)
{
  	char buffer[256];

	open_file = file_open("/root/PROGRAMMING/Kernel_Hacking/", O_RDWR, 0);
	if (open_file == NULL) {
		pr_err("Error in opening the file");
		goto end;
	} else {
		pr_info("File opened without any issue");
	}

	file_read(open_file, 0, buffer, 256);
	pr_info("%s", buffer);

	end:
  	return 0;
}

static void __exit file_exit(void)
{
	file_close(open_file);
}

MODULE_LICENSE("GPL");
module_init(file_init);
module_exit(file_exit);
