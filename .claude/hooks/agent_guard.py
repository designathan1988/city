"""The project's hook on agents (PreToolUse on the Agent, Bash and PowerShell tools, PostToolUseFailure on Agent, SubagentStop):
CLAUDE.md's rule that at most AT_ONCE agents run at once and that an agent runs commands only in the isolated research folder,
never in the game's tree, in the foreground and under a deadline (the owner, 2026-11-12): any other Bash or PowerShell call made
inside an agent (the event carries agent_id) is refused. A call to the Agent
tool is refused (exit 2, the reason on stderr; Claude Code's hooks reference: exit 2 blocks a PreToolUse call) only while AT_ONCE
agents of the session run. A call let
through holds a place in the session's count; an agent that stops (SubagentStop) or a call that failed to start one
(PostToolUseFailure) gives its place back. The count is one file per session in the system's temporary folder, written under an
OS file lock (tools/tasks/locks.py), so two calls of one message never both take the last place. Standard library only."""
import json
import os
import re
import sys
import tempfile
import time

ROOT = os.path.abspath(os.path.join(os.path.dirname(os.path.abspath(__file__)), "..", ".."))
sys.path.insert(0, os.path.join(ROOT, "tools", "tasks"))
from locks import FileLock  # noqa: E402  the OS file lock every tool of this machine shares

READER = "reader"
# The agents whose tools only read: the project's own, and Claude Code's built-in Explore (no Edit, Write or NotebookEdit),
# which a session has from its start while a project agent added during a session loads only in the next one.
READERS = (READER, "Explore")
DEFAULT_TYPE = "general-purpose"  # what the Agent tool starts when no subagent_type is given
AT_ONCE = 5  # the owner's limit (2026-11-12: "limit it to 5")
REFUSED = 2
COUNTS = os.path.join(tempfile.gettempdir(), "city-agent-guard")
LOCK_WAIT_S = 5.0  # inside the hook's own deadline (.claude/settings.json timeout)
LOCK_POLL_S = 0.05


def _count_file(session: str) -> str:
    return os.path.join(COUNTS, re.sub(r"[^A-Za-z0-9_-]", "_", session or "no-session") + ".json")


def _held(path: str) -> list:
    try:
        with open(path, encoding="utf-8") as fh:
            return list(json.load(fh))
    except (OSError, ValueError):
        return []


def _write(path: str, places: list) -> None:
    os.makedirs(os.path.dirname(path), exist_ok=True)
    tmp = f"{path}.{os.getpid()}.tmp"
    with open(tmp, "w", encoding="utf-8") as fh:
        json.dump(places, fh)
    os.replace(tmp, path)


def refusal(tool_input: dict, running: int) -> str | None:
    """Why this call to the Agent tool may not start, or None."""
    # The owner, 2026-11-12: an agent of any kind may read, write code and test; only how many run at once is bounded.
    if running >= AT_ONCE:
        return f"{running} agents already run (at most {AT_ONCE} at once): wait for one to finish"
    return None


RUNS = ("Bash", "PowerShell")  # the tools that execute
# The owner, 2026-11-12: an agent may write code and test it to validate research, only in the isolated research folder, never in
# the game's tree, in the foreground and under a deadline (an agent's run froze the machine before these bounds).
RESEARCH = "documents/projects/defect-finder-research"
GAME = "desktop/city"
AGENT_DEADLINE_MS = 600_000


def agent_run_refusal(tool_input: dict) -> str | None:
    """Why a command an agent asked to run may not run, or None."""
    text = str(tool_input.get("command") or "").replace("\\", "/").lower()
    if RESEARCH not in text:
        return f"an agent runs commands only in the research folder (cd into C:/Users/Jonathan/Documents/Projects/defect-finder-research)"
    if GAME in text:
        return "an agent runs nothing in the game's tree"
    if tool_input.get("run_in_background"):
        return "an agent runs nothing in the background"
    if not tool_input.get("timeout") or int(tool_input.get("timeout")) > AGENT_DEADLINE_MS:
        return f"an agent's command carries a timeout of at most {AGENT_DEADLINE_MS} ms"
    return None


def handle(event: dict) -> tuple[int, str]:
    """The answer to one hook event: (exit code, reason)."""
    name = event.get("hook_event_name")
    # Claude Code's hooks reference: inside a subagent the event carries agent_id and agent_type
    if name == "PreToolUse" and event.get("tool_name") in RUNS and event.get("agent_id"):
        why = agent_run_refusal(event.get("tool_input") or {})
        return (REFUSED, why) if why else (0, "")
    path = _count_file(str(event.get("session_id") or ""))
    lock = FileLock(path + ".lock")
    deadline = time.monotonic() + LOCK_WAIT_S
    while not lock.try_acquire(shared=False):
        if time.monotonic() > deadline:
            return (REFUSED, "the agents' count is held by another call past its wait") if name == "PreToolUse" else (0, "")
        time.sleep(LOCK_POLL_S)
    try:
        places = _held(path)
        if name == "PreToolUse" and event.get("tool_name") == "Agent":
            why = refusal(event.get("tool_input") or {}, len(places))
            if why:
                return REFUSED, why
            _write(path, places + [str(event.get("tool_use_id") or time.time_ns())])
        elif name == "PostToolUseFailure" and event.get("tool_name") == "Agent":
            gone = str(event.get("tool_use_id") or "")
            _write(path, [p for p in places if p != gone] if gone in places else places[1:])
        elif name == "SubagentStop":
            _write(path, places[1:])
        return 0, ""
    finally:
        lock.release()


def main() -> int:
    try:
        event = json.load(sys.stdin)
    except ValueError:
        return 0
    code, why = handle(event)
    if why:
        print(why, file=sys.stderr)
    return code


if __name__ == "__main__":
    sys.exit(main())
