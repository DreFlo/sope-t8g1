#ifndef MP2_2_SERVER_UTILS_H_
#define MP2_2_SERVER_UTILS_H_

#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include "common.h"



typedef struct {
    pid_t s_pid;
    pthread_t s_tid;
    Message msg;
}   ServerMessage;

typedef struct {
    int i;
    Message msg;
} WorkerMessage;

extern unsigned buffer_length;
extern ServerMessage * buffer;
extern sem_t semaphore;

void enqueue(ServerMessage s_msg);

void dequeue(ServerMessage *s_msg);

#endif