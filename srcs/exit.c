#include "../inc/traceroute.h"
#define RED "\033[38;5;124m"
#define RESET "\033[0m"

/*
 * Free potential allocated memory
*/
void	free_memory(void)
{
	if (env.error_msg)
		free(env.error_msg);
	if (env.sendaddr)
		free(env.sendaddr);
	if (env.recvaddr)
		free(env.recvaddr);
	if (env.target.info)
		freeaddrinfo(env.target.info);
}

/*
 * Free potential (and surely) allocated file descriptor
*/
void	free_fd(void)
{
	if (env.sendsock > -1)
		close(env.sendsock);
	if (env.recvsock > -1)
		close(env.recvsock);
}

/*
 * Exit if error in program
*/
void	error_exit(char *error)
{
	fprintf(stderr, "ft_traceroute: %s%s%s\n", RED, error, RESET);
	free_memory();
	free_fd();
	exit(1);
}

/*
 * Exit if everything is ok
 * If SIGINT, reach target or reach hops limit for example
*/
void	program_exit(void)
{
	free_memory();
	free_fd();
	exit(0);
}
