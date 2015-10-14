
#ifndef NETAGG_H
#define NETAGG_H
/*
 * ==========================================
 *      NetAgg DATA STRUCTURES
 * ==========================================
 */
#include <linux/types.h>
#define NETAGG_MODULE_NAME "NetAgg"
#ifdef __KERNEL__
#ifdef CONFIG_KUSP_NETAGG_DSKI
#include <linux/kusp/dski.h>
#define NETAGG_DEBUG(fmt, args...) DSTRM_DEBUG(NETAGG, DEBUG, fmt, ## args)
#else
#define NETAGG_DEBUG(fmt, args...)
#endif /* CONFIG_KUSP_NETAGG_DSKI */

/*
 * Required Proc File-system Struct
 *
 * Used to map entry into proc file table upon module insertion
 */
extern struct proc_dir_entry *my_netfilter_proc_entry;
#endif /* __KERNEL__ */

typedef struct rule
{
    __be32 pri_daddr;
    __be32 sec_daddr;
    __be16 pri_dport;
    __be16 sec_dport;
    __be64 binary_vector;
}rule_t;

struct rule_list{
    struct rule rule;
    struct rule_list *next;
    int index;
};

#define NETAGG_MAGIC 'm'

#define NETAGG_IOCTL_APPEND_SENDER   _IOW(NETAGG_MAGIC,    1, rule_t)
#define NETAGG_IOCTL_APPEND_RECEIVER   _IOW(NETAGG_MAGIC,    2, rule_t)
#define NETAGG_IOCTL_FLUSH_SENDER    _IO(NETAGG_MAGIC,     3)
#define NETAGG_IOCTL_FLUSH_RECEIVER    _IO(NETAGG_MAGIC,     4)

#endif /* NETAGG_H */
