#include "mylib.h"
// mmap1

long main(long argc, char **argv)
{
    if((argc == 2) || (argc == 3)){
        long file_size = atol(argv[1]);

        srand(time(0));
        char *filename;
        if(argc == 2){
            filename = (char*)malloc(sizeof(char) * 4);
            //filename = "tst";
            filename[0] = 't';
            filename[1] = 's';
            filename[2] = 't';
            filename[3] = 0;
        } else {
            filename = argv[2];
        }
        gen_file(filename, file_size);
        printf("file created:\n%s\n", filename);

        /*
        // check load
        char *nf;
        long ni;
        load_file(filename, &nf, &ni);
        printf("loaded file:\n%ld\n|%s|\n", ni, nf);
        */

        if(argc == 2)
            free(filename);
    } else {
        printf("usage:\n./exe file_size [filename]\n");
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
