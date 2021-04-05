#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <pthread.h>
#include <time.h>
#include <unistd.h>

// Placeholder
unsigned int runtime = 0;
// Placeholder

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
 * @brief Worker thread function
 * @param arg Pointer to a request number
 * @return void* 
 */
void *rot(void *arg) {
    // store request number locally and free arg pointer
    int i = * (int *) arg;
    free(arg);

    // 
    char request_str[1024];
    char thread_fifo_path[256];
    int thread_fifo;

    snprintf(request_str, 1024, "%d %ld %d %lu -1", i, random() % 9 + 1, getpid(), pthread_self());
    snprintf(thread_fifo_path, 256, "/tmp/%d.%lu", getpid(), pthread_self());

    if (mkfifo(thread_fifo_path, ALLPERMS) != 0) {
        perror("mkfifo thread");
    }

    if ((thread_fifo = open(thread_fifo_path, O_RDONLY)) != 0) {
        perror("open thread fifo");
    }

    // critical writing region

    pthread_mutex_lock(&mutex);

    write(fifo_file, request_str, 1024);

    pthread_mutex_unlock(&mutex);

    printf("%ld ; %d ; pid ; tid : res ; IWANT\n", time(NULL), i);

    // end critical writing region

    if (close(thread_fifo) != 0) {
        perror("close thread fifo");
    }

    if (unlink(thread_fifo_path) != 0) {
        perror("unlink thread fifo");
    }

    return NULL;
}

int main(int argc, char ** argv, char ** envp) {
    char fifoname[256];

    // save program start time
    time_t start_time = time(NULL);

    // init mutex
    pthread_mutex_init(&mutex, NULL);

    // check and read command line arguments

    if (argc != 4 || (argc == 4 && strcmp(argv[1], "-t") != 0)) print_usage();

    sscanf(argv[2], "%u", &runtime);
    sscanf(argv[3], "%s", fifoname);

    // end command line arguments section

    srandom(time(NULL));

    mkfifo(fifoname, ALLPERMS);

    if ((fifo_file = open(fifoname, O_WRONLY)) == -1) {
        perror("open main fifo");
    }

    // create threads
    while (time(NULL) < start_time + runtime) {
        // create interval between thread creation
        struct timespec wait_time;
        wait_time.tv_sec = 0;
        wait_time.tv_nsec = 1000000 + ((random() % 25) * 1000000);
        nanosleep(&wait_time, NULL);

        //
        int * i_ptr = malloc(sizeof(int));
        *i_ptr = thread_no;

        pthread_create(&ids[thread_no], NULL, rot, i_ptr);

        thread_no++;
    }

    // ensure all threads are done
    for (int i = 0; i < thread_no; i++) {
        pthread_join(ids[i], NULL);
    }

    if (close(fifo_file) == -1) {
        perror("close main fifo");
    }

    pthread_mutex_destroy(&mutex);

    return 0;
}