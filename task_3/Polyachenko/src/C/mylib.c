#include "mylib.h"

long my_min(long a, long b){ return a < b ? a : b; }

long save_file(char *filename, char *data, long file_size)
{
    // open file
    FILE *new_file;
    new_file = fopen(filename,"w");

    // error check
    if(!new_file){
        perror(filename);
        return EXIT_FAILURE;
    }

    fprintf(new_file, "%ld\n%s", file_size, data);

    // safe exit
    // fclose() fails if file_size is in [1000;5000]
    if(fclose(new_file) != 0){
        perror(filename);
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}

long load_file(char *filename, char** new_data, long *file_size)
{
    // open file
    FILE *new_file;
    new_file = fopen(filename,"r");

    // error check
    if(!new_file){
        perror(filename);
        return EXIT_FAILURE;
    }

    // read size for allacation
    fscanf(new_file, "%ld", file_size);
    // read the whole file at once
    *new_data = (char*)malloc(sizeof(char) * (*file_size + 10));
    fscanf(new_file, "%s", *new_data);
    new_data[*file_size] = 0; // NULL terminator

    // safe exit
    // fclose() fails if file_size is in [1000;5000]
    if(fclose(new_file) != 0){
        perror(filename);
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}

char *gen_data(long sz)
{
    long i;
    char *raw_data = (char*)malloc(sizeof(char) * sz);

    for(i = 0; i < sz; ++i)
        raw_data[i] = (rand() % 10) + '0'; // random char ['0';'9']

    return raw_data;
}

long gen_file(char *filename, long file_size)
// generate a file with FILE_SIZE bytes
{
    // generate random data
    char *raw_data = gen_data(file_size + 1);
    raw_data[file_size] = 0; // NULL terminator

    save_file(filename, raw_data, file_size);

    // safe exit
    free(raw_data);
    return EXIT_SUCCESS;
}

long cmp_files(char **files, long file_size)
// 1 if files are identical, 0 otherwise
{
    long i;

    for(i = 0; i < file_size; ++i){
        if(files[0][i] != files[1][i]){
            return 0;
        }
    }

    return 1;
}

// ------------------------------ double pipe ---------------------------

long dpipe_init_pipes(dpipe_t *dp)
{
    if((pipe(dp->p2c) < 0) || (pipe(dp->c2p) < 0)){
        printf("Pipe creation has failed!\n");
        return -1;
    }

    return 0;
}

long dpipe_cleanup(dpipe_t *dp, long is_parent)
{
    if(is_parent){
        // parent can write to p2c[1] and read from c2p[0]
        close(dp->p2c[0]);
        close(dp->c2p[1]);
    } else {
        // child can read from p2c[0] and write to c2p[1]
        close(dp->p2c[1]);
        close(dp->c2p[0]);
    }

    return 0;
}


ssize_t dpipe_send(dpipe_t *dp, const void *msg, size_t sz, long is_parent)
{
    return write(is_parent ? dp->p2c[1] : dp->c2p[1], msg, sz);
}

ssize_t dpipe_rcv(dpipe_t *dp, void *msg, size_t sz, long is_parent)
{
    return read(is_parent ? dp->c2p[0] : dp->p2c[0], msg, sz);
}

