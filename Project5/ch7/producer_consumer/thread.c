#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>
#include "mutex.h"
#include "thread.h"
#include "buffer.h"

void *producer(void *param){
  buffer_item item;
  srand(time(NULL));
  while (1){
    sleep(rand() % 3);
    item = rand() % 100;
    sem_wait(&empty);
    pthread_mutex_lock(&mutex);

    if (insert_item(item)){
      printf("%s\n", "Producer insert item error");
      pthread_mutex_unlock(&mutex);
      sem_post(&empty);
      continue;
    }
    printf("%lu producer insert item %2d success. \n Current buffer is:\n", pthread_self(), item);
    printBuffer();

    pthread_mutex_unlock(&mutex);
    sem_post(&full);
  }
}

void *consumer(void *param){
  buffer_item item;
  while (1){
    sleep(rand() % 3);
    sem_wait(&full);
    pthread_mutex_lock(&mutex);

    if (remove_item(&item)){
      printf("%s\n", "Consumer remove item error");
      pthread_mutex_unlock(&mutex);
      sem_post(&full);
      continue;
    }
    printf("%lu consumer remove item %2d success. Current buffer is:\n", pthread_self(), item);
    printBuffer();

    pthread_mutex_unlock(&mutex);
    sem_post(&empty);
  }
}
