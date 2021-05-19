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
int fd;
pthread_t ids[1024];
int buffer_length = 1;
ServerMessage *buffer;
extern int index_buffer;
pthread_t c_id;
pthread_mutex_t mutex;

bool too_late = false;

void *worker_thread_rot(void *wmsg)
{
    // block SIGPIPE and SIGALRM to ensure only main thread handles them
    sigset_t set;
    sigemptyset(&set);
    sigaddset(&set, SIGPIPE);
    sigaddset(&set, SIGALRM);
    pthread_sigmask(SIG_BLOCK, &set, NULL);

    WorkerMessage *wmsgtemp = (WorkerMessage *)wmsg;

    Message msg = wmsgtemp->msg;

    // Send to library

    msg.tskres = too_late ? -1 : task(msg.tskload);

    ServerMessage smsg;
    smsg.s_pid = msg.pid;
    smsg.s_tid = msg.tid;

    msg.pid = getpid();
    msg.tid = pthread_self();
    msg.rid = wmsgtemp->i;
    smsg.msg = msg;

    bool ans;

    do {
        pthread_mutex_lock(&mutex);

        ans = enqueue(smsg);

        pthread_mutex_unlock(&mutex);

        usleep(5);
    } while (!ans);


    if (!too_late)
        output(&msg, TSKEX);

    free(wmsg);

    return NULL;
}

void *consumer_thread(void *arg)
{
    // block SIGPIPE and SIGALRM to ensure only main thread handles them
    sigset_t set;
    sigemptyset(&set);
    sigaddset(&set, SIGPIPE);
    sigaddset(&set, SIGALRM);
    pthread_sigmask(SIG_BLOCK, &set, NULL);

    ServerMessage * smsg;

    while (1)
    {
        do {
            pthread_mutex_lock(&mutex);

            smsg = dequeue();

            pthread_mutex_unlock(&mutex);

            usleep(5);
        } while (smsg == NULL);

        smsg->msg.tid = pthread_self();

        int p_fifo;
        char private_fifoname[256];
        snprintf(private_fifoname, sizeof(private_fifoname), "/tmp/%d.%lu", smsg->s_pid, smsg->s_tid);

        if ((p_fifo = open(private_fifoname, O_WRONLY)) < 0)
        {
            output(&smsg->msg, FAILD);
            continue;
        }

        if (too_late) smsg->msg.tskres = -1;

        int written_bytes = write(p_fifo, (void *)&smsg->msg, sizeof(Message));

        if (written_bytes == sizeof(Message) && smsg->msg.tskres != -1)
        {
            output(&smsg->msg, TSKDN);
        }
        else if (written_bytes == sizeof(Message) && smsg->msg.tskres == -1)
        {
            output(&smsg->msg, LATE);
        }
        else
        {
            output(&smsg->msg, FAILD);
        }

        free(smsg);
    }

    return NULL;
}

int main(int argc, char **argv)
{
    sigset_t set;
    sigemptyset(&set);
    sigaddset(&set, SIGPIPE);
    sigprocmask(SIG_BLOCK, &set, NULL);

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

    // Fifo Creation
    if (mkfifo(fifoname, ALLPERMS) < 0)
        perror("Could not make fifo!");

    if ((fd = open(fifoname, O_RDONLY)) < 0)
        perror("Could not open fifo!");

    // Read FIFO
    void *msg = malloc(sizeof(Message));

    alarm(runtime);

    pthread_create(&c_id, NULL, consumer_thread, NULL);

    while (1)
    {
        if (read(fd, msg, sizeof(Message)) <= 0)
        {
            continue;
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

    exit(EXIT_SUCCESS);
}