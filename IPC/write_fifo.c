#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>

int
main(int argc, char *argv[])
{
  int fd;
  if ((fd = open("fifo", O_WRONLY)) < 0)
    {
      perror("open fifo failed");
      exit(EXIT_FAILURE);
    }

  char buf[1024];
  int n = sprintf(buf, "write process pid: %d\n", getpid());
  if (write(fd, buf, n + 1) < 0)
    {
      perror("write fifo failed");
      close(fd);
      exit(EXIT_FAILURE);
    }

  printf("%s: pid is %d\n", argv[0], getpid());

  close(fd);
  exit(EXIT_SUCCESS);
}
