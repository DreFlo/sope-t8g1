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
ServerMessage * buffer;
sem_t semaphore;

void * worker_thread_rot(void* wmsg) {
    WorkerMessage *wmsgtemp = (WorkerMessage*)wmsg;

    Message msg = wmsgtemp->msg;
    int i = wmsgtemp->i;

    // Send to library

    msg.tskres = task(msg.tskload);
    

}


int main(int argc, char ** argv) {

    // Check for arguments
    if(!(argc == 4 || argc == 6))
        perror("Number of arguments wrong!");

    unsigned runtime;

    sscanf(argv[2], "%u", &runtime);

    if(argc == 6) {
        sscanf(argv[4], "%u", &buffer_length);
        fifoname = argv[5];
    }
    else fifoname = argv[3];


    buffer = malloc(buffer_length*sizeof(ServerMessage));

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
    if(mkfifo(fifoname, ALLPERMS) < 0)
        perror("Could not make fifo!");
    
    int fd;

    if((fd = open(fifoname, O_RDONLY)) < 0)
        perror("Could not open fifo!");

    
    // Read FIFO
    void * msg = malloc(sizeof(Message));

    alarm(runtime);

    while(1){
        if(read(fd, msg, sizeof(Message)) < 0) perror("Could not read the message from FIFO!");


        WorkerMessage * wmsg = malloc(sizeof(WorkerMessage));
        wmsg->i = thread_no;
        wmsg->msg = *(Message*)(msg);
        pthread_create(&ids[thread_no], NULL, worker_thread_rot, wmsg);

        thread_no++;
    }
    
    return 1;
}