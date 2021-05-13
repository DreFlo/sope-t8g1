#include <fcntl.h>
#include <pthread.h>
#include <semaphore.h>
#include <signal.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdbool.h>

#include "common.h"
#include "lib.h"
#include "server_signals.h"
#include "server_utils.h"

unsigned int thread_no = 0;
char *fifoname;
pthread_t ids[1024];
unsigned buffer_length = 1;
ServerMessage *buffer;
sem_t semaphore;
extern int index_buffer;
pthread_t c_id;

void *worker_thread_rot(void *wmsg)
{
    WorkerMessage *wmsgtemp = (WorkerMessage *)wmsg;

    Message msg = wmsgtemp->msg;

    // Send to library

    msg.tskres = task(msg.tskload);

    ServerMessage smsg;
    smsg.s_pid = msg.pid;
    smsg.s_tid = msg.tid;

    msg.pid = getpid();
    msg.tid = pthread_self();
    msg.rid = wmsgtemp->i;
    smsg.msg = msg;

    enqueue(smsg);

    output(&msg, TSKEX);

    free(wmsg);

    return NULL;
} 

void *consumer_thread(void * arg)
{
    ServerMessage smsg;

    while(1) {
        dequeue(&smsg);

        int p_fifo;
        char private_fifoname[256];
        snprintf(private_fifoname, 256 * sizeof(char),"/tmp/%d.%lu", smsg.s_pid, smsg.s_tid);
        if ((p_fifo = open(private_fifoname, O_WRONLY)) < 0) {
            continue;
        }
        write(p_fifo, (void*) &smsg.msg, sizeof(Message));
        output(&smsg.msg, TSKDN);
    }
    
    return NULL;
}

int main(int argc, char **argv)
{

    // Check for arguments
    if (!(argc == 4 || argc == 6))
        perror("Number of arguments wrong!");

    unsigned runtime;

    sscanf(argv[2], "%u", &runtime);

    if (argc == 6)
    {
        sscanf(argv[4], "%u", &buffer_length);
        fifoname = argv[5];
    }
    else
        fifoname = argv[3];

    buffer = malloc(buffer_length * sizeof(ServerMessage));

    // Set signal handlers
    struct sigaction sighandler;
    sigset_t smask;

    if (sigemptyset(&smask) == -1)
        perror("[server] sigsetfunctions()");
    sighandler.sa_handler = sigalrm_handler;
    sighandler.sa_mask = smask;
    sighandler.sa_flags = 0;
    if (sigaction(SIGALRM, &sighandler, NULL) == -1)
        perror("sigaction");
    // SIGPIPE_HANDLER not working
    /*
    if (sigemptyset(&smask) == -1)
        perror("[server] sigsetfunctions()");
    sighandler.sa_handler = sigpipe_handler;
    sighandler.sa_mask = smask;
    sighandler.sa_flags = 0;
    if (sigaction(SIGALRM, &sighandler, NULL) == -1)
        perror("sigaction");
*/
    // Semaphore creation

    sem_init(&semaphore, 0, buffer_length);

    // Fifo Creation
    if (mkfifo(fifoname, ALLPERMS) < 0)
        perror("Could not make fifo!");

    int fd;

    if ((fd = open(fifoname, O_RDONLY)) < 0)
        perror("Could not open fifo!");

    // Read FIFO
    void *msg = malloc(sizeof(Message));

    alarm(runtime);

    pthread_create(&c_id, NULL, consumer_thread, NULL);

    while (1)
    {

        if (read(fd, msg, sizeof(Message)) <= 0) {
            perror("Could not read the message from FIFO!");
            break;
        }
        Message output_msg = *(Message *)msg;

        output_msg.rid = thread_no;
        output_msg.pid = getpid();
        output_msg.tid = pthread_self();

        output(&output_msg, RECVD);

        WorkerMessage *wmsg = malloc(sizeof(WorkerMessage));
        wmsg->i = thread_no;
        wmsg->msg = *(Message *)(msg);
        pthread_create(&ids[thread_no], NULL, worker_thread_rot, wmsg);

        thread_no++;
    }

    for (int i = 0; i < thread_no; i++) {
        pthread_join(ids[i], NULL);
    }

    while(!queue_empty());

    pthread_join(c_id, NULL);

    unlink(fifoname);

    return 1;
}