#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/time.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>
#include <resolv.h>
#include <errno.h>

int main(int argc, char* argv[])
{
    if(argc > 2){
        printf("usage:\n%s [period=15sec]\n", argv[0]);
        return 0;
    }
    int T = (argc == 2 ? atoi(argv[1]) : 15);

    pid_t parpid;
    if((parpid = fork()) < 0){
        perror("can't fork");
        exit(1);
    }
    else if(parpid != 0){
        printf("backuper pid: %d\n", parpid);
        exit(0);
    }

    setsid();
    fclose(stdin);
    fclose(stdout);
    fclose(stderr);

    while(1){
        if(system("python create_backup.py")){
            system("echo \"backup terminated\" >> backup.log");
            system("rm stop_backup");
            return 0;
        }
        sleep(T);
    }

    return 0;
}
