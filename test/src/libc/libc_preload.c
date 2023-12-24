#include <pthread.h>
#include <semaphore.h>

int pthread_create(pthread_t *restrict thread,
                   const pthread_attr_t *restrict attr,
                   void *(*start_routine)(void *),
                   void *restrict arg) {
  (void)thread;
  (void)attr;
  (void)start_routine;
  (void)arg;

  return 0;
}

int pthread_join(pthread_t thread, void **retval) {
  (void)thread;
  (void)retval;

  return 0;
}

int sem_wait(sem_t *sem) {
  (void)sem;

  return 0;
}

int sem_post(sem_t *sem) {
  (void)sem;

  return 0;
}

int sem_destroy(sem_t *sem) {
  (void)sem;

  return 0;
}
