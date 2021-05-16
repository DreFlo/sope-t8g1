#include "server_signals.h"

void sigpipe_handler() {
    return;
}

void sigalrm_handler() {
    show = true;
    while (!queue_empty());
    
    unlink(fifoname);
    exit(EXIT_SUCCESS);
}
