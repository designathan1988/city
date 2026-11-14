"""The work's note (doc/work/<name>.md) as the hooks read it: which file it is, its section `## Now` (the work's state at this
moment: what a compaction or a new session goes on from), when that section last changed and against which block, what is
asked of it, and what the summary of a compaction must keep.

What survives a compaction (code.claude.com/docs/en/context-window, "What survives compaction"): CLAUDE.md and auto memory
are re-injected from disk; invoked skills' bodies come back up to 5,000 tokens each and 25,000 in all, the oldest dropped
first; up to five files read or edited come back, but "a file over 5,000 tokens comes back as a path reference without its
content"; SessionStart hooks of the compact source run and their output is added. A note is longer than that, so after a
compaction the gate has it Read whole (port_gate.ungrounded); what a hook gives back of it is its `## Now`, so that section
must be the state at this moment: the gate measures its age by the section itself (NOW_SEEN), never by the file, since a
note written below it with the state above left as it was would give a compaction a state that is no more.

Why `fit`: Claude Code caps each text a hook gives at 10,000 characters and replaces a longer one with "the file path and a
preview of up to the first 2,000 characters"; it "doesn't ask Claude to read the file", and "this cap has no setting or
environment variable to raise it" (code.claude.com/docs/en/hooks). So every text a hook answers passes through `fit`: a
longer one is cut with a pointer to its whole, never left to the 2,000-character preview. `## Now` itself has no bound: what
does not fit the hook's text is in the note, which the gate has Read whole."""

import hashlib
import json
import re
import time
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
WORK = ROOT / "doc" / "work"
ASKED = ROOT / "dist" / "block" / "note_asked"   # when the note was last asked for (its time of writing)
NOW_SEEN = ROOT / "dist" / "block" / "now_seen.json"   # when the note's `## Now` last changed, and the block as it stood
HOOK_MOST = 9500     # characters of a hook's answer: Claude Code gives 10,000 whole, a longer one as a 2,000 preview
NOTE_SECONDS = 900   # `## Now` older than this while a block is not closed: it is rewritten before the work goes on
NOW = re.compile(r"^## +Now\b", re.IGNORECASE)
CUT = "\n[cut here: a hook's text holds 10,000 characters. Read the note itself for the rest.]"
SHAPE = ("its section `## Now`, the first after the title, rewritten (not appended to) each time: the owner's standing orders "
         "in their words, where the block stands (its state, the verifications used), what is in hand with its facts "
         "(file:line), and the next steps in order")


def key(path) -> str:
    """A path as two records of it compare equal."""
    return str(path).replace("\\", "/").lower()


def rel(path: Path) -> str:
    try:
        return path.relative_to(ROOT).as_posix()
    except ValueError:
        return path.as_posix()


def note_file(k: dict) -> Path | None:
    """The note of the work in hand: the block's (k, its state) when it is not closed, else the one written last in
    doc/work; none when there is none."""
    if k.get("state") != "closed":
        named = WORK / f"{k.get('name')}.md"
        return named if named.is_file() else None
    notes = sorted(WORK.glob("*.md"), key=lambda f: f.stat().st_mtime) if WORK.is_dir() else []
    return notes[-1] if notes else None


def now_of(text: str) -> str:
    """The lines under a note's `## Now`, to its next section; nothing when it has none."""
    out, inside = [], False
    for line in text.splitlines():
        if line.startswith("## "):
            if inside:
                break
            inside = bool(NOW.match(line))
        elif inside:
            out.append(line)
    return "\n".join(out).strip()


def now_fault(text: str) -> str:
    """Why a note may not be left with this text; nothing when it may."""
    if not now_of(text):
        return "it has no section `## Now`. A note holds " + SHAPE + "."
    return ""


def mark(k: dict) -> list:
    """The block as a `## Now` is written against: its name, its state, its round."""
    return [k.get("name"), k.get("state"), k.get("round", 0)]


def now_seen(k: dict) -> dict:
    """When the note's `## Now` last changed (`at`, seconds) and the block as it stood then (`block`, its mark), kept in
    NOW_SEEN and moved whenever the section's text differs from the one kept; nothing when there is no note. The first time a
    note is seen, its file's time and the block as it stands are taken."""
    note = note_file(k)
    if note is None:
        return {}
    digest = hashlib.sha1(now_of(note.read_text(encoding="utf-8")).encode("utf-8")).hexdigest()
    try:
        seen = json.loads(NOW_SEEN.read_text(encoding="utf-8")) if NOW_SEEN.is_file() else {}
    except (OSError, json.JSONDecodeError):
        seen = {}
    if seen.get("note") != rel(note) or seen.get("hash") != digest:
        seen = {"note": rel(note), "hash": digest, "at": note.stat().st_mtime, "block": mark(k)}
        NOW_SEEN.parent.mkdir(parents=True, exist_ok=True)
        NOW_SEEN.write_text(json.dumps(seen) + "\n", encoding="utf-8")
    return seen


def stale(k: dict) -> str:
    """Why the work may not go on before the note's `## Now` is rewritten: the block moved since it was written (opened,
    verified, opened again on a cause), or it is older than NOTE_SECONDS. Nothing when the block is closed or `## Now` is
    the state at this moment."""
    if k.get("state") in (None, "closed"):
        return ""
    note = note_file(k)
    name = f"doc/work/{k.get('name')}.md"
    if note is None:
        return (f"CLAUDE.md: the block {k.get('name')} has no note {name}. Write it before anything else, with " + SHAPE
                + ": a compaction keeps nothing of what was read but that file.")
    seen = now_seen(k)
    if seen.get("block") != mark(k):
        was = seen.get("block") or [None, "unknown", 0]
        return (f"CLAUDE.md: the block {k.get('name')} is {k.get('state')} (round {k.get('round', 0)}) and the `## Now` of "
                f"{name} was written when it was {was[1]} (round {was[2]}): after a compaction it would give back a state "
                "that is no more. Rewrite `## Now` first (" + SHAPE + "); then do this again.")
    old = time.time() - float(seen.get("at", 0))
    if old < NOTE_SECONDS:
        return ""
    return (f"CLAUDE.md: the `## Now` of {name} was last rewritten {int(old // 60)} minutes ago; it may not fall more than "
            f"{NOTE_SECONDS // 60} behind the work, since it is what a compaction gives back (a note written below it with "
            "`## Now` left as it was does not count). Rewrite `## Now` first (" + SHAPE + ") and add below it what was read "
            "and done since; then do this again.")


def fit(text: str, most: int = HOOK_MOST) -> str:
    """A hook's answer within what Claude Code gives the session whole."""
    return text if len(text) <= most else text[:most - len(CUT)] + CUT


def note_back(k: dict) -> list:
    """The state the note holds, as a session's start and a compaction give it back."""
    note = note_file(k)
    if note is None:
        return ["There is no note of the work in doc/work: what was read before is lost; write one as the work goes."]
    now = now_of(note.read_text(encoding="utf-8"))
    seen = now_seen(k)
    minutes = int((time.time() - float(seen.get("at", note.stat().st_mtime))) // 60)
    if not now:
        return [f"The note {rel(note)} has no section `## Now`: Read it whole, then write that section first ({SHAPE})."]
    was = seen.get("block") or [None, "unknown", 0]
    moved = ("" if was == mark(k) else f" It was written when the block was {was[1]} (round {was[2]}); the block is "
             f"{k.get('state')} now (round {k.get('round', 0)}): rewrite it first.")
    return [f"The note {rel(note)}, its `## Now` as rewritten {minutes} minutes ago (the rest of the file is what was read, "
            f"designed and done: Read it whole before anything else).{moved}", now]


def checkpoint(k: dict) -> str:
    """The reminder to rewrite the note's `## Now`, when it is older than NOTE_SECONDS and was not asked for in that time;
    else nothing."""
    now = time.time()
    note = note_file(k)
    if note is not None and now - float(now_seen(k).get("at", 0)) < NOTE_SECONDS:
        return ""
    if ASKED.is_file() and now - ASKED.stat().st_mtime < NOTE_SECONDS:
        return ""
    ASKED.parent.mkdir(parents=True, exist_ok=True)
    ASKED.write_text(str(int(now)), encoding="utf-8")
    name = k.get("name") if k.get("state") != "closed" else None
    where = f"doc/work/{name}.md" if name else (rel(note) if note else "doc/work/<the work's name>.md")
    return (f"Checkpoint (CLAUDE.md): the `## Now` of the work's note was not rewritten in the last {NOTE_SECONDS // 60} "
            f"minutes. A compaction gives back that section and keeps nothing of what was read but that file. Write into "
            f"{where} now, before going on: rewrite " + SHAPE
            + "; below it, what was read (facts with file:line), what was decided for each case and what is done.")


def summary_orders(k: dict) -> str:
    """What the summary of a compaction must keep: PreCompact's stdout is its instructions, and Claude Code "cuts longer
    output at the cap" of 10,000 characters (code.claude.com/docs/en/hooks), so it is given through `fit`."""
    note = note_file(k)
    where = rel(note) if note else "none yet"
    return ("Keep in the summary, exactly and whole: (1) every order the owner gave in this conversation, in the owner's own "
            "words, the latest last; (2) the work in hand, its board line (doc/port.md) and the block's state with the "
            f"verifications used; (3) the path of the work's note ({where}) and that it is Read whole before anything else; "
            "(4) every fact read from the code (with file:line) and every design decision that is not in that note yet; "
            "(5) the files changed so far, what is coded and what is left, in order; (6) the commands that were run and what "
            "each answered, the failures with their causes; (7) that after this compaction the hook refuses every edit and "
            "command until the skill `port` is invoked again and the note is Read (and the owner's words that the hook's "
            "text cannot hold, written to dist/block/owner_words.md, when it says so), and that the skills "
            "superpowers:systematic-debugging (before a cause is named) and superpowers:verification-before-completion "
            "(before a commit), Context7 and the internet (before the game's code is edited) are used again. Do not shorten "
            "file paths, numbers or names.")
