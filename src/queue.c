#include <pthread.h>
#include <stdlib.h>

#include "queue.h"

struct task_queue_t {
  task_node_t *tail;
  task_node_t *head;
  pthread_mutex_t mutex;
};

int queue_size(task_queue_t *queue) {
  int size = 0;

  pthread_mutex_lock(&queue->mutex);

  for_each_task(curr, queue) {
    size++;
  }

  pthread_mutex_unlock(&queue->mutex);

  return size;
}

task_node_t *create_task(void (*task)(void *), void *param) {
  struct task_node_t *node = NULL;

  node = (task_node_t *)malloc(sizeof(task_node_t));
  if (node) {
    node->next = node->prev = NULL;
    node->task = task;
    node->param = param;
  }

  return node;
}

void enqueue(task_queue_t *queue, task_node_t *elem) {
  pthread_mutex_lock(&queue->mutex);

  /* if the queue is empty */
  if (!queue->head && !queue->tail) {
    queue->tail = queue->head = elem;
  } else {
    queue->tail->prev = elem;
    elem->next = queue->tail;
    queue->tail = elem;
  }

  pthread_mutex_unlock(&queue->mutex);
}

struct task_node_t *dequeue(task_queue_t *queue) {
  struct task_node_t *to_del = queue->head;

  pthread_mutex_lock(&queue->mutex);

  /* if the list is not empty */
  if (to_del) {
    queue->head = to_del->prev;
    /* if the queue has more than one element */
    if (to_del->prev) {
      to_del->prev->next = NULL;
      to_del->prev = NULL;
    } else {
      queue->head = queue->tail = NULL;
    }
  }

  pthread_mutex_unlock(&queue->mutex);

  return to_del;
}

void init_queue(task_queue_t **queue) {
  *queue = malloc(sizeof(task_queue_t));
  if (*queue) {
    (*queue)->head = (*queue)->tail = NULL;
  }

  pthread_mutex_init(&(*queue)->mutex, NULL);
}

void destroy_queue(task_queue_t *queue) {
  for_each_task_safe(curr, queue) {
    struct task_node_t *node = dequeue(queue);

    free(node);
  }

  pthread_mutex_destroy(&queue->mutex);
  free(queue);
}
