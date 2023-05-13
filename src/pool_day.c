#include "pool_day.h"

#include <pthread.h>
#include <semaphore.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

/**
 * @brief Main structure of the library, it defines a handle to the pool.
 */
struct pool_day {
  uint8_t size;        //!< Size of the pool.
  bool must_stop;      //!< Flag indicating wheter all threads must stop its
                       //   execution.
  sem_t semaphore;     //!< Pool's semaphore.
  pthread_t *threads;  //!< Threads whose makes part of the pool.
  task_queue_t *tasks; //!< Pool's tasks.
};

static void *thread_routine(void *param) {
  pool_day_t pool = (pool_day_t)param;

  while (!pool->must_stop) {
    sem_wait(&pool->semaphore);

    if (pool->must_stop) {
      break;
    }

    task_t *entry = dequeue(pool->tasks);
    if (entry) {
      entry->task(entry->param);
      free(entry);
    }
  }

  return NULL;
}

void enqueue_task(pool_day_t pool, task_t *task) {
  if (pool) {
    enqueue(pool->tasks, task);
    sem_post(&pool->semaphore);
  }
}

pool_day_t create_pool(uint8_t pool_size) {
  pool_day_t pool;

  pool = malloc(sizeof(*pool));
  if (!pool) {
    return NULL;
  }

  pool->size = pool_size;
  pool->must_stop = false;
  pool->threads = malloc(sizeof(pthread_t) * pool_size);

  if (!pool->threads) {
    free(pool);
    return NULL;
  }

  init_queue(&pool->tasks);
  sem_init(&pool->semaphore, 0, 0);

  for (uint8_t i = 0; i < pool_size; i++) {
    pthread_create(&pool->threads[i], NULL, thread_routine, pool);
  }

  return pool;
}

void destroy_pool(pool_day_t *pool) {
  if (!(*pool)) {
    return;
  }

  (*pool)->must_stop = true;
  for (uint8_t i = 0; i < (*pool)->size; i++) {
    sem_post(&(*pool)->semaphore);
  }

  for (uint8_t i = 0; i < (*pool)->size; i++) {
    pthread_join((*pool)->threads[i], NULL);
  }

  free((*pool)->threads);
  sem_destroy(&(*pool)->semaphore);
  destroy_queue((*pool)->tasks);

  free(*pool);
  *pool = NULL;
}

uint8_t idle_tasks(pool_day_t pool) {
  return pool ? queue_size(pool->tasks) : 0;
}

void abort_tasks(pool_day_t pool) {
  if (pool) {
    pool->must_stop = true;
  }
}
