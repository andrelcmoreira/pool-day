/**
 * @file
 *
 * @brief
 */
#ifndef THREAD_UTILS_H_
#define THREAD_UTILS_H_

#define THREAD_SAFE_ZONE(mutex, code)  \
  pthread_mutex_lock(mutex); \
  code \
  pthread_mutex_unlock(mutex);

#endif  // THREAD_UTILS_H_
