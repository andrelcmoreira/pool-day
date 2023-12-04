/**
 * @file
 *
 * @brief Contains the logging functions of the library
 */
#ifndef LOG_H_
#define LOG_H_

#include <stdio.h>

#ifdef LOGGING
#define POOL_DAY_LOG(...) \
  fprintf(stdout, "%s:%d | %s | INFO: ", __FILE__, __LINE__, __func__); \
  fprintf(stdout, __VA_ARGS__);
#define POOL_DAY_ERROR(...) \
  fprintf(stderr, "%s:%d | %s | ERROR: ", __FILE__, __LINE__, __func__); \
  fprintf(stderr, __VA_ARGS__);
#else
#define POOL_DAY_LOG(...)
#define POOL_DAY_ERROR(...)
#endif  // LOGGING

#endif  // LOG_H_
