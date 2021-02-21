
#include "./inc/traceroute.h"

void	getaddr_error(int error_code)
{
	char	*error;

	error = NULL;
	if (error_code == EAI_NONAME)
		error = "NAME or SERVICE is unknown";
	else if (error_code == EAI_AGAIN)
		error = "Temporary failure in name resolution";
	else if (error_code == EAI_FAIL)
		error = "Non-recoverable failure in name res";
	else if (error_code == EAI_MEMORY)
		error = "Memory allocation failure";
	else if (error_code == EAI_SYSTEM)
		error = "System error";
	else
		error = "Error in adress resolution";
	printf("ERROR: %s\n", error);
    exit(1);
}

int main()
{

    struct addrinfo hints;
    bzero(&hints, sizeof(hints));
    hints.ai_family = AF_INET6;
    hints.ai_socktype = SOCK_DGRAM;
    hints.ai_protocol = IPPROTO_UDP;

    struct addrinfo *result;
    int ret;
    if ((ret = getaddrinfo("google.com", NULL, &hints, &(result))) != 0)
    {
        printf(">>>> GETADDR FAILED ret = %d\n", ret);
        getaddr_error(ret);
    }
    else
        printf(">>>> GETADDR SUCCEED\n");
    printf("IP = %s\n", inet_ntoa(*(struct in_addr*)result->ai_addr));

    char src_addr[40];
    inet_ntop(AF_INET6, (result->ai_addr), &(src_addr[0]), 40);
    printf("src_addr = %s\n", src_addr);








    int sockfd;

    if ((sockfd = socket(AF_INET6, SOCK_DGRAM, IPPROTO_UDP)) < 0)
    {
        printf(">>>> SOCKET FAILED\n");
        perror("SOCKET: ");
    }
    else
        printf(">>>> SOCKET SUCCEED socket = %d\n", sockfd);

    char sendbuf[32];
    int retsend;

    bzero(&sendbuf, sizeof(sendbuf));
    printf("AVANT SEND\n");

    // ((struct sockaddr_in6 *)result->ai_addr)->sin6_port = htons(33435);



    int set_ttl = 1;

    if ((setsockopt(sockfd, IPPROTO_IPV6, IP_TTL, &set_ttl, (socklen_t)(sizeof(int)))) < 0)
        printf(">>>> SETSOCKOPT FAILED\n");
    else
        printf(">>>> SETSOCK OPT SUCCEED\n");

    struct sockaddr_in6 sendaddr;
    bzero(&(sendaddr), sizeof(sendaddr));

    sendaddr.sin6_family = AF_INET6;
    // if (inet_pton(AF_INET6, "2001:4860:4860::8888", &sendaddr.sin6_addr) != 1)
    //     printf(">>>> INET_PTON FAILED\n");
    // else
    //     printf(">>>> INET_PTON SUCCEED\n");
    sendaddr.sin6_addr = ((struct sockaddr_in6 *)result->ai_addr)->sin6_addr;

    sendaddr.sin6_port = htons(33435);


    int recvsock;

    if ((recvsock = socket(AF_INET6, SOCK_RAW, IPPROTO_ICMPV6)) < 0)
    {
        printf(">>>> SOCKET REC FAILED\n");
        perror("SOCKET: ");
    }
    else
        printf(">>>> SOCKET REC SUCCEED socket = %d\n", sockfd);





    if ((retsend = sendto(sockfd, &sendbuf[0], sizeof(sendbuf), 0, (struct sockaddr*)&sendaddr, (socklen_t)sizeof(sendaddr))) < 0)
    {
        printf(">>>> SEND FAILED ret = %d\n", retsend);
        printf("ERRNO = %d\n", errno);
        perror("SEND ERROR: ");
    }
    else
        printf(">>>> SENDTO SUCCEED ret = %d\n", retsend);

    char buf[4096];

    struct sockaddr_in6 recvaddr;
    int soutLen = sizeof(recvaddr);

    int msgLen = recvfrom(recvsock, buf, sizeof(buf), 0, (struct sockaddr *)&recvaddr, (socklen_t *)&soutLen);
    if (msgLen == -1)
    {
        printf("ERRNO = %d\n", errno);
        perror("RECV ERROR: ");
    }
    printf("msglen = %d\n", msgLen);
}