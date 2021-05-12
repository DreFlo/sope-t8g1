#include <fcntl.h>
#include <pthread.h>
#include <semaphore.h>
#include <signal.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

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
    //int i = wmsgtemp->i;

    // Send to library

    msg.tskres = task(msg.tskload);

    ServerMessage smsg;
    smsg.s_pid = getpid();
    smsg.s_tid = pthread_self();
    smsg.msg = msg;
    printf("BEF\n");

    enqueue(smsg);

    printf("AFTER\n");
    output(&msg, TSKEX);

    pthread_join(pthread_self(), NULL);

    return NULL;
} 

void *consumer_thread()
{
    ServerMessage *smsg = NULL;

    dequeue(smsg);

    if(smsg != NULL){
        int p_fifo;
        char fifoname[256];
        snprintf(fifoname, sizeof(fifoname),"/tmp/%d.%lu", smsg->msg.pid, smsg->msg.tid);

        if ((p_fifo = open(fifoname, O_WRONLY)) < 0)
            perror("Could not open fifo!");

        write(p_fifo, (void*) &smsg->msg, sizeof(smsg->msg));
        
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

    printf("ASDAS\n");

    // Set signal handlers
    struct sigaction sighandler;
    sigset_t smask;

    if (sigemptyset(&smask) == -1)
        perror("[client] sigsetfunctions()");
    sighandler.sa_handler = sigalrm_handler;
    sighandler.sa_mask = smask;
    sighandler.sa_flags = 0;
    if (sigaction(SIGALRM, &sighandler, NULL) == -1)
        perror("sigaction");

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

    while (index_buffer < buffer_length)
    {
        if (read(fd, msg, sizeof(Message)) < 0)
            perror("Could not read the message from FIFO!");


        WorkerMessage *wmsg = malloc(sizeof(WorkerMessage));
        wmsg->i = thread_no;
        wmsg->msg = *(Message *)(msg);
        pthread_create(&ids[thread_no], NULL, worker_thread_rot, wmsg);

        thread_no++;
    }

    return 1;
}