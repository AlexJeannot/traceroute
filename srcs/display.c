#include "../inc/traceroute.h"

void display_introduction(void)
{
    printf("traceroute to %s [%s] (%s), 30 hops max, %d byte packets\n",env.args.target, env.target.hostname, env.target.ip, env.args.packet_size);
}

void display_help(int code)
{
	printf("Usage: ft_traceroute [-h6] [-f first_ttl] [-w waittime] host [packetlen]\n");
    printf("Options:\n");
    printf("   -h                Display help\n");
    printf("   -6                Use IPv6\n");
    printf("   -f  first_ttl     Start from the first_ttl hop (default: 1)\n");
    printf("   -w  waittime      Time to wait for probe in seconds (default: 5.0)\n");
    printf("   -f  first_ttl     Start from the first_ttl hop (default: 1)\n\n");
    printf("Arguments:\n");
    printf("   host              The host to traceroute to\n");
    printf("   packetlen         Full packet length including IP header, UDP header and data (default: 60)\n");


    exit(code);
}

void	display_wrong_option(char option)
{
	printf("Ping: invalid option -- '%c'\n", option);
	display_help(1);
}

void display_ttl(void)
{
    char display_ttl[6];

    if (env.ttl < 10)
        sprintf(display_ttl, " %d  ", env.ttl);
    else
        sprintf(display_ttl, "%d  ", env.ttl);
        
    write(1, &display_ttl[0], ft_strlen(display_ttl));
}