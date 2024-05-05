/**
 * @file
 *
 * @brief Contains the basic definitions and data structures of the task queue.
 */
#ifndef QUEUE_H_
#define QUEUE_H_

#include <stdbool.h>
#include <stdint.h>

/**
 * @brief For-each macro implementation.
 *
 * @details It can be used to iterate over the queue.
 */
#define for_each_task(curr, queue) \
  for (struct task *curr = queue->head; curr; curr = curr->prev)

/**
 * @brief Safe implementation of for-each macro.
 *
 * @details It can be used to delete elements of the queue while iterating over
 * it.
 */
#define for_each_task_safe(curr, queue) \
  for (struct task *curr = queue->head, *tmp = curr ? curr->prev : NULL; \
    curr; \
    curr = tmp, tmp = tmp ? tmp->prev : NULL)

/**
 * @brief Pool task definition.
 */
struct task {
  struct task *next;     //!< Next element of the current instance.
  struct task *prev;     //!< Previous element of the current instance.
  void *(*task)(void *); //!< Task callback.
  void *param;           //!< Parameter of the task callback.
  void *ret_val;         //!< Task return value.
};

typedef struct task task_t; //!< Structure representing an item on the task
                            // queue.
typedef struct task_queue task_queue_t; //!< Structure representing the queue
                                        // itself.

/**
 * @brief Get the queue size.
 *
 * @param[in] queue Pointer to the queue.
 *
 * @return The size of the queue.
 */
uint8_t queue_size(task_queue_t *queue);

/**
 * @brief Create a new task.
 *
 * @note The created task doesn't require a manual release once it's bound to a
 * pool. Otherwise it can be released using the function destroy_task.
 *
 * @param[in] task Task callback.
 * @param[in] param Task parameter.
 *
 * @return Pointer to the new task.
 */
task_t *create_task(void *(*task)(void *), void *param);

/**
 * @brief Destroy a given task.
 *
 * @param[in] task Task to be destroyed.
 */
void destroy_task(task_t *task);

/**
 * @brief Enqueue a new task into the queue.
 *
 * @param[in,out] queue Pointer to the queue.
 * @param[in]     task Pointer to the task to be enqueued.
 */
void enqueue(task_queue_t *queue, task_t *task);

/**
 * @brief Check if a given task is present on the queue.
 *
 * @param[in] queue Pointer to the queue.
 * @param[in] task Pointer to the task to be checked.
 *
 * @return True if the queue has the given task; otherwise False.
 */
bool has_task(task_queue_t *queue, task_t *task);

/**
 * @brief Dequeue a task from the queue.
 *
 * @param[in,out] queue Pointer to the queue.
 *
 * @return The head task.
 */
task_t *dequeue(task_queue_t *queue);

/**
 * @brief Initialize the queue.
 *
 * @param[out] queue Pointer to the queue.
 */
void init_queue(task_queue_t **queue);

/**
 * @brief Destroy the queue.
 *
 * @param[in,out] queue Pointer to the queue.
 */
void destroy_queue(task_queue_t *queue);

#endif  // QUEUE_H_
