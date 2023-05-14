#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <time.h>
#include <sys/time.h>

#define PORT 7521
#define MESSAGE_SIZE 1024
void ClassifiedCommunication(char *side, char *ip, int port, int type, char *param);

int sockfd_tcp_sock, sockfd_s, newsockfd;

void error(const char *msg)
{
    perror(msg);
    exit(1);
}

float time_since(struct timeval start)
{
    struct timeval end;
    gettimeofday(&end, 0);
    float milliseconds = (end.tv_sec - start.tv_sec) * 1000.0f + (end.tv_usec - start.tv_usec) / 1000.0f;
    return milliseconds;
    
}

void clienTcp(char *type,char *param)
{
    char buffer[MESSAGE_SIZE];
    int portno;
    struct sockaddr_in serv_addr;
    srand(time(NULL));
    // Client mode.
    sockfd_tcp_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd_tcp_sock < 0)
    {
        error("ERROR opening socket");
    }

    bzero((char *)&serv_addr, sizeof(serv_addr));
    portno = PORT;

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(portno);

    if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0)
    {
        error("ERROR invalid server address");
    }

    if (connect(sockfd_tcp_sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
        error("ERROR connecting to server");
    }
    else
    {
        printf("accept to TCP soket in server\n");
    }
    // Concatenate the two words into the new string
    strcpy(buffer, type);
    strcat(buffer, " ");
    strcat(buffer, param);


    int n = send(sockfd_tcp_sock, buffer, strlen(buffer), 0);
    if (n < 0)
    {
        error("ERROR writing to socket");
    }
    else
    {
        printf("send %s\n",buffer);
    }

    close(sockfd_tcp_sock);
}

void serverTcp(int port)
{
    // time_t start_time;
    int portno;
    socklen_t clilen;
    char buffer[MESSAGE_SIZE];
    struct sockaddr_in serv_addr, cli_addr;
    // int n;
    srand(time(NULL));

    // Server mode.
    sockfd_s = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd_s < 0)
    {
        error("ERROR opening socket");
    }

    bzero((char *)&serv_addr, sizeof(serv_addr));
    portno = PORT;

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(portno);
    int yes = 1;
    if (setsockopt(sockfd_s, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1)
    {
        error("ERROR setting socket option");
    }
    if (bind(sockfd_s, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
        error("ERROR on binding");
    }

    listen(sockfd_s, 5);
    clilen = sizeof(cli_addr);
    newsockfd = accept(sockfd_s, (struct sockaddr *)&cli_addr, &clilen);
    if (newsockfd < 0)
    {
        error("ERROR on accept");
    }

    bzero(buffer, MESSAGE_SIZE);

    int n = recv(newsockfd, buffer, MESSAGE_SIZE - 1, 0);
    if (n < 0)
    {
        error("ERROR reading from socket");
    }
    
    int type1;
    char *type = strtok(buffer, " ");
    if (strcmp(type,"ipv4"))
        type1 = 1;
    else if (strcmp(type,"ipv6"))
        type1 = 2;
    else if (strcmp(type,"uds"))
        type1 = 3;
    else if (strcmp(type,"mmap"))
        type1 = 4;
    else if (strcmp(type,"pipe"))
        type1 = 5;
    char *param = strtok(NULL, " ");
    ClassifiedCommunication("s","",port,type1,param);
}

