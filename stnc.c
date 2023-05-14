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
#include "stnc.h"
#include "part_a.c"
#include "uds.c"
#include "filename.c"
#include "createFile.c"
#define ipv4 1
#define ipv6 2
#define uds 3
#define mmap 4
#define pipe 5


int p_flag = 1;

void ClassifiedCommunication(char *side, char *ip, int port, int type, char *param)
{
    if (p_flag)
    {
        printf("handle_%s_%d %d %s\n", side, type, type, param);
    }
    
    if (strcmp(side, "c") == 0)
    {
        switch (type)
        {
            case 1:
                if (strcmp(param, "tcp") == 0)
                {
                    clientIpv4Tcp(ip, port);
                }
                else if (strcmp(param, "udp") == 0)
                {
                    clientIpv4Udp(ip, port);
                }
                break;
                
            case 2:
                if (strcmp(param, "tcp") == 0)
                {
                    clientIpv6Tcp(ip, port);
                }
                else if (strcmp(param, "udp") == 0)
                {
                    clientIpv6Udp(ip, port);
                }
                break;
                
            case 3:
                if (strcmp(param, "dgram") == 0)
                {
                    clientUdsDgram();
                }
                else if (strcmp(param, "stream") == 0)
                {
                    clientUdsStream();
                }
                break;
                
            case 4:
                if (strcmp(param, "filename") == 0)
                {
                    clientMmapFilename();
                }
                break;
                
            case 5:
                if (strcmp(param, "filename") == 0)
                {
                    clientPipeFilename();
                }
                break;
                
            default:
                printf("Unknown option: %d %s\n", type, param);
                break;
        }
    }
    else
    {
        switch (type)
        {
            case 1:
                if (strcmp(param, "tcp") == 0)
                {
                    serverIpv4Tcp(port);
                }
                else if (strcmp(param, "udp") == 0)
                {
                    serverIpv4Udp(port);
                }
                break;
                
            case 2:
                if (strcmp(param, "tcp") == 0)
                {
                    serverIpv6Tcp(port);
                }
                else if (strcmp(param, "udp") == 0)
                {
                    serverIpv6Udp(port);
                }
                break;
                
            case 3:
                if (strcmp(param, "dgram") == 0)
                {
                    serverUdsDgram();
                }
                else if (strcmp(param, "stream") == 0)
                {
                    serverUdsStream();
                }
                break;
                
            case 4:
                if (strcmp(param, "filename") == 0)
                {
                    serverMmapFilename();
                }
                break;
                
            case 5:
                if (strcmp(param, "filename") == 0)
                {
                    serverPipeFilename();
                }
                break;
                
            default:
                printf("Unknown option: %d %s\n", type, param);
                break;
        }
    }
}
