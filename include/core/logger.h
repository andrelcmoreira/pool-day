/**
 * @file
 *
 * @brief Contains the logging functions of the library.
 */
#ifndef LOGGER_H_
#define LOGGER_H_

#include <stdio.h>
#include <string.h>

/**
 * @brief Log severity.
 */
typedef enum {
  POOL_DAY_LOG_INFO,
  POOL_DAY_LOG_ERROR
} pool_day_log_severity_t;

#define FILENAME strrchr(__FILE__, '/') + 1

#ifdef LIB_LOGGING
#define POOL_DAY_LOG(...) \
  __log_msg(POOL_DAY_LOG_INFO, FILENAME, __func__, __LINE__, __VA_ARGS__)
#define POOL_DAY_ERROR(...) \
  __log_msg(POOL_DAY_LOG_ERROR, FILENAME, __func__, __LINE__, __VA_ARGS__)
#else
#define POOL_DAY_LOG(...)
#define POOL_DAY_ERROR(...)
#endif  // LIB_LOGGING

/**
 * @brief Log a message to the screen.
 *
 * @note This function is thread safe.
 *
 * @param sev Severity of log message.
 * @param file_name File name where the log function is being called.
 * @param func_name Function name where the log function is being called.
 * @param line_no Line number where the log function is being caled.
 * @param fmt Message format to be logged.
 */
void __log_msg(pool_day_log_severity_t sev, char *file_name,
               const char *func_name, int line_no, const char *fmt, ...);

#endif  // LOGGER_H_
