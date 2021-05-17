#include "server_utils.h"

int index_buffer = 0;
int semaphoreval;

void enqueue(ServerMessage s_msg) {
    if(sem_wait(&semaphore) < 0) perror("sem_wait() error");

    buffer[index_buffer] = s_msg;
    index_buffer++;

}


void dequeue(ServerMessage *s_msg) {
    // wait while queue empty
    do {
        sem_getvalue(&semaphore, &semaphoreval);
    } while( semaphoreval >= buffer_length );

    *s_msg = buffer[0];

    for(int i = 1; i < index_buffer; i++) {
        buffer[i-1] = buffer[i];
    }

    index_buffer--;

    if(sem_post(&semaphore) < 0) perror("sem_post() error");
}

bool queue_empty() {
    /*
    static unsigned use_no = 0;
    use_no++;
    if (use_no > 100000) return true;
    */
    return semaphoreval >= buffer_length;
}

void output(Message *msg, Operation op)
{
    char op_string[MAX_OP_LEN];

    switch (op)
    {
    case IWANT:
        snprintf(op_string, MAX_OP_LEN, "IWANT");
        break;
    case RECVD:
        snprintf(op_string, MAX_OP_LEN, "RECVD");
        break;
    case TSKEX:
        snprintf(op_string, MAX_OP_LEN, "TSKEX");
        break;
    case TSKDN:
        snprintf(op_string, MAX_OP_LEN, "TSKDN");
        break;
    case GOTRS:
        snprintf(op_string, MAX_OP_LEN, "GOTRS");
        break;
    case LATE:
        snprintf(op_string, MAX_OP_LEN, "2LATE");
        break;
    case CLOSD:
        snprintf(op_string, MAX_OP_LEN, "CLOSD");
        break;
    case GAVUP:
        snprintf(op_string, MAX_OP_LEN, "GAVUP");
        break;
    case FAILD:
        snprintf(op_string, MAX_OP_LEN, "FAILD");
        break;
    };

    char output[200];
    if (snprintf(output, sizeof(output), "%ld; %d; %d; %d; %lu; %d; %s\n", time(NULL), msg->rid, msg->tskload, msg->pid, msg->tid, msg->tskres, op_string) < 0)
    {
        free(msg);
        perror("[server] snprintf failed in output");
        exit(EXIT_FAILURE);
    }

    printf("%s", output);
}
