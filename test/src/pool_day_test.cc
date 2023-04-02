#include <gtest/gtest.h>

extern "C" {
#include "pool_day.h"
}

class PoolDayTest : public ::testing::Test {
  public:
   PoolDayTest() : pool_{create_pool(1)} {}

   ~PoolDayTest() {
     destroy_pool(&pool_);
   }

  protected:
   pool_day_t pool_; //!< Library instance.
};

/**
 * @brief Given we have a pool with no enqueued tasks, when we try to get the
 * number of idle tasks in the pool, then 0 must be returned.
 */
TEST_F(PoolDayTest, GetIdleTasksCountWithNoTasks) {
  EXPECT_EQ(idle_tasks(pool_), 0);
}

/**
 * @brief Given we have a pool with a single enqueued task, when we try to get
 * the number of idle tasks in the pool, then 1 must be returned.
 */
TEST_F(PoolDayTest, GetIdleTasksCountWithSingleTask) {
  task_t *t = create_task(nullptr, nullptr);

  enqueue_task(pool_, t);

  EXPECT_EQ(idle_tasks(pool_), 1);
}

/**
 * @brief Given we have a pool with several enqueued tasks, when we try to get
 * the number of idle tasks in the pool, then the correct task number must be
 * returned.
 */
TEST_F(PoolDayTest, GetIdleTasksCountWithSeveralTasks) {
  task_t *t1 = create_task(nullptr, nullptr);
  task_t *t2 = create_task(nullptr, nullptr);
  task_t *t3 = create_task(nullptr, nullptr);
  task_t *t4 = create_task(nullptr, nullptr);
  task_t *t5 = create_task(nullptr, nullptr);

  enqueue_task(pool_, t1);
  enqueue_task(pool_, t2);
  enqueue_task(pool_, t3);
  enqueue_task(pool_, t4);
  enqueue_task(pool_, t5);

  EXPECT_EQ(idle_tasks(pool_), 5);
}
