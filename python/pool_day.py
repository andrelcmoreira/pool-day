from pool_day import _pd_handle, pool_day_callback

from ctypes import byref, c_void_p, c_uint8


@pool_day_callback
def thread_cb(param):
    print('hello from python callback!!!')


def create_pool(size):
    return _pd_handle.create_pool(c_uint8(size))


def destroy_pool(pool):
    return _pd_handle.destroy_pool(byref(pool))


def enqueue_task(pool, task):
    return _pd_handle.enqueue_task(pool, task)


def create_task(cb, param):
    return _pd_handle.create_task(cb, param)


def abort_tasks(pool):
    return _pd_handle.abort_tasks(pool)


def idle_tasks(pool):
    return _pd_handle.idle_tasks(pool)


def main():
    pool = create_pool(2)

    t1 = create_task(thread_cb, 'hello 1')
    t2 = create_task(thread_cb, 'hello 2')
    t3 = create_task(thread_cb, 'hello 3')

    print('enqueue_task ret =', enqueue_task(pool, t1))
    print('enqueue_task ret =', enqueue_task(pool, t2))
    print('enqueue_task ret =', enqueue_task(pool, t3))
    print('idle_tasks ret =', idle_tasks(pool))
    input('')
    print('destroy_pool ret =', destroy_pool(pool))


if __name__ == "__main__":
    main()
