#include "mylib.h"

long main(long argc, char **argv)
{
    if(argc == 3){
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
        strcpy(parent_filename, filename);
        strcpy(child_filename, filename);
        strcat(parent_filename, "_parent");
        strcat(child_filename, "_child");
        srand(time(0));

        dpipe_t dp;
        dpipe_init_pipes(&dp);

        char sync_msg[2];
        int msgflg = IPC_CREAT | 0666;
        int ftok_salt = 1234;
        int msgid;
        int msg_type = 1;

        pid_t this_is_parent = fork();
        dpipe_cleanup(&dp, this_is_parent);
        if(this_is_parent){
            char *parent_data = gen_data(file_size);
            //save_file(parent_filename, parent_data, file_size);

            if((msgid = msgget(ftok("msg.c", ftok_salt), msgflg)) < 0){
                perror("msgget_sender");
                return EXIT_FAILURE;
            }
            message_buf sbuf;
            //sbuf.mtext = (char*)malloc(sizeof(char) * buf_size);
            sbuf.mtype = msg_type;

            long i, el_left = file_size, shift = 0;
            time_t start_t = clock();
            do{
                memcpy(sbuf.mtext, parent_data + shift, my_min(buf_size, el_left));
                if(msgsnd(msgid, &sbuf, buf_size, 0) < 0){
                    printf ("msg_id = %d, msg_type = %ld, buf_size = %ld, msg_text:\n|%s|\n", msgid, sbuf.mtype, buf_size, sbuf.mtext);
                    perror("msgsnd");
                    return EXIT_FAILURE;
                }
                //printf("msg_id = %d, sent msg:\n|%s|\n", msgid, sbuf.mtext);
                shift += buf_size;
                el_left = file_size - shift;

                //dpipe_send(&dp,(void*)sync_msg, 2, this_is_parent); // our child starts to run
                //dpipe_rcv(&dp,(void*)sync_msg, 2, this_is_parent); // we wait for the child
            }while(el_left > 0);
            double write_t = (double)(clock() - start_t) / CLOCKS_PER_SEC;

            free(parent_data);
            //free(sbuf.mtext);

            dpipe_rcv(&dp,(void*)sync_msg, 2, this_is_parent);
            //printf("write time: %lf\n", write_t);
            printf("%lf\n", write_t);

            /*
            char cmd[1000];
            strcpy(cmd, "./cmp.py");
            strcat(cmd, " ");
            strcat(cmd, parent_filename);
            strcat(cmd, " ");
            strcat(cmd, child_filename);
            printf("%s\n", cmd);
            system(cmd);
            */

        } else {
            char *child_data = (char*)malloc(sizeof(char) * file_size);

            if((msgid = msgget(ftok("msg.c", ftok_salt), msgflg)) < 0){
                perror("msgget_receiver");
                return EXIT_FAILURE;
            }
            message_buf rbuf;
            //rbuf.mtext = (char*)malloc(sizeof(char) * buf_size);
            rbuf.mtype = msg_type;

            long i, el_left = file_size, shift = 0;
            sleep(1);
            time_t start_t = clock();
            do{
                //dpipe_rcv(&dp,(void*)sync_msg, 2, this_is_parent); // wait for write

                if(msgrcv(msgid, &rbuf, buf_size, rbuf.mtype, MSG_NOERROR) < 0){
                    printf ("msg_id = %d, msg_type = %ld, buf_size = %ld, msg_text:\n|%s|\n", msgid, rbuf.mtype, buf_size, rbuf.mtext);
                    perror("msgrcv");
                    return EXIT_FAILURE;
                }
                memcpy(child_data + shift, rbuf.mtext, my_min(buf_size, el_left));
                shift += buf_size;
                el_left = file_size - shift;

                //dpipe_send(&dp,(void*)sync_msg, 2, this_is_parent); // run the parent to write
            }while(el_left > 0);
            double read_t = (double)(clock() - start_t) / CLOCKS_PER_SEC;

            //save_file(child_filename, child_data, file_size);
            //printf("read time: %lf\n", read_t);
            printf("%lf\n", read_t);
            dpipe_send(&dp,(void*)sync_msg, 2, this_is_parent);

            free(child_data);
            //free(rbuf.mtext);
        }
    } else {
        //printf("usage:\n%s file_size buf_size filename\n", argv[0]);
        printf("usage:\n%s file_size filename\n", argv[0]);
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
