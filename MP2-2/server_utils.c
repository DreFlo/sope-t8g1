#include "server_utils.h"

int front = -1;
int rear = -1;
int size = -1;
int semaphoreval;

void enqueue(ServerMessage s_msg) {
    if(sem_wait(&semaphore) < 0) perror("sem_wait() error");

    /*
        Initialize the queue
    */
    if(size < 0) {
        buffer[0] = s_msg;
        front = rear = 0;
        size = 1;
    }
    /*
        When reachs end
    */ 
    else if (rear == buffer_length-1) {
        buffer[0] = s_msg;
        rear = 0;
        size++;
    } 
    /*
        Normal add to queue
    */
    else {
        buffer[++rear] = s_msg;
        size++;
    }

}


ServerMessage* dequeue() {
    // wait while queue empty
    while(queue_empty());


    ServerMessage* s_msg = malloc(sizeof(ServerMessage));
    *s_msg = buffer[front++];
    size--;

    if(sem_post(&semaphore) < 0) perror("sem_post() error");

    return s_msg;
}

bool queue_empty() {
    return size < 0;
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
