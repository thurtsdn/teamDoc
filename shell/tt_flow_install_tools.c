/*
 * It is ai tools which can send tt flow to the datapath of ovs.
 * Introduction:
 * 1. complie the program:
 *     "~$ gcc tt_flow_install_tools.c -c tfit"
 * 2. type this command in the terminal:
 *     "~$ ./tfit 1 2 3 4 5 6 7"
 *     it means that sending a tt_flow like this:      
 *         uint8_t port = 1,
 *         uint8_t etype = 3,
 *         uint8_t flow_id = 3,
 *         uint32_t scheduled_time = 4,
 *         uint32_t period = 5,
 *         uint32_t buffer_id = 6,
 *         uint32_t pkt_size = 7,
 */

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <poll.h>
#include <string.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <signal.h>
#include <stdint.h>
#include <linux/genetlink.h>

//Code based on http://people.ee.ethz.ch/~arkeller/linux/multi/kernel_user_space_howto-3.html

/* Generic macros for dealing with netlink sockets. Might be duplicated
 * elsewhere. It is recommended that commercial grade applications use
 * libnl or libnetlink and use the interfaces provided by the library
 */
#define GENLMSG_DATA(glh) ((void *)(NLMSG_DATA(glh) + GENL_HDRLEN))
#define GENLMSG_PAYLOAD(glh) (NLMSG_PAYLOAD(glh, 0) - GENL_HDRLEN)
#define NLA_DATA(na) ((void *)((char*)(na) + NLA_HDRLEN))

#define OVS_TT_FAMILY "ovs_tt"
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
};


/* commands: enumeration of all commands (functions), 
 * used by userspace application to identify command to be executed
 */
enum ovs_tt_cmd {
    OVS_TT_FLOW_CMD_NEW,
    OVS_TT_FLOW_CMD_DEL,
    OVS_TT_FLOW_CMD_GET,
};

struct ovs_header {
    int dp_ifindex;
};

/* Message structure for ONF_ET_TT_FLOW_MDOD. */
struct tt_flow {
    uint16_t port; /* The entry related port. */
    uint16_t etype; /* Send entry or receive entry. */
    uint16_t flow_id; /* The identify of a flow. */
    uint64_t scheduled_time; /* The scheduled time that the flow packet is received or sent. */
    uint64_t period; /* The scheduling period. */
    uint16_t buffer_id; /* Buffered packet to apply to. */
    uint16_t pkt_size; /* The flow packet size. */
};

//memory for netlink request and response messages - headers are included
struct message { 			
    struct nlmsghdr nh;
    struct genlmsghdr gh;
    struct ovs_header oh;
    char payload[256];
};

int main(int argc, char* argv[]) {
	int sd;
	struct sockaddr_nl nladdr;
	struct message request;
	struct message reply;
	struct nlattr *nla;
	int len;
	int nl_family_id; 		

	//Step 1: Open the socket. Note that protocol = NETLINK_GENERIC
    sd = socket(AF_NETLINK, SOCK_RAW, NETLINK_GENERIC);
    if (sd < 0) {
  		perror("socket()");
  		return -1;
    }

	//Step 2: Bind the socket.
 	memset(&nladdr, 0, sizeof(nladdr));
 	nladdr.nl_family = AF_NETLINK;
 	nladdr.nl_groups = 0;
 	if (bind(sd, (struct sockaddr *) &nladdr, sizeof(nladdr)) < 0) {
  		perror("bind()");
  		close(sd);
  		return -1;
 	}

	//Step 3. Resolve the family ID corresponding to the string "CONTROL_EXMPL"
	nla = (struct nlattr *) GENLMSG_DATA(&request);
	nla->nla_type = CTRL_ATTR_FAMILY_NAME;
	nla->nla_len = NLA_HDRLEN + strlen(OVS_TT_FAMILY) + 1;
	strcpy((char *)NLA_DATA(nla), OVS_TT_FAMILY); 
    	
	request.gh.cmd = CTRL_CMD_GETFAMILY;
    request.gh.version = 1;
    
    request.nh.nlmsg_len = NLMSG_HDRLEN + GENL_HDRLEN + NLA_ALIGN(nla->nla_len);
    request.nh.nlmsg_type = GENL_ID_CTRL;
    request.nh.nlmsg_flags = NLM_F_REQUEST;
    request.nh.nlmsg_seq = 0;
    request.nh.nlmsg_pid = 0;

 	//Send the family ID request message to the netlink controller
 	len = sendto(sd, (char *) &request, request.nh.nlmsg_len,
  				0, (struct sockaddr *) &nladdr, sizeof(nladdr));
 	if (len != request.nh.nlmsg_len) {
  		perror("sendto()");
  		close(sd);
  		return -1;
    }

 	//Wait for the response message
    len = recv(sd, &reply, sizeof(reply), 0);
    if (len < 0) {
     	perror("recv()");
       	return -1;
    }	

    //Validate response message
    if (!NLMSG_OK((&reply.nh), len)) {
       	fprintf(stderr, "family ID request : invalid message\n");
       	return -1;
    }
    	
	
	if (reply.nh.nlmsg_type == NLMSG_ERROR) { //error
       	fprintf(stderr, "family ID request : receive error\n");
       	return -1;
    }

    //Extract family ID
    nla = (struct nlattr *) GENLMSG_DATA(&reply);
    nla = (struct nlattr *) ((char *) nla + NLA_ALIGN(nla->nla_len));
    if (nla->nla_type == CTRL_ATTR_FAMILY_ID) {
       	nl_family_id = *(__u16 *) NLA_DATA(nla);
    }

    //Step 4. Send own custom message
    memset(&request, 0, sizeof(request));
    memset(&reply, 0, sizeof(reply));

    request.nh.nlmsg_type = nl_family_id;
    request.nh.nlmsg_flags = NLM_F_REQUEST;
    request.nh.nlmsg_seq = 60;
    request.nh.nlmsg_pid = getpid();
    request.nh.nlmsg_len = NLMSG_HDRLEN;

    request.gh.cmd = OVS_TT_FLOW_CMD_NEW; 
    request.gh.version = OVS_TT_VERSION;
    request.nh.nlmsg_len += GENL_HDRLEN;

    request.oh.dp_ifindex = 0;
    request.nh.nlmsg_len += sizeof(struct ovs_header);

    struct tt_flow tf = {
        .port = (argc > 1)? (uint16_t)atoi(argv[1]) : 0,
        .etype = (argc > 2)? (uint16_t)atoi(argv[2]) : 0,
        .flow_id = (argc > 3)? (uint16_t)atoi(argv[3]) : 0,
        .scheduled_time = (argc > 4)? (uint64_t)strtoull(argv[4], NULL, 10) : 0,
        .period = (argc > 5)? (uint64_t)strtoull(argv[5], NULL, 10) : 0,
        .buffer_id = (argc > 6)? (uint16_t)atoi(argv[6]) : 0,
        .pkt_size = (argc > 7)? (uint16_t)atoi(argv[7]) : 0,
    };

    uint16_t port = tf.port;
    nla = (struct nlattr *) ((char *)&request + request.nh.nlmsg_len);
    nla->nla_type = OVS_TT_FLOW_ATTR_PORT; 
    nla->nla_len = NLA_HDRLEN + sizeof(port); 
    memcpy(NLA_DATA(nla), &port, sizeof(port));
    request.nh.nlmsg_len += NLA_ALIGN(nla->nla_len);

    uint16_t etype = tf.etype;
    nla = (struct nlattr *) ((char *)&request + request.nh.nlmsg_len);
    nla->nla_type = OVS_TT_FLOW_ATTR_ETYPE; 
    nla->nla_len = NLA_HDRLEN + sizeof(etype); 
    memcpy(NLA_DATA(nla), &etype, sizeof(etype));
    request.nh.nlmsg_len += NLA_ALIGN(nla->nla_len);

    uint16_t flow_id = tf.flow_id;
    nla = (struct nlattr *) ((char *)&request + request.nh.nlmsg_len);
    nla->nla_type = OVS_TT_FLOW_ATTR_FLOW_ID; 
    nla->nla_len = NLA_HDRLEN + sizeof(flow_id); 
    memcpy(NLA_DATA(nla), &flow_id, sizeof(flow_id));
    request.nh.nlmsg_len += NLA_ALIGN(nla->nla_len);

    uint64_t scheduled_time = tf.scheduled_time;
    printf("%llu\n", scheduled_time);
    nla = (struct nlattr *) ((char *)&request + request.nh.nlmsg_len);
    nla->nla_type = OVS_TT_FLOW_ATTR_SCHEDULED_TIME; 
    nla->nla_len = NLA_HDRLEN + sizeof(scheduled_time); 
    memcpy(NLA_DATA(nla), &scheduled_time, sizeof(scheduled_time));
    request.nh.nlmsg_len += NLA_ALIGN(nla->nla_len);

    uint64_t period = tf.period;
    nla = (struct nlattr *) ((char *)&request + request.nh.nlmsg_len);
    nla->nla_type = OVS_TT_FLOW_ATTR_PERIOD; 
    nla->nla_len = NLA_HDRLEN + sizeof(period); 
    memcpy(NLA_DATA(nla), &period, sizeof(period));
    request.nh.nlmsg_len += NLA_ALIGN(nla->nla_len);

    uint16_t buffer_id = tf.buffer_id;
    nla = (struct nlattr *) ((char *)&request + request.nh.nlmsg_len);
    nla->nla_type = OVS_TT_FLOW_ATTR_BUFFER_ID; 
    nla->nla_len = NLA_HDRLEN + sizeof(buffer_id); 
    memcpy(NLA_DATA(nla), &buffer_id, sizeof(buffer_id));
    request.nh.nlmsg_len += NLA_ALIGN(nla->nla_len);

    uint16_t pkt_size = tf.pkt_size;
    nla = (struct nlattr *) ((char *)&request + request.nh.nlmsg_len);
    nla->nla_type = OVS_TT_FLOW_ATTR_PKT_SIZE; 
    nla->nla_len = NLA_HDRLEN + sizeof(pkt_size); 
    memcpy(NLA_DATA(nla), &pkt_size, sizeof(pkt_size));
    request.nh.nlmsg_len += NLA_ALIGN(nla->nla_len);

 	//Send the custom message
 	len = sendto(sd, (char *) &request, request.nh.nlmsg_len,
  				0, (struct sockaddr *) &nladdr, sizeof(nladdr));
 	if (len != request.nh.nlmsg_len) {
  		perror("sendto()");
  		close(sd);
  		return -1;
   	}

    /*
   	//Receive reply from kernel
   	len = recv(sd, &reply, sizeof(reply), 0);
   	if (len < 0) {
       	perror("recv()");
       	return -1;
   	}

 	//Validate response message
   	if (reply.nh.nlmsg_type == NLMSG_ERROR) { 
       	printf("Error while receiving reply from kernel: NACK Received\n");
       	close(sd);
       	return -1;
   	}
   	if (len < 0) {
       	printf("Error while receiving reply from kernel\n");
       	close(sd);
       	return -1;
   	}
   	if (!NLMSG_OK((&reply.nh), len)) {
       	printf("Error while receiving reply from kernel: Invalid Message\n");
       	close(sd);
   		return -1;
 	}

   	//Parse the reply message
   	nla = (struct nlattr *) GENLMSG_DATA(&reply);
   	printf("Kernel replied: OVS_TT_FLOW_ATTR_PORT: %d\n", *(int *)NLA_DATA(nla));

	nla = (struct nlattr *)((char *)nla + nla->nla_len);
	printf("Kernel replied: OVS_TT_FLOW_ATTR_ETYPE: %d\n", *(int *)NLA_DATA(nla));
    */

	//Step 5. Close the socket and quit
   	close(sd);
   	return 0;
}
