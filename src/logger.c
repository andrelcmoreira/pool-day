#include "core/logger.h"

#include <stdarg.h>
#include <stdio.h>
#include <pthread.h>

#include "core/thread_utils.h"

#define MAX_LOG_BUFFER  1024

static pthread_mutex_t log_mutex;  //!< Mutex for log operations.

void __attribute__((unused)) log_info(const char *fmt, ...) {
  char log_buffer[MAX_LOG_BUFFER] = {0};
  va_list args;

  THREAD_SAFE_ZONE(&log_mutex, {
    va_start(args, fmt);

    vsnprintf(log_buffer, MAX_LOG_BUFFER, fmt, args);
    fprintf(stdout, "%s\n", log_buffer);

    va_end(args);
  });
}

void __attribute__((unused)) log_error(const char *fmt, ...) {
  char log_buffer[MAX_LOG_BUFFER] = {0};
  va_list args;

  THREAD_SAFE_ZONE(&log_mutex, {
    va_start(args, fmt);

    vsnprintf(log_buffer, MAX_LOG_BUFFER, fmt, args);
    fprintf(stderr, "%s\n", log_buffer);

    va_end(args);
  });
}
