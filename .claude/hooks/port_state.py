"""Where the port stands, read from the project's own records and put in the session's context: the next feature
(the first unchecked line of doc/port.md), how many are done, the open block (dist/block/state.json) with the state its
note holds (the `## Now` of doc/work/<block>.md), what is uncommitted and the last commits.

    python .claude/hooks/port_state.py            the state, as text (any time, by hand)
    python .claude/hooks/port_state.py hook       Claude Code's hook: the event's JSON on stdin, the state as
                                                  hookSpecificOutput.additionalContext on stdout (SessionStart: whole;
                                                  UserPromptSubmit and SubagentStart: one line; PreToolUse: the
                                                  refusals of port_gate.py, and of everything but the hooks' own
                                                  repair when that file does not load;
                                                  PostToolUse: the gates of the file just edited, and the note asked
                                                  for when it is old; PreCompact: what the summary of a compaction
                                                  must keep; Stop: refused once while a block is not closed and no
                                                  run of its own is going)
Nothing read may be lost to a compaction: the note doc/work/<name>.md is what survives one (CLAUDE.md). The gate keeps it no
more than port_note.NOTE_SECONDS behind the work, the summary is told to keep what is not in it yet, its `## Now` comes back
whole after (every answer here fits what Claude Code gives a session whole: port_note.fit), and nothing is edited or run
after a compaction before the skill `port` is invoked and the note Read again (port_gate.ungrounded).
"""

import json
import re
import subprocess
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
sys.path.insert(0, str(Path(__file__).resolve().parent))

try:
    import port_note  # noqa: E402  the work's note: its state, its age, what a compaction keeps
    from port_gate import CONTEXT7, after_edit, block, gate  # noqa: E402  the hook's refusals, and the gates after an edit
    FAULT = ""
except Exception as fault:  # noqa: BLE001  a gate that does not load, whatever the reason, refuses all but its own repair
    FAULT = f"{type(fault).__name__}: {fault}"

BOARD = ROOT / "doc" / "port.md"
LINE = re.compile(r"^- \[( |x|-)\] (.+)$")
SHOWN = 400          # characters of a board line shown
BRIEF = 160          # characters of the next line in the one-line state
STOP_SHOWN = 1500    # characters of the note's state shown when a stop is refused
GIT_SECONDS = 5
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
    if k["state"] != "closed":
        return (f"Port ({b['done']} of {b['total']} done, doc/port.md): IN HAND = the block {k.get('name')} ({k['state']}), by "
                f"the `## Now` of doc/work/{k.get('name')}.md; no other work before it is closed. The skill `port`, Context7 "
                "and the internet first; steps and commands: CLAUDE.md, 'One functionality, start to end'.")
    return (f"Port ({b['done']} of {b['total']} done, doc/port.md): next = {b['next'][:BRIEF] or 'nothing left'}; no block open. "
            "Skills and Context7 first; steps and commands: CLAUDE.md, 'One functionality, start to end'.")


def whole(source: str = "") -> str:
    b, k = board(), block()
    if "error" in b:
        return "City, the port: " + b["error"]
    lines = [f"City, where the port stands ({b['done']} of {b['total']} functionalities done; the board is doc/port.md):"]
    after = f"{b['next'][:SHOWN] or 'nothing left on the board'}" + (f"  [{b['section']}]" if b["next"] else "")
    if k["state"] == "closed":
        lines.append("NEXT: " + after)
        lines.append(f"FIRST: invoke the project's skill `port` (Skill tool): it is the procedure. Load Context7 ({CONTEXT7}) "
                     "and WebSearch. The game's code cannot be edited before the skill, Context7 and the internet were used, "
                     "nor without its block open.")
        row = FEATURE.match(b["next"])
        if row:
            lines.append(f"TRACE IT (both games, about a minute): python tools/tracer/port.py sheet {row.group(1)}; then read "
                         "its code whole in both games, code the whole functionality with nothing run, and at the end build, "
                         "test, photograph and analyse (CLAUDE.md, 'One functionality, start to end'). Its cases: the lines "
                         "under it in doc/port.md.")
        lines.append("Block: none open.")
    else:
        note = {"open": "being coded, nothing runs until `block verify`",
                "verify": "being checked: its code changes only after `block open --cause`"}.get(k["state"], "")
        lines.append(f"IN HAND: the block {k.get('name')}, {k['state']}" + (f" ({note})" if note else "")
                     + ". It is finished before anything else; the board's line after it: " + after)
    changed = git("status", "--short")
    lines.append(f"Uncommitted: {len(changed)} file(s)" + (": " + ", ".join(c[3:] for c in changed[:6]) if changed else ""))
    lines.append("Last commits: " + " | ".join(c[:110] for c in git("log", "-3", "--format=%h %s")))
    if source == "compact":
        lines.append("The conversation was just compacted. The summary above is no source: the orders are CLAUDE.md (given "
                     "again whole by Claude Code), the procedure is the skill `port`, what was read and decided is the note.")
    if k["state"] != "closed":
        lines.append(f"STAND ON THE RECORDS FIRST (the hook refuses every edit and command until both are done): 1. the Skill "
                     f"tool, `port`; 2. the Read tool, doc/work/{k.get('name')}.md whole. Then, at their points: Context7 and "
                     "the internet before the game's code, superpowers:systematic-debugging before a cause is named, "
                     "superpowers:verification-before-completion before a commit.")
    if k["state"] != "closed" or source == "compact":
        lines += port_note.note_back(k)
    return port_note.fit("\n".join(lines))


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
    return (f"The block {name} is {k['state']}: its functionality is not delivered, and the owner does not accept work left on "
            "the way nor a question about what to do next. Stop only when a third verification failed or when the owner "
            "alone can decide; then the first line of the answer says which, with the proof. Else go on now with the next "
            "step of its note. " + "\n".join(port_note.note_back(k))[:STOP_SHOWN])


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
        sys.stdout.write(port_note.summary_orders(block()))
        return 0
    if event == "PostToolUse":
        asked = "\n".join(said for said in (after_edit(payload), port_note.checkpoint(block())) if said)
        if asked:
            sys.stdout.write(json.dumps({"hookSpecificOutput": {"hookEventName": event, "additionalContext": port_note.fit(asked)}}))
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
