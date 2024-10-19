/**
 * @file
 *
 * @brief Contains the basic definitions and data structures of the task queue.
 *
 * @authors
 * Copyright (C) 2023 André L. C. Moreira <andrelcmoreira@disroot.org>
 *
 * @copyright
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program. If not, see <https://www.gnu.org/licenses/>.
 */
#ifndef QUEUE_H_
#define QUEUE_H_

#include <stdbool.h>
#include <stdint.h>

typedef struct task task_t; //!< Structure representing an item on the task
                            // queue.
typedef struct task_queue task_queue_t; //!< Structure representing the queue
                                        // itself.

/**
 * @brief Get the queue size.
 *
 * @note This function is thread-safe.
 *
 * @param[in] queue Pointer to the queue.
 *
 * @return The size of the queue.
 */
uint32_t queue_size(task_queue_t *queue);

/**
 * @brief Enqueue a new task into the queue.
 *
 * @note This function is thread-safe.
 *
 * @param[in,out] queue Pointer to the queue.
 * @param[in]     task Pointer to the task to be enqueued.
 */
void enqueue(task_queue_t *queue, task_t *task);

/**
 * @brief Dequeue a task from the queue.
 *
 * @note This function is thread-safe.
 *
 * @param[in] queue Pointer to the queue.
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
 * @note This function is thread-safe.
 *
 * @param[in,out] queue Pointer to the queue.
 */
void destroy_queue(task_queue_t *queue);

#endif  // QUEUE_H_
