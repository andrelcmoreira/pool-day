#include "internal/task.h"

#include <stdlib.h>

task_t *create_task(void *(*task)(void *), void *param) {
  task_t *node;

  node = (task_t *)calloc(1, sizeof(task_t));
  if (node) {
    node->next = node->prev = NULL;
    node->task = task;
    node->param = param;
    node->ret_val = NULL;

    sem_init(&node->ready, 0, 0);
  }

  return node;
}
