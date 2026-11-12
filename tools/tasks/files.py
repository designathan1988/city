# The file primitives the tools' writers share: a temporary name beside a file, a text written whole or not at all, a file or
# folder removed (read-only files included: Windows refuses to unlink a file whose read-only bit is set, as git sets on its
# objects; the bit is cleared and the removal tried again, Python's own shutil.rmtree example), the bytes a file or folder holds.
from __future__ import annotations

import os
import shutil
import stat
import threading
from pathlib import Path


def tmp_path(path: Path) -> Path:
    return path.with_name(f"{path.name}.tmp-{os.getpid()}-{threading.get_ident()}")


def write_atomic(path: Path, text: str) -> None:
    path.parent.mkdir(parents=True, exist_ok=True)
    tmp = tmp_path(path)
    tmp.write_text(text, encoding="utf-8")
    os.replace(tmp, path)


def _writable_again(func, path, _exc) -> None:
    """rmtree's onexc: clear the read-only bit and try the same removal once more; a second failure propagates."""
    os.chmod(path, stat.S_IWRITE)
    func(path)


def remove(path: Path) -> None:
    if path.is_dir() and not path.is_symlink():
        shutil.rmtree(path, onexc=_writable_again)
    elif path.exists() or path.is_symlink():
        try:
            path.unlink()
        except PermissionError:
            _writable_again(os.unlink, path, None)


def size_of(path: Path) -> int:
    """The bytes a file holds, or every file under a folder."""
    if path.is_file():
        return path.stat().st_size
    return sum(f.stat().st_size for f in path.rglob("*") if f.is_file())
