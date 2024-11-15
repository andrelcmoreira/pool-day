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

/**
 * @brief For-each macro implementation.
 *
 * @details It can be used to iterate over the list.
 */
#define for_each_task(curr, list) \
  for (struct task *curr = list->head; curr; curr = curr->prev)

/**
 * @brief Safe implementation of for-each macro.
 *
 * @details It can be used to delete elements of the list while iterating over
 * it.
 */
#define for_each_task_safe(curr, list) \
  for (struct task *curr = list->head, *tmp = curr ? curr->prev : NULL; \
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
  sem_t ready;           //!< Task's semaphore.
};

typedef struct task task_t; //!< Structure representing an item on the task
                            // list.
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

#endif  // TASK_H_
