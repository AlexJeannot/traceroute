#include "../inc/traceroute.h"

void free_memory(void)
{
    if (env.error_msg)
        free(env.error_msg);
    if (env.sendaddr)
        free(env.sendaddr);
    if (env.recvaddr)
        free(env.recvaddr);
}

void free_fd(void)
{
    if (env.sendsock > -1)
        close(env.sendsock);
    if (env.recvsock > -1)
        close(env.recvsock);
}

void error_exit(char *error)
{
    fprintf(stderr, "ft_traceroute: %s\n", error);
    free_memory();
    free_fd();
    exit(1);
}

void program_exit(void)
{
    free_memory();
    free_fd();
    exit(0);
}