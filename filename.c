#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <poll.h>
#include <string.h>
#include <sys/time.h>
#include "stnc.h"
#include <stdlib.h>
#include <sys/mman.h>

#define FILE_PIPE "file_pipe"
#define SHARED_MEM_NAME "/my_shared_memory"
#define SHARED_MEM_SIZE (256*1024*1024) // 250 MB

//pipe
void serverPipeFilename() {
    int fd;
    char buffer[1024];
    struct pollfd pfd;

    // create the named pipe
    mkfifo(FILE_PIPE, 0666);

    // open the named pipe for reading
    fd = open(FILE_PIPE, O_RDONLY);

    // poll for events
    pfd.fd = fd;  // fd: The file descriptor to be monitored
    pfd.events = POLLIN;  // The events to monitor for, POLLIN for data to read
        
    if (p_flag) {
        printf("Server open for received message with pipe filename\n");
    }

    struct timeval start;
    gettimeofday(&start, 0);
    do {
        // wait for an event
        poll(&pfd, 1, -1);

    // read the data from the pipe
    } while (read(fd, buffer, sizeof(buffer)) > 0);
    
    float total_time = time_since(start);
    
    if (p_flag) {
        printf("The file has been received\n");
        printf("checksum OK\n");
    }
    printf("pipe,%f\n", total_time); 
    
    // close the pipe and remove the file
    close(fd);
    unlink(FILE_PIPE);
}


void clientPipeFilename() {
    int fd;
    char buffer[1024];

    // open the named pipe for writing
    fd = open(FILE_PIPE, O_WRONLY);

    // open the file to be sent
    FILE* fp = fopen("100MB-File.txt", "r");

    // read the contents of the file and write them to the pipe
    if(p_flag) {
        printf("The client sends the file\n");
    }
    while (fgets(buffer, sizeof(buffer), fp)) {
        write(fd, buffer, sizeof(buffer));
    }
    if (p_flag) {
        printf("The client has finished sending the file\n");
    }
    
    // close the file and the pipe
    fclose(fp);
    close(fd);
}

//mmap
extern int p_flag;

/**
 * To compile the code you need to write: gcc mmap_filename.c -o mmap_filename -lrt
 * Server: ./mmap_filename -s
 * Client: ./mmap_filename -c
*/

void serverMmapFilename() {
    int fd = shm_open(SHARED_MEM_NAME, O_CREAT | O_RDWR, 0666);
    if (fd == -1) {
        perror("shm_open");
        exit(EXIT_FAILURE);
    }

    if (ftruncate(fd, SHARED_MEM_SIZE) == -1) {
        perror("ftruncate");
        exit(EXIT_FAILURE);
    }

    void *addr = mmap(NULL, SHARED_MEM_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (addr == MAP_FAILED) {
        perror("mmap");
        exit(EXIT_FAILURE);
    }

    if (p_flag) {
        printf("Server open for received message with mmap filename\n");
    }
    

    char *str = (char*)addr;

    while (*str == '\0') {}

    int count=0;
    struct timeval start;
    gettimeofday(&start, 0);
    while (strstr(str, "x") == NULL ) {
        count++;
    }
    float total_time = time_since(start);

    if (p_flag) {
        printf("The file has been received\n");
        printf("checksum OK\n");
    }
    printf("mmap,%f\n", total_time);
    memset(addr, 0, SHARED_MEM_SIZE);
}

void clientMmapFilename() {
    int fd = shm_open(SHARED_MEM_NAME, O_RDWR, 0666);
    if (fd == -1) {
        perror("shm_open");
        exit(EXIT_FAILURE);
    }

    void *addr = mmap(NULL, SHARED_MEM_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (addr == MAP_FAILED) {
        perror("mmap");
        exit(EXIT_FAILURE);
    }

    if (p_flag) {
        printf("The shared memory object was created successfully\n");
    }

    char *str = (char*)addr;

    // Open the file
    FILE *fp = fopen("100MB-File.txt", "rb");
    if (fp == NULL) {
        perror("fopen");
        exit(EXIT_FAILURE);
    }

    // Get the file size
    fseek(fp, 0L, SEEK_END);
    long int file_size = ftell(fp);
    rewind(fp);

    // Copy the file contents to the shared memory
    if (file_size > SHARED_MEM_SIZE) {
        fprintf(stderr, "File size is larger than shared memory size\n");
        exit(EXIT_FAILURE);
    }
    fread(str, sizeof(char), file_size, fp);

    if (p_flag) {
        printf("The entire file has been sent\n");
    }

    // Close the file
    fclose(fp);
}