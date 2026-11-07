"""Runs the tasks of a selfcheck side by side and answers for each one. A task lives in a process of its own, so a crash or a hang costs that task and
nothing else: what is ready starts the moment the machine has room, the longest known first, none waits behind another, and each end is told at once.

No task has a time limit. A hang is told from work by the task's own life: a process tree that has sat still for longer than it ever worked is not working.
It is stopped, and the last stacks it wrote are the report."""
import os
import subprocess
import sys
import threading
import time

import psutil

from ..core import read_json, write_json

LOOK_EVERY = 1
LOOKS_TO_TELL = 30
SHOW_EVERY = 5
CHILD_MEMORY = 400 * 2 ** 20
BASE_MEMORY = 2 * 2 ** 30  # a task that reads this repository, until a first run says what it really took


class Job:
    def __init__(self, name, kind, spec, check, heavy=False, env=None):
        self.name, self.kind, self.spec, self.check, self.heavy, self.env = name, kind, spec, check, heavy, env or {}


class _Running:
    def __init__(self, job, process, cores):
        self.job, self.process, self.cores = job, process, cores
        self.started = self.moved = self.looked = time.time()
        self.spent = 0.0
        self.peak = 0
        self.late = False


def _tree(pid):
    try:
        top = psutil.Process(pid)
        return [top, *top.children(recursive=True)]
    except psutil.Error:
        return []


def _measure(pid):
    spent, memory = 0.0, 0
    for p in _tree(pid):
        try:
            t = p.cpu_times()
            spent += t.user + t.system
            memory += p.memory_info().rss
        except psutil.Error:
            continue
    return spent, memory


class Supervisor:
    def __init__(self, ctx, finished, base_memory):
        self.ctx, self.finished = ctx, finished
        self.base_memory = max(base_memory, BASE_MEMORY)
        self.waiting, self.closed, self.lock, self.shown = [], False, threading.Lock(), 0.0
        self.record = os.path.join(ctx.home, "selfcheck_times.json")
        self.known = read_json(self.record, {})
        self.cpus = os.cpu_count() or 1

    def _seconds(self, job):
        return self.known.get(job.name, [float("inf"), 0])[0]

    def _cores(self, job):
        return max(1, self.cpus // 2) if job.heavy else max(1, self.cpus // 8)

    def _memory_of(self, job):
        seen = self.known.get(job.name, [0, 0])[1]
        return seen or self.base_memory + (self._cores(job) * CHILD_MEMORY if job.heavy else 0)

    def _start(self, job):
        out = os.path.join(self.ctx.store, "jobs", job.name)
        env = dict(os.environ, ARCH_CORES=str(self._cores(job)), **job.env)
        with open(out + ".log", "wb") as log:
            process = subprocess.Popen((sys.executable, sys.argv[0], "selfcheck", self.ctx.path, "--job", job.name), stdout=log, stderr=subprocess.STDOUT, env=env)
        if not self.ctx.ready.is_set() and hasattr(psutil, "BELOW_NORMAL_PRIORITY_CLASS"):
            psutil.Process(process.pid).nice(psutil.BELOW_NORMAL_PRIORITY_CLASS)  # the scan everything else needs goes first
        self.ctx.say(f"START {job.name}")
        return _Running(job, process, self._cores(job))

    def _promised(self, running):
        """What started tasks will still ask for: free memory now says nothing about a process that is only beginning."""
        scan = 0 if self.ctx.ready.is_set() else self.base_memory + self.cpus * CHILD_MEMORY
        return scan + sum(max(self._memory_of(r.job) - r.peak, 0) for r in running)

    def _admit(self, waiting, running):
        """Every task starts now: the system shares the processors among them. Only memory that does not exist holds a task back, and then only that one:
        what counts is what the system can still commit, the limit a process really dies at."""
        for job in list(waiting):
            free = psutil.virtual_memory().available + psutil.swap_memory().free - self._promised(running)
            if not running or free > self._memory_of(job):
                waiting.remove(job)
                running.append(self._start(job))

    def _stop(self, r):
        for p in reversed(_tree(r.process.pid)):
            try:
                p.kill()
            except psutil.Error:
                continue

    def _why_stopped(self, r, reason):
        path = os.path.join(self.ctx.store, "jobs", r.job.name + ".stack")
        stacks = ""
        if os.path.exists(path):
            with open(path, encoding="utf8", errors="replace") as f:
                stacks = f.read()[-1500:]
        return f"{reason}: {stacks}"

    def _look(self, r, now, running):
        """Returns why a task must stop, if it must."""
        r.looked = now
        spent, memory = _measure(r.process.pid)
        r.peak = max(r.peak, memory)
        if spent != r.spent:  # a pool that closes takes its processes' time with it: any change is life
            r.spent, r.moved = spent, now
        usual = self._seconds(r.job)
        if not r.late and now - r.started > 2 * usual:
            r.late = True
            self.ctx.say(f"LATE  {r.job.name}: {round(now - r.started)} s, usually {usual} s; nothing waits for it")
        still = now - r.moved
        # While others work, a process that sits still may only be waiting for the machine: a hang shows once nothing else moves after it did.
        # Nor can a few looks tell a hang from a read of the disk, which costs no processor.
        if r.spent and still > max(r.moved - r.started, LOOKS_TO_TELL * LOOK_EVERY) and not any(o is not r and o.moved > r.moved for o in running):
            return self._why_stopped(r, f"still for {round(still)} s after working {round(r.moved - r.started)} s")
        return None

    def _end(self, r, failure):
        job = r.job
        seconds = round(time.time() - r.started)
        if failure is None and r.process.returncode != 0:
            with open(os.path.join(self.ctx.store, "jobs", job.name + ".log"), encoding="utf8", errors="replace") as f:
                failure = f"exit {r.process.returncode}: {f.read()[-1500:]}"
        if failure is None:
            self.known[job.name] = [seconds, r.peak]
        self.ctx.say(f"{'END  ' if failure is None else 'BROKE'} {job.name} {seconds} s {failure or ''}")
        self.finished(job, failure)

    def _show(self, running, now):
        """The state of everything, as it is, for whoever follows the run: what works, for how long, what it spent, what sits still and what waits."""
        self.shown = now
        rows = [f"{r.job.name} {round(now - r.started)}s cpu {round(r.spent)}s {round(r.peak / 2 ** 30, 1)}GB" + (f" STILL {round(now - r.moved)}s" if now - r.moved > 2 * LOOK_EVERY else "") for r in running]
        self.ctx.say(f"NOW   {len(running)} running, {len(self.waiting)} waiting {[j.name for j in self.waiting]}: " + "; ".join(rows))

    def add(self, jobs):
        with self.lock:
            self.waiting = sorted(self.waiting + list(jobs), key=lambda j: (-self._seconds(j), j.name))

    def close(self):
        self.closed = True

    def run(self):
        running = []
        while self.waiting or running or not self.closed:
            with self.lock:
                self._admit(self.waiting, running)
            time.sleep(0.2)
            now = time.time()
            if now - self.shown >= SHOW_EVERY:
                self._show(running, now)
            for r in list(running):
                if r.process.poll() is not None:
                    running.remove(r)
                    self._end(r, None)
                elif now - r.looked >= LOOK_EVERY:
                    why = self._look(r, now, running)
                    if why:
                        self._stop(r)
                        r.process.wait()
                        running.remove(r)
                        self._end(r, why)
        write_json(self.record, self.known)
