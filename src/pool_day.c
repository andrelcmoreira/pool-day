#include "pool_day.h"

#include <pthread.h>
#include <semaphore.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "core/logger.h"

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

    POOL_DAY_LOG("thread '0x%x' woke up\n", pthread_self());

    if (pool->must_stop) {
      POOL_DAY_LOG("thread '0x%x' aborting...\n", pthread_self());
      break;
    }

    task_t *entry = dequeue(pool->tasks);
    if (entry) {
      POOL_DAY_LOG("thread '0x%x' running task\n", pthread_self());
      entry->task(entry->param);
      POOL_DAY_LOG("thread '0x%x' finished the task\n", pthread_self());
      free(entry);
    }
  }

  POOL_DAY_LOG("thread '0%x' finishing...\n", pthread_self());

  return NULL;
}

pool_day_retcode_t enqueue_task(pool_day_t pool, task_t *task) {
  if (!pool || !task) {
    POOL_DAY_ERROR("null parameter\n");
    return POOL_DAY_ERROR_NULL_PARAM;
  }

  enqueue(pool->tasks, task);
  sem_post(&pool->semaphore);

  POOL_DAY_LOG("task enqueued with success\n");

  return POOL_DAY_SUCCESS;
}

pool_day_t create_pool(uint8_t pool_size) {
  pool_day_t pool;

  POOL_DAY_LOG("pool size: %u\n", pool_size);

  pool = malloc(sizeof(*pool));
  if (!pool) {
    POOL_DAY_ERROR("fail to allocate memory for a new pool\n");
    return NULL;
  }

  pool->size = pool_size;
  pool->must_stop = false;
  pool->threads = malloc(sizeof(pthread_t) * pool_size);

  if (!pool->threads) {
    POOL_DAY_ERROR("fail to allocate memory for the pool threads\n");
    free(pool);
    return NULL;
  }

  init_queue(&pool->tasks);
  sem_init(&pool->semaphore, 0, 0);

  for (uint8_t i = 0; i < pool_size; i++) {
    pthread_create(&pool->threads[i], NULL, thread_routine, pool);
    POOL_DAY_LOG("thread '%u' created\n", i);
  }

  POOL_DAY_LOG("pool create with success\n");

  return pool;
}

pool_day_retcode_t destroy_pool(pool_day_t *pool) {
  if (!pool || !(*pool)) {
    POOL_DAY_ERROR("null pool handle\n");
    return POOL_DAY_ERROR_NULL_PARAM;
  }

  POOL_DAY_LOG("waking up all sleeping threads\n");

  (*pool)->must_stop = true;
  for (uint8_t i = 0; i < (*pool)->size; i++) {
    sem_post(&(*pool)->semaphore);
  }

  POOL_DAY_LOG("joining all threads of the pool\n");

  for (uint8_t i = 0; i < (*pool)->size; i++) {
    pthread_join((*pool)->threads[i], NULL);
  }

  free((*pool)->threads);
  sem_destroy(&(*pool)->semaphore);
  destroy_queue((*pool)->tasks);

  free(*pool);
  *pool = NULL;

  POOL_DAY_LOG("pool destroyed with success\n");

  return POOL_DAY_SUCCESS;
}

uint8_t idle_tasks(pool_day_t pool) {
  return pool ? queue_size(pool->tasks) : 0;
}

pool_day_retcode_t abort_tasks(pool_day_t pool) {
  if (!pool) {
    POOL_DAY_ERROR("null pool handle\n");
    return POOL_DAY_ERROR_NULL_PARAM;
  }

  pool->must_stop = true;

  POOL_DAY_LOG("stopping all threads\n");

  return POOL_DAY_SUCCESS;
}
