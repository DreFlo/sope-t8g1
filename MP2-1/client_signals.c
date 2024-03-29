#include "client_signals.h"

void sigpipe_handler()
{
    fprintf(stdout, "[client] server closed pipe\n");

    for (unsigned int i = 0; i < thread_no; i++)
    {
        pthread_kill(ids[i], SIGTERM);
    }

    for (unsigned int i = 0; i < thread_no; i++)
    {
        pthread_join(ids[i], NULL);
    }

    if (close(fifo_file) == -1)
    {
        perror("[client] failed to close main fifo");
        exit(EXIT_FAILURE);
    }

    pthread_mutex_destroy(&mutex);

    exit(EXIT_SUCCESS);
}

void sigalrm_handler()
{
    fprintf(stdout, "[client] client timeout\n");

    for (unsigned int i = 0; i < thread_no; i++)
    {
        pthread_cancel(ids[i]);
    }

    for (unsigned int i = 0; i < thread_no; i++) 
    {
        pthread_join(ids[i], NULL);
    }

    if (close(fifo_file) == -1)
    {
        perror("[client] failed to close main fifo");
        exit(EXIT_FAILURE);
    }

    pthread_mutex_destroy(&mutex);

    exit(EXIT_SUCCESS);
}
