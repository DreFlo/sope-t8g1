#ifndef _CLIENT_SIGNALS_H
#define _CLIENT_SIGNALS_H

#include "signal.h"
#include "stdlib.h"
#include "stdio.h"

// TODO
/**
 * @brief Handler for SIGPIPE signal received when Server closes named pipe
 */
void sigpipe_handler();

// TODO
/**
 * @brief Handler for SIGALRM signal received when Client timesout 
 */
void sigalrm_handler();

#endif // _CLIENT_SIGNALS_H