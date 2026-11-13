"""Claude Code hook: puts the owner's standing orders back into the model's context at every session start
(startup, resume, clear, compact, fork), at every prompt and at every subagent's start: CLAUDE.md, the skill
superpowers:using-superpowers, Context7 for every library, engine or API, and the skills the work needs.

Modes (the first argument), each answering on stdout with the hook's JSON (hookSpecificOutput.additionalContext):
  claude-md  CLAUDE.md as it stands on disk (SessionStart); a hook string is capped at 10,000 characters, so a
             CLAUDE.md past the cap is not cut: the model is told to Read the file and the user is warned.
  orders     the orders for the event in the payload: SessionStart (after a compaction, also the skills this
             session had invoked, read from its transcript), UserPromptSubmit, SubagentStart.
"""

import json
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
CLAUDE_MD = ROOT / "CLAUDE.md"
HOOK_TEXT_CAP = 10_000  # Claude Code's cap on one additionalContext string (code.claude.com/docs/en/hooks)
CONTEXT7_LOAD = ('ToolSearch with query "select:mcp__plugin_context7_context7__resolve-library-id,'
                 'mcp__plugin_context7_context7__query-docs"')
CONTEXT7_USE = ("Context7 (resolve-library-id, then query-docs) for every library, engine or API the work touches "
                "(Godot, godot-cpp, the C++ standard library, CMake, Python and its libraries, Claude Code itself), "
                "at every step: reading, tracing, research, design, coding, debugging, checks and review")


def session_orders(source: str) -> str:
    lines = [
        f"Standing orders of the owner (CLAUDE.md, 'Always, in every session and every conversation'), put back by "
        f"the project's hook at session {source}. Do them now, before any other answer or action:",
        "1. Invoke the Skill tool with superpowers:using-superpowers and follow it: every skill that may apply is "
        "invoked before acting, for the rest of the session.",
        f"2. Load Context7: {CONTEXT7_LOAD}. Then use {CONTEXT7_USE}.",
        "3. Invoke with the Skill tool every skill the work in hand needs, now and at each change of step "
        "(superpowers:brainstorming before creating, superpowers:systematic-debugging on a failure, "
        "superpowers:test-driven-development while coding, superpowers:verification-before-completion before "
        "saying done, and the others the work calls for).",
        "4. Follow CLAUDE.md in full; its text is injected beside this message.",
    ]
    if source == "compact":
        lines.append("The conversation was just compacted: the skills' texts and Context7's tool schemas loaded before "
                     "may be gone from the context. Steps 1 to 3 are done again now, before the work goes on.")
    return "\n".join(lines)


def skills_used(transcript_path: str) -> str:
    """The skills this session invoked (Skill tool calls in its transcript), in order of first use."""
    if not transcript_path:
        return "The hook received no transcript path: the skills invoked before the compaction cannot be listed."
    transcript = Path(transcript_path)
    if not transcript.is_file():
        return f"The session's transcript is not at {transcript}: the skills invoked before cannot be listed."
    seen: dict[str, None] = {}
    with transcript.open(encoding="utf-8", errors="replace") as lines:
        for line in lines:
            if '"Skill"' not in line:
                continue
            try:
                entry = json.loads(line)
            except json.JSONDecodeError:
                continue  # the line Claude Code is still appending may be half-written; whole lines carry the calls
            message = entry.get("message")
            content = message.get("content") if isinstance(message, dict) else None
            for block in content if isinstance(content, list) else ():
                if isinstance(block, dict) and block.get("type") == "tool_use" and block.get("name") == "Skill":
                    name = (block.get("input") or {}).get("skill")
                    if isinstance(name, str) and name:
                        seen.setdefault(name, None)
    if not seen:
        return "No skill was invoked in this session before the compaction."
    return ("Skills this session invoked before the compaction, in order of first use: " + ", ".join(seen)
            + ". Invoke again, with the Skill tool, each one the work in hand still needs.")


def prompt_orders() -> str:
    return ("Standing orders for this message (CLAUDE.md, put back by the project's hook): if the text of "
            "superpowers:using-superpowers is not in this conversation, invoke it first with the Skill tool; check "
            "the skills before answering and invoke every one this message's work needs; use " + CONTEXT7_USE
            + "; if the Context7 tools are not loaded (after a compaction they may not be), load them with "
            + CONTEXT7_LOAD + ".")


def subagent_orders() -> str:
    return ("Standing orders for this subagent (the project's CLAUDE.md, at " + str(CLAUDE_MD) + "): load Context7 "
            "with " + CONTEXT7_LOAD + " and use " + CONTEXT7_USE + "; invoke with the Skill tool the skills your "
            "task needs before doing it; follow CLAUDE.md.")


def claude_md_context(source: str) -> tuple[str, str]:
    """The CLAUDE.md text for the model, and a warning for the user when it cannot travel whole."""
    if not CLAUDE_MD.is_file():
        return (f"CLAUDE.md is not at {CLAUDE_MD}: the project's orders could not be injected.",
                f"standing_orders hook: CLAUDE.md is missing at {CLAUDE_MD}.")
    text = CLAUDE_MD.read_text(encoding="utf-8")
    header = f"CLAUDE.md ({CLAUDE_MD}), read again by the project's hook at session {source}:\n\n"
    if len(header) + len(text) <= HOOK_TEXT_CAP:
        return header + text, ""
    return (f"CLAUDE.md ({CLAUDE_MD}) has {len(text)} characters, more than the {HOOK_TEXT_CAP} a hook can inject: "
            f"Read the whole file now with the Read tool, before any other answer or action.",
            f"standing_orders hook: CLAUDE.md has {len(text)} characters, past the {HOOK_TEXT_CAP} a hook can inject; "
            f"Claude was told to Read it.")


def main() -> int:
    mode = sys.argv[1] if len(sys.argv) > 1 else ""
    raw = sys.stdin.buffer.read().decode("utf-8")
    payload = json.loads(raw) if raw.strip() else {}
    event = payload.get("hook_event_name", "")
    source = payload.get("source", "start")
    warning = ""
    if mode == "claude-md" and event == "SessionStart":
        context, warning = claude_md_context(source)
    elif mode == "orders" and event == "SessionStart":
        context = session_orders(source)
        if source == "compact":
            context += "\n" + skills_used(payload.get("transcript_path", ""))
    elif mode == "orders" and event == "UserPromptSubmit":
        context = prompt_orders()
    elif mode == "orders" and event == "SubagentStart":
        context = subagent_orders()
    else:
        print(f"standing_orders hook: no answer for mode '{mode}' at event '{event}'", file=sys.stderr)
        return 1
    answer: dict = {"hookSpecificOutput": {"hookEventName": event, "additionalContext": context}}
    if warning:
        answer["systemMessage"] = warning
    sys.stdout.write(json.dumps(answer))
    return 0


if __name__ == "__main__":
    sys.exit(main())
