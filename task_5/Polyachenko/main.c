#include "mylib.h"

/*
 * std test:
 * make
 *
 * compilation:
 * make compile
 *
 * usage:
 * ./exe     [file_size=100     base_filename="tst"]
 *
 *
 * example:
 * ./a.out 200 mytst
 *
 * copy 200-bytes file
 * Output files will be called "mytst_parent" and "mytst_child"
 */

char *receive_data;
int k = 0;
int file_sz;

void sig_handler(int signo)
{
    switch(signo){
    case SIGUSR1:
        receive_data[k] = '1';
        ++k;
        break;
    case SIGUSR2:
        receive_data[k] = '0';
        ++k;
        break;
    }
    fprintf(stderr, "%lf %% done   \r", (double)k / file_sz * 100); // print is more human-readable, but it causes problems with syncing
    //printf("%lf %% done   \r", (double)k / file_sz * 100);
}

int main(int argc, char **argv)
{
    if(argc > 3){
        printf("usage:\n%s    [file_size    file_name]\n", argv[0]);
        return 0;
    }

    char *filename;
    int filename_sz;
    switch(argc){ // parse input
    case 1: // size & name are default
        file_sz = 100;
        filename_sz = 3;
        filename = (char*)malloc(sizeof(char) * filename_sz);
        strcpy(filename, "tst");
        break;
    case 2: // custom size
        file_sz = atoi(argv[1]);
        filename_sz = 3;
        filename = (char*)malloc(sizeof(char) * filename_sz);
        strcpy(filename, "tst");
    case 3: // custom size & name
        filename = argv[2];
        filename_sz = strlen(filename);
        file_sz = atoi(argv[1]);
    }
    char *parent_filename = (char*)malloc(sizeof(char) * (filename_sz + 10));
    char *child_filename = (char*)malloc(sizeof(char) * (filename_sz + 10));
    strcpy(parent_filename, filename);
    strcpy(child_filename, filename);
    strcat(parent_filename, "_parent");
    strcat(child_filename, "_child");

    sem_t *sem1;
    if((sem1 = sem_open("/mysem", O_CREAT, 0776, 0)) == SEM_FAILED){
        perror("sem1_open");
        return EXIT_FAILURE;
    }

    struct timespec tim_sender, tim2, tim_rec;
    int sleep_t = 5000000; // 10 ms
    tim_sender.tv_sec = 0;
    tim_sender.tv_nsec = sleep_t;
    tim_rec.tv_sec = 0;
    tim_rec.tv_nsec = sleep_t/2*3;


    srand(time(0));
    int i;
    pid_t child_pid = fork();
    if(child_pid){ // sender
        char *parent_data = gen_data(file_sz);
        save_file(parent_filename, parent_data, file_sz);
        sem_wait(sem1);

        for(i = 0; i < file_sz; ++i){
            kill(child_pid, parent_data[i] == '1' ? SIGUSR1 : SIGUSR2);
            nanosleep(&tim_sender, &tim2);
        }

        sem_wait(sem1);

        char cmd[1000];
        strcpy(cmd, "./cmp.py");
        strcat(cmd, " ");
        strcat(cmd, parent_filename);
        strcat(cmd, " ");
        strcat(cmd, child_filename);
        printf("\n%s\n", cmd);
        system(cmd);

        free(parent_data);
        free(parent_filename);
    } else { // receiver
        receive_data = (char*)malloc(sizeof(char) * file_sz);

        if(signal(SIGUSR1, sig_handler) == SIG_ERR)
            printf("\ncan't catch a SIGUSR1\n");
        if(signal(SIGUSR2, sig_handler) == SIG_ERR)
            printf("\ncan't catch a SIGUSR2\n");
        sem_post(sem1);

        while(k < file_sz)
            nanosleep(&tim_sender, &tim2);

        save_file(child_filename, receive_data, file_sz);

        sem_post(sem1);

        free(child_filename);
        free(receive_data);
    }
}
