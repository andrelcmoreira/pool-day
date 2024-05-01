#include <gtest/gtest.h>

#include "mock/callback_mock.h"

extern "C" {
#include "pool_day.h"

extern void *thread_func(void *param);
}

using testing::_;
using testing::InvokeWithoutArgs;
using testing::Test;

class PoolDayTest : public Test {
 public:
  PoolDayTest() : pool_{create_pool(1)} {}

  void SetUp(void) override {
    ON_CALL(CbWrapper::mock(), TaskCb(_))
      .WillByDefault(
        InvokeWithoutArgs([&]() {
          abort_tasks(pool_);  // to break the thread loop
        }
      ));
  }

  ~PoolDayTest() {
    destroy_pool(&pool_);
  }

 protected:
  pool_day_t pool_;   //!< Library instance.
  CbWrapper wrapper_; //!< Callback instance.
};

/**
 * @brief When we try to create a pool with size 0, then null must be returned.
 */
TEST_F(PoolDayTest, CreatePollWithInvalidSize) {
  EXPECT_EQ(create_pool(0), nullptr);
}

/**
 * @brief Given we have a valid pool handle, when we enqueue a single task into
 * it, then it must be added to the pool's task queue.
 */
TEST_F(PoolDayTest, EnqueueSingleTaskWithPoolEmpty) {
  auto task = create_task(nullptr, nullptr, 0);

  EXPECT_EQ(idle_tasks(pool_), 0);
  EXPECT_EQ(enqueue_task(pool_, task), POOL_DAY_SUCCESS);
  EXPECT_EQ(idle_tasks(pool_), 1);
}

/**
 * @brief Given we have a valid pool handle, when we enqueue several tasks into
 * it, then they must be added to the pool's task queue.
 */
TEST_F(PoolDayTest, EnqueueTaskWithPoolNotEmpty) {
  auto t1 = create_task(nullptr, nullptr, 0);
  auto t2 = create_task(nullptr, nullptr, 0);
  auto t3 = create_task(nullptr, nullptr, 0);
  auto t4 = create_task(nullptr, nullptr, 0);
  auto t5 = create_task(nullptr, nullptr, 0);

  EXPECT_EQ(idle_tasks(pool_), 0);

  EXPECT_EQ(enqueue_task(pool_, t1), POOL_DAY_SUCCESS);
  EXPECT_EQ(enqueue_task(pool_, t2), POOL_DAY_SUCCESS);
  EXPECT_EQ(enqueue_task(pool_, t3), POOL_DAY_SUCCESS);
  EXPECT_EQ(enqueue_task(pool_, t4), POOL_DAY_SUCCESS);
  EXPECT_EQ(enqueue_task(pool_, t5), POOL_DAY_SUCCESS);

  EXPECT_EQ(idle_tasks(pool_), 5);
}

/**
 * @brief Given we have a task, when we try to enqueue it to a null pool handle,
 * then nothing must happen and the suitable error code must be returned.
 */
TEST_F(PoolDayTest, EnqueueTaskWithNullPool) {
  auto task = create_task(nullptr, nullptr, 0);

  EXPECT_EQ(enqueue_task(nullptr, task), POOL_DAY_ERROR_NULL_PARAM);
  free(task);
}

/**
 * @brief Given we have a valid pool, when we try to enqueue a null task to it,
 * then nothing must happen and the suitable error code must be returned.
 */
TEST_F(PoolDayTest, EnqueueTaskWithNullTask) {
  auto task = create_task(nullptr, nullptr, 0);

  EXPECT_EQ(enqueue_task(pool_, nullptr), POOL_DAY_ERROR_NULL_PARAM);
  free(task);
}

/**
 * @brief Given we have a pool with no tasks, when we try to get the number of
 * idle tasks of the pool, then 0 must be returned.
 */
TEST_F(PoolDayTest, GetIdleTasksCountWithNoTasks) {
  EXPECT_EQ(idle_tasks(pool_), 0);
}

/**
 * @brief Given we have a pool with a single enqueued task, when we try to get
 * the number of idle tasks in the pool, then 1 must be returned.
 */
TEST_F(PoolDayTest, GetIdleTasksCountWithSingleTask) {
  {
    auto task = create_task(nullptr, nullptr, 0);

    enqueue_task(pool_, task);
  }

  EXPECT_EQ(idle_tasks(pool_), 1);
}

/**
 * @brief Given we have a pool with several enqueued tasks, when we try to get
 * the number of idle tasks in the pool, then the correct task number must be
 * returned.
 */
TEST_F(PoolDayTest, GetIdleTasksCountWithSeveralTasks) {
  {
    auto t1 = create_task(nullptr, nullptr, 0);
    auto t2 = create_task(nullptr, nullptr, 0);
    auto t3 = create_task(nullptr, nullptr, 0);
    auto t4 = create_task(nullptr, nullptr, 0);
    auto t5 = create_task(nullptr, nullptr, 0);

    enqueue_task(pool_, t1);
    enqueue_task(pool_, t2);
    enqueue_task(pool_, t3);
    enqueue_task(pool_, t4);
    enqueue_task(pool_, t5);
  }

  EXPECT_EQ(idle_tasks(pool_), 5);
}

/**
 * @brief Given we have a null pool handle, when we try to get the number of
 * idle tasks from it, then 0 must be returned.
 */
TEST_F(PoolDayTest, GetIdleTasksWithNullHandle) {
  EXPECT_EQ(idle_tasks(nullptr), 0);
}

/**
 * @brief Given we have a null pool handle, when we try to destroy it, then
 * nothing must happen and the suitable error code must be returned.
 */
TEST_F(PoolDayTest, DestroyPollWithNullHandle) {
  auto ret = destroy_pool(nullptr);

  EXPECT_EQ(ret, POOL_DAY_ERROR_NULL_PARAM);
}

/**
 * @brief Given the pool has one task with null parameter, when the task is
 * scheduled for execution, then the task's callback must be called.
 */
TEST_F(PoolDayTest, ExecuteTaskWithNullParameterWithSuccess) {
  {
    auto task = create_task(CbWrapper::TaskCb, nullptr, 0);
    enqueue_task(pool_, task);
  }

  EXPECT_CALL(CbWrapper::mock(), TaskCb(nullptr)).Times(1);

  auto ret = thread_func(pool_);
  EXPECT_EQ(ret, nullptr);
}

/**
 * @brief Given the pool has one task with a parameter, when the task is
 * scheduled for execution, then the task's callback must be called with the
 * parameter.
 */
TEST_F(PoolDayTest, ExecuteTaskWithParameterWithSuccess) {
  char param[]{ "param" };

  {
    auto task = create_task(CbWrapper::TaskCb, param, 0);
    enqueue_task(pool_, task);
  }

  EXPECT_CALL(CbWrapper::mock(), TaskCb(param)).Times(1);

  auto ret = thread_func(pool_);
  EXPECT_EQ(ret, nullptr);
}

/**
 * @brief Given the pool has one task and the 'must_stop' flag is true, when the
 * task is scheduled for execution, then the task's callback must not be called.
 */
TEST_F(PoolDayTest, ExecuteTaskWithMustStopSet) {
  {
    auto task = create_task(CbWrapper::TaskCb, nullptr, 0);
    enqueue_task(pool_, task);
  }

  EXPECT_CALL(CbWrapper::mock(), TaskCb(_)).Times(0);

  abort_tasks(pool_);

  auto ret = thread_func(pool_);
  EXPECT_EQ(ret, nullptr);
}

/**
 * @brief When we try to abort the pool tasks with a null handle, then the
 * suitable error code must be returned.
 */
TEST_F(PoolDayTest, AbortTasksWithNullPoolHandle) {
  EXPECT_EQ(abort_tasks(nullptr), POOL_DAY_ERROR_NULL_PARAM);
}
