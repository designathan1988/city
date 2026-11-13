# OS file locks, shared or exclusive, that hold across processes and across handles of one process (LockFileEx on Windows,
# flock on POSIX): the locks a task declares (the dll readers/writer lock of every tool on this machine, the GPU), the
# machine's processors (machine.py), the task store's ledger (ledger.py) and the pruning of old runs (runs.py).
from __future__ import annotations

import os
from pathlib import Path

if os.name == "nt":
    import ctypes as C
    import ctypes.wintypes as W
    import msvcrt

    _k32 = C.WinDLL("kernel32", use_last_error=True)

    class _Overlapped(C.Structure):
        _fields_ = [("Internal", C.c_void_p), ("InternalHigh", C.c_void_p), ("Offset", W.DWORD), ("OffsetHigh", W.DWORD),
                    ("hEvent", W.HANDLE)]

    _k32.LockFileEx.argtypes = [W.HANDLE, W.DWORD, W.DWORD, W.DWORD, W.DWORD, C.POINTER(_Overlapped)]
    _k32.LockFileEx.restype = W.BOOL
    _k32.UnlockFileEx.argtypes = [W.HANDLE, W.DWORD, W.DWORD, W.DWORD, C.POINTER(_Overlapped)]
    _k32.UnlockFileEx.restype = W.BOOL
    _FAIL_IMMEDIATELY, _EXCLUSIVE = 0x1, 0x2
    _LOCK_VIOLATION = 33
else:
    import fcntl


class FileLock:
    """One handle on a lock file. try_acquire never waits: the caller decides what to do while it is held elsewhere."""

    def __init__(self, path: Path | str):
        self.path = Path(path)
        self._fd = None

    def try_acquire(self, shared: bool) -> bool:
        if self._fd is not None:
            raise RuntimeError(f"{self.path}: already held by this handle")
        self.path.parent.mkdir(parents=True, exist_ok=True)
        fd = os.open(self.path, os.O_RDWR | os.O_CREAT, 0o644)
        try:
            ok = self._lock(fd, shared)
        except BaseException:
            os.close(fd)
            raise
        if not ok:
            os.close(fd)
            return False
        self._fd = fd
        return True

    def release(self) -> None:
        if self._fd is None:
            return
        fd, self._fd = self._fd, None
        try:
            if os.name == "nt":
                if not _k32.UnlockFileEx(msvcrt.get_osfhandle(fd), 0, 1, 0, C.byref(_Overlapped())):
                    raise OSError(C.get_last_error(), f"UnlockFileEx {self.path}")
            else:
                fcntl.flock(fd, fcntl.LOCK_UN)
        finally:
            os.close(fd)

    @staticmethod
    def _lock(fd: int, shared: bool) -> bool:
        if os.name == "nt":
            flags = _FAIL_IMMEDIATELY | (0 if shared else _EXCLUSIVE)
            if _k32.LockFileEx(msvcrt.get_osfhandle(fd), flags, 0, 1, 0, C.byref(_Overlapped())):
                return True
            err = C.get_last_error()
            if err == _LOCK_VIOLATION:
                return False
            raise OSError(err, "LockFileEx")
        try:
            fcntl.flock(fd, (fcntl.LOCK_SH if shared else fcntl.LOCK_EX) | fcntl.LOCK_NB)
            return True
        except BlockingIOError:
            return False

    def __enter__(self):
        return self

    def __exit__(self, *exc):
        self.release()
