#include <sys/stat.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int
main(int argc, char *argv[])
{
  if (mkfifo("fifo", 0666) < 0 && errno != EEXIST) {
      perror("Create FIFO Failed");
      exit(EXIT_FAILURE);
  }

  int fd;
  if ((fd = open("fifo", O_RDONLY)) < 0) {
      perror("Open FIFO Failed");
      exit(EXIT_FAILURE);
  }

  int len;
  char buf[1024];
  while ((len = read(fd, buf, 1024)) > 0) {
      write(STDOUT_FILENO, buf, len);
  }

  close(fd);
  exit(EXIT_SUCCESS);
}
