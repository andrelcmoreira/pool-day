from contextlib import contextmanager
from ctypes import byref

from pool_day.c_defs import _pd_handle, _Task, _PoolDay


class PoolDay:

    """
    """

    def __init__(self, pool: _PoolDay):
        self._pool = pool

    def enqueue_task(self, task: _Task):
        """
        """
        return _pd_handle.enqueue_task(self._pool, task)

    def abort_tasks(self):
        """
        """
        return _pd_handle.abort_tasks(self._pool)

    def queued_tasks(self) -> int:
        """
        """
        return _pd_handle.queued_tasks(self._pool)

    def wait_task_finish(self, task: _Task):
        """
        """
        return _pd_handle.wait_task_finish(self._pool, task)


def create_task(cb, param) -> _Task:
    """
    """
    return _pd_handle.create_task(cb, param)


@contextmanager
def create_pool(size: int):
    """
    """
    pool = _pd_handle.create_pool(size)
    yield PoolDay(pool)
    _pd_handle.destroy_pool(byref(pool))
