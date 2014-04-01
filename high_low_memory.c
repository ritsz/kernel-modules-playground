#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/highmem.h>
#include <linux/gfp.h>
#include <linux/mm_types.h>
#include <asm/io.h>


MODULE_LICENSE("Dual BSD/GPL");

static int hello_init(void) {


	/*
	 *	The GFP_USER WILL RETURN AN ADDRESS WHERE
	 *	VIRTUAL 	< 	0xFE000000 and
	 *	PHYSICAL 	< 	896MB
	 */

	struct page *highmem_page = alloc_page(GFP_USER );
	if ( ! highmem_page ) printk(KERN_ERR "Couldn't allocate highmem page with order : 3\n" );
  	else {

  		//unsigned long page_addr = page_address(highmem_page);
  		//printk(KERN_ERR "Address %lu\n", page_addr);

    		long highmem_page_address = kmap( highmem_page );
    		if ( ! highmem_page_address ) printk( KERN_ERR "Couldn't map highmem pages.\n");
    		else {
      			printk( KERN_ERR "Address for highuser pages is : %lx. Order: %d.\n", 
            		highmem_page_address,
            		0
           		);
     		}	

     		long phy_addr = virt_to_phys(page_address(highmem_page));
     		printk(KERN_ERR "Physical Address is %lx \n", phy_addr);
  	}

  	kunmap(highmem_page);
  	__free_page( highmem_page);

  	/*
	 *	The GFP_HIGHUSER WILL RETURN AN ADDRESS WHERE
	 *	VIRTUAL 	> 	0xFE000000 and
	 *	PHYSICAL 	> 	896MB
	 */ 	

  	highmem_page = alloc_page(GFP_HIGHUSER );
	if ( ! highmem_page ) printk(KERN_ERR "Couldn't allocate highmem page with order : 3\n" );
  	else {

  		long highmem_page_address = kmap( highmem_page );
    		if ( ! highmem_page_address ) printk( KERN_ERR "Couldn't map highmem pages.\n");
    		else {
      			printk( KERN_ERR "Address for highuser pages is : %lx. Order: %d.\n", 
            			highmem_page_address,
            			0
           		);
     		}

     		long phy_addr = virt_to_phys(page_address(highmem_page));
     		printk(KERN_ERR "Physical Address is %lx \n", phy_addr);
  	}

  	kunmap(highmem_page);
  	__free_page( highmem_page);

  	return 0;
}

static void hello_exit(void) {
  printk("<1> Bye, cruel world\n");
}

module_init(hello_init);
module_exit(hello_exit);
