#ifndef MP2_2_SERVER_SIGNALS_H_
#define MP2_2_SERVER_SIGNALS_H_


#include <signal.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <pthread.h>
#include <unistd.h>

extern char * fifoname;

/**
 * @brief Handler for SIGALRM signal received when Client timesout 
 */
void sigalrm_handler();


#endif
