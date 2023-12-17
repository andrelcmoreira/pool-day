#include "core/logger.h"

#include <stdarg.h>
#include <stdio.h>
#include <pthread.h>

#include "core/utils.h"

static pthread_mutex_t log_mutex;  //!< Mutex for log operations.

void log_msg(const char *fmt, ...) {
  va_list args;

  THREAD_SAFE_ZONE(&log_mutex, {
    va_start(args, fmt);
    vprintf(fmt, args);
    va_end(args);
  });
}
