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

char *gen_data(long sz)
{
    long i;
    char *raw_data = (char*)malloc(sizeof(char) * sz);

    for(i = 0; i < sz; ++i)
        //raw_data[i] = (rand() % 10) + '0'; // random char ['0';'9']
        raw_data[i] = '1';

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
