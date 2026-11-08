# The scheduler: tasks run in parallel within their resources (cores, any counted resource, OS locks such as the dll
# readers/writer lock) and their dependencies. A failed or hung task only SKIPs its dependents; every task has its own
# deadline; recorded times order the queue (longest path first) and never stop anything.
from __future__ import annotations

import hashlib
import json
import os
import threading
import time
import traceback
from pathlib import Path
from typing import Callable

from . import inputs, proc, verdicts
from .cache import Cache, remove, safe
from .locks import FileLock, take_all
from .runs import RunRecord, check_graph
from .task import (BROKEN, CACHEABLE, CACHED_FAIL, CACHED_PASS, FAIL, HERE, PASS, ROOT, SKIPPED, TIMEOUT, Result,
                   Task)

LOCKS = "dist/tasks/locks"
LOCK_RETRY_S = 0.25  # a task waiting for a lock another process holds is tried again this often
WAIT_TURN_S = 1.0  # the scheduler wakes at least this often to look at the clock and the locks


def own_sources() -> list[str]:
    """The orchestrator's own code, part of every key: a change to how results are judged invalidates them."""
    return sorted(p.as_posix() for p in HERE.glob("*.py"))


class Pool:
    def __init__(self, root: Path | str | None = None, jobs: int | None = None, capacity: dict | None = None,
                 on_result: Callable[[Result], None] | None = None, use_cache: bool = True, label: str = "run",
                 on_finish: Callable[[str, Result], None] | None = None):
        self.root = Path(root or ROOT).resolve()
        self.capacity = {"cores": jobs or os.cpu_count() or 1}
        self.capacity.update({k: v for k, v in (capacity or {}).items() if k != "cores"})
        self.used = {k: 0 for k in self.capacity}
        self.on_result = on_result
        self.on_finish = on_finish  # (ident, result): for callers that submit under idents of their own
        self.use_cache = use_cache
        self.cache = Cache(self.root)
        self.snapshot = inputs.snapshot(self.root)
        self.snapshot.invalidate()  # a run reads the worktree as it is now, not as an earlier run in this process saw it
        self.cond = threading.Condition()
        self.tasks: dict[str, Task] = {}
        self.order: list[str] = []
        self.state: dict[str, str] = {}
        self.keys: dict[str, str] = {}
        self.results: dict[str, Result] = {}
        self.held: dict[str, list[FileLock]] = {}
        self.prio: dict[str, float] = {}
        self.record = RunRecord(self.root, label)
        self.run_dir = self.record.dir
        self.closed = False
        self.thread = threading.Thread(target=self._loop, name="tasks-scheduler", daemon=True)
        self.thread.start()

    # submission -----------------------------------------------------------------------------------------------------
    def submit(self, task: Task, ident: str | None = None) -> str:
        ident = ident or task.name
        with self.cond:
            if ident in self.tasks:
                raise ValueError(f"task submitted twice: {ident}")
            self.tasks[ident] = task
            self.order.append(ident)
            self.state[ident] = "waiting"
            self._prioritise()
            self.cond.notify_all()
        return ident

    def wait_all(self) -> list[Result]:
        with self.cond:
            while any(s != "done" for s in self.state.values()):
                self.cond.wait(WAIT_TURN_S)
            return [self.results[i] for i in self.order]

    def close(self) -> None:
        with self.cond:
            self.closed = True
            self.cond.notify_all()
        self.thread.join()
        self.record.close()

    # scheduling -----------------------------------------------------------------------------------------------------
    def _prioritise(self) -> None:
        """Longest path first: a task's own recorded time plus the longest chain of tasks that wait for it."""
        after = {i: [d for d in self.tasks[i].after if d in self.tasks] for i in self.tasks}
        users = {i: [j for j in self.tasks if i in after[j]] for i in self.tasks}
        memo: dict[str, float] = {}

        def path(i: str, seen: frozenset) -> float:
            if i not in memo:
                memo[i] = self.record.times.get(self.tasks[i].name, 0.0) + max(
                    (path(u, seen | {i}) for u in users[i] if u not in seen), default=0.0)
            return memo[i]
        self.prio = {i: path(i, frozenset()) for i in self.tasks}

    def _loop(self) -> None:
        with self.cond:
            while not self.closed:
                self._advance()
                self.cond.wait(LOCK_RETRY_S if self._waiting_on_locks else WAIT_TURN_S)

    def _advance(self) -> None:
        for i in self.order:
            if self.state[i] != "waiting":
                continue
            deps = self.tasks[i].after
            missing = [d for d in deps if d not in self.tasks]
            if missing:
                self._finish(i, Result(self.tasks[i].name, BROKEN, None, 0.0, "", "", f"unknown dependency: {', '.join(missing)}"))
                continue
            if any(self.state[d] != "done" for d in deps):
                continue
            failed = [d for d in deps if not self.results[d].passed]
            if failed:
                why = ", ".join(f"{d} {self.results[d].status}" for d in failed)
                self._finish(i, Result(self.tasks[i].name, SKIPPED, None, 0.0, "", "", f"after {why}"))
                continue
            self.state[i] = "keying"
            threading.Thread(target=self._key, args=(i,), name=f"key {i}", daemon=True).start()
        self._waiting_on_locks = False
        ready = sorted((i for i in self.order if self.state[i] == "ready"),
                       key=lambda i: (-self.prio.get(i, 0.0), self.order.index(i)))
        blocked_locks: set[str] = set()
        blocked_counts: set[str] = set()
        for i in ready:
            t = self.tasks[i]
            need = self._needs(t)
            locks = [name for name, _ in self._lock_names(t)]
            if any(name in blocked_locks for name in locks) or any(k in blocked_counts for k, v in need.items() if v):
                continue  # a task ahead waits for the same thing: no overtaking it
            short = [k for k, v in need.items() if self.used[k] + v > self.capacity[k]]
            if short:
                blocked_counts |= set(short)
                blocked_locks |= set(locks)
                continue
            got = self._take_locks(t)
            if got is None:
                blocked_locks |= set(locks)
                self._waiting_on_locks = True
                continue
            for k, v in need.items():
                self.used[k] += v
            self.held[i] = got
            self.state[i] = "running"
            threading.Thread(target=self._execute, args=(i,), name=f"run {i}", daemon=True).start()

    _waiting_on_locks = False

    def _needs(self, t: Task) -> dict[str, int]:
        res = dict(t.resources or {})
        res.pop("locks", None)
        res.setdefault("cores", 1)
        need = {}
        for k, v in res.items():
            if k not in self.capacity:
                continue  # a resource with no capacity on this run is not counted
            n = self.capacity[k] if v == "all" else int(v)
            need[k] = min(n, self.capacity[k])  # a task asking more than the machine has runs alone, not never
        return need

    def _lock_names(self, t: Task) -> list[tuple[str, bool]]:
        """The OS locks a task holds while it runs, (name, shared), in one global order: its named resources, and one
        exclusive lock per declared output. Two processes never write one output at once: a run that stored a result
        while another still wrote the same file in place cached the half-written file (two converter extractions shared
        one Jelly analysis and stored it torn, 2026-11-08)."""
        named = [(safe(name), mode == "shared") for name, mode in (t.resources or {}).get("locks", {}).items()]
        outputs = [(f"output-{hashlib.sha1(str(self.cache.output_path(o).resolve()).lower().encode()).hexdigest()}", False)
                   for o in t.outputs]
        return sorted(named + outputs)

    def _take_locks(self, t: Task) -> list[FileLock] | None:
        return take_all(self.root / LOCKS, self._lock_names(t))

    # one task -------------------------------------------------------------------------------------------------------
    def _task_key(self, t: Task) -> str:
        chosen_env = sorted((t.env or {}).items())
        extra = [json.dumps({"name": t.name, "argv": t.argv, "env": chosen_env, "extra": t.extra, "verdict": t.verdict,
                             "params": t.params, "outputs": t.outputs, "cwd": t.cwd}, sort_keys=True)]
        return inputs.key_of(list(t.inputs) + own_sources(), extra, root=self.root)

    def _hit(self, t: Task, key: str) -> tuple[Result, bool] | None:
        """The stored result of the key, its outputs put back, and whether any were; None when none is stored."""
        doc = self.cache.lookup(t.name, key) if self.use_cache and key else None
        if doc is None:
            return None
        log = self.cache.restore(t.name, key, doc)
        status = CACHED_PASS if doc["status"] == PASS else CACHED_FAIL
        return Result(t.name, status, doc["exit_code"], doc["seconds"], log, key, doc.get("message", "")), bool(doc["saved"])

    def _key(self, i: str) -> None:
        t = self.tasks[i]
        try:
            key = self._task_key(t)
            hit = self._hit(t, key)
            if hit is not None:
                with self.cond:
                    self._finish(i, hit[0], wrote=hit[1])
                return
        except Exception:
            with self.cond:
                self._finish(i, Result(t.name, BROKEN, None, 0.0, "", "", "keying failed:\n" + traceback.format_exc()))
            return
        with self.cond:
            self.keys[i] = key
            self.state[i] = "ready"
            self.cond.notify_all()

    def _execute(self, i: str) -> None:
        t = self.tasks[i]
        # With its locks held: another process may have run the same key while this one waited for them (`_lock_names`).
        try:
            hit = self._hit(t, self.keys.get(i, ""))
        except Exception:
            hit = Result(t.name, BROKEN, None, 0.0, "", "", "restoring the result failed:\n" + traceback.format_exc()), True
        if hit is not None:
            self._give_back(i, hit[0])
            return
        log = self.record.log(i)
        self.record.event({"event": "start", "task": i, "name": t.name})
        t0 = time.monotonic()
        code, message = None, ""
        try:
            if t.fresh:
                for out in t.outputs:
                    remove(self.cache.output_path(out))
            cwd = t.cwd if t.cwd is None or os.path.isabs(t.cwd) else str(self.root / t.cwd)
            code, timed_out = proc.run(t.argv, t.timeout_s, cwd=cwd or str(self.root), env=t.env, log=log, hidden=t.hidden)
            seconds = time.monotonic() - t0
            if timed_out:
                status, message = TIMEOUT, f"killed at its deadline of {t.timeout_s:g} s"
            elif code not in (0, 1):
                status, message = BROKEN, f"exit {code}"
            else:
                status, message = verdicts.judge(t, str(log), code, self.root)
                if status == PASS:
                    gone = [o for o in t.outputs if not self.cache.output_path(o).exists()]
                    if gone:
                        status, message = FAIL, f"declared output missing: {', '.join(gone)}"
        except Exception:
            seconds = time.monotonic() - t0
            status, message = BROKEN, traceback.format_exc()
        result = Result(t.name, status, code, round(seconds, 2), str(log), self.keys.get(i, ""), message)
        try:
            if status in CACHEABLE:
                self.cache.store(t.name, result.key, result, t.outputs)
        except Exception:
            result.status, result.message = BROKEN, "storing the result failed:\n" + traceback.format_exc()
        if status != BROKEN:
            self.record.took(t.name, seconds)
        self._give_back(i, result)

    def _give_back(self, i: str, result: Result) -> None:
        """A running task done: its counted resources and its locks given back, its result recorded."""
        with self.cond:
            for k, v in self._needs(self.tasks[i]).items():
                self.used[k] -= v
            for lock in self.held.pop(i, []):
                lock.release()
            self._finish(i, result, wrote=True)

    def _finish(self, i: str, result: Result, wrote: bool = False) -> None:
        """Under the condition: records the result, tells whoever listens, wakes the scheduler. `wrote`: a process ran or
        outputs were restored, so the files the next key reads may have changed."""
        self.results[i] = result
        self.state[i] = "done"
        if wrote:
            self.snapshot.invalidate()
        self.record.event({"event": "finish", "task": i, **result.to_json()})
        try:
            if self.on_result:
                self.on_result(result)
            if self.on_finish:
                self.on_finish(i, result)
        except Exception:
            traceback.print_exc()  # a broken listener is reported; the run itself goes on
        self.cond.notify_all()

def run(tasks: list[Task], jobs: int | None = None, on_result: Callable[[Result], None] | None = None, *,
        root: Path | str | None = None, use_cache: bool = True, capacity: dict | None = None, label: str = "run"
        ) -> list[Result]:
    """Runs the tasks; results in the order given. Raises ValueError on a malformed set (names, dependencies, cycles)."""
    check_graph(tasks)
    pool = Pool(root=root, jobs=jobs, capacity=capacity, on_result=on_result, use_cache=use_cache, label=label)
    try:
        for t in tasks:
            pool.submit(t)
        return pool.wait_all()
    finally:
        pool.close()
