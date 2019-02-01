#include <stdio.h> 
#include <sys/types.h> 
#include <unistd.h> 
#include <stdlib.h>

int cmd_quem(void) {
  int result;
  int pipefd[2];
  FILE *cmd_output;
  char buf[1024];
  int status;

  result = pipe(pipefd);
  if (result < 0) {
    perror("pipe");
    exit(-1);
  }

  result = fork();
  if(result < 0) {
    exit(-1);
  }

  if (result == 0) {
    dup2(pipefd[1], STDOUT_FILENO); /* Duplicate writing end to stdout */
    close(pipefd[0]);
    close(pipefd[1]);

    execl("/usr/bin/ls", "ls", NULL);
    exit(1);
  }

  /* Parent process */
  close(pipefd[1]); /* Close writing end of pipe */

  cmd_output = fdopen(pipefd[0], "r");

  if (fgets(buf, sizeof buf, cmd_output)) {
    printf("Data from who command: %s\n", buf);
  } else {
    printf("No data received.\n");
  }

  exit(&status);
  printf("Child exit status = %d\n", status);

  return 0;
}