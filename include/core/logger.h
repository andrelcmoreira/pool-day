/**
 * @file
 *
 * @brief Contains the logging functions of the library.
 */
#ifndef LOGGER_H_
#define LOGGER_H_

#define _STR(x) #x
#define STR(x) _STR(x)

#define LOG_PREFIX \
  __FILE__ ":" STR(__LINE__) " | " STR(__func__)

#define LOG_TAG_INFO "INFO"
#define LOG_TAG_ERROR "ERROR"

#ifdef LIB_LOGGING
#define POOL_DAY_LOG(fmt, ...) \
  log_info(LOG_PREFIX " | " LOG_TAG_INFO ": "fmt)
#define POOL_DAY_ERROR(fmt, ...) \
  log_error(LOG_PREFIX " | " LOG_TAG_ERROR ": "fmt)
#else
#define POOL_DAY_LOG(fmt, ...)
#define POOL_DAY_ERROR(fmt, ...)
#endif  // LIB_LOGGING

/**
 * @brief Log a message to the screen with 'INFO' severity.
 *
 * @param fmt Message format to be logged.
 */
void log_info(const char *fmt, ...);

/**
 * @brief Log a message to the screen with 'ERROR' severity.
 *
 * @param fmt Message format to be logged.
 */
void log_error(const char *fmt, ...);

#endif  // LOGGER_H_
