#include "internal/logger.h"

#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <pthread.h>

#include "internal/utils.h"

#define GET_LOG_FILE(sev)    (sev == POOL_DAY_INFO_INFO) ? stdout : stderr
#define GET_LOG_TAG(sev)     (sev == POOL_DAY_INFO_INFO) ? "INFO" : "ERROR"
#define GET_FILE_NAME(path)  strrchr(path, '/') + 1

#define LOG_FMT           "%s:%d\t| %s\t| %s:"
#define LOG_BUFFER_SIZE   4096

static pthread_mutex_t log_mutex;  //!< Mutex for log operations.

void __log_msg(pool_day_log_severity_t sev, const char *file_name,
               const char *func_name, int line_no, const char *fmt, ...) {
  char log_buffer[LOG_BUFFER_SIZE] = {0};
  va_list args;

  THREAD_SAFE_ZONE(&log_mutex, {
    va_start(args, fmt);

    vsnprintf(log_buffer, LOG_BUFFER_SIZE, fmt, args);
    fprintf(GET_LOG_FILE(sev), LOG_FMT " %s\n", GET_FILE_NAME(file_name),
            line_no, func_name, GET_LOG_TAG(sev), log_buffer);

    va_end(args);
  });
}
