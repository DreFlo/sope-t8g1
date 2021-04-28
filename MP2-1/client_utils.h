#ifndef MP2_1_CLIENT_UTILS_H_
#define MP2_1_CLIENT_UTILS_H_
#define MAX_OP_LEN 6
#define MAX_RANDOM_NUMBER 1000


#include "./stdio.h"
#include "./stdlib.h"
#include "./common.h"
#include "./time.h"

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
 * @brief Prints operation register
 * @param msg Struct that contains the message passed between client and server
 * @param op Operation to register
 */
void output(Message *msg, Operation op);

void *thread_gavup(void *arg);

#endif // MP2_1_CLIENT_UTILS_H_