#include "../inc/traceroute.h"

void set_select(void)
{
    FD_ZERO(&(env.read_set));
    FD_SET(env.recvsock, &(env.read_set));
    bzero(&(env.time.to), sizeof(env.time.to));
    env.time.to.tv_sec = env.args.to;
    env.time.to.tv_usec = (env.args.to - (long double)env.time.to.tv_sec) * 1000000;
}

void increment_port(void)
{
    env.dport++;
    if (env.dport > 65535)
        env.dport = 33435;
}

void set_next_trace(void)
{
    set_ttl();
    display_ttl();
    bzero(&env.node, sizeof(env.node));
    env.time.ts_before = 0;
    env.time.ts_after = 0;
}

void set_default_info(void)
{
    bzero(&env, sizeof(env));
    env.ttl = 1;
    env.dport = 33435;
    env.args.to = 5;
    env.args.packet_size = 0;
    set_ipv4();
}