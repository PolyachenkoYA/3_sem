#pragma once

#pragma GCC diagnostic ignored "-Wunused-result"

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h> /* mmap() is defined in this header */
#include <sys/ipc.h>
#include <sys/msg.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <pthread.h>
#include <semaphore.h>
#include <sys/shm.h>
#include <string.h>


#include <time.h>
#include "BUF_SIZ.h"

#define SEMAPHORE_NAME_1 "/my_named_posix_semaphore"
#define SEMAPHORE_NAME_2 "/my_named_posix_semaphor"

typedef struct msgbuf {
    long mtype;
    char mtext[BUF_SIZ];
} message_buf;

long my_min(long a, long b);

long save_file(char *filename, char *data, long file_size);

long gen_file(char *filename, long file_size);
// generate a file with FILE_SIZE bytes

char *gen_data(long sz);
// generate SZ random bytes

