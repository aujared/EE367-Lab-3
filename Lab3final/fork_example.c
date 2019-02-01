#include <stdio.h> 
#include <sys/types.h> 
#include<sys/wait.h> 
#include <unistd.h> 
#include <stdlib.h>

int main () {
    int result;
    int pipefd[2];
    FILE *cmd_output;
    char buf[1024];
    int status;
    int n;

    result = pipe(pipefd);
    int pid = fork();

    if(pid < 0) { 
        printf("An error has occured");
        exit(-1);
    }
    if(pid == 0) {
        printf("I'm a child\n");
        dup2(pipefd[1], STDOUT_FILENO); /* Duplicate writing end to stdout */
        close(pipefd[0]);
        execl("/usr/bin/ls", "ls", NULL);
        exit(1);
    }
    else {
        printf("I'm a parent :D\n");
        close(pipefd[1]); /* Close writing end of pipe */

        wait(NULL);
        //cmd_output = fdopen(pipefd[0], "r");
         //if (fgets(buf, sizeof buf, cmd_output)) {
             //printf("Data from who command: %s\n", buf);
        //}
        if ((n = read(pipefd[0], buf, 1024)) >= 0) {
            buf[n] = 0;
            printf("read %d bytes from the pipe:  \n\"%s\"\n", n, buf);
        } 
        else {
            printf("No data received.\n");
        }
    }


}