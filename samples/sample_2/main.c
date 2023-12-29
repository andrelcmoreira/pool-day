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

  pool = create_pool(3);
  if (!pool) {
    // handle error
    exit(EXIT_FAILURE);
  }

  task_t *t1 = create_task(func, (void *)"foo");
  task_t *t2 = create_task(func, (void *)"bar");
  task_t *t3 = create_task(func, (void *)"baz");
  task_t *t4 = create_task(func, (void *)"qux");
  task_t *t5 = create_task(func, (void *)"blah");

  assert(enqueue_task(pool, t1) == POOL_DAY_SUCCESS);
  assert(enqueue_task(pool, t2) == POOL_DAY_SUCCESS);

  printf("press enter to enqueue two more tasks\n");
  getchar();

  assert(enqueue_task(pool, t3) == POOL_DAY_SUCCESS);
  assert(enqueue_task(pool, t4) == POOL_DAY_SUCCESS);

  printf("press enter to enqueue one more task\n");
  getchar();

  assert(enqueue_task(pool, t5) == POOL_DAY_SUCCESS);

  printf("press enter to destroy the pool\n");
  getchar();

  destroy_pool(&pool);

  exit(EXIT_SUCCESS);
}
