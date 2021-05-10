#include "server_signals.h"



void sigalrm_handler() {
    unlink(fifoname);
    exit(EXIT_SUCCESS);
}
