from ctypes import CDLL as cdll
from ctypes import CFUNCTYPE as c_func_type
from ctypes import POINTER as c_pointer
from ctypes import (
    Structure,
    c_int,
    c_uint8,
    c_void_p,
)


class PoolDay(Structure):
    pass


class Task(Structure):
    pass


lib_cb = c_func_type(c_void_p, c_void_p)

_lib_handle = cdll('../build/libpool-day.so')

# create_pool
_lib_handle.create_pool.argtypes = [c_uint8]
_lib_handle.create_pool.restype = c_pointer(PoolDay)

# destroy_pool
_lib_handle.destroy_pool.argtypes = [c_void_p]
_lib_handle.destroy_pool.restype = c_int

# abort_tasks
_lib_handle.abort_tasks.argtypes = [c_pointer(PoolDay)]
_lib_handle.abort_tasks.restype = c_int

# enqueue_task
_lib_handle.enqueue_task.argtypes = [c_pointer(PoolDay), c_pointer(Task)]
_lib_handle.enqueue_task.restype = c_int

# idle_tasks
_lib_handle.idle_tasks.argtypes = [c_pointer(PoolDay)]
_lib_handle.idle_tasks.restype = c_uint8

# create_task
_lib_handle.create_task.argtypes = [c_void_p, c_void_p]
_lib_handle.create_task.restype = c_pointer(Task)
