#include "../inc/traceroute.h"

void parse_packet_size(char *arg)
{
	int16_t	count;

	count = 0;
	while (arg[count])
	{
		if (!(ft_isdigit(arg[count])))
			error_msg_exit("packet size", arg);
		count++;
	}
	env.args.packet_size = atoi(arg);
	if (env.args.packet_size < 0)
		error_exit("invalid packet size: invalid value (must be > 0)");
	else if (env.args.packet_size > 0 && env.args.packet_size < 28)
		env.args.packet_size = 28;
	else if (env.args.packet_size > 65000)
		error_exit("invalid packet size: invalid value (must be <= 65000)");
}

/*
 * Parsing of all arguments (options and hostname)
*/ 
void	parse_args(int argc, char **argv)
{
	int16_t	pos;

	pos = 1;
	if (argc < 2)
		display_help(1);
    while (pos < argc)
    {
        if (argv[pos][0] == '-' && argv[pos][1])
            pos += parse_options(&(argv[pos][1]), argv[pos + 1]);
        else
        {
            if (!(env.args.target))
            	env.args.target = argv[pos];
			else if (!(env.args.packet_size))
				parse_packet_size(argv[pos]);
			else
				error_msg_exit("extra argument", argv[pos]);
        }
        pos++;
    }
	if (!(env.args.target))
		error_exit("host argument missing");
	if (!(env.args.packet_size))
		env.args.packet_size = 60;
}
