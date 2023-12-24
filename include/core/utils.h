/**
 * @file
 *
 * @brief Contains a set of utilities available for use by the library.
 */
#ifndef UTILS_H_
#define UTILS_H_

#define THREAD_SAFE_ZONE(mutex, code)  \
  pthread_mutex_lock(mutex); \
  code \
  pthread_mutex_unlock(mutex);

#ifndef UNIT_TESTS
#define STATIC static
#else
#define STATIC
#endif  // UNIT_TESTS

#endif  // UTILS_H_
