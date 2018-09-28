#include "mylib.h"
// cmp

long main(long argc, char **argv)
{
    if(argc == 3){
        char **filename = (char**)malloc(sizeof(char*) * 2);
        char **file_data = (char**)malloc(sizeof(char*) * 2);
        long file_size[2];
        long i;

        // load files
        for(i = 0; i < 2; ++i){
            filename[i] = argv[i + 1];
            load_file(filename[i], &(file_data[i]), &(file_size[i]));
        }

        long ans = (file_size[0] == file_size[1] ? cmp_files(file_data, file_size[0]) : 0);
        printf("%s\n%s\n%s\n", filename[0], ans ? "=" : "!=", filename[1]);
        return ans;
    } else {
        printf("usage:\n./exe filepath1 filepath2\n");
        return -1;
    }

    return -2;
}

