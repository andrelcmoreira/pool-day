#include "core/list.h"

#include <pthread.h>
#include <stdlib.h>

#include "core/task.h"
#include "core/utils.h"

/**
 * @brief Task list definition.
 */
struct task_list {
  task_t *tail;           //!< Tail of the list.
  task_t *head;           //!< Head of the list.
  pthread_mutex_t mutex;  //!< Mutex of the list.
};

uint32_t list_size(task_list_t *list) {
  uint32_t size = 0;

  if (list) {
    THREAD_SAFE_ZONE(&list->mutex, {
      for_each_task(curr, list) {
        size++;
      }
    })
  }

  return size;
}

void remove_task(task_list_t *list, task_t *task) {
  (void)list;
  (void)task;
}

void insert_task(task_list_t *list, task_t *task) {
  (void)list;
  (void)task;
}

bool has_task(task_list_t *list, task_t *task) {
  bool ret = false;

  if (list && task) {
    THREAD_SAFE_ZONE(&list->mutex, {
      for_each_task(curr, list) {
        if (curr == task) {
          ret = true;
          break;
        }
      }
    })
  }

  return ret;
}

void init_list(task_list_t **list) {
  *list = calloc(1, sizeof(task_list_t));
  if (!(*list)) {
    return;
  }

  pthread_mutex_init(&(*list)->mutex, NULL);
}

void destroy_list(task_list_t *list) {
  if (list) {
    THREAD_SAFE_ZONE(&list->mutex, {
      for_each_task_safe(curr, list) {
        remove_task(list, curr);

        free(curr);
      }
    })

    pthread_mutex_destroy(&list->mutex);
    free(list);
  }
}
