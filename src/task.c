#include "task.h"

#include <stdlib.h>
#include <string.h>

#include "internal/task_def.h"

// cppcheck-suppress unusedFunction
task_t create_task(uint32_t id, void *(*task)(void *), void *param,
                   size_t param_size, bool auto_release,
                   void (*start_cb)(uint32_t, void *),
                   void (*end_cb)(uint32_t, void *, void *)) {
  task_t node;

  node = (task_t)calloc(1, sizeof(*node));
  if (node) {
    node->id = id;
    node->task = task;
    node->on_task_start = start_cb;
    node->on_task_end = end_cb;
    node->is_orphan = true;
    node->auto_release = auto_release;

    if (param) {
      node->param = malloc(param_size);
      memcpy(node->param, param, param_size);
    }

    sem_init(&node->ready, 0, 0);
  }

  return node;
}

void destroy_task(task_t task) {
  // if the task is orphaned, it means that the user will take care of its release,
  // otherwise it is managed by the pool
  if (task && task->is_orphan) {
    sem_destroy(&task->ready);

    if (task->param) {
      free(task->param);
    }
    free(task);
  }
}
