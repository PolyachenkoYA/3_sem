#include "mylib.h"

long main(long argc, char **argv)
{
    if((argc == 4) || (argc == 3)){
        long file_size = atol(argv[1]);
        long buf_size = atol(argv[2]);
        char *filename = argv[3];
        char parent_filename[100];
        char child_filename[100];
        int dbg = (argc == 4);
        if(dbg){
            strcpy(parent_filename, filename);
            strcpy(child_filename, filename);
            strcat(parent_filename, "_parent");
            strcat(child_filename, "_child");
        }
        srand(time(0));

        sem_t *sem1;
        sem_t *sem2;
        if((sem1 = sem_open(SEMAPHORE_NAME_1, O_CREAT, 0776, 0)) == SEM_FAILED){
            perror("sem1_open");
            return EXIT_FAILURE;
        }
        if((sem2 = sem_open(SEMAPHORE_NAME_2, O_CREAT, 0776, 0)) == SEM_FAILED){
            perror("sem2_open");
            return EXIT_FAILURE;
        }

        char *data; /* pointer to the shm */
        int shmid = shmget(ftok("shm.c", 1234), buf_size, IPC_CREAT | 0666);
        struct timespec start, stop;

        pid_t this_is_parent = fork();
        if(this_is_parent){
            data = shmat(shmid, (void *) 0, 0);
            char *parent_data = gen_data(file_size);
            if(dbg){
                save_file(parent_filename, parent_data, file_size);
            }

            long i, el_left = file_size, shift = 0;
            if(clock_gettime(CLOCK_MONOTONIC,&start) == -1){
                perror("clock gettime");
                return EXIT_FAILURE;
            }
            do{
                memcpy(data, parent_data + shift,  my_min(buf_size, el_left));
                shift += buf_size;
                el_left = file_size - shift;

                sem_post(sem1);
                sem_wait(sem2);
            }while(el_left > 0);
            if( clock_gettime( CLOCK_MONOTONIC, &stop) == -1 ) {
                perror( "clock gettime" );
                return EXIT_FAILURE;
            }
            double write_t = (stop.tv_sec - start.tv_sec) + ( stop.tv_nsec - start.tv_nsec) / 1.0e9;

            free(parent_data);
            shmctl(shmid, IPC_RMID, 0);
            sem_wait(sem1);
            sem_close(sem1);

            if(dbg){
                printf("write time: %lf\n", write_t);
                char cmd[1000];
                strcpy(cmd, "./cmp.py");
                strcat(cmd, " ");
                strcat(cmd, parent_filename);
                strcat(cmd, " ");
                strcat(cmd, child_filename);
                printf("%s\n", cmd);
                system(cmd);
            } else {
                printf("%lf\n", write_t);
            }
        } else {
            data = shmat(shmid, (void *) 0, 0);
            char *child_data = (char*)malloc(sizeof(char) * file_size);

            long i, el_left = file_size, shift = 0;
            if(clock_gettime(CLOCK_MONOTONIC,&start) == -1){
                perror("clock gettime");
                return EXIT_FAILURE;
            }
            do{
                sem_wait(sem1);

                memcpy(child_data + shift, data, my_min(buf_size, el_left));
                shift += buf_size;
                el_left = file_size - shift;

                sem_post(sem2);
            }while(el_left > 0);
            if( clock_gettime( CLOCK_MONOTONIC, &stop) == -1 ) {
                perror( "clock gettime" );
                return EXIT_FAILURE;
            }
            double read_t = (stop.tv_sec - start.tv_sec) + ( stop.tv_nsec - start.tv_nsec) / 1.0e9;

            if(dbg){
                save_file(child_filename, child_data, file_size);
                printf("read time: %lf\n", read_t);
            } else {
                printf("%lf\n", read_t);
            }

            sem_post(sem1);
            sem_close(sem2);
            free(child_data);
            //shmctl(shmid, IPC_RMID, 0);
        }
    } else {
        printf("usage:\n%s file_size buf_size [filename]\n", argv[0]);
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
