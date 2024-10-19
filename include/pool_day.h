/**
 * @file
 *
 * @brief Contains the basic definitions and data structures of the library.
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
#ifndef POOL_DAY_H_
#define POOL_DAY_H_

#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus

#include "internal/errors.h"
#include "internal/task.h"

typedef struct pool_day *pool_day_t;  //!< Handle to the pool.

/**
 * @brief Enqueue a new task into the pool.
 *
 * @param[in,out] pool Handle to the thread pool.
 * @param[in]     task The task to be enqueued.
 *
 * @return POOL_DAY_SUCCESS on success; otherwise the suitable error code.
 */
pool_day_retcode_t enqueue_task(pool_day_t pool, task_t *task);

/**
 * @brief Create a new pool according to the specified size.
 *
 * @param[in] pool_size Size of the pool to be created.
 *
 * @return A handle to the created pool.
 */
pool_day_t create_pool(uint32_t pool_size);

/**
 * @brief Destroy the pool.
 *
 * @param[in,out] pool Pointer to the pool handle.
 *
 * @return POOL_DAY_SUCCESS on success; otherwise the suitable error code.
 */
pool_day_retcode_t destroy_pool(pool_day_t *pool);

/**
 * @brief Get the number of queued tasks, waiting for execution, in the queue.
 *
 * @param[in] pool Handle to the thread pool.
 *
 * @return The number of tasks in the queue.
 */
uint32_t queued_tasks(pool_day_t pool);

/**
 * @brief Wait for the finish of a given task.
 *
 * Sample:
 * @code{.c}
 * pool_day_t pool;
 *
 * pool = create_pool(size);
 * if (pool) {
 *   task_t *t = create_task(callback, (void *)param);
 *
 *   enqueue_task(pool, t);
 *
 *   void *ret = wait_task_finish(pool, t);
 *   destroy_pool(&pool);
 * }
 * @endcode
 *
 * @note This function blocks the current thread.
 * @note The task is released after the execution of this function.
 *
 * @param[in] pool Handle to the thread pool.
 * @param[in] task Task handler.
 *
 * @return The return value of the task.
 */
void *wait_task_finish(pool_day_t pool, task_t *task);

/**
 * @brief Abort the execution of incoming tasks.
 *
 * @param[out] pool Handle to the thread pool.
 *
 * @return POOL_DAY_SUCCESS on success; otherwise the suitable error code.
 */
pool_day_retcode_t abort_tasks(pool_day_t pool);

#ifdef __cplusplus
}
#endif  // __cplusplus

#endif  // POOL_DAY_H_
