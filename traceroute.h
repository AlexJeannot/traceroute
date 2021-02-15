#ifndef TRACEROUTE_H
# define TRACEROUTE_H

#include <stdio.h> 
#include <stdlib.h> 
#include <unistd.h> 
#include <string.h> 
#include <sys/types.h> 
#include <sys/socket.h> 
#include <arpa/inet.h> 
#include <netinet/in.h> 
#include <netdb.h>
#include <errno.h>
#include <sys/time.h>
#include <fcntl.h>

typedef struct icmp_h
{
    uint8_t type;
    uint8_t code;
    uint16_t checksum;
    uint32_t padding;
    
} icmp_header;

#endif