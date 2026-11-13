# One task's own work, off the scheduler's lock (pool.py runs it on a thread of its own and holds what admission granted
# it): its process tree run to its end or its deadline, its lines followed as it writes them (listen.py); what that came to
# (the deadline, the exit code, the task's verdict, verdicts.py, and its declared outputs); its result kept in the cache, where
# a store that fails changes nothing of the verdict.
from __future__ import annotations

import os
import sys
from pathlib import Path
from typing import Callable

from . import proc, verdicts
from .cache import Cache, remove
from .listen import Follower
from .task import BROKEN, FAIL, PASS, TIMEOUT, Result, Task


def run_tree(ident: str, t: Task, log: Path, cache: Cache, cores: list[int], on_line: Callable[[str, str], None] | None,
             on_tree: Callable[[proc.Process | None], None]) -> tuple[int | None, float]:
    """The task's process tree run on `cores` to its end or its deadline: (its exit code, None when it was killed at its
    deadline; the most MiB it held at once). `on_tree` is told of the tree while it runs (and of None once it has ended);
    `on_line` hears each line it writes, the last ones before this returns."""
    if t.fresh:
        for out in t.outputs:
            remove(cache.output_path(out))
    cwd = t.cwd if t.cwd is None or os.path.isabs(t.cwd) else str(cache.root / t.cwd)
    follower = None
    try:
        with proc.Process(t.argv, cwd=cwd or str(cache.root), env=t.env, log=log, hidden=t.hidden, cores=cores) as tree:
            follower = Follower(ident, log, on_line) if on_line else None
            on_tree(tree)
            try:
                return tree.wait(t.timeout_s), tree.peak_memory_mb()
            finally:
                on_tree(None)
    finally:
        if follower is not None:
            follower.stop()  # after the whole tree is gone: what it wrote last is said before its result


def outcome(t: Task, log: str, code: int | None, cache: Cache) -> tuple[str, str]:
    """What a finished process came to: TIMEOUT at its deadline, BROKEN for an exit that is neither 0 nor 1, else its
    verdict, a PASS whose declared outputs are not all there failing."""
    if code is None:
        return TIMEOUT, f"killed at its deadline of {t.timeout_s:g} s"
    if code not in (0, 1):
        return BROKEN, f"exit {code}"
    status, message = verdicts.judge(t, log, code, cache.root)
    gone = [o for o in t.outputs if not cache.output_path(o).exists()] if status == PASS else []
    return (FAIL, f"declared output missing: {', '.join(gone)}") if gone else (status, message)


def keep(cache: Cache, t: Task, result: Result) -> None:
    """The result stored in the cache. A store that fails changes nothing of the verdict (what the task did stands; only its
    reuse by a later run is lost): the message says so and one line goes to stderr."""
    try:
        cache.store(t.name, result.key, result, t.outputs)
    except Exception as e:
        why = f"{type(e).__name__}: {e}"
        result.message = f"{result.message} (not kept in the cache: {why})".lstrip()
        sys.stderr.write(f"{t.name}: not kept in the cache: {why}\n")
        sys.stderr.flush()
