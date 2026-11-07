# Shared fixtures of the tools/tasks tests: a throwaway git repo as the root, Python one-liners as task processes. Nothing
# here opens a window or starts Godot: every child is a console Python started without a console.
import os
import shutil
import stat
import subprocess
import sys
import tempfile
import unittest
from pathlib import Path

TOOLS = Path(__file__).resolve().parent.parent.parent
sys.path.insert(0, str(Path(__file__).resolve().parent.parent.parent))

from tasks import proc  # noqa: E402

PY = sys.executable


def git(root: Path, *args: str) -> None:
    subprocess.run(["git", "-C", str(root), "-c", "user.name=tests", "-c", "user.email=tests@localhost", *args],
                   check=True, capture_output=True, timeout=60, creationflags=proc.NO_WINDOW)


class RepoCase(unittest.TestCase):
    """A fresh git repo with one committed file (a.txt) per test."""

    def setUp(self):
        self.root = Path(tempfile.mkdtemp(prefix="tasks-test-")).resolve()
        git(self.root, "init", "-q")
        (self.root / "a.txt").write_text("one\n", encoding="utf-8")
        git(self.root, "add", "a.txt")
        git(self.root, "commit", "-q", "-m", "start")

    def tearDown(self):
        shutil.rmtree(self.root, onexc=_writable)


def _writable(fn, path, exc):
    """git writes its objects read-only; Windows refuses to delete those until they are writable again."""
    if not isinstance(exc, PermissionError):
        raise exc
    os.chmod(path, stat.S_IWRITE)
    fn(path)


def py(code: str) -> list[str]:
    return [PY, "-c", code]
