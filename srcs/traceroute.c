#include "../inc/traceroute.h"

t_env env;

/*
 * Wait for ICMP reply after sending UDP datagram
 * Recursion if wrong ICMP packet has been retrieve
 * On linux select modify timeout so it be not be reinitialized to defined value
 * On mac it will be (too bad)
*/
void	retrieve_icmp_reply(int count)
{
	select((env.recvsock + 1), &(env.read_set), NULL, NULL, &(env.time.to));
	if (FD_ISSET(env.recvsock, &(env.read_set)))
	{
		if (manage_icmp_reply() == W_PACKET)
			retrieve_icmp_reply(count);
	}
	else
		manage_no_reply(count);
}

/*
 * Traceroute loop
 * Display introduction
 * Set parameters for next three probes
 * Send three UDP datagrams
 * Display remaining router informations
 * If target or hop max reached then exit
*/
void	traceroute_loop(void)
{
	int8_t count;

	display_introduction();
	while (1)
	{
		count = 0;
		set_next_trace();
		while (count < 3)
		{
			send_datagram();
			set_select();
			retrieve_icmp_reply(count);
			count++;
		}
		display_remaining_node();
		if (env.target.reached || env.ttl >= 30)
			program_exit();
		env.ttl++;
	}
}

/*
 * Main function for traceroute
 * Function names speak for themself
*/
int	main(int argc, char **argv)
{
	verify_user_rights();
	manage_signal();
	set_default_info();
	parse_args(argc, argv);
	create_socket();
	get_targetinfo();
	traceroute_loop();
}
