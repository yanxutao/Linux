#include <sys/types.h>
#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

int
main(int argc, char *argv[])
{
  int pipefd[2];
  pid_t cpid;
  char buf;

  if (argc != 2) {
      fprintf(stderr, "Usage: %s <string>\n", argv[0]);
      exit(EXIT_FAILURE);
  }

  if (pipe(pipefd) == -1) {
      perror("pipe");
      exit(EXIT_FAILURE);
  }

  cpid = fork();
  if (cpid == -1) {
      perror("fork");
      exit(EXIT_FAILURE);
  }

  if (cpid == 0) {
      close(pipefd[1]);

      pid_t pid = getpid();
      printf("child process pid: %d\n", pid);

      char *str = "child process str: ";
      write(STDOUT_FILENO, str, strlen(str));

      while (read(pipefd[0], &buf, 1) > 0) {
          write(STDOUT_FILENO, &buf, 1);
      }

      write(STDOUT_FILENO, "\n", 1);

      close(pipefd[0]);
      exit(EXIT_SUCCESS);

  } else {
      close(pipefd[0]);

      pid_t pid = getpid();
      printf("parent process pid: %d\n", pid);

      char *str = "parent process str: ";
      write(STDOUT_FILENO, str, strlen(str));

      write(pipefd[1], argv[1], strlen(argv[1]));

      write(STDOUT_FILENO, argv[1], strlen(argv[1]));
      write(STDOUT_FILENO, "\n", 1);

      close(pipefd[1]);
      exit(EXIT_SUCCESS);
  }
}
