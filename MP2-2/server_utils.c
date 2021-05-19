#include "server_utils.h"

int index_buffer = -1;
int semaphoreval;

bool enqueue(ServerMessage s_msg)
{
    if (index_buffer >= buffer_length - 1) 
        return false;
    else {
        buffer[++index_buffer] = s_msg;
        return true;
    }
}

ServerMessage * dequeue()
{
    if (queue_empty()) return NULL;

    ServerMessage * ret = malloc(sizeof(ServerMessage));
    *ret = buffer[0];

    for (int i = 1; i <= index_buffer; i++)
    {
        buffer[i - 1] = buffer[i];
    }

    index_buffer--;

    return ret;
}

bool queue_empty()
{
    static unsigned t = 0;
    if (too_late) {
        t++;
    }
    if (t > 10000) return true;
    return index_buffer < 0;
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

    write(STDOUT_FILENO, output, strlen(output));
}

void consumer_cleanup(void * arg) {
    int latest = *(int *)arg;
    write(STDOUT_FILENO, "Here\n", strlen("Here\n"));
    Message msg = {latest, getpid(), pthread_self(), 0, -1};
    for (int i = latest + 1; i < thread_no; i++) {
        msg.rid = i;
        output(&msg, FAILD);
    }
    free(arg);
}
