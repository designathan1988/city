# What the tasks of a run hold of the machine, and whether one more may start now. A task takes: the counted resources of the
# run (its processors within the run's own share, any other counted name), its memory (admitted by measure: the most it was
# seen to take, runs.py, or what it declares until then), its locks (OS files a repository's tools share: the dll
# readers/writer lock, the GPU a timing test takes alone) and its processors (the machine's, machine.py: lock files every
# run shares). It starts only with all of them and holds nothing while it waits; what it waits for is said. A task behind
# the first that does not fit in memory or processors does not overtake it unless it delays it not; a lock reserves nothing,
# so no task waits behind the slowest holder of a lock it could share. When its process tree ends, its counts and processors go back at once and its
# locks once its result is stored (release).
from __future__ import annotations

import time
from dataclasses import dataclass, field
from pathlib import Path

from . import proc
from .cache import safe
from .locks import FileLock
from .machine import Machine
from .runs import RunRecord
from .task import Task

LOCKS = "dist/tasks/locks"
MEMORY = "memory_mb"  # a task's declared estimate of the memory it takes, in its resources: used until it has been measured
UNKNOWN_MB = 512.0  # a task never run that declares none
MEMORY_MARGIN = 1.2  # over the most a task was seen to take: one run's peak is not the next one's
RESERVE_SHARE = 0.15  # of the machine's memory: left free for whatever is not a task (the owner's own programs)
ALL = "all"  # a task's cores: every processor free when it starts, one at least


@dataclass
class Grant:
    """What a running task holds."""
    need: dict[str, int]
    cores: dict[int, FileLock]
    locks: list[FileLock]
    memory_mb: float
    expected: float | None = None  # the seconds it took before; None: never timed
    started: float = field(default_factory=time.monotonic)
    tree: proc.Process | None = None  # its process tree, once started

    def peak_mb(self) -> float:
        return self.tree.peak_memory_mb() if self.tree is not None else 0.0

    def ends(self) -> float:
        """When it is expected to end, on the monotonic clock; never, for one never timed."""
        return self.started + self.expected if self.expected is not None else float("inf")


class Admission:
    def __init__(self, root: Path, machine: Machine, jobs: int | None, capacity: dict | None, record: RunRecord):
        self.root = root
        self.machine = machine
        self.record = record
        self.capacity = {"cores": jobs or len(machine.cores)}  # the processors this run may fill at once
        self.capacity.update({k: v for k, v in (capacity or {}).items() if k != "cores"})
        self.used = {k: 0 for k in self.capacity}
        self.granted: dict[str, Grant] = {}
        self.retry = False  # a task waits for something another process may free: looked at again soon, with no event to wake on
        self.head: tuple[float, int] | None = None  # this turn's first task that did not fit: its memory and its processors

    def begin(self) -> None:
        """A turn of the scheduler starts: the ready tasks are offered again, the most urgent first."""
        self.retry = False
        self.head = None

    def needs(self, t: Task) -> dict[str, int]:
        """The counted resources the task is counted for while it waits and runs."""
        res = dict(t.resources or {})
        res.pop("locks", None)
        res.setdefault("cores", 1)
        need = {}
        for k, v in res.items():
            if k not in self.capacity:
                continue  # a resource with no capacity on this run is not counted (a memory estimate is no count)
            n = 1 if v == ALL else int(v)  # all: counted for one until it starts, when it takes what is free
            need[k] = min(n, self.capacity[k])  # a task asking more than the machine has runs alone, not never
        return need

    def memory_of(self, t: Task) -> float:
        """The memory the task is admitted for, MiB: the most it was seen to take, with a margin; what it declares, never run."""
        seen = self.record.memory.get(t.name)
        return seen * MEMORY_MARGIN if seen else float((t.resources or {}).get(MEMORY, UNKNOWN_MB))

    def try_start(self, i: str, t: Task, waiting_cores: int, expected: float | None = None) -> str | None:
        """Takes all the task needs and returns None; or takes nothing and returns what it waits for. The tasks are offered
        the most urgent first. The first that does not fit (memory, processors) is the one nothing may delay: a task behind it
        starts only if it fits now and either ends before that one can start (`expected`: the seconds it took before) or
        takes no more than will be left over when it does (EASY backfilling: one reservation, for the first that waits; it
        is not starved, and nothing that fits stands idle behind it). A lock is no reservation: a task that waits for one
        (a GPU timing test that takes the GPU alone while others draw on it) holds back no task that can take it now, so no
        task ever waits behind the slowest holder of a lock it could share; the one that waits starts when the lock is free.
        `waiting_cores`: the processors the other tasks of the run that still wait will ask for."""
        need = self.needs(t)
        locks = (t.resources or {}).get("locks", {})
        short = [k for k, v in need.items() if self.used[k] + v > self.capacity[k]]
        mb = self.memory_of(t)
        if short or not self._memory_fits(mb):
            self.head = self.head or (mb, need["cores"])
            return ", ".join(short) if short else f"memory ({mb:.0f} MiB)"
        if self.head is not None and not self._delays_nobody(mb, need["cores"], expected):
            return "the task ahead of it, which it would delay"
        got = self._take_locks(locks)
        if got is None:
            self.retry = True
            return "lock " + ", ".join(sorted(locks))
        room = self.capacity["cores"] - self.used["cores"]
        if (t.resources or {}).get("cores") == ALL:
            least, most = 1, room
        else:  # with what it asked, the free ones nobody of this run that still waits will ask for
            least, most = need["cores"], need["cores"] + max(0, room - need["cores"] - waiting_cores)
        cores = self.machine.take_cores(least, most)
        if cores is None:  # another run holds them
            for lock in got:
                lock.release()
            self.head = self.head or (mb, need["cores"])
            self.retry = True
            return f"processors ({least})"
        need["cores"] = len(cores)
        for k, v in need.items():
            self.used[k] += v
        self.granted[i] = Grant(need, cores, got, mb, expected)
        return None

    def _delays_nobody(self, mb: float, cores: int, expected: float | None) -> bool:
        """Whether a task that fits now may start though one ahead of it waits: it ends before that one can start, or it
        takes no more than will be left over when that one does."""
        head_mb, head_cores = self.head
        available, total = self.machine.memory()
        free_mb = available - total * RESERVE_SHARE - sum(max(0.0, g.memory_mb - g.peak_mb()) for g in self.granted.values())
        free_cores = self.capacity["cores"] - self.used["cores"]
        can_start = time.monotonic()  # when the one ahead can: as the running tasks end, the soonest first
        for g in sorted(self.granted.values(), key=Grant.ends):
            if free_mb >= head_mb and free_cores >= head_cores:
                break
            free_mb += g.memory_mb
            free_cores += g.need["cores"]
            can_start = g.ends()
        ends_before = expected is not None and time.monotonic() + expected <= can_start
        return ends_before or (mb <= free_mb - head_mb and cores <= free_cores - head_cores)

    def release(self, i: str, keep_locks: bool = False) -> list[FileLock]:
        """What a task held goes back: its counted resources and its processors at once; its locks too, or, with
        `keep_locks`, they are returned for the caller to release when it is done with what they guard (the outputs a
        task's result stores while no other task may write them)."""
        grant = self.granted.pop(i)
        for k, v in grant.need.items():
            self.used[k] -= v
        for lock in grant.cores.values():
            lock.release()
        if keep_locks:
            return list(grant.locks)
        for lock in grant.locks:
            lock.release()
        return []

    def _memory_fits(self, mb: float) -> bool:
        """Whether a task of `mb` may start now. With nothing of this run running it may, whatever it takes: a task larger
        than the machine runs alone, not never."""
        if not self.granted:
            return True
        available, total = self.machine.memory()
        coming = sum(max(0.0, g.memory_mb - g.peak_mb()) for g in self.granted.values())
        return available - coming - mb >= total * RESERVE_SHARE

    def _take_locks(self, locks: dict[str, str]) -> list[FileLock] | None:
        got = []
        for name, mode in sorted(locks.items()):
            lock = FileLock(self.root / LOCKS / f"{safe(name)}.lock")
            if not lock.try_acquire(shared=(mode == "shared")):
                for g in got:
                    g.release()
                return None
            got.append(lock)
        return got
