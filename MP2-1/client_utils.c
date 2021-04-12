#include "client_utils.h"

void read_msg_from_server(int thread_fd, struct message * msg) {
    char str[1024];

    // read server response
    read(thread_fd, str, 1024);

    // store server response
    sscanf(str, "%d %d %lu %d %d", &msg->rid, &msg->pid, &msg->tid, &msg->tskload, &msg->tskres);
}