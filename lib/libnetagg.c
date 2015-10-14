#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#define __USE_GNU
#include <string.h>
#include <errno.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <linux/unistd.h>

#include <linux/netagg.h>
#include <netagg.h>
#include <fcntl.h>
#include <linux/types.h>


int netagg_open(void) {

	int fd = open("/dev/netagg", 'w');

	if (fd < 0)
		return -fd;

	return fd;
}


int netagg_close(int fd) {
	
	int ret = 0;
	ret = close(fd);
	return ret;
}

/* Append rule sender side. This will diverge a subset of the TCP packets according to the binary send vector */
int netagg_append_sender(int hw_fd, __be64 vector, __be32 pri_daddr, __be32 sec_daddr, __be16 pri_port, __be16 sec_port)
{
	int ret = 0;
	rule_t ioctl_args;
	memset(&ioctl_args, 0, sizeof(ioctl_args));
	
	ioctl_args.binary_vector = vector;
	ioctl_args.pri_daddr = pri_daddr;
	ioctl_args.sec_daddr = sec_daddr;
	ioctl_args.pri_port = pri_port;
	ioctl_args.sec_port = sec_port;

	ret = ioctl(hw_fd, NETAGG_IOCTL_APPEND_SENDER, &ioctl_args);

	return ret;
}
/* Append rule receiver side. This will mearge all TCP packets matching sec_daddr and sec_port to pri_daddr and pri_port */
int netagg_append_receiver(int hw_fd, __be32 pri_daddr, __be32 sec_daddr, __be16 pri_port, __be16 sec_port)
{
	int ret = 0;
	rule_t ioctl_args;
	memset(&ioctl_args, 0, sizeof(ioctl_args));
	
	ioctl_args.pri_daddr = pri_daddr;
	ioctl_args.sec_daddr = sec_daddr;
	ioctl_args.pri_port = pri_port;
	ioctl_args.sec_port = sec_port;

	ret = ioctl(hw_fd, NETAGG_IOCTL_APPEND_RECEIVER, &ioctl_args);

	return ret;
}
/* Flush sender side rules */
int netagg_flush_sender(int hw_fd)
{
	int ret = 0;
	ret = ioctl(hw_fd, NETAGG_IOCTL_FLUSH_SENDER);
	return ret;
}
/* Flush receiver side rules */
int netagg_flush_receiver(int hw_fd)
{
	int ret = 0;
	ret = ioctl(hw_fd, NETAGG_IOCTL_FLUSH_RECEIVER);
	return ret;
}
