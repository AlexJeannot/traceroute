#include "../inc/traceroute.h"

/*
 * Signals handler function
 * SIGINT and SIGQUIT exit program (free resources in program_exit())
*/
void	signal_handler(int code)
{
	if (code == SIGINT || code == SIGQUIT)
		program_exit();
}

/*
 * Set signal manager
*/
void	manage_signal(void)
{
	signal(SIGINT, &signal_handler);
	signal(SIGQUIT, &signal_handler);
}