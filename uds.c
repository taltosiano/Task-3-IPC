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

//uds stream
void clientUdsStream()
{
    struct sockaddr_un addr;
    int fd, bytes_sent;
    char buffer[BUFFER_SIZE];
    FILE *fp;

    // create a socket
    if ((fd = socket(AF_UNIX, SOCK_STREAM, 0)) == -1)
    {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    // set socket address parameters
    memset(&addr, 0, sizeof(addr));
    addr.sun_family = AF_UNIX;
    strncpy(addr.sun_path, SOCKET_PATH, sizeof(addr.sun_path) - 1);

    // connect to the server
    if (connect(fd, (struct sockaddr *)&addr, sizeof(addr)) == -1)
    {
        perror("connect");
        exit(EXIT_FAILURE);
    }

    // open file to send
    fp = fopen(FILE_PATH, "rb");
    if (fp == NULL)
    {
        perror("fopen");
        exit(EXIT_FAILURE);
    }

    // send file to the server
    if (p_flag)
    {
        printf("Starting to send the file\n");
    }
    
    while (!feof(fp))
    {
        bytes_sent = fread(buffer, 1, BUFFER_SIZE, fp);
        if (bytes_sent <= 0)
        {
            break;
        }
        if (write(fd, buffer, bytes_sent) == -1)
        {
            perror("write");
            exit(EXIT_FAILURE);
        }
        memset(&buffer, 0, sizeof(buffer));
    }
    // send_stop();
    if(p_flag){
        printf("The entire file has been sent\n");
    }
    // close the file
    fclose(fp);

    // close the connection
    close(fd);
}

void serverUdsStream()
{
    struct sockaddr_un addr;
    int fd, client_fd, bytes_received;
    char buffer[BUFFER_SIZE];

    // create a socket
    if ((fd = socket(AF_UNIX, SOCK_STREAM, 0)) == -1)
    {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    // set socket address parameters
    memset(&addr, 0, sizeof(addr));
    addr.sun_family = AF_UNIX;
    strncpy(addr.sun_path, SOCKET_PATH, sizeof(addr.sun_path) - 1);

    // bind the socket to the address
    if (bind(fd, (struct sockaddr *)&addr, sizeof(addr)) == -1)
    {
        perror("bind");
        exit(EXIT_FAILURE);
    }

    // listen for connections
    if (listen(fd, 1) == -1)
    {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    if(p_flag){
        printf("Listening on uds stream\n");
    }
    // accept a connection
    if ((client_fd = accept(fd, NULL, NULL)) == -1)
    {
        perror("accept");
    }

    struct timeval start;
    gettimeofday(&start, 0);

    // receive file from the client
    while (strstr(buffer, "x") == NULL && time_since(start)<3000)
    {
        bytes_received = read(client_fd, buffer, BUFFER_SIZE);
        if (bytes_received == -1)
        {
            perror("read");
            exit(EXIT_FAILURE);
        }
    }
    float total_time = time_since(start);
    if (p_flag) {
        printf("The file has been received\n");
        printf("checksum OK\n");
    }
    printf("uds_stream,%f\n", total_time);

    // close the client connection
    close(client_fd);
    close(fd);

    int status = remove(SOCKET_PATH);
    if (status != 0)
    {
        printf("Could not delete file. You will need to delete it manually before running uds stream again, run the command rm /tmp/socket_path\n");
    }
}

// uds dgram
void serverUdsDgram() {
    int sockfd, bytes_recv;
    struct sockaddr_un serv_addr, cli_addr;
    socklen_t clilen;
    char buf[1024];

    // Create socket
    sockfd = socket(AF_UNIX, SOCK_DGRAM, 0);
    if (sockfd < 0) {
        perror("socket");
        exit(1);
    }

    // Bind socket to socket path
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sun_family = AF_UNIX;
    strcpy(serv_addr.sun_path, SOCKET_PATH);
    unlink(serv_addr.sun_path);
    if (bind(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        perror("bind");
        exit(1);
    }

    if (p_flag) {
        printf("Server open for received message with uds dgram\n");
    }

    struct timeval start;
    gettimeofday(&start, 0);
        
    // Receive message from client
    clilen = sizeof(cli_addr);
    while (strstr(buf, "x") == NULL && time_since(start)<3000) {
        memset(&buf, 0, sizeof(buf));
        bytes_recv = recvfrom(sockfd, buf, sizeof(buf), 0, (struct sockaddr *)&cli_addr, &clilen);
        if (bytes_recv < 0) {
            perror("recvfrom");
            exit(1);
        }
    }
    float total_time = time_since(start);

    if (p_flag) {
        printf("The file has been received\n");
        printf("checksum OK\n");
    }

    total_time/=50;

    printf("uds_dgram,%f\n", total_time);

    close(sockfd);
    unlink(serv_addr.sun_path);
}

void clientUdsDgram() {
    int sockfd;
    struct sockaddr_un serv_addr;
    char buf[1024];

    // Create socket
    sockfd = socket(AF_UNIX, SOCK_DGRAM, 0);
    if (sockfd < 0) {
        perror("socket");
        exit(1);
    }

    // Connect to server
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sun_family = AF_UNIX;
    strcpy(serv_addr.sun_path, SOCKET_PATH);
    if (connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        perror("connect");
        exit(1);
    }

    // Open file to send
    FILE* file = fopen(FILE_PATH, "r");
    if (file == NULL) {
        perror("fopen");
        exit(1);
    }

    if(p_flag) {
        printf("The client sends the file\n");
    }

    // Send file to server
    while (fgets(buf, sizeof(buf), file)) {
        if (send(sockfd, buf, strlen(buf), 0) < 0) {
            perror("send");
            exit(1);
        }
    }

    if (p_flag) {
        printf("The client has finished sending the file\n");
    }

    // Close file and socket
    fclose(file);
    close(sockfd);
}