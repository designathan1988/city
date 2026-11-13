# The block rule's mechanism (CLAUDE.md, Method step 5): a block is coded whole with nothing run inside it, then verified once.
# Its state is one file, dist/block/state.json, moved only by `enter` through MOVES: closed -> open (the code written, nothing run) ->
# verify (the one verification and the close's operations run) -> open again (a failure fixed in the code) or closed.
# `refusal` is the one answer every gatekeeper asks (the orchestrator's `run`, build.sh, deliver.sh);
# `command_refusal` is the same answer for one shell command line.
# `close` appends the block's times to the process history (FR-050). Standard library only.
from __future__ import annotations

import argparse
import datetime
import json
import os
import re
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
STATE = Path("dist/block/state.json")
HISTORY = Path("log/history/process.md")
TIMES_HEADER = "| Block | Opened | Verified | Closed | Coding (min) | Verifying (min) |"
CLOSED, OPEN, VERIFY = "closed", "open", "verify"
MOVES = {CLOSED: (OPEN,), OPEN: (VERIFY,), VERIFY: (OPEN, CLOSED)}
WHAT = ("run", "build", "export", "commit")  # what the tools ask before they act
STAMP = "%Y-%m-%dT%H:%M"
SECONDS_PER_MINUTE = 60

# A shell command refused while a block is open: a segment whose program is one of these, or a Python or shell call of one of
# these entry points. Words of the commands themselves; nothing else of a command line is judged.
ENGINES = re.compile(r"^(godot[\w.]*|roadcraft)(\.exe)?$", re.I)
BUILDERS = {"cmake", "ninja", "make", "mingw32-make", "ctest"}
RUN_CALLS = re.compile(
    r"-m\s+(unittest|pytest)\b|\bpytest\b|run_gates\.py|lab_scan\.py|tools[/\\]tasks[/\\]cli\.py\s+run\b"
    r"|tools[/\\]build[/\\](build|quick|deliver|pgo)\.sh|\bgit\s+commit\b", re.I)
SEGMENT = re.compile(r"(?:^|[;\n|(]|&&|\|\|)\s*(?:&\s*|\.\s+|call\s+|exec\s+|start(?:-process)?\s+)?[\"']?([^\s\"';&|()]+)", re.I)


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


def enter(target: str, name: str | None = None, root: Path = ROOT) -> dict:
    """The one setter of the block's state: refused unless MOVES allows the move."""
    doc = load(root)
    if target not in MOVES.get(doc["state"], ()):
        raise BlockError(f"the block is {doc['state']}: it may move to {', '.join(MOVES.get(doc['state'], ())) or 'nothing'}, "
                         f"not to {target}")
    if target == OPEN and doc["state"] == CLOSED:
        if not name:
            raise BlockError("a block is opened with its name")
        doc = {"state": OPEN, "name": name, "opened": _now(), "verified": None}
    elif target == VERIFY:
        doc.update(state=VERIFY, verified=_now())
    else:
        doc["state"] = target
    if target == CLOSED:
        _append_times(doc, root)
        doc = {"state": CLOSED, "name": None, "opened": None, "verified": None, "last": doc["name"]}
    _save(doc, root)
    return doc


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


def command_refusal(command: str, root: Path = ROOT) -> str | None:
    """Why a shell command may not run now (the engine, a build, a test runner, the gates, a run, an export, a commit), or None."""
    if load(root)["state"] != OPEN:
        return None
    for m in SEGMENT.finditer(command):
        program = os.path.basename(m.group(1).replace("\\", "/")).lower()
        if ENGINES.match(program) or program in BUILDERS:
            return refusal(f"the command '{program}'", root)
    hit = RUN_CALLS.search(command)
    return refusal(f"'{hit.group(0)}'", root) if hit else None


def main(argv: list[str]) -> int:
    ap = argparse.ArgumentParser(prog="cli.py block", description="the block rule: open, verify, close, status, check")
    ap.add_argument("action", choices=(OPEN, VERIFY, "close", "status", "check"))
    ap.add_argument("name", nargs="?", help="the block's name (open); what is asked (check: run, build, export, commit)")
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
        print(f"block {doc['state']}" + (f": {doc['name']} (opened {doc['opened']}, verified {doc['verified'] or '-'})"
                                         if doc["state"] != CLOSED else ""))
        return 0
    try:
        doc = enter(CLOSED if a.action == "close" else a.action, a.name, root)
    except BlockError as e:
        print(f"refused: {e}", file=sys.stderr)
        return 1
    print(f"block {doc['state']}" + (f": {doc['name']}" if doc.get("name") else f" ({doc.get('last')})"))
    return 0


if __name__ == "__main__":
    sys.exit(main(sys.argv[1:]))
