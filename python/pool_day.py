from contextlib import contextmanager
from ctypes import byref
from time import sleep

from pool_day import _pd_handle, pool_day_callback


@pool_day_callback
def thread_cb(param):
    for i in range(1, 10):
        print('thread %d: hello from python callback, i: %d' % (param, i))
        sleep(param)


@contextmanager
def create_pool(size):
    pool = _pd_handle.create_pool(size)
    yield pool
    _pd_handle.destroy_pool(byref(pool))


def destroy_pool(pool):
    return _pd_handle.destroy_pool(byref(pool))


def enqueue_task(pool, task):
    return _pd_handle.enqueue_task(pool, task)


def create_task(cb, param):
    return _pd_handle.create_task(cb, param)


def abort_tasks(pool):
    return _pd_handle.abort_tasks(pool)


def queued_tasks(pool):
    return _pd_handle.queued_tasks(pool)


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
