"""The work's note (doc/work/<name>.md) as the hooks read it: which file it is, its section `## Now` (the work's state at this
moment: what a compaction or a new session goes on from), how old it is, what is asked of it, and what the summary of a
compaction must keep.

Why `## Now` and why `fit`: Claude Code caps a hook's text at 10,000 characters and replaces a longer one with "a preview of
up to the first 2,000 characters", and "doesn't ask Claude to read the file, so keep anything Claude must always see within
the cap" (code.claude.com/docs/en/hooks). A note is a history that only grows; its head is its oldest part. So the state
lives in one bounded section, given back whole, and every text a hook answers passes through `fit`."""

import re
import time
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
WORK = ROOT / "doc" / "work"
ASKED = ROOT / "dist" / "block" / "note_asked"   # when the note was last asked for (its time of writing)
HOOK_MOST = 9500     # characters of a hook's answer (Claude Code cuts at 10,000)
NOW_MOST = 6000      # characters of a note's `## Now`: it fits a hook's answer beside the state's own lines
NOTE_SECONDS = 900   # the note older than this while a block is not closed: it is written before the work goes on
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
    now = now_of(text)
    if not now:
        return "it has no section `## Now`. A note holds " + SHAPE + "."
    if len(now) > NOW_MOST:
        return (f"its `## Now` is {len(now)} characters, over the {NOW_MOST} a hook can give back whole after a compaction. "
                "It is the state at this moment, not the history: move what is done into the sections below it.")
    return ""


def stale(k: dict) -> str:
    """Why the work may not go on before its note is written; nothing when the block is closed or the note is fresh."""
    if k.get("state") in (None, "closed"):
        return ""
    note = note_file(k)
    name = f"doc/work/{k.get('name')}.md"
    if note is None:
        return (f"CLAUDE.md: the block {k.get('name')} has no note {name}. Write it before anything else, with " + SHAPE
                + ": a compaction keeps nothing of what was read but that file.")
    old = time.time() - note.stat().st_mtime
    if old < NOTE_SECONDS:
        return ""
    return (f"CLAUDE.md: the note {name} was last written {int(old // 60)} minutes ago (it may not fall more than "
            f"{NOTE_SECONDS // 60} behind the work: a compaction keeps nothing but that file). Rewrite its `## Now` first ("
            + SHAPE + ") and add below it what was read and done since; then do this again.")


def fit(text: str, most: int = HOOK_MOST) -> str:
    """A hook's answer within what Claude Code gives the session whole."""
    return text if len(text) <= most else text[:most - len(CUT)] + CUT


def note_back(k: dict) -> list:
    """The state the note holds, as a session's start and a compaction give it back."""
    note = note_file(k)
    if note is None:
        return ["There is no note of the work in doc/work: what was read before is lost; write one as the work goes."]
    now = now_of(note.read_text(encoding="utf-8"))
    minutes = int((time.time() - note.stat().st_mtime) // 60)
    if not now:
        return [f"The note {rel(note)} has no section `## Now`: Read it whole, then write that section first ({SHAPE})."]
    return [f"The note {rel(note)}, its `## Now` as written {minutes} minutes ago (the rest of the file is what was read, "
            "designed and done: Read it whole before anything else):", now]


def checkpoint(k: dict) -> str:
    """The reminder to write the note, when it is older than NOTE_SECONDS and was not asked for in that time; else nothing."""
    now = time.time()
    note = note_file(k)
    if note is not None and now - note.stat().st_mtime < NOTE_SECONDS:
        return ""
    if ASKED.is_file() and now - ASKED.stat().st_mtime < NOTE_SECONDS:
        return ""
    ASKED.parent.mkdir(parents=True, exist_ok=True)
    ASKED.write_text(str(int(now)), encoding="utf-8")
    name = k.get("name") if k.get("state") != "closed" else None
    where = f"doc/work/{name}.md" if name else (rel(note) if note else "doc/work/<the work's name>.md")
    return (f"Checkpoint (CLAUDE.md): the work's note was not written in the last {NOTE_SECONDS // 60} minutes. A compaction keeps "
            f"nothing of what was read but that file. Write into {where} now, before going on: rewrite " + SHAPE
            + "; below it, what was read (facts with file:line), what was decided for each case and what is done.")


def summary_orders(k: dict) -> str:
    """What the summary of a compaction must keep (PreCompact's stdout is its instructions, with no cap)."""
    note = note_file(k)
    where = rel(note) if note else "none yet"
    return ("Keep in the summary, exactly and whole: (1) every order the owner gave in this conversation, in the owner's own "
            "words, the latest last; (2) the work in hand, its board line (doc/port.md) and the block's state with the "
            f"verifications used; (3) the path of the work's note ({where}) and that it is Read whole before anything else; "
            "(4) every fact read from the code (with file:line) and every design decision that is not in that note yet; "
            "(5) the files changed so far, what is coded and what is left, in order; (6) the commands that were run and what "
            "each answered, the failures with their causes; (7) that after this compaction the hook refuses every edit and "
            "command until the skill `port` is invoked again and the note is Read, and that the skills "
            "superpowers:systematic-debugging (before a cause is named) and superpowers:verification-before-completion "
            "(before a commit), Context7 and the internet (before the game's code is edited) are used again. Do not shorten "
            "file paths, numbers or names.")
