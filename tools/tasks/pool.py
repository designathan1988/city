# The scheduler: tasks run in parallel within what the machine has, by what each one takes (admission.py: its processors,
# which every run of the machine shares; its memory, admitted by measure; its locks). So the number of tasks at once follows
# from the tasks and the machine, any number of runs share it, and a task waits only for what it needs. A failed or hung task
# only SKIPs its dependents; every task has its own deadline, for its start as for its run; recorded times order the queue
# (longest path first). The cores a task is counted for are the processors its process tree runs on (an affinity): a process
# that sizes its thread pools by the machine cannot take what was given to others. While tasks are unfinished the run says
# what it is doing (on_progress) and each running task's lines as it writes them (on_line); every listener is called off the
# scheduler's lock (listen.py). One task's own process, verdict and store are execute.py's.
from __future__ import annotations

import json
import threading
import time
import traceback
from pathlib import Path
from typing import Callable

from . import execute, inputs, proc
from .admission import Admission, Grant
from .cache import Cache
from .listen import Delivery
from .machine import Machine
from .runs import RunRecord, check_graph, priorities
from .task import BROKEN, CACHEABLE, CACHED_FAIL, CACHED_PASS, HERE, PASS, ROOT, SKIPPED, TIMEOUT, Result, Task

LOCK_RETRY_S = 0.25  # a task waiting for a lock or a processor another process holds is tried again this often
WAIT_TURN_S = 1.0  # the scheduler wakes at least this often to look at the clock and the locks
PROGRESS_S = 10.0  # while tasks are unfinished, what the run is doing is said this often
UNSTARTED = ("waiting", "keying", "ready")


def own_sources() -> list[str]:
    """The orchestrator's own code, part of every key: a change to how results are judged invalidates them."""
    return sorted(p.as_posix() for p in HERE.glob("*.py"))


class Pool:
    def __init__(self, root: Path | str | None = None, jobs: int | None = None, capacity: dict | None = None,
                 on_result: Callable[[Result], None] | None = None, use_cache: bool = True, label: str = "run",
                 on_finish: Callable[[str, Result], None] | None = None, machine: Machine | None = None,
                 on_progress: Callable[[dict], None] | None = None, progress_s: float = PROGRESS_S,
                 on_line: Callable[[str, str], None] | None = None):
        self.root = Path(root or ROOT).resolve()
        self.on_result = on_result
        self.on_finish = on_finish  # (ident, result): for callers that submit under idents of their own
        self.on_progress = on_progress
        self.on_line = on_line  # (ident, line): each line a running task writes to its log, as it comes
        self.progress_s = progress_s
        self._said = time.monotonic()
        self._turned = time.monotonic()  # the scheduler's last turn: the waits of the ready tasks are counted from it
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
        self.ready_at: dict[str, float] = {}  # when a task became ready
        self.started_at: dict[str, float] = {}  # when it started
        self.waited: dict[str, float] = {}  # the seconds a ready task waited that count against its deadline
        self.record = RunRecord(self.root, label)
        self.admission = Admission(self.root, machine or Machine(), jobs, capacity, self.record)
        self.delivery = Delivery(self._deliver, "tasks-delivery")
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
            self.prio = priorities(self.tasks, self.record.times)
            self.cond.notify_all()
        return ident

    def wait_all(self) -> list[Result]:
        """Every result, once every task is done and every listener has been told of it."""
        with self.cond:
            while any(s != "done" for s in self.state.values()):
                self.cond.wait(WAIT_TURN_S)
            results = [self.results[i] for i in self.order]
        self.delivery.drain()
        return results

    def close(self) -> None:
        with self.cond:
            self.closed = True
            self.cond.notify_all()
        self.thread.join()
        self.delivery.close()
        self.record.close()

    # scheduling -----------------------------------------------------------------------------------------------------
    def _loop(self) -> None:
        while True:
            with self.cond:
                if self.closed:
                    return
                self._advance()
                report = self._report()
                if report is None:
                    self.cond.wait(min(LOCK_RETRY_S if self.admission.retry else WAIT_TURN_S, self.progress_s))
            try:
                if report is not None:
                    self.on_progress(report)  # off the lock: a slow listener delays no start, finish or deadline
            except Exception:
                traceback.print_exc()  # a broken listener is reported; the run itself goes on

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
        now = time.monotonic()
        step, self._turned = now - self._turned, now
        idle = not self.admission.granted  # nothing of this run runs: a ready task waits on what is held outside it
        self.admission.begin()
        place = {i: n for n, i in enumerate(self.order)}
        ready = sorted((i for i in self.order if self.state[i] == "ready"), key=lambda i: (-self.prio.get(i, 0.0), place[i]))
        for i in ready:
            others = sum(self.admission.needs(self.tasks[j])["cores"] for j in self.order if j != i and self.state[j] in UNSTARTED)
            why = self.admission.try_start(i, self.tasks[i], others, self.record.times.get(self.tasks[i].name))
            if why is not None:
                self.why[i] = why
                self._count_wait(i, why, min(step, now - self.ready_at[i]) if idle else 0.0)
                continue
            self.why.pop(i, None)
            self.state[i] = "running"
            self.started_at[i] = now
            threading.Thread(target=self._execute, args=(i,), name=f"run {i}", daemon=True).start()

    def _count_wait(self, i: str, why: str, seconds: float) -> None:
        """Under the condition: a ready task's wait counted against its own deadline. It counts only while no task of this
        run runs: a wait for this run's own tasks is bounded already, each of them ending by its deadline; a wait for what
        another process holds (a lock, the processors of another run) is bounded here. Past it the task ends TIMEOUT."""
        t = self.tasks[i]
        self.waited[i] = self.waited.get(i, 0.0) + seconds
        if self.waited[i] >= t.timeout_s:
            self._finish(i, Result(t.name, TIMEOUT, None, round(self.waited[i], 2), "", self.keys.get(i, ""),
                                   f"never started within its deadline of {t.timeout_s:g} s: waited for {why}"))

    # what the run is doing --------------------------------------------------------------------------------------------
    def progress(self) -> dict:
        """Under the condition: how far the run is, each running task with its seconds, the seconds it took before and the
        memory it holds, and what each task that has not started waits for."""
        now, granted = time.monotonic(), self.admission.granted
        running = [{"task": i, "seconds": round(now - self.started_at[i], 1),
                    "expected": self.record.times.get(self.tasks[i].name),
                    "memory_mb": round(granted[i].peak_mb()) if i in granted else 0,
                    "cores": len(granted[i].cores) if i in granted else 0}  # none once its tree ended: its result is stored
                   for i in self.order if self.state[i] == "running"]
        waiting = {i: self.why.get(i, "its turn") for i in self.order if self.state[i] in UNSTARTED}
        return {"done": sum(s == "done" for s in self.state.values()), "total": len(self.order), "running": running,
                "waiting": waiting, "memory_free_mb": round(self.admission.machine.memory()[0])}

    def _report(self) -> dict | None:
        """Under the condition: what the run is doing, when it is time to say it and a task is unfinished (running or
        waiting, for its turn, a lock, processors or memory)."""
        now = time.monotonic()
        if self.on_progress is None or now - self._said < self.progress_s or all(s == "done" for s in self.state.values()):
            return None
        self._said = now
        return self.progress()

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
            self.ready_at[i] = time.monotonic()
            self.cond.notify_all()

    def _execute(self, i: str) -> None:
        t = self.tasks[i]
        log = self.record.log(i)
        grant = self.admission.granted[i]
        self.record.event({"event": "start", "task": i, "name": t.name, "expected": self.record.times.get(t.name),
                           "cores": sorted(grant.cores), "memory_mb": round(grant.memory_mb)})
        t0 = time.monotonic()
        code, peak, broke = None, 0.0, ""
        try:
            code, peak = execute.run_tree(i, t, log, self.cache, sorted(grant.cores), self.on_line,
                                          lambda tree: self._set_tree(grant, tree))
        except Exception:
            broke = traceback.format_exc()
        seconds = time.monotonic() - t0
        with self.cond:  # the tree ended: its processors and counts go to the next task now, its locks after the store
            try:
                locks = self.admission.release(i, keep_locks=True)
            except Exception:
                locks, broke = [], broke or traceback.format_exc()
            self.cond.notify_all()
        result = Result(t.name, BROKEN, code, round(seconds, 2), str(log), self.keys.get(i, ""), broke)
        try:
            if not broke:
                result.status, result.message = execute.outcome(t, str(log), code, self.cache)
            if result.status in CACHEABLE:
                execute.keep(self.cache, t, result)
            if result.status != BROKEN:
                self.record.took(t.name, seconds, peak)
        except Exception:
            result.status, result.message = BROKEN, traceback.format_exc()
        finally:
            with self.cond:
                try:
                    for lock in locks:
                        lock.release()
                finally:
                    self._finish(i, result, wrote=True)

    def _set_tree(self, grant: Grant, tree: proc.Process | None) -> None:
        with self.cond:
            grant.tree = tree  # what progress() reads the memory of

    def _finish(self, i: str, result: Result, wrote: bool = False) -> None:
        """Under the condition: records the result, hands it to the delivery (listen.py: the listeners are called off this
        lock), wakes the scheduler. `wrote`: a process ran or outputs were restored, so the files the next key reads may
        have changed."""
        self.results[i] = result
        self.state[i] = "done"
        self.why.pop(i, None)
        if wrote:
            self.snapshot.refresh()
        self.record.event({"event": "finish", "task": i, **result.to_json()})
        self.delivery.put(i, result)
        self.cond.notify_all()

    def _deliver(self, i: str, result: Result) -> None:
        if self.on_result:
            self.on_result(result)
        if self.on_finish:
            self.on_finish(i, result)


def run(tasks: list[Task], jobs: int | None = None, on_result: Callable[[Result], None] | None = None, *,
        root: Path | str | None = None, use_cache: bool = True, capacity: dict | None = None, label: str = "run",
        machine: Machine | None = None, on_progress: Callable[[dict], None] | None = None,
        progress_s: float = PROGRESS_S, on_line: Callable[[str, str], None] | None = None) -> list[Result]:
    """Runs the tasks; results in the order given. Raises ValueError on a malformed set (names, dependencies, cycles)."""
    check_graph(tasks)
    pool = Pool(root=root, jobs=jobs, capacity=capacity, on_result=on_result, use_cache=use_cache, label=label,
                machine=machine, on_progress=on_progress, progress_s=progress_s, on_line=on_line)
    try:
        for t in tasks:
            pool.submit(t)
        return pool.wait_all()
    finally:
        pool.close()
