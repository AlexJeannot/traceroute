#include "../inc/traceroute.h"

void set_ttl(void)
{
    int set_ttl;

    set_ttl = env.ttl;
    if ((setsockopt(env.sendsock, env.ip.protocol, IP_TTL, (char *)&set_ttl, (socklen_t)(sizeof(set_ttl)))) < 0)
        error_exit("TTL setup failed");
}

void get_ip_addr(struct sockaddr* info, int type, int pos)
{
    char *addr;

    addr = (type == NODE) ? &(env.node[pos].ip[0]) : &(env.target.ip[0]);
    if (env.ip.type == 4)
        inet_ntop(env.ip.family, &(((struct sockaddr_in*)info)->sin_addr), addr, env.ip.addrlen);
    else
        inet_ntop(env.ip.family, &(((struct sockaddr_in6*)info)->sin6_addr), addr, env.ip.addrlen);
}

void get_hostname(struct sockaddr* info, int type, int pos)
{
    char *hostname;

    hostname = (type == NODE) ? &(env.node[pos].hostname[0]) : &(env.target.hostname[0]);
    if (getnameinfo(info, (socklen_t)sizeof(info), hostname, NI_MAXHOST, NULL, 0, 0) != 0)
        hostname = "N/A";
}

void create_socket(void)
{
    env.sendsock = -1;
    env.recvsock = -1;

    if ((env.sendsock = socket(env.ip.family, SOCK_DGRAM, IPPROTO_UDP)) < 0)
        error_exit("UDP socket creation failed");

    if ((env.recvsock = socket(env.ip.family, SOCK_RAW, env.ip.icmp)) < 0)
        error_exit("ICMP socket creation failed");
}

void get_targetinfo(void)
{
    struct addrinfo hints;
    int ret;

    bzero(&hints, sizeof(hints));

    hints.ai_family = env.ip.family;
    hints.ai_socktype = SOCK_DGRAM;
    hints.ai_protocol = IPPROTO_UDP;

    if ((ret = getaddrinfo(env.args.target, NULL, &hints, &(env.target.info))) != 0)
        addrinfo_error(env.args.target, ret);

    get_ip_addr(env.target.info->ai_addr, TARGET, 0);
    get_hostname(env.target.info->ai_addr, TARGET, 0);
}