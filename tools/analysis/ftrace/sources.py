"""Lists the files of a source tree (git's view when the tree is in a work tree) and reads the text ones."""
import os
import subprocess

# git's own binary probe: a NUL byte in the first 8000 bytes marks a file as binary (buffer_is_binary).
BINARY_PROBE_BYTES = 8000
GIT_DEADLINE_S = 30


def list_files(root):
    """Relative POSIX paths of every file under root: tracked and untracked-not-ignored when git knows the tree."""
    root = os.path.abspath(root)
    rels = _git_files(root) or _walk_files(root)
    return sorted({r.replace('\\', '/') for r in rels})


def read_text(root, rel):
    """The file's bytes, or None when the file is binary."""
    with open(os.path.join(root, rel), 'rb') as handle:
        data = handle.read()
    return None if b'\0' in data[:BINARY_PROBE_BYTES] else data


def _git_files(root):
    try:
        done = subprocess.run(['git', '-C', root, 'ls-files', '-z', '--cached', '--others', '--exclude-standard'],
                              capture_output=True, timeout=GIT_DEADLINE_S)
    except (OSError, subprocess.TimeoutExpired):
        return None
    if done.returncode != 0:
        return None
    names = done.stdout.decode('utf-8', 'surrogateescape').split('\0')
    return [n for n in names if n and os.path.isfile(os.path.join(root, n))]


def _walk_files(root):
    found = []
    for folder, dirs, files in os.walk(root):
        dirs[:] = sorted(d for d in dirs if not d.startswith('.'))
        found.extend(os.path.relpath(os.path.join(folder, f), root) for f in files)
    return found
