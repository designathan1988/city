"""Where the port stands, read from the project's own records and put in the session's context: the next feature
(the first unchecked line of doc/port.md), how many are done, the open block (dist/block/state.json) with the next steps
its work file names (doc/work/<block>.md), what is uncommitted and the last commits.

    python .claude/hooks/port_state.py            the state, as text (any time, by hand)
    python .claude/hooks/port_state.py hook       Claude Code's hook: the event's JSON on stdin, the state as
                                                  hookSpecificOutput.additionalContext on stdout (SessionStart: whole;
                                                  UserPromptSubmit and SubagentStart: one line; PreToolUse of an
                                                  edit: the game's code is not edited before a skill and Context7
                                                  were used in the session, CLAUDE.md step 1)
The orders are CLAUDE.md's.
"""

import json
import re
import subprocess
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
BOARD = ROOT / "doc" / "port.md"
BLOCK = ROOT / "dist" / "block" / "state.json"
WORK = ROOT / "doc" / "work"
LINE = re.compile(r"^- \[( |x|-)\] (.+)$")
NEXT_HEADING = re.compile(r"^## +Next\b", re.IGNORECASE)
SHOWN = 400          # characters of a board line or a work file's step shown
BRIEF = 160          # characters of the next line in the one-line state
NEXT_LINES = 8      # lines of the work file's "Next" section shown
GIT_SECONDS = 5
FEATURE = re.compile(r"^([a-z-]+) (\d+):")   # a board line of an area's sheet: its area and row
GAME_CODE = ("native/src/", "project/")      # what the edit gate holds
CONTEXT7 = ("ToolSearch `select:mcp__plugin_context7_context7__resolve-library-id,"
            "mcp__plugin_context7_context7__query-docs`")


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


def block() -> dict:
    if not BLOCK.is_file():
        return {"state": "closed", "name": None}
    try:
        return json.loads(BLOCK.read_text(encoding="utf-8"))
    except (OSError, json.JSONDecodeError) as fault:
        return {"state": f"unreadable ({fault})", "name": None}


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
            "Skills and Context7 first; steps and commands: CLAUDE.md, 'One feature, start to end'.")


def whole(source: str = "") -> str:
    b, k = board(), block()
    if "error" in b:
        return "City, the port: " + b["error"]
    lines = [f"City, where the port stands ({b['done']} of {b['total']} features done; the board is doc/port.md):",
             f"NEXT: {b['next'][:SHOWN] or 'nothing left on the board'}" + (f"  [{b['section']}]" if b["next"] else "")]
    lines.append(f"FIRST: invoke the skills the work needs (Skill tool) and load Context7 ({CONTEXT7}); the game's code "
                 "cannot be edited before both were used.")
    row = FEATURE.match(b["next"])
    if row:
        lines.append(f"TRACE IT (both games, under a minute): python tools/tracer/port.py sheet {row.group(1)} "
                     f"--feature {row.group(2)}; then read its code whole in both games, code it whole with nothing run, "
                     "and at the end build, test, photograph and analyse (CLAUDE.md, 'One feature, start to end').")
    if k["state"] == "closed":
        lines.append("Block: none open.")
    else:
        note = {"open": "being coded, nothing runs until `block verify`", "verify": "its checks are running"}.get(k["state"], "")
        lines.append(f"Block: {k.get('name')} is {k['state']}" + (f" ({note})" if note else "") + ".")
        if (WORK / f"{k.get('name')}.md").is_file():
            lines.append(f"Its work file doc/work/{k.get('name')}.md says next:")
            lines += ["  " + step for step in work_next(str(k.get("name")))]
    changed = git("status", "--short")
    lines.append(f"Uncommitted: {len(changed)} file(s)" + (": " + ", ".join(c[3:] for c in changed[:6]) if changed else ""))
    lines.append("Last commits: " + " | ".join(c[:110] for c in git("log", "-3", "--format=%h %s")))
    if source == "compact":
        lines.append("The conversation was just compacted: read CLAUDE.md's steps and the work file above again before going on.")
    return "\n".join(lines)


def used(transcript: str) -> tuple:
    """Whether this session, since its start or its last compaction, invoked a skill and asked Context7 (read from its
    transcript's tool calls). A transcript that cannot be read holds nothing back."""
    path = Path(transcript) if transcript else None
    if path is None or not path.is_file():
        return True, True
    skill = docs = False
    with path.open(encoding="utf-8", errors="replace") as lines:
        for line in lines:
            if "compact_boundary" in line:
                skill = docs = False
            if '"tool_use"' not in line:
                continue
            skill = skill or '"name":"Skill"' in line or '"name": "Skill"' in line
            docs = docs or "context7__query-docs" in line
    return skill, docs


def gate(payload: dict) -> dict | None:
    """The refusal of an edit of the game's code made before a skill and Context7 were used; nothing for any other edit."""
    written = str((payload.get("tool_input") or {}).get("file_path", "")).replace("\\", "/")
    root = ROOT.as_posix()
    inside = written[len(root) + 1:] if written.lower().startswith(root.lower() + "/") else written
    if not inside.startswith(GAME_CODE):
        return None
    skill, docs = used(payload.get("transcript_path", ""))
    if skill and docs:
        return None
    owed = [what for what, done in (("invoke the skills the work needs with the Skill tool", skill),
                                   (f"load Context7 ({CONTEXT7}) and ask it (query-docs) about the APIs this code calls", docs))
            if not done]
    return {"hookSpecificOutput": {"hookEventName": "PreToolUse", "permissionDecision": "deny",
                                   "permissionDecisionReason": "CLAUDE.md step 1, before the game's code is edited: "
                                                               + "; ".join(owed) + ". Then make this edit again."}}


def main() -> int:
    if len(sys.argv) < 2:
        print(whole())
        return 0
    if sys.argv[1] != "hook":
        print(f"port_state.py: unknown argument '{sys.argv[1]}' (none, or 'hook')", file=sys.stderr)
        return 2
    raw = sys.stdin.buffer.read().decode("utf-8", errors="replace")
    payload = json.loads(raw) if raw.strip() else {}
    event = payload.get("hook_event_name", "")
    if event == "PreToolUse":
        refusal = gate(payload)
        if refusal:
            sys.stdout.write(json.dumps(refusal))
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
