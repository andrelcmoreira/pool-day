#include <stdio.h>
#include <unistd.h>

#include "pool_day.h"

#define RUN_TEST(test_name) \
  printf("starting %s\n\n", #test_name); \
  run_##test_name(); \
  printf("finishing %s\n\n", #test_name);

#define TESTCASE(test_name) \
  void run_##test_name()

void func(void *param) {
  char *str = (char *)param;

  for (int i = 0; i < 10; i++) {
    printf("%s %d\n", str, i);
    usleep(1000000);
  }
}

TESTCASE(test_1) {
  pool_day_t pool;

  pool = create_pool(1);
  if (pool) {
    destroy_pool(&pool);
  }
}

TESTCASE(test_2) {
  pool_day_t pool;

  pool = create_pool(4);
  if (pool) {
    destroy_pool(&pool);
  }
}

TESTCASE(test_3) {
  pool_day_t pool;
  task_t *task_1, *task_2, *task_3, *task_4, *task_5;

  pool = create_pool(2);
  if (pool) {
    task_1 = create_task(func, (void *)"foo");
    task_2 = create_task(func, (void *)"bar");
    task_3 = create_task(func, (void *)"baz");
    task_4 = create_task(func, (void *)"qux");
    task_5 = create_task(func, (void *)"blah");

    enqueue_task(pool, task_1);
    enqueue_task(pool, task_2);
    enqueue_task(pool, task_3);
    enqueue_task(pool, task_4);
    enqueue_task(pool, task_5);

    printf("PRESS ENTER TO DESTROY THE POOL!!!!\n\n");

    getchar();
    destroy_pool(&pool);
  }
}

TESTCASE(test_4) {
  pool_day_t pool;
  task_t *task_1, *task_2, *task_3, *task_4, *task_5;

  pool = create_pool(3);
  if (pool) {
    task_1 = create_task(func, (void *)"foo");
    task_2 = create_task(func, (void *)"bar");
    task_3 = create_task(func, (void *)"baz");
    task_4 = create_task(func, (void *)"qux");
    task_5 = create_task(func, (void *)"blah");

    enqueue_task(pool, task_1);
    enqueue_task(pool, task_2);

    printf("PRESS ENTER TO ENQUEUE TWO MORE TASK!!!!\n\n");
    getchar();

    enqueue_task(pool, task_3);
    enqueue_task(pool, task_4);

    printf("PRESS ENTER TO ENQUEUE ONE MORE TASK!!!!\n\n");
    getchar();

    enqueue_task(pool, task_5);

    printf("PRESS ENTER TO DESTROY THE POOL!!!!\n\n");

    getchar();
    destroy_pool(&pool);
  }
}

TESTCASE(test_5) {
  pool_day_t pool;
  task_t *task_1, *task_2, *task_3, *task_4, *task_5;

  pool = create_pool(1);
  if (pool) {
    task_1 = create_task(func, (void *)"foo");
    task_2 = create_task(func, (void *)"bar");
    task_3 = create_task(func, (void *)"baz");
    task_4 = create_task(func, (void *)"qux");
    task_5 = create_task(func, (void *)"blah");

    enqueue_task(pool, task_1);

    printf("PRESS ENTER TO ENQUEUE ONE MORE TASK!!!!\n\n");
    getchar();
    enqueue_task(pool, task_2);

    printf("PRESS ENTER TO ENQUEUE ONE MORE TASK!!!!\n\n");
    getchar();
    enqueue_task(pool, task_3);

    printf("PRESS ENTER TO ENQUEUE ONE MORE TASK!!!!\n\n");
    getchar();
    enqueue_task(pool, task_4);

    printf("PRESS ENTER TO ENQUEUE ONE MORE TASK!!!!\n\n");
    getchar();

    enqueue_task(pool, task_5);

    printf("PRESS ENTER TO DESTROY THE POOL!!!!\n\n");

    getchar();
    destroy_pool(&pool);
  }
}

int main(void) {
  RUN_TEST(test_1);
  RUN_TEST(test_2);
  RUN_TEST(test_3);
  RUN_TEST(test_4);
  RUN_TEST(test_5);

  return 0;
}
