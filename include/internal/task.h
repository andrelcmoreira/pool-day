/**
 * @file
 *
 * @brief Contains the definitions of task structure and helper methods and
 * macros to handle it.
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
#ifndef TASK_H_
#define TASK_H_

#include <semaphore.h>
#include <stdbool.h>
#include <stdint.h>

/**
 * @brief Pool task definition.
 */
struct task {
  uint32_t id;                           //!< Task identifier.
  struct task *next;                     //!< Next element of the current instance.
  struct task *prev;                     //!< Previous element of the current instance.
  void *(*task)(void *);                 //!< Task callback.
  void *param;                           //!< Parameter of the task callback.
  void *ret_val;                         //!< Task return value.
  void (*on_task_start)(uint32_t);       //!< Callback executed when the task starts.
  void (*on_task_end)(uint32_t, void *); //!< Callback executed when the task ends.
  sem_t ready;                           //!< Task's semaphore.
};

typedef struct task task_t; //!< Structure representing an item on the task
                            // queue.
/**
 * @brief Create a new task.
 *
 * @note The created task doesn't require a manual release once it's bound to a
 * pool.
 *
 * @param[in] id Task identifier.
 * @param[in] task Task callback.
 * @param[in] param Task parameter.
 * @param[in] param_size Size of the task parameter.
 * @param[in] start_cb Callback executed when the task starts.
 * @param[in] end_cb Callback executed when the task ends.
 *
 * @return Pointer to the new task.
 */
task_t *create_task(uint32_t id, void *(*task)(void *), void *param,
                    size_t param_size, void (*start_cb)(uint32_t),
                    void (*end_cb)(uint32_t, void *));

/**
 * @brief Destroy a task.
 *
 * @param[in] task Pointer to the task to be destroyed.
 */
void destroy_task(task_t *task);

#endif  // TASK_H_
