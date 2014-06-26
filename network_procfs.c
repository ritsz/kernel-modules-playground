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
#include <linux/proc_fs.h>
#include <asm/uaccess.h>

static struct nf_hook_ops nfho;   //net filter hook option struct
struct sk_buff *sock_buff;
struct iphdr *ip_header;            //ip header struct

struct network_list {
	atomic_t count;
	char addr[16];
	struct network_list *next;
};

struct network_list *root, *tail; 
static struct proc_dir_entry *net_file;

DEFINE_SPINLOCK(net_lock);

void free(struct network_list *node) {
	if (node == NULL)
		return;
	pr_info("free node for %s\n", node->addr);
	free(node->next);
	kfree(node);
}

void add_to_list(char *saddr){

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
			pr_info("%s already present", saddr);
			atomic_inc(&(temp->count));
			added = 1;
			break;
		}
	}
	/* If packet not added to list */
	if (!added) {
		pr_info("New node for source %s", saddr);
		tail->next =  kmalloc(sizeof(struct network_list), GFP_KERNEL);
		atomic_set(&(tail->count), 1);
		strcpy(tail->addr, saddr);
		tail = tail->next;
		tail->next = NULL;
	}
	spin_unlock(&net_lock);
}

unsigned int hook_func(const struct nf_hook_ops *ops, struct sk_buff *skb,
		const struct net_device *in, const struct net_device *out, 
		int (*okfn)(struct sk_buff *))
{
        sock_buff = skb;
        ip_header = (struct iphdr *)skb_network_header(sock_buff);    //grab network header using accessor
        if(!sock_buff) {
		return NF_ACCEPT;
	}
	char source[16];
	snprintf(source, 16, "%pI4", &ip_header->saddr); 
	pr_info("SOURCE ADDRESS %pI4\n", &ip_header->saddr);
        add_to_list(source);
	return NF_ACCEPT;
}

static int init_packet(void)
{
	root =  kmalloc(sizeof(struct network_list), GFP_KERNEL);
	atomic_set(&(root->count), 0);
	root->next = NULL;
	strcpy(root->addr, "");
	tail = root;

        nfho.hook = hook_func;          //function to call when conditions below met
        nfho.hooknum = 1;               //called right when packet coming to the interafce is received
        nfho.pf = PF_INET;                      //IPV4 packets
        nfho.priority = NF_IP_PRI_FIRST;        //set to highest priority over all other hook functions
        nf_register_hook(&nfho);                //register hook
	
	net_file = proc_create_data("seq_proc", 0644, NULL, &fops, NULL);
	if(net_file == NULL) {
		return -ENOMEM;
	}

	return 0;
}

void cleanup_packet(void)
{
	free(root);
        nf_unregister_hook(&nfho);
}

module_init(init_packet);
module_exit(cleanup_packet);
MODULE_LICENSE("GPL");