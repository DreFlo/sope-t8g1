#include "client_signals.h"

void sigpipe_handler()
{
    fprintf(stderr, "[client] server closed pipe\n");
    for (unsigned int i = 0; i < thread_no; i++)
    {
        pthread_cancel(ids[i]);
    }
    for (unsigned int i = 0; i < thread_no; i++)
    {
        pthread_join(ids[i], NULL);
    }
    exit(EXIT_SUCCESS);
}

void sigalrm_handler()
{
    fprintf(stderr, "[client] client timeout\n");
    exit(EXIT_FAILURE);
}
