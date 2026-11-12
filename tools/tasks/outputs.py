# The bounds of what the work writes under dist/ (tools/tasks/suites.json "lifecycle"): the one reader of them. Each owner asks
# for its own folder's bound here (the task cache its max_mb, the runs their keep, deliver.sh its builds' keep) and applies it
# on every write; lifecycle.py applies them all at once (cli.py clean). Standard library only.
from __future__ import annotations

import json
from pathlib import Path

HERE = Path(__file__).resolve().parent
ROOT = HERE.parent.parent
SUITES = HERE / "suites.json"
MB = 1024 * 1024


def lifecycle(suites: Path | str | None = None) -> dict:
    with open(suites or SUITES, encoding="utf-8") as fh:
        return json.load(fh)["lifecycle"]


def declared(suites: Path | str | None = None) -> list[dict]:
    """Every declared output folder with its owner and its bound."""
    return list(lifecycle(suites)["outputs"])


def bound(path: str, suites: Path | str | None = None) -> dict:
    """The declared entry of one output folder (repo-relative, forward slashes); a folder nobody declared is an error of its
    writer, never a folder without a bound."""
    for entry in declared(suites):
        if entry["path"] == path:
            return entry
    raise KeyError(f"{path}: no owner and no bound in tools/tasks/suites.json lifecycle.outputs")


def stale_days(suites: Path | str | None = None) -> int:
    return int(lifecycle(suites)["stale_days"])
