#include <gtest/gtest.h>

extern "C" {
#include "core/queue.h"
}

class QueueTest : public ::testing::Test {
  public:
   void SetUp() override {
     init_queue(&queue_);
   }

   void TearDown() override {
     destroy_queue(queue_);
   }

  protected:
   task_queue_t *queue_; //!< Queue instance.
};

/**
 * @brief Given we have an empty queue, when we try to get its size, then 0 must
 * be returned.
 */
TEST_F(QueueTest, GetQueueSizeWithEmptyQueue) {
  EXPECT_EQ(queue_size(queue_), 0);
}

/**
 * @brief Given we have a queue with several elements, when we get its size,
 * then a non zero value must be returned.
 */
TEST_F(QueueTest, GetQueueSizeWithNotEmptyQueue) {
  {
    auto t1 = create_task(nullptr, nullptr, 0);
    auto t2 = create_task(nullptr, nullptr, 0);
    auto t3 = create_task(nullptr, nullptr, 0);
    auto t4 = create_task(nullptr, nullptr, 0);
    auto t5 = create_task(nullptr, nullptr, 0);

    enqueue(queue_, t1);
    enqueue(queue_, t2);
    enqueue(queue_, t3);
    enqueue(queue_, t4);
    enqueue(queue_, t5);
  }

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
 * @brief Given we have a not empty queue, when we add several elements into it,
 * then its size must be increased.
 */
TEST_F(QueueTest, EnqueueElementWithNotEmptyQueue) {
  {
    auto t1 = create_task(nullptr, nullptr, 0);
    auto t2 = create_task(nullptr, nullptr, 0);

    enqueue(queue_, t1);
    enqueue(queue_, t2);

    EXPECT_EQ(queue_size(queue_), 2);
  }

  auto t3 = create_task(nullptr, nullptr, 0);

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
 * @brief Given we have a not empty queue, when we try to add a null element
 * into it, then its size must remain the same.
 */
TEST_F(QueueTest, EnqueueNullElementWithNotEmptyQueue) {
  {
    auto t1 = create_task(nullptr, nullptr, 0);
    auto t2 = create_task(nullptr, nullptr, 0);

    enqueue(queue_, t1);
    enqueue(queue_, t2);
  }

  EXPECT_EQ(queue_size(queue_), 2);
  enqueue(queue_, nullptr);
  EXPECT_EQ(queue_size(queue_), 2);
}

/**
 * @brief Given we have a null queue, when we try to add an element to it,
 * then nothing must happen.
 */
TEST_F(QueueTest, EnqueueElementWithNullQueue) {
  auto t1 = create_task(nullptr, nullptr, 0);

  enqueue(nullptr, t1);
  free(t1);
}

/**
 * @brief Given we have an empty queue, when we try to dequeue an element from
 * it, then null must be returned and the size of the queue must remain the
 * same.
 */
TEST_F(QueueTest, DequeueElementWithEmptyQueue) {
  EXPECT_EQ(queue_size(queue_), 0);

  auto ret = dequeue(queue_);

  EXPECT_EQ(ret, nullptr);
  EXPECT_EQ(queue_size(queue_), 0);
}

/**
 * @brief Given we have a not empty queue, when we try to dequeue a single
 * element from it, then the first element of the queue must be returned and the
 * size of the queue must be decreased by 1.
 */
TEST_F(QueueTest, DequeueSingleElementWithNotEmptyQueue) {
  auto t1 = create_task(nullptr, nullptr, 0);
  auto t2 = create_task(nullptr, nullptr, 0);
  auto t3 = create_task(nullptr, nullptr, 0);

  enqueue(queue_, t1);
  enqueue(queue_, t2);
  enqueue(queue_, t3);

  EXPECT_EQ(queue_size(queue_), 3);

  auto ret = dequeue(queue_);

  EXPECT_EQ(ret, t1);
  EXPECT_EQ(queue_size(queue_), 2);

  free(ret);
}

/**
 * @brief Given we have a not empty queue, when we try to dequeue multiple
 * elements from it, then the correct elements of the queue must be returned in
 * the correct sequence and the size of the queue must be decreased according to
 * the operations.
 */
TEST_F(QueueTest, DequeueMultipleElementsWithNotEmptyQueue) {
  auto t1 = create_task(nullptr, nullptr, 0);
  auto t2 = create_task(nullptr, nullptr, 0);
  auto t3 = create_task(nullptr, nullptr, 0);
  auto t4 = create_task(nullptr, nullptr, 0);
  task_t *ret;

  enqueue(queue_, t1);
  enqueue(queue_, t2);
  enqueue(queue_, t3);
  enqueue(queue_, t4);

  EXPECT_EQ(queue_size(queue_), 4);

  {
    ret = dequeue(queue_);

    EXPECT_EQ(ret, t1);
    EXPECT_EQ(queue_size(queue_), 3);

    free(ret);
  }

  {
    ret = dequeue(queue_);

    EXPECT_EQ(ret, t2);
    EXPECT_EQ(queue_size(queue_), 2);

    free(ret);
  }

  {
    ret = dequeue(queue_);

    EXPECT_EQ(ret, t3);
    EXPECT_EQ(queue_size(queue_), 1);

    free(ret);
  }

  {
    ret = dequeue(queue_);

    EXPECT_EQ(ret, t4);
    EXPECT_EQ(queue_size(queue_), 0);

    free(ret);
  }

  {
    ret = dequeue(queue_);

    EXPECT_EQ(ret, nullptr);
    EXPECT_EQ(queue_size(queue_), 0);
  }
}

/**
 * @brief Given we have a null queue, when we try to dequeue an element from
 * it, then null must be returned.
 */
TEST_F(QueueTest, DequeueElementWithNullQueue) {
  auto ret = dequeue(nullptr);

  EXPECT_EQ(ret, nullptr);
}
