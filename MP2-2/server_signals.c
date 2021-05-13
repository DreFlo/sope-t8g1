#include "server_signals.h"

void sigpipe_handler() {
    return;
}

void sigalrm_handler() {
    unlink(fifoname);
    exit(EXIT_SUCCESS);
}
