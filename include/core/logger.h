/**
 * @file
 *
 * @brief Contains the logging functions of the library.
 */
#ifndef LOGGER_H_
#define LOGGER_H_

#define _STR(x) #x
#define STR(x) _STR(x)

#define LOG_HEADER \
  __FILE__ ":" STR(__LINE__) " | " STR(__func__)

#define LOG_TAG_INFO  "INFO"
#define LOG_TAG_ERROR "ERROR"

#ifdef LIB_LOGGING
#define POOL_DAY_LOG(...) \
  log_msg(LOG_SEVERITY_INFO, LOG_HEADER " | " LOG_TAG_INFO ": " __VA_ARGS__)
#define POOL_DAY_ERROR(...) \
  log_msg(LOG_SEVERITY_ERROR, LOG_HEADER " | " LOG_TAG_ERROR ": " __VA_ARGS__)
#else
#define POOL_DAY_LOG(...)
#define POOL_DAY_ERROR(...)
#endif  // LIB_LOGGING

/**
 * @brief Log severity definitions.
 */
typedef enum {
  LOG_SEVERITY_INFO,
  LOG_SEVERITY_ERROR
} log_severity_t;

/**
 * @brief Log a message to the screen.
 *
 * @note This function is thread safe.
 *
 * @param sev Log severity.
 * @param fmt Message format to be logged.
 */
void log_msg(log_severity_t sev, const char *fmt, ...);

#endif  // LOGGER_H_
