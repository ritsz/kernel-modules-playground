#include <linux/module.h>
#include <net/sock.h>
#include <linux/netlink.h>
#include <linux/skbuff.h>

#define NETLINK_USER 31
#define MULTICAST_GROUP 0x21

struct sock *nl_sk = NULL;

static void hello_nl_recv_msg(struct sk_buff *skb)
{

    	struct nlmsghdr *nlh;
    	int pid;
    	struct sk_buff *skb_out;
    	int msg_size;
    	char *msg = "Hello from kernel";
    	int res;
	
    	printk(KERN_INFO "Entering: %s\n", __FUNCTION__);

    	msg_size = strlen(msg);

    	nlh = (struct nlmsghdr *)skb->data;
    	printk(KERN_INFO "Netlink received msg payload: %s\n", (char *)nlmsg_data(nlh));
    	pid = nlh->nlmsg_pid; /*pid of sending process*/
	pr_info("Sending data back to task no. %d\n", pid);

    	skb_out = nlmsg_new(msg_size, 0);

    	if (!skb_out) {
		printk(KERN_ERR "Failed to allocate new skb\n");
        	return;

    	}
    	nlh = nlmsg_put(skb_out, 0, 1, NLMSG_DONE, msg_size, 0);
	if (!nlh) {
		pr_err("nlmsg_put() : No netlink message header\n");
		return;
	}

    	//NETLINK_CB(skb_out).dst_group = MULTICAST_GROUP;
	strncpy(nlmsg_data(nlh), msg, msg_size);

    	res = netlink_broadcast(nl_sk, skb_out, pid, MULTICAST_GROUP, GFP_KERNEL);

    	if (res < 0)
        	printk(KERN_INFO "Error while sending back to user : %d\n", res);
}

struct netlink_kernel_cfg netlink_cfg = {
	.input = hello_nl_recv_msg,
};


static int __init hello_init(void)
{

    	printk("Entering: %s\n", __FUNCTION__);
	nl_sk = netlink_kernel_create(&init_net, NETLINK_USER, &netlink_cfg);
    	if (!nl_sk) {

       		printk(KERN_ALERT "Error creating socket.\n");
        	return -10;

    	}

    	return 0;
}

static void __exit hello_exit(void)
{

    	printk(KERN_INFO "exiting hello module\n");
    	netlink_kernel_release(nl_sk);
}

module_init(hello_init);
module_exit(hello_exit);

MODULE_LICENSE("GPL");
