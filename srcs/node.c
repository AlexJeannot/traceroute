#include "../inc/traceroute.h"

void display_node(int pos)
{
    char node[1088];

    sprintf(node, "%s (%s)  %.3LF ms", env.node[pos].hostname ,env.node[pos].ip, env.node[pos].interval);
    write(1, &node[0], ft_strlen(node));
    env.node[pos].displayed = 1;
}

void display_interval(int pos)
{
    char interval[16];

    sprintf(interval, "  %.3LF ms", env.node[pos].interval);
    write(1, &interval[0], ft_strlen(interval));

    env.node[pos].displayed = 1;
}

void display_remaining_node(void)
{
    int8_t pos;

    pos = 0;
    while (pos < 3)
    {
        if (env.node[pos].on && !(env.node[pos].displayed))
        {
            if (pos == 1)
            {
                if (is_same_node(0, 2))
                    display_interval(2);
                write(1, "\n    ", 5);
                display_node(pos);
            }
            else if (pos == 2)
            {
                if (is_same_node(1, 2))
                    display_interval(pos);
                else
                {
                    write(1, "\n    ", 5);
                    display_node(pos);
                }
            }
        }
        pos++;
    }
    write(1, "\n", 1);
}

void add_node_info(void)
{
    int8_t pos;

    pos = 0;
    while (pos < 2 && env.node[pos].on)
        pos++;

    env.node[pos].on = 1;
    get_ip_addr(env.recvaddr, NODE, pos);
    get_hostname(env.recvaddr, NODE, pos);
    env.node[pos].interval = (env.time.ts_after - env.time.ts_before);

    if (pos == 0)
        display_node(pos);
    else if (pos == 1 && is_same_node(0, pos))
        display_interval(pos);

}