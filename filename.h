#ifndef STNC_H
#define STNC_H

#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <poll.h>
#include <string.h>
#include <sys/time.h>
#include <stdlib.h>
#include <sys/mman.h>

#define FILE_PIPE "file_pipe"
#define SHARED_MEM_NAME "/my_shared_memory"
#define SHARED_MEM_SIZE (256*1024*1024) // 250 MB

extern int p_flag;

class Stnc {
public:
    void serverPipeFilename();
    void clientPipeFilename();
    void serverMmapFilename();
    void clientMmapFilename();
};

#endif
