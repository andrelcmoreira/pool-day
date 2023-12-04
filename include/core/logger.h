/**
 * @file
 *
 * @brief Contains the logging functions of the library
 */
#ifndef LOGGER_H_
#define LOGGER_H_

#ifdef LIB_LOGGING
#define POOL_DAY_LOG(...)   log_info(__VA_ARGS__)
#define POOL_DAY_ERROR(...) log_error(__VA_ARGS__)
#else
#define POOL_DAY_LOG(...)
#define POOL_DAY_ERROR(...)
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
