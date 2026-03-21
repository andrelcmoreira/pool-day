/**
 * @file
 *
 * @brief Contains the definitions of task structure and helper methods and
 * macros to handle it.
 *
 * @authors
 * Copyright (C) 2023 André L. C. Moreira <andrelcmoreira@proton.me>
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
#ifndef TASK_H_
#define TASK_H_

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus

typedef struct task *task_t; //!< Handle to the task.

/**
 * @brief Create a new synchronous task.
 *
 * @note The created task requires a manual release once it's bound to a
 * pool.
 *
 * @param[in] id Task identifier.
 * @param[in] task Task callback.
 * @param[in] param Task parameter.
 * @param[in] param_size Size of the task parameter.
 *
 * @return Handle to the new task.
 */
task_t create_sync_task(uint32_t id, void *(*task)(void *), void *param,
                        size_t param_size);

/**
 * @brief Create a new asynchronous task.
 *
 * @note When the 'auto_release' parameter is set to false, the created task
 * requires a manual release once it's bound to a pool.
 *
 * @param[in] id Task identifier.
 * @param[in] task Task callback.
 * @param[in] param Task parameter.
 * @param[in] param_size Size of the task parameter.
 * @param[in] auto_release Flag indicating whether the task must be released
 * after its execution.
 * @param[in] start_cb Callback executed when the task starts.
 * @param[in] end_cb Callback executed when the task ends.
 *
 * @return Handle to the new task.
 */
task_t create_async_task(uint32_t id, void *(*task)(void *), void *param,
                         size_t param_size, bool auto_release,
                         void (*start_cb)(uint32_t, const void *),
                         void (*end_cb)(uint32_t, const void *, void *));

/**
 * @brief Destroy a task.
 *
 * @param[in] task Handle to the task to be destroyed.
 */
void destroy_task(task_t task);

#ifdef __cplusplus
}
#endif  // __cplusplus

#endif  // TASK_H_
