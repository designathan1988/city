"""The hook's refusals (PreToolUse, called by port_state.py): what may not be done yet by CLAUDE.md. The game's code edited
with no block open, or before the skill `port`, Context7 and the internet were used in the session; the engine or the compiler
started outside the project's tools; a failed verification's cause named before the debugging skill was followed; a block's
work committed before the verification skill was. What the session did is read from its transcript's own records."""

import json
import re
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
BLOCK = ROOT / "dist" / "block" / "state.json"
GAME_CODE = ("native/src/", "project/")      # what the edit gate holds
# the engine, the compiler's driver or the build system started as a command of its own (not named inside another's arguments)
ENGINE = re.compile(r"(?:^|[;&|(]\s*)(?:\"?[\w:/\\.-]*godot[\w.-]*\"?|ninja|cmake)(?:\s|$)", re.IGNORECASE)
CONTEXT7 = ("ToolSearch `select:mcp__plugin_context7_context7__resolve-library-id,"
            "mcp__plugin_context7_context7__query-docs`")


def block() -> dict:
    if not BLOCK.is_file():
        return {"state": "closed", "name": None}
    try:
        return json.loads(BLOCK.read_text(encoding="utf-8"))
    except (OSError, json.JSONDecodeError) as fault:
        return {"state": f"unreadable ({fault})", "name": None}


def used(transcript: str) -> dict:
    """What this session did since its start or its last compaction, from its transcript's own records (a compaction's boundary
    is a system entry, a call is a tool_use block of an assistant's message; the same words inside a file read or a command's
    output are no record): the skills invoked, whether Context7 was asked, whether the internet was searched or read. None
    for a transcript that cannot be read: it holds nothing back."""
    path = Path(transcript) if transcript else None
    if path is None or not path.is_file():
        return {}
    did = {"skills": set(), "docs": False, "web": False, "known": True}
    with path.open(encoding="utf-8", errors="replace") as lines:
        for line in lines:
            if not any(mark in line for mark in ("compact_boundary", '"Skill"', "context7__query-docs", "WebSearch", "WebFetch")):
                continue
            try:
                entry = json.loads(line)
            except json.JSONDecodeError:
                continue   # the line being written now
            if entry.get("type") == "system" and entry.get("subtype") == "compact_boundary":
                did = {"skills": set(), "docs": False, "web": False, "known": True}
                continue
            message = entry.get("message")
            content = message.get("content") if isinstance(message, dict) else None
            for part in content if isinstance(content, list) else ():
                if not isinstance(part, dict) or part.get("type") != "tool_use":
                    continue
                name = str(part.get("name", ""))
                if name == "Skill":
                    did["skills"].add(str((part.get("input") or {}).get("skill", "")))
                did["docs"] = did["docs"] or name.endswith("context7__query-docs")
                did["web"] = did["web"] or name in ("WebSearch", "WebFetch")
    return did


def deny(reason: str) -> dict:
    return {"hookSpecificOutput": {"hookEventName": "PreToolUse", "permissionDecision": "deny", "permissionDecisionReason": reason}}


def gate(payload: dict) -> dict | None:
    """What may not be done yet (CLAUDE.md): the game's code edited with no block open, or before a skill, Context7 and the
    internet were used in the session; a failed verification's cause named before the debugging skill was followed; a block's
    work committed before the verification skill was. Nothing for anything else."""
    did = used(payload.get("transcript_path", ""))
    skills = did.get("skills", set())
    command = str((payload.get("tool_input") or {}).get("command", ""))
    if payload.get("tool_name") == "Bash":
        if ENGINE.search(command):
            return deny("CLAUDE.md: the engine and the compiler are started only by the project's tools, which hold the block's "
                        "rule (nothing runs while code is written, a failed check is not run again as it is): "
                        "`python tools/tasks/cli.py run <task>`, `tools/build/build.sh core-windows`.")
        if not did:
            return None
        if re.search(r"\bblock\s+open\b", command) and "--cause" in command and not any("systematic-debugging" in k for k in skills):
            return deny("CLAUDE.md step 7: a failed check's cause is found by the skill superpowers:systematic-debugging (invoke it "
                        "with the Skill tool and follow it: read the failing lines, the code they name, the evidence) before "
                        "it is named at `block open --cause`. A guess is no cause.")
        if re.search(r"\bgit\b[^|;&]*\bcommit\b", command) and block()["state"] == "verify" and not any(
                "verification-before-completion" in k for k in skills):
            return deny("CLAUDE.md step 8: before a block's work is committed, invoke superpowers:verification-before-completion "
                        "with the Skill tool and follow it (each claim against the output of its check).")
        return None
    written = str((payload.get("tool_input") or {}).get("file_path", "")).replace("\\", "/")
    root = ROOT.as_posix()
    inside = written[len(root) + 1:] if written.lower().startswith(root.lower() + "/") else written
    if not inside.startswith(GAME_CODE):
        return None
    k = block()
    if k["state"] != "open":
        return deny(f"CLAUDE.md step 6: the game's code is written only with its block open (the block is {k['state']}). "
                    + ("A fix after a failed check: `python tools/tasks/cli.py block open --cause \"<check>: <file:line>: <why>\"`."
                       if k["state"] == "verify" else "`python tools/tasks/cli.py block open <area>` first."))
    if not did:
        return None
    owed = [what for what, done in (
        ("invoke the project's skill `port` with the Skill tool (the procedure: it is invoked again after every compaction)",
         any(k == "port" or k.endswith(":port") for k in skills)),
        (f"load Context7 ({CONTEXT7}) and ask it (query-docs) about the APIs this code calls", did["docs"]),
        ("search the internet (WebSearch, WebFetch) for the design decision and for each value of the real world this code "
         "takes from a norm or a published method", did["web"])) if not done]
    return deny("CLAUDE.md steps 1 and 5, before the game's code is edited: " + "; ".join(owed) + ". Then make this edit again.") if owed else None
