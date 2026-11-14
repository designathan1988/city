"""What the session may run with no block open, whose word lifts a bound, and whether the look's photos were looked at
(called by port_gate.py).

With no block open the game is looked at once: one run of a few checks and one measure, between a block's close and the next
one's opening. A second run, a whole suite, the 24 km measures, the hunt and the ready run are had only on the owner's order,
given in the owner's own words (`python tools/tasks/cli.py block order "<words>"`, or a tool's `--owner "<words>"`): the
words are looked for in what the owner wrote in this session, so an order is never made up. This binds the session, not the
owner: a run started by hand passes no hook.
A block's commit asks the look's photos opened after they were taken, with the references they are compared to."""

import json
import re
import time
from datetime import datetime
from pathlib import Path

import port_note

ROOT = Path(__file__).resolve().parents[2]
STATE = ROOT / "dist" / "block" / "state.json"
LOOKS = ROOT / "dist" / "block" / "looks.json"    # the runs had with no block open, under the state file as it stood
ORDER = ROOT / "dist" / "block" / "order.json"    # the owner's order standing (tools/tasks/block.py writes it; a run spends it)
SUITES = ROOT / "tools" / "tasks" / "suites.json"
SHOTS = ROOT / "dist" / "captures"                # a photo plan's pictures: dist/captures/<plan>/*.png
REFS = ROOT / "doc" / "look"                      # the references of the game's look
MOST = 6                                          # tasks a look may name (tools/tasks/block_review.py MAX_CHECKS)
MEASURES = ("lab_test",)
WORDS_LEAST = 12                                  # characters of an order: shorter is no sentence of the owner's
VALUED = ("--jobs", "--json", "--suites", "--root")
WIDE = ("suites", "groups", "sets")               # names of suites.json that stand for many tasks
ORDERED = re.compile(r"\b(km24\.py|hunt\.py|quick\.sh)\b")
FREE = re.compile(r"\b(build\.sh|deliver\.sh)\b")
NAMED = re.compile(r"cli\.py[\"']?\s+run\b([^;&|\n]*)")
SAID = re.compile(r"(?:--owner|\bblock\s+order)\s+(?:\"([^\"]*)\"|'([^']*)')")
HOW = ("What is wrong is found by reading the code, and a fix is checked in its block (`python tools/tasks/cli.py block open "
       "<name>`, then its one verification). On the owner's order only: `python tools/tasks/cli.py block order \"<the owner's "
       "own words>\"`, then this run again.")


def norm(text: str) -> str:
    return " ".join(str(text).casefold().split())


def claimed(command: str) -> str | None:
    """The owner's words a command carries (`--owner "..."`, `block order "..."`); None when it carries none."""
    found = SAID.search(command)
    return None if not found else (found.group(1) if found.group(1) is not None else found.group(2))


def owner_texts(transcript: str):
    """What the owner wrote in this session, from its record: a message of the owner's is a queued prompt, a prompt attached
    while the session worked, or a user's entry that is no tool's result, no summary of a compaction and no text of the tools."""
    path = Path(transcript) if transcript else None
    if path is None or not path.is_file():
        return
    with path.open(encoding="utf-8", errors="replace") as lines:
        for line in lines:
            if '"user"' not in line and "queue-operation" not in line and '"attachment"' not in line:
                continue
            try:
                entry = json.loads(line)
            except json.JSONDecodeError:
                continue
            kind = entry.get("type")
            if kind == "queue-operation" and isinstance(entry.get("content"), str):
                yield entry["content"]
            elif kind == "attachment" and isinstance((entry.get("attachment") or {}).get("prompt"), str):
                yield entry["attachment"]["prompt"]
            elif kind == "user" and not (entry.get("isMeta") or entry.get("isCompactSummary") or "toolUseResult" in entry):
                content = (entry.get("message") or {}).get("content")
                if isinstance(content, str):
                    yield content
                for part in content if isinstance(content, list) else ():
                    if isinstance(part, dict) and part.get("type") == "text":
                        yield str(part.get("text", ""))


def unsaid(transcript: str, words: str) -> str:
    """Why these words are no order of the owner's; nothing when the owner wrote them in this session."""
    asked = norm(words)
    if len(asked) < WORDS_LEAST:
        return (f"an order is given in the owner's own words, a sentence of theirs ({WORDS_LEAST} characters at least): "
                f"'{words}' is none.")
    if any(asked in norm(text) for text in owner_texts(transcript)):
        return ""
    return (f"the owner did not write '{words}' in this session: an order is the owner's own sentence, copied as it was "
            "written, never one made up nor a summary of one. With no such sentence there is no order.")


def names_of(command: str) -> list:
    """The suites, groups and tasks a run names."""
    found = NAMED.search(command)
    names, skip = [], False
    for token in found.group(1).split() if found else ():
        if skip or token.startswith("--"):
            skip = token in VALUED
            continue
        names += [n for n in token.strip("\"'").split(",") if n]
    return names


def wide(names: list) -> bool:
    """More than a look: more names than a look holds, or a name that stands for many tasks."""
    try:
        doc = json.loads(SUITES.read_text(encoding="utf-8"))
    except (OSError, json.JSONDecodeError):
        doc = {}
    many = {name for part in WIDE for name in (doc.get(part) or {})}
    return len(names) > MOST or any(n.startswith("@") or n in many for n in names)


def had() -> dict:
    """The runs the session had under the state file as it stands; none once a block moved."""
    stamp = STATE.stat().st_mtime_ns if STATE.is_file() else 0
    try:
        doc = json.loads(LOOKS.read_text(encoding="utf-8")) if LOOKS.is_file() else {}
    except (OSError, json.JSONDecodeError):
        doc = {}
    return doc if doc.get("stamp") == stamp else {"stamp": stamp}


def keep(doc: dict) -> None:
    LOOKS.parent.mkdir(parents=True, exist_ok=True)
    LOOKS.write_text(json.dumps(doc, indent=1) + "\n", encoding="utf-8")


def held(command: str, state: str) -> str:
    """Why this run of the project's tools may not start; nothing when it may. A run that passes is written down, and an
    order of the owner's that let it pass is spent by it."""
    if FREE.search(command) and not ORDERED.search(command):
        return ""
    tool = ORDERED.search(command)
    names = names_of(command)
    kind = "measure" if names and all(n.split("@")[0] in MEASURES for n in names) else "checks"
    doc = had()
    why = ""
    if tool:
        why = f"`{tool.group(1)}` runs only on the owner's order (CLAUDE.md, Commands)."
    elif state != "closed":
        return ""       # a block's own runs: tools/tasks/block.py holds them
    elif wide(names):
        why = (f"`run {' '.join(names)}` is more than a look ({MOST} tasks at most, each named): a whole suite or a group "
               "runs only on the owner's order.")
    elif doc.get(kind):
        why = (f"with no block open the game was already looked at (`{doc[kind]['command']}`, {doc[kind]['at']}): one run "
               "of checks and one measure, then the code is read. A second run to see is trial and error.")
    if why and not ORDER.is_file():
        return "CLAUDE.md: " + why + " " + HOW
    if why:
        ORDER.unlink()
    elif state == "closed":
        doc[kind] = {"command": command[:200], "at": time.strftime("%Y-%m-%d %H:%M")}
        keep(doc)
    return ""


def when(stamp) -> float:
    """A record's time as seconds; now for a record that has none."""
    try:
        return datetime.fromisoformat(str(stamp).replace("Z", "+00:00")).timestamp()
    except ValueError:
        return time.time()


def unseen(read: dict, plan: str) -> list:
    """The pictures a commit asks looked at and that were not, since the session's start or its last compaction: each photo
    of the plan, opened after it was taken, and each reference of the look."""
    shots = sorted((SHOTS / plan).glob("*.png"))
    if not shots:
        return [f"{port_note.rel(SHOTS / plan)}: no photo there"]
    late = [f for f in shots if read.get(port_note.key(f), 0.0) < f.stat().st_mtime]
    return [port_note.rel(f) for f in late + [f for f in sorted(REFS.rglob("*.png")) if port_note.key(f) not in read]]
