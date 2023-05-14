#ifndef TCP_COMMUNICATION_H
#define TCP_COMMUNICATION_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <time.h>
#include <sys/time.h>
#include "classified_communication.h"

#define PORT 7521
#define MESSAGE_SIZE 1024

extern int sockfd_tcp_sock, sockfd_s, newsockfd;

void error(const char *msg);
float time_since(struct timeval start);
void clienTcp(char *type, char *param);
void serverTcp(int port);

#endif