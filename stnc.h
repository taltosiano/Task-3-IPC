#ifndef STNC_H
#define STNC_H
#include "tcp_sock.c"

// Global variable
extern int p_flag;
void ClassifiedCommunication(char *side, char *ip, int port, int type, char *param);

void part_a_start_server(int port);
void part_a_connect_server(char *ip, int port);



//100MB file
void createFile();


#endif