# The machine every run shares: its processors, each held as an OS lock file by the task running on it, and its memory as the
# operating system counts it now. A run counts nothing for itself: two runs started side by side see the same processors
# taken and the same memory free, and a holder that dies frees what it held (locks.FileLock). The folder of the lock files is
# the machine's own (the temporary folder), not a repository's: every checkout on the machine shares its processors.
from __future__ import annotations

import tempfile
from pathlib import Path
from typing import Callable

from . import proc
from .locks import FileLock

FOLDER = "tasks-machine"  # under the temporary folder: the processors' lock files


class Machine:
    def __init__(self, folder: Path | str | None = None, cores: list[int] | None = None,
                 memory: Callable[[], tuple[float, float]] | None = None):
        self.folder = Path(folder) if folder else Path(tempfile.gettempdir()) / FOLDER
        self.cores = list(cores) if cores is not None else proc.usable_cores()
        self._memory = memory or proc.memory_mb

    def memory(self) -> tuple[float, float]:
        """(MiB available now, MiB in all)."""
        return self._memory()

    def take_cores(self, least: int, most: int) -> dict[int, FileLock] | None:
        """Up to `most` processors nobody holds, each with its lock; None, holding nothing, when fewer than `least` are free."""
        held: dict[int, FileLock] = {}
        for core in self.cores:
            if len(held) >= most:
                break
            lock = FileLock(self.folder / f"core-{core}.lock")
            if lock.try_acquire(shared=False):
                held[core] = lock
        if len(held) < least:
            for lock in held.values():
                lock.release()
            return None
        return held
