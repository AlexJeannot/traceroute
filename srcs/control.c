#include "../inc/traceroute.h"

/*
 * Verify if user has root rights
*/
void		verify_user_rights(void)
{
	if (getuid() != 0)
		error_exit("you must be root to run this program");
}

/*
 * Verify if icmp reply correpond to "TTL exceeded"
 * IPv4 -> type 11 & code 0
 * IPv6 -> type 3
*/
int		ttl_exceeded(int type, int code)
{
	if (env.ip.type == 4 && type == 11 && code == 0)
		return (1);
	if (env.ip.type == 6 && type == 3)
		return (1);
	return (0);
}

/*
 * Verify if icmp reply correpond to "Destination port unreachable"
 * IPv4 -> type 3 & code 3
 * IPv6 -> type 1
*/
int		dest_port_unreach(int type, int code)
{
	if (env.ip.type == 4 && type == 3 && code == 3)
		return (1);
	if (env.ip.type == 6 && type == 1)
		return (1);
	return (0);
}

/*
 * Verify if ip of node[first_pos] is same as node[second_pos]
*/
int		is_same_node(int first_pos, int second_pos)
{
	if (ft_strncmp(&(env.node[first_pos].ip[0]), &(env.node[second_pos].ip[0]), 16) == 0)
		return (1);
	return (0);
}

/*
 * Verify if ICMP reply correspond to this program UDP datagram
 * Because IMCP reply encapsulate request IP packet
 * Unwrap until UDP datagram to verify source port (bind before)
 * res_iphdr is uniquely for IPv4 because by default IPv6 socket does not give IP header
*/
struct icmp	*is_right_packet(char *data)
{
	struct ip	*res_iphdr;
	struct icmp	*icmphdr;
	struct ip	*req_iphdr;
	struct udphdr	*udphdr;
	int16_t		offset;

	res_iphdr = (struct ip *)&data[0];
	offset = res_iphdr->ip_hl * 4;
	icmphdr = (env.ip.type == 4) ? (struct icmp *)&data[offset] : (struct icmp *)&data[0];
	offset = (env.ip.type == 4) ? offset + 8 : 8;
	req_iphdr = (struct ip *)&data[offset];
	offset += (env.ip.type == 4) ? req_iphdr->ip_hl * 4 : 40;
	udphdr = (struct udphdr *)&data[offset];

	if (ntohs(udphdr->uh_sport) == env.sport && ntohs(udphdr->uh_dport) == (env.dport - 1))
		return (icmphdr);
	return (NULL);
}
