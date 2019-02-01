#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>
#include "mutex.h"
#include "thread.h"
#include "buffer.h"

int main(int argc, char const *argv[]){
  if (argc < 4){
    fprintf(stderr, "%s\n", "Need at least 3 arguments");
    return -1;
  }
  if (atoi(argv[1]) <= 0) {
    fprintf(stderr, "%s\n", "Sleep time must be positive numbers");
    return -1;
  }
  if (atoi(argv[2]) <= 0) {
    fprintf(stderr, "%s\n", "Producer thread numbers must be positive");
    return -1;
  }
  if (atoi(argv[3]) <= 0) {
    fprintf(stderr, "%s\n", "Consumer thread numbers must be positive");
    return -1;
  }

  pthread_mutex_init(&mutex, NULL);
  sem_init(&empty, 0, BUFFER_SIZE);
  sem_init(&full, 0, 0);
  int i;
  for (i = 0; i < atoi(argv[2]); ++i){
    pthread_t tid;
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_create(&tid, &attr, producer, NULL);
  }

  for (i = 0; i < atoi(argv[3]); ++i) {
    pthread_t tid;
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_create(&tid, &attr, consumer, NULL);
  }
  sleep(atoi(argv[1]));
  exit(0);

}
