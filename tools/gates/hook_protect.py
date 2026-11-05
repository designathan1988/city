#!/usr/bin/env python3
"""Claude Code PreToolUse hook (Edit|Write|Bash|PowerShell): the files that define the gates and the hooks are the
user's; a session can read and run them but never change them. Everything else passes in silence (exit 0, no output).

Protected, relative to the project root: tools/gates/rules.json, tools/gates/owners.json, tools/gates/run_gates.py,
tools/gates/gate_*.py, tools/gates/hook_*.py, the schemas that already exist (a schema for a new table may be created),
and .claude/settings*.json. A shell command that names a protected file passes only when every segment naming it is a
plain read (cat, head, grep, git diff...) or runs the script (python tools/gates/run_gates.py). The body of a heredoc is
data, not a command, unless it feeds an interpreter (python, node, sh...): a note that merely mentions a protected file
passes. A program that writes the file from inside its own code is not seen here: the commits of tools/gates are the
user's to review.

A git commit that carries code (native/, project/) is the delivery point of the process, so it needs the evidence the
tests leave on disk: one log per scene and renderer in dist/tests, as tools/build/run_tests.sh writes them, every one
newer than the code being committed and none with a failure. Without that the commit is refused with the command."""
import fnmatch
import json
import os
import re
import subprocess
import sys

PROTECTED = ("tools/gates/rules.json", "tools/gates/owners.json", "tools/gates/run_gates.py",
             "tools/gates/gate_*.py", "tools/gates/hook_*.py", ".claude/settings.json", ".claude/settings.local.json")
EXISTING_ONLY = ("tools/gates/schemas/*.json",)
NAME = re.compile(r"[\w./\\~:-]*(?:tools[/\\]gates[/\\][\w.*-]+|\.claude[/\\]settings[\w.]*)")
SEGMENT = re.compile(r"\|\||&&|\||;|\n")
PREFIX = re.compile(r"^\s*(?:cd\s+\S+\s*(?:&&|;)\s*)?(?:[\w-]+=\S+\s+)*")
READERS = re.compile(r"^(?:cat|head|tail|less|more|wc|grep|rg|egrep|fgrep|findstr|diff|cmp|file|stat|ls|dir|md5sum|"
                     r"sha\w+sum|type|echo|printf|Get-Content|gc|Select-String|sls|Get-Item|Test-Path|"
                     r"git(?:\s+-C\s+\S+)?\s+(?:diff|log|show|status|blame|ls-files|grep|add|commit)|sed\s+-n|awk|"
                     r"python3?|py)\b")
INTERPRETER = re.compile(r"^(?:python3?|py|node|perl|ruby|bash|sh|zsh|pwsh|powershell)\b")
TESTS_SCRIPT = "tools/build/run_tests.sh"
TEST_LOGS = "dist/tests"
TEST_CMD = "bash tools/build/build.sh e2e"
CODE_ROOTS = ("native/", "project/")
CODE_SKIP = (".uid",)
FAIL_MARK = re.compile(r"SCRIPT ERROR|\bFAIL\b")
GIT_COMMIT = re.compile(r"^git(?:\s+-[cC]\s+\S+)*\s+commit\b")
GIT_ADD = re.compile(r"^git(?:\s+-[cC]\s+\S+)*\s+add\b")
CODE_PATH = re.compile(r"(?<![\w/.-])(?:native|project)/[\w./-]+")
COMMIT_ALL = re.compile(r"\s(?:--all|-[a-zA-Z]*a[a-zA-Z]*)(?:\s|$)")
HEREDOC = re.compile(r"<<-?\s*(['\"]?)(\w+)\1")
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


def strip_heredocs(command):
    """Drops the body of every heredoc whose command is not an interpreter: it is data, not a command."""
    out, lines, i = [], command.split("\n"), 0
    while i < len(lines):
        line = lines[i]
        out.append(line)
        i += 1
        m = HEREDOC.search(line)
        if not m or INTERPRETER.match(PREFIX.sub("", line, count=1)):
            continue
        while i < len(lines) and lines[i].strip() != m.group(2):
            i += 1
        i += 1
    return "\n".join(out)


def shell_violation(command, root):
    for segment in SEGMENT.split(strip_heredocs(command)):
        names = [n for n in NAME.findall(segment) if protected(relative(os.path.join(root, n), root), root)]
        if names and not segment_allowed(segment):
            return names[0].replace("\\", "/")
    return None


def git_lines(root, *args):
    out = subprocess.run(["git", *args], cwd=root, capture_output=True, text=True, encoding="utf8", errors="replace")
    return [l.rstrip() for l in out.stdout.splitlines() if l.strip()] if out.returncode == 0 else []


def expected_logs(root):
    """The log files run_tests.sh writes: every scene in gl_compatibility, the BOTH_RENDERERS ones in forward_plus too."""
    try:
        text = open(os.path.join(root, TESTS_SCRIPT), encoding="utf8").read()
    except OSError:
        return []
    def names(var):
        m = re.search(r"^" + var + r"=\((.*?)\)", text, re.M)
        return [os.path.basename(t) for t in m.group(1).split()] if m else []
    logs = [n + "_gl_compatibility.log" for n in names("TESTS")] + [n + "_forward_plus.log" for n in names("BOTH_RENDERERS")]
    return [os.path.join(root, TEST_LOGS, l) for l in logs]


def committed_code(command, segments, root):
    """The code files a commit in this command would carry: what is staged, plus every change when the same command
    adds files or commits with -a (the add has not run when the hook sees the command), plus the code paths the command
itself names, since a write in the same command has not happened yet either."""
    adds = any(GIT_ADD.match(PREFIX.sub("", seg, count=1)) for seg in segments)
    if adds or COMMIT_ALL.search(command):
        files = [l[3:] for l in git_lines(root, "status", "--porcelain") if len(l) > 3]  # 'XY path'
    else:
        files = git_lines(root, "diff", "--cached", "--name-only")
    files += CODE_PATH.findall(command)  # written by this same command, before the commit runs
    return [f for f in files if f.startswith(CODE_ROOTS) and not f.endswith(CODE_SKIP)]


def commit_without_evidence(command, root):
    segments = SEGMENT.split(strip_heredocs(command))
    if not any(GIT_COMMIT.match(PREFIX.sub("", seg, count=1)) for seg in segments):
        return None
    code = [f for f in committed_code(command, segments, root) if os.path.exists(os.path.join(root, f))]
    if not code:
        return None
    newest = max(os.path.getmtime(os.path.join(root, f)) for f in code)
    logs = expected_logs(root)
    missing = [os.path.relpath(l, root) for l in logs if not os.path.exists(l)]
    if missing or not logs:
        return "faltam logs de teste em " + TEST_LOGS + " (" + ", ".join(missing[:3]) + ")"
    stale = [os.path.relpath(l, root) for l in logs if os.path.getmtime(l) < newest]
    if stale:
        return "logs de teste mais velhos que o codigo (" + ", ".join(stale[:3]) + ")"
    failed = [os.path.relpath(l, root) for l in logs if FAIL_MARK.search(open(l, encoding="utf8", errors="replace").read())]
    if failed:
        return "teste com falha (" + ", ".join(failed[:3]) + ")"
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
        evidence = commit_without_evidence(inp.get("command", ""), root)
        if evidence:
            deny("Commit de codigo sem evidencia de teste: " + evidence + ". Rode " + TEST_CMD +
                 " (todas as cenas, com display) e corrija toda falha antes de commitar.")
    return 0


if __name__ == "__main__":
    sys.exit(main())
