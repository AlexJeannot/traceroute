#include "../inc/traceroute.h"

/*
 * Prepare select()
 * Add reception socket in read fd_set
 * Set select timeout to wait time (by default 5 seconds)
*/
void	set_select(void)
{
	FD_ZERO(&(env.read_set));
	FD_SET(env.recvsock, &(env.read_set));
	bzero(&(env.time.to), sizeof(env.time.to));
	env.time.to.tv_sec = env.args.to;
	env.time.to.tv_usec = (env.args.to - (long double)env.time.to.tv_sec) * 1000000;
}

/*
 * Increment port
 * If reach maximum port then set it to first traceroute port
*/
void	increment_port(void)
{
	env.dport++;
	if (env.dport > 65535)
		env.dport = 33435;
}

/*
 * Set next three probes
 * Set TTL and increment it by one
 * Display TTL
 * bzero() router informations array
 * set both timestamp to zero
*/
void	set_next_trace(void)
{
	set_ttl();
	display_ttl();
	bzero(&env.node, sizeof(env.node));
	env.time.ts_before = 0;
	env.time.ts_after = 0;
}

/*
 * Set default informations before parsing arguments
 * Packet size is set to -1 and will be set to default value if unspecified
*/
void	set_default_info(void)
{
	bzero(&env, sizeof(env));
	env.ttl = 1;
	env.dport = 33435;
	env.args.to = 5;
	env.args.packet_size = -1;
	set_ipv4();
}
