#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <pthread.h>
#include <time.h>
#include <unistd.h>
#include "client_utils.h"

// IMPORTANT
// SERVER CLIENT MESSAGES FORMAT
// i pid tid t res

unsigned int thread_no = 0;
int fifo_file;

pthread_t ids[1024];
pthread_mutex_t mutex;

/**
 * @brief Prints the format of the command line arguments of the program and exits. 
 */
void print_usage() {
    printf("Usage: c <-t nsecs> fifoname\n\n");
    printf("nsecs - number of seconds (approx.) the program will run\n");
    printf("fifoname - name (absolute or relative) of the public channel through which the Client sends requests to the Server\n\n");
    exit(EXIT_SUCCESS);
}

/**
 * @brief Worker thread function. Handles all an individual thread does.
 * @param arg Pointer to a request number
 * @return void* 
 */
void *thread_rot(void *arg) {
    // store request number locally and free arg pointer
    int i = * (int *) arg;
    free(arg);

    char str[1024];
    char thread_fifo_path[256];
    int thread_fifo;
    int t = random() % 9 + 1;

    struct message msg;    

    // format strings
    snprintf(str, 1024, "%d %d %lu %d -1", i, getpid(), pthread_self(), t);
    snprintf(thread_fifo_path, 256, "/tmp/%d.%lu", getpid(), pthread_self());

    if (mkfifo(thread_fifo_path, ALLPERMS) != 0) {
        perror("mkfifo thread");
    }

    printf("\n\n\n\n%s\n\n\n\n", str);

    // write info to public fifo
    // begin critical writing region

    pthread_mutex_lock(&mutex);

    write(fifo_file, str, 1024);

    pthread_mutex_unlock(&mutex);

    // end critical writing region

    // print request sent message
    printf("%ld ; %d ; %d ; %d ; %lu : -1 ; IWANT\n", time(NULL), i, t, getpid(), pthread_self());

    // open private fifo, waits for server
    while ((thread_fifo = open(thread_fifo_path, O_RDONLY)) < 0);

    // read server response
    read_msg_from_server(thread_fifo, &msg);

    printf("%ld ; %d ; %d ; %d : %lu ; %d ; GOTRS\n", time(NULL), msg.rid, msg.tskload, msg.pid, msg.tid, msg.tskres);

    // close and remove private fifo
    if (close(thread_fifo) != 0) {
        perror("close thread fifo");
    }

    if (unlink(thread_fifo_path) != 0) {
        perror("unlink thread fifo");
    }

    return NULL;
}

int main(int argc, char ** argv) {
    char fifoname[256];
    unsigned runtime;

    // save program start time
    time_t start_time = time(NULL);

    // init mutex
    pthread_mutex_init(&mutex, NULL);

    // check and read command line arguments

    if (argc != 4 || strcmp(argv[1], "-t") != 0) print_usage();

    sscanf(argv[2], "%u", &runtime);
    sscanf(argv[3], "%s", fifoname);

    // end command line arguments section

    // set random seed
    srandom(time(NULL));

    mkfifo(fifoname, ALLPERMS);

    // open fifo, waits for server
    while ((fifo_file = open(fifoname, O_WRONLY)) < 0);

    // create threads
    while (time(NULL) < start_time + runtime) {
        // create interval between thread creation
        struct timespec wait_time;
        wait_time.tv_sec = 0;
        wait_time.tv_nsec = 1000000 + ((random() % 25) * 1000000);
        nanosleep(&wait_time, NULL);

        // save thread number independent variable
        int * i_ptr = malloc(sizeof(int));
        *i_ptr = thread_no;

        pthread_create(&ids[thread_no], NULL, thread_rot, i_ptr);

        thread_no++;
    }

    // ensure all threads are done
    for (unsigned int i = 0; i < thread_no; i++) {
        pthread_join(ids[i], NULL);
    }

    if (close(fifo_file) == -1) {
        perror("close main fifo");
    }

    pthread_mutex_destroy(&mutex);

    return 0;
}