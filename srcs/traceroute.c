#include "../inc/traceroute.h"

t_env env;

void retrieve_icmp_reply(int count)
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

void traceroute_loop(void)
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

int main(int argc, char **argv)
{
    verify_user_rights();
    manage_signal();
    set_default_info();
    parse_args(argc, argv);
    create_socket();
    get_targetinfo();
    traceroute_loop();
}