#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "pool_day.h"
#include "task.h"

#define DUMMY_TASK_ID 0

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

  char str[] = "foo";
  task_t task = create_sync_task(DUMMY_TASK_ID, func, (void *)str,
                                 sizeof(char) * strlen(str) + 1);

  assert(enqueue_task(pool, task) == POOL_DAY_SUCCESS);
  char *ret = (char *)get_task_result(task);

  printf("result = %s\n", ret);

  destroy_task(task);
  destroy_pool(&pool);
  free(ret);

  exit(EXIT_SUCCESS);
}
