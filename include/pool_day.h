/**
 * @file
 *
 * @brief Contains the basic definitions and data structures of the library.
 */
#ifndef POOL_DAY_H_
#define POOL_DAY_H_

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
 *   destroy_pool(&pool);
 * }
 * @endcode
 *
 * @param pool Handle to the thread pool.
 * @param task The task to be enqueued.
 *
 * @return POOL_DAY_SUCCESS on success; otherwise the suitable error code.
 */
pool_day_retcode_t enqueue_task(pool_day_t pool, task_t *task);

/**
 * @brief Create a new pool according to the specified size.
 *
 * @param pool_size Size of the pool to be created.
 *
 * @return A handle to the created pool.
 */
pool_day_t create_pool(uint8_t pool_size);

/**
 * @brief Destroy the pool.
 *
 * @param pool Pointer to the pool handle.
 *
 * @return POOL_DAY_SUCCESS on success; otherwise the suitable error code.
 */
pool_day_retcode_t destroy_pool(pool_day_t *pool);

/**
 * @brief Get the number of idle tasks, waiting for execution, in the queue.
 *
 * @param pool Handle to the thread pool.
 *
 * @return The number of tasks in the queue.
 */
uint8_t idle_tasks(pool_day_t pool);

/**
 * @brief Abort the execution of incoming tasks.
 *
 * @param pool Handle to the thread pool.
 *
 * @return POOL_DAY_SUCCESS on success; otherwise the suitable error code.
 */
pool_day_retcode_t abort_tasks(pool_day_t pool);

#ifdef __cplusplus
}
#endif  // __cplusplus

#endif  // POOL_DAY_H_
