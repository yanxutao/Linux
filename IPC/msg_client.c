/*
 * msg_client.c
 *
 *  Created on: Aug 27, 2017
 *      Author: yanxutao
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/msg.h>

struct message
{
  long msg_type;
  char msg_text[256];
};

int
main(int argc, char *argv[])
{
  key_t key = ftok(".", 'z');
  if (key < 0)
    {
      perror("msg_client: ftok error");
      exit(EXIT_FAILURE);
    }

  printf("%s: message key is %d.\n", argv[0], key);

  int msqid = msgget(key, IPC_CREAT | 0777);
  if (msqid == -1)
    {
      perror("msg_client: msgget error");
      exit(EXIT_FAILURE);
    }

  printf("%s: msqid is %d.\n", argv[0], msqid);
  printf("%s: pid is %d.\n", argv[0], getpid());

  struct message msg;

  msg.msg_type = 888;
  sprintf(msg.msg_text, "Hello, I'm %s %d", argv[0], getpid());
  msgsnd(msqid, &msg, sizeof(msg.msg_text), 0);

  msgrcv(msqid, &msg, 256, 999, 0);
  printf("%s: receive message. mtype is: %ld.\n", argv[0], msg.msg_type);
  printf("%s: receive message. mtext is: %s.\n", argv[0], msg.msg_text);

  return 0;
}
