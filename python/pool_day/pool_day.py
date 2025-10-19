from contextlib import contextmanager
from ctypes import byref

from pool_day.c_defs import _pd_handle, PoolDay, Task


@contextmanager
def create_pool(size: int):
    """
    """
    pool = _pd_handle.create_pool(size)
    yield pool
    _pd_handle.destroy_pool(byref(pool))


def destroy_pool(pool: PoolDay):
    """
    """
    return _pd_handle.destroy_pool(byref(pool))


def enqueue_task(pool: PoolDay, task: Task):
    """
    """
    return _pd_handle.enqueue_task(pool, task)


def create_task(cb, param):
    """
    """
    return _pd_handle.create_task(cb, param)


def abort_tasks(pool: PoolDay):
    """
    """
    return _pd_handle.abort_tasks(pool)


def queued_tasks(pool: PoolDay) -> int:
    """
    """
    return _pd_handle.queued_tasks(pool)


def wait_task_finish(pool: PoolDay, task: Task):
    """
    """
    return _pd_handle.wait_task_finish(pool, task)
