#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/netfilter.h>
#include <linux/netfilter_ipv4.h>

static struct nf_hook_ops nfho;
/*
  struct nf_hook_ops {
        struct list_head list;
        nf_hookfn       *hook;          // Function to be hooked
        struct module   *owner;         // Owner of the module
        void            *priv;		// Private data
        u_int8_t        pf;		// Protocol family PF_UNSPEC, PF_LOCAL, PF_INET, PF_PACKET
        unsigned int    hooknum;	// 
        int             priority;
  };
  */
unsigned int hook_func(unsigned int hooknum, struct sk_buff **skb, 
		const struct net_device *in, const struct net_device *out, 
		int (*okfn)(struct sk_buff *))
{
	pr_info("Packet Going out\n");
	return NF_ACCEPT;
	/*
	  	Responses from hook functions.
		#define NF_DROP 0
		#define NF_ACCEPT 1
		#define NF_STOLEN 2
		#define NF_QUEUE 3
		#define NF_REPEAT 4
		#define NF_STOP 5
		#define NF_MAX_VERDICT NF_STOP
	*/
}

int init_module()
{
	nfho.hook = hook_func;          //function to call when conditions below met
  	nfho.hooknum = 3;            	//called right when packet going out of device
	 /* IP Hooks
	//After promisc drops, checksum checks.
	#define NF_IP_PRE_ROUTING       0       
	// If the packet is destined for this box.
	#define NF_IP_LOCAL_IN          1	
	// If the packet is destined for another interface.
	#define NF_IP_FORWARD           2	
	// Packets coming from a local process.
	#define NF_IP_LOCAL_OUT         3	
	// Packets about to hit the wire. 
	#define NF_IP_POST_ROUTING      4
	#define NF_IP_NUMHOOKS          5
	*/

	nfho.pf = PF_INET;                      //IPV4 packets
  	nfho.priority = NF_IP_PRI_FIRST;        //set to highest priority over all other hook functions
  	nf_register_hook(&nfho);                //register hook
  	/*
	   The above function adds the nf_hook_ops element to an array of
	   nf_hooks like so  struct list_head nf_hooks[NFPROTO_NUMPROTO][NF_MAX_HOOKS]
	   nf_hooks is a list_head structure. The nf_hook_ops->list is added
	   using list_add_rcu() to the nf_hooks list_head array depending upon
	   the priority. nf_hooks[][] is an array of list_head. There is a
	   separate list for each PROTOCOL and HOOK. Thus nf_hooks[PF_INET][NF_IP_FORWARD] 
	   is a list of hooks for that particular PF and HOOK pair. The position
	   in which this new hook is added in the hook list depends on the
	   priority.
	 */
	return 0;                               //return 0 for success
}

void cleanup_module()
{
	nf_unregister_hook(&nfho);             //cleanup – unregister hook
	/*
	   list_del_rcu of this hook from the list_head array, followed by
	   synchronize_net. synchronize_net waits for packets currently being
	   received to be done.
	 */
}
