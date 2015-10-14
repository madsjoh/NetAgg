#include <linux/kernel.h>
#include <linux/module.h>
#include <asm/uaccess.h>
#include <linux/fs.h>
#include <linux/miscdevice.h>
#include <linux/proc_fs.h>
#include <linux/types.h>
#include <linux/netfilter_ipv4.h>
#include <linux/netfilter.h>
#include <linux/skbuff.h>
#include <linux/init.h>
#include <linux/ip.h>
#include <linux/tcp.h>
#include <linux/version.h>
#include <net/tcp.h>
#include <net/ip.h>
#include "../include/linux/netagg.h"

/* Rule lists */
struct rule_list *sender_list;
struct rule_list *receiver_list;
/* Netfiler hooks */
static struct nf_hook_ops hook_output, hook_prerouting;
/* Dev entry*/
struct proc_dir_entry *netagg_proc_entry;

/*
 * ==================================================
 *    Appending and flushing rules in a linked list
 * ==================================================
 */
static void netagg_append_sender(struct rule *rule){
	struct rule_list *tmp = kzalloc(sizeof(struct rule_list),GFP_KERNEL);
	memcpy((void *)&tmp->rule, (void *)rule,sizeof(struct rule));
	if(sender_list == NULL){
		sender_list = tmp;
	}
	else{
		struct rule_list *i = sender_list;
		while(i->next != NULL) i = i->next;
		i->next = tmp;
	}
}
static void netagg_append_receiver(struct rule *rule){
	struct rule_list *tmp = kzalloc(sizeof(struct rule_list),GFP_KERNEL);
	memcpy((void *)&tmp->rule, (void *)rule,sizeof(struct rule));
	if(receiver_list == NULL){
		receiver_list = tmp;
	}
	else{
		struct rule_list *i = receiver_list;
		while(i->next != NULL) i = i->next;
		i->next = tmp;
	}
}
static void free_rule_list(struct rule_list *n){
	if(n != NULL)
	{
		free_rule_list(n->next);
		kfree(n);
	}
}
static void netagg_flush_sender(void){
	free_rule_list(sender_list);
}
static void netagg_flush_receiver(void){
	free_rule_list(receiver_list);
}
/* Checks bit at index in binary vector*/
int checkBit(int index, __be64 vector){
	__be64 mask = 01;
	mask <<= index;
	mask &= vector;
	mask >>= index;
	return (int) mask;
}
/* Filter packets in output hook, this is locally generated packets at the sender side. */
static void output_filter(struct sk_buff *skb)
{
	struct rule_list *tmp = sender_list;
	int tcplen = 0;
	if(skb != NULL){
		/* retrive IP and TCP header from SKB*/
		struct iphdr *ip 	= (struct iphdr *)skb_network_header(skb);
		struct tcphdr *tcp 	= (struct tcphdr *) (skb->data + (ip->ihl * 4));
		/* Go through rule list*/
		while(tmp != NULL){
			if(tmp->rule.pri_daddr == ntohl(ip->daddr) && tmp->rule.pri_dport == ntohs(tcp->dest)){
				/* match */
				if(checkBit(tmp->index, tmp->rule.binary_vector)){
					/* diverge packet */
					ip->daddr = htonl(tmp->rule.sec_daddr);
					tcp->dest = htons(tmp->rule.sec_dport);
					tcplen = (skb->len - (ip->ihl << 2));
					tcp->check = 0;
					tcp->check = ~tcp_v4_check(tcplen, ip->saddr,ip->daddr, 0);
					skb->csum_start = skb_transport_header(skb) - skb->head;
					skb->csum_offset = offsetof(struct tcphdr, check);
					ip->check = 0;
					ip->check = ip_fast_csum((u8 *)ip, ip->ihl);
				}
				tmp->index = ((tmp->index + 1) % 64);
				break;
			}
			tmp = tmp->next;
		}
	}
}
/* Filter packets in prerouting hook, this is incoming packets before they are routed at the receiver side.*/
static void prerouting_filter(struct sk_buff *skb)
{
	struct rule_list *tmp = receiver_list;
	int tcplen = 0;
	if(skb != NULL){
		/* retrive IP and TCP header from SKB*/
		struct iphdr *ip 	= (struct iphdr *)skb_network_header(skb);
		struct tcphdr *tcp 	= (struct tcphdr *) (skb->data + (ip->ihl * 4));
		/* Go through rule list*/
		while(tmp != NULL){
			if(tmp->rule.sec_daddr == ntohl(ip->daddr) && tmp->rule.sec_dport == ntohs(tcp->dest)){
				/* match, merge packet */
				ip->daddr = htonl(tmp->rule.pri_daddr);
				tcp->dest = htons(tmp->rule.pri_dport);
				tcplen = (skb->len - (ip->ihl << 2));
				tcp->check = 0;
				tcp->check = ~tcp_v4_check(tcplen, ip->saddr,ip->daddr, 0);
				skb->csum_start = skb_transport_header(skb) - skb->head;
				skb->csum_offset = offsetof(struct tcphdr, check);
				ip->check = 0;
				ip->check = ip_fast_csum((u8 *)ip, ip->ihl);
				tmp->index = ((tmp->index + 1) % 64);
				break;
			}
			tmp = tmp->next;
		}
	}
}
#if LINUX_VERSION_CODE < KERNEL_VERSION(3,13,0)
static unsigned int hook_func(unsigned int hooknum, struct sk_buff *skb, const struct net_device *in, const struct net_device *out, int (*okfn)(struct sk_buff *))
{
	switch(hooknum){
	case NF_INET_LOCAL_OUT :
	{
		output_filter(skb);
		break;
	}
	case NF_INET_PRE_ROUTING :
	{
		prerouting_filter(skb);
		break;
	}	
	}
	return NF_ACCEPT;
}
#endif

#if LINUX_VERSION_CODE >= KERNEL_VERSION(3,13,0)
static unsigned int hook_func(const struct nf_hook_ops *ops, struct sk_buff *skb,
			      const struct net_device *in, const struct net_device *out, int (*okfn)(struct sk_buff *))
{
	switch(ops->hooknum){
	case NF_INET_LOCAL_OUT :
	{
		output_filter(skb);
		break;
	}
	case NF_INET_PRE_ROUTING :
	{
		prerouting_filter(skb);
		break;
	}	
	}
	return NF_ACCEPT;
}
#endif
/*
 * ==================================================
 *    		    IOCTL Interface
 * ==================================================
 */
static int netagg_open(struct inode *inode, struct file *file)
{
	return 0;
}
static int netagg_close(struct inode *inode, struct file *file)
{
	return 0;
}
static long netagg_ioctl(struct file *file, unsigned int ioctl_num, unsigned long ioctl_param){
	int ret = 0;
	int i = 0;
	struct rule rule;
	switch (ioctl_num) {
	case NETAGG_IOCTL_APPEND_SENDER :
		i = copy_from_user((void *) &rule, (void *) ioctl_param, _IOC_SIZE(ioctl_num));
		netagg_append_sender(&rule);
		break;
	case NETAGG_IOCTL_APPEND_RECEIVER :
	        i = copy_from_user((void *) &rule, (void *) ioctl_param, _IOC_SIZE(ioctl_num));
		netagg_append_receiver(&rule);
		break;
	case NETAGG_IOCTL_FLUSH_SENDER :
		netagg_flush_sender();
		break;
	case NETAGG_IOCTL_FLUSH_RECEIVER :
		netagg_flush_receiver();
		break;
	default:
		ret = -EINVAL;
	}
	return ret;
}
struct file_operations
netagg_dev_fops = {
	.owner          = THIS_MODULE,
	.unlocked_ioctl = netagg_ioctl,
	.open           = netagg_open,
	.release        = netagg_close,
};
static struct miscdevice
netagg_misc = {
	.minor = MISC_DYNAMIC_MINOR,
	.name  = NETAGG_MODULE_NAME,
	.fops  = &netagg_dev_fops,
};

/*
 *===============================================
 *        Module init and exit fuctions 
 *===============================================
 */
static __init int netagg_init(void)
{
	int ret = 0;
	ret = misc_register(&netagg_misc);
	if(ret < 0){
		goto out;
	}
	/* init rule lists*/
	sender_list = NULL;
	receiver_list = NULL;
	/* 
	 *=============================
	 *   SETUP NETFILTER HOOKS
	 *=============================
	 */

	/* OUTPUT HOOK */
	hook_output.hook = hook_func;
	hook_output.hooknum = NF_INET_LOCAL_OUT;
	hook_output.pf = PF_INET;
	hook_output.priority = NF_IP_PRI_FIRST;
	nf_register_hook(&hook_output);

	/* PREROUTING HOOK */
	hook_prerouting.hook = hook_func;
	hook_prerouting.hooknum = NF_INET_PRE_ROUTING;
	hook_prerouting.pf = PF_INET;
	hook_prerouting.priority = NF_IP_PRI_FIRST;
	nf_register_hook(&hook_prerouting);

	printk("NetAgg module installed\n");
out:
	return ret;

}
static __exit void netagg_exit(void)
{
	nf_unregister_hook(&hook_output);
	nf_unregister_hook(&hook_prerouting);
	misc_deregister(&netagg_misc);
	free_rule_list(sender_list);
	free_rule_list(receiver_list);
	printk("NetAgg module uninstalled\n");
}

module_init(netagg_init);
module_exit(netagg_exit);

MODULE_AUTHOR("Mads Johannessen <madsjoh@ifi.uio.no>");
MODULE_DESCRIPTION("Transparently aggregate IP packets from TCP connections over two network paths.");
MODULE_LICENSE("GPL");
MODULE_VERSION("1.0");

