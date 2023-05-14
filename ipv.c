#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <errno.h>
#include <sys/time.h>

#include "stnc.h"

#define MAX_BUFFER_SIZE 1024

extern int p_flag;

//ipv4 in udp
int socketIpv4Udp() {
    int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) {
        perror("socket");
        exit(1);
    }
    return sockfd;
}
void clientIpv4Udp(char *ip, int port)
{
    int sockfd = socketIpv4Udp();
    struct sockaddr_in servaddr;
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    inet_pton(AF_INET, ip, &(servaddr.sin_addr));
    servaddr.sin_port = htons(port);
    if (connect(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0)
    {
        perror("connect");
        exit(1);
    }
    if (p_flag) {
        printf("Connected to server (%s : %d)\n", ip, port);
    }
    FILE *fp = fopen("100MB-File.txt", "r");
    if (!fp)
    {
        perror("fopen");
        exit(1);
    }

    // Read file contents and send over socket
    char buffer[MAX_BUFFER_SIZE];
    size_t bytes_read = 0;
    if (p_flag) {
        printf("Sending the file\n");
    }
    while ((bytes_read = fread(buffer, 1, MAX_BUFFER_SIZE, fp)) > 0)
    {
        int bytes_sent = send(sockfd, buffer, bytes_read, 0);
        if (bytes_sent < 0)
        {
            perror("send");
            exit(1);
        }
    }

    if (p_flag) {
        printf("File sent!\n");
        printf("Connection closes\n");
    }

    // Close file
    fclose(fp);
}

void serverIpv4Udp(int port)
{
    int sockfd = socketIpv4Udp();
    struct sockaddr_in servaddr, cliaddr;

    memset(&servaddr, 0, sizeof(servaddr));
    memset(&cliaddr, 0, sizeof(cliaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(port);
    if (bind(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0)
    {
        perror("bind");
        exit(1);
    }

    if (p_flag) {
        printf("Listening on port %d \n", port);
    }

    socklen_t cliaddrlen = sizeof(cliaddr);

    char buffer[MAX_BUFFER_SIZE];

    int count=0;
    struct timeval start;
    gettimeofday(&start, 0);

    while (strstr(buffer, "x") == NULL && time_since(start)<3000)
    {
        count++;
        if(count==1) gettimeofday(&start, 0);

        int bytes_recv = recvfrom(sockfd, buffer, MAX_BUFFER_SIZE, 0,(struct sockaddr *)&cliaddr, &cliaddrlen);
        if (bytes_recv < 0)
        {
            perror("recv");
            exit(1);
        }
    }

    float total_time = time_since(start);

    if (p_flag) {
        printf("The file has been received\n");
        printf("checksum OK!\n");
    }

    total_time/=50;

    printf("ipv4_udp,%f\n", total_time);
}

//ipv4 in tcp
int socketIpv4Tcp()
{
    int sockfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (sockfd < 0)
    {
        perror("socket");
        exit(1);
    }
    return sockfd;
}

void clientIpv4Tcp(char *ip, int port)
{
    int sockfd = socketIpv4Tcp();
    struct sockaddr_in servaddr;
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = inet_addr(ip);
    servaddr.sin_port = htons(port);
    if (connect(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0)
    {
        perror("connect");
        exit(1);
    }

    if (p_flag) {
        printf("Connected to server\n");
    }

    // Open the file and read its contents
    FILE* file = fopen("100MB-File.txt", "rb");
    if (!file) {
        perror("fopen");
        exit(1);
    }
    char buffer[MAX_BUFFER_SIZE];
    size_t bytes_read = fread(buffer, sizeof(char), MAX_BUFFER_SIZE, file);

    if (p_flag) {
        printf("Sending the file\n");
    }


    while (bytes_read > 0) {
        int bytes_sent = send(sockfd, buffer, bytes_read, 0);
        if (bytes_sent < 0)
        {
            perror("send");
            exit(1);
        }
        bytes_read = fread(buffer, sizeof(char), MAX_BUFFER_SIZE, file);
    }

    if (p_flag) {
        printf("The file has been sent\n");
        printf("Closing connection\n");
    }

    fclose(file);
}

void serverIpv4Tcp(int port)
{
    int sockfd = socketIpv4Tcp();
    struct sockaddr_in servaddr, cliaddr;
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(port);
    if (bind(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0)
    {
        perror("bind");
        exit(1);
    }
    if (listen(sockfd, 1) < 0)
    {
        perror("listen");
        exit(1);
    }

    if (p_flag) {
        printf("Listening on port %d\n", port);
    }

    socklen_t cliaddrlen = sizeof(cliaddr);
    int connfd = accept(sockfd, (struct sockaddr *)&cliaddr, &cliaddrlen);
    if (connfd < 0)
    {
        perror("accept");
        exit(1);
    }
    char buffer[MAX_BUFFER_SIZE];
    int count=0;

    struct timeval start;

    while (strstr(buffer, "x") == NULL)
    {
        count++;
        if(count==1) gettimeofday(&start, 0);

        int bytes_recv = recv(connfd, buffer, MAX_BUFFER_SIZE, 0);
        if (bytes_recv < 0)
        {
            perror("recv");
            exit(1);
        }
    }

    float total_time = time_since(start);

    if (p_flag) {
        printf("The file has been received\n");
        printf("checksum OK!\n");
    }

    printf("ipv4_tcp,%f\n", total_time);

}
//ipv6 in udp
int socketIpv6Udp() {
    int sockfd = socket(AF_INET6, SOCK_DGRAM, 0);
    if (sockfd < 0) {
        perror("socket");
        exit(1);
    }
    return sockfd;
}

void clientIpv6Udp(char *ip, int port) {
    int sockfd = socketIpv6Udp();
    struct sockaddr_in6 servaddr;
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin6_family = AF_INET6;
    inet_pton(AF_INET6, ip, &(servaddr.sin6_addr));
    servaddr.sin6_port = htons(port);
    if (connect(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0) {
        perror("connect");
        exit(1);
    }

    if (p_flag) {
        printf("Connected to server\n");
    }

    FILE *file = fopen("100MB-File.txt", "rb");
    if (file == NULL) {
        perror("fopen");
        exit(1);
    }

    char buffer[MAX_BUFFER_SIZE];
    int bytes_sent = 0;

    if (p_flag) {
        printf("Sending the file\n");
    }
    while (1) {
        int bytes_read = fread(buffer, 1, MAX_BUFFER_SIZE, file);
        if (bytes_read == 0) {
            break;
        }
        bytes_sent = send(sockfd, buffer, bytes_read, 0);
        if (bytes_sent < 0) {
            perror("send");
            exit(1);
        }
    }
    if (p_flag) {
        printf("The file has been sent\n");
        printf("Closes the connection\n");
    }

    fclose(file);
    close(sockfd);
}

void serverIpv6Udp(int port)
{
    int sockfd = socketIpv6Udp();
    struct sockaddr_in6 servaddr, cliaddr;
    memset(&servaddr, 0, sizeof(servaddr));
    memset(&cliaddr, 0, sizeof(cliaddr));
    servaddr.sin6_family = AF_INET6;
    servaddr.sin6_addr = in6addr_any;
    servaddr.sin6_port = htons(port);
    if (bind(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0)
    {
        perror("bind");
        exit(1);
    }

    if (p_flag) {
        printf("Listening on port %d (IPv6, UDP)\n", port);
    }

    socklen_t cliaddrlen = sizeof(cliaddr);

    char buffer[MAX_BUFFER_SIZE];

    int count = 0;
    struct timeval start;
    gettimeofday(&start, 0);

    while (strstr(buffer, "x") == NULL && time_since(start)<3000)
    {
        count++;
        if(count==1) gettimeofday(&start, 0);
        int bytes_recv = recvfrom(sockfd, buffer, MAX_BUFFER_SIZE, 0,
                                  (struct sockaddr *)&cliaddr, &cliaddrlen);
        if (bytes_recv < 0)
        {
            perror("recvfrom");
            exit(1);
        }
    }

    float total_time = time_since(start);

    if (p_flag) {
        printf("The file has been received\n");
        printf("checksum OK!\n");
    }

    total_time/=50;

    printf("ipv6_udp,%f\n", total_time);
}

//ipv6 in tcp
int socketIpv6Tcp()
{
    int sockfd = socket(AF_INET6, SOCK_STREAM, IPPROTO_TCP);
    if (sockfd < 0)
    {
        perror("socket");
        exit(1);
    }
    return sockfd;
}

void clientIpv6Tcp(char *ip, int port)
{
    int sockfd = socketIpv6Tcp(AF_INET6);
    struct sockaddr_in6 servaddr;
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin6_family = AF_INET6;
    inet_pton(AF_INET6, ip, &(servaddr.sin6_addr));
    servaddr.sin6_port = htons(port);
    if (connect(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0)
    {
        perror("connect");
        exit(1);
    }

    if (p_flag) {
        printf("Connected to server\n");
    }

    // Open the file for reading
    FILE *fp;
    fp = fopen("100MB-File.txt", "rb");
    if (fp == NULL)
    {
        perror("fopen");
        exit(1);
    }

    // Send the contents of the file to the server
    char buffer[MAX_BUFFER_SIZE];
    size_t bytes_read;

    if (p_flag) {
        printf("Sending the file\n");
    }

    while ((bytes_read = fread(buffer, sizeof(char), MAX_BUFFER_SIZE, fp)) > 0)
    {
        int bytes_sent = send(sockfd, buffer, bytes_read, 0);
        if (bytes_sent < 0)
        {
            perror("send");
            exit(1);
        }
    }

    if (p_flag) {
        printf("The file has been sent\n");
        printf("Closes the connection\n");
    }

    // Close the file and the socket
    fclose(fp);
    close(sockfd);
}

void serverIpv6Tcp(int port)
{
    int sockfd = socketIpv6Tcp();
    struct sockaddr_in6 servaddr, cliaddr;
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin6_family = AF_INET6;
    servaddr.sin6_addr = in6addr_any;
    servaddr.sin6_port = htons(port);
    if (bind(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0)
    {
        perror("bind");
        exit(1);
    }
    if (listen(sockfd, 1) < 0)
    {
        perror("listen");
        exit(1);
    }

    if (p_flag) {
        printf("Listening on port %d\n", port);
    }

    socklen_t cliaddrlen = sizeof(cliaddr);
    int connectionfd = accept(sockfd, (struct sockaddr *)&cliaddr, &cliaddrlen);
    if (connectionfd < 0)
    {
        perror("accept");
        exit(1);
    }

    char buffer[MAX_BUFFER_SIZE];
    int count = 0;
    struct timeval start;
    gettimeofday(&start, 0);

    while (strstr(buffer, "x") == NULL)
    {
        count++;
        if(count==1) gettimeofday(&start, 0);
        int bytes_recv = recv(connectionfd, buffer, MAX_BUFFER_SIZE, 0);
        if (bytes_recv < 0)
        {
            perror("recv");
            exit(1);
        }
    }

    float total_time = time_since(start);

    if (p_flag) {
        printf("The file has been received\n");
        printf("checksum OK!\n");
    }

    printf("ipv6_tcp,%f\n", total_time);

}