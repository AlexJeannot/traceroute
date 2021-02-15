#include "traceroute.h"

void display_addr_info(struct addrinfo *addr)
{
    int count;

    count = 0;
    printf("\n=================== ADDR INFOS ===================\n");
    while (addr)
    {
        printf("\n-------------- %d --------------\n", count++);
        printf("ai_flags = %d\n", addr->ai_flags);
        printf("ai_family = %d\n", addr->ai_family);
        printf("ai_socktype = %d\n", addr->ai_socktype);
        printf("ai_protocol = %d\n", addr->ai_protocol);
        printf("ai_addrlen = %u\n", addr->ai_addrlen);
        printf("ai_canonname = %s\n", addr->ai_canonname);
        printf("res->ai_addr->sa_family = %hhu\n", addr->ai_addr->sa_family);
        printf("--------------------------------\n");
        addr = addr->ai_next;
    }
    printf("\n==================================================\n\n");
}

int main()
{
    int sockfd;
    int recvsock;


    uid_t uid = getuid();
    printf("uid = %d\n", uid);
    if (uid != 0)
    {
        printf("You need to be root\n");
        exit(1);
    }

    if ((sockfd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0)
        printf("SOCKET ERROR\n");

    if ((recvsock = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP)) < 0)
        printf("SOCKET ERROR\n");


    
    struct sockaddr_in servaddr;
    struct sockaddr_in recvaddr;
    struct sockaddr_in bindaddr;
    struct addrinfo *clientaddr;
    struct addrinfo hints;

    bzero(&(servaddr), sizeof(servaddr));
    bzero(&(clientaddr), sizeof(clientaddr));                                         
    bzero(&(hints), sizeof(hints));
    bzero(&(bindaddr), sizeof(bindaddr));
    bzero(&(recvaddr), sizeof(recvaddr));

    servaddr.sin_family = AF_INET;
    int retinet = servaddr.sin_addr.s_addr = inet_addr("8.8.8.8");
    servaddr.sin_port = htons(33436); //33435

    bindaddr.sin_family = AF_INET;
    int retinet2 = bindaddr.sin_addr.s_addr = INADDR_ANY;
    // servaddr.sin_port = htons(30000);

    printf("retinet = %d\n", retinet2);
    // printf("retinet2 = %d\n", retinet2);

    recvaddr.sin_family = AF_INET;
    recvaddr.sin_addr.s_addr = INADDR_ANY;

    int retb;

    // if ((retb = bind(recvsock, (const struct sockaddr*)&(bindaddr), sizeof(bindaddr))) < 0)
    // {
    //     printf("BIND FAILED\n");
    //     printf("ERRNO = %d\n", errno);
    //     perror("BIND ERROR: ");
    // }


    printf("retb = %d\n", retb);
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_DGRAM;
    hints.ai_protocol = IPPROTO_UDP;

    if (getaddrinfo("8.8.8.8", NULL, &hints, &(clientaddr)) != 0)
        printf("GETADDRINFO FAILED\n");
    display_addr_info(clientaddr);

    char sendbuf[24];
    int ret;

    int msgLen;
    char buf[1024];
    int soutLen;

    bzero(&sendbuf, sizeof(sendbuf));
    bzero(&buf, sizeof(buf));

    // if((msgLen = recvfrom(sockfd, buf, sizeof(buf), 0,(struct sockaddr *)&cli, &soutLen))<0){
    //     perror("udpServer: recvfrom()"); 
    //     exit( 1); 
    // }

    fd_set read;
    fd_set write;

    FD_ZERO(&read);
    FD_ZERO(&write);

    FD_SET(recvsock, &read);
    // FD_SET(sockfd, &write);

    int max = (sockfd > recvsock) ? sockfd : recvsock;
    max++;

    // printf("sockfd = %d\n", sockfd);
    // printf("recvsock = %d\n", recvsock);
    // printf("max = %d\n", max);  

    // printf("AVANT SNTD\n");


    // int setsock = setsockopt(sockfd, IPPROTO_IP, IP_TTL, &ttl, (socklen_t)(sizeof(ttl)));
    // printf("Setsock = %d\n", setsock);

    // if ((ret = sendto(sockfd, sendbuf, 24, 0, (struct sockaddr *)&(servaddr), (socklen_t)sizeof(servaddr))) < 0)
    // {
    //     printf("SEND FAILED ret = %d\n", ret);
    //     printf("ERRNO = %d\n", errno);
    //     perror("SEND ERROR: ");
    // }
    int ttl = 0;
    int count = 0;
    struct timeval timeout;
    // struct timezone tz;
    ttl = 1;

    // fcntl(recvsock, F_SETFL, O_NONBLOCK);
    while (1)
    {
        FD_ZERO(&read);
        FD_ZERO(&write);

        FD_SET(recvsock, &read);

        // gettimeofday(&timeout, &tz);
        timeout.tv_sec = 3;
        timeout.tv_usec = 0;
        // ttl++;
        // ttl = 1;
        printf("ttl = %d\n", ttl);
        int setsock = setsockopt(sockfd, IPPROTO_IP, IP_TTL, &ttl, (socklen_t)(sizeof(ttl)));
        printf("Setsock = %d\n", setsock);


	    struct timeval	recv_to;


	    recv_to.tv_sec = 1;
	    recv_to.tv_usec = 0;

        // setsock = setsockopt(recvsock, SOL_SOCKET, SO_RCVTIMEO, (char*)&(recv_to), (socklen_t)(sizeof(recv_to)));
        // printf("Setsock = %d\n", setsock);

        char dest_addr[INET_ADDRSTRLEN];

        inet_ntop(AF_INET,&(servaddr.sin_addr) , &(dest_addr[0]), INET_ADDRSTRLEN);
        printf("dest_addr = %s\n", dest_addr);
        if ((ret = sendto(sockfd, sendbuf, 24, 0, (struct sockaddr *)&(servaddr), (socklen_t)sizeof(servaddr))) < 0)
        {
            printf("SEND FAILED ret = %d\n", ret);
            printf("ERRNO = %d\n", errno);
            perror("SEND ERROR: ");
        }
        printf(" SELECT\n");

        select(recvsock + 1, &read, NULL, 0, &timeout);
        // printf("APRES SELECT\n");

        printf("FD_ISSET(recvsock, &read) = %d\n", FD_ISSET(recvsock, &read));
        if (FD_ISSET(recvsock, &read))
        {

            // printf("recvsock SELECT READ\n");

            printf("\n===================== REC %d =======================\n", count);
            soutLen = sizeof(recvaddr);

            msgLen = recvfrom(recvsock, buf, sizeof(buf), MSG_WAITALL,(struct sockaddr *)&recvaddr, (socklen_t *)&soutLen);
            if (msgLen == -1)
            {
                printf("ERRNO = %d\n", errno);
                perror("RECV ERROR: ");
            }
            printf("msglen = %d\n", msgLen);
            // printf("-------------\nbuf = %s\nmsgLen = %d\n", buf, msgLen);

            char src_addr[INET_ADDRSTRLEN];

            inet_ntop(AF_INET,&(recvaddr.sin_addr) , &(src_addr[0]), INET_ADDRSTRLEN);
            printf("src_addr = %s\n", src_addr);

            icmp_header *ptr;


            ptr = (icmp_header *)&buf[20];
            printf("type = %d\n", ptr->type);
            printf("code = %d\n", ptr->code);
            // printf("type = %d\n", ptr->type);
            // if (ptr->type == 11)
            // {
            //     printf("target = %s\n", src_addr);
            //     exit(0);
            // }

            bzero(&buf, sizeof(buf));
            msgLen = -3;



            FD_CLR(recvsock, &read);
            printf("===================== REC %d =======================\n\n", count);
        }


        if (FD_ISSET(sockfd, &write))
        {
            printf("sockfd SELECT WRITE\n");
        }

        // bzero(&buf, sizeof(buf));
        // msgLen = -3;
        // msgLen = recvfrom(recvsock, buf, sizeof(buf), MSG_DONTWAIT,(struct sockaddr *)&recvaddr, (socklen_t *)&soutLen);
        // printf("-------------\nbuf = %s\nmsgLen = %d\n", buf, msgLen); 

        // bzero(&buf, sizeof(buf));
        // msgLen = -3;
        // msgLen = recvfrom(recvsock, buf, sizeof(buf), MSG_DONTWAIT,(struct sockaddr *)&recvaddr, (socklen_t *)&soutLen);
        // printf("-------------\nbuf = %s\nmsgLen = %d\n", buf, msgLen);
        ttl++;
        printf("sleep\n");
        sleep(3);
        count++;
    }
}