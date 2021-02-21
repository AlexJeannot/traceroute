#include "../inc/traceroute.h"

void set_ipv6(void)
{
	env.ip.type = 6;
	env.ip.protocol = IPPROTO_IPV6;
	env.ip.family = AF_INET6;
    env.ip.addrlen = INET6_ADDRSTRLEN;
	env.ip.icmp = IPPROTO_ICMPV6;
}

void bind_sendsock_ipv6(void)
{
    struct sockaddr_in6 bindaddr;
    struct sockaddr_in6 getport;
    socklen_t size;

    bzero(&bindaddr, sizeof(bindaddr));
    bindaddr.sin6_family = env.ip.family;
    bindaddr.sin6_addr = in6addr_any;

    if (bind(env.sendsock, (struct sockaddr *)&bindaddr, (socklen_t)sizeof(bindaddr)) != 0)
        error_exit("bind sending socket failed");

    bzero(&getport, sizeof(getport));
    size = sizeof(getport);
    if (getsockname(env.sendsock, (struct sockaddr *)&getport, &size) != 0)
        error_exit("retreiving source port failed\n");
    env.sport = ntohs(getport.sin6_port);
    printf("env.sport = %d\n", env.sport);
}

void set_sendaddr_ipv6(void)
{
    struct sockaddr_in6 *sendaddr;

    if (!(sendaddr = (struct sockaddr_in6*)malloc(sizeof(struct sockaddr_in6))))
        error_exit("sending structure allocation");

    bzero(sendaddr, sizeof(struct sockaddr_in6));
    sendaddr->sin6_family = env.ip.family;
    sendaddr->sin6_addr = ((struct sockaddr_in6 *)env.target.info->ai_addr)->sin6_addr;
    sendaddr->sin6_port = htons(env.dport);

    increment_port();
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