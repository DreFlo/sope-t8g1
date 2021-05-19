#ifndef MP2_2_SERVER_UTILS_H_
#define MP2_2_SERVER_UTILS_H_
#define MAX_OP_LEN 6

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdbool.h>
#include <unistd.h>
#include <string.h>
#include "common.h"

typedef struct
{
	pid_t s_pid;
	pthread_t s_tid;
	Message msg;
} ServerMessage;

typedef struct
{
	int i;
	Message msg;
} WorkerMessage;

typedef enum
{
	IWANT,
	RECVD,
	TSKEX,
	TSKDN,
	GOTRS,
	LATE, //2LATE
	CLOSD,
	GAVUP,
	FAILD
} Operation;

extern int buffer_length;
extern ServerMessage *buffer;
extern bool too_late;
extern int thread_no;

bool enqueue(ServerMessage s_msg);

ServerMessage * dequeue();

bool queue_empty();

/**
 * @brief Prints operation register
 * @param msg Struct that contains the message passed between client and server
 * @param op Operation to register
 */
void output(Message *msg, Operation op);

void consumer_cleanup(void * arg);

#endif  // MP2_2_SERVER_UTILS_H_