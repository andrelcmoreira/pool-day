#include "core/logger.h"

#include <pthread.h>

// TODO(andrelcmoreira) thread-safe logging.

void __attribute__((unused)) log_info(const char *fmt, ...) {
  (void)fmt;
}

void __attribute__((unused)) log_error(const char *fmt, ...) {
  (void)fmt;
}
