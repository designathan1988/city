# The lifecycle of what the work leaves on disk (spec 001 FR-014 to FR-020): `worktrees` lists every worktree, sibling copy of
# the repository, nested repository, branch and stash with its class and age, and fails when one is older than its work
# (already in main, a copy, or untouched past suites.json lifecycle.stale_days); `clean` holds every output folder under dist/
# to the bound its owner declared there (outputs.py), now, folder by folder, and lists (or with --unowned removes) a folder no
# one declared. The classes are research R10's mechanical part; what an unmerged piece is worth is read by a person.
from __future__ import annotations

import argparse
import datetime
import os
import sys
import time
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent.parent))
from tasks import cache, outputs  # noqa: E402
from tasks.inputs import git  # noqa: E402
from tasks.task import ROOT  # noqa: E402

DAY_S = 86400
MAIN = "main"
SKIP_WALK = {".git", "dist", "node_modules", ".godot", "__pycache__", "worktrees"}  # where no nested repository of ours lives
OK, FAILED = 0, 1


def _age_days(when: str) -> float:
    return (time.time() - datetime.datetime.fromisoformat(when).timestamp()) / DAY_S


def _mb(path: Path) -> float:
    return cache.size_of(path) / outputs.MB if path.exists() else 0.0


def _in_main(root: Path, ref: str) -> bool:
    try:
        git(root, "merge-base", "--is-ancestor", ref, MAIN)
        return True
    except RuntimeError:
        return False


def _worktrees(root: Path, dirty: bool) -> list[dict]:
    rows, cur = [], {}
    for line in git(root, "worktree", "list", "--porcelain").splitlines() + [""]:
        if line:
            k, _, v = line.partition(" ")
            cur[k] = v
            continue
        if cur and Path(cur["worktree"]).resolve() != root:
            path, head = Path(cur["worktree"]), cur.get("HEAD", "")
            row = {"kind": "worktree", "name": str(path), "head": head[:8], "branch": cur.get("branch", "(detached)")}
            if not path.is_dir():
                row.update(cls="missing", stale=True, why="its folder is gone: git worktree prune")
            else:
                row["last"] = git(root, "log", "-1", "--format=%cI", head).strip()
                merged = _in_main(root, head)
                if dirty:
                    row["uncommitted"] = len([x for x in git(path, "status", "--porcelain").splitlines() if x])
                row["cls"] = ("in-main" + ("-dirty" if row.get("uncommitted") else "-clean" if dirty else "")) if merged else "unmerged"
                row["stale"], row["why"] = (True, "its commit is in main") if merged else (None, "")
            rows.append(row)
        cur = {}
    return rows


def _copies(root: Path, worktree_paths: set) -> list[dict]:
    """Folders beside the repository named after it (city-*) that are no worktree and hold none: copies with no owner."""
    rows = []
    for d in sorted(root.parent.iterdir()):
        if not d.is_dir() or not d.name.startswith(root.name + "-"):
            continue
        r = d.resolve()
        if r in worktree_paths or any(p.is_relative_to(r) for p in worktree_paths):
            continue
        rows.append({"kind": "copy", "name": str(d), "cls": "copy", "stale": True, "why": "a copy of the project with no owner"})
    return rows


def _nested(root: Path) -> list[dict]:
    rows = []
    for dirpath, dirs, files in os.walk(root):
        here = Path(dirpath)
        if here != root and (".git" in dirs or ".git" in files):
            rows.append({"kind": "nested", "name": str(here.relative_to(root).as_posix()), "cls": "nested-repo", "stale": True,
                         "why": "a repository inside the project"})
            dirs[:] = []
            continue
        dirs[:] = [d for d in dirs if d not in SKIP_WALK and not d.startswith("build-")]
    return rows


def _branches(root: Path) -> list[dict]:
    merged = {b.strip("* ").strip() for b in git(root, "branch", "--merged", MAIN).splitlines()}
    rows = []
    for line in git(root, "for-each-ref", "refs/heads", "--format=%(refname:short)\t%(committerdate:iso-strict)").splitlines():
        name, when = line.split("\t")
        if name == MAIN:
            continue
        if name in merged:
            rows.append({"kind": "branch", "name": name, "last": when, "cls": "merged", "stale": True, "why": "fully merged"})
        else:
            ahead = git(root, "rev-list", "--count", f"{MAIN}..{name}").strip()
            rows.append({"kind": "branch", "name": name, "last": when, "cls": f"unmerged ({ahead} commits)", "stale": None, "why": ""})
    return rows


def _stashes(root: Path) -> list[dict]:
    rows = []
    for line in git(root, "stash", "list", "--format=%gd\t%cI\t%gs").splitlines():
        ref, when, subject = line.split("\t", 2)
        rows.append({"kind": "stash", "name": ref, "last": when, "cls": "stash", "stale": None, "why": subject[:90]})
    return rows


def survey(root: Path = ROOT, full: bool = False, suites: Path | None = None) -> list[dict]:
    """Every piece, each with its class and whether it is older than its work."""
    root = Path(root).resolve()
    days = outputs.stale_days(suites)
    trees = _worktrees(root, dirty=full)
    rows = trees + _copies(root, {Path(t["name"]).resolve() for t in trees}) + _nested(root) + _branches(root) + _stashes(root)
    for row in rows:
        if row["stale"] is None:
            old = _age_days(row["last"]) > days
            row["stale"], row["why"] = old, (f"untouched for more than {days} days" if old else row["why"])
    if full:
        for row in rows:
            if row["kind"] in ("worktree", "copy", "nested"):
                path = Path(row["name"]) if row["kind"] != "nested" else root / row["name"]
                row["size_mb"] = round(_mb(path))
                print(f"  .. measured {row['name']}: {row['size_mb']} MB", flush=True)
    return rows


def cmd_worktrees(argv: list[str]) -> int:
    ap = argparse.ArgumentParser(prog="cli.py worktrees", description=__doc__)
    ap.add_argument("--all", action="store_true", help="also each worktree's uncommitted files and every folder's size (slow)")
    ap.add_argument("--root", default=str(ROOT), help=argparse.SUPPRESS)
    ap.add_argument("--suites", help=argparse.SUPPRESS)
    a = ap.parse_args(argv)
    rows = survey(Path(a.root), a.all, a.suites)
    for row in rows:
        extra = "".join(f" {k}={row[k]}" for k in ("head", "branch", "uncommitted", "size_mb", "last") if k in row)
        print(f"{'STALE' if row['stale'] else 'ok   '} {row['kind']:<8} {row['cls']:<22} {row['name']}{extra}"
              + (f"  ({row['why']})" if row["why"] else ""))
    stale = sum(1 for r in rows if r["stale"])
    print(f"{len(rows)} piece(s), {stale} older than their work")
    return FAILED if stale else OK
