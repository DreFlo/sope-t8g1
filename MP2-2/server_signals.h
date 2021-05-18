#ifndef MP2_2_SERVER_SIGNALS_H_
#define MP2_2_SERVER_SIGNALS_H_


#include <signal.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <pthread.h>
#include <unistd.h>

#include "server_utils.h"

extern char * fifoname;
extern unsigned int thread_no;
extern pthread_t ids[1024];
extern pthread_t c_id;
extern bool too_late;
extern int fd;

/**
 * @brief Handler for SIGALRM signal received when Client timesout 
 */
void sigalrm_handler();


#endif //MP2_2_SERVER_SIGNALS_H_
