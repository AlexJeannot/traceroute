#include "../inc/traceroute.h"

/*
 * Send UDP datagram to target
 * Amount of data is packet size (by default: 60 for IPv4 / 80 for IPv6) minus IP header size and UDP header size
 * Set sending structure
 * Sendto() while amount of data sent is not equal to total
*/
void	send_datagram(void)
{
	char		data[env.args.packet_size - (env.ip.header_size + 8)];
	socklen_t	size;
	int32_t		tosend;
	int32_t		sent;
	int32_t		total;

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

/*
 * Retrieve ICMP reply and analyze it
 * Set reception structure
 * Verify if received packet corresponding to program UDP datagram
 * - If true -> Set router informations
 *           -> If ICMP reply is "dest port unreachable" -> set end program variable
 * 
 * - If false -> Return wrong packet value to return in select()
*/
int8_t	manage_icmp_reply(void)
{
	char		data[1024];
	struct icmp	*icmphdr;
	socklen_t	size;
	int8_t		ret_val;

	ret_val = R_PACKET;
	bzero(&data, sizeof(data));
	(env.ip.type == 4) ? set_recvaddr_ipv4() : set_recvaddr_ipv6();
	size = (env.ip.type == 4) ? sizeof(struct sockaddr_in) : sizeof(struct sockaddr_in6);

	int ret;
	if ((ret = recvfrom(env.recvsock, data, sizeof(data), 0, env.recvaddr, &size)) < 0)
		error_exit("recvfrom() failed");
	env.time.ts_after = get_ts();

	if ((icmphdr = is_right_packet(&data[0])))
	{
		if (ttl_exceeded(icmphdr->icmp_type, icmphdr->icmp_code)
				|| dest_port_unreach(icmphdr->icmp_type, icmphdr->icmp_code))
			add_node_info();

		if (dest_port_unreach(icmphdr->icmp_type, icmphdr->icmp_code))
			env.target.reached = 1;
	}
	else
		ret_val = W_PACKET;

	free(env.recvaddr);
	env.recvaddr = NULL;
	return (ret_val);
}

/*
 * If wait time end in select() and no resonse received
 * Display * and go on to next prob
*/
void	manage_no_reply(int pos)
{
	static int8_t	prev = 2;

	if (pos && prev != (pos - 1))
		write(1, " * ", 4);
	else
		write(1, "* ", 3);
	prev = pos;
}
