# The block rule's mechanism (CLAUDE.md, Method step 5): a block is coded whole with nothing run inside it, then verified once.
# Its state is one file, dist/block/state.json, moved only by `enter` through MOVES: closed -> open (the code written, nothing run) ->
# verify (the one verification and the close's operations run) -> open again (a failure fixed in the code) or closed.
# `refusal` is the one answer every gatekeeper asks (the orchestrator's `run`, build.sh, deliver.sh, km24.py).
# `close` refuses while the block's work file lacks the method's records (block_method.py), then appends the block's times to the
# process history (FR-050). Run as a script by the build scripts, so it finds its package itself.
from __future__ import annotations

import argparse
import datetime
import json
import os
import subprocess
import sys
from pathlib import Path

if str(Path(__file__).resolve().parents[1]) not in sys.path:
    sys.path.insert(0, str(Path(__file__).resolve().parents[1]))
from tasks import block_item, block_method, proc  # noqa: E402

ROOT = Path(__file__).resolve().parents[2]
STATE = Path("dist/block/state.json")
HISTORY = Path("doc/history/process.md")
TIMES_HEADER = "| Block | Opened | Verified | Closed | Coding (min) | Verifying (min) |"
CLOSED, OPEN, VERIFY = "closed", "open", "verify"
MOVES = {CLOSED: (OPEN,), OPEN: (VERIFY,), VERIFY: (OPEN, CLOSED)}
WHAT = ("run", "build", "export", "commit")  # what the tools ask before they act
STAMP = "%Y-%m-%dT%H:%M"
SECONDS_PER_MINUTE = 60


class BlockError(Exception):
    """A move the table refuses; the message names the state and what may follow it."""


def _now() -> str:
    return datetime.datetime.now().strftime(STAMP)


def load(root: Path = ROOT) -> dict:
    """The block's state; no file is a closed block."""
    path = Path(root) / STATE
    if not path.is_file():
        return {"state": CLOSED, "name": None, "opened": None, "verified": None}
    with open(path, encoding="utf-8") as fh:
        return json.load(fh)


def _save(doc: dict, root: Path) -> None:
    path = Path(root) / STATE
    path.parent.mkdir(parents=True, exist_ok=True)
    part = path.with_suffix(".part")
    part.write_bytes((json.dumps(doc, indent=1) + "\n").encode("utf-8"))
    os.replace(part, path)


def enter(target: str, name: str | None = None, root: Path = ROOT, item: str | None = None) -> dict:
    """The one setter of the block's state: refused unless MOVES allows the move. The code of an item is opened (`item`: the block's
    first, or the next after a verification) only when the item holds its reading, trace, research and failing red check
    (block_item.py); opened again with no item, it is the same item's code after a failure."""
    doc = load(root)
    if target not in MOVES.get(doc["state"], ()):
        raise BlockError(f"the block is {doc['state']}: it may move to {', '.join(MOVES.get(doc['state'], ())) or 'nothing'}, "
                         f"not to {target}")
    if target == OPEN and (doc["state"] == CLOSED or item):
        block_name = name if doc["state"] == CLOSED else doc["name"]
        if not block_name:
            raise BlockError("a block is opened with its name")
        if not item:
            raise BlockError("a block's code is opened for an item: --item <ID>, its `## Item` section in the work file")
        missing = block_item.opening_gaps(Path(root), block_name, item)
        if missing:
            raise BlockError(f"item {item} is not ready for its code:\n  " + "\n  ".join(missing))
        # the item's preparation began when the one before it went to its verification (the block's first: at the session's last
        # compaction, the guard's own default); its code from now (edit_guard.py reads both)
        prepared_from = doc.get("verified_utc") if doc["state"] == VERIFY else None
        if doc["state"] == CLOSED:
            doc = {"state": OPEN, "name": block_name, "opened": _now(), "verified": None, "head": _head(root)}
        doc.update(state=OPEN, item=item, fails={}, item_from=prepared_from, item_opened=block_item.utc_now())
    elif target == VERIFY:
        doc.update(state=VERIFY, verified=_now(), verified_utc=block_item.utc_now())
    else:
        doc["state"] = target
    if target == CLOSED:
        missing = block_method.gaps(Path(root), doc)
        if missing:
            raise BlockError("the work file lacks the method's records:\n  " + "\n  ".join(missing))
        _append_times(doc, root)
        doc = {"state": CLOSED, "name": None, "opened": None, "verified": None, "last": doc["name"]}
    _save(doc, root)
    return doc


def _head(root: Path) -> str | None:
    """The commit the block opens on (what its close measures the changed code against); None outside a repository."""
    done = subprocess.run(["git", "-C", str(root), "rev-parse", "HEAD"], capture_output=True, text=True,
                          timeout=block_method.GIT_TIMEOUT_S, creationflags=proc.NO_WINDOW)
    return done.stdout.strip() if done.returncode == 0 else None


def _minutes(a: str | None, b: str | None) -> str:
    if not a or not b:
        return "-"
    span = datetime.datetime.strptime(b, STAMP) - datetime.datetime.strptime(a, STAMP)
    return f"{span.total_seconds() / SECONDS_PER_MINUTE:.0f}"


def _append_times(doc: dict, root: Path) -> None:
    """One row of the block-times table of the process history, after its last row; LF bytes kept as they are."""
    path = Path(root) / HISTORY
    closed = _now()
    row = (f"| {doc['name']} | {doc['opened']} | {doc['verified'] or '-'} | {closed} | {_minutes(doc['opened'], doc['verified'])} "
           f"| {_minutes(doc['verified'], closed)} |")
    lines = path.read_bytes().decode("utf-8").split("\n") if path.is_file() else []
    if TIMES_HEADER not in lines:
        raise BlockError(f"{HISTORY} has no block-times table ({TIMES_HEADER})")
    at = lines.index(TIMES_HEADER) + 2  # past the header and its separator row
    while at < len(lines) and lines[at].startswith("|"):
        at += 1
    lines.insert(at, row)
    path.write_bytes("\n".join(lines).encode("utf-8"))


def refusal(what: str, root: Path = ROOT) -> str | None:
    """Why `what` (a run, a build, an export, a commit) may not happen now, or None when it may."""
    doc = load(root)
    if doc["state"] != OPEN:
        return None
    return (f"refused: the block '{doc['name']}' is open (since {doc['opened']}): nothing is run, built, exported or committed "
            f"while its code is written (CLAUDE.md, the block rule). `python tools/tasks/cli.py block verify` opens the one "
            f"verification. Asked: {what}")


def check_refusal(tasks: list[str], root: Path = ROOT) -> str | None:
    """Why the checks `tasks` may not run again for the item under verification (block_item.run_refusal); None when they may."""
    doc = load(root)
    return block_item.run_refusal(Path(root), doc, tasks) if doc["state"] == VERIFY else None


def record(results: dict[str, str], root: Path = ROOT) -> None:
    """A verification's results (task: status) remembered for its item: what failed, with the causes and sources written then."""
    doc = load(root)
    if doc["state"] == VERIFY:
        _save(block_item.remember(Path(root), doc, results), root)


def main(argv: list[str]) -> int:
    ap = argparse.ArgumentParser(prog="cli.py block", description="the block rule: open, verify, close, status, check")
    ap.add_argument("action", choices=(OPEN, VERIFY, "close", "status", "check"))
    ap.add_argument("name", nargs="?", help="the block's name (open); what is asked (check: run, build, export, commit)")
    ap.add_argument("--item", help="open: the item whose code is opened (its `## Item` section in the work file)")
    ap.add_argument("--root", default=str(ROOT), help=argparse.SUPPRESS)
    a = ap.parse_args(argv)
    root = Path(a.root)
    if a.action == "check":
        why = refusal(a.name or "run", root)
        if why:
            print(why, file=sys.stderr)
        return 1 if why else 0
    if a.action == "status":
        doc = load(root)
        print(f"block {doc['state']}" + (f": {doc['name']} (opened {doc['opened']}, verified {doc['verified'] or '-'}, item "
                                         f"{doc.get('item') or '-'}, failing {sorted(doc.get('fails', {})) or '-'})"
                                         if doc["state"] != CLOSED else ""))
        return 0
    try:
        doc = enter(CLOSED if a.action == "close" else a.action, a.name, root, a.item)
    except BlockError as e:
        print(f"refused: {e}", file=sys.stderr)
        return 1
    print(f"block {doc['state']}" + (f": {doc['name']}" if doc.get("name") else f" ({doc.get('last')})"))
    return 0


if __name__ == "__main__":
    sys.exit(main(sys.argv[1:]))
