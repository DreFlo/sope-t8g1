#include "client_signals.h"

void sigpipe_handler() {
    printf("pipe closed\n\n");
    exit(EXIT_FAILURE);
}

void sigalrm_handler() {
    return;
}