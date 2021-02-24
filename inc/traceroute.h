#ifndef TRACEROUTE_H
# define TRACEROUTE_H

# include <stdio.h>
# include <stdlib.h>
# include <unistd.h>
# include <string.h>
# include <sys/types.h>
# include <sys/socket.h>
# include <arpa/inet.h>
# include <netinet/in.h>
# include <netdb.h>
# include <errno.h>
# include <sys/time.h>
# include <fcntl.h>
# include <netinet/ip.h>
# include <netinet/ip_icmp.h>
# include <netinet/udp.h>
# include <signal.h>

# define NODE 1
# define TARGET 2
# define R_PACKET 3
# define W_PACKET 4

/*
 * Structure to store router informations
*/
typedef struct	s_node
{
	char		ip[INET6_ADDRSTRLEN];
	int8_t		on;                     // If informations
	int8_t		displayed;              // If already displayed
	long double	interval;
	char		hostname[NI_MAXHOST];
}		t_node;

/*
 * Structure to store argument values
*/
typedef struct	s_args
{
	char		*target;                // Target name
	long double	to;                     // Wait time
	int32_t		packet_size;
}		t_args;

/*
 * Structure to store ip informations (IPv4 / IPv6)
*/
typedef struct	s_ip
{
	int8_t	type;                       // 4 or 6
	int8_t	protocol;                   // IPPROTO_IP or IPPROTO_IPv6
	int8_t	family;                     // AF_INET or AF_INET6
	int8_t	addrlen;                    // INET_ADDRSTRLEN or INET6_ADDRSTRLEN
	int8_t	icmp;                       // IPPROTO_ICMP or IPPROTO_ICMPV6
    int8_t  header_size;                // 20 bytes or 40 bytes
}		t_ip;

/*
 * Structure to store target informations
*/
typedef struct	s_target
{
	struct addrinfo	*info;
	char		    ip[INET6_ADDRSTRLEN];
	int8_t		    reached;
	char		    hostname[NI_MAXHOST];
}		t_target;

/*
 * Structure to store time informations
*/
typedef struct	s_time
{
	long double	    ts_before;
	long double	    ts_after;
	struct timeval	to;             // Use in select() call
}		t_time;

typedef struct	s_env
{
	t_args			args;
	t_ip			ip;
	t_target		target;
	int32_t			sendsock;
	int32_t			recvsock;
	struct sockaddr *sendaddr;
	struct sockaddr *recvaddr;
	fd_set			read_set;
	t_time			time;
	t_node			node[3];
	int32_t			ttl;
	int32_t			dport;      // Destination port
	uint16_t		sport;      // Source port
	char			*error_msg;
}				t_env;

extern t_env	env;

/*
 * ANNEXES.C
*/
long double	get_ts(void);
int		ft_isdigit(int c);
int		ft_isalpha(int c);
size_t		ft_strlen(const char *str);
int		ft_strncmp(const char *s1, const char *s2, size_t n);
size_t		ft_strlcpy(char *dest, const char *src, size_t size);

/*
 * ARGS.C
*/
void		parse_args(int argc, char **argv);

/*
 * CONTROL.C
*/
void		verify_user_rights(void);
int		ttl_exceeded(int type, int code);
int		dest_port_unreach(int type, int code);
int		is_same_node(int first_pos, int second_pos);
struct icmp	*is_right_packet(char *data);

/*
 * DISPLAY.C
*/
void		display_introduction(void);
void		display_help(int code);
void		display_wrong_option(char option);
void		display_ttl(void);

/*
 * ERROR.C
*/
void		error_msg_exit(char *option, char *arg);
void		addrinfo_error(char *target, int error_code);

/*
 * EXIT.C
*/
void		free_memory(void);
void		free_fd(void);
void		error_exit(char *error);
void		program_exit(void);

/*
 * IPV4.C
*/
void		set_ipv4(void);
void		bind_sendsock_ipv4(void);
void		set_sendaddr_ipv4(void);
void		set_recvaddr_ipv4(void);

/*
 * IPV6.C
*/
void		set_ipv6(void);
void		bind_sendsock_ipv6(void);
void		set_sendaddr_ipv6(void);
void		set_recvaddr_ipv6(void);

/*
 * MANAGE_IO.C
*/
void		send_datagram(void);
int8_t		manage_icmp_reply(void);
void		manage_no_reply(int pos);

/*
 * NETWORK.C
*/
void		set_ttl(void);
void		get_ip_addr(struct sockaddr *info, int type, int pos);
void		get_hostname(struct sockaddr *info, int type, int pos);
void		create_socket(void);
void		get_targetinfo(void);

/*
 * NODE.C
*/
void		display_remaining_node(void);
void		add_node_info(void);

/*
 * OPTIONS.C
*/
int		parse_options(char *option, char *next_arg);

/*
 * SETTINGS.C
*/
void		set_select(void);
void		increment_port(void);
void		set_next_trace(void);
void		set_default_info(void);

/*
 * SIGNAL.C
*/
void		signal_handler(int code);
void		manage_signal(void);

#endif
