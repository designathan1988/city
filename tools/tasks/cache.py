# The result store, keyed by content: task results with their logs and outputs (dist/cache/tasks/<task>/<key[:2]>/<key>/)
# and in-process sub-results of tools (memo). Every entry written is handed to the store's ledger (ledger.py), which keeps the
# store inside its bound; every hit marks its entry used. Also the owner of dist/held: a file another process holds (an open
# game's DLL) is set aside there, never overwritten (place, free_dlls), and that folder is held to its bound.
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

from . import outputs
from .files import remove, size_of, tmp_path, write_atomic
from .ledger import Ledger, used
from .task import ROOT, Result

__all__ = ["STORE", "HELD", "Cache", "memo", "remove", "write_atomic", "safe", "place", "set_aside", "free_dlls",
           "remove_unless_held", "ledger", "size_of"]

STORE = "dist/cache/tasks"
HELD = "dist/held"  # a file another process holds (an open game's DLL) is moved aside here, never overwritten
BIN = "project/bin"  # the libraries the game loads: an open game holds them (free_dlls)


def safe(name: str) -> str:
    return re.sub(r"[^A-Za-z0-9_.-]", "_", name)


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


def place(src: Path, dst: Path, root: Path) -> None:
    """Puts a copy of `src` at `dst`. A file already equal stays; a file held by another process is moved aside first."""
    if src.is_dir():
        remove(dst)
        shutil.copytree(src, dst)
        return
    if _same_file(src, dst):
        return
    dst.parent.mkdir(parents=True, exist_ok=True)
    tmp = tmp_path(dst)
    shutil.copy2(src, tmp)
    try:
        os.replace(tmp, dst)
    except PermissionError:
        set_aside(dst, root)
        os.replace(tmp, dst)


def set_aside(path: Path, root: Path) -> Path:
    """Moves a file another process may hold into HELD (Windows lets a file in use be renamed, never replaced nor deleted:
    the process goes on with it there) under a name no other move takes, `<name>.<pid>.<time_ns>`; returns where it went."""
    aside = Path(root) / HELD / f"{path.name}.{os.getpid()}.{time.time_ns()}"
    aside.parent.mkdir(parents=True, exist_ok=True)
    os.replace(path, aside)
    return aside


def free_dlls(root: Path | str) -> None:
    """Every DLL of project/bin set aside and copied back to its place, so a link may write over the copy while an open game
    goes on with the one set aside (tools/build/free_dll.sh, before each link of build.sh); then HELD held to its bound. A move
    or a copy that fails raises: the link that follows would fail on the held file."""
    root = Path(root).resolve()
    for dll in sorted((root / BIN).glob("*.dll")):
        shutil.copy2(set_aside(dll, root), dll)
    _hold_to_bound(root)


def _hold_to_bound(root: Path) -> None:
    """HELD's bound (suites.json lifecycle, keep): the files set aside last kept, the older removed, oldest first; one a
    process still holds stays, and is said."""
    keep = outputs.bound(HELD)["keep"]
    base = root / HELD
    held = sorted((p for p in base.iterdir() if p.is_file()), key=_set_aside_at) if base.is_dir() else []
    for old in held[:max(0, len(held) - keep)]:
        remove_unless_held(old, root)


def _set_aside_at(path: Path) -> int:
    """When a held file was set aside, in ns: the time its name carries (set_aside), else its modification time. A rename
    keeps a file's own time, which is when it was built, not when it was set aside."""
    last = path.name.rsplit(".", 1)[-1]
    return int(last) if last.isdigit() else path.stat().st_mtime_ns


def remove_unless_held(path: Path, root: Path) -> bool:
    """Removes a file or folder; False, and said ("kept, in use: <path>"), when a running process holds a file of it (an open
    game's DLL: Windows refuses to delete it, PermissionError): that file stays, what else could go is gone. Any other
    failure propagates."""
    try:
        remove(path)
        return True
    except PermissionError as e:
        held = Path(e.filename) if e.filename else path
        print(f"kept, in use: {held.relative_to(root).as_posix() if held.is_relative_to(root) else held}", flush=True)
        return False


_ledgers: dict[Path, Ledger] = {}


def ledger(root: Path | str) -> Ledger:
    """The store's ledger of a repository, its bound (suites.json lifecycle) read once a process."""
    root = Path(root).resolve()
    if root not in _ledgers:
        _ledgers[root] = Ledger(root / STORE, outputs.bound(STORE)["max_mb"] * outputs.MB)
    return _ledgers[root]


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
        used(self.entry(name, key))
        return json.loads(doc.read_text(encoding="utf-8"))

    def output_path(self, out: str) -> Path:
        p = Path(out)
        return p if p.is_absolute() else self.root / p

    def store(self, name: str, key: str, result: Result, outputs: list[str]) -> None:
        """The result kept under its key with its log and outputs, counted by the ledger. Raises when it could not be kept,
        and then nothing of it stays: no half-written folder, no entry the ledger does not count."""
        final = self.entry(name, key)
        if (final / "result.json").is_file():
            return
        tmp = tmp_path(final)
        remove(tmp)
        try:
            self._fill(tmp, result, outputs)
            final.parent.mkdir(parents=True, exist_ok=True)
            try:
                os.replace(tmp, final)
            except OSError:
                if not (final / "result.json").is_file():  # another process stored the same key first: the same result
                    raise
                return
        finally:
            remove(tmp)
        try:
            ledger(self.root).add(final)
        except BaseException:
            remove(final)  # an entry the ledger does not count would let the store pass its bound
            raise

    def _fill(self, tmp: Path, result: Result, outputs: list[str]) -> None:
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
    base = Path(root or ROOT).resolve()
    path = base / STORE / safe(tool) / "memo" / key[:2] / f"{key}.json"
    if path.is_file():
        value = json.loads(path.read_text(encoding="utf-8"))
        used(path)
    else:
        value = fn()
        write_atomic(path, json.dumps(value, ensure_ascii=False))
        ledger(base).add(path)
        value = json.loads(json.dumps(value))  # the same shape as a value read back (tuples become lists)
    with _memo_lock:
        _memo[(tool, key)] = value
    return value
