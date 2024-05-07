/**
 * @file
 *
 * @brief Contains the basic definitions and data structures of the library.
 */
#ifndef POOL_DAY_H_
#define POOL_DAY_H_

#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus

#include "core/errors.h"
#include "core/queue.h"

typedef struct pool_day *pool_day_t;  //!< Handle to the pool.

/**
 * @brief Enqueue a new task into the pool.
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
 * @brief Get the number of finished tasks.
 *
 * @param[in] pool Handle to the thread pool.
 *
 * @return The number of finished tasks.
 */
uint32_t finished_tasks(pool_day_t pool);

/**
 * @brief Wait for the finish of a given task.
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
