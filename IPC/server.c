/*
 * server.c
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
del_sem(int sem_id)
{
  union semaphore tmp;
  if (semctl(sem_id, 0, IPC_RMID, tmp) == -1)
    {
      perror("server: del_sem error");
      return -1;
    }

  return 0;
}

int
get_sem(key_t key)
{
  int sem_id = semget(key, 1, IPC_CREAT | 0666);
  if (sem_id == -1)
    {
      perror("server: get_sem error");
      return -1;
    }

  sem_init(sem_id, 1);
  return sem_id;
}

int
main(int argc, char *argv[])
{
  key_t key = ftok(".", 'z');
  if (key < 0)
    {
      perror("server: ftok error");
      exit(EXIT_FAILURE);
    }

  int shmid = shmget(key, 1024, IPC_CREAT | 0666);
  if (shmid == -1)
    {
      perror("server: shmget error");
      exit(EXIT_FAILURE);
    }

  char *shm = (char *)shmat(shmid, 0, 0);
  if (shm == (void *) -1)
    {
      perror("server: shmat error");
      exit(EXIT_FAILURE);
    }

  int mqid = msgget(key, IPC_CREAT | 0777);
  if (mqid == -1)
    {
      perror("server: msgget error");
      exit(EXIT_FAILURE);
    }

  int semid = get_sem(key);

  struct message msg;

  while (1)
    {
      msgrcv(mqid, &msg, 1, 888, 0);
      if (msg.msg_text == 'q')
        {
          break;
        }
      else if (msg.msg_text == 'r')
        {
          sem_p(semid);
          printf("%s: %s\n", argv[0], shm);
          sem_v(semid);
        }
      else
        {
          printf("oops, something went wrong\n");
        }
    }

  shmdt(shm);

  struct shmid_ds buf1;
  shmctl(shmid, IPC_RMID, &buf1);

  struct msqid_ds buf2;
  msgctl(mqid, IPC_RMID, &buf2);

  del_sem(semid);

  return 0;
}
