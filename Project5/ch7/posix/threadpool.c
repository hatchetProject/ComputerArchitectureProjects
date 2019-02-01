/**
 * Implementation of thread pool.
 */

#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <semaphore.h>
#include "threadpool.h"

#define QUEUE_SIZE 10
#define NUMBER_OF_THREADS 3

#define TRUE 1

// this represents work that has to be
// completed by a thread in the pool
typedef struct
{
    void (*function)(void *p);
    void *argument;
} threadpool_task_t;

typedef enum{
  threadpool_invalid = -1,
  threadpool_lock_failure = -2,
  threadpool_queue_full = -3,
  threadpool_shutdown = -4,
  threadpool_thread_failure = -5
} threadpool_error_t;

typedef enum{
  threadpool_graceful = 1
} threadpool_destroy_flags_t;


typedef enum{
  immediate_shutdown = 1,
  graceful_shutdown = 2
} threadpool_shutdown_t;

typedef struct {
  pthread_mutex_t lock;
  pthread_cond_t notify;
  pthread_t *threads;
  threadpool_task_t *queue;
  int thread_count;
  int queue_size;
  int head;
  int tail;
  int count;
  int shutdown;
  int started;
} threadpool_t;

threadpool_t *pool;
threadpool_task_t task;
int threadpool_free();
// the worker thread in the thread pool
void *worker(void *param)
{
    // execute the task
    for (; ;){
      pthread_mutex_lock(&(pool->lock));
      while ((pool->count == 0) && (!pool->shutdown)) {
        pthread_cond_wait(&(pool->notify), &(pool->lock));
      }

      if ((pool->shutdown == immediate_shutdown) ||
          ((pool->shutdown == graceful_shutdown) &&
          (pool->count == 0))) {
            break;
          }
      task.function = pool->queue[pool->head].function;
      task.argument = pool->queue[pool->head].argument;

      pool->head += 1;
      pool->head = (pool->head == pool->queue_size) ? 0 : pool->head;
      pool->count -= 1;

      pthread_mutex_unlock(&(pool->lock));

      (*(task.function))(task.argument);
    }
    pool->started--;
    pthread_mutex_unlock(&(pool->lock));

    pthread_exit(0);
}

// initialize the thread pool
void pool_init(void)
{
    if ((pool = (threadpool_t *) malloc(sizeof(threadpool_t))) == NULL){
      return;
    }
    pool->thread_count = 0;

    pool->queue_size = QUEUE_SIZE;
    pool->head = pool->tail = pool->count = 0;
    pool->shutdown = pool->started = 0;

    pool->threads = (pthread_t *)malloc(sizeof(pthread_t) * NUMBER_OF_THREADS);
    pool->queue = (threadpool_task_t *)malloc(sizeof(threadpool_task_t) * QUEUE_SIZE);

    if ((pthread_mutex_init(&(pool->lock), NULL) != 0) ||
        (pthread_cond_init(&(pool->notify), NULL) != 0) ||
        (pool->threads == NULL) ||
        (pool->queue) == NULL){
          if (pool){
            threadpool_free(pool);
          }
        }
    int i;
    for (i = 0; i < NUMBER_OF_THREADS; ++i){
      if (pthread_create(&(pool->threads[i]), NULL, worker, (void*)pool) != 0){
        pool_shutdown();
      }
      pool->thread_count++;
      pool->started++;
    }
}


/**
 * Submits work to the pool.
 */
int pool_submit(void (*somefunction)(void *p), void *argument)
{
    int err = 0;
    int next;
    if (pool == NULL || somefunction == NULL){
      return threadpool_invalid;
    }

    if (pthread_mutex_lock(&(pool->lock)) != 0){
      return threadpool_lock_failure;
    }

    next = pool->tail + 1;
    next = (next == pool->queue_size) ? 0:next;
    do {
      if (pool->count == pool->queue_size){
        err = threadpool_queue_full;
        printf("%s\n", "threadpool_queue_full");
        break;
      }
      if (pool->shutdown){
        err = threadpool_shutdown;
        printf("%s\n", "threadpool_shutdown");
        break;
      }
      pool->queue[pool->tail].function = somefunction;
      pool->queue[pool->tail].argument = argument;
      pool->tail = next;
      pool->count += 1;

      if (pthread_cond_signal(&(pool->notify)) != 0){
        err = threadpool_lock_failure;
        break;
      }
    } while (0);

    if (pthread_mutex_unlock(&pool->lock) != 0){
      err = threadpool_lock_failure;
    }
    return err;
}

// inner function
int threadpool_free(){
  if (pool == NULL || pool->started > 0){
    return -1;
  }

  if (pool->threads){
    free(pool->threads);
    free(pool->queue);
    pthread_mutex_lock(&(pool->lock));
    pthread_mutex_destroy(&(pool->lock));
    pthread_cond_destroy(&(pool->notify));
  }
  free(pool);
  return 0;
}


// shutdown the thread pool
void pool_shutdown(void)
{
    int i, err = 0;
    if (pool == NULL){
      printf("%s\n", "threadpool_invalid");
      return;
    }
    if (pthread_mutex_lock(&(pool->lock)) != 0){
      printf("%s\n", "threadpool_lock_failure");
      return;
    }

    do {
      if (pool->shutdown){
        err = threadpool_shutdown;
        break;
      }
      pool->shutdown = (threadpool_graceful) ? graceful_shutdown : immediate_shutdown;
      if ((pthread_cond_broadcast(&(pool->notify)) != 0) ||
          (pthread_mutex_unlock(&(pool->lock)) != 0)){
            err = threadpool_lock_failure;
            break;
          }
      for (i = 0; i < pool->thread_count; i++) {
        if (pthread_join(pool->threads[i], NULL) != 0){
          err = threadpool_thread_failure;
          break;
        }
      }
    } while (0);
    if (!err) {
      threadpool_free(pool);
    }
}
