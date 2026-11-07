"""Work spread over the machine: one pool for the whole process, born only when there is more to do than a process does alone."""
import atexit
import multiprocessing
import os

import psutil

WORKER_BYTES = 3 << 29
_POOL = []


def cores():
    """What this process may use: a check that runs beside others is told its share, so together they never ask more of the machine than it has."""
    return int(os.environ.get("ARCH_CORES") or os.cpu_count() or 1)


def _shared_pool():
    if not _POOL:
        # A parser holds a translation unit and its shared headers in memory: more of them than memory holds page each other out and all go slower.
        room = int(psutil.virtual_memory().available // WORKER_BYTES)
        _POOL.append(multiprocessing.get_context("spawn").Pool(max(1, min(cores(), room))))
        atexit.register(close)
    return _POOL[0]


def close():
    """Idle processes hold memory another task needs."""
    if _POOL:
        _POOL.pop().terminate()


def each(fn, tasks):
    """Results of `fn` over the tasks, in any order. A few tasks cost less here than waking a pool."""
    if len(tasks) < cores():
        return map(fn, tasks)
    return _shared_pool().imap_unordered(fn, tasks, chunksize=1)
