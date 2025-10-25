from ctypes import CDLL as cdll
from ctypes import POINTER as c_pointer
from ctypes import CFUNCTYPE as c_func_type
from ctypes import cast
from ctypes import byref
from ctypes import (
    Structure,
    c_int,
    c_uint8,
    c_void_p,
    c_int
)
from dataclasses import dataclass
from typing import Any


class CPoolDay(Structure):
    pass


class CTask(Structure):
    pass


@dataclass
class TaskResult:
    result: Any


def pool_day_callback(cb):

    @c_func_type(c_void_p, c_void_p)
    def _cb(param):
        ret = cb(param)

        print('pool_day_callback ret:', ret)
        print('pool_day_callback ret type:', type(ret))
        print('pool_day_callback param value:', param)
        print('pool_day_callback param type:', type(param))

        return ret

    return _cb


def to_result(ptr: c_void_p) -> TaskResult:
    return TaskResult(result=ptr)


_pd_handle = cdll('/usr/lib/libpool-day.so')

# create_pool
_pd_handle.create_pool.argtypes = [c_uint8]
_pd_handle.create_pool.restype = c_pointer(CPoolDay)

# destroy_pool
_pd_handle.destroy_pool.argtypes = [c_void_p]
_pd_handle.destroy_pool.restype = c_int

# abort_tasks
_pd_handle.abort_tasks.argtypes = [c_pointer(CPoolDay)]
_pd_handle.abort_tasks.restype = c_int

# enqueue_task
_pd_handle.enqueue_task.argtypes = [c_pointer(CPoolDay), c_pointer(CTask)]
_pd_handle.enqueue_task.restype = c_int

# queued_tasks
_pd_handle.queued_tasks.argtypes = [c_pointer(CPoolDay)]
_pd_handle.queued_tasks.restype = c_uint8

# create_task
_pd_handle.create_task.argtypes = [c_void_p, c_void_p]
_pd_handle.create_task.restype = c_pointer(CTask)

# wait_task_finish
_pd_handle.wait_task_finish.argtypes = [c_pointer(CPoolDay), c_pointer(CTask)]
_pd_handle.wait_task_finish.restype = c_void_p
