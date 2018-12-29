#include <net/genetlink.h>
#include <linux/module.h>
#include <linux/kernel.h>
//Code based on http://people.ee.ethz.ch/~arkeller/linux/multi/kernel_user_space_howto-3.html

#define OVS_TT_FAMILY "ovs_tt_test"
#define OVS_TT_VERSION 0x1

/* attributes (variables):
 * the index in this enum is used as a reference for the type,
 * userspace application has to indicate the corresponding type
 * the policy is used for security considerations 
 */
enum {
    OVS_TT_FLOW_ATTR_UNSPEC,
    OVS_TT_FLOW_ATTR_PORT,
    OVS_TT_FLOW_ATTR_ETYPE,
    OVS_TT_FLOW_ATTR_FLOW_ID,
    OVS_TT_FLOW_ATTR_SCHEDULED_TIME,
    OVS_TT_FLOW_ATTR_PERIOD,
    OVS_TT_FLOW_ATTR_BUFFER_ID,
    OVS_TT_FLOW_ATTR_PKT_SIZE,
    __OVS_TT_FLOW_ATTR_MAX,
};
#define OVS_TT_FLOW_ATTR_MAX (__OVS_TT_FLOW_ATTR_MAX - 1)

/* attribute policy: defines which attribute has which type (e.g int, char * etc)
 * possible values defined in net/netlink.h 
 */
static struct nla_policy doc_exmpl_genl_policy[OVS_TT_FLOW_ATTR_MAX + 1] = {
    [OVS_TT_FLOW_ATTR_PORT] = { .type = NLA_U8 },
    [OVS_TT_FLOW_ATTR_ETYPE] = { .type = NLA_U8 },
    [OVS_TT_FLOW_ATTR_FLOW_ID] = { .type = NLA_U8 },
    [OVS_TT_FLOW_ATTR_SCHEDULED_TIME] = { .type = NLA_U32 },
    [OVS_TT_FLOW_ATTR_PERIOD] = { .type = NLA_U32 },
    [OVS_TT_FLOW_ATTR_BUFFER_ID] = { .type = NLA_U32 },
    [OVS_TT_FLOW_ATTR_PKT_SIZE] = { .type = NLA_U32 },
};

//family definition
static struct genl_family doc_exmpl_gnl_family = {
	.id = GENL_ID_GENERATE,         //Genetlink should generate an id
	.hdrsize = 0,
	.name = OVS_TT_FAMILY,          //The name of this family, used by userspace application
	.version = OVS_TT_VERSION,      //Version number  
	.maxattr = OVS_TT_FLOW_ATTR_MAX,
};

/* commands: enumeration of all commands (functions), 
 * used by userspace application to identify command to be executed
 */
enum ovs_tt_cmd {
    OVS_TT_FLOW_CMD_NEW,
    OVS_TT_FLOW_CMD_DEL,
    OVS_TT_FLOW_CMD_GET,
    __OVS_TT_FLOW_CMD_MAX
};
#define OVS_TT_FLOW_CMD_MAX (__OVS_TT_FLOW_CMD_MAX - 1)

//An echo command, receives a message, prints it and sends another message back
int doc_exmpl_echo(struct sk_buff *skb_2, struct genl_info *info) 
{
	struct nlattr *nla;
	struct sk_buff *skb;
	int rc;
	void *msg_head;

	int port;
	int etype;
	int flow_id;
	int scheduled_time;
	int period;
	int buffer_id;
	int pkt_size;

	if (info == NULL) {
		goto out;
	}
  
	/* For each attribute there is an index in info->attrs which points to a nlattr structure
	 * in this structure the data is given
	 */
	nla = info->attrs[OVS_TT_FLOW_ATTR_PORT];
	port = *(int *)nla_data(nla);
	printk(KERN_CRIT "OVS_TT_FLOW_ATTR_PORT: %d\n", port);

	nla = info->attrs[OVS_TT_FLOW_ATTR_ETYPE];
	etype = *(int *)nla_data(nla);
	printk(KERN_CRIT "OVS_TT_FLOW_ATTR_ETYPE: %d\n", etype);

	nla = info->attrs[OVS_TT_FLOW_ATTR_FLOW_ID];
	flow_id = *(int *)nla_data(nla);
	printk(KERN_CRIT "OVS_TT_FLOW_ATTR_FLOW_ID: %d\n", flow_id);

	nla = info->attrs[OVS_TT_FLOW_ATTR_SCHEDULED_TIME];
	scheduled_time = *(int *)nla_data(nla);
	printk(KERN_CRIT "OVS_TT_FLOW_ATTR_SCHEDULED_TIME: %d\n", scheduled_time);

	nla = info->attrs[OVS_TT_FLOW_ATTR_PERIOD];
	period = *(int *)nla_data(nla);
	printk(KERN_CRIT "OVS_TT_FLOW_ATTR_PERIOD: %d\n", period);

	nla = info->attrs[OVS_TT_FLOW_ATTR_BUFFER_ID];
	buffer_id = *(int *)nla_data(nla);
	printk(KERN_CRIT "OVS_TT_FLOW_ATTR_BUFFER_ID: %d\n", buffer_id);

	nla = info->attrs[OVS_TT_FLOW_ATTR_PKT_SIZE];
	pkt_size = *(int *)nla_data(nla);
	printk(KERN_CRIT "OVS_TT_FLOW_ATTR_PKT_SIZE: %d\n", pkt_size);

	//Send a message back
	//Allocate some memory, since the size is not yet known use NLMSG_GOODSIZE
	skb = genlmsg_new(NLMSG_GOODSIZE, GFP_KERNEL);
	if (skb == NULL) {
		goto out;
	}

	//Create the message headers
	/* arguments of genlmsg_put: 
	   	struct sk_buff *, 
           	int (sending) pid, 
           	int sequence number, 
           	struct genl_family *, 
           	int flags, 
           	u8 command index (why do we need this?) */
 	msg_head = genlmsg_put(skb, 0, info->snd_seq + 1, &doc_exmpl_gnl_family, 0, OVS_TT_FLOW_CMD_NEW);
 	if (msg_head == NULL) {
  		rc = -ENOMEM;
  		goto out;
 	}
 	//Add a DOC_EXMPL_A_MSG attribute (actual the attribute type of the message sent from userspace)
	rc = nla_put_u8(skb, OVS_TT_FLOW_ATTR_PORT, port);
 	if (rc != 0) {
  		goto out;
 	}
 	
	rc = nla_put_u8(skb, OVS_TT_FLOW_ATTR_ETYPE, etype);
 	if (rc != 0) {
  		goto out;
 	}
	
	//Finalize the message
 	genlmsg_end(skb, msg_head);

    	//Send the message back
 	rc = genlmsg_unicast(genl_info_net(info), skb, info->snd_portid);
 	if (rc != 0) {
  		goto out;
 	}
 	return 0;

out:
 	printk(KERN_CRIT "An error occured in doc_exmpl_echo:\n");
 	return 0;
}

//Commands: mapping between the command enumeration and the actual function
struct genl_ops doc_exmpl_gnl_ops_echo[] = {
	{
		.cmd = OVS_TT_FLOW_CMD_NEW,
 		.flags = 0,
 		.policy = doc_exmpl_genl_policy,
 		.doit = doc_exmpl_echo,
 		.dumpit = NULL,
	}
};

static int __init gnKernel_init(void) 
{
 	int rc;
 	printk(KERN_CRIT "Generic Netlink Example Module inserted.\n");
        
 	//Register a generic netlink family with ops
 	rc = genl_register_family_with_ops(&doc_exmpl_gnl_family, doc_exmpl_gnl_ops_echo);
 	if (rc != 0) {
  		printk(KERN_CRIT "Register family with ops: %i\n",rc);
  		genl_unregister_family(&doc_exmpl_gnl_family);
  		goto failure;
 	}
 	return 0; 
failure:
 	printk(KERN_CRIT "An error occured while inserting the generic netlink example module\n");
 	return -1;
}

static void __exit gnKernel_exit(void) 
{
 	int ret;
 	printk(KERN_CRIT "Generic Netlink Example Module unloaded.\n");
 
    	//Unregister the family
 	ret = genl_unregister_family(&doc_exmpl_gnl_family);
 	if(ret !=0) {
  		printk(KERN_CRIT "Unregister family %i\n",ret);
  		return;
 	}
}

module_init(gnKernel_init);
module_exit(gnKernel_exit);
MODULE_LICENSE("GPL");
