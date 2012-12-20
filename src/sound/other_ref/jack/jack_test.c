#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <sys/un.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <linux/types.h>
#include <linux/netlink.h>
#include <errno.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>

#define BUF_LEN         1024

#define HW_EVENT_GROUP	    2
#define HWE_DEF_PRIORITY	1
#define HWE_HIGH_PRIORITY	0

struct mxc_hw_event {
	unsigned int event;
	int args;
};

static int
u_nl_sock(int proto, int grp) 
{
	int s;
	struct sockaddr_nl snl; 

	bzero(&snl, sizeof(struct sockaddr_nl)); 
	snl.nl_family = AF_NETLINK; 
	snl.nl_pid = getpid(); 
	snl.nl_groups = grp;

//	s = socket(AF_NETLINK, SOCK_RAW, proto);
	s = socket(AF_NETLINK, SOCK_DGRAM, proto);
	if (s == -1)  
	{ 
		perror("socket"); 
		return -1; 
	} 

	if (bind(s, (struct sockaddr *)&snl, sizeof(struct sockaddr_nl)) < 0)
	{ 
		perror("bind"); 
		close(s); 
		return -1; 
	} 
	return s; 
} 

static int
u_nl_recv(int fd, void *from, void *buf, int n)
{
	struct sockaddr_nl  dest_addr;
	struct nlmsghdr *nlh = NULL;
	struct iovec iov;
	struct msghdr msg;
	char   nl_buf[BUF_LEN];
	int ret = 0;

    memset(nl_buf, 0, BUF_LEN);
	iov.iov_base = (void *)nl_buf;
	iov.iov_len = BUF_LEN;
	msg.msg_name = (void *)&dest_addr;
	msg.msg_namelen = sizeof(dest_addr);
	msg.msg_iov = &iov;
	msg.msg_iovlen = 1;
	printf("--- Waiting for message from kernel ---\n");
	ret = recvmsg(fd, &msg, 0);
	if (ret > 0)
	{
	    nlh = (struct nlmsghdr *)nl_buf;
	    printf(" Received message payload len: %d\n", nlh->nlmsg_len);
	    memcpy(buf, NLMSG_DATA(nlh), n);
	    if (from)
	    {
	    	memcpy(from, &dest_addr, sizeof(struct sockaddr_nl));
	    }
	}
	return ret;
}

int
main(int argc, char *argv[])
{
	int fd;
	struct mxc_hw_event ev;

	fd = u_nl_sock(NETLINK_USERSOCK, HW_EVENT_GROUP);
	if(fd < 0)
	{
		printf("--- can not create a netlink socket ---\n");
		exit(1);
	}
    printf("--- enter loop ---\n");
	while(1)
	{
		if (u_nl_recv(fd, NULL, &ev, sizeof(struct mxc_hw_event)) > 0)
		{
			printf("--- netlink message: event = %d, args = %d ---\n", ev.event, ev.args);
		}
	}
	close(fd);

	exit(0);
}



