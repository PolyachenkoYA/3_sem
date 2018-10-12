#include "mylib.h"

long main(long argc, char **argv)
{
    if((argc == 3) || (argc == 2)){
        //long buf_size = atol(argv[2]);
        long buf_size = BUF_SIZ;
        if(buf_size > 8192){
            printf("BUF_SIZ can't be > 8192 here\n");
            return EXIT_FAILURE;
        }
        long file_size = atol(argv[1]);
        char *filename = argv[2];
        char parent_filename[100];
        char child_filename[100];
        int dbg = (argc == 3);
        if(dbg){
            strcpy(parent_filename, filename);
            strcpy(child_filename, filename);
            strcat(parent_filename, "_parent");
            strcat(child_filename, "_child");
        }
        srand(time(0));

        sem_t *sem1;
        if((sem1 = sem_open(SEMAPHORE_NAME_1, O_CREAT, 0776, 0)) == SEM_FAILED){
            perror("sem1_open");
            return EXIT_FAILURE;
        }

        int msgflg = IPC_CREAT | 0666;
        int ftok_salt = 1234;
        int msgid;
        int msg_type = 1;
        struct timespec start, stop;

        pid_t this_is_parent = fork();
        if(this_is_parent){
            char *parent_data = gen_data(file_size);
            if(dbg){
                save_file(parent_filename, parent_data, file_size);
            }

            if((msgid = msgget(ftok("msg.c", ftok_salt), msgflg)) < 0){
                perror("msgget_sender");
                return EXIT_FAILURE;
            }
            message_buf sbuf;
            sbuf.mtype = msg_type;

            long i, el_left = file_size, shift = 0;
            if(clock_gettime(CLOCK_MONOTONIC,&start) == -1){
                perror("clock gettime");
                return EXIT_FAILURE;
            }
            do{
                memcpy(sbuf.mtext, parent_data + shift, my_min(buf_size, el_left));
                if(msgsnd(msgid, &sbuf, buf_size, 0) < 0){
                    printf ("msg_id = %d, msg_type = %ld, buf_size = %ld, msg_text:\n|%s|\n", msgid, sbuf.mtype, buf_size, sbuf.mtext);
                    perror("msgsnd");
                    return EXIT_FAILURE;
                }
                shift += buf_size;
                el_left = file_size - shift;

            }while(el_left > 0);
            if( clock_gettime( CLOCK_MONOTONIC, &stop) == -1 ) {
                perror( "clock gettime" );
                return EXIT_FAILURE;
            }
            double write_t = (stop.tv_sec - start.tv_sec) + ( stop.tv_nsec - start.tv_nsec) / 1.0e9;

            free(parent_data);
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
            char *child_data = (char*)malloc(sizeof(char) * file_size);

            if((msgid = msgget(ftok("msg.c", ftok_salt), msgflg)) < 0){
                perror("msgget_receiver");
                return EXIT_FAILURE;
            }
            message_buf rbuf;
            rbuf.mtype = msg_type;

            long i, el_left = file_size, shift = 0;
            if(clock_gettime(CLOCK_MONOTONIC,&start) == -1){
                perror("clock gettime");
                return EXIT_FAILURE;
            }
            do{
                if(msgrcv(msgid, &rbuf, buf_size, rbuf.mtype, MSG_NOERROR) < 0){
                    printf ("msg_id = %d, msg_type = %ld, buf_size = %ld, msg_text:\n|%s|\n", msgid, rbuf.mtype, buf_size, rbuf.mtext);
                    perror("msgrcv");
                    return EXIT_FAILURE;
                }
                memcpy(child_data + shift, rbuf.mtext, my_min(buf_size, el_left));
                shift += buf_size;
                el_left = file_size - shift;
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
            free(child_data);
        }
    } else {
        printf("usage:\n%s file_size [filename]\n", argv[0]);
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
