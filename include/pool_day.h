/**
 * @file
 *
 * @brief Contains the basic definitions and data structures of the library.
 */
#ifndef POOL_DAY_H_
#define POOL_DAY_H_

#include <stdint.h>

#include "queue.h"

typedef struct pool_day_t *pool_day_t;  //!< Handle to the pool.

/**
 * @brief Enqueue a new task into the pool.
 *
 * @param pool Handle to the thread pool.
 * @param task The task to be enqueued.
 */
void enqueue_task(pool_day_t pool, task_t *task);

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
 */
void destroy_pool(pool_day_t *pool);

#endif  // POOL_DAY_H_
