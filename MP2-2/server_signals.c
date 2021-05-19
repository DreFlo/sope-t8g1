#include "server_signals.h"

void sigalrm_handler()
{
    unlink(fifoname);

    too_late = true;
}
