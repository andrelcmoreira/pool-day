#include <gtest/gtest.h>

#include "mock/callback_mock.h"

extern "C" {
#include "pool_day.h"

extern void *thread_func(void *param);
}

using testing::_;
using testing::Invoke;
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
          return nullptr;
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
 * @brief Custom matcher to compare void pointers containing strings.
 */
MATCHER_P(StrEqVoidPointer, expected_string, "") {
  return std::string(static_cast<const char*>(arg)) == expected_string;
}

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
  auto task = create_task(0, nullptr, nullptr, 0, nullptr, nullptr);

  EXPECT_EQ(queued_tasks(pool_), 0);
  EXPECT_EQ(enqueue_task(pool_, task), POOL_DAY_SUCCESS);
  EXPECT_EQ(queued_tasks(pool_), 1);

  // to force the task destruction on destroy_pool call
  task->is_orphan = true;
}

/**
 * @brief Given we have a valid pool handle, when we enqueue several tasks into
 * it, then they must be added to the pool's task queue.
 */
TEST_F(PoolDayTest, EnqueueTaskWithPoolNotEmpty) {
  auto t1 = create_task(0, nullptr, nullptr, 0, nullptr, nullptr);
  auto t2 = create_task(1, nullptr, nullptr, 0, nullptr, nullptr);
  auto t3 = create_task(2, nullptr, nullptr, 0, nullptr, nullptr);
  auto t4 = create_task(3, nullptr, nullptr, 0, nullptr, nullptr);
  auto t5 = create_task(4, nullptr, nullptr, 0, nullptr, nullptr);

  EXPECT_EQ(queued_tasks(pool_), 0);

  EXPECT_EQ(enqueue_task(pool_, t1), POOL_DAY_SUCCESS);
  EXPECT_EQ(enqueue_task(pool_, t2), POOL_DAY_SUCCESS);
  EXPECT_EQ(enqueue_task(pool_, t3), POOL_DAY_SUCCESS);
  EXPECT_EQ(enqueue_task(pool_, t4), POOL_DAY_SUCCESS);
  EXPECT_EQ(enqueue_task(pool_, t5), POOL_DAY_SUCCESS);

  EXPECT_EQ(queued_tasks(pool_), 5);

  // to force the task destruction on destroy_pool call
  t1->is_orphan = true;
  t2->is_orphan = true;
  t3->is_orphan = true;
  t4->is_orphan = true;
  t5->is_orphan = true;
}

/**
 * @brief Given we have a task, when we try to enqueue it to a null pool handle,
 * then nothing must happen and the suitable error code must be returned.
 */
TEST_F(PoolDayTest, EnqueueTaskWithNullPool) {
  auto task = create_task(0, nullptr, nullptr, 0, nullptr, nullptr);

  EXPECT_EQ(enqueue_task(nullptr, task), POOL_DAY_ERROR_NULL_PARAM);
  free(task);
}

/**
 * @brief Given we have a valid pool, when we try to enqueue a null task to it,
 * then nothing must happen and the suitable error code must be returned.
 */
TEST_F(PoolDayTest, EnqueueTaskWithNullTask) {
  auto task = create_task(0, nullptr, nullptr, 0, nullptr, nullptr);

  EXPECT_EQ(enqueue_task(pool_, nullptr), POOL_DAY_ERROR_NULL_PARAM);
  free(task);
}

/**
 * @brief Given we have a valid pool and a task already bound to a pool, when we
 * try to enqueue it, then nothing must happen and the suitable error code
 * must be returned.
 */
TEST_F(PoolDayTest, EnqueueTaskAlreadyBound) {
  auto another_pool = create_pool(1);
  auto task = create_task(0, nullptr, nullptr, 0, nullptr, nullptr);

  EXPECT_EQ(enqueue_task(pool_, task), POOL_DAY_SUCCESS);

  destroy_pool(&another_pool);

  // to force the task destruction on destroy_pool call
  task->is_orphan = true;
}

/**
 * @brief Given we have a pool with no tasks, when we try to get the number of
 * queued tasks of the pool, then 0 must be returned.
 */
TEST_F(PoolDayTest, GetQueuedTasksCountWithNoTasks) {
  EXPECT_EQ(queued_tasks(pool_), 0);
}

/**
 * @brief Given we have a pool with a single enqueued task, when we try to get
 * the number of queued tasks in the pool, then 1 must be returned.
 */
TEST_F(PoolDayTest, GetQueuedTasksCountWithSingleTask) {
  auto task = create_task(0, nullptr, nullptr, 0, nullptr, nullptr);

  enqueue_task(pool_, task);

  EXPECT_EQ(queued_tasks(pool_), 1);

  // to force the task destruction on destroy_pool call
  task->is_orphan = true;
}

/**
 * @brief Given we have a pool with several enqueued tasks, when we try to get
 * the number of queued tasks in the pool, then the correct task number must be
 * returned.
 */
TEST_F(PoolDayTest, GetQueuedTasksCountWithSeveralTasks) {
  auto t1 = create_task(0, nullptr, nullptr, 0, nullptr, nullptr);
  auto t2 = create_task(1, nullptr, nullptr, 0, nullptr, nullptr);
  auto t3 = create_task(2, nullptr, nullptr, 0, nullptr, nullptr);
  auto t4 = create_task(3, nullptr, nullptr, 0, nullptr, nullptr);
  auto t5 = create_task(4, nullptr, nullptr, 0, nullptr, nullptr);

  enqueue_task(pool_, t1);
  enqueue_task(pool_, t2);
  enqueue_task(pool_, t3);
  enqueue_task(pool_, t4);
  enqueue_task(pool_, t5);

  EXPECT_EQ(queued_tasks(pool_), 5);

  // to force the task destruction on destroy_pool call
  t1->is_orphan = true;
  t2->is_orphan = true;
  t3->is_orphan = true;
  t4->is_orphan = true;
  t5->is_orphan = true;
}

/**
 * @brief Given we have a null pool handle, when we try to get the number of
 * queued tasks from it, then 0 must be returned.
 */
TEST_F(PoolDayTest, GetQueuedTasksWithNullHandle) {
  EXPECT_EQ(queued_tasks(nullptr), 0);
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
  auto task = create_task(0, CbWrapper::TaskCb, nullptr, 0, nullptr, nullptr);
  char ret_val[]{ "hello, i'm the return of the task" };

  enqueue_task(pool_, task);

  EXPECT_CALL(CbWrapper::mock(), OnTaskStartCb(_, _))
    .Times(0);
  EXPECT_CALL(CbWrapper::mock(), TaskCb(nullptr))
    .Times(1)
    .WillOnce(
      InvokeWithoutArgs([&]() {
        abort_tasks(pool_);  // to break the thread loop
        return ret_val;
      })
    );
  EXPECT_CALL(CbWrapper::mock(), OnTaskEndCb(_, _, _))
    .Times(0);

  auto ret = thread_func(pool_);
  EXPECT_EQ(ret, nullptr);
  EXPECT_EQ(task->ret_val, ret_val);

  // cleanup
  destroy_task(task);
}

/**
 * @brief Given the pool has one task with callbacks, when the task is
 * scheduled for execution, then the task's callbacks must be called at the
 * suitable times.
 */
TEST_F(PoolDayTest, ExecuteTaskWithCallbacks) {
  auto task = create_task(0, CbWrapper::TaskCb, nullptr, 0,
                          CbWrapper::OnTaskStartCb, CbWrapper::OnTaskEndCb);
  char ret_val[]{ "hello, i'm the return of the task" };

  enqueue_task(pool_, task);

  EXPECT_CALL(CbWrapper::mock(), OnTaskStartCb(task->id, task->param))
    .Times(1);
  EXPECT_CALL(CbWrapper::mock(), TaskCb(nullptr))
    .Times(1)
    .WillOnce(
      InvokeWithoutArgs([&]() {
        abort_tasks(pool_);  // to break the thread loop
        return ret_val;
      })
    );
  EXPECT_CALL(CbWrapper::mock(), OnTaskEndCb(task->id, task->param, ret_val))
    .Times(1);

  auto ret = thread_func(pool_);
  EXPECT_EQ(ret, nullptr);
}

/**
 * @brief Given the pool has one task with only start callback, when the task is
 * scheduled for execution, then the task's start callback must be called.
 */
TEST_F(PoolDayTest, ExecuteTaskWithStartCallbackOnly) {
  auto task = create_task(0, CbWrapper::TaskCb, nullptr, 0,
                          CbWrapper::OnTaskStartCb, nullptr);
  char ret_val[]{ "hello, i'm the return of the task" };

  enqueue_task(pool_, task);

  EXPECT_CALL(CbWrapper::mock(), OnTaskStartCb(task->id, task->param))
    .Times(1);
  EXPECT_CALL(CbWrapper::mock(), TaskCb(nullptr))
    .Times(1)
    .WillOnce(
      InvokeWithoutArgs([&]() {
        abort_tasks(pool_);  // to break the thread loop
        return ret_val;
      })
    );
  EXPECT_CALL(CbWrapper::mock(), OnTaskEndCb(_, _, _))
    .Times(0);

  auto ret = thread_func(pool_);
  EXPECT_EQ(ret, nullptr);
  EXPECT_EQ(task->ret_val, ret_val);

  // cleanup
  destroy_task(task);
}

/**
 * @brief Given the pool has one task with only end callback, when the task is
 * scheduled for execution, then the task's end callback must be called.
 */
TEST_F(PoolDayTest, ExecuteTaskWithEndCallbackOnly) {
  auto task = create_task(0, CbWrapper::TaskCb, nullptr, 0, nullptr,
                          CbWrapper::OnTaskEndCb);
  char ret_val[]{ "hello, i'm the return of the task" };

  enqueue_task(pool_, task);

  EXPECT_CALL(CbWrapper::mock(), OnTaskStartCb(_, _))
    .Times(0);
  EXPECT_CALL(CbWrapper::mock(), TaskCb(nullptr))
    .Times(1)
    .WillOnce(
      InvokeWithoutArgs([&]() {
        abort_tasks(pool_);  // to break the thread loop
        return ret_val;
      })
    );
  EXPECT_CALL(CbWrapper::mock(), OnTaskEndCb(task->id, task->param, ret_val))
    .Times(1);

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
  char ret_val[]{ "hello, i'm the return of the task" };
  auto task = create_task(0, CbWrapper::TaskCb, param,
                          sizeof(char) * strlen(param) + 1, nullptr, nullptr);

  enqueue_task(pool_, task);

  EXPECT_CALL(CbWrapper::mock(), OnTaskStartCb(_, _))
    .Times(0);
  EXPECT_CALL(CbWrapper::mock(), TaskCb(StrEqVoidPointer(param)))
    .Times(1)
    .WillOnce(
      InvokeWithoutArgs([&]() {
        abort_tasks(pool_);  // to break the thread loop
        return ret_val;
      })
    );
  EXPECT_CALL(CbWrapper::mock(), OnTaskEndCb(_, _, _))
    .Times(0);

  auto ret = thread_func(pool_);
  EXPECT_EQ(ret, nullptr);
  EXPECT_EQ(task->ret_val, ret_val);

  // cleanup
  destroy_task(task);
}

/**
 * @brief Given the pool has one task and the 'must_stop' flag is true, when the
 * task is scheduled for execution, then the task's callback must not be called.
 */
TEST_F(PoolDayTest, ExecuteTaskWithMustStopSet) {
  auto task = create_task(0, CbWrapper::TaskCb, nullptr, 0, nullptr, nullptr);

  enqueue_task(pool_, task);

  EXPECT_CALL(CbWrapper::mock(), TaskCb(_)).Times(0);

  abort_tasks(pool_);

  auto ret = thread_func(pool_);
  EXPECT_EQ(ret, nullptr);

  // to force the task destruction on destroy_pool call
  task->is_orphan = true;
}

/**
 * @brief When we try to abort the pool tasks with a null handle, then the
 * suitable error code must be returned.
 */
TEST_F(PoolDayTest, AbortTasksWithNullPoolHandle) {
  EXPECT_EQ(abort_tasks(nullptr), POOL_DAY_ERROR_NULL_PARAM);
}

/**
 * @brief Given we have an enqueued task, when we wait for the finish of the
 * task, then the correct return value must be returned with success.
 */
TEST_F(PoolDayTest, GetTaskResultWithSuccess) {
  int ret_val{1234};
  char param[]{ "param" };
  auto task = create_task(0, CbWrapper::TaskCb, param,
                          sizeof(char) * strlen(param) + 1, nullptr, nullptr);

  {
    enqueue_task(pool_, task);

    EXPECT_CALL(CbWrapper::mock(), OnTaskStartCb(_, _))
      .Times(0);
    EXPECT_CALL(CbWrapper::mock(), TaskCb(StrEqVoidPointer(param)))
      .Times(1)
      .WillOnce(
        Invoke([&]() {
          abort_tasks(pool_);
          return &ret_val;
        })
      );
    EXPECT_CALL(CbWrapper::mock(), OnTaskEndCb(_, _, _))
      .Times(0);

    thread_func(pool_);
  }

  auto ret = reinterpret_cast<int *>(get_task_result(task));
  EXPECT_EQ(*ret, ret_val);

  // cleanup
  destroy_task(task);
}

/**
 * @brief Given we have a null task and a valid pool handle, when we wait for
 * the finish of the task, then null must be returned.
 */
TEST_F(PoolDayTest, GetTaskResultWithNullPoolTask) {
  EXPECT_EQ(get_task_result(nullptr), nullptr);
}

/**
 * @brief Given we have a task not bound to the pool, when we wait for
 * the finish of the task, then null must be returned.
 */
TEST_F(PoolDayTest, GetTaskResultWithWithUnboundTask) {
  auto task = create_task(0, nullptr, nullptr, 0, nullptr, nullptr);

  EXPECT_EQ(get_task_result(task), nullptr);

  // cleanup
  destroy_task(task);
}
