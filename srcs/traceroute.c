#include "../inc/traceroute.h"

void traceroute_loop(void)
{
    int count;

    display_introduction();
    while (1)
    {
        count = 0;
        set_next_trace();
        while (count < 3)
        {
            send_datagram();
            set_select();
            select((env.recvsock + 1), &(env.read_set), NULL, NULL, &(env.time.to));
            if (FD_ISSET(env.recvsock, &(env.read_set)))
                manage_icmp_reply();
            else
                manage_no_reply(count);

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