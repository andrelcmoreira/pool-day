/**
 * @file
 *
 * @brief Contains the basic definitions and data structures of the task list.
 */
#ifndef LIST_H_
#define LIST_H_

#include <stdbool.h>
#include <stdint.h>

typedef struct task task_t; //!< Structure representing an item on the task
                            // list.
typedef struct task_list task_list_t; //!< Structure representing the list
                                      // itself.

/**
 * @brief Get the list size.
 *
 * @note This function is thread-safe.
 *
 * @param[in] list Pointer to the list.
 *
 * @return The size of the list.
 */
uint32_t list_size(task_list_t *list);

/**
 * @brief Remove a task from the list.
 *
 * @note This function is thread-safe.
 *
 * @param[in] list Pointer to the list.
 * @param[in] task Pointer to the task to be removed.
 */
void remove_task(task_list_t *list, task_t *task);

/**
 * @brief Enlist a new task into the list.
 *
 * @note This function is thread-safe.
 *
 * @param[in,out] list Pointer to the list.
 * @param[in]     task Pointer to the task to be enlistd.
 */
void insert_task(task_list_t *list, task_t *task);

/**
 * @brief Check if a given task is present on the list.
 *
 * @note This function is thread-safe.
 *
 * @param[in] list Pointer to the list.
 * @param[in] task Pointer to the task to be checked.
 *
 * @return True if the list has the given task; otherwise False.
 */
bool has_task(task_list_t *list, task_t *task);

/**
 * @brief Initialize the list.
 *
 * @param[out] list Pointer to the list.
 */
void init_list(task_list_t **list);

/**
 * @brief Destroy the list.
 *
 * @note This function is thread-safe.
 *
 * @param[in,out] list Pointer to the list.
 */
void destroy_list(task_list_t *list);

#endif  // LIST_H_
