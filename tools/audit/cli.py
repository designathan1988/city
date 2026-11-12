"""The game's audit: one report of everything wrong that a run or the tree itself can state, grouped by owner.

    python tools/audit/cli.py logs [run folder ...]   the logs of those runs (default: the newest dist/tasks run)
    python tools/audit/cli.py static                  what the tree states without running (static.py)
    python tools/audit/cli.py all [run folder ...]    both

The report is written to dist/audit/<stamp>/report.md and report.json (and copied to dist/audit/latest.*). Exit 1 while any
cause that is not expected stands, 2 when there was nothing to read: a check that cannot fail checks nothing.
"""
from __future__ import annotations

import shutil
import sys
import time
from pathlib import Path

if __package__ in (None, ""):
    sys.path.insert(0, str(Path(__file__).resolve().parents[1]))
    __package__ = "audit"

from . import logs, static  # noqa: E402
from .findings import Finding, write_report  # noqa: E402

ROOT = Path(__file__).resolve().parents[2]
RUNS = ROOT / "dist" / "tasks"
OUT = ROOT / "dist" / "audit"


def newest_run() -> Path | None:
    runs = [p for p in RUNS.iterdir() if p.is_dir() and any(p.glob("*.log"))] if RUNS.is_dir() else []
    return max(runs, key=lambda p: p.stat().st_mtime) if runs else None


def main(argv: list[str]) -> int:
    if not argv or argv[0] not in ("logs", "static", "all"):
        print(__doc__)
        return 2
    what, folders = argv[0], [Path(a) for a in argv[1:]]
    findings: list[Finding] = []
    sources: list[str] = []
    if what in ("logs", "all"):
        if not folders:
            run = newest_run()
            folders = [run] if run else []
        for folder in folders:
            if not folder.is_dir() or not any(folder.glob("*.log")):
                print("no logs in %s" % folder)
                return 2
            findings.extend(logs.read_run(folder))
            sources.append("logs of %s (%d)" % (folder.name, len(list(folder.glob("*.log")))))
    if what in ("static", "all"):
        found, read = static.read_tree(ROOT)
        findings.extend(found)
        sources.append("the tree (%d files read)" % read)
    if not sources:
        print("nothing to read")
        return 2
    out = OUT / time.strftime("%Y%m%d-%H%M%S")
    causes, occurrences = write_report(findings, sources, out)
    for name in ("report.md", "report.json"):
        shutil.copyfile(out / name, OUT / ("latest" + Path(name).suffix))
    print("%d cause(s), %d occurrence(s): %s" % (causes, occurrences, (out / "report.md").as_posix()))
    return 1 if causes else 0


if __name__ == "__main__":
    sys.exit(main(sys.argv[1:]))
