# The block rule's mechanism (CLAUDE.md, "One functionality, start to end"): a functionality is coded whole with nothing run, then
# checked at its end, and never by trial and error. Its state is one file, dist/block/state.json, moved only by `enter` through MOVES:
# closed -> open (the code being written, nothing run) -> verify (its checks run) -> open again (a failure fixed in the code) or
# closed. `refusal` is the one answer every gatekeeper asks (the orchestrator's `run` and `serve`, build.sh, deliver.sh, km24.py).
# While the checks run: the game's code does not change (a fix is made with the block open again); a check that failed is not run
# again; the block opens again only with the cause of each failed check named at a `file:line` that exists; and after MAX_ROUNDS
# verifications that failed the tool stops until the owner's word is given. `close` appends the block's times to the process
# history, so the time each functionality takes shows. Run as a script by the build scripts, so it finds its package itself.
from __future__ import annotations

import argparse
import datetime
import hashlib
import json
import os
import re
import subprocess
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
STATE = Path("dist/block/state.json")
HISTORY = Path("doc/history/process.md")
TIMES_HEADER = "| Block | Opened | Verified | Closed | Coding (min) | Verifying (min) |"
CLOSED, OPEN, VERIFY = "closed", "open", "verify"
MOVES = {CLOSED: (OPEN,), OPEN: (VERIFY,), VERIFY: (OPEN, CLOSED)}
WHAT = ("run", "build", "export", "commit")  # what the tools ask before they act
STAMP = "%Y-%m-%dT%H:%M"
SECONDS_PER_MINUTE = 60
MAX_ROUNDS = 2  # verifications that may fail before the tool stops: a third failure is a wrong design, the owner's to hear
CODE = ("native/src", "project/game", "project/ui", "project/render", "project/data", "project/tests")  # the game's code
MADE = (".uid", ".import", ".translation")  # what the engine writes beside a source: no change of the code
PASSED = ("PASS", "CACHED_PASS")
BUILDS = ("build_debug", "build_release", "import", "scripts")  # what a check stands on: its failure is no failed verification
PLACE = re.compile(r"([\w./-]+\.\w+):(\d+)")  # a cause's `file:line`
GIT_SECONDS = 30


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


def tree(root: Path = ROOT) -> str | None:
    """The game's code as it stands, as one number: every changed or new file of CODE with its size and time of writing (what
    the engine writes beside a source left out). None where git cannot say."""
    try:
        done = subprocess.run(["git", "-C", str(root), "ls-files", "-m", "-o", "--exclude-standard", "--", *CODE],
                              capture_output=True, text=True, encoding="utf-8", errors="replace", timeout=GIT_SECONDS)
    except (OSError, subprocess.TimeoutExpired):
        return None
    if done.returncode != 0:
        return None
    seen = hashlib.sha256()
    for rel in sorted(set(done.stdout.splitlines())):
        path = Path(root) / rel
        if rel.endswith(MADE) or not path.is_file():
            continue
        at = path.stat()
        seen.update(f"{rel}|{at.st_size}|{at.st_mtime_ns}\n".encode("utf-8"))
    return seen.hexdigest()


def failed(doc: dict) -> list[str]:
    """The checks of this verification that failed (what a check stands on, a build, apart)."""
    return sorted(n for n, status in (doc.get("runs") or {}).items() if status not in PASSED and n not in BUILDS)


def _cause(doc: dict, cause: str | None, owner: str | None, root: Path) -> None:
    """A verification that failed opens again only with its cause: every failed check named, and a `file:line` that exists;
    past MAX_ROUNDS failed verifications only with the owner's word."""
    broke = [n for n, status in (doc.get("runs") or {}).items() if status not in PASSED]
    if not broke:
        return
    said = cause or ""
    missing = [n for n in broke if n not in said]
    places = [(p, int(line)) for p, line in PLACE.findall(said)]
    real = [p for p, line in places if (Path(root) / p).is_file()
            and line <= len((Path(root) / p).read_bytes().splitlines())]
    if missing or not real:
        raise BlockError(f"this verification failed ({', '.join(broke)}): the block opens again with the cause read in the code, "
                         f"`block open --cause \"<check>: <file:line>: <what is wrong there and why>\"`, every failed check named "
                         f"({', '.join(missing) or 'all named'}) and a file:line that exists ({'none given' if not real else 'given'}). "
                         f"A guess is no cause: read the failing lines and the code they name first")
    if failed(doc):
        doc["round"] = int(doc.get("round") or 0) + 1
    if doc["round"] > MAX_ROUNDS and not owner:
        raise BlockError(f"{doc['round']} verifications of '{doc['name']}' failed: more than {MAX_ROUNDS} is no fix, the design is "
                         f"wrong. Stop, tell the owner what failed and why each fix did not hold, and open again only with "
                         f"the owner's word: `block open --cause \"...\" --owner \"<the owner's words>\"`")
    doc.setdefault("causes", []).append(f"{_now()} round {doc['round']}: {said}" + (f" [owner: {owner}]" if owner else ""))
    note = Path(root) / "doc" / "work" / f"{doc['name']}.md"
    if note.is_file():
        with open(note, "a", encoding="utf-8", newline="\n") as fh:
            fh.write(f"\n- Cause named at `block open` ({doc['causes'][-1]})\n")


def record(statuses: dict, root: Path = ROOT) -> None:
    """What the checks of a verification answered, kept with the block: a run tells its tasks' verdicts here."""
    doc = load(root)
    if doc["state"] != VERIFY:
        return
    doc.setdefault("runs", {}).update(statuses)
    _save(doc, root)


def enter(target: str, name: str | None = None, root: Path = ROOT, cause: str | None = None, owner: str | None = None) -> dict:
    """The one setter of the block's state: refused unless MOVES allows the move. A closed block opens with its name; a block in
    verification opens again (a failure fixed in the code) under the name it has, its failed checks' cause named."""
    doc = load(root)
    if target not in MOVES.get(doc["state"], ()):
        raise BlockError(f"the block is {doc['state']}: it may move to {', '.join(MOVES.get(doc['state'], ())) or 'nothing'}, "
                         f"not to {target}")
    if target == OPEN and doc["state"] == CLOSED:
        if not name:
            raise BlockError("a block is opened with its name")
        doc = {"state": OPEN, "name": name, "opened": _now(), "verified": None, "round": 0, "runs": {}, "causes": []}
    elif target == VERIFY:
        doc.update(state=VERIFY, verified=_now(), runs={}, tree=tree(root))
    elif target == OPEN:
        _cause(doc, cause, owner, root)
        doc.update(state=OPEN, runs={})
    else:
        if failed(doc):
            raise BlockError(f"checks of this verification failed ({', '.join(failed(doc))}): a block closes on checks that pass")
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


def refusal(what: str, root: Path = ROOT, tasks: list[str] | None = None) -> str | None:
    """Why `what` (a run of `tasks`, a build, an export, a commit) may not happen now, or None when it may."""
    doc = load(root)
    if doc["state"] == OPEN:
        return (f"refused: the block '{doc['name']}' is open (since {doc['opened']}): no test, build, measurement, photo or "
                f"export runs and nothing is committed while the code is written (CLAUDE.md, steps 6 and 7; skills and "
                f"Context7 go on being used). `python tools/tasks/cli.py block verify` starts its checks. Asked: {what}")
    if doc["state"] != VERIFY:
        return None
    again = [f"{n} ({(doc.get('runs') or {})[n]})" for n in tasks or () if (doc.get("runs") or {}).get(n, "PASS") not in PASSED]
    if again:
        return (f"refused: {', '.join(again)} failed in this verification of '{doc['name']}' and is not run again as it is. "
                f"Read its failing lines and the code they name, then `python tools/tasks/cli.py block open --cause "
                f"\"<check>: <file:line>: <why>\"`, fix it in the code, `block verify`, and run it once. Asked: {what}")
    now = tree(root)
    if doc.get("tree") and now and now != doc["tree"]:
        return (f"refused: the game's code changed since `block verify` of '{doc['name']}' ({', '.join(CODE)}): nothing is built "
                f"or run on code changed while its checks run. A fix is made with the block open: `python tools/tasks/cli.py "
                f"block open --cause \"<check>: <file:line>: <why>\"`, then `block verify`. Asked: {what}")
    return None


def main(argv: list[str]) -> int:
    ap = argparse.ArgumentParser(prog="cli.py block", description="the block rule: open, verify, close, status, check")
    ap.add_argument("action", choices=(OPEN, VERIFY, "close", "status", "check"))
    ap.add_argument("name", nargs="?", help="the block's name (open); what is asked (check: run, build, export, commit)")
    ap.add_argument("--cause", help="open, after a verification that failed: each failed check, a file:line and why")
    ap.add_argument("--owner", help=f"open, past {MAX_ROUNDS} failed verifications: the owner's words to go on")
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
        print(f"block {doc['state']}" + (f": {doc['name']} (opened {doc['opened']}, verified {doc['verified'] or '-'}; "
                                         f"failed verifications {doc.get('round') or 0} of {MAX_ROUNDS}; failed now: "
                                         f"{', '.join(failed(doc)) or 'none'})" if doc["state"] != CLOSED else ""))
        return 0
    try:
        doc = enter(CLOSED if a.action == "close" else a.action, a.name, root, a.cause, a.owner)
    except BlockError as e:
        print(f"refused: {e}", file=sys.stderr)
        return 1
    print(f"block {doc['state']}" + (f": {doc['name']}" if doc.get("name") else f" ({doc.get('last')})"))
    return 0


if __name__ == "__main__":
    sys.exit(main(sys.argv[1:]))
