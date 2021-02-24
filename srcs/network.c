#include "../inc/traceroute.h"

/*
 * Set TTL value
 * Start at first ttl (by default 1)
 * Increment every three probs
*/
void	set_ttl(void)
{
	int32_t	set_ttl;
	int8_t	ttl_flag;

	set_ttl = env.ttl;
	ttl_flag = (env.ip.type == 4) ? IP_TTL : IPV6_UNICAST_HOPS;
	if ((setsockopt(env.sendsock, env.ip.protocol, ttl_flag, (char *)&set_ttl, (socklen_t)(sizeof(set_ttl)))) < 0)
		error_exit("TTL setup failed");
}

/*
 * Set IP adress in router informations or target information
*/
void	get_ip_addr(struct sockaddr *info, int type, int pos)
{
	char	*addr;

	addr = (type == NODE) ? &(env.node[pos].ip[0]) : &(env.target.ip[0]);
	if (env.ip.type == 4)
		inet_ntop(env.ip.family, &(((struct sockaddr_in*)info)->sin_addr), addr, env.ip.addrlen);
	else
		inet_ntop(env.ip.family, &(((struct sockaddr_in6*)info)->sin6_addr), addr, env.ip.addrlen);
}

/*
 * Get hostname by DNS resolution
 * Set it in router informations or target information
*/
void	get_hostname(struct sockaddr* info, int type, int pos)
{
	char		*hostname;
	socklen_t	size;

	hostname = (type == NODE) ? &(env.node[pos].hostname[0]) : &(env.target.hostname[0]);
	size = (env.ip.type == 4) ? sizeof(struct sockaddr_in) : sizeof(struct sockaddr_in6);
	if (getnameinfo(info, size, hostname, NI_MAXHOST, NULL, 0, 0) != 0)
		ft_strlcpy(hostname, "N/A", 4);
}

/*
 * Sockets creation
 * UDP socket for sending
 * ICMP socket for receiving
*/
void	create_socket(void)
{
	env.sendsock = -1;
	env.recvsock = -1;

	if ((env.sendsock = socket(env.ip.family, SOCK_DGRAM, IPPROTO_UDP)) < 0)
		error_exit("UDP socket creation failed");



	if ((env.recvsock = socket(env.ip.family, SOCK_RAW, env.ip.icmp)) < 0)
		error_exit("ICMP socket creation failed");

	(env.ip.type == 4) ? bind_sendsock_ipv4() : bind_sendsock_ipv6();


}

/*
 * Get target informations
 * Sending informations
 * IP adress and hostname for introduction line
*/
void	get_targetinfo(void)
{
	struct addrinfo	hints;
	int		ret;

	bzero(&hints, sizeof(hints));

	hints.ai_family = env.ip.family;
	hints.ai_socktype = SOCK_DGRAM;
	hints.ai_protocol = IPPROTO_UDP;

	if ((ret = getaddrinfo(env.args.target, NULL, &hints, &(env.target.info))) != 0)
		addrinfo_error(env.args.target, ret);

	get_ip_addr(env.target.info->ai_addr, TARGET, 0);
	get_hostname(env.target.info->ai_addr, TARGET, 0);
}
