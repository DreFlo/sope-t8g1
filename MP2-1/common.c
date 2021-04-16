#include "common.h"

void output(Message *msg, Operation op){
    char op_string[MAX_OP_LEN];

    switch(op){
        case IWANT:
            sprintf(op_string, "IWANT");
            break;
        case RECVD:
            sprintf(op_string, "RECVD");
            break;
        case TSKEX:
            sprintf(op_string, "TSKEX");
            break;
        case TSKDN:
            sprintf(op_string, "TSKDN");
            break;
        case GOTRS:
            sprintf(op_string, "GOTRS");
            break;
        case LATE:
            sprintf(op_string, "2LATE");
            break;
        case CLOSD:
            sprintf(op_string, "CLOSD");
            break;
        case GAVUP:
            sprintf(op_string, "GAVUP");
            break;
        case FAILD:
            sprintf(op_string, "FAILD");
            break;
    };

    char output[200];
    if (sprintf(output, "%ld ; %d ; %d ; %d : %lu ; %d ; %s\n", time(NULL), msg->i, msg->t, msg->pid, msg->tid, msg->res, op_string) < 0){
        free(msg);
        perror("[client] sprintf failed in output");
        exit(EXIT_FAILURE);
    }

    printf("%s", output);
}