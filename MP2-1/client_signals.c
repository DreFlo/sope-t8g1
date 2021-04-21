#include "client_signals.h"

void sigpipe_handler() {
    fprintf(stderr, "[client] server closed pipe\n");
    for (unsigned int i = 0; i < thread_no; i++) {
        pthread_cancel(ids[i]);
    }
    exit(EXIT_FAILURE);
}

void sigalrm_handler() {
    fprintf(stderr, "[client] client timeout\n");
    exit(EXIT_FAILURE);
}