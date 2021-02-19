#include "traceroute.h"

void display_addr_info(struct addrinfo *addr)
{
    int count;

    count = 0;
    printf("\n=================== ADDR INFOS ===================\n");
    while (addr)
    {
        printf("\n-------------- %d --------------\n", count++);
        printf("ai_flags = %d\n", addr->ai_flags);
        printf("ai_family = %d\n", addr->ai_family);
        printf("ai_socktype = %d\n", addr->ai_socktype);
        printf("ai_protocol = %d\n", addr->ai_protocol);
        printf("ai_addrlen = %u\n", addr->ai_addrlen);
        printf("ai_canonname = %s\n", addr->ai_canonname);
        printf("res->ai_addr->sa_family = %hhu\n", addr->ai_addr->sa_family);
        printf("--------------------------------\n");
        addr = addr->ai_next;
    }
    printf("\n==================================================\n\n");
}

void error_exit(char *error)
{
    fprintf(stderr, "ft_traceroute: error exit: %s\n", error);
    exit(1);
}

void display_ttl(void)
{
    char display_ttl[6];

    if (env.ttl < 10)
        sprintf(display_ttl, " %d  ", env.ttl);
    else
        sprintf(display_ttl, "%d  ", env.ttl);
        
    write(1, &display_ttl[0], ft_strlen(display_ttl));
}

void set_ttl(void)
{
    if ((setsockopt(env.sendsock, env.ip.protocol, IP_TTL, &env.ttl, (socklen_t)(sizeof(env.ttl)))) < 0)
        error_exit("TTL setup");
    display_ttl();
}

long double	get_ts(void)
{
	struct	timeval tv;
	struct	timezone tz;

	gettimeofday(&tv, &tz);
	return (((long double)tv.tv_sec * 1000) + ((long double)tv.tv_usec / 1000));
}




void send_datagram(void)
{
    char sendbuf[150];
    int ret;

    bzero(&sendbuf, sizeof(sendbuf));

    (env.ip.type == 4) ? set_sendaddr_ipv4() : set_sendaddr_ipv6();

    
    socklen_t size;
    size = (env.ip.type == 4) ? sizeof(struct sockaddr_in) : sizeof(struct sockaddr_in6);
    // printf("CHECK 3\n");


    char send_addr[env.ip.addrlen];
    bzero(&send_addr, sizeof(send_addr));
    // printf("CHECK 4\n");

    if (env.ip.type == 4)
        inet_ntop(env.ip.family, &(((struct sockaddr_in*)env.sendaddr)->sin_addr), &(send_addr[0]), env.ip.addrlen);
    else
        inet_ntop(env.ip.family, &(((struct sockaddr_in6*)env.sendaddr)->sin6_addr), &(send_addr[0]), env.ip.addrlen);
    // printf("CHECK 5\n");

    // printf(">>>>>>>> send ip: %s\n", send_addr);

    // printf("AVANT SEND\n");
    env.time.ts_before = get_ts();
    // printf("SIZE = %u\n", size);
    if ((ret = sendto(env.sendsock, &sendbuf[0], sizeof(sendbuf), 0, env.sendaddr, size)) < 0)
    {
        printf("SEND FAILED ret = %d\n", ret);
        printf("ERRNO = %d\n", errno);
        perror("SEND ERROR: ");
    }
    free(env.sendaddr);
    env.sendaddr = NULL;
}

void set_select(void)
{
    FD_ZERO(&(env.read_set));
    FD_SET(env.recvsock, &(env.read_set));
    bzero(&(env.time.to), sizeof(env.time.to));
    env.time.to.tv_sec = 5;
}

int ttl_exceeded(int type, int code)
{
    if (type == 11 && code == 0)
        return (1);
    return (0);
}

int dest_port_unreach(int type, int code)
{
    if (type == 3 && code == 3)
        return (1);
    return (0);
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

void display_node(int pos)
{
    char node[1088];

    sprintf(node, "%s (%s)  %.3LF ms", env.node[pos].hostname ,env.node[pos].ip, env.node[pos].interval);
    write(1, &node[0], ft_strlen(node));
    env.node[pos].displayed = 1;
}

void display_interval(int pos)
{
    char interval[16];

    sprintf(interval, "  %.3LF ms", env.node[pos].interval);
    write(1, &interval[0], ft_strlen(interval));

    env.node[pos].displayed = 1;
}

int is_same_node(int first_pos, int second_pos)
{

    if (ft_strncmp(&(env.node[first_pos].ip[0]), &(env.node[second_pos].ip[0]), 16) == 0)
        return (1);
    return (0);
}

void display_remaining_node(void)
{
    int pos;

    pos = 0;
    while (pos < 3)
    {
        if (env.node[pos].on && !(env.node[pos].displayed))
        {
            if (pos == 1)
            {
                if (is_same_node(0, 2))
                    display_interval(2);
                write(1, "\n    ", 5);
                display_node(pos);
            }
            else if (pos == 2)
            {
                if (is_same_node(1, 2))
                    display_interval(pos);
                else
                {
                    write(1, "\n    ", 5);
                    display_node(pos);
                }
            }
        }
        pos++;
    }
    write(1, "\n", 1);
}



void add_node_info(void)
{
    int pos;

    pos = 0;
    while (pos < 2 && env.node[pos].on)
        pos++;

    env.node[pos].on = 1;
    get_ip_addr(env.recvaddr, NODE, pos);
    get_hostname(env.recvaddr, NODE, pos);
    env.node[pos].interval = (env.time.ts_after - env.time.ts_before);

    if (pos == 0)
        display_node(pos);
    else if (pos == 1 && is_same_node(0, pos))
        display_interval(pos);
}

int is_end(int type, int code)
{
    if (type == 3 && code == 3)
        return (1);
    return (0);
}

void program_exit(void)
{

    exit(0);
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
    {
        printf("ERRNO = %d\n", errno);
        perror("RECV ERROR: ");
    }
    env.time.ts_after = get_ts();

    icmp_r = (struct icmp_h *)&buf[20];

    if (ttl_exceeded(icmp_r->type, icmp_r->code) || dest_port_unreach(icmp_r->type, icmp_r->code))
        add_node_info();

    if (is_end(icmp_r->type, icmp_r->code))
        env.end = 1;


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

void traceroute_loop(void)
{
    int count;

    count = 0;

    while (1)
    {
        count = 0;

        set_ttl();
        bzero(&env.node, sizeof(env.node));
        while (count < 3)
        {
            send_datagram();
            set_select();
            select((env.recvsock + 1), &(env.read_set), NULL, NULL, &(env.time.to));
            if (FD_ISSET(env.recvsock, &(env.read_set)))
                manage_icmp_reply();
            else
                manage_no_reply(count);

            count++;
        }
        display_remaining_node();
        if (env.end)
            program_exit();
        env.ttl++;
    }
}

void verify_user_rights(void)
{
    if (getuid() != 0)
    {
        fprintf(stderr, "ft_traceroute: you must be root to run this program\n");
        exit(1);
    }
}

void create_socket(void)
{
    env.sendsock = -1;
    env.recvsock = -1;

    printf("socket env.ip.family = %d\n", env.ip.family);

    if ((env.sendsock = socket(env.ip.family, SOCK_DGRAM, IPPROTO_UDP)) < 0)
        error_exit("UDP socket creation");

    if ((env.recvsock = socket(env.ip.family, SOCK_RAW, env.ip.icmp)) < 0)
        error_exit("ICMP socket creation");
}

void addrinfo_error(int code)
{
    error_exit("ADDRINFO");
}

void get_targetinfo(void)
{
    struct addrinfo hints;
    int ret;

    bzero(&hints, sizeof(hints));
    bzero(&env.target.info, sizeof(env.target.info));

    hints.ai_family = env.ip.family;
    hints.ai_socktype = SOCK_DGRAM;
    hints.ai_protocol = IPPROTO_UDP;

    printf("env.args.target = %s\n", env.args.target);
    if ((ret = getaddrinfo(env.args.target, NULL, &hints, &(env.target.info))) != 0)
        addrinfo_error(ret);

    char send_addr[env.ip.addrlen];
    bzero(&send_addr, sizeof(send_addr));

    if (env.ip.type == 4)
        inet_ntop(env.ip.family, &(((struct sockaddr_in*)env.target.info->ai_addr)->sin_addr), &(send_addr[0]), env.ip.addrlen);
    else
        inet_ntop(env.ip.family, &(((struct sockaddr_in6*)env.target.info->ai_addr)->sin6_addr), &(send_addr[0]), env.ip.addrlen);

    printf(">>>>>>>>>> send_addr = %s\n", send_addr);
}

void display_help(int code)
{
    printf("EXIT DISPLAY TODO\n");
    exit(code);
}

void	display_wrong_option(char option)
{
	printf("Ping: invalid option -- '%c'\n", option);
	display_help(1);
}

int main(int argc, char **argv)
{
    env.port = 33435;
    env.args.target = NULL;

    verify_user_rights();
    set_ipv4();
    parse_args(argc, argv);

    create_socket();
    get_targetinfo();



    traceroute_loop();
}