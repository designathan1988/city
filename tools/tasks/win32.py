# The Windows side of proc: a process tree in a Job Object (KILL_ON_JOB_CLOSE; created suspended, assigned, then resumed, so
# not one grandchild escapes the job), its standard handles on a log, and the private desktop WinSta0\roadcraft_tests whose
# windows never reach the user's screen. Imported only on Windows.
from __future__ import annotations

import atexit
import ctypes as C
import ctypes.wintypes as W
import msvcrt
import os
import shutil
import subprocess
import threading
import time

DESKTOP = "roadcraft_tests"
KILL_WAIT_S = 30  # after the kill, the wait for the tree to be gone; a tree that outlives it is an error

k32 = C.WinDLL("kernel32", use_last_error=True)
u32 = C.WinDLL("user32", use_last_error=True)


class STARTUPINFOW(C.Structure):
    _fields_ = [("cb", W.DWORD), ("lpReserved", W.LPWSTR), ("lpDesktop", W.LPWSTR), ("lpTitle", W.LPWSTR),
                ("dwX", W.DWORD), ("dwY", W.DWORD), ("dwXSize", W.DWORD), ("dwYSize", W.DWORD),
                ("dwXCountChars", W.DWORD), ("dwYCountChars", W.DWORD), ("dwFillAttribute", W.DWORD),
                ("dwFlags", W.DWORD), ("wShowWindow", W.WORD), ("cbReserved2", W.WORD), ("lpReserved2", C.c_void_p),
                ("hStdInput", W.HANDLE), ("hStdOutput", W.HANDLE), ("hStdError", W.HANDLE)]


class STARTUPINFOEXW(C.Structure):
    _fields_ = [("StartupInfo", STARTUPINFOW), ("lpAttributeList", C.c_void_p)]


class PROCESS_INFORMATION(C.Structure):
    _fields_ = [("hProcess", W.HANDLE), ("hThread", W.HANDLE), ("dwProcessId", W.DWORD), ("dwThreadId", W.DWORD)]


class IO_COUNTERS(C.Structure):
    _fields_ = [(n, C.c_uint64) for n in ("ReadOps", "WriteOps", "OtherOps", "ReadBytes", "WriteBytes", "OtherBytes")]


class JOBOBJECT_BASIC_LIMIT_INFORMATION(C.Structure):
    _fields_ = [("PerProcessUserTimeLimit", C.c_int64), ("PerJobUserTimeLimit", C.c_int64), ("LimitFlags", W.DWORD),
                ("MinimumWorkingSetSize", C.c_size_t), ("MaximumWorkingSetSize", C.c_size_t),
                ("ActiveProcessLimit", W.DWORD), ("Affinity", C.c_size_t), ("PriorityClass", W.DWORD),
                ("SchedulingClass", W.DWORD)]


class JOBOBJECT_EXTENDED_LIMIT_INFORMATION(C.Structure):
    _fields_ = [("BasicLimitInformation", JOBOBJECT_BASIC_LIMIT_INFORMATION), ("IoInfo", IO_COUNTERS),
                ("ProcessMemoryLimit", C.c_size_t), ("JobMemoryLimit", C.c_size_t),
                ("PeakProcessMemoryUsed", C.c_size_t), ("PeakJobMemoryUsed", C.c_size_t)]


JobObjectBasicProcessIdList, JobObjectExtendedLimitInformation = 3, 9
JOB_PID_ROOM = 64  # process ids asked for at first; the list grows when the job holds more
ERROR_MORE_DATA = 234
SYNCHRONIZE, PROCESS_QUERY_LIMITED_INFORMATION = 0x00100000, 0x1000
JOB_OBJECT_LIMIT_KILL_ON_JOB_CLOSE = 0x2000
CREATE_SUSPENDED, CREATE_UNICODE_ENVIRONMENT = 0x4, 0x400
EXTENDED_STARTUPINFO_PRESENT, CREATE_NO_WINDOW = 0x80000, 0x08000000
STARTF_USESTDHANDLES = 0x100
PROC_THREAD_ATTRIBUTE_HANDLE_LIST = 0x20002
GENERIC_ALL = 0x10000000
WAIT_OBJECT_0, WAIT_TIMEOUT = 0, 0x102
INFINITE_MS = 0xFFFFFFFE  # the longest finite wait WaitForSingleObject takes; longer timeouts wait in turns
KILLED_EXIT = 1
RESUME_FAILED = 0xFFFFFFFF

k32.CreateJobObjectW.restype = W.HANDLE
k32.CreateJobObjectW.argtypes = [C.c_void_p, W.LPCWSTR]
k32.SetInformationJobObject.argtypes = [W.HANDLE, C.c_int, C.c_void_p, W.DWORD]
k32.QueryInformationJobObject.argtypes = [W.HANDLE, C.c_int, C.c_void_p, W.DWORD, C.c_void_p]
k32.AssignProcessToJobObject.argtypes = [W.HANDLE, W.HANDLE]
k32.TerminateJobObject.argtypes = [W.HANDLE, W.UINT]
k32.CreateProcessW.argtypes = [W.LPCWSTR, W.LPWSTR, C.c_void_p, C.c_void_p, W.BOOL, W.DWORD, C.c_void_p, W.LPCWSTR,
                               C.c_void_p, C.POINTER(PROCESS_INFORMATION)]
k32.ResumeThread.argtypes = [W.HANDLE]
k32.ResumeThread.restype = W.DWORD
k32.TerminateProcess.argtypes = [W.HANDLE, W.UINT]
k32.OpenProcess.restype = W.HANDLE
k32.OpenProcess.argtypes = [W.DWORD, W.BOOL, W.DWORD]
k32.WaitForSingleObject.argtypes = [W.HANDLE, W.DWORD]
k32.WaitForSingleObject.restype = W.DWORD
k32.GetExitCodeProcess.argtypes = [W.HANDLE, C.POINTER(W.DWORD)]
k32.CloseHandle.argtypes = [W.HANDLE]
k32.InitializeProcThreadAttributeList.argtypes = [C.c_void_p, W.DWORD, W.DWORD, C.POINTER(C.c_size_t)]
k32.UpdateProcThreadAttribute.argtypes = [C.c_void_p, W.DWORD, C.c_size_t, C.c_void_p, C.c_size_t, C.c_void_p, C.c_void_p]
k32.DeleteProcThreadAttributeList.argtypes = [C.c_void_p]
u32.CreateDesktopW.restype = W.HANDLE
u32.CreateDesktopW.argtypes = [W.LPCWSTR, W.LPCWSTR, C.c_void_p, W.DWORD, W.DWORD, C.c_void_p]
u32.CloseDesktop.argtypes = [W.HANDLE]


def _check(ok, what: str):
    if not ok:
        raise OSError(C.get_last_error(), what)
    return ok


_desktop = None
_desktop_lock = threading.Lock()


def hidden_desktop() -> str:
    """The private desktop of the interactive window station, opened once and kept while this process lives."""
    global _desktop
    with _desktop_lock:
        if _desktop is None:
            _desktop = _check(u32.CreateDesktopW(DESKTOP, None, None, 0, GENERIC_ALL, None), "CreateDesktop")
            atexit.register(u32.CloseDesktop, _desktop)
    return f"WinSta0\\{DESKTOP}"


def _environment(env: dict | None):
    merged = {k.upper(): (k, v) for k, v in os.environ.items()}
    for k, v in (env or {}).items():
        merged[k.upper()] = (k, str(v))
    block = "".join(f"{k}={v}\0" for _, (k, v) in sorted(merged.items())) + "\0"
    return C.create_unicode_buffer(block, len(block))


def command_line(argv: list[str] | str, env: dict | None) -> tuple[str | None, str]:
    """(application, command line): a list resolved on PATH (a .bat or .cmd through cmd.exe), a string taken as written."""
    if isinstance(argv, str):
        return None, argv
    exe = shutil.which(argv[0], path=(env or {}).get("PATH") or os.environ.get("PATH"))
    if exe is None:
        raise FileNotFoundError(f"not found: {argv[0]}")
    line = subprocess.list2cmdline([exe] + list(argv[1:]))
    if exe.lower().endswith((".bat", ".cmd")):
        comspec = os.environ.get("COMSPEC", r"C:\Windows\System32\cmd.exe")
        return comspec, f'"{comspec}" /d /s /c "{line}"'
    return exe, line


class JobTree:
    """One process and everything it starts, in a job of their own."""

    def __init__(self, argv, cwd, env, log_file, hidden: bool):
        exe, line = command_line(argv, env)
        self.job = _check(k32.CreateJobObjectW(None, None), "CreateJobObject")
        limits = JOBOBJECT_EXTENDED_LIMIT_INFORMATION()
        limits.BasicLimitInformation.LimitFlags = JOB_OBJECT_LIMIT_KILL_ON_JOB_CLOSE
        try:
            _check(k32.SetInformationJobObject(self.job, JobObjectExtendedLimitInformation, C.byref(limits),
                                               C.sizeof(limits)), "SetInformationJobObject")
            pi = self._create(exe, line, cwd, env, log_file, hidden)
        except BaseException:
            k32.CloseHandle(self.job)
            raise
        self.pid, self.hp = pi.dwProcessId, pi.hProcess
        if not k32.AssignProcessToJobObject(self.job, pi.hProcess):
            err = C.get_last_error()
            k32.TerminateProcess(pi.hProcess, KILLED_EXIT)
            for h in (pi.hThread, pi.hProcess, self.job):
                k32.CloseHandle(h)
            raise OSError(err, "AssignProcessToJobObject")
        resumed = k32.ResumeThread(pi.hThread)
        k32.CloseHandle(pi.hThread)
        if resumed == RESUME_FAILED:
            raise OSError(C.get_last_error(), "ResumeThread")

    @staticmethod
    def _create(exe, line, cwd, env, log_file, hidden) -> PROCESS_INFORMATION:
        with open(os.devnull, "rb") as nul:
            handles = [msvcrt.get_osfhandle(nul.fileno()), msvcrt.get_osfhandle(log_file.fileno())]
            for h in handles:
                os.set_handle_inheritable(h, True)
            size = C.c_size_t()
            k32.InitializeProcThreadAttributeList(None, 1, 0, C.byref(size))
            attrs = C.create_string_buffer(size.value)
            _check(k32.InitializeProcThreadAttributeList(attrs, 1, 0, C.byref(size)), "InitializeProcThreadAttributeList")
            try:
                listed = (W.HANDLE * len(handles))(*handles)  # only these two reach the child, whatever else is inheritable
                _check(k32.UpdateProcThreadAttribute(attrs, 0, PROC_THREAD_ATTRIBUTE_HANDLE_LIST, listed, C.sizeof(listed),
                                                     None, None), "UpdateProcThreadAttribute")
                si = STARTUPINFOEXW()
                si.StartupInfo.cb = C.sizeof(STARTUPINFOEXW)
                si.StartupInfo.dwFlags = STARTF_USESTDHANDLES
                si.StartupInfo.hStdInput = handles[0]
                si.StartupInfo.hStdOutput = si.StartupInfo.hStdError = handles[1]
                si.lpAttributeList = C.addressof(attrs)
                desktop = C.create_unicode_buffer(hidden_desktop()) if hidden else None
                if desktop is not None:
                    si.StartupInfo.lpDesktop = C.cast(desktop, W.LPWSTR)
                pi = PROCESS_INFORMATION()
                flags = CREATE_SUSPENDED | CREATE_UNICODE_ENVIRONMENT | EXTENDED_STARTUPINFO_PRESENT | CREATE_NO_WINDOW
                if not k32.CreateProcessW(exe, C.create_unicode_buffer(line), None, None, True, flags, _environment(env),
                                          str(cwd) if cwd else None, C.byref(si), C.byref(pi)):
                    raise OSError(C.get_last_error(), f"CreateProcess {line[:200]}")
                return pi
            finally:
                k32.DeleteProcThreadAttributeList(attrs)

    def wait(self, timeout_s: float | None) -> int | None:
        end = None if timeout_s is None else time.monotonic() + timeout_s
        while True:
            left = INFINITE_MS if end is None else max(0, int((end - time.monotonic()) * 1000))
            r = k32.WaitForSingleObject(self.hp, min(left, INFINITE_MS))
            if r == WAIT_OBJECT_0:
                code = W.DWORD()
                _check(k32.GetExitCodeProcess(self.hp, C.byref(code)), "GetExitCodeProcess")
                return code.value
            if r != WAIT_TIMEOUT:
                raise OSError(C.get_last_error(), "WaitForSingleObject")
            if end is not None and time.monotonic() >= end:
                return None

    def _pids(self) -> list[int]:
        room = JOB_PID_ROOM
        while True:
            class PidList(C.Structure):
                _fields_ = [("assigned", W.DWORD), ("listed", W.DWORD), ("ids", C.c_size_t * room)]
            ids = PidList()
            if k32.QueryInformationJobObject(self.job, JobObjectBasicProcessIdList, C.byref(ids), C.sizeof(ids), None):
                return [int(ids.ids[n]) for n in range(ids.listed)]
            if C.get_last_error() != ERROR_MORE_DATA:
                raise OSError(C.get_last_error(), "QueryInformationJobObject")
            room *= 2

    def kill(self) -> None:
        """Kills every process of the job and returns once each one has ended: the job's own count of active processes
        drops before the processes are gone, so each is waited for by its handle."""
        end, ended = time.monotonic() + KILL_WAIT_S, set()
        while True:
            pids = [pid for pid in self._pids() if pid not in ended]
            handles = [(pid, h) for pid, h in ((pid, k32.OpenProcess(SYNCHRONIZE, False, pid)) for pid in pids) if h]
            if not handles:
                return
            try:
                k32.TerminateJobObject(self.job, KILLED_EXIT)
                for pid, h in handles:
                    left = max(0, int((end - time.monotonic()) * 1000))
                    if k32.WaitForSingleObject(h, left) != WAIT_OBJECT_0:
                        raise TimeoutError(f"process tree {self.pid}: process {pid} outlived the kill by {KILL_WAIT_S} s")
                    ended.add(pid)
            finally:
                for _, h in handles:
                    k32.CloseHandle(h)

    def close(self) -> None:
        k32.CloseHandle(self.hp)
        k32.CloseHandle(self.job)


def alive(pid: int) -> bool:
    h = k32.OpenProcess(SYNCHRONIZE | PROCESS_QUERY_LIMITED_INFORMATION, False, pid)
    if not h:
        return False
    try:
        return k32.WaitForSingleObject(h, 0) == WAIT_TIMEOUT
    finally:
        k32.CloseHandle(h)
