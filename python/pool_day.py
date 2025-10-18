from contextlib import contextmanager
from ctypes import byref

from pool_day import _pd_handle


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
