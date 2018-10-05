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

#include <time.h>
#include "BUF_SIZ.h"

typedef struct {
  int p2c[2];
  int c2p[2];
} dpipe_t;

typedef struct msgbuf {
    long mtype;
    //char *mtext;
    //char mtext[100];
    char mtext[BUF_SIZ];
} message_buf;

long my_min(long a, long b);

long save_file(char *filename, char *data, long file_size);

long gen_file(char *filename, long file_size);
// generate a file with FILE_SIZE bytes

char *gen_data(long sz);
// generate SZ random bytes

long load_file(char *filename, char** new_data, long *file_size);
// allocate memory & load file into it

long cmp_files(char **files, long file_size);
// 1 if files are identical, 0 otherwise

long dpipe_init_pipes(dpipe_t *dp);
long dpipe_cleanup(dpipe_t *dp, long is_parent);
ssize_t dpipe_send(dpipe_t *dp, const void *msg, size_t sz, long is_parent);
ssize_t dpipe_rcv(dpipe_t *dp, void *msg, size_t sz, long is_parent);

