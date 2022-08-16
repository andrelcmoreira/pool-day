#ifndef QUEUE_H_
#define QUEUE_H_

#define for_each_task(curr, queue) \
  for (struct task_node_t *curr = queue->head; curr; curr = curr->prev)

#define for_each_task_safe(curr, queue) \
  for (struct task_node_t *curr = queue->head, *tmp = curr ? curr->prev : NULL; \
    curr; \
    curr = tmp, tmp = tmp ? tmp->prev : NULL)

/**
 * @brief Structure representing an item on the task queue.
 */
struct task_node_t {
  struct task_node_t *next;
  struct task_node_t *prev;
  void (*task)(void *);
  void *param;
};

typedef struct task_node_t task_node_t;
typedef struct task_queue_t task_queue_t;

/**
 * @brief Get the queue size.
 *
 * @param queue Pointer to the queue.
 *
 * @return The size of the queue.
 */
int queue_size(task_queue_t *queue);

/**
 * @brief Create a new task.
 *
 * @param task Task callback.
 * @param param Task parameter.
 *
 * @return Pointer to the new task.
 */
struct task_node_t *create_task(void (*task)(void *), void *param);

/**
 * @brief Enqueue a new task into the queue.
 *
 * @param queue Pointer to the queue.
 * @param task Pointer to the task to be enqueued.
 */
void enqueue(task_queue_t *queue, task_node_t *task);

/**
 * @brief Dequeue a task from the queue.
 *
 * @param queue Pointer to the queue.
 *
 * @return The head task.
 */
struct task_node_t *dequeue(task_queue_t *queue);

/**
 * @brief Initialize the queue.
 *
 * @param queue Pointer to the queue.
 */
void init_queue(task_queue_t **queue);

/**
 * @brief Destroy the queue.
 *
 * @param queue Pointer to the queue.
 */
void destroy_queue(task_queue_t *queue);

#endif  // QUEUE_H_
