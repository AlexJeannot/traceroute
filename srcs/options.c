#include "../inc/traceroute.h"

int	parse_first_ttl(char *current_arg, char *next_arg)
{
	int16_t	count;
	char*	arg;

	count = 0;
	arg = (current_arg[count]) ? current_arg : next_arg;
	if (arg && arg[count])
	{
		while (arg[count])
		{
			if (!(ft_isdigit(arg[count])))
				error_msg_exit("first ttl", arg);
			count++;
		}
	}
	else
		error_exit("option requires an argument -- f");
    env.ttl = atoi(arg);
	if (env.ttl < 1 || env.ttl > 255)
		error_exit("invalid first ttl: invalid value (must be > 0 and < 256)");
	return (current_arg[0] ? 0 : 1);
}

int	parse_timeout(char *current_arg, char *next_arg)
{
	int16_t	count;
	char*	arg;

	count = 0;
	arg = (current_arg[count]) ? current_arg : next_arg;
	if (arg && arg[count])
	{
		while (arg[count])
		{
			if (!(ft_isdigit(arg[count])) && arg[count] != '.')
				error_msg_exit("wait time", arg);
			count++;
		}
		env.args.to = strtold(arg, NULL);
		if (errno == ERANGE || errno == EINVAL)
			error_msg_exit("wait time", arg);
		else if (env.args.to < 0 || env.args.to > 3600)
			error_exit("invalid wait time: invalid value (must be > 0 and <= 3600)");
	}
	else
		error_exit("option requires an argument -- w");
	return (current_arg[0] ? 0 : 1);
}

int	parse_port(char *current_arg, char *next_arg)
{
	int16_t	count;
	char*	arg;

	count = 0;
	arg = (current_arg[count]) ? current_arg : next_arg;
	if (arg && arg[count])
	{
		while (arg[count])
		{
			if (!(ft_isdigit(arg[count])))
				error_msg_exit("port", arg);
			count++;
		}
		env.dport = atoi(arg);
		if (env.dport < 1 || env.dport > 65535)
			error_exit("invalid port: invalid value (must be > 0 and <= 65535)");
	}
	else
		error_exit("option requires an argument -- p");
	return (current_arg[0] ? 0 : 1);
}

/*
 * Parsing of options (beginning by an -)
*/ 
int	parse_options(char *option, char* next_arg)
{
	int8_t	count;

	count = 0;
	while (option[count])
	{
		if (option[count] == 'h')
			display_help(0);
		else if (option[count] == '6')
			set_ipv6();
		else if (option[count] == 'f' || option[count] == 'w' || option[count] == 'p')
		{
			if (option[count] == 'f')
				count = parse_first_ttl(&(option[count + 1]), next_arg);
			else if (option[count] == 'w')
				count = parse_timeout(&(option[count + 1]), next_arg);
			else if (option[count] == 'p')
				count = parse_port(&(option[count + 1]), next_arg);
			return (count);
		}
		else
			display_wrong_option(option[count]);
		count++;
	}
	return (0);
}