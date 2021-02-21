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
#define R_PACKET 3
#define W_PACKET 4

typedef struct icmp_h
{
    uint8_t type;
    uint8_t code;
    uint16_t checksum;
    uint32_t padding;
    
} icmp_header;

typedef struct s_node
{
    char ip[INET6_ADDRSTRLEN];
    int8_t on;
    int8_t displayed;
    long double interval;
    char hostname[NI_MAXHOST];
} t_node;

typedef struct s_args
{
    char *target;
    long double to;
    int32_t packet_size;
} t_args;

typedef struct s_ip
{
    int8_t type;
    int8_t protocol;
    int8_t family;
    int8_t addrlen;
    int8_t icmp;
} t_ip;

typedef struct s_target
{
    struct addrinfo *info;
    char ip[INET6_ADDRSTRLEN];
    int8_t reached;
    char hostname[NI_MAXHOST];
} t_target;

typedef struct s_time
{
    long double ts_before;
    long double ts_after;
    struct timeval to;
} t_time;

typedef struct s_env
{
    t_args args;
    t_ip ip;
    t_target target;
    int32_t sendsock;
    int32_t recvsock;
    struct sockaddr *sendaddr;
    struct sockaddr *recvaddr;
    fd_set read_set;
    t_time time;
    t_node node[3];
    int8_t end;
    int32_t ttl;
    int32_t dport;
    uint16_t sport;
    char *error_msg;
} t_env;

t_env env;

/*
 * ANNEXES.C
*/
long double     get_ts(void);
int	ft_isdigit(int c);
int	ft_isalpha(int c);
size_t		ft_strlen(const char *str);
int	ft_strncmp(const char *s1, const char *s2, size_t n);

/*
 * ARGS.C
*/
void	parse_args(int argc, char **argv);

/*
 * CONTROL.C
*/
void verify_user_rights(void);
int ttl_exceeded(int type, int code);
int dest_port_unreach(int type, int code);
int is_same_node(int first_pos, int second_pos);
struct icmp* is_right_packet(char *data);

/*
 * DISPLAY.C
*/
void display_introduction(void);
void display_help(int code);
void	display_wrong_option(char option);
void display_ttl(void);

/*
 * ERROR.C
*/
void error_msg_exit(char *option, char *arg);
void	addrinfo_error(char *target, int error_code);

/*
 * EXIT.C
*/
void free_memory(void);
void free_fd(void);
void error_exit(char *error);
void program_exit(void);

/*
 * IPV4.C
*/
void set_ipv4(void);
void bind_sendsock_ipv4(void);
void set_sendaddr_ipv4(void);
void set_recvaddr_ipv4(void);

/*
 * IPV6.C
*/
void set_ipv6(void);
void bind_sendsock_ipv6(void);
void set_sendaddr_ipv6(void);
void set_recvaddr_ipv6(void);

/*
 * MANAGE_IO.C
*/
void send_datagram(void);
int8_t manage_icmp_reply(void);
void manage_no_reply(int pos);

/*
 * NETWORK.C
*/
void set_ttl(void);
void get_ip_addr(struct sockaddr* info, int type, int pos);
void get_hostname(struct sockaddr* info, int type, int pos);
void create_socket(void);
void get_targetinfo(void);

/*
 * NODE.C
*/
void display_remaining_node(void);
void add_node_info(void);

/*
 * OPTIONS.C
*/
int	parse_options(char *option, char* next_arg);

/*
 * SETTINGS.C
*/
void set_select(void);
void increment_port(void);
void set_next_trace(void);
void set_default_info(void);

/*
 * SIGNAL.C
*/
void	signal_handler(int code);
void	manage_signal(void);

#endif