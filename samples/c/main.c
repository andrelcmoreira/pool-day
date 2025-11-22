#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "pool_day.h"

void task_start_callback(uint32_t tid) {
  printf("task '%u' starting...\n", tid);
}

void task_end_callback(uint32_t tid, void *ret_val) {
  printf("task '%u' ended with return value: %s\n", tid, (char *)ret_val);
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
  task_t *task;

  pool = create_pool(1);
  if (!pool) {
    // handle error
    exit(EXIT_FAILURE);
  }

  task = create_task(123, func, (void *)"foo", task_start_callback,
                     task_end_callback);

  assert(enqueue_task(pool, task) == POOL_DAY_SUCCESS);
  char *ret = (char *)wait_task_finish(pool, task);

  destroy_pool(&pool);

  printf("result = %s\n", ret);
  free(ret);

  exit(EXIT_SUCCESS);
}
