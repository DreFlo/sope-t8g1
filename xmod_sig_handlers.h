#ifndef SIG_HANDLERS_H
#define SIG_HANDLERS_H

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

bool main_proc;                             /* Is the main process */
pid_t proc_id;                              /* Proccess id */
char * proc_start_path;                     /* Path the process was started with */
unsigned int nftot;                         /* Total number of files found */
unsigned int nfmod;                         /* Total number of files modified */

int sig_no;

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

#endif //SIG_HANDLERS_H
