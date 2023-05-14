#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <errno.h>

#define BUFFER_SIZE 1024

void part_a_connect_server(char *ip, int port)
{
    int sockfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (sockfd < 0)
    {
        perror("socket");
        exit(1);
    };
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
    printf("Connected to server at %s on port %d\n", ip, port);
    char buffer[BUFFER_SIZE];
    fd_set set;
    FD_ZERO(&set);
    while (1)
    {
        FD_SET(STDIN_FILENO, &set);
        FD_SET(sockfd, &set);
        int max_fd = sockfd > STDIN_FILENO ? sockfd : STDIN_FILENO;
        select(max_fd + 1, &set, NULL, NULL, NULL);
        if (FD_ISSET(sockfd, &set))
        {
            int bytes_recv = recv(sockfd, buffer, BUFFER_SIZE, 0);
            if (bytes_recv < 0)
            {
                perror("recv");
                exit(1);
            }
            else if (bytes_recv == 0)
            {
                printf("Server disconnected\n");
                exit(1);
            }
            buffer[bytes_recv] = '\0';
            printf("Client: %s", buffer);
        }
        if (FD_ISSET(STDIN_FILENO, &set))
        {
            if (fgets(buffer, BUFFER_SIZE, stdin) != NULL)
            {
                int bytes_sent = send(sockfd, buffer, strlen(buffer), 0);
                if (bytes_sent < 0)
                {
                    perror("send");
                    exit(1);
                }
            }
        }
    }
}

void part_a_start_server(int port)
{
    int sockfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (sockfd < 0)
    {
        perror("socket");
        exit(1);
    };
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
    printf("Listening on port %d\n", port);
    socklen_t cliaddrlen = sizeof(cliaddr);
    int connfd = accept(sockfd, (struct sockaddr *)&cliaddr, &cliaddrlen);
    if (connfd < 0)
    {
        perror("accept");
        exit(1);
    }
    char buffer[BUFFER_SIZE];

    fd_set set;
    FD_ZERO(&set);

    while (1)
    {
        FD_SET(STDIN_FILENO, &set);
        FD_SET(connfd, &set);
        int max_fd = connfd > STDIN_FILENO ? connfd : STDIN_FILENO;
        select(max_fd + 1, &set, NULL, NULL, NULL);
        if (FD_ISSET(connfd, &set))
        {
            int bytes_recv = recv(connfd, buffer, BUFFER_SIZE, 0);
            if (bytes_recv < 0)
            {
                perror("recv");
                exit(1);
            }
            else if (bytes_recv == 0)
            {
                printf("Client disconnected\n");
                exit(1);
            }
            buffer[bytes_recv] = '\0';
            printf("Server: %s", buffer);
        }
        if (FD_ISSET(STDIN_FILENO, &set))
        {
            if (fgets(buffer, BUFFER_SIZE, stdin) != NULL)
            {
                int bytes_sent = send(connfd, buffer, strlen(buffer), 0);
                if (bytes_sent < 0)
                {
                    perror("send");
                    exit(1);
                }
            }
        }
    }
}