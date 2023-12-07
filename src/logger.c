#include "core/logger.h"

#include <stdarg.h>
#include <stdio.h>
#include <pthread.h>

#include "core/utils.h"

static pthread_mutex_t log_mutex;  //!< Mutex for log operations.

void __attribute__((unused)) log_msg(log_severity_t sev, const char *fmt, ...) {
  va_list args;
  FILE *out = (sev == LOG_SEVERITY_INFO) ? stdout : stderr;

  THREAD_SAFE_ZONE(&log_mutex, {
    va_start(args, fmt);
    vfprintf(out, fmt, args);
    va_end(args);
  });
}
