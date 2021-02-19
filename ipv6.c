#include "traceroute.h"

void set_ipv6(void)
{
	env.ip.type = 6;
	env.ip.protocol = IPPROTO_IPV6;
	env.ip.family = AF_INET6;
    env.ip.addrlen = INET6_ADDRSTRLEN;
	env.ip.icmp = IPPROTO_ICMPV6;
}

void set_sendaddr_ipv6(void)
{
    struct sockaddr_in6 *sendaddr;

    if (!(sendaddr = (struct sockaddr_in6*)malloc(sizeof(struct sockaddr_in6))))
        error_exit("sending structure allocation");

    bzero(sendaddr, sizeof(struct sockaddr_in6));
    sendaddr->sin6_family = env.ip.family;
    sendaddr->sin6_addr = ((struct sockaddr_in6 *)env.target.info->ai_addr)->sin6_addr;
    sendaddr->sin6_port = htons(env.port);

    env.port++;
    env.sendaddr = (struct sockaddr*)sendaddr;
}

void set_recvaddr_ipv6(void)
{
    struct sockaddr_in6 *recvaddr;

    if (!(recvaddr = (struct sockaddr_in6*)malloc(sizeof(struct sockaddr_in6))))
        error_exit("receiving structure allocation");
    bzero(recvaddr, sizeof(struct sockaddr_in6));

    env.recvaddr = (struct sockaddr*)recvaddr;
}