#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/netfilter.h>
#include <linux/netfilter_ipv4.h>
#include <linux/skbuff.h>
#include <linux/icmp.h>
#include <linux/ip.h>

static struct nf_hook_ops nfho;   //net filter hook option struct
struct sk_buff *sock_buff;
struct icmphdr *icmp_header;          //icmp header struct (not used)
struct iphdr *ip_header;            //ip header struct

unsigned int hook_func(const struct nf_hook_ops *ops, struct sk_buff *skb,
		const struct net_device *in, const struct net_device *out, 
		int (*okfn)(struct sk_buff *))
{
        sock_buff = skb;
        ip_header = (struct iphdr *)skb_network_header(sock_buff);    //grab network header using accessor
        if(!sock_buff) {
		return NF_ACCEPT;
	}
	/* ICMP has a protocol number of 1. We are trying to drop ICMP packets
	 * destined to the local device/interface
	 */
        if (ip_header->protocol == 1) {
                /*
		   Utility functions to get transport, network, mac headers
		   skb_transport_header
		   skb_network_header
		   skb_mac_header
		 */
		/* Print the source address of the ICMP packets before dropping
		 * them
		 */
		char source[16];
		snprintf(source, 16, "%pI4", &ip_header->saddr); 
		pr_info("SOURCE ADDRESS %pI4\n", &ip_header->saddr);
		
		icmp_header = (struct icmphdr *)skb_transport_header(sock_buff);
                pr_info("Dropping ping packet\n");
                return NF_DROP;
        }

        return NF_ACCEPT;
}

static int init_packet(void)
{
        nfho.hook = hook_func;          //function to call when conditions below met
        nfho.hooknum = 1;               //called right when packet coming to the interafce is received
        nfho.pf = PF_INET;                      //IPV4 packets
        nfho.priority = NF_IP_PRI_FIRST;        //set to highest priority over all other hook functions
        nf_register_hook(&nfho);                //register hook

	return 0;
}

void cleanup_packet(void)
{
        nf_unregister_hook(&nfho);
}

module_init(init_packet);
module_exit(cleanup_packet);
MODULE_LICENSE("GPL");
