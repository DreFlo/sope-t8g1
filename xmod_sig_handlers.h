#ifndef _SIG_HANDLERS_H_
#define _SIG_HANDLERS_H_

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <dirent.h>
#include <signal.h>
#include <time.h>
#include "xmod_macros.h"

extern bool main_proc;
extern pid_t proc_id;
extern char * proc_start_path;
extern unsigned int nftot;
extern unsigned int nfmod;

/**
 * @brief Handler for SIGINT signals 
 */
void sigint_handler();

/**
 * @brief Handler for SIGQUIT signals
 */
void sigquit_handler();

/**
 * @brief Handler for SIGCONT signals
 */
void sigcont_handler();

#endif //_SIG_HANDLERS_H_
