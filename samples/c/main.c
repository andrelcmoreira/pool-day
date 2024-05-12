#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "pool_day.h"

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

  task = create_task(func, (void *)"foo");

  assert(enqueue_task(pool, task) == POOL_DAY_SUCCESS);
  char *ret = (char *)wait_task_finish(pool, task);

  destroy_pool(&pool);

  printf("result = %s\n", ret);
  free(ret);

  exit(EXIT_SUCCESS);
}
