#include "client_signals.h"

void sigpipe_handler()
{
    fprintf(stderr, "[client] server closed pipe\n");

    for (unsigned int i = 0; i < thread_no; i++)
    {
        pthread_kill(ids[i], SIGTERM);
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

    for (unsigned int i = 0; i < thread_no; i++)
    {
        fprintf(stderr, "%u\n", i);
        pthread_cancel(ids[i]);
    }
    
    exit(EXIT_SUCCESS);
}
