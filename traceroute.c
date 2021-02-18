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

void set_ttl(void)
{
    printf("ttl = %d\n", env.ttl);
    int set_ttl = env.ttl;
    if ((setsockopt(env.sendsock, env.ip.protocol, IP_TTL, &set_ttl, (socklen_t)(sizeof(int)))) < 0)
        error_exit("TTL setup");
}


uint16_t	calcul_checksum(void *data, int size)
{
	uint64_t	checksum;
	uint16_t	*addr;

	checksum = 0;
	addr = data;
	while (size > 1)
	{
		checksum += *addr;
		addr++;
		size -= (int)sizeof(uint16_t);
	}
	if (size == 1)
		checksum += *(uint8_t*)addr;
	checksum = (checksum >> 16) + (checksum & 0xFFFF);
	checksum += (checksum >> 16);
	checksum = ~checksum;
	return ((uint16_t)checksum);
}

struct sockaddr *set_sendaddr_ipv4(void)
{
    struct sockaddr_in *sendaddr;

    if (!(sendaddr = (struct sockaddr_in*)malloc(sizeof(struct sockaddr_in))))
        error_exit("sending structure allocation");

    bzero(&(*sendaddr), sizeof(*sendaddr));
    sendaddr->sin_family = env.ip.family;
    sendaddr->sin_addr = ((struct sockaddr_in *)env.targetinfo->ai_addr)->sin_addr;
    sendaddr->sin_port = htons(env.port);

    env.port++;
    return((struct sockaddr*)sendaddr);
}

struct sockaddr *set_sendaddr_ipv6(void)
{
    struct sockaddr_in6 *sendaddr;

    if (!(sendaddr = (struct sockaddr_in6*)malloc(sizeof(struct sockaddr_in6))))
        error_exit("sending structure allocation");

    bzero(&(*sendaddr), sizeof(*sendaddr));
    sendaddr->sin6_family = env.ip.family;
    sendaddr->sin6_addr = ((struct sockaddr_in6 *)env.targetinfo->ai_addr)->sin6_addr;
    sendaddr->sin6_port = htons(env.port);

    env.port++;
    return((struct sockaddr*)sendaddr);
}


void send_datagram(void)
{
    struct sockaddr *sendaddr;
    char sendbuf[150];
    int ret;

    bzero(&sendbuf, sizeof(sendbuf));

    sendaddr = (env.ip.type == 4) ? set_sendaddr_ipv4() : set_sendaddr_ipv6();

    printf("env.ip.family = %d\n", env.ip.family);
    printf("sizeof(struct sockaddr) = %lu\n", sizeof(struct sockaddr));
    printf("sizeof(struct sockaddr_in) = %lu\n", sizeof(struct sockaddr_in));
    printf("sizeof(struct sockaddr_in6) = %lu\n", sizeof(struct sockaddr_in6));
    
    uint64_t size;
    size = (env.ip.type == 4) ? sizeof(struct sockaddr_in) : sizeof(struct sockaddr_in6);


    char send_addr[env.ip.addrlen];
    bzero(&send_addr, sizeof(send_addr));

    if (env.ip.type == 4)
        inet_ntop(env.ip.family, &(((struct sockaddr_in*)sendaddr)->sin_addr), &(send_addr[0]), env.ip.addrlen);
    else
        inet_ntop(env.ip.family, &(((struct sockaddr_in6*)sendaddr)->sin6_addr), &(send_addr[0]), env.ip.addrlen);

    printf(">>>>>>>> ip: %s\n", send_addr);

    printf("AVANT SEND\n");
    if ((ret = sendto(env.sendsock, &sendbuf[0], sizeof(sendbuf), 0, sendaddr, (socklen_t)size)) < 0)
    {
        printf("SEND FAILED ret = %d\n", ret);
        printf("ERRNO = %d\n", errno);
        perror("SEND ERROR: ");
    }
    free(sendaddr);
}

void set_select(void)
{
    FD_ZERO(&(env.read_set));
    FD_SET(env.recvsock, &(env.read_set));
    bzero(&(env.timeout), sizeof(env.timeout));
    env.timeout.tv_sec = 5;
}

struct sockaddr *set_recvaddr_ipv4(void)
{
    struct sockaddr_in *recvaddr;

    if (!(recvaddr = (struct sockaddr_in*)malloc(sizeof(struct sockaddr_in))))
        error_exit("receiving structure allocation");
    bzero(&(*recvaddr), sizeof(*recvaddr));
    recvaddr->sin_addr.s_addr = htonl(INADDR_ANY);


    return ((struct sockaddr*)recvaddr);
}

struct sockaddr *set_recvaddr_ipv6(void)
{
    struct sockaddr_in6 *recvaddr;

    if (!(recvaddr = (struct sockaddr_in6*)malloc(sizeof(struct sockaddr_in6))))
        error_exit("receiving structure allocation");
    bzero(&(*recvaddr), sizeof(*recvaddr));
    recvaddr->sin6_addr = in6addr_any;

    return ((struct sockaddr*)recvaddr);
}

void manage_icmp_reply(void)
{
    printf("\n===================== REC %d =======================\n", env.ttl);
    struct sockaddr* recvaddr;

    recvaddr = (env.ip.type == 4) ? set_recvaddr_ipv4() : set_recvaddr_ipv6();

    uint64_t size;
    size = (env.ip.type == 4) ? sizeof(struct sockaddr_in) : sizeof(struct sockaddr_in6);

    char buf[1024];
    bzero(&buf, sizeof(buf));

    int msgLen = recvfrom(env.recvsock, buf, sizeof(buf), 0, recvaddr, (socklen_t *)&size);
    if (msgLen == -1)
    {
        printf("ERRNO = %d\n", errno);
        perror("RECV ERROR: ");
    }
    printf("msglen = %d\n", msgLen);
    // printf("-------------\nbuf = %s\nmsgLen = %d\n", buf, msgLen);

    char recv_addr[env.ip.addrlen];
    bzero(&recv_addr, sizeof(recv_addr));

    if (env.ip.type == 4)
        inet_ntop(env.ip.family, &(((struct sockaddr_in*)recv_addr)->sin_addr), &(recv_addr[0]), env.ip.addrlen);
    else
        inet_ntop(env.ip.family, &(((struct sockaddr_in6*)recv_addr)->sin6_addr), &(recv_addr[0]), env.ip.addrlen);

    printf(">>>>>>>> ip: %s\n", recv_addr);

    struct icmp_h *ptr;


    ptr = (struct icmp_h *)&buf[20];
    printf("type = %d\n", ptr->type);
    printf("code = %d\n", ptr->code);


    bzero(&buf, sizeof(buf));
    msgLen = -3;
    free(recvaddr);
    printf("===================== REC %d =======================\n\n",  env.ttl);
}

void manage_no_reply(void)
{
    printf("NO REPLY\n");
}

void traceroute_loop(void)
{
    int count;

    count = 0;
    while (1)
    {
        count = 0;
        set_ttl();
        while (count < 1)
        {
            send_datagram();
            set_select();
            select((env.recvsock + 1), &(env.read_set), NULL, NULL, &(env.timeout));
            if (FD_ISSET(env.recvsock, &(env.read_set)))
            {
                manage_icmp_reply();
                // FD_CLR(env.recvsock, &(env.read_set));
            }

            else
                manage_no_reply();

            printf("env.timeout.tv_sec = %ld\n", env.timeout.tv_sec);
            printf("env.timeout.tv_usec = %ld\n", env.timeout.tv_usec);
            sleep(5);
            count++;
        }
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
    bzero(&env.targetinfo, sizeof(env.targetinfo));

    hints.ai_family = env.ip.family;
    hints.ai_socktype = SOCK_DGRAM;
    hints.ai_protocol = IPPROTO_UDP;

    printf("env.args.target = %s\n", env.args.target);
    if ((ret = getaddrinfo(env.args.target, NULL, &hints, &(env.targetinfo))) != 0)
        addrinfo_error(ret);

    char send_addr[env.ip.addrlen];
    bzero(&send_addr, sizeof(send_addr));

    if (env.ip.type == 4)
        inet_ntop(env.ip.family, &(((struct sockaddr_in*)env.targetinfo->ai_addr)->sin_addr), &(send_addr[0]), env.ip.addrlen);
    else
        inet_ntop(env.ip.family, &(((struct sockaddr_in6*)env.targetinfo->ai_addr)->sin6_addr), &(send_addr[0]), env.ip.addrlen);

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

void set_ipv4(void)
{
	env.ip.type = 4;
	env.ip.protocol = IPPROTO_IP;
	env.ip.family = AF_INET;
    env.ip.addrlen = INET_ADDRSTRLEN;
	env.ip.icmp = IPPROTO_ICMP;
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

    bzero(&(env.recvaddr), sizeof(env.recvaddr));                                         

    struct sockaddr_in recvaddr;
    bzero(&(env.recvaddr), sizeof(env.recvaddr));  
    recvaddr.sin_family = env.ip.family;
    recvaddr.sin_addr.s_addr = inet_addr("192.168.1.68");


    traceroute_loop();
}