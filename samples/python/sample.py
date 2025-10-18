from time import sleep

from pool_day import (
    create_pool,
    create_task,
    enqueue_task,
    pool_day_callback,
    queued_tasks
)


@pool_day_callback
def thread_cb(param):
    for i in range(1, 10):
        print('thread %d: hello from python callback, i: %d' % (param, i))
        sleep(param)


def main():
    with create_pool(2) as pool:
        t1 = create_task(thread_cb, 1)
        t2 = create_task(thread_cb, 2)
        t3 = create_task(thread_cb, 3)

        print('enqueue_task ret =', enqueue_task(pool, t1))
        print('enqueue_task ret =', enqueue_task(pool, t2))
        print('enqueue_task ret =', enqueue_task(pool, t3))
        print('queued_tasks ret =', queued_tasks(pool))
        input('')


if __name__ == "__main__":
    main()
