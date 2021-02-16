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
    // printf("ttl = %d\n", env.ttl);
    // int set_ttl = env.ttl;
    // if ((setsockopt(env.sendsock, IPPROTO_IP, IP_TTL, &set_ttl, (socklen_t)(sizeof(int)))) < 0)
    //     error_exit("TTL setup");
}

struct my_udp
{
    struct udphdr d;
    // char data[1472];
    char data[32];
};

struct tosend
{
    struct iphdr p;
    struct my_udp my_d;
};

struct checksum_udp
{
    uint32_t saddr;
    uint32_t daddr;
    uint8_t zero;
    uint8_t protocol;
    uint16_t len;
    struct my_udp header;
};

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

void send_datagram(void)
{
    char sendbuf[150];
    int ret;
    static int port = 33435;
    static int port2 = 47500;

    struct tosend packet;


    bzero(&packet, sizeof(packet));
    packet.p.version = 4;
    packet.p.ihl = 5;
    packet.p.tos = 0;
    packet.p.tot_len = sizeof(packet);
    packet.p.id = htons(44);
    packet.p.frag_off = htons(0);
    // packet.p.frag_off = htons(16384);
    printf(">>>>>>>>>>> TTL = %d\n", env.ttl);
    packet.p.ttl = env.ttl;
    packet.p.protocol = 17;
    packet.p.check = 0;
    packet.p.check = calcul_checksum(&packet.p, sizeof(packet.p));
    // env.ttl++;

    inet_pton(AF_INET, "10.0.2.15", &packet.p.saddr);
    inet_pton(AF_INET, "8.8.8.8", &packet.p.daddr);

    struct udphdr datagram;

    bzero(&(packet.my_d.data), sizeof(packet.my_d.data));
    packet.my_d.d.source = htons(port2);
    port2++;
    printf(">>>>>>>>>>> PORT = %d\n", port);
    packet.my_d.d.dest = htons(port);
    port += 3;
    packet.my_d.d.len = htons(sizeof(packet.my_d));
    packet.my_d.d.check = 0;

    struct checksum_udp test;
    test.saddr = packet.p.saddr;
    test.daddr = packet.p.daddr;
    test.zero = 0;
    test.protocol = 17;
    test.len = htons(sizeof(packet.my_d));
    test.header = packet.my_d;
    packet.my_d.d.check = calcul_checksum(&(test), sizeof(test));



    bzero(&sendbuf, sizeof(sendbuf));
    printf("AVANT SEND\n");
    if ((ret = sendto(env.sendsock, &packet, sizeof(packet), 0, env.targetinfo->ai_addr, (socklen_t)sizeof(env.sendaddr))) < 0)
    {
        printf("SEND FAILED ret = %d\n", ret);
        printf("ERRNO = %d\n", errno);
        perror("SEND ERROR: ");
    }
}

void set_select(void)
{
    FD_ZERO(&(env.read_set));
    FD_SET(env.recvsock, &(env.read_set));
    bzero(&(env.timeout), sizeof(env.timeout));
    env.timeout.tv_sec = 5;
}

void manage_icmp_reply(void)
{
    printf("\n===================== REC %d =======================\n", env.ttl);
    int soutLen = sizeof(env.recvaddr);
    char buf[1024];

    int msgLen = recvfrom(env.recvsock, buf, sizeof(buf), 0, (struct sockaddr *)&env.recvaddr, (socklen_t *)&soutLen);
    if (msgLen == -1)
    {
        printf("ERRNO = %d\n", errno);
        perror("RECV ERROR: ");
    }
    printf("msglen = %d\n", msgLen);
    // printf("-------------\nbuf = %s\nmsgLen = %d\n", buf, msgLen);

    char src_addr[INET_ADDRSTRLEN];

    inet_ntop(AF_INET,&(env.recvaddr.sin_addr) , &(src_addr[0]), INET_ADDRSTRLEN);
    printf("src_addr = %s\n", src_addr);

    struct icmphdr *ptr;


    ptr = (struct icmphdr *)&buf[20];
    printf("type = %d\n", ptr->type);
    printf("code = %d\n", ptr->code);


    bzero(&buf, sizeof(buf));
    msgLen = -3;

    printf("===================== REC %d =======================\n\n",  env.ttl);
}

void manage_no_reply(void)
{
    printf("NO REPLY\n");
}

void traceroute_loop(void)
{
    int count;


    // int val = IP_PMTUDISC_PROBE;
    // int ret = setsockopt(env.sendsock, IPPROTO_IP, IP_MTU_DISCOVER, &val, sizeof(val));
    // perror("DONT FRAG: ");
    // printf("RET IP DONT FRAG = %d\n", ret);

    int val = 1;
    int ret = setsockopt(env.sendsock, IPPROTO_IP, IP_HDRINCL, &val, sizeof(val));
    printf("RET HDRINCL = %d\n", ret);
    perror("HDRINCL: ");


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

    if ((env.sendsock = socket(AF_INET, SOCK_RAW, IPPROTO_UDP)) < 0)
        error_exit("UDP socket creation");

    if ((env.recvsock = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP)) < 0)
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

    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_RAW;
    hints.ai_protocol = IPPROTO_UDP;

    if ((ret = getaddrinfo("8.8.8.8", NULL, &hints, &(env.targetinfo))) != 0)
        addrinfo_error(ret);
    ((struct sockaddr_in*)env.targetinfo->ai_addr)->sin_port = htons(33435);
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
    verify_user_rights();
    parse_args(argc, argv);
    create_socket();
    get_targetinfo();
    
    // struct sockaddr_in servaddr;
    struct sockaddr_in recvaddr;
    struct sockaddr_in bindaddr;
    struct addrinfo *clientaddr;
    struct addrinfo hints;

    bzero(&(env.sendaddr), sizeof(env.sendaddr));
    bzero(&(env.recvaddr), sizeof(env.recvaddr));                                         
    bzero(&(hints), sizeof(hints));
    bzero(&(bindaddr), sizeof(bindaddr));
    bzero(&(recvaddr), sizeof(recvaddr));
    env.args.target = NULL;

    env.sendaddr.sin_family = AF_INET;
    int retinet = env.sendaddr.sin_addr.s_addr = inet_addr("8.8.8.8");
    env.sendaddr.sin_port = htons(33435); //33435

    bindaddr.sin_family = AF_INET;
    int retinet2 = bindaddr.sin_addr.s_addr = inet_addr("10.0.2.15");
    // servaddr.sin_port = htons(30000);

    printf("retinet = %d\n", retinet2);
    // printf("retinet2 = %d\n", retinet2);

    recvaddr.sin_family = AF_INET;
    recvaddr.sin_addr.s_addr = inet_addr("10.0.2.15");

    int retb;

    printf("retb = %d\n", retb);
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_DGRAM;
    hints.ai_protocol = IPPROTO_UDP;

    if (getaddrinfo("8.8.8.8", NULL, &hints, &(clientaddr)) != 0)
        printf("GETADDRINFO FAILED\n");
    display_addr_info(clientaddr);


    traceroute_loop();
}