# The task store's bound (tools/tasks/suites.json lifecycle, max_mb), kept on every write: each entry the store writes is added
# to its ledger, and when the total passes the bound the least recently used entries go (a hit touches its entry: an entry's
# time of use is its mtime). An entry is a task's result folder, a memo's file or a materialized folder.
from __future__ import annotations

import os
import time
from pathlib import Path

from .files import remove, size_of, write_atomic
from .locks import FileLock

COMPLETE = ".complete"  # the mark of a materialized folder built whole
LEDGER = ".ledger"  # in the store: one line an entry written, "<bytes> <entry relative to the store>"; an entry's last line wins
TOTAL = ".total"  # in the store: the bytes the ledger's entries hold, kept with it
LEDGER_LOCK = ".ledger.lock"
LEDGER_WAIT_S = 60  # another writer holds the ledger only for an append or an eviction; past this the write fails, never waits on
POLL_S = 0.1
KEEP_SHARE = 0.9  # an eviction goes down to this share of the bound, so the writes after it do not evict again at once
YOUNG_S = 600  # an entry used this recently is never evicted: a task may be restoring it right now


def entries_on_disk(base: Path):
    """Every entry the store holds: a task's result folder, a memo's file, a materialized folder."""
    for tool in (p for p in base.iterdir() if p.is_dir()) if base.is_dir() else []:
        if tool.name == "_materialized":
            yield from (d for d in tool.iterdir() if d.is_dir() and (d / COMPLETE).is_file())
        for prefix in (p for p in tool.iterdir() if p.is_dir() and len(p.name) == 2):
            yield from (d for d in prefix.iterdir() if d.is_dir() and (d / "result.json").is_file())
        memo_dir = tool / "memo"
        if memo_dir.is_dir():
            yield from (f for prefix in memo_dir.iterdir() if prefix.is_dir() for f in prefix.iterdir() if f.suffix == ".json")


def used(path: Path) -> None:
    """A hit: the entry's time of use is now."""
    try:
        os.utime(path)
    except OSError:  # evicted by another process between the read and now: the read already holds its content
        pass


class Ledger:
    """What the store holds, in bytes, kept by every writer: the one place its bound is held."""

    def __init__(self, base: Path, max_bytes: int):
        self.base, self.max_bytes = base, max_bytes

    def hold(self) -> FileLock:
        lock = FileLock(self.base / LEDGER_LOCK)
        deadline = time.monotonic() + LEDGER_WAIT_S
        while not lock.try_acquire(shared=False):
            if time.monotonic() > deadline:
                raise TimeoutError(f"{self.base / LEDGER}: another writer held it past {LEDGER_WAIT_S} s")
            time.sleep(POLL_S)
        return lock

    def total(self) -> int:
        """The total, the ledger written from the disk the first time (a store kept before the ledger existed). Called held."""
        total = self.base / TOTAL
        if total.is_file():
            return int(total.read_text(encoding="utf-8") or 0)
        rows = [(e, size_of(e)) for e in entries_on_disk(self.base)]
        (self.base / LEDGER).write_bytes("".join(f"{n} {e.relative_to(self.base).as_posix()}\n" for e, n in rows).encode())
        return sum(n for _e, n in rows)

    def add(self, entry: Path) -> None:
        """An entry written: counted, and the least recently used ones evicted when the total passes the bound."""
        size = size_of(entry)
        self.base.mkdir(parents=True, exist_ok=True)
        with self.hold():
            counted = (self.base / TOTAL).is_file()  # else the ledger is written from the disk now, this entry with the rest
            total = self.total()
            if counted:
                total += size
                with open(self.base / LEDGER, "ab") as fh:
                    fh.write(f"{size} {entry.relative_to(self.base).as_posix()}\n".encode())
            if total > self.max_bytes:
                total = self.evict(KEEP_SHARE * self.max_bytes)
            write_atomic(self.base / TOTAL, str(total))

    def evict(self, down_to: float) -> int:
        """The least recently used entries removed until the total is `down_to` or less (entries used in the last YOUNG_S
        kept); the ledger rewritten with what is left. Called held. Returns the total."""
        sizes = {}
        for line in (self.base / LEDGER).read_bytes().decode().splitlines():
            n, _, rel = line.partition(" ")
            sizes[rel] = int(n)
        alive = [(rel, n, (self.base / rel).stat().st_mtime) for rel, n in sizes.items() if (self.base / rel).exists()]
        total = sum(n for _r, n, _t in alive)
        now = time.time()
        for rel, n, last in sorted(alive, key=lambda r: r[2]):
            if total <= down_to or now - last < YOUNG_S:
                break
            remove(self.base / rel)
            sizes.pop(rel)
            total -= n
        kept = [(rel, sizes[rel]) for rel, _n, _t in alive if rel in sizes]
        (self.base / LEDGER).write_bytes("".join(f"{n} {rel}\n" for rel, n in kept).encode())
        return total

    def settle(self, down_to: float) -> int:
        """The bound applied now (cli.py clean): the ledger written from the disk if none was, evicted, its total kept."""
        self.base.mkdir(parents=True, exist_ok=True)
        with self.hold():
            self.total()
            total = self.evict(down_to)
            write_atomic(self.base / TOTAL, str(total))
        return total
