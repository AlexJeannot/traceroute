#include "traceroute.h"

int	parse_first_ttl(char *current_arg, char *next_arg)
{
	int	count;
	char*	arg;

	count = 0;
	arg = (current_arg[count]) ? current_arg : next_arg;
	if (arg[count])
	{
		while (arg[count])
		{
			if (!(ft_isdigit(arg[count])))
			{
				sprintf(&(env.args.error_msg[0]), "invalid first TTL: '%s'", &(arg[count]));
				error_exit(&(env.args.error_msg[0]));
			}
			count++;
		}
	}
	else
		error_exit("option requires an argument -- f");
    env.ttl = atoi(arg);
	if (env.ttl < 1 || env.ttl > 255)
		error_exit("invalid first TTL: invalid value");
	return (current_arg[0] ? 0 : 1);
}

/*
 * Parsing of counter option (-c)
 * next_arg is used if '-c 42' != '-c42'
*/ 
// int	parse_counter(char *current_arg, char *next_arg)
// {
// 	int	count;
// 	char*	arg;

// 	count = 0;
// 	arg = (current_arg[count]) ? current_arg : next_arg;
// 	if (arg[count])
// 	{
// 		while (arg[count])
// 		{
// 			if (!(ft_isdigit(arg[count])))
// 			{
// 				sprintf(&(env.args.error_msg[0]), "invalid counter: '%s'", &(arg[count]));
// 				error_exit(&(env.args.error_msg[0]));
// 			}
// 			count++;
// 		}
// 	}
// 	else
// 		error_exit("option requires an argument -- c");
// 	if ((env.args.counter = atoi(arg)) < 1)
// 		error_exit("invalid counter: must be > 0");
// 	return (current_arg[0] ? 0 : 1);
// }




/*
 * Parsing of options (beginning by an -)
*/ 
int	parse_options(char *option, char* next_arg)
{
	int	count;

	count = 0;
	while (option[count])
	{
		// if (option[count] == 'v')
		// 	env.args.verbose = 1;
		if (option[count] == 'h')
			display_help(0);
		else if (option[count] == 'f')
		{
			if (option[count] == 'f')
				count = parse_first_ttl(&(option[count + 1]), next_arg);
			// else if (option[count] == 'c')
			// 	count = parse_counter(&(option[count + 1]), next_arg);
			return (count);
		}
		else
			display_wrong_option(option[count]);
		count++;
	}
	return (0);
}

/*
 * Parsing of all arguments (options and hostname)
*/ 
void	parse_args(int argc, char **argv)
{
	int	pos;

	pos = 1;
	if (argc < 2)
		display_help(1);
    while (pos < argc)
    {
        if (argv[pos][0] == '-' && argv[pos][1])
            pos += parse_options(&(argv[pos][1]), argv[pos + 1]);
        else
        {
            if (env.args.target)
                error_exit("Multiple targets");
            env.args.target = argv[pos];
        }
        pos++;
    }
    if (!(env.ttl))
        env.ttl = 1;
}
