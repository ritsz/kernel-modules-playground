#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/kallsyms.h>

typedef int (*function)(void);

/* A hack that allows you to call functions in the symbol table of the kernel
 * using symbol lookup function.
 */

static int __init init_kallsyms(void) {
	char *sym_name = "thread_fn";
    	unsigned long symbol_addr = kallsyms_lookup_name(sym_name);
    	// int* rday_3 = sym_addr;
    	// printk("%d, %lu\n", *rday_3, sym_addr);
    	if (sym_addr) {
    	    function func_call = (function)symbol_addr;
    	    func_call();
    	} else {
    	    printk(KERN_ERR"Unresolved Symbol %s", sym_name);
    	}
    return 0;
}

static void __exit cleanup_kallsyms(void)
{
   printk(KERN_INFO "%s: Module Unloaded.\n", THIS_MODULE->name);
}

MODULE_LICENSE("GPL");
module_init(init_kallsyms);
module_exit(cleanup_kallsyms);
