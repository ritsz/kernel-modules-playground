#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/netfilter.h>
#include <linux/netfilter_ipv4.h>
#include <linux/skbuff.h>
#include <linux/ip.h>
#include <linux/string.h>
#include <linux/list.h>
#include <linux/slab.h>
#include <linux/spinlock.h>
#include <linux/seq_file.h>
#include <linux/proc_fs.h>
#include <asm/uaccess.h>
#include <linux/seq_file.h>

static struct nf_hook_ops nfho;   
struct sk_buff *sock_buff;
struct iphdr *ip_header;

struct network_list {	
	char addr[16];
	atomic_t count;
	atomic64_t data_used;
	struct network_list *next;
};

struct network_list *root, *tail; 
static struct proc_dir_entry *net_file;

DEFINE_SPINLOCK(net_lock);

void free(struct network_list *node) {
	if (node == NULL)
		return;
	pr_info("free node for %s\n", node->addr);
	/* Recursively free the linked list */
	free(node->next);
	kfree(node);
}

void add_to_list(char *saddr, int data_size){

	struct network_list *temp = root;
	int added = 0;

	/* Best to use spinlocks when modifying global data structures that
	 * could get modified simultaneously
	 */
	spin_lock(&net_lock);
	while(temp != NULL) {
		/* If address don't match, check next node */
		if (strcmp(temp->addr, saddr) != 0) {
			temp = temp->next;
		} else {
			//else if match found
			atomic_inc(&(temp->count));
			atomic64_add(data_size, &(temp->data_used));
			added = 1;
			break;
		}
	}
	/* If packet not added to list */
	if (!added) {
		pr_info("New node for source %s\n", saddr);
		tail->next =  kmalloc(sizeof(struct network_list), GFP_KERNEL);
		tail = tail->next;
		atomic_set(&(tail->count), 1);
		atomic64_set(&(tail->data_used), data_size);
		strcpy(tail->addr, saddr);
		tail->next = NULL;
	}
	spin_unlock(&net_lock);
}

unsigned int hook_func(const struct nf_hook_ops *ops, struct sk_buff *skb,
		const struct net_device *in, const struct net_device *out, 
		int (*okfn)(struct sk_buff *))
{ 
	char source[16];
	sock_buff = skb;
        ip_header = (struct iphdr *)skb_network_header(sock_buff);    //grab network header using accessor
        
	if(!sock_buff) {
		return NF_ACCEPT;
	}
	
	snprintf(source, 16, "%pI4", &ip_header->saddr); 
      	/* If the source address isn't a loopback interface, log the data in the
	 * list. sk_buff::data_len contains the data size that has been
	 * transmitted in that buffer. Tested using ping -s 1024 facebook.com 
	 */
	if (source[0] == '0' || source[0] != '1' || source[1] != '2' || source[2] != '7')
		add_to_list(source, sock_buff->data_len);
	
	return NF_ACCEPT;
}

static void *my_start(struct seq_file *s, loff_t *pos)
{
	pr_info("START\n");
	return root->next;
}

static void *my_next(struct seq_file *s, void *v, loff_t *pos)
{
	struct network_list *ptr = ((struct network_list *) v)->next;
	if (!ptr)
		return NULL;
	else
		return ptr;
}

static void my_stop(struct seq_file *s, void *v)
{
	pr_info("STOP\n");
}

static int my_show(struct seq_file *s, void *v)
{
	struct network_list *ptr = ((struct network_list *) v);
	seq_printf(s, "%s\t:\t%d Packets\t:\t%ld KB\n", ptr->addr, atomic_read(&(ptr->count)),
			atomic64_read(&(ptr->data_used))/1024 );
        return 0;
}

static struct seq_operations swaps_op = {
	.start =        my_start,
	.next =         my_next,
	.stop =        	my_stop,
	.show =         my_show
};

static int my_open(struct inode *inode, struct file *file)
{
	return seq_open(file, &swaps_op);
} 

static int my_write(struct file *f, const char __user *buf,
		                    size_t len, loff_t *off)
{
	spin_lock(&net_lock);
	pr_info("TRY TO CLEAR LIST\n");
	struct network_list *temp = root;
	
	while(temp != NULL) {
		atomic_set(&(temp->count), 0);
		atomic64_set(&(temp->data_used), 0);
		temp = temp->next;
	}

	spin_unlock(&net_lock);
	return len;
}

static struct file_operations fops = {
	.owner = THIS_MODULE,
	.open = my_open,
	.write = my_write,
	.read = seq_read,
	.llseek = seq_lseek,
	.release = seq_release,
};

static int init_packet(void)
{
	root =  kmalloc(sizeof(struct network_list), GFP_KERNEL);
	atomic_set(&(root->count), 0);
	atomic64_set(&(root->data_used), 0);
	root->next = NULL;
	strcpy(root->addr, "");
	tail = root;

        nfho.hook = hook_func;          //function to call when conditions below met
        nfho.hooknum = 1;               //called right when packet coming to the interafce is received
        nfho.pf = PF_INET;                      //IPV4 packets
        nfho.priority = NF_IP_PRI_FIRST;        //set to highest priority over all other hook functions
        nf_register_hook(&nfho);                //register hook
	
	net_file = proc_create_data("net_proc", 0644, NULL, &fops, NULL);
	if(net_file == NULL) {
		return -ENOMEM;
	}

	return 0;
}

void cleanup_packet(void)
{
	remove_proc_entry("net_proc", NULL);
	free(root);
        nf_unregister_hook(&nfho);
}

module_init(init_packet);
module_exit(cleanup_packet);
MODULE_LICENSE("GPL");
MODULE_AUTHOR("ritsz");
MODULE_DESCRIPTION("A module that promiscuously listens to packets destined to \
		local device and logs their metadata");
