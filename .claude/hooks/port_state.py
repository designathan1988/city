"""Where the port stands, read from the project's own records and put in the session's context: the next feature
(the first unchecked line of doc/port.md), how many are done, the open block (dist/block/state.json) with the next steps
its work file names (doc/work/<block>.md), what is uncommitted and the last commits.

    python .claude/hooks/port_state.py            the state, as text (any time, by hand)
    python .claude/hooks/port_state.py hook       Claude Code's hook: the event's JSON on stdin, the state as
                                                  hookSpecificOutput.additionalContext on stdout (SessionStart: whole;
                                                  UserPromptSubmit and SubagentStart: one line; PreToolUse: the
                                                  refusals of port_gate.py, and of everything but the hooks' own
                                                  repair when that file does not load;
                                                  PostToolUse: when the work's note was not written for NOTE_SECONDS,
                                                  it asks for it, once per that time; PreCompact: what the summary
                                                  of a compaction must keep; Stop: refused once while a block is not
                                                  closed and no run of its own is going)
Nothing read may be lost to a compaction: the note doc/work/<name>.md is what survives one (CLAUDE.md), and this hook asks
for it while the work goes, tells the summary to keep it, and gives it back whole after. The orders are CLAUDE.md's.
"""

import json
import re
import subprocess
import sys
import time
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
sys.path.insert(0, str(Path(__file__).resolve().parent))

try:
    from port_gate import CONTEXT7, block, gate  # noqa: E402  the hook's refusals (PreToolUse)
    FAULT = ""
except Exception as fault:  # noqa: BLE001  a gate that does not load, whatever the reason, refuses all but its own repair
    FAULT = f"{type(fault).__name__}: {fault}"

BOARD = ROOT / "doc" / "port.md"
WORK = ROOT / "doc" / "work"
LINE = re.compile(r"^- \[( |x|-)\] (.+)$")
NEXT_HEADING = re.compile(r"^## +Next\b", re.IGNORECASE)
SHOWN = 400          # characters of a board line or a work file's step shown
BRIEF = 160          # characters of the next line in the one-line state
WORK_WHOLE = 9000    # characters of the work file given back whole after a compaction (a hook's text holds 10,000)
NEXT_LINES = 8      # lines of the work file's "Next" section shown
GIT_SECONDS = 5
NOTE_SECONDS = 900   # the note older than this while tools run: it is asked for
ASKED = ROOT / "dist" / "block" / "note_asked"   # when the note was last asked for (its time of writing)
FEATURE = re.compile(r"^([a-z-]+), complete")   # a board line of an area's sheet: its area


def board() -> dict:
    """The board's count and its first unchecked line, with the section it stands in."""
    if not BOARD.is_file():
        return {"error": f"{BOARD.relative_to(ROOT).as_posix()} is missing: the port has no board"}
    done = total = 0
    section = first = first_section = ""
    for line in BOARD.read_text(encoding="utf-8").splitlines():
        if line.startswith("## "):
            section = line[3:].strip()
        found = LINE.match(line)
        if not found or found.group(1) == "-":
            continue
        total += 1
        if found.group(1) == "x":
            done += 1
        elif not first:
            first, first_section = found.group(2), section
    return {"done": done, "total": total, "next": first, "section": first_section}


def work_next(name: str) -> list:
    """The lines under the work file's "Next" heading: what the block's author left to do."""
    path = WORK / f"{name}.md"
    out, inside = [], False
    for line in path.read_text(encoding="utf-8").splitlines():
        if line.startswith("## "):
            inside = bool(NEXT_HEADING.match(line))
        elif inside and line.strip():
            out.append(line.strip()[:SHOWN])
    return out[:NEXT_LINES] or [f"doc/work/{name}.md has no line under '## Next'"]


def note_file() -> Path | None:
    """The note of the work in hand: the open block's, else the one written last in doc/work; none when there is none."""
    k = block()
    if k["state"] != "closed":
        named = WORK / f"{k.get('name')}.md"
        return named if named.is_file() else None
    notes = sorted(WORK.glob("*.md"), key=lambda f: f.stat().st_mtime) if WORK.is_dir() else []
    return notes[-1] if notes else None


def note_back() -> list:
    """The note given back whole: what a compaction, or a new session, goes on from."""
    note = note_file()
    if note is None:
        return ["There is no note of the work in doc/work: what was read before is lost; write one as the work goes."]
    text = note.read_text(encoding="utf-8")
    rel = note.relative_to(ROOT).as_posix()
    return [f"The note {rel}, what was read, decided, done and left"
            + (f" (its first {WORK_WHOLE} characters; Read the file for the rest)" if len(text) > WORK_WHOLE else "") + ":",
            text[:WORK_WHOLE]]


def checkpoint() -> str:
    """The reminder to write the note, when it is older than NOTE_SECONDS and was not asked for in that time; else nothing."""
    now = time.time()
    note = note_file()
    if note is not None and now - note.stat().st_mtime < NOTE_SECONDS:
        return ""
    if ASKED.is_file() and now - ASKED.stat().st_mtime < NOTE_SECONDS:
        return ""
    ASKED.parent.mkdir(parents=True, exist_ok=True)
    ASKED.write_text(str(int(now)), encoding="utf-8")
    k = block()
    name = k.get("name") if k["state"] != "closed" else None
    where = f"doc/work/{name}.md" if name else (note.relative_to(ROOT).as_posix() if note else "doc/work/<the work's name>.md")
    return (f"Checkpoint (CLAUDE.md): the work's note was not written in the last {NOTE_SECONDS // 60} minutes. A compaction keeps "
            f"nothing of what was read but that file. Write into {where} now, before going on: the owner's orders of this "
            "session in their words, what was read (facts with file:line), what was decided for each case, what is done "
            "(files changed) and what is left, in order.")


def summary_orders() -> str:
    """What the summary of a compaction must keep (PreCompact's stdout is its instructions)."""
    note = note_file()
    rel = note.relative_to(ROOT).as_posix() if note else "none yet"
    return ("Keep in the summary, exactly and whole: (1) every order the owner gave in this conversation, in the owner's own "
            "words, the latest last; (2) the work in hand, its board line (doc/port.md) and the open block; (3) the path of "
            f"the work's note ({rel}) and that it is Read whole before anything else; (4) every fact read from the code "
            "(with file:line) and every design decision that is not in that note yet; (5) the files changed so far, what "
            "is coded and what is left, in order; (6) the commands that were run and what each answered, the failures "
            "with their causes. Do not shorten file paths, numbers or names.")


def git(*args: str) -> list:
    try:
        done = subprocess.run(["git", "-C", str(ROOT), *args], capture_output=True, text=True, encoding="utf-8",
                              errors="replace", timeout=GIT_SECONDS)
    except (OSError, subprocess.TimeoutExpired) as fault:
        return [f"git {args[0]} did not answer: {fault}"]
    return done.stdout.splitlines() if done.returncode == 0 else [f"git {args[0]} failed: {done.stderr.strip()[:SHOWN]}"]


def one_line() -> str:
    b, k = board(), block()
    if "error" in b:
        return "Port: " + b["error"]
    held = "no block open" if k["state"] == "closed" else f"block {k.get('name')} is {k['state']}"
    return (f"Port ({b['done']} of {b['total']} done, doc/port.md): next = {b['next'][:BRIEF] or 'nothing left'}; {held}. "
            "Skills and Context7 first; steps and commands: CLAUDE.md, 'One functionality, start to end'.")


def whole(source: str = "") -> str:
    b, k = board(), block()
    if "error" in b:
        return "City, the port: " + b["error"]
    lines = [f"City, where the port stands ({b['done']} of {b['total']} functionalities done; the board is doc/port.md):",
             f"NEXT: {b['next'][:SHOWN] or 'nothing left on the board'}" + (f"  [{b['section']}]" if b["next"] else "")]
    lines.append(f"FIRST: invoke the project's skill `port` (Skill tool): it is the procedure. Load Context7 ({CONTEXT7}) and "
                 "WebSearch. The game's code cannot be edited before the skill, Context7 and the internet were used, nor "
                 "without its block open.")
    row = FEATURE.match(b["next"])
    if row:
        lines.append(f"TRACE IT (both games, about a minute): python tools/tracer/port.py sheet {row.group(1)}; then read "
                     "its code whole in both games, code the whole functionality with nothing run, and at the end build, "
                     "test, photograph and analyse (CLAUDE.md, 'One functionality, start to end'). Its cases: the lines "
                     "under it in doc/port.md.")
    if k["state"] == "closed":
        lines.append("Block: none open.")
    else:
        note = {"open": "being coded, nothing runs until `block verify`", "verify": "its checks are running"}.get(k["state"], "")
        lines.append(f"Block: {k.get('name')} is {k['state']}" + (f" ({note})" if note else "") + ".")
        if source != "compact" and (WORK / f"{k.get('name')}.md").is_file():
            lines.append(f"Its note doc/work/{k.get('name')}.md says next:")
            lines += ["  " + step for step in work_next(str(k.get("name")))]
        elif source != "compact":
            lines.append(f"It has no note doc/work/{k.get('name')}.md: a compaction would lose what was read. Write it now.")
    changed = git("status", "--short")
    lines.append(f"Uncommitted: {len(changed)} file(s)" + (": " + ", ".join(c[3:] for c in changed[:6]) if changed else ""))
    lines.append("Last commits: " + " | ".join(c[:110] for c in git("log", "-3", "--format=%h %s")))
    if source == "compact":
        lines.append("The conversation was just compacted: go on from the note below (Read it whole if it was cut), never from memory.")
        lines += note_back()
    return "\n".join(lines)


def runs_live() -> bool:
    """A run of the project's tools is still going (the orchestrator's own record)."""
    sys.path.insert(0, str(ROOT / "tools"))
    try:
        from tasks.runs import live_runs
        return bool(live_runs(ROOT))
    except (ImportError, OSError):
        return False


def unfinished(payload: dict) -> str:
    """Why the session may not stop yet: a block that is not closed, with no run of its own still going. Nothing the second
    time it is asked in a row (the session then says why it stops)."""
    k = block()
    if payload.get("stop_hook_active") or k["state"] not in ("open", "verify") or runs_live():
        return ""
    name = str(k.get("name"))
    steps = work_next(name) if (WORK / f"{name}.md").is_file() else [f"doc/work/{name}.md is missing: write it"]
    return (f"The block {name} is {k['state']}: its functionality is not delivered, and the owner does not accept work left on "
            "the way nor a question about what to do next. Go on now with the next step of its note: " + " | ".join(steps)
            + " Stop only when a third verification failed or when the owner alone can decide; then the first line of the "
            "answer says which, with the proof.")


def broken(event: str, payload: dict) -> int:
    """The gate did not load: every tool call is refused but an edit of the hooks themselves, and every other event says so."""
    said = (f"The hook's gate .claude/hooks/port_gate.py does not load ({FAULT}): nothing of CLAUDE.md is held by the tools. "
            "Repair that file with Edit before anything else.")
    if event == "PreToolUse":
        path = str((payload.get("tool_input") or {}).get("file_path", "")).replace("\\", "/")
        if "/.claude/hooks/" not in path:
            sys.stdout.write(json.dumps({"hookSpecificOutput": {"hookEventName": event, "permissionDecision": "deny",
                                                                "permissionDecisionReason": said}}))
    elif event in ("SessionStart", "UserPromptSubmit", "SubagentStart", "PostToolUse"):
        sys.stdout.write(json.dumps({"hookSpecificOutput": {"hookEventName": event, "additionalContext": said}}))
    else:
        print(said, file=sys.stderr)
    return 0


def main() -> int:
    if len(sys.argv) < 2:
        print(FAULT and f"port_gate.py does not load: {FAULT}" or whole())
        return 1 if FAULT else 0
    if sys.argv[1] != "hook":
        print(f"port_state.py: unknown argument '{sys.argv[1]}' (none, or 'hook')", file=sys.stderr)
        return 2
    raw = sys.stdin.buffer.read().decode("utf-8", errors="replace")
    payload = json.loads(raw) if raw.strip() else {}
    event = payload.get("hook_event_name", "")
    if FAULT:
        return broken(event, payload)
    if event == "Stop":
        held = unfinished(payload)
        if held:
            sys.stdout.write(json.dumps({"decision": "block", "reason": held}))
        return 0
    if event == "PreToolUse":
        refusal = gate(payload)
        if refusal:
            sys.stdout.write(json.dumps(refusal))
        return 0
    if event == "PreCompact":
        sys.stdout.write(summary_orders())
        return 0
    if event == "PostToolUse":
        asked = checkpoint()
        if asked:
            sys.stdout.write(json.dumps({"hookSpecificOutput": {"hookEventName": event, "additionalContext": asked}}))
        return 0
    if event == "SessionStart":
        context = whole(payload.get("source", ""))
    elif event == "UserPromptSubmit":
        context = one_line()
    elif event == "SubagentStart":
        context = "City project: the orders are CLAUDE.md at " + str(ROOT) + " (read it whole first). " + one_line()
    else:
        print(f"port_state.py: no answer for the event '{event}'", file=sys.stderr)
        return 1
    sys.stdout.write(json.dumps({"hookSpecificOutput": {"hookEventName": event, "additionalContext": context}}))
    return 0


if __name__ == "__main__":
    sys.exit(main())
