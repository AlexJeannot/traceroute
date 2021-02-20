#include "../inc/traceroute.h"

void verify_user_rights(void)
{
    if (getuid() != 0)
        error_exit("you must be root to run this program");
}

int ttl_exceeded(int type, int code)
{
    if (type == 11 && code == 0)
        return (1);
    return (0);
}

int dest_port_unreach(int type, int code)
{
    if (type == 3 && code == 3)
        return (1);
    return (0);
}

int is_same_node(int first_pos, int second_pos)
{

    if (ft_strncmp(&(env.node[first_pos].ip[0]), &(env.node[second_pos].ip[0]), 16) == 0)
        return (1);
    return (0);
}

