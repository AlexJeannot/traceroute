#include "../inc/traceroute.h"

void set_ipv4(void)
{
	env.ip.type = 4;
	env.ip.protocol = IPPROTO_IP;
	env.ip.family = AF_INET;
    env.ip.addrlen = INET_ADDRSTRLEN;
	env.ip.icmp = IPPROTO_ICMP;
}

void set_sendaddr_ipv4(void)
{
    struct sockaddr_in *sendaddr;

    if (!(sendaddr = (struct sockaddr_in*)malloc(sizeof(struct sockaddr_in))))
        error_exit("sending structure allocation");
    // printf("CHECK 1.1\n");

    // printf("sizeof(*sendaddr) = %lu\n", sizeof(struct sockaddr_in));
    bzero(sendaddr, sizeof(struct sockaddr_in));
    sendaddr->sin_family = env.ip.family;
    sendaddr->sin_addr = ((struct sockaddr_in *)env.target.info->ai_addr)->sin_addr;
    sendaddr->sin_port = htons(env.port);
    // printf("CHECK 1.2\n");

    increment_port();
    env.sendaddr = (struct sockaddr*)sendaddr;
}

void set_recvaddr_ipv4(void)
{
    struct sockaddr_in *recvaddr;

    if (!(recvaddr = (struct sockaddr_in*)malloc(sizeof(struct sockaddr_in))))
        error_exit("receiving structure allocation");
    bzero(recvaddr, sizeof(struct sockaddr_in));

    env.recvaddr = (struct sockaddr*)recvaddr;
}