#ifndef TRACEROUTE_H
# define TRACEROUTE_H

#include <stdio.h> 
#include <stdlib.h> 
#include <unistd.h> 
#include <string.h> 
#include <sys/types.h> 
#include <sys/socket.h> 
#include <arpa/inet.h> 
#include <netinet/in.h> 
#include <netdb.h>
#include <errno.h>
#include <sys/time.h>
#include <fcntl.h>
#include <netinet/ip.h>
#include <netinet/ip_icmp.h>
#include <netinet/udp.h>

#define NODE 1
#define TARGET 2

typedef struct icmp_h
{
    uint8_t type;
    uint8_t code;
    uint16_t checksum;
    uint32_t padding;
    
} icmp_header;

typedef struct s_node
{
    int on;
    char ip[INET6_ADDRSTRLEN];
    char hostname[NI_MAXHOST];
    long double interval;
    int displayed;
} t_node;

typedef struct s_args
{
    char *target;
    char error_msg[1024];
} t_args;

typedef struct s_ip
{
    int type;
    int protocol;
    int family;
    int addrlen;
    int icmp;

} t_ip;

typedef struct s_target
{
    struct addrinfo *info;
    char ip[INET6_ADDRSTRLEN];
    char hostname[NI_MAXHOST];
} t_target;

typedef struct s_time
{
    struct timeval to;
    long double ts_before;
    long double ts_after;
} t_time;

typedef struct s_env
{
    int sendsock;
    int recvsock;
    t_args args;
    t_ip ip;
    struct sockaddr *sendaddr;
    struct sockaddr *recvaddr;
    fd_set read_set;
    t_target target;
    t_time time;
    // struct addrinfo *targetinfo;
    t_node node[3];
    int end;
    int ttl;
    int max_ttl;
    uint16_t port;

} t_env;

t_env env;

void display_help(int code);
void	parse_args(int argc, char **argv);
void error_exit(char *error);
int	ft_isdigit(int c);
void	display_wrong_option(char option);
size_t		ft_strlen(const char *str);
int	ft_strncmp(const char *s1, const char *s2, size_t n);

//ipv4
void set_ipv4(void);
void set_sendaddr_ipv4(void);
void set_recvaddr_ipv4(void);

//ipv6
void set_ipv6(void);
void set_sendaddr_ipv6(void);
void set_recvaddr_ipv6(void);

#endif