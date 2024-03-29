#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <pthread.h>
#include <time.h>
#include <unistd.h>
#include <stdbool.h>
#include "common.h"
#include "client_utils.h"
#include "client_signals.h"

unsigned int thread_no = 0; /* number of threads in the process */
int fifo_file;  /* public fifo file descriptor */

pthread_t ids[1024];    /* array with thread ids */
pthread_mutex_t mutex;  /* mutex for writing to public fifo */

/**
 * @brief Worker thread function. Handles all an individual thread does.
 * @param arg Pointer to a request number
 * @return void* 
 */
void *thread_rot(void *arg)
{
    // block SIGPIPE and SIGALRM to ensure only main thread handles them
    sigset_t set;
    sigemptyset(&set);
    sigaddset(&set, SIGPIPE);
    sigaddset(&set, SIGALRM);
    pthread_sigmask(SIG_BLOCK, &set, NULL);

    // store request number locally and free arg pointer
    int i = *(int *)arg;
    free(arg);

    //------------------------------------

    // create and assign needed member variables 
    char * thread_fifo_path = (char *) malloc(256);
    int thread_fifo;
    int t = random() % 9 + 1;

    Message msg = {i, getpid(), pthread_self(), t, -1};

    // format string
    snprintf(thread_fifo_path, 256, "/tmp/%d.%lu", getpid(), pthread_self());

    if (mkfifo(thread_fifo_path, ALLPERMS) != 0)
    {
        perror("[client] failed to create private fifo");
        exit(EXIT_FAILURE);
    }

    // write info to public fifo
    // begin critical writing region---------------------------

    pthread_mutex_lock(&mutex);

    int num = write(fifo_file, (void *)&msg, sizeof(Message));

    pthread_mutex_unlock(&mutex);

    // end critical writing region-----------------------------

    // set GAVUP message in case of timeout
    Cancelation * gavup_msg = malloc(sizeof(Cancelation));

    gavup_msg->msg.pid = getpid();
    gavup_msg->msg.tid = pthread_self();
    gavup_msg->msg.rid = i;
    gavup_msg->msg.tskload = t;
    gavup_msg->msg.tskres = -1;
    gavup_msg->fifopath = thread_fifo_path;

    pthread_cleanup_push(thread_gavup, (void *) gavup_msg);

    // block SIGTERM (used to terminate threads that haven't made a request to server)
    sigemptyset(&set);
    sigaddset(&set, SIGTERM);
    pthread_sigmask(SIG_BLOCK, &set, NULL);

    // print request sent message
    if (num < 0)
    {
        perror("[client] failed to write in public fifo");
        exit(EXIT_FAILURE);
    }
    else if (num > 0)
    {
        Message r_msg = {i, getpid(), pthread_self(), t, -1};
        output(&r_msg, IWANT);
    }

    // open private fifo, waits for server
    while ((thread_fifo = open(thread_fifo_path, O_RDONLY)) < 0)
        ;

    // read server response
    num = read(thread_fifo, (void *)&msg, sizeof(Message));

    // thread already received response, disable GAVUP
    pthread_cleanup_pop(0);

    msg.pid = getpid();
    msg.tid = pthread_self();

    if (num <= 0)
    {
        perror("[client] failed to read private fifo");
        exit(EXIT_FAILURE);
    }
    else
    {
        if (msg.tskres == -1)
            output(&msg, CLOSD);
        else
            output(&msg, GOTRS);
    }

    // close and remove private fifo
    if (close(thread_fifo) != 0)
    {
        perror("[client] failed to close private fifo");
        exit(EXIT_FAILURE);
    }

    if (unlink(thread_fifo_path) != 0)
    {
        perror("[client] failed to unlink private fifo");
        exit(EXIT_FAILURE);
    }

    free(gavup_msg);
    free(thread_fifo_path);

    return NULL;
}

int main(int argc, char **argv)
{
    char fifoname[256]; /* public fifo path */
    unsigned runtime;   /* max program running time */

    struct sigaction sighandler;
    sigset_t smask;

    // init mutex
    pthread_mutex_init(&mutex, NULL);

    // check and read command line arguments

    if (argc != 4 || strcmp(argv[1], "-t") != 0)
        print_usage();

    sscanf(argv[2], "%u", &runtime);
    sscanf(argv[3], "%s", fifoname);

    // end command line arguments section

    // set sig handlers
    if (sigemptyset(&smask) == -1)
        perror("[client] sigsetfunctions()");
    sighandler.sa_handler = sigpipe_handler;
    sighandler.sa_mask = smask;
    sighandler.sa_flags = 0;
    if (sigaction(SIGPIPE, &sighandler, NULL) == -1)
        perror("sigaction");

    if (sigemptyset(&smask) == -1)
        perror("[client] sigsetfunctions()");
    sighandler.sa_handler = sigalrm_handler;
    sighandler.sa_mask = smask;
    sighandler.sa_flags = 0;
    if (sigaction(SIGALRM, &sighandler, NULL) == -1)
        perror("sigaction");

    // set random seed
    srandom(time(NULL));

    // set timeout
    alarm(runtime);

    // open fifo, waits for server
    while ((fifo_file = open(fifoname, O_WRONLY)) < 0);

    // create threads
    while (1)
    {
        // create interval between thread creation
        struct timespec wait_time;
        wait_time.tv_sec = 0;
        wait_time.tv_nsec = 1000000 + ((random() % 25) * 1000000);
        nanosleep(&wait_time, NULL);

        // save thread number independent variable
        int *i_ptr = malloc(sizeof(int));
        *i_ptr = thread_no;

        pthread_create(&ids[thread_no], NULL, thread_rot, i_ptr);

        thread_no++;
    }

    return 0;
}
