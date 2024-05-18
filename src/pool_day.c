#include "pool_day.h"

#include <pthread.h>
#include <semaphore.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "core/logger.h"
#include "core/queue.h"
#include "core/utils.h"

/**
 * @brief Main structure of the library, it defines a handle to the pool.
 */
struct pool_day {
  uint32_t size;        //!< Size of the pool.
  bool must_stop;       //!< Flag indicating wheter all threads must
                        //   stop its execution.
  sem_t semaphore;      //!< Pool's semaphore.
  pthread_t *threads;   //!< Threads whose makes part of the pool.
  task_queue_t *tasks;  //!< Pool's queued tasks.
};

__static void *thread_func(void *param) {
  pool_day_t pool = (pool_day_t)param;

  while (!pool->must_stop) {
    sem_wait(&pool->semaphore);

    POOL_DAY_INFO("thread '0x%x' woke up", pthread_self());

    if (pool->must_stop) {
      POOL_DAY_INFO("thread '0x%x' aborting...", pthread_self());
      break;
    }

    task_t *entry = dequeue(pool->tasks);
    if (entry) {
      POOL_DAY_INFO("thread '0x%x' running the task", pthread_self());
      void *ret = entry->task(entry->param);
      POOL_DAY_INFO("thread '0x%x' finished the task", pthread_self());

      entry->ret_val = ret;
      sem_post(&entry->ready);
    }
  }

  POOL_DAY_INFO("thread '0%x' finishing...", pthread_self());

  return NULL;
}

pool_day_retcode_t enqueue_task(pool_day_t pool, task_t *task) {
  if (!pool || !task) {
    POOL_DAY_ERROR("null parameter");
    return POOL_DAY_ERROR_NULL_PARAM;
  }

  enqueue(pool->tasks, task);
  sem_post(&pool->semaphore);

  POOL_DAY_INFO("task enqueued with success");

  return POOL_DAY_SUCCESS;
}

pool_day_t create_pool(uint32_t pool_size) {
  pool_day_t pool;

  POOL_DAY_INFO("pool size: %u", pool_size);

  if (!pool_size) {
    POOL_DAY_ERROR("bad pool size");
    return NULL;
  }

  pool = calloc(1, sizeof(*pool));
  if (!pool) {
    POOL_DAY_ERROR("fail to allocate memory for a new pool");
    return NULL;
  }

  pool->size = pool_size;
  pool->must_stop = false;
  pool->threads = calloc(pool_size, sizeof(pthread_t));

  if (!pool->threads) {
    POOL_DAY_ERROR("fail to allocate memory for the pool threads");
    free(pool);
    return NULL;
  }

  init_queue(&pool->tasks);
  sem_init(&pool->semaphore, 0, 0);

  for (uint32_t i = 0; i < pool_size; i++) {
    pthread_create(&pool->threads[i], NULL, thread_func, pool);
    POOL_DAY_INFO("thread '%u' created", i);
  }

  POOL_DAY_INFO("pool created with success");

  return pool;
}

pool_day_retcode_t destroy_pool(pool_day_t *pool) {
  if (!pool || !(*pool)) {
    POOL_DAY_ERROR("null pool handle");
    return POOL_DAY_ERROR_NULL_PARAM;
  }

  POOL_DAY_INFO("waking up all sleeping threads");

  (*pool)->must_stop = true;
  for (uint32_t i = 0; i < (*pool)->size; i++) {
    POOL_DAY_INFO("waking up thread '%u'", i);
    sem_post(&(*pool)->semaphore);
  }

  POOL_DAY_INFO("joining all threads of the pool");

  for (uint32_t i = 0; i < (*pool)->size; i++) {
    POOL_DAY_INFO("finishing thread '%u'", i);
    pthread_join((*pool)->threads[i], NULL);
    POOL_DAY_INFO("thread '%u' finished", i);
  }

  free((*pool)->threads);
  sem_destroy(&(*pool)->semaphore);
  destroy_queue((*pool)->tasks);

  free(*pool);
  *pool = NULL;

  POOL_DAY_INFO("pool destroyed with success");

  return POOL_DAY_SUCCESS;
}

uint32_t tasks(pool_day_t pool) {
  return pool ? queue_size(pool->tasks) : 0;
}

pool_day_retcode_t abort_tasks(pool_day_t pool) {
  if (!pool) {
    POOL_DAY_ERROR("null pool handle");
    return POOL_DAY_ERROR_NULL_PARAM;
  }

  pool->must_stop = true;

  POOL_DAY_INFO("stopping all threads");

  return POOL_DAY_SUCCESS;
}

uint32_t queued_tasks(pool_day_t pool) {
  return pool ? queue_size(pool->tasks) : 0;
}

void *wait_task_finish(pool_day_t pool, task_t *task) {
  void *ret;

  if (!pool || !task) {
    POOL_DAY_ERROR("null pool handle or task");
    return NULL;
  }

  POOL_DAY_INFO("waiting for the finish of the task");
  sem_wait(&task->ready);
  POOL_DAY_INFO("task finished");

  ret = task->ret_val;
  free(task);

  return ret;
}
