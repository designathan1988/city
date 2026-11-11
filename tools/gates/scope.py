"""What the gates look at: the repository's files as git knows them (tools/tasks' worktree snapshot: tracked plus
untracked-not-ignored, never a filesystem walk), the excluded folders of rules.json, and the lines each file changed
against HEAD. Every git question goes through tools/tasks/inputs.git, the tools' one wrapper (a deadline, its own stdin,
no console window)."""
import os
import re
import subprocess
import sys

TOOLS_DIR = os.path.abspath(os.path.join(os.path.dirname(os.path.abspath(__file__)), ".."))
if TOOLS_DIR not in sys.path:
    sys.path.insert(0, TOOLS_DIR)
from tasks import inputs  # noqa: E402  the one owner of what the worktree holds and changed, and of git's calls

FIXTURES = "tools/gates/tests/fixtures/"  # the self-test's inputs: deliberately failing files, never the real scope
_HUNK = re.compile(r"^@@ -\d+(?:,\d+)? \+(\d+)(?:,(\d+))? @@", re.M)


class GitError(Exception):
    """git could not answer: the scope is unknown, so the run fails."""


def _git(root, *args):
    """git's answer through the tools' wrapper; paths written as they are (core.quotePath off), a failure a GitError."""
    try:
        return inputs.git(root, "-c", "core.quotePath=false", *args)
    except subprocess.TimeoutExpired as e:
        raise GitError(f"git {' '.join(args)}: no answer within {inputs.GIT_TIMEOUT_S} s") from e
    except (RuntimeError, OSError) as e:  # a non-zero exit, or git missing
        raise GitError(str(e)) from e


def _snapshot(root):
    return inputs.snapshot(root)


def listed_files(root):
    """Repo-relative posix paths of the worktree as git sees it: tracked files still on disk plus untracked files that
    .gitignore does not exclude."""
    return sorted(f for f in _snapshot(root).files() if os.path.isfile(os.path.join(root, f)))


def tracked_files(root):
    """Every path in git's index, deleted from the worktree or not."""
    return {f for f in _git(root, "ls-files", "-z").split(chr(0)) if f}


def excluded(path, exclude_dirs):
    """rules.json/exclude_dirs: an entry with '/' is a path prefix; a bare name excludes that folder anywhere."""
    if path.startswith(FIXTURES):
        return True
    parts = path.split("/")
    for e in exclude_dirs:
        if "/" in e:
            if path == e or path.startswith(e + "/"):
                return True
        elif e in parts[:-1]:
            return True
    return False


def head_commit(root):
    """The commit HEAD names, or '' when git cannot tell."""
    try:
        return _git(root, "rev-parse", "HEAD").strip()
    except GitError:
        return ""


def before_text(root, path):
    """The file as HEAD holds it, or None when HEAD has no such file (new, or git cannot tell)."""
    try:
        return _git(root, "show", f"HEAD:{path}")
    except GitError:
        return None


def changed_lines(root, paths=None):
    """{path: new-side line numbers changed against HEAD, or None for a file git does not track yet}; limited to
    `paths` when given."""
    changed = _snapshot(root).changed()
    if paths is not None:
        wanted = set(paths)
        changed = [f for f in changed if f in wanted]
    if not changed:
        return {}
    tracked = tracked_files(root)
    result = {f: None for f in changed if f not in tracked}
    edited = [f for f in changed if f in tracked]
    if not edited:
        return result
    out = _git(root, "diff", "HEAD", "--unified=0", "--no-color", "--no-ext-diff", "--no-renames")
    current = None
    for f in edited:
        result[f] = set()
    for line in out.splitlines():
        if line.startswith("+++ "):
            target = line[4:]
            current = target[2:] if target.startswith("b/") else None
        elif line.startswith("@@") and current in result and result[current] is not None:
            m = _HUNK.match(line)
            start, count = int(m.group(1)), int(m.group(2) if m.group(2) is not None else 1)
            result[current].update(range(start, start + count))
    return result
