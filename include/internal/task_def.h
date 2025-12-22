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
#ifndef TASK_DEF_H_
#define TASK_DEF_H_

#include <semaphore.h>
#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus

/**
 * @brief Pool task definition.
 */
struct task {
  uint32_t id;                                    //!< Task identifier.
  bool is_orphan;                                 //!< Flag indicating whether the task is orphaned.
  bool auto_release;                              //!< Flag indicating whether the task must be released after its execution.
  struct task *next;                              //!< Next element of the current instance.
  struct task *prev;                              //!< Previous element of the current instance.
  void *(*task)(void *);                          //!< Task callback.
  void *param;                                    //!< Parameter of the task callback.
  void *ret_val;                                  //!< Task return value.
  void (*on_task_start)(uint32_t, void *);        //!< Callback executed when the task starts.
  void (*on_task_end)(uint32_t, void *, void *);  //!< Callback executed when the task ends.
  sem_t ready;                                    //!< Task's semaphore.
};

typedef struct task *task_t; //!< Structure representing an item on the task queue.

#ifdef __cplusplus
}
#endif  // __cplusplus

#endif  // TASK_DEF_H_
