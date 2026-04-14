from time import sleep

from pool_day import create_pool, create_sync_task, pool_day_callback


@pool_day_callback
def thread_cb(param):
    for i in range(1, 10):
        print(f'thread {param}: hello from python callback, i: {i}')
        sleep(param)

    #return f'task {param} done'
    return param


def main():
    with create_pool(2) as pool:
        t1 = create_sync_task(thread_cb, 1)
        t2 = create_sync_task(thread_cb, 2)
        t3 = create_sync_task(thread_cb, 3)

        print('task 1 enqueued, ret =', pool.enqueue_task(t1))
        print('task 2 enqueued, ret =', pool.enqueue_task(t2))
        print('task 3 enqueued, ret =', pool.enqueue_task(t3))
        print('number of queued tasks =', pool.queued_tasks())

        ret = pool.get_task_result(t3)

        print(type(ret))
        print(ret)


if __name__ == "__main__":
    main()
