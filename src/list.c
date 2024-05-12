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
  if (list && task) {
    THREAD_SAFE_ZONE(&list->mutex, {
      for_each_task(curr, list) {
        if (curr == task) {
          if (list->head == task) {
            list->head = list->head->prev;
            // if the the new head points to a valid list element
            if (list->head) {
              list->head->next = NULL;
            }
            task->next = task->prev = NULL;
          } else if (list->tail == task) {
            list->tail = task->next;
            list->tail->prev = NULL;
            task->next = task->prev = NULL;
          } else {
            task->prev->next = task->next;
            task->next->prev = task->prev;
            task->next = task->prev = NULL;
          }
          break;
        }
      }
    })
  }
}

void insert(task_list_t *list, task_t *task) {
  if (list && task) {
    THREAD_SAFE_ZONE(&list->mutex, {
      // if the list is empty
      if (!list->head && !list->tail) {
        list->tail = list->head = task;
      } else {
        list->tail->prev = task;
        task->next = list->tail;
        list->tail = task;
      }
    })
  }
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
        //remove_task(list, curr);
        //free(curr);
      }
    })

    pthread_mutex_destroy(&list->mutex);
    free(list);
  }
}
