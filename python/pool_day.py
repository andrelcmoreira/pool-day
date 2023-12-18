from pool_day import _lib_handle

from ctypes import byref, c_uint8


def create_pool(size):
    return _lib_handle.create_pool(c_uint8(size))


def destroy_pool(pool):
    return _lib_handle.destroy_pool(byref(pool))


def abort_tasks(pool):
    return _lib_handle.abort_tasks(pool)


def main():
    pool = create_pool(5)

    print('ret =', abort_tasks(pool))
    print('ret =', destroy_pool(pool))


if __name__ == "__main__":
    main()
