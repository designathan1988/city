# The records of a run: its folder under dist/tasks (one log per task, the event stream events.jsonl, latest.txt naming the
# newest run), the times recorded per task name (dist/tasks/times.json, used only to order the queue), and the check of a
# task set's shape before it runs (names, dependencies, cycles).
from __future__ import annotations

import json
import os
import threading
import time
from datetime import datetime
from pathlib import Path

from . import proc
from .cache import remove, safe, write_atomic

RUNS = "dist/tasks"
TIMES = "dist/tasks/times.json"
KEPT_RUNS = 30  # run folders (logs, events) kept; results live on in the cache


class RunRecord:
    def __init__(self, root: Path, label: str):
        self.root = root
        stamp = datetime.now().strftime("%Y%m%d-%H%M%S")
        self.dir = root / RUNS / f"{stamp}-{os.getpid()}-{safe(label)}"
        self.dir.mkdir(parents=True, exist_ok=True)
        write_atomic(root / RUNS / "latest.txt", self.dir.name)
        self._prune()
        self._events = open(self.dir / "events.jsonl", "a", encoding="utf-8")
        self._lock = threading.Lock()
        path = root / TIMES
        self.times: dict[str, float] = json.loads(path.read_text(encoding="utf-8")) if path.is_file() else {}

    def log(self, ident: str) -> Path:
        return self.dir / f"{safe(ident)}.log"

    def event(self, doc: dict) -> None:
        line = json.dumps({"t": round(time.time(), 3), **doc}, ensure_ascii=False)
        with self._lock:
            self._events.write(line + "\n")
            self._events.flush()

    def took(self, name: str, seconds: float) -> None:
        with self._lock:
            self.times[name] = seconds
            write_atomic(self.root / TIMES, json.dumps(self.times, indent=1, sort_keys=True))

    def close(self) -> None:
        self._events.close()

    def _prune(self) -> None:
        """The oldest run folders past KEPT_RUNS go, except those of a run whose process still lives."""
        runs = sorted(p for p in (self.root / RUNS).iterdir() if p.is_dir() and p.name[:1].isdigit())
        for old in runs[:-KEPT_RUNS]:
            parts = old.name.split("-")
            if len(parts) > 2 and parts[2].isdigit() and proc.alive(int(parts[2])):
                continue
            remove(old)


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
