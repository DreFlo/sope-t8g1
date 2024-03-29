#include "client_utils.h"

void print_usage()
{
    printf("Usage: c <-t nsecs> fifoname\n\n");
    printf("nsecs - number of seconds (approx.) the program will run\n");
    printf("fifoname - name (absolute or relative) of the public channel through which the Client sends requests to the Server\n\n");
    exit(EXIT_SUCCESS);
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
        perror("[client] snprintf failed in output");
        exit(EXIT_FAILURE);
    }

    printf("%s", output);
}

void thread_gavup(void *arg)
{
    Cancelation c = *(Cancelation *) arg;

    // print GAVUP message
    output(&c.msg, GAVUP);

    // remove private fifo
    if (unlink(c.fifopath) != 0)
    {
        perror("[client] failed to unlink private fifo");
        exit(EXIT_FAILURE);
    }

    free(c.fifopath);
    free(arg);
}
