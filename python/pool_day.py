from pool_day import _lib_handle

from ctypes import byref, c_void_p, c_uint8
from ctypes import CFUNCTYPE as c_func_type

@c_func_type(c_void_p, c_void_p)
def thread_cb(param):
    print('hello from python callback!!!')


def create_pool(size):
    return _lib_handle.create_pool(c_uint8(size))


def destroy_pool(pool):
    return _lib_handle.destroy_pool(byref(pool))


def enqueue_task(pool, task):
    return _lib_handle.enqueue_task(pool, task)


def create_task(cb, param):
    return _lib_handle.create_task(cb, param)


def abort_tasks(pool):
    return _lib_handle.abort_tasks(pool)


def idle_tasks(pool):
    return _lib_handle.idle_tasks(pool)


def main():
    pool = create_pool(2)

    t1 = create_task(thread_cb, 'hello 1')
    t2 = create_task(thread_cb, 'hello 1')
    t3 = create_task(thread_cb, 'hello 1')

    print('enqueue_task ret =', enqueue_task(pool, t1))
    print('enqueue_task ret =', enqueue_task(pool, t2))
    print('enqueue_task ret =', enqueue_task(pool, t3))
    print('idle_tasks ret =', idle_tasks(pool))
    input('')
    print('destroy_pool ret =', destroy_pool(pool))


if __name__ == "__main__":
    main()
