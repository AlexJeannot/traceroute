#include "../inc/traceroute.h"

void verify_user_rights(void)
{
    if (getuid() != 0)
        error_exit("you must be root to run this program");
}

int ttl_exceeded(int type, int code)
{
    if (env.ip.type == 4 && type == 11 && code == 0)
        return (1);
    if (env.ip.type == 6 && type == 3)
        return (1);
    return (0);
}

int dest_port_unreach(int type, int code)
{
    if (env.ip.type == 4 && type == 3 && code == 3)
        return (1);
    if (env.ip.type == 6 && type == 1)
        return (1);
    return (0);
}

int is_same_node(int first_pos, int second_pos)
{

    if (ft_strncmp(&(env.node[first_pos].ip[0]), &(env.node[second_pos].ip[0]), 16) == 0)
        return (1);
    return (0);
}

struct icmp* is_right_packet(char *data)
{
    struct ip *res_iphdr;
    struct icmp *icmphdr;
    struct ip *req_iphdr;
    struct udphdr *udphdr;
    int16_t offset;

    res_iphdr = (struct ip *)&data[0];
    offset = res_iphdr->ip_hl * 4;
    icmphdr = (env.ip.type == 4) ? (struct icmp *)&data[offset] : (struct icmp *)&data[0];
    offset = (env.ip.type == 4) ? offset + 8 : 8;
    req_iphdr = (struct ip *)&data[offset];
    offset += (env.ip.type == 4) ? req_iphdr->ip_hl * 4 : 40;
    udphdr = (struct udphdr *)&data[offset];

    if (ntohs(udphdr->uh_sport) == env.sport)
        return (icmphdr);
    return (NULL);
}

