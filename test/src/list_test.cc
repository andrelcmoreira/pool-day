#include <gtest/gtest.h>

extern "C" {
#include "core/list.h"
#include "core/task.h"
}

class ListTest : public ::testing::Test {
  public:
   void SetUp() override {
     init_list(&list_);
   }

   void TearDown() override {
     destroy_list(list_);
   }

  protected:
   task_list_t *list_; //!< List instance.
};

/**
 * @brief Given we have an empty list, when we try to get its size, then 0 must
 * be returned.
 */
TEST_F(ListTest, GetListSizeWithEmptyList) {
  EXPECT_EQ(list_size(list_), 0);
}

/**
 * @brief Given we have a list with several elements, when we get its size,
 * then a non zero value must be returned.
 */
TEST_F(ListTest, GetListSizeWithNotEmptyList) {
  {
    auto t1 = create_task(nullptr, nullptr);
    auto t2 = create_task(nullptr, nullptr);
    auto t3 = create_task(nullptr, nullptr);
    auto t4 = create_task(nullptr, nullptr);
    auto t5 = create_task(nullptr, nullptr);

    insert(list_, t1);
    insert(list_, t2);
    insert(list_, t3);
    insert(list_, t4);
    insert(list_, t5);
  }

  EXPECT_EQ(list_size(list_), 5);
}

/**
 * @brief Given we have a null list, when we try to get its size, then 0 must
 * be returned.
 */
TEST_F(ListTest, GetListSizeWithNullList) {
  EXPECT_EQ(list_size(nullptr), 0);
}

/**
 * @brief Given we have a not empty list, when we add several elements into it,
 * then its size must be increased.
 */
TEST_F(ListTest, InsertElementWithNotEmptyList) {
  {
    auto t1 = create_task(nullptr, nullptr);
    auto t2 = create_task(nullptr, nullptr);

    insert(list_, t1);
    insert(list_, t2);

    EXPECT_EQ(list_size(list_), 2);
  }

  auto t3 = create_task(nullptr, nullptr);

  insert(list_, t3);

  EXPECT_EQ(list_size(list_), 3);
}

/**
 * @brief Given we have an empty list, when we try to add a null elements into
 * it, then its size must remain 0.
 */
TEST_F(ListTest, InsertNullElementWithEmptyList) {
  EXPECT_EQ(list_size(list_), 0);
  insert(list_, nullptr);
  EXPECT_EQ(list_size(list_), 0);
}

/**
 * @brief Given we have a not empty list, when we try to add a null element
 * into it, then its size must remain the same.
 */
TEST_F(ListTest, InsertNullElementWithNotEmptyList) {
  {
    auto t1 = create_task(nullptr, nullptr);
    auto t2 = create_task(nullptr, nullptr);

    insert(list_, t1);
    insert(list_, t2);
  }

  EXPECT_EQ(list_size(list_), 2);
  insert(list_, nullptr);
  EXPECT_EQ(list_size(list_), 2);
}

/**
 * @brief Given we have a null list, when we try to add an element to it,
 * then nothing must happen.
 */
TEST_F(ListTest, InsertElementWithNullList) {
  auto t1 = create_task(nullptr, nullptr);

  insert(nullptr, t1);
  free(t1);
}

/**
 * @brief Given we have a not empty list, when we try to remove multiple
 * elements from it, then they must be removed from list with success.
 */
TEST_F(ListTest, RemoveMultipleElementsWithNotEmptyList) {
  auto t1 = create_task(nullptr, nullptr);
  auto t2 = create_task(nullptr, nullptr);
  auto t3 = create_task(nullptr, nullptr);
  auto t4 = create_task(nullptr, nullptr);

  insert(list_, t1);
  insert(list_, t2);
  insert(list_, t3);
  insert(list_, t4);

  EXPECT_EQ(list_size(list_), 4);

  {
    EXPECT_TRUE(has_task(list_, t1));
    remove_task(list_, t1);
    EXPECT_FALSE(has_task(list_, t1));

    free(t1);
  }

  {
    EXPECT_TRUE(has_task(list_, t2));
    remove_task(list_, t2);
    EXPECT_FALSE(has_task(list_, t2));

    free(t2);
  }

  {
    EXPECT_TRUE(has_task(list_, t3));
    remove_task(list_, t3);
    EXPECT_FALSE(has_task(list_, t3));

    free(t3);
  }

  {
    EXPECT_TRUE(has_task(list_, t4));
    remove_task(list_, t4);
    EXPECT_FALSE(has_task(list_, t4));

    free(t4);
  }
}

/**
 * @brief Given we have a null list, when we try to remove an element from
 * it, then null nothing must happen.
 */
TEST_F(ListTest, RemoveElementWithNullList) {
  auto t = create_task(nullptr, nullptr);

  remove_task(nullptr, t);
  // cleanup
  free(t);
}

/**
 * @brief Given we have a null list, when we try to check if a given task
 * exists, then false must be returned.
 */
TEST_F(ListTest, HasTaskWithNullList) {
  auto t = create_task(nullptr, nullptr);

  EXPECT_FALSE(has_task(nullptr, t));

  // cleanup
  free(t);
}

/**
 * @brief Given we have a list and a null task, when we try to check if the
 * task exists on the list, then false must be returned.
 */
TEST_F(ListTest, HasTaskWithNullTask) {
  EXPECT_FALSE(has_task(list_, nullptr));
}

/**
 * @brief Given we have a list and a not enlistd task, when we try to check if
 * the task exists on the list, then false must be returned.
 */
TEST_F(ListTest, HasTaskWithNotInsertdTask) {
  auto t1 = create_task(nullptr, nullptr);
  auto t2 = create_task(nullptr, nullptr);

  insert(list_, t1);

  EXPECT_FALSE(has_task(list_, t2));

  // cleanup
  free(t2);
}

/**
 * @brief Given we have a list and a listd task, when we try to check if the
 * task exists on the list, then true must be returned.
 */
TEST_F(ListTest, HasTaskWithInsertdTask) {
  auto t1 = create_task(nullptr, nullptr);

  insert(list_, t1);

  EXPECT_TRUE(has_task(list_, t1));
}

/**
 * @brief Given we have a list and multiple listd tasks, when we try to check
 * if these tasks exists on the list, then true must be returned.
 */
TEST_F(ListTest, HasTaskWithMultipleInsertdTasks) {
  auto t1 = create_task(nullptr, nullptr);
  auto t2 = create_task(nullptr, nullptr);
  auto t3 = create_task(nullptr, nullptr);

  insert(list_, t1);
  insert(list_, t2);
  insert(list_, t3);

  EXPECT_TRUE(has_task(list_, t1));
  EXPECT_TRUE(has_task(list_, t2));
  EXPECT_TRUE(has_task(list_, t3));
}

/**
 * @brief Given we have a list, multiple listd tasks and multiple not listd
 * tasks, when we try to check if the listd and not listd tasks exists on the
 * list, then true must be returned for the first group and false must be
 * returned for the second one.
 */
TEST_F(ListTest, HasTaskWithMultipleNotInsertdTasks) {
  auto t1 = create_task(nullptr, nullptr);
  auto t2 = create_task(nullptr, nullptr);
  auto t3 = create_task(nullptr, nullptr);
  auto t4 = create_task(nullptr, nullptr);
  auto t5 = create_task(nullptr, nullptr);

  insert(list_, t1);
  insert(list_, t2);
  insert(list_, t3);

  EXPECT_TRUE(has_task(list_, t1));
  EXPECT_TRUE(has_task(list_, t2));
  EXPECT_TRUE(has_task(list_, t3));
  EXPECT_FALSE(has_task(list_, t4));
  EXPECT_FALSE(has_task(list_, t5));

  // cleanup
  free(t4);
  free(t5);
}

/**
 * @brief
 */
TEST_F(ListTest, RemoveTaskWithNullTask) {
  EXPECT_EQ(list_size(list_), 0);
  remove_task(list_, nullptr);
  EXPECT_EQ(list_size(list_), 0);
}

/**
 * @brief Given we have a list with a single element, when we try to remove it,
 * then the task must be removed with success.
 */
TEST_F(ListTest, RemoveTaskWithSingleElement) {
  auto t = create_task(nullptr, nullptr);

  insert(list_, t);

  EXPECT_TRUE(has_task(list_, t));
  remove_task(list_, t);
  EXPECT_FALSE(has_task(list_, t));

  // cleanup
  free(t);
}

/**
 * @brief
 */
TEST_F(ListTest, RemoveTaskAtMiddleWithMultipleElements) {
  auto t1 = create_task(nullptr, nullptr);
  auto t2 = create_task(nullptr, nullptr);
  auto t3 = create_task(nullptr, nullptr);

  insert(list_, t1);
  insert(list_, t2);
  insert(list_, t3);

  EXPECT_TRUE(has_task(list_, t1));
  EXPECT_TRUE(has_task(list_, t2));
  EXPECT_TRUE(has_task(list_, t3));

  remove_task(list_, t2);

  EXPECT_TRUE(has_task(list_, t1));
  EXPECT_FALSE(has_task(list_, t2));
  EXPECT_TRUE(has_task(list_, t3));

  // cleanup
  free(t2);
}

/**
 * @brief
 */
TEST_F(ListTest, RemoveTaskAtStartWithMultipleElements) {
  auto t1 = create_task(nullptr, nullptr);
  auto t2 = create_task(nullptr, nullptr);
  auto t3 = create_task(nullptr, nullptr);

  insert(list_, t1);
  insert(list_, t2);
  insert(list_, t3);

  EXPECT_TRUE(has_task(list_, t1));
  EXPECT_TRUE(has_task(list_, t2));
  EXPECT_TRUE(has_task(list_, t3));

  remove_task(list_, t1);

  EXPECT_FALSE(has_task(list_, t1));
  EXPECT_TRUE(has_task(list_, t2));
  EXPECT_TRUE(has_task(list_, t3));

  // cleanup
  free(t1);
}

/**
 * @brief
 */
TEST_F(ListTest, RemoveTaskAtEndWithMultipleElements) {
  auto t1 = create_task(nullptr, nullptr);
  auto t2 = create_task(nullptr, nullptr);
  auto t3 = create_task(nullptr, nullptr);

  insert(list_, t1);
  insert(list_, t2);
  insert(list_, t3);

  EXPECT_TRUE(has_task(list_, t1));
  EXPECT_TRUE(has_task(list_, t2));
  EXPECT_TRUE(has_task(list_, t3));

  remove_task(list_, t3);

  EXPECT_TRUE(has_task(list_, t1));
  EXPECT_TRUE(has_task(list_, t2));
  EXPECT_FALSE(has_task(list_, t3));

  // cleanup
  free(t3);
}
