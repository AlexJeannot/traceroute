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

typedef struct icmp_h
{
    uint8_t type;
    uint8_t code;
    uint16_t checksum;
    uint32_t padding;
    
} icmp_header;

typedef struct s_args
{
    char *target;
    char error_msg[1024];
} t_args;

typedef struct s_env
{
    int sendsock;
    int recvsock;
    t_args args;
    struct sockaddr_in sendaddr;
    struct sockaddr_in recvaddr;
    fd_set read_set;
    struct timeval timeout;
    struct addrinfo *targetinfo;
    int ttl;

} t_env;

struct iphdr {

    uint8_t     ihl:4;
    uint8_t    version:4;

    uint8_t   tos;
    uint16_t  tot_len;
    uint16_t  id;
    uint16_t  frag_off;
    uint8_t   ttl;
    uint8_t   protocol;
    uint16_t  check;
    uint32_t  saddr;
    uint32_t  daddr;
         /*The options start here. */
};

t_env env;

void display_help(int code);
void	parse_args(int argc, char **argv);
void error_exit(char *error);
int	ft_isdigit(int c);
void	display_wrong_option(char option);

#endif