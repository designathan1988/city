# The records of a run: its folder under dist/tasks (one log per task, the event stream events.jsonl, the times its tasks
# took, times.json; the newest run is the folder of the latest name, newest_run), the times known per task name (every kept
# run's, the newest winning, used only to order the queue), and the check of a task set's shape before it runs (names,
# dependencies, cycles). No file is shared by runs that write at once: each writes in its own folder.
from __future__ import annotations

import json
import os
import threading
import time
from datetime import datetime
from pathlib import Path

from . import proc
from .cache import remove, safe, write_atomic
from .locks import FileLock

RUNS = "dist/tasks"
RUN_TIMES = "times.json"  # in a run's folder: the seconds each of its tasks took, by task name
RUN_MEMORY = "memory.json"  # beside it: the most MiB each task's process tree held at once
KEPT_RUNS = 30  # run folders (logs, events) kept; results live on in the cache
PRUNE_LOCK = ".prune.lock"  # held by the one run that prunes the old run folders (RunRecord._prune)


class RunRecord:
    def __init__(self, root: Path, label: str):
        self.root = root
        stamp = datetime.now().strftime("%Y%m%d-%H%M%S")
        self.dir = root / RUNS / f"{stamp}-{os.getpid()}-{safe(label)}"
        self.dir.mkdir(parents=True, exist_ok=True)
        self._prune()
        self._events = open(self.dir / "events.jsonl", "a", encoding="utf-8")
        self._lock = threading.Lock()
        self._own: dict[str, float] = {}  # this run's times, written to its folder
        self._own_memory: dict[str, float] = {}
        self.times: dict[str, float] = {}  # every kept run's, oldest first so the newest wins (pool.py orders by them)
        self.memory: dict[str, float] = {}  # the most any kept run saw each task take, MiB (pool.py admits by it)
        for run in sorted(p for p in (root / RUNS).iterdir() if p.is_dir() and p.name[:1].isdigit() and p != self.dir):
            try:
                self.times.update(json.loads((run / RUN_TIMES).read_text(encoding="utf-8")))
            except FileNotFoundError:  # a run that recorded no time yet, or one another run pruned while this one read
                continue
            try:
                for name, mb in json.loads((run / RUN_MEMORY).read_text(encoding="utf-8")).items():
                    self.memory[name] = max(self.memory.get(name, 0.0), float(mb))
            except FileNotFoundError:  # a run of before the memory was kept, or of a platform that keeps none
                continue

    def log(self, ident: str) -> Path:
        return self.dir / f"{safe(ident)}.log"

    def event(self, doc: dict) -> None:
        line = json.dumps({"t": round(time.time(), 3), **doc}, ensure_ascii=False)
        with self._lock:
            self._events.write(line + "\n")
            self._events.flush()

    def took(self, name: str, seconds: float, memory_mb: float = 0.0) -> None:
        """A task's time and the most memory its processes held, written with this run's others to this run's own folder (no
        other run writes there). A platform that keeps no memory figure gives 0, which is not written."""
        with self._lock:
            self.times[name] = seconds
            self._own[name] = seconds
            write_atomic(self.dir / RUN_TIMES, json.dumps(self._own, indent=1, sort_keys=True))
            if memory_mb > 0.0:
                self.memory[name] = max(self.memory.get(name, 0.0), memory_mb)
                self._own_memory[name] = round(memory_mb, 1)
                write_atomic(self.dir / RUN_MEMORY, json.dumps(self._own_memory, indent=1, sort_keys=True))

    def close(self) -> None:
        self._events.close()

    def _prune(self) -> None:
        """The oldest run folders past KEPT_RUNS go, except those of a run whose process still lives.

        Runs start together (a test runner's files in parallel, each with its serve process), and two removing the same
        folder trip over each other (one deletes what the other is deleting; Windows refuses a rename of a folder another
        handle is in). One run prunes at a time: the one holding the lock (an OS file lock, locks.FileLock, released when
        its process ends however it ends); a run that finds it held leaves the pruning to that one and to the next run.
        """
        with FileLock(self.root / RUNS / PRUNE_LOCK) as lock:
            if not lock.try_acquire(shared=False):
                return
            runs = sorted(p for p in (self.root / RUNS).iterdir() if p.is_dir() and p.name[:1].isdigit())
            for old in runs[:-KEPT_RUNS]:
                parts = old.name.split("-")
                if len(parts) > 2 and parts[2].isdigit() and proc.alive(int(parts[2])):
                    continue
                remove(old)


def newest_run(root: Path) -> Path | None:
    """The folder of the newest run: run names start with their start time (RunRecord), so the greatest name is the newest."""
    base = root / RUNS
    names = sorted(p.name for p in base.iterdir() if p.is_dir() and p.name[:1].isdigit()) if base.is_dir() else []
    return base / names[-1] if names else None


def live_runs(root: Path) -> list[Path]:
    """The folders of the runs whose process still lives (a run's folder carries its process id), oldest first."""
    base = root / RUNS
    out = []
    for p in sorted(base.iterdir()) if base.is_dir() else []:
        parts = p.name.split("-")
        if p.is_dir() and p.name[:1].isdigit() and len(parts) > 2 and parts[2].isdigit() and proc.alive(int(parts[2])):
            out.append(p)
    return out


def check_graph(tasks) -> None:
    """Raises ValueError for a name used twice, an unknown dependency or a cycle."""
    names = [t.name for t in tasks]
    dup = sorted({n for n in names if names.count(n) > 1})
    if dup:
        raise ValueError(f"task names used twice: {', '.join(dup)}")
    known = set(names)
    for t in tasks:
        unknown = [d for d in t.after if d not in known]
        if unknown:
            raise ValueError(f"{t.name}: unknown dependency {', '.join(unknown)}")
    after = {t.name: t.after for t in tasks}
    state: dict[str, int] = {}

    def visit(n: str, chain: list[str]) -> None:
        if state.get(n) == 2:
            return
        if state.get(n) == 1:
            raise ValueError(f"dependency cycle: {' -> '.join(chain + [n])}")
        state[n] = 1
        for d in after[n]:
            visit(d, chain + [n])
        state[n] = 2
    for n in names:
        visit(n, [])
