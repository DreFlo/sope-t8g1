#include "client_signals.h"

void sigpipe_handler() {
    exit(EXIT_FAILURE);
}

void sigalrm_handler() {
    return;
}