#ifndef UDS_COMMUNICATION_H
#define UDS_COMMUNICATION_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <errno.h>
#include <string.h>
#include "stnc.h"

#define SOCKET_PATH "/tmp/socket_path"
#define BUFFER_SIZE 1024
#define FILE_PATH "100MB-File.txt"

void clientUdsStream();
void serverUdsStream();
void serverUdsDgram();
void clientUdsDgram();

#endif