#ifndef STNC_H
#define STNC_H

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

#define MAX_BUFFER_SIZE 1024

extern int p_flag;

int socketIpv4Udp();
void clientIpv4Udp(char *ip, int port);
void serverIpv4Udp(int port);

int socketIpv4Tcp();
void clientIpv4Tcp(char *ip, int port);
void serverIpv4Tcp(int port);

int socketIpv6Udp();
void clientIpv6Udp(char *ip, int port);
void serverIpv6Udp(int port);

int socketIpv6Tcp();
void clientIpv6Tcp(char *ip, int port);
void serverIpv6Tcp(int port);

#endif /* STNC_H */