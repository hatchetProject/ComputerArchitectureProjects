#include <pthread.h>
#include <semaphore.h>

pthread_mutex_t mutex;
sem_t empty, full;
