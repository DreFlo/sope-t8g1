#include "server_signals.h"

void sigalrm_handler()
{
    unlink(fifoname);

    too_late = true;
    over = true;

    while (!queue_empty())
        ;

    close(fd);

    exit(EXIT_SUCCESS);
}
