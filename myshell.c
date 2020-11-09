#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

#define MAX_COUNT 50
#define MAX_STR 100



void print_prompt() {
    printf("sunghyun$ ");
}

void read_command(char *command, char *argv[]) {
    
    int i = 0;
    char str[MAX_STR];
    fgets(str, MAX_STR, stdin);

    char *ptr = strtok(str, " \n\t");    //첫번째 strtok 사용.
    while (ptr != NULL)              //ptr이 NULL일때까지 (= strtok 함수가 NULL을 반환할때까지)
    {
        argv[i++] = strdup(ptr);
        ptr = strtok(NULL, " \n\t");     //자른 문자 다음부터 구분자 또 찾기
    }
    argv[i] = '\0';
    strcpy(command, argv[0]);
}


int main() {
    pid_t pid;
    char command[MAX_STR];
    char *argv[MAX_COUNT];

    while(1){
        print_prompt();
        read_command(command, argv);

        if(strcmp(command, "exit") == 0)
            exit(0);

        pid = fork();
        switch(pid)
        {
            case -1:
                perror("fork failed");
                exit(1);
            
            /*child process*/
            case 0:
                printf("(Child start) \n");
                execvp(command, argv);
                break;

            /*parent process*/
            default:
                printf("(Parent wait) \n");
                wait(NULL);
                printf("(Parent wait finished) \n");
        }
    }
}
