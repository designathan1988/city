# The scheduler: tasks run in parallel within what the machine has, by what each one takes (admission.py: its processors,
# which every run of the machine shares; its memory, admitted by measure; its locks). So the number of tasks at once follows
# from the tasks and the machine, any number of runs share it, and a task waits only for what it needs. A failed or hung task
# only SKIPs its dependents; every task has its own deadline; recorded times order the queue (longest path first). The cores
# a task is counted for are the processors its process tree runs on (an affinity): a process that sizes its thread pools by
# the machine cannot take what was given to others. While tasks run, the run says what it is doing (on_progress).
from __future__ import annotations

import json
import os
import threading
import time
import traceback
from pathlib import Path
from typing import Callable

from . import inputs, proc, verdicts
from .admission import Admission
from .cache import Cache, remove
from .machine import Machine
from .runs import RunRecord, check_graph
from .task import (BROKEN, CACHEABLE, CACHED_FAIL, CACHED_PASS, FAIL, HERE, PASS, ROOT, SKIPPED, TIMEOUT, Result,
                   Task)

LOCK_RETRY_S = 0.25  # a task waiting for a lock or a processor another process holds is tried again this often
WAIT_TURN_S = 1.0  # the scheduler wakes at least this often to look at the clock and the locks
PROGRESS_S = 10.0  # while tasks run, what the run is doing is said this often
UNSTARTED = ("waiting", "keying", "ready")


def own_sources() -> list[str]:
    """The orchestrator's own code, part of every key: a change to how results are judged invalidates them."""
    return sorted(p.as_posix() for p in HERE.glob("*.py"))


class Pool:
    def __init__(self, root: Path | str | None = None, jobs: int | None = None, capacity: dict | None = None,
                 on_result: Callable[[Result], None] | None = None, use_cache: bool = True, label: str = "run",
                 on_finish: Callable[[str, Result], None] | None = None, machine: Machine | None = None,
                 on_progress: Callable[[dict], None] | None = None, progress_s: float = PROGRESS_S):
        self.root = Path(root or ROOT).resolve()
        self.on_result = on_result
        self.on_finish = on_finish  # (ident, result): for callers that submit under idents of their own
        self.on_progress = on_progress
        self.progress_s = progress_s
        self._said = time.monotonic()
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
        self.prio: dict[str, float] = {}
        self.why: dict[str, str] = {}  # what a task that has not started waits for
        self.record = RunRecord(self.root, label)
        self.run_dir = self.record.dir
        self.admission = Admission(self.root, machine or Machine(), jobs, capacity, self.record)
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
        users: dict[str, list[str]] = {i: [] for i in self.tasks}
        for j in self.tasks:
            for d in self.tasks[j].after:
                if d in users:
                    users[d].append(j)
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
                self._say()
                self.cond.wait(min(LOCK_RETRY_S if self.admission.retry else WAIT_TURN_S, self.progress_s))

    def _advance(self) -> None:
        for i in self.order:
            if self.state[i] != "waiting":
                continue
            deps = self.tasks[i].after
            missing = [d for d in deps if d not in self.tasks]
            if missing:
                self._finish(i, Result(self.tasks[i].name, BROKEN, None, 0.0, "", "", f"unknown dependency: {', '.join(missing)}"))
                continue
            unfinished = [d for d in deps if self.state[d] != "done"]
            if unfinished:
                self.why[i] = "after " + ", ".join(unfinished)
                continue
            failed = [d for d in deps if not self.results[d].passed]
            if failed:
                why = ", ".join(f"{d} {self.results[d].status}" for d in failed)
                self._finish(i, Result(self.tasks[i].name, SKIPPED, None, 0.0, "", "", f"after {why}"))
                continue
            self.state[i] = "keying"
            self.why[i] = "its key"
            threading.Thread(target=self._key, args=(i,), name=f"key {i}", daemon=True).start()
        self.admission.begin()
        place = {i: n for n, i in enumerate(self.order)}
        ready = sorted((i for i in self.order if self.state[i] == "ready"), key=lambda i: (-self.prio.get(i, 0.0), place[i]))
        blocked: set[str] = set()  # the locks a task ahead waits for: no task behind it takes them first
        for i in ready:
            others = sum(self.admission.needs(self.tasks[j])["cores"] for j in self.order if j != i and self.state[j] in UNSTARTED)
            why = self.admission.try_start(i, self.tasks[i], others, blocked, self.record.times.get(self.tasks[i].name))
            if why is not None:
                self.why[i] = why
                continue
            self.why.pop(i, None)
            self.state[i] = "running"
            threading.Thread(target=self._execute, args=(i,), name=f"run {i}", daemon=True).start()

    # what the run is doing --------------------------------------------------------------------------------------------
    def progress(self) -> dict:
        """Under the condition: how far the run is, each running task with its seconds, the seconds it took before and the
        memory it holds, and what each task that has not started waits for."""
        now = time.monotonic()
        running = [{"task": i, "seconds": round(now - g.started, 1), "expected": self.record.times.get(self.tasks[i].name),
                    "memory_mb": round(g.peak_mb()), "cores": len(g.cores)} for i, g in self.admission.granted.items()]
        waiting = {i: self.why.get(i, "its turn") for i in self.order if self.state[i] in UNSTARTED}
        return {"done": sum(s == "done" for s in self.state.values()), "total": len(self.order), "running": running,
                "waiting": waiting, "memory_free_mb": round(self.admission.machine.memory()[0])}

    def _say(self) -> None:
        now = time.monotonic()
        if self.on_progress is None or now - self._said < self.progress_s or not self.admission.granted:
            return
        self._said = now
        try:
            self.on_progress(self.progress())
        except Exception:
            traceback.print_exc()  # a broken listener is reported; the run itself goes on

    # one task -------------------------------------------------------------------------------------------------------
    def _task_key(self, t: Task) -> str:
        chosen_env = sorted((t.env or {}).items())
        extra = [json.dumps({"name": t.name, "argv": t.argv, "env": chosen_env, "extra": t.extra, "verdict": t.verdict,
                             "params": t.params, "outputs": t.outputs, "cwd": t.cwd}, sort_keys=True)]
        return inputs.key_of(list(t.inputs) + own_sources(), extra, root=self.root)

    def _key(self, i: str) -> None:
        t = self.tasks[i]
        try:
            key = self._task_key(t)
            doc = self.cache.lookup(t.name, key) if self.use_cache else None
            if doc is not None:
                log = self.cache.restore(t.name, key, doc)
                status = CACHED_PASS if doc["status"] == PASS else CACHED_FAIL
                result = Result(t.name, status, doc["exit_code"], doc["seconds"], log, key, doc.get("message", ""))
                with self.cond:
                    self._finish(i, result, wrote=bool(doc["saved"]))
                return
        except Exception:
            with self.cond:
                self._finish(i, Result(t.name, BROKEN, None, 0.0, "", "", "keying failed:\n" + traceback.format_exc()))
            return
        with self.cond:
            self.keys[i] = key
            self.state[i] = "ready"
            self.why[i] = "its turn"
            self.cond.notify_all()

    def _execute(self, i: str) -> None:
        t = self.tasks[i]
        log = self.record.log(i)
        grant = self.admission.granted[i]
        cores = sorted(grant.cores)
        self.record.event({"event": "start", "task": i, "name": t.name, "expected": self.record.times.get(t.name),
                           "cores": cores, "memory_mb": round(grant.memory_mb)})
        t0 = time.monotonic()
        code, message, peak = None, "", 0.0
        try:
            if t.fresh:
                for out in t.outputs:
                    remove(self.cache.output_path(out))
            cwd = t.cwd if t.cwd is None or os.path.isabs(t.cwd) else str(self.root / t.cwd)
            with proc.Process(t.argv, cwd=cwd or str(self.root), env=t.env, log=log, hidden=t.hidden, cores=cores) as tree:
                grant.tree = tree
                try:
                    code = tree.wait(t.timeout_s)
                    peak = tree.peak_memory_mb()
                finally:
                    with self.cond:
                        grant.tree = None
            seconds = time.monotonic() - t0
            if code is None:
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
            self.record.took(t.name, seconds, peak)
        with self.cond:
            self.admission.release(i)
            self._finish(i, result, wrote=True)

    def _finish(self, i: str, result: Result, wrote: bool = False) -> None:
        """Under the condition: records the result, tells whoever listens, wakes the scheduler. `wrote`: a process ran or
        outputs were restored, so the files the next key reads may have changed."""
        self.results[i] = result
        self.state[i] = "done"
        self.why.pop(i, None)
        if wrote:
            self.snapshot.refresh()
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
        root: Path | str | None = None, use_cache: bool = True, capacity: dict | None = None, label: str = "run",
        machine: Machine | None = None, on_progress: Callable[[dict], None] | None = None,
        progress_s: float = PROGRESS_S) -> list[Result]:
    """Runs the tasks; results in the order given. Raises ValueError on a malformed set (names, dependencies, cycles)."""
    check_graph(tasks)
    pool = Pool(root=root, jobs=jobs, capacity=capacity, on_result=on_result, use_cache=use_cache, label=label,
                machine=machine, on_progress=on_progress, progress_s=progress_s)
    try:
        for t in tasks:
            pool.submit(t)
        return pool.wait_all()
    finally:
        pool.close()
