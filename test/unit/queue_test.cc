#include <gtest/gtest.h>

extern "C" {
#include "queue.h"
}

class QueueTest : public ::testing::Test {
  public:
    QueueTest() {
      init_queue(&queue_);
    }

    ~QueueTest() {
      destroy_queue(queue_);
    }

  protected:
    task_queue_t *queue_; //!< Queue instance.
};

/**
 * @brief Given we have a queue, when we try to get its size, then 0 must be
 * returned.
 */
TEST_F(QueueTest, GetQueueSizeWithEmptyQueue) {
  EXPECT_EQ(queue_size(queue_), 0);
}

/**
 * @brief Given we have a queue, when we add several elements to it and get its
 * size, then a non zero value must be returned.
 */
TEST_F(QueueTest, GetQueueSizeWithNotEmptyQueue) {
  task_t *t1 = create_task(nullptr, nullptr);
  task_t *t2 = create_task(nullptr, nullptr);
  task_t *t3 = create_task(nullptr, nullptr);
  task_t *t4 = create_task(nullptr, nullptr);
  task_t *t5 = create_task(nullptr, nullptr);

  enqueue(queue_, t1);
  enqueue(queue_, t2);
  enqueue(queue_, t3);
  enqueue(queue_, t4);
  enqueue(queue_, t5);

  EXPECT_EQ(queue_size(queue_), 5);
}

/**
 * @brief Given we have a null queue, when we try to get its size, then 0 must
 * be returned.
 */
TEST_F(QueueTest, GetQueueSizeWithNullQueue) {
  EXPECT_EQ(queue_size(nullptr), 0);
}

/**
 * @brief Given we have an empty queue, when we add several elements into it,
 * then its size must be greater than 0.
 */
TEST_F(QueueTest, EnqueueElementWithEmptyQueue) {
  task_t *t1 = create_task(nullptr, nullptr);
  task_t *t2 = create_task(nullptr, nullptr);

  enqueue(queue_, t1);
  enqueue(queue_, t2);

  EXPECT_EQ(queue_size(queue_), 2);
}

/**
 * @brief Given we have a not empty queue, when we add several elements into it,
 * then its size must be greater than 0.
 */
TEST_F(QueueTest, EnqueueElementWithNotEmptyQueue) {
  {
    task_t *t1 = create_task(nullptr, nullptr);
    task_t *t2 = create_task(nullptr, nullptr);

    enqueue(queue_, t1);
    enqueue(queue_, t2);

    EXPECT_EQ(queue_size(queue_), 2);
  }

  task_t *t3 = create_task(nullptr, nullptr);

  enqueue(queue_, t3);

  EXPECT_EQ(queue_size(queue_), 3);
}

/**
 * @brief Given we have an empty queue, when we try to add a null elements into
 * it, then its size must remain 0.
 */
TEST_F(QueueTest, EnqueueNullElementWithEmptyQueue) {
  EXPECT_EQ(queue_size(queue_), 0);
  enqueue(queue_, nullptr);
  EXPECT_EQ(queue_size(queue_), 0);
}

/**
 * @brief Given we have a non empty queue, when we try to add a null element
 * into it, then its size must remain 0.
 */
TEST_F(QueueTest, EnqueueNullElementWithNotEmptyQueue) {
  {
    task_t *t1 = create_task(nullptr, nullptr);
    task_t *t2 = create_task(nullptr, nullptr);

    enqueue(queue_, t1);
    enqueue(queue_, t2);
  }

  EXPECT_EQ(queue_size(queue_), 2);
  enqueue(queue_, nullptr);
  EXPECT_EQ(queue_size(queue_), 2);
}

/**
 * @brief Given we have a null queue, when we try to add a element into it,
 * then nothing must happen.
 */
TEST_F(QueueTest, EnqueueElementWithNullQueue) {
  task_t *t1 = create_task(nullptr, nullptr);

  enqueue(nullptr, t1);
  free(t1);
}
