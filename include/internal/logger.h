/**
 * @file
 *
 * @brief Contains the logging functions of the library.
 *
 * @authors
 * Copyright (C) 2023 André L. C. Moreira <andrelcmoreira@disroot.org>
 *
 * @copyright
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program. If not, see <https://www.gnu.org/licenses/>.
 */
#ifndef LOGGER_H_
#define LOGGER_H_

/**
 * @brief Log severity.
 */
typedef enum {
  POOL_DAY_INFO_INFO,
  POOL_DAY_INFO_ERROR
} pool_day_log_severity_t;

#ifdef LIB_LOGGING
#define POOL_DAY_INFO(...) \
  __log_msg(POOL_DAY_INFO_INFO, __FILE__, __func__, __LINE__, __VA_ARGS__)
#define POOL_DAY_ERROR(...) \
  __log_msg(POOL_DAY_INFO_ERROR, __FILE__, __func__, __LINE__, __VA_ARGS__)
#else
#define POOL_DAY_INFO(...)
#define POOL_DAY_ERROR(...)
#endif  // LIB_LOGGING

/**
 * @brief Log a message to the screen.
 *
 * @note This function is thread safe.
 *
 * @param[in] sev Severity of log message.
 * @param[in] file_name File name where the log function is being called.
 * @param[in] func_name Function name where the log function is being called.
 * @param[in] line_no Line number where the log function is being caled.
 * @param[in] fmt Message format to be logged.
 */
void __log_msg(pool_day_log_severity_t sev, const char *file_name,
               const char *func_name, int line_no, const char *fmt, ...);

#endif  // LOGGER_H_
