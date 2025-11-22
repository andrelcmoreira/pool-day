#include "internal/task.h"

#include <stdlib.h>

// cppcheck-suppress unusedFunction
task_t *create_task(void *(*task)(void *), void *param, void (*start_cb)(void),
                    void (*end_cb)(void *)) {
  task_t *node;

  node = (task_t *)calloc(1, sizeof(task_t));
  if (node) {
    node->task = task;
    node->param = param; // TODO: should we deep copy the param?
    node->on_task_start = start_cb;
    node->on_task_end = end_cb;

    sem_init(&node->ready, 0, 0);
  }

  return node;
}
