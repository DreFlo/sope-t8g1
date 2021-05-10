#include "server_utils.h"

int index_buffer = 0;

void enqueue(ServerMessage s_msg) {
    if(sem_wait(&semaphore) < 0) perror("sem_wait() error");

    buffer[index_buffer] = s_msg;
    index_buffer++;

}


void dequeue(ServerMessage *s_msg) {
    int semaphoreval;
    do {
        sem_getvalue(&semaphore, &semaphoreval);
    } while( semaphoreval >= buffer_length );

    *s_msg = buffer[0];

    for(int i = 1; i < index_buffer; i++) {
        buffer[i-1] = buffer[i];
    }
    index_buffer--;

    if(sem_post(&semaphore) < 0) perror("sem_post() error");
}
