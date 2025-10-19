from ctypes import CDLL as cdll
from ctypes import POINTER as c_pointer
from ctypes import CFUNCTYPE as c_func_type
from ctypes import (
    Structure,
    c_int,
    c_uint8,
    c_void_p,
)


class _PoolDay(Structure):
    pass


class _Task(Structure):
    pass


def pool_day_callback(cb):

    @c_func_type(c_void_p, c_void_p)
    def _cb(param):
        cb(param) # TODO: return the task result properly

    return _cb


_pd_handle = cdll('/usr/lib/libpool-day.so')

# create_pool
_pd_handle.create_pool.argtypes = [c_uint8]
_pd_handle.create_pool.restype = c_pointer(_PoolDay)

# destroy_pool
_pd_handle.destroy_pool.argtypes = [c_void_p]
_pd_handle.destroy_pool.restype = c_int

# abort_tasks
_pd_handle.abort_tasks.argtypes = [c_pointer(_PoolDay)]
_pd_handle.abort_tasks.restype = c_int

# enqueue_task
_pd_handle.enqueue_task.argtypes = [c_pointer(_PoolDay), c_pointer(_Task)]
_pd_handle.enqueue_task.restype = c_int

# queued_tasks
_pd_handle.queued_tasks.argtypes = [c_pointer(_PoolDay)]
_pd_handle.queued_tasks.restype = c_uint8

# create_task
_pd_handle.create_task.argtypes = [c_void_p, c_void_p]
_pd_handle.create_task.restype = c_pointer(_Task)

# wait_task_finish
_pd_handle.wait_task_finish.argtypes = [c_pointer(_PoolDay), c_pointer(_Task)]
_pd_handle.wait_task_finish.restype = c_void_p
