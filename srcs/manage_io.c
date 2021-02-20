#include "../inc/traceroute.h"

void send_datagram(void)
{
    char data[env.args.packet_size - 28];
    socklen_t size;
    int tosend;
    int sent;
    int total;

    bzero(&data, sizeof(data));
    (env.ip.type == 4) ? set_sendaddr_ipv4() : set_sendaddr_ipv6();
    size = (env.ip.type == 4) ? sizeof(struct sockaddr_in) : sizeof(struct sockaddr_in6);

    tosend = sizeof(data);
    total = tosend;
    sent = 0;
    env.time.ts_before = get_ts();
    while ((sent += sendto(env.sendsock, &data[0], tosend, 0, env.sendaddr, size)) < total)
    {
        if (sent == -1)
            error_exit("sendto() failed");
        tosend -= sent;
    }

    free(env.sendaddr);
    env.sendaddr = NULL;
}

void manage_icmp_reply(void)
{
    char buf[1024];
    struct icmp_h *icmp_r;
    socklen_t size;

    bzero(&buf, sizeof(buf));
    (env.ip.type == 4) ? set_recvaddr_ipv4() : set_recvaddr_ipv6();
    size = (env.ip.type == 4) ? sizeof(struct sockaddr_in) : sizeof(struct sockaddr_in6);

    if (recvfrom(env.recvsock, buf, sizeof(buf), 0, env.recvaddr, &size) < 0)
        error_exit("recvfrom() failed");
    env.time.ts_after = get_ts();

    icmp_r = (struct icmp_h *)&buf[20];
    if (ttl_exceeded(icmp_r->type, icmp_r->code) || dest_port_unreach(icmp_r->type, icmp_r->code))
        add_node_info();

    if (dest_port_unreach(icmp_r->type, icmp_r->code))
        env.target.reached = 1;

    free(env.recvaddr);
    env.recvaddr = NULL;
}

void manage_no_reply(int pos)
{
    static int prev = 2;

    if (pos && prev != (pos - 1))
        write(1, " * ", 4);
    else
        write(1, "* ", 3);
    prev = pos;
}

