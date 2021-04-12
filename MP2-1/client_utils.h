#ifndef CLIENT_UTILS_H
#define CLIENT_UTILS_H

#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <pthread.h>
#include <time.h>
#include <unistd.h>

struct message {
    int rid; /* request id */
    pid_t pid;   /* process id */
    pthread_t tid;   /* thread id */
    int tskload; /* task load */
    int tskres; /* task result */
} msg_t, *msg_p;

/**
 * @brief Read message from open fifo into msg
 * 
 * @param thread_fd file descriptor of named pipe
 * @param msg pointer to struct to load message info into
 */
void read_msg_from_server(int thread_fd, struct message * msg);

#endif  // CLIENT_UTILS_H