# Every output folder under dist/ held to its bound now (cli.py clean; spec 001 FR-017, FR-018): max_mb, the least recently
# used entries past it go (the task cache through its own ledger); keep, the newest entries past the count go, except those an
# open problem's evidence names; while_open, an entry goes when the problem it is named after is no longer open; whole_mb, a folder or file written as one piece goes whole past its size. A folder under dist/ that no one declared is listed, and removed with --unowned. What each
# step frees is printed folder by folder as it goes; --dry-run removes nothing. The bounds are outputs.py's. A register that
# cannot be read stops it before anything goes (what it keeps is unknown); a file a running process holds is kept and said.
from __future__ import annotations

import argparse
import sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent.parent))
from atlas import problems as register  # noqa: E402  the register: which problems are open, what their evidence names
from tasks import cache, outputs  # noqa: E402
from tasks.task import ROOT  # noqa: E402

DIST = "dist"
OK, FAILED = 0, 1


def _open_problems(root: Path) -> list[dict]:
    """The open problems of the register. A register that cannot be read raises (register.RegisterError): what it would keep
    (an open problem's evidence) is unknown, so nothing may be removed by it."""
    return [p for p in register.read(str(root / register.REGISTER)) if p.get("status") not in register.FINAL]


def _named(problems: list[dict], folder: str) -> set:
    """The entries of a folder that an open problem's evidence names."""
    prefix = folder.rstrip("/") + "/"
    return {e[len(prefix):].split("/", 1)[0] for p in problems for e in p.get("evidence") or [] if e.startswith(prefix)}


def _children(path: Path, own: set) -> list[Path]:
    """A folder's entries, oldest first, but those declared as outputs of their own (dist/evidence/research in dist/evidence),
    each held to its own owner's bound, and the owner's index files (dist/builds/latest.txt), which are no entry."""
    return sorted((c for c in path.iterdir() if c.name not in own), key=lambda p: p.stat().st_mtime) if path.is_dir() else []


def _declared_inside(entry: dict, declared: list[dict]) -> set:
    """The names of the entries of a declared folder that are declared outputs themselves."""
    prefix = entry["path"].rstrip("/") + "/"
    return {e["path"][len(prefix):] for e in declared if e["path"].startswith(prefix) and "/" not in e["path"][len(prefix):]}


def _over_bound(path: Path, entry: dict, problems: list[dict], own: set = frozenset()) -> list[Path]:
    """What the bound of one declared folder removes now, oldest first (`problems`: the open ones of the register)."""
    if "whole_mb" in entry:
        return [path] if path.exists() and cache.size_of(path) > entry["whole_mb"] * outputs.MB else []
    if "keep" in entry:
        kept = _named(problems, entry["path"])
        loose = [c for c in _children(path, own) if c.name not in kept and not c.name.startswith(".")]
        return loose[:-entry["keep"]] if len(loose) > entry["keep"] else []
    if "while_open" in entry:
        alive = {p["id"] for p in problems}
        return [c for c in _children(path, own) if c.name not in alive]
    if "max_mb" in entry:
        sized = [(c, cache.size_of(c)) for c in _children(path, own) if not c.name.startswith(".")]
        total, out = sum(n for _c, n in sized), []
        for c, n in sized:
            if total <= entry["max_mb"] * outputs.MB:
                break
            out.append(c)
            total -= n
        return out
    return []


def unowned(root: Path, declared: list[dict]) -> list[Path]:
    """Folders and files under dist/ no declared output is nor holds."""
    paths = [e["path"] for e in declared]
    out = []

    def walk(rel: str):
        for c in sorted((root / rel).iterdir()) if (root / rel).is_dir() else []:
            r = f"{rel}/{c.name}"
            if r in paths:
                continue
            if any(p.startswith(r + "/") for p in paths):
                walk(r)
            else:
                out.append(c)
    walk(DIST)
    return out


def _remove(c: Path, root: Path) -> int:
    """Removes one entry; returns the bytes that went. A file a running process holds is kept and said
    (cache.remove_unless_held), and only what else of the entry went is counted; any other failure propagates."""
    before = cache.size_of(c)
    if cache.remove_unless_held(c, root):
        return before
    return before - (cache.size_of(c) if c.exists() else 0)


def clean(root: Path = ROOT, dry_run: bool = False, with_unowned: bool = False, only: str | None = None,
          suites: Path | None = None) -> float:
    """Applies every bound (or the one of `only`); returns the megabytes freed (or that would be). The register is read
    first: one that cannot be read stops the whole clean before anything is removed (register.RegisterError)."""
    root = Path(root).resolve()
    declared = outputs.declared(suites)
    problems = _open_problems(root)
    freed = 0.0
    for entry in declared:
        if only and entry["path"] != only:
            continue
        path = root / entry["path"]
        if entry["path"] == cache.STORE and "max_mb" in entry and not dry_run:
            before = cache.size_of(path) if path.is_dir() else 0
            total = cache.ledger(root).settle(entry["max_mb"] * outputs.MB)
            gone = (before - cache.size_of(path)) / outputs.MB
            freed += gone
            print(f"{entry['path']}: {gone:.0f} MB freed (its ledger holds {total / outputs.MB:.0f} MB)", flush=True)
            continue
        removed = _over_bound(path, entry, problems, _declared_inside(entry, declared) | set(entry.get("index", ())))
        gone = sum(cache.size_of(c) if dry_run else _remove(c, root) for c in removed)
        mb = gone / outputs.MB
        freed += mb
        print(f"{entry['path']}: {len(removed)} entr{'y' if len(removed) == 1 else 'ies'}, {mb:.0f} MB"
              f"{' would go' if dry_run else ' freed'}", flush=True)
    if not only:
        for c in unowned(root, declared):
            mb = cache.size_of(c) / outputs.MB
            print(f"no owner: {c.relative_to(root).as_posix()} ({mb:.0f} MB)"
                  + (" removed" if with_unowned and not dry_run else ""), flush=True)
            if with_unowned and not dry_run:
                freed += _remove(c, root) / outputs.MB
    print(f"{'would free' if dry_run else 'freed'} {freed:.0f} MB")
    return freed


def cmd_clean(argv: list[str]) -> int:
    ap = argparse.ArgumentParser(prog="cli.py clean", description="every output folder under dist/ held to its bound now")
    ap.add_argument("--dry-run", action="store_true", help="print what each bound would free, remove nothing")
    ap.add_argument("--unowned", action="store_true", help="also remove what lies under dist/ with no declared owner")
    ap.add_argument("--only", help="one declared folder (deliver.sh: dist/builds after an export)")
    ap.add_argument("--root", default=str(ROOT), help=argparse.SUPPRESS)
    ap.add_argument("--suites", help=argparse.SUPPRESS)
    a = ap.parse_args(argv)
    if a.only:
        outputs.bound(a.only, a.suites)  # a folder no one declared is an error, never cleaned by a guess
    try:
        clean(Path(a.root), a.dry_run, a.unowned, a.only, a.suites)
    except register.RegisterError as e:
        print(f"clean stopped, nothing removed: the register cannot be read ({e})", file=sys.stderr)
        return FAILED
    return OK
