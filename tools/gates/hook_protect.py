#!/usr/bin/env python3
"""Claude Code PreToolUse hook (Edit|Write|Bash|PowerShell): the files that define the gates and the hooks are the
user's; a session can read and run them but never change them. Everything else passes in silence (exit 0, no output).

Protected, relative to the project root: tools/gates/rules.json, tools/gates/owners.json, tools/gates/run_gates.py,
tools/gates/gate_*.py, tools/gates/hook_*.py, the schemas that already exist (a schema for a new table may be created),
and .claude/settings*.json. A shell command that names a protected file passes only when every segment naming it is a
plain read (cat, head, grep, git diff...) or runs the script (python tools/gates/run_gates.py). A program that writes
the file from inside its own code is not seen here: the commits of tools/gates are the user's to review."""
import fnmatch
import json
import os
import re
import sys

PROTECTED = ("tools/gates/rules.json", "tools/gates/owners.json", "tools/gates/run_gates.py",
             "tools/gates/gate_*.py", "tools/gates/hook_*.py", ".claude/settings.json", ".claude/settings.local.json")
EXISTING_ONLY = ("tools/gates/schemas/*.json",)
NAME = re.compile(r"[\w./\\~:-]*(?:tools[/\\]gates[/\\][\w.*-]+|\.claude[/\\]settings[\w.]*)")
SEGMENT = re.compile(r"\|\||&&|\||;|\n")
PREFIX = re.compile(r"^\s*(?:cd\s+\S+\s*(?:&&|;)\s*)?(?:[\w-]+=\S+\s+)*")
READERS = re.compile(r"^(?:cat|head|tail|less|more|wc|grep|rg|egrep|fgrep|findstr|diff|cmp|file|stat|ls|dir|md5sum|"
                     r"sha\w+sum|type|Get-Content|gc|Select-String|sls|Get-Item|Test-Path|git\s+(?:diff|log|show|status|"
                     r"blame|ls-files|grep)|sed\s+-n|awk|python3?|py)\b")
HARMLESS_REDIRECT = re.compile(r"(?:\d?&?>>?\s*(?:/dev/null|\$null|nul)\b|\d>&\d)")
RUNS_SCRIPT = re.compile(r"^(?:python3?|py)(?:\s+-[\w-]+)*\s+\"?[\w./\\:-]*tools[/\\]gates[/\\][\w.-]+\.py\"?")


def relative(path, root):
    try:
        rel = os.path.relpath(os.path.abspath(path), root)
    except ValueError:
        return None
    return rel.replace("\\", "/")


def protected(rel, root):
    if rel is None or rel.startswith(".."):
        return False
    if any(fnmatch.fnmatch(rel, p) for p in PROTECTED):
        return True
    return any(fnmatch.fnmatch(rel, p) for p in EXISTING_ONLY) and os.path.exists(os.path.join(root, rel))


def segment_allowed(segment):
    body = HARMLESS_REDIRECT.sub("", PREFIX.sub("", segment, count=1))
    if RUNS_SCRIPT.match(body):
        return ">" not in body
    if body.startswith(("python", "py ")):
        return False
    return bool(READERS.match(body)) and ">" not in body


def shell_violation(command, root):
    for segment in SEGMENT.split(command):
        names = [n for n in NAME.findall(segment) if protected(relative(os.path.join(root, n), root), root)]
        if names and not segment_allowed(segment):
            return names[0].replace("\\", "/")
    return None


def deny(reason):
    sys.stdout.reconfigure(encoding="utf-8")
    print(json.dumps({"hookSpecificOutput": {"hookEventName": "PreToolUse", "permissionDecision": "deny",
                                             "permissionDecisionReason": reason}}, ensure_ascii=False))


def main():
    try:
        data = json.load(sys.stdin)
    except ValueError:
        return 0
    root = os.path.abspath(os.environ.get("CLAUDE_PROJECT_DIR") or data.get("cwd") or ".")
    tool, inp = data.get("tool_name", ""), data.get("tool_input") or {}
    if tool in ("Edit", "Write", "MultiEdit", "NotebookEdit"):
        rel = relative(inp.get("file_path", ""), root)
        if protected(rel, root):
            deny(f"{rel} define os gates ou os hooks: so o usuario altera. Corrija o codigo, os dados ou o mapa.")
    elif tool in ("Bash", "PowerShell"):
        hit = shell_violation(inp.get("command", ""), root)
        if hit:
            deny(f"{hit} define os gates ou os hooks: so o usuario altera (leitura e execucao continuam livres).")
    return 0


if __name__ == "__main__":
    sys.exit(main())
