from ctypes import (
    CDLL,
    POINTER,
    Structure,
    c_int,
    c_uint8,
    c_void_p
)


class PoolDay(Structure):
    pass


class Task(Structure):
    pass


_lib_handle = CDLL('../build/libpool-day.so')

# create_pool
_lib_handle.create_pool.argtypes = [c_uint8]
_lib_handle.create_pool.restype = POINTER(PoolDay)

# destroy_pool
_lib_handle.destroy_pool.argtypes = [c_void_p]
_lib_handle.destroy_pool.restype = c_int

# abort_tasks
_lib_handle.abort_tasks.argtypes = [POINTER(PoolDay)]
_lib_handle.abort_tasks.restype = c_int

# enqueue_task
_lib_handle.enqueue_task.argtypes = [POINTER(PoolDay), POINTER(Task)]
_lib_handle.enqueue_task.restype = c_int
