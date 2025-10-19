from time import sleep

from pool_day import create_pool, create_task, pool_day_callback


@pool_day_callback
def thread_cb(param):
    for i in range(1, 10):
        print(f'thread {param}: hello from python callback, i: {i}')
        sleep(param)

    return f'task {param} done'


def main():
    with create_pool(2) as pool:
        t1 = create_task(thread_cb, 1)
        t2 = create_task(thread_cb, 2)
        t3 = create_task(thread_cb, 3)

        print('enqueue_task ret =', pool.enqueue_task(t1))
        print('enqueue_task ret =', pool.enqueue_task(t2))
        print('enqueue_task ret =', pool.enqueue_task(t3))
        print('queued_tasks ret =', pool.queued_tasks())

        ret = pool.wait_task_finish(t3)
        print(ret)


if __name__ == "__main__":
    main()
