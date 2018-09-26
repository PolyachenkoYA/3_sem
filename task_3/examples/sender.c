#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <stdio.h>
#include <string.h>

#define MSG_SIZE     1024
// нужна структурка данных, в которой хранится тип сообщения и буфер
typedef struct msgbuf {
    long    mtype;
    char    mtext[MSG_SIZE];
} message_buf;

main()
{
    int msgid;
    int msgflg = IPC_CREAT | 0666;
    key_t key;
    message_buf sbuf;
    size_t buf_length;

    key = 1024; //rewrite with ftok

    if ((msgid = msgget(key, msgflg )) < 0) {
        perror("msgget");
        exit(1);
    }
    else 
     printf("msgget: msgget succeeded: msgid = %d\n", msgid);


    sbuf.mtype = 1;
    
    printf("msgget: msgget succeeded: msgid = %d\n", msgid);
    
    (void) strcpy(sbuf.mtext, "HELLO");
    
    printf("msgget: msgget succeeded: msgid = %d\n", msgid);
    
    buf_length = strlen(sbuf.mtext) + 1 ; // данные сообщения по прежнему считаем '\0'-terminated - строкой.
    
    if (msgsnd(msgid, &sbuf, buf_length, IPC_NOWAIT) < 0) {
       printf ("%d, %d, %s, %d\n", msgid, sbuf.mtype, sbuf.mtext, buf_length);
        perror("msgsnd");
        exit(1);
    }

   else 
      printf("Message: \"%s\" Sent\n", sbuf.mtext);
      
    return 0;
}
