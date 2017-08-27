/*
 * client.c
 * shm, sem and msg
 *
 *  Created on: Aug 27, 2017
 *      Author: yanxutao
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <sys/msg.h>

struct message
{
  long msg_type;
  char msg_text;
};

union semaphore
{
  int val;
  struct semid_ds *buf;
  unsigned short *array;
};

int
sem_init(int sem_id, int value)
{
  union semaphore tmp;
  tmp.val = value;

  if (semctl(sem_id, 0, SETVAL, tmp) == -1)
    {
      perror("server: sem_init error");
      return -1;
    }

  return 0;
}

int
sem_p(int sem_id)
{
  struct sembuf sb;
  sb.sem_num = 0;
  sb.sem_op = -1;
  sb.sem_flg = SEM_UNDO;

  if (semop(sem_id, &sb, 1) == -1)
    {
      perror("server: sem_p error");
      return -1;
    }

  return 0;
}

int
sem_v(int sem_id)
{
  struct sembuf sb;
  sb.sem_num = 0;
  sb.sem_op = 1;
  sb.sem_flg = SEM_UNDO;

  if (semop(sem_id, &sb, 1) == -1)
    {
      perror("server: sem_v error");
      return -1;
    }

  return 0;
}

int
main(int argc, char *argv[])
{
  key_t key = ftok(".", 'z');
  if (key < 0)
    {
      perror("client: ftok error");
      exit(EXIT_FAILURE);
    }

  int shmid = shmget(key, 1024, 0);
  if (shmid == -1)
    {
      perror("client: shmget error");
      exit(EXIT_FAILURE);
    }

  char *shm = (char *)shmat(shmid, 0, 0);
  if (shm == (void *) -1)
    {
      perror("client: shmat error");
      exit(EXIT_FAILURE);
    }

  int mqid = msgget(key, 0);
  if (mqid == -1)
    {
      perror("client: msgget error");
      exit(EXIT_FAILURE);
    }

  int semid = semget(key, 0, 0);
  if (semid == -1)
    {
      perror("client: semget error");
      exit(EXIT_FAILURE);
    }

  printf("s: send data to server\n");
  printf("q: quit\n");

  int flag = 1;
  struct message msg;

  while (flag == 1)
    {
      printf("please input command: ");

      char c;
      scanf("%c", &c);

      switch(c)
      {
        case 's':
          printf("data to send: ");

          sem_p(semid);
          scanf("%s", shm);
          sem_v(semid);
          while ((c = getchar()) != '\n' && c != EOF);

          msg.msg_type = 888;
          msg.msg_text = 'r';
          msgsnd(mqid, &msg, sizeof(msg.msg_text), 0);

          break;
        case 'q':
          msg.msg_type = 888;
          msg.msg_text = 'q';
          msgsnd(mqid, &msg, sizeof(msg.msg_text), 0);

          flag = 0;

          break;
        default:
          printf("unknown command\n");
          while ((c = getchar()) != '\n' && c != EOF);
      }
    }

  shmdt(shm);

  return 0;
}
