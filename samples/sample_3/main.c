#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "pool_day.h"

void func(void *param) {
  char *str = (char *)param;

  for (int i = 0; i < 10; i++) {
    printf("%s %d\n", str, i);
    usleep(1000000);
  }
}

int main(void) {
  pool_day_t pool;

  pool = create_pool(1);
  if (pool) {
    // handle error
    exit(EXIT_FAILURE);
  }

  task_t *task_1 = create_task(func, (void *)"foo");
  task_t *task_2 = create_task(func, (void *)"bar");
  task_t *task_3 = create_task(func, (void *)"baz");
  task_t *task_4 = create_task(func, (void *)"qux");
  task_t *task_5 = create_task(func, (void *)"blah");

  assert(enqueue_task(pool, task_1) == POOL_DAY_SUCCESS);

  printf("press enter to enqueue one more task\n");
  getchar();

  assert(enqueue_task(pool, task_2) == POOL_DAY_SUCCESS);

  printf("press enter to enqueue one more task\n");
  getchar();

  assert(enqueue_task(pool, task_3) == POOL_DAY_SUCCESS);

  printf("press enter to enqueue one more task\n");
  getchar();

  assert(enqueue_task(pool, task_4) == POOL_DAY_SUCCESS);

  printf("press enter to enqueue one more task\n");
  getchar();

  assert(enqueue_task(pool, task_5) == POOL_DAY_SUCCESS);

  printf("press enter to destroy the pool\n");
  getchar();

  destroy_pool(&pool);

  exit(EXIT_SUCCESS);
}
