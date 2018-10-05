#include "mylib.h"

long main(long argc, char **argv)
{
    if(argc == 4){
        long file_size = atol(argv[1]);
        long buf_size = atol(argv[2]);
        char *filename = argv[3];
        char parent_filename[100];
        char child_filename[100];
        strcpy(parent_filename, filename);
        strcpy(child_filename, filename);
        strcat(parent_filename, "_parent");
        strcat(child_filename, "_child");
        srand(time(0));

        dpipe_t dp;
        dpipe_init_pipes(&dp);

        void *m1 = mmap(0, buf_size, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
        char *data = m1;
        char msg[2];

        pid_t this_is_parent = fork();
        dpipe_cleanup(&dp, this_is_parent);
        if(this_is_parent){
            char *parent_data = gen_data(file_size);
            //save_file(parent_filename, parent_data, file_size);

            long i, el_left = file_size, shift = 0;
            time_t start_t = clock();
            do{
                //printf("parent is about to write from %ld to %ld\n", shift, shift + my_min(buf_size, el_left) - 1);
                memcpy(data, parent_data + shift,  my_min(buf_size, el_left));
                //printf("written from %ld to %ld; parent_el_left = %ld\n", shift, shift + my_min(buf_size, el_left) - 1, el_left);
                shift += buf_size;
                el_left = file_size - shift;

                dpipe_send(&dp,(void*)msg, 2, this_is_parent); // our child starts to run
                dpipe_rcv(&dp,(void*)msg, 2, this_is_parent); // we wait for the child
            }while(el_left > 0);
            double write_t = (double)(clock() - start_t) / CLOCKS_PER_SEC;

            free(parent_data);
            munmap(m1, buf_size);

            dpipe_rcv(&dp,(void*)msg, 2, this_is_parent);
            //printf("write time: %lf\n", write_t);
            printf("%lf\n", write_t);
            /*
            char cmd[1000];
            strcpy(cmd, "./cmp.py");
            strcat(cmd, " ");
            strcat(cmd, parent_filename);
            strcat(cmd, " ");
            strcat(cmd, child_filename);
            system(cmd);
            //printf("%s\n", (system(cmd) > 0 ? "OK" : "FAIL"));
            */
        } else {
            char *child_data = (char*)malloc(sizeof(char) * file_size);

            long i, el_left = file_size, shift = 0;
            time_t start_t = clock();
            do{
                dpipe_rcv(&dp,(void*)msg, 2, this_is_parent); // wait for write

                //printf("child is about to read from %ld to %ld\n", shift, shift + my_min(buf_size, el_left);
                memcpy(child_data + shift, data, my_min(buf_size, el_left));
                //printf("read from %ld to %ld; child_el_left = %ld\n", shift, shift + my_min(buf_size, el_left) - 1, el_left);
                shift += buf_size;
                el_left = file_size - shift;

                dpipe_send(&dp,(void*)msg, 2, this_is_parent); // run the parent to write
            }while(el_left > 0);
            double read_t = (double)(clock() - start_t) / CLOCKS_PER_SEC;

            //save_file(child_filename, child_data, file_size);
            //printf("read time: %lf\n", read_t);
            printf("%lf\n", read_t);
            dpipe_send(&dp,(void*)msg, 2, this_is_parent);

            free(child_data);
            munmap(m1, buf_size);
        }
    } else {
        printf("usage:\n./exe file_size buf_size filename\n");
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
