#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "pool_day.h"

#define DUMMY_TASK_ID 0

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

  pool = create_pool(1);
  if (!pool) {
    // handle error
    exit(EXIT_FAILURE);
  }

  char str[] = "foo";
  task_t *task = create_task(DUMMY_TASK_ID, func, (void *)str,
                             sizeof(char) * strlen(str) + 1,
                             task_start_callback, task_end_callback);

  assert(enqueue_task(pool, task) == POOL_DAY_SUCCESS);
  char *ret = (char *)get_task_result(task);

  printf("result = %s\n", ret);

  destroy_pool(&pool);
  destroy_task(task);
  free(ret);

  exit(EXIT_SUCCESS);
}
