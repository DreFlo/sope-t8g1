#ifndef MP2_1_CLIENT_SIGNALS_H_
#define MP2_1_CLIENT_SIGNALS_H_

#include <signal.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <pthread.h>

extern pthread_t main_thread_id;
extern unsigned int thread_no;
extern pthread_t ids[1024];

/**
 * @brief Handler for SIGPIPE signal received when Server closes named pipe
 */
void sigpipe_handler();

/**
 * @brief Handler for SIGALRM signal received when Client timesout 
 */
void sigalrm_handler();

/**
 * @brief Handler for SIGINT signal 
 */
void sigint_handler();

#endif // MP2_1_CLIENT_SIGNALS_H_