#include "../inc/traceroute.h"

/*
 * Set ip struct with IPv4 informations
*/
void	set_ipv4(void)
{
	env.ip.type = 4;
	env.ip.protocol = IPPROTO_IP;
	env.ip.family = AF_INET;
	env.ip.addrlen = INET_ADDRSTRLEN;
	env.ip.icmp = IPPROTO_ICMP;
	env.ip.header_size = 20;
}

/*
 * Bind sending socket to retrieve port (and then use it in ICMP reply)
 * getsockname() to retrieve informations about socket
*/
void	bind_sendsock_ipv4(void)
{
	struct sockaddr_in	bindaddr;
	struct sockaddr_in	getport;
	socklen_t		size;

	bzero(&bindaddr, sizeof(bindaddr));
	bindaddr.sin_family = env.ip.family;
	bindaddr.sin_addr.s_addr = INADDR_ANY;

	if (bind(env.sendsock, (struct sockaddr *)&bindaddr, (socklen_t)sizeof(bindaddr)) != 0)
		error_exit("bind sending socket failed");

	bzero(&getport, sizeof(getport));
	size = sizeof(getport);
	if (getsockname(env.sendsock, (struct sockaddr *)&getport, &size) != 0)
		error_exit("retreiving source port failed\n");
	env.sport = ntohs(getport.sin_port);
}

/*
 * Set IPv4 sending structure with family, ip adress and port
 * Increment port for next UDP datagram
 * Set global pointer to sending structure (used for both IPv4 and IPv6)
*/
void	set_sendaddr_ipv4(void)
{
	struct sockaddr_in	*sendaddr;

	if (!(sendaddr = (struct sockaddr_in*)malloc(sizeof(struct sockaddr_in))))
		error_exit("sending structure allocation");

	bzero(sendaddr, sizeof(struct sockaddr_in));
	sendaddr->sin_family = env.ip.family;
	sendaddr->sin_addr = ((struct sockaddr_in *)env.target.info->ai_addr)->sin_addr;
	sendaddr->sin_port = htons(env.dport);

	increment_port();
	env.sendaddr = (struct sockaddr*)sendaddr;
}

/*
 * Set IPv4 reception structure to retrieve informations about incoming packets
 * Set global pointer to reception structure (used for both IPv4 and IPv6)
*/
void	set_recvaddr_ipv4(void)
{
	struct sockaddr_in	*recvaddr;

	if (!(recvaddr = (struct sockaddr_in*)malloc(sizeof(struct sockaddr_in))))
		error_exit("receiving structure allocation");
	bzero(recvaddr, sizeof(struct sockaddr_in));

	env.recvaddr = (struct sockaddr*)recvaddr;
}
