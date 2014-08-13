#include <linux/kernel.h>
#include <linux/module.h>

/* Mouse data port and scan port are the same as that of keyboard, infact same
 * for all PS2 devices I think. 0x60 for data and 0x64 for scancode. Mouse
 * doesn't trigger an interrupt (maybe IRQ12). To check if data is available, we
 * need to scan bit 0 on port 0x64. If value is 1, data is available in the
 * buffer. If bit 5 on port 0x64 is set, the data at 0x60 port is mouse data or
 * else it is keyboard data.
 */

/* UNSTABLE. VERY. Need to fix.
 */

static int mouse_init(void)
{
	int loops_for_mouse = 0;
	char keys = ' ' , x_mov = ' ', y_mov = ' ', scan_code = ' ';
	
	pr_info("Loading Mouse Module\n");

	while(loops_for_mouse < 10000) {
		loops_for_mouse++;
	}
	loops_for_mouse = 0;
	while (loops_for_mouse < 100) {
		scan_code = inb(0x64);
		if ((scan_code&0b00000001) == 1) {
			pr_info("Data Available");
			loops_for_mouse++;
		} else {
			continue;
		}
		
		if ((scan_code&0b00100000) == 1) {
			pr_info("MOUSE\n");
		} else {
			pr_info("KEYBOARD\n");
		}
		keys = inb(0x60);
		x_mov = inb(0x60);
		y_mov = inb(0x60);
		pr_info("Keys : %d\nX : %d\nY : %d\n", keys, x_mov, y_mov);
	}
	pr_info("DONE. Unload now\n");
	return 0;
}

static void mouse_exit(void)
{
	pr_info("Unloading Mouse module");
}

module_init(mouse_init);
module_exit(mouse_exit);
MODULE_LICENSE("GPL");
MODULE_AUTHOR("RITESH RANJAN");
