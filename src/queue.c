#include <pthread.h>
#include <stdlib.h>

#include "internal/queue.h"
#include "internal/task_def.h"
#include "internal/utils.h"
#include "task.h"

/**
 * @brief For-each macro implementation.
 *
 * @details It can be used to iterate over the queue.
 */
#define for_each_task(curr, queue) \
  for (struct task *curr = queue->head; curr; curr = curr->prev)

/**
 * @brief Safe implementation of for-each macro.
 *
 * @details It can be used to delete elements of the queue while iterating over
 * it.
 */
#define for_each_task_safe(curr, queue) \
  for (struct task *curr = queue->head, *tmp = curr ? curr->prev : NULL; \
    curr; \
    curr = tmp, tmp = tmp ? tmp->prev : NULL)

/**
 * @brief Task queue definition.
 */
struct task_queue {
  task_t tail;            //!< Tail of the queue.
  task_t head;            //!< Head of the queue.
  pthread_mutex_t mutex;  //!< Mutex of the queue.
};

uint32_t queue_size(task_queue_t *queue) {
  uint32_t size = 0;

  if (queue) {
    THREAD_SAFE_ZONE(&queue->mutex, {
      for_each_task(curr, queue) {
        size++;
      }
    })
  }

  return size;
}

void enqueue(task_queue_t *queue, task_t elem) {
  if (queue && elem) {
    THREAD_SAFE_ZONE(&queue->mutex, {
      // if the queue is empty
      if (!queue->head && !queue->tail) {
        queue->tail = queue->head = elem;
      } else {
        queue->tail->prev = elem;
        elem->next = queue->tail;
        queue->tail = elem;
      }
    })
  }
}

static task_t __dequeue(task_queue_t *queue) {
  task_t to_del = queue->head;

  // if the queue is not empty
  if (to_del) {
    queue->head = to_del->prev;
    // if the queue has more than one element
    if (to_del->prev) {
      to_del->prev = NULL;
      queue->head->next = NULL;
    } else {
      queue->head = queue->tail = NULL;
    }
  }

  return to_del;
}

task_t dequeue(task_queue_t *queue) {
  task_t to_del = NULL;

  if (queue) {
    THREAD_SAFE_ZONE(&queue->mutex, {
      to_del = __dequeue(queue);
    })
  }

  return to_del;
}

void init_queue(task_queue_t **queue) {
  *queue = calloc(1, sizeof(task_queue_t));
  if (!(*queue)) {
    return;
  }

  pthread_mutex_init(&(*queue)->mutex, NULL);
}

void destroy_queue(task_queue_t *queue) {
  if (queue) {
    THREAD_SAFE_ZONE(&queue->mutex, {
      for_each_task_safe(curr, queue) {
        task_t node = __dequeue(queue);

        destroy_task(node);
      }
    })

    pthread_mutex_destroy(&queue->mutex);
    free(queue);
  }
}
