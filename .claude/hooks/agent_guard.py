"""The project's hook on agents (PreToolUse and PostToolUseFailure on the Agent tool, SubagentStop): CLAUDE.md's rule that agents
only read, and few at once (the owner, 2026-11-12). A call to the Agent tool is refused (exit 2, the reason on stderr; Claude
Code's hooks reference: exit 2 blocks a PreToolUse call) unless it starts the project's reader (READER, .claude/agents/reader.md,
whose tools read and nothing else), in no worktree of its own, while fewer than AT_ONCE readers of the session run. A call let
through holds a place in the session's count; a reader that stops (SubagentStop) or a call that failed to start one
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
DEFAULT_TYPE = "general-purpose"  # what the Agent tool starts when no subagent_type is given
AT_ONCE = 2  # the owner's "with moderation, never many at the same time" (2026-11-12)
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
    kind = str(tool_input.get("subagent_type") or DEFAULT_TYPE)
    if kind != READER:
        return (f"agents only read (CLAUDE.md, the rule on agents): start the project's '{READER}' agent "
                f"(.claude/agents/reader.md), not '{kind}'; editing, removing, building and running stay with the main session")
    if tool_input.get("isolation"):
        return f"a reader works in no worktree of its own ('isolation': {tool_input.get('isolation')!r})"
    if running >= AT_ONCE:
        return f"{running} readers already run (at most {AT_ONCE} at once): wait for one to finish"
    return None


def handle(event: dict) -> tuple[int, str]:
    """The answer to one hook event: (exit code, reason)."""
    name = event.get("hook_event_name")
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
        elif name == "SubagentStop" and event.get("agent_type") == READER:
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
