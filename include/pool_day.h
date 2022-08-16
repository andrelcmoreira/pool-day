#ifndef POOL_DAY_H_
#define POOL_DAY_H_

#include <stdint.h>

#include "queue.h"

/*
 * @brief Handle to the pool.
 */
typedef struct pool_day_t *pool_day_t;

/**
 * @brief Enqueue a new task into the pool.
 *
 * @param pool Handle to the thread pool.
 * @param t The task to be enqueued.
 */
void enqueue_task(pool_day_t pool, task_node_t *t);

/**
 * @brief Create a new pool.
 *
 * @param pool_size Size of the pool.
 * @return The pool handle.
 */
pool_day_t create_pool(const uint8_t pool_size);

/**
 * @brief Destroy the pool.
 *
 * @param pool Pointer to the pool handle.
 */
void destroy_pool(pool_day_t *pool);

#endif  // POOL_DAY_H_
