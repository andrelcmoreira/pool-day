#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "pool_day.h"
#include "task.h"

#define HELLO_WORLD_TASK_ID 0

volatile bool task_finished = false;

void task_start_callback(uint32_t tid, const void *param) {
  (void)param;
  printf("task '%u' starting...\n", tid);
}

void task_end_callback(uint32_t tid, const void *param, void *ret_val) {
  (void)param;

  task_finished = true;
  printf("task '%u' ended with return value: %s\n", tid, (char *)ret_val);
  free(ret_val);
}

void *func(void *param) {
  char *str = (char *)param;
  char *ret = (char *)calloc(10, sizeof(char));

  for (int i = 0; i < 10; i++) {
    printf("%s %d\n", str, i);
    usleep(1000000);
  }

  strcat(ret, "success!");

  return ret;
}

int main(void) {
  pool_day_t pool;

  pool = create_pool(1);
  if (!pool) {
    // handle error
    exit(EXIT_FAILURE);
  }

  char str[] = "hello, world!";
  task_t task = create_async_task(HELLO_WORLD_TASK_ID, func, (void *)str,
                                  sizeof(char) * strlen(str) + 1, false,
                                  task_start_callback, task_end_callback);

  if (!task) {
    // handle error
    exit(EXIT_FAILURE);
  }

  assert(enqueue_task(pool, task) == POOL_DAY_SUCCESS);

  while(!task_finished);

  destroy_task(task);
  destroy_pool(&pool);

  exit(EXIT_SUCCESS);
}
