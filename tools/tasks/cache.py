# The result store, keyed by content: task results with their logs and outputs (dist/cache/tasks/<task>/<key[:2]>/<key>/),
# in-process sub-results of tools (memo), and shared directories built once under a lock (materialize).
from __future__ import annotations

import hashlib
import json
import os
import re
import shutil
import threading
import time
from pathlib import Path
from typing import Callable

from .locks import FileLock
from .task import ROOT, Result

STORE = "dist/cache/tasks"
HELD = "dist/held"  # a file another process holds (an open game's DLL) is moved aside here, never overwritten
LOCK_POLL_S = 0.1  # how often a materialize waiting for another builder looks again, inside its deadline
COMPLETE = ".complete"


def safe(name: str) -> str:
    return re.sub(r"[^A-Za-z0-9_.-]", "_", name)


def _tmp(path: Path) -> Path:
    return path.with_name(f"{path.name}.tmp-{os.getpid()}-{threading.get_ident()}")


def write_atomic(path: Path, text: str) -> None:
    path.parent.mkdir(parents=True, exist_ok=True)
    tmp = _tmp(path)
    tmp.write_text(text, encoding="utf-8")
    os.replace(tmp, path)


def _same_file(a: Path, b: Path) -> bool:
    if not (a.is_file() and b.is_file()) or a.stat().st_size != b.stat().st_size:
        return False
    return _digest(a) == _digest(b)


def _digest(p: Path) -> str:
    h = hashlib.sha256()
    with open(p, "rb") as f:
        for chunk in iter(lambda: f.read(1 << 20), b""):
            h.update(chunk)
    return h.hexdigest()


def remove(path: Path) -> None:
    if path.is_dir() and not path.is_symlink():
        shutil.rmtree(path)
    elif path.exists() or path.is_symlink():
        path.unlink()


def place(src: Path, dst: Path, root: Path) -> None:
    """Puts a copy of `src` at `dst`. A file already equal stays; a file held by another process is moved aside first."""
    if src.is_dir():
        remove(dst)
        shutil.copytree(src, dst)
        return
    if _same_file(src, dst):
        return
    dst.parent.mkdir(parents=True, exist_ok=True)
    tmp = _tmp(dst)
    shutil.copy2(src, tmp)
    try:
        os.replace(tmp, dst)
    except PermissionError:
        aside = root / HELD / f"{dst.name}.{os.getpid()}.{time.time_ns()}"
        aside.parent.mkdir(parents=True, exist_ok=True)
        os.replace(dst, aside)
        os.replace(tmp, dst)


class Cache:
    def __init__(self, root: Path | str | None = None):
        self.root = Path(root or ROOT).resolve()
        self.base = self.root / STORE

    def entry(self, name: str, key: str) -> Path:
        return self.base / safe(name) / key[:2] / key

    def lookup(self, name: str, key: str) -> dict | None:
        doc = self.entry(name, key) / "result.json"
        if not doc.is_file():
            return None
        return json.loads(doc.read_text(encoding="utf-8"))

    def output_path(self, out: str) -> Path:
        p = Path(out)
        return p if p.is_absolute() else self.root / p

    def store(self, name: str, key: str, result: Result, outputs: list[str]) -> None:
        final = self.entry(name, key)
        if (final / "result.json").is_file():
            return
        tmp = _tmp(final)
        remove(tmp)
        tmp.mkdir(parents=True)
        if result.log and Path(result.log).is_file():
            shutil.copy2(result.log, tmp / "log.txt")
        saved = []
        for i, out in enumerate(outputs):
            src = self.output_path(out)
            if src.exists():
                dst = tmp / "outputs" / str(i)
                dst.parent.mkdir(parents=True, exist_ok=True)
                (shutil.copytree if src.is_dir() else shutil.copy2)(src, dst)
                saved.append(out)
        doc = {"status": result.status, "exit_code": result.exit_code, "seconds": result.seconds, "message": result.message,
               "outputs": outputs, "saved": saved}
        (tmp / "result.json").write_text(json.dumps(doc, indent=1), encoding="utf-8")
        final.parent.mkdir(parents=True, exist_ok=True)
        try:
            os.replace(tmp, final)
        except OSError:
            if not (final / "result.json").is_file():  # another process stored the same key first: that is the same result
                raise
            remove(tmp)

    def restore(self, name: str, key: str, doc: dict) -> str:
        """Puts the cached outputs back; returns the cached log's path."""
        entry = self.entry(name, key)
        for i, out in enumerate(doc["outputs"]):
            if out in doc["saved"]:
                place(entry / "outputs" / str(i), self.output_path(out), self.root)
        log = entry / "log.txt"
        return str(log) if log.is_file() else ""


_memo: dict[tuple[str, str], object] = {}
_memo_lock = threading.Lock()


def memo(tool: str, key: str, fn: Callable[[], object], root: Path | str | None = None):
    """fn()'s value (JSON) kept under (tool, key): computed once per key, in this process and on disk."""
    with _memo_lock:
        if (tool, key) in _memo:
            return _memo[(tool, key)]
    path = Path(root or ROOT).resolve() / STORE / safe(tool) / "memo" / key[:2] / f"{key}.json"
    if path.is_file():
        value = json.loads(path.read_text(encoding="utf-8"))
    else:
        value = fn()
        write_atomic(path, json.dumps(value, ensure_ascii=False))
        value = json.loads(json.dumps(value))  # the same shape as a value read back (tuples become lists)
    with _memo_lock:
        _memo[(tool, key)] = value
    return value


def store_path(tool: str, name: str, root: Path | str | None = None) -> Path:
    """A file of a tool's own in the store (a table it updates itself, such as symbols by module)."""
    return Path(root or ROOT).resolve() / STORE / safe(tool) / name


def materialize(key: str, build: Callable[[Path], None], timeout_s: float, root: Path | str | None = None) -> Path:
    """The directory for `key`, built once by `build(path)` under an OS file lock. Waiting for another builder ends at
    `timeout_s` with TimeoutError; a failing build raises, and its partial directory is not kept."""
    base = Path(root or ROOT).resolve() / STORE / "_materialized"
    final = base / key
    if (final / COMPLETE).is_file():
        return final
    lock = FileLock(base / f"{key}.lock")
    deadline = time.monotonic() + timeout_s
    while not lock.try_acquire(shared=False):
        if time.monotonic() > deadline:
            raise TimeoutError(f"materialize {key}: another builder held the lock past {timeout_s} s")
        time.sleep(LOCK_POLL_S)
    try:
        if (final / COMPLETE).is_file():
            return final
        tmp = _tmp(final)
        remove(tmp)
        tmp.mkdir(parents=True)
        try:
            build(tmp)
            (tmp / COMPLETE).write_text(key, encoding="utf-8")
            remove(final)
            os.replace(tmp, final)
        except BaseException:
            remove(tmp)
            raise
        return final
    finally:
        lock.release()
