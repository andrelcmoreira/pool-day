#include <stdio.h>
#include <unistd.h>

#include "pool_day.h"

void func(void *param) {
  char *str = (char *)param;

  for (int i = 0; i < 10; i++) {
    printf("%s %d\n", str, i);
    usleep(1000000);
  }
}

void test_1() {
  printf("STARTING %s\n\n", __FUNCTION__);
  pool_day_t pool;

  pool = create_pool(1);
  if (pool) {
    destroy_pool(&pool);
  }

  printf("FINISHING %s\n\n", __FUNCTION__);
}

void test_2() {
  printf("STARTING %s\n\n", __FUNCTION__);

  pool_day_t pool;

  pool = create_pool(4);
  if (pool) {
    destroy_pool(&pool);
  }

  printf("FINISHING %s\n\n", __FUNCTION__);
}

void test_3() {
  printf("STARTING %s\n\n", __FUNCTION__);

  pool_day_t pool;
  task_node_t *task_1, *task_2, *task_3, *task_4, *task_5;

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

  printf("FINISHING %s\n\n", __FUNCTION__);
}

void test_4() {
  printf("STARTING %s\n\n", __FUNCTION__);

  pool_day_t pool;
  task_node_t *task_1, *task_2, *task_3, *task_4, *task_5;

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

  printf("FINISHING %s\n\n", __FUNCTION__);
}

void test_5() {
  printf("STARTING %s\n\n", __FUNCTION__);

  pool_day_t pool;
  task_node_t *task_1, *task_2, *task_3, *task_4, *task_5;

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

  printf("FINISHING %s\n\n", __FUNCTION__);
}

int main(void) {
  test_1();
  test_2();
  test_3();
  test_4();
  test_5();

  return 0;
}
