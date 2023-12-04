/**
 * @file
 *
 * @brief Contains the logging functions of the library
 */
#ifndef LOG_H_
#define LOG_H_

#ifdef LIB_LOGGING
#define POOL_DAY_LOG(...) \
  fprintf(stdout, "%s:%d | %s | INFO: "__VA_ARGS__, __FILE__, __LINE__, \
          __FUNCTION__)
#define POOL_DAY_ERROR(...) \
  fprintf(stderr, "%s:%d | %s | ERROR: "__VA_ARGS__, __FILE__, __LINE__, \
          __FUNCTION__)
#else
#define POOL_DAY_LOG(...)
#define POOL_DAY_ERROR(...)
#endif  // LIB_LOGGING

#endif  // LOG_H_
