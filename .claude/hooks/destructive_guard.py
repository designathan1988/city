"""PreToolUse hook of the project (Bash and PowerShell): the limits of the owner's standing authorisation (CLAUDE.md, the
Authority section) held at the shell. Refused (exit 2, the reason on stderr): a deletion (rm, Remove-Item, del, rmdir)
by a relative path, a path through "..", a variable the hook cannot read, or a place outside the project, its sibling copies on
the Desktop, the Codex worktrees, the system's temporary folder and this project's own memory folder; a force push; a rewritten
history (rebase, filter-branch, filter-repo, commit --amend). Everything else passes."""
import json
import os
import re
import sys

ROOT = os.path.abspath(os.path.join(os.path.dirname(os.path.abspath(__file__)), "..", ".."))
REFUSED = 2
DELETERS = {"rm", "remove-item", "ri", "del", "erase", "rmdir", "rd"}
SEGMENTS = re.compile(r"&&|\|\||[;\n|]")
TOKEN = re.compile(r'"[^"]*"|\'[^\']*\'|\S+')
ABSOLUTE = re.compile(r"^([A-Za-z]:[\\/]|/|\\\\)")
CMD_SWITCH = re.compile(r"^/[A-Za-z]$")  # cmd's rmdir /s /q
VARIABLE = re.compile(r"\$|%[A-Za-z_]+%")
REWRITES = re.compile(r"\bgit\s+(rebase|filter-branch|filter-repo)\b|\bgit\s+commit\b[^;&|\n]*--amend\b", re.I)
PUSH = re.compile(r"\bgit\s+push\b([^;&|\n]*)", re.I)
FORCE = re.compile(r"(^|\s)(-f|--force(-with-lease)?(=\S*)?|\+\S+)(?=\s|$)")


MSYS_DRIVE = re.compile(r"^/([A-Za-z])(/|$)")  # Git Bash's form of a Windows drive: /c/Users is C:/Users


def _norm(path):
    if os.name == "nt":
        path = MSYS_DRIVE.sub(lambda m: m.group(1) + ":/", path)
    return os.path.normcase(os.path.abspath(path))


def allowed_roots():
    """The places a deletion may reach: the project, its sibling copies, the Codex worktrees, the temporary folder, the memory
    folder Claude Code keeps for this project (its path with every separator and colon written as '-')."""
    home = os.path.expanduser("~")
    desktop = os.path.dirname(ROOT)
    roots = [ROOT, os.path.join(home, ".codex", "worktrees"), os.environ.get("TEMP") or os.environ.get("TMPDIR") or "/tmp",
             os.path.join(home, ".claude", "projects", re.sub(r"[^A-Za-z0-9]", "-", ROOT), "memory")]
    if os.path.isdir(desktop):
        prefix = os.path.basename(ROOT) + "-"
        roots += [os.path.join(desktop, n) for n in os.listdir(desktop) if n.startswith(prefix)]
    return [_norm(r) for r in roots]


def _inside(path, roots):
    p = _norm(path)
    return any(p == r or p.startswith(r + os.sep) for r in roots)


def _deletion_refusal(tokens, roots):
    for raw in tokens[1:]:
        arg = raw.strip("\"'")
        if not arg or arg.startswith("-") or CMD_SWITCH.match(arg):
            continue
        if VARIABLE.search(arg):
            return f"a deletion of '{arg}', whose path the hook cannot read: write the absolute path"
        if not ABSOLUTE.match(arg):
            return f"a deletion by the relative path '{arg}': delete only by an absolute path"
        if ".." in re.split(r"[\\/]", arg):
            return f"a deletion through '..' ('{arg}')"
        if not _inside(arg, roots):
            return f"a deletion outside the project and its listed copies ('{arg}')"
    return None


def refusal(command, roots=None):
    """The reason a shell command breaks the authorisation's limits, or None."""
    if REWRITES.search(command):
        return "a rewritten history (rebase, filter-branch, filter-repo, commit --amend): make a new commit instead"
    for m in PUSH.finditer(command):
        if FORCE.search(m.group(1)):
            return "a force push"
    roots = allowed_roots() if roots is None else roots
    for segment in SEGMENTS.split(command):
        tokens = TOKEN.findall(segment.strip().lstrip("&").strip())
        if tokens and os.path.basename(tokens[0].strip("\"'").replace("\\", "/")).lower() in DELETERS:
            why = _deletion_refusal(tokens, roots)
            if why:
                return why
    return None


def main():
    try:
        event = json.load(sys.stdin)
    except ValueError:
        return 0
    why = refusal(str((event.get("tool_input") or {}).get("command") or ""))
    if why:
        print(f"refused by the owner's standing authorisation's limits (CLAUDE.md): {why}", file=sys.stderr)
        return REFUSED
    return 0


if __name__ == "__main__":
    sys.exit(main())
