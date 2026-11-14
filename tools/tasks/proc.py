# Starts and stops processes by their own tree, so a deadline kills everything a task started and nothing else. Windows: a
# Job Object per process (win32.py), hidden=True on the private desktop whose windows never reach the user's screen. POSIX:
# a session of its own, killed by process group (hidden=True under xvfb-run when there is one).
from __future__ import annotations

import os
import shlex
import shutil
import subprocess
import time
from pathlib import Path

if os.name == "nt":
    from . import win32

# For the short helper calls made through subprocess (git, a symbolizer): a console program started from a process with no
# console would open a console window of its own on the user's screen.
NO_WINDOW = subprocess.CREATE_NO_WINDOW if os.name == "nt" else 0
KILL_WAIT_S = 30  # after the kill, the wait for the tree to be gone; a tree that outlives it is an error
_LIVE: set = set()  # the trees this process started and has not closed: what a run that ends at its deadline kills (kill_live)


class Process:
    """A started process tree; its deadline is its waiter's (wait), and close() kills what is left of it."""

    def __init__(self, argv: list[str] | str, *, cwd: str | Path | None = None, env: dict | None = None,
                 log: str | Path | None = None, hidden: bool = False, cores: list[int] | None = None):
        self.started = time.monotonic()
        self._done = False
        log_file = open(log, "wb") if log else open(os.devnull, "wb")
        try:
            if os.name == "nt":
                self._tree = win32.JobTree(argv, cwd, env, log_file, hidden, cores)
                self.pid = self._tree.pid
            else:
                self._start_posix(argv, cwd, env, hidden, log_file, cores)
        finally:
            log_file.close()  # the child has its own handle
        _LIVE.add(self)

    def _start_posix(self, argv, cwd, env, hidden, log_file, cores):
        args = shlex.split(argv) if isinstance(argv, str) else list(argv)
        if hidden and shutil.which("xvfb-run"):
            args = ["xvfb-run", "-a"] + args
        merged = dict(os.environ)
        merged.update({k: str(v) for k, v in (env or {}).items()})
        pin = (lambda: os.sched_setaffinity(0, cores)) if cores and hasattr(os, "sched_setaffinity") else None
        self._p = subprocess.Popen(args, cwd=cwd, env=merged, stdin=subprocess.DEVNULL, stdout=log_file,
                                   stderr=subprocess.STDOUT, start_new_session=True, preexec_fn=pin)
        self.pid = self._p.pid

    def wait(self, timeout_s: float | None) -> int | None:
        """The exit code of the process (not its tree), or None while it still runs at `timeout_s`."""
        if os.name == "nt":
            return self._tree.wait(timeout_s)
        try:
            return self._p.wait(timeout_s)
        except subprocess.TimeoutExpired:
            return None

    def peak_memory_mb(self) -> float:
        """The most memory the tree held at once so far, MiB; 0 where the platform keeps no such figure for a tree."""
        if os.name == "nt" and not self._done:
            return self._tree.peak_memory_mb()
        return 0.0

    def kill_tree(self) -> None:
        """Kills the process and everything it started (only that: the tree is known by its job or its process group)."""
        if self._done:
            return
        if os.name == "nt":
            self._tree.kill()
            return
        import signal
        try:
            os.killpg(self.pid, signal.SIGKILL)
        except ProcessLookupError:
            pass  # the group is already gone: the state the kill asks for
        if self._p.poll() is None:
            self._p.wait(KILL_WAIT_S)

    def close(self) -> None:
        """Kills what is left of the tree and frees the handles."""
        if self._done:
            return
        self.kill_tree()
        self._done = True
        _LIVE.discard(self)
        if os.name == "nt":
            self._tree.close()

    def __enter__(self):
        return self

    def __exit__(self, *exc):
        self.close()


def run(argv: list[str] | str, timeout_s: float, *, cwd=None, env=None, log=None, hidden=False,
        cores: list[int] | None = None) -> tuple[int | None, bool]:
    """(exit code, timed out). On the deadline the whole tree is killed; after a normal exit, what the process left
    running is killed too (a task ends with its tree). `cores`: the logical processors the tree may run on."""
    with Process(argv, cwd=cwd, env=env, log=log, hidden=hidden, cores=cores) as p:
        code = p.wait(timeout_s)
        return code, code is None


def kill_live() -> list[int]:
    """Kills every tree this process started and has not closed, and names them (their process ids): a run that ends at
    its own deadline (deadline.py) leaves nothing running. A tree that cannot be killed is named with the others; the
    caller ends the process, and on Windows the job that held the tree ends it then (win32.py, KILL_ON_JOB_CLOSE)."""
    killed = []
    for tree in list(_LIVE):
        killed.append(tree.pid)
        try:
            tree.kill_tree()
        except OSError:
            continue  # the next tree is still killed; this one ends with its job when the process does
    return killed


def usable_cores() -> list[int]:
    """The logical processors this process may run on: what a run has to hand out."""
    if os.name == "nt":
        return win32.usable_cores()
    if hasattr(os, "sched_getaffinity"):
        return sorted(os.sched_getaffinity(0))
    return list(range(os.cpu_count() or 1))


def memory_mb() -> tuple[float, float]:
    """(MiB of physical memory available now, MiB in all) on this machine."""
    if os.name == "nt":
        return win32.memory_mb()
    page = os.sysconf("SC_PAGE_SIZE") / (1 << 20)
    return os.sysconf("SC_AVPHYS_PAGES") * page, os.sysconf("SC_PHYS_PAGES") * page


def alive(pid: int) -> bool:
    """Whether a process id names a running process (for checks on trees this module started)."""
    if os.name == "nt":
        return win32.alive(pid)
    try:
        os.kill(pid, 0)
    except ProcessLookupError:
        return False
    return True
