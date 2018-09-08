/*
 *  This is a shell emulator
 *
 *  checked functions:
 *  pwd, ls, echo, gcc, cp, rm
 *
 *  redirection of the input/output doesn't work.
 *  Perhaps it's because in
 *  $./my_exe > output
 *  substrings ">" and "output" aren't really arguments of the my_exe, so exec can't handle them properly
 *
 *  cd doesn't work. It doesn't fail and doesn't throw an error status, but it does nothing as far as I could check
 *
 *  compilation:
 *  gcc main.c
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>
#include <sys/wait.h>

size_t string_parser(char *input_s, char *delim, char ***word_array);
int is_present(char c, char *s);

int main()
{
    long int max_cmd_len = sysconf(_SC_ARG_MAX) + 1;
    char *s;
    char **cmd;

    while(1){
        s = malloc(sizeof(char) * max_cmd_len); // it doesn't work if malloc is used only once at the very beginning

        s[0] = 0;
        printf("(empy str for exit) >>>\n");
        scanf("%[^\n]%*c", s);
        if(!(*s)) // exit on empty string
            break;

        printf("     command:\n     |%s|\n", s);

        //string_parser(s, " -,", &cmd);
        string_parser(s, " ,", &cmd); // '-' can't be a delimiter because flags usage format is "-some_flag"

        printf("     command output:\n-------------------------\n");

        pid_t pid = fork();

        if(pid){ // parent
            int status;
            waitpid(pid, &status, 0);
            printf("\n------------------------\n     ret code: %d\n\n", WEXITSTATUS(status));
        } else { // child
            execvp(cmd[0], cmd);
        }

        free(s);
    }

    return 0;
}

size_t string_parser(char *input_s, char *delim, char ***word_array)
{
    size_t n = 0; // number of words
    char *s = input_s;

    while(*s){
        while (is_present(*s, delim)) ++s;
        n += (*s != '\0');
        while (*s && !is_present(*s, delim)) ++s;
    }

    *word_array = malloc(sizeof(char*) * (n + 1)); // +1 for NULL char* in the end
    word_array[n] = NULL;
    if(n){
        size_t i = 0;
        s = input_s;

        while(*s){
            while(is_present(*s, delim)) ++s;
            if(*s){
                const char *q = s;
                while (*s && !is_present(*s, delim)) ++s;

                size_t l = s - q;

                (*word_array)[i] = (char *)malloc(l + 1);

                strncpy((*word_array)[i], q, l);
                (*word_array)[i][l] = '\0';

                ++i;
            }
        }
    }

    return n;
}

int is_present(char c, char *s)
{
    int i;
    int l = strlen(s);

    for(i = 0; i < l; ++i){
        if(c == s[i]){
            return 1;
        }
    }

    return 0;
}
