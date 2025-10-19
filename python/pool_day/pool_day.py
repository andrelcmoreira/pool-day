from contextlib import contextmanager
from ctypes import byref

from pool_day.c_defs import _pd_handle, _Task, _PoolDay


class PoolDay:

    """
    PoolDay instance.

    Implements a wrapper around the pool-day C library.
    """

    def __init__(self, pool: _PoolDay):
        """
        Initialize the PoolDay instance.

        :pool: The pool structure instance.
        """
        self._pool = pool

    def enqueue_task(self, task: _Task) -> int:
        """
        Enqueue a task to the pool.

        :task: The task instance.
        :return: 0 on success, the suitable error code on failure.
        """
        return _pd_handle.enqueue_task(self._pool, task)

    def abort_tasks(self) -> int:
        """
        Abort all pending tasks in the pool.

        :return: 0 on success, the suitable error code on failure.
        """
        return _pd_handle.abort_tasks(self._pool)

    def queued_tasks(self) -> int:
        """
        Get the number of queued tasks in the pool.

        :return: Number of queued tasks.
        """
        return _pd_handle.queued_tasks(self._pool)

    def wait_task_finish(self, task: _Task):
        """
        Wait for a task to finish.

        :task: The task instance.
        :return: The return value of the task's callback function.
        """
        return _pd_handle.wait_task_finish(self._pool, task)


def create_task(cb, param) -> _Task:
    """
    Create a new pool's task instance.

    :cb: The callback function.
    :param: The parameter passed to the callback function.

    :return: A new task instance.
    """
    return _pd_handle.create_task(cb, param)


@contextmanager
def create_pool(size: int):
    """
    Create a new pool instance.

    :size: Number of threads in the pool.

    :return: A pool-day instance.
    """
    pool = _pd_handle.create_pool(size)
    yield PoolDay(pool)
    _pd_handle.destroy_pool(byref(pool))
