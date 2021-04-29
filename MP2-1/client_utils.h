#ifndef MP2_1_CLIENT_UTILS_H_
#define MP2_1_CLIENT_UTILS_H_
#define MAX_OP_LEN 6
#define MAX_RANDOM_NUMBER 1000


#include "stdio.h"
#include "stdlib.h"
#include "common.h"
#include "time.h"
#include <unistd.h>

typedef struct {
	Message msg;
	char * fifopath;
} Cancelation;

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

/**
 * @brief Prints the format of the command line arguments of the program and exits. 
 */
void print_usage();

/**
 * @brief Prints operation register
 * @param msg Struct that contains the message passed between client and server
 * @param op Operation to register
 */
void output(Message *msg, Operation op);

/**
 * @brief Routine used when thread is cancelled 
 * @param arg Pointer to a Cancelation struct
 */
void thread_gavup(void *arg);

#endif // MP2_1_CLIENT_UTILS_H_