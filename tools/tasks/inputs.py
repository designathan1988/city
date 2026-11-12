# Content keys and change scope: the one owner of "what a file holds" (git blob ids, never modification times) and of "what
# the worktree changed" (git status). Every tool that keys a result or scopes a change asks here.
from __future__ import annotations

import hashlib
import json
import os
import re
import subprocess
import threading
from pathlib import Path

from .proc import NO_WINDOW
from .task import ROOT

GIT_TIMEOUT_S = 120  # one git listing or hashing call; a git that hangs is an error, not a wait
GITLINK = "160000"  # a submodule entry: a commit id, not a file


def git(root: Path | str, *args: str, stdin: str | None = None) -> str:
    """git's answer, text: the one wrapper of the tools (a deadline, no console window, no optional locks).

    git's input is its own: the text given, or nothing (the null device), never the caller's. Under `cli.py serve` the
    caller's input is the protocol pipe, with serve's own read of it pending; a git that inherited it blocked in its start
    (Git for Windows, compat/winansi.c detect_msys_tty: with stdout and stderr not a console it asks the name of a piped
    stdin through NtQueryObject, which waits for the pending read) until the next request came or its deadline passed."""
    feed = {"input": stdin} if stdin is not None else {"stdin": subprocess.DEVNULL}
    r = subprocess.run(["git", "--no-optional-locks", "-C", str(root), *args], capture_output=True,
                       text=True, encoding="utf-8", errors="surrogateescape", timeout=GIT_TIMEOUT_S,
                       creationflags=NO_WINDOW, **feed)
    if r.returncode != 0:
        raise RuntimeError(f"git {' '.join(args)} failed ({r.returncode}): {r.stderr.strip()}")
    return r.stdout


def git_bytes(root: Path | str, *args: str) -> bytes:
    """git's answer as bytes, for content whose bytes matter (a blob's line endings): the text form above reads with universal
    newlines and would turn every CRLF into LF. Same wrapper otherwise: a deadline, no console window, no optional locks."""
    r = subprocess.run(["git", "--no-optional-locks", "-C", str(root), *args], capture_output=True, stdin=subprocess.DEVNULL,
                       timeout=GIT_TIMEOUT_S, creationflags=NO_WINDOW)
    if r.returncode != 0:
        raise RuntimeError(f"git {' '.join(args)} failed ({r.returncode}): {r.stderr.decode('utf-8', 'replace').strip()}")
    return r.stdout


def blob_id(path: Path) -> str:
    """The git blob id of a file's bytes, for files git does not track (ignored outputs, paths outside the repo)."""
    h = hashlib.sha1()
    data = path.read_bytes()
    h.update(b"blob %d\0" % len(data))
    h.update(data)
    return h.hexdigest()


def glob_regex(pattern: str) -> re.Pattern:
    """`**` any depth, `*` and `?` inside one path segment; matched against '/'-separated paths."""
    out, i = [], 0
    while i < len(pattern):
        c = pattern[i]
        if pattern.startswith("**/", i):
            out.append("(?:.*/)?")
            i += 3
        elif pattern.startswith("**", i):
            out.append(".*")
            i += 2
        elif c == "*":
            out.append("[^/]*")
            i += 1
        elif c == "?":
            out.append("[^/]")
            i += 1
        else:
            out.append(re.escape(c))
            i += 1
    return re.compile("".join(out) + r"\Z")


def _literal_prefix(pattern: str) -> str:
    head = re.split(r"[*?]", pattern, 1)[0]
    return head.rsplit("/", 1)[0] if "/" in head else ""


def _is_glob(pattern: str) -> bool:
    return any(c in pattern for c in "*?")


class Snapshot:
    """The worktree as git sees it: tracked files with their index blob ids, which of them differ on disk, the untracked ones.
    Built on demand. `refresh()` after a task ran: what is on disk is read again (git status, the hashes of changed and untracked
    files) and the index is kept, since a task writes files and never stages them; `invalidate()` drops everything."""

    def __init__(self, root: Path | str | None = None):
        self.root = Path(root or ROOT).resolve()
        self._lock = threading.RLock()
        self._state = None
        self._index = None
        self._hashed: dict[str, str] = {}

    def invalidate(self) -> None:
        with self._lock:
            self._index = None
            self.refresh()

    def refresh(self) -> None:
        with self._lock:
            self._state = None
            self._hashed = {}

    def _index_of(self) -> dict:
        if self._index is None:
            index = {}
            for entry in git(self.root, "ls-files", "-s", "-z").split("\0"):
                if not entry:
                    continue
                meta, path = entry.split("\t", 1)
                mode, blob, _stage = meta.split()
                if mode != GITLINK:
                    index[path] = blob
            self._index = index
        return self._index

    def _load(self) -> dict:
        with self._lock:
            if self._state is not None:
                return self._state
            index = self._index_of()
            dirty, gone, untracked, changed = set(), set(), set(), []
            parts = git(self.root, "status", "--porcelain=v1", "-z", "--untracked-files=all", "--no-renames").split("\0")
            for entry in parts:
                if len(entry) < 4:
                    continue
                xy, path = entry[:2], entry[3:]
                if xy == "??":
                    untracked.add(path)
                elif xy[1] == "D":
                    gone.add(path)
                elif xy[1] != " " or xy[0] == "U":
                    dirty.add(path)
                changed.append(path)
            files = sorted((set(index) - gone) | untracked)
            self._state = {"index": index, "dirty": dirty | untracked, "files": files, "set": set(files),
                           "changed": sorted({p for p in changed if (self.root / p).is_file()})}
            return self._state

    def files(self) -> list[str]:
        return self._load()["files"]

    def changed(self) -> list[str]:
        """Files the worktree changed against HEAD (staged or not) and untracked ones, each as it is now on disk."""
        return self._load()["changed"]

    def resolve(self, patterns: list[str]) -> tuple[list[str], list[str]]:
        """(matched files, patterns that matched nothing). Patterns match git's files (tracked and untracked, ignored left
        out); a pattern with no match there, or an absolute one, is looked for on disk (ignored outputs such as dist/)."""
        state = self._load()
        include = [p for p in patterns if not p.startswith("!")]
        exclude = [glob_regex(p[1:]) for p in patterns if p.startswith("!")]
        found, missing = set(), []
        for pattern in include:
            pattern = pattern.replace("\\", "/").rstrip("/")
            hits = self._match(pattern, state)
            if not hits:
                missing.append(pattern)
            found |= hits
        kept = sorted(f for f in found if not any(x.match(f) for x in exclude))
        return kept, missing

    def _match(self, pattern: str, state: dict) -> set[str]:
        absolute = os.path.isabs(pattern)
        if not absolute:
            if _is_glob(pattern):
                rx = glob_regex(pattern)
                hits = {f for f in state["files"] if rx.match(f)}
            elif pattern in state["set"]:
                hits = {pattern}
            else:
                prefix = pattern + "/"
                hits = {f for f in state["files"] if f.startswith(prefix)}
            if hits:
                return hits
        return self._on_disk(pattern, absolute)

    def _on_disk(self, pattern: str, absolute: bool) -> set[str]:
        base = Path(pattern if absolute else self.root / pattern)
        if not _is_glob(pattern):
            if base.is_file():
                return {pattern}
            if not base.is_dir():
                return set()
            pattern += "/**"
        top = _literal_prefix(pattern)
        rx = glob_regex(pattern)
        start = Path(top) if absolute else self.root / top
        hits = set()
        for dirpath, _dirs, names in os.walk(start):
            for n in names:
                full = Path(dirpath, n)
                shown = full.as_posix() if absolute else full.relative_to(self.root).as_posix()
                if rx.match(shown):
                    hits.add(shown)
        return hits

    def blobs(self, paths: list[str]) -> dict[str, str]:
        """path -> content id: the index blob id of a clean tracked file, `git hash-object` of a changed or untracked one,
        the same blob hash computed here for files git does not see."""
        state = self._load()
        out, ask = {}, []
        with self._lock:
            for p in paths:
                if p in self._hashed:
                    out[p] = self._hashed[p]
                elif p in state["set"] and p not in state["dirty"]:
                    out[p] = state["index"][p]
                elif p in state["set"]:
                    ask.append(p)
                else:
                    full = Path(p) if os.path.isabs(p) else self.root / p
                    out[p] = self._hashed[p] = blob_id(full)
        if ask:
            ids = git(self.root, "hash-object", "--stdin-paths", stdin="\n".join(ask) + "\n").split()
            if len(ids) != len(ask):
                raise RuntimeError(f"git hash-object gave {len(ids)} ids for {len(ask)} files")
            with self._lock:
                for p, i in zip(ask, ids):
                    out[p] = self._hashed[p] = i
        return out


_snapshots: dict[Path, Snapshot] = {}
_snapshots_lock = threading.Lock()


def snapshot(root: Path | str | None = None) -> Snapshot:
    """The process's snapshot of a repo (one per root)."""
    r = Path(root or ROOT).resolve()
    with _snapshots_lock:
        if r not in _snapshots:
            _snapshots[r] = Snapshot(r)
        return _snapshots[r]


def invalidate(root: Path | str | None = None) -> None:
    """After writing files: the next key reads the worktree again."""
    snapshot(root).invalidate()


def files(patterns: list[str], root: Path | str | None = None) -> list[str]:
    """The files the patterns name (repo-relative), git's view first."""
    return snapshot(root).resolve(patterns)[0]


def changed(include: list[str], exclude: list[str] = (), root: Path | str | None = None) -> list[str]:
    """The changed files (against HEAD, untracked included, deleted left out) that match `include` and not `exclude`."""
    inc = [glob_regex(p) for p in include]
    exc = [glob_regex(p) for p in exclude]
    return [f for f in snapshot(root).changed() if any(r.match(f) for r in inc) and not any(r.match(f) for r in exc)]


def content_ids(paths: list[str], root: Path | str | None = None) -> dict[str, str]:
    """path -> content id for many files in one pass (one git call for all the changed ones): ask this first, then key_of
    per file reads the ids back."""
    return snapshot(root).blobs(list(paths))


def key_of(paths: list[str], extra=(), root: Path | str | None = None) -> str:
    """SHA-256 of the sorted path:content ids of what `paths` resolve to, the patterns that matched nothing, and `extra`."""
    snap = snapshot(root)
    found, missing = snap.resolve(list(paths))
    ids = snap.blobs(found)
    doc = {"files": [[f, ids[f]] for f in found], "missing": sorted(missing), "extra": [str(e) for e in extra]}
    return hashlib.sha256(json.dumps(doc, sort_keys=True, ensure_ascii=False).encode("utf-8", "surrogateescape")).hexdigest()
