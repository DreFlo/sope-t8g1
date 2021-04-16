#ifndef _COMMON_H
#define _COMMON_H 1

#include <time.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct {
	int i; 											// request id
	int t;											// task load
	pid_t pid; 										// process id
	pthread_t tid;									// thread id
	int res;										// response
} Message;

#define MAX_OP_LEN 6

typedef enum{
	IWANT, 
	RECVD, 
	TSKEX, 
	TSKDN, 
	GOTRS, 
	LATE,											//2LATE 
	CLOSD, 
	GAVUP, 
	FAILD
} Operation;

/**
 * @brief Prints operation register
 * @param msg Struct that contains the message passed between client and server
 * @param op Operation to register
 */
void output(Message *msg, Operation op);

#endif // _COMMON_H
