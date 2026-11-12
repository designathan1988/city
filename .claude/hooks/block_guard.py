"""PreToolUse hook of the project (Bash and PowerShell): the block rule held at the shell. While a block is open
(tools/tasks/block.py) a command that starts the engine, a build, a test runner, the gates, an orchestrator run, an export or a
commit is refused: exit 2, the reason on stderr (Claude Code's hooks reference: exit 2 blocks a PreToolUse call). The answer is
block.py's own, so the tools and this hook can never disagree."""
import json
import os
import sys

ROOT = os.path.abspath(os.path.join(os.path.dirname(os.path.abspath(__file__)), "..", ".."))
REFUSED = 2

sys.path.insert(0, os.path.join(ROOT, "tools", "tasks"))
import block  # noqa: E402  the block rule's one owner (standard library only)


def main():
    try:
        event = json.load(sys.stdin)
    except ValueError:
        return 0
    command = str((event.get("tool_input") or {}).get("command") or "")
    why = block.command_refusal(command)
    if why:
        print(why, file=sys.stderr)
        return REFUSED
    return 0


if __name__ == "__main__":
    sys.exit(main())
