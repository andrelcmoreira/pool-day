#include "pool_day.h"

#include <pthread.h>
#include <semaphore.h>
#include <stdbool.h>
#include <stdlib.h>

#include "internal/logger.h"
#include "internal/queue.h"
#include "internal/task_def.h"
#include "internal/utils.h"
#include "task.h"

/**
 * @brief Main structure of the library, it defines a handle to the pool.
 */
struct pool_day {
  uint32_t size;        //!< Size of the pool.
  bool must_stop;       //!< Flag indicating wheter all threads must stop its execution.
  sem_t lock;           //!< Pool's semaphore.
  pthread_t *threads;   //!< Threads whose makes part of the pool.
  task_queue_t *tasks;  //!< Pool's queued tasks.
};

__static void *thread_func(void *param) {
  pool_day_t pool = (pool_day_t)param;

  while (!pool->must_stop) {
    sem_wait(&pool->lock);

    POOL_DAY_DEBUG("thread '0x%x' woke up", pthread_self());

    if (pool->must_stop) {
      POOL_DAY_DEBUG("thread '0x%x' aborting...", pthread_self());
      break;
    }

    task_t entry = dequeue(pool->tasks);
    if (entry) {
      entry->is_orphan = true;

      if (entry->on_task_start) {
        entry->on_task_start(entry->id, entry->param);
      }

      POOL_DAY_DEBUG("thread '0x%x' running the task '0x%x'", pthread_self(),
                     entry->id);
      void *ret = entry->task(entry->param);
      POOL_DAY_DEBUG("thread '0x%x' finished the task '0x%x'", pthread_self(),
                     entry->id);

      entry->ret_val = ret;
      if (entry->on_task_end) {
        POOL_DAY_DEBUG("thread '0x%x' executing end callback for task '0x%x'",
                       pthread_self(), entry->id);
        entry->on_task_end(entry->id, entry->param, entry->ret_val);
      }

      sem_post(&entry->ready);
    }
  }

  POOL_DAY_DEBUG("thread '0%x' finishing...", pthread_self());

  return NULL;
}

// cppcheck-suppress unusedFunction
pool_day_retcode_t enqueue_task(pool_day_t pool, task_t task) {
  if (!pool || !task) {
    POOL_DAY_ERROR("null parameter supplied");
    return POOL_DAY_ERROR_NULL_PARAM;
  }

  task->ret_val = NULL;
  task->is_orphan = false;

  enqueue(pool->tasks, task);
  sem_post(&pool->lock);

  POOL_DAY_DEBUG("task enqueued with success");

  return POOL_DAY_SUCCESS;
}

// cppcheck-suppress unusedFunction
pool_day_t create_pool(uint32_t pool_size) {
  pool_day_t pool;

  POOL_DAY_DEBUG("pool size: %u", pool_size);

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
  sem_init(&pool->lock, 0, 0);

  for (uint32_t i = 0; i < pool_size; i++) {
    pthread_create(&pool->threads[i], NULL, thread_func, pool);
    POOL_DAY_DEBUG("thread '%u' created", i);
  }

  POOL_DAY_DEBUG("pool created with success");

  return pool;
}

// cppcheck-suppress unusedFunction
pool_day_retcode_t destroy_pool(pool_day_t *pool) {
  if (!pool || !(*pool)) {
    POOL_DAY_ERROR("null pool handle");
    return POOL_DAY_ERROR_NULL_PARAM;
  }

  POOL_DAY_DEBUG("waking up all sleeping threads");

  (*pool)->must_stop = true;
  for (uint32_t i = 0; i < (*pool)->size; i++) {
    POOL_DAY_DEBUG("waking up thread '%u'", i);
    sem_post(&(*pool)->lock);
  }

  POOL_DAY_DEBUG("joining all threads of the pool");

  for (uint32_t i = 0; i < (*pool)->size; i++) {
    POOL_DAY_DEBUG("finishing thread '%u'", i);
    pthread_join((*pool)->threads[i], NULL);
    POOL_DAY_DEBUG("thread '%u' finished", i);
  }

  free((*pool)->threads);
  sem_destroy(&(*pool)->lock);
  destroy_queue((*pool)->tasks);

  free(*pool);
  *pool = NULL;

  POOL_DAY_DEBUG("pool destroyed with success");

  return POOL_DAY_SUCCESS;
}

// cppcheck-suppress unusedFunction
pool_day_retcode_t abort_tasks(pool_day_t pool) {
  if (!pool) {
    POOL_DAY_ERROR("null pool handle");
    return POOL_DAY_ERROR_NULL_PARAM;
  }

  pool->must_stop = true;

  POOL_DAY_DEBUG("stopping all threads");

  return POOL_DAY_SUCCESS;
}

// cppcheck-suppress unusedFunction
uint32_t queued_tasks(pool_day_t pool) {
  return pool ? queue_size(pool->tasks) : 0;
}

// cppcheck-suppress unusedFunction
void *get_task_result(task_t task) {
  if (!task) {
    POOL_DAY_ERROR("null task provided");
    return NULL;
  }

  POOL_DAY_DEBUG("waiting for the finish of the task");
  sem_wait(&task->ready);
  POOL_DAY_DEBUG("task finished");

  return task->ret_val;
}

void wait_task_finish(task_t task) {
  (void)get_task_result(task);
}
