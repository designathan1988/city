"""What the gates look at: the repository's files as git knows them (tools/tasks' worktree snapshot: tracked plus
untracked-not-ignored, never a filesystem walk), the excluded folders of rules.json, and the lines each file changed
against HEAD."""
import os
import re
import subprocess
import sys

TOOLS_DIR = os.path.abspath(os.path.join(os.path.dirname(os.path.abspath(__file__)), ".."))
FIXTURES = "tools/gates/tests/fixtures/"  # the self-test's inputs: deliberately failing files, never the real scope
_HUNK = re.compile(r"^@@ -\d+(?:,\d+)? \+(\d+)(?:,(\d+))? @@", re.M)


class GitError(Exception):
    """git could not answer: the scope is unknown, so the run fails."""


def _git(root, *args):
    proc = subprocess.run(["git", "-c", "core.quotePath=false", "-C", root, *args], capture_output=True)
    if proc.returncode != 0:
        raise GitError(f"git {' '.join(args)}: {proc.stderr.decode('utf-8', 'replace').strip()}")
    return proc.stdout.decode("utf-8", "replace")


def _snapshot(root):
    sys.path.insert(0, TOOLS_DIR)
    from tasks.inputs import snapshot  # the one owner of what the worktree holds and changed
    return snapshot(root)


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
